This Linux kernel module create a device called */dev/xkcdrandom* that returns [values that are guaranteed to be random](https://xkcd.com/221/).

To build it, you'll need the *linux-headers* package for your current Linux kernel. Then, run

    make

to build the module, and load it using

    sudo insmod xkcdrandom.ko

You can then enjoy your randomness by reading from the character device */dev/xkcdrandom*!

All files in this repository are available under the terms of [CC0](https://creativecommons.org/publicdomain/zero/1.0/).
