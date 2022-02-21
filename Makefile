SRC_V4H  := $(shell pwd)/v4h

all:
	$(MAKE) -C $(SRC_V4H)

modules_install:
	$(MAKE) -C $(SRC_V4H) modules_install

clean:
	$(MAKE) -C $(SRC_V4H) clean

PHONY: all modules_install clean
