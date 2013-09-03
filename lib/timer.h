#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

void init_timer(void);

typedef struct _TIMER {
    uint16_t *TxCON;
    uint16_t *PRx;
    uint16_t *TMRx;
    uint16_t *IFSy;
    uint16_t *IECy;
    uint8_t flagbit;
    uint16_t octselnum;
    uint16_t ocsyncselnum;
    uint16_t aftercount;
    void (*every)(struct _TIMER *self);
    void (*after)(struct _TIMER *self);
} _TIMER;

extern _TIMER timer1, timer2, timer3, timer4, timer5;

void timer_init(_TIMER *self, uint16_t *TxCON, uint16_t *PRx, 
                uint16_t *TMRx, uint16_t *IFSy, uint16_t *IECy, 
                uint8_t flagbit, uint16_t octselnum, uint16_t ocsyncselnum);
void timer_setPeriod(_TIMER *self, float period);
float timer_period(_TIMER *self);
void timer_setFreq(_TIMER *self, float freq);
float timer_freq(_TIMER *self);
float timer_time(_TIMER *self);
void timer_start(_TIMER *self);
void timer_stop(_TIMER *self);
uint16_t timer_flag(_TIMER *self);
void timer_lower(_TIMER *self);
uint16_t timer_read(_TIMER *self);
void timer_enableInterrupt(_TIMER *self);
void timer_disableInterrupt(_TIMER *self);
void timer_every(_TIMER *self, float interval, void (*callback)(_TIMER *self));
void timer_after(_TIMER *self, float delay, uint16_t num_times, 
                 void (*callback)(_TIMER *self));
void timer_cancel(_TIMER *self);

#endif
