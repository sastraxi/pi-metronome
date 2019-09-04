from __future__ import print_function, absolute_import
import dbus
import dbus.mainloop.glib
import dbus.service

import functools

try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

import ble.adapters as adapters

from ble.constants import *
from service.metronome import *

class Application(dbus.service.Object):
	"""
	org.bluez.GattApplication1 interface implementation
	"""
	def __init__(self, bus):
		self.path = '/'
		self.services = []
		dbus.service.Object.__init__(self, bus, self.path)
		self.add_service(BpmService(bus, 0))

	def get_path(self):
		return dbus.ObjectPath(self.path)

	def add_service(self, service):
		self.services.append(service)

	@dbus.service.method(DBUS_OM_IFACE, out_signature='a{oa{sa{sv}}}')
	def GetManagedObjects(self):
		response = {}
		print('GetManagedObjects')

		for service in self.services:
			response[service.get_path()] = service.get_properties()
			chrcs = service.get_characteristics()
			for chrc in chrcs:
				response[chrc.get_path()] = chrc.get_properties()
				descs = chrc.get_descriptors()
				for desc in descs:
					response[desc.get_path()] = desc.get_properties()

		return response

def register_app_cb():
	print('GATT application registered')

def register_app_error_cb(mainloop, error):
	print('Failed to register application: ' + str(error))
	mainloop.quit()

def gatt_server_main(mainloop, bus, adapter_name):
	adapter = adapters.find_adapter(bus, GATT_MANAGER_IFACE, adapter_name)
	if not adapter:
		raise Exception('GattManager1 interface not found')

	service_manager = dbus.Interface(
			bus.get_object(BLUEZ_SERVICE_NAME, adapter),
			GATT_MANAGER_IFACE)

	app = Application(bus)

	print('Registering GATT application...')

	service_manager.RegisterApplication(app.get_path(), {},
			reply_handler=register_app_cb,
			error_handler=functools.partial(register_app_error_cb, mainloop))

