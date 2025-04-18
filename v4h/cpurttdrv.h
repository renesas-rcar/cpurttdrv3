/****************************************************************************/
/*
 * FILE          : cpurttdrv.h
 * DESCRIPTION   : CPU Runtime Test driver for sample code
 * CREATED       : 2021.04.17
 * MODIFIED      : 2025.04.10
 * AUTHOR        : Renesas Electronics Corporation
 * TARGET DEVICE : R-Car V4H
 * TARGET OS     : BareMetal
 * HISTORY       :
 *                 2021.10.19 Modify the definition value used in A2 Runtime Test.
 *                            Move definition values that do not need to be shared with the user layer.
 *                 2021.02.14 Modify for V4H.
 *                 2022.03.04 Remove ITARGETS11 defines.
 *                 2022.04.08 Add enum type rerated to modifing irq affinity setting.
 *                            Modify the setting of SGI issue register.
 *                 2022.06.13 Modify the wakeup method when CPU runtime test.
 *                 2023.08.23 Support A2 runtime test.
 *                 2024.10.10 Support V4H Variants
 *                            Add definition, enum and struct type related to Product ID
 *                 2025.04.10 Support Region ID ON
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

#ifndef CPURTTDRV_H
#define CPURTTDRV_H

#include "cpurtt_common.h"

#define UDF_CPURTT_UIO_DRIVER_NAME    "fbc_uio_share"     /* cpurtt driver name for uio */

#define DRV_CPURTTKER_SMONI_BUF_SIZE 128U
/* Region ID configuration */
#ifdef CONFIG_ENABLE_RGID
/* Region 2 (Region ID ON) */
#define DRV_RTTKER_OTPMEM_RGID 0x2ULL
#define DRV_RTTKER_FBC_RGID    0x2ULL
#define DRV_RTTKER_APMU_RGID   0x2ULL
#else
/* Region 0 (Region ID OFF/Default) */
#define DRV_RTTKER_OTPMEM_RGID 0x0ULL
#define DRV_RTTKER_FBC_RGID    0x0ULL
#define DRV_RTTKER_APMU_RGID   0x0ULL
#endif

/* VARIANT ID REG */
#define DRV_RTTKER_OTPMON17_ADDR  (0xE61BF144ULL | ((DRV_RTTKER_OTPMEM_RGID & 0xFULL) << 36ULL))

/* VARIANT ID */
#define DRV_RTTKER_PRODUCT_V4H_7    0x00000000U 
#define DRV_RTTKER_PRODUCT_V4H_5    0x00000001U
#define DRV_RTTKER_PRODUCT_V4H_3    0x00000002U

#define DRV_RTTKER_PRODUCTID_MASK   0x000000FFU

/* FieldBIST related defined values */
/* RTTFINISH1 REG */
#define DRV_RTTKER_RTTFINISH1   (0xFF830100ULL | ((DRV_RTTKER_FBC_RGID & 0xFULL) << 36ULL))
#define DRV_RTTKER_RTTFINISH2   (0xFF830104ULL | ((DRV_RTTKER_FBC_RGID & 0xFULL) << 36ULL))
#define DRV_RTTKER_RTTFINISH3   (0xFF830108ULL | ((DRV_RTTKER_FBC_RGID & 0xFULL) << 36ULL))

/* RTTEX REG */
#define DRV_RTTKER_ISP0_RTTEX   0xFEAC1000U
#define DRV_RTTKER_ISP1_RTTEX   0xFEAC2000U
#define DRV_RTTKER_IMR0_RTTEX   0xFE601000U
#define DRV_RTTKER_IMR1_RTTEX   0xFE602000U
#define DRV_RTTKER_IMR2_RTTEX   0xFE603000U
#define DRV_RTTKER_IMS0_RTTEX   0xFE605000U
#define DRV_RTTKER_IMS1_RTTEX   0xFE606000U
#define DRV_RTTKER_IMP0_RTTEX   0xFF8C0000U
#define DRV_RTTKER_IMP1_RTTEX   0xFF8C1000U
#define DRV_RTTKER_IMP2_RTTEX   0xFF8C2000U
#define DRV_RTTKER_IMP3_RTTEX   0xFF8C3000U
#define DRV_RTTKER_OCV0_RTTEX   0xFF8C4000U
#define DRV_RTTKER_OCV1_RTTEX   0xFF8C5000U
#define DRV_RTTKER_OCV2_RTTEX   0xFF8C6000U
#define DRV_RTTKER_OCV3_RTTEX   0xFF8C7000U
#define DRV_RTTKER_DP0_RTTEX    0xFFF56000U
#define DRV_RTTKER_DP1_RTTEX    0xFFF57000U
#define DRV_RTTKER_CNN_RTTEX    0xFFF50000U
#define DRV_RTTKER_DSP0_RTTEX   0xFFF64000U
#define DRV_RTTKER_DSP1_RTTEX   0xFFF65000U
#define DRV_RTTKER_DSP2_RTTEX   0xFFF66000U
#define DRV_RTTKER_DSP3_RTTEX   0xFFF67000U
#define DRV_RTTKER_CNRAM_RTTEX  0xFFF55000U
#define DRV_RTTKER_SLIM0_RTTEX  0xFF8F4000U
#define DRV_RTTKER_SLIM1_RTTEX  0xFF8F5000U
#define DRV_RTTKER_CA76D0_RTTEX 0xFF870000U
#define DRV_RTTKER_CA760_RTTEX  0xFF871000U
#define DRV_RTTKER_CA761_RTTEX  0xFF872000U
#define DRV_RTTKER_CA76D1_RTTEX 0xFF873000U
#define DRV_RTTKER_CA762_RTTEX  0xFF874000U
#define DRV_RTTKER_CA763_RTTEX  0xFF875000U
#define DRV_RTTKER_UMFL0_RTTEX  0xE7B61000U
#define DRV_RTTKER_SMPS0_RTTEX  0xE7B64000U
#define DRV_RTTKER_SMPO0_RTTEX  0xE7B63000U
#define DRV_RTTKER_PAP_RTTEX    0xE7B62000U


#define DRV_CPURTTKER_OFFSET_RTTEX 0x000U

#define DRV_RTTKER_RTTEX_BIT_EX 0x00000001U

#define DRV_CPURTTKER_FBIST_FINISH_ISSUED     1U
#define DRV_CPURTTKER_FBIST_FINISH_NOT_ISSUED 0U

typedef enum
{
    DRV_RTTKER_RTTFINISH_GRP1,
    DRV_RTTKER_RTTFINISH_GRP2,
    DRV_RTTKER_RTTFINISH_GRP3,
} drvRTT_rttfinish_t;

#define DRV_RTTKER_RTTFINISH_BIT_WIDTH 32

/* Bit number of RTTFINISH1 register */
#define DRV_RTTKER_IMR0_RTTFINISH_BIT   31U
#define DRV_RTTKER_ISP1_RTTFINISH_BIT   27U
#define DRV_RTTKER_ISP0_RTTFINISH_BIT   26U
#define DRV_RTTKER_CA763_RTTFINISH_BIT  14U
#define DRV_RTTKER_CA762_RTTFINISH_BIT  13U
#define DRV_RTTKER_CA761_RTTFINISH_BIT  12U
#define DRV_RTTKER_CA760_RTTFINISH_BIT  11U
#define DRV_RTTKER_CA76D1_RTTFINISH_BIT 10U
#define DRV_RTTKER_CA76D0_RTTFINISH_BIT  9U
/* Bit number of RTTFINISH2 register */
#define DRV_RTTKER_OCV3_RTTFINISH_BIT   30U
#define DRV_RTTKER_OCV2_RTTFINISH_BIT   29U
#define DRV_RTTKER_OCV1_RTTFINISH_BIT   28U
#define DRV_RTTKER_OCV0_RTTFINISH_BIT   27U
#define DRV_RTTKER_DP1_RTTFINISH_BIT    26U
#define DRV_RTTKER_DP0_RTTFINISH_BIT    25U
#define DRV_RTTKER_IMP3_RTTFINISH_BIT   24U
#define DRV_RTTKER_IMP2_RTTFINISH_BIT   23U
#define DRV_RTTKER_IMP1_RTTFINISH_BIT   22U
#define DRV_RTTKER_IMP0_RTTFINISH_BIT   21U
#define DRV_RTTKER_SMPO0_RTTFINISH_BIT  15U
#define DRV_RTTKER_SMPS0_RTTFINISH_BIT  11U
#define DRV_RTTKER_UMFL0_RTTFINISH_BIT   9U
#define DRV_RTTKER_PAP_RTTFINISH_BIT     7U
#define DRV_RTTKER_IMS1_RTTFINISH_BIT    4U
#define DRV_RTTKER_IMS0_RTTFINISH_BIT    3U
#define DRV_RTTKER_IMR2_RTTFINISH_BIT    1U
#define DRV_RTTKER_IMR1_RTTFINISH_BIT    0U
/* Bit number of RTTFINISH3 register */
#define DRV_RTTKER_SLIM1_RTTFINISH_BIT  22U
#define DRV_RTTKER_SLIM0_RTTFINISH_BIT  21U
#define DRV_RTTKER_CNRAM_RTTFINISH_BIT  20U
#define DRV_RTTKER_DSP3_RTTFINISH_BIT   17U
#define DRV_RTTKER_DSP2_RTTFINISH_BIT   16U
#define DRV_RTTKER_DSP1_RTTFINISH_BIT   15U
#define DRV_RTTKER_DSP0_RTTFINISH_BIT   14U
#define DRV_RTTKER_CNN_RTTFINISH_BIT     3U


/* Type of HIERARCHY */
#define DRV_RTTKER_HIERARCHY_CPU   0U
#define DRV_RTTKER_HIERARCHY_OTHER 1U

#define DRV_CPURTTKER_APMU_CORE_BASE      (0xE6170800ULL | ((DRV_RTTKER_APMU_RGID & 0xFULL) << 36ULL))
#define DRV_CPURTTKER_CLUSTER_OFFSET           0x200U
#define DRV_CPURTTKER_CLUSTER_CORE_OFFSET      0x040U
#define DRV_CPURTTKER_PWRCTRLC_WUP_BIT    0x00000001U

#define DRV_RTTKER_FIELD_BIST_INT_CPU 0x01U
#define DRV_RTTKER_AFFINITY_MASK_BIT 0x01U

/* return code */
#define FBIST_CB_CLOSE_REQ 1

#define CB_QUEUE_STATUS_EMPTY   0x00
#define CB_QUEUE_STATUS_ENA     0x01
#define CB_QUEUE_STATUS_FULL    0x02

/* Data definition value to be set in the argument of R_SMONI_API_RuntimeTestA2Execute other than CPU0 */
#define DRV_RTTKER_A2_PARAM_RTTEX_DATA  0x00000000U

/* Definition of the kernel CPURTT device module name */
#define UDF_CPURTT_DRIVER_NAME        "cpurttdrv"     /* cpurtt driver name */
#define UDF_CPURTT_CLASS_NAME         "cpurttmod"     /* cpurtt driver class name */

typedef enum
{
    DRV_RTTKER_HIERARCHY_ISP0,  /* Video IO Hierarchy (ISP0) */
    DRV_RTTKER_HIERARCHY_ISP1,  /* Video IO Hierarchy (ISP1) */
    DRV_RTTKER_HIERARCHY_IMR0,  /* Video Codec Hierarchy (IMR2(IMR0)) */
    DRV_RTTKER_HIERARCHY_IMR1,  /* Video Codec Hierarchy (IMR3(IMR1)) */
    DRV_RTTKER_HIERARCHY_IMR2,  /* Video Codec Hierarchy (IMR4(IMR2)) */
    DRV_RTTKER_HIERARCHY_IMS0,  /* Video Codec Hierarchy (IMR0(IMS0)) */
    DRV_RTTKER_HIERARCHY_IMS1,  /* Video Codec Hierarchy (IMR1(IMS1)) */
    DRV_RTTKER_HIERARCHY_IMP0,  /* Image Recognition Hierarchy (IMP core0) */
    DRV_RTTKER_HIERARCHY_IMP1,  /* Image Recognition Hierarchy (IMP core1) */
    DRV_RTTKER_HIERARCHY_IMP2,  /* Image Recognition Hierarchy (IMP core2) */
    DRV_RTTKER_HIERARCHY_IMP3,  /* Image Recognition Hierarchy (IMP core3) */
    DRV_RTTKER_HIERARCHY_OCV0,  /* Image Recognition Hierarchy (OCV core0) */
    DRV_RTTKER_HIERARCHY_OCV1,  /* Image Recognition Hierarchy (OCV core1) */
    DRV_RTTKER_HIERARCHY_OCV2,  /* Image Recognition Hierarchy (OCV core2) */
    DRV_RTTKER_HIERARCHY_OCV3,  /* Image Recognition Hierarchy (OCV core3) */
    DRV_RTTKER_HIERARCHY_DP0,   /* Image Recognition Hierarchy (IMP DMAC0, IMP PSC0) */
    DRV_RTTKER_HIERARCHY_DP1,   /* Image Recognition Hierarchy (IMP DMAC1, IMP DMAC2) */
    DRV_RTTKER_HIERARCHY_CNN,   /* Image Recognition Hierarchy (IMP CNN) */
    DRV_RTTKER_HIERARCHY_DSP0,  /* Image Recognition Hierarchy (DSP0) */
    DRV_RTTKER_HIERARCHY_DSP1,  /* Image Recognition Hierarchy (DSP1) */
    DRV_RTTKER_HIERARCHY_DSP2,  /* Image Recognition Hierarchy (DSP2) */
    DRV_RTTKER_HIERARCHY_DSP3,  /* Image Recognition Hierarchy (DSP3) */
    DRV_RTTKER_HIERARCHY_CNRAM, /* Image Recognition Hierarchy (CNRAM) */
    DRV_RTTKER_HIERARCHY_SLIM0, /* Image Recognition Hierarchy (slimDMAC0) */
    DRV_RTTKER_HIERARCHY_SLIM1, /* Image Recognition Hierarchy (slimDMAC1) */
    DRV_RTTKER_HIERARCHY_CA76D0,  /* Cortex-A76 CPU Cluster 0 Hierarchy (A2 domain) */
    DRV_RTTKER_HIERARCHY_CA760,   /* Cortex-A76 CPU Core 0 Hierarchy (A1 domain) */
    DRV_RTTKER_HIERARCHY_CA761,   /* Cortex-A76 CPU Core 1 Hierarchy (A1 domain) */
    DRV_RTTKER_HIERARCHY_CA76D1,  /* Cortex-A76 CPU Cluster 1 Hierarchy (A2 domain) */
    DRV_RTTKER_HIERARCHY_CA762,   /* Cortex-A76 CPU Core 2 Hierarchy (A1 domain) */
    DRV_RTTKER_HIERARCHY_CA763,   /* Cortex-A76 CPU Core 3 Hierarchy (A1 domain) */
    DRV_RTTKER_HIERARCHY_UMFL0, /* Vision IP Hierarchy (Optical Flow 0) */
    DRV_RTTKER_HIERARCHY_SMPS0, /* Vision IP Hierarchy (SMPS0) */
    DRV_RTTKER_HIERARCHY_SMPO0, /* Vision IP Hierarchy (SMPO0) */
    DRV_RTTKER_HIERARCHY_PAP,   /* Vision IP Hierarchy (PAP) */
    DRV_RTTKER_HIERARCHY_MAX
} drvRTT_hierarchy_t;

typedef enum
{
    DRV_CPURTTKER_CPUNUM_CPU0 = 0,
    DRV_CPURTTKER_CPUNUM_CPU1,
    DRV_CPURTTKER_CPUNUM_CPU2,
    DRV_CPURTTKER_CPUNUM_CPU3,
    DRV_CPURTTKER_CPUNUM_MAX
} drvRTT_cpunum_t;

#define DRV_CPURTTKER_CPUNUM_INVALID  DRV_CPURTTKER_CPUNUM_MAX

typedef enum
{
    DRV_CPURTTKER_CLUSTERNUM_0 = 0,
    DRV_CPURTTKER_CLUSTERNUM_1,
    DRV_CPURTTKER_CLUSTERNUM_MAX
} drvRTT_clusternum_t;

typedef enum
{
    DRV_CPURTTKER_CLUSTERNUM_CPU0 = 0,
    DRV_CPURTTKER_CLUSTERNUM_CPU1,
    DRV_CPURTTKER_CLUSTERNUM_CPUMAX
} drvRTT_clustercpunum_t;

typedef enum
{
    DRV_CPURTTKER_TESTTYPE_A1 = 0,
    DRV_CPURTTKER_TESTTYPE_A2,
    DRV_CPURTTKER_TESTTYPE_MAX
} drvRTT_testtype_t;

/* CPU CORE STATE */
typedef enum
{
    DRV_RTTKER_CPU_CORE_ENABLE = 0,
    DRV_RTTKER_CPU_CORE_DISABLE
} drvRTT_CpuCoreState_t;

typedef struct
{
    uint32_t mHierarchyType;
    drvRTT_hierarchy_t mHierarchy;
    uint32_t mRttfinishBit;
    uint32_t mRttexAdr;
    drvRTT_rttfinish_t mRttfinishGrp;
} drvCPURTT_FbistHdr_t;

typedef struct
{
    uint8_t head;
    uint8_t pos;
    uint8_t status;
    drvCPURTT_CallbackInfo_t CbInfo[DRV_RTTKER_HIERARCHY_MAX];
}drvCPURTT_CbInfoQueue_t;

typedef struct
{
    uint16_t mClusterNum;
    uint16_t mCpuNum;
} drvCPURTT_A2ThreadArg_t;

typedef int (*A2ThreadTable)(void*);

struct fbc_uio_share_soc_res {
	char **clk_names;
};

/* */
struct fbc_uio_share_platform_data {
	struct platform_device *pdev;
	struct uio_info *uio_info;
	void __iomem *base_addr[1];
	struct clk *clks[3];
	int clk_count;
    spinlock_t lock;
    unsigned long flags;
};

enum {
    UIO_IRQ_DISABLED = 0,
};

#endif
