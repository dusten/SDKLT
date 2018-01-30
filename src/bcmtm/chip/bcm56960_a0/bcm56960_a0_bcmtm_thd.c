/*! \file bcm56960_a0_bcmtm_thd.c
 *
 * TM memory settings for bcm56960_a0 device.
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

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <sal/sal_sleep.h>
#include <sal/sal_alloc.h>
#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicd_mem.h>

#include <bcmbd/chip/bcm56960_a0_acc.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcm56960_a0/bcm56960_a0_bcmtm_types.h>
#include <bcm56960_a0/bcm56960_a0_bcmtm_drv.h>
#include <bcmtm/bcmtm_internal_tdm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_device_info.h>
#include <bcmtm/lt_thd/bcmtm_buffer_thd.h>
#include <bcmcfg/bcmcfg_lt.h>

/*******************************************************************************
 * Local definitions
 */
#define PIPE_RESET_TIMEOUT_MSEC         50

#define JUMBO_MAXSZ                     0x3fe8

#define TH_CELLS_PER_OBM                1012

#define TH_PORTS_PER_PBLK               4
#define TH_PBLKS_PER_PIPE               8
#define TH_PIPES_PER_DEV                4
#define TH_PBLKS_PER_DEV                (TH_PBLKS_PER_PIPE * TH_PIPES_PER_DEV)


#define TH_PORTS_PER_PIPE               (TH_PORTS_PER_PBLK * TH_PBLKS_PER_PIPE)

#define TH_NUM_LAYERS  2
#define TH_NUM_XPES    4
#define TH_NUM_PIPES   4


#define TH_MMU_NUM_RQE_QUEUES           11

#define MMU_CFG_MMU_BYTES_TO_CELLS(_byte, _cellhdr) \
            (((_byte) + (_cellhdr) - 1) / (_cellhdr))

struct bcmtm_dev_buf_info bcmtm_dev_buf_info = {
    .mmu_phy_cells_xpe =  23040,
    .mmu_rsvd_cfap_cells_xpe = 720,
    .mmu_cfap_bank_full = 767,
};

/*******************************************************************************
 * Private functions
 */
static int
default_pg_headroom(int unit, int port, int lossless)
{
    int speed = 1000, hdrm = 0;
    int lport = 0;
    uint8_t port_oversub = 0;
    bcmdrd_pbmp_t oversub_map;
    bcmtm_drv_info_t *drv_info;

    bcmtm_drv_info_get(unit, &drv_info);
    if (port == CMIC_PORT) {
        return 77;
    } else if (!lossless) {
        return 0;
    } else if (port >= 132 && port <= 135) { /* Loopback port */
        return 150;
    }

    lport = drv_info->port_p2l_mapping[port];
    speed = drv_info->port_speed_max[lport];
    BCMDRD_PBMP_ASSIGN(oversub_map,
            ((drv_info->port_schedule_state).in_port_map.oversub_pbm));
    if (BCMDRD_PBMP_MEMBER(oversub_map, port)) {
        port_oversub = 1;
    }

    if ((speed >= 1000) && (speed < 20000)) {
        hdrm = port_oversub ? 189 : 166;
    } else if ((speed >= 20000) && (speed <= 30000)) {
        hdrm = port_oversub ? 217 : 194;
    } else if ((speed >= 30000) && (speed <= 42000)) {
        hdrm = port_oversub ? 340 : 286;
    } else if ((speed >= 50000) && (speed < 100000)) {
        hdrm = port_oversub ? 358 : 304;
    } else if (speed >= 100000) {
        hdrm = port_oversub ? 648 : 519;
    } else {
        hdrm = port_oversub ? 189 : 166;
    }
    return hdrm;
}


static int
bcmtm_thd_egr_uc_portsp_init(int unit, bcmdrd_pbmp_t mmu_pbmp)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = TM_THD_EGR_UC_PORT_SERVICE_POOLt;
    uint32_t shared_limit, resume_limit;
    size_t num_fid = TM_THD_EGR_UC_PORT_SERVICE_POOLt_FIELD_COUNT;
    uint32_t ltm_buf[2][3] = {{0}};
    int idx = 0;
    size_t count = 0;
    uint32_t fval;
    int port, mport, pipe, lport = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    shared_limit = bcmtm_device_info[unit].num_cells;
    resume_limit = shared_limit -
               ( 2 * MMU_CFG_MMU_BYTES_TO_CELLS(bcmtm_device_info[unit].default_mtu +
                                           bcmtm_device_info[unit].pkt_hdr_sz,
                                           bcmtm_device_info[unit].cell_sz));
    for (count = 0; count < num_fid; count++){
        switch(count){
            case TM_THD_EGR_UC_PORT_SERVICE_POOLt_SHARED_LIMIT_CELLSf:
                sid = MMU_THDU_CONFIG_PORTm;
                fid = SHARED_LIMITf;
                fval = shared_limit;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_EGR_UC_PORT_SERVICE_POOLt_SHARED_RESUME_LIMITf:
                sid = MMU_THDU_RESUME_PORTm;
                fid = SHARED_RESUMEf;
                fval = resume_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            case TM_THD_EGR_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf:
                sid = MMU_THDU_CONFIG_PORTm;
                fid = YELLOW_LIMITf;
                fval = shared_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_EGR_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMITf:
                sid = MMU_THDU_RESUME_PORTm;
                fid = YELLOW_RESUMEf;
                fval = resume_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            case TM_THD_EGR_UC_PORT_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf:
                sid = MMU_THDU_CONFIG_PORTm;
                fid =  RED_LIMITf;
                fval = shared_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_EGR_UC_PORT_SERVICE_POOLt_RED_SHARED_RESUME_LIMITf:
                sid = MMU_THDU_RESUME_PORTm;
                fid = RED_RESUMEf;
                fval = resume_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            default:
                break;
        }
    }
    for (count = 0; count < 2; count++) {
        BCMDRD_PBMP_ITER(mmu_pbmp, port) {
            lport = drv_info->port_p2l_mapping[port];
            pipe = drv_info->port_pipe_mapping[lport];
            mport = drv_info->port_p2m_mapping[port];
            idx = (mport & 0x3f) * 4 + 0;
            if (count == 0)
                sid = MMU_THDU_CONFIG_PORTm;
            else
                sid = MMU_THDU_RESUME_PORTm;
            BCMTM_PT_DYN_INFO(pt_dyn_info, idx, pipe);
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                        sid , ltid,
                        (void*)&pt_dyn_info, ltm_buf[count]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_ucq_init(int unit, int lossless, bcmdrd_pbmp_t mmu_pbmp)
{
    bcmltd_sid_t ltid = TM_THD_UC_Qt;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    uint32_t shared_limit = 0;
    uint32_t reset_limit = 0;
    size_t num_fid= TM_THD_UC_Qt_FIELD_COUNT;
    uint32_t count = 0;
    uint32_t ltm_buf[3][3] = {{0}};
    int pipe =  0, numq, qbase = 0, port, idx = 0, lport ;
    uint32_t fval = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    shared_limit = bcmtm_device_info[unit].num_cells - ((lossless) ? 88 : 0) -
        ((bcmtm_dev_buf_info.asf_rsvd + bcmtm_dev_buf_info.egr_rsvd) << 1);
    reset_limit = 16;

    for (count = 0; count < num_fid; count++) {
        switch(count) {
            case TM_THD_UC_Qt_SHARED_LIMIT_CELLS_STATICf:
                sid = MMU_THDU_CONFIG_QUEUE0m;
                fid = Q_SHARED_LIMIT_CELLf;
                fval = shared_limit;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_UC_Qt_SHARED_LIMIT_DYNAMICf:
                sid = MMU_THDU_CONFIG_QUEUE0m;
                fid = Q_SHARED_ALPHA_CELLf;
                fval = shared_limit;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_UC_Qt_YELLOW_LIMIT_CELLS_STATICf:
                sid = MMU_THDU_CONFIG_QUEUE0m;
                fid = LIMIT_YELLOW_CELLf;
                fval = reset_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_UC_Qt_RED_LIMIT_CELLS_STATICf:
                sid = MMU_THDU_CONFIG_QUEUE0m;
                fid = LIMIT_RED_CELLf;
                fval = reset_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_UC_Qt_RESET_OFFSET_CELLSf:
                sid = MMU_THDU_OFFSET_QUEUE0m;
                fid = RESET_OFFSET_CELLf;
                fval = reset_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            case TM_THD_UC_Qt_RESET_OFFSET_YELLOW_CELLSf:
                sid = MMU_THDU_OFFSET_QUEUE0m;
                fid = RESET_OFFSET_YELLOW_CELLf;
                fval = reset_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            case TM_THD_UC_Qt_RESET_OFFSET_RED_CELLSf:
                sid = MMU_THDU_OFFSET_QUEUE0m;
                fid = RESET_OFFSET_RED_CELLf;
                fval = reset_limit/8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            default:
                break;
        }
    }

    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        pipe = drv_info->port_pipe_mapping[lport];
        qbase = drv_info->uc_cosq_base[lport];
        numq =  drv_info->num_uc_cosq[lport];
        if (numq == 0){
            continue;
        }
        for(idx = 0; idx < numq; idx++) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, (qbase+idx), pipe);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDU_CONFIG_QUEUE0m,
                             ltid, (void*)&pt_dyn_info, ltm_buf[0]));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDU_OFFSET_QUEUE0m,
                             ltid, (void*)&pt_dyn_info, ltm_buf[1]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_ing_hp_init(int unit, int key_id)
{
    uint32_t ltmbuf = 0;
    int xpe = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid[] = {
        THDI_HDRM_BUFFER_CELL_LIMIT_HP_MMU_XPE0_LAYER0r,
        THDI_HDRM_BUFFER_CELL_LIMIT_HP_MMU_XPE1_LAYER0r,
        THDI_HDRM_BUFFER_CELL_LIMIT_HP_MMU_XPE2_LAYER1r,
        THDI_HDRM_BUFFER_CELL_LIMIT_HP_MMU_XPE3_LAYER1r,
    };
    bcmltd_sid_t ltid = TM_THD_ING_HEADROOM_POOLt;
    uint32_t fval = (bcmtm_dev_buf_info.pg_headroom)/2;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, key_id, 0);

    for(xpe = 0; xpe < 4 ; xpe++) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid[xpe], LIMITf, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid[xpe], ltid,
                    (void *)&pt_dyn_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_port_pg_init(int unit,
                       int lossless,
                       bcmdrd_pbmp_t mmu_pbmp)
{
    bcmltd_sid_t ltid = TM_THD_PORT_PRI_GRPt;
    bcmdrd_sid_t sid = THDI_PORT_PG_CONFIGm;
    size_t num_fid = TM_THD_PORT_PRI_GRPt_FIELD_COUNT;
    size_t count = 0;
    int port, pipe, mport, lport;
    int idx = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t fval = 0;
    uint32_t ltm_buf[3] = {0};
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    while(count < num_fid) {
        switch(count) {
            case TM_THD_PORT_PRI_GRPt_MIN_GUARANTEE_CELLSf:
                fval = 8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, PG_MIN_LIMITf, ltm_buf, &fval));
                break;
            case TM_THD_PORT_PRI_GRPt_USE_DYNAMIC_SHARED_LIMITSf:
                fval = 1;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, PG_SHARED_DYNAMICf, ltm_buf, &fval));
                break;
            case TM_THD_PORT_PRI_GRPt_SHARED_LIMIT_CELLS_STATICf:
            case TM_THD_PORT_PRI_GRPt_SHARED_LIMIT_DYNAMICf:
                fval = 8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, PG_SHARED_LIMITf, ltm_buf, &fval));
                break;
            case TM_THD_PORT_PRI_GRPt_RESET_OFFSET_CELLSf:
                fval = 2;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, PG_RESET_OFFSETf, ltm_buf, &fval));
                break;
            case TM_THD_PORT_PRI_GRPt_USE_GLOBAL_HEADROOMf:
                fval = 1;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, PG_GBL_HDRM_ENf, ltm_buf, &fval));
                break;
             default:
                break;
        }
        count++;
    }
    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        pipe = drv_info->port_pipe_mapping[lport];
        mport = drv_info->port_p2m_mapping[port];
        if (mport >= 0) {
            idx = (mport & 0x3f) * 8 + 7;
            fval = default_pg_headroom(unit, port, 1);
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, PG_HDRM_LIMITf, ltm_buf, &fval));
            BCMTM_PT_DYN_INFO(pt_dyn_info, idx, pipe);
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                        (void *)&pt_dyn_info, ltm_buf));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_int_pri_pg_map_init(int unit, bcmdrd_pbmp_t mmu_pbmp)
{
    bcmdrd_sid_t sid;
    static bcmdrd_fid_t fid[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf,
    };
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = TM_THD_PORT_INT_PRI_TO_PRI_GRPt;
    int int_pri = 0;
    uint32_t ltmbuf = 0;
    uint32_t pgid = 7; /*Default PG ID*/
    int port, mport = 1;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);

    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        mport = drv_info->port_p2m_mapping[port];
        if (mport >= 0) {
            for (int_pri = 0; int_pri < 16; int_pri++) {
                if (int_pri <= 7) {
                    sid = THDI_PORT_PRI_GRP0r;
                } else {
                    sid = THDI_PORT_PRI_GRP1r;
                }
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid[int_pri], &ltmbuf, &pgid));
                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                            (void *)&pt_dyn_info, &ltmbuf));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_mcq_init(int unit, int lossless, bcmdrd_pbmp_t mmu_pbmp)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = TM_THD_MC_Qt;
    size_t num_fid = TM_THD_MC_Qt_FIELD_COUNT;
    uint32_t count = 0, idx = 0, numq;
    uint32_t ltm_buf[3][3] = {{0}};
    uint32_t shared, reset = 0;
    int pipe = 0, qbase, port, lport;
    uint32_t fval;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    shared = bcmtm_device_info[unit].num_cells - ((lossless) ? 88 : 0) -
           ((bcmtm_dev_buf_info.asf_rsvd + bcmtm_dev_buf_info.egr_rsvd) << 1);
    reset = 2 * MMU_CFG_MMU_BYTES_TO_CELLS(bcmtm_device_info[unit].default_mtu +
                                         bcmtm_device_info[unit].pkt_hdr_sz,
                                         bcmtm_device_info[unit].cell_sz);

    for (count = 0; count < num_fid; count++) {
        switch(count) {
            case TM_THD_MC_Qt_SHARED_LIMIT_CELLS_STATICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = Q_SHARED_LIMITf;
                fval = shared;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_MC_Qt_YELLOW_LIMIT_CELLS_STATICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = YELLOW_SHARED_LIMITf;
                fval = shared/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_MC_Qt_RED_LIMIT_CELLS_STATICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = RED_SHARED_LIMITf;
                fval = shared/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_MC_Qt_RESET_OFFSET_CELLSf:
                sid = MMU_THDM_DB_QUEUE_OFFSETm;
                fid = RESUME_OFFSETf;
                fval = reset/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            default:
                break;
        }
    }

    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        pipe = drv_info->port_pipe_mapping[lport];
        qbase = drv_info->mc_cosq_base[lport];
        numq = drv_info->num_mc_cosq[lport];
        if (numq <= 0) {
            continue;
        }
        for (idx = 0; idx < numq; idx++) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, qbase + idx, pipe);
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                        MMU_THDM_DB_QUEUE_CONFIGm, ltid,
                        (void *)&pt_dyn_info, ltm_buf[0]));
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                        MMU_THDM_DB_QUEUE_OFFSETm, ltid,
                        (void *)&pt_dyn_info, ltm_buf[1]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_egr_mc_portsp_init(int unit, bcmdrd_pbmp_t mmu_pbmp)
{
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmltd_sid_t ltid = TM_THD_EGR_MC_PORT_SERVICE_POOLt;
    size_t num_fid = TM_THD_EGR_MC_PORT_SERVICE_POOLt_FIELD_COUNT;
    size_t count = 0;
    uint32_t ltmbuf[3] = {0};
    uint32_t fval = 0;
    uint32_t shared_limit, resume_limit;
    int pipe = 0, port, lport;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    int idx, mport = 0;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    shared_limit = bcmtm_device_info[unit].num_cells;
    resume_limit = shared_limit -
             ( 2 * MMU_CFG_MMU_BYTES_TO_CELLS(bcmtm_device_info[unit].default_mtu +
                                        bcmtm_device_info[unit].pkt_hdr_sz,
                                        bcmtm_device_info[unit].cell_sz));
    for (count = 0; count < num_fid; count++) {
        switch(count) {
            case TM_THD_EGR_MC_PORT_SERVICE_POOLt_SHARED_LIMIT_CELLSf:
                sid = MMU_THDM_DB_PORTSP_CONFIGm;
                fid = SHARED_LIMITf;
                fval = shared_limit;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
                break;
            case TM_THD_EGR_MC_PORT_SERVICE_POOLt_SHARED_RESUME_LIMITf:
                sid = MMU_THDM_DB_PORTSP_CONFIGm;
                fid = SHARED_RESUME_LIMITf;
                fval = resume_limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
                break;
            case TM_THD_EGR_MC_PORT_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf:
                sid = MMU_THDM_DB_PORTSP_CONFIGm;
                fid = YELLOW_SHARED_LIMITf;
                fval = shared_limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
                break;
            case TM_THD_EGR_MC_PORT_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMITf:
                sid = MMU_THDM_DB_PORTSP_CONFIGm;
                fid = YELLOW_RESUME_LIMITf;
                fval = resume_limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
                break;
            case TM_THD_EGR_MC_PORT_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf:
                sid = MMU_THDM_DB_PORTSP_CONFIGm;
                fid = RED_SHARED_LIMITf;
                fval = shared_limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
                break;
            case TM_THD_EGR_MC_PORT_SERVICE_POOLt_RED_SHARED_RESUME_LIMITf:
                sid = MMU_THDM_DB_PORTSP_CONFIGm;
                fid = RED_RESUME_LIMITf;
                fval = resume_limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
                break;
            default:
                break;
        }
    }

    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        pipe = drv_info->port_pipe_mapping[lport];
        mport = drv_info->port_p2m_mapping[port];
        if (mport < 0) {
            continue;
        }
        idx = (mport & 0x3f) + 0 * 34;
        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid + pipe, ltid,
                    (void *) &pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_ucq_grp_init(int unit, int lossless)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = TM_THD_UC_Q_GRPt;
    uint32_t ltm_buf[3][3] = {{0}};
    uint32_t count = 0;
    size_t num_fid = TM_THD_UC_Q_GRPt_FIELD_COUNT;
    uint32_t shared, reset;
    uint32_t fval;
    int index = 0;
    int pipe;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);
    shared = bcmtm_device_info[unit].num_cells - ((lossless) ? 88 : 0) -
        ((bcmtm_dev_buf_info.asf_rsvd + bcmtm_dev_buf_info.egr_rsvd) << 1);
    reset = 16;

    for (count = 0; count < num_fid; count++){
        switch(count) {
            case TM_THD_UC_Q_GRPt_SHARED_LIMIT_CELLS_STATICf:
                sid = MMU_THDU_CONFIG_QGROUP0m;
                fid = Q_SHARED_LIMIT_CELLf;
                fval = shared;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_UC_Q_GRPt_YELLOW_LIMIT_CELLS_STATICf:
                sid = MMU_THDU_CONFIG_QGROUP0m;
                fid = LIMIT_YELLOW_CELLf;
                fval = shared/ 8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_UC_Q_GRPt_RED_LIMIT_CELLS_STATICf:
                sid = MMU_THDU_CONFIG_QGROUP0m;
                fid = LIMIT_RED_CELLf;
                fval = shared/ 8 ;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[0], &fval));
                break;
            case TM_THD_UC_Q_GRPt_RESET_OFFSET_CELLSf:
                sid = MMU_THDU_OFFSET_QGROUP0m;
                fid = RESET_OFFSET_CELLf;
                fval = reset/ 8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            case TM_THD_UC_Q_GRPt_RESET_OFFSET_YELLOW_CELLSf:
                sid = MMU_THDU_OFFSET_QGROUP0m;
                fid = RESET_OFFSET_YELLOW_CELLf;
                fval = reset/ 8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            case TM_THD_UC_Q_GRPt_RESET_OFFSET_RED_CELLSf:
                sid = MMU_THDU_OFFSET_QGROUPm;
                fid = RESET_OFFSET_RED_CELLf;
                fval = reset/ 8;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltm_buf[1], &fval));
                break;
            default:
                break;
        }

    }
    for (pipe = 0; pipe < 4 ; pipe++){
        for (index = 0; index < 36; index++) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDU_CONFIG_QGROUP0m,
                                        ltid,(void*)&pt_dyn_info,ltm_buf[0]));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, MMU_THDU_OFFSET_QGROUPm,
                                        ltid,(void*)&pt_dyn_info,ltm_buf[1]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcmtm_egr_sp_thd_init(int unit)
{
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = TM_THD_EGR_SERVICE_POOLt;
    size_t num_fid = TM_THD_EGR_SERVICE_POOLt_FIELD_COUNT;
    size_t count = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t limit = 0;
    uint32_t ltmbuf = 0;
    uint32_t fval = 0;
    int pipe = 0;

    SHR_FUNC_ENTER(unit);

    limit = bcmtm_device_info[unit].num_cells - (44 + bcmtm_dev_buf_info.asf_rsvd) - 44;
    while(count < num_fid) {
        if (count == TM_THD_EGR_SERVICE_POOLt_TM_THD_SERVICE_POOL_IDf) {
            count++;
            continue;
        }
        ltmbuf = 0;
        switch(count) {
            case TM_THD_EGR_SERVICE_POOLt_SHARED_LIMIT_CELLSf:
                sid = MMU_THDM_DB_POOL_SHARED_LIMITr;
                fid = SHARED_LIMITf;
                fval = limit;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
                break;
            case TM_THD_EGR_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf:
                sid = MMU_THDM_DB_POOL_RESUME_LIMITr;
                fid = RESUME_LIMITf;
                fval = limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
                break;
            case TM_THD_EGR_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf:
                sid = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
                fid = YELLOW_SHARED_LIMITf;
                fval = limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
                break;
            case TM_THD_EGR_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLSf:
                sid = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;
                fid = YELLOW_RESUME_LIMITf;
                fval = limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
                break;
            case TM_THD_EGR_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf:
                sid = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
                fid = RED_SHARED_LIMITf;
                fval = limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
                break;
            case TM_THD_EGR_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLSf:
                sid = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;
                fid = RED_RESUME_LIMITf;
                fval = limit/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
                break;
            default:
                sid = 0;
                break;
        }
        for (pipe = 0; pipe < 4 && sid != 0 ; pipe++) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
                SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                            (void*)&pt_dyn_info, &ltmbuf));
        }
        count++;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_ing_sp_init(int unit,
                        int key_id)
{
    bcmltd_sid_t ltid = TM_THD_ING_SERVICE_POOLt;
    int xpe = 0;
    size_t num_fid = 0;
    uint32_t ltmbuf = 0;
    uint32_t headroom = 0;
    uint32_t buf_limit = 0;
    uint32_t resume_limit = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid_shared_limit[] =  {
            THDI_BUFFER_CELL_LIMIT_SP_MMU_XPE0_LAYER0r,
            THDI_BUFFER_CELL_LIMIT_SP_MMU_XPE1_LAYER0r,
            THDI_BUFFER_CELL_LIMIT_SP_MMU_XPE2_LAYER1r,
            THDI_BUFFER_CELL_LIMIT_SP_MMU_XPE3_LAYER1r,
    };
    bcmdrd_sid_t sid_resume_limit[] = {
            THDI_CELL_RESET_LIMIT_OFFSET_SP_MMU_XPE0_LAYER0r,
            THDI_CELL_RESET_LIMIT_OFFSET_SP_MMU_XPE1_LAYER0r,
            THDI_CELL_RESET_LIMIT_OFFSET_SP_MMU_XPE2_LAYER1r,
            THDI_CELL_RESET_LIMIT_OFFSET_SP_MMU_XPE3_LAYER1r
    };

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, key_id, 0);

    headroom =  2 * (MMU_CFG_MMU_BYTES_TO_CELLS(
                (bcmtm_device_info[unit].max_pkt_sz + bcmtm_device_info[unit].pkt_hdr_sz),
                bcmtm_device_info[unit].cell_sz));

    resume_limit = 2 * (MMU_CFG_MMU_BYTES_TO_CELLS(
                (bcmtm_device_info[unit].jumbo_pkt_sz + bcmtm_device_info[unit].pkt_hdr_sz),
                bcmtm_device_info[unit].cell_sz));

    buf_limit = bcmtm_device_info[unit].num_cells;

    buf_limit -= (((bcmtm_dev_buf_info.pg_guarantee/2) +
                   (bcmtm_dev_buf_info.pg_headroom/2) +
                   bcmtm_dev_buf_info.asf_rsvd)+ headroom);

    for (xpe = 0; xpe < 4; xpe++) {
        num_fid = TM_THD_ING_SERVICE_POOLt_FIELD_COUNT;
        while(--num_fid > 0) {
            ltmbuf = 0;
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
            if (num_fid == TM_THD_ING_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf){
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid_resume_limit[xpe], OFFSETf,
                                     &ltmbuf, &resume_limit));
                SHR_IF_ERR_EXIT(
                        bcmtm_pt_indexed_write(unit, sid_resume_limit[xpe], ltid,
                            (void *)&pt_dyn_info, &ltmbuf));
            } else if(num_fid == TM_THD_ING_SERVICE_POOLt_SHARED_LIMIT_CELLSf){
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid_shared_limit[xpe], LIMITf,
                                     &ltmbuf, &buf_limit));
                SHR_IF_ERR_EXIT(
                        bcmtm_pt_indexed_write(unit, sid_shared_limit[xpe], ltid,
                            (void *)&pt_dyn_info, &ltmbuf));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_bcmtm_thd_cfap_configure(int unit)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmltd_sid_t ltid = 0;
    uint32_t ltmbuf = 0;
    uint32_t fval = 0;
    int baseidx = 0, idx = 0;
    uint32_t jumbo_frame_cells;

    SHR_FUNC_ENTER(unit);

    /*CFAPFULLTHRESHOLDSET*/
    sid = CFAPFULLTHRESHOLDSETr;
    fid = CFAPFULLSETPOINTf;
    fval = bcmtm_dev_buf_info.mmu_phy_cells_xpe -
        bcmtm_dev_buf_info.mmu_rsvd_cfap_cells_xpe; /*0x5730*/
    for (baseidx = 0; baseidx < TH_NUM_XPES; baseidx++) {
        ltmbuf = 0;
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, baseidx);
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,
                    (void *)&pt_dyn_info, &ltmbuf));
    }
    /*CFAPFULLTHRESHOLDRESET*/
    sid = CFAPFULLTHRESHOLDRESETr;
    fid = CFAPFULLRESETPOINTf;
    jumbo_frame_cells =
        MMU_CFG_MMU_BYTES_TO_CELLS(bcmtm_device_info[unit].jumbo_pkt_sz+
                bcmtm_device_info[unit].pkt_hdr_sz, bcmtm_device_info[unit].cell_sz);

    fval -= (2 * jumbo_frame_cells); /*0x56d6*/

    for (baseidx = 0; baseidx < TH_NUM_XPES; baseidx++) {
        ltmbuf = 0;
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0 , baseidx);
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,
                    (void *)&pt_dyn_info, &ltmbuf));
    }
    /*CFAPBANKFULL */
    sid = CFAPBANKFULLr;
    fid = LIMITf;
    fval = bcmtm_dev_buf_info.mmu_cfap_bank_full;  /*0x2FF*/
    /*maxiumum number of instances 16*/
    for (idx = 0; idx < 15; idx++) {
        for (baseidx = 0; baseidx < TH_NUM_XPES; baseidx++) {
            ltmbuf = 0;
            BCMTM_PT_DYN_INFO(pt_dyn_info, idx, baseidx);
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT(
                    bcmtm_pt_indexed_write(unit, sid, ltid,
                        (void *)&pt_dyn_info, &ltmbuf));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_bcmtm_thd_input_configure(int unit)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmltd_sid_t ltid = 0; 
    uint32_t ltmbuf = 0;
    uint32_t fval = 0;

    SHR_FUNC_ENTER(unit);

    /* THDI_GLOBAL_HRM_LIMIT */
    sid = THDI_GLOBAL_HDRM_LIMITr;
    fid = GLOBAL_HDRM_LIMITf;
    fval = MMU_CFG_MMU_BYTES_TO_CELLS(
            (bcmtm_device_info[unit].max_pkt_sz + bcmtm_device_info[unit].pkt_hdr_sz),
            bcmtm_device_info[unit].cell_sz); /*0x2e*/
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid,
                (void*)&pt_dyn_info, &ltmbuf));

    SHR_IF_ERR_EXIT(bcmtm_thd_ing_sp_init(unit, 0));

    SHR_IF_ERR_EXIT(bcmtm_thd_ing_hp_init(unit, 0));

    /* THDI_PORT_MAX_PKT_SIZE */
    sid = THDI_PORT_MAX_PKT_SIZEr;
    fid = PORT_MAX_PKT_SIZEf;
    fval = MMU_CFG_MMU_BYTES_TO_CELLS(
            (bcmtm_device_info[unit].max_pkt_sz + bcmtm_device_info[unit].pkt_hdr_sz),
            bcmtm_device_info[unit].cell_sz); /*0x2e*/
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    ltmbuf = 0;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid,
                (void*)&pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_bcmtm_thd_output_configure(int unit, int lossless)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmltd_sid_t ltid = 0; 
    uint32_t ltmbuf = 0;
    uint32_t fval = 0;
    uint16_t limit = 0;
    uint8_t pipe = 0;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);

    /* OP_THDU_CONFIG */
    sid = OP_THDU_CONFIGr;
    fval = 1;
    fid = ENABLE_QUEUE_AND_GROUP_TICKETf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    fid = MOP_POLICY_1Bf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void*)&pt_dyn_info, &ltmbuf));


    SHR_IF_ERR_EXIT(bcmtm_egr_sp_thd_init(unit));


    /* MMU_THDM_DB_DEVICE_THR_CONFIG */
    sid = MMU_THDM_DB_DEVICE_THR_CONFIGr;
    fid = MOP_POLICYf;
    ltmbuf = 0;
    fval = 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void*)&pt_dyn_info, &ltmbuf));

    for (pipe = 0; pipe < 4; pipe++) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, pipe);
        /*mcq entries*/
        limit = bcmtm_device_info[unit].mcq_entry -  bcmtm_dev_buf_info.mcq_rsvd;
        sid = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
        fid = SHARED_LIMITf;
        ltmbuf = 0;
        fval = limit/4;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));

        /*MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMIT*/
        sid = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
        fid = YELLOW_SHARED_LIMITf;
        ltmbuf = 0;
        fval = limit/8;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));

        /*MMU_THDM_MCQE_POOL_RED_SHARED_LIMIT*/
        sid = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
        fid = RED_SHARED_LIMITf;
        ltmbuf = 0;
        fval = limit/8;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));
        /*MMU_THDM_MCQE_POOL_RESUME_LIMIT*/
        sid = MMU_THDM_MCQE_POOL_RESUME_LIMITr;
        fid = RESUME_LIMITf;
        ltmbuf = 0;
        fval = limit/4;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));

        /*MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMIT*/
        sid = MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr;
        fid = YELLOW_RESUME_LIMITf;
        ltmbuf = 0;
        fval = limit/8;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));

        /*MMU_THDM_MCQE_POOL_RED_RESUME_LIMIT*/
        sid = MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr;
        fid = RED_RESUME_LIMITf;
        ltmbuf = 0;
        fval = limit/8;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid,(void*)&pt_dyn_info, &ltmbuf));
    }
    /*queue group */
    SHR_IF_ERR_EXIT(bcmtm_thd_ucq_grp_init(unit, lossless));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_bcmtm_ing_per_port_thd_init(int unit,
                                        bcmdrd_pbmp_t mmu_pbmp)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t ltmbuf[2] = {0};
    uint32_t fval;
    uint32_t pool_id = 0; /**/
    int port = 0, mport;
    int index = 0;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        mport = drv_info->port_p2m_mapping[port];
        if (mport >= 0) {
            /*THDI_PORT_SP_CONFIG*/
            sid = THDI_PORT_SP_CONFIGm;
            fid = PORT_SP_MAX_LIMITf;
            for (pool_id = 0; pool_id < 4; pool_id++) {
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                index = (mport & 0x3f)* 4;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pool_id);
                sid = THDI_PORT_SP_CONFIGm;
                fid = PORT_SP_MAX_LIMITf;
                /*0x4ec5*/
                fval = bcmtm_device_info[unit].num_cells;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

                fid = PORT_SP_RESUME_LIMITf;
                fval = (bcmtm_device_info[unit].num_cells - 16)/8;
                SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
                SHR_IF_ERR_EXIT(
                        bcmtm_pt_indexed_write(unit, sid, ltid,
                            (void *)&pt_dyn_info, ltmbuf));
            }

            /* THDI_INPUT_PORT_XON_ENABLES */
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
            sid = THDI_INPUT_PORT_XON_ENABLESr;
            fid = INPUT_PORT_RX_ENABLEf;
            sal_memset(ltmbuf, 0, sizeof(ltmbuf));
            fval = 1;
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

            fid = PORT_PRI_XON_ENABLEf;
            fval = ((1<<16) -1);
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

            fid = PORT_PAUSE_ENABLEf;
            fval = 1;
            SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
            SHR_IF_ERR_EXIT(
                    bcmtm_pt_indexed_write(unit, sid, ltid,
                        (void *)&pt_dyn_info, ltmbuf));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_bcmtm_mc_thd_init(int unit,
                              int lossless,
                              bcmdrd_pbmp_t mmu_pbmp)
{
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmltd_sid_t ltid = 0;
    uint32_t ltmbuf[3] = {0};
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t fval = 0, limit = 0;
    int port, pipe= 0, numq, qbase, idx, mport, lport;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmtm_thd_mcq_init(unit, lossless, mmu_pbmp));
    bcmtm_drv_info_get(unit, &drv_info);

    /*MMU_THDM_MCQE_QUEUE_CONFIG*/
    sid = MMU_THDM_MCQE_QUEUE_CONFIGm;
    fid = Q_MIN_LIMITf;
    fval = 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    limit = bcmtm_device_info[unit].mcq_entry - bcmtm_dev_buf_info.mcq_rsvd;
    fval = limit/4;
    fid = Q_SHARED_LIMITf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    fid = RED_SHARED_LIMITf;
    fval = limit/8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    fval = limit/8;
    fid = YELLOW_SHARED_LIMITf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        pipe = drv_info->port_pipe_mapping[lport];
        qbase = drv_info->mc_cosq_base[lport];
        numq =  drv_info->num_mc_cosq[lport];
        if (numq == 0) {
            continue;
        }
        for (idx = 0; idx < numq; idx++) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, qbase + idx, pipe);
            SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                        (void *)&pt_dyn_info, ltmbuf));
        }
    }

    /* MMU_THDM_MCQE_PORTSP_CONFIG */
    limit = bcmtm_device_info[unit].mcq_entry;
    sid = MMU_THDM_MCQE_PORTSP_CONFIGm;
    fid = SHARED_LIMITf;
    fval = (limit/4) - 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    fid = SHARED_RESUME_LIMITf;
    fval = (limit/8) - 2;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));


    fid = YELLOW_SHARED_LIMITf;
    fval = (limit/8) - 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    fid = YELLOW_RESUME_LIMITf;
    fval = (limit/8) - 2;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    fid = RED_SHARED_LIMITf;
    fval = (limit/8) - 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    fid = RED_RESUME_LIMITf;
    fval = (limit/8) - 2;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        pipe = drv_info->port_pipe_mapping[lport];
        mport = drv_info->port_p2m_mapping[port];
        if (mport < 0) {
            continue;
        }
        idx = (mport & 0x3f) + 0 * 34;
        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid + pipe, ltid,
                    (void *)&pt_dyn_info, ltmbuf));

    }

    SHR_IF_ERR_EXIT(bcmtm_thd_egr_mc_portsp_init(unit, mmu_pbmp));

    limit = bcmtm_device_info[unit].num_cells- ((lossless) ? 88 : 0) -
        ((bcmtm_dev_buf_info.asf_rsvd + bcmtm_dev_buf_info.egr_rsvd) << 1);
    /* MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOW */
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    sid = MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr;
    fid = YELLOW_RESUME_OFFSETf;
    ltid = 0;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    fval =  (limit -16) / 8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    /* MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_RED */
    sid = MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr;
    fid = YELLOW_RESUME_OFFSETf;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    fval =  (limit -16) / 8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    /*MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_RED*/
    sid = MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_REDr;
    fid = RED_RESUME_OFFSETf;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    fval =  (limit -16) / 8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    /*MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_YELLOW*/
    sid = MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_REDr;
    fid = RED_RESUME_OFFSETf;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    fval =  (limit -16) / 8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_rqe_queue_init(int unit, int lossless)
{
    bcmdrd_sid_t sid = MMU_THDR_DB_LIMIT_MIN_PRIQr;
    bcmdrd_fid_t fid = MIN_LIMITf;
    uint32_t fval = 8;
    uint32_t db_limit_min_priq = 0;
    uint32_t db_config_priq = 0;
    uint32_t db_limit_color_priq = 0;
    uint32_t db_reset_color_priq = 0;
    uint32_t qe_limit_min_priq = 0;
    uint32_t qe_config_priq = 0;
    uint32_t qe_limit_color_priq = 0;
    uint32_t qe_reset_color_priq = 0;
    uint32_t egr_shared_total = 0;
    uint32_t mtu_cells = 0;
    int idx = 0;
    bcmltd_sid_t ltid = 0;

    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);
    /* MMU_THDR_DB_LIMIT_MIN_PRIQ */
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &db_limit_min_priq, &fval));

    /* MMU_THDR_DB_CONFIG_PRIQ.SHARED_LIMIT */
    sid = MMU_THDR_DB_CONFIG_PRIQr;
    fid = SHARED_LIMITf;
    egr_shared_total = bcmtm_device_info[unit].num_cells - ((lossless) ? 88 : 0) -
                  ((bcmtm_dev_buf_info.asf_rsvd + bcmtm_dev_buf_info.egr_rsvd) << 1);
    fval = egr_shared_total;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &db_config_priq, &fval ));

    /* MMU_THDR_DB_CONFIG_PRIQ.RESET_OFFSET */
    fid = RESET_OFFSETf;
    fval = 2;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &db_config_priq, &fval));

    /*MMU_THDR_DB_LIMIT_COLOR_PRIQ.SHARED_RED_LIMIT*/
    sid = MMU_THDR_DB_LIMIT_COLOR_PRIQr;
    fid = SHARED_RED_LIMITf;
    fval = egr_shared_total/8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &db_limit_color_priq, &fval));

    /*MMU_THDR_DB_LIMIT_COLOR_PRIQ.SHARED_YELLOW_LIMIT */
    fid = SHARED_RED_LIMITf;
    fval = egr_shared_total/8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &db_limit_color_priq, &fval));

    /*MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQ.RESET_OFFSET_RED*/
    sid = MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr;
    fid = RESET_OFFSET_REDf;
    mtu_cells = MMU_CFG_MMU_BYTES_TO_CELLS(bcmtm_device_info[unit].default_mtu +
                                      bcmtm_device_info[unit].pkt_hdr_sz,
                                      bcmtm_device_info[unit].cell_sz);
    fval = (mtu_cells * 2)/8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &db_reset_color_priq, &fval));

    /*MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQ.RESET_OFFSET_YELLOW*/
    fid = RESET_OFFSET_YELLOWf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &db_reset_color_priq, &fval));

    /*MMU_THDR_QE_LIMIT_MIN_PRIQ.MIN_LIMIT*/
    sid = MMU_THDR_QE_LIMIT_MIN_PRIQr;
    fid = MIN_LIMITf;
    fval = 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &qe_limit_min_priq, &fval));

    fval = (bcmtm_device_info[unit].mcq_entry - 1) / (8 * 11);
    /*MMU_THDR_QE_CONFIG_PRIQ.SHARED_LIMIT*/
    sid = MMU_THDR_QE_CONFIG_PRIQr;
    fid = SHARED_LIMITf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &qe_config_priq, &fval));


    /*MMU_THDR_QE_LIMIT_COLOR_PRIQ.SHARED_RED_LIMIT*/
    sid = MMU_THDR_QE_LIMIT_COLOR_PRIQr;
    fid = SHARED_RED_LIMITf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &qe_limit_color_priq, &fval));

    /*MMU_THDR_QE_LIMIT_COLOR_PRIQ.SHARED_YELLOW_LIMIT*/
    fid = SHARED_YELLOW_LIMITf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &qe_limit_color_priq, &fval));

    /*MMU_THDR_QE_CONFIG_PRIQ.RESET_OFFSET*/
    sid = MMU_THDR_QE_CONFIG_PRIQr;
    fid = RESET_OFFSETf;
    fval = 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &qe_config_priq, &fval));

    /*MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQ.RESET_OFFSET_RED*/
    sid = MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr;
    fid = RESET_OFFSET_REDf;
    fval = mtu_cells / 8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &qe_reset_color_priq, &fval));

    /*MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQ.RESET_OFFSET_YELLOW*/
    fid = RESET_OFFSET_YELLOWf;
    fval = mtu_cells / 8;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &qe_reset_color_priq, &fval));


    for (idx = 0; idx < TH_MMU_NUM_RQE_QUEUES; idx++) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, idx, 0);
        /*MMU_THDR_DB_LIMIT_MIN_PRIQ*/
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, MMU_THDR_DB_LIMIT_MIN_PRIQr, ltid,
                    (void *)&pt_dyn_info, &db_limit_min_priq));

        /*MMU_THDR_DB_CONFIG_PRIQ*/
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, MMU_THDR_DB_CONFIG_PRIQr, ltid,
                    (void *)&pt_dyn_info, &db_config_priq));

        /*MMU_THDR_DB_LIMIT_COLOR_PRIQ */
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, ltid,
                    (void *)&pt_dyn_info, &db_config_priq));

        /*MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQ*/
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit,
                    MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr, ltid,
                    (void *)&pt_dyn_info, &db_reset_color_priq));

        /*MMU_THDR_QE_LIMIT_MIN_PRIQ*/
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, MMU_THDR_QE_LIMIT_MIN_PRIQr, ltid,
                    (void *)&pt_dyn_info, &qe_limit_min_priq));
        /*MMU_THDR_QE_CONFIG_PRIQ*/
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, MMU_THDR_QE_CONFIG_PRIQr, ltid,
                    (void *)&pt_dyn_info, &qe_config_priq));

        /*MMU_THDR_QE_LIMIT_COLOR_PRIQ*/
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr, ltid,
                    (void *)&pt_dyn_info, &qe_config_priq));

        /*MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQ*/
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit,
                    MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr, ltid,
                    (void *)&pt_dyn_info, &qe_config_priq));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_thd_rqe_pool_init(int unit, int lossless)
{
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmltd_sid_t ltid = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info ;
    uint32_t index = 0;
    uint32_t ltmbuf = 0;
    uint32_t limit;
    uint32_t mtu_cells;
    uint32_t rqlen = 0;
    uint32_t fval = 0;
    int pipe = 0;

    SHR_FUNC_ENTER(unit);

    limit = bcmtm_device_info[unit].num_cells - 88;
    mtu_cells = MMU_CFG_MMU_BYTES_TO_CELLS(bcmtm_device_info[unit].default_mtu +
            bcmtm_device_info[unit].pkt_hdr_sz,
            bcmtm_device_info[unit].cell_sz);

    for (pipe = 0; pipe < TH_PIPES_PER_DEV; pipe++) {
        BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
        /*MMU_THDR_DB_CONFIG_SP*/
        sid = MMU_THDR_DB_CONFIG_SPr;
        fid = SHARED_LIMITf;
        fval = limit;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

        fid = RESUME_LIMITf;
        fval = (limit - ( 2 * mtu_cells))/8;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));

        /*MMU_THDR_DB_SP_SHARED_LIMIT*/
        ltmbuf = 0;
        sid = MMU_THDR_DB_SP_SHARED_LIMITr;
        fid = SHARED_RED_LIMITf;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

        fid = SHARED_YELLOW_LIMITf;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));

        /*MMU_THDR_DB_RESUME_COLOR_LIMIT_SP*/
        sid = MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr;
        ltmbuf = 0;
        fid = RESUME_RED_LIMITf;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

        fid = RESUME_YELLOW_LIMITf;
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));

    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
    /*MMU_THDR_QE_CONFIG_SP*/
    rqlen = ((bcmtm_device_info[unit].mcq_entry/4) - 88/8);
    sid = MMU_THDR_QE_CONFIG_SPr;
    fid = SHARED_LIMITf;
    fval = rqlen;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = RESUME_LIMITf;
    fval = rqlen - 1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));

    /*MMU_THDR_QE_SHARED_COLOR_LIMIT_SP*/
    ltmbuf = 0;
    sid = MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr;
    fid = SHARED_RED_LIMITf;
    fval = rqlen;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = SHARED_YELLOW_LIMITf;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    fval = rqlen;
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));
    /*MMU_THDR_QE_RESUME_COLOR_LIMIT_SP*/
    sid = MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr;
    ltmbuf = 0;
    fid = RESUME_RED_LIMITf;
    fval = rqlen -1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
    fid = RESUME_YELLOW_LIMITf;
    fval = rqlen -1;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
            bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/* Function to get the number of ports present in a given Port Macro */
static int
ports_per_pm_get(int unit, int pm_id)
{
    int port_base, lport, num_ports = 0;
    bcmtm_drv_info_t *drv_info;

    bcmtm_drv_info_get(unit, &drv_info);
    if (pm_id >= TH_PBLKS_PER_DEV) {
        return SHR_E_PARAM;
    }

    port_base = 1 + (pm_id * TH_PORTS_PER_PBLK);
    lport = drv_info->port_p2l_mapping[port_base];
    if (lport  != -1) {
        num_ports = 1;
        if ((drv_info->port_num_lanes[lport] == 2) &&
            (drv_info->port_p2l_mapping[port_base + 2] != -1)) {
            /* for cases when num_lanes is given in the config (40g).
             */
            num_ports = 2;
        }

        /* When phy_port_base + 1 is valid,
         *      All 4 ports of the PM(port macro) are valid.
         * When phy_port_base + 2 is valid,
         *      The PM is operating in Dual lane mode
         */
        if (drv_info->port_p2l_mapping[port_base + 1] != -1) {
            num_ports = 4;
            if (drv_info->port_p2l_mapping[port_base + 2] == -1) {
                num_ports = 2;
            }
        } else if (drv_info->port_p2l_mapping[port_base + 2] != -1) {
            num_ports = 2;
        }
    }
    return num_ports;
}

static int
mmu_thd_mode_get(int unit){

    bcmltd_sid_t ltid = TM_THD_CONFIGt;
    uint64_t field[TM_THD_CONFIGt_FIELD_COUNT];
    size_t actual;
    int rv = 0;

    rv = bcmcfg_table_get(unit, ltid, TM_THD_CONFIGt_FIELD_COUNT,
                field, &actual);
    if (rv == SHR_E_UNAVAIL) {
        /* by default in lossless mode */
       return 1;
    }
    return !(field[TM_THD_CONFIGt_MMU_THRESHOLDS_MODEf]);
}

/*******************************************************************************
 * Public functions
 */
int
bcm56960_a0_bcmtm_mmu_config_init(int unit)
{
    int default_mtu_cells;
    int idx, pipe, port, lport;
    int lossless = 1;
    int ing_rsvd_cells[4];
    int egr_rsvd_cells[4];
    int asf_rsvd_cells[4];
    int min_pm_id, max_pm_id, num_ports;
    int oversub_mode = FALSE;
    bcmdrd_pbmp_t mmu_pbmp;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    sal_memset(&bcmtm_dev_buf_info, 0, sizeof(struct bcmtm_dev_buf_info));
    bcmtm_dev_buf_info.mmu_phy_cells_xpe =  23040,
    bcmtm_dev_buf_info.mmu_rsvd_cfap_cells_xpe = 720,
    bcmtm_dev_buf_info.mmu_cfap_bank_full = 767,

    lossless = mmu_thd_mode_get(unit);

    for (pipe = 0; pipe < 4; pipe++) {
        ing_rsvd_cells[pipe] = 0;
        egr_rsvd_cells[pipe] = 0;
        asf_rsvd_cells[pipe] = 0;
        min_pm_id = pipe * 8;
        max_pm_id = min_pm_id + 8;
        for (idx = min_pm_id; idx < max_pm_id; idx++) {
            num_ports = ports_per_pm_get(unit, idx);
            if (num_ports == 1) {
                ing_rsvd_cells[pipe] += ((0 + 0 + 0) * num_ports);
                egr_rsvd_cells[pipe] += (0 * num_ports);
                if (oversub_mode) {
                    asf_rsvd_cells[pipe] += 48;
                } else {
                    asf_rsvd_cells[pipe] += 20;
                }
            } else if (num_ports == 2) {
                if (oversub_mode) {
                    asf_rsvd_cells[pipe] += 96;
                } else {
                    asf_rsvd_cells[pipe] += 40;
                }
            } else if (num_ports == 4) {
                if (oversub_mode) {
                    asf_rsvd_cells[pipe] += 168;
                } else {
                    asf_rsvd_cells[pipe] += 80;
                }
            } else {
                continue;
            }
        }
    }

     default_mtu_cells =
            MMU_CFG_MMU_BYTES_TO_CELLS(bcmtm_device_info[unit].default_mtu +
                                     bcmtm_device_info[unit].pkt_hdr_sz,
                                     bcmtm_device_info[unit].cell_sz);

    bcmtm_dev_buf_info.asf_rsvd = asf_rsvd_cells[0];
    bcmtm_dev_buf_info.egr_rsvd = egr_rsvd_cells[0];

     SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_thd_cfap_configure(unit));

    /* internal priority to priority group mapping */
    BCMDRD_PBMP_ASSIGN(mmu_pbmp,
        ((drv_info->port_schedule_state).in_port_map.physical_pbm));
    SHR_IF_ERR_EXIT(bcmtm_thd_int_pri_pg_map_init(unit, mmu_pbmp));
    BCMDRD_PBMP_ITER(mmu_pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        bcmtm_dev_buf_info.pg_headroom += default_pg_headroom(unit, port, 1);
        bcmtm_dev_buf_info.pg_guarantee += default_mtu_cells;
        bcmtm_dev_buf_info.mcq_rsvd +=
            (4 * drv_info->num_mc_cosq[lport]);
    }
    /* Input thresholds */
    SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_thd_input_configure(unit));

    /* output thresholds */
    SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_thd_output_configure(unit, lossless));

    /* Input port per port settings */
    SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_ing_per_port_thd_init(unit, mmu_pbmp));

    SHR_IF_ERR_EXIT(bcmtm_thd_port_pg_init(unit, lossless, mmu_pbmp));
    /***********************************
    * THDO
 */
    /* Output port per port per queue setting for regular multicast queue */
    SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_mc_thd_init(unit, lossless, mmu_pbmp));

    /* Output port per port per queue setting for regular unicast queue */
    SHR_IF_ERR_EXIT(bcmtm_thd_ucq_init(unit, lossless, mmu_pbmp));

    SHR_IF_ERR_EXIT(bcmtm_thd_egr_uc_portsp_init(unit, mmu_pbmp));

    /* RQE */
    SHR_IF_ERR_EXIT(bcmtm_thd_rqe_queue_init(unit, lossless));

    /* per pool RQE settings, pool idx 0 is utilized */
    SHR_IF_ERR_EXIT(bcmtm_thd_rqe_pool_init(unit, lossless));

exit:
    SHR_FUNC_EXIT();
}
