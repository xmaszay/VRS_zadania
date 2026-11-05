#ifndef LPS22HB_H_
#define LPS22HB_H_

#include <stdint.h>

#define LPS22HB_DEVICE_ADDRESS      0xBAU
#define LPS22HB_WHO_AM_I_VALUE      0xB1U

#define LPS22HB_INTERRUPT_CFG       0x0BU
#define LPS22HB_WHO_AM_I            0x0FU
#define LPS22HB_CTRL_REG1           0x10U
#define LPS22HB_CTRL_REG2           0x11U
#define LPS22HB_CTRL_REG3           0x12U
#define LPS22HB_RES_CONF            0x1AU
#define LPS22HB_STATUS              0x27U
#define LPS22HB_PRESS_OUT_XL        0x28U
#define LPS22HB_PRESS_OUT_L         0x29U
#define LPS22HB_PRESS_OUT_H         0x2AU
#define LPS22HB_TEMP_OUT_L          0x2BU
#define LPS22HB_TEMP_OUT_H          0x2CU

#define LPS22HB_CTRL_REG1_ODR_1HZ   (0x01U << 4)
#define LPS22HB_CTRL_REG1_ODR_10HZ  (0x02U << 4)
#define LPS22HB_CTRL_REG1_ODR_25HZ  (0x03U << 4)
#define LPS22HB_CTRL_REG1_EN_LPFP   0x08U
#define LPS22HB_CTRL_REG1_LPFP_CFG  0x04U
#define LPS22HB_CTRL_REG1_BDU       0x02U

#define LPS22HB_CTRL_REG2_IF_ADD_INC 0x10U
#define LPS22HB_CTRL_REG2_SWRESET   0x04U

#define LPS22HB_STATUS_P_DA         0x01U
#define LPS22HB_STATUS_T_DA         0x02U

#define LPS22HB_PRESSURE_SENS       4096.0f
#define LPS22HB_TEMPERATURE_SENS    100.0f

#define LPS22HB_OK                  0U
#define LPS22HB_ERROR               1U

typedef uint8_t (*lps22hb_read_fn)(uint8_t slave_address, uint8_t register_address, uint8_t *data, uint8_t length);
typedef uint8_t (*lps22hb_write_fn)(uint8_t slave_address, uint8_t register_address, uint8_t *data, uint8_t length);

void lps22hb_register_callbacks(lps22hb_read_fn read_fn, lps22hb_write_fn write_fn);

uint8_t lps22hb_read(uint8_t register_address, uint8_t *data, uint8_t length);
uint8_t lps22hb_write(uint8_t register_address, uint8_t *data, uint8_t length);

uint8_t lps22hb_init(void);
uint8_t lps22hb_get_pressure(float *pressure);
uint8_t lps22hb_get_temperature(float *temperature);

#endif
