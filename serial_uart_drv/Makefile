obj-m := hw_serial.o
KDIR := ../../bsp_src/linux/
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

