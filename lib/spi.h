#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>
#include "pin.h"

void init_spi(void);

typedef struct {
    uint16_t *SPIxSTAT;
    uint16_t *SPIxCON1;
    uint16_t *SPIxCON2;
    uint16_t *SPIxBUF;
    uint16_t *MISOrpinr;
    uint8_t MISOrpshift;
    int16_t MOSIrpnum;
    int16_t SCKrpnum;
    _PIN *MISO;
    _PIN *MOSI;
    _PIN *SCK;
} _SPI;

extern _SPI spi1, spi2, spi3;

void spi_init(_SPI *self, uint16_t *SPIxSTAT, uint16_t *SPIxCON1, 
              uint16_t *SPIxCON2, uint16_t *SPIxBUF, 
              uint16_t *MISOrpinr, uint8_t MISOrpshift, 
              int16_t MOSIrpnum, int16_t SCKrpnum);
void spi_open(_SPI *self, _PIN *MISO, _PIN *MOSI, _PIN *SCK, float freq);
void spi_close(_SPI *self);
uint8_t spi_transfer(_SPI *self, uint8_t val);

#endif
