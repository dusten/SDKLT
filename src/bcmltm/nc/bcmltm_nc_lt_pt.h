/*! \file bcmltm_nc_lt_pt.h
 *
 * Logical Table Manager - Node Cookies.
 *
 * Logical Table mappings with Physical Table groups.
 *
 * This file contains routines to construct LT information
 * for LT with Physical Table groups.  The information is used by
 * the LTM metadata during the FA and EE stages.
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

#ifndef BCMLTM_NC_LT_PT_H
#define BCMLTM_NC_LT_PT_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include "bcmltm_nc_lta_xfrm.h"

/*!
 * Number of PT keyed Op Information node cookies needed per Keyed LT.
 *
 * The PT Keyed Operations Information node cookie can be shared among
 * several PTM operations as follows:
 *     LOOKUP/TRAVERSE(GET_FIRST, GET_NEXT)
 *     INSERT/DELETE
 */
#define BCMLTM_NC_PT_KEYED_NUM    2

/*! Index for PT Keyed for INSERT and DELETE */
#define BCMLTM_NC_PT_KEYED_INS_DEL        0

/*! Index for PT Keyed for LOOKUP and TRAVERSE */
#define BCMLTM_NC_PT_KEYED_LOOK_TRAV      1

/*!
 * \brief Logical Table information for LT with Physical map groups.
 *
 * This structure contains information derived from the LRD
 * for logical tables with direct physical map groups.
 *
 * The information is referenced by the LTM metadata and
 * is used for the FA and EE node cookies.
 *
 * Node cookies that have the same information for several opcodes
 * can be shared among the corresponding operation node functions.
 */
typedef struct bcmltm_nc_lt_pt_info_s {
    /*!
     * Working buffer block ID for the LT index.
     *
     * This is only applicable for Index LTs (Simple Index or
     * Index with Allocation).
     */
    bcmltm_wb_block_id_t lt_index_wb_block_id;

    /*!
     * LT index calculation information.
     *
     * This is the FA node cookie for LTM index resource operation
     * on Index LTs.
     */
    bcmltm_fa_lt_index_t *fa_lt_index;

    /*!
     * LT index information.
     *
     * This is the EE node cookie for LTM index resource operation
     * on Index LTs.
     */
    bcmltm_ee_index_info_t *ee_lt_index;

    /*!
     * List of working buffer coordinates to copy,
     *   - from: LT index and param
     *   - to  : PTM index and param sections.
     *
     * This is the FA node cookie used during a TRAVERSE operation
     * to get the next entry and propagate the keys into the PTM WB
     * on Index LTs.
     */
    bcmltm_wb_copy_list_t *lt_to_pt_index;

    /*!
     * List of mapped physical tables that correspond to an Index LT.
     * This is the EE node cookie for PTM index operations.
     */
    bcmltm_pt_list_t *pt_list;

    /*!
     * List of mapped physical tables that correspond to a Keyed LT.
     * This is the EE node cookie for PTM keyed operations.
     */
    bcmltm_pt_keyed_op_info_t *pt_keyed_list[BCMLTM_NC_PT_KEYED_NUM];

    /*!
     * List of direct physical mapped key fields on input direction.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_to_ptm_mapping_t *ptm_mapping_keys_in;

    /*!
     * List of direct physical mapped key fields on output direction.
     *
     * This is the FA node cookie used for mapping PT working buffer
     * locations to corresponding LT fields.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     *
     * Note that for Index LTs, the field mapping input and output
     * information are the same; however, these are different on Keyed LTs.
     */
    bcmltm_to_ptm_mapping_t *ptm_mapping_keys_out;

    /*!
     * List of direct physical mapped key fields that are required
     * during an INSERT operation on Index with Allocation LTs.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations on an INSERT operation.
     *
     * This is applicable for Index with Allocation LTs on INSERT.
     */
    bcmltm_to_ptm_mapping_t *ptm_mapping_keys_alloc_required;

    /*!
     * List of direct physical mapped key fields that are optional
     * during an INSERT operation on Index with Allocation LTs.
     *
     * This is the FA node cookie used for mapping LT fields to
     * corresponding PT working buffer locations on an INSERT operation.
     * If optional keys are not present, they are allocated during
     * the operation.
     *
     * This is applicable for Index with Allocation LTs on INSERT.
     */
    bcmltm_to_ptm_mapping_t *ptm_mapping_keys_alloc_optional;

    /*!
     * List of direct physical mapped value fields.
     *
     * This is the FA node cookie used for mapping LT fields to/from
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed,
     * for any direction (input or output).
     */
    bcmltm_to_ptm_mapping_t *ptm_mapping_values;

    /*!
     * List of direct fixed mapped fields.
     *
     * This is the FA node cookie used for copying fixed values into
     * corresponding PT working buffer locations.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_fixed_field_list_t *fixed_fields;

    /*!
     * List of Forward Key Field Transforms information used
     * as FA node cookies.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_nc_lta_xfrm_info_list_t *fwd_key_xfrm_info_list;

    /*!
     * List of Forward Value Field Transform information used
     * as FA node cookies.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_nc_lta_xfrm_info_list_t *fwd_value_xfrm_info_list;

    /*!
     * List of Reverse Key Field Transform information used
     * as FA node cookies.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_nc_lta_xfrm_info_list_t *rev_key_xfrm_info_list;

    /*!
     * List of Reverse Value Field Transform information used
     * as FA node cookies.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_nc_lta_xfrm_info_list_t *rev_value_xfrm_info_list;

    /*!
     * List of LTA Transform Copy LTA Field information used
     * as FA node cookies.
     *
     * This is applicable for any directly mapped LT, Index or Keyed.
     */
    bcmltm_nc_lta_xfrm_info_list_t *copy_xfrm_info_list;

} bcmltm_nc_lt_pt_info_t;


/*!
 * \brief Create LT cookies info for PT map groups for given table.
 *
 * Create the LT cookies information needed by the LTM metadata
 * for LT with Physical table group mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] pt_ptr Returning pointer to LT info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_pt_info_create(int unit, bcmlrd_sid_t sid,
                            const bcmltm_table_attr_t *attr,
                            const bcmltm_map_pt_t *pt_map,
                            bcmltm_nc_lt_pt_info_t **pt_ptr);

/*!
 * \brief Destroy LT cookies info for PT map groups.
 *
 * Destroy LT cookies info for PT map groups.
 *
 * \param [in] pt Pointer to LT info for PT map groups to destroy.
 */
extern void
bcmltm_nc_lt_pt_info_destroy(bcmltm_nc_lt_pt_info_t *pt);

#endif /* BCMLTM_NC_LT_PT_H */
