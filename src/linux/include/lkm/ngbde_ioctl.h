/*! \file ngbde_ioctl.h
 *
 * NGBDE device I/O control definitions.
 *
 * This file is intended for use in both kernel mode and user mode.
 *
 * IMPORTANT!
 * All shared structures must be properly 64-bit aligned.
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

#ifndef NGBDE_IOCTL_H
#define NGBDE_IOCTL_H

#include <linux/types.h>
#include <linux/ioctl.h>

/*! Message-signaled interrupts, PCI interrupts are operating in MSI mode. */
#define NGBDE_DEV_F_MSI         (1 << 0)

/*! Must be updated if backward compatibility is broken. */
#define NGBDE_IOC_VERSION       1

/*! LUBDE IOCTL command magic. */
#define NGBDE_IOC_MAGIC 'L'

/*!
 * \name IOCTL commands for the NGBDE kernel module.
 * \anchor NGBDE_IOC_xxx
 *
 * Note that we use __u64 for the IOCTL parameter size because
 * sizeof(void *) is different between 32-bit and 64-bit code, and we
 * need a 32-bit user mode application to generate the same IOCTL
 * command codes as a 64-bit kernel when using the _IOW macro.
 */

/*! \{ */

/*! Get kernel module information. */
#define NGBDE_IOC_MOD_INFO      _IOW(NGBDE_IOC_MAGIC, 0, __u64)

/*! Get information about registered devices. */
#define NGBDE_IOC_PROBE_INFO    _IOW(NGBDE_IOC_MAGIC, 1, __u64)

/*! Get detailed switch device information. */
#define NGBDE_IOC_DEV_INFO      _IOW(NGBDE_IOC_MAGIC, 2, __u64)

/*! Get a physical memory address associated with a switch device. */
#define NGBDE_IOC_PHYS_ADDR     _IOW(NGBDE_IOC_MAGIC, 3, __u64)

/*! Interrupt control command (see \ref NGBDE_ICTL_xxx). */
#define NGBDE_IOC_INTR_CTRL     _IOW(NGBDE_IOC_MAGIC, 4, __u64)

/*! Add interrupt status/mask register for kernel to control. */
#define NGBDE_IOC_IRQ_REG_ADD   _IOW(NGBDE_IOC_MAGIC, 5, __u64)

/*! Write to a shared interrupt mask register. */
#define NGBDE_IOC_IRQ_MASK_WR   _IOW(NGBDE_IOC_MAGIC, 6, __u64)

/*! Map device registers in kernel space. */
#define NGBDE_IOC_PIO_WIN_MAP   _IOW(NGBDE_IOC_MAGIC, 7, __u64)

/*! Map interrupt controller registers in kernel space. */
#define NGBDE_IOC_IIO_WIN_MAP   _IOW(NGBDE_IOC_MAGIC, 8, __u64)

/*! \} */

/*! IOCTL command return code for success. */
#define NGBDE_IOC_SUCCESS       0

/*! IOCTL command return code for failure. */
#define NGBDE_IOC_FAIL          ((__u32)-1)

/*!
 * \name Interrupt control commands.
 * \anchor NGBDE_ICTL_xxx
 */

/*! \{ */

/*! Connect interrupt handler. */
#define NGBDE_ICTL_INTR_CONN    0

/*! Disconnect interrupt handler. */
#define NGBDE_ICTL_INTR_DISC    1

/*! Wait for interrupt. */
#define NGBDE_ICTL_INTR_WAIT    2

/*! Force waiting thread to return. */
#define NGBDE_ICTL_INTR_STOP    3

/*! Clear list of interrupt status/mask registers. */
#define NGBDE_ICTL_REGS_CLR     4

/*! \} */

/*! Kernel module information. */
struct ngbde_ioc_mod_info_s {

    /*! IOCTL version used by kernel module. */
    __u16 version;
};

/*! Probing results. */
struct ngbde_ioc_probe_info_s {

    /*! Number of switch devices. */
    __u16 num_swdev;
};

/*! Device information. */
struct ngbde_ioc_dev_info_s {

    /*! Device type. */
    __u16 type;

    /*! Device flags. */
    __u16 flags;

    /*! Vendor ID (typically the PCI vendor ID). */
    __u16 vendor_id;

    /*! Device ID (typically the PCI vendor ID). */
    __u16 device_id;

    /*! Device revision (typically the PCI device revision). */
    __u16 revision;

    /*! Device model (device-identification beyond PCI generic ID). */
    __u16 model;
};

/*!
 * \name I/O resource types.
 * \anchor NGBDE_IO_RSRC_xxx
 */

/*! \{ */

/*! Memory-mapped I/O. */
#define NGBDE_IO_RSRC_DEV_IO    0

/*! DMA memory pool. */
#define NGBDE_IO_RSRC_DMA_MEM   1

/*! \} */

/*!
 * \brief Resource ID (IOCTL input).
 *
 * This structure is used to query a physical address resource in the
 * kernel module. The caller must provide a resource type (device I/O,
 * DMA memory, etc.) and a resource instance number (e.g. a PCI BAR
 * address will have multiple instances).
 *
 * Also see \ref ngbde_ioc_phys_addr_s.
 */
struct ngbde_ioc_rsrc_id_s {

    /*! Resource type (\ref NGBDE_IO_RSRC_xxx). */
    __u32 type;

    /*! Resource instance number. */
    __u32 inst;
};

/*!
 * \brief Physical device address.
 *
 * This structure is returned in response to the \ref
 * NGBDE_IOC_PHYS_ADDR command. The caller must identify the requested
 * physical address using the \ref ngbde_ioc_rsrc_id_s structure.
 */
struct ngbde_ioc_phys_addr_s {

    /*! Physical address. */
    __u64 addr;

    /*! Resource size (in bytes). */
    __u32 size;
};

/*! Interrupt control operation */
struct ngbde_ioc_intr_ctrl_s {

    /*! Interrupt instance for this device. */
    __u32 irq_num;

    /*! Interrupt control command. */
    __u32 cmd;
};

/*! Add interrupt register information. */
struct ngbde_ioc_irq_reg_add_s {

    /*! Interrupt instance for this device. */
    __u32 irq_num;

    /*! Interrupt status register address offset. */
    __u32 status_reg;

    /*! Interrupt mask register address offset. */
    __u32 mask_reg;

    /*! Interrupt mask for interrupts handled by the kernel. */
    __u32 kmask;

    /*! Reserved for future use. */
    __u32 flags;
};

/*! Memory-mapped I/O window */
struct ngbde_ioc_pio_win_s {

    /*! Physical address */
    __u32 addr;

    /*! Resource size */
    __u32 size;
};

/*! Interrupt mask register write */
struct ngbde_ioc_irq_mask_wr_s {

    /*! Interrupt instance for this device. */
    __u32 irq_num;

    /*! Register offset. */
    __u32 offs;

    /*! Value to write. */
    __u32 val;
};

/*! IOCTL operation data. */
union ngbde_ioc_op_s {

    /*! Get kernel module information. */
    struct ngbde_ioc_mod_info_s mod_info;

    /*! Get information about registered devices. */
    struct ngbde_ioc_probe_info_s probe_info;

    /*! Get detailed switch device information. */
    struct ngbde_ioc_dev_info_s dev_info;

    /*! Resource ID (input). */
    struct ngbde_ioc_rsrc_id_s rsrc_id;

    /*! Get a physical memory address associated with a switch device. */
    struct ngbde_ioc_phys_addr_s phys_addr;

    /*! Interrupt control command (see \ref NGBDE_ICTL_xxx). */
    struct ngbde_ioc_intr_ctrl_s intr_ctrl;

    /*! Add interrupt status/mask register for kernel to control. */
    struct ngbde_ioc_irq_reg_add_s irq_reg_add;

    /*! Write to a shared interrupt mask register. */
    struct ngbde_ioc_irq_mask_wr_s irq_mask_wr;

    /*! Map device registers in kernel space. */
    struct ngbde_ioc_pio_win_s pio_win;
};

/*! IOCTL command message. */
struct ngbde_ioc_cmd_s {

    /*! Device handle. */
    __u32 devid;

    /*! Return code (0 means success). */
    __u32 rc;

    /*! IOCTL operation. */
    union ngbde_ioc_op_s op;
};

#endif /* NGBDE_IOCTL_H */
