## HOW-TO

1. Open a terminal (CTRL + ALT + T)
```
make clean all
./install.sh
gcc poll_test.c -o poll_test
./poll_test
```

2. Open another terminal
3. ```echo``` anything to ```/proc/cdev_wait```

⋅⋅⋅**Example:** ```echo "Hello there!" > /proc/cdev_wait```

4. Use ```./remove.sh``` to remove module and the device file.
