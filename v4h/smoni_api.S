/*******************************************************************************
 * Copyright (c) 2018-2022 Renesas Electronics Corporation. All rights reserved.
 *
 * DESCRIPTION   : The source code of Secure Monitor.
 * CREATED       : 2018.06.13
 * MODIFIED      : 2022.12.14
 * TARGET OS     : OS agnostic.
 ******************************************************************************/

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF001 */
#define SMONI_FUSA_ERROR_PARAM_ASM     0x00000100
/* Covers:  V3H_SM_DD_SMONI_DEF01_DEF013 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF002 */
#define SMONI_SMC_ID_SET_TIMEOUT_ASM    0xC2000009
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF018 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF004 */
#define SMONI_SMC_ID_RTT_LOCK_ACQ_ASM   0xC2000005
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF013 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF005 */
#define SMONI_SMC_ID_RTT_LOCK_REL_ASM   0xC2000006
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF014 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF006 */
#define SMONI_SMC_ID_RTT_A1_EXEC_ASM    0xC2000002
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF011 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF007 */
#define SMONI_SMC_ID_RTT_A2_EXEC_ASM    0xC2000003
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF012 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF008 */
#define SMONI_SMC_ID_RTT_FBA_READ_ASM   0xC2000007
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF015 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF009 */
#define SMONI_SMC_ID_RTT_FBA_WRITE_ASM  0xC2000008
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF016 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_DEF010 */
#define SMONI_SMC_ID_SELF_CHK_EXEC_ASM  0xC200000A
/* Covers: V3H_SM_DD_SMONI_DEF05_DEF019 */

#define SMONI_SMC_ID_CALL_UDF1_ASM      0xC3000101

#define SMONI_SMC_ID_CALL_UDF2_ASM      0xC3000104

.global R_SMONI_API_SetTimeout
.global R_SMONI_API_RuntimeTestLockAcquire
.global R_SMONI_API_RuntimeTestLockRelease
.global R_SMONI_API_RuntimeTestA1Execute
.global R_SMONI_API_RuntimeTestA2Execute
.global R_SMONI_API_RuntimeTestFbaRead
.global R_SMONI_API_RuntimeTestFbaWrite
.global R_SMONI_API_SelfCheckExecute
.global R_SMONI_API_CallUDF1
.global R_SMONI_API_CallUDF2

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC001 */
.type R_SMONI_API_SetTimeout, @function
R_SMONI_API_SetTimeout:
  /* Covers: V3H_SM_DD_SMONIAPI_EX008 */
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC001_001 */
  mov w2, w1
  mov w1, w0
  ldr w0, =SMONI_SMC_ID_SET_TIMEOUT_ASM
  smc #0
  ret
  /* Covers: V3H_SM_DD_SMONIAPI_EX008_001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC003 */
.type R_SMONI_API_RuntimeTestLockAcquire, @function
R_SMONI_API_RuntimeTestLockAcquire:
  /* Covers: V3H_SM_DD_SMONIAPI_EX003 */
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC003_001 */
  ldr w0, =SMONI_SMC_ID_RTT_LOCK_ACQ_ASM
  smc #0
  ret
  /* Covers: V3H_SM_DD_SMONIAPI_EX003_001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC004 */
.type R_SMONI_API_RuntimeTestLockRelease, @function
R_SMONI_API_RuntimeTestLockRelease:
  /* Covers: V3H_SM_DD_SMONIAPI_EX004 */
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC004_001 */
  ldr w0, =SMONI_SMC_ID_RTT_LOCK_REL_ASM
  smc #0
  ret
  /* Covers: V3H_SM_DD_SMONIAPI_EX004_001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC005 */
.type R_SMONI_API_RuntimeTestA1Execute, @function
R_SMONI_API_RuntimeTestA1Execute:
  /* Covers: V3H_SM_DD_SMONIAPI_EX001 */
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC005_001 */
  mov w1, w0
  ldr w0, =SMONI_SMC_ID_RTT_A1_EXEC_ASM
  smc #0
  ret
  /* Covers: V3H_SM_DD_SMONIAPI_EX001_001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC006 */
.type R_SMONI_API_RuntimeTestA2Execute, @function
R_SMONI_API_RuntimeTestA2Execute:
  /* Covers: V3H_SM_DD_SMONIAPI_EX002 */
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC006_001 */
  mov w1, w0
  ldr w0, =SMONI_SMC_ID_RTT_A2_EXEC_ASM
  smc #0
  ret
  /* Covers: V3H_SM_DD_SMONIAPI_EX002_001 */

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007 */
.type R_SMONI_API_RuntimeTestFbaRead, @function
R_SMONI_API_RuntimeTestFbaRead:
  /* Covers: V3H_SM_DD_SMONIAPI_EX005 */

  /* Error if pointer is NULL or count is zero */
  cbz x0, 9f
  cbz x1, 9f
  cbz x2, 9f

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_002 */
  /* Save the X20-X25 to the stack */
  sub sp, sp, #0x30
  stp x20, x21, [sp, #0x00]
  stp x22, x23, [sp, #0x10]
  stp x24, x25, [sp, #0x20]
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_002 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_003 */
  /* Save the parameters */
  mov x20, x0
  mov x21, x1
  mov x22, x2
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_003 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_004 */
  /* Initialize the loop counter and status code */
  mov x23, #0
  mov x24, #0
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_004 */
1:
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_005 */
  /* Clear the targets */
  mov x4, #0
  mov x5, #0
  mov x6, #0
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_005 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_006 */
  /* Get the remaining register count */
  sub x0, x22, x23
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_006 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_007 */
  /* Set the targets */
  ldr w4, [x20], #4
  cmp x0, #1
  b.eq 2f
  ldr w5, [x20], #4
  cmp x0, #2
  b.eq 2f
  ldr w6, [x20], #4
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_007 */

2:
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_008 */
  /* Issue the SMC */
  ldr w0, =SMONI_SMC_ID_RTT_FBA_READ_ASM
  smc #0
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_008 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_009 */
  /* Save the status code */
  orr x24, x24, x0
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_009 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_010 */
  /* Get the remaining register count */
  sub x0, x22, x23
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_010 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_011 */
  /* Set the values */
  str w1, [x21], #4
  cmp x0, #1
  b.eq 3f
  str w2, [x21], #4
  cmp x0, #2
  b.eq 3f
  str w3, [x21], #4
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_011 */
3:

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_012 */
  /* Loop if register remains */
  add x23, x23, #3
  cmp x22, x23
  b.hi 1b
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_012 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_013 */
  /* Write the status to return value */
  mov x0, x24
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_013 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_014 */
  /* Restore the X20-X25 from the stack */
  ldp x24, x25, [sp, #0x20]
  ldp x22, x23, [sp, #0x10]
  ldp x20, x21, [sp, #0x00]
  add sp, sp, #0x30
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_014 */

  ret

9:
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC007_001 */
  /* Return with the parameter error */
  mov x0, #SMONI_FUSA_ERROR_PARAM_ASM
  /* Covers: V3H_SM_DD_SMONIAPI_EX005_001 */
  ret

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008 */
.type R_SMONI_API_RuntimeTestFbaWrite, @function
R_SMONI_API_RuntimeTestFbaWrite:
  /* Covers: V3H_SM_DD_SMONIAPI_EX006 */

  /* Error if pointer is NULL or count is zero */
  cbz x0, 9f
  cbz x1, 9f
  cbz x2, 9f

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_002 */
  /* Save the X20-X25 to the stack */
  sub sp, sp, #0x30
  stp x20, x21, [sp, #0x00]
  stp x22, x23, [sp, #0x10]
  stp x24, x25, [sp, #0x20]
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_002 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_003 */
  /* Save the parameters */
  mov x20, x0
  mov x21, x1
  mov x22, x2
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_003 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_004 */
  /* Initialize the loop counter and status code */
  mov x23, #0
  mov x24, #0
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_004 */
1:

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_005 */
  /* Clear the targets */
  mov x4, #0
  mov x5, #0
  mov x6, #0
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_005 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_006 */
  /* Get the remaining register count */
  sub x0, x22, x23
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_006 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_007 */
  /* Set the targets and values */
  ldr w4, [x20], #4
  ldr w1, [x21], #4
  cmp x0, #1
  b.eq 2f
  ldr w5, [x20], #4
  ldr w2, [x21], #4
  cmp x0, #2
  b.eq 2f
  ldr w6, [x20], #4
  ldr w3, [x21], #4
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_007 */

2:

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_008 */
  /* Issue the SMC */
  ldr w0, =SMONI_SMC_ID_RTT_FBA_WRITE_ASM
  smc #0
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_008 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_009 */
  /* Save the status code */
  orr x24, x24, x0
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_009 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_010 */
  /* Loop if register remains */
  add x23, x23, #3
  cmp x22, x23
  b.hi 1b
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_010 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_011 */
  /* Write the status to return value */
  mov x0, x24
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_011 */

  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_012 */
  /* Restore the X20-X25 from the stack */
  ldp x24, x25, [sp, #0x20]
  ldp x22, x23, [sp, #0x10]
  ldp x20, x21, [sp, #0x00]
  add sp, sp, #0x30
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_012 */

  ret

9:
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC008_001 */
  /* Return with the parameter error */
  mov x0, #SMONI_FUSA_ERROR_PARAM_ASM
  /* Covers: V3H_SM_DD_SMONIAPI_EX006_001 */
  ret

/* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC009 */
.type R_SMONI_API_SelfCheckExecute, @function
R_SMONI_API_SelfCheckExecute:
  /* Covers: V3H_SM_DD_SMONIAPI_EX009 */
  /* Traceability ID: V3H_SM_CD_SMONIAPI_S01_FUNC009_001 */
  mov w2, w1
  mov w1, w0
  ldr w0, =SMONI_SMC_ID_SELF_CHK_EXEC_ASM
  smc #0
  /* Covers: V3H_SM_DD_SMONIAPI_EX009_001 */
  ret

.type R_SMONI_API_CallUDF1, @function
R_SMONI_API_CallUDF1:
  mov w3, w2
  mov w2, w1
  mov w1, w0
  ldr w0, =SMONI_SMC_ID_CALL_UDF1_ASM
  smc #0
  ret

.type R_SMONI_API_CallUDF2, @function
R_SMONI_API_CallUDF2:
  mov w3, w2
  mov w2, w1
  mov w1, w0
  ldr w0, =SMONI_SMC_ID_CALL_UDF2_ASM
  smc #0
  ret


