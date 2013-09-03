#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"

void blink(_TIMER *self) {
    led_toggle(&led1);
}

int main(void) {
    init_clock();
    init_ui();
    init_timer();

    timer_after(&timer2, 0.5, 20, blink);

    while (1) {
        led_write(&led2, !sw_read(&sw2));
        led_write(&led3, !sw_read(&sw3));
    }
}

