/*! \file bcmcnet_cmicd_pdma_hw.c
 *
 * Utility routines for handling BCMCNET hardware (CMICd).
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

#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>
#include <bcmcnet/bcmcnet_rxtx.h>
#include <bcmcnet/bcmcnet_cmicd.h>

/*!
 * Read 32-bit register
 */
static inline void
cmicd_pdma_reg_read32(struct pdma_hw *hw, uint32_t addr, uint32_t *data)
{
    if (hw->dev->dev_read32) {
        hw->dev->dev_read32(hw->dev, addr, data);
    } else {
        DEV_READ32(&hw->dev->ctrl, addr, data);
    }
}

/*!
 * Write 32-bit register
 */
static inline void
cmicd_pdma_reg_write32(struct pdma_hw *hw, uint32_t addr, uint32_t data)
{
    if (hw->dev->dev_write32) {
        hw->dev->dev_write32(hw->dev, addr, data);
    } else {
        DEV_WRITE32(&hw->dev->ctrl, addr, data);
    }
}

/*!
 * Enable interrupt for a channel
 */
static inline void
cmicd_pdma_intr_enable(struct pdma_hw *hw, int cmc, int chan, uint32_t mask)
{
    uint32_t reg = CMICD_IRQ_STAT(cmc);

    hw->dev->intr_unmask(hw->dev, cmc, chan, reg, mask);
}

/*!
 * Disable interrupt for a channel
 */
static inline void
cmicd_pdma_intr_disable(struct pdma_hw *hw, int cmc, int chan, uint32_t mask)
{
    uint32_t reg = CMICD_IRQ_STAT(cmc);

    hw->dev->intr_mask(hw->dev, cmc, chan, reg, mask);
}

/*!
 * Initialize HW
 */
static int
cmicd_pdma_hw_init(struct pdma_hw *hw)
{
    uint32_t val;
    int gi, qi;

    for (gi = 0; gi < hw->dev->num_groups; gi++) {
        if (!hw->dev->ctrl.grp[gi].attached) {
            continue;
        }
        for (qi = 0; qi < CMICD_PDMA_CMC_CHAN; qi++) {
            hw->hdls.reg_wr32(hw, CMICD_PDMA_CTRL(gi, qi), 0);
            hw->hdls.reg_wr32(hw, CMICD_PDMA_STAT_CLR(gi), CMICD_PDMA_DESC_CMPLT(qi));
            hw->hdls.reg_wr32(hw, CMICD_PDMA_STAT_CLR(gi), CMICD_PDMA_DESC_CNTLD(qi));
        }
    }

    hw->info.name = CMICD_DEV_NAME;
    hw->hdls.reg_rd32(hw, CMICD_CMICM_REV_ID, &val);
    hw->info.ver_no = val;
    hw->hdls.reg_rd32(hw, CMICD_DEV_REV_ID, &val);
    hw->info.dev_id = val & 0xffff;
    hw->info.rev_id = val >> 16;
    hw->info.num_cmcs = CMICD_PDMA_CMC_MAX;
    hw->info.cmc_chans = CMICD_PDMA_CMC_CHAN;
    hw->info.num_chans = CMICD_PDMA_CMC_MAX * CMICD_PDMA_CMC_CHAN;
    hw->info.rx_dcb_size = CMICD_PDMA_DCB_SIZE;
    hw->info.tx_dcb_size = CMICD_PDMA_DCB_SIZE;
    hw->info.rx_ph_size = 0;
    hw->info.tx_ph_size = 0;
    hw->info.intr_start_num = CMICD_IRQ_START_NUM;
    hw->info.intr_num_offset = CMICD_IRQ_NUM_OFFSET;
    hw->info.intr_mask_shift = CMICD_IRQ_MASK_SHIFT;

    return SHR_E_NONE;
}

/*!
 * Configure HW
 */
static int
cmicd_pdma_hw_config(struct pdma_hw *hw)
{
    struct dev_ctrl *ctrl = &hw->dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    struct pdma_tx_queue *txq = NULL;
    uint32_t val, que_ctrl;
    int grp, que;
    uint32_t qi;

    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        rxq = (struct pdma_rx_queue *)ctrl->rx_queue[qi];
        grp = rxq->group_id;
        que = rxq->chan_id % CMICD_PDMA_CMC_CHAN;
        que_ctrl = ctrl->grp[grp].que_ctrl[que];

        hw->hdls.reg_rd32(hw, CMICD_PDMA_CTRL(grp, que), &val);
        if (que_ctrl & PDMA_PKT_BYTE_SWAP) {
            val |= CMICD_PDMA_PKT_BIG_ENDIAN;
        }
        if (que_ctrl & PDMA_OTH_BYTE_SWAP) {
            val |= CMICD_PDMA_DESC_BIG_ENDIAN;
        }
        if (!(hw->dev->flags & PDMA_CHAIN_MODE)) {
            val |= CMICD_PDMA_CONTINUOUS;
        }
        val |= CMICD_PDMA_CNTLD_INTR;
        val &= ~CMICD_PDMA_DIR;
        hw->hdls.reg_wr32(hw, CMICD_PDMA_CTRL(grp, que), val);
    }

    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        txq = (struct pdma_tx_queue *)ctrl->tx_queue[qi];
        grp = txq->group_id;
        que = txq->chan_id % CMICD_PDMA_CMC_CHAN;
        que_ctrl = ctrl->grp[grp].que_ctrl[que];

        hw->hdls.reg_rd32(hw, CMICD_PDMA_CTRL(grp, que), &val);
        if (que_ctrl & PDMA_PKT_BYTE_SWAP) {
            val |= CMICD_PDMA_PKT_BIG_ENDIAN;
        }
        if (que_ctrl & PDMA_OTH_BYTE_SWAP) {
            val |= CMICD_PDMA_DESC_BIG_ENDIAN;
        }
        if (!(hw->dev->flags & PDMA_CHAIN_MODE)) {
            val |= CMICD_PDMA_CONTINUOUS;
        }
        val |= CMICD_PDMA_CNTLD_INTR | CMICD_PDMA_DIR;
        hw->hdls.reg_wr32(hw, CMICD_PDMA_CTRL(grp, que), val);
    }

    hw->hdls.reg_wr32(hw, CMICD_RXBUF_EPINTF_RELEASE, 0);
    hw->hdls.reg_wr32(hw, CMICD_RXBUF_EPINTF_RELEASE, 1);

    return SHR_E_NONE;
}

/*!
 * Reset HW
 */
static int
cmicd_pdma_hw_reset(struct pdma_hw *hw)
{
    int gi, qi;

    for (gi = 0; gi < hw->dev->num_groups; gi++) {
        if (!hw->dev->ctrl.grp[gi].attached) {
            continue;
        }
        for (qi = 0; qi < CMICD_PDMA_CMC_CHAN; qi++) {
            hw->hdls.reg_wr32(hw, CMICD_PDMA_CTRL(gi, qi), 0);
        }
    }

    return SHR_E_NONE;
}

/*!
 * Start a channel
 */
static int
cmicd_pdma_chan_start(struct pdma_hw *hw, int chan)
{
    uint32_t val;
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    hw->hdls.reg_rd32(hw, CMICD_PDMA_CTRL(grp, que), &val);
    val |= CMICD_PDMA_ENABLE;
    hw->hdls.reg_wr32(hw, CMICD_PDMA_CTRL(grp, que), val);

    MEMORY_BARRIER;

    return SHR_E_NONE;
}

/*!
 * Stop a channel
 */
static int
cmicd_pdma_chan_stop(struct pdma_hw *hw, int chan)
{
    uint32_t val;
    int grp, que;
    int retry = CMICD_HW_RETRY_TIMES;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    hw->hdls.reg_rd32(hw, CMICD_PDMA_CTRL(grp, que), &val);
    val |= CMICD_PDMA_ENABLE | CMICD_PDMA_ABORT;
    hw->hdls.reg_wr32(hw, CMICD_PDMA_CTRL(grp, que), val);

    MEMORY_BARRIER;

    do {
        hw->hdls.reg_rd32(hw, CMICD_PDMA_STAT(grp), &val);
    } while ((val & CMICD_PDMA_ACTIVE(que)) && (--retry > 0));

    hw->hdls.reg_rd32(hw, CMICD_PDMA_CTRL(grp, que), &val);
    val &= ~(CMICD_PDMA_ENABLE | CMICD_PDMA_ABORT);
    hw->hdls.reg_wr32(hw, CMICD_PDMA_CTRL(grp, que), val);

    hw->hdls.reg_wr32(hw, CMICD_PDMA_STAT_CLR(grp), CMICD_PDMA_DESC_CNTLD(que));

    MEMORY_BARRIER;

    return SHR_E_NONE;
}

/*!
 * Setup a channel
 */
static int
cmicd_pdma_chan_setup(struct pdma_hw *hw, int chan, uint64_t addr)
{
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    hw->hdls.reg_wr32(hw, CMICD_PDMA_DESC(grp, que), addr);

    MEMORY_BARRIER;

    return SHR_E_NONE;
}

/*!
 * Set halt point for a channel
 */
static int
cmicd_pdma_chan_goto(struct pdma_hw *hw, int chan, uint64_t addr)
{
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    hw->hdls.reg_wr32(hw, CMICD_PDMA_DESC_HALT(grp, que), addr);

    MEMORY_BARRIER;

    return SHR_E_NONE;
}

/*!
 * Clear a channel
 */
static int
cmicd_pdma_chan_clear(struct pdma_hw *hw, int chan)
{
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    hw->hdls.reg_wr32(hw, CMICD_PDMA_STAT_CLR(grp), CMICD_PDMA_DESC_CNTLD(que));

    MEMORY_BARRIER;

    return SHR_E_NONE;
}

/*!
 * Enable interrupt for a channel
 */
static int
cmicd_pdma_chan_intr_enable(struct pdma_hw *hw, int chan)
{
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    cmicd_pdma_intr_enable(hw, grp, que, CMICD_IRQ_DESC_CNTLD(que));

    return SHR_E_NONE;
}

/*!
 * Disable interrupt for a channel
 */
static int
cmicd_pdma_chan_intr_disable(struct pdma_hw *hw, int chan)
{
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    cmicd_pdma_intr_disable(hw, grp, que, CMICD_IRQ_DESC_CNTLD(que));

    return SHR_E_NONE;
}

/*!
 * Query interrupt status for a channel
 *
 * In group mode (interrupt processing per CMC), need to query each channel's
 * interrupt status.
 */
static int
cmicd_pdma_chan_intr_query(struct pdma_hw *hw, int chan)
{
    uint32_t val;
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    hw->hdls.reg_rd32(hw, CMICD_IRQ_STAT(grp), &val);

    return val & CMICD_IRQ_DESC_CNTLD(que);
}

/*!
 * Check interrupt validity for a channel
 *
 * In group mode (interrupt processing per CMC), need to check each channel's
 * interrupt validity based on its interrupt mask.
 */
static int
cmicd_pdma_chan_intr_check(struct pdma_hw *hw, int chan)
{
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    if (!(hw->dev->ctrl.grp[grp].irq_mask & CMICD_IRQ_DESC_CNTLD(que))) {
        return 0;
    }

    return cmicd_pdma_chan_intr_query(hw, chan);
}

/*!
 * Coalesce interrupt for a channel
 */
static int
cmicd_pdma_chan_intr_coalesce(struct pdma_hw *hw, int chan, int count, int timer)
{
    uint32_t val;
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    val = CMICD_PDMA_INTR_COAL_ENA |
          CMICD_PDMA_INTR_THRESH(count) |
          CMICD_PDMA_INTR_TIMER(timer);
    hw->hdls.reg_wr32(hw, CMICD_PDMA_INTR_COAL(grp, que), val);

    return SHR_E_NONE;
}

/*!
 * Dump registers for a channel
 */
static int
cmicd_pdma_chan_reg_dump(struct pdma_hw *hw, int chan)
{
    uint32_t val;
    int grp, que;

    grp = chan / CMICD_PDMA_CMC_CHAN;
    que = chan % CMICD_PDMA_CMC_CHAN;

    hw->hdls.reg_rd32(hw, CMICD_PDMA_CTRL(grp, que), &val);
    CNET_PR("CMIC_CMC%d_CH%d_DMA_CTRL: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_DESC(grp, que), &val);
    CNET_PR("CMIC_CMC%d_DMA_DESC%d: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_CURR_DESC(grp, que), &val);
    CNET_PR("CMIC_CMC%d_CH%d_DMA_CURR_DESC: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_DESC_HALT(grp, que), &val);
    CNET_PR("CMIC_CMC%d_DMA_CH%d_DESC_HALT_ADDR: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_COS_RX0(grp, que), &val);
    CNET_PR("CMIC_CMC%d_CH%d_COS_CTRL_RX_0: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_COS_RX1(grp, que), &val);
    CNET_PR("CMIC_CMC%d_CH%d_COS_CTRL_RX_1: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_COS_MASK0(grp), &val);
    CNET_PR("CMIC_CMC%d_PROGRAMMABLE_COS_MASK0: 0x%08x\n", grp, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_COS_MASK1(grp), &val);
    CNET_PR("CMIC_CMC%d_PROGRAMMABLE_COS_MASK1: 0x%08x\n", grp, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_INTR_COAL(grp, que), &val);
    CNET_PR("CMIC_CMC%d_DMA_CH%d_INTR_COAL: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_RBUF_THRE(grp, que), &val);
    CNET_PR("CMIC_CMC%d_CH%d_RXBUF_THRESHOLD_CONFIG: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_STAT(grp), &val);
    CNET_PR("CMIC_CMC%d_DMA_STAT: 0x%08x\n", grp, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_STAT_HI(grp), &val);
    CNET_PR("CMIC_CMC%d_DMA_STAT_HI: 0x%08x\n", grp, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_STAT_CLR(grp), &val);
    CNET_PR("CMIC_CMC%d_DMA_STAT_CLR: 0x%08x\n", grp, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_COUNT_RX(grp, que), &val);
    CNET_PR("CMIC_CMC%d_PKT_COUNT_CH%d_RXPKT: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_PDMA_COUNT_TX(grp, que), &val);
    CNET_PR("CMIC_CMC%d_PKT_COUNT_CH%d_TXPKT: 0x%08x\n", grp, que, val);

    hw->hdls.reg_rd32(hw, CMICD_IRQ_STAT(grp), &val);
    CNET_PR("CMIC_CMC%d_IRQ_STAT0: 0x%08x\n", grp, val);

    hw->hdls.reg_rd32(hw, CMICD_IRQ_PCI_MASK(grp), &val);
    CNET_PR("CMIC_CMC%d_PCIE_IRQ_MASK0: 0x%08x\n", grp, val);

    hw->hdls.reg_rd32(hw, CMICD_DEV_REV_ID, &val);
    CNET_PR("CMIC_DEV_REV_ID: 0x%08x\n", val);

    hw->hdls.reg_rd32(hw, CMICD_CMICM_REV_ID, &val);
    CNET_PR("CMIC_CMICM_REV_ID: 0x%08x\n", val);

    return SHR_E_NONE;
}

/*!
 * Initialize function pointers
 */
int
bcmcnet_cmicd_pdma_hw_hdls_init(struct pdma_hw *hw)
{
    if (!hw) {
        return SHR_E_PARAM;
    }

    hw->hdls.reg_rd32 = cmicd_pdma_reg_read32;
    hw->hdls.reg_wr32 = cmicd_pdma_reg_write32;
    hw->hdls.hw_init = cmicd_pdma_hw_init;
    hw->hdls.hw_config = cmicd_pdma_hw_config;
    hw->hdls.hw_reset = cmicd_pdma_hw_reset;
    hw->hdls.chan_start = cmicd_pdma_chan_start;
    hw->hdls.chan_stop = cmicd_pdma_chan_stop;
    hw->hdls.chan_setup = cmicd_pdma_chan_setup;
    hw->hdls.chan_goto = cmicd_pdma_chan_goto;
    hw->hdls.chan_clear = cmicd_pdma_chan_clear;
    hw->hdls.chan_intr_enable = cmicd_pdma_chan_intr_enable;
    hw->hdls.chan_intr_disable = cmicd_pdma_chan_intr_disable;
    hw->hdls.chan_intr_query = cmicd_pdma_chan_intr_query;
    hw->hdls.chan_intr_check = cmicd_pdma_chan_intr_check;
    hw->hdls.chan_intr_coalesce = cmicd_pdma_chan_intr_coalesce;
    hw->hdls.chan_reg_dump = cmicd_pdma_chan_reg_dump;

    return SHR_E_NONE;
}

