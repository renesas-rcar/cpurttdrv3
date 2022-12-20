/****************************************************************************/
/*
 * FILE          : cpurttdrv.c
 * DESCRIPTION   : CPU Runtime Test driver for sample code
 * CREATED       : 2021.04.20
 * MODIFIED      : 2022.12.14
 * AUTHOR        : Renesas Electronics Corporation
 * TARGET DEVICE : R-Car V4H
 * TARGET OS     : BareMetal
 * HISTORY       : 
 *                 2021.05.31 Modify so that ioctl arguments can be held correctly when multiple ioctls are executed.
 *                 2021.07.15 Modify prefix of smoni api.
 *                 2021.10.19 Modify the storage method of fail information in FbistInterruptHandler.
 *                            Modify the execution method of A2 Runtime Test.
 *                 2021.11.22 Update software version.
 *                 2022.01.24 Add ioctl command for HWA Runtime Test.
 *                 2022.02.14 Modify for V4H.
 *                 2022.03.04 Modify RTTEX address. Remove ITARGETS11 check.
 *                 2022.04.08 Modify irq affinity setting at A1/A2 Runtime Test.
 *                            Remove RTTEX data check at FbistInterruptHandler and HWA Execute.
 *                            Modify the execution method of A2 Runtime Test.
 *                            Modify the setting of SGI issue register.
 *                 2022.05.13 Modify fbist interrupt proccess to support SMPS0, SMPO0 and PAP.
 *                 2022.06.13 Modify the wakeup method when CPU runtime test.
 *                 2023.08.23 Support A2 runtime test.
 *                            Remove processing related to ConfigRegCheck and HWA Execute.
 *                 2022.12.14 Add smoni UDF.
 */
/****************************************************************************/
/*
 * Copyright(C) 2021-2022 Renesas Electronics Corporation. All Rights Reserved.
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation.
 * No part of this program may be reproduced or disclosed to
 * others, in any form, without the prior written permission
 * of Renesas Electronics Corporation.
 *
 ****************************************************************************/
/***********************************************************
 Includes <System Includes>, "Project Includes"
 ***********************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <asm/irqflags.h>
#include <linux/completion.h>
#include <linux/cpumask.h>
#include <linux/cpuset.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/semaphore.h>
#include <linux/uio_driver.h>
#include <linux/io.h>
#include <linux/of_irq.h>
#include <linux/pm_runtime.h>
#include <linux/clk.h>

#include "cpurttdrv.h"
#include "cpurtt_common.h"
#include "smoni_def.h"
#include "smoni_api_v2.h"

#undef IS_INTERRUPT

#define DRIVER_VERSION "0.6.0"

/***********************************************************
 Macro definitions
 ***********************************************************/

/***********************************************************
 Typedef definitions
 ***********************************************************/

/***********************************************************
 Exported global variables (to be accessed by other files)
 ***********************************************************/

/***********************************************************
 Private global variables and functions
 ***********************************************************/

/* Variables used for device registration of cpurttmod */
static unsigned int cpurtt_major = 0;        /*  0:auto */
static struct class *cpurtt_class = NULL;

/* Variables used unique to cpurttmod */
static uint32_t g_SmoniAddrBuf[DRV_CPURTTKER_CPUNUM_MAX][DRV_CPURTTKER_SMONI_BUF_SIZE]; /* address buffer for parameter of R_SMONI_API_RuntimeTestFbaWrite/R_SMONI_API_RuntimeTestFbaRead */
static uint32_t g_SmoniDataBuf[DRV_CPURTTKER_CPUNUM_MAX][DRV_CPURTTKER_SMONI_BUF_SIZE]; /* data buffer for parameter of R_SMONI_API_RuntimeTestFbaWrite/R_SMONI_API_RuntimeTestFbaRead */
static struct completion g_A2StartSynCompletion[DRV_CPURTTKER_CLUSTERNUM_MAX];
static struct completion g_A2EndSynCompletion[DRV_CPURTTKER_CLUSTERNUM_MAX][DRV_CPURTTKER_CLUSTERNUM_CPUMAX];
static struct completion g_A2ThWakeupCompletion[DRV_CPURTTKER_CLUSTERNUM_MAX][DRV_CPURTTKER_CLUSTERNUM_CPUMAX];

static uint16_t g_A2SmoniResult[DRV_CPURTTKER_CLUSTERNUM_MAX][DRV_CPURTTKER_CLUSTERNUM_CPUMAX];
static drvCPURTT_A2rttParam_t g_A2Param[DRV_CPURTTKER_CLUSTERNUM_MAX][DRV_CPURTTKER_CLUSTERNUM_CPUMAX];

static struct task_struct *g_A2Task[DRV_CPURTTKER_CLUSTERNUM_MAX][DRV_CPURTTKER_CLUSTERNUM_CPUMAX];
static unsigned int g_TaskExitFlg = 0;

static void __iomem *g_RegBaseRttfinish1 = NULL;
static void __iomem *g_RegBaseRttfinish2 = NULL;
static void __iomem *g_RegBaseRttfinish3 = NULL;
static void __iomem *g_RegBasePwrctrlc[DRV_CPURTTKER_CPUNUM_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL
};

static void __iomem *g_RegBaseAddrTable[DRV_RTTKER_HIERARCHY_MAX] = {
    NULL, /* Video IO Hierarchy (ISP0) */
    NULL, /* Video IO Hierarchy (ISP1) */
    NULL, /* Video Codec Hierarchy (IMR2(IMR0)) */
    NULL, /* Video Codec Hierarchy (IMR3(IMR1)) */
    NULL, /* Video Codec Hierarchy (IMR4(IMR2)) */
    NULL, /* Video Codec Hierarchy (IMR0(IMS0)) */
    NULL, /* Video Codec Hierarchy (IMR1(IMS1)) */
    NULL, /* Image Recognition Hierarchy (IMP core0) */
    NULL, /* Image Recognition Hierarchy (IMP core1) */
    NULL, /* Image Recognition Hierarchy (IMP core2) */
    NULL, /* Image Recognition Hierarchy (IMP core3) */
    NULL, /* Image Recognition Hierarchy (OCV core0) */
    NULL, /* Image Recognition Hierarchy (OCV core1) */
    NULL, /* Image Recognition Hierarchy (OCV core2) */
    NULL, /* Image Recognition Hierarchy (OCV core3) */
    NULL, /* Image Recognition Hierarchy (IMP DMAC0, IMP PSC0) */
    NULL, /* Image Recognition Hierarchy (IMP DMAC1, IMP DMAC2) */
    NULL, /* Image Recognition Hierarchy (IMP CNN) */
    NULL, /* Image Recognition Hierarchy (DSP0) */
    NULL, /* Image Recognition Hierarchy (DSP1) */
    NULL, /* Image Recognition Hierarchy (DSP2) */
    NULL, /* Image Recognition Hierarchy (DSP3) */
    NULL, /* Image Recognition Hierarchy (CNRAM) */
    NULL, /* Image Recognition Hierarchy (slimDMAC0) */
    NULL, /* Image Recognition Hierarchy (slimDMAC1) */
    NULL, /* Cortex-A76 CPU Cluster 0 Hierarchy (A2 domain) */
    NULL, /* Cortex-A76 CPU Core 0 Hierarchy (A1 domain) */
    NULL, /* Cortex-A76 CPU Core 1 Hierarchy (A1 domain) */
    NULL, /* Cortex-A76 CPU Cluster 1 Hierarchy (A2 domain) */
    NULL, /* Cortex-A76 CPU Core 2 Hierarchy (A1 domain) */
    NULL, /* Cortex-A76 CPU Core 3 Hierarchy (A1 domain) */
    NULL, /* Vision IP Hierarchy (Optical Flow 0) */
    NULL, /* Vision IP Hierarchy (SMPS0) */
    NULL, /* Vision IP Hierarchy (SMPO0) */
    NULL, /* Vision IP Hierarchy (PAP) */
};

static const phys_addr_t drvRTT_PhyRegAddr[DRV_RTTKER_HIERARCHY_MAX] = {
    DRV_RTTKER_ISP0_RTTEX,
    DRV_RTTKER_ISP1_RTTEX,
    DRV_RTTKER_IMR0_RTTEX,
    DRV_RTTKER_IMR1_RTTEX,
    DRV_RTTKER_IMR2_RTTEX,
    DRV_RTTKER_IMS0_RTTEX,
    DRV_RTTKER_IMS1_RTTEX,
    DRV_RTTKER_IMP0_RTTEX,
    DRV_RTTKER_IMP1_RTTEX,
    DRV_RTTKER_IMP2_RTTEX,
    DRV_RTTKER_IMP3_RTTEX,
    DRV_RTTKER_OCV0_RTTEX,
    DRV_RTTKER_OCV1_RTTEX,
    DRV_RTTKER_OCV2_RTTEX,
    DRV_RTTKER_OCV3_RTTEX,
    DRV_RTTKER_DP0_RTTEX,
    DRV_RTTKER_DP1_RTTEX,
    DRV_RTTKER_CNN_RTTEX,
    DRV_RTTKER_DSP0_RTTEX,
    DRV_RTTKER_DSP1_RTTEX,
    DRV_RTTKER_DSP2_RTTEX,
    DRV_RTTKER_DSP3_RTTEX,
    DRV_RTTKER_CNRAM_RTTEX,
    DRV_RTTKER_SLIM0_RTTEX,
    DRV_RTTKER_SLIM1_RTTEX,
    DRV_RTTKER_CA76D0_RTTEX,
    DRV_RTTKER_CA760_RTTEX,
    DRV_RTTKER_CA761_RTTEX,
    DRV_RTTKER_CA76D1_RTTEX,
    DRV_RTTKER_CA762_RTTEX,
    DRV_RTTKER_CA763_RTTEX,
    DRV_RTTKER_UMFL0_RTTEX,
    DRV_RTTKER_SMPS0_RTTEX,
    DRV_RTTKER_SMPO0_RTTEX,
    DRV_RTTKER_PAP_RTTEX,
};

static const uint64_t drvCPURTTKER_WakeupReg[DRV_CPURTTKER_CPUNUM_MAX + DRV_CPURTTKER_CLUSTERNUM_MAX] = {
    DRV_CPURTTKER_CPUNUM_INVALID,
    DRV_CPURTTKER_CPUNUM_CPU0,
    DRV_CPURTTKER_CPUNUM_CPU1,
    DRV_CPURTTKER_CPUNUM_INVALID,
    DRV_CPURTTKER_CPUNUM_CPU2,
    DRV_CPURTTKER_CPUNUM_CPU3
};

static const uint32_t drvCPURTTKER_InterruptCpuA1[DRV_CPURTTKER_CPUNUM_MAX] = {
    DRV_CPURTTKER_CPUNUM_CPU1,      /* target interrupt cpu at CA760 */
    DRV_CPURTTKER_CPUNUM_CPU0,      /* target interrupt cpu at CA761 */
    DRV_CPURTTKER_CPUNUM_CPU3,      /* target interrupt cpu at CA762 */
    DRV_CPURTTKER_CPUNUM_CPU2       /* target interrupt cpu at CA763 */
};
static const uint32_t drvCPURTTKER_InterruptCpuA2[DRV_CPURTTKER_CLUSTERNUM_MAX] = {
    DRV_CPURTTKER_CPUNUM_CPU0,      /* target interrupt cpu at CA76D0 */
    DRV_CPURTTKER_CPUNUM_CPU2      /* target interrupt cpu at CA76D1 */
};

static drvCPURTT_CbInfoQueue_t g_CallbackInfo;
static struct semaphore CallbackSem;
static bool FbistCloseReq;

static struct platform_device *g_cpurtt_pdev = NULL;

static int CpurttDrv_open(struct inode *inode, struct file *file);
static int CpurttDrv_close(struct inode *inode, struct file *file);
static long CpurttDrv_ioctl(struct file* filp, unsigned int cmd, unsigned long arg );
static int CpurttDrv_init(void);
static void CpurttDrv_exit(void);

int drvCPURTT_UDF_A2RuntimeThreadN(void *aArg);

static long drvCPURTT_UDF_RuntimeTestInit(void);
static long drvCPURTT_UDF_RuntimeTestDeinit(void);
static long drvCPURTT_UDF_SmoniApiExe(drvCPURTT_SmoniTable_t index, uint32_t aCpuId, uint32_t *aArg, uint32_t *aSmoniret);

static long drvCPURTT_UDF_FbistInit(void);
static long drvCPURTT_UDF_FbistDeInit(void);
static long drvCPURTT_UDF_WaitCbNotice(drvCPURTT_CallbackInfo_t *aParam);

static int fbc_uio_share_clk_enable(struct fbc_uio_share_platform_data *pdata)
{
    int ret;
    int i;

    for (i = 0; i < pdata->clk_count; i++) {
        ret = clk_prepare_enable(pdata->clks[i]);
        if (ret) {
            dev_err(&pdata->pdev->dev, "Clock enable failed\n");
            break;
        }
    }

    if (ret)
    {
        for (i--; i >= 0; i--)
            clk_disable_unprepare(pdata->clks[i]);
    }

    return ret;
}

static void fbc_uio_share_clk_disable(struct fbc_uio_share_platform_data *pdata)
{
    int i;

    for (i = 0; i < pdata->clk_count; i++)
        clk_disable_unprepare(pdata->clks[i]);
}

static int drvCPURTT_FbistHierarchyCheckFbistFinish(const drvCPURTT_FbistHdr_t *fbisthdr, uint32_t *checkResult)
{
    int ret;
    uint32_t RegValRttfinish;
    if ((NULL != fbisthdr) && (NULL != checkResult))
    {
        ret = 0;
        switch(fbisthdr->mRttfinishGrp)
        {
            case DRV_RTTKER_RTTFINISH_GRP1:
                RegValRttfinish = readl(g_RegBaseRttfinish1);
                break;
            case DRV_RTTKER_RTTFINISH_GRP2:
                RegValRttfinish = readl(g_RegBaseRttfinish2);
                break;
            case DRV_RTTKER_RTTFINISH_GRP3:
                RegValRttfinish = readl(g_RegBaseRttfinish3);
                break;
            default:
                ret = -EINVAL;
                break;
        }

        if (0 == ret)
        {
            /* fieldBIST Completion Check for interrupt factors */
            if (0x1U == (((RegValRttfinish >> fbisthdr->mRttfinishBit) & 0x1U)))
            {
                *checkResult = DRV_CPURTTKER_FBIST_FINISH_ISSUED;
            }
            else
            {
                *checkResult = DRV_CPURTTKER_FBIST_FINISH_NOT_ISSUED;
            }
        }
    }
    else
    {
        ret = -EINVAL;
    }
    return ret;
}

static void FbistInterruptHandler(int irq, struct uio_info *uio_info)
{
    drvRTT_hierarchy_t i;
    uint32_t WriteRegVal;
    uint32_t HierarchyBit;
    uint32_t Address;
    uint32_t Smoniret;
    uint64_t FbistCbRes = 0;
    uint64_t RfsoOutputRes = 0;
    uint32_t ReadRegVal;
    int ret;
    uint32_t FbistFinish;
    uint32_t ReadData;
    uint32_t CpuIndex;

    static const drvCPURTT_FbistHdr_t FbisthdrInfo[DRV_RTTKER_HIERARCHY_MAX] = {
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_ISP0,	DRV_RTTKER_ISP0_RTTFINISH_BIT,	DRV_RTTKER_ISP0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_ISP1,	DRV_RTTKER_ISP1_RTTFINISH_BIT,	DRV_RTTKER_ISP1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMR0,	DRV_RTTKER_IMR0_RTTFINISH_BIT,	DRV_RTTKER_IMR0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMR1,	DRV_RTTKER_IMR1_RTTFINISH_BIT,	DRV_RTTKER_IMR1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMR2,	DRV_RTTKER_IMR2_RTTFINISH_BIT,	DRV_RTTKER_IMR2_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMS0,	DRV_RTTKER_IMS0_RTTFINISH_BIT,	DRV_RTTKER_IMS0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMS1,	DRV_RTTKER_IMS1_RTTFINISH_BIT,	DRV_RTTKER_IMS1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMP0,	DRV_RTTKER_IMP0_RTTFINISH_BIT,	DRV_RTTKER_IMP0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMP1,	DRV_RTTKER_IMP1_RTTFINISH_BIT,	DRV_RTTKER_IMP1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMP2,	DRV_RTTKER_IMP2_RTTFINISH_BIT,	DRV_RTTKER_IMP2_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_IMP3,	DRV_RTTKER_IMP3_RTTFINISH_BIT,	DRV_RTTKER_IMP3_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_OCV0,	DRV_RTTKER_OCV0_RTTFINISH_BIT,	DRV_RTTKER_OCV0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_OCV1,	DRV_RTTKER_OCV1_RTTFINISH_BIT,	DRV_RTTKER_OCV1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_OCV2,	DRV_RTTKER_OCV2_RTTFINISH_BIT,	DRV_RTTKER_OCV2_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_OCV3,	DRV_RTTKER_OCV3_RTTFINISH_BIT,	DRV_RTTKER_OCV3_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_DP0,	DRV_RTTKER_DP0_RTTFINISH_BIT,	DRV_RTTKER_DP0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_DP1,	DRV_RTTKER_DP1_RTTFINISH_BIT,	DRV_RTTKER_DP1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_CNN,	DRV_RTTKER_CNN_RTTFINISH_BIT,	DRV_RTTKER_CNN_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_DSP0,	DRV_RTTKER_DSP0_RTTFINISH_BIT,	DRV_RTTKER_DSP0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_DSP1,	DRV_RTTKER_DSP1_RTTFINISH_BIT,	DRV_RTTKER_DSP1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_DSP2,	DRV_RTTKER_DSP2_RTTFINISH_BIT,	DRV_RTTKER_DSP2_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_DSP3,	DRV_RTTKER_DSP3_RTTFINISH_BIT,	DRV_RTTKER_DSP3_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_CNRAM,	DRV_RTTKER_CNRAM_RTTFINISH_BIT,	DRV_RTTKER_CNRAM_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_SLIM0,	DRV_RTTKER_SLIM0_RTTFINISH_BIT,	DRV_RTTKER_SLIM0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_SLIM1,	DRV_RTTKER_SLIM1_RTTFINISH_BIT,	DRV_RTTKER_SLIM1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP3},
        {DRV_RTTKER_HIERARCHY_CPU,	DRV_RTTKER_HIERARCHY_CA76D0,	DRV_RTTKER_CA76D0_RTTFINISH_BIT,	DRV_RTTKER_CA76D0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_CPU,	DRV_RTTKER_HIERARCHY_CA760,	DRV_RTTKER_CA760_RTTFINISH_BIT,	DRV_RTTKER_CA760_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_CPU,	DRV_RTTKER_HIERARCHY_CA761,	DRV_RTTKER_CA761_RTTFINISH_BIT,	DRV_RTTKER_CA761_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_CPU,	DRV_RTTKER_HIERARCHY_CA76D1,	DRV_RTTKER_CA76D1_RTTFINISH_BIT,	DRV_RTTKER_CA76D1_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_CPU,	DRV_RTTKER_HIERARCHY_CA762,	DRV_RTTKER_CA762_RTTFINISH_BIT,	DRV_RTTKER_CA762_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_CPU,	DRV_RTTKER_HIERARCHY_CA763,	DRV_RTTKER_CA763_RTTFINISH_BIT,	DRV_RTTKER_CA763_RTTEX,	DRV_RTTKER_RTTFINISH_GRP1},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_UMFL0,	DRV_RTTKER_UMFL0_RTTFINISH_BIT,	DRV_RTTKER_UMFL0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_SMPS0,	DRV_RTTKER_SMPS0_RTTFINISH_BIT,	DRV_RTTKER_SMPS0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_SMPO0,	DRV_RTTKER_SMPO0_RTTFINISH_BIT,	DRV_RTTKER_SMPO0_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},
        {DRV_RTTKER_HIERARCHY_OTHER,	DRV_RTTKER_HIERARCHY_PAP,	DRV_RTTKER_PAP_RTTFINISH_BIT,	DRV_RTTKER_PAP_RTTEX,	DRV_RTTKER_RTTFINISH_GRP2},

    };

    HierarchyBit = 0;
    Address = 0;
    Smoniret = 0;
    ReadRegVal = 0U;

    for (i=0; i<DRV_RTTKER_HIERARCHY_MAX; i++)
    {
        
        ret = drvCPURTT_FbistHierarchyCheckFbistFinish(&FbisthdrInfo[i], &FbistFinish);
        if (0 == ret)
        {
            if (DRV_CPURTTKER_FBIST_FINISH_ISSUED == FbistFinish)
            {
                WriteRegVal = (uint32_t) 0x0U;
                if (DRV_RTTKER_HIERARCHY_CPU == FbisthdrInfo[i].mHierarchyType)
                {
                    /* If the hierarchy is CPU, clear the RTTEX register via the Smoni API */
                    HierarchyBit = ((uint32_t)((FbisthdrInfo[i].mRttfinishBit + (FbisthdrInfo[i].mRttfinishGrp * DRV_RTTKER_RTTFINISH_BIT_WIDTH)) << (uint32_t)16U));
                    Address = (HierarchyBit | DRV_CPURTTKER_OFFSET_RTTEX);

                    Smoniret = R_SMONI_API_RuntimeTestFbaWrite(&Address, (uint32_t *)&WriteRegVal, 1U);
                    if (SMONI_FUSA_OK == Smoniret)
                    {
                        /* Set the execution request information of the callback function to the user layer */
                        FbistCbRes |= (uint64_t)1U << FbisthdrInfo[i].mHierarchy;
                        /* The process write WUP_REQ bit of PWRCTRLx register to 1 in order to wake up the core in A1 Runtime Test. */
                        CpuIndex = drvCPURTTKER_WakeupReg[FbisthdrInfo[i].mHierarchy - DRV_RTTKER_HIERARCHY_CA76D0];
                        if ((uint32_t)DRV_CPURTTKER_CPUNUM_INVALID > CpuIndex)
                        {
                           /* The process write WUP_REQ bit of PWRCTRLx register to 1 in order to wake up the core in A1 Runtime Test. */
                            ReadData = readl(g_RegBasePwrctrlc[CpuIndex]);
                            writel((ReadData | DRV_CPURTTKER_PWRCTRLC_WUP_BIT), g_RegBasePwrctrlc[CpuIndex]);
                            asm volatile("dsb sy" );
                        }
                    }
                    else
                    {
                       /* If an exclusize error occurs in the Smoni API, set the output terminal output request information to the user layer  */
                        RfsoOutputRes |= (uint64_t)1U << FbisthdrInfo[i].mHierarchy;
                    }
                }
                else
                {

                     ReadRegVal = readl(g_RegBaseAddrTable[FbisthdrInfo[i].mHierarchy]);
                     if (DRV_RTTKER_RTTEX_BIT_EX == (ReadRegVal & DRV_RTTKER_RTTEX_BIT_EX))
                     {
                         /* clear RTTEX register*/
                         writel(0U, g_RegBaseAddrTable[FbisthdrInfo[i].mHierarchy]);

                         /* Set the execution request information of the callback function to the user layer */
                         FbistCbRes |= (uint64_t)1U << FbisthdrInfo[i].mHierarchy;
                    }
                    else
                    {
                       /* If an exclusize error occurs in the Smoni API, set the output terminal output request information to the user layer  */
                        RfsoOutputRes |= (uint64_t)1U << FbisthdrInfo[i].mHierarchy;
                    }
                }
            }
        }
    }

    if (g_CallbackInfo.pos < DRV_RTTKER_HIERARCHY_MAX)
    {
        /* Callback request execution information is stored in the queue buffer  */
        g_CallbackInfo.CbInfo[(g_CallbackInfo.head + g_CallbackInfo.pos) % DRV_RTTKER_HIERARCHY_MAX ].FbistCbRequest = FbistCbRes;
        g_CallbackInfo.CbInfo[(g_CallbackInfo.head + g_CallbackInfo.pos) % DRV_RTTKER_HIERARCHY_MAX ].RfsoOutputPinRequest = RfsoOutputRes;
        g_CallbackInfo.pos++;
        g_CallbackInfo.status = CB_QUEUE_STATUS_ENA;
    }
    else
    {
        /* If the queue buffer is full Set the status to full  */
        g_CallbackInfo.status = CB_QUEUE_STATUS_FULL;
    }

    /* release semafore for execute callback to the user layer */
    up(&CallbackSem);
}

static irqreturn_t fbc_uio_share_irq_handler(int irq, struct uio_info *uio_info)
{
    FbistInterruptHandler(irq, uio_info);

    return IRQ_HANDLED;
}

static const struct fbc_uio_share_soc_res fbc_uio_share_soc_res = {
    .clk_names = (char*[]){
        "fbc_post",
        "fbc_post2",
        "fbc_post4",
        NULL
    },
};

static const struct of_device_id fbc_uio_share_of_table[] = {
    {
        .compatible = "renesas,uio-fbc" , .data = &fbc_uio_share_soc_res,
    }, {
    },
};
MODULE_DEVICE_TABLE(of, fbc_uio_share_of_table);

static int fbc_uio_share_init_clocks(struct platform_device *pdev, char **clk_names, struct fbc_uio_share_platform_data *pdata)
{
    int i;

    if (!clk_names)
        return 0;

    for (i = 0; clk_names[i]; i++) {
        pdata->clks[i] = devm_clk_get(&pdev->dev, clk_names[i]);
        if (IS_ERR(pdata->clks[i])) {
            int ret = PTR_ERR(pdata->clks[i]);

            if (ret != -EPROBE_DEFER)
                dev_err(&pdev->dev, "Failed to get %s clock\n", clk_names[i]);
            return ret;
        }
    }

    pdata->clk_count = i;

    return i;
}

static int fbc_uio_share_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct uio_info *uio_info;
    struct uio_mem *uio_mem;
    struct resource *res;
    struct fbc_uio_share_platform_data *priv;
    const struct fbc_uio_share_soc_res *soc_res;
    int ret;

    /* check device id */
    soc_res = of_device_get_match_data(dev);
    if (!soc_res)
    {
        dev_err(dev, "failed to not match soc resource\n");
        return -EINVAL;
    }

    priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
    {
        dev_err(dev, "failed to alloc fbc_uio_share_platform_data memory\n");
        return -ENOMEM;
    }

    /* Construct the uio_info structure */
    uio_info = devm_kzalloc(dev, sizeof(*uio_info), GFP_KERNEL);
    if (!uio_info)
    {
        dev_err(dev, "failed to alloc uio_info memory\n");
        return -ENOMEM;
    }

    uio_info->version = DRIVER_VERSION;
    uio_info->name = pdev->name;

    /* Set up memory resource */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res)
    {
        dev_err(dev, "failed to get fbc_uio_share memory\n");
        return -EINVAL;
    }

    uio_mem = &uio_info->mem[0];
    uio_mem->memtype = UIO_MEM_PHYS;
    uio_mem->addr = res->start;
    uio_mem->size = resource_size(res);
    uio_mem->name = res->name;

    ret = fbc_uio_share_init_clocks(pdev, soc_res->clk_names, priv);
    if (ret<0)
        return ret;

    spin_lock_init(&priv->lock);
    priv->flags = 0; /* interrupt is enabled to begin with */

    /* Set up interrupt */
    uio_info->irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
    if (uio_info->irq == -ENXIO) {
        dev_err(dev, "failed to parse fbc_uio_share interrupt\n");
        return -EINVAL;
    }
    uio_info->handler = fbc_uio_share_irq_handler;

    uio_info->priv = priv;

    /* Register the UIO device */
    if ((ret = uio_register_device(dev, uio_info))) {
            dev_err(dev, "failed to register UIO device for fbc_uio_share\n");
            return ret;
        }

    /* Register this pdev and uio_info with the platform data */
    priv->pdev = pdev;
    priv->uio_info = uio_info;
    platform_set_drvdata(pdev, priv);

    g_cpurtt_pdev = pdev;

    /* Register the UIO device with the PM framework */
    pm_runtime_enable(dev);

    /* Print some information */
    dev_info(dev, "probed fbc_uio_share\n");

    return 0;
}

static int fbc_uio_share_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct fbc_uio_share_platform_data *priv = platform_get_drvdata(pdev);
    struct uio_info *uio_info = priv->uio_info;

    pm_runtime_disable(&priv->pdev->dev);

    spin_lock(&priv->lock);
    if (!__test_and_set_bit(UIO_IRQ_DISABLED, &priv->flags))
        disable_irq((unsigned int)uio_info->irq);
    spin_unlock(&priv->lock);

    uio_unregister_device(priv->uio_info);

    platform_set_drvdata(pdev, NULL);

    dev_info(dev, "removed fbc_uio_share\n");

    return 0;
}

static struct platform_driver fbc_uio_share_driver = {
    .probe         = fbc_uio_share_probe,
    .remove        = fbc_uio_share_remove,
    .driver        = {
        .name = UDF_CPURTT_UIO_DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(fbc_uio_share_of_table),
    }
};

static long drvCPURTT_InitRegAddr(void)
{
    int i;
    int j;
    struct resource *Resource;
    unsigned long PwrctrlcBaseAddress;
    int CpuIndex;

    if (NULL == g_RegBaseRttfinish1)
    {
        Resource = request_mem_region(DRV_RTTKER_RTTFINISH1, 4U, UDF_CPURTT_DRIVER_NAME);
        if (NULL == Resource)
        {
            pr_err("failed to get RTTFINISH1 resource\n");
            return -ENOSPC;
        }

        g_RegBaseRttfinish1 = ioremap(DRV_RTTKER_RTTFINISH1, 4U);
        if(NULL == g_RegBaseRttfinish1)
        {
            pr_err("failed to get RTTFINISH1 address \n");
            return -EFAULT;
        }
    }

    if (NULL == g_RegBaseRttfinish2)
    {
        Resource = request_mem_region(DRV_RTTKER_RTTFINISH2, 4U, UDF_CPURTT_DRIVER_NAME);
        if (NULL == Resource)
        {
            pr_err("failed to get RTTFINISH2 resource\n");
            return -ENOSPC;
        }

        g_RegBaseRttfinish2 = ioremap(DRV_RTTKER_RTTFINISH2, 4U);
        if(NULL == g_RegBaseRttfinish2)
        {
            pr_err("failed to get RTTFINISH2 address \n");
            return -EFAULT;
        }
    }

    if (NULL == g_RegBaseRttfinish3)
    {
        Resource = request_mem_region(DRV_RTTKER_RTTFINISH3, 4U, UDF_CPURTT_DRIVER_NAME);
        if (NULL == Resource)
        {
            pr_err("failed to get RTTFINISH3 resource\n");
            return -ENOSPC;
        }

        g_RegBaseRttfinish3 = ioremap(DRV_RTTKER_RTTFINISH3, 4U);
        if(NULL == g_RegBaseRttfinish3)
        {
            pr_err("failed to get RTTFINISH3 address \n");
            return -EFAULT;
        }
    }

    for (i = 0; i < (uint32_t) DRV_RTTKER_HIERARCHY_MAX; i++)
    {
        if (NULL == g_RegBaseAddrTable[i])
        {
            Resource = request_mem_region(drvRTT_PhyRegAddr[i], 4U, UDF_CPURTT_DRIVER_NAME);
            if (NULL == Resource)
            {
                pr_err("failed to get drvRTT_PhyRegAddr[%d] resource\n", i);
                return -ENOSPC;
            }

            g_RegBaseAddrTable[i] = ioremap(drvRTT_PhyRegAddr[i], 4U);
            if(NULL == g_RegBaseAddrTable[i])
            {
                pr_err("failed to get drvRTT_PhyRegAddr[%d] address \n", i);
                return -EFAULT;
            }
        }
    }

    for (i = 0; i < (uint32_t) DRV_CPURTTKER_CLUSTERNUM_MAX; i++)
    {
        for (j = 0; j < (uint32_t) DRV_CPURTTKER_CLUSTERNUM_CPUMAX; j++)
        {
            CpuIndex = (i * DRV_CPURTTKER_CLUSTERNUM_CPUMAX) + j;
            if (NULL == g_RegBasePwrctrlc[CpuIndex])
            {
                PwrctrlcBaseAddress = DRV_CPURTTKER_APMU_CORE_BASE + (i * DRV_CPURTTKER_CLUSTER_OFFSET) + (j * DRV_CPURTTKER_CLUSTER_CORE_OFFSET);
                Resource = request_mem_region(PwrctrlcBaseAddress, 4U, UDF_CPURTT_DRIVER_NAME);
                if (NULL == Resource)
                {
                    pr_err("failed to get Pwrctrlc[%d] resource\n", CpuIndex);
                    return -ENOSPC;
                }

                g_RegBasePwrctrlc[CpuIndex] = ioremap(PwrctrlcBaseAddress, 4U);
                if(NULL == g_RegBasePwrctrlc[CpuIndex])
                {
                    pr_err("failed to get PwrctrlcBaseAddress[%d] address \n", CpuIndex);
                    return -EFAULT;
                }
            }
        }
    }

    return 0;
}

static void drvCPURTT_DeInitRegAddr(void)
{
    int i;
    int j;
    unsigned long PwrctrlcBaseAddress;
    int CpuIndex;

    /* rerealse io address */
    iounmap(g_RegBaseRttfinish1);
    release_mem_region(DRV_RTTKER_RTTFINISH1, 4U);
    g_RegBaseRttfinish1 = NULL;

    iounmap(g_RegBaseRttfinish2);
    release_mem_region(DRV_RTTKER_RTTFINISH2, 4U);
    g_RegBaseRttfinish2 = NULL;

    iounmap(g_RegBaseRttfinish3);
    release_mem_region(DRV_RTTKER_RTTFINISH3, 4U);
    g_RegBaseRttfinish3 = NULL;

    for (i = 0; i < (uint32_t)DRV_RTTKER_HIERARCHY_MAX; i++)
    {
        iounmap(g_RegBaseAddrTable[i]);
        release_mem_region(drvRTT_PhyRegAddr[i], 4U);
        g_RegBaseAddrTable[i] = NULL;
    }

    for (i = 0; i < (uint32_t) DRV_CPURTTKER_CLUSTERNUM_MAX; i++)
    {
        for (j = 0; j < (uint32_t) DRV_CPURTTKER_CLUSTERNUM_CPUMAX; j++)
        {
            CpuIndex = (i * DRV_CPURTTKER_CLUSTERNUM_CPUMAX) + j;
            PwrctrlcBaseAddress = DRV_CPURTTKER_APMU_CORE_BASE + (i * DRV_CPURTTKER_CLUSTER_OFFSET) + (j * DRV_CPURTTKER_CLUSTER_CORE_OFFSET);

            iounmap(g_RegBasePwrctrlc[CpuIndex]);
            release_mem_region(PwrctrlcBaseAddress, 4U);
            g_RegBasePwrctrlc[CpuIndex] = NULL;
        }
    }
}

/* enable FbistInterrupt */
static long drvCPURTT_EnableFbistInterrupt(struct fbc_uio_share_platform_data *priv, unsigned int aAffinity)
{
    long ret;
    cpumask_t CpuMask;
    struct uio_info *uio_info = priv->uio_info;
    unsigned long flags;

    spin_lock_irqsave(&priv->lock, flags);
    if (__test_and_clear_bit(UIO_IRQ_DISABLED, &priv->flags))
    {
        enable_irq((unsigned int)uio_info->irq);
    }
    spin_unlock_irqrestore(&priv->lock, flags);

    cpumask_clear(&CpuMask);
    cpumask_set_cpu(aAffinity, &CpuMask);
    ret = (long)irq_set_affinity_hint((unsigned int)uio_info->irq, &CpuMask);
    if (ret < 0)
    {
        pr_err("cannot set irq affinity %d\n", (unsigned int)uio_info->irq);
        return -EINVAL;
    }

    return ret;
}

/* thread for A2 Runtime Test thread by CPU N */
int drvCPURTT_UDF_A2RuntimeThreadN(void *aArg)
{
    unsigned int CpuNum;
    long Ret = 0;
    cpumask_t CpuMask;
    unsigned int ClusterNum;
    drvCPURTT_A2ThreadArg_t *Arg;
    unsigned int BindCpuNum;
    unsigned long InterruptFlag;

    Arg = (drvCPURTT_A2ThreadArg_t*)aArg;
    CpuNum = Arg->mCpuNum;
    ClusterNum = Arg->mClusterNum;

    BindCpuNum = (ClusterNum * (uint16_t)DRV_CPURTTKER_CLUSTERNUM_CPUMAX) + CpuNum;

    cpumask_clear(&CpuMask);
    cpumask_set_cpu(BindCpuNum, &CpuMask);
    Ret = sched_setaffinity(current->pid, &CpuMask);

    if(Ret < 0)
    {
        pr_err("KCPUTHREAD[%d] sched_setaffinity fail %ld\n", BindCpuNum, Ret);
        complete_and_exit(&g_A2EndSynCompletion[ClusterNum][CpuNum], Ret);
    }

    g_A2Task[ClusterNum][CpuNum]->rt_priority = 1; /* max priority */
    g_A2Task[ClusterNum][CpuNum]->policy = SCHED_RR; /* max policy */

    /* Notify the generator that the thread has started */
    complete(&g_A2ThWakeupCompletion[ClusterNum][CpuNum]);

    do
    {
        /* Waiting for start request from user  */
        Ret = wait_for_completion_interruptible(&g_A2StartSynCompletion[ClusterNum]);
        if (0 != Ret)
        {
            pr_err(" CPU%d kthread wait completion error = %ld\n",CpuNum, Ret);
            g_TaskExitFlg = true;
        }

        if (!g_TaskExitFlg)
        {
            /* On CPU0, interrupt mask.  */
            if(CpuNum == (unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPU0)
            {
                InterruptFlag = arch_local_irq_save();
            }

            g_A2SmoniResult[ClusterNum][CpuNum] = R_SMONI_API_RuntimeTestA2Execute(g_A2Param[ClusterNum][CpuNum].Rttex);

            /* On CPU0, restore interrupt mask. */
            if(CpuNum == (unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPU0)
            {
                arch_local_irq_restore(InterruptFlag);
            }

           /* Notify that A2RuntimeTest is complete  */
            complete(&g_A2EndSynCompletion[ClusterNum][CpuNum]);
        }
        else
        {
            /* Exit if there is a thread termination request */
            complete_and_exit(&g_A2EndSynCompletion[ClusterNum][CpuNum], Ret);
        }

    }while(1);

    return 0;
}

/* Change interrupt affinity for field BIST finish interrupt */
static long drvCPURTT_SetInterruptCpu(unsigned int aCpuId, drvRTT_testtype_t aRuntimeTestType)
{
    long ret;
    struct platform_device *pdev = g_cpurtt_pdev;
    struct fbc_uio_share_platform_data *priv = platform_get_drvdata(pdev);
    unsigned int IrqNum = (unsigned int)priv->uio_info->irq;
    unsigned int ClusterNum;

    switch (aRuntimeTestType)
    {
        case DRV_CPURTTKER_TESTTYPE_A1:
            ret = (long)irq_set_affinity_hint(IrqNum, cpumask_of((unsigned int)drvCPURTTKER_InterruptCpuA1[aCpuId]));
            if(ret < 0)
            {
                pr_err( "irq_set_affinity_hint failed %ld \n",ret);
            }
            break;

        case DRV_CPURTTKER_TESTTYPE_A2:
            ClusterNum = aCpuId / DRV_CPURTTKER_CLUSTERNUM_CPUMAX;
            ret = (long)irq_set_affinity_hint(IrqNum, cpumask_of((unsigned int)drvCPURTTKER_InterruptCpuA2[ClusterNum]));
            if(ret < 0)
            {
                pr_err( "irq_set_affinity_hint failed %ld \n",ret);
            }
            break;

        default:
            ret = -EINVAL;
            pr_err("drvCPURTT_SetInterruptCpu parameter fail %ld \n",ret);
            break;
    }
    return ret;
}

/* Perform the preparatory work required to execute cpurtt with this function.  */
static long drvCPURTT_UDF_RuntimeTestInit(void)
{
    long ret = 0;
    unsigned int CpuIndex;
    unsigned int ClusterIndex;
    drvCPURTT_A2ThreadArg_t ThreadArg;

    g_TaskExitFlg = 0;
      /* initalize for A2 RuntimeTest*/
    for(ClusterIndex=0; ClusterIndex<(unsigned int)DRV_CPURTTKER_CLUSTERNUM_MAX; ClusterIndex++)
    {
        for(CpuIndex=0; CpuIndex<(unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPUMAX; CpuIndex++)
        {
            if(g_A2Task[ClusterIndex][CpuIndex] == NULL)
            {
                init_completion(&g_A2EndSynCompletion[ClusterIndex][CpuIndex]);
                init_completion(&g_A2ThWakeupCompletion[ClusterIndex][CpuIndex]);

                ThreadArg.mClusterNum = ClusterIndex;
                ThreadArg.mCpuNum = CpuIndex;

                g_A2Task[ClusterIndex][CpuIndex] = kthread_run(drvCPURTT_UDF_A2RuntimeThreadN, (void*)&ThreadArg, "cpurtt_A2rtt_%d_%d", ClusterIndex, CpuIndex);

                if (IS_ERR(g_A2Task[ClusterIndex][CpuIndex]))
                {
                    pr_err("A2_rtt_thread_run[%d][%d] failed", ClusterIndex, CpuIndex);
                    return -EINVAL;
                }

                ret = wait_for_completion_interruptible(&g_A2ThWakeupCompletion[ClusterIndex][CpuIndex]);
                if (0 != ret)
                {
                    pr_err("Thread[%d][U%d] kthread creat err = %ld\n", ClusterIndex, CpuIndex, ret);
                    break;
                }
            }
        }
    }

    return ret;
}

/* This function releases the resources prepared by drvCPURTT_UDF_RuntimeTestInit.  */
static long drvCPURTT_UDF_RuntimeTestDeinit(void)
{
    long ret = 0;

    return ret;
}

/* This function executes the smoni api specified by index. */
static long drvCPURTT_UDF_SmoniApiExe(drvCPURTT_SmoniTable_t index, uint32_t aCpuId, uint32_t *aArg, uint32_t *aSmoniret)
{
    long      ret = -1;
    cpumask_t CpuMask;
    unsigned long InterruptFlag;
    drvCPURTT_A1rttParam_t SmoniArgA1;
    drvCPURTT_A2rttParam_t SmoniArgA2;
    drvCPURTT_FbaWriteParam_t SmoniArgFwrite;
    drvCPURTT_FbaReadParam_t SmoniArgFread;
    drvCPURTT_SetTimeoutParam_t SmoniArgTimeout;
    drvCPURTT_SelfCheckParam_t SmoniArgSelf;
    drvCPURTT_UdfParam_t SmoniArgUdf;
    unsigned int CpuCnt;
    unsigned int CpuNum;
    unsigned int ClusterNum;

    if (!(0xFFFFFFF0 & aCpuId))
    {
        cpumask_clear(&CpuMask); 
        cpumask_set_cpu(aCpuId, &CpuMask); 
        ret = sched_setaffinity(current->pid, &CpuMask);
        if(ret < 0)
        {
            pr_err("sched_setaffinity fail %ld\n", ret);
            return ret;
        }
    }
    else
    {
        pr_err("affinity cpu number = %x fail %ld\n", aCpuId, ret);
        return ret;
    }

    switch (index)
    {
        case DRV_CPURTT_SMONIAPI_SETTIMEOUT:

            /* Copy smoni api arguments to kernel memory. */
            ret = copy_from_user(&SmoniArgTimeout, (const void __user *)(aArg), sizeof(drvCPURTT_SetTimeoutParam_t));
            if (ret == 0)
            {
                *aSmoniret = R_SMONI_API_SetTimeout(SmoniArgTimeout.Target, SmoniArgTimeout.MicroSec);
            }
            break;

        case DRV_CPURTT_SMONIAPI_LOCKACQUIRE:

            *aSmoniret = R_SMONI_API_RuntimeTestLockAcquire();
            ret = 0;

            break;

        case DRV_CPURTT_SMONIAPI_LOCKRELEASE:

            *aSmoniret = R_SMONI_API_RuntimeTestLockRelease();
            ret = 0;

            break;

        case DRV_CPURTT_SMONIAPI_A1EXE:

            /* Copy smoni api arguments to kernel memory. */
            ret = copy_from_user(&SmoniArgA1, (const void __user *)(aArg), sizeof(drvCPURTT_A1rttParam_t));

            if (ret == 0U)
            {
                /* Change interrupt affinity of fieldBIST finish Interrupt. */
                ret = drvCPURTT_SetInterruptCpu(aCpuId, DRV_CPURTTKER_TESTTYPE_A1);
                if (ret != 0)
                {
                    break;
                }

                InterruptFlag = arch_local_irq_save();
                *aSmoniret = R_SMONI_API_RuntimeTestA1Execute(SmoniArgA1.Rttex);
                arch_local_irq_restore(InterruptFlag);
            }

            break;

        case DRV_CPURTT_SMONIAPI_A2EXE:
            ret = copy_from_user(&SmoniArgA2, (const void __user *)(aArg), sizeof(drvCPURTT_A2rttParam_t));
            if (ret == 0U)
            {
                ClusterNum = aCpuId / (unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPUMAX;
                CpuNum = aCpuId % (unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPUMAX;
                if ((CpuNum == (unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPU0) && (ClusterNum < (unsigned int)DRV_CPURTTKER_CLUSTERNUM_MAX))
                {
                    /* Set argument for A2 Runtime Test. */
                    g_A2Param[ClusterNum][DRV_CPURTTKER_CLUSTERNUM_CPU0].Rttex = SmoniArgA2.Rttex;
                    for(CpuCnt=(unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPU1; CpuCnt<(unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPUMAX; CpuCnt++)
                    {
                        /* Another CPU0, Argument is dummy data. */
                        g_A2Param[ClusterNum][CpuCnt].Rttex = DRV_RTTKER_A2_PARAM_RTTEX_DATA;
                    }

                    ret = drvCPURTT_SetInterruptCpu(aCpuId, DRV_CPURTTKER_TESTTYPE_A2);
                    if (ret != 0)
                    {
                        break;
                    }

                    /* A2 Runtime Test Execution thread start request  */
                    for(CpuCnt=(unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPU0; CpuCnt<(unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPUMAX; CpuCnt++)
                    {
                        complete(&g_A2StartSynCompletion[ClusterNum]);
                    }

                    for (CpuCnt=(unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPU0; CpuCnt<(unsigned int)DRV_CPURTTKER_CLUSTERNUM_CPUMAX; CpuCnt++)
                    {
                        /* Wait until A2 Runtime Test completion notification is received from the A2 Runtime Test execution thread all CPUs  */
                        ret = wait_for_completion_interruptible(&g_A2EndSynCompletion[ClusterNum][CpuCnt]);
                        if (ret != 0)
                        {
                            pr_err(" Cluster%d CPU%d wait completion error = %ld\n", ClusterNum, CpuCnt, ret);
                            break;
                        }
                    }

                    if (ret != 0)
                    {
                        break;
                    }
                    *aSmoniret = g_A2SmoniResult[ClusterNum][CpuNum];
                }
                else
                {
                    pr_err(" A2 Runtime Test Cpu fail aCpuId = %d  %ld\n", aCpuId, ret);
                    ret = -EINVAL;
                }
           }
           break;

        case DRV_CPURTT_SMONIAPI_FBAWRITE:

            /* Copy smoni api arguments to kernel memory. */
            ret = copy_from_user(&SmoniArgFwrite, (const void __user *)(aArg), sizeof(drvCPURTT_FbaWriteParam_t));
            if (ret == 0U)
            {
                /* Copy the access destination address data to kernel memory.  */
                ret = copy_from_user(&g_SmoniAddrBuf[aCpuId][0], (const void __user *)SmoniArgFwrite.AddrBuf, (SmoniArgFwrite.RegCount * sizeof(uint32_t)));
                if (ret == 0U)
                {
                    /* Copy the data to write to kernel memory. */
                    ret = copy_from_user(&g_SmoniDataBuf[aCpuId][0], (const void __user *)SmoniArgFwrite.DataBuf, (SmoniArgFwrite.RegCount * sizeof(uint32_t)));
                    if (ret == 0U)
                    {
                        *aSmoniret = R_SMONI_API_RuntimeTestFbaWrite(&g_SmoniAddrBuf[aCpuId][0], &g_SmoniDataBuf[aCpuId][0], SmoniArgFwrite.RegCount);
                    }
                }
            }
            break;

        case DRV_CPURTT_SMONIAPI_FBAREAD:

            /* Copy smoni api arguments to kernel memory. */
            ret = copy_from_user(&SmoniArgFread, (const void __user *)(aArg), sizeof(drvCPURTT_FbaReadParam_t));
            if (ret == 0U)
            {
                /* Copy the access destination address data to kernel memory.  */
                ret = copy_from_user(&g_SmoniAddrBuf[aCpuId][0], (const void __user *)SmoniArgFread.AddrBuf, (SmoniArgFread.RegCount * sizeof(uint32_t)));
                if (ret == 0U)
                {
                    *aSmoniret = R_SMONI_API_RuntimeTestFbaRead(&g_SmoniAddrBuf[aCpuId][0], &g_SmoniDataBuf[aCpuId][0], SmoniArgFread.RegCount);
                    if (SMONI_FUSA_OK == *aSmoniret)
                    {
                        /* Copy the read data to user memory. */
                        ret = copy_to_user((void __user *)SmoniArgFread.DataBuf, (const void *)&g_SmoniDataBuf[aCpuId][0], (SmoniArgFread.RegCount * sizeof(uint32_t)));
                    }
                }
            }
            break;

        case DRV_CPURTT_SMONIAPI_SELFCHECK:

            /* Copy smoni api arguments to kernel memory. */
            ret = copy_from_user(&SmoniArgSelf, (const void __user *)(aArg), sizeof(drvCPURTT_SelfCheckParam_t));
            if (ret == 0U)
            {
                *aSmoniret = R_SMONI_API_SelfCheckExecute(SmoniArgSelf.Rttex, SmoniArgSelf.TargetHierarchy);
            }
            break;

        case DRV_CPURTT_SMONIAPI_UDF1:

            /* Copy smoni api arguments to kernel memory. */
            ret = copy_from_user(&SmoniArgUdf, (const void __user *)(aArg), sizeof(drvCPURTT_UdfParam_t));
            if (ret == 0U)
            {
                *aSmoniret = R_SMONI_API_CallUDF1(SmoniArgUdf.Arg1, SmoniArgUdf.Arg2, SmoniArgUdf.Arg3);
            }
            break;

        case DRV_CPURTT_SMONIAPI_UDF2:

            /* Copy smoni api arguments to kernel memory. */
            ret = copy_from_user(&SmoniArgUdf, (const void __user *)(aArg), sizeof(drvCPURTT_UdfParam_t));
            if (ret == 0U)
            {
                *aSmoniret = R_SMONI_API_CallUDF2(SmoniArgUdf.Arg1, SmoniArgUdf.Arg2, SmoniArgUdf.Arg3);
            }
            break;

        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}

static long drvCPURTT_UDF_FbistInit(void)
{
    long ret = -1;
    struct platform_device *pdev = g_cpurtt_pdev;
    struct fbc_uio_share_platform_data *priv = platform_get_drvdata(pdev);

    ret = pm_runtime_get_sync(&priv->pdev->dev);
    ret = fbc_uio_share_clk_enable(priv);
    if (ret != 0)
    {
        pm_runtime_put_sync(&priv->pdev->dev);
        return ret;
    }

    /* get resource for fba and fbc regster address */
    ret = drvCPURTT_InitRegAddr();
    if (ret < 0)
    {
        pm_runtime_put_sync(&priv->pdev->dev);
        return ret;
    }

    /* enabeled fbist finish interrupt with set interrupt affinity */
    ret = drvCPURTT_EnableFbistInterrupt(priv, DRV_RTTKER_FIELD_BIST_INT_CPU);
    if (ret != 0)
    {
        pm_runtime_put_sync(&priv->pdev->dev);
        return ret;
    }

    /* iInitialization of parameters related to callback execution request  */
    FbistCloseReq = false;
    g_CallbackInfo.head = 0;
    g_CallbackInfo.pos = 0;
    g_CallbackInfo.status = CB_QUEUE_STATUS_EMPTY;

    pm_runtime_put_sync(&priv->pdev->dev);

    return ret;
}

static long drvCPURTT_UDF_FbistDeInit(void)
{
    long ret = 0;
    struct platform_device *pdev = g_cpurtt_pdev;
    struct fbc_uio_share_platform_data *priv = platform_get_drvdata(pdev);
    struct uio_info *uio_info = priv->uio_info;

    fbc_uio_share_clk_disable(priv);
    pm_runtime_put_sync(&priv->pdev->dev);

    spin_lock(&priv->lock);
    if (!__test_and_set_bit(UIO_IRQ_DISABLED, &priv->flags))
    {
        irq_set_affinity_hint((unsigned int)uio_info->irq, NULL);
        disable_irq((unsigned int)uio_info->irq);
    }
    spin_unlock(&priv->lock);

    drvCPURTT_DeInitRegAddr();

    /* Release of semaphore waiting for callback notification  */
    FbistCloseReq = true;
    up(&CallbackSem);

    return ret;

}

static long drvCPURTT_UDF_WaitCbNotice(drvCPURTT_CallbackInfo_t *aParam)
{
    long      ret = 0;
    struct platform_device *pdev = g_cpurtt_pdev;
    struct fbc_uio_share_platform_data *priv = platform_get_drvdata(pdev);
    struct uio_info *uio_info = priv->uio_info;
    unsigned long flags;
    int      intret = 0;

    intret = down_interruptible(&CallbackSem); /* wait for semaphore release */

    if(!intret)
    {
        spin_lock(&priv->lock);
        if (!__test_and_set_bit(UIO_IRQ_DISABLED, &priv->flags))
        {
            disable_irq((unsigned int)uio_info->irq);
        }
        spin_unlock(&priv->lock);
        if (FbistCloseReq == false)
        {
            if (g_CallbackInfo.pos > 0)
            {
                aParam->FbistCbRequest = g_CallbackInfo.CbInfo[g_CallbackInfo.head].FbistCbRequest;
                aParam->RfsoOutputPinRequest = g_CallbackInfo.CbInfo[g_CallbackInfo.head].RfsoOutputPinRequest;
                g_CallbackInfo.head = (uint8_t)((g_CallbackInfo.head + 1U) % DRV_RTTKER_HIERARCHY_MAX);
                g_CallbackInfo.pos--;
                g_CallbackInfo.status = CB_QUEUE_STATUS_ENA;
            }
            else
            {
                g_CallbackInfo.status = CB_QUEUE_STATUS_EMPTY;
            }
        }
        else
        {
            g_CallbackInfo.status = CB_QUEUE_STATUS_EMPTY;
            ret = FBIST_CB_CLOSE_REQ;
        }

        spin_lock_irqsave(&priv->lock, flags);
        if (__test_and_clear_bit(UIO_IRQ_DISABLED, &priv->flags))
        {
            enable_irq((unsigned int)uio_info->irq);
        }
        spin_unlock_irqrestore(&priv->lock, flags);
    }
    else
    {
        pr_err("wait callback err = %d\n",intret);
        ret = (long)intret;
    }

    return ret;
}

/* This function executes the purttmod open system call.  */
static int CpurttDrv_open(struct inode *inode, struct file *file)
{
    return 0;
}

/* This function executes the purttmod close system call.  */
static int CpurttDrv_close(struct inode *inode, struct file *file)
{
    return 0;
}

/* This function executes the cpurttmod ioctl system call.  */
static long CpurttDrv_ioctl( struct file* filp, unsigned int cmd, unsigned long arg )
{
    long ret;
    drvCPURTT_CallbackInfo_t CbInfo;
    drvCPURTT_SmoniParam_t smoni_param;

    /* Executes the process corresponding to the command specified in the argument "cmd" passed by ioctl execution from the user layer. */
    switch (cmd) {
        case DRV_CPURTT_IOCTL_DEVINIT:
            ret = drvCPURTT_UDF_RuntimeTestInit();
            break;

        case DRV_CPURTT_IOCTL_DEVDEINIT:
            ret = drvCPURTT_UDF_RuntimeTestDeinit();
            break;

        case DRV_CPURTT_IOCTL_SMONI:
            /* Copy smoni api arguments to kernel memory. */            
            ret = copy_from_user(&smoni_param, (void __user *)arg, sizeof(drvCPURTT_SmoniParam_t));
            if (ret != 0) {
                ret = -EFAULT;
            }
            
            if (ret == 0)
            {
                ret = drvCPURTT_UDF_SmoniApiExe((drvCPURTT_SmoniTable_t)smoni_param.Index, smoni_param.CpuId, smoni_param.Arg, &smoni_param.RetArg);
            }

            if (ret == 0)
            {
                /* Copy the execution result of smoni api to user memory.  */
                if (copy_to_user((void __user *)arg, &smoni_param, sizeof(drvCPURTT_SmoniParam_t))) {
                    ret = -EFAULT;
                }
            }
            break;

        case DRV_CPURTT_IOCTL_DEVFBISTINIT:

            ret = drvCPURTT_UDF_FbistInit();
            break;

        case DRV_CPURTT_IOCTL_DEVFBISTDEINIT:

            ret = drvCPURTT_UDF_FbistDeInit();
            break;

        case DRV_CPURTT_IOCTL_WAIT_CALLBACK:

            ret = drvCPURTT_UDF_WaitCbNotice(&CbInfo);
            if (ret == 0)
            {
               if((DRV_CPURTT_CB_REQ_NON != CbInfo.FbistCbRequest) || (DRV_CPURTT_CB_REQ_NON != CbInfo.RfsoOutputPinRequest))
               {
                   /* Copy the execution result of smoni api to user memory.  */
                   if (copy_to_user((void __user *)arg, &CbInfo, sizeof(drvCPURTT_CallbackInfo_t))) {
                       ret = -EFAULT;
                   }
               }
               else
               {
                   ret = -EFAULT;
               }
            }
            break;

        default:
            ret = -EFAULT;
            break;
    }

    return ret;
}

static struct file_operations s_CpurttDrv_fops = {
    .open    = CpurttDrv_open,
    .release = CpurttDrv_close,
    .unlocked_ioctl = CpurttDrv_ioctl,
    .compat_ioctl   = CpurttDrv_ioctl,    /* for 32 bit system */
};

/* This function is executed when cpurttmod is loaded, and cpurttmod initialization processing is performed. */
static int CpurttDrv_init(void)
{
    int ret = 0;
    struct device *dev;
    unsigned int CpuIndex;
    unsigned int ClusterIndex;

    /* register character device for cpurttdrv */
    ret = register_chrdev(cpurtt_major, UDF_CPURTT_DRIVER_NAME, &s_CpurttDrv_fops);
    if (ret < 0)
    {
        pr_err( "register_chrdev = %d\n", cpurtt_major);
        return -EFAULT;
    }
    else
    {
        cpurtt_major = (unsigned int)ret;
        ret = 0;
    }

    /* create class module */
    cpurtt_class = class_create(THIS_MODULE, UDF_CPURTT_CLASS_NAME);
    if (IS_ERR(cpurtt_class))
    {
        pr_err("unable to create cpurttmod class\n");
        ret = PTR_ERR(cpurtt_class);
        unregister_chrdev(cpurtt_major, UDF_CPURTT_DRIVER_NAME);
        pr_err( "class_create = %d\n", ret);
        return ret;
    }

    /* create device file for cpurttdrv */
    dev = device_create(cpurtt_class, NULL, MKDEV(cpurtt_major, 0), NULL, "%s%d", UDF_CPURTT_CLASS_NAME, MINOR(MKDEV(cpurtt_major, 0)));
    if (IS_ERR(dev))
    {
        device_destroy(cpurtt_class, MKDEV(cpurtt_major, 0));
        class_destroy(cpurtt_class);
        unregister_chrdev(cpurtt_major, UDF_CPURTT_DRIVER_NAME);
        pr_err("cpurttmod: unable to create device cpurttmod%d\n", MINOR(MKDEV(cpurtt_major, 0)));
        ret = PTR_ERR(dev);
        return ret;
    }

    platform_driver_register(&fbc_uio_share_driver);
    if (g_cpurtt_pdev == NULL) {
        device_destroy(cpurtt_class, MKDEV(cpurtt_major, 0));
        class_destroy(cpurtt_class);
        unregister_chrdev(cpurtt_major, UDF_CPURTT_DRIVER_NAME);
        platform_driver_unregister(&fbc_uio_share_driver);
        pr_err("failed to cpurtt_driver_register\n");
        return -EINVAL;
    }

    init_completion(&g_A2StartSynCompletion[DRV_CPURTTKER_CLUSTERNUM_0]);
    init_completion(&g_A2StartSynCompletion[DRV_CPURTTKER_CLUSTERNUM_1]);
    sema_init(&CallbackSem, 0);

    for(ClusterIndex=0; ClusterIndex<DRV_CPURTTKER_CLUSTERNUM_MAX; ClusterIndex++)
    {
        for(CpuIndex=0; CpuIndex<DRV_CPURTTKER_CLUSTERNUM_CPUMAX; CpuIndex++)
        {
            g_A2Task[ClusterIndex][CpuIndex] = NULL;
        }
    }

    return ret;
}

/* This function is executed when cpurttmod is unloaded and frees cpurttmod resources.  */
static void CpurttDrv_exit(void)
{
    unsigned int ClusterIndex;
    unsigned int CpuIndex;

    /* release cpurtt thread for A2 Runtime Test*/
    g_TaskExitFlg = 1;

    for(ClusterIndex=0; ClusterIndex<DRV_CPURTTKER_CLUSTERNUM_MAX; ClusterIndex++)
    {
        complete_all(&g_A2StartSynCompletion[ClusterIndex]);
        for(CpuIndex=0; CpuIndex<DRV_CPURTTKER_CLUSTERNUM_CPUMAX; CpuIndex++)
        {
            if(g_A2Task[ClusterIndex][CpuIndex]!=NULL)
            {
                (void)wait_for_completion_interruptible(&g_A2EndSynCompletion[ClusterIndex][CpuIndex]);
                g_A2Task[ClusterIndex][CpuIndex] = NULL;
            }
        }
    }

    /* unload fbc uio_chare driver */
    platform_driver_unregister(&fbc_uio_share_driver);

    /* release device clase */
    device_destroy(cpurtt_class, MKDEV(cpurtt_major, 0));
    class_destroy(cpurtt_class);

    /* unload character device */
    unregister_chrdev(cpurtt_major, "UDF_CPURTT_DRIVER_NAME");
}

module_init(CpurttDrv_init);
module_exit(CpurttDrv_exit);

MODULE_AUTHOR("RenesasElectronicsCorp.");
MODULE_DESCRIPTION("CPURTT drive");
MODULE_LICENSE("GPL v2");
