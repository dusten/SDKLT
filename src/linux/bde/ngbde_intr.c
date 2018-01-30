/*! \file ngbde_intr.c
 *
 * API for controlling a thread-based user-mode interrupt handler.
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

#include <ngbde.h>

/*! \cond */
static int intr_debug = 0;
module_param(intr_debug, int, 0);
MODULE_PARM_DESC(intr_debug,
"Interrupt debug output enable (default 0).");
/*! \endcond */

static int
ngbde_intr_shared_write32(struct ngbde_dev_s *sd, struct ngbde_intr_ctrl_s *ic,
                          uint32_t reg_offs, uint32_t reg_val, uint32_t shr_mask)
{
    unsigned long flags;
    struct ngbde_shr_reg_s *sr;
    int idx;

    sr = NULL;
    for (idx = 0; idx < NGBDE_NUM_INTR_SHR_REGS_MAX; idx++) {
        if (sd->intr_shr_reg[idx].reg_offs == 0) {
            /* If not found, then we add a new entry */
            sd->intr_shr_reg[idx].reg_offs = reg_offs;
        }
        if (sd->intr_shr_reg[idx].reg_offs == reg_offs) {
            sr = &sd->intr_shr_reg[idx];
            break;
        }
    }

    if (sr == NULL) {
        return -1;
    }

    spin_lock_irqsave(&sd->lock, flags);

    sr->cur_val &= ~shr_mask;
    sr->cur_val |= (reg_val & shr_mask);

    NGBDE_IOWRITE32(sr->cur_val, ic->iomem + reg_offs);

    spin_unlock_irqrestore(&sd->lock, flags);

    return 0;
}

/*!
 * \brief Interrupt handler for user mode thread.
 *
 * This function will determine whether a user-mode interrupt has
 * occurred by reading the configured interrupt status and mask
 * registers.
 *
 * If an interrupt has occurred, any waiting user-mode thread is woken
 * up.
 *
 * \param [in] ic Interrupt control information.
 *
 * \retval 1 One or more user mode interrupts occurred.
 * \retval 0 No user mode interrupts occurred.
 */
static int
ngbde_user_isr(ngbde_intr_ctrl_t *ic)
{
    int idx;
    int active_interrupts = 0;
    uint32_t stat = 0, mask = 0;
    uint32_t kmask;

    /* Check if any enabled interrupts are active */
    for (idx = 0; idx < ic->num_regs; idx++) {
        ngbde_irq_reg_t *ir = &ic->regs[idx];

        /* Get mask of all kernel interrupt sources for this register address */
        kmask = ir->kmask;

        stat = NGBDE_IOREAD32(&ic->iomem[ir->status_reg]);
        mask = NGBDE_IOREAD32(&ic->iomem[ir->mask_reg]);

        if (stat & mask & ~kmask) {
            active_interrupts = 1;
            break;
        }
    }

    /* No active interrupts to service */
    if (!active_interrupts) {
        return 0;
    }

    /* Disable (mask off) all interrupts */
    for (idx = 0; idx < ic->num_regs; idx++) {
        ngbde_irq_reg_t *ir = &ic->regs[idx];

        /* Get mask of all kernel interrupt sources for this register address */
        kmask = ir->kmask;

        if (kmask == 0xffffffff) {
            /* Kernel driver owns all interrupts in this register */
            continue;
        } else if (kmask) {
            /* Synchronized write */
            struct ngbde_dev_s *sd = ngbde_swdev_get(ic->kdev);
            if (ngbde_intr_shared_write32(sd, ic, ir->mask_reg, 0, ~kmask) < 0) {
                printk("%s: Failed to write shared register for device %d\n",
                       MOD_NAME, ic->kdev);
                /* Fall back to normal write to ensure interrupts are masked */
                NGBDE_IOWRITE32(0, &ic->iomem[ir->mask_reg]);
            }
        } else {
            NGBDE_IOWRITE32(0, &ic->iomem[ir->mask_reg]);
        }
    }

    atomic_set(&ic->run_user_thread, 1);
    wake_up_interruptible(&ic->user_thread_wq);

    return 1;
}

/*!
 * \brief Interrupt handler for kernel driver.
 *
 * Typically used by the KNET driver.
 *
 * \param [in] ic Interrupt control information.
 *
 * \retval 1 One or more kernel mode interrupts occurred.
 * \retval 0 No kernel mode interrupts occurred.
 */
static int
ngbde_kernel_isr(ngbde_intr_ctrl_t *ic)
{
    if (ic->isr_func) {
        return ic->isr_func(ic->isr_data);
    }
    return 0;
}

/*!
 * \brief Linux ISR
 *
 * Will call the user-mode interrupts handler and optionally also a
 * kernel mode interrupt handler (typically KNET).
 *
 * \param [in] irq_num Interrupt vector from kernel.
 * \param [in] data Interrupt control information
 *
 * \retval IRQ_NONE Interrupt not recognized.
 * \retval IRQ_HANDLED Interrupt recognized and handled (masked off).
 */
static irqreturn_t
ngbde_isr(int irq_num, void *data)
{
    struct ngbde_intr_ctrl_s *ic = (struct ngbde_intr_ctrl_s *)data;
    irqreturn_t rv = IRQ_NONE;

    if (ngbde_user_isr(ic)) {
        rv = IRQ_HANDLED;
    }
    if (ngbde_kernel_isr(ic)) {
        rv = IRQ_HANDLED;
    }
    return rv;
}

int
ngbde_intr_connect(int kdev, unsigned int irq_num)
{
    struct ngbde_dev_s *sd;
    struct ngbde_intr_ctrl_s *ic;
    unsigned long irq_flags;

    sd = ngbde_swdev_get(kdev);
    if (!sd) {
        return -1;
    }

    if (irq_num >= NGBDE_NUM_IRQS_MAX) {
        return -1;
    }

    ic = &sd->intr_ctrl[irq_num];

    if (ic->irq_active) {
        return 0;
    }

    if (sd->irq_line >= 0) {
        if (sd->pio_mem == NULL) {
            printk("%s: No memory-mapped I/O for device %d\n",
                   MOD_NAME, kdev);
            return -1;
        }
        ic->kdev = kdev;
        ic->iomem = sd->pio_mem;
        if (sd->iio_mem) {
            if (intr_debug) {
                printk("INTR: Using dedicated interrupt controller\n");
            }
            ic->iomem = sd->iio_mem;
        }
        init_waitqueue_head(&ic->user_thread_wq);
        atomic_set(&ic->run_user_thread, 0);
        irq_flags = IRQF_SHARED;
        ic->irq_vect = sd->irq_line;

#if defined(CONFIG_PCI_MSI)
        /*
        * The pci_enable_msi function must be called after enabling
        * BAR0_PAXB_OARR_FUNC0_MSI_PAGE, otherwise, MSI interrupts
        * cannot be triggered!
        */
        if (sd->use_msi && pci_enable_msi(sd->pci_dev) == 0) {
            irq_flags = 0;
            ic->irq_vect = sd->pci_dev->irq;
        }
#endif
        if (request_irq(ic->irq_vect, ngbde_isr, irq_flags, MOD_NAME, ic) < 0) {
            printk("%s: Could not get IRQ %d for device %d\n",
                   MOD_NAME, ic->irq_vect, kdev);
            return -1;
        }
        ic->irq_active = 1;
    }

    return 0;
}

int
ngbde_intr_disconnect(int kdev, unsigned int irq_num)
{
    struct ngbde_dev_s *sd;
    struct ngbde_intr_ctrl_s *ic;

    sd = ngbde_swdev_get(kdev);
    if (!sd) {
        return -1;
    }

    if (irq_num >= NGBDE_NUM_IRQS_MAX) {
        return -1;
    }

    ic = &sd->intr_ctrl[irq_num];

    if (!ic->irq_active) {
        return 0;
    }

    if (ic->irq_vect >= 0) {
        free_irq(ic->irq_vect, ic);
        if (sd->use_msi) {
            pci_disable_msi(sd->pci_dev);
        }
        ic->irq_active = 0;
    }

    return 0;
}

void
ngbde_intr_cleanup(void)
{
    struct ngbde_dev_s *swdev;
    unsigned int num_swdev, idx, irq_num;

    ngbde_swdev_get_all(&swdev, &num_swdev);

    for (idx = 0; idx < num_swdev; idx++) {
        for (irq_num = 0; irq_num < NGBDE_NUM_IRQS_MAX; irq_num++) {
            ngbde_intr_disconnect(idx, irq_num);
        }
    }
}

int
ngbde_intr_wait(int kdev, unsigned int irq_num)
{
    struct ngbde_dev_s *sd;
    struct ngbde_intr_ctrl_s *ic;

    sd = ngbde_swdev_get(kdev);
    if (!sd) {
        return -1;
    }

    if (irq_num >= NGBDE_NUM_IRQS_MAX) {
        return -1;
    }

    ic = &sd->intr_ctrl[irq_num];

    if (!ic->irq_active) {
        return 0;
    }

    wait_event_interruptible(ic->user_thread_wq,
                             atomic_read(&ic->run_user_thread) != 0);
    atomic_set(&ic->run_user_thread, 0);

    return 0;
}

int
ngbde_intr_stop(int kdev, unsigned int irq_num)
{
    struct ngbde_dev_s *sd;
    struct ngbde_intr_ctrl_s *ic;

    sd = ngbde_swdev_get(kdev);
    if (!sd) {
        return -1;
    }

    if (irq_num >= NGBDE_NUM_IRQS_MAX) {
        return -1;
    }

    ic = &sd->intr_ctrl[irq_num];

    if (!ic->irq_active) {
        return 0;
    }

    /* Wake up user thread */
    atomic_set(&ic->run_user_thread, 1);
    wake_up_interruptible(&ic->user_thread_wq);

    return 0;
}

int
ngbde_intr_regs_clr(int kdev, unsigned int irq_num)
{
    struct ngbde_dev_s *sd;
    struct ngbde_intr_ctrl_s *ic;

    sd = ngbde_swdev_get(kdev);
    if (!sd) {
        return -1;
    }

    if (irq_num >= NGBDE_NUM_IRQS_MAX) {
        return -1;
    }

    ic = &sd->intr_ctrl[irq_num];

    ic->num_regs = 0;
    memset(ic->regs, 0, sizeof(ic->regs));

    return 0;
}

int
ngbde_intr_reg_add(int kdev, unsigned int irq_num,
                   struct ngbde_irq_reg_s *ireg)
{
    struct ngbde_dev_s *sd;
    struct ngbde_intr_ctrl_s *ic;
    struct ngbde_irq_reg_s *ir;

    sd = ngbde_swdev_get(kdev);
    if (!sd) {
        return -1;
    }

    if (irq_num >= NGBDE_NUM_IRQS_MAX) {
        return -1;
    }

    ic = &sd->intr_ctrl[irq_num];

    if (ic->num_regs >= NGBDE_NUM_IRQ_REGS_MAX) {
        return -1;
    }

    ir = &ic->regs[ic->num_regs++];

    memcpy(ir, ireg, sizeof (*ir));

    if (intr_debug) {
        printk("INTR: Adding interrupt register 0x%08x/0x%08x (0x%08x)\n",
               ir->status_reg, ir->mask_reg, ir->kmask);
    }

    return ic->num_regs;
}

int
ngbde_intr_mask_write(int kdev, unsigned int irq_num, int kapi,
                      uint32_t status_reg, uint32_t mask_val)
{
    struct ngbde_dev_s *sd;
    struct ngbde_intr_ctrl_s *ic;
    struct ngbde_irq_reg_s *ir;
    unsigned int idx;
    uint32_t bmask;

    sd = ngbde_swdev_get(kdev);
    if (!sd) {
        return -1;
    }

    if (irq_num >= NGBDE_NUM_IRQS_MAX) {
        return -1;
    }

    ic = &sd->intr_ctrl[irq_num];

    ir = ic->regs;
    for (idx = 0; idx < ic->num_regs; idx++) {
        if (ir->status_reg == status_reg) {
            bmask = kapi ? ir->kmask : ~ir->kmask;
            ngbde_intr_shared_write32(sd, ic, ir->mask_reg, mask_val, bmask);
            return 0;
        }
        ir++;
    }

    return -1;
}
