/*******************************************************************************
 * Copyright (c) 2018-2023 Renesas Electronics Corporation. All rights reserved.
 *
 * DESCRIPTION   : The source code of Secure Monitor.
 * CREATED       : 2018.06.13
 * MODIFIED      : 2022.12.14
 * TARGET OS     : OS agnostic.
 ******************************************************************************/

#ifndef SMONI_API_H
#define SMONI_API_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/types.h>

/***********************************************************
 Macro definitions
 ***********************************************************/
/*******************************************************************************************************************//**
 * @ingroup  CPURTT_DRV
 * @defgroup CPURTT_DRV_Defines macro definitions
 * Please refer to User's Manual.
 * @{
 **********************************************************************************************************************/
/** @} */

/***********************************************************
 Typedef definitions
 ***********************************************************/
/*******************************************************************************************************************//**
 * @ingroup  CPURTT_DRV
 * @defgroup CPURTT_DRV_Types type definitions
 * Please refer to User's Manual.
 * @{
 **********************************************************************************************************************/
/** @} */

/***********************************************************
 Exported global functions (to be accessed by other files)
 ***********************************************************/
/*******************************************************************************************************************//**
 * @ingroup  CPURTT_DRV
 * @defgroup CPURTT_DRV_Function function definitions
 *
 * @{
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      This function saves the following timeout in memory of Secure Monitor. \n
               - PSCI, A1 Runtime Test, A2 Runtime Test exclusive control time. \n
               - Synchronization time between CPUs during A2 Runtime Test.
 * @param[in]  LuiTargetTimeout  Timeout Setting.
 * @param[in]  LuiMicroSecond    The timeout in microsecond.
 * @retval     SMONI_FUSA_OK              The function completed without any error.
 * @retval     SMONI_FUSA_ERROR_INTERNAL  Fatal error occurs in Secure Monitor internal functions.
 * @retval     SMONI_FUSA_ERROR_PARAM     The target timeout Id is invalid.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_SetTimeout(uint32_t LuiTargetTimeout, uint32_t LuiMicroSecond);
/*******************************************************************************************************************//**
 * @brief   This function sets the X0 register to 0xC2000005, then issue SMC. Acquires the lock for Runtime Test by SMC.
 * @retval  SMONI_FUSA_OK              The function completed without any error.
 * @retval  SMONI_FUSA_ERROR_INTERNAL  Fatal error occurs in Secure Monitor internal functions. \n
 *                                     It is recommended to restart the system.
 * @retval  SMONI_FUSA_ERROR_LOCKED    The lock is already acquired. \n
 *                                     It is recommended to check the lock sequence.
 * @retval  SMONI_FUSA_ERROR_NOTREADY  All CPUs are not on. It is recommended to check the CPU.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_RuntimeTestLockAcquire(void);
/*******************************************************************************************************************//**
 * @brief   This function sets the X0 register to 0xC2000006, then, issue SMC. Releases the lock for Runtime Test by SMC.
 * @retval  SMONI_FUSA_OK              The function completed without any error.
 * @retval  SMONI_FUSA_ERROR_INTERNAL  Fatal error occurs in Secure Monitor internal functions. \n
 *                                     It is recommended to restart the system.
 * @retval  SMONI_FUSA_ERROR_LOCKED    A1/A2 Runtime Test is executed. \n
 *                                     It is recommended to retry after A1/A2 Runtime Test finished.
 * @retval  SMONI_FUSA_ERROR_UNLOCKED  The lock is already released. \n
 *                                     It is recommended to check the lock sequence.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_RuntimeTestLockRelease(void);
/*******************************************************************************************************************//**
 * @brief   This function sets the X0 register to 0xC2000002, X1 to the value of LuiRttex, then, issue SMC. \n
            Execute the EL3 required part of entering/exiting operation of A1 Runtime Test by SMC.
 * @retval  SMONI_FUSA_OK              The function completed without any error.
 * @retval  SMONI_FUSA_ERROR_INTERNAL  The RTTEX is invalid. \n
 *                                     It is recommended to retry with a valid parameter.
 * @retval  SMONI_FUSA_ERROR_PARAM     A1/A2 Runtime Test is executed. \n
 *                                     It is recommended to retry after A1/A2 Runtime Test finished.
 * @retval  SMONI_FUSA_ERROR_PARALLEL  Already executed the A1/A2 Runtime Test. \n
 *                                     It is recommended to retry after A1/A2 Runtime Test is finished.
 * @retval  SMONI_FUSA_ERROR_UNLOCKED  The lock is not acquired. \n
 *                                     It is recommended to retry after the lock is acquired.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_RuntimeTestA1Execute(uint32_t LuiRttex);
/*******************************************************************************************************************//**
 * @brief   This function sets the X0 register to 0xC2000003, X1 to the value of LuiRttex, then, issue SMC. \n
            Execute the EL3 required part of entering/exiting operation of A2 Runtime Test by SMC.
 * @retval  SMONI_FUSA_OK              The function completed without any error.
 * @retval  SMONI_FUSA_ERROR_INTERNAL  Fatal error occurs in Secure Monitor internal functions. \n
 *                                     It is recommended to restart the system.
 * @retval  SMONI_FUSA_ERROR_PARAM     The RTTEX is invalid. \n
 *                                     It is recommended to retry with valid parameters.
 * @retval  SMONI_FUSA_ERROR_TIMEOUT   CPUs cannot synchronize within the timeout. \n
 *                                     It is recommended to retry so that all CPUs execute at the same time.
 * @retval  SMONI_FUSA_ERROR_PARALLEL  Already executed the A1/A2 Runtime Test. \n
 *                                     It is recommended to retry after A1/A2 Runtime Test is finished.
 * @retval  SMONI_FUSA_ERROR_UNLOCKED  The lock is not acquired. \n
 *                                     It is recommended to retry after the lock is acquired.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_RuntimeTestA2Execute(uint32_t LuiRttex);
/*******************************************************************************************************************//**
 * @brief   This function sets the X0 register to 0xC2000007, X4-X6 to the elements of LpRegTargets, then, issues SMC. \n
            SMC is issued multiple times according to LuiRegCount. Reads the registers of FBA by SMC.
 * @retval  SMONI_FUSA_OK              The function completed without any error.
 * @retval  SMONI_FUSA_ERROR_INTERNAL  This error code will be returned in the following cases.
                                       - The pointer is NULL, or the count is zero.
                                       - The hierarchy is invalid, or the offset is not the list.
                                       It is recommended to restart the system.
 * @retval  SMONI_FUSA_ERROR_PARAM     The parameter (LpRegTargets, LpRegCount or LuiRegValues) is invalid. It is recommended retry with valid parameters.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_RuntimeTestFbaRead(uint32_t *LpRegTargets, uint32_t *LpRegValues, uint32_t LuiRegCount);
/*******************************************************************************************************************//**
 * @brief   This function sets the X0 register to 0xC2000008, X1-X6 to the elements of LpRegTargets and LpRegValues. \n
            SMC is issued multiple times according to LuiRegCount. Writes the registers of FBA by SMC.
 * @retval  SMONI_FUSA_OK  The function completed without any error.
 * @retval  SMONI_FUSA_ERROR_INTERNAL   This error code will be returned in the following cases.
                                        - The pointer is NULL, or the count is zero.
                                        - The hierarchy is invalid, or the offset is not the list.
                                        It is recommended to restart the system.
 * @retval  SMONI_FUSA_ERROR_PARAM      The parameter (LpRegTargets, LpRegValues or LuiRegCount) is invalid. It is recommended retry with valid parameters.
 * @retval  SMONI_FUSA_ERROR_EXCLUSIVE  This error code will be returned in the following cases.
                                        - Write RTTEX for Runtime Test when 1 is not set in the hierarchy bit of RTTFINISH1.
                                        - Write the other registers (except RTTEX) when running A1/A2 Runtime Test.
                                        - Write RTTEX for Self Check when b'11 (finish) is not set in the STS bit of SLFRST.
                                        - Write the other registers (except RTTEX) when running Self Check.
                                        It is recommended to retry at timing other than above.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_RuntimeTestFbaWrite(uint32_t *LpRegTargets, uint32_t *LpRegValues, uint32_t LuiRegCount);
/*******************************************************************************************************************//**
 * @brief   This function sets the X0 register to 0xC200000A, X1 to the value of LuiRttex, X2 to the value of LuiTargetHierarchy then, issue SMC. \n
            Execute the Self Check by setting SLFE = B'1 and KCD = H'F1 of RTTEX register.
 * @retval  SMONI_FUSA_OK  The function completed without any error.
 * @retval  SMONI_FUSA_ERROR_INTERNAL  Fatal error occurs in Secure Monitor internal functions. It is recommended to restart the system. \n
                                       The RTTEX or TargetHierarchy is invalid. It is recommended to retry with a valid parameter.
 * @retval  SMONI_FUSA_ERROR_PARALLEL  This error code will be returned in the following cases.
                                       - Already executed the Self Check by another CPU.
                                       - Already executed the A1/A2 Runtime Test.
                                       It is recommended to retry after Self Check or A1/A2 Runtime Test is finished.
 * @retval  SMONI_FUSA_ERROR_UNLOCKED  The lock is not acquired. It is recommended to retry after the lock is acquired.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_SelfCheckExecute(uint32_t LuiRttex, uint32_t LuiTargetHierarchy);
/*******************************************************************************************************************//**
 * @brief   This function calls Smoni_UserDeFunc1 and returns the result as is.
 * @retval  (User defined)  Return Codes from Smoni_UserDeFunc1.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_CallUDF1(uint32_t LuiArgs1, uint32_t LuiArgs2, uint32_t LuiArgs3);
/*******************************************************************************************************************//**
 * @brief   This function calls Smoni_UserDeFunc2 and returns the result as is.
 * @retval  (User defined)  Return Codes from Smoni_UserDeFunc2.
 **********************************************************************************************************************/
uint32_t R_SMONI_API_CallUDF2(uint32_t LuiArgs1, uint32_t LuiArgs2, uint32_t LuiArgs3);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SMONI_API_H */
