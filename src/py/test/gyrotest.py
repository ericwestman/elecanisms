#!/usr/local/bin/python

import usb.core
import struct


class gyrotest:

    def __init__(self):
        self.TOGGLE_LED1 = 1
        self.TOGGLE_LED2 = 2
        self.TOGGLE_LED3 = 3
        self.READ_GYRO_TEMP = 4
        self.WHO_AM_I = 5

        self.dev = usb.core.find(idVendor=0x6666, idProduct=0x0003)
        if self.dev is None:
            raise ValueError('No USB device found matching idVendor = 0x6666' +
                             ' and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def toggle_led(self, id):
        try:
            self.dev.ctrl_transfer(0x40, id)
        except usb.core.USBError:
            print('Could not toggle LED ' + str(id))

    def read_temp(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_GYRO_TEMP,
                                         data_or_wLength=1)
        except usb.core.USBError:
            print('Could not send READ_GYRO_TEMP vendor request.')

        return struct.unpack('b', ret)[0]

    def who_am_i(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.WHO_AM_I,
                                         data_or_wLength=1)
        except usb.core.USBError:
            print('Could not send WHO_AM_I vendor request.')

        return struct.unpack('B', ret)

