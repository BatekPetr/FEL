#include "mbed_i2c.h"

// MBED-Shield - PB8 SCL, PB9 SDA
void InitI2C(void)
{
  InitIOPort(GPIOB, 8, portALTOUT);
  InitIOPort(GPIOB, 9, portALTOD); // !!! OPEN DRAIN

  if (!(RCC->APB2ENR & RCC_APB2ENR_AFIOEN))
  {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2RSTR |= RCC_APB2RSTR_AFIORST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_AFIORST;
  }
  
  AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;     // see. DS table at pg. 33/105

  if (!(RCC->APB1ENR & RCC_APB1ENR_I2C1EN))
  {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
  }
  
  {
    word tout;
    
    I2C1->CR1 |= I2C_CR1_SWRST;   // reset peripheral signal
    for (tout = 100; tout; tout--)  // short delay
      __nop();
    I2C1->CR1 = 0;
  }

  // configuration
  I2C1->CR1 = I2C_CR1_PE;         // enable peripheral, remainnig bits = 0
  I2C1->CR2 = 0;                  // clear all cfg. bits
  I2C1->CR2 &= ~ I2C_CR2_FREQ;    // clear bits FREQ[5:0]
  I2C1->CR2 |= I2C_CR2_FREQ_2;    // 4MHz
  I2C1->CR1 = 0;                  // disable preipheral
  
  // setting generated bz wizard (CubeMX)
  // clock control - must be configured when PE = 0
  I2C1->CCR = 0;                  // F/S = 0 (slow), duty = 0 (pomer 2)
  I2C1->CCR |= 28;                // (11 bitu) urcuje dobu trvani high i low urovne, vychazi z FREQ a APB1, min. 4
  I2C1->TRISE = 33;               // teoreticky FREQ + 1 ???
  
  I2C1->CR1 |= I2C_CR1_ACK;       // enable ACK

  #define I2C_ADDRESSINGMODE_7BIT         ((uint32_t)0x00004000)
  I2C1->OAR1 = I2C_ADDRESSINGMODE_7BIT;   // dle Cube
  
  #define I2C_DUALADDRESS_DISABLE         ((uint32_t)0x00000000)
  #define I2C_DUALADDRESS_DISABLED                I2C_DUALADDRESS_DISABLE
  I2C1->OAR2 = I2C_DUALADDRESS_DISABLED;  // dle Cube
  // end Wizard settings
  
  I2C1->CR1 |= I2C_CR1_PE;        // enable peripheral
}

// read 16-bit status
static __inline uint16_t I2C_sr(void) 
{
  uint16_t sr;

  sr  = I2C1->SR1;
  sr |= I2C1->SR2 << 16;
  return (sr);
}

const uint _timeoutI2C = 10000;

// Perform I2C Start-Condition (RM 26.3.1. fig. 268)
bool I2C_Start(void) 
{
  uint w = _timeoutI2C;
  
  I2C1->CR1 |= I2C_CR1_START;
  while (!(I2C_sr() & I2C_SR1_SB))      // wait for start condition generated
  {
    if (w)
      w--;
    else
      break;
  }
  
  return w;
}

// Perform I2C Start-Condition (RM 26.3.1. fig. 268)
bool I2C_Stop(void)
{
  uint w = _timeoutI2C;

  I2C1->CR1 |= I2C_CR1_STOP;
  while (I2C_sr() & (I2C_SR2_MSL << 16))         // Wait until BUSY bit reset          
  {
    if (w)
      w--;
    else
      break;
  }
  
  return w;
}

// send Address
bool I2C_Addr(uint8_t adr)
{
  uint w = _timeoutI2C;
  
  I2C1->DR = adr;
  while(!(I2C_sr() & I2C_SR1_ADDR))  // wait for sending completion
  {
    if (w)
      w--;
    else
      break;
  }
  
  return true;
}

// send data
bool I2C_Write(uint8_t c)
{
  uint w = _timeoutI2C;
  
  I2C1->DR = c;
  while (!(I2C_sr() & I2C_SR1_BTF))
  {
    if (w)
      w--;
    else
      break;
  }
  
  return true;
}

// read data - ACK = 1 
uint8_t I2C_Read(int ack)
{
  uint w = _timeoutI2C;

  // Enable/disable Master acknowledge
  if (ack) I2C1->CR1 |= I2C_CR1_ACK;
  else     I2C1->CR1 &= ~I2C_CR1_ACK;

  while (!(I2C_sr() & I2C_SR1_RXNE))
  {
    if (w)
      w--;
    else
      break;
  }
  
  return (I2C1->DR);
}

// Temperature LM75B = address 0x90, 16-bit value
uint16_t ReadTempI2C(void)
{
  uint16_t w = 0;

  // 7-bit address, last bit R = 1, W = 0
  I2C_Start();
  I2C_Addr(0x90 | 1);                  // read
  w = I2C_Read(1);
  w <<= 8;
  w |= I2C_Read(0);
  I2C_Stop();
  return w;
}

bool WriteTempI2C(uint8_t reg, uint8_t value)
{
  // 7-bit address, last bit R = 1, W = 0
  I2C_Start();
  I2C_Addr(0x90);                     // write
  I2C_Write(reg);                     // register address
  I2C_Write(value);                   // data
  I2C_Stop();
  return true;
}

// Accel Freescale MMA7660 = address 0x98, 4 data bytes
bool ReadXYZI2C(uint8_t *px, uint8_t *py, uint8_t *pz, uint8_t *ps)
{
  // 7-bit address, last bit R = 1, W = 0
  I2C_Start();
  I2C_Addr(0x98);        // write
  I2C_Write(0x00);       // address of first register - see tab. 9 in MMA7660FC.pdf
  I2C_Start();
  I2C_Addr(0x98 | 1);    // read
  *px = I2C_Read(1);     // $00 XOUT 6-bit output value X
  *py = I2C_Read(1);     // $01 YOUT 6-bit output value Y
  *pz = I2C_Read(1);     // $02 ZOUT 6-bit output value Z
  *ps = I2C_Read(0);     // $03 TILT Tilt Status Shake Alert Tap
  I2C_Stop();
  return true;
}

bool WriteXYZI2C(uint8_t reg, uint8_t value)
{
  I2C_Start();
  I2C_Addr(0x98);                     // write
  I2C_Write(reg);                     // address
  I2C_Write(value);                   // data
  I2C_Stop();
  return true;
}
