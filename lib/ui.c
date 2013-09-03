#include <p24FJ128GB206.h>
#include "common.h"
#include "ui.h"

_LED led1, led2, led3;
_SW sw1, sw2, sw3;

void init_ui(void) {
    led_init(&led1, (uint16_t *)&PORTD, (uint16_t *)&TRISD, 
             (uint16_t *)&ANSD, 7);
    led_init(&led2, (uint16_t *)&PORTF, (uint16_t *)&TRISF, 
             (uint16_t *)&ANSF, 0);
    led_init(&led3, (uint16_t *)&PORTF, (uint16_t *)&TRISF, 
             (uint16_t *)&ANSF, 1);
    sw_init(&sw1, (uint16_t *)&PORTC, (uint16_t *)&TRISC, 
            (uint16_t *)&ANSC, 15);
    sw_init(&sw2, (uint16_t *)&PORTC, (uint16_t *)&TRISC, 
            (uint16_t *)&ANSC, 12);
    sw_init(&sw3, (uint16_t *)&PORTB, (uint16_t *)&TRISB, 
            (uint16_t *)&ANSB, 12);
}

void led_init(_LED *self, uint16_t *port, uint16_t *tris, 
              uint16_t *ans, uint8_t bit) {
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

void led_write(_LED *self, uint16_t val) {
    if (val)
        bitset(self->port, self->bit);
    else
        bitclear(self->port, self->bit);
}

uint16_t led_read(_LED *self) {
    return bitread(self->port, self->bit);
}

void sw_init(_SW *self, uint16_t *port, uint16_t *tris, 
             uint16_t *ans, uint8_t bit) {
    self->port = port;
    self->tris = tris;
    self->ans = ans;
    self->bit = bit;

    bitclear(self->ans, self->bit);
    bitset(self->tris, self->bit);
}

uint16_t sw_read(_SW *self) {
    return bitread(self->port, self->bit);
}
