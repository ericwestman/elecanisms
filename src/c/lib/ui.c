#include <p24FJ128GB206.h>
#include "util.h"
#include "ui.h"

_LED led1, led2, led3;
_SW sw1, sw2, sw3;

void init_ui(void) {
    led_init(&led1, (unsigned int *)&PORTD, (unsigned int *)&TRISD, 
             (unsigned int *)&ANSD, 7);
    led_init(&led2, (unsigned int *)&PORTF, (unsigned int *)&TRISF, 
             (unsigned int *)&ANSF, 0);
    led_init(&led3, (unsigned int *)&PORTF, (unsigned int *)&TRISF, 
             (unsigned int *)&ANSF, 1);
    sw_init(&sw1, (unsigned int *)&PORTC, (unsigned int *)&TRISC, 
            (unsigned int *)&ANSC, 15);
    sw_init(&sw2, (unsigned int *)&PORTC, (unsigned int *)&TRISC, 
            (unsigned int *)&ANSC, 12);
    sw_init(&sw3, (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
            (unsigned int *)&ANSB, 12);
}

void led_init(_LED *self, unsigned int *port, unsigned int *tris, 
              unsigned int *ans, unsigned char bit) {
    self->port = port;
    self->tris = tris;
    self->ans = ans;
    self->bit = bit;

    bitclear(self->ans, self->bit);
    bitclear(self->port, self->bit);
    bitclear(self->tris, self->bit);
}

void led_on(_LED *self) {
    bitset(self->port, self->bit);
}

void led_off(_LED *self) {
    bitclear(self->port, self->bit);
}

void led_toggle(_LED *self) {
    bitflip(self->port, self->bit);
}

void led_write(_LED *self, unsigned int val) {
    if (val)
        bitset(self->port, self->bit);
    else
        bitclear(self->port, self->bit);
}

unsigned int led_read(_LED *self) {
    return bitread(self->port, self->bit);
}

void sw_init(_SW *self, unsigned int *port, unsigned int *tris, 
             unsigned int *ans, unsigned char bit) {
    self->port = port;
    self->tris = tris;
    self->ans = ans;
    self->bit = bit;

    bitclear(self->ans, self->bit);
    bitset(self->tris, self->bit);
}

unsigned int sw_read(_SW *self) {
    return bitread(self->port, self->bit);
}
