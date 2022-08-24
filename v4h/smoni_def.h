/*******************************************************************************
 * Copyright (c) 2018-2022 Renesas Electronics Corporation. All rights reserved.
 *
 * DESCRIPTION   : The source code of Secure Monitor.
 * CREATED       : 2018.06.13
 * MODIFIED      : 2022.08.10
 * TARGET OS     : OS agnostic.
 ******************************************************************************/

#ifndef SMONI_DEF_H
#define SMONI_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

/* The status code returned by FuSa API. */
/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF001 */
#define SMONI_FUSA_OK              (0x00000000U)   /* Completed without faults/errors.     */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF004 */
#define SMONI_FUSA_ERROR_TIMEOUT   (0x00000004U)   /* A timeout error occurred.            */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF004 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF005 */
#define SMONI_FUSA_ERROR_PARALLEL  (0x00000008U)   /* A parallel error occurred.           */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF005 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF006 */
#define SMONI_FUSA_ERROR_INTERNAL  (0x00000010U)   /* An internal error occurred.          */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF006 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF007 */
#define SMONI_FUSA_ERROR_EXCLUSIVE (0x00000020U)   /* An exclusive control error occurred. */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF007 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF008 */
#define SMONI_FUSA_ERROR_LOCKED    (0x00000040U)   /* A locked error occurred.             */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF008 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF009 */
#define SMONI_FUSA_ERROR_UNLOCKED  (0x00000080U)   /* An unlocked error occurred.          */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF009 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF010 */
#define SMONI_FUSA_ERROR_PARAM     (0x00000100U)   /* An parameter error occurred.         */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF010 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF011 */
#define SMONI_FUSA_ERROR_NOTREADY  (0x00000200U)   /* An parameter error occurred.         */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF011 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF041 */
#define SMONI_FUSA_FAULT_WRITE     (0x00000400U)   /* Internal Bus Interface Check error occurred. */
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF012 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF012 */
#define SMONI_TIMEOUT_LOCK_REG_ACCESS        0U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF013 */
#define SMONI_TIMEOUT_LOCK_PSCI_STATE_CTRL   1U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF002 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF015 */
#define SMONI_TIMEOUT_LOCK_RTT_STATE_CTRL    3U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF004 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF016 */
#define SMONI_TIMEOUT_SYNC_RTT_A2_API        4U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF005 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF017 */
#define SMONI_TIMEOUT_SYNC_RTT_A2_SYNC_MAIN  5U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF006 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF018 */
#define SMONI_TIMEOUT_SYNC_RTT_A2_SYNC_SUB   6U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF007 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF019 */
#define SMONI_TIMEOUT_SYNC_RTT_A2_WAKEUP     7U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF008 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF020 */
#define SMONI_TIMEOUT_SYNC_RTT_A2_FINISH     8U
/* Covers:  V3H_SM_DD_SMONI_DEF02_DEF009 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF031 */
#define SMONI_SMC_ID_SET_TIMEOUT    0xC2000009U
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF008 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF033 */
#define SMONI_SMC_ID_RTT_LOCK_ACQ   0xC2000005U
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF003 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF034 */
#define SMONI_SMC_ID_RTT_LOCK_REL   0xC2000006U
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF004 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF035 */
#define SMONI_SMC_ID_RTT_A1_EXEC    0xC2000002U
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF036 */
#define SMONI_SMC_ID_RTT_A2_EXEC    0xC2000003U
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF002 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF037 */
#define SMONI_SMC_ID_RTT_FBA_READ   0xC2000007U
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF005 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF038 */
#define SMONI_SMC_ID_RTT_FBA_WRITE  0xC2000008U
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF006 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_H01_DEF039 */
#define SMONI_SMC_ID_SELF_CHK_EXEC  0xC200000AU
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF009 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SMONI_DEF_H */
