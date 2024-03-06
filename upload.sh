#!/usr/bin/env bash

arduino-cli upload -p /dev/cu.usbmodemF412FA6550142 --fqbn arduino:renesas_uno:unor4wifi src/client/client.ino
arduino-cli monitor -p /dev/cu.usbmodemF412FA6550142
