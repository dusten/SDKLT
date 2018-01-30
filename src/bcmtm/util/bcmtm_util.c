/*! \file bcmtm_util.c
 *
 * TM Driver utility functions.
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

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
/*******************************************************************************
* Local definitions
 */

/*! TM port operation handler function. */
typedef int
(*tm_port_op_f)(int unit, bcmtm_pport_t pport, uint32_t param);

/*! This structure is used to define the operation for the TM. */
typedef struct tm_port_op_hdl_s {

    /*! Operation name. */
    char *name;

    /*! Operation handler. */
    tm_port_op_f func;

} tm_port_op_hdl_t;


/*******************************************************************************
* Private functions
 */

/*!
 * \brief Execute ingress path up/down sequence for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *                sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_ing_up(int unit, bcmtm_pport_t port, uint32_t up)
{
    if (up) {
        return bcmtm_port_ing_up(unit, port);
    }

    return bcmtm_port_ing_down(unit, port);
}

/*!
 * \brief Execute MMU path up/down sequence for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *                sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_mmu_up(int unit, bcmtm_pport_t port, uint32_t up)
{
    if (up) {
        return bcmtm_port_mmu_up(unit, port);
    }

    return bcmtm_port_mmu_down(unit, port);
}

/*!
 * \brief Execute egress path up/down sequence for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *                sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_egr_up(int unit, bcmtm_pport_t port, uint32_t up)
{
    if (up) {
        return bcmtm_port_egr_up(unit, port);
    }

    return bcmtm_port_egr_down(unit, port);
}


/*******************************************************************************
* Public functions
 */

bcmdrd_sid_t
bcmtm_pt_sid_unique_xpe_pipe(int unit, bcmdrd_sid_t sid, int xpe, int pipe)
{
    bcmtm_drv_t *tm_drv;

    (void)(bcmtm_drv_get(unit, &tm_drv));

    return (tm_drv->pt_sid_uniq_acc(unit, sid, xpe, pipe));
}

int
bcmtm_port_num_mc_cosq_get(int unit,
        bcmtm_lport_t lport,
        int *num_mc_q)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *num_mc_q = drv_info->num_mc_cosq[lport];
    if (*num_mc_q == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_num_uc_cosq_get(int unit,
        bcmtm_lport_t lport,
        int *num_uc_q)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *num_uc_q = drv_info->num_uc_cosq[lport];
    if (*num_uc_q == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_mc_cosq_base_get(int unit,
        bcmtm_lport_t lport,
        int *mc_q_base)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *mc_q_base = drv_info->mc_cosq_base[lport];
    if (*mc_q_base == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_uc_cosq_base_get(int unit,
        bcmtm_lport_t lport,
        int *uc_q_base)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *uc_q_base = drv_info->uc_cosq_base[lport];
    if (*uc_q_base == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_is_cpu(int unit, bcmtm_lport_t lport)
{
    int i;
    bcmtm_drv_info_t *drv_info;
    bcmtm_drv_info_get(unit, &drv_info);
    for (i = 0; i < MAX_NUM_CPU_PORTS; i++) {
        if ((drv_info->cpu_ports[i]).log_port == lport) {
           return TRUE;
        }
    }
    return FALSE;
}

int
bcmtm_port_is_lb(int unit, bcmtm_lport_t lport)
{
    int i;
    bcmtm_drv_info_t *drv_info;
    bcmtm_drv_info_get(unit, &drv_info);
    for (i = 0; i < MAX_NUM_LB_MGMT_PORTS; i++) {
        if ((drv_info->lb_ports[i]).log_port == lport) {
            return TRUE;
        }
    }
    return FALSE;
}

int
bcmtm_port_is_mgmt(int unit, bcmtm_lport_t lport)
{
    int i;
    bcmtm_drv_info_t *drv_info;
    bcmtm_drv_info_get(unit, &drv_info);
    for (i = 0; i < MAX_NUM_LB_MGMT_PORTS; i++) {
        if ((drv_info->mgmt_ports[i]).log_port == lport) {
            return TRUE;
        }
    }
    return FALSE;
}

int
bcmtm_port_is_fp(int unit, bcmtm_lport_t lport)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_drv_info_get(unit, &drv_info);
    if ((bcmtm_port_is_mgmt(unit,  lport)) ||
        (bcmtm_port_is_lb(unit, lport))||
        (bcmtm_port_is_cpu(unit, lport))||
        (drv_info->port_l2p_mapping[lport] == -1)){
        return FALSE;
    }
    return TRUE;
}

int
bcmtm_port_mmu_port_get(int unit,
        bcmtm_lport_t lport,
        int *mmu_port)
{
    bcmtm_drv_info_t *drv_info;
    int pport;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    pport = drv_info->port_l2p_mapping[lport];
    if (pport == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    *mmu_port = drv_info->port_p2m_mapping[pport];
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_mmu_chip_port_get(int unit,
        bcmtm_lport_t lport,
        int *mmu_chip_port)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *mmu_chip_port = drv_info->port_l2m_chip_mapping[lport];
    if (*mmu_chip_port == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_idb_port_get(int unit,
        bcmtm_lport_t lport,
        int* idb_port)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *idb_port = drv_info->port_l2i_mapping[lport];
    if (*idb_port == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_phys_port_get(int unit,
        bcmtm_lport_t lport,
        int *pport)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *pport = drv_info->port_l2p_mapping[lport];
    if (*pport == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_speed_max_get(int unit,
        bcmtm_lport_t lport,
        int *max_port_speed)
{
    bcmtm_drv_info_t *drv_info;
    int pport;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    pport = drv_info->port_l2p_mapping[lport];
    if (pport == -1) {
        *max_port_speed = -1;
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    *max_port_speed = drv_info->port_speed_max[lport];
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_pipe_get(int unit,
        bcmtm_lport_t lport,
        int *pipe)
{
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    *pipe = drv_info->port_pipe_mapping[lport];
    if (*pipe == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_mmu_local_port_get(int unit,
        bcmtm_lport_t lport,
        int *mmu_lport)
{
    bcmtm_drv_info_t *drv_info;
    int pport;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    pport = drv_info->port_l2p_mapping[lport];
    if (pport == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    *mmu_lport = drv_info->port_p2m_local_mapping[pport];
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_ing_down(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_VERBOSE_CHECK
        (tm_drv->port_ing_down, SHR_E_NONE);
    SHR_RETURN_VAL_EXIT
        (tm_drv->port_ing_down(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_ing_up(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_VERBOSE_CHECK
        (tm_drv->port_ing_up, SHR_E_NONE);
    SHR_RETURN_VAL_EXIT
        (tm_drv->port_ing_up(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_mmu_down(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_VERBOSE_CHECK
        (tm_drv->port_mmu_down, SHR_E_NONE);
    SHR_RETURN_VAL_EXIT
        (tm_drv->port_mmu_down(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_mmu_up(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_VERBOSE_CHECK
        (tm_drv->port_mmu_up, SHR_E_NONE);
    SHR_RETURN_VAL_EXIT
        (tm_drv->port_mmu_up(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_egr_down(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_VERBOSE_CHECK
        (tm_drv->port_egr_down, SHR_E_NONE);
    SHR_RETURN_VAL_EXIT
        (tm_drv->port_egr_down(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_egr_up(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_VERBOSE_CHECK
        (tm_drv->port_egr_up, SHR_E_NONE);
    SHR_RETURN_VAL_EXIT
        (tm_drv->port_egr_up(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_op_exec(int unit, bcmpc_pport_t pport,
                   char *op_name, uint32_t op_param)
{
    static tm_port_op_hdl_t port_ops[] = {
        { "ing_up", tm_port_ing_up },
        { "mmu_up", tm_port_mmu_up },
        { "egr_up", tm_port_egr_up },
    };
    tm_port_op_hdl_t *op_hdl;
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < COUNTOF(port_ops); i++) {
        op_hdl = &port_ops[i];

        if (sal_strcmp(op_name, op_hdl->name) != 0) {
            continue;
        }

        SHR_RETURN_VAL_EXIT
            (op_hdl->func(unit, pport, op_param));
    }

    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Operation not supported[%s]\n"), op_name));

    SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

void
bcmtm_field_populate(int unit,
        bcmltd_fields_t *flist,
        bcmltd_fid_t fid,
        uint32_t idx,
        uint64_t fval)
{
    int count = 0;
    count = flist->count;
    flist->field[count]->id = fid;
    flist->field[count]->data = fval;
    flist->field[count]->idx= idx;
    flist->count++;
}

int
bcmtm_fval_get_from_field_array (int unit,
        bcmlrd_fid_t fid,
        const bcmltd_field_t *field_array,
        uint64_t *fval)
{
    bcmltd_field_t *field = (bcmltd_field_t *)field_array;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(field, SHR_E_NOT_FOUND);

    while(field) {
        if (field->id == fid) {
            *fval = field->data;
            break;
        }
        field =  field->next;
    }
    if (!field) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

