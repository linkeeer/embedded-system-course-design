TARGET = test_led
CROSS_COMPILE = arm-linux-

CC              = $(CROSS_COMPILE)gcc
STRIP           = $(CROSS_COMPILE)strip
#CFLAGS          = -O2

ifeq ($(KERNELRELEASE),)
    KERNELDIR ?= /root/kernel/linux-2.6.24.4
    PWD := $(shell pwd)

all:	$(TARGET) modules

$(TARGET):
	$(CC) -o $(TARGET) $(TARGET).c

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions $(TARGET)

.PHONY:	modules modules_install clean

else
obj-m := led.o
endif

