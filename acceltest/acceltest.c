#include <p24FJ128GB206.h>
#include "common.h"
#include "config.h"
#include "usb.h"
#include "ui.h"
#include "timer.h"
#include "spi.h"
#include "imu.h"

#define TOGGLE_LED1         1
#define TOGGLE_LED2         2 
#define TOGGLE_LED3         3
#define READ_ACCEL_XYZ      4
#define GET_ACCEL_OFFSETS   5

int accel_xyz[3];
int accel_offsets[3];
int gyro_xyz[3];

void VendorRequests(void) {
    WORD32 address;
    WORD value;

    switch (USB_setup.bRequest) {
        case TOGGLE_LED1:
            led_toggle(&led1);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED2:
            led_toggle(&led2);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED3:
            led_toggle(&led3);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case READ_ACCEL_XYZ:
            accel_read_xyz((int *)accel_xyz);
            BD[EP0IN].address[0] = accel_xyz[0];
            BD[EP0IN].address[1] = accel_xyz[1];
            BD[EP0IN].address[2] = accel_xyz[2];
            BD[EP0IN].bytecount = 3;
            BD[EP0IN].status = 0xC8;
            break;
        case GET_ACCEL_OFFSETS:
            BD[EP0IN].address[0] = accel_offsets[0] & 0x00FF;
            BD[EP0IN].address[1] = accel_offsets[0] >> 8;
            BD[EP0IN].address[2] = accel_offsets[1] & 0x00FF;
            BD[EP0IN].address[3] = accel_offsets[1] >> 8;
            BD[EP0IN].address[4] = accel_offsets[2] & 0x00FF;
            BD[EP0IN].address[5] = accel_offsets[2] >> 8;
            BD[EP0IN].bytecount = 6;
            BD[EP0IN].status = 0xC8;
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    WORD32 address;

    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}


void main(void) {
    init_ui();
    init_timer();
    init_pin();
    init_spi();
    imu_init();
    accel_set_measure_mode();
    accel_calibrate((int *)accel_offsets);
    InitUSB();

    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    timer_setPeriod(&timer2, 0.5);
    timer_start(&timer2);
    while (1) {
        ServiceUSB();                       // service any pending USB requests
        if (timer_flag(&timer2)==1) {
            timer_lower(&timer2);
            led_toggle(&led1);
        }
    }
}
