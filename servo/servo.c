#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    init_oc();

    led_on(&led1);
    pin_analogIn(&A[0]);
    pin_digitalOut(&D[0]);

    timer_setPeriod(&timer2, 0.5);
    timer_start(&timer2);

    uint16_t analog_0, command;
    oc_servo(&oc1, &D[0], &timer1, 20E-3, 6E-4, 2.4E-3, 0);

    while (1) {

        if (timer_flag(&timer2)) {
            timer_lower(&timer2);
            led_toggle(&led1);
        }

        analog_0 = pin_read(&A[0]);
        pin_write(&D[0], analog_0);

        // if (analog_0 > 30000) {
        //     led_write(&led2, 1);
        // } else {
        //     led_write(&led2, 0);
        // }

        

    }
}

