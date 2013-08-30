#ifndef _PIN_H_
#define _PIN_H_

extern void init_pin(void);

typedef struct _PIN {
    unsigned int *port;
    unsigned int *tris;
    unsigned int *ans;
    unsigned char bit;
    int annum;
    unsigned char rpshift;
    int rpnum;
    unsigned int *rpor;
    void *owner;
    void (*write)(struct _PIN *self, unsigned int val);
    unsigned int (*read)(struct _PIN *self);
} _PIN;

extern _PIN D[14], A[6];

extern void pin_init(_PIN *self, unsigned int *port, unsigned int *tris, 
                     unsigned int *ans, unsigned char bit, int annum, 
                     unsigned char rpshift, unsigned int rpnum, unsigned int *rpor);
extern void pin_digitalIn(_PIN *self);
extern void pin_digitalOut(_PIN *self);
extern void pin_analogIn(_PIN *self);
extern void pin_set(_PIN *self);
extern void pin_clear(_PIN *self);
extern void pin_toggle(_PIN *self);
extern void pin_write(_PIN *self, unsigned int val);
extern unsigned int pin_read(_PIN *self);

#endif
