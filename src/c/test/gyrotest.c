/*
 * =====================================================================================
 *
 *       Filename:  gyrotest.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/30/2013 11:14:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <p24FJ128GB206.h>
#include "usb.h"
#include "util.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"
#include "spi.h"

#define WHO_AM_I        0x0F
#define CTRL_REG1      0x20
#define CTRL_REG2      0x21
#define CTRL_REG3      0x22
#define CTRL_REG4      0x23
#define CTRL_REG5      0x24
#define REFERENCE       0x25
#define OUT_TEMP        0x26
#define STATUS_REG      0x27
#define OUT_X_L         0x28
#define OUT_X_H         0x29
#define OUT_Y_L         0x2A
#define OUT_Y_H         0x2B
#define OUT_Z_L         0x2C
#define OUT_Z_H         0x2D
#define FIFO_CTRL_REG   0x2E
#define FIFO_SRC_REG    0x2F
#define INT1_CFG        0x30
#define INT1_SRC        0x31
#define INT1_TSH_XH     0x32
#define INT1_TSH_XL     0x33
#define INT1_TSH_YH     0x34
#define INT1_TSH_YL     0x35
#define INT1_TSH_ZH     0x36
#define INT1_TSH_ZL     0x37
#define INT1_DURATION   0x38

#define GYRO_CS         PORTBbits.RB11

#define TOGGLE_LED1         1
#define TOGGLE_LED2         2 
#define TOGGLE_LED3         3
#define READ_GYRO_TEMP      4
#define WHO_AM_I            5

_CONFIG1(FWDTEN_OFF & WINDIS_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF);
_CONFIG2(POSCMOD_NONE & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSECME & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_ON);
_CONFIG3(SOSCSEL_EC & WUTSEL_LEG & WPDIS_WPEN & WPCFG_WPCFGEN & WPEND_WPSTARTMEM & WPFP_WPFP3);

_PIN GYRO_MOSI, GYRO_MISO, GYRO_SCK;

int gyro_xyz[3];

unsigned char gyro_read(unsigned char);

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
        case READ_GYRO_TEMP:
            led_toggle(&led2);
            led_toggle(&led3);
            BD[EP0IN].address[0] = gyro_read(OUT_TEMP);
            BD[EP0IN].bytecount = 1;
            BD[EP0IN].status = 0xC8;
            break;
        case WHO_AM_I:
            led_toggle(&led3);
            BD[EP0IN].address[0] = gyro_read(WHO_AM_I);
            BD[EP0IN].bytecount = 1;
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

void gyro_write(unsigned char address, unsigned char value){
    GYRO_CS = 0;
    spi_transfer(&spi1, address&0x3F);
    spi_transfer(&spi1, value&0xFF);
    GYRO_CS = 1;
}

unsigned char gyro_read(unsigned char address) {
    unsigned char result;
    GYRO_CS = 0;
    spi_transfer(&spi1, 0x80|(address&0x3F));
    result = spi_transfer(&spi1, 0x00);
    GYRO_CS = 1;
    return result;
}

void gyro_init(void) {
    TRISBbits.TRISB11 = 0; //set the CS pin to be an output

    pin_init(&GYRO_MOSI, (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
            (unsigned int *)NULL, 8, -1, 0, 8, (unsigned int *)&RPOR4);
    pin_init(&GYRO_SCK, (unsigned int *)&PORTB, (unsigned int*)&TRISB,
            (unsigned int *)NULL, 9, -1, 8, 9, (unsigned int*)&RPOR4);
    pin_init(&GYRO_MISO, (unsigned int *)&PORTB, (unsigned int *)&TRISB,
            (unsigned int *)NULL, 14, -1, 0, 14, (unsigned int *)&RPOR7);

    spi_open(&spi1, &GYRO_MISO, &GYRO_MOSI, &GYRO_SCK, 2e6);
}

void main(void) {
    CLKDIV = 0x0100;
    init_ui();
    init_timer();
    init_pin();
    init_spi();
    gyro_init();
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
