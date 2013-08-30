#ifndef _SPI_H_
#define _SPI_H_

#include "pin.h"

extern void init_spi(void);

typedef struct {
    unsigned int *SPIxSTAT;
    unsigned int *SPIxCON1;
    unsigned int *SPIxCON2;
    unsigned int *SPIxBUF;
    unsigned int *MISOrpinr;
    unsigned char MISOrpshift;
    int MOSIrpnum;
    int SCKrpnum;
    _PIN *MISO;
    _PIN *MOSI;
    _PIN *SCK;
} _SPI;

extern _SPI spi1, spi2, spi3;

extern void spi_init(_SPI *self, unsigned int *SPIxSTAT, unsigned int *SPIxCON1, 
                     unsigned int *SPIxCON2, unsigned int *SPIxBUF, 
                     unsigned int *MISOrpinr, unsigned char MISOrpshift, 
                     int MOSIrpnum, int SCKrpnum);
extern void spi_open(_SPI *self, _PIN *MISO, _PIN *MOSI, _PIN *SCK, float freq);
extern void spi_close(_SPI *self);
extern unsigned char spi_transfer(_SPI *self, unsigned char val);

#endif
