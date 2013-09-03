#ifndef _PIN_H_
#define _PIN_H_

#include <stdint.h>

void init_pin(void);

typedef struct _PIN {
    uint16_t *port;
    uint16_t *tris;
    uint16_t *ans;
    uint8_t bit;
    int16_t annum;
    uint8_t rpshift;
    int16_t rpnum;
    uint16_t *rpor;
    void *owner;
    void (*write)(struct _PIN *self, uint16_t val);
    uint16_t (*read)(struct _PIN *self);
} _PIN;

extern _PIN D[14], A[6];

void pin_init(_PIN *self, uint16_t *port, uint16_t *tris, 
              uint16_t *ans, uint8_t bit, int16_t annum, 
              uint8_t rpshift, uint16_t rpnum, uint16_t *rpor);
void pin_digitalIn(_PIN *self);
void pin_digitalOut(_PIN *self);
void pin_analogIn(_PIN *self);
void pin_set(_PIN *self);
void pin_clear(_PIN *self);
void pin_toggle(_PIN *self);
void pin_write(_PIN *self, uint16_t val);
uint16_t pin_read(_PIN *self);

#endif
