obj-m += pcd_m.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean:
	 make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

