obj-m += lfsr.o

PWD := $(CURDIR)

ccflags-y := -Wimplicit-fallthrough=0 -std=gnu99

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
