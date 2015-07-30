#!/usr/bin/python
# accessory.py
# License GPLv2
# (c) Manuel Di Cerbo, Nexus-Computing GmbH

import usb.core
import usb.util
import struct
import time
import threading
import os
import socket
import sys
import sysv_ipc
import ctypes
from ctypes import*
from array import array
from attribs import*
		
ACCESSORY_VID = 0x18d1
ACCESSORY_PID = (0x2d00,0x2d01,0x2d04,0x2d05)
DEVICE_VID = 0x04e8 #galxy nexus : 0x04e8 , nexus one : 0x18d1   
DEVICE_PID = (0x6860,0x6861,0xec00,0x4e22)
NETLINK_KOBJECT_UEVENT = 15

FIFO_FILE = "/tmp/fifo"
BUFF_SIZE = 153600
KEY_NUM = 9527

arr = array('B',[77]*BUFF_SIZE)
arr_addr = arr.buffer_info()[0]
arr_len = len(arr) * arr.itemsize #itemsize 1byte

def main():
	while True:
		print("starting accessory task")    
		try:
		  	time.sleep(1)
			accessory_task()
			time.sleep(1)
		except ValueError:
			pass
		print("accessory task finished")
		
def accessory_task():
	print("Looking for device with VID 0x%0.4x" % DEVICE_VID)	
	dev = usb.core.find(idVendor=DEVICE_VID)#find device		
		
	if dev is None:#if is not connected
		raise ValueError("No compatible device not found with VID 0x%0.4x" % DEVICE_VID)	
	
	print("compatible device found with VID 0x%0.4x" % DEVICE_VID)

	if dev.idProduct in ACCESSORY_PID:
		print("device is in accessory mode, PID 0x%0.4x" % dev.idProduct)
		
	else:
		print("device is not in accessory mode yet")	
		accessory(dev)	#find device again, send accessory info to device		
		dev = usb.core.find(idVendor=ACCESSORY_VID)
		if dev is None:
			print("dev is None")
			raise ValueError("No compatible device not found")
	
		if dev.idProduct in ACCESSORY_PID:
			print("device is in accessory mode")
		else:
			print("dev.idProduct in ACCESSORY_PID == False")
			raise ValueError("it looks like device doesnt't support Accessory or app is not installed.")


#	dev.set_configuration()
	#check for working in properly again
	#even if the Android device is already in accessory mode
	#setting the configuration will result in the
	#UsbManager starting an "accessory connected" intent
	#and hence a small delay is required before communication
	#works properly

#
	
	print("setting up...")

	#tries = 5
	#while True:
		#try:
			#if tries <= 0:
				#raise ValueError("it looks like really failed. bye")
			#dev.set_configuration()
			#break
	#	except:
		#	print("Failed to set up. retrying...")
			#tries -= 1
			#time.sleep(1)
	print("OK")

	time.sleep(1)

	dev = usb.core.find(idVendor = ACCESSORY_VID)
	if dev is None:
		dev = usb.core.find(idVendor = DEVICE_VID)

	if dev is None:
		raise ValueError("device set to accessory mode but VID %04X not found"% DEVICE_VID)



	print("Getting file descriptor")
	cfg = dev.get_active_configuration()#get file discriptor, make environment for IO
	if_num = cfg[(0,0)].bInterfaceNumber
#	alternate_setting = usb.control.get_interface(dev, if_num)
#	intf = usb.util.find_descriptor(cfg, bInterfaceNumber = if_num, bAlternateSetting=alternate_setting)
	intf = usb.util.find_descriptor(cfg, bInterfaceNumber = if_num)	
#	ep_out = usb.util.find_descriptor(
#		intf,
#		custom_match = \
#		lambda e: \
#			usb.util.endpoint_direction(e.bEndpointAddress) == \
#			usb.util.ENDPOINT_OUT
#	)

	ep_in = usb.util.find_descriptor(
		intf,
		custom_match = \
		lambda e: \
			usb.util.endpoint_direction(e.bEndpointAddress) == \
			usb.util.ENDPOINT_IN
	)

#	print("Starting writer thread")
#	writer_thread = threading.Thread(target = writer, args = (ep_out, ))
#	writer_thread.start() #writing what user want in thread 
#	endpoint = dev[0][(0,0)][0]
#	length = -1

	memory = sysv_ipc.SharedMemory(9527)
	
#ctypes.memmove(memory.address, arr_addr, arr_len)
	print("Shared Memory, reading from android")
	cnt = 0
	while True:#read data from android
		try:
			print("before ep_in.read")
			data = ep_in.read( size = BUFF_SIZE, timeout = 0)
#			for i in range(0,30000,30):
#				print("read value :%d"% data[i])
#				print("read value :%d"% data[i+1])	
#				print("------------%d"% i)
#			print("read value153599 %d" % data[153599])
			
#			print data.buffer_info()[0]
#			print data.itemsize
#			print len(data)
#	writer_thread.join()
			print("read value0 : %d"% data[0])

			print("read value1 : %d"% data[1])

#			ctypes.memmove(memory.address, data.buffer_info()[0], len(data))
#			ctypes.memmove(memory.address, arr_addr, arr_len)
		except usb.core.USBError:
			print("failed to send IN transfer")
			break
	
#	writer_thread.join()
#	fd.close()		
	print("close fifo")		
	print("exiting application")

def writer (ep_out):
	while True:
		try:
			length = ep_out.write([0], timeout = 0)
			print("%d bytes written" % length)
			time.sleep(0.5)
		except usb.core.USBError:
			print("error in writer thread")
			break

def accessory(dev):
	version = dev.ctrl_transfer(
				usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_IN,
				51, 0, 0, 2)

	print("version is: %d" % struct.unpack('<H',version))

	assert dev.ctrl_transfer(
			usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_OUT,
			52, 0, 0, MANUFACTURER) == len(MANUFACTURER)
	
	assert dev.ctrl_transfer(
			usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_OUT,
			52, 0, 1, MODEL_NAME) == len(MODEL_NAME)
	
	assert dev.ctrl_transfer(
			usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_OUT,
			52, 0, 2, DESCRIPTION) == len(DESCRIPTION)

	assert dev.ctrl_transfer(
			usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_OUT,
			52, 0, 3, VERSION) == len(VERSION)

	assert dev.ctrl_transfer(
			usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_OUT,
			52, 0, 4, URL) == len(URL)

	assert dev.ctrl_transfer(
			usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_OUT,
			52, 0, 5, SERIAL_NUMBER) == len(SERIAL_NUMBER)

	dev.ctrl_transfer(
			usb.util.CTRL_TYPE_VENDOR | usb.util.CTRL_OUT,
			53, 0, 0, None)
	
	time.sleep(1)

if __name__ == "__main__":
	main()

