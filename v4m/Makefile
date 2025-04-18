obj-m :=cpurttmod3_v4m.o
cpurttmod3_v4m-objs := smoni_api.o cpurttdrv.o

$(info RGID_ON = $(RGID_ON))

SRC := $(shell pwd)

ifeq ("$(RGID_ON)","1")
    EXTRA_CFLAGS += -DCONFIG_ENABLE_RGID
endif

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules_install

clean:
	rm -f *.o *.a *~ core .depend .*.cmd *.ko *.mod*
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

