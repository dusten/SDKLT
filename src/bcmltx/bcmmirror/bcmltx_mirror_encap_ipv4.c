/*! \file bcmltx_mirror_encap_ipv4.c
 *
 * Mirror Encap IPV4 Handler.
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
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmltx/bcmmirror/bcmltx_mirror_encap_ipv4.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_COMMON

int
bcmltx_mirror_encap_ipv4_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_ipv4_transform \n")));

    if ((in->count < 3) || (arg->rfields < 6)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    out->count=0;
    /*! Source IPv4. */
    out->field[0]->data = (uint32_t)(in->field[0]->data >> 32);
    out->field[0]->id   = arg->rfield[out->count];
    out->field[0]->idx  = 0;
    out->count++;

    /*! Destination IPv4. */
    out->field[1]->data = (uint32_t)in->field[0]->data;
    out->field[1]->id   = arg->rfield[out->count];
    out->field[1]->idx  = 0;
    out->count++;

    /*! Type of service. */
    out->field[2]->data = (in->field[2]->data >> 16)& 0xFF;
    out->field[2]->id   = arg->rfield[out->count];
    out->field[2]->idx  = 0;
    out->count++;

    /*! Time to live. */
    out->field[3]->data = (in->field[1]->data >> 24)& 0xFF;
    out->field[3]->id   = arg->rfield[out->count];
    out->field[3]->idx  = 0;
    out->count++;

    /*! Proto. */
    out->field[4]->data = (in->field[1]->data >> 16)& 0xFF;
    out->field[4]->id   = arg->rfield[out->count];
    out->field[4]->idx  = 0;
    out->count++;

    /*! Do not fragment. */
    out->field[5]->data = (in->field[1]->data >> 46)& 0x1;
    out->field[5]->id   = arg->rfield[out->count];
    out->field[5]->idx  = 0;
    out->count++;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mirror_encap_ipv4_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t  src_ipv4, dst_ipv4;
    uint8_t   tos, ttl, dnf, proto;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_ipv4_transform \n")));

    if ((in->count < 6) || (arg->rfields < 1)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    src_ipv4   = in->field[0]->data;
    dst_ipv4   = in->field[1]->data;
    tos        = in->field[2]->data;
    ttl        = in->field[3]->data;
    proto      = in->field[4]->data;
    dnf        = in->field[5]->data;

    /*
     * ERSPAN::ERSPAN_HEADER_V4 [191:32] bits
     * [ 95:32]  mapped to out->field[0] [63:0]
     * [159:96]  mapped to out->field[1] [63:0]
     * [191:160] mapped to out->field[2] [31:0]
     */
    out->count=0;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->idx  = out->count;
    out->field[0]->data = ((uint64_t)src_ipv4 << 32) | dst_ipv4;
    out->count++;

    out->field[1]->id   = arg->rfield[0];
    out->field[1]->idx  = out->count;
    out->field[1]->data = ((uint64_t) ttl << 24) |
                          ((uint64_t) proto << 16) | ((uint64_t)dnf << 46);
    out->count++;

    out->field[2]->id   = arg->rfield[0];
    out->field[2]->idx  = out->count;
    out->field[2]->data = ((uint64_t)0x45 << 24) | ((uint64_t)tos << 16);
    out->count++;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

 exit:
    SHR_FUNC_EXIT();
}
