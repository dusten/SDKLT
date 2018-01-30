/*! \file bcmmgmt_sysm.h
 *
 * Component definitions for system manager.
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

#ifndef BCMMGMT_SYSM_H
#define BCMMGMT_SYSM_H

#include <sal/sal_types.h>

/*!
 * \brief System Manager component IDs for SDK.
 *
 * Component owner should add his/her component ID here
 */
typedef enum {
    BCMMGMT_TRM_COMP_ID = 0,
    BCMMGMT_LT_COMP_ID = 1,
    BCMMGMT_BD_COMP_ID = 2,
    BCMMGMT_CFG_COMP_ID = 3,
    BCMMGMT_DRD_COMP_ID = 4,
    BCMMGMT_LTD_COMP_ID = 5,
    BCMMGMT_PTM_COMP_ID = 6,
    BCMMGMT_LTM_COMP_ID = 7,
    BCMMGMT_IMM_FE_COMP_ID = 8,
    BCMMGMT_IMM_BE_COMP_ID = 9,
    BCMMGMT_FP_COMP_ID = 10,
    BCMMGMT_LM_COMP_ID = 11,
    BCMMGMT_ECMP_COMP_ID = 12,
    BCMMGMT_TRUNK_COMP_ID = 13,
    BCMMGMT_LTP_COMP_ID = 14,
    BCMMGMT_PC_COMP_ID = 15,
    BCMMGMT_METER_COMP_ID = 16,
    BCMMGMT_TNL_COMP_ID = 17,
    BCMMGMT_VLAN_COMP_ID = 18,
    BCMMGMT_TM_COMP_ID = 19,
    BCMMGMT_ECN_COMP_ID = 20,
    BCMMGMT_LRD_COMP_ID = 21,
    BCMMGMT_L2_COMP_ID = 22,
    BCMMGMT_EVM_COMP_ID = 23,
    BCMMGMT_PORT_COMP_ID = 24,
    BCMMGMT_DLB_COMP_ID = 25,
    BCMMGMT_MIRROR_COMP_ID = 26,
    BCMMGMT_RM_HASH_COMP_ID = 27,
    BCMMGMT_RM_TCAM_COMP_ID = 28,
    BCMMGMT_RM_ALPM_COMP_ID = 29,
    BCMMGMT_UFT_COMP_ID = 30,
    BCMMGMT_FCL_COMP_ID = 31,
    BCMMGMT_CTH_ALPM_COMP_ID = 32,
    BCMMGMT_CTH_ALPM_BE_COMP_ID = 33,
    BCMMGMT_MAX_COMP_ID
} bcmmgmt_sysm_comp_id_t;

/*!
 * \brief Component start callback function signature.
 *
 * This function signature is used by each component to register itself
 * with the different system manager categories.
 * This function will be the first function of the component being called.
 * Its only purpose is to register the system manager callback functions
 * for this component. No other activity should be performed.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcmmgmt_comp_start_f)(void);

/*!
 * \brief System Manager component list.
 *
 * This structure contains a list of component start functions, which
 * are used to register each component with the System Manager.
 */
typedef struct bcmmgmt_comp_list_s {

    /*! Array of component start functions. */
    const bcmmgmt_comp_start_f *comp_start;

    /*! Number of entries in start function array. */
    const uint32_t num_comp;

} bcmmgmt_comp_list_t;

#endif /* BCMMGMT_SYSM_H */
