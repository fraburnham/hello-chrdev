ifneq ($(KERNELRELEASE),)

obj-m  := hello-dev.o

else

KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	rm hello-dev.ko hello-dev.o

endif
