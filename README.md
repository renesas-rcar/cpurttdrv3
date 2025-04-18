# Overview
CPURTT kernel driver is used to execute Runtime test in 2 modes on V4H and V4M devices:
* Region ID (RGID) ON. 
* Region ID (RGID) OFF. 

In each mode, when accessing to registers, user needs to set the RGID value of each mode to bits from 36 to 39 of each register address.

The RGID is set to the default value as below. 
| **Mode**  | **Value** |
|:----------|:----------|
| RGID ON   |2| 
| RGID OFF  |0|

**Note:** <br> 1. The values of RGID of each HW modules are configured by IPL. Please make sure to set it consistently. <br> 2. When the RGID ON mode is used, the valid RGID values are from 1 to 15.

# Build guideline

To build the source code of CPURTT kernel driver, user needs to follow the required setting described for each mode as below.

## RGID ON mode

* Set the flag as below:
```bash
$ RGID_ON="1"
```
**For example:**

If the user builds the source code using a Makefile, a flag can be passed as a variable like this:
```bash
make all RGID_ON="1"
```

## RGID OFF mode 

* This is the default build environment, so there is no required setting.

# RGID Configuration 

To specify the Region, user needs to modify the values of definitions described in below table in the location path.

| **Definition**         | **Meaning**                                              | **Location**                        | **Remark** |
|:-----------------------|:---------------------------------------------------------|:------------------------------------|:--------------------|
| DRV_RTTKER_OTPMEM_RGID |Region ID to access to base address of OTP_MEM_1 registers|./cpurttdrv/{device_name}/cpurttdrv.h|device_name: v4h, v4m|
| DRV_RTTKER_FBC_RGID    |Region ID to access to base address of FBC registers      |./cpurttdrv/{device_name}/cpurttdrv.h|device_name: v4h, v4m|
| DRV_RTTKER_APMU_RGID   |Region ID to access to base address of APMU registers     |./cpurttdrv/{device_name}/cpurttdrv.h|device_name: v4h, v4m|

**For example:**

To access to HW IP at Region 3, user should configure the RGID value as below:

```bash
#define DRV_RTTKER_OTPMEM_RGID 0x3ULL
#define DRV_RTTKER_FBC_RGID    0x3ULL
#define DRV_RTTKER_APMU_RGID   0x3ULL
```