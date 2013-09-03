#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include "pin.h"

void init_i2c(void);

typedef struct {
    uint16_t *I2CxRCV;
    uint16_t *I2CxTRN;
    uint16_t *I2CxBRG;
    uint16_t *I2CxCON;
    uint16_t *I2CxSTAT;
    uint16_t *I2CxADD;
    uint16_t *I2CxMSK;
    uint16_t *IFSy;
    uint8_t MI2CxIF;
    _PIN *SDA;
    _PIN *SCL;
} _I2C;

extern _I2C i2c1, i2c2, i2c3;

void i2c_init(_I2C *self, uint16_t *I2CxRCV, uint16_t *I2CxTRN, 
              uint16_t *I2CxBRG, uint16_t *I2CxCON, 
              uint16_t *I2CxSTAT, uint16_t *I2CxADD, 
              uint16_t *I2CxMSK, uint16_t *IFSy, 
              uint8_t MI2CxIF, _PIN *SDA, _PIN *SCL);
void i2c_open(_I2C *self, float freq);
void i2c_close(_I2C *self);
void i2c_start(_I2C *self);
void i2c_restart(_I2C *self);
void i2c_ack(_I2C *self);
void i2c_nak(_I2C *self);
void i2c_stop(_I2C *self);
void i2c_idle(_I2C *self);
uint8_t i2c_putc(_I2C *self, uint8_t ch);
uint8_t i2c_getc(_I2C *self);

#endif

