obj-m :=cpurttmod3_v4m.o
cpurttmod3_v4m-objs := smoni_api.o cpurttdrv.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *.a *~ core .depend .*.cmd *.ko *.mod*
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

