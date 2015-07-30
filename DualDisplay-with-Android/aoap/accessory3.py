#!/usr/bin/python

import usb.core
import usb.util
import fcntl
import time
import threading
import os
import sys
import socket

MANUFACTURER = "Nexus-Computing GmbH"
MODEL_NAME = "Simple Slider"
DESCRIPTION = "A Simple Slider"
VERSION = "0.1"
URL = "http://www.nexus-computing.ch/SimpleAccessory.apk"
SERIAL_NUMBER = "1337"

NETLINK_KOBJECT_UEVENT = 15
VID = 0x04e8
ACCESSORY_VID = 0X18d1
ACCESSORY_PID = (0x2D00, 0x2d01, 0x2D04, 0x2D05)

def main():

	while True:
		print("========================================")
		print("Looking for devices...")
		try:
			sock = socket.socket(
			socket.AF_NETLINK,
			socket.SOCK_RAW,
			NETLINK_KOBJECT_UEVENT	
			)
			sock.bind((os.getpid(), -1))
			vid = 0
			while True:
				print("data = sock.recv(512) start")
				data = sock.recv(512)
				print("data = sock.recv(512) done")
				try:
					vid = parse_uevent(data)
					if vid != None:
						print("Device found!")
						break
				except ValueError:
					print("Unable to parse uevent")
			sock.close()
			print("Connecting...")
			accessory_task(vid)
		except ValueError as e:
			print(e)
		print("End of connection")


def parse_uevent(data):
		print("in parse_uevent")
		lines = data.split('\0')
		keys = []
		for line in lines:
			val = line.split('=')
			if len(val) == 2:
				keys.append((val[0], val[1]))

			attributes = dict(keys)
			if 'ACTION' in attributes and 'PRODUCT' in attributes:
				if attributes['ACTION'] == 'add':
					parts = attributes['PRODUCT'].split('/')
					return int(parts[0], 16)
	    
			return None


def accessory_task(vid):
	print("Looking for device with VID %04X" % vid)
	
	dev = usb.core.find(idVendor=vid)

	if dev is None:
		raise ValueError("No compatible device not found with VID %04X" % vid)
				   
	print("Compatible device found with VID %04X" % vid)



	if dev.idProduct in ACCESSORY_PID:
		print("Device is in accessory mode, PID %04X" % dev.idProduct)
	else:
		
		print("Device is not in accessory mode yet, VID %04X" % vid)	
		print("Setting up...")
		accessory(dev)
		
		dev = usb.core.find(idVendor=ACCESSORY_VID)
		if dev is None:
			print("dev == None")
			raise ValueError("No compatible device not found")
			   
		if dev.idProduct in ACCESSORY_PID:
			print("Device is in accessory mode, PID %04X" % dev.idProduct)
		else:
			print("dev.idProduct in ACCESSORY_PID == False")
			raise ValueError("It looks like device doesn't supports Accessory or app is not installed.")


	print("Setting up...")

	tries = 5
	while True:
		try:
			if tries <= 0:
				raise ValueError("It looks like really failed. bye~") 
			dev.set_configuration()
			break
		except:
			print("Failed to set up. retrying...")
			tries -= 1
			time.sleep(1)
	print("OK.")
	
	# even if the Android device is already in accessory mode
	# setting the configuration will result in the	
	# UsbManager starting an "accessory connected" intent
	# and hence a small delay is required before communication
	# works properly
	time.sleep(1)
	print("Looking for device which is set to accessory mode.")
	dev = usb.core.find(idVendor = ACCESSORY_VID);

	if dev is None:
		dev = usb.core.find(idVendor = vid);

	if dev is None:
		raise ValueError("Device set to accessory mode but VID %04X not found" % vid)

	
	print("Getting file descriptor")
	cfg = dev.get_active_configuration()
	if_num = cfg[(0,0)].bInterfaceNumber
	intf = usb.util.find_descriptor(cfg, bInterfaceNumber = if_num)

	ep_out = usb.util.find_descriptor(
				intf,
				custom_match = \
				lambda e: \
					usb.util.endpoint_direction(e.bEndpointAddress) == \
					usb.util.ENDPOINT_OUT
				)
	ep_in = usb.util.find_descriptor(
				intf,
				custom_match = \
				lambda e: \
					usb.util.endpoint_direction(e.bEndpointAddress) == \
					usb.util.ENDPOINT_IN
				)
	   




	print("Starting writer thread")
	writer_thread = threading.Thread(target = writer, args = (ep_out, ))
	writer_thread.start()


	length = -1
	print("Reading...")
	while True:
		try:
			print("TR: Reading..." )
			data = ep_in.read(size = 1, timeout = 0)
			print("TR: Read value %d" % data[0])
		except usb.core.USBError as e:
			print("TR: Failed to send IN transfer")
			print(e)
			break
	writer_thread.join()
	print("Exiting application")


def writer (ep_out):
	while True:
		try:
			print("TW: Writing...")
			length = ep_out.write([0], timeout = 0)
			print("TW: %d bytes written" % length)
			time.sleep(0.5)
		except usb.core.USBError:
			print("TW: Error in writer thread")
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
	pid_file = '/root/program.pid'
	fp = open(pid_file, 'w')
	try:
				fcntl.lockf(fp, fcntl.LOCK_EX | fcntl.LOCK_NB)
	except IOError:
				sys.exit(0)
	
	main()

