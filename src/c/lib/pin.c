#include <p24FJ128GB206.h>
#include "util.h"
#include "pin.h"

_PIN D[14], A[6];

void __digitalWrite(_PIN *self, unsigned int val) {
    if (val)
        bitset(self->port, self->bit);
    else
        bitclear(self->port, self->bit);
}

unsigned int __digitalRead(_PIN *self) {
    return bitread(self->port, self->bit);
}

unsigned int __analogRead(_PIN *self) {
    AD1CHS = self->annum;
    AD1CON1bits.SAMP = 1;
    while (AD1CON1bits.DONE==0) {}
    return ADC1BUF0;
}

void init_pin(void) {
    unsigned int i;

    pin_init(&D[0], (unsigned int *)&PORTE, (unsigned int *)&TRISE, 
             (unsigned int *)NULL, 7, -1, 0, -1, (unsigned int *)NULL);
    pin_init(&D[1], (unsigned int *)&PORTE, (unsigned int *)&TRISE, 
             (unsigned int *)NULL, 6, -1, 0, -1, (unsigned int *)NULL);
    pin_init(&D[2], (unsigned int *)&PORTF, (unsigned int *)&TRISF, 
             (unsigned int *)NULL, 4, -1, 0, 10, (unsigned int *)&RPOR5);
    pin_init(&D[3], (unsigned int *)&PORTF, (unsigned int *)&TRISF, 
             (unsigned int *)NULL, 5, -1, 8, 17, (unsigned int *)&RPOR8);
    pin_init(&D[4], (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
             (unsigned int *)&ANSB, 15, 15, 8, 29, (unsigned int *)&RPOR14);
    pin_init(&D[5], (unsigned int *)&PORTF, (unsigned int *)&TRISF, 
             (unsigned int *)NULL, 3, -1, 0, 16, (unsigned int *)&RPOR8);
    pin_init(&D[6], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 8, -1, 0, 2, (unsigned int *)&RPOR1);
    pin_init(&D[7], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 11, -1, 0, 12, (unsigned int *)&RPOR6);
    pin_init(&D[8], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 9, -1, 0, 4, (unsigned int *)&RPOR2);
    pin_init(&D[9], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 10, -1, 8, 3, (unsigned int *)&RPOR1);
    pin_init(&D[10], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 0, -1, 8, 11, (unsigned int *)&RPOR5);
    pin_init(&D[11], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 1, -1, 0, 24, (unsigned int *)&RPOR12);
    pin_init(&D[12], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 2, -1, 8, 23, (unsigned int *)&RPOR11);
    pin_init(&D[13], (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)NULL, 3, -1, 0, 22, (unsigned int *)&RPOR11);
    pin_init(&A[0], (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
             (unsigned int *)&ANSB, 0, 0, 0, 0, (unsigned int *)&RPOR0);
    pin_init(&A[1], (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
             (unsigned int *)&ANSB, 1, 1, 8, 1, (unsigned int *)&RPOR0);
    pin_init(&A[2], (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
             (unsigned int *)&ANSB, 2, 2, 8, 13, (unsigned int *)&RPOR6);
    pin_init(&A[3], (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
             (unsigned int *)&ANSB, 3, 3, 0, -1, (unsigned int *)NULL);
    pin_init(&A[4], (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
             (unsigned int *)&ANSB, 4, 4, 0, 28, (unsigned int *)&RPOR14);
    pin_init(&A[5], (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
             (unsigned int *)&ANSB, 5, 5, 0, 18, (unsigned int *)&RPOR9);
    for (i = 0; i<14; i++)
        pin_digitalIn(&D[i]);
    for (i = 0; i<6; i++)
        pin_analogIn(&A[i]);

    // Configure and enable the ADC module: 
    //   set output data format to fractional (FORM<1:0> = 0b10)
    //   set conversion trigger source select bits to internal counter (SSRC<2:0> = 0b111)
    //   set auto-sample time bits to 12*TAD (SAMC<4:0> = 12)
    //   set conversion clock select bits to 64*TCY (ADCS<7:0> = 64)
    //   set initial channel selection to AN0
    AD1CON1 = 0x02E0; 
    AD1CON2 = 0;
    AD1CON3 = 0x0C40;
    AD1CHS = 0;
    AD1CON1bits.ADON = 1;
}

void pin_init(_PIN *self, unsigned int *port, unsigned int *tris, 
              unsigned int *ans, unsigned char bit, int annum, 
              unsigned char rpshift, unsigned int rpnum, unsigned int *rpor) {
    self->port = port;
    self->tris = tris;
    self->ans = ans;
    self->bit = bit;
    self->annum = annum;
    self->rpshift = rpshift;
    self->rpnum = rpnum;
    self->rpor = rpor;
    self->owner = NULL;
    self->write = NULL;
    self->read = NULL;
}

void pin_digitalIn(_PIN *self) {
    if (self->ans)
        bitclear(self->ans, self->bit);
    bitset(self->tris, self->bit);
    self->write = NULL;
    self->read = __digitalRead;    
}

void pin_digitalOut(_PIN *self) {
    if (self->ans)
        bitclear(self->ans, self->bit);
    bitclear(self->port, self->bit);
    bitclear(self->tris, self->bit);
    self->write = __digitalWrite;
    self->read = __digitalRead;
}

void pin_analogIn(_PIN *self) {
    if (!(self->ans))
        return;
    bitset(self->tris, self->bit);
    bitset(self->ans, self->bit);
    self->write = NULL;
    self->read = __analogRead;
}

void pin_set(_PIN *self) {
    if (self->write==__digitalWrite)
        bitset(self->port, self->bit);
}

void pin_clear(_PIN *self) {
    if (self->write==__digitalWrite)
        bitclear(self->port, self->bit);
}

void pin_toggle(_PIN *self) {
    if (self->write==__digitalWrite)
        bitflip(self->port, self->bit);
}

void pin_write(_PIN *self, unsigned int val) {
    if (self->write)
        self->write(self, val);
}

unsigned int pin_read(_PIN *self) {
    if (self->read)
        return self->read(self);
    else
        return 0xFFFF;
}

