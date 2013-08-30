#!/usr/local/bin/python

import usb.core
import struct


class xltest:

    def __init__(self):
        self.TOGGLE_LED1 = 1
        self.TOGGLE_LED2 = 2
        self.TOGGLE_LED3 = 3
        self.READ_ACCEL_XYZ = 4
        self.GET_ACCEL_OFFSETS = 5
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

    def read_accel_xyz(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_ACCEL_XYZ,
                                         data_or_wLength=3)
        except usb.core.USBError:
            print('Could not send READ_ACCEL_XYZ vendor request.')

        return struct.unpack('3b', ret)
        #return ret

    def get_accel_offsets(self):
        try:
            ret = self.dev.ctrl_transfer(0XC0, self.GET_ACCEL_OFFSETS,
                                         data_or_wLength=6)
            ret = struct.unpack('3h', ret)
        except usb.core.USBError:
            print('Could not send GET_ACCEL_OFFSETS vendor request.')

        return tuple([(x - 2048)/2 if x > 1203 else x/2 for x in ret])
