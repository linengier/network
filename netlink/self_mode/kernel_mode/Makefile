
obj-m:=netlink_mode.o

KERDIR=/lib/modules/$(shell uname -r)/build
CURDIR=$(shell pwd)

all:
	make -C $(KERDIR) M=$(CURDIR) modules
clean:
	make -C $(KERDIR) M=$(CURDIR) clean

