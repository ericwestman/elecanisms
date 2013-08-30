#ifndef _TIMER_H_
#define _TIMER_H_

extern void init_timer(void);

typedef struct _TIMER {
    unsigned int *TxCON;
    unsigned int *PRx;
    unsigned int *TMRx;
    unsigned int *IFSy;
    unsigned int *IECy;
    unsigned char flagbit;
    unsigned int octselnum;
    unsigned int ocsyncselnum;
    unsigned int aftercount;
    void (*every)(struct _TIMER *self);
    void (*after)(struct _TIMER *self);
} _TIMER;

extern _TIMER timer1, timer2, timer3, timer4, timer5;

extern void timer_init(_TIMER *self, unsigned int *TxCON, unsigned int *PRx, 
                       unsigned int *TMRx, unsigned int *IFSy, unsigned int *IECy, 
                       unsigned char flagbit, unsigned int octselnum, unsigned int ocsyncselnum);
extern void timer_setPeriod(_TIMER *self, float period);
extern float timer_period(_TIMER *self);
extern void timer_setFreq(_TIMER *self, float freq);
extern float timer_freq(_TIMER *self);
extern float timer_time(_TIMER *self);
extern void timer_start(_TIMER *self);
extern void timer_stop(_TIMER *self);
extern unsigned int timer_flag(_TIMER *self);
extern void timer_lower(_TIMER *self);
extern unsigned int timer_read(_TIMER *self);
extern void timer_enableInterrupt(_TIMER *self);
extern void timer_disableInterrupt(_TIMER *self);
extern void timer_every(_TIMER *self, float interval, void (*callback)(_TIMER *self));
extern void timer_after(_TIMER *self, float delay, unsigned int num_times, 
                        void (*callback)(_TIMER *self));
extern void timer_cancel(_TIMER *self);

#endif
