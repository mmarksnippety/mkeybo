# #!/usr/bin/env python3
# #
# # Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
# #
# # SPDX-License-Identifier: BSD-3-Clause
# #
#
# # sudo pip3 install pyusb

import sys

from evdev import InputDevice, categorize, ecodes

device_file = sys.argv[1]  # "/dev/input/event24"
print("Device: ", device_file)
device = InputDevice(device_file)
for event in device.read_loop():
    if event.type == ecodes.EV_KEY:
        print(categorize(event))



# import usb.core
# import usb.util
#
# # find our device
# dev = usb.core.find(idVendor=0xcafe, idProduct=0x4004)
#
# # was it found?
# if dev is None:
#     raise ValueError('Device not found')
#
# print("Dev")
# for cfg in dev:
#     print(str(cfg.bConfigurationValue) + '\n')
#     for intf in cfg:
#         print('\t' + \
#               str(intf.bInterfaceNumber) + \
#               ',' + \
#               str(intf.bAlternateSetting) + \
#               '\n')
#         for ep in intf:
#             print('\t\t' + \
#                   str(ep.bEndpointAddress) + \
#                   '\n')
#
# # get an endpoint instance
# cfg = dev.get_active_configuration()
# intf = cfg[(0, 0)]
#
# # outep = usb.util.find_descriptor(
# #     intf,
# #     # match the first OUT endpoint
# #     custom_match= \
# #         lambda e: \
# #             usb.util.endpoint_direction(e.bEndpointAddress) == \
# #             usb.util.ENDPOINT_OUT)
# #
# # inep = usb.util.find_descriptor(
# #     intf,
# #     # match the first IN endpoint
# #     custom_match= \
# #         lambda e: \
# #             usb.util.endpoint_direction(e.bEndpointAddress) == \
# #             usb.util.ENDPOINT_IN)
# #
# # assert inep is not None
# # assert outep is not None
# #
# # test_string = "Hello World!"
# # outep.write(test_string)
# # from_device = inep.read(len(test_string))
#
# # print("Device Says: {}".format(''.join([chr(x) for x in from_device])))
