# Python BLE GATT Server (peripheral)
GATT is constructed out of one or more server devices (BLE peripherals) and a client device (BLE central).

A GATT server is usually a small device such as a sensor, but for some use cases you might want to have a Linux computer such as a RPi used as a GATT server. This example is meant to demonstrate how this can be done.

## Setup
The instructions in this document were tested on Ubuntu 16.04.

### Bluez Configuration
Enable experimental features for the bluetooth driver: 
- `sudo nano /lib/systemd/system/bluetooth.service`
- Add `--experimental` to `ExecStart`. The line should look like this: 

    `ExecStart=/usr/local/libexec/bluetooth/bluetoothd --experimental`

Reload the service:
```bash
systemctl daemon-reload
sudo service bluetooth restart
```

### virtualenv
Install dependencies: `sudo apt-get install virtualenv python-dev libdbus-1-dev libdbus-glib-1-dev python-gi`

`cd` to the the root of this repository and:

```bash
virtualenv venv
source venv/bin/activate
pip install -r requirements.txt
ln -s /usr/lib/python2.7/dist-packages/gi venv/lib/python2.7/site-packages/
```

## License
The code in this repository is based on code taken from the [BlueZ](http://www.bluez.org/) project. It is licensed under GPL 2.0
