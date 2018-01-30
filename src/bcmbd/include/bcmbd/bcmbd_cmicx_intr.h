/*! \file bcmbd_cmicx_intr.h
 *
 * Interrupt definitions for CMICx.
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

#ifndef BCMBD_CMICX_INTR_H
#define BCMBD_CMICX_INTR_H

#include <bcmbd/bcmbd_intr.h>
#include <bcmbd/bcmbd_cmicx_lp_intr.h>

/*! \{
 *
 * \brief Interrupt source base offsets.
 *
 * All interrupt sources have a unique value, which is derived from a
 * base offset and a bit number in an interrupt status register.
 */
#define IPROC_IRQ_BASE0                 (0 * 32)
#define IPROC_IRQ_BASE1                 (1 * 32)
#define IPROC_IRQ_BASE2                 (2 * 32)
#define IPROC_IRQ_BASE3                 (3 * 32)
#define IPROC_IRQ_BASE4                 (4 * 32)
#define IPROC_IRQ_BASE5                 (5 * 32)
#define IPROC_IRQ_BASE6                 (6 * 32)
#define IPROC_IRQ_BASE7                 (7 * 32)
/*! \} */

/*! \{
 *
 * \brief Interrupt sources in the INTC_INTR_STATUS_REGx registers.
 * \anchor CMICX_IRQ_xxx
 *
 * Please refer to the hardware documentation for details.
 */
#define IPROC_IRQ_SW_PROG_INTR                  (IPROC_IRQ_BASE2 + 9)   /* 73 */

#define IPROC_IRQ_CHIP_LP_INTR                  (IPROC_IRQ_BASE3 + 23)  /* 119 */

#define CMICX_IRQ_CMC0_CH0_DESC_DONE            (IPROC_IRQ_BASE4 + 0)
#define CMICX_IRQ_CMC0_CH0_CHAIN_DONE           (IPROC_IRQ_BASE4 + 1)
#define CMICX_IRQ_CMC0_CH0_INTR_COALESC         (IPROC_IRQ_BASE4 + 2)
#define CMICX_IRQ_CMC0_CH0_DESC_CTRLD           (IPROC_IRQ_BASE4 + 3)
#define CMICX_IRQ_CMC0_CH1_DESC_DONE            (IPROC_IRQ_BASE4 + 4)
#define CMICX_IRQ_CMC0_CH1_CHAIN_DONE           (IPROC_IRQ_BASE4 + 5)
#define CMICX_IRQ_CMC0_CH1_INTR_COALESC         (IPROC_IRQ_BASE4 + 6)
#define CMICX_IRQ_CMC0_CH1_DESC_CTRLD           (IPROC_IRQ_BASE4 + 7)
#define CMICX_IRQ_CMC0_CH2_DESC_DONE            (IPROC_IRQ_BASE4 + 8)
#define CMICX_IRQ_CMC0_CH2_CHAIN_DONE           (IPROC_IRQ_BASE4 + 9)
#define CMICX_IRQ_CMC0_CH2_INTR_COALESC         (IPROC_IRQ_BASE4 + 10)
#define CMICX_IRQ_CMC0_CH2_DESC_CTRLD           (IPROC_IRQ_BASE4 + 11)
#define CMICX_IRQ_CMC0_CH3_DESC_DONE            (IPROC_IRQ_BASE4 + 12)
#define CMICX_IRQ_CMC0_CH3_CHAIN_DONE           (IPROC_IRQ_BASE4 + 13)
#define CMICX_IRQ_CMC0_CH3_INTR_COALESC         (IPROC_IRQ_BASE4 + 14)
#define CMICX_IRQ_CMC0_CH3_DESC_CTRLD           (IPROC_IRQ_BASE4 + 15)
#define CMICX_IRQ_CMC0_CH4_DESC_DONE            (IPROC_IRQ_BASE4 + 16)
#define CMICX_IRQ_CMC0_CH4_CHAIN_DONE           (IPROC_IRQ_BASE4 + 17)
#define CMICX_IRQ_CMC0_CH4_INTR_COALESC         (IPROC_IRQ_BASE4 + 18)
#define CMICX_IRQ_CMC0_CH4_DESC_CTRLD           (IPROC_IRQ_BASE4 + 19)
#define CMICX_IRQ_CMC0_CH5_DESC_DONE            (IPROC_IRQ_BASE4 + 20)
#define CMICX_IRQ_CMC0_CH5_CHAIN_DONE           (IPROC_IRQ_BASE4 + 21)
#define CMICX_IRQ_CMC0_CH5_INTR_COALESC         (IPROC_IRQ_BASE4 + 22)
#define CMICX_IRQ_CMC0_CH5_DESC_CTRLD           (IPROC_IRQ_BASE4 + 23)
#define CMICX_IRQ_CMC0_CH6_DESC_DONE            (IPROC_IRQ_BASE4 + 24)
#define CMICX_IRQ_CMC0_CH6_CHAIN_DONE           (IPROC_IRQ_BASE4 + 25)
#define CMICX_IRQ_CMC0_CH6_INTR_COALESC         (IPROC_IRQ_BASE4 + 26)
#define CMICX_IRQ_CMC0_CH6_DESC_CTRLD           (IPROC_IRQ_BASE4 + 27)
#define CMICX_IRQ_CMC0_CH7_DESC_DONE            (IPROC_IRQ_BASE4 + 28)
#define CMICX_IRQ_CMC0_CH7_CHAIN_DONE           (IPROC_IRQ_BASE4 + 29)
#define CMICX_IRQ_CMC0_CH7_INTR_COALESC         (IPROC_IRQ_BASE4 + 30)
#define CMICX_IRQ_CMC0_CH7_DESC_CTRLD           (IPROC_IRQ_BASE4 + 31)

#define CMICX_IRQ_CMC0_XC_MEMDMA_CH0_DONE       (IPROC_IRQ_BASE5 + 0)
#define CMICX_IRQ_CMC0_XC_MEMDMA_CH1_DONE       (IPROC_IRQ_BASE5 + 1)
#define CMICX_IRQ_CMC0_SBUSDMA_CH0_DONE         (IPROC_IRQ_BASE5 + 2)
#define CMICX_IRQ_CMC0_SBUSDMA_CH1_DONE         (IPROC_IRQ_BASE5 + 3)
#define CMICX_IRQ_CMC0_SBUSDMA_CH2_DONE         (IPROC_IRQ_BASE5 + 4)
#define CMICX_IRQ_CMC0_SBUSDMA_CH3_DONE         (IPROC_IRQ_BASE5 + 5)
#define CMICX_IRQ_CMC0_DOUBLE_BIT_ECCERR        (IPROC_IRQ_BASE5 + 6)
#define CMICX_IRQ_CMC0_SINGLE_BIT_ECCERR        (IPROC_IRQ_BASE5 + 7)
#define CMICX_IRQ_CMC1_CH0_DESC_DONE            (IPROC_IRQ_BASE5 + 16)
#define CMICX_IRQ_CMC1_CH0_CHAIN_DONE           (IPROC_IRQ_BASE5 + 17)
#define CMICX_IRQ_CMC1_CH0_INTR_COALESC         (IPROC_IRQ_BASE5 + 18)
#define CMICX_IRQ_CMC1_CH0_DESC_CTRLD           (IPROC_IRQ_BASE5 + 19)
#define CMICX_IRQ_CMC1_CH1_DESC_DONE            (IPROC_IRQ_BASE5 + 20)
#define CMICX_IRQ_CMC1_CH1_CHAIN_DONE           (IPROC_IRQ_BASE5 + 21)
#define CMICX_IRQ_CMC1_CH1_INTR_COALESC         (IPROC_IRQ_BASE5 + 22)
#define CMICX_IRQ_CMC1_CH1_DESC_CTRLD           (IPROC_IRQ_BASE5 + 23)
#define CMICX_IRQ_CMC1_CH2_DESC_DONE            (IPROC_IRQ_BASE5 + 24)
#define CMICX_IRQ_CMC1_CH2_CHAIN_DONE           (IPROC_IRQ_BASE5 + 25)
#define CMICX_IRQ_CMC1_CH2_INTR_COALESC         (IPROC_IRQ_BASE5 + 26)
#define CMICX_IRQ_CMC1_CH2_DESC_CTRLD           (IPROC_IRQ_BASE5 + 27)
#define CMICX_IRQ_CMC1_CH3_DESC_DONE            (IPROC_IRQ_BASE5 + 28)
#define CMICX_IRQ_CMC1_CH3_CHAIN_DONE           (IPROC_IRQ_BASE5 + 29)
#define CMICX_IRQ_CMC1_CH3_INTR_COALESC         (IPROC_IRQ_BASE5 + 30)
#define CMICX_IRQ_CMC1_CH3_DESC_CTRLD           (IPROC_IRQ_BASE5 + 31)

#define CMICX_IRQ_CMC1_CH4_DESC_DONE            (IPROC_IRQ_BASE6 + 0)
#define CMICX_IRQ_CMC1_CH4_CHAIN_DONE           (IPROC_IRQ_BASE6 + 1)
#define CMICX_IRQ_CMC1_CH4_INTR_COALESC         (IPROC_IRQ_BASE6 + 2)
#define CMICX_IRQ_CMC1_CH4_DESC_CTRLD           (IPROC_IRQ_BASE6 + 3)
#define CMICX_IRQ_CMC1_CH5_DESC_DONE            (IPROC_IRQ_BASE6 + 4)
#define CMICX_IRQ_CMC1_CH5_CHAIN_DONE           (IPROC_IRQ_BASE6 + 5)
#define CMICX_IRQ_CMC1_CH5_INTR_COALESC         (IPROC_IRQ_BASE6 + 6)
#define CMICX_IRQ_CMC1_CH5_DESC_CTRLD           (IPROC_IRQ_BASE6 + 7)
#define CMICX_IRQ_CMC1_CH6_DESC_DONE            (IPROC_IRQ_BASE6 + 8)
#define CMICX_IRQ_CMC1_CH6_CHAIN_DONE           (IPROC_IRQ_BASE6 + 9)
#define CMICX_IRQ_CMC1_CH6_INTR_COALESC         (IPROC_IRQ_BASE6 + 10)
#define CMICX_IRQ_CMC1_CH6_DESC_CTRLD           (IPROC_IRQ_BASE6 + 11)
#define CMICX_IRQ_CMC1_CH7_DESC_DONE            (IPROC_IRQ_BASE6 + 12)
#define CMICX_IRQ_CMC1_CH7_CHAIN_DONE           (IPROC_IRQ_BASE6 + 13)
#define CMICX_IRQ_CMC1_CH7_INTR_COALESC         (IPROC_IRQ_BASE6 + 14)
#define CMICX_IRQ_CMC1_CH7_DESC_CTRLD           (IPROC_IRQ_BASE6 + 15)
#define CMICX_IRQ_CMC1_XC_MEMDMA_CH0_DONE       (IPROC_IRQ_BASE6 + 16)
#define CMICX_IRQ_CMC1_XC_MEMDMA_CH1_DONE       (IPROC_IRQ_BASE6 + 17)
#define CMICX_IRQ_CMC1_SBUSDMA_CH0_DONE         (IPROC_IRQ_BASE6 + 18)
#define CMICX_IRQ_CMC1_SBUSDMA_CH1_DONE         (IPROC_IRQ_BASE6 + 19)
#define CMICX_IRQ_CMC1_SBUSDMA_CH2_DONE         (IPROC_IRQ_BASE6 + 20)
#define CMICX_IRQ_CMC1_SBUSDMA_CH3_DONE         (IPROC_IRQ_BASE6 + 21)
#define CMICX_IRQ_CMC1_DOUBLE_BIT_ECCERR        (IPROC_IRQ_BASE6 + 22)
#define CMICX_IRQ_CMC1_SINGLE_BIT_ECCERR        (IPROC_IRQ_BASE6 + 23)

#define CMICX_IRQ_FIFO_CH0_DMA_INTR             (IPROC_IRQ_BASE7 + 0)
#define CMICX_IRQ_FIFO_CH1_DMA_INTR             (IPROC_IRQ_BASE7 + 1)
#define CMICX_IRQ_FIFO_CH2_DMA_INTR             (IPROC_IRQ_BASE7 + 2)
#define CMICX_IRQ_FIFO_CH3_DMA_INTR             (IPROC_IRQ_BASE7 + 3)
#define CMICX_IRQ_FIFO_CH4_DMA_INTR             (IPROC_IRQ_BASE7 + 4)
#define CMICX_IRQ_FIFO_CH5_DMA_INTR             (IPROC_IRQ_BASE7 + 5)
#define CMICX_IRQ_FIFO_CH6_DMA_INTR             (IPROC_IRQ_BASE7 + 6)
#define CMICX_IRQ_FIFO_CH7_DMA_INTR             (IPROC_IRQ_BASE7 + 7)
#define CMICX_IRQ_FIFO_CH8_DMA_INTR             (IPROC_IRQ_BASE7 + 8)
#define CMICX_IRQ_FIFO_CH9_DMA_INTR             (IPROC_IRQ_BASE7 + 9)
#define CMICX_IRQ_FIFO_CH10_DMA_INTR            (IPROC_IRQ_BASE7 + 10)
#define CMICX_IRQ_FIFO_CH11_DMA_INTR            (IPROC_IRQ_BASE7 + 11)
#define CMICX_IRQ_SCHAN_CH0_OP_DONE             (IPROC_IRQ_BASE7 + 12)
#define CMICX_IRQ_SCHAN_CH1_OP_DONE             (IPROC_IRQ_BASE7 + 13)
#define CMICX_IRQ_SCHAN_CH2_OP_DONE             (IPROC_IRQ_BASE7 + 14)
#define CMICX_IRQ_SCHAN_CH3_OP_DONE             (IPROC_IRQ_BASE7 + 15)
#define CMICX_IRQ_SCHAN_CH4_OP_DONE             (IPROC_IRQ_BASE7 + 16)
#define CMICX_IRQ_SCHAN_FIFO_CH0_DONE           (IPROC_IRQ_BASE7 + 17)
#define CMICX_IRQ_SCHAN_FIFO_CH1_DONE           (IPROC_IRQ_BASE7 + 18)
#define CMICX_IRQ_COMMON_POOL_2BIT_ECCERR       (IPROC_IRQ_BASE7 + 19)
#define CMICX_IRQ_COMMON_POOL_1BIT_ECCERR       (IPROC_IRQ_BASE7 + 20)
#define CMICX_IRQ_RPE_1BIT_ECCERR               (IPROC_IRQ_BASE7 + 24)
#define CMICX_IRQ_RPE_2BIT_ECCERR               (IPROC_IRQ_BASE7 + 25)
#define CMICX_IRQ_RPE_SBUSDMA_PKT_DROP          (IPROC_IRQ_BASE7 + 26)
#define CMICX_IRQ_RPE_PIO_MEMDMA_PKT_DROP       (IPROC_IRQ_BASE7 + 27)

/*! \} */

/*!
 * \brief Assign per-source interrupt handler.
 *
 * Assign an interrupt handler for a specific interrupt source.
 *
 * Refer to \ref bcmbd_intr_f for a more detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number (\ref CMICX_IRQ_xxx).
 * \param [in] intr_func Per-source interrupt handler function.
 * \param [in] intr_param Per-source interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid interrupt source.
 */
extern int
bcmbd_cmicx_intr_func_set(int unit, unsigned int intr_num,
                          bcmbd_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Enable interrupt source.
 *
 * Allow a given interrupt source to generate hardware interrupts.
 *
 * A per-source interrupt handler should be installed before an
 * interrupt is enabled (\ref bcmbd_cmicx_intr_func_set).
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number (\ref CMICX_IRQ_xxx).
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicx_intr_enable(int unit, int intr_num);

/*!
 * \brief Disable interrupt source.
 *
 * Prevent a given interrupt source from generating hardware
 * interrupts.
 *
 * Must not be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number (\ref CMICX_IRQ_xxx).
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicx_intr_disable(int unit, int intr_num);

/*!
 * \brief Disable interrupt source from interrupt handler context.
 *
 * Prevent a given interrupt source from generating hardware
 * interrupts.
 *
 * Should only be called from interrupt context.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_num Interrupt source number (\ref CMICX_IRQ_xxx).
 *
 * \return Nothing.
 */
extern void
bcmbd_cmicx_intr_disable_deferred(int unit, int intr_num);

/*!
 * \brief Clear all interrupt register configurations.
 *
 * Remove all interrupt register configurations created using \ref
 * bcmbd_cmicx_intr_reg_set.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_intr_regs_clr(int unit);

/*!
 * \brief Add interrupt register configuration.
 *
 * Add an entry to the interrupt register set table.
 *
 * This function is normally not called directly. Use \ref
 * bcmbd_cmicx_intr_regs_init instead.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_base Interrupt source base for these registers.
 * \param [in] status_offs Interrupt status register offset.
 * \param [in] mask_offs Interrupt mask register offset.
 * \param [in] kmask Interrupt mask bits owned by a kernel driver.
 *
 * \retval 0 No errors
 */
extern int
bcmbd_cmicx_intr_reg_set(int unit, unsigned int intr_base,
                         uint32_t status_offs, uint32_t mask_offs,
                         uint32_t kmask);

/*!
 * \brief Configure interrupt status and mask registers.
 *
 * This function will configure all the required the interrupt status
 * and mask registers for a device using \ref
 * bcmbd_cmicx_intr_reg_set.
 *
 * The initialization will take into account which CPU (external or
 * internal) the SDK is running on, as this will affect which
 * interrupt mask registers should be used.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_intr_regs_init(int unit);

/*!
 * \brief Initialize interrupt sub-system.
 *
 * This is a convenience function that clears all interrupt mask
 * registers, initializes interrupt register controls and installs the
 * low-priority interrupt handler.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_intr_init(int unit);

/*!
 * \brief Disconnect CMICx ISR from system interrupt handler.
 *
 * This function will disconnect the CMICx ISR (interrupt service
 * routine) from the system interrupt handler via the
 * application-provided functions in the DRD library.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid DRD interface encountered.
 */
extern int
bcmbd_cmicx_intr_disconnect(int unit);

/*!
 * \brief Connect CMICx ISR to system interrupt handler.
 *
 * This function will connect the CMICx ISR (interrupt service
 * routine) to the system interrupt handler via the
 * application-provided functions in the DRD library.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid DRD interface encountered.
 */
extern int
bcmbd_cmicx_intr_connect(int unit);

/*!
 * \brief Enable or disable KNET interrupt mode.
 *
 * Low-level packet I/O (interrupts and DMA) can be handled either in
 * user space or kernel space (if applicable to the operating system).
 *
 * This function will configure interrupt handling according to the
 * selected mode of operation (KNET or non-KNET).
 *
 * \param [in] unit Unit number.
 * \param [in] enable Set to true for KNET mode.
 *
 * \retval SHR_E_NONE Operating mode successfully updated.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_cmicx_intr_knet_enable_set(int unit, bool enable);

/*!
 * \brief Get KNET interrupt mode enable status.
 *
 * Retrieve the mode that was currently selected using \ref
 * bcmbd_cmicx_intr_knet_enable_set.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Set to true if KNET mode is enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_cmicx_intr_knet_enable_get(int unit, bool *enabled);

#endif /* BCMBD_CMICX_INTR_H */
