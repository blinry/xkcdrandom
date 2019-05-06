obj-m += xkcdrandom.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	# Prefixed with a - so that we don't abort when xkcdrandom isn't loaded yet.
	-sudo rmmod xkcdrandom
	sudo insmod xkcdrandom.ko
