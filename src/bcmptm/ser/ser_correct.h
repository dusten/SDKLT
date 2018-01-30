/*! \file ser_correct.h
 *
 * Interface functions for SER correction
 *
 * This file contains implementation of SER correction
 */
/*
 * Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
 * Broadcom Limited and/or its subsidiaries.
 * 
 * Broadcom Switch Software License
 * 
 * This license governs the use of the accompanying Broadcom software. Your 
 * use of the software indicates your acceptance of the terms and conditions 
 * of this license. If you do not agree to the terms and conditions of this 
 * license, do not use the software.
 * 1. Definitions
 *    "Licensor" means any person or entity that distributes its Work.
 *    "Software" means the original work of authorship made available under 
 *    this license.
 *    "Work" means the Software and any additions to or derivative works of 
 *    the Software that are made available under this license.
 *    The terms "reproduce," "reproduction," "derivative works," and 
 *    "distribution" have the meaning as provided under U.S. copyright law.
 *    Works, including the Software, are "made available" under this license 
 *    by including in or with the Work either (a) a copyright notice 
 *    referencing the applicability of this license to the Work, or (b) a copy 
 *    of this license.
 * 2. Grant of Copyright License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    copyright license to reproduce, prepare derivative works of, publicly 
 *    display, publicly perform, sublicense and distribute its Work and any 
 *    resulting derivative works in any form.
 * 3. Grant of Patent License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    patent license to make, have made, use, offer to sell, sell, import, and 
 *    otherwise transfer its Work, in whole or in part. This patent license 
 *    applies only to the patent claims licensable by Licensor that would be 
 *    infringed by Licensor's Work (or portion thereof) individually and 
 *    excluding any combinations with any other materials or technology.
 *    If you institute patent litigation against any Licensor (including a 
 *    cross-claim or counterclaim in a lawsuit) to enforce any patents that 
 *    you allege are infringed by any Work, then your patent license from such 
 *    Licensor to the Work shall terminate as of the date such litigation is 
 *    filed.
 * 4. Redistribution
 *    You may reproduce or distribute the Work only if (a) you do so under 
 *    this License, (b) you include a complete copy of this License with your 
 *    distribution, and (c) you retain without modification any copyright, 
 *    patent, trademark, or attribution notices that are present in the Work.
 * 5. Derivative Works
 *    You may specify that additional or different terms apply to the use, 
 *    reproduction, and distribution of your derivative works of the Work 
 *    ("Your Terms") only if (a) Your Terms provide that the limitations of 
 *    Section 7 apply to your derivative works, and (b) you identify the 
 *    specific derivative works that are subject to Your Terms. 
 *    Notwithstanding Your Terms, this license (including the redistribution 
 *    requirements in Section 4) will continue to apply to the Work itself.
 * 6. Trademarks
 *    This license does not grant any rights to use any Licensor's or its 
 *    affiliates' names, logos, or trademarks, except as necessary to 
 *    reproduce the notices described in this license.
 * 7. Limitations
 *    Platform. The Work and any derivative works thereof may only be used, or 
 *    intended for use, with a Broadcom switch integrated circuit.
 *    No Reverse Engineering. You will not use the Work to disassemble, 
 *    reverse engineer, decompile, or attempt to ascertain the underlying 
 *    technology of a Broadcom switch integrated circuit.
 * 8. Termination
 *    If you violate any term of this license, then your rights under this 
 *    license (including the license grants of Sections 2 and 3) will 
 *    terminate immediately.
 * 9. Disclaimer of Warranty
 *    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 *    KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
 *    NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
 *    THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
 *    IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
 * 10. Limitation of Liability
 *    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
 *    THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
 *    SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
 *    INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
 *    OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
 *    (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
 *    LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
 *    COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
 *    THE POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef SER_CORRECT_H
#define SER_CORRECT_H

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>


/*******************************************************************************
 * Defines
 */
/*!
 * \brief information read from SER_RESULT_0m or SER_RESULT_1m
 */
typedef struct bcmptm_ser_tcam_correct_info_s {
    /*! value of field RANGEf */
    uint32_t hw_idx;
    /*! value of field SBUS_ADDRf */
    uint32_t sbus_addr;
    /*! value of field ACC_TYPEf */
    uint32_t acc_type;
} bcmptm_ser_tcam_crt_info_t;

/*!
 * \brief information read from MMU_XCFG_MEM_FAIL_ADDR_64m
 * \n or MMU_SCFG_MEM_FAIL_ADDR_64m or MMU_GCFG_MEM_FAIL_ADDR_64m
 */
typedef struct bcmptm_ser_mmu_correct_info_s {
    /*! value of field RANGEf, multiple bits have error */
    uint32_t multi_bits;
    /*! value of field SBUS_ADDRf,  "1bit" or "parity/2bit" */
    uint32_t err_type;
    /*! value of field ACC_TYPEf */
    uint32_t eaddr;
    /*! block type of mem reporting SER error */
    int      blk_type;
    /*! instance of block XPE, SC etc. */
    int      blk_inst;
} bcmptm_ser_mmu_crt_info_t;

/*!
 * \brief information for SER error of buffer or bus.
 */
typedef struct bcmptm_ser_misc_blk_correct_info_s {
    /*! Multiple bits have error */
    int             multi_bits;
    /*! entry index*/
    int             entry_idx;
    /*! 2bit ECC error */
    int             err_type;
    /*! block type of mem reporting SER error */
    int             blk_type;
    /*! instance of memory table */
    int             tbl_inst;
    /*!
     * SID of status register for buffer or bus, such as: IDB_IS_TDM_CAL_ECC_STATUSr.
     * According to status register, SERC can know which memory needs to be corrected or cleared.
     */
    bcmdrd_sid_t    status_reg;
    /*! Software ID of buffer or bus, created by "BCMPTM_SER_BB_SID_MAGIC_CREATE" */
    bcmdrd_sid_t    magic_sid;
} bcmptm_ser_misc_blk_crt_info_t;

/*!
 * \brief information read from ING_SER_FIFOm
 * \n or EGR_SER_FIFOm
 */
typedef struct bcmptm_ser_ing_egr_correct_info_s {
    /*! value of field NON_SBUSf, 0: sbus accessabe, 1: non-sbus accessable*/
    uint32_t non_sbus;
    /*! value of field DROPf, 1: SER caused packet drop */
    uint32_t drop;
    /*! value of field ECC_PARITYf, ecc or parity SER error */
    uint32_t ecc_parity;
    /*! value of field MEM_TYPEf,  1: reg, 0: mem */
    uint32_t reg;
    /*! value of field ADDRESSf */
    uint32_t address;
    /*! value of field INSTRUCTION_TYPEf */
    uint32_t instruction_type;
    /*! value of field MULTIPLEf */
    uint32_t multi;
    /*! value of field MEMINDEXf or REGINDEXf */
    uint32_t index;
    /*! block type of mem reporting SER error */
    int      blk_type;
    /*! ING EGR pipe number */
    int      blk_inst;
    /*! stage of pipe */
    int      pipe_stage;
    /*! memory base, used to find buffer or bus */
    int      reg_mem_base;
} bcmptm_ser_ing_egr_crt_info_t;

/*!
 * \brief Information read from SER_RESULT_0m/SER_RESULT_1m or
 * \n information read from MMU_XCFG_MEM_FAIL_ADDR_64m/
 * \n MMU_SCFG_MEM_FAIL_ADDR_64m/MMU_GCFG_MEM_FAIL_ADDR_64m or
 * \n status registers or ING_SER_FIFOm/EGR_SER_FIFOm
 */
typedef union bcmptm_ser_crt_info_s {
    bcmptm_ser_tcam_crt_info_t     ser_engine_tcam;
    bcmptm_ser_mmu_crt_info_t      ser_mmu_mem;
    bcmptm_ser_misc_blk_crt_info_t ser_misc_mem;
    bcmptm_ser_ing_egr_crt_info_t  ser_ing_egr_mem;
} bcmptm_ser_raw_crt_info_t;


typedef struct bcmptm_ser_raw_info_s {
    /* valid : 0, information in "serc_info" is invalid */
    uint32_t                   valid;
    bcmptm_ser_raw_crt_info_t  serc_info;
} bcmptm_ser_raw_info_t;

/*!
 * \brief  Pause SERC thread
 *
 * \param [in] unit                  Logical device id
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_serc_pause(int unit);

/*!
 * \brief  Resume SERC thread
 *
 * \param [in] unit                  Logical device id
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_serc_resume(int unit);

/*!
 * \brief  disable SERC thread to call bcmptm_ser_severity_fault_check
 * C-unit will inject so many SER errors to one PT, so need to disable severity fault checking
 *
 * \param [in] unit Logical device id
 * \param [in] disable 1: disable severity fault checking, 0: enable severity fault checking
 */
extern void
bcmptm_ser_severity_fault_checking_disable(int unit, int enable);

#endif /* SER_CORRECT_H */
