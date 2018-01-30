/*! \file port_vlan_xlate_ing_xlate_tbl_sel_first_lookup.c
 *
 * PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_FIRST_LOOKUP Transform Handler
 *
 * This file contains field transform information for
 * PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_FIRST_LOOKUP.
 *
 * PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_FIRST_LOOKUP corresponds to the field
 * arg->rfield[0](forward transform), arg->field[0](reverse transform).
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
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmport/bcmport_port_vlan_xlate_ing_xlate_tbl_sel_first_lookup.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT

typedef  enum lt_ing_xlate_tbl_sel_e {
    VLAN_ING_XLATE_IVID_OVID = 0,
    VLAN_ING_XLATE_IVID_OVID_MODPORT = 1,
    VLAN_ING_XLATE_IVID_OVID_TRUNK = 2,
    VLAN_ING_XLATE_IVID_OVID_PORT_GRP = 3,
    VLAN_ING_XLATE_OTAG = 4,
    VLAN_ING_XLATE_OTAG_MODPORT = 5,
    VLAN_ING_XLATE_OTAG_TRUNK = 6,
    VLAN_ING_XLATE_OTAG_PORT_GRP = 7,
    VLAN_ING_XLATE_ITAG = 8,
    VLAN_ING_XLATE_ITAG_MODPORT = 9,
    VLAN_ING_XLATE_ITAG_TRUNK = 10,
    VLAN_ING_XLATE_ITAG_PORT_GRP = 11,
    VLAN_ING_XLATE_OVID = 12,
    VLAN_ING_XLATE_OVID_MODPORT = 13,
    VLAN_ING_XLATE_OVID_TRUNK = 14,
    VLAN_ING_XLATE_OVID_PORT_GRP = 15,
    VLAN_ING_XLATE_IVID = 16,
    VLAN_ING_XLATE_IVID_MODPORT = 17,
    VLAN_ING_XLATE_IVID_TRUNK = 18,
    VLAN_ING_XLATE_IVID_PORT_GRP = 19,
    VLAN_ING_XLATE_CFI_PRI = 20,
    VLAN_ING_XLATE_CFI_PRI_MODPORT = 21,
    VLAN_ING_XLATE_CFI_PRI_TRUNK = 22,
    VLAN_ING_XLATE_CFI_PRI_PORT_GRP = 23
} lt_ing_xlate_tbl_sel_t;

typedef enum key_type_e {
    IVID_OVID = 0,
    OTAG = 1,
    ITAG = 2,
    OVID = 4,
    IVID = 5,
    CFI_PRI = 6
} key_type_t;

typedef enum port_type_e {
    ANY_TYPE = 0,
    MODPORT = 1,
    TRUNK = 1,
    SVP = 2,
    PORT_GRP = 3
} port_type_t;

typedef struct ing_xlate_tbl_sel_info_s {
    lt_ing_xlate_tbl_sel_t   tbl_sel;
    key_type_t               key_type;
    port_type_t              port_type;
} ing_xlate_tbl_sel_info_t;

static ing_xlate_tbl_sel_info_t ing_xlate_tbl_sel_info[] = {
    {VLAN_ING_XLATE_IVID_OVID,              IVID_OVID,        ANY_TYPE},
    {VLAN_ING_XLATE_IVID_OVID_MODPORT,      IVID_OVID,        MODPORT},
    {VLAN_ING_XLATE_IVID_OVID_TRUNK,        IVID_OVID,        TRUNK},
    {VLAN_ING_XLATE_IVID_OVID_PORT_GRP,     IVID_OVID,        PORT_GRP},
    {VLAN_ING_XLATE_OTAG,                   OTAG,             ANY_TYPE},
    {VLAN_ING_XLATE_OTAG_MODPORT,           OTAG,             MODPORT},
    {VLAN_ING_XLATE_OTAG_TRUNK,             OTAG,             TRUNK},
    {VLAN_ING_XLATE_OTAG_PORT_GRP,          OTAG,             PORT_GRP},
    {VLAN_ING_XLATE_ITAG,                   ITAG,             ANY_TYPE},
    {VLAN_ING_XLATE_ITAG_MODPORT,           ITAG,             MODPORT},
    {VLAN_ING_XLATE_ITAG_TRUNK,             ITAG,             TRUNK},
    {VLAN_ING_XLATE_ITAG_PORT_GRP,          ITAG,             PORT_GRP},
    {VLAN_ING_XLATE_OVID,                   OVID,             ANY_TYPE},
    {VLAN_ING_XLATE_OVID_MODPORT,           OVID,             MODPORT},
    {VLAN_ING_XLATE_OVID_TRUNK,             OVID,             TRUNK},
    {VLAN_ING_XLATE_OVID_PORT_GRP,          OVID,             PORT_GRP},
    {VLAN_ING_XLATE_IVID,                   IVID,             ANY_TYPE},
    {VLAN_ING_XLATE_IVID_MODPORT,           IVID,             MODPORT},
    {VLAN_ING_XLATE_IVID_TRUNK,             IVID,             TRUNK},
    {VLAN_ING_XLATE_IVID_PORT_GRP,          IVID,             PORT_GRP},
    {VLAN_ING_XLATE_CFI_PRI,                CFI_PRI,          ANY_TYPE},
    {VLAN_ING_XLATE_CFI_PRI_MODPORT,        CFI_PRI,          MODPORT},
    {VLAN_ING_XLATE_CFI_PRI_TRUNK,          CFI_PRI,          TRUNK},
    {VLAN_ING_XLATE_CFI_PRI_PORT_GRP,       CFI_PRI,          PORT_GRP}
};

/*
 * \brief PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_FIRST_LOOKUP forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_FIRST_LOOKUP field.
 * \param [out] out             PORT_TAB.VT_PORT_TYPE_SELECT/VT_KEY_TYPE fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmport_port_vlan_xlate_ing_xlate_tbl_sel_first_lookup_transform(int unit,
                                                                 const bcmltd_fields_t *in,
                                                                 bcmltd_fields_t *out,
                                                                 const bcmltd_transform_arg_t *arg)
{
    size_t i;
    int rv = SHR_E_NONE;
    uint32_t key_type_id = VT_KEY_TYPEf;
    uint32_t port_type_id = VT_PORT_TYPE_SELECTf;
    uint64_t key_type_val = 0;
    uint64_t port_type_val = 0;
    int found = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmport_port_vlan_xlate_ing_xlate_tbl_sel_first_lookup_transform\n")));

    if (in->count == 0) {
        goto exit;
    }

    for (i = 0;
         i < (sizeof(ing_xlate_tbl_sel_info) / sizeof(ing_xlate_tbl_sel_info[0]));
         i++) {
        if (in->field[0]->data == (uint64_t)ing_xlate_tbl_sel_info[i].tbl_sel) {
            key_type_val  = (uint64_t)ing_xlate_tbl_sel_info[i].key_type;
            port_type_val = (uint64_t)ing_xlate_tbl_sel_info[i].port_type;
            found = 1;
            break;
        }
    }

    if (!found) {
        rv = SHR_E_PARAM;
        SHR_RETURN_VAL_EXIT(rv);
    }

    out->count = 0;
    out->field[out->count]->id   = key_type_id;
    out->field[out->count]->data = key_type_val;
    out->count++;
    out->field[out->count]->id   = port_type_id;
    out->field[out->count]->data = port_type_val;
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_FIRST_LOOKUP reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_TAB/VT_PORT_TYPE_SELECT/VT_KEY_TYPE fields.
 * \param [out] out             PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_FIRST_LOOKUP field.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmport_port_vlan_xlate_ing_xlate_tbl_sel_first_lookup_rev_transform(int unit,
                                                                     const bcmltd_fields_t *in,
                                                                     bcmltd_fields_t *out,
                                                                     const bcmltd_transform_arg_t *arg)
{
    size_t i;
    int rv = SHR_E_NONE;
    uint32_t key_type_id = VT_KEY_TYPEf;
    uint32_t port_type_id = VT_PORT_TYPE_SELECTf;
    uint64_t key_type_val = 0;
    uint64_t port_type_val = 0;
    int found = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmport_port_vlan_xlate_ing_xlate_tbl_sel_first_lookup_rev_transform\n")));

    if (in->count == 0) {
        goto exit;
    }

    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == key_type_id) {
            key_type_val = in->field[i]->data;
        } else if (in->field[i]->id == port_type_id) {
            port_type_val = in->field[i]->data;
        } else {
            rv = SHR_E_FAIL;
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    out->count = 0;
    for (i = 0;
         i < (sizeof(ing_xlate_tbl_sel_info) / sizeof(ing_xlate_tbl_sel_info[0]));
         i++) {
        if (key_type_val == (uint64_t)ing_xlate_tbl_sel_info[i].key_type &&
            port_type_val == (uint64_t)ing_xlate_tbl_sel_info[i].port_type) {
            out->field[out->count]->id = arg->rfield[0];
            out->field[out->count]->data = (uint64_t)ing_xlate_tbl_sel_info[i].tbl_sel;
            out->count++;
            found = 1;
            break;
        }
    }

    if (!found) {
        rv = SHR_E_PARAM;
        SHR_RETURN_VAL_EXIT(rv);
    }

 exit:
    SHR_FUNC_EXIT();
}
