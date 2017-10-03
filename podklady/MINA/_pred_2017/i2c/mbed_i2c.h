#ifndef _MBED_I2C_H
#define _MBED_I2C_H

#include "mbed.h"

void InitI2C(void);
bool I2C_Start(void);
bool I2C_Stop(void);
bool I2C_Addr(uint8_t adr);
bool I2C_Write(uint8_t c);
uint8_t I2C_Read(int ack);
uint16_t ReadTempI2C(void);
bool WriteTempI2C(uint8_t adr, uint8_t value);
bool ReadXYZI2C(uint8_t *px, uint8_t *py, uint8_t *pz, uint8_t *ps);
bool WriteXYZI2C(uint8_t adr, uint8_t value);

#endif // _MBED_I2C_H
