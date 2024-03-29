from __future__ import print_function, absolute_import
from ble.characteristic import Characteristic
from ble.service import Service
from ble.descriptor import Descriptor
from ble import exceptions

from ble.constants import *

import dbus
import dbus.service

import subprocess, threading, time

##################################
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

##################################
DEFAULT_BPM = 120

class BpmService(Service):
  BPM_SVC_UUID = '839e1106-a81b-4162-9650-e7e66cd07e1c'

  def __init__(self, bus, index):
    Service.__init__(self, bus, index, self.BPM_SVC_UUID, True)
    self.process = None
    self.thread = None
    self.add_characteristic(BpmCharacteristic(bus, 0, self))

  def launch_bpm(self, bpm):
    def run_thread():
      print('run thread!', repr(self))
      self.process = subprocess.Popen(["/home/pi/dev/pi-metronome/src/bpm", repr(bpm), "0"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
      self.process.wait()
      period = self.process.stdout.read()
      print('output of bpm: ', period)
      
    if self.process is not None:
      self.process.terminate()
      self.thread.join()
      self.process = None

    if bpm != 0:
      self.thread = threading.Thread(target=run_thread)
      self.thread.daemon = True
      self.thread.start()


class BpmCharacteristic(Characteristic):
  BPM_CHRC_UUID = 'a700e2e1-8f5a-41ef-baa6-6704802bdcbf'

  def __init__(self, bus, index, service):
    Characteristic.__init__(
        self, bus, index,
        self.BPM_CHRC_UUID,
        ['read', 'write'],
        service)
    self.value = num_to_dbus(DEFAULT_BPM)
    self.service.launch_bpm(DEFAULT_BPM)

  def ReadValue(self, options):
    print('BpmCharacteristic Read: ' + repr(self.value))
    return self.value

  def WriteValue(self, value, options):
    new_bpm = dbus_to_num(value)
    if new_bpm != dbus_to_num(self.value):
      print('Setting BPM to ' + repr(new_bpm))
      self.value = value
      self.service.launch_bpm(new_bpm)


