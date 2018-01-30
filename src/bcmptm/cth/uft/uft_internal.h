/*! \file uft_internal.h
 *
 * defines internal to UFT
 *
 * This file defines data structures of Unified Format Table.
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

#ifndef UFT_INTERNAL_H
#define UFT_INTERNAL_H

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>

/* In most cases, one exact match group resource's change only affects
 * one mainly pt_reg_info. Such as L3_ENTRY_IPV4_UNICASTm,
 * L3_ENTRY_IPV4_MULTICASTm, L3_ENTRY_IPV6_UNICASTm,
 * L3_ENTRY_IPV6_MULTICASTm, they have same logical banks, bukets.
 * But for EXACT_MATCH_2/4m, they have different buckets, bucket base.
 */
#define UFT_MAX_PT_REG_INFOS_PER_EM_GROUP (2)

#define UFT_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)


/*! Data structure to save the info of TABLE_EM_CONTROLt entry */
typedef struct uft_tbl_em_ctrl_s {
    /*! Logical table id. */
    uint32_t lt_id;

    /*! Hash group type id. */
    uint32_t grp_id;

    /*! validity of grp_id. */
    bool  grp_id_valid;

    /*! Move depth. */
    uint8_t move_depth;

    /*! validity of move_depth. */
    bool move_depth_valid;
} uft_tbl_em_ctrl_t;

/*! Data structure to save the info of DEVICE_EM_GROUPt entry */
typedef struct uft_dev_em_grp_s {
    /*! EM group id. */
    uint32_t grp_id;

    /*! Whether enable robust hash. */
    bool robust;

    /*! validity of robust. */
    bool robust_valid;

    /*! Hash vector type. */
    uint8_t vector_type;

    /*! validity of vector_type. */
    bool vector_type_valid;

    /*! Number of hash banks in this group. */
    uint8_t bank_cnt;

    /*! validity of bank_cnt. */
    bool bank_cnt_valid;

    /*! Hash vector type. */
    uint8_t bank[BCMPTM_RM_HASH_MAX_BANK_COUNT];

    /*! validity of bank array. */
    bool bank_valid[BCMPTM_RM_HASH_MAX_BANK_COUNT ];

    /*! validity of vector_type. */
    bool sibling_dev_grp_id_valid;
} uft_dev_em_grp_t;

/*! Data structure to save the info of DEVICE_EM_BANKt entry */
typedef struct uft_dev_em_bank_s {
    /*! EM bank id. */
    uint32_t bank_id;

    /*! Hash offset of this bank when it is used as hast table. */
    uint8_t offset;

    /*! validity of robust. */
    bool offset_valid;
} uft_dev_em_bank_t;

/*!
 * \brief Initialize imm callback function.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_uft_imm_register_init(int unit);

#endif /* UFT_INTERNAL_H */
