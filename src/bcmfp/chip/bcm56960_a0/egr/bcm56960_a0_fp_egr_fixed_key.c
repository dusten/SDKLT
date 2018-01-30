/*! \file bcm56960_a0_fp_egr_fixed_key.c
 *
 * API for fixed key entry/group attributes for Tomahawk(56960_A0)
 * device.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56960_a0_fp.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

#define BCM56960_A0_KEY_MATCH_TYPE_IPV4_SINGLE            (1)
#define BCM56960_A0_KEY_MATCH_TYPE_IPV6_SINGLE            (2)
#define BCM56960_A0_KEY_MATCH_TYPE_IPV6_DOUBLE            (3)
#define BCM56960_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE      (4)
#define BCM56960_A0_KEY_MATCH_TYPE_L2_SINGLE              (5)
#define BCM56960_A0_KEY_MATCH_TYPE_IPV4_IPV6_DOUBLE       (6)
#define BCM56960_A0_KEY_MATCH_TYPE_HIGIG_DOUBLE           (9)
#define BCM56960_A0_KEY_MATCH_TYPE_LOOPBACK_DOUBLE        (10)
#define BCM56960_A0_KEY_MATCH_TYPE_L2_DOUBLE              (11)


int
bcmfp_bcm56960_a0_egress_group_selcode_key_get(int unit,
                                               bcmfp_group_t *fg)
{
    bcmfp_group_type_t port_pkt_type;
    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    port_pkt_type = fg->group_port_pkt_type;

    switch (fg->group_slice_mode) {
        case BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 0;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    fg->ext_codes[0].fpf3 = EFP_KEY2;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 1;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IP:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 3;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    fg->ext_codes[0].fpf3 = EFP_KEY3;
                    fg->ext_codes[1].fpf3 = EFP_KEY2;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY8;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 2;
            fg->ext_codes[1].fpf2 = 2;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_HIGIG_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY6;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_LOOPBACK_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY7;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_FRONT_PACKET_ANY:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 4;
            fg->ext_codes[1].fpf2 = 4;
            break;
        case BCMFP_GROUP_SLICE_MODE_L3_ALT_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    fg->ext_codes[0].fpf3 = EFP_KEY1;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    fg->ext_codes[0].fpf3 = EFP_KEY2;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    fg->ext_codes[0].fpf3 = EFP_KEY8;
                    fg->ext_codes[1].fpf3 = EFP_KEY4;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            /* FPF2 will be used to hold H/W slice mode in
             * EFP_SLICE_CONTROL register.
             */
            fg->ext_codes[0].fpf2 = 5;
            fg->ext_codes[1].fpf2 = 5;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_egress_entry_key_match_type_set(int unit,
                               bcmfp_stage_t *stage,
                               bcmfp_group_id_t group_id,
                               bcmfp_group_slice_mode_t slice_mode,
                               bcmfp_group_type_t  port_pkt_type,
                               uint32_t **ekw)
{
    int key_match_sbit = 0;
    int key_match_width = 4;
    int key_match_mask_sbit = 0;
    uint8_t part_idx = 0;
    uint8_t parts_cnt = 0;
    uint32_t data = 0;
    uint32_t mask = 0xf;
    uint32_t group_flags = 0;
    int minbit;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_group_flags_get(unit, stage->stage_id, group_id, &group_flags));

    SHR_IF_ERR_EXIT(bcmfp_group_parts_count(unit, group_flags, &parts_cnt));

    switch (slice_mode) {
        case BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_ANY:
                    data = BCM56960_A0_KEY_MATCH_TYPE_L2_SINGLE;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV4_SINGLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV6_SINGLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56960_A0_KEY_MATCH_TYPE_L2_SINGLE;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_ANY_SINGLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IP:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV4_SINGLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56960_A0_KEY_MATCH_TYPE_L2_SINGLE;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV6_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56960_A0_KEY_MATCH_TYPE_L2_DOUBLE;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_ANY_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_HIGIG_PACKET_ANY:
                    data = BCM56960_A0_KEY_MATCH_TYPE_HIGIG_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_LOOPBACK_PACKET_ANY:
                    data = BCM56960_A0_KEY_MATCH_TYPE_LOOPBACK_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_FRONT_PACKET_ANY:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_ALT_DOUBLE_WIDE:
            switch (port_pkt_type) {
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV4_L2_L3_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6:
                    data = BCM56960_A0_KEY_MATCH_TYPE_IPV4_IPV6_DOUBLE;
                    break;
                case BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP:
                    data = BCM56960_A0_KEY_MATCH_TYPE_L2_DOUBLE;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            break;

        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

    minbit = bcmdrd_pt_field_minbit(unit, stage->hw_entry_info[0]->sid, KEYf);
    if (minbit < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    key_match_sbit = minbit + 236;

    minbit = bcmdrd_pt_field_minbit(unit, stage->hw_entry_info[0]->sid, MASKf);
    if (minbit < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    key_match_mask_sbit = minbit + 236;

    /* Update each part with the proper key match_type. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        bcmdrd_field_set(ekw[part_idx],
                         key_match_sbit,
                         key_match_sbit + key_match_width -1,
                         &data);
        bcmdrd_field_set(ekw[part_idx],
                         key_match_mask_sbit,
                         key_match_mask_sbit + key_match_width -1,
                         &mask);
    }

exit:

    SHR_FUNC_EXIT();
}
