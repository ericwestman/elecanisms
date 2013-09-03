#ifndef _UI_H_
#define _UI_H_

#include <stdint.h>

void init_ui(void);

typedef struct {
    uint16_t *port;
    uint16_t *tris;
    uint16_t *ans;
    uint8_t bit;
} _LED;

typedef struct {
    uint16_t *port;
    uint16_t *tris;
    uint16_t *ans;
    uint8_t bit;
} _SW;

extern _LED led1, led2, led3;

void led_init(_LED *self, uint16_t *port, uint16_t *tris, 
              uint16_t *ans, uint8_t bit);
void led_on(_LED *self);
void led_off(_LED *self);
void led_toggle(_LED *self);
void led_write(_LED *self, uint16_t val);
uint16_t led_read(_LED *self);

extern _SW sw1, sw2, sw3;

void sw_init(_SW *self, uint16_t *port, uint16_t *tris, 
             uint16_t *ans, uint8_t bit);
uint16_t sw_read(_SW *self);

#endif
