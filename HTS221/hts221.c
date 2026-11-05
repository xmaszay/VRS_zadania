#include "hts221.h"

static hts221_read_fn hts221_read_cb = 0;
static hts221_write_fn hts221_write_cb = 0;

static float t0_degc, t1_degc;
static int16_t t0_out, t1_out;
static float h0_rh, h1_rh;
static int16_t h0_t0_out, h1_t0_out;

void hts221_register_callbacks(hts221_read_fn read_fn, hts221_write_fn write_fn)
{
	hts221_read_cb = read_fn;
	hts221_write_cb = write_fn;
}

uint8_t hts221_read(uint8_t register_address, uint8_t *data, uint8_t length)
{
	if(hts221_read_cb == 0)
	{
		return HTS221_ERROR;
	}

	if(length > 1)
	{
		register_address |= HTS221_AUTO_INCREMENT;
	}

	return hts221_read_cb(HTS221_DEVICE_ADDRESS, register_address, data, length);
}

uint8_t hts221_write(uint8_t register_address, uint8_t *data, uint8_t length)
{
	if(hts221_write_cb == 0)
	{
		return HTS221_ERROR;
	}

	if(length > 1)
	{
		register_address |= HTS221_AUTO_INCREMENT;
	}

	return hts221_write_cb(HTS221_DEVICE_ADDRESS, register_address, data, length);
}

static uint8_t hts221_read_calibration(void)
{
	uint8_t calib[16];

	if(hts221_read(HTS221_CALIB_START, calib, sizeof(calib)) != HTS221_OK)
	{
		return HTS221_ERROR;
	}

	h0_rh = calib[0] / 2.0f;
	h1_rh = calib[1] / 2.0f;

	t0_degc = (uint16_t)(((calib[5] & 0x03U) << 8) | calib[2]) / 8.0f;
	t1_degc = (uint16_t)(((calib[5] & 0x0CU) << 6) | calib[3]) / 8.0f;

	h0_t0_out = (int16_t)((calib[7] << 8) | calib[6]);
	h1_t0_out = (int16_t)((calib[11] << 8) | calib[10]);
	t0_out = (int16_t)((calib[13] << 8) | calib[12]);
	t1_out = (int16_t)((calib[15] << 8) | calib[14]);

	if(t1_out == t0_out || h1_t0_out == h0_t0_out)
	{
		return HTS221_ERROR;
	}

	return HTS221_OK;
}

uint8_t hts221_init(void)
{
	uint8_t who_am_i = 0;
	uint8_t value;

	if(hts221_read(HTS221_WHO_AM_I, &who_am_i, 1) != HTS221_OK || who_am_i != HTS221_WHO_AM_I_VALUE)
	{
		return HTS221_ERROR;
	}

	value = HTS221_AV_CONF_AVGT_16 | HTS221_AV_CONF_AVGH_32;
	if(hts221_write(HTS221_AV_CONF, &value, 1) != HTS221_OK)
	{
		return HTS221_ERROR;
	}

	value = HTS221_CTRL_REG1_PD | HTS221_CTRL_REG1_BDU | HTS221_CTRL_REG1_ODR_7HZ;
	if(hts221_write(HTS221_CTRL_REG1, &value, 1) != HTS221_OK)
	{
		return HTS221_ERROR;
	}

	return hts221_read_calibration();
}

uint8_t hts221_get_temperature(float *temperature)
{
	uint8_t data[2];
	int16_t t_out;

	if(temperature == 0 || hts221_read(HTS221_TEMP_OUT_L, data, 2) != HTS221_OK)
	{
		return HTS221_ERROR;
	}

	t_out = (int16_t)((data[1] << 8) | data[0]);
	*temperature = t0_degc + (float)(t_out - t0_out) * (t1_degc - t0_degc) / (float)(t1_out - t0_out);

	return HTS221_OK;
}

uint8_t hts221_get_humidity(float *humidity)
{
	uint8_t data[2];
	int16_t h_out;
	float h;

	if(humidity == 0 || hts221_read(HTS221_HUMIDITY_OUT_L, data, 2) != HTS221_OK)
	{
		return HTS221_ERROR;
	}

	h_out = (int16_t)((data[1] << 8) | data[0]);
	h = h0_rh + (float)(h_out - h0_t0_out) * (h1_rh - h0_rh) / (float)(h1_t0_out - h0_t0_out);

	if(h < 0.0f)
	{
		h = 0.0f;
	}
	else if(h > 100.0f)
	{
		h = 100.0f;
	}

	*humidity = h;

	return HTS221_OK;
}

