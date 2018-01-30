/*! \file bcmtnl_internal.c
 *
 * All Tunnel Internal Handlers
 * This file contains APIs, defines for common encap handler implementation
 * across IPv4/v6 and MPLS.
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

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <sal/sal_alloc.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmbd/bcmbd.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bsl/bsl.h>

#include <bcmtnl/bcmtnl_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMTNL_COMMON

/*******************************************************************************
 * Local definitions
 */

bcmtnl_encap_drv_t      *bcmtnl_encap_drv[BCMDRD_CONFIG_MAX_UNITS];
bcmtnl_encap_sw_state_t bcmtnl_encap_sw_state[BCMDRD_CONFIG_MAX_UNITS];
bcmtnl_encap_sw_state_t bcmtnl_encap_sw_state_ha[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if a field is present in the input list.
 *
 * Parse the input list to check if the field is present. If present,
 * return the value of the field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Input parameters entered by user.
 * \param [in]  fid             Field to be searched in the list.
 * \param [out] present         Pointer to return value (TRUE or FALSE)
 * \param [out] val             Data corresponding to the field if present.
 *
 * \retval SHR_E_NONE  OK
 */
static int
bcmtnl_entry_get_fid_present(int unit,
                             const bcmltd_fields_t *in,
                             bcmdrd_fid_t fid,
                             bool *present,
                             uint64_t *val)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    *present = false;
    *val = 0;

    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == fid) {
            *present = true;
            *val = in->field[i]->data;
            break;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if a particular MPLS tunnel entry is present.
 *
 * Verify if the MPLS tunnel entry is present and return the
 * base tunnel id and offset values.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  mpls_tnl_id     TNL_ENCAP_ID to search for.
 * \param [out] tnl_id          Base tunnel ID corresponding.
 * \param [out] sub_id          Offset within the base tunnel entry.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  Entry not found.
 */
static int
bcmtnl_mpls_encap_entry_present(int unit,
                                uint64_t mpls_tnl_id,
                                uint64_t *tnl_id,
                                uint32_t *sub_id)
{
    int         count = 0, max_label_per_tnl = 0;
    int         tnl_type;
    int         width = 0;
    uint64_t    idx = 0;
    bcmtnl_mpls_encap_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);

    tnl_type = TNL_TYPE_MPLS;
    width = BCMTNL_MPLS_WIDTH(unit);
    max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);
    entry = &BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id];

    /* Check if the entry is present, Get the ipv4 base idx. */
    idx = ((mpls_tnl_id / max_label_per_tnl) * width);
    SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                       count,
                       idx,
                       width);

    /*
     * If the base index bit is not set in the base tunnel bitmap or if
     * it is set and the tunnel type of the base tunnel is not MPLS,
     * the base tunnel is being used by another tunnel entry, return E_NOT_FOUND.
     */
    if ((count != width) ||
        (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[idx] != tnl_type)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Check the mpls tunnel bitmap to see if the bit corresponding to
     * the mpls tunnel ID is set.
     */
    SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                       count,
                       mpls_tnl_id,
                       entry->max_labels);
    if (count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Return the base tunnel ID and offset within the base tunnel. */
    *tnl_id = (mpls_tnl_id / max_label_per_tnl);
    *sub_id = (mpls_tnl_id % max_label_per_tnl);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if a particular IP tunnel entry is present.
 *
 * Verify if the IPv4 or IPv6 tunnel entry is present.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID(TNL_IPV4_ENCAP or TNL_IPV6_ENCAP).
 * \param [in]  tnl_id          TNL_ENCAP_ID to search for.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  Entry not found.
 */
static int
bcmtnl_ip_encap_entry_present(int unit,
                              bcmltd_sid_t sid,
                              uint64_t tnl_id)
{
    int         count = 0, width = 0;
    uint8_t     tnl_type = 0;
    uint64_t    idx = 0;

    SHR_FUNC_ENTER(unit);

    switch (sid) {
    case TNL_IPV4_ENCAPt:
        width = BCMTNL_IPV4_WIDTH(unit);
        tnl_type = TNL_TYPE_IPV4;
        break;
    case TNL_IPV6_ENCAPt:
        width = BCMTNL_IPV6_WIDTH(unit);
        tnl_type = TNL_TYPE_IPV6;
        break;
    default:
        break;
    }

    /* Check if the entry is present. */
    idx = (tnl_id * width);
    SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                       count,
                       idx,
                       width);
    /*
     * If the base index bit is not set in the base tunnel bitmap or if
     * it is set and the tunnel type of the base tunnel is not MPLS,
     * the base tunnel is being used by another tunnel entry, return E_NOT_FOUND.
     */
    if ((count != width) ||
        (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[idx] != tnl_type)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the next valid MPLS tunnel ID.
 *
 * Given an MPLS tunnel ID, get the next valid or in-use MPLS tunnel
 * ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  tnl_id          MPLS tunnel ID.
 * \param [out] mpls_next_id    Next valid mpls tunnel ID.
 * \param [out] next_id         Base tunnel ID corresponding to mpls_next_id.
 * \param [out] sub_id          Offset within the base tunnel for mpls_next_id.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  No more valid MPLS tunnels found.
 */
static int
bcmtnl_mpls_encap_get_next_index(int unit,
                                 uint64_t tnl_id,
                                 uint64_t *mpls_next_id,
                                 uint64_t *next_id,
                                 uint32_t *sub_id)
{
    int     num_mpls_tnl = 0, count = 0, idx = 0, max_labels = 0;
    int     start_idx = 0, max_label_per_tnl = 0, width = 0;
    int     base_idx = 0, tnl_type;
    bcmtnl_mpls_encap_entry_t   *entry = NULL;

    SHR_FUNC_ENTER(unit);

    tnl_type = TNL_TYPE_MPLS;
    width = BCMTNL_MPLS_WIDTH(unit);
    start_idx = BCMTNL_MPLS_START_IDX(unit);
    max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);
    num_mpls_tnl = (BCMTNL_NUM_MPLS(unit));

    if (tnl_id == 0) {
        /* If input tunnel ID is 0, start from the first valid MPLS tunnel ID. */
        idx = ((start_idx * max_label_per_tnl) / width);
    } else {
        /* If input tunnel ID is non-zero, get the next possible MPLS tunnel ID. */
        entry = &(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[tnl_id]);
        max_labels = entry->max_labels;
        idx = (tnl_id + max_labels);
    }

    /*
     * Starting from the first/next possible MPLS tunnel ID, go through MPLS
     * tunnel bitmap to find the next in-use MPLS tunnel ID.
     */
    for (; idx < num_mpls_tnl; (idx = idx + max_labels)) {
        entry = &(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[idx]);
        max_labels = entry->max_labels;
        if (max_labels != 0) {
            SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                               count,
                               idx,
                               max_labels);
            base_idx = ((idx / max_label_per_tnl) * width);
            /*
             * If MPLS tunnel bitmap count is equal to max_labels and if the base tunnel
             * entry type is MPLS, return this ID.
             */
            if (count == max_labels &&
                BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[base_idx] ==
                tnl_type) {
                *next_id = (idx / max_label_per_tnl);
                *sub_id = (idx % max_label_per_tnl);
                *mpls_next_id = idx;
                break;
            } else {
                /*
                 * Not a valid MPLS entry, continue.
                 */
                continue;
            }
        } else {
            /*
             * No mpls entry corresponding to this MPLS tunnel ID.
             * Set max_labels to 1 to increment the loop counter.
             */
            max_labels = 1;
            continue;
        }
    }

    if (idx == num_mpls_tnl) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the next valid IP tunnel ID.
 *
 * Given an IP tunnel ID, get the next valid or in-use IP tunnel
 * ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID
 * \param [in]  tnl_id          IPv4 OR IPv6 tunnel ID.
 * \param [out] next_id         Next valie IP tunnel ID.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  No more valid IP tunnels found.
 */
static int
bcmtnl_ip_encap_get_next_index(int unit,
                               uint32_t sid,
                               uint64_t tnl_id,
                               uint64_t *next_id)
{
    int     num_tnl = 0, count = 0, idx = 0;
    int     start_idx = 0, width = 0, tnl_type = 0;

    SHR_FUNC_ENTER(unit);

    num_tnl = BCMTNL_NUM_IPV4(unit);
    switch (sid) {
    case TNL_IPV4_ENCAPt:
        tnl_type = TNL_TYPE_IPV4;
        width = BCMTNL_IPV4_WIDTH(unit);
        start_idx = BCMTNL_IPV4_START_IDX(unit);
        break;
    case TNL_IPV6_ENCAPt:
        tnl_type = TNL_TYPE_IPV6;
        width = BCMTNL_IPV6_WIDTH(unit);
        start_idx = BCMTNL_IPV6_START_IDX(unit);
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (tnl_id == 0) {
        idx = start_idx;
    } else {
        tnl_id++;
        idx = (tnl_id * width);
    }

    for (; idx < num_tnl; (idx = idx + width)) {
        SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                           count,
                           idx,
                           width);
        if (count == width &&
            BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[idx] == tnl_type) {
            *next_id = (idx / width);
            break;
        } else {
            continue;
        }
    }

    if (idx == num_tnl) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse MPLS tunnel entry.
 *
 * Parse the input list to retrieve the MPLS parameters
 * which need to be maintained in software.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Input parameters list.
 * \param [in]  entry           MPLS tunnel entry s/w state.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_UNAVAIL    If unit. LT or fields are not valid.
 */
static int
bcmtnl_mpls_encap_entry_parse(int unit,
                              const bcmltd_fields_t *in,
                              bcmtnl_mpls_encap_entry_t *entry)
{
    uint32_t i = 0;
    uint32_t max_label_fid = TNL_MPLS_ENCAPt_MAX_LABELSf;
    uint32_t num_label_fid = TNL_MPLS_ENCAPt_NUM_LABELSf;
    bcmlrd_field_def_t field_def;

    SHR_FUNC_ENTER(unit);

    /* If num_labels is equal to zero, reset to default value. */
    if (entry->num_labels == 0) {
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        TNL_MPLS_ENCAPt,
                                        num_label_fid,
                                        &field_def));
        entry->num_labels = BCMTNL_FIELD_DEF(field_def);
    }

    /* If max_labels is equal to zero, reset to default value. */
    if (entry->max_labels == 0) {
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        TNL_MPLS_ENCAPt,
                                        max_label_fid,
                                        &field_def));
        entry->max_labels = BCMTNL_FIELD_DEF(field_def);
    }

    /* Overwrite with user provided values. */
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == num_label_fid) {
            entry->num_labels = in->field[i]->data;
        }
        if (in->field[i]->id == max_label_fid) {
            entry->max_labels = in->field[i]->data;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a free index for MPLS tunnel
 *
 * Allocate an entry from the base tunnel and return the
 * index as MPLS tunnel ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  max_labels      Maximum number of labels that
 *                              this MPLS tunnel can have.
 * \param [out] mpls_tnl_id     Return value of allocated MPLS tunnel ID.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_FULL       Tunnel hardware table full.
 */
static int
bcmtnl_mpls_encap_alloc_free_index(int unit,
                                   int max_labels,
                                   uint64_t *mpls_tnl_id)
{
    int         num_tnl = 0, count = 0;
    int         max_label_per_tnl = 0;
    int         start_idx, width, next_idx = 0;
    int         idx = 0, sub_idx = 0, base_idx = 0;
    bool        found = false;

    SHR_FUNC_ENTER(unit);

    width = BCMTNL_MPLS_WIDTH(unit);
    num_tnl = BCMTNL_NUM_IPV4(unit);
    start_idx = BCMTNL_MPLS_START_IDX(unit);
    max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);

    /* Get an unused base tnl index from the base tnl bitmap. */
    for (idx = start_idx; idx < num_tnl; (idx = idx + width)) {
        SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                           count,
                           idx,
                           width);
        if (count == 0) {
            /* Tunnel entry is free, use for MPLS. */
            *mpls_tnl_id = (((uint64_t)idx * max_label_per_tnl) / width);
            break;
        } else {
            /*
             * If this index is used, check if it is an MPLS tunnel and if there
             * are free entries.
             */
            if (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[idx] ==
                TNL_TYPE_MPLS) {
                /*
                 * Mpls tunnel, check if the sub entry is free and can hold
                 * max labels.
                 */
                base_idx = ((idx * max_label_per_tnl) / width);
                SHR_BITCOUNT_RANGE(
                    BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                    count,
                    base_idx,
                    max_label_per_tnl);
                if (count < max_label_per_tnl) {
                    /*
                     * There is some space here.
                     * Start from the first offset to look for free space.
                     */
                    for (sub_idx = 0; sub_idx < max_label_per_tnl; sub_idx++) {
                        next_idx = (base_idx + sub_idx);
                        SHR_BITCOUNT_RANGE(
                            BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                            count,
                            next_idx,
                            max_labels);
                        if ((count != 0) ||
                            ((sub_idx + max_labels) > max_label_per_tnl)) {
                            /* Not enough space or goes beyond one entry. */
                            continue;
                        } else {
                            if (max_labels > 2) {
                                /*
                                 * To represent more than two labels, last two
                                 * bits must be free. 3 and 4 label entries can
                                 * start at offsets 0 or 4.
                                 */
                                if (!(sub_idx & ~(0x3))) {
                                    continue;
                                }

                                if (max_labels > 7) {
                                    /*
                                     * To accommodate more than four labels, last three
                                     * bits must be free. 5, 6, 7 and 8 label entries can
                                     * start only at offset 0.
                                     */
                                    if (!(sub_idx & ~(0x7))) {
                                        continue;
                                    }
                                }
                            }
                            /* Found a suitable index. */
                            found = true;
                            *mpls_tnl_id = next_idx;
                            break;
                        }
                    }
                }
            }
        }
        /* Break away from the loop if index is found. */
        if (found == true) {
            break;
        }
    }

    if (idx == num_tnl) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a free index for MPLS tunnel.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  with_id         TRUE if user has specified Tunnel ID.
 * \param [in]  max_labels      Maximum number of labels that
 *                              this MPLS tunnel can have.
 * \param [in]  mpls_tnl_id     MPLS tunnel ID specified by user or
 *                              MPLS tunnel ID allocated by this function.
 * \param [out]  tnl_id         Base tunnel ID corresponding to mpls_tnl_id.
 * \param [out]  sub_id         Offset within the base tunnel for mpls_tnl_id.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_FULL       Tunnel hardware table full (with_id == false).
 * \retval SHR_E_EXISTS     User provided ID not free (with_id == true).
 */
static int
bcmtnl_mpls_encap_get_free_index(int unit,
                                 bool with_id,
                                 int max_labels,
                                 uint64_t *mpls_tnl_id,
                                 uint64_t *tnl_id,
                                 uint32_t *sub_id)
{
    int         count = 0, max_label_per_tnl = 1;
    uint32_t    width;
    uint64_t    idx = 0;

    SHR_FUNC_ENTER(unit);

    width   = BCMTNL_MPLS_WIDTH(unit);
    max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);

    if (with_id == true) {
        /*
         * Check if the user provided index is free.
         */
        idx = ((*mpls_tnl_id / max_label_per_tnl) * width);
        SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                           count,
                           idx,
                           width);
        /*
         * If base index is being used and if it is an MPLS tunnel,
         * do further check to see if the MPLS tunnel index is free.
         */
        if (count != 0) {
            if (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[idx] !=
                TNL_TYPE_MPLS) {
                /* Not an mpls tunnel, entire tunnel entry is used, bad id. */
                SHR_RETURN_VAL_EXIT(SHR_E_BADID);
            } else {
                /* Mpls tunnel, check if the sub entry is free. */
                SHR_BITCOUNT_RANGE(
                    BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                    count,
                    *mpls_tnl_id,
                    max_labels);
                if (count != 0) {
                    /* Sub entry is not free. */
                    SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
                }
            }
        }

        *tnl_id = (*mpls_tnl_id / max_label_per_tnl);
        *sub_id = (*mpls_tnl_id % max_label_per_tnl);
    } else {
        /*
         * User has not specified a tunnel ID.
         * Get a free index from scanning the used bitmap.
         */
        SHR_IF_ERR_EXIT(
            bcmtnl_mpls_encap_alloc_free_index(unit,
                                               max_labels,
                                               mpls_tnl_id));
        if (*mpls_tnl_id == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        }

        *tnl_id = (*mpls_tnl_id / max_label_per_tnl);
        *sub_id = (*mpls_tnl_id % max_label_per_tnl);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a free index for IP tunnel.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  with_id         TRUE if user has specified Tunnel ID.
 * \param [in]  tnl_id          Tunnel ID specified by user if with_id == true
 *                              Allocate tnl_id if with_id == false
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_FULL       Tunnel hardware table full (with_id == false).
 * \retval SHR_E_EXISTS     User provided ID not free (with_id == true).
 */
static int
bcmtnl_ip_encap_get_free_index(int unit,
                               bcmltd_sid_t sid,
                               bool with_id,
                               uint64_t *tnl_id)
{
    int         num_tnl = 0, count = 0;
    int         start_idx, width;
    int         idx = 0;

    SHR_FUNC_ENTER(unit);

    num_tnl = BCMTNL_NUM_IPV4(unit);
    switch (sid) {
    case TNL_IPV4_ENCAPt:
        width = BCMTNL_IPV4_WIDTH(unit);
        start_idx = BCMTNL_IPV4_START_IDX(unit);
        break;
    case TNL_IPV6_ENCAPt:
        width = BCMTNL_IPV6_WIDTH(unit);
        start_idx = BCMTNL_IPV6_START_IDX(unit);
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (with_id == true) {
        /* Check if the user provided index is free. */
        idx = (*tnl_id * width);
        SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                           count,
                           idx,
                           width);
        if (count != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
        }
    } else {
        /*
         * Get a free index.
         * Single-wide for IPv4 and double-wide for IPv6.
         */
        for (idx = start_idx; idx < num_tnl; (idx = idx + width)) {
            SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                               count,
                               idx,
                               width);
            if (count == 0) {
                *tnl_id = (idx / width);
                break;
            }
        }

        if (idx == num_tnl) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read an entry from Tunnel hardware table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  req_ltid        Logical table ID.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_id          Tunnel table hardware index to read from.
 * \param [out] entry_buf       Pointer to read buffer.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  Hardware table not found.
 * \retval SHR_E_PARAM      Index is wrong..
 */
static int
bcmtnl_encap_hw_entry_read(int unit,
                           uint32_t trans_id,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t hw_sid,
                           uint32_t tnl_id,
                           uint8_t *entry_buf)
{
    uint32_t    rsp_flags;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = tnl_id;
    pt_dyn_info.tbl_inst = 0;

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, hw_sid, &sinfo));

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    hw_sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL,
                                    BCMPTM_OP_READ,
                                    0,
                                    sinfo.entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write an entry into Tunnel hardware table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  req_ltid        Logical table ID.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_id          Tunnel table hardware index to read from.
 * \param [in]  entry_buf       Pointer to write buffer.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  Hardware table not found.
 * \retval SHR_E_PARAM      Index is wrong..
 */
static int
bcmtnl_encap_hw_entry_write(int unit,
                            uint32_t trans_id,
                            bcmltd_sid_t req_ltid,
                            bcmdrd_sid_t hw_sid,
                            uint32_t tnl_id,
                            uint8_t *entry_buf)
{
    uint32_t    rsp_flags;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = tnl_id;
    pt_dyn_info.tbl_inst = 0;

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, hw_sid, &sinfo));

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    hw_sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL,
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)entry_buf,
                                    0,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear MPLS tunnel entry.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  mpls_tnl_id     MPLS tunnel ID.
 * \param [in]  entry_buf       Pointer to tunnel entry buffer.
 *
 * \retval SHR_E_NONE       OK.
 */
static int
bcmtnl_mpls_encap_hw_entry_clear(int unit,
                                 uint32_t hw_sid,
                                 uint64_t mpls_tnl_id,
                                 uint8_t *entry_buf)
{
    uint32_t    base_idx = 0, sub_id = 0;
    int         max_label_per_tnl = 0, count = 0;
    bool        entry_remove = false;
    bcmtnl_mpls_encap_entry_t   *entry = NULL;

    SHR_FUNC_ENTER(unit);

    max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);
    sub_id = (mpls_tnl_id % max_label_per_tnl);
    entry = &(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id]);

    /* Get the offset into the first entry of this tunnel. */
    base_idx = ((mpls_tnl_id / max_label_per_tnl) * max_label_per_tnl);

    /*
     * Count the number of bits in this MPLS tunnel hardware entry.
     */
    SHR_BITCOUNT_RANGE(
        BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
        count,
        base_idx,
        max_label_per_tnl);

    /*
     * Last MPLS entry being removed from this tunnel entry,
     * Clear tunnel type.
     */
    if (count <= entry->max_labels) {
        entry_remove = true;
    }

    SHR_IF_ERR_EXIT(BCMTNL_MPLS_HW_ENTRY_CLEAR(unit)(unit,
                                                     hw_sid,
                                                     sub_id,
                                                     entry,
                                                     entry_remove,
                                                     entry_buf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Populate the Tunnel LT entry from HW fields.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_type        Tunnel entry type.
 * \param [in]  entry_buf       Pointer to hardware entry.
 * \param [in]  tnl_id          Tunnel ID.
 * \param [in]  sub_id          Offset within the base tunnel.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_UNAVAIL    Logical table not valid for this unit.
 */
static int
bcmtnl_encap_lt_entry_populate(int unit,
                               uint32_t hw_sid,
                               int tnl_type,
                               uint8_t *entry_buf,
                               uint64_t tnl_id,
                               uint32_t sub_id,
                               bcmltd_fields_t *out)
{
    int      max_label_per_tnl = 0;
    uint32_t mpls_tnl_id = 0;
    bcmtnl_mpls_encap_entry_t   *entry = NULL;

    SHR_FUNC_ENTER(unit);

    /* Convert HW entry to LT entry. */
    switch (tnl_type) {
    case TNL_TYPE_IPV4:
        SHR_IF_ERR_EXIT(BCMTNL_IPV4_HW_TO_LT_ENTRY(unit)(unit,
                                                         hw_sid,
                                                         tnl_id,
                                                         entry_buf,
                                                         out));
        break;
    case TNL_TYPE_IPV6:
        SHR_IF_ERR_EXIT(BCMTNL_IPV6_HW_TO_LT_ENTRY(unit)(unit,
                                                         hw_sid,
                                                         tnl_id,
                                                         entry_buf,
                                                         out));
        break;
    case TNL_TYPE_MPLS:
        max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);
        mpls_tnl_id = ((tnl_id * max_label_per_tnl) + sub_id);
        entry = &(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id]);
        SHR_IF_ERR_EXIT(BCMTNL_MPLS_HW_TO_LT_ENTRY(unit)(unit,
                                                         hw_sid,
                                                         mpls_tnl_id,
                                                         entry,
                                                         entry_buf,
                                                         out));
        break;
    default:
        break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Prepare the Tunnel hardware entry from LT fields.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_type        Tunnel entry type.
 * \param [in]  sub_id          Offset within the base tunnel.
 * \param [in]  entry           MPLS entry s/w state.
 * \param [in]  in              Input parameter list.
 * \param [out] entry_buf       Pointer to populated hardware entry buffer.
 *
 * \retval SHR_E_NONE       OK.
 */
static int
bcmtnl_encap_hw_entry_prepare(int unit,
                              uint32_t hw_sid,
                              int tnl_type,
                              uint32_t sub_id,
                              bcmtnl_mpls_encap_entry_t *entry,
                              const bcmltd_fields_t *in,
                              uint8_t *entry_buf)
{
    SHR_FUNC_ENTER(unit);

    /* Convert LT entry to HW entry. */
    switch (tnl_type) {
    case TNL_TYPE_IPV4:
        SHR_IF_ERR_EXIT(BCMTNL_IPV4_LT_TO_HW_ENTRY(unit)(unit,
                                                         hw_sid,
                                                         in,
                                                         entry_buf));
        break;
    case TNL_TYPE_IPV6:
        SHR_IF_ERR_EXIT(BCMTNL_IPV6_LT_TO_HW_ENTRY(unit)(unit,
                                                         hw_sid,
                                                         in,
                                                         entry_buf));
        break;
    case TNL_TYPE_MPLS:
        SHR_IF_ERR_EXIT(BCMTNL_MPLS_LT_TO_HW_ENTRY(unit)(unit,
                                                         hw_sid,
                                                         sub_id,
                                                         entry,
                                                         in,
                                                         entry_buf));
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
  Public functions
 */
/*!
 * \fn bcmtnl_entry_validate
 *
 * \brief Internal validation of all field values entered by user.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          LT operation code.
 * \param [in]  sid             Logical Table ID.
 * \param [in]  in              Input parameters entered by user.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_entry_validate(int unit,
                      bcmlt_opcode_t opcode,
                      bcmltd_sid_t sid,
                      const bcmltd_fields_t *in)
{
    uint32_t    i = 0, max_label_per_tnl = 0;
    uint32_t    key_fid = 0, fid = 0, sub_id = 0;
    uint64_t    data = 0, mpls_tnl_id = 0;
    bool        key_present = false;
    bcmlrd_field_def_t          f_info;
    bcmtnl_mpls_encap_entry_t   entry;

    SHR_FUNC_ENTER(unit);

    switch (sid) {
    case TNL_IPV4_ENCAPt:
        key_fid = TNL_IPV4_ENCAPt_TNL_ENCAP_IDf;
        break;
    case TNL_IPV6_ENCAPt:
        key_fid = TNL_IPV6_ENCAPt_TNL_ENCAP_IDf;
        break;
    case TNL_MPLS_ENCAPt:
        key_fid = TNL_MPLS_ENCAPt_TNL_ENCAP_IDf;
        max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);
        break;
    default:
        break;
    }

    /* Validate all fields are within the min/max range. */
    while (i < in->count) {
        fid = in->field[i]->id;
        data = in->field[i]->data;

        sal_memset(&f_info, 0, sizeof(f_info));
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        sid,
                                        fid,
                                        &f_info));

        /* Look for Key field. */
        if (fid == key_fid) {
            key_present = true;
            if (sid == TNL_MPLS_ENCAPt) {
                mpls_tnl_id = data;
            }
        }

        if (f_info.access == BCMLTD_FIELD_ACCESS_READ_ONLY) {
            /* Log the field for which it failed */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        if ((data < BCMTNL_FIELD_MIN(f_info)) ||
            (data > BCMTNL_FIELD_MAX(f_info))) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        i++;
    }

    /*
     * All LT operations except insert must mandatorily specify key field.
     */
    if ((opcode != BCMLT_OPCODE_INSERT) && (key_present == false)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (sid == TNL_MPLS_ENCAPt) {
        if (opcode == BCMLT_OPCODE_UPDATE) {
            /* Retrieve the mpls sw state. */
            sal_memcpy(
                &entry,
                &BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id],
                sizeof(entry));
        } else {
            sal_memset(&entry, 0, sizeof(entry));
        }

        /* Parse and populate mpls_entry. */
        SHR_IF_ERR_EXIT(bcmtnl_mpls_encap_entry_parse(unit,
                                                      in,
                                                      &entry));
        /* Num labels cannot be greater than max labels. */
        if (entry.num_labels > entry.max_labels) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        /*
         * MAX_LABELS field in TNL_MPLS_ENCAP LT should not
         * be updated.
         */
        if (opcode == BCMLT_OPCODE_UPDATE) {
            if (entry.max_labels !=
                BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id].
                max_labels) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }

        /*
         * For insert, update operations, validate that the user provided
         * key is compatible with the number of labels.
         */
        if ((key_present == true) &&
            ((opcode == BCMLT_OPCODE_INSERT) ||
             (opcode == BCMLT_OPCODE_UPDATE))) {

            sub_id = (mpls_tnl_id % max_label_per_tnl);
            /*
             * If the index specified by user cannot hold
             * the max labels specified, return E_PARAM.
             */
            if ((sub_id + entry.max_labels) > max_label_per_tnl) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Validate the user provided id. */
            switch (entry.max_labels) {
            case 1:
            case 2:
                break;
            case 3:
            case 4:
                /* Bits 0 and 1 should be 0 for a chain of 3 to 4 labels. */
                if (sub_id & ~(0x3)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                /* Bits 0, 1 and 2 should be 0 for a chain of 5 to 8 labels. */
                if (sub_id & ~(0x7)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_entry_insert
 *
 * \brief Insert a new entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  opcode          Transaction opcode.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_entry_insert(int unit,
                    uint32_t trans_id,
                    bcmlt_opcode_t opcode,
                    uint32_t lt_sid,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out)
{
    int         idx = 0;
    uint8_t     tnl_type = 0;
    uint32_t    key_fid = 0, hw_sid = 0;
    uint64_t    tnl_id  = 0, mpls_tnl_id = 0;
    uint8_t     *entry_buf = NULL;
    uint32_t    entry_sz = 0;
    uint32_t    sub_id = 0;
    bool        with_id = false, ret_id = false;
    bcmtnl_mpls_encap_entry_t   entry;

    SHR_FUNC_ENTER(unit);

    switch (lt_sid) {
    case TNL_IPV4_ENCAPt:
        key_fid = TNL_IPV4_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_IPV4;
        hw_sid = BCMTNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        key_fid = TNL_IPV6_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_IPV6;
        hw_sid = BCMTNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        key_fid = TNL_MPLS_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_MPLS;
        hw_sid = BCMTNL_MPLS_HW_SID(unit);
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Check if key is specified and return the value. */
    SHR_IF_ERR_EXIT
        (bcmtnl_entry_get_fid_present(unit, in, key_fid, &with_id, &tnl_id));

    sal_memset(&entry, 0, sizeof(entry));
    switch (opcode) {
    case BCMLT_OPCODE_INSERT:
        /*
         * If key is specified, check if that ID is free and allocate it.
         * If key is not specified, get a free index.
         */
        if ((lt_sid == TNL_IPV4_ENCAPt) || (lt_sid == TNL_IPV6_ENCAPt)) {
            SHR_IF_ERR_EXIT
                (bcmtnl_ip_encap_get_free_index(unit,
                                                lt_sid,
                                                with_id,
                                                &tnl_id));
        } else {
            mpls_tnl_id = tnl_id;

            /* Parse and populate mpls_entry. */
            SHR_IF_ERR_EXIT(bcmtnl_mpls_encap_entry_parse(unit,
                                                          in,
                                                          &entry));

            /*
             * If key is specified, check if that ID is free and allocate it.
             * If key is not specified, get a free index.
             */
            SHR_IF_ERR_EXIT
                (bcmtnl_mpls_encap_get_free_index(unit,
                                                  with_id,
                                                  entry.max_labels,
                                                  &mpls_tnl_id,
                                                  &tnl_id,
                                                  &sub_id));
        }
        break;
    case BCMLT_OPCODE_UPDATE:
        /*
         * For update operations, check if the entry exists,
         * else return E_NOT_FOUND.
         */
        if ((lt_sid == TNL_IPV4_ENCAPt) || (lt_sid == TNL_IPV6_ENCAPt)) {
            SHR_IF_ERR_EXIT
                (bcmtnl_ip_encap_entry_present(unit,
                                               lt_sid,
                                               tnl_id));
        } else {
            /*
             * Check if the mpls entry exists.
             * If yes, return, base id and sub id.
             * mpls_tnl_id  -> mpls index (user provided)
             * tnl_id       -> base tunnel index of MPLS h/w table.
             * sub_id       -> offset within the base MPLS tunnel.
             */
            mpls_tnl_id = tnl_id;
            SHR_IF_ERR_EXIT
                (bcmtnl_mpls_encap_entry_present(unit,
                                                 mpls_tnl_id,
                                                 &tnl_id,
                                                 &sub_id));

            /*
             * Retrieve the s/w state corresponding to this mpls entry.
             */
            sal_memcpy(
                &entry,
                &BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id],
                sizeof(entry));

            /* Update based on user input. */
            SHR_IF_ERR_EXIT(bcmtnl_mpls_encap_entry_parse(unit,
                                                          in,
                                                          &entry));
        }
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Read the entry from hardware. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "tunnel encap entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcmtnl_encap_hw_entry_read(unit,
                                    trans_id,
                                    lt_sid,
                                    hw_sid,
                                    tnl_id,
                                    entry_buf));

    /* Prepare the hw entry. */
    SHR_IF_ERR_EXIT
        (bcmtnl_encap_hw_entry_prepare(unit,
                                       hw_sid,
                                       tnl_type,
                                       sub_id,
                                       &entry,
                                       in,
                                       entry_buf));

    /* Write back the entry. */
    SHR_IF_ERR_EXIT
        (bcmtnl_encap_hw_entry_write(unit,
                                     trans_id,
                                     lt_sid,
                                     hw_sid,
                                     tnl_id,
                                     entry_buf));

    if ((with_id == false) && (out != NULL)) {
        ret_id = true;
    }

    /* Mark the used tnl id in the bitmap and set tnl type. */
    switch (lt_sid) {
    case TNL_IPV4_ENCAPt:
        SHR_BITSET(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap, tnl_id);
        BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[tnl_id] = TNL_TYPE_IPV4;
        /* Return back allocated index for insert without key. */
        if (ret_id == true) {
            out->field[0]->id = TNL_IPV4_ENCAPt_TNL_ENCAP_IDf;
            out->field[0]->data = tnl_id;
            out->count = 1;
        }
        break;
    case TNL_IPV6_ENCAPt:
        SHR_BITSET_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                         (tnl_id * BCMTNL_IPV6_WIDTH(unit)),
                         BCMTNL_IPV6_WIDTH(unit));
        /* Save the tunnel type of this base tunnel. */
        for (idx = 0; idx < BCMTNL_IPV6_WIDTH(unit); idx++) {
            BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type
                [((tnl_id * BCMTNL_IPV6_WIDTH(unit)) + idx)] = TNL_TYPE_IPV6;
        }
        /* Return back allocated index for insert without key. */
        if (ret_id == true) {
            out->field[0]->id = TNL_IPV6_ENCAPt_TNL_ENCAP_IDf;
            out->field[0]->data = tnl_id;
            out->count = 1;
        }
        break;
    case TNL_MPLS_ENCAPt:
        /* Mark the base tnl index as used in base tnl bitmap. */
        SHR_BITSET_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                         (tnl_id * BCMTNL_MPLS_WIDTH(unit)),
                         BCMTNL_MPLS_WIDTH(unit));
        /* Mark the mpls index as used in the mpls tnl bitmap. */
        SHR_BITSET_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                         mpls_tnl_id, entry.max_labels);
        /* Save the mpls entry in sw state. */
        sal_memcpy(
            &BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id],
            &entry,
            sizeof(entry));

        /* Save the tunnel type of this base tunnel. */
        for (idx = 0; idx < BCMTNL_MPLS_WIDTH(unit); idx++) {
            BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type
                [((tnl_id * BCMTNL_MPLS_WIDTH(unit)) + idx)] = TNL_TYPE_MPLS;
        }
        /* Return back allocated index for insert without key. */
        if (ret_id == true) {
            out->field[0]->id = TNL_MPLS_ENCAPt_TNL_ENCAP_IDf;
            out->field[0]->data = mpls_tnl_id;
            out->count = 1;
        }
        break;
    }

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_entry_lookup
 *
 * \brief Lookup an entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_entry_lookup(int unit,
                    uint32_t trans_id,
                    uint32_t lt_sid,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out)
{
    bool        with_id = false;
    int         tnl_type = 0;
    uint32_t    key_fid = 0, hw_sid = 0, sub_id = 0;
    uint64_t    tnl_id  = 0, mpls_tnl_id = 0;
    uint8_t     *entry_buf = NULL;
    uint32_t    entry_sz = 0;

    SHR_FUNC_ENTER(unit);

    switch (lt_sid) {
    case TNL_IPV4_ENCAPt:
        key_fid = TNL_IPV4_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_IPV4;
        hw_sid = BCMTNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        key_fid = TNL_IPV6_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_IPV6;
        hw_sid = BCMTNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        key_fid = TNL_MPLS_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_MPLS;
        hw_sid = BCMTNL_MPLS_HW_SID(unit);
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Retrieve the key value from the input fields. */
    SHR_IF_ERR_EXIT
        (bcmtnl_entry_get_fid_present(unit, in, key_fid, &with_id, &tnl_id));

    /* Check if the entry is present in the table, else return E_NOT_FOUND. */
    if (lt_sid == TNL_IPV4_ENCAPt || lt_sid == TNL_IPV6_ENCAPt) {
        SHR_IF_ERR_EXIT
            (bcmtnl_ip_encap_entry_present(unit,
                                           lt_sid,
                                           tnl_id));
    } else {
        mpls_tnl_id = tnl_id;
        SHR_IF_ERR_EXIT
            (bcmtnl_mpls_encap_entry_present(unit,
                                             mpls_tnl_id,
                                             &tnl_id,
                                             &sub_id));
    }

    /* Read the entry from hardware. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "tunnel encap entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcmtnl_encap_hw_entry_read(unit,
                                    trans_id,
                                    lt_sid,
                                    hw_sid,
                                    tnl_id,
                                    entry_buf));

    /*
     * Populate the output field list by fetching the
     * fields from the harwdare entry.
     */
    SHR_IF_ERR_EXIT
        (bcmtnl_encap_lt_entry_populate(unit,
                                        hw_sid,
                                        tnl_type,
                                        entry_buf,
                                        tnl_id,
                                        sub_id,
                                        out));
exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_entry_delete
 *
 * \brief Delete an entry from the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_entry_delete(int unit,
                    uint32_t trans_id,
                    uint32_t lt_sid,
                    const bcmltd_fields_t *in)
{
    bool        with_id = false;
    int         idx = 0, max_label_per_tnl = 0;
    int         count = 0;
    uint32_t    key_fid = 0, hw_sid = 0, sub_id = 0;
    uint64_t    tnl_id  = 0, mpls_tnl_id = 0;
    uint8_t     *entry_buf = NULL;
    uint32_t    entry_sz = 0, base_idx = 0;
    bcmtnl_mpls_encap_entry_t *entry = NULL;

    SHR_FUNC_ENTER(unit);

    switch (lt_sid) {
    case TNL_IPV4_ENCAPt:
        key_fid = TNL_IPV4_ENCAPt_TNL_ENCAP_IDf;
        hw_sid = BCMTNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        key_fid = TNL_IPV6_ENCAPt_TNL_ENCAP_IDf;
        hw_sid = BCMTNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        key_fid = TNL_MPLS_ENCAPt_TNL_ENCAP_IDf;
        hw_sid = BCMTNL_MPLS_HW_SID(unit);
        max_label_per_tnl = BCMTNL_MAX_MPLS_LABELS(unit);
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Retrieve the key value from the input fields. */
    SHR_IF_ERR_EXIT
        (bcmtnl_entry_get_fid_present(unit, in, key_fid, &with_id, &tnl_id));

    /* Check if the entry is present in the table, else return E_NOT_FOUND. */
    if ((lt_sid == TNL_IPV4_ENCAPt) || (lt_sid == TNL_IPV6_ENCAPt)) {
        SHR_IF_ERR_EXIT
            (bcmtnl_ip_encap_entry_present(unit,
                                           lt_sid,
                                           tnl_id));
    } else {
        mpls_tnl_id = tnl_id;
        entry = &BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id];
        SHR_IF_ERR_EXIT
            (bcmtnl_mpls_encap_entry_present(unit,
                                             mpls_tnl_id,
                                             &tnl_id,
                                             &sub_id));
    }

    /* Allocate memory for writing to the hardware table. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "tunnel encap entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    /* Clear the entry data.*/
    sal_memset(entry_buf, 0, entry_sz);

    if (lt_sid == TNL_MPLS_ENCAPt) {
        /* For MPLS, clear the fields specific to this entry. */
        SHR_IF_ERR_EXIT
            (bcmtnl_encap_hw_entry_read(unit,
                                        trans_id,
                                        lt_sid,
                                        hw_sid,
                                        tnl_id,
                                        entry_buf));

        SHR_IF_ERR_EXIT
            (bcmtnl_mpls_encap_hw_entry_clear(unit,
                                              hw_sid,
                                              mpls_tnl_id,
                                              entry_buf));
    }

    /* Write back to the hardware table. */
    SHR_IF_ERR_EXIT
        (bcmtnl_encap_hw_entry_write(unit,
                                     trans_id,
                                     lt_sid,
                                     hw_sid,
                                     tnl_id,
                                     entry_buf));

    /* Clear the bits from the bitmap and clear tnl type. */
    switch (lt_sid) {
    case TNL_IPV4_ENCAPt:
        SHR_BITCLR(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap, tnl_id);
        BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type[tnl_id] = TNL_TYPE_NONE;
        break;
    case TNL_IPV6_ENCAPt:
        SHR_BITCLR_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                         (tnl_id * BCMTNL_IPV6_WIDTH(unit)),
                         BCMTNL_IPV6_WIDTH(unit));
        /* Clear tunnel type of all base tunnel entries used by this tunnel. */
        for (idx = 0; idx < BCMTNL_IPV6_WIDTH(unit); idx++) {
            BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type
                [((tnl_id * BCMTNL_IPV6_WIDTH(unit)) + idx)] = TNL_TYPE_NONE;
        }
        break;
    case TNL_MPLS_ENCAPt:
        /* Mark the mpls index as free in the mpls tnl bitmap. */
        SHR_BITCLR_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                         mpls_tnl_id, entry->max_labels);
        /* Clear the mpls entry in sw state. */
        sal_memset(
            &BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry[mpls_tnl_id],
            0,
            sizeof(bcmtnl_mpls_encap_entry_t));

        /* Check if all mpls entries in the base tnl have been deleted. */
        base_idx = (tnl_id * max_label_per_tnl);
        SHR_BITCOUNT_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
                           count,
                           base_idx,
                           max_label_per_tnl);
        if (!count) {
            /*
             * No more mpls entries in this base tnl.
             * Clear the bit in the base tnl bitmap.
             */
            SHR_BITCLR_RANGE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
                             (tnl_id * BCMTNL_MPLS_WIDTH(unit)),
                             BCMTNL_MPLS_WIDTH(unit));
            for (idx = 0; idx < BCMTNL_MPLS_WIDTH(unit); idx++) {
                BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type
                    [((tnl_id * BCMTNL_MPLS_WIDTH(unit)) + idx)] = TNL_TYPE_NONE;
            }
        }
        break;
    }

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_entry_traverse
 *
 * \brief Lookup an entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_entry_traverse(int unit,
                      uint32_t trans_id,
                      uint32_t lt_sid,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out)
{
    bool        with_id = false;
    int         rv = 0, tnl_type = 0;
    uint32_t    key_fid = 0, hw_sid = 0;
    uint64_t    tnl_id  = 0, next_id = 0;
    uint64_t    mpls_next_id = 0;
    uint8_t     *entry_buf = NULL;
    uint32_t    entry_sz = 0, sub_id = 0;

    SHR_FUNC_ENTER(unit);

    switch (lt_sid) {
    case TNL_IPV4_ENCAPt:
        key_fid = TNL_IPV4_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_IPV4;
        hw_sid = BCMTNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        key_fid = TNL_IPV6_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_IPV6;
        hw_sid = BCMTNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        key_fid = TNL_MPLS_ENCAPt_TNL_ENCAP_IDf;
        tnl_type = TNL_TYPE_MPLS;
        hw_sid = BCMTNL_MPLS_HW_SID(unit);
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (in == NULL) {
        /* Get first. */
        tnl_id = 0;
    } else {
        /* Get next. */
        /* Retrieve the key value from the input fields. */
        SHR_IF_ERR_EXIT
            (bcmtnl_entry_get_fid_present(unit, in, key_fid, &with_id, &tnl_id));
    }

    /*
     * Get the first used index if tnl_id == 0.
     * Else, get the next used index.
     */
    if ((lt_sid == TNL_IPV4_ENCAPt) || (lt_sid == TNL_IPV6_ENCAPt)) {
        rv = bcmtnl_ip_encap_get_next_index(unit,
                                            lt_sid,
                                            tnl_id,
                                            &next_id);
    } else {
        rv = bcmtnl_mpls_encap_get_next_index(unit,
                                              tnl_id,
                                              &mpls_next_id,
                                              &next_id,
                                              &sub_id);
    }

    if (rv == SHR_E_NOT_FOUND) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    } else if (rv != SHR_E_NONE) {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Read the entry from hardware. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "tunnel encap entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcmtnl_encap_hw_entry_read(unit,
                                    trans_id,
                                    lt_sid,
                                    hw_sid,
                                    next_id,
                                    entry_buf));

    /*
     * Populate the output field list by fetching the
     * fields from the hardware entry.
     */
    SHR_IF_ERR_EXIT
        (bcmtnl_encap_lt_entry_populate(unit,
                                        hw_sid,
                                        tnl_type,
                                        entry_buf,
                                        next_id,
                                        sub_id,
                                        out));
exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_sw_state_cleanup
 *
 * \brief Free Tunnel SW structures.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_sw_state_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap != NULL) {
        SHR_FREE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap);
    }

    if (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap != NULL) {
        SHR_FREE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap);
    }

    if (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type != NULL) {
        SHR_FREE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type);
    }

    if (BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry != NULL) {
        SHR_FREE(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_sw_state_init
 *
 * \brief Initialise Tunnel SW structures.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  warm            Cold/Warm boot.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_sw_state_init(int unit, bool warm)
{
    int num_ip_tnl, num_mpls_tnl;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    num_ip_tnl = BCMTNL_NUM_IPV4(unit);
    num_mpls_tnl = BCMTNL_NUM_MPLS(unit);

    /* SW state for maintaing IPv4/v6/MPLS database in the Encap Table */
    ha_req_size = SHR_BITALLOCSIZE(num_ip_tnl);
    ha_alloc_size = ha_req_size;
    /* Base IPv4/v6 tunnel bitmap */
    BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMTNL_BASE_TNL_BMAP_SUB_COMP_ID,
                         &ha_alloc_size);

    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap, 0,
                   ha_alloc_size);
    }

    /* SW state for maintaing temp IPv4/v6/MPLS database in the Encap Table */
    /* Base IPv4/v6 tunnel bitmap */
    SHR_ALLOC(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap, ha_alloc_size,
              "base tunnel bitmap");
    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap, SHR_E_MEMORY);
    sal_memset(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap, 0,
               ha_alloc_size);

    /* Entry 0 is unused and reserved */
    SHR_BITSET(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap, 0);

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap, ha_alloc_size);

    /* MPLS tunnel bitmap */
    ha_req_size = SHR_BITALLOCSIZE(num_mpls_tnl);
    ha_alloc_size = ha_req_size;
    BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMTNL_MPLS_TNL_BMAP_SUB_COMP_ID,
                         &ha_alloc_size);

    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (warm) {
        sal_memset(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap, 0,
                   ha_alloc_size);
    }

    /* MPLS tunnel temp bitmap */
    SHR_ALLOC(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap, ha_alloc_size,
              "mpls tunnel bitmap");
    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap, SHR_E_MEMORY);
    sal_memset(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap, 0,
               ha_alloc_size);

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap, ha_alloc_size);

    /* SW state for maintaing IPv4/v6/MPLS tunnel types in the Encap Table */
    ha_req_size = (num_ip_tnl * sizeof(uint8_t));
    ha_alloc_size = ha_req_size;
    /* Base IPv4/v6 tunnel bitmap */
    BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMTNL_BASE_TNL_TYPE_SUB_COMP_ID,
                         &ha_alloc_size);

    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type, 0,
                   ha_alloc_size);
    }

    /* SW state for maintaing temp IPv4/v6/MPLS database in the Encap Table */
    SHR_ALLOC(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type, ha_alloc_size,
              "base tunnel bitmap");
    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type, SHR_E_MEMORY);
    sal_memset(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type, 0,
               ha_alloc_size);

    /* Entry 0 is unused and reserved */
    BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type[0] = TNL_TYPE_NONE;

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type, ha_alloc_size);

    /* SW state for maintaing MPLS entries in the Encap Table */
    ha_req_size = (num_mpls_tnl * sizeof(bcmtnl_mpls_encap_entry_t));
    ha_alloc_size = ha_req_size;
    BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMTNL_MPLS_ENTRY_STATE_SUB_COMP_ID,
                         &ha_alloc_size);

    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry, 0,
                   ha_alloc_size);
    }

    /* SW state for maintaing temp MPLS entry state in the Encap Table */
    SHR_ALLOC(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry, ha_alloc_size,
              "MPLS entry state");
    SHR_NULL_CHECK(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry, SHR_E_MEMORY);
    sal_memset(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry, 0,
               ha_alloc_size);

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry,
               ha_alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        if (BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap != NULL) {
            shr_ha_mem_free(unit, BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap);
        }

        if (BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap != NULL) {
            shr_ha_mem_free(unit, BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap);
        }

        if (BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type != NULL) {
            shr_ha_mem_free(unit, BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type);
        }

        if (BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry != NULL) {
            shr_ha_mem_free(unit, BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry);
        }
        bcmtnl_sw_state_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_transaction_commit
 *
 * \brief Commit SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          Logical table ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_transaction_commit(int unit,
                          uint32_t trans_id,
                          uint32_t lt_sid)
{
    int         num_ip_tnl, num_mpls_tnl;
    uint32_t    alloc_sz = 0;

    SHR_FUNC_ENTER(unit);

    num_ip_tnl = BCMTNL_NUM_IPV4(unit);
    num_mpls_tnl = BCMTNL_NUM_MPLS(unit);

    alloc_sz =  SHR_BITALLOCSIZE(num_mpls_tnl);
    sal_memcpy(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap,
               BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
               alloc_sz);

    alloc_sz =  SHR_BITALLOCSIZE(num_ip_tnl);
    sal_memcpy(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap,
               BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
               alloc_sz);

    alloc_sz = (num_ip_tnl * sizeof(uint8_t));
    sal_memcpy(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type,
               BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type,
               alloc_sz);

    alloc_sz = (num_mpls_tnl * sizeof(bcmtnl_mpls_encap_entry_t));
    sal_memcpy(BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry,
               BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry,
               alloc_sz);

    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtnl_transaction_abort
 *
 * \brief Abort SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          Logical table ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_transaction_abort(int unit,
                         uint32_t trans_id,
                         uint32_t lt_sid)
{
    uint32_t    alloc_sz = 0;
    int         num_ip_tnl, num_mpls_tnl;

    SHR_FUNC_ENTER(unit);

    num_ip_tnl = BCMTNL_NUM_IPV4(unit);
    num_mpls_tnl = BCMTNL_NUM_MPLS(unit);

    alloc_sz =  SHR_BITALLOCSIZE(num_mpls_tnl);
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_tnl_bitmap,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_tnl_bitmap,
               alloc_sz);

    alloc_sz =  SHR_BITALLOCSIZE(num_ip_tnl);
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_bitmap,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_bitmap,
               alloc_sz);

    alloc_sz = (num_ip_tnl * sizeof(uint8_t));
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_base_tnl_type,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_base_tnl_type,
               alloc_sz);

    alloc_sz = (num_mpls_tnl * sizeof(bcmtnl_mpls_encap_entry_t));
    sal_memcpy(BCMTNL_ENCAP_SW_STATE(unit).bcmtnl_mpls_entry,
               BCMTNL_ENCAP_SW_STATE_HA(unit).bcmtnl_mpls_entry,
               alloc_sz);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Set pointer to tunnel encap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Chip driver structure.
 *
 * \return SHR_E_NONE
 */
int
bcmtnl_encap_drv_set(int unit,
                     bcmtnl_encap_drv_t *drv)
{
    bcmtnl_encap_drv[unit] = drv;

    return SHR_E_NONE;
}

/*!
 * \brief Get pointer to tunnel encap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
int
bcmtnl_encap_drv_get(int unit,
                     bcmtnl_encap_drv_t **drv)
{
    *drv = bcmtnl_encap_drv[unit];

    return SHR_E_NONE;
}
