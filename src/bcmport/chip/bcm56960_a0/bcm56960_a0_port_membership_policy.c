/*! \file bcm56960_a0_port_membership_policy.c
 *
 * Miscellaneous utilities for PORT_MEMBERSHIP_POLICY.
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
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm.h>
#include <bcmport/bcmport_util.h>
#include "bcmport/chip/bcmport_membership_policy_chip.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPORT_PORT_MEMBERSHIP_POLICY


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static int
bcm56960_a0_port_tab_fields_set(int unit, uint32_t trans_id,
                                bcmltd_sid_t lt_id, shr_port_t port,
                                port_membership_policy_info_t *lt_info)
{
    PORT_TABm_t port_tab_buf;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&port_tab_buf, 0, sizeof(port_tab_buf));
    /* Read original entry. */
    buf_ptr = (uint32_t *)&port_tab_buf;
    SHR_IF_ERR_EXIT(
        bcmport_pt_read(unit, trans_id, lt_id,
                        PORT_TABm, port, buf_ptr));

    /* Set ing_vlan_membership_check. */
    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_ING_VLAN_MEMBERSHIP_CHECK)) {
        field_value = lt_info->ing_vlan_membership_check;
        bcmdrd_pt_field_set(unit, PORT_TABm, buf_ptr, EN_IFILTERf, &field_value);
    }

    /* Set up hardware table index to write. */
    SHR_IF_ERR_EXIT(
            bcmport_pt_write(unit, trans_id, lt_id,
                             PORT_TABm, port, buf_ptr));

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56960_a0_egr_port_fields_set(int unit, uint32_t trans_id,
                                bcmltd_sid_t lt_id, shr_port_t port,
                                port_membership_policy_info_t *lt_info)
{
    EGR_PORTm_t egr_port_tab_buf;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&egr_port_tab_buf, 0, sizeof(egr_port_tab_buf));

    /* Read original entry. */
    buf_ptr = (uint32_t *)&egr_port_tab_buf;
    SHR_IF_ERR_EXIT(
        bcmport_pt_read(unit, trans_id, lt_id,
                        EGR_PORTm, port, buf_ptr));

    /* Set egr_vlan_membership_check. */
    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_EGR_VLAN_MEMBERSHIP_CHECK)) {
        field_value = lt_info->egr_vlan_membership_check;
        bcmdrd_pt_field_set(unit, EGR_PORTm, buf_ptr, EN_EFILTERf,
                            &field_value);
    }
    /* Set up hardware table index to write. */
    SHR_IF_ERR_EXIT(
            bcmport_pt_write(unit, trans_id, lt_id,
                             EGR_PORTm, port, buf_ptr));

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56960_a0_ing_en_efilter_bmp_fields_set(int unit, uint32_t trans_id,
                                          bcmltd_sid_t lt_id, shr_port_t port,
                                          port_membership_policy_info_t *lt_info)
{
    ING_EN_EFILTER_BITMAPm_t ing_en_efilter_bmp_buf;
    bcmdrd_pbmp_t pbmp;
    uint32_t * buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&ing_en_efilter_bmp_buf, 0, sizeof(ing_en_efilter_bmp_buf));

    /* Read original entry. */
    buf_ptr = (uint32_t *)&ing_en_efilter_bmp_buf;
    SHR_IF_ERR_EXIT(
        bcmport_pt_read(unit, trans_id, lt_id,
                        ING_EN_EFILTER_BITMAPm, 0, buf_ptr));

    ING_EN_EFILTER_BITMAPm_BITMAPf_GET(ing_en_efilter_bmp_buf, pbmp.w);

    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_EGR_VLAN_MEMBERSHIP_CHECK)) {
        if (lt_info->egr_vlan_membership_check) {
            BCMDRD_PBMP_PORT_ADD(pbmp, lt_info->port_id);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(pbmp, lt_info->port_id);
        }
    }

    /* Set egr_vlan_membership_check. */
    ING_EN_EFILTER_BITMAPm_BITMAPf_SET(ing_en_efilter_bmp_buf, pbmp.w);

    SHR_IF_ERR_EXIT(
            bcmport_pt_write(unit, trans_id, lt_id,
                             ING_EN_EFILTER_BITMAPm, 0, buf_ptr));

exit:
    SHR_FUNC_EXIT();

}

int
bcm56960_a0_port_membership_policy_set(int unit, uint32_t trans_id,
                                       bcmltd_sid_t lt_id, shr_port_t port,
                                       port_membership_policy_info_t *lt_info)
{
    bool port_tab_update = false;
    bool ing_en_efilter_bitmap_update = false;
    bool egr_port_update = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_EGR_VLAN_MEMBERSHIP_CHECK)) {
        ing_en_efilter_bitmap_update = true;
        egr_port_update = true;
    }

    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_ING_VLAN_MEMBERSHIP_CHECK) ||
        BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_SKIP_VLAN_CHECK)) {
        port_tab_update = true;
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcm56960_a0_port_membership_policy_set.\n")));

    if (port_tab_update) {
        SHR_IF_ERR_EXIT(
            bcm56960_a0_port_tab_fields_set(unit, trans_id, lt_id, port, lt_info));
    }

    if (egr_port_update) {
        SHR_IF_ERR_EXIT(
            bcm56960_a0_egr_port_fields_set(unit, trans_id, lt_id, port, lt_info));
    }

    if (ing_en_efilter_bitmap_update) {
       SHR_IF_ERR_EXIT(
           bcm56960_a0_ing_en_efilter_bmp_fields_set(unit, trans_id, lt_id, port, lt_info));
    }
exit:
    SHR_FUNC_EXIT();
}
