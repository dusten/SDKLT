/*! \file bcmltm_fa_util.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Utility functions for field manipulations
 *
 * This file contains the LTM FA stage utilities, including copying
 * API field values to/from the Working Buffer.
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

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_field.h>

#include <bcmltm/bcmltm_fa_util_internal.h>

/*******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

bool
bcmltm_field_fit_check(bcmltm_wb_coordinate_t *wbc, uint32_t *field_val)
{
    uint32_t    mask;    /* Value mask                      */
    uint16_t    len;     /* Bits in field                   */
    int         idx;     /* Iteration index.                */


    len = wbc->maxbit - wbc->minbit + 1;
    idx = (len - 1) >> 5;
    len = len % 32;

    if (((idx % 2) == 0) && (field_val[idx + 1] != 0)) {
        /* Set bits in high word beyond range of field */
        return FALSE;
    }

    if (!len) {
       return TRUE;
    }

    mask = (1 << len) - 1;
    if((field_val[idx] & ~mask) != 0) {
        return (FALSE);
    }
    return (TRUE);
}

bcmltm_field_list_t *
bcmltm_api_find_field(bcmltm_field_list_t *in_fields,
                      uint32_t api_field_id,
                      uint32_t field_idx)
{
    bcmltm_field_list_t *flist = in_fields;

    while (flist != NULL) {
        if ((flist->id == api_field_id) &&
            (flist->idx == field_idx)) {
            return flist;
        }
        flist = flist->next;
    }

    return NULL;
}

void
bcmltm_field_value_into_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer)
{
    uint32_t *entry_base;

    if ((wbc->minbit == BCMLTM_FIELD_BIT_POSITION_INVALID) ||
        (wbc->maxbit == BCMLTM_FIELD_BIT_POSITION_INVALID)) {
        return;
    }
    entry_base = ltm_buffer + wbc->buffer_offset;

    bcmdrd_field_set(entry_base,
                     wbc->minbit, wbc->maxbit,
                     field_val);
}

void
bcmltm_field_value_from_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer)
{
    uint32_t *entry_base;

    if ((wbc->minbit == BCMLTM_FIELD_BIT_POSITION_INVALID) ||
        (wbc->maxbit == BCMLTM_FIELD_BIT_POSITION_INVALID)) {
        return;
    }

    entry_base = ltm_buffer + wbc->buffer_offset;

    bcmdrd_field_get(entry_base,
                     wbc->minbit, wbc->maxbit,
                     field_val);

    if ((wbc->maxbit - wbc->minbit) < 32) {
        /* Avoid trash in the upper word.  Pending uint64_t handling. */
        field_val[1] = 0;
    }
}

void
bcmltm_field_value_into_buffer(bcmltm_ptm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer)
{
    bcmltm_field_value_into_wbc(&(field_map->wbc),
                                field_val, ltm_buffer);
}

void
bcmltm_field_value_from_buffer(bcmltm_ptm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer)
{
    bcmltm_field_value_from_wbc(&(field_map->wbc),
                                field_val, ltm_buffer);
}

void
bcmltm_uint64_to_uint32_array(uint64_t field_val64, uint32_t *field_val)
{
    field_val[0] = field_val64 & 0xffffffff;
    field_val[1] = (field_val64 >> 32) & 0xffffffff;
}

void
bcmltm_uint32_array_to_uint64(uint32_t *field_val, uint64_t *field_val64)
{
    *field_val64 = (((uint64_t)field_val[1]) << 32) | field_val[0];
}

bcmltm_field_list_t *
bcmltm_lta_find_field(const bcmltd_fields_t *lta_fields,
                      uint32_t field_id,
                      uint32_t field_idx)
{
    int fix, max_fields;
    bcmltd_field_t **lta_fields_ptrs;

    lta_fields_ptrs = lta_fields->field;
    max_fields = lta_fields->count;

    for (fix = 0; fix < max_fields; fix++) {
        if ((lta_fields_ptrs[fix]->id == field_id) &&
            (lta_fields_ptrs[fix]->idx == field_idx)) {
            return lta_fields_ptrs[fix];
        }
    }

    return NULL;
}
