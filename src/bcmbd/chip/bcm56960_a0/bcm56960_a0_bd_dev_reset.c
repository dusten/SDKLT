/*! \file bcm56960_a0_bd_dev_reset.c
 *
 * Issue device reset and pull all chip blocks out of reset.
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
#include <shr/shr_pb.h>

#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56960_a0_acc.h>

#include "bcm56960_a0_drv.h"
#include "bcm56960_a0_internal.h"

/* Log source for this component */
#define BSL_LOG_MODULE                  BSL_LS_BCMBD_DEV

/*******************************************************************************
* Local definitions
 */

#define PLL_LOCK_MSEC                   100
#define RESET_SLEEP_USEC                100

/*******************************************************************************
* Private functions
 */

/*!
 * \brief Convert the physical port number to block port number.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] blktype Block type.
 *
 * \return The block port number or -1 when failed to convert.
 */
static int
block_port_get(int unit, int port, int blktype)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk)) {
        return -1;
    }

    return pblk.lane;
}

/*!
 * \brief Get the first phyiscal port bitmap of the given block type.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype Block type.
 * \param [out] pbmp The first physical port bitmap of \c blktype.
 *
 * \retval 0 No errors.
 * \retval -1 Failed to get the first port bitmap of \c blktype.
 */
static int
block_pbmp_get(int unit, int blktype, bcmdrd_pbmp_t *pbmp)
{
    int port;

    if (bcmdrd_dev_blktype_pbmp(unit, blktype, pbmp) < 0) {
        return -1;
    }

    BCMDRD_PBMP_ITER(*pbmp, port) {
        /* Only save the first port of each block instances. */
        if (block_port_get(unit, port, blktype) != 0) {
            BCMDRD_PBMP_PORT_REMOVE(*pbmp, port);
        }
    }

    return 0;
}

/*!
 * \brief Reset CLPORT blocks.
 *
 * This function is used to reset the CLPORT blocks.
 *
 * In order to reduce the total waiting time, we reset the blocks in parallel.
 *
 * \param [in] unit Unit number.
 *
 * \return The counts of read or write error.
 */
static int
clport_reset(int unit)
{
    int ioerr = 0, port;
    bcmdrd_pbmp_t pbmp;
    CLPORT_MAC_CONTROLr_t cmac_ctrl;
    CLPORT_XGXS0_CTRL_REGr_t xgxs_ctrl;

    if (block_pbmp_get(unit, BLKTYPE_CLPORT, &pbmp) < 0) {
        return 0;
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Assert MAC reset. */
        ioerr += READ_CLPORT_MAC_CONTROLr(unit, &cmac_ctrl, port);
        CLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(cmac_ctrl, 1);
        ioerr += WRITE_CLPORT_MAC_CONTROLr(unit, cmac_ctrl, port);
    }

    sal_usleep(10);

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Deassert MAC reset. */
        CLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(cmac_ctrl, 0);
        ioerr += WRITE_CLPORT_MAC_CONTROLr(unit, cmac_ctrl, port);

        /* Configure clock source */
        ioerr += READ_CLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
        CLPORT_XGXS0_CTRL_REGr_REFIN_ENf_SET(xgxs_ctrl, 1);
        ioerr += WRITE_CLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);

        /* Deassert power down */
        ioerr += READ_CLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
        CLPORT_XGXS0_CTRL_REGr_PWRDWNf_SET(xgxs_ctrl, 0);
        ioerr += WRITE_CLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    }

    sal_usleep(RESET_SLEEP_USEC);

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Reset XGXS */
        ioerr += READ_CLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
        CLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(xgxs_ctrl, 0);
        ioerr += WRITE_CLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    }

    sal_usleep(RESET_SLEEP_USEC + 10000);

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Bring XGXS out of reset */
        CLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(xgxs_ctrl, 1);
        ioerr += WRITE_CLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    }

    sal_usleep(RESET_SLEEP_USEC);

    return ioerr;
}

/*!
 * \brief Reset XLPORT blocks.
 *
 * This function is used to reset the XLPORT blocks.
 *
 * In order to reduce the total waiting time, we reset the blocks in parallel.
 *
 * \param [in] unit Unit number.
 *
 * \return The counts of read or write error.
 */
static int
xlport_reset(int unit)
{
    int ioerr = 0, port;
    bcmdrd_pbmp_t pbmp;
    XLPORT_MAC_CONTROLr_t xmac_ctrl;
    XLPORT_XGXS0_CTRL_REGr_t xgxs_ctrl;

    if (block_pbmp_get(unit, BLKTYPE_XLPORT, &pbmp) < 0) {
        return 0;
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Assert MAC reset. */
        ioerr += READ_XLPORT_MAC_CONTROLr(unit, &xmac_ctrl, port);
        XLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(xmac_ctrl, 1);
        ioerr += WRITE_XLPORT_MAC_CONTROLr(unit, xmac_ctrl, port);
    }

    sal_usleep(10);

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Deassert MAC reset. */
        XLPORT_MAC_CONTROLr_XMAC0_RESETf_SET(xmac_ctrl, 0);
        ioerr += WRITE_XLPORT_MAC_CONTROLr(unit, xmac_ctrl, port);

        /* Configure clock source. */
        ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
        XLPORT_XGXS0_CTRL_REGr_REFIN_ENf_SET(xgxs_ctrl, 1);
        ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);

        /* Deassert power down. */
        ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
        XLPORT_XGXS0_CTRL_REGr_PWRDWNf_SET(xgxs_ctrl, 0);
        ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    }

    sal_usleep(RESET_SLEEP_USEC);

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Reset XGXS. */
        ioerr += READ_XLPORT_XGXS0_CTRL_REGr(unit, &xgxs_ctrl, port);
        XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(xgxs_ctrl, 0);
        ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    }

    sal_usleep(RESET_SLEEP_USEC + 10000);

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Bring XGXS out of reset. */
        XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(xgxs_ctrl, 1);
        ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(unit, xgxs_ctrl, port);
    }

    sal_usleep(RESET_SLEEP_USEC);

    return ioerr;
}

static int
wait_for_pll_lock(int unit)
{
    int ioerr = 0;
    bool real_hw = bcmdrd_feature_is_real_hw(unit);
    int msec, idx, locks;
    shr_pb_t *pb;
    TOP_XG_PLL_STATUSr_t pll_status[4];
    TOP_TS_PLL_STATUSr_t ts_pll_status;
    TOP_BS_PLL0_STATUSr_t bs_pll0_status;

    if (!real_hw) {
        return ioerr;
    }

    /* Wait for LC PLL locks */
    for (msec = 0; msec < PLL_LOCK_MSEC; msec++) {
        locks = 0;
        for (idx = 0; idx < COUNTOF(pll_status); idx++) {
            ioerr += READ_TOP_XG_PLL_STATUSr(unit, idx, &pll_status[idx]);
            locks += TOP_XG_PLL_STATUSr_TOP_XGPLL_LOCKf_GET(pll_status[idx]);
        }
        if (locks == COUNTOF(pll_status)) {
            break;
        }
        sal_usleep(1000);
    }
    if (msec >= PLL_LOCK_MSEC) {
        pb = shr_pb_create();
        shr_pb_printf(pb, "LC PLL did not lock, status = ");
        for (idx = 0; idx < COUNTOF(pll_status); idx++) {
            shr_pb_printf(pb, "0x%08"PRIx32" ",
                          TOP_XG_PLL_STATUSr_GET(pll_status[idx]));
        }
        shr_pb_printf(pb, "\n");
        LOG_WARN(BSL_LS_BCMBD_DEV,
                 (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    /* Wait for timesync/broadsync PLL locks */
    for (msec = 0; msec < PLL_LOCK_MSEC; msec++) {
        ioerr += READ_TOP_TS_PLL_STATUSr(unit, &ts_pll_status);
        ioerr += READ_TOP_BS_PLL0_STATUSr(unit, &bs_pll0_status);
        if (TOP_TS_PLL_STATUSr_PLL_LOCKf_GET(ts_pll_status) == 1 &&
            TOP_BS_PLL0_STATUSr_PLL_LOCKf_GET(bs_pll0_status) == 1) {
            break;
        }
        sal_usleep(1000);
    }
    if (msec >= PLL_LOCK_MSEC) {
        LOG_WARN(BSL_LS_BCMBD_DEV,
                 (BSL_META_U(unit,
                             "TS/BS PLL did not lock, status = "
                             "0x%08"PRIx32" 0x%08"PRIx32"\n"),
                  TOP_TS_PLL_STATUSr_GET(ts_pll_status),
                  TOP_BS_PLL0_STATUSr_GET(bs_pll0_status)));
    }

    return ioerr;
}

/*******************************************************************************
* Public functions
 */

int
bcm56960_a0_bd_dev_reset(int unit)
{
    int ioerr = 0;
    int wait_usec = 10000;
    int idx;
    CMIC_SBUS_RING_MAPr_t sbus_ring_map;
    CMIC_SBUS_TIMEOUTr_t sbus_to;
    CMIC_CPS_RESETr_t cps_reset;
    TOP_SOFT_RESET_REGr_t top_sreset;
    TOP_SOFT_RESET_REG_2r_t top_sreset_2;
    TOP_SOFT_RESET_REG_3r_t top_sreset_3;
    TOP_CORE_PLL1_CTRL_1r_t core_pll_ctrl_1;
    TOP_XG_PLL_CTRL_0r_t pll_ctrl_0;
    TOP_XG_PLL_CTRL_4r_t pll_ctrl_4;
    TOP_TS_PLL_CTRL_0r_t ts_pll_ctrl_0;
    TOP_TS_PLL_CTRL_2r_t ts_pll_ctrl_2;
    TOP_TS_PLL_CTRL_3r_t ts_pll_ctrl_3;
    TOP_TS_PLL_CTRL_4r_t ts_pll_ctrl_4;
    TOP_BS_PLL0_CTRL_0r_t bs_pll0_ctrl_0;
    TOP_BS_PLL0_CTRL_2r_t bs_pll0_ctrl_2;
    TOP_BS_PLL0_CTRL_3r_t bs_pll0_ctrl_3;
    TOP_BS_PLL0_CTRL_4r_t bs_pll0_ctrl_4;
    TOP_BS_PLL1_CTRL_0r_t bs_pll1_ctrl_0;
    TOP_BS_PLL1_CTRL_2r_t bs_pll1_ctrl_2;
    TOP_BS_PLL1_CTRL_3r_t bs_pll1_ctrl_3;
    TOP_BS_PLL1_CTRL_4r_t bs_pll1_ctrl_4;
    TOP_PVTMON_CTRL_0r_t pvtmon_ctrl;
    uint32_t ring_map[] = { 0x55222100, 0x30004005, 0x43333333, 0x44444444,
                            0x34444444, 0x03333333, 0x30000000, 0x00005000 };

    /* Initialize endian mode for correct reset access */
    ioerr += bcmbd_cmicd_init(unit);

    /* Pull reset line */
    ioerr += READ_CMIC_CPS_RESETr(unit, &cps_reset);
    CMIC_CPS_RESETr_CPS_RESETf_SET(cps_reset, 1);
    ioerr += WRITE_CMIC_CPS_RESETr(unit, cps_reset);

    /* Wait for all tables to initialize */
    sal_usleep(wait_usec);

    /* Re-initialize endian mode after reset */
    ioerr += bcmbd_cmicd_init(unit);

    /*
     * SBUS ring and block number:
     * ring 0: IP(1), LBPORT0(54), LBPORT1(51), LBPORT2(52), LBPORT3(53)
     * ring 1: EP(2)
     * ring 2: MMU_XPE(3), MMU_SC(4), MMU_GLB(5)
     * ring 3: PM7(22), PM6(21), PM5(20), PM4(19), PM3(18), PM2(17), PM1(16),
     *           PM0(15), PM31(46), PM30(45), PM29(44), PM28(43), PM27(42),
     *           PM26(41), PM25(40), PM24(39), CLPORT32(55)
     * ring 4: PM32(11), PM8(23), PM9(24), PM10(25), PM11(26), PM12(27),
     *           PM13(28), PM14(29), PM15(30), PM16(31), PM17(32), PM18(33),
     *           PM19(34), PM20(35), PM21(36), PM22(37), PM23(38)
     * ring 5: OTPC(6), AVS(59), TOP(7), SER(8)
     */
    for (idx = 0; idx < COUNTOF(ring_map); idx++) {
        CMIC_SBUS_RING_MAPr_SET(sbus_ring_map, ring_map[idx]);
        ioerr += WRITE_CMIC_SBUS_RING_MAPr(unit, idx, sbus_ring_map);
    }
    CMIC_SBUS_TIMEOUTr_SET(sbus_to, 0x7d0);
    ioerr += WRITE_CMIC_SBUS_TIMEOUTr(unit, sbus_to);

    /* Reset all blocks */
    TOP_SOFT_RESET_REGr_CLR(top_sreset);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);

    /* Power on core pLL1 */
    ioerr += READ_TOP_CORE_PLL1_CTRL_1r(unit, &core_pll_ctrl_1);
    TOP_CORE_PLL1_CTRL_1r_PWRONf_SET(core_pll_ctrl_1, 0);
    ioerr += WRITE_TOP_CORE_PLL1_CTRL_1r(unit, core_pll_ctrl_1);

    /* Program LCPLL frequency */
    for (idx = 0; idx < 4; idx++) {
        ioerr += READ_TOP_XG_PLL_CTRL_4r(unit, idx, &pll_ctrl_4);
        TOP_XG_PLL_CTRL_4r_POST_RST_SELf_SET(pll_ctrl_4, 3);
        ioerr += WRITE_TOP_XG_PLL_CTRL_4r(unit, idx, pll_ctrl_4);

        ioerr += READ_TOP_XG_PLL_CTRL_0r(unit, idx, &pll_ctrl_0);
        TOP_XG_PLL_CTRL_0r_PDIVf_SET(pll_ctrl_0, 1);
        TOP_XG_PLL_CTRL_0r_NDIV_INTf_SET(pll_ctrl_0, 20);
        ioerr += WRITE_TOP_XG_PLL_CTRL_0r(unit, idx, pll_ctrl_0);
    }

    /* Configure TS PLL */
    ioerr += READ_TOP_TS_PLL_CTRL_0r(unit, &ts_pll_ctrl_0);
    TOP_TS_PLL_CTRL_0r_POST_RESETB_SELECTf_SET(ts_pll_ctrl_0, 3);
    ioerr += WRITE_TOP_TS_PLL_CTRL_0r(unit, ts_pll_ctrl_0);

    ioerr += READ_TOP_TS_PLL_CTRL_2r(unit, &ts_pll_ctrl_2);
    TOP_TS_PLL_CTRL_2r_PDIVf_SET(ts_pll_ctrl_2, 2);
    TOP_TS_PLL_CTRL_2r_CH0_MDIVf_SET(ts_pll_ctrl_2, 5);
    ioerr += WRITE_TOP_TS_PLL_CTRL_2r(unit, ts_pll_ctrl_2);

    ioerr += READ_TOP_TS_PLL_CTRL_3r(unit, &ts_pll_ctrl_3);
    TOP_TS_PLL_CTRL_3r_NDIV_INTf_SET(ts_pll_ctrl_3, 100);
    TOP_TS_PLL_CTRL_3r_NDIV_FRACf_SET(ts_pll_ctrl_3, 0);
    ioerr += WRITE_TOP_TS_PLL_CTRL_3r(unit, ts_pll_ctrl_3);

    ioerr += READ_TOP_TS_PLL_CTRL_4r(unit, &ts_pll_ctrl_4);
    TOP_TS_PLL_CTRL_4r_KAf_SET(ts_pll_ctrl_4, 0);
    TOP_TS_PLL_CTRL_4r_KIf_SET(ts_pll_ctrl_4, 2);
    TOP_TS_PLL_CTRL_4r_KPf_SET(ts_pll_ctrl_4, 3);
    ioerr += WRITE_TOP_TS_PLL_CTRL_4r(unit, ts_pll_ctrl_4);

    /* Configure BS PLL0 */
    ioerr += READ_TOP_BS_PLL0_CTRL_0r(unit, &bs_pll0_ctrl_0);
    TOP_BS_PLL0_CTRL_0r_POST_RESETB_SELECTf_SET(bs_pll0_ctrl_0, 3);
    ioerr += WRITE_TOP_BS_PLL0_CTRL_0r(unit, bs_pll0_ctrl_0);

    ioerr += READ_TOP_BS_PLL0_CTRL_2r(unit, &bs_pll0_ctrl_2);
    TOP_BS_PLL0_CTRL_2r_PDIVf_SET(bs_pll0_ctrl_2, 2);
    TOP_BS_PLL0_CTRL_2r_CH0_MDIVf_SET(bs_pll0_ctrl_2, 125);
    ioerr += WRITE_TOP_BS_PLL0_CTRL_2r(unit, bs_pll0_ctrl_2);

    ioerr += READ_TOP_BS_PLL0_CTRL_3r(unit, &bs_pll0_ctrl_3);
    TOP_BS_PLL0_CTRL_3r_NDIV_INTf_SET(bs_pll0_ctrl_3, 100);
    TOP_BS_PLL0_CTRL_3r_NDIV_FRACf_SET(bs_pll0_ctrl_3, 0);
    ioerr += WRITE_TOP_BS_PLL0_CTRL_3r(unit, bs_pll0_ctrl_3);

    ioerr += READ_TOP_BS_PLL0_CTRL_4r(unit, &bs_pll0_ctrl_4);
    TOP_BS_PLL0_CTRL_4r_KAf_SET(bs_pll0_ctrl_4, 0);
    TOP_BS_PLL0_CTRL_4r_KIf_SET(bs_pll0_ctrl_4, 2);
    TOP_BS_PLL0_CTRL_4r_KPf_SET(bs_pll0_ctrl_4, 3);
    ioerr += WRITE_TOP_BS_PLL0_CTRL_4r(unit, bs_pll0_ctrl_4);

    /* Configure BS PLL1 */
    ioerr += READ_TOP_BS_PLL1_CTRL_0r(unit, &bs_pll1_ctrl_0);
    TOP_BS_PLL1_CTRL_0r_POST_RESETB_SELECTf_SET(bs_pll1_ctrl_0, 3);
    ioerr += WRITE_TOP_BS_PLL1_CTRL_0r(unit, bs_pll1_ctrl_0);

    ioerr += READ_TOP_BS_PLL1_CTRL_2r(unit, &bs_pll1_ctrl_2);
    TOP_BS_PLL1_CTRL_2r_PDIVf_SET(bs_pll1_ctrl_2, 2);
    TOP_BS_PLL1_CTRL_2r_CH0_MDIVf_SET(bs_pll1_ctrl_2, 125);
    ioerr += WRITE_TOP_BS_PLL1_CTRL_2r(unit, bs_pll1_ctrl_2);

    ioerr += READ_TOP_BS_PLL1_CTRL_3r(unit, &bs_pll1_ctrl_3);
    TOP_BS_PLL1_CTRL_3r_NDIV_INTf_SET(bs_pll1_ctrl_3, 100);
    TOP_BS_PLL1_CTRL_3r_NDIV_FRACf_SET(bs_pll1_ctrl_3, 0);
    ioerr += WRITE_TOP_BS_PLL1_CTRL_3r(unit, bs_pll1_ctrl_3);

    ioerr += READ_TOP_BS_PLL1_CTRL_4r(unit, &bs_pll1_ctrl_4);
    TOP_BS_PLL1_CTRL_4r_KAf_SET(bs_pll1_ctrl_4, 0);
    TOP_BS_PLL1_CTRL_4r_KIf_SET(bs_pll1_ctrl_4, 2);
    TOP_BS_PLL1_CTRL_4r_KPf_SET(bs_pll1_ctrl_4, 3);
    ioerr += WRITE_TOP_BS_PLL1_CTRL_4r(unit, bs_pll1_ctrl_4);

    /* Bring PLLs out of reset */
    ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL0_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL1_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL2_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL3_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_TS_PLL_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_BS_PLL0_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_BS_PLL1_RST_Lf_SET(top_sreset_2, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);
    sal_usleep(wait_usec);

    /* Initialize LCPLLs */
    ioerr += wait_for_pll_lock(unit);

    /* De-assert LCPLL's post reset */
    ioerr += READ_TOP_SOFT_RESET_REG_2r(unit, &top_sreset_2);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL0_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL1_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL2_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_XG_PLL3_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_TS_PLL_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_BS_PLL0_POST_RST_Lf_SET(top_sreset_2, 1);
    TOP_SOFT_RESET_REG_2r_TOP_BS_PLL1_POST_RST_Lf_SET(top_sreset_2, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REG_2r(unit, top_sreset_2);
    sal_usleep(wait_usec);

    /* Set correct value for BG_ADJ */
    ioerr += READ_TOP_PVTMON_CTRL_0r(unit, &pvtmon_ctrl);
    TOP_PVTMON_CTRL_0r_BG_ADJf_SET(pvtmon_ctrl, 0);
    ioerr += WRITE_TOP_PVTMON_CTRL_0r(unit, pvtmon_ctrl);

    /* Bring port blocks out of reset */
    TOP_SOFT_RESET_REGr_CLR(top_sreset);
    TOP_SOFT_RESET_REGr_TOP_PM32_RST_Lf_SET(top_sreset, 1);
    TOP_SOFT_RESET_REGr_TOP_TS_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    TOP_SOFT_RESET_REG_3r_SET(top_sreset_3, 0xffffffff);
    ioerr += WRITE_TOP_SOFT_RESET_REG_3r(unit, top_sreset_3);
    sal_usleep(wait_usec);

    /* Bring IP, EP, and MMU blocks out of reset */
    ioerr += READ_TOP_SOFT_RESET_REGr(unit, &top_sreset);
    TOP_SOFT_RESET_REGr_TOP_IP_RST_Lf_SET(top_sreset, 1);
    TOP_SOFT_RESET_REGr_TOP_EP_RST_Lf_SET(top_sreset, 1);
    TOP_SOFT_RESET_REGr_TOP_MMU_RST_Lf_SET(top_sreset, 1);
    ioerr += WRITE_TOP_SOFT_RESET_REGr(unit, top_sreset);
    sal_usleep(wait_usec);

    /* Reset port blocks */
    ioerr += clport_reset(unit);
    ioerr += xlport_reset(unit);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}
