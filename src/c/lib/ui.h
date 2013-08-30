#ifndef _UI_H_
#define _UI_H_

extern void init_ui(void);

typedef struct {
    unsigned int *port;
    unsigned int *tris;
    unsigned int *ans;
    unsigned char bit;
} _LED;

typedef struct {
    unsigned int *port;
    unsigned int *tris;
    unsigned int *ans;
    unsigned char bit;
} _SW;

extern _LED led1, led2, led3;

extern void led_init(_LED *self, unsigned int *port, unsigned int *tris, 
                     unsigned int *ans, unsigned char bit);
extern void led_on(_LED *self);
extern void led_off(_LED *self);
extern void led_toggle(_LED *self);
extern void led_write(_LED *self, unsigned int val);
extern unsigned int led_read(_LED *self);

extern _SW sw1, sw2, sw3;

extern void sw_init(_SW *self, unsigned int *port, unsigned int *tris, 
                    unsigned int *ans, unsigned char bit);
extern unsigned int sw_read(_SW *self);

#endif
