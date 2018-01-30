/*! \file bcmtnl_decap_internal.h
 *
 * TNL_IPV4/IPV6/MPLS_ENCAP Custom Handler header file
 * This file contains APIs, defines for IPv4, IPv6, MPLS encap
 * handler functions.
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

#ifndef BCMTNL_DECAP_INTERNAL_H
#define BCMTNL_DECAP_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>


/*! TNL decap LT to HW entry map. */
typedef int (*bcmtnl_mpls_decap_hw_entry_map_f)(int unit,
                                                bcmltd_sid_t lt_sid,
                                                bcmdrd_sid_t sid,
                                                const bcmltd_fields_t *in,
                                                uint32_t **ekw,
                                                uint32_t **edw);
/*! TNL decap HW to LT entry map. */
typedef int (*bcmtnl_mpls_decap_lt_entry_map_f)(int unit,
                                                bcmltd_sid_t lt_sid,
                                                bcmdrd_sid_t sid,
                                                uint32_t **ekw,
                                                uint32_t **edw,
                                                const bcmltd_fields_t *out);

/*! Structure to define TNL decap driver attributes. */
typedef struct bcmtnl_decap_drv_s {
    /*! Number of PTs associated with this LT. */
    int         sid_count;
    /*! List of PT names. */
    uint32_t                         *sid_list;
    /*! List of PT modes. */
    bcmptm_rm_hash_entry_mode_t      *entry_attrs_list;
    /*! Function pointer to map TNL_MPLS_DECAP LT entry to HW entry. */
    bcmtnl_mpls_decap_hw_entry_map_f mpls_decap_lt_to_hw_map;
    /*! Function pointer to map TNL_MPLS_DECAP HW entry to LT entry. */
    bcmtnl_mpls_decap_lt_entry_map_f mpls_decap_hw_to_lt_map;
} bcmtnl_decap_drv_t;

/*! BCMTNL decap driver for this unit. */
#define BCMTNL_DECAP_DRV(unit)                  (bcmtnl_decap_drv[unit])
/*! BCMTNL decap driver PT count for this unit. */
#define BCMTNL_DECAP_SID_COUNT(unit)            BCMTNL_DECAP_DRV(unit)->sid_count
/*! BCMTNL decap driver PT list for this unit. */
#define BCMTNL_DECAP_SID_LIST(unit)             BCMTNL_DECAP_DRV(unit)->sid_list
/*! BCMTNL decap driver PT RM entry modes for this unit. */
#define BCMTNL_DECAP_ENTRY_ATTRS_LIST(unit)     BCMTNL_DECAP_DRV(unit)->entry_attrs_list
/*! Function pointer to map TNL_MPLS_DECAP LT entry to HW entry for this unit. */
#define BCMTNL_MPLS_DECAP_LT_TO_HW_MAP(unit)    BCMTNL_DECAP_DRV(unit)->mpls_decap_lt_to_hw_map
/*! Function pointer to map TNL_MPLS_DECAP HW entry to LT entry for this unit. */
#define BCMTNL_MPLS_DECAP_HW_TO_LT_MAP(unit)    BCMTNL_DECAP_DRV(unit)->mpls_decap_hw_to_lt_map

/*! Convert uint32 array into one uint64 variable. */
#define BCMTNL_DECAP_UINT32_TO_UINT64(val32, val64)     \
    do {                                                \
        val64 = val32[1];                               \
        val64 = ((val64 << 32) | val32[0]);             \
    } while(0)

/*! Convert uint64 variable into uint32 array. */
#define BCMTNL_DECAP_UINT64_TO_UINT32(val64, val32)     \
    do {                                                \
        val32[0] = (uint32_t)val64;                     \
        val32[1] = (uint32_t)(val64 >> 32);             \
    } while(0)

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_tnl_decap_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_tnl_decap_drv_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief TNL_MPLS_DECAP validation function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          LT opcode.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Data Validation of received arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_mpls_decap_validate(int unit,
                           bcmlt_opcode_t opcode,
                           bcmltd_sid_t sid,
                           const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP insert function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Insert values to physical table with entry allocation.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_mpls_decap_insert(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t sid,
                         const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP lookup function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [in]  sid             Logical table ID.
 * \param [out] out             Fetched Field Values.
 *
 * Lookup values from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_mpls_decap_lookup(int unit,
                         uint32_t trans_id,
                         const bcmltd_fields_t *in,
                         bcmltd_sid_t sid,
                         bcmltd_fields_t *out);
/*!
 * \brief TNL_MPLS_DECAP delete function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Delete entry from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_mpls_decap_delete(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t sid,
                         const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP update function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Update existing entry with new values in physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_mpls_decap_update(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t sid,
                         const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP traverse function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [in]  sid             Logical table ID.
 * \param [out] out             Fetched Field Values.
 *
 * Lookup values from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_mpls_decap_traverse(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_sid_t sid,
                           bcmltd_fields_t *out);

/*!
 * \brief Set pointer to tunnel decap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Chip driver structure.
 *
 * \return SHR_E_NONE
 */
extern int
bcmtnl_decap_drv_set(int unit, bcmtnl_decap_drv_t *drv);

/*!
 * \brief Get pointer to tunnel decap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmtnl_decap_drv_get(int unit, bcmtnl_decap_drv_t **drv);

#endif /* BCMTNL_DECAP_INTERNAL_H */
