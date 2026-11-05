#include "lps22hb.h"
#include <math.h>

static lps22hb_read_fn lps22hb_read_cb = 0;
static lps22hb_write_fn lps22hb_write_cb = 0;

void lps22hb_register_callbacks(lps22hb_read_fn read_fn, lps22hb_write_fn write_fn)
{
	lps22hb_read_cb = read_fn;
	lps22hb_write_cb = write_fn;
}

uint8_t lps22hb_read(uint8_t register_address, uint8_t *data, uint8_t length)
{
	if(lps22hb_read_cb == 0)
	{
		return LPS22HB_ERROR;
	}

	return lps22hb_read_cb(LPS22HB_DEVICE_ADDRESS, register_address, data, length);
}

uint8_t lps22hb_write(uint8_t register_address, uint8_t *data, uint8_t length)
{
	if(lps22hb_write_cb == 0)
	{
		return LPS22HB_ERROR;
	}

	return lps22hb_write_cb(LPS22HB_DEVICE_ADDRESS, register_address, data, length);
}

uint8_t lps22hb_init(void)
{
	uint8_t who_am_i = 0;
	uint8_t value;

	if(lps22hb_read(LPS22HB_WHO_AM_I, &who_am_i, 1) != LPS22HB_OK || who_am_i != LPS22HB_WHO_AM_I_VALUE)
	{
		return LPS22HB_ERROR;
	}

	value = LPS22HB_CTRL_REG2_IF_ADD_INC;
	if(lps22hb_write(LPS22HB_CTRL_REG2, &value, 1) != LPS22HB_OK)
	{
		return LPS22HB_ERROR;
	}

	value = LPS22HB_CTRL_REG1_ODR_10HZ | LPS22HB_CTRL_REG1_EN_LPFP;
	if(lps22hb_write(LPS22HB_CTRL_REG1, &value, 1) != LPS22HB_OK)
	{
		return LPS22HB_ERROR;
	}

	return LPS22HB_OK;
}

uint8_t lps22hb_get_pressure(float *pressure)
{
	uint8_t data[3];
	int32_t p_out;

	if(pressure == 0 || lps22hb_read(LPS22HB_PRESS_OUT_XL, data, 3) != LPS22HB_OK)
	{
		return LPS22HB_ERROR;
	}

	p_out = (int32_t)(((uint32_t)data[2] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[0] << 8)) >> 8;
	*pressure = (float)p_out / LPS22HB_PRESSURE_SENS;

	return LPS22HB_OK;
}

uint8_t lps22hb_get_temperature(float *temperature)
{
	uint8_t data[2];
	int16_t t_out;

	if(temperature == 0 || lps22hb_read(LPS22HB_TEMP_OUT_L, data, 2) != LPS22HB_OK)
	{
		return LPS22HB_ERROR;
	}

	t_out = (int16_t)((data[1] << 8) | data[0]);
	*temperature = (float)t_out / LPS22HB_TEMPERATURE_SENS;

	return LPS22HB_OK;
}

#define LPS22HB_REFERENCE_TIMEOUT  1000U

uint8_t lps22hb_get_reference_pressure(float *reference_pressure, uint8_t samples)
{
	uint8_t status;
	uint32_t timeout;
	float pressure;
	float sum = 0.0f;

	if(reference_pressure == 0 || samples == 0)
	{
		return LPS22HB_ERROR;
	}

	for(uint8_t i = 0; i < samples; i++)
	{
		timeout = LPS22HB_REFERENCE_TIMEOUT;
		do
		{
			if(lps22hb_read(LPS22HB_STATUS, &status, 1) != LPS22HB_OK || --timeout == 0)
			{
				return LPS22HB_ERROR;
			}
		}
		while(!(status & LPS22HB_STATUS_P_DA));

		if(lps22hb_get_pressure(&pressure) != LPS22HB_OK)
		{
			return LPS22HB_ERROR;
		}

		sum += pressure;
	}

	*reference_pressure = sum / samples;

	return LPS22HB_OK;
}

float lps22hb_calculate_altitude(float pressure, float reference_pressure)
{
	return LPS22HB_ALTITUDE_CONST * (1.0f - powf(pressure / reference_pressure, LPS22HB_ALTITUDE_EXP));
}
