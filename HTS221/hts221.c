#include "hts221.h"

static hts221_read_fn hts221_read_cb = 0;
static hts221_write_fn hts221_write_cb = 0;

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

	return HTS221_OK;
}

