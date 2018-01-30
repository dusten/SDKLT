/*! \file bcmlrd_field_def_get.c
 *
 * Get the field definition for the given table and map.
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

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_internal.h>

static bcmltd_field_acc_t
field_access_get(uint32_t flags)
{
    return (flags & BCMLRD_FIELD_F_READ_ONLY) ?
        BCMLTD_FIELD_ACCESS_READ_ONLY :
        BCMLTD_FIELD_ACCESS_READ_WRITE;
}

/*!
 * \brief Get field definition information by given table field data
 *        and map field data.
 *
 * This routine gets the field definition information for mapped fields.
 * The given field ID must be a valid mapped field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  fid             Field ID.
 * \param [in]  tbl_field       Table Field data.
 * \param [in]  map_field       Map Field data.
 * \param [out] field_def       Field Def data.
 *
 * \retval 0 OK
 * \retval <0 ERROR
 */
static int
bcmlrd_core_field_def_get(int unit,
                          const bcmlrd_fid_t fid,
                          const bcmlrd_table_rep_t *tbl,
                          const bcmlrd_map_t *map,
                          bcmlrd_field_def_t *field_def)
{
    const bcmlrd_field_data_t *map_field;
    const bcmltd_field_rep_t *tbl_field;

    map_field = &map->field_data->field[fid];
    tbl_field = &tbl->field[fid];

    sal_memset(field_def, 0, sizeof(*field_def));

    field_def->id = fid;
    field_def->name = tbl_field->name;
    field_def->key = !!(tbl_field->flags & BCMLRD_FIELD_F_KEY);
    field_def->symbol = !!(tbl_field->flags & BCMLRD_FIELD_F_ENUM);
    field_def->array = true; /* treat every field as an array */
    field_def->width = (map_field->width > 0) ? map_field->width:
                                                tbl_field->width;
    field_def->depth = (map_field->depth > 0) ? map_field->depth :
                                                tbl_field->depth;
    if (field_def->depth > 0) {
        /* number of elements of an array field is depth */
        field_def->elements = field_def->depth;
    } else {
        /* number of elements of a scalar field depends on width */
        field_def->elements =
            (field_def->width + (BCM_FIELD_SIZE_BITS - 1)) / BCM_FIELD_SIZE_BITS;
    }

    /* field access flags may be from the logical table definition
       or the field mapping */
    field_def->access = field_access_get(tbl_field->flags | map_field->flags);

    if (tbl_field->width == 1) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_BOOL;
        field_def->min.is_true = map_field->min->is_true;
        field_def->def.is_true = map_field->def->is_true;
        field_def->max.is_true = map_field->max->is_true;
    } else if (tbl_field->width <= 8) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U8;
        field_def->min.u8 = map_field->min->u8;
        field_def->def.u8 = map_field->def->u8;
        field_def->max.u8 = map_field->max->u8;
    } else if (tbl_field->width <= 16) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U16;
        field_def->min.u16 = map_field->min->u16;
        field_def->def.u16 = map_field->def->u16;
        field_def->max.u16 = map_field->max->u16;
    } else if (tbl_field->width <= 32) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U32;
        field_def->min.u32 = map_field->min->u32;
        field_def->def.u32 = map_field->def->u32;
        field_def->max.u32 = map_field->max->u32;
    } else if (tbl_field->width <= 64) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U64;
        field_def->min.u64 = map_field->min->u64;
        field_def->def.u64 = map_field->def->u64;
        field_def->max.u64 = map_field->max->u64;
    } else {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_RAW;
        field_def->min.binary = map_field->min->binary;
        field_def->def.binary = map_field->def->binary;
        field_def->max.binary = map_field->max->binary;
    }

    return SHR_E_NONE;
}

int
bcmlrd_field_def_get(int unit,
                     const bcmlrd_fid_t fid,
                     const bcmlrd_table_rep_t *tbl,
                     const bcmlrd_map_t *map,
                     bcmlrd_field_def_t *field_def)
{
    int rv = SHR_E_UNAVAIL;

    /* Get field definition for mapped field */
    if (!bcmlrd_field_is_unmapped_internal(map, fid)) {
        rv = bcmlrd_core_field_def_get(unit, fid, tbl, map, field_def);
    }

    return rv;
}
