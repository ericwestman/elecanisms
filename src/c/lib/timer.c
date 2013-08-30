#include <p24FJ128GB206.h>
#include "util.h"
#include "timer.h"

_TIMER timer1, timer2, timer3, timer4, timer5;
float timer_multipliers[4] = { TCY, 8.*TCY, 64.*TCY, 256.*TCY };

void timer_serviceInterrupt(_TIMER *self) {
    timer_lower(self);
    if (self->every) {
        self->every(self);
    } else if (self->after) {
        if (self->aftercount) {
            self->after(self);
            self->aftercount--;
        } else {
            timer_disableInterrupt(self);
            self->after = NULL;
        }
    } else {
        timer_disableInterrupt(self);
    }
}

void __attribute__((interrupt, auto_psv)) _T1Interrupt(void) {
    timer_serviceInterrupt(&timer1);
}

void __attribute__((interrupt, auto_psv)) _T2Interrupt(void) {
    timer_serviceInterrupt(&timer2);
}

void __attribute__((interrupt, auto_psv)) _T3Interrupt(void) {
    timer_serviceInterrupt(&timer3);
}

void __attribute__((interrupt, auto_psv)) _T4Interrupt(void) {
    timer_serviceInterrupt(&timer4);
}

void __attribute__((interrupt, auto_psv)) _T5Interrupt(void) {
    timer_serviceInterrupt(&timer5);
}

void init_timer(void) {
    timer_init(&timer1, (unsigned int *)&T1CON, (unsigned int *)&PR1, (unsigned int *)&TMR1, 
               (unsigned int *)&IFS0, (unsigned int *)&IEC0, 3, 4, 11);
    timer_init(&timer2, (unsigned int *)&T2CON, (unsigned int *)&PR2, (unsigned int *)&TMR2, 
               (unsigned int *)&IFS0, (unsigned int *)&IEC0, 7, 0, 12);
    timer_init(&timer3, (unsigned int *)&T3CON, (unsigned int *)&PR3, (unsigned int *)&TMR3, 
               (unsigned int *)&IFS0, (unsigned int *)&IEC0, 8, 1, 13);
    timer_init(&timer4, (unsigned int *)&T4CON, (unsigned int *)&PR4, (unsigned int *)&TMR4, 
               (unsigned int *)&IFS1, (unsigned int *)&IEC1, 11, 2, 14);
    timer_init(&timer5, (unsigned int *)&T5CON, (unsigned int *)&PR5, (unsigned int *)&TMR5, 
               (unsigned int *)&IFS1, (unsigned int *)&IEC1, 12, 3, 15);
}

void timer_init(_TIMER *self, unsigned int *TxCON, unsigned int *PRx, 
                unsigned int *TMRx, unsigned int *IFSy, unsigned int *IECy, 
                unsigned char flagbit, unsigned int octselnum, unsigned int ocsyncselnum) {
    self->TxCON = TxCON;
    self->PRx = PRx;
    self->TMRx = TMRx;
    self->IFSy = IFSy;
    self->IECy = IECy;
    self->flagbit = flagbit;
    self->octselnum = octselnum;
    self->ocsyncselnum = ocsyncselnum;
    self->aftercount = 0;
    self->every = NULL;
    self->after = NULL;
}

void timer_setPeriod(_TIMER *self, float period) {
    if (period>(256.*65536.*TCY)) {
        return;
    } else if (period>(64.*65536.*TCY)) {
        timer_stop(self);
        poke(self->TxCON, 0x0030);
        poke(self->PRx, (unsigned int)(period*(FCY/256.))-1);
    } else if (period>(8.*65536.*TCY)) {
        timer_stop(self);
        poke(self->TxCON, 0x0020);
        poke(self->PRx, (unsigned int)(period*(FCY/64.))-1);
    } else if (period>(65536.*TCY)) {
        timer_stop(self);
        poke(self->TxCON, 0x0010);
        poke(self->PRx, (unsigned int)(period*(FCY/8.))-1);
    } else {
        timer_stop(self);
        poke(self->TxCON, 0x0000);
        poke(self->PRx, (unsigned int)(period*FCY)-1);
    }
}

float timer_period(_TIMER *self) {
    unsigned int prescalar = (peek(self->TxCON)&0x0030)>>4;

    return timer_multipliers[prescalar]*((float)peek(self->PRx)+1.);
}

void timer_setFreq(_TIMER *self, float freq) {
    timer_setPeriod(self, 1./freq);
}

float timer_freq(_TIMER *self) {
    return 1./timer_period(self);
}

float timer_time(_TIMER *self) {
    unsigned int prescalar = (peek(self->TxCON)&0x0030)>>4;

    return timer_multipliers[prescalar]*((float)peek(self->PRx)+1.);
}

void timer_start(_TIMER *self) {
    timer_lower(self);
    poke(self->TMRx, 0);
    bitset(self->TxCON, 15);
}

void timer_stop(_TIMER *self) {
    bitclear(self->TxCON, 15);
}

unsigned int timer_flag(_TIMER *self) {
    return bitread(self->IFSy, self->flagbit);
}

void timer_lower(_TIMER *self) {
    bitclear(self->IFSy, self->flagbit);
}

unsigned int timer_read(_TIMER *self) {
    return peek(self->TMRx);
}

void timer_enableInterrupt(_TIMER *self) {
    bitset(self->IECy, self->flagbit);
}

void timer_disableInterrupt(_TIMER *self) {
    bitclear(self->IECy, self->flagbit);
}

void timer_every(_TIMER *self, float interval, void (*callback)(_TIMER *self)) {
    timer_disableInterrupt(self);
    timer_setPeriod(self, interval);
    self->aftercount = 0;
    self->every = callback;
    self->after = NULL;
    timer_enableInterrupt(self);
    timer_start(self);
}

void timer_after(_TIMER *self, float delay, unsigned int num_times, 
                 void (*callback)(_TIMER *self)) {
    timer_disableInterrupt(self);
    timer_setPeriod(self, delay);
    self->aftercount = num_times;
    self->every = NULL;
    self->after = callback;
    timer_enableInterrupt(self);
    timer_start(self);
}

void timer_cancel(_TIMER *self) {
    timer_disableInterrupt(self);
    timer_lower(self);
    timer_stop(self);
    self->aftercount = 0;
    self->every = NULL;
    self->after = NULL;
}

