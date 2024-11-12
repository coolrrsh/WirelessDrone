obj-m += pwm_driver.o
KDIR = ../../bsp_src/linux/
all:
	make -C $(KDIR) M=$(shell pwd) modules
clean:
	make -C $(KDIR) M=$(shell pwd) clean

