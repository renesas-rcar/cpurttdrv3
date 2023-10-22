SRC_V4H  := $(shell pwd)/v4h
SRC_V4M  := $(shell pwd)/v4m

all:
	$(MAKE) -C $(SRC_V4H)
	$(MAKE) -C $(SRC_V4M)

modules_install:
	$(MAKE) -C $(SRC_V4H) modules_install
	$(MAKE) -C $(SRC_V4M) modules_install

clean:
	$(MAKE) -C $(SRC_V4H) clean
	$(MAKE) -C $(SRC_V4M) clean

PHONY: all modules_install clean
