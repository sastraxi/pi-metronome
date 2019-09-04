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

class BpmService(Service):
	TEST_SVC_UUID = '12345678-1234-5678-1234-56789abcdef0'

	def __init__(self, bus, index):
		Service.__init__(self, bus, index, self.TEST_SVC_UUID, True)
		self.add_characteristic(BpmCharacteristic(bus, 0, self))

class BpmCharacteristic(Characteristic):
	TEST_CHRC_UUID = '12345678-1234-5678-1234-56789abcdef1'

	def __init__(self, bus, index, service):
		Characteristic.__init__(
				self, bus, index,
				self.TEST_CHRC_UUID,
				['read', 'write'],
				service)
		self.value = []
		self.add_descriptor(TestDescriptor(bus, 0, self))

	def ReadValue(self, options):
		print('TestCharacteristic Read: ' + repr(self.value))
		return self.value

	def WriteValue(self, value, options):
		print('TestCharacteristic Write: ' + repr(value))
		self.value = value

class TestDescriptor(Descriptor):
	"""
	Dummy test descriptor. Returns a static value.

	"""
	TEST_DESC_UUID = '12345678-1234-5678-1234-56789abcdef2'

	def __init__(self, bus, index, characteristic):
		Descriptor.__init__(
				self, bus, index,
				self.TEST_DESC_UUID,
				['read', 'write'],
				characteristic)

	def ReadValue(self, options):
		return [
				dbus.Byte('T'), dbus.Byte('e'), dbus.Byte('s'), dbus.Byte('t')
		]

	def WriteValue(self, value, options):
		print("TestDescriptor Write!", value, options)
