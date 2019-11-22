KERNELDIR=/lib/modules/`uname -r`/build
#ARCH=i386
#KERNELDIR=/usr/src/kernels/`uname -r`-i686

EXTRA_CFLAGS += -I$(PWD)
EXTRA_CFLAGS += -std=gnu99
MODULES = charDeviceDriver.ko charDeviceDriverBlocking.ko
obj-m += charDeviceDriver.o charDeviceDriverBlocking.o

PROGS = ioctl

all: $(MODULES)  $(PROGS)

charDeviceDriver.ko: charDeviceDriver.c
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -f $(PROGS) *.o

install:
	make -C $(KERNELDIR) M=$(PWD) modules_install

quickInstall:
	cp $(MODULES) /lib/modules/`uname -r`/extra

ioctl: ioctl.o
	gcc -Wall -Werror -o $@ $<

ioctl.o: ioctl.c
	gcc -Wall -Werror -c $<

custom_test1: custom_test1.o
	gcc -Wall -Werror -o custom_test1 custom_test1.o -pthread

custom_test1.o: custom_test1.c
	gcc -Wall -Werror -c custom_test1.c -pthread