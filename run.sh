#!/bin/bash

echo ""
echo -n "1.	Writing to /dev/char_device ..."
sudo echo -n "$1" > /dev/char_device
echo "	[✔] Done!"
echo "2.	Reading from /dev/char_device now ..."
echo -n "3.	Result: "
sudo cat /dev/char_device
echo ""
echo ""
