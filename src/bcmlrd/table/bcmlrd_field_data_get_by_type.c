/*! \file bcmlrd_field_data_get_by_type.c
 *
 * Get field data by field data type.
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
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_table.h>

/*!
 * \brief Get field value from bcmltd_field_data_t based on the dtag.
 *
 * \param [in] dtag     Dtag.
 * \param [in] fd       Pointer to field data.
 *
 * \retval default/min/max value of the field.
 */
static uint64_t
field_value_by_dtag(bcmltd_field_data_tag_t dtag,
                    bcmltd_field_data_t *fd)
{
    uint64_t value = 0;

    switch (dtag) {
    case BCMLT_FIELD_DATA_TAG_BOOL:
        value = fd->is_true;
        break;
    case BCMLT_FIELD_DATA_TAG_U8:
        value = fd->u8;
        break;
    case BCMLT_FIELD_DATA_TAG_U16:
        value = fd->u16;
        break;
    case BCMLT_FIELD_DATA_TAG_U32:
        value = fd->u32;
        break;
    case BCMLT_FIELD_DATA_TAG_U64:
        value = fd->u64;
        break;
    default:
        /* This function is for narrow field */
        break;
    }

    return value;
}

#define DBYTE(x) ((uint64_t) fbb[x])

int
bcmlrd_field_data_get_by_type(int unit,
                              const bcmlrd_sid_t sid,
                              const bcmlrd_fid_t fid,
                              const bcmlrd_field_data_type_t type,
                              const uint32_t  num_alloc,
                              uint64_t* field_def,
                              uint32_t* num_actual)
{
    bcmlrd_field_def_t fdef;
    uint32_t i, max;
    uint64_t val;
    uint8_t *fbb;
    uint32_t num_wide;
    int rv;

    rv = bcmlrd_table_field_def_get(unit, sid, fid, &fdef);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    switch(type) {
    case BCMLRD_FIELD_DATA_TYPE_MIN:
        val = field_value_by_dtag(fdef.dtag, &fdef.min);
        fbb = fdef.min.binary.bytes;
        break;
    case BCMLRD_FIELD_DATA_TYPE_MAX:
        val = field_value_by_dtag(fdef.dtag, &fdef.max);
        fbb = fdef.max.binary.bytes;
        break;
    case BCMLRD_FIELD_DATA_TYPE_DEFAULT:
    default:
        val = field_value_by_dtag(fdef.dtag, &fdef.def);
        fbb = fdef.def.binary.bytes;
        break;
    }

    *num_actual = 0;
    if (fdef.depth > 0) {
        max = (num_alloc < fdef.depth) ? num_alloc
                                       : fdef.depth;
        for (i = 0; i < max; i++) {
            field_def[i] = val;
            (*num_actual)++;
        }
    } else if (fdef.width <= 64) {
        field_def[0] = val;
        *num_actual = 1;
    } else {
        num_wide = (fdef.width + (BCM_FIELD_SIZE_BITS - 1)) / BCM_FIELD_SIZE_BITS;
        max = (num_alloc < num_wide) ? num_alloc : num_wide;
        for (i = 0; i < max; i++) {
            field_def[i] = (DBYTE(8*i + 7) << 8*7) |
                           (DBYTE(8*i + 6) << 8*6) |
                           (DBYTE(8*i + 5) << 8*5) |
                           (DBYTE(8*i + 4) << 8*4) |
                           (DBYTE(8*i + 3) << 8*3) |
                           (DBYTE(8*i + 2) << 8*2) |
                           (DBYTE(8*i + 1) << 8*1) |
                           (DBYTE(8*i));
            (*num_actual)++;
        }
    }

    return SHR_E_NONE;
}

