/*! \file bcmecmp_lth_underlay_group.c
 *
 * ECMP_UNDERLAY logical table handler functions.
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
#include <shr/shr_pb.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmecmp/bcmecmp_lth_underlay_group.h>
#include <bcmecmp/bcmecmp_lth_common.h>
#include <bcmecmp/bcmecmp_group.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief ECMP_UNDERLAY logical table entry fields print function.
 *
 * Debug ECMP_UNDERLAY logical table entry fields and values print function.
 *
 * \param [in] unit Unit number.
 * \param [in] fields Local table entry fields array.
 * \param [in] in Indicates type of field array In(1) vs Out(0).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 */
static int
bcmecmp_ulay_ltd_fields_print(int unit,
                              const bcmltd_fields_t *fields,
                              bool in)
{
    size_t fld_idx;                    /* Field index. */
    shr_pb_t *pb = shr_pb_create();    /* Pointer to print buffer. */
    const char *fnames[] = {"ECMP_ID", /* LT entry fields. */
                            "LB_MODE",
                            "NHOP_SORTED",
                            "MAX_PATHS",
                            "NUM_PATHS",
                            "NHOP_ID"};

    SHR_FUNC_ENTER(unit);

    /* Null pointer input parameter check. */
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t\t%s:COUNT=%u.\n",
                    in ? "IN" : "OUT",
                    (unsigned int)fields->count);
    shr_pb_printf(pb, "\t\t===============\n");
    for (fld_idx = 0; fld_idx < fields->count; fld_idx++) {
        shr_pb_printf(pb,
                      "\t\t%s=%" PRIu64 "\tfld_idx=%u idx=%u.\n",
                      fnames[fields->field[fld_idx]->id],
                      fields->field[fld_idx]->data,
                      (unsigned int)fld_idx,
                      fields->field[fld_idx]->idx);
    }
    shr_pb_printf(pb, "\n");
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "%s"), shr_pb_str(pb)));
    exit:
        shr_pb_destroy(pb);
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table entry logical fields parse routine.
 *
 * Parse ECMP_UNDERLAY logical table entry logical fields and store the data in
 * lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in Logical table database input field array.
 * \param [in] sid Logical table database source identifier.
 * \param [out] lt_entry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcmecmp_ulay_lt_fields_parse(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_sid_t sid,
                             bcmecmp_lt_entry_t *lt_entry)
{
    size_t i;      /* LT entry fields iterator. */
    uint32_t fid;  /* LT field identifier. */
    uint64_t fval; /* LT field value. */

    SHR_FUNC_ENTER(unit);

    /* Check LT entry buffer is valid. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_ulay_lt_fields_parse:CNT=%d LT_SID=%d\n"),
             (uint32_t)(in->count), sid));

    /* Validate LT source identifier and confirm it's overlay LT. */
    if (ECMP_UNDERLAYt != sid) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < in->count; i++) {
        fid = in->field[i]->id;
        fval = in->field[i]->data;
        switch (fid) {
            case ECMP_UNDERLAYt_ECMP_IDf:
                BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
                lt_entry->ecmp_id = (bcmecmp_id_t)fval;
                break;

            case ECMP_UNDERLAYt_LB_MODEf:
                BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE);
                lt_entry->lb_mode = (uint32_t) fval;
                break;

            case ECMP_UNDERLAYt_NHOP_SORTEDf:
                BCMECMP_LT_FIELD_SET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED);
                lt_entry->nhop_sorted = fval;
                break;

            case ECMP_UNDERLAYt_MAX_PATHSf:
                BCMECMP_LT_FIELD_SET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
                lt_entry->max_paths = (uint32_t) fval;
                break;

            case ECMP_UNDERLAYt_NUM_PATHSf:
                BCMECMP_LT_FIELD_SET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
                lt_entry->num_paths = (uint32_t) fval;
                break;

            case ECMP_UNDERLAYt_NHOP_IDf:
                BCMECMP_LT_FIELD_SET
                    (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
                lt_entry->nhop_id[in->field[i]->idx] = (int) fval;

                /* Increment parsed NHOP_IDs count. */
                lt_entry->nhop_ids_count += 1;
                break;

            default:
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief ECMP_UNDERLAY logical table operation input parameters validation.
 *
 * Validate input logical field values based on the logical table operation
 * type.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode Logical table operation code.
 * \param [in] in Input field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_lth_underlay_group_validate(int unit,
                                    bcmlt_opcode_t opcode,
                                    const bcmltd_fields_t *in,
                                    const bcmltd_generic_arg_t *arg)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP LT entry data. */

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_validate.\n")));

    /* Verify ECMP feature is initialized. */
    BCMECMP_INIT(unit);

    /* Verify device is configured in Hierarchical ECMP mode. */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unsupported logical table for "
                                "ECMP_MODE=%s.\n"),
                    mode[BCMECMP_MODE(unit)]));
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /* Print fields when debugging is enabled by using this function. */
    LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_ulay_ltd_fields_print(unit, in, 1)));

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Initialize overlay LT entry buffer. */
    bcmecmp_lt_entry_t_init(unit, arg->sid, lt_entry);

    /* Parse overlay LT entry fields and get the input values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_ulay_lt_fields_parse(unit, in, arg->sid, lt_entry));

    /* Validate input parameter values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_lt_input_params_validate(unit, opcode, arg->sid, lt_entry));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table entry insert.
 *
 * Inserts an entry into ECMP logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] in Input field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_lth_underlay_group_insert(int unit,
                                  uint32_t trans_id,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_insert.\n")));

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Assign transaction identifier value. */
    lt_entry->trans_id = trans_id;

    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_group_add(unit, lt_entry));

    BCMECMP_TBL_BK_ENT_STAGED_SET
        (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
         lt_entry->ecmp_id);

    /* Save the Group's Trasanction ID for use during commit/abort operation. */
    BCMECMP_GRP_TRANS_ID_BK(unit, lt_entry->ecmp_id) = trans_id;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table entry lookup.
 *
 * Lookup an entry in ECMP_UNDERLAY logical table and populate entry logical
 * field values in logical table database out field array.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND Entry not found.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_lth_underlay_group_lookup(int unit,
                                  uint32_t trans_id,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg)
{
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */
    int rv; /* Return value. */

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_lookup.\n")));

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Assign transaction identifier value. */
    lt_entry->trans_id = trans_id;

    rv = bcmecmp_group_get(unit, lt_entry);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    if (SHR_E_NONE == rv) {
        /* Entry found populate retrieved 'out' field values. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_lt_fields_populate(unit, lt_entry, 0, out));

        /* Print fields when debugging is enabled by using this function. */
        LOG_DEBUG(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    bcmecmp_ulay_ltd_fields_print(unit, in, 1)));
        LOG_DEBUG(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    bcmecmp_ulay_ltd_fields_print(unit, out, 0)));
    } else {
        /* No fields retrieved as entry was not found set 'count' to zero. */
        out->count = 0;
        SHR_RETURN_VAL_EXIT(rv);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table entry delete.
 *
 * Delete an entry from ECMP logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] in Input field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND Entry not found.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_lth_underlay_group_delete(int unit,
                                  uint32_t trans_id,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_delete.\n")));

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Assign transaction identifier value. */
    lt_entry->trans_id = trans_id;

    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_group_delete(unit, lt_entry));

    BCMECMP_TBL_BK_ENT_STAGED_SET
        (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
         lt_entry->ecmp_id);

    /* Save the Group's Trasanction ID for use during commit/abort operation. */
    BCMECMP_GRP_TRANS_ID_BK(unit, lt_entry->ecmp_id) = trans_id;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table entry update.
 *
 * Update an existing entry in ECMP logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] in Input field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_lth_underlay_group_update(int unit,
                                  uint32_t trans_id,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP entry data. */

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_update.\n")));

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Assign transaction identifier value. */
    lt_entry->trans_id = trans_id;

    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_group_update(unit, lt_entry));

    BCMECMP_TBL_BK_ENT_STAGED_SET
        (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
         lt_entry->ecmp_id);

    /* Save the Group's Trasanction ID for use during commit/abort operation. */
    BCMECMP_GRP_TRANS_ID_BK(unit, lt_entry->ecmp_id) = trans_id;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table get first entry.
 *
 * Traverses ECMP_UNDERLAY logical table and returns key-field and data-field
 * values of the first entry found in this logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [out] out LTD output field set.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_NOT_FOUND No entries found in the logical table.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_lth_underlay_group_first(int unit,
                                 uint32_t trans_id,
                                 bcmltd_fields_t *out,
                                 const bcmltd_generic_arg_t *arg)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP LT entry data ptr. */
    int rv;                              /* Return value. */

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    /* Validate input parameters. */
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_first: LT_SID=%d.\n"),
              arg->sid));

    /* Verify ECMP feature has been initialized on this BCM unit. */
    BCMECMP_INIT(unit);

    /*
     * Validate Logical Table SID value. Verify if it matches Underlay ECMP
     * resolution group SID value initialized for the configured device ECMP
     * resolution mode.
     */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)
        || (arg->sid != BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY))) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LT unavailable for ECMP_MODE=%s.\n"),
                     mode[BCMECMP_MODE(unit)]));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Initialize LT entry buffer. */
    bcmecmp_lt_entry_t_init(unit, arg->sid, lt_entry);

    /* Initialize LT entry transaction identifier value. */
    lt_entry->trans_id = trans_id;

    /* Find the first valid group configured in this ECMP logical table. */
    rv = bcmecmp_group_find(unit, TRUE, lt_entry);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    if (SHR_E_NONE == rv) {
        /* Entry found populate retrieved 'out' field values. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_lt_fields_populate(unit, lt_entry, TRUE, out));

        /* Print fields when debugging is enabled by using this function. */
        LOG_DEBUG(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    bcmecmp_ulay_ltd_fields_print(unit, out, 0)));
    } else {
        /* No fields retrieved as table is empty, set 'count' to zero. */
        out->count = 0;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No entries found: rv=%d count=%u.\n"),
                     rv, (unsigned int)out->count));

        SHR_RETURN_VAL_EXIT(rv);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table get next entry.
 *
 * Traverses ECMP_UNDERLAY logical table starting from the specified ECMP_ID
 * until the maximum ECMP_ID value supported by this logical table, searching
 * for the next valid entry inserted in this table.
 *
 * If an entry is found, this function returns the entry ECMP_ID key-field
 * value along with the data-field values. If no entry is found after the
 * given ECMP_ID, this function returns SHR_E_NOT_FOUND error.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] in LTD input field set.
 * \param [out] out LTD output field set.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_NOT_FOUND No entries found after the given ECMP_ID.
 * \retval SHR_E_MEMORY Logcal table entry memory allocation failed.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_lth_underlay_group_next(int unit,
                                uint32_t trans_id,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_lt_entry_t *lt_entry = NULL; /* ECMP LT entry data ptr. */
    bcmecmp_lt_entry_t *in_entry = NULL; /* In ECMP LT entry data. */
    int rv;                              /* Return value. */

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    /* Validate input parameters. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_next: LT_SID=%d.\n"),
              arg->sid));

    /* Verify ECMP feature has been initialized on this BCM unit. */
    BCMECMP_INIT(unit);

    /*
     * Validate Logical Table SID value. Verify if it matches Underlay ECMP
     * resolution group SID value initialized for the configured device ECMP
     * resolution mode.
     */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)
        || (arg->sid != BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY))) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LT unavailable for ECMP_MODE=%s.\n"),
                     mode[BCMECMP_MODE(unit)]));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Allocate input LT entry buffer. */
    BCMECMP_ALLOC(in_entry, sizeof(*in_entry), "bcmecmpLthUlayGrpNxtInEnt");

    /* Get logical table entry buffer pointer. */
    lt_entry = BCMECMP_LT_ENT_PTR(unit);

    /* Initialize IN and OUT logical table entry buffers. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_lt_entry_t_init(unit, arg->sid, in_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_lt_entry_t_init(unit, arg->sid, lt_entry));

    /* Parse the input fields and retrieve the field values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_ulay_lt_fields_parse(unit, in, arg->sid, in_entry));

    /*
     * Initialize the start ECMP_ID value for next valid entry search
     * operation.
     */
    lt_entry->ecmp_id = (in_entry->ecmp_id + 1);

    /* Initialize LT entry transaction identifier value. */
    lt_entry->trans_id = trans_id;

    /* Find the next group in this ECMP logical table. */
    rv = bcmecmp_group_find(unit, FALSE, lt_entry);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    if (SHR_E_NONE == rv) {
        /* Entry found populate retrieved 'out' field values. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_lt_fields_populate(unit, lt_entry, TRUE, out));

        /* Print fields when debugging is enabled by using this function. */
        LOG_DEBUG(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    bcmecmp_ulay_ltd_fields_print(unit, in, 1)));
        LOG_DEBUG(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    bcmecmp_ulay_ltd_fields_print(unit, out, 0)));
    } else {
        /* No fields retrieved as entry was not found set 'count' to zero. */
        out->count = 0;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No entries found: rv=%d count=%u.\n"),
                     rv, (unsigned int)out->count));

        SHR_RETURN_VAL_EXIT(rv);
    }

    exit:
        BCMECMP_FREE(in_entry);
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table transaction commit function.
 *
 * This function commits a transaction that was previously defined by a series
 * of insert/update/delete and lookup operations. A transaction may contain
 * multiple operations that were staged before.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmecmp_lth_underlay_group_commit(int unit,
                                  uint32_t trans_id,
                                  const bcmltd_generic_arg_t *arg)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    /* Validate input parameters. */
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_commit: LT_SID=%d.\n"),
              arg->sid));

    /* Verify ECMP feature is initialized. */
    BCMECMP_INIT(unit);

    /* Verify device is configured in Hierarchical ECMP mode. */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No support as ECMP_MODE=%s.\n"),
                                mode[BCMECMP_MODE(unit)]));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
         ecmp_id++ ) {

        if (!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                BCMECMP_GRP_TYPE_UNDERLAY),
                                                ecmp_id)
            || (trans_id != BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id))) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: ecmp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     ecmp_id,
                     trans_id,
                     BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id),
                     BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit,
                                                     group,
                                                     BCMECMP_GRP_TYPE_UNDERLAY),
                                                     ecmp_id),
                     arg->sid));


        if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)
            || BCMECMP_GRP_ECMP_NHOP_BK(unit, ecmp_id)) {
            /* ECMP_NHOP can be true only for ECMP_OVERLAY LT entries. */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else {
            if (BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                                    BCMECMP_GRP_TYPE_UNDERLAY),
                                                    ecmp_id)) {
                /* Insert/Update commit. */
                sal_memcpy(BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id),
                           BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                           (sizeof(bcmecmp_nhop_id_t) * BCMECMP_GRP_MAX_PATHS
                                                            (unit, ecmp_id)));
            } else {
                /* Delete commit. */
                if (BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id)) {
                    BCMECMP_FREE(BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id));
                }
            }
        }

        if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT
            || BCMECMP_GRP_LB_MODE_BK(unit, ecmp_id)
                            == BCMECMP_LB_MODE_RESILIENT) {
            /*
             * Reslient hashing mode supported only for ECMP, ECMP_HIERARCHICAL,
             * ECMP_FAST and ECMP_OVERLAY logical tables.
             */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        BCMECMP_GRP_TYPE_BK(unit, ecmp_id) = BCMECMP_GRP_TYPE(unit, ecmp_id);
        BCMECMP_GRP_LT_SID_BK(unit, ecmp_id) = BCMECMP_GRP_LT_SID(unit,
                                                                  ecmp_id);
        BCMECMP_GRP_LB_MODE_BK(unit, ecmp_id) = BCMECMP_GRP_LB_MODE(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_NHOP_SORTED_BK(unit, ecmp_id) = BCMECMP_GRP_NHOP_SORTED
                                                             (unit, ecmp_id);
        BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id) = BCMECMP_GRP_MAX_PATHS(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_NUM_PATHS_BK(unit, ecmp_id) = BCMECMP_GRP_NUM_PATHS(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_ECMP_NHOP_BK(unit, ecmp_id) = BCMECMP_GRP_ECMP_NHOP(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_RH_SIZE_BK(unit, ecmp_id) = BCMECMP_GRP_RH_SIZE(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id) =
                            BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id);
        BCMECMP_GRP_MEMB_TBL_START_IDX_BK(unit, ecmp_id) =
                            BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);

        tbl_size = (size_t)BCMECMP_TBL_SIZE(unit, member,
                                            BCMECMP_GRP_TYPE_UNDERLAY)
                            + BCMECMP_TBL_SIZE(unit, member,
                                               BCMECMP_GRP_TYPE_OVERLAY);
        sal_memcpy(tbl_ptr->ent_bk_arr, tbl_ptr->ent_arr,
                   (sizeof(bcmecmp_hw_entry_info_t) * tbl_size));

        BCMECMP_TBL_BK_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                    BCMECMP_GRP_TYPE_UNDERLAY), ecmp_id)
            = BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                                  BCMECMP_GRP_TYPE_UNDERLAY),
                                                  ecmp_id);

        BCMECMP_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY),
                                            ecmp_id);
    }
    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "exit: trans_id=%u sid=%u.\n"),
                     trans_id, arg->sid));
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_UNDERLAY logical table transaction abort function.
 *
 * This function aborts a transaction that was previously staged via a series
 * of insert/update/delete and lookup operations. As a result, all the staging
 * will be cleaned up and actual logical table entry change will not take place.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmecmp_lth_underlay_group_abort(int unit,
                                 uint32_t trans_id,
                                 const bcmltd_generic_arg_t *arg)
{
    const char *const mode[BCMECMP_MODE_COUNT] = BCMECMP_MODE_NAME;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_tbl_prop_t *tbl_ptr = NULL;
    size_t tbl_size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    /* Validate input parameters. */
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_lth_underlay_group_abort: LT_SID=%d.\n"),
              arg->sid));

    /* Verify ECMP feature is initialized. */
    BCMECMP_INIT(unit);

    /* Verify device is configured in Hierarchical ECMP mode. */
    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No support as ECMP_MODE=%s.\n"),
                                mode[BCMECMP_MODE(unit)]));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Member table pointer must be valid for any further processing. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_INTERNAL);

    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
         ecmp_id++ ) {

        if (!BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit, group,
                                                BCMECMP_GRP_TYPE_UNDERLAY),
                                                ecmp_id)
            || (trans_id != BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id))) {
            continue;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "enter: ecmp_id=%d trans_id/bk_id=%u/%u"
                                " staged=0x%x sid=%u.\n"),
                     ecmp_id,
                     trans_id,
                     BCMECMP_GRP_TRANS_ID_BK(unit, ecmp_id),
                     BCMECMP_TBL_BK_ENT_STAGED_GET(BCMECMP_TBL_PTR(unit,
                                                     group,
                                                     BCMECMP_GRP_TYPE_UNDERLAY),
                                                     ecmp_id),
                     arg->sid));

        if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)
            || BCMECMP_GRP_ECMP_NHOP_BK(unit, ecmp_id)) {
            /* ECMP_NHOP can be true only for ECMP_OVERLAY LT entries. */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else {
            if (BCMECMP_TBL_BK_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY),
                                            ecmp_id)
                && BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY),
                                            ecmp_id) == 0) {
                /* Revert delete. */
                BCMECMP_ALLOC(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                              BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id) *
                              sizeof(*(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id))),
                              "bcmecmpRhGrpAbortGrpNhopArr");
                sal_memcpy(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                           BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id),
                           (sizeof(bcmecmp_nhop_id_t) *
                            BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id)));

            } else if (BCMECMP_TBL_BK_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                                    BCMECMP_GRP_TYPE_UNDERLAY),
                                                    ecmp_id) == 0
                        && BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                                    BCMECMP_GRP_TYPE_UNDERLAY),
                                                    ecmp_id)) {
                /* Revert insert. */
                SHR_NULL_CHECK(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                                                    SHR_E_INTERNAL);
                BCMECMP_FREE(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id));

                SHR_NULL_CHECK(BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id),
                                                       SHR_E_INTERNAL);
                BCMECMP_FREE(BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id));
            } else {
                /* Revert update. */
                SHR_NULL_CHECK(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                                                    SHR_E_INTERNAL);
                SHR_NULL_CHECK(BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id),
                                                       SHR_E_INTERNAL);
                sal_memcpy(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                           BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id),
                           (sizeof(bcmecmp_nhop_id_t) *
                            BCMECMP_GRP_MAX_PATHS_BK(unit, ecmp_id)));
            }
        }

        if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT
            || BCMECMP_GRP_LB_MODE_BK(unit, ecmp_id)
                        == BCMECMP_LB_MODE_RESILIENT) {
            /*
             * Reslient hashing mode supported only for ECMP, ECMP_HIERARCHICAL,
             * ECMP_FAST and ECMP_OVERLAY logical tables.
             */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        BCMECMP_GRP_TYPE(unit, ecmp_id) = BCMECMP_GRP_TYPE_BK(unit, ecmp_id);
        BCMECMP_GRP_LT_SID(unit, ecmp_id) = BCMECMP_GRP_LT_SID_BK(unit,
                                                                  ecmp_id);
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_GRP_LB_MODE_BK(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) = BCMECMP_GRP_NHOP_SORTED_BK
                                                             (unit, ecmp_id);
        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = BCMECMP_GRP_MAX_PATHS_BK(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = BCMECMP_GRP_NUM_PATHS_BK(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id) = BCMECMP_GRP_ECMP_NHOP_BK(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_RH_SIZE(unit, ecmp_id) = BCMECMP_GRP_RH_SIZE_BK(unit,
                                                                    ecmp_id);
        BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) =
                        BCMECMP_GRP_RH_ENTRIES_CNT_BK(unit, ecmp_id);
        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) =
                        BCMECMP_GRP_MEMB_TBL_START_IDX_BK(unit, ecmp_id);

        tbl_size = (size_t)BCMECMP_TBL_SIZE(unit, member,
                                            BCMECMP_GRP_TYPE_UNDERLAY)
                                + BCMECMP_TBL_SIZE(unit, member,
                                                   BCMECMP_GRP_TYPE_OVERLAY);
        sal_memcpy(tbl_ptr->ent_arr, tbl_ptr->ent_bk_arr,
                   (sizeof(bcmecmp_hw_entry_info_t) * tbl_size));

        BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY), ecmp_id)
                = BCMECMP_TBL_BK_REF_CNT(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY),
                                            ecmp_id);
        BCMECMP_TBL_BK_ENT_STAGED_CLEAR(BCMECMP_TBL_PTR(unit, group,
                                            BCMECMP_GRP_TYPE_UNDERLAY),
                                            ecmp_id);
    }

    exit:
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "exit: trans_id=%u sid=%u.\n"),
                     trans_id, arg->sid));
        SHR_FUNC_EXIT();
}
