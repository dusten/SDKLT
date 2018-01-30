/*! \file bcmbd_cmicd_init.c
 *
 * <description>
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

#include <bcmbd/bcmbd_cmicd_acc.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

/*
 * Basic CMIC Initialization
 */
#define CMIC_BIG_ENDIAN_PIO             0x01000001

int
bcmbd_cmicd_init(int unit)
{
    bool byte_swap_pio;
    CMIC_CMC_PCIE_IRQ_MASK0r_t irq_mask0;
    CMIC_CMC_PCIE_IRQ_MASK1r_t irq_mask1;
    CMIC_COMMON_UC0_PIO_ENDIANESSr_t uc0_pio_en;
    CMIC_COMMON_PCIE_PIO_ENDIANESSr_t pcie_pio_en;
    bcmdrd_dev_t *dev;
    int cmc = BCMBD_CMICD_CMC_GET(unit);
    int ioerr = 0;

    /*
     * Certain PCIe cores may occasionally return invalid data in the
     * first PCI read following a soft-reset (CPS reset). The following
     * read operation is a dummy read to ensure that any invalid data
     * is flushed from the PCI read pipeline.
     */
    ioerr += READ_CMIC_CMC_PCIE_IRQ_MASK0r(unit, cmc, &irq_mask0);
    ioerr += READ_CMIC_CMC_PCIE_IRQ_MASK1r(unit, cmc, &irq_mask1);

    /* Configure endian */
    byte_swap_pio = bcmdrd_dev_byte_swap_pio_get(unit);
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_IPROC)) {
        CMIC_COMMON_UC0_PIO_ENDIANESSr_CLR(uc0_pio_en);
        if (byte_swap_pio) {
            CMIC_COMMON_UC0_PIO_ENDIANESSr_SET(uc0_pio_en, CMIC_BIG_ENDIAN_PIO);
        }
        ioerr += WRITE_CMIC_COMMON_UC0_PIO_ENDIANESSr(unit, uc0_pio_en);
    } else {
        CMIC_COMMON_PCIE_PIO_ENDIANESSr_CLR(pcie_pio_en);
        if (byte_swap_pio) {
            CMIC_COMMON_PCIE_PIO_ENDIANESSr_SET(pcie_pio_en, CMIC_BIG_ENDIAN_PIO);
        }
        ioerr += WRITE_CMIC_COMMON_PCIE_PIO_ENDIANESSr(unit, pcie_pio_en);
    }

    /* Disable Interrupts */
    CMIC_CMC_PCIE_IRQ_MASK0r_CLR(irq_mask0);
    ioerr += WRITE_CMIC_CMC_PCIE_IRQ_MASK0r(unit, cmc, irq_mask0);
    CMIC_CMC_PCIE_IRQ_MASK1r_CLR(irq_mask1);
    ioerr += WRITE_CMIC_CMC_PCIE_IRQ_MASK1r(unit, cmc, irq_mask1);

    

    /* Additional configuration required when on PCI bus */
    dev = bcmdrd_dev_get(unit);
    if (dev && (dev->io.bus_type == BCMDRD_BT_PCI)) {
        int idx;
        CMIC_CMC_HOSTMEM_ADDR_REMAPr_t hostmem_remap;
        uint32_t remap_val[] = { 0x144D2450, 0x19617595, 0x1E75C6DA, 0x1f };

        /* Send DMA data to external host memory when on PCI bus */
        for (idx = 0; idx < COUNTOF(remap_val); idx++) {
            CMIC_CMC_HOSTMEM_ADDR_REMAPr_SET(hostmem_remap, remap_val[idx]);
            ioerr += WRITE_CMIC_CMC_HOSTMEM_ADDR_REMAPr(unit, cmc, idx,
                                                        hostmem_remap);
        }
    }

    return ioerr;
}
