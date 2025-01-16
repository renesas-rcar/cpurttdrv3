/****************************************************************************/
/*
 * FILE          : cpurtt_common.h
 * DESCRIPTION   : CPU Runtime Test driver
 * CREATED       : 2021.02.15
 * MODIFIED      : 2025.01.16
 * AUTHOR        : Renesas Electronics Corporation
 * TARGET DEVICE : R-Car V4M
 * TARGET OS     : BareMetal
 * HISTORY       :
 *                 2021.02.15 Create New File corresponding to BareMetal
 *                 2021.03.12 Fix for beta2 release
 *                 2021.04.15 Fix for beta3 release
 *                 2021.10.19 Delete unused definition value.
 *                            Move definition values that do not need to be shared with the user layer.
 *                 2022.01.24 Added definition value for HWA Runtime Test.
 *                 2022.02.14 Modify for V4H.
 *                 2022.04.08 Modify drvCPURTT_CallbackInfo_t.
 *                 2022.08.23 Support A2 runtime test.
 *                            Remove processing related to ConfigRegCheck and HWA Execute.
 *                 2022.12.14 Add smoni UDF.
 *                 2025.01.16 Modify ioctl command for drvCPURTT_UDF_FbistInitialize.
 */
/****************************************************************************/
/*
 * Copyright(C) 2021-2025 Renesas Electronics Corporation. All Rights Reserved.
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation.
 * No part of this program may be reproduced or disclosed to
 * others, in any form, without the prior written permission
 * of Renesas Electronics Corporation.
 *
 ****************************************************************************/

#ifndef CPURTT_COMMON_H
#define CPURTT_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

/* for smoni_api parameter */
typedef struct {
    uint32_t Index;
    uint32_t CpuId;
    uint32_t RetArg;
    void*    Arg;
} drvCPURTT_SmoniParam_t;

typedef struct {
    uint32_t Rttex;
} drvCPURTT_A1rttParam_t;

typedef struct {
    uint32_t Rttex;
} drvCPURTT_A2rttParam_t;

typedef struct {
    uintptr_t AddrBuf;
    uintptr_t DataBuf;
    uint32_t RegCount;

} drvCPURTT_FbaWriteParam_t;

typedef struct {
    uintptr_t AddrBuf;
    uintptr_t DataBuf;
    uint32_t RegCount;
} drvCPURTT_FbaReadParam_t;

typedef struct {
    uint32_t Target;
    uint32_t MicroSec;
} drvCPURTT_SetTimeoutParam_t;

typedef struct {
    uint32_t Rttex;
    uint32_t TargetHierarchy;
} drvCPURTT_SelfCheckParam_t;

typedef struct {
    uint32_t Arg1;
    uint32_t Arg2;
    uint32_t Arg3;
} drvCPURTT_UdfParam_t;

/* index for smoni api */
typedef enum
{
    DRV_CPURTT_SMONIAPI_SETTIMEOUT,
    DRV_CPURTT_SMONIAPI_LOCKACQUIRE,
    DRV_CPURTT_SMONIAPI_LOCKRELEASE,
    DRV_CPURTT_SMONIAPI_A1EXE,
    DRV_CPURTT_SMONIAPI_A2EXE,
    DRV_CPURTT_SMONIAPI_FBAWRITE,
    DRV_CPURTT_SMONIAPI_FBAREAD,
    DRV_CPURTT_SMONIAPI_SELFCHECK,
    DRV_CPURTT_SMONIAPI_UDF1,
    DRV_CPURTT_SMONIAPI_UDF2,
    DRV_CPURTT_SMONIAPI_SMONITABLE_MAX
} drvCPURTT_SmoniTable_t;

typedef enum
{
    DRV_CPURTT_PRODUCT_V4M_7 = 0,
    DRV_CPURTT_PRODUCT_V4M_5,
    DRV_CPURTT_PRODUCT_V4M_3,
    DRV_CPURTT_PRODUCT_V4M_2,
} drvCPURTT_ProductId_t;

typedef struct {
    uint64_t FbistCbRequest;
    uint64_t RfsoOutputPinRequest;
} drvCPURTT_CallbackInfo_t;

typedef struct {
    drvCPURTT_ProductId_t ProductId;
} drvCPURTT_FbistInitParam_t;

/* Command definition for ioctl */
#define DRV_CPURTT_IOCTL_MAGIC  (0x9AU)
#define DRV_CPURTT_CMD_CODE     (0x1000U)

#define DRV_CPURTT_IOCTL_DEVINIT    _IO( DRV_CPURTT_IOCTL_MAGIC, DRV_CPURTT_CMD_CODE )                                      /* ioctl command for drvCPURTT_UDF_DrvInitialize */
#define DRV_CPURTT_IOCTL_DEVDEINIT  _IO( DRV_CPURTT_IOCTL_MAGIC, DRV_CPURTT_CMD_CODE + 1 )                                  /* ioctl command for drvCPURTT_UDF_DrvDeInitialize */
#define DRV_CPURTT_IOCTL_SMONI      _IOWR( DRV_CPURTT_IOCTL_MAGIC, DRV_CPURTT_CMD_CODE + 2, drvCPURTT_SmoniParam_t )        /* ioctl command for drvCPURTT_UDF_SmoniApiExecute */
#define DRV_CPURTT_IOCTL_DEVFBISTINIT    _IOR( DRV_CPURTT_IOCTL_MAGIC, DRV_CPURTT_CMD_CODE + 3, drvCPURTT_FbistInitParam_t) /* ioctl command for drvCPURTT_UDF_FbistInitialize */
#define DRV_CPURTT_IOCTL_DEVFBISTDEINIT  _IO( DRV_CPURTT_IOCTL_MAGIC, DRV_CPURTT_CMD_CODE + 4 )                             /* ioctl command for drvCPURTT_UDF_FbistDeInitialize */
#define DRV_CPURTT_IOCTL_WAIT_CALLBACK  _IOWR( DRV_CPURTT_IOCTL_MAGIC, DRV_CPURTT_CMD_CODE + 5 , drvCPURTT_CallbackInfo_t)  /* ioctl command for drvCPURTT_UDF_WaitCallback */

/* Definition of the kernel CPURTT device module name */
#define UDF_CPURTT_MODULE_NAME        "cpurttmod0"    /* cpurtt driver minor number */

/* Definition for callback control information */
#define DRV_CPURTT_CB_REQ_NON           (0x0000000000000000U)
#define DRV_CPURTT_CB_REQ_CALLBACK      (0x0000000000000001U)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* CPURTT_COMMON_H */

