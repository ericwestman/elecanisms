#include <p24FJ128GB206.h>
#include "usb.h"
#include "util.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"
#include "spi.h"

#define XOUTL   0x00
#define XOUTH   0x01
#define YOUTL   0x02
#define YOUTH   0x03
#define ZOUTL   0x04
#define ZOUTH   0x05
#define XOUT8   0x06
#define YOUT8   0x07
#define ZOUT8   0x08
#define STATUS  0x09
#define DETSRC  0x0A
#define TOUT    0x0B
#define I2CADD  0x0D
#define USRINF  0x0E
#define WHOAMI  0x0F
#define XOFFL   0x10
#define XOFFH   0x11
#define YOFFL   0x12
#define YOFFH   0x13
#define ZOFFL   0x14
#define ZOFFH   0x15
#define MCTL    0x16
#define INTRST  0x17
#define CTL1    0x18
#define CTL2    0x19
#define LDTH    0x1A
#define PDTH    0x1B
#define PW      0x1C
#define LT      0x1D
#define TW      0x1E

#define ACCEL_CS            PORTBbits.RB13 

#define STANDBY_MODE        0b00000000
#define MEASURE_MODE        0b00000001
#define LEVEL_DETECT_MODE   0b00000010
#define PULSE_DETECT_MODE   0b00000011

#define RANGE_8G            0b00000000
#define RANGE_2G            0b00000100
#define RANGE_4G            0b00001000

#define TOGGLE_LED1         1
#define TOGGLE_LED2         2 
#define TOGGLE_LED3         3
#define READ_ACCEL_XYZ      4
#define GET_ACCEL_OFFSETS   5



_CONFIG1(FWDTEN_OFF & WINDIS_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF);
_CONFIG2(POSCMOD_NONE & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSECME & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_ON);
_CONFIG3(SOSCSEL_EC & WUTSEL_LEG & WPDIS_WPEN & WPCFG_WPCFGEN & WPEND_WPSTARTMEM & WPFP_WPFP3);


_PIN ACCEL_MOSI, ACCEL_MISO, ACCEL_SCK;

int accel_xyz[3];
int accel_offsets[3];

void accel_read_xyz(int * data);

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

void accel_write(unsigned char address, unsigned char value) {
    unsigned char tmp;

    ACCEL_CS = 0;
    spi_transfer(&spi1, 0x80|((address&0x3F)<<1));
    spi_transfer(&spi1,value&0xFF);
    ACCEL_CS = 1;
}

unsigned char accel_read(unsigned char address) {
    unsigned char result;
    ACCEL_CS = 0;
    spi_transfer(&spi1, (address&0x3F)<<1);
    result = spi_transfer(&spi1, 0x00);
    ACCEL_CS = 1;
    return result;
}

signed char accel_read_signed(unsigned char address) {

}

void accel_init(void) {

    TRISBbits.TRISB13 = 0;

    pin_init(&ACCEL_MOSI, (unsigned int *)&PORTB, (unsigned int *)&TRISB, 
            (unsigned int *)NULL, 8, -1, 0, 8, (unsigned int *)&RPOR4);
    pin_init(&ACCEL_SCK, (unsigned int *)&PORTB, (unsigned int*)&TRISB,
            (unsigned int *)NULL, 9, -1, 8, 9, (unsigned int*)&RPOR4);
    pin_init(&ACCEL_MISO, (unsigned int *)&PORTB, (unsigned int *)&TRISB,
            (unsigned int *)NULL, 14, -1, 0, 14, (unsigned int *)&RPOR7);
    
    spi_open(&spi1, &ACCEL_MISO, &ACCEL_MOSI, &ACCEL_SCK, 2e6);
  
    accel_write(I2CADD, 0x80);        //Disable I2C
}

void accel_set_measure_mode(void) {
    accel_write(MCTL, 0x05);              //2g measurement mode
    accel_write(CTL1, 0x80); //set absolute condition, 125Hz BW, all axes enabled
}

void accel_read_xyz(int * data){

    data[0] = (int)(signed char)accel_read(XOUT8);
    data[1] = (int)(signed char)accel_read(YOUT8);
    data[2] = (int)(signed char)accel_read(ZOUT8);

}

void accel_calibrate(int * offsets) {
    int xcal, ycal, zcal, z;
    
    accel_write(XOFFL, 0);
    accel_write(XOFFH, 0);
    accel_write(YOFFL, 0);
    accel_write(YOFFH, 0);
    accel_write(ZOFFL, 0);
    accel_write(ZOFFH, 0);

    accel_read_xyz((int *)accel_xyz);
    xcal = -2 * accel_xyz[0];
    xcal = xcal < 0 ? xcal + 2048 : xcal;
    ycal = -2 * accel_xyz[1];
    ycal = ycal < 0 ? ycal + 2048 : ycal;
    zcal = -2 * (accel_xyz[2] - 64);
    zcal = zcal < 0 ? zcal + 2048 : zcal;

    accel_write(XOFFL, xcal&0x00FF);
    accel_write(XOFFH, xcal>>8);
    accel_write(YOFFL, ycal&0x00FF);
    accel_write(YOFFH, ycal>>8);
    accel_write(ZOFFL, zcal&0x00FF);
    accel_write(ZOFFH, zcal>>8);

    offsets[0] = xcal;
    offsets[1] = ycal;
    offsets[2] = zcal;
}

void main(void) {
    CLKDIV = 0x0100;
    init_ui();
    init_timer();
    init_pin();
    init_spi();
    accel_init();
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
