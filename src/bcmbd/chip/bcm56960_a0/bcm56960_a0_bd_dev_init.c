/*! \file bcm56960_a0_bd_dev_init.c
 *
 * Perform basic device initialization.
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

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd_cmicd_mem.h>

#include <bcmbd/chip/bcm56960_a0_acc.h>

#include "bcm56960_a0_drv.h"
#include "bcm56960_a0_internal.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
* Local definitions
 */

/* Time to wait for IP and EP reset */
#define PIPE_RESET_TIMEOUT_MSEC         50

/* Number of pipes per device */
#define TH_PIPES_PER_DEV                4

struct port_info {
    int lport;
    int pport;
};

static struct port_info cpu_port = { 0, 0 };
static struct port_info lb_ports[] = {
    { 33,  132 },   /* loopback port 0 */
    { 67,  133 },   /* loopback port 1 */
    { 101, 134 },   /* loopback port 2 */
    { 135, 135 }    /* loopback port 3 */
};

#define P2L(_u, _p) p2l(_u, _p, 0)
#define L2P(_u, _p) p2l(_u, _p, 1)

/*******************************************************************************
* Private functions
 */

static int
ipep_init(int unit)
{
    int ioerr = 0;
    int in_progress;
    int count;
    int idx, pipe;
    uint32_t pipe_map;
    bool sim = bcmdrd_feature_is_sim(unit);
    ING_HW_RESET_CONTROL_1r_t ing_hreset_ctrl_1;
    ING_HW_RESET_CONTROL_2r_t ing_hreset_ctrl_2;
    EGR_HW_RESET_CONTROL_0r_t egr_hreset_ctrl_0;
    EGR_HW_RESET_CONTROL_1r_t egr_hreset_ctrl_1;

    /* Initialize IPIPE memory */
    ING_HW_RESET_CONTROL_1r_CLR(ing_hreset_ctrl_1);
    ioerr += WRITE_ING_HW_RESET_CONTROL_1r(unit, ing_hreset_ctrl_1);
    ING_HW_RESET_CONTROL_2r_CLR(ing_hreset_ctrl_2);
    ING_HW_RESET_CONTROL_2r_RESET_ALLf_SET(ing_hreset_ctrl_2, 1);
    ING_HW_RESET_CONTROL_2r_VALIDf_SET(ing_hreset_ctrl_2, 1);
    /* Set count to # entries in largest IPIPE table (L3_DEFIP_ALPM_IPV4m) */
    count = L3_DEFIP_ALPM_IPV4m_MAX - L3_DEFIP_ALPM_IPV4m_MIN + 1;
    ING_HW_RESET_CONTROL_2r_COUNTf_SET(ing_hreset_ctrl_2, count);
    ioerr += WRITE_ING_HW_RESET_CONTROL_2r_ALL(unit, ing_hreset_ctrl_2);

    /* Initialize EPIPE memory */
    EGR_HW_RESET_CONTROL_0r_CLR(egr_hreset_ctrl_0);
    ioerr += WRITE_EGR_HW_RESET_CONTROL_0r(unit, egr_hreset_ctrl_0);
    EGR_HW_RESET_CONTROL_1r_CLR(egr_hreset_ctrl_1);
    EGR_HW_RESET_CONTROL_1r_RESET_ALLf_SET(egr_hreset_ctrl_1, 1);
    EGR_HW_RESET_CONTROL_1r_VALIDf_SET(egr_hreset_ctrl_1, 1);
    /* Set count to # entries in largest EPIPE table (EGR_L3_NEXT_HOP) */
    count = EGR_L3_NEXT_HOPm_MAX - EGR_L3_NEXT_HOPm_MIN + 1;
    EGR_HW_RESET_CONTROL_1r_COUNTf_SET(egr_hreset_ctrl_1, count);
    ioerr += WRITE_EGR_HW_RESET_CONTROL_1r_ALL(unit, egr_hreset_ctrl_1);

    /* Get the valid pipes. */
    if (bcmdrd_dev_valid_pipes_get(unit, &pipe_map) != 0) {
        return SHR_E_INTERNAL;
    }

    /* Wait for IPIPE memory initialization done. */
    in_progress = pipe_map;
    for (idx = 0; idx < PIPE_RESET_TIMEOUT_MSEC; idx++) {
        if (sim) {
            break;
        }
        for (pipe = 0; pipe < TH_PIPES_PER_DEV; pipe++) {
            if (in_progress & (1 << pipe)) {
                /* not done yet */
                ioerr += READ_ING_HW_RESET_CONTROL_2r(unit, pipe,
                                                      &ing_hreset_ctrl_2);
                if (ING_HW_RESET_CONTROL_2r_DONEf_GET(ing_hreset_ctrl_2)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (in_progress == 0) {
            break;
        }
        sal_usleep(1000);
    }
    if (idx >= PIPE_RESET_TIMEOUT_MSEC) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "ING_HW_RESET timeout. "
                             "pipe_map=%"PRIu32", in_progress=%"PRIu32"\n"),
                  pipe_map, in_progress));
        return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
    }

    /* Wait for EPIPE memory initialization done. */
    in_progress = pipe_map;
    for (idx = 0; idx < PIPE_RESET_TIMEOUT_MSEC; idx++) {
        if (sim) {
            break;
        }
        for (pipe = 0; pipe < TH_PIPES_PER_DEV; pipe++) {
            if (in_progress & (1 << pipe)) {
                /* not done yet */
                ioerr += READ_EGR_HW_RESET_CONTROL_1r(unit, pipe,
                                                      &egr_hreset_ctrl_1);
                if (EGR_HW_RESET_CONTROL_1r_DONEf_GET(egr_hreset_ctrl_1)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (in_progress == 0) {
            break;
        }
        sal_usleep(1000);
    }
    if (idx >= PIPE_RESET_TIMEOUT_MSEC) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "EGR_HW_RESET timeout\n")));
        return ioerr ? SHR_E_IO : SHR_E_TIMEOUT;
    }

    ING_HW_RESET_CONTROL_2r_CLR(ing_hreset_ctrl_2);
    ioerr += WRITE_ING_HW_RESET_CONTROL_2r_ALL(unit, ing_hreset_ctrl_2);
    EGR_HW_RESET_CONTROL_1r_CLR(egr_hreset_ctrl_1);
    ioerr += WRITE_EGR_HW_RESET_CONTROL_1r_ALL(unit, egr_hreset_ctrl_1);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
misc_init(int unit)
{
    int ioerr = 0;
    int pipe;
    int port;
    bcmdrd_pbmp_t pbmp;
    IDB_CA_CPU_CONTROLr_t ca_cpu_ctrl;
    IDB_CA_LPBK_CONTROLr_t ca_lpbk_ctrl;
    IDB_HW_CONTROLr_t idb_hctrl;
    MMU_GCFG_MISCCONFIGr_t mmu_misccfg;

    /* Initialize IDB memory */
    IDB_HW_CONTROLr_CLR(idb_hctrl);
    ioerr += WRITE_IDB_HW_CONTROLr_ALL(unit, idb_hctrl);
    IDB_HW_CONTROLr_IS_MEM_INITf_SET(idb_hctrl, 1);
    ioerr += WRITE_IDB_HW_CONTROLr_ALL(unit, idb_hctrl);
    IDB_HW_CONTROLr_CLR(idb_hctrl);
    ioerr += WRITE_IDB_HW_CONTROLr_ALL(unit, idb_hctrl);

    /* Initialize MMU memory */
    MMU_GCFG_MISCCONFIGr_CLR(mmu_misccfg);
    ioerr += WRITE_MMU_GCFG_MISCCONFIGr(unit, 0, mmu_misccfg);
    MMU_GCFG_MISCCONFIGr_INIT_MEMf_SET(mmu_misccfg, 1);
    ioerr += WRITE_MMU_GCFG_MISCCONFIGr(unit, 0, mmu_misccfg);
    MMU_GCFG_MISCCONFIGr_CLR(mmu_misccfg);
    ioerr += WRITE_MMU_GCFG_MISCCONFIGr(unit, 0, mmu_misccfg);

    /* Enable Field Processor metering clock */
    ioerr += READ_MMU_GCFG_MISCCONFIGr(unit, 0, &mmu_misccfg);
    MMU_GCFG_MISCCONFIGr_REFRESH_ENf_SET(mmu_misccfg, 1);
    ioerr += WRITE_MMU_GCFG_MISCCONFIGr(unit, 0, mmu_misccfg);

    /* Toggle cpu port cell assembly reset to send initial credit to EP */
    IDB_CA_CPU_CONTROLr_CLR(ca_cpu_ctrl);
    IDB_CA_CPU_CONTROLr_PORT_RESETf_SET(ca_cpu_ctrl, 1);
    ioerr += WRITE_IDB_CA_CPU_CONTROLr(unit, 0, ca_cpu_ctrl);
    IDB_CA_CPU_CONTROLr_CLR(ca_cpu_ctrl);
    ioerr += WRITE_IDB_CA_CPU_CONTROLr(unit, 0, ca_cpu_ctrl);

    /* Toggle loopback port cell assembly reset to send initial credit to EP */
    bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_LBPORT, &pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        pipe = bcmdrd_dev_phys_port_pipe(unit, port);
        if (pipe < 0) {
            continue;
        }
        IDB_CA_LPBK_CONTROLr_CLR(ca_lpbk_ctrl);
        IDB_CA_LPBK_CONTROLr_PORT_RESETf_SET(ca_lpbk_ctrl, 1);
        ioerr += WRITE_IDB_CA_LPBK_CONTROLr(unit, pipe, ca_lpbk_ctrl);
        IDB_CA_LPBK_CONTROLr_CLR(ca_lpbk_ctrl);
        ioerr += WRITE_IDB_CA_LPBK_CONTROLr(unit, pipe, ca_lpbk_ctrl);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
p2l(int unit, int port, int inverse)
{
    int pidx;

    if ((port < 0) || (port >= NUM_PHYS_PORTS)) {
        return -1;
    }

    if (inverse) {
        if (port == cpu_port.lport) {
            return cpu_port.pport;
        }
        for (pidx = 0; pidx < COUNTOF(lb_ports); pidx++) {
            if (port == lb_ports[pidx].lport) {
                return lb_ports[pidx].pport;
            }
        }
    } else {
        if (port == cpu_port.pport) {
            return cpu_port.lport;
        }
        for (pidx = 0; pidx < COUNTOF(lb_ports); pidx++) {
                if (port == lb_ports[pidx].pport) {
                    return lb_ports[pidx].lport;
                }
        }
    }

    return -1;
}

static void
logical_pbmp_lpbk_get(int unit, bcmdrd_pbmp_t *lpbmp_lpbk)
{
    int port;
    int lport;
    bcmdrd_pbmp_t ppbmp;

    bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_LBPORT, &ppbmp);

    BCMDRD_PBMP_CLEAR(*lpbmp_lpbk);
    BCMDRD_PBMP_ITER(ppbmp, port) {
        lport = P2L(unit, port);
        if (lport >= 0) {
            BCMDRD_PBMP_PORT_ADD(*lpbmp_lpbk, lport);
        }
    }
}


static int
switching_init(int unit)
{
    int ioerr = 0, idx, lport_cmic;
    bcmdrd_pbmp_t lpbmp;
    uint32_t pbm;
    EPC_LINK_BMAPm_t epc_link;

    SHR_FUNC_ENTER(unit);

    /* Get the logical port bitmaps. */
    lport_cmic = P2L(unit, CMIC_PORT);

    /* Enable CPU port in MMU */
    BCMDRD_PBMP_CLEAR(lpbmp);
    BCMDRD_PBMP_PORT_ADD(lpbmp, lport_cmic);
    EPC_LINK_BMAPm_CLR(epc_link);
    for (idx = 0; idx < 5; idx++) {
        pbm = BCMDRD_PBMP_WORD_GET(lpbmp, idx);
        EPC_LINK_BMAPm_SET(epc_link, idx, pbm);
    }
    ioerr += WRITE_EPC_LINK_BMAPm(unit, 0, epc_link);

    SHR_RETURN_VAL_EXIT(ioerr ? SHR_E_IO : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Special initialization for simulation environment.
 *
 * Some simulators require sane values in the logical to physical port mapping
 * tables for correct access to registers indexed by physical port number.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_ACCESS Failed to access the registers.
 */
static int
sim_init(int unit)
{
    int ioerr = 0, port;
    bcmdrd_pbmp_t pbmp;
    EGR_DEV_TO_PHYS_MAPr_t egr_d2p;

    bcmdrd_dev_phys_pbmp(unit, &pbmp);

    EGR_DEV_TO_PHYS_MAPr_CLR(egr_d2p);
    BCMDRD_PBMP_ITER(pbmp, port) {
        EGR_DEV_TO_PHYS_MAPr_PHYSICAL_PORT_NUMBERf_SET(egr_d2p, port);
        ioerr += WRITE_EGR_DEV_TO_PHYS_MAPr(unit, port, egr_d2p);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static int
port_init(int unit)
{
    int ioerr = 0, idx;
    int pipe, lport, class;
    uint32_t pbm[PBM_PORT_WORDS];
    bcmdrd_pbmp_t lpbmp_all, lpbmp_lb;
    DEVICE_LOOPBACK_PORTS_BITMAPm_t dev_lpbk_pbmp;
    MULTIPASS_LOOPBACK_BITMAPm_t mltpss_lpbk_bmp;
    CPU_PBMm_t cpu_pbm;
    CPU_PBM_2m_t cpu_pbm_2;
    EGR_ING_PORTm_t egr_ing_port;
    EGR_MMU_CELL_CREDITr_t egr_mmu_crdt;
    EGR_XMIT_START_COUNTm_t egr_xmt_cnt;
    EGR_ENABLEm_t egr_en;
    ING_CONFIG_64r_t ing_cfg_64;
    EGR_CONFIG_1r_t egr_cfg_1;

    SHR_FUNC_ENTER(unit);

    /* Get the logical port bitmaps. */
    bcmdrd_dev_logic_pbmp(unit, &lpbmp_all);
    logical_pbmp_lpbk_get(unit, &lpbmp_lb);

    /* Configure CPU port */
    CPU_PBMm_CLR(cpu_pbm);
    CPU_PBMm_SET(cpu_pbm, 0, 1);
    ioerr += WRITE_CPU_PBMm(unit, 0, cpu_pbm);
    CPU_PBM_2m_CLR(cpu_pbm_2);
    CPU_PBM_2m_SET(cpu_pbm_2, 0, 1);
    ioerr += WRITE_CPU_PBM_2m(unit, 0, cpu_pbm_2);

    /* Configure loopback ports */
    sal_memset(pbm, 0, sizeof(pbm));
    BCMDRD_PBMP_ITER(lpbmp_lb, lport) {
        PBM_PORT_ADD(pbm, lport);
    }
    DEVICE_LOOPBACK_PORTS_BITMAPm_CLR(dev_lpbk_pbmp);
    DEVICE_LOOPBACK_PORTS_BITMAPm_BITMAPf_SET(dev_lpbk_pbmp, pbm);
    ioerr += WRITE_DEVICE_LOOPBACK_PORTS_BITMAPm(unit, 0, dev_lpbk_pbmp);

    BCMDRD_PBMP_ITER(lpbmp_lb, lport) {
        pipe = bcmdrd_dev_logic_port_pipe(unit, lport);
        if (pipe < 0) {
            continue;
        }
        sal_memset(pbm, 0, sizeof(pbm));
        PBM_PORT_ADD(pbm, lport);
        MULTIPASS_LOOPBACK_BITMAPm_CLR(mltpss_lpbk_bmp);
        MULTIPASS_LOOPBACK_BITMAPm_BITMAPf_SET(mltpss_lpbk_bmp, pbm);
        ioerr += WRITE_MULTIPASS_LOOPBACK_BITMAPm(unit, pipe, 0,
                                                  mltpss_lpbk_bmp);
    }

    /* Configure port type. */
    EGR_ING_PORTm_CLR(egr_ing_port);
    EGR_ING_PORTm_PORT_TYPEf_SET(egr_ing_port, 1);
    ioerr += WRITE_EGR_ING_PORTm(unit, 136, egr_ing_port); /* cpu_hg_index */
    EGR_ING_PORTm_PORT_TYPEf_SET(egr_ing_port, 2);
    BCMDRD_PBMP_ITER(lpbmp_lb, lport) {
        ioerr += WRITE_EGR_ING_PORTm(unit, lport, egr_ing_port);
    }

    /* Configure EP credit */
    EGR_MMU_CELL_CREDITr_CLR(egr_mmu_crdt);
    EGR_MMU_CELL_CREDITr_CREDITf_SET(egr_mmu_crdt, 13);
    lport = P2L(unit, CMIC_PORT);
    ioerr += WRITE_EGR_MMU_CELL_CREDITr(unit, lport, egr_mmu_crdt);
    EGR_MMU_CELL_CREDITr_CREDITf_SET(egr_mmu_crdt, 44);
    BCMDRD_PBMP_ITER(lpbmp_lb, lport) {
        ioerr += WRITE_EGR_MMU_CELL_CREDITr(unit, lport, egr_mmu_crdt);
    }

    /* Configure egress transmit start count */
    EGR_XMIT_START_COUNTm_CLR(egr_xmt_cnt);
    EGR_XMIT_START_COUNTm_THRESHOLDf_SET(egr_xmt_cnt, 18);
    BCMDRD_PBMP_ITER(lpbmp_all, lport) {
        pipe = bcmdrd_dev_logic_port_pipe(unit, lport);
        for (class = 0; class < 13; class++) {
            idx = (lport % 34) * 16 + class;
            ioerr += WRITE_EGR_XMIT_START_COUNTm(unit, pipe, idx, egr_xmt_cnt);
        }
    }

    /* Enable egress */
    EGR_ENABLEm_CLR(egr_en);
    EGR_ENABLEm_PRT_ENABLEf_SET(egr_en, 1);
    BCMDRD_PBMP_ITER(lpbmp_all, lport) {
        ioerr += WRITE_EGR_ENABLEm(unit, lport, egr_en);
    }

    ioerr += BCMBD_CMICD_MEM_CLEAR(unit, EPC_LINK_BMAPm);

    ioerr += READ_ING_CONFIG_64r(unit, &ing_cfg_64);
    ING_CONFIG_64r_L3SRC_HIT_ENABLEf_SET(ing_cfg_64, 1);
    ING_CONFIG_64r_L2DST_HIT_ENABLEf_SET(ing_cfg_64, 1);
    ING_CONFIG_64r_APPLY_EGR_MASK_ON_L2f_SET(ing_cfg_64, 1);
    ING_CONFIG_64r_APPLY_EGR_MASK_ON_L3f_SET(ing_cfg_64, 1);
    ING_CONFIG_64r_ARP_RARP_TO_FPf_SET(ing_cfg_64, 0x3); /* enable both ARP & RARP */
    ING_CONFIG_64r_ARP_VALIDATION_ENf_SET(ing_cfg_64, 1);
    ING_CONFIG_64r_IGNORE_HG_HDR_LAG_FAILOVERf_SET(ing_cfg_64, 0);
    ioerr += WRITE_ING_CONFIG_64r(unit, ing_cfg_64);

    ioerr += READ_EGR_CONFIG_1r(unit, &egr_cfg_1);
    EGR_CONFIG_1r_RING_MODEf_SET(egr_cfg_1, 1);
    ioerr += WRITE_EGR_CONFIG_1r(unit, egr_cfg_1);

    SHR_RETURN_VAL_EXIT(ioerr ? SHR_E_IO : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */

int
bcm56960_a0_bd_dev_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ipep_init(unit));

    SHR_IF_ERR_EXIT
        (misc_init(unit));

    SHR_IF_ERR_EXIT
        (port_init(unit));

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
        SHR_IF_ERR_EXIT
            (sim_init(unit));
    }

    SHR_IF_ERR_EXIT
        (switching_init(unit));

exit:
    SHR_FUNC_EXIT();
}
