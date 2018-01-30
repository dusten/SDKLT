/*! \file bcm56960_a0_tnl_encap_drv.c
 *
 * This file defines the detach, attach routines of
 * TNL encap driver for bcm56960_a0.
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

#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmtnl/bcmtnl_internal.h>
#include "bcm56960_a0_tnl_encap_drv.h"

#define BSL_LOG_MODULE BSL_LS_BCMTNL_COMMON

/*******************************************************************************
 * Local definitions
 */

uint32_t bcm56960_a0_tnl_mpls_label_fid_arr[] = {
    MPLS_LABEL_0f,
    MPLS_LABEL_1f,
    MPLS_LABEL_2f,
    MPLS_LABEL_3f,
};
uint32_t bcm56960_a0_tnl_mpls_ttl_fid_arr[] = {
    MPLS_TTL_0f,
    MPLS_TTL_1f,
    MPLS_TTL_2f,
    MPLS_TTL_3f,
};
uint32_t bcm56960_a0_tnl_mpls_exp_sel_fid_arr[] = {
    MPLS_EXP_SELECT_0f,
    MPLS_EXP_SELECT_1f,
    MPLS_EXP_SELECT_2f,
    MPLS_EXP_SELECT_3f,
};
uint32_t bcm56960_a0_tnl_mpls_exp_ptr_fid_arr[] = {
    MPLS_EXP_MAPPING_PTR_0f,
    MPLS_EXP_MAPPING_PTR_1f,
    MPLS_EXP_MAPPING_PTR_2f,
    MPLS_EXP_MAPPING_PTR_3f,
};
uint32_t bcm56960_a0_tnl_mpls_exp_fid_arr[] = {
    MPLS_EXP_0f,
    MPLS_EXP_1f,
    MPLS_EXP_2f,
    MPLS_EXP_3f,
};
uint32_t bcm56960_a0_tnl_mpls_pri_fid_arr[] = {
    NEW_PRI_0f,
    NEW_PRI_1f,
    NEW_PRI_2f,
    NEW_PRI_3f,
};
uint32_t bcm56960_a0_tnl_mpls_cfi_fid_arr[] = {
    NEW_CFI_0f,
    NEW_CFI_1f,
    NEW_CFI_2f,
    NEW_CFI_3f,
};

uint32_t bcm56960_a0_tnl_encap_mpls_push_fid_arr[] = {
    MPLS_PUSH_ACTION_0f,
    MPLS_PUSH_ACTION_1f,
    MPLS_PUSH_ACTION_2f,
    MPLS_PUSH_ACTION_3f,
};

/* Driver structure. */
bcmtnl_encap_drv_t bcm56960_a0_tnl_encap_drv = {
    .num_ipv4_tnl = BCM56960_A0_NUM_IPV4_TNL,
    .num_ipv6_tnl = BCM56960_A0_NUM_IPV6_TNL,
    .num_mpls_tnl = BCM56960_A0_NUM_MPLS_TNL,
    .max_labels = BCM56960_A0_MAX_LABELS,
    .ipv4_tnl_width = BCM56960_A0_IPV4_TNL_WIDTH,
    .ipv6_tnl_width = BCM56960_A0_IPV6_TNL_WIDTH,
    .mpls_tnl_width = BCM56960_A0_MPLS_TNL_WIDTH,
    .ipv4_start_idx = BCM56960_A0_IPV4_TNL_START_IDX,
    .ipv6_start_idx = BCM56960_A0_IPV6_TNL_START_IDX,
    .mpls_start_idx = BCM56960_A0_MPLS_TNL_START_IDX,
    .ipv4_hw_sid = EGR_IP_TUNNELm,
    .ipv6_hw_sid = EGR_IP_TUNNEL_IPV6m,
    .mpls_hw_sid = EGR_IP_TUNNEL_MPLSm,
    .ipv4_hw_to_lt_map = bcm56960_a0_tnl_ipv4_encap_lt_entry_populate,
    .ipv6_hw_to_lt_map = bcm56960_a0_tnl_ipv6_encap_lt_entry_populate,
    .mpls_hw_to_lt_map = bcm56960_a0_tnl_mpls_encap_lt_entry_populate,
    .ipv4_lt_to_hw_map = bcm56960_a0_tnl_ipv4_encap_hw_entry_prepare,
    .ipv6_lt_to_hw_map = bcm56960_a0_tnl_ipv6_encap_hw_entry_prepare,
    .mpls_lt_to_hw_map = bcm56960_a0_tnl_mpls_encap_hw_entry_prepare,
    .mpls_hw_entry_clear = bcm56960_a0_tnl_mpls_encap_hw_entry_clear,
};

/*******************************************************************************
 * Private functions
 */

/*
 * \brief Map LT field to HW field
 *
 * Get the HW field corresponding to the LT field for
 * TNL_IPV4_ENCAP LT.
 *
 * \param [in]  lt_fid  LT field.
 * \param [out] hw_fid  HW table field
 */
static void
bcm56960_a0_tnl_ipv4_lt_fid_to_hw_fid(uint32_t lt_fid,
                                      uint32_t *hw_fid)
{
    switch (lt_fid) {
    case TNL_IPV4_ENCAPt_DST_MACf:
        *hw_fid = DEST_ADDRf;
        break;
    case TNL_IPV4_ENCAPt_SRC_MACf:
        *hw_fid = MACSAf;
        break;
    case TNL_IPV4_ENCAPt_SRC_IPV4f:
        *hw_fid = SIPf;
        break;
    case TNL_IPV4_ENCAPt_DST_IPV4f:
        *hw_fid = DIPf;
        break;
    case TNL_IPV4_ENCAPt_TNL_TYPEf:
        *hw_fid = TUNNEL_TYPEf;
        break;
    case TNL_IPV4_ENCAPt_TNL_TTLf:
        *hw_fid = TTLf;
        break;
    case TNL_IPV4_ENCAPt_IPV4_IN_IPV4_DF_MODEf:
        *hw_fid = IPV4_DF_SELf;
        break;
    case TNL_IPV4_ENCAPt_IPV6_IN_IPV4_DF_MODEf:
        *hw_fid = IPV6_DF_SELf;
        break;
    case TNL_IPV4_ENCAPt_PHB_EGR_DSCP_ACTIONf:
        *hw_fid = DSCP_SELf;
        break;
    case TNL_IPV4_ENCAPt_PHB_EGR_IP_INT_PRI_TO_DSCP_IDf:
        *hw_fid = DSCP_MAPPING_PTRf;
        break;
    case TNL_IPV4_ENCAPt_DSCPf:
        *hw_fid = DSCPf;
        break;
    case TNL_IPV4_ENCAPt_SRC_L4_PORTf:
        *hw_fid = L4_SRC_PORTf;
        break;
    case TNL_IPV4_ENCAPt_DST_L4_PORTf:
        *hw_fid = L4_DEST_PORTf;
        break;
    case TNL_IPV4_ENCAPt_ECN_TNL_ENCAP_IDf:
        *hw_fid = ECN_ENCAP_MAPPING_PTRf;
        break;
    default:
        break;
    }
}

/*
 * \brief Map LT field to HW field
 *
 * Get the HW field corresponding to the LT field for
 * TNL_IPV6_ENCAP LT.
 *
 * \param [in]  lt_fid  LT field.
 * \param [out] partial Indicates if LT fid maps to a part og HW field
 * \param [out] hw_fid  HW field
 * \param [out] s_bit   Start bit of HW field if partial==true.
 * \param [out] e_bit   End bit of HW field if partial==true.
 */
static void
bcm56960_a0_tnl_ipv6_lt_fid_to_hw_fid(uint32_t lt_fid,
                                      bool *partial,
                                      uint32_t *hw_fid,
                                      uint32_t *s_bit,
                                      uint32_t *e_bit)
{
    *partial = false;
    switch (lt_fid) {
    case TNL_IPV6_ENCAPt_DST_MACf:
        *hw_fid = DEST_ADDRf;
        break;
    case TNL_IPV6_ENCAPt_SRC_MACf:
        *hw_fid = MACSAf;
        break;
    case TNL_IPV6_ENCAPt_SRC_IPV6u_LOWERf:
        *partial = true;
        *s_bit = 102;
        *e_bit = 165;
        break;
    case TNL_IPV6_ENCAPt_SRC_IPV6u_UPPERf:
        *partial = true;
        *s_bit = 166;
        *e_bit = 229;
        break;
    case TNL_IPV6_ENCAPt_DST_IPV6u_LOWERf:
        *partial = true;
        *s_bit = 292;
        *e_bit = 355;
        break;
    case TNL_IPV6_ENCAPt_DST_IPV6u_UPPERf:
        *partial = true;
        *s_bit = 356;
        *e_bit = 419;
        break;
    case TNL_IPV6_ENCAPt_TNL_TYPEf:
        *hw_fid = TUNNEL_TYPEf;
        break;
    case TNL_IPV6_ENCAPt_HOP_LIMITf:
        *hw_fid = TTLf;
        break;
    case TNL_IPV6_ENCAPt_FLOW_LABELf:
        *hw_fid = FLOW_LABELf;
        break;
    case TNL_IPV6_ENCAPt_PHB_EGR_DSCP_ACTIONf:
        *hw_fid = DSCP_SELf;
        break;
    case TNL_IPV6_ENCAPt_PHB_EGR_IP_INT_PRI_TO_DSCP_IDf:
        *hw_fid = DSCP_MAPPING_PTRf;
        break;
    case TNL_IPV6_ENCAPt_DSCPf:
        *hw_fid = DSCPf;
        break;
    case TNL_IPV6_ENCAPt_SRC_L4_PORTf:
        *hw_fid = L4_SRC_PORTf;
        break;
    case TNL_IPV6_ENCAPt_DST_L4_PORTf:
        *hw_fid = L4_DEST_PORTf;
        break;
    case TNL_IPV6_ENCAPt_ECN_TNL_ENCAP_IDf:
        *hw_fid = ECN_ENCAP_MAPPING_PTRf;
        break;
    default:
        break;
    }
}

/*
 * \brief Map LT field to HW field
 *
 * Get the HW field corresponding to the LT field for
 * TNL_MPLS_ENCAP LT.
 *
 * \param [in]  lt_fid  LT field.
 * \param [in]  sub_id  Offset of the label within the MLPS base entry.
 * \param [in]  lbl_idx Index of the label in the label array.
 * \param [out] hw_fid  HW table field.
 */
static void
bcm56960_a0_tnl_mpls_lt_fid_to_hw_fid(uint32_t lt_fid,
                                      uint32_t sub_id,
                                      uint32_t lbl_idx,
                                      uint32_t *hw_fid)
{
    uint32_t arr_idx = 0;

    /* Get the array index from offset and label index. */
    arr_idx = (sub_id + lbl_idx);

    switch (lt_fid) {
    case TNL_MPLS_ENCAPt_LABELf:
        *hw_fid = bcm56960_a0_tnl_mpls_label_fid_arr[arr_idx];
        break;
    case TNL_MPLS_ENCAPt_LABEL_TTLf:
        *hw_fid = bcm56960_a0_tnl_mpls_ttl_fid_arr[arr_idx];
        break;
    case TNL_MPLS_ENCAPt_MPLS_TNL_EXP_MODEf:
        *hw_fid = bcm56960_a0_tnl_mpls_exp_sel_fid_arr[arr_idx];
        break;
    case TNL_MPLS_ENCAPt_PHB_EGR_MPLS_IDf:
        *hw_fid = bcm56960_a0_tnl_mpls_exp_ptr_fid_arr[arr_idx];
        break;
    case TNL_MPLS_ENCAPt_LABEL_EXPf:
        *hw_fid = bcm56960_a0_tnl_mpls_exp_fid_arr[arr_idx];
        break;
    case TNL_MPLS_ENCAPt_PRIf:
        *hw_fid = bcm56960_a0_tnl_mpls_pri_fid_arr[arr_idx];
        break;
    case TNL_MPLS_ENCAPt_CFIf:
        *hw_fid = bcm56960_a0_tnl_mpls_cfi_fid_arr[arr_idx];
        break;
    default:
        break;
    }
}

/*******************************************************************************
 * Public functions
 */

/*
 * \brief Populate LT fields from HW entry.
 *
 * Construct the LT field list from the HW entry for
 * TNL_IPV4_ENCAP LT.
 *
 * \param [in] unit      Unit number.
 * \param [in] hw_sid    HW table id.
 * \param [in] tnl_id    Tunnel ID.
 * \param [in] entry_buf HW entry data.
 * \param [out] out      Output field list.
 */
int
bcm56960_a0_tnl_ipv4_encap_lt_entry_populate(int unit,
                                             uint32_t hw_sid,
                                             uint32_t tnl_id,
                                             uint8_t *entry_buf,
                                             bcmltd_fields_t *out)
{
    size_t   num_fid = 0, count = 0;
    uint32_t idx = 0, hw_fid = 0;
    uint32_t fid = 0, table_sz = 0;
    uint64_t data = 0;
    bcmlrd_fid_t   *fid_list = NULL;
    uint32_t fbuf[2] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TNL_IPV4_ENCAPt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    /* Get the list of valid fields for this LT. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               TNL_IPV4_ENCAPt,
                               num_fid,
                               fid_list,
                               &count));

    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        out->field[idx]->id = fid;

        fbuf[0] = fbuf[1] = 0;
        if (fid == TNL_IPV4_ENCAPt_TNL_ENCAP_IDf) {
            out->field[idx]->data = tnl_id;
        } else {
            /* Get fields from HW buffer and populate out list. */
            bcm56960_a0_tnl_ipv4_lt_fid_to_hw_fid(fid, &hw_fid);
            bcmdrd_pt_field_get(unit,
                                hw_sid,
                                (uint32_t *)entry_buf,
                                hw_fid,
                                fbuf);
            BCMTNL_ENCAP_UINT32_TO_UINT64(fbuf, data);
            out->field[idx]->data = data;
        }
    }
    out->count = idx;

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*
 * \brief Prepare HW entry from LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_IPV4_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
int
bcm56960_a0_tnl_ipv4_encap_hw_entry_prepare(int unit,
                                            uint32_t hw_sid,
                                            const bcmltd_fields_t *in,
                                            uint8_t *entry_buf)
{
    int      idx = 0, count = 0;
    uint32_t hw_fid = 0;
    uint32_t fid = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};

    SHR_FUNC_ENTER(unit);

    count = in->count;
    /* Scan input LT field list. */
    for (idx = 0; idx < count; idx++) {
        fid = in->field[idx]->id;
        data = in->field[idx]->data;

        if (fid == TNL_IPV4_ENCAPt_TNL_ENCAP_IDf) {
            /* Skip SW only fields. */
            continue;
        } else {
            /* Convert LT field into corresponding HW field. */
            bcm56960_a0_tnl_ipv4_lt_fid_to_hw_fid(fid, &hw_fid);

            BCMTNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
            bcmdrd_pt_field_set(unit,
                                hw_sid,
                                (uint32_t *)entry_buf,
                                hw_fid,
                                fbuf);
        }
    }

    /* Set entry type to IPV4. */
    data = TNL_TYPE_IPV4;

    BCMTNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit,
                        hw_sid,
                        (uint32_t *)entry_buf,
                        ENTRY_TYPEf,
                        fbuf);

    SHR_FUNC_EXIT();
}

/*
 * \brief Populate LT fields from HW entry.
 *
 * Construct the LT field list from the HW entry for
 * TNL_IPV6_ENCAP LT.
 *
 * \param [in] unit      Unit number.
 * \param [in] hw_sid    HW table id.
 * \param [in] tnl_id    Tunnel ID.
 * \param [in] entry_buf HW entry data.
 * \param [out] out      Output field list.
 */
int
bcm56960_a0_tnl_ipv6_encap_lt_entry_populate(int unit,
                                             uint32_t hw_sid,
                                             uint32_t tnl_id,
                                             uint8_t *entry_buf,
                                             bcmltd_fields_t *out)
{
    size_t   num_fid = 0, count = 0;
    uint32_t idx = 0, hw_fid = 0;
    uint32_t s_bit = 0, e_bit = 0;
    uint32_t fid = 0, table_sz = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};
    bool     partial = false;
    bcmlrd_fid_t   *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TNL_IPV6_ENCAPt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    /* Get the list of valid fields for this LT. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               TNL_IPV6_ENCAPt,
                               num_fid,
                               fid_list,
                               &count));

    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        out->field[idx]->id = fid;

        fbuf[0] = fbuf[1] = 0;
        if (fid == TNL_IPV6_ENCAPt_TNL_ENCAP_IDf) {
            out->field[idx]->data = tnl_id;
        } else {
            /* Get fields from HW buffer and populate out list. */
            bcm56960_a0_tnl_ipv6_lt_fid_to_hw_fid(fid,
                                                  &partial,
                                                  &hw_fid,
                                                  &s_bit,
                                                  &e_bit);

            if (partial == true) {
                bcmdrd_field_get((uint32_t *)entry_buf,
                                 s_bit,
                                 e_bit,
                                 fbuf);
            } else {
                bcmdrd_pt_field_get(unit,
                                    hw_sid,
                                    (uint32_t *)entry_buf,
                                    hw_fid,
                                    fbuf);
            }
            BCMTNL_ENCAP_UINT32_TO_UINT64(fbuf, data);
            out->field[idx]->data = data;
        }
    }
    out->count = idx;

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*
 * \brief Prepare HW entry from LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_IPV6_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
int
bcm56960_a0_tnl_ipv6_encap_hw_entry_prepare(int unit,
                                            uint32_t hw_sid,
                                            const bcmltd_fields_t *in,
                                            uint8_t *entry_buf)
{
    int      idx = 0, count = 0;
    uint32_t hw_fid = 0;
    uint32_t s_bit = 0, e_bit = 0;
    uint32_t fid = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};
    bool     partial = false;

    SHR_FUNC_ENTER(unit);

    count = in->count;
    /* Scan input LT field list. */
    for (idx = 0; idx < count; idx++) {
        fid = in->field[idx]->id;
        data = in->field[idx]->data;

        if (fid == TNL_IPV6_ENCAPt_TNL_ENCAP_IDf) {
            /* Skip SW only fields. */
            continue;
        } else {
            /* Convert LT field into corresponding HW field. */
            bcm56960_a0_tnl_ipv6_lt_fid_to_hw_fid(fid,
                                                  &partial,
                                                  &hw_fid,
                                                  &s_bit,
                                                  &e_bit);
            BCMTNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
            if (partial == true) {
                bcmdrd_field_set((uint32_t *)entry_buf,
                                 s_bit,
                                 e_bit,
                                 fbuf);
            } else {
                bcmdrd_pt_field_set(unit,
                                    hw_sid,
                                    (uint32_t *)entry_buf,
                                    hw_fid,
                                    fbuf);
            }
        }
    }

    /* Set entry type to IPv6. */
    data = TNL_TYPE_IPV6;
    BCMTNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit,
                        hw_sid,
                        (uint32_t *)entry_buf,
                        ENTRY_TYPEf,
                        fbuf);

    SHR_FUNC_EXIT();
}

/*
 * \brief Populate LT fields from HW entry.
 *
 * Construct the LT field list from the HW entry for
 * TNL_MPLS_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] mpls_tnl_id  Tunnel ID.
 * \param [in] entry        MPLS tunnel sw entry.
 * \param [in] entry_buf    HW entry data.
 * \param [out] out         Output field list.
 */
int
bcm56960_a0_tnl_mpls_encap_lt_entry_populate(int unit,
                                             uint32_t hw_sid,
                                             uint32_t mpls_tnl_id,
                                             bcmtnl_mpls_encap_entry_t *entry,
                                             uint8_t *entry_buf,
                                             bcmltd_fields_t *out)
{
    size_t   num_fid = 0, count = 0;
    uint32_t idx = 0, hw_fid = 0, sub_id = 0;
    uint32_t fid = 0, table_sz = 0, out_idx = 0;
    int      max_labels = 0, lbl_idx = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};
    bcmlrd_fid_t   *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TNL_MPLS_ENCAPt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    /* Get the list of valid fields for this LT. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               TNL_MPLS_ENCAPt,
                               num_fid,
                               fid_list,
                               &count));

    max_labels = entry->max_labels;
    sub_id = (mpls_tnl_id % BCM56960_A0_MAX_LABELS);

    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        out->field[out_idx]->id = fid;

        fbuf[0] = fbuf[1] = 0;
        /* Get SW fields from SW state. */
        if (fid == TNL_MPLS_ENCAPt_TNL_ENCAP_IDf) {
            out->field[out_idx]->data = mpls_tnl_id;
            out_idx++;
        } else if (fid == TNL_MPLS_ENCAPt_MAX_LABELSf) {
            out->field[out_idx]->data = entry->max_labels;
            out_idx++;
        } else if (fid == TNL_MPLS_ENCAPt_NUM_LABELSf) {
            out->field[out_idx]->data = entry->num_labels;
            out_idx++;
        } else {
            /* Get fields from HW buffer and populate out list. */
            for (lbl_idx = 0; lbl_idx < max_labels; lbl_idx++) {
                bcm56960_a0_tnl_mpls_lt_fid_to_hw_fid(fid,
                                                      sub_id,
                                                      lbl_idx,
                                                      &hw_fid);
                bcmdrd_pt_field_get(unit,
                                    hw_sid,
                                    (uint32_t *)entry_buf,
                                    hw_fid,
                                    fbuf);
                out->field[out_idx]->id = fid;
                out->field[out_idx]->idx = lbl_idx;

                BCMTNL_ENCAP_UINT32_TO_UINT64(fbuf, data);
                out->field[out_idx]->data = data;
                out_idx++;
            }
        }
    }
    out->count = out_idx;

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*
 * \brief Prepare HW entry from LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_MPLS_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] sub_id       Offset within the MPLS base entry.
 * \param [in] entry        MPLS tunnel sw entry.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
int
bcm56960_a0_tnl_mpls_encap_hw_entry_prepare(int unit,
                                            uint32_t hw_sid,
                                            uint32_t sub_id,
                                            bcmtnl_mpls_encap_entry_t *entry,
                                            const bcmltd_fields_t *in,
                                            uint8_t *entry_buf)
{
    int      num_labels = 0, count = 0;
    int      lbl_idx = 0, idx = 0;
    uint32_t fid = 0, hw_fid = 0, arr_idx = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};

    SHR_FUNC_ENTER(unit);

    count = in->count;
    num_labels = entry->num_labels;

    /* Scan input LT field list. */
    for (idx = 0; idx < count; idx++) {
        fid = in->field[idx]->id;
        lbl_idx = in->field[idx]->idx;
        data = in->field[idx]->data;

        if ((fid == TNL_MPLS_ENCAPt_TNL_ENCAP_IDf) ||
            (fid == TNL_MPLS_ENCAPt_MAX_LABELSf) ||
            (fid == TNL_MPLS_ENCAPt_NUM_LABELSf)) {
            /* Skip SW only fields. */
            continue;
        } else {
            /* Convert LT field into corresponding HW field. */
            bcm56960_a0_tnl_mpls_lt_fid_to_hw_fid(fid,
                                                  sub_id,
                                                  lbl_idx,
                                                  &hw_fid);

            BCMTNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
            bcmdrd_pt_field_set(unit,
                                hw_sid,
                                (uint32_t *)entry_buf,
                                hw_fid,
                                fbuf);
        }
    }

    /* Program push action for each label in the entry. */
    for (lbl_idx = 0; lbl_idx < num_labels; lbl_idx++) {
        arr_idx = (sub_id + lbl_idx);
        hw_fid = bcm56960_a0_tnl_encap_mpls_push_fid_arr[arr_idx];

        /* Push this label. */
        data = BCMTNL_MPLS_PUSH_ONE;

        /*
         * If this label is not the last label of this mpls entry,
         * mark action as PUSH_NEXT, push this label and next label.
         */
        if ((lbl_idx + 1) < num_labels) {
            data = BCMTNL_MPLS_PUSH_TWO;
        }

        BCMTNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
        bcmdrd_pt_field_set(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            hw_fid,
                            fbuf);
    }

    /* Set entry type to MPLS. */
    data = TNL_TYPE_MPLS;
    BCMTNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit,
                        hw_sid,
                        (uint32_t *)entry_buf,
                        ENTRY_TYPEf,
                        fbuf);

    SHR_FUNC_EXIT();
}

/*
 * \brief Clear MPLS HW entry.
 *
 * Clear the HW entry corresponding to the TNL_MPLS_ENCAP LT
 * entry that is being deleted.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] sub_id       Offset within the MPLS base entry.
 * \param [in] entry        MPLS tunnel sw entry.
 * \param [in] entry_remove Flag indicating if entire MPLS base
 *                          tunnel entry has to be cleared.
 * \param [out] entry_buf   HW entry data.
 */
int
bcm56960_a0_tnl_mpls_encap_hw_entry_clear(int unit,
                                          uint32_t hw_sid,
                                          uint32_t sub_id,
                                          bcmtnl_mpls_encap_entry_t *entry,
                                          bool entry_remove,
                                          uint8_t *entry_buf)
{
    int      lbl_idx = 0, max_labels = 0;
    size_t   num_fid = 0, count = 0;
    uint32_t idx = 0, hw_fid = 0, arr_idx = 0;
    uint32_t fid = 0, table_sz = 0;
    uint32_t data = 0;
    bcmlrd_fid_t   *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TNL_MPLS_ENCAPt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    /* Get the list of valid fields for this LT. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               TNL_MPLS_ENCAPt,
                               num_fid,
                               fid_list,
                               &count));

    max_labels = entry->max_labels;
    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];

        if ((fid == TNL_MPLS_ENCAPt_TNL_ENCAP_IDf) ||
            (fid == TNL_MPLS_ENCAPt_MAX_LABELSf) ||
            (fid == TNL_MPLS_ENCAPt_NUM_LABELSf)) {
            /* Skip SW only fields. */
            continue;
        } else {
            /*
             * For each label in this entry, convert LT field
             * into corresponding HW field.
             */
            for (lbl_idx = 0; lbl_idx < max_labels; lbl_idx++) {
                bcm56960_a0_tnl_mpls_lt_fid_to_hw_fid(fid,
                                                      sub_id,
                                                      lbl_idx,
                                                      &hw_fid);
                data = 0;
                bcmdrd_pt_field_set(unit,
                                    hw_sid,
                                    (uint32_t *)entry_buf,
                                    hw_fid,
                                    &data);
            }
        }
    }

    /* Clear push action of the tunnel. */
    for (lbl_idx = 0; lbl_idx < max_labels; lbl_idx++) {
        arr_idx = (sub_id + lbl_idx);
        hw_fid = bcm56960_a0_tnl_encap_mpls_push_fid_arr[arr_idx];

        data = 0;
        bcmdrd_pt_field_set(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            hw_fid,
                            &data);
    }

    /*
     * Last MPLS entry being removed from this tunnel entry,
     * Clear tunnel entry type.
     */
    if (entry_remove == true) {
        data = 0;
        bcmdrd_pt_field_set(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            ENTRY_TYPEf,
                            &data);
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Tunnel encap driver attach
 *
 * Attach tunnel encap driver for this unit
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_tnl_encap_drv_attach (int unit)
{
    return bcmtnl_encap_drv_set(unit, &bcm56960_a0_tnl_encap_drv);
}

/*!
 * \brief Tunnel encap driver detach
 *
 * Detach tunnel encap driver from this unit
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_tnl_encap_drv_detach (int unit)
{
    return bcmtnl_encap_drv_set(unit, NULL);
}
