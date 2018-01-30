/*! \file bcmltx_std_table_validator.c
 *
 * Built-in Field Validation functions -
 *
 * 1) Input field list validation (deprecated, now in LTM core)
 * 2) Port bitmap array validation.
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
#include <shr/shr_bitop.h>
#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmltx/bcmltx_std_table_validator.h>
#include <bcmltx/bcmltx_util.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_COMMON

int
bcmltx_std_table_validator(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_field_val_arg_t *arg)
{
    uint32_t aix, fid;
    uint32_t found_count = 0;
    bcmltx_field_list_t *field_ptr;
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmlrd_map_t *map = NULL;
    bcmlrd_sid_t sid;
    uint32_t idx, idx_count;

    SHR_FUNC_ENTER(unit);

    if ((arg->field == NULL) ||
        (arg->fields == 0) ||
        (arg->comp_data == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid field validation information\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    sid = arg->comp_data->sid;
    tbl = bcmlrd_table_get(sid);
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));
    if ((tbl == NULL) || (map == NULL)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not get LRD table,map representation: "
                              "sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Wide Field */
    for (idx = 0; idx < arg->fields; idx++) {
        fid = arg->field[idx];
        found_count = 0;

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        for (aix = 0; aix < idx_count; aix++) {
            field_ptr = bcmltx_lta_find_field(in, fid, aix);
            if (field_ptr == NULL) {
                /* Missing array element in list */
                if (fid < tbl->fields) {
                    /* Field validation failure */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                          "LTX %s: Missing field value"
                                          " for field %s[%d]\n"),
                                 tbl->name,
                                 tbl->field[fid].name,
                                 aix));
                }
            } else {
                found_count++;
            }
        }
        if (found_count != 0 &&
            found_count != idx_count) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    /* READ ONLY */
    if ((opcode == BCMLT_OPCODE_INSERT) ||
        (opcode == BCMLT_OPCODE_UPDATE)) {

        uint32_t flags;
        uint32_t error = 0;

        for (idx = 0; idx < arg->fields; idx++) {
            fid = arg->field[idx];

            field_ptr = bcmltx_lta_find_field(in, fid, 0);
            if (field_ptr != NULL) {

                flags = map->field_data->field[fid].flags;
                if (flags & BCMLRD_FIELD_F_READ_ONLY) {
                    error++;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                          "LTX %s: Read only field %s\n"),
                                 tbl->name,
                                 tbl->field[fid].name));
                }
            }
        }

        if (error > 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_lta_val_port_bitmap(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_field_val_arg_t *arg)
{
    int rv;
    uint32_t port, port_num, fid, aix;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltx_field_list_t *field_ptr;
    const bcmlrd_table_rep_t *tbl;

    if ((arg->fields != 3) ||
        (arg->comp_data == NULL)) {
        /* Not supported yet */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Inconsistent LTA information\n")));
        return FALSE;
    }

    if (arg->field[0] == BCMLTX_FAVC_PORT_LOGICAL) {
        port_num = arg->field[1];
        fid = arg->field[2];
        for (port = 0; port < port_num; port++) {
            if ((port % BCM_FIELD_SIZE_BITS) == 0) {
                /* New field index needed */
                aix = port / BCM_FIELD_SIZE_BITS;
                field_ptr = bcmltx_lta_find_field(in, fid, aix);
                if (field_ptr == NULL) {
                    /* Missing array element in list */
                    tbl = bcmlrd_table_get(arg->comp_data->sid);
                    if ((tbl != NULL) &&
                        (fid < tbl->fields)) {
                        /* Field validation failure */
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit,
                                                "LTM %s: Missing field value"
                                                " for field %s[%d]\n"),
                                     tbl->name,
                                     tbl->field[fid].name,
                                     aix));
                    }
                    return SHR_E_PARAM;
                }
                sal_memset(field_val, 0, sizeof(field_val));
                rv = bcmltx_field_data_to_uint32_array(field_ptr,
                                                       field_val);
                if (SHR_E_NONE != rv) {
                    return rv;
                }
            }
            if (SHR_BITGET(field_val, (port % BCM_FIELD_SIZE_BITS))) {
                if (bcmpc_lport_to_pport(unit, port) == BCMPC_INVALID_PPORT) {
                    return FALSE;
                }
            }
        }
    } else {
        /* Not supported yet */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Port Space %d not supported in pbm check\n"),
                                arg->field[0]));
        return FALSE;
    }

    return TRUE;
}

