#!/bin/bash

dev="char_device"
major="555"

sudo insmod ugur_kmo.ko name="Ugur"
sudo mknod "/dev/$dev" c "$major" 0
sudo chmod 666 /dev/$dev

