/*! \file bcmtnl_decap_internal.c
 *
 * TNL_MPLS_DECAP/TNL_MPLS_DECAP_TRUNK Custom Handler
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

/*******************************************************************************
  Includes
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtnl/bcmtnl_decap.h>

#include <bcmtnl/bcmtnl_decap_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMTNL_COMMON

bcmtnl_decap_drv_t *bcmtnl_decap_drv[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
  Private functions
 */

static int
bcmtnl_mpls_decap_hw_entry_read(int unit,
                                bcmlt_opcode_t opcode,
                                uint32_t trans_id,
                                bcmltd_sid_t sid,
                                bcmdrd_sid_t hw_sid,
                                bcmptm_rm_hash_entry_mode_t mode,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out)
{
    int         rv = 0, i = 0, ent_count;
    uint32_t    **ekw = NULL;
    uint32_t    **edw = NULL;
    uint32_t    **rsp_ekw = NULL;
    uint32_t    **rsp_edw = NULL;
    uint32_t    wsize = 0, alloc_sz = 0;
    uint32_t    req_flags = 0, rsp_flags = 0;
    bcmptm_op_type_t    ptm_op;
    bcmltd_sid_t        rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&hash_attrs, 0, sizeof(bcmptm_rm_hash_entry_attrs_t));
    sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));
    sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));
    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    ent_count = 4;
    alloc_sz = (sizeof(uint32_t *) * ent_count);
    SHR_ALLOC(ekw, alloc_sz, "Key ptrs");
    SHR_NULL_CHECK(ekw, SHR_E_MEMORY);
    sal_memset(ekw, 0, alloc_sz);
    SHR_ALLOC(edw, alloc_sz, "Data ptrs");
    SHR_NULL_CHECK(edw, SHR_E_MEMORY);
    sal_memset(edw, 0, alloc_sz);

    SHR_ALLOC(rsp_ekw, alloc_sz, "Response Key ptrs");
    SHR_NULL_CHECK(rsp_ekw, SHR_E_MEMORY);
    sal_memset(rsp_ekw, 0, alloc_sz);
    SHR_ALLOC(rsp_edw, alloc_sz, "Response Data ptrs");
    SHR_NULL_CHECK(rsp_edw, SHR_E_MEMORY);
    sal_memset(rsp_edw, 0, alloc_sz);

    for (i = 0; i < ent_count; i++) {
        alloc_sz = (sizeof(uint32_t) * ent_count);
        SHR_ALLOC(ekw[i], alloc_sz, "Key value");
        SHR_NULL_CHECK(ekw[i], SHR_E_MEMORY);
        sal_memset(ekw[i], 0, alloc_sz);

        SHR_ALLOC(edw[i], alloc_sz, "Data value");
        SHR_NULL_CHECK(edw[i], SHR_E_MEMORY);
        sal_memset(edw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_ekw[i], alloc_sz, "Response Key value");
        SHR_NULL_CHECK(rsp_ekw[i], SHR_E_MEMORY);
        sal_memset(rsp_ekw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_edw[i], alloc_sz, "Response Data value");
        SHR_NULL_CHECK(rsp_edw[i], SHR_E_MEMORY);
        sal_memset(rsp_edw[i], 0, alloc_sz);
    }

    wsize = bcmdrd_pt_entry_wsize(unit, hw_sid);

    if (in != NULL) {
        BCMTNL_MPLS_DECAP_LT_TO_HW_MAP(unit)(unit, sid, hw_sid, in, ekw, edw);
    }

    hash_attrs.entry_mode = mode;
    lt_mreq_info.entry_attrs = &hash_attrs;
    lt_mreq_info.entry_key_words = ekw;
    lt_mreq_info.entry_data_words = edw;
    lt_mreq_info.pdd_l_dtyp = NULL;
    lt_mreq_info.same_key = 0;
    lt_mreq_info.rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info.rsp_entry_data_buf_wsize = wsize;

    lt_mrsp_info.rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info.rsp_entry_data_words = rsp_edw;
    lt_mrsp_info.rsp_pdd_l_dtyp = NULL;

    pt_dyn_info.tbl_inst = -1;

    if (opcode == BCMLT_OPCODE_LOOKUP) {
        ptm_op = BCMPTM_OP_LOOKUP;
    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        if (in == NULL) {
            ptm_op = BCMPTM_OP_GET_FIRST;
        } else {
            ptm_op = BCMPTM_OP_GET_NEXT;
        }
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    rv = bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  sid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  ptm_op,
                                  &lt_mreq_info,
                                  trans_id,
                                  &lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags);
    if (SHR_FAILURE(rv)) {
        SHR_RETURN_VAL_EXIT(rv);
    }

    if (out != NULL) {
        BCMTNL_MPLS_DECAP_HW_TO_LT_MAP(unit)(unit,
                                             sid,
                                             hw_sid,
                                             rsp_ekw,
                                             rsp_edw,
                                             out);
    }

exit:
    for (i = 0; i < ent_count; i++) {
        SHR_FREE(ekw[i]);
        SHR_FREE(edw[i]);
        SHR_FREE(rsp_ekw[i]);
        SHR_FREE(rsp_edw[i]);
    }
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);

    SHR_FUNC_EXIT();
}

static int
bcmtnl_mpls_decap_hw_entry_write(int unit,
                                 bcmlt_opcode_t opcode,
                                 uint32_t trans_id,
                                 bcmltd_sid_t sid,
                                 bcmdrd_sid_t hw_sid,
                                 bcmptm_rm_hash_entry_mode_t mode,
                                 const bcmltd_fields_t *in)
{
    int         i = 0, ent_count = 0;
    uint32_t    **ekw = NULL;
    uint32_t    **edw = NULL;
    uint32_t    **rsp_ekw = NULL;
    uint32_t    **rsp_edw = NULL;
    uint32_t    wsize = 0, alloc_sz = 0;
    uint32_t    req_flags = 0, rsp_flags = 0;
    bcmptm_op_type_t    ptm_op;
    bcmltd_sid_t        rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&hash_attrs, 0, sizeof(bcmptm_rm_hash_entry_attrs_t));
    sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));
    sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));
    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    ent_count = 4;
    alloc_sz = (sizeof(uint32_t *) * ent_count);
    SHR_ALLOC(ekw, alloc_sz, "Key ptrs");
    SHR_NULL_CHECK(ekw, SHR_E_MEMORY);
    sal_memset(ekw, 0, alloc_sz);
    SHR_ALLOC(edw, alloc_sz, "Data ptrs");
    SHR_NULL_CHECK(edw, SHR_E_MEMORY);
    sal_memset(edw, 0, alloc_sz);

    SHR_ALLOC(rsp_ekw, alloc_sz, "Response Key ptrs");
    SHR_NULL_CHECK(rsp_ekw, SHR_E_MEMORY);
    sal_memset(rsp_ekw, 0, alloc_sz);
    SHR_ALLOC(rsp_edw, alloc_sz, "Response Data ptrs");
    SHR_NULL_CHECK(rsp_edw, SHR_E_MEMORY);
    sal_memset(rsp_edw, 0, alloc_sz);

    for (i = 0; i < ent_count; i++) {
        alloc_sz = (sizeof(uint32_t) * ent_count);
        SHR_ALLOC(ekw[i], alloc_sz, "Key value");
        SHR_NULL_CHECK(ekw[i], SHR_E_MEMORY);
        sal_memset(ekw[i], 0, alloc_sz);

        SHR_ALLOC(edw[i], alloc_sz, "Data value");
        SHR_NULL_CHECK(edw[i], SHR_E_MEMORY);
        sal_memset(edw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_ekw[i], alloc_sz, "Response Key value");
        SHR_NULL_CHECK(rsp_ekw[i], SHR_E_MEMORY);
        sal_memset(rsp_ekw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_edw[i], alloc_sz, "Response Data value");
        SHR_NULL_CHECK(rsp_edw[i], SHR_E_MEMORY);
        sal_memset(rsp_edw[i], 0, alloc_sz);
    }

    wsize = bcmdrd_pt_entry_wsize(unit, hw_sid);

    BCMTNL_MPLS_DECAP_LT_TO_HW_MAP(unit)(unit, sid, hw_sid, in, ekw, edw);

    hash_attrs.entry_mode = mode;
    lt_mreq_info.entry_attrs = &hash_attrs;
    lt_mreq_info.entry_key_words = ekw;
    lt_mreq_info.entry_data_words = edw;
    lt_mreq_info.pdd_l_dtyp = NULL;
    lt_mreq_info.same_key = 0;
    lt_mreq_info.rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info.rsp_entry_data_buf_wsize = wsize;

    lt_mrsp_info.rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info.rsp_entry_data_words = rsp_edw;
    lt_mrsp_info.rsp_pdd_l_dtyp = NULL;

    pt_dyn_info.tbl_inst = -1;

    if (opcode == BCMLT_OPCODE_UPDATE) {
        /* Read first for update. */
        SHR_IF_ERR_EXIT(
            bcmptm_ltm_mreq_keyed_lt(unit,
                                     req_flags,
                                     sid,
                                     (void *)&pt_dyn_info,
                                     NULL,
                                     BCMPTM_OP_LOOKUP,
                                     &lt_mreq_info,
                                     trans_id,
                                     &lt_mrsp_info,
                                     &rsp_ltid,
                                     &rsp_flags));

        /* Copy response data into request data. */
        alloc_sz = (sizeof(uint32_t) * ent_count);
        for (i = 0; i < ent_count; i++) {
            sal_memset(ekw[i], 0, alloc_sz);
            sal_memset(edw[i], 0, alloc_sz);
            sal_memcpy(edw[i], rsp_edw[i], alloc_sz);
        }
        /* Update the data buffer with user input. */
        BCMTNL_MPLS_DECAP_LT_TO_HW_MAP(unit)(unit, sid, hw_sid, in, ekw, edw);
    }

    if (opcode == BCMLT_OPCODE_INSERT) {
        ptm_op = BCMPTM_OP_INSERT;
    } else if (opcode == BCMLT_OPCODE_DELETE) {
        ptm_op = BCMPTM_OP_DELETE;
    } else if (opcode == BCMLT_OPCODE_UPDATE) {
        ptm_op = BCMPTM_OP_INSERT;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmptm_ltm_mreq_keyed_lt(unit,
                                             req_flags,
                                             sid,
                                             (void *)&pt_dyn_info,
                                             NULL,
                                             ptm_op,
                                             &lt_mreq_info,
                                             trans_id,
                                             &lt_mrsp_info,
                                             &rsp_ltid,
                                             &rsp_flags));

exit:
    for (i = 0; i < ent_count; i++) {
        SHR_FREE(ekw[i]);
        SHR_FREE(edw[i]);
        SHR_FREE(rsp_ekw[i]);
        SHR_FREE(rsp_edw[i]);
    }
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);

    SHR_FUNC_EXIT();
}

/******************************************************************************
  Public functions
 */

/*!
 * \fn bcmtnl_mpls_decap_validate
 *
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
                           const bcmltd_fields_t *in)
{
    int         count = 0, idx = 0;
    uint32_t    fid = 0;
    bool        key1 = false, key2 = false;

    SHR_FUNC_ENTER(unit);

    count = in->count;

    for (idx = 0; idx < count; idx++) {
        fid = in->field[idx]->id;

        switch (sid) {
        case TNL_MPLS_DECAPt:
            if (fid == TNL_MPLS_DECAPt_MPLS_LABELf) {
                key1 = true;
            }

            if (fid == TNL_MPLS_DECAPt_MODPORTf) {
                key2 = true;
            }
            break;
        case TNL_MPLS_DECAP_TRUNKt:
            if (fid == TNL_MPLS_DECAP_TRUNKt_MPLS_LABELf) {
                key1 = true;
            }

            if (fid == TNL_MPLS_DECAP_TRUNKt_TRUNK_IDf) {
                key2 = true;
            }
            break;
        default:
            break;
        }
    }

    if (key1 == false || key2 == false) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_mpls_decap_insert
 *
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
                         const bcmltd_fields_t *in)
{
    int         rv = 0, hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMTNL_DECAP_SID_COUNT(unit);

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMTNL_DECAP_SID_LIST(unit)[i];
        mode = BCMTNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        rv = 0;
        rv = bcmtnl_mpls_decap_hw_entry_read(unit,
                                             BCMLT_OPCODE_LOOKUP,
                                             trans_id,
                                             sid,
                                             hw_sid,
                                             mode,
                                             in,
                                             NULL);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
        }

        SHR_IF_ERR_EXIT(
            bcmtnl_mpls_decap_hw_entry_write(unit,
                                             BCMLT_OPCODE_INSERT,
                                             trans_id,
                                             sid,
                                             hw_sid,
                                             mode,
                                             in));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_mpls_decap_lookup
 *
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
                         bcmltd_fields_t *out)
{
    int         hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMTNL_DECAP_SID_COUNT(unit);

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMTNL_DECAP_SID_LIST(unit)[i];
        mode = BCMTNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        SHR_IF_ERR_EXIT(
            bcmtnl_mpls_decap_hw_entry_read(unit,
                                            BCMLT_OPCODE_LOOKUP,
                                            trans_id,
                                            sid,
                                            hw_sid,
                                            mode,
                                            in,
                                            out));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_mpls_decap_delete
 *
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
                         const bcmltd_fields_t *in)
{
    int         hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMTNL_DECAP_SID_COUNT(unit);

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMTNL_DECAP_SID_LIST(unit)[i];
        mode = BCMTNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        SHR_IF_ERR_EXIT(
            bcmtnl_mpls_decap_hw_entry_read(unit,
                                            BCMLT_OPCODE_LOOKUP,
                                            trans_id,
                                            sid,
                                            hw_sid,
                                            mode,
                                            in,
                                            NULL));

        SHR_IF_ERR_EXIT(
            bcmtnl_mpls_decap_hw_entry_write(unit,
                                             BCMLT_OPCODE_DELETE,
                                             trans_id,
                                             sid,
                                             hw_sid,
                                             mode,
                                             in));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_mpls_decap_update
 *
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
                         const bcmltd_fields_t *in)
{
    int         hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMTNL_DECAP_SID_COUNT(unit);

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMTNL_DECAP_SID_LIST(unit)[i];
        mode = BCMTNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        SHR_IF_ERR_EXIT(
            bcmtnl_mpls_decap_hw_entry_read(unit,
                                            BCMLT_OPCODE_LOOKUP,
                                            trans_id,
                                            sid,
                                            hw_sid,
                                            mode,
                                            in,
                                            NULL));

        SHR_IF_ERR_EXIT(
            bcmtnl_mpls_decap_hw_entry_write(unit,
                                             BCMLT_OPCODE_UPDATE,
                                             trans_id,
                                             sid,
                                             hw_sid,
                                             mode,
                                             in));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_mpls_decap_traverse
 *
 * \brief TNL_MPLS_DECAP traverse function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  in              Input Values.
 * \param [in]  sid             Logical table ID.
 * \param [out] out             Fetched Field Values.
 *
 * Traverse and return values from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_mpls_decap_traverse(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_sid_t sid,
                           bcmltd_fields_t *out)
{
    int         hw_count = 0, i = 0, rv = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMTNL_DECAP_SID_COUNT(unit);

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMTNL_DECAP_SID_LIST(unit)[i];
        mode = BCMTNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        rv = bcmtnl_mpls_decap_hw_entry_read(unit,
                                             BCMLT_OPCODE_TRAVERSE,
                                             trans_id,
                                             sid,
                                             hw_sid,
                                             mode,
                                             in,
                                             out);
        if (SHR_FAILURE(rv)) {
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Set pointer to tunnel decap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Chip driver structure.
 *
 * \return SHR_E_NONE
 */
int
bcmtnl_decap_drv_set(int unit,
                     bcmtnl_decap_drv_t *drv)
{
    bcmtnl_decap_drv[unit] = drv;

    return SHR_E_NONE;
}

/*!
 * \brief Get pointer to tunnel decap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
int
bcmtnl_decap_drv_get(int unit,
                     bcmtnl_decap_drv_t **drv)
{
    *drv = bcmtnl_decap_drv[unit];

    return SHR_E_NONE;
}
