#ifndef HTS221_H_
#define HTS221_H_

#include <stdint.h>

#define HTS221_DEVICE_ADDRESS       0xBEU
#define HTS221_WHO_AM_I_VALUE       0xBCU

#define HTS221_WHO_AM_I             0x0FU
#define HTS221_AV_CONF              0x10U
#define HTS221_CTRL_REG1            0x20U
#define HTS221_CTRL_REG2            0x21U
#define HTS221_CTRL_REG3            0x22U
#define HTS221_STATUS_REG           0x27U

#define HTS221_HUMIDITY_OUT_L       0x28U
#define HTS221_HUMIDITY_OUT_H       0x29U
#define HTS221_TEMP_OUT_L           0x2AU
#define HTS221_TEMP_OUT_H           0x2BU
#define HTS221_CALIB_START          0x30U

#define HTS221_AUTO_INCREMENT       0x80U

#define HTS221_AV_CONF_AVGT_16      (0x03U << 3)
#define HTS221_AV_CONF_AVGH_32      (0x03U << 0)

#define HTS221_CTRL_REG1_PD         0x80U
#define HTS221_CTRL_REG1_BDU        0x04U
#define HTS221_CTRL_REG1_ODR_1HZ    0x01U
#define HTS221_CTRL_REG1_ODR_7HZ    0x02U
#define HTS221_CTRL_REG1_ODR_12HZ5  0x03U

#define HTS221_STATUS_T_DA          0x01U
#define HTS221_STATUS_H_DA          0x02U

#define HTS221_OK                   0U
#define HTS221_ERROR                1U

typedef uint8_t (*hts221_read_fn)(uint8_t slave_address, uint8_t register_address, uint8_t *data, uint8_t length);
typedef uint8_t (*hts221_write_fn)(uint8_t slave_address, uint8_t register_address, uint8_t *data, uint8_t length);

void hts221_register_callbacks(hts221_read_fn read_fn, hts221_write_fn write_fn);

uint8_t hts221_read(uint8_t register_address, uint8_t *data, uint8_t length);
uint8_t hts221_write(uint8_t register_address, uint8_t *data, uint8_t length);

uint8_t hts221_init(void);

uint8_t hts221_get_temperature(float *temperature);
uint8_t hts221_get_humidity(float *humidity);

#endif
