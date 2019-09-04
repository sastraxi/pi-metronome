from __future__ import print_function, absolute_import
from ble.characteristic import Characteristic
from ble.service import Service
from ble.descriptor import Descriptor
from ble import exceptions

from ble.constants import *

import dbus
import dbus.service

try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

import array
from random import randint

DEFAULT_BPM = 120

# N.B. our numbers are little-endian (least significant byte first)

def num_to_dbus(num):
	if num == 0: return [dbus.byte(0)]

	arr = dbus.Array()
	while num > 0:
		arr.append(dbus.Byte(num % 256))
		num = num >> 8

	return arr

def dbus_to_num(arr):
	return sum([int(v) * (2 ** (i * 8)) for (i, v) in enumerate(arr)])


class BpmService(Service):
	BPM_SVC_UUID = '839e1106-a81b-4162-9650-e7e66cd07e1c'

	def __init__(self, bus, index):
		Service.__init__(self, bus, index, self.BPM_SVC_UUID, True)
		self.add_characteristic(BpmCharacteristic(bus, 0, self))

class BpmCharacteristic(Characteristic):
	BPM_CHRC_UUID = 'a700e2e1-8f5a-41ef-baa6-6704802bdcbf'

	def __init__(self, bus, index, service):
		Characteristic.__init__(
				self, bus, index,
				self.BPM_CHRC_UUID,
				['read', 'write'],
				service)
		self.value = num_to_dbus(DEFAULT_BPM)

	def ReadValue(self, options):
		print('BpmCharacteristic Read: ' + repr(self.value))
		return self.value

	def WriteValue(self, value, options):
		print('BpmCharacteristic Write: ' + repr(value))
		self.value = value
		print('Decoded as: ' + repr(dbus_to_num(value)))
