/*! \file bcm56960_a0_cmic_intr.h
 *
 * Device-specific CMIC interrupt sources for BCM56960_A0.
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

#ifndef BCM56960_A0_CMIC_INTR_H
#define BCM56960_A0_CMIC_INTR_H

#include <bcmbd/bcmbd_cmicd_intr.h>

/*!
 * \name Device-specific CMIC interrupt sources.
 *
 * Device-specific interrupt sources in the CMIC_CMCx_IRQ_MASKx
 * registers.
 *
 * Please refer to the hardware documentation for details.
 *
 * \{
 */
#define PCIE_ECRC_ERR_INTR              (CMICD_IRQ_BASE1 + 1)
#define UART0_INTERRUPT                 (CMICD_IRQ_BASE1 + 6)
#define UART1_INTERRUPT                 (CMICD_IRQ_BASE1 + 7)
#define UC_0_PMUIRQ                     (CMICD_IRQ_BASE1 + 19)
#define UC_1_PMUIRQ                     (CMICD_IRQ_BASE1 + 20)
#define WDT_0_INTR                      (CMICD_IRQ_BASE1 + 21)
#define WDT_1_INTR                      (CMICD_IRQ_BASE1 + 22)
#define PCIEINTF_NEEDS_CLEANUP          (CMICD_IRQ_BASE1 + 27)
#define SRAM_ECC_INTR                   (CMICD_IRQ_BASE1 + 28)

#define CEV_TO_CMIC_INTR                (CMICD_IRQ_BASE3 + 0)
#define L2_MGMT_TO_CMIC_INTR            (CMICD_IRQ_BASE3 + 1)
#define PVTMON_INTR                     (CMICD_IRQ_BASE3 + 2)
#define PLL_LOCK_LOSS_INTR              (CMICD_IRQ_BASE3 + 3)
#define MMU_TO_CMIC_MEMFAIL_INTR        (CMICD_IRQ_BASE3 + 4)
#define PIPE0_EP2_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 5)
#define PIPE1_EP2_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 6)
#define PIPE2_EP2_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 7)
#define PIPE3_EP2_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 8)
#define PIPE0_IP0_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 9)
#define PIPE1_IP0_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 10)
#define PIPE2_IP0_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 11)
#define PIPE3_IP0_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 12)
#define PIPE0_IP5_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 13)
#define PIPE1_IP5_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 14)
#define PIPE2_IP5_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 15)
#define PIPE3_IP5_TO_CMIC_PERR_INTR     (CMICD_IRQ_BASE3 + 16)
#define AVS_SW_MDONE_INTR               (CMICD_IRQ_BASE3 + 17)
#define AVS_ROSC_THRESHOLD2_INTR        (CMICD_IRQ_BASE3 + 18)
#define AVS_ROSC_THRESHOLD1_INTR        (CMICD_IRQ_BASE3 + 19)
#define AVS_POWERWATCHDOG_INTR          (CMICD_IRQ_BASE3 + 20)
#define AVS_VDDC_MON_WARNING1_INTR      (CMICD_IRQ_BASE3 + 21)
#define AVS_VDDC_MON_WARNING0_INTR      (CMICD_IRQ_BASE3 + 22)
#define AVS_TEMP_RESET_INTR             (CMICD_IRQ_BASE3 + 23)
#define AVS_TEMP_LOW_THRESHOLD_INTR     (CMICD_IRQ_BASE3 + 24)
#define AVS_TEMP_HIGH_THRESHOLD_INTR    (CMICD_IRQ_BASE3 + 25)

#define PM0_INTR                        (CMICD_IRQ_BASE4 + 0)
#define PM1_INTR                        (CMICD_IRQ_BASE4 + 1)
#define PM2_INTR                        (CMICD_IRQ_BASE4 + 2)
#define PM3_INTR                        (CMICD_IRQ_BASE4 + 3)
#define PM4_INTR                        (CMICD_IRQ_BASE4 + 4)
#define PM5_INTR                        (CMICD_IRQ_BASE4 + 5)
#define PM6_INTR                        (CMICD_IRQ_BASE4 + 6)
#define PM7_INTR                        (CMICD_IRQ_BASE4 + 7)
#define PM8_INTR                        (CMICD_IRQ_BASE4 + 8)
#define PM9_INTR                        (CMICD_IRQ_BASE4 + 9)
#define PM10_INTR                       (CMICD_IRQ_BASE4 + 10)
#define PM11_INTR                       (CMICD_IRQ_BASE4 + 11)
#define PM12_INTR                       (CMICD_IRQ_BASE4 + 12)
#define PM13_INTR                       (CMICD_IRQ_BASE4 + 13)
#define PM14_INTR                       (CMICD_IRQ_BASE4 + 14)
#define PM15_INTR                       (CMICD_IRQ_BASE4 + 15)
#define PM16_INTR                       (CMICD_IRQ_BASE4 + 16)
#define PM17_INTR                       (CMICD_IRQ_BASE4 + 17)
#define PM18_INTR                       (CMICD_IRQ_BASE4 + 18)
#define PM19_INTR                       (CMICD_IRQ_BASE4 + 19)
#define PM20_INTR                       (CMICD_IRQ_BASE4 + 20)
#define PM21_INTR                       (CMICD_IRQ_BASE4 + 21)
#define PM22_INTR                       (CMICD_IRQ_BASE4 + 22)
#define PM23_INTR                       (CMICD_IRQ_BASE4 + 23)
#define PM24_INTR                       (CMICD_IRQ_BASE4 + 24)
#define PM25_INTR                       (CMICD_IRQ_BASE4 + 25)
#define PM26_INTR                       (CMICD_IRQ_BASE4 + 26)
#define PM27_INTR                       (CMICD_IRQ_BASE4 + 27)
#define PM28_INTR                       (CMICD_IRQ_BASE4 + 28)
#define PM29_INTR                       (CMICD_IRQ_BASE4 + 29)
#define PM30_INTR                       (CMICD_IRQ_BASE4 + 30)
#define PM31_INTR                       (CMICD_IRQ_BASE4 + 31)

#define PM32_INTR                       (CMICD_IRQ_BASE5 + 0)
/*! \} */

#endif /* BCM56960_A0_CMIC_INTR_H */
