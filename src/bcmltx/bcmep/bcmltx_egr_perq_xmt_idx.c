/*! \file bcmltx_egr_perq_xmt_idx.c
 *
 * EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE Transform Handler
 *
 * This file contains field transform information for
 * EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE.
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
#include <bcmltx/bcmep/bcmltx_egr_perq_xmt_idx.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmtm/bcmtm_drv.h>

#define Q_TYPE_UC  (0)
#define Q_TYPE_MC  (1)
#define Q_TYPE_CPU (2)

int
bcmltx_egr_perq_xmt_idx_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    int pipe, index, q_type, lport;
    int cpu_cosq_num, gp_cosq_num;
    int q, uc_cosq_num, mc_cosq_num, lmport;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LS_BCMLTX_EP, (BSL_META(
                "bcmltx_egr_perq_xmt_idx_transform \n")));

    if ((in->count != 2)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 3) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    lport = in->field[0]->data;
    pipe = bcmdrd_dev_logic_port_pipe(unit, lport);
    if (pipe < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    q                   = in->field[1]->data;
    q_type              = arg->value[0];
    cpu_cosq_num        = arg->value[1];
    gp_cosq_num         = arg->value[2];

    SHR_IF_ERR_EXIT(bcmtm_port_num_uc_cosq_get(unit, lport, &uc_cosq_num));
    mc_cosq_num = gp_cosq_num - uc_cosq_num;

    if (bcmtm_port_is_cpu(unit, lport)) {
        if (q_type == Q_TYPE_MC) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        q_type = Q_TYPE_CPU;
    } else if (q_type == Q_TYPE_UC) {
        if (q >= uc_cosq_num) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    } else if (q_type == Q_TYPE_MC) {
        if (q >= mc_cosq_num) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    switch(q_type) {
        case Q_TYPE_CPU:
            break;
        case Q_TYPE_MC:
            q += uc_cosq_num;
            /* fall-through */
        case Q_TYPE_UC:
            q += cpu_cosq_num;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

    SHR_IF_ERR_EXIT(bcmtm_port_mmu_local_port_get(unit, lport, &lmport));
    index = lmport * gp_cosq_num + q;

    out->count=0;
    /* ouput __INDEX */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->data = index;
    out->count++;
    /* ouput __INSTANCE */
    out->field[out->count]->id   = arg->rfield[1];
    out->field[out->count]->data = pipe;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_egr_perq_xmt_idx_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    int tport, lmport, lport = -1, q_diff, q = -1;
    int instance, index, cpu_cosq_num, uc_cosq_num, gp_cosq_num;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
    bcmdrd_port_num_domain_t pnd = BCMDRD_PND_LOGIC;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LS_BCMLTX_EP, (BSL_META(
                "bcmltx_egr_perq_xmt_idx_rev_transform \n")));

    if ((in->count != 2)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 3) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    index               = in->field[0]->data;
    instance            = in->field[1]->data;
    cpu_cosq_num        = arg->value[1];
    gp_cosq_num         = arg->value[2];

    SHR_IF_ERR_EXIT(bcmdrd_dev_valid_ports_get(unit, &pbmp));
    BCMDRD_PBMP_ITER(pbmp, tport) {
        if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], tport)) {
            continue;
        }
        if(bcmdrd_dev_logic_port_pipe(unit, tport) == instance) {
            if (bcmtm_port_is_cpu(unit, tport) &&
                index < cpu_cosq_num) {
                lport = tport;
                q = index;
                break;
            }
            if(bcmtm_port_mmu_local_port_get(unit, tport, &lmport) != SHR_E_NONE) {
                continue;
            }
            q_diff = index - cpu_cosq_num - (lmport * gp_cosq_num);
            if((q_diff >= 0) && (q_diff < gp_cosq_num)) {
                lport = tport;
                SHR_IF_ERR_EXIT(
                    bcmtm_port_num_uc_cosq_get(unit, lport, &uc_cosq_num));
                q = (q_diff < uc_cosq_num)? q_diff: (q_diff - uc_cosq_num);
                break;
            }
        }
    }

    out->count=0;
    /* ouput PORT_ID */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->data = lport;
    out->count++;
    /* ouput MC/UC_Q */
    out->field[out->count]->id   = arg->rfield[1];
    out->field[out->count]->data = q;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

