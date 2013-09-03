#include <p24FJ128GB206.h>
#include "common.h"
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
    timer_init(&timer1, (uint16_t *)&T1CON, (uint16_t *)&PR1, (uint16_t *)&TMR1, 
               (uint16_t *)&IFS0, (uint16_t *)&IEC0, 3, 4, 11);
    timer_init(&timer2, (uint16_t *)&T2CON, (uint16_t *)&PR2, (uint16_t *)&TMR2, 
               (uint16_t *)&IFS0, (uint16_t *)&IEC0, 7, 0, 12);
    timer_init(&timer3, (uint16_t *)&T3CON, (uint16_t *)&PR3, (uint16_t *)&TMR3, 
               (uint16_t *)&IFS0, (uint16_t *)&IEC0, 8, 1, 13);
    timer_init(&timer4, (uint16_t *)&T4CON, (uint16_t *)&PR4, (uint16_t *)&TMR4, 
               (uint16_t *)&IFS1, (uint16_t *)&IEC1, 11, 2, 14);
    timer_init(&timer5, (uint16_t *)&T5CON, (uint16_t *)&PR5, (uint16_t *)&TMR5, 
               (uint16_t *)&IFS1, (uint16_t *)&IEC1, 12, 3, 15);
}

void timer_init(_TIMER *self, uint16_t *TxCON, uint16_t *PRx, 
                uint16_t *TMRx, uint16_t *IFSy, uint16_t *IECy, 
                uint8_t flagbit, uint16_t octselnum, uint16_t ocsyncselnum) {
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
        poke(self->PRx, (uint16_t)(period*(FCY/256.))-1);
    } else if (period>(8.*65536.*TCY)) {
        timer_stop(self);
        poke(self->TxCON, 0x0020);
        poke(self->PRx, (uint16_t)(period*(FCY/64.))-1);
    } else if (period>(65536.*TCY)) {
        timer_stop(self);
        poke(self->TxCON, 0x0010);
        poke(self->PRx, (uint16_t)(period*(FCY/8.))-1);
    } else {
        timer_stop(self);
        poke(self->TxCON, 0x0000);
        poke(self->PRx, (uint16_t)(period*FCY)-1);
    }
}

float timer_period(_TIMER *self) {
    uint16_t prescalar = (peek(self->TxCON)&0x0030)>>4;

    return timer_multipliers[prescalar]*((float)peek(self->PRx)+1.);
}

void timer_setFreq(_TIMER *self, float freq) {
    timer_setPeriod(self, 1./freq);
}

float timer_freq(_TIMER *self) {
    return 1./timer_period(self);
}

float timer_time(_TIMER *self) {
    uint16_t prescalar = (peek(self->TxCON)&0x0030)>>4;

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

uint16_t timer_flag(_TIMER *self) {
    return bitread(self->IFSy, self->flagbit);
}

void timer_lower(_TIMER *self) {
    bitclear(self->IFSy, self->flagbit);
}

uint16_t timer_read(_TIMER *self) {
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

void timer_after(_TIMER *self, float delay, uint16_t num_times, 
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

