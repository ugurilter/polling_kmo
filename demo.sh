#!/bin/bash

proc="/proc/cdev_wait"

sudo echo "Hello World !" > "$proc"
sudo echo "This is a test message." > "$proc"
sudo echo "Lorem ipsum dolor sit amet," > "$proc"
sudo echo "pri explicari splendide elaboraret no." > "$proc"
