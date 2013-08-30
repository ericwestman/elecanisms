#include <p24FJ128GB206.h>
#include "util.h"
#include "spi.h"
#include "ui.h"

_SPI spi1, spi2, spi3;

void init_spi(void) {
    spi_init(&spi1, (unsigned int *)&SPI1STAT, (unsigned int *)&SPI1CON1, 
             (unsigned int *)&SPI1CON2, (unsigned int *)&SPI1BUF, 
             (unsigned int *)&RPINR20, 0, 7, 8);
    spi_init(&spi2, (unsigned int *)&SPI2STAT, (unsigned int *)&SPI2CON1, 
             (unsigned int *)&SPI2CON2, (unsigned int *)&SPI2BUF, 
             (unsigned int *)&RPINR22, 0, 10, 11);
    spi_init(&spi3, (unsigned int *)&SPI3STAT, (unsigned int *)&SPI3CON1, 
             (unsigned int *)&SPI3CON2, (unsigned int *)&SPI3BUF, 
             (unsigned int *)&RPINR28, 0, 32, 33);
}

void spi_init(_SPI *self, unsigned int *SPIxSTAT, unsigned int *SPIxCON1, 
              unsigned int *SPIxCON2, unsigned int *SPIxBUF, 
              unsigned int *MISOrpinr, unsigned char MISOrpshift, 
              int MOSIrpnum, int SCKrpnum) {
    self->SPIxSTAT = SPIxSTAT;
    self->SPIxCON1 = SPIxCON1;
    self->SPIxCON2 = SPIxCON2;
    self->SPIxBUF = SPIxBUF;
    self->MISOrpinr = MISOrpinr;
    self->MISOrpshift = MISOrpshift;
    self->MOSIrpnum = MOSIrpnum;
    self->SCKrpnum = SCKrpnum;
    self->MISO = NULL;
    self->MOSI = NULL;
    self->SCK = NULL;
}

void spi_open(_SPI *self, _PIN *MISO, _PIN *MOSI, _PIN *SCK, float freq) {
    unsigned int primary, secondary;

    if ((MISO->rpnum==-1) || (MOSI->rpnum==-1) || (SCK->rpnum==-1))
        return; // At least one of the specified pins is not an RP pin
    if ((MISO->owner==NULL) && (MOSI->owner==NULL) && (SCK->owner==NULL)) {
        // All of the specified pins are available and RP pins, so configure 
        // as specified
        pin_digitalIn(MISO);
        pin_digitalOut(MOSI);
        pin_set(MOSI);
        pin_digitalOut(SCK);
        pin_clear(SCK);
        self->MISO = MISO;
        MISO->owner = (void *)self;
        MISO->write = NULL;
        MISO->read = NULL;
        self->MOSI = MOSI;
        MOSI->owner = (void *)self;
        MOSI->write = NULL;
        MOSI->read = NULL;
        self->SCK = SCK;
        SCK->owner = (void *)self;
        SCK->write = NULL;
        SCK->read = NULL;
        __builtin_write_OSCCONL(OSCCON&0xBF);
        *(self->MISOrpinr) &= ~(0x3F<<(self->MISOrpshift));
        *(self->MISOrpinr) |= (MISO->rpnum)<<(self->MISOrpshift);
        *(MOSI->rpor) &= ~(0x3F<<(MOSI->rpshift));
        *(MOSI->rpor) |= (self->MOSIrpnum)<<(MOSI->rpshift);
        *(SCK->rpor) &= ~(0x3F<<(SCK->rpshift));
        *(SCK->rpor) |= (self->SCKrpnum)<<(SCK->rpshift);
        __builtin_write_OSCCONL(OSCCON|0x40);
    } else if ((self->MISO!=MISO) || (self->MOSI!=MOSI) || (self->SCK!=SCK)) {
        return; // At least one of the specified pins does not match the 
                // previous assignment
    }
    // Clip freq to be in allowable range of values
    if (freq>(FCY/2.))
        freq = FCY/2.;
    if (freq<(FCY/(64.*8.)))
        freq = FCY/(64.*8.);
    // Select primary prescale bits
    if (freq<=(FCY/(2.*64.))) {
        freq *= 64.;
        primary = 0;    // Set primary prescale bits for 64:1
    } else if (freq<=(FCY/(2.*16.))) {
        freq *= 16.;
        primary = 1;    // Set primary prescale bits for 16:1
    } else if (freq<=(FCY/(2.*4.))) {
        freq *= 4.;
        primary = 2;    // Set primary prescale bits for 4:1
    } else {
        primary = 3;    // Set primary prescale bits for 1:1
    }
    // Compute secondary prescale value to get closest SPI clock freq to that 
    // specified
    secondary = (unsigned int)(0.5+FCY/freq);
    secondary = (8-secondary)<<2;   // Map secondary prescale bits for SPIxCON1
    // Configure the SPI module
    //   set SPI module to 8-bit master mode
    //   set SMP = 0, CKE = 1, and CKP = 0
    //   set SPRE and PPRE bits to get the closest SPI clock freq to that 
    //   specified
    *(self->SPIxCON1) = 0x0120|primary|secondary;
    *(self->SPIxCON2) = 0;
    // Enable the SPI module and clear status flags
    *(self->SPIxSTAT) = 0x8000;
}

void spi_close(_SPI *self) {
    *(self->SPIxSTAT) = 0;
    *(self->SPIxCON1) = 0;
    *(self->SPIxCON2) = 0;
    if (self->MISO) {
        __builtin_write_OSCCONL(OSCCON&0xBF);
        *(self->MISOrpinr) |= 0x3F<<(self->MISOrpshift);
        __builtin_write_OSCCONL(OSCCON|0x40);
        self->MISO->owner = NULL;
        pin_digitalIn(self->MISO);
        self->MISO = NULL;
    }
    if (self->MOSI) {
        __builtin_write_OSCCONL(OSCCON&0xBF);
        *(self->MOSI->rpor) &= ~(0x3F<<(self->MOSI->rpshift));
        __builtin_write_OSCCONL(OSCCON|0x40);
        self->MOSI->owner = NULL;
        pin_digitalOut(self->MOSI);
        pin_set(self->MOSI);
        self->MOSI = NULL;
    }
    if (self->SCK) {
        __builtin_write_OSCCONL(OSCCON&0xBF);
        *(self->SCK->rpor) &= ~(0x3F<<(self->SCK->rpshift));
        __builtin_write_OSCCONL(OSCCON|0x40);
        self->SCK->owner = NULL;
        pin_digitalOut(self->SCK);
        pin_clear(self->SCK);
        self->SCK = NULL;
    }
}

unsigned char spi_transfer(_SPI *self, unsigned char val) {
    *(self->SPIxBUF) = (unsigned int)val;
    while (!bitread(self->SPIxSTAT, 0));
    return (unsigned char)(*(self->SPIxBUF));
}
