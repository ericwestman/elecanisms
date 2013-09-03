#ifndef _OC_H_
#define _OC_H_

#include <stdint.h>
#include "pin.h"
#include "timer.h"

void init_oc(void);

typedef struct {
    uint16_t *OCxCON1;
    uint16_t *OCxCON2;
    uint16_t *OCxRS;
    uint16_t *OCxR;
    uint16_t *OCxTMR;
    uint16_t rpnum;
    uint16_t servooffset;
    uint16_t servomultiplier;
    _PIN *pin;
} _OC;

extern _OC oc1, oc2, oc3, oc4, oc5, oc6, oc7, oc8, oc9;

void oc_init(_OC *self, uint16_t *OCxCON1, uint16_t *OCxCON2, 
             uint16_t *OCxRS, uint16_t *OCxR, uint16_t *OCxTMR, 
             uint16_t rpnum);
void oc_free(_OC *self);
void oc_pwm(_OC *self, _PIN *out, _TIMER *timer, float freq, uint16_t duty);
void oc_servo(_OC *self, _PIN *out, _TIMER *timer, float interval, 
              float min_width, float max_width, uint16_t pos);

#endif
