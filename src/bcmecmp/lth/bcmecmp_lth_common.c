/*! \file bcmecmp_lth_common.c
 *
 * ECMP custom table handlers common functions.
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
 * Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmecmp/bcmecmp_lth_common.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief LT entry next-hop ids input parameter validation function.
 *
 * Validates total NHOP_IDs count matches NUM_PATHS value and the NHOP_IDs have
 * been populated at correct indices in the passed NHOP_ID array.
 *
 * For example:
 * ===========
 * If NUM_PATHS=2 and NHOP_ID[0],NHOP_ID[1023] are supplied then it's invalid
 * configuration. NHOP_ID[0] and NHOP_ID[1] should be populated for this
 * configuration.
 *
 * This function will check if NHOP_ID[0] and NHOP_ID[1] have valid NHOP_ID
 * values at these indices. If they are equal to BCMECMP_INVALID, then
 * Invalid Parameter error is returned.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode LT entry operation code.
 * \param [in] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Input parameter failed validation check.
 */
static int
bcmecmp_lt_nhops_validate(int unit,
                          bcmlt_opcode_t opcode,
                          bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t idx = 0; /* NHOP_IDs iterator variable. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* ECMP group identifier. */

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_nhops_validate.\n")));

    /* Get LT entry group level. */
    gtype = lt_entry->grp_type;

    /* Get the group ECMP_ID value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Check and return error if ECMP_UNDERLAY_ID is also set. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_UNDERLAY_ID field cannot be set for this"
                                " entry.\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Validate checkes based on the LT operation type. */
    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            /*
             * Check if total number of NHOP_ID[] elements is less than the
             * group's MAX_PATHS value, return error if it exceeds this limit.
             */
            if (lt_entry->nhop_ids_count > lt_entry->max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Total(%d) NHOP_ID[] fields exceeds "
                                        "MAX_PATHS=%u count.\n"),
                             lt_entry->nhop_ids_count,
                             lt_entry->max_paths));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMLT_OPCODE_UPDATE:
            /*
             * The count of NHOP_IDs supplied during UPDATE cannot exceed group
             * MAX_PATHS value.
             */
            if (lt_entry->nhop_ids_count > BCMECMP_GRP_MAX_PATHS
                                                    (unit, ecmp_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Total(%d) NHOP_IDs exceed "
                                        "MAX_PATHS=%u count.\n"),
                            lt_entry->nhop_ids_count,
                            BCMECMP_GRP_MAX_PATHS(unit, ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Verify specified ECMP_NHOP value is same as previously
             * inserted.
             */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                && (lt_entry->ecmp_nhop
                    != BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_NHOP=%d expected.\n"),
                             BCMECMP_GRP_ECMP_NHOP
                                (unit, ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }


    /*
     * Validate the NHOP_IDs have been passed at correct indices. Iterate over
     * count no. of NHOP_IDs and confirm all of them are valid.
     */
    for (idx = 0; idx < lt_entry->nhop_ids_count; idx++) {
        /*
         * If a NHOP_ID was pouplated at wrong index, then it will be
         * equal to BCMECMP_INVALID and this condition will be TRUE.
         */
        if (BCMECMP_INVALID == lt_entry->nhop_id[idx]) {

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "NHOP_ID[Index=%d] value required.\n"),
                        idx));

            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        /*
         * Verify NHOP_ID values are within valid NHOP_ID range of this
         * device.
         */
        if (lt_entry->nhop_id[idx] < BCMECMP_LT_MIN_NHOP_ID
                                            (unit, group, gtype)
             || lt_entry->nhop_id[idx] > BCMECMP_LT_MAX_NHOP_ID
                                            (unit, group, gtype)) {

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "NHOP_ID[%d]=%d is invalid.\n"),
                         idx,
                         lt_entry->nhop_id[idx]));

            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief LT entry Underlay ECMP IDs input parameter validation function.
 *
 * Verifies total ECMP_UNDERLAY_IDs count matches NUM_PATHS value and the
 * ECMP_UNDERLAY_IDs have been populated at correct indices in the supplied
 * ECMP_UNDERLAY_ID array.
 *
 * For example:
 * ===========
 * If NUM_PATHS=2 and ECMP_UNDERLAY_ID[0],ECMP_UNDERLAY_ID[1023] are supplied
 * then it's invalid configuration. ECMP_UNDERLAY_ID[0] and ECMP_UNDERLAY_ID[1]
 * should be populated for this configuration.
 *
 * This function will also validate supplied ECMP_UNDERLAY_IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode LT entry operation code.
 * \param [in] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Input parameter failed validation check.
 */
static int
bcmecmp_lt_uecmp_ids_validate(int unit,
                              bcmlt_opcode_t opcode,
                              bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t idx = 0; /* ECMP_UNDERLAY_ID array index iterator variable. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* ECMP group identifier. */

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_uecmp_ids_validate.\n")));

    /* Check and return error if NHOP_ID is also set. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Expected ECMP_UNDERLAY_ID to be set as "
                                "ECMP_NHOP=%d.\n"),
                     lt_entry->ecmp_nhop));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Get the group ECMP_ID value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Validate checkes based on the LT operation type. */
    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            /* Check and return error if ECMP_NHOP is not TRUE. */
            if (!lt_entry->ecmp_nhop) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Expected ECMP_NHOP=(1)/(TRUE).\n")));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Check if total number of ECMP_UNDERLAY_ID[] elements is less
             * than the group's MAX_PATHS value, return error if it exceeds
             * this limit.
             */
            if (lt_entry->uecmp_ids_count > lt_entry->max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Total(%d) ECMP_UNDERLAY_ID[] fields "
                                        "exceeds MAX_PATHS=%u count.\n"),
                             lt_entry->uecmp_ids_count,
                             lt_entry->max_paths));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMLT_OPCODE_UPDATE:
            /*
             * The count of UNDERLAY_ECMP_ID[] fields set during UPDATE cannot
             * exceed group MAX_PATHS value.
             */
            if (lt_entry->uecmp_ids_count > BCMECMP_GRP_MAX_PATHS
                                                        (unit, ecmp_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Total(%d) ECMP_UNDERLAY_ID[] fields "
                                        "exceed MAX_PATHS=%u count.\n"),
                            lt_entry->uecmp_ids_count,
                            BCMECMP_GRP_MAX_PATHS(unit, ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Verify specified ECMP_NHOP value is same as previously
             * inserted.
             */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                && (lt_entry->ecmp_nhop
                    != BCMECMP_GRP_ECMP_NHOP(unit, lt_entry->ecmp_id))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_NHOP=%d expected.\n"),
                             BCMECMP_GRP_ECMP_NHOP
                                (unit, lt_entry->ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Check if ECMP_NHOP is TRUE for this entry as ECMP_UNDERLAY_ID
             * field has been set in this update call. If ECMP_NHOP is
             * false, return invalid parameter error.
             */
            if (!BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                    && !BCMECMP_GRP_ECMP_NHOP(unit, lt_entry->ecmp_id)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_UNDERLAY_ID cannot be set"
                                        " when ECMP_NHOP=%d.\n"),
                             BCMECMP_GRP_ECMP_NHOP
                                (unit, lt_entry->ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }


    /*
     * Validate the ECMP_UNDERLAY_IDs have been passed at correct indices.
     * Iterate over count no. of ECMP_UNDERLAY_IDs and confirm all of them are
     * valid.
     */
    for (idx = 0; idx < lt_entry->uecmp_ids_count; idx++) {
        /*
         * If a ECMP_UNDERLAY_ID was pouplated at wrong index, then it will be
         * equal to BCMECMP_INVALID and this condition will be TRUE.
         */
        if (BCMECMP_INVALID == lt_entry->uecmp_id[idx]) {

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_UNDERLAY_ID[index=%d] value "
                                    "required.\n"),
                        idx));

            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        /* Verify supplied Underly ECMP group ID is in valid range. */
        if (lt_entry->uecmp_id[idx] < BCMECMP_LT_MIN_ECMP_ID
                                            (unit, group,
                                             BCMECMP_GRP_TYPE_UNDERLAY)
            || lt_entry->uecmp_id[idx] > BCMECMP_LT_MAX_ECMP_ID
                                            (unit, group,
                                             BCMECMP_GRP_TYPE_UNDERLAY)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_UNDERLAY_ID[%d]=%d invalid.\n"),
                        idx,
                        lt_entry->uecmp_id[idx]));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Initalize bcmecmp_lt_entry_t structure.
 *
 * Initializes bcmecmp_lt_entry_t structure member variables to required initial
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LTD Source identifier.
 * \param [in] lt_entry Pointer to bcmecmp_lt_entry_t structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmecmp_lt_entry_t_init(int unit,
                        bcmltd_sid_t sid,
                        bcmecmp_lt_entry_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_entry_t_init.\n")));

    if (lt_entry) {

        /* Initialize group type value. */
        switch (sid) {
            case ECMP_OVERLAYt:
                lt_entry->grp_type = BCMECMP_GRP_TYPE_OVERLAY;
                break;
            case ECMP_UNDERLAYt:
                lt_entry->grp_type = BCMECMP_GRP_TYPE_UNDERLAY;
                break;
            default:
                lt_entry->grp_type = BCMECMP_GRP_TYPE_SINGLE;
        }

        /* Initialize LT source ID value. */
        lt_entry->glt_sid = sid;

        /* Initialize LT Transaction ID value to invalid. */
        lt_entry->trans_id = (uint32_t)BCMECMP_INVALID;

        /* Initialize LT fields set bitmap to null. */
        lt_entry->fbmp = 0;

        /* Initialize ECMP group identifier value to invalid. */
        lt_entry->ecmp_id = BCMECMP_INVALID;

        /* Initialize ECMP group default load balancing mode to Regular mode. */
        lt_entry->lb_mode = BCMECMP_LB_MODE_REGULAR;

        /* Set LT entry's default ECMP_NHOP field value. */
        if (BCMECMP_GRP_TYPE_OVERLAY == lt_entry->grp_type) {
            /*
             * For ECMP_OVERLAY LT, set the default value of this field
             * to the value retrieved from device's LRD.
             */
            lt_entry->ecmp_nhop = BCMECMP_LT_DEF_ECMP_NHOP
                                        (unit, group, lt_entry->grp_type);
        } else {
            /* This field is not present in non-overlay LTs. */
            lt_entry->ecmp_nhop = FALSE;
        }

        /* Initialize ECMP_UNDERLAY_IDs array elements to invalid */
        sal_memset
            (lt_entry->uecmp_id, BCMECMP_INVALID, sizeof(lt_entry->uecmp_id));

        /* Initialize total Underlay ECMP IDs parsed count to zero. */
        lt_entry->uecmp_ids_count = 0;

        /* Initialize NHOP_IDs array elements to invalid */
        sal_memset
            (lt_entry->nhop_id, BCMECMP_INVALID, sizeof(lt_entry->nhop_id));

        /* Initialize total NHOP_IDs parsed count to zero. */
        lt_entry->nhop_ids_count = 0;

        /* By default next-hops are not sorted. */
        lt_entry->nhop_sorted = FALSE;

        /* Initialize to device ECMP groups max_paths value. */
        lt_entry->max_paths = BCMECMP_LT_MAX_PATHS
                                    (unit, group, lt_entry->grp_type);

        /* Initialize configured paths to zero. */
        lt_entry->num_paths = 0;

        /*
         * Initialize resilient hashing group size encoding value to device
         * minimum RH group size encoding value.
         */
        lt_entry->rh_size_enc = BCMECMP_LT_MIN_RH_SIZE
                                    (unit, group, lt_entry->grp_type);

        /*
         * Initialize resilient hashing group entries count value to device
         * minimum RH group size value.
         */
        lt_entry->rh_entries_cnt = (uint32_t)(1 << lt_entry->rh_size_enc);

        /* Initialize RH-ECMP group entries array base pointer to null. */
        lt_entry->rh_entries_arr = NULL;

        /*
         * Initialize RH group next-hop members ECMP member table entries
         * usage count array base pointer to null.
         */
        lt_entry->rh_memb_ecnt_arr = NULL;

        /* Initialize Group Member table start index to invalid. */
        lt_entry->mstart_idx = BCMECMP_INVALID;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief LTA out fields populate routine.
 *
 * Get ECMP LT field values from lt_entry and populate them in LTD out fields.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry ECMP LT entry data.
 * \param [in] include_key Include LT key field in the out fields list.
 * \param [out] out LTA out field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c lt_entry to \c out.
 */
int
bcmecmp_lt_fields_populate(int unit,
                           bcmecmp_lt_entry_t *lt_entry,
                           bool include_key,
                           bcmltd_fields_t *out)
{
    size_t fld_idx = 0; /* LT field index. */
    uint32_t path = 0, num_paths = 0, grp_mpaths = 0; /* Group paths. */
    bcmecmp_grp_ltd_fields_t *fids = NULL; /* Group LTD Field IDs. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Group NUM_PATHS field value. */
    num_paths = lt_entry->num_paths;

    /* Group MAX_PATHS field value. */
    grp_mpaths = lt_entry->max_paths;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_fields_populate[num_paths=%d].\n"),
                 num_paths));

    gtype = lt_entry->grp_type;

    fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                            (unit, group, gtype);
    SHR_NULL_CHECK(fids, SHR_E_INTERNAL);

    /* Check if LT key field must be populated. */
    if (include_key) {
        /* Set ECMP_ID logical field value. */
        out->field[fld_idx]->id = fids->ecmp_id_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = (uint64_t) lt_entry->ecmp_id;
    }

    /* Set LB_MODE logical field value. */
    out->field[fld_idx]->id = fids->lb_mode_fid;
    out->field[fld_idx]->idx = 0;
    out->field[fld_idx++]->data = (uint64_t) lt_entry->lb_mode;

    /* Set NHOP_SORTED logical field value. */
    out->field[fld_idx]->id = fids->nhop_sorted_fid;
    out->field[fld_idx]->idx = 0;
    out->field[fld_idx++]->data = lt_entry->nhop_sorted;

    /* Set MAX_PATHS logical field value. */
    out->field[fld_idx]->id = fids->max_paths_fid;
    out->field[fld_idx]->idx = 0;
    out->field[fld_idx++]->data = (uint64_t) lt_entry->max_paths;

    /* Set NUM_PATHS logical field value. */
    out->field[fld_idx]->id = fids->num_paths_fid;
    out->field[fld_idx]->idx = 0;
    out->field[fld_idx++]->data = (uint64_t) lt_entry->num_paths;

    /*
     * Set ECMP_NHOP logical field value for Overlay logical table entries if
     * this field is supported in this table for this device.
     */
    if (BCMECMP_GRP_TYPE_OVERLAY == gtype
        && (BCMLTD_SID_INVALID != fids->nhop_id_fid)) {
        out->field[fld_idx]->id = fids->ecmp_nhop_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = lt_entry->ecmp_nhop;
    }

    /*
     * Check if it's an Overlay ECMP group LT and ECMP_NHOP is true. If this
     * condition is satisfied, then populate ECMP_UNDERLAY_ID field values for
     * this group.
     */
    if (BCMECMP_GRP_TYPE_OVERLAY == gtype &&  lt_entry->ecmp_nhop) {
        /* Populate retrieved ECMP_UNDERLAY_ID values. */
        for (path = 0; path < grp_mpaths; path++, fld_idx++) {
            out->field[fld_idx]->id = fids->uecmp_id_fid;
            out->field[fld_idx]->data = (uint64_t)lt_entry->uecmp_id[path];
            out->field[fld_idx]->idx = (uint32_t)path;
        }

        /* Set unused ECMP_UNDERLAY_ID fields to default value. */
        for (path = grp_mpaths;
             path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
             path++, fld_idx++) {
            out->field[fld_idx]->id = fids->uecmp_id_fid;
            out->field[fld_idx]->data = (uint64_t)BCMECMP_LT_MIN_UECMP_ID
                                                    (unit, group, gtype);
            out->field[fld_idx]->idx = (uint32_t)path;
        }

        /*
         * Initialize unused NHOP_ID logical fields to default value if this
         * field is supported in this logical table for this device.
         */
        if (BCMLTD_SID_INVALID != fids->nhop_id_fid) {
            for (path = 0;
                 path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
                 path++, fld_idx++) {
                out->field[fld_idx]->id = fids->nhop_id_fid;
                out->field[fld_idx]->data =
                    (uint64_t)BCMECMP_LT_MIN_NHOP_ID(unit, group, gtype);
                out->field[fld_idx]->idx = (uint32_t)path;
            }
        }
    } else {
        if (BCMLTD_SID_INVALID != fids->nhop_id_fid) {
            /* Populate retrieved NHOP_ID[] field values. */
            for (path = 0; path < grp_mpaths; path++, fld_idx++) {
                out->field[fld_idx]->id = fids->nhop_id_fid;
                out->field[fld_idx]->data
                                    = (uint64_t)lt_entry->nhop_id[path];
                out->field[fld_idx]->idx = (uint32_t)path;
            }

            /* Set unused NHOP_ID[] fields to default value. */
            for (path = grp_mpaths;
                 path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
                 path++, fld_idx++) {
                out->field[fld_idx]->id = fids->nhop_id_fid;
                out->field[fld_idx]->data =
                    (uint64_t)BCMECMP_LT_MIN_NHOP_ID(unit, group, gtype);
                out->field[fld_idx]->idx = (uint32_t)path;
            }
        }

        /*
         * Initialize ECMP_UNDERLAY_ID fields to default value. When
         * ECMP_NHOP is FALSE, entry can support only regular next-hops as
         * group members.
         */
        if (BCMECMP_GRP_TYPE_OVERLAY == gtype && !lt_entry->ecmp_nhop) {
            for (path = 0;
                 path < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
                 path++, fld_idx++) {
                out->field[fld_idx]->id = fids->uecmp_id_fid;
                out->field[fld_idx]->data
                            = (uint64_t)BCMECMP_LT_MIN_UECMP_ID
                                            (unit, group, gtype);
                out->field[fld_idx]->idx = 0;
            }
        }
    }

    /*
     * Skip setting RH_SIZE field value for Underlay logical table
     * entries.
     */
    if (BCMECMP_GRP_TYPE_UNDERLAY != gtype) {
        out->field[fld_idx]->id = fids->rh_size_fid;
        out->field[fld_idx]->idx = 0;
        out->field[fld_idx++]->data = (uint64_t)lt_entry->rh_size_enc;
    }

    /* Set Out fields array legth. */
    out->count = fld_idx;

    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "bcmecmp_lt_fields_populate[flds_cnt=%u] "
                                "- exit.\n"),
                    (uint32_t)fld_idx));
        SHR_FUNC_EXIT();
}

/*!
 * \brief LT entry input parameters validate function.
 *
 * Validate LT entry input parameter values based on the LT operation type.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode LT entry operation code.
 * \param [in] sid Logical table source identifier.
 * \param [in] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmecmp_lt_input_params_validate(int unit,
                                 bcmlt_opcode_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_lt_input_params_validate.\n")));

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Get LT entry group level. */
    gtype = lt_entry->grp_type;

    /*
     * ECMP_ID value is must to perform any LT operation.
     * Check and return error if this field value is not supplied.
     */
    if (!BCMECMP_LT_FIELD_GET
            (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID field value not specified.\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Verify ECMP_ID is in valid range. */
    if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID)
        && (lt_entry->ecmp_id <  BCMECMP_LT_MIN_ECMP_ID
                                        (unit, group, gtype)
            || lt_entry->ecmp_id > BCMECMP_LT_MAX_ECMP_ID
                                        (unit, group, gtype))) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d is invalid.\n"),
                     lt_entry->ecmp_id));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Validate input fields based on the type of LT table operation. */
    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            /* Verify specified ECMP group is not in use. */
            if (BCMECMP_TBL_REF_CNT
                    (BCMECMP_TBL_PTR(unit, group, gtype), lt_entry->ecmp_id)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d exists.\n"),
                             lt_entry->ecmp_id));

                SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
            }

            /* Verify specified LB_MODE is valid value. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)
                && (lt_entry->lb_mode > BCMECMP_LT_MAX_LB_MODE
                                                (unit, group, gtype)
                    || lt_entry->lb_mode < BCMECMP_LT_MIN_LB_MODE
                                                (unit, group, gtype))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "LB_MODE=%d is invalid.\n"),
                             lt_entry->lb_mode));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* For LB_MODE field value of RAW, return not supported error. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RAW) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "LB_MODE=%d not supported.\n"),
                             lt_entry->lb_mode));

                SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            }

            /* NHOP_SORTED must be FALSE/0 for RH LB_MODE. */
            if (lt_entry->nhop_sorted
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NHOP_SORTED=%u is invalid.\n"),
                             lt_entry->nhop_sorted));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * MAX_PATHS value must be equal to the default MAX_PATHS value if
             * it is set for RESILIENT hashing LB_MODE.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->max_paths != BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype))
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid for "
                                        "RESILIENT LB_MODE.\n"),
                             lt_entry->max_paths));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is in valid range. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && (lt_entry->max_paths < BCMECMP_LT_MIN_PATHS
                                                (unit, group, gtype)
                    || lt_entry->max_paths > BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u is invalid.\n"),
                             lt_entry->max_paths));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Verify specified MAX_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && (lt_entry->max_paths & (lt_entry->max_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u not a power-of-two.\n"),
                             lt_entry->max_paths));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && lt_entry->num_paths > BCMECMP_LT_MAX_NPATHS
                                            (unit, group, gtype)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid "
                                        "(Grp MAX_PATHS=%u).\n"),
                             lt_entry->num_paths,
                             BCMECMP_LT_MAX_NPATHS(unit, group, gtype)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Verify specified non-zero NUM_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->num_paths
                && (lt_entry->num_paths & (lt_entry->num_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u not a power-of-two.\n"),
                             lt_entry->num_paths));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Active paths cannot exceed maximum paths supported by the
             * group.
             */
            if (lt_entry->num_paths > lt_entry->max_paths) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u exceeds MAX_PATHS=%u.\n"),
                            lt_entry->num_paths,
                            lt_entry->max_paths));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }


            /*
             * Check if LT entry belongs to an Overlay Group and it's Underlay
             * ECMP_ID field array is set.
             */
            if (gtype == BCMECMP_GRP_TYPE_OVERLAY
                && (BCMECMP_LT_FIELD_GET
                        (lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID)
                    || lt_entry->ecmp_nhop)) {
                /*
                 * Validate ECMP_UNDERLAY_IDs and verify NUM_PATHS count
                 * matches total underlay ECMP IDs supplied.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_lt_uecmp_ids_validate(unit, opcode, lt_entry));
            } else {
                /*
                 * Validate NHOP_IDs and verify NUM_PATHS count matches total
                 * next-hop IDs supplied.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_lt_nhops_validate(unit, opcode, lt_entry));
            }

            /*
             * RH_SIZE field if set for non-resilient hashing LB_MODE, the value
             * must be equal to the default value i.e. RH_SIZE_64.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)
                && lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "RH_SIZE cannot be set for "
                                        "LB_MODE=%d.\n"),
                            lt_entry->lb_mode));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Verify RH_SIZE value is in valid range. */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)
                && (lt_entry->rh_size_enc < BCMECMP_LT_MIN_RH_SIZE
                                                (unit, group, gtype)
                    || lt_entry->rh_size_enc > BCMECMP_LT_MAX_RH_SIZE
                                                (unit, group, gtype))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "RH_SIZE=%d is invalid.\n"),
                             lt_entry->rh_size_enc));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Number of next-hops for RH load balancing must be less than or
             * equal to the RH group size value when RH_SIZE is less than
             * MAX_PATHS value.
             */
            if (lt_entry->num_paths > lt_entry->rh_entries_cnt
                && lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u > RH_SIZE=%u for "
                                        "LB_MODE=%u.\n"),
                             lt_entry->num_paths,
                             lt_entry->rh_entries_cnt,
                             lt_entry->lb_mode));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);

            }
            break;

        case BCMLT_OPCODE_UPDATE:
            /*
             * ECMP_ID must be in use and logical table source identifier value
             * of the entry being updated must match the LT SID of the entry
             * stored during INSERT operation.
             */
            if (!BCMECMP_TBL_REF_CNT
                    (BCMECMP_TBL_PTR(unit, group, gtype), lt_entry->ecmp_id)
                || (BCMECMP_GRP_LT_SID(unit, lt_entry->ecmp_id) != sid)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d not in-use.\n"),
                             lt_entry->ecmp_id));

                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }

            /* Verify specified LB_MODE value is same as previously inserted. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)
                && (lt_entry->lb_mode
                    != BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "LB_MODE=%d expected.\n"),
                             BCMECMP_GRP_LB_MODE
                                (unit, lt_entry->ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Verify specified NHOP_SORTED value is same as previously
             * inserted.
             */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED)
                && (lt_entry->nhop_sorted
                    != BCMECMP_GRP_NHOP_SORTED(unit, lt_entry->ecmp_id))
                ) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NHOP_SORTED=%d expected.\n"),
                             BCMECMP_GRP_NHOP_SORTED
                                (unit, lt_entry->ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)
                && (lt_entry->max_paths
                    != BCMECMP_GRP_MAX_PATHS(unit, lt_entry->ecmp_id))
                ) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "MAX_PATHS=%u expected.\n"),
                             BCMECMP_GRP_MAX_PATHS
                                (unit, lt_entry->ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Verify NUM_PATHS value is in valid range if it's non-zero. */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && lt_entry->num_paths > BCMECMP_GRP_MAX_PATHS
                                            (unit, lt_entry->ecmp_id)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u is invalid "
                                        "(Grp MAX_PATHS=%u).\n"),
                            lt_entry->num_paths,
                            BCMECMP_GRP_MAX_PATHS
                                (unit, lt_entry->ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Verify specified non-zero NUM_PATHS value is a power-of-two. */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT)
                && BCMECMP_LT_POW_TWO_PATHS(unit, group, gtype)
                && lt_entry->num_paths
                && (lt_entry->num_paths & (lt_entry->num_paths - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u not a power-of-two.\n"),
                             lt_entry->num_paths));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /* Check if it's overlay ECMP group and ECMP_NHOP is TRUE. */
            if (gtype == BCMECMP_GRP_TYPE_OVERLAY
                && (BCMECMP_LT_FIELD_GET
                        (lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID)
                    || (BCMECMP_LT_FIELD_GET
                            (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                        && lt_entry->ecmp_nhop)
                    || (!BCMECMP_LT_FIELD_GET
                            (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)
                        && BCMECMP_GRP_ECMP_NHOP(unit, lt_entry->ecmp_id)))) {
                /*
                 * Validate ECMP_UNDERLAY_IDs and also verify NUM_PATHS count
                 * matches total underlay ECMP IDs supplied.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_lt_uecmp_ids_validate(unit, opcode, lt_entry));
            } else {
                /*
                 * Validate NHOP_IDs and also verify NUM_PATHS count matches
                 * total next-hop IDs supplied.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_lt_nhops_validate(unit, opcode, lt_entry));
            }

            /*
             * Verify specified RH_SIZE value is same as previously
             * inserted.
             */
            if (BCMECMP_LT_FIELD_GET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)
                && (lt_entry->rh_size_enc != BCMECMP_GRP_RH_SIZE
                                                (unit, lt_entry->ecmp_id))) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "RH_SIZE=%d expected.\n"),
                             BCMECMP_GRP_RH_SIZE
                                (unit, lt_entry->ecmp_id)));

                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * Number of next-hops for RH load balancing must be less than or
             * equal to the RH group size value when RH_SIZE is less than
             * MAX_PATHS value.
             */
            if (BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && (lt_entry->num_paths > BCMECMP_GRP_RH_ENTRIES_CNT
                                                (unit, lt_entry->ecmp_id))
                && (BCMECMP_GRP_LB_MODE
                        (unit, lt_entry->ecmp_id) == BCMECMP_LB_MODE_RESILIENT)
                ) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "NUM_PATHS=%u greater than RH_SIZE=%u "
                                        ".\n"),
                             lt_entry->num_paths,
                             BCMECMP_GRP_RH_ENTRIES_CNT
                                (unit, lt_entry->ecmp_id)));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);

            }
            break;

        case BCMLT_OPCODE_DELETE:
        case BCMLT_OPCODE_LOOKUP:
        case BCMLT_OPCODE_TRAVERSE:
            /*
             * ECMP_ID must be in use and logical table source identifier value
             * of the entry being Deleted/Looked-up must match the LT SID of the
             * entry stored during INSERT operation.
             */
            if (!BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group, gtype),
                                     lt_entry->ecmp_id)
                || (BCMECMP_GRP_LT_SID(unit, lt_entry->ecmp_id) != sid)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "ECMP_ID=%d not in-use.\n"),
                             lt_entry->ecmp_id));

                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    exit:
        SHR_FUNC_EXIT();
}
