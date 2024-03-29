# Raspberry Pi Visual Metronome

Install dependencies:
```
sudo apt install build-essential autotools-dev autoconf automake libtool libfftw3-dev pigpio

# runtime deps
sudo apt install bluez pulseaudio-module-bluetooth python-gobject python-gobject-2 bluez-tools
```

Build the program:
```
aclocal && autoconf && automake
./configure && make
```

If you get errors, run these first:
```
aclocal
automake
```

Run it:
```
parec --format=s16le --rate=22050 --channels=1 --device=alsa_output.firewire-0x00148605c440c13a.multichannel-output.monitor | ./src/metronome
```

You'll have to change the device name above with a `RUNNING` sink monitor from `pactl list`.

## To make your bluetooth device discoverable

```
bluetoothctl discoverable on
```

## Some helpful resources

* https://stackoverflow.com/questions/2531827/what-are-makefile-am-and-makefile-in
* https://www.hamvocke.com/blog/makefiles-for-accessibility/
* https://unix.stackexchange.com/questions/105964/launch-a-fake-minimal-x-session-for-pulseaudio-dbus
* https://stackoverflow.com/questions/7502380/streaming-pulseaudio-to-file-possibly-with-gstreamer
* https://www.instructables.com/id/Turn-your-Raspberry-Pi-into-a-Portable-Bluetooth-A/
* https://github.com/jobpassion/raspberryPi/blob/master/BluetoothSpeaker.md

## Libraries used

* [dr_wav](https://mackron.github.io/dr_wav) (included as `dr_wav.h`)
* [fftw3](http://www.fftw.org/)
