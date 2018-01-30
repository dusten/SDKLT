/*! \file bcma_sys_probe.c
 *
 * User mode interfaces for Linux BDE kernel module.
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_thread.h>

#include <bsl/bsl.h>

#include <shr/shr_mpool.h>
#include <shr/shr_timeout.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlu/bcmlu_ngbde.h>
#include <bcmlu/bcmpkt/bcmlu_knet.h>
#include <bcmlu/bcmpkt/bcmlu_unet.h>

#include <bcma/sys/bcma_sys_probe.h>

/*
 * The path to the driver device files
 */
#define DEV_FNAME       "/dev/mem"
#define DEV_FMODE       (O_RDWR | O_SYNC)

/*!
 * \brief Tracked information about probed devices.
 *
 * Note that we only support a single IRQ line per device. If this
 * needs to be exapnded, then the thread and isr data must be tracked
 * per IRQ line.
 */
typedef struct sys_dev_s {

    /*! Device number in kernel BDE. */
    int kdev;

    /*! IRQ line services by ISR thread. */
    int intr_thread_irq_num;

    /*! Run state for ISR thread. */
    bool intr_thread_running;

    /*! Stop signal for ISR thread. */
    bool intr_thread_stop;

    /*! ISR thread ID. */
    sal_thread_t intr_thread_id;

    /*! ISR thread lock. */
    sal_mutex_t intr_lock;

    /*! Interrupt handler provided by the SDK. */
    bcmdrd_hal_isr_func_f isr_func;

    /*! Context for the interrupt handler. */
    void *isr_data;

    /*! Physical address of DMA memory pool. */
    uint64_t dma_pbase;

    /*! Size (in bytes) of DMA memory pool. */
    size_t dma_psize;

    /*! Logical (mapped) address of DMA memory pool. */
    void *dma_lbase;

    /*! Physical address of HA DMA memory pool. */
    uint64_t ha_dma_pbase;

    /*! Size (in bytes) of HA DMA memory pool. */
    size_t ha_dma_psize;

    /*! Logical (mapped) address of HA DMA memory pool. */
    void *ha_dma_lbase;

    /*! DMA memory manager (for alloc/free). */
    shr_mpool_handle_t mpool;

    /*! Override device ID. */
    bcmdrd_dev_id_t override_id;

} sys_dev_t;

/*! Actual devices in our system. */
static sys_dev_t sys_devs[BCMDRD_CONFIG_MAX_UNITS];

/*! Device file descriptor for IOCTL. */
static int devfd = -1;

/*!
 * \brief Open device file.
 *
 * Open device file for use by mmap.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
static int
dev_open(void)
{
    if (devfd >= 0) {
        /* Already open */
        return 0;
    }

    /* Open the device driver */
    if ((devfd = open(DEV_FNAME, DEV_FMODE)) < 0) {
        perror("open " DEV_FNAME ": ");
        return -1;
    }

    return 0;
}

/*!
 * \brief Map I/O memory into host memory space.
 *
 * <long-description>
 *
 * \param [in] offset Physical address start.
 * \param [in] size Size of region to map into host CPU memory space.
 *
 * \return Pointer to mapped region, or NULL on failure.
 */
void *
io_remap(uint64_t offset, size_t size)
{
    dev_open();

    return bcmlu_mmap2(devfd, offset >> 12, size);
}

/*!
 * \brief Unmap I/O memory from host memory space.
 *
 * \param [in] iomem Mapped host CPU memory space.
 * \param [in] size Size of region to unmap.
 *
 * \return Nothing.
 */
void
io_unmap(void *iomem, size_t size)
{
    munmap(iomem, size);
}

/*!
 * \brief Convert logical DMA address to physical DMA address.
 *
 * \param [in] sd Device handle.
 * \param [in] addr Logical DMA address.
 *
 * \return Physical DMA address.
 */
static uint64_t
dma_l2p(sys_dev_t *sd, void *addr)
{
    uint64_t pbase = sd->dma_pbase;
    unsigned long lbase = (unsigned long)sd->dma_lbase;
    unsigned long laddr = (unsigned long)addr;

    if (addr == NULL) {
        return 0;
    }

    /* DMA memory is one contiguous block */
    return pbase + (laddr - lbase);
}

/*!
 * \brief Interrupt handler thread.
 *
 * This thread is a high-priority thread which runs the user mode
 * interrupt handler.
 *
 * The thread will be sleeping in the kernel BDE until a hardware
 * interrupt occurs, upon which the thread is woken up by the real
 * kernel mode ISR.
 *
 * \param [in] data Interrupt handler context (\ref sys_dev_t).
 *
 * \return Nothing.
 */
static void
intr_thread(void *data)
{
    sys_dev_t *sd = (sys_dev_t *)data;

    sd->intr_thread_running = true;

    while (1) {
        bcmlu_ngbde_intr_wait(sd->kdev, sd->intr_thread_irq_num);
        if (sd->intr_thread_stop) {
            break;
        }
        if (sd->isr_func) {
            /* Run ISR with sync lock held */
            sal_mutex_take(sd->intr_lock, SAL_MUTEX_FOREVER);
            sd->isr_func(sd->isr_data);
            sal_mutex_give(sd->intr_lock);
        }
    }

    sd->intr_thread_running = false;
}

/*!
 * \brief Install user-mode interrupt handler.
 *
 * The user mode interrupt handler will be executed by a high-priority
 * thread, which is kicked off by the real hardware interrupt.
 *
 * See also \ref intr_thread.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] isr_func User mode interrupt handler.
 * \param [in] isr_data Interrupt handler context.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
static int
intr_connect(void *devh, int irq_num,
             bcmdrd_hal_isr_func_f isr_func, void *isr_data)
{
    sys_dev_t *sd = (sys_dev_t *)devh;

    if (bcmlu_ngbde_intr_connect(sd->kdev, irq_num) < 0) {
        return -1;
    }

    sd->isr_func = isr_func;
    sd->isr_data = isr_data;

    sd->intr_thread_stop = false;
    sd->intr_thread_irq_num = irq_num;
    sd->intr_thread_id = sal_thread_create("intr_thread",
                                           4096, SAL_THREAD_PRIO_NO_PREEMPT,
                                           intr_thread, devh);
    if (sd->intr_thread_id == SAL_THREAD_ERROR) {
        sd->intr_thread_stop = true;
        bcmlu_ngbde_intr_disconnect(sd->kdev, irq_num);
        return -1;
    }

    return 0;
}

/*!
 * \brief Uninstall user-mode interrupt handler.
 *
 * Remove interrupt handler previously installed using \ref
 * intr_connect.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
static int
intr_disconnect(void *devh, int irq_num)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = 100000;
    int min_polls = 1;

    sd->intr_thread_stop = true;

    if (bcmlu_ngbde_intr_stop(sd->kdev, irq_num) < 0) {
        return -1;
    }

    shr_timeout_init(&to, timeout_usec, min_polls);
    while (1) {
        if (!sd->intr_thread_running) {
            break;
        }
        if (shr_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META("Failed to stop interrupt thread\n")));
            return -1;
        }
    }

    if (bcmlu_ngbde_intr_disconnect(sd->kdev, irq_num) < 0) {
        return -1;
    }

    /* Clear associated resource variables */
    sd->intr_thread_stop = false;
    sd->intr_thread_irq_num = 0;
    sd->isr_func = NULL;
    sd->isr_data = NULL;

    return 0;
}

/*!
 * \brief Serialize function with interrupt handler.
 *
 * Take interrupt lock and adjust scheduling priority to avoid
 * priority inversion.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] cb Function to be serialized with interrupt context.
 * \param [in] data Pass-through data for \c cb function.
 *
 * \return Return value of callback function.
 */
static int
intr_sync(void *devh, int irq_num,
          bcmdrd_intr_sync_cb_f cb, void *data)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    pthread_t thread;
    struct sched_param param;
    int policy, prio;
    bool sched_restore = false;
    int rv;

    if (!cb) {
        /* Silently ignore */
        return 0;
    }

    /* Calling this function from the interrupt thread will cause a deadlock */
    if (sal_thread_self() == sd->intr_thread_id) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("intr_sync called from interrupt thread.\n")));
        return 0;
    }

    thread = pthread_self();
    if (thread && pthread_getschedparam(thread, &policy, &param) == 0) {
        prio = param.sched_priority;
        if (policy != SCHED_FIFO || prio != SAL_THREAD_RT_PRIO_HIGHEST) {
            /* Set same scheduling policy as interrupt thread */
            param.sched_priority = SAL_THREAD_RT_PRIO_HIGHEST;
            pthread_setschedparam(thread, SCHED_FIFO, &param);
            sched_restore = true;
        }
    }

    sal_mutex_take(sd->intr_lock, SAL_MUTEX_FOREVER);

    /* Perform callback */
    rv = cb(data);

    sal_mutex_give(sd->intr_lock);

    if (sched_restore) {
        /* Restore scheduling policy if it was modified */
        param.sched_priority = prio;
        pthread_setschedparam(thread, policy, &param);
    }

    return rv;
}

/*!
 * \brief Configure the interrupt handler thread.
 *
 * The information provided allows the kernel mode ISR to access and
 * interpret the IRQ registers associated with this IRQ line.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] intr_info Interrupt configuration information.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
static int
intr_configure(void *devh, int irq_num, bcmdrd_hal_intr_info_t *intr_info)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    bcmdrd_hal_irq_reg_t *ir;
    int rv, idx;

    if (intr_info == NULL) {
        return -1;
    }

    if (intr_info->piowin_size > 0) {
        rv = bcmlu_ngbde_pio_map(sd->kdev, intr_info->piowin_addr,
                                 intr_info->piowin_size);
        if (rv < 0) {
            return -1;
        }
    }

    if (intr_info->iiowin_size > 0) {
        rv = bcmlu_ngbde_iio_map(sd->kdev, intr_info->iiowin_addr,
                                 intr_info->iiowin_size);
        if (rv < 0) {
            return -1;
        }
    }

    if (bcmlu_ngbde_intr_regs_clr(sd->kdev, irq_num) < 0) {
        return -1;
    }

    for (idx = 0; idx < intr_info->num_irq_regs; idx++) {
        ir = &intr_info->irq_regs[idx];
        rv = bcmlu_ngbde_intr_reg_add(sd->kdev, irq_num,
                                      ir->status_reg, ir->mask_reg, ir->kmask);
        if (rv < 0) {
            return -1;
        }
    }

    return 0;
}

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by the user-mode interrupt handler when a
 * shared interrupt mask register needs to be updated.
 *
 * Since the register is shared with another (kernel mode) interrupt
 * handler, access must be protected by a lock.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] offs Interrupt mask register offset.
 * \param [in] val New value to write to register.
 *
 * \retval 0 No errors
 * \retval <0 Something went wrong.
 */
static int
intr_mask_write(void *devh, int irq_num, uint32_t offs, uint32_t val)
{
    sys_dev_t *sd = (sys_dev_t *)devh;

    if (bcmlu_ngbde_intr_mask_write(sd->kdev, irq_num, offs, val) < 0) {
        return -1;
    }

    return 0;
}

/*!
 * \brief Allocate a block of DMA memory.
 *
 * This function is the system call-back function for dma_alloc.
 *
 * \param [in] devh Device context.
 * \param [in] size Size of DMA memory to allocate.
 * \param [out] dma_addr Physical address of DMA memory.
 *
 * \return Logical address of DMA mmoery, or NULL if error.
 */
static void *
dma_alloc(void *devh, size_t size, uint64_t *dma_addr)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    void *dma_mem;

    if (sd == NULL) {
        return NULL;
    }

    dma_mem = shr_mpool_alloc(sd->mpool, size);
    if (dma_addr) {
        *dma_addr = dma_l2p(sd, dma_mem);
    }

    return dma_mem;
}

/*!
 * \brief Free block of DMA memory.
 *
 * This function is the system call-back function for dma_alloc.
 *
 * \param [in] devh Device context.
 * \param [in] size Size of DMA memory to free.
 * \param [in] dma_mem Logical address of DMA memory.
 * \param [in] dma_addr Physical address of DMA memory.
 */
static void
dma_free(void *devh, size_t size, void *dma_mem, uint64_t dma_addr)
{
    sys_dev_t *sd = (sys_dev_t *)devh;

    if (sd) {
        shr_mpool_free(sd->mpool, dma_mem);
    }
}

/*!
 * \brief Map DMA memory to device.
 *
 * This function simply returns the physical bus address for the DMA
 * memory block.
 *
 * \param [in] devh Device context.
 * \param [in] dma_mem CPU address of memory block to map.
 * \param [in] size Size of memory block to map.
 * \param [in] to_dev True if transferring from host memory to device.
 *
 * \return Physical bus address or 0 if error.
 */
static uint64_t
dma_map(void *devh, void *dma_mem, size_t size, bool to_dev)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    uint64_t pbase;

    /* All DMA memory is uncached */
    pbase = dma_l2p(sd, dma_mem);

    /* Sanity check */
    if (pbase < sd->dma_pbase ||
        (pbase + size) >= (sd->dma_pbase + sd->dma_psize)) {
        return 0;
    }

    return pbase;
}

/*!
 * \brief Unmap DMA memory for device.
 *
 * Empty function since all DMA memory is uncached.
 *
 * \param [in] devh Device context.
 * \param [in] dma_addr Physical bus address of mapped memory.
 * \param [in] size Size of memory block to unmap.
 * \param [in] to_dev True if transferring from host memory to device.
 */
static void
dma_unmap(void *devh, uint64_t dma_addr, size_t size, bool to_dev)
{
    /* All DMA memory is uncached */
}

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_probe_cleanup(void)
{
    int unit;
    sys_dev_t *sd;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        sd = &sys_devs[unit];
        if (bcmdrd_dev_exists(unit)) {
            bcmdrd_dev_destroy(unit);
        }
        if (sd->intr_lock) {
            sal_mutex_destroy(sd->intr_lock);
            sd->intr_lock = NULL;
        }
        if (sd->mpool) {
            shr_mpool_destroy(sd->mpool);
            sd->mpool = NULL;
        }
        if (sd->dma_lbase) {
            bcmlu_ngbde_munmap(sd->dma_lbase, sd->dma_psize);
            sd->dma_lbase = NULL;
        }
        if (sd->ha_dma_lbase) {
            bcmlu_ngbde_munmap(sd->ha_dma_lbase, sd->ha_dma_psize);
            sd->ha_dma_lbase = NULL;
        }
    }
    if (devfd >= 0) {
        close(devfd);
        devfd = -1;
    }
    return 0;
}

int
bcma_sys_probe(void)
{
    int ndevs = 0;
    int unit;
    int bdx;
    sys_dev_t *sd;
    bcmlu_dev_info_t dev_info, *di = &dev_info;
    bcmlu_dma_info_t dma_info, *dmi = &dma_info;
    bcmdrd_dev_id_t id;
    bcmdrd_hal_io_t io;
    bcmdrd_hal_intr_t intr;
    bcmdrd_hal_dma_t dma;
    int bde_devs;

    /* Open BDE device */
    if (bcmlu_ngbde_init() != 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to open BDE device.\n")));
        return -1;
    }

    /* Attach KNET driver*/
    if (bcmlu_knet_attach() < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to attach KNET driver.\n")));
    } else if (bcmlu_unet_attach() < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to attach UNET driver.\n")));
    }

    /* Get all available devices */
    if (bcmlu_ngbde_num_dev_get(&bde_devs) < 0) {
        return -1;
    }
    if (bde_devs == 0) {
        LOG_WARN(BSL_LS_SYS_PCI,
                 (BSL_META("No devices found.\n")));
        return -1;
    }

    /* For each device in the system */
    for (bdx = 0; bdx < bde_devs; bdx++) {

        /* Next unit number */
        unit = ndevs;

        /* Convenience pointer */
        sd = &sys_devs[unit];

        /* Kernel device handle */
        sd->kdev = bdx;

        /* Initialize ID structure */
        memset(&id, 0, sizeof(id));

        /* Get device details */
        if (bcmlu_ngbde_dev_info_get(bdx, di) < 0) {
            return -1;
        }

        if (sd->intr_lock == NULL) {
            sd->intr_lock = sal_mutex_create("bcmaSysIntr");
        }
        if (sd->intr_lock == NULL) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META("Unable to create interrupt lock.\n")));
            return -1;
        }

        /* Add device info to the ID structure */
        id.vendor_id = di->vendor_id;
        id.device_id = di->device_id;
        id.revision = di->revision;

        /* Check for override ID */
        if (sd->override_id.vendor_id) {
            sal_memcpy(&id, &sd->override_id, sizeof(bcmdrd_dev_id_t));
        }

        /* Create device */
        if (bcmdrd_dev_create(unit, &id) < 0) {
            LOG_WARN(BSL_LS_SYS_PCI,
                     (BSL_META_U(unit,
                                 "Failed to create device "
                                 "(%04x:%04x:%02x:%08x:%08x)\n"),
                      di->vendor_id, di->device_id, di->revision,
                      (unsigned int)di->iowin[0].addr,
                      (unsigned int)di->iowin[1].addr));
            continue;
        }

        /* Initialize I/O structure */
        memset(&io, 0, sizeof(io));

        if (di->flags & BCMLU_DEV_F_MSI) {
            io.flags |= BCMDRD_HAL_IO_F_MSI;
        }

        
        io.bus_type = BCMDRD_BT_PCI;

        /* Configure host bus byte swapping. */
        io.byte_swap_pio = SYS_BE_PIO;
        io.byte_swap_packet_dma = SYS_BE_PACKET;
        io.byte_swap_non_packet_dma = SYS_BE_OTHER;

        /*! Physical address(es) for memory-mapped I/O. */
        io.phys_addr[0] = di->iowin[0].addr;
        io.phys_addr[1] = di->iowin[1].addr;

        /* Callback context is local system device */
        io.devh = sd;

        /*! Map I/O memory into CPU space. */
        io.ioremap = io_remap;
        io.iounmap = io_unmap;

        if (bcmdrd_dev_hal_io_init(unit, &io) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META_U(unit,
                                  "Failed to initialize I/O\n")));
            continue;
        }

        /* Get DMA resources */
        if (bcmlu_ngbde_dma_info_get(bdx, dmi) == 0) {

            /* Save physical properties of DMA memory */
            sd->dma_pbase = dmi->addr;
            sd->dma_psize = dmi->size;

            /* Get required HA DMA memory size */
            (void)bcmdrd_dev_ha_dma_mem_get(unit, &sd->ha_dma_psize, NULL);
            if (sd->ha_dma_psize && dmi->size > sd->ha_dma_psize) {
                /* Let HA DMA occupy lower part of kernel memory block */
                sd->ha_dma_pbase = dmi->addr;
                sd->ha_dma_psize = sd->ha_dma_psize;
                /* Let normal DMA occupy upper part of kernel memory block */
                sd->dma_pbase += sd->ha_dma_psize;
                sd->dma_psize -= sd->ha_dma_psize;
            }

            /* Map DMA memory from kernel space */
            sd->dma_lbase = bcmlu_ngbde_mmap(sd->dma_pbase, sd->dma_psize);
            if (sd->dma_lbase == NULL) {
                LOG_WARN(BSL_LS_SYS_PCI,
                         (BSL_META_U(unit,
                                     "Unable to map DMA memory.\n")));
            }

            if (sd->dma_lbase) {
                /* Create memory pool for use by DMA memory allocator */
                sd->mpool = shr_mpool_create(sd->dma_lbase, sd->dma_psize, 0);
                if (sd->mpool == NULL) {
                    LOG_WARN(BSL_LS_SYS_PCI,
                             (BSL_META_U(unit,
                                         "Unable to create DMA mpool.\n")));
                }
            }

            if (sd->mpool) {
                /* Initialize DMA structure */
                memset(&dma, 0, sizeof(dma));

                /* Callback context is local system device */
                dma.devh = sd;

                /*! Configure function vectors */
                dma.dma_alloc = dma_alloc;
                dma.dma_free = dma_free;
                dma.dma_map = dma_map;
                dma.dma_unmap = dma_unmap;

                if (bcmdrd_dev_hal_dma_init(unit, &dma) < 0) {
                    LOG_ERROR(BSL_LS_SYS_PCI,
                              (BSL_META_U(unit,
                                          "Failed to initialize DMA\n")));
                    continue;
                }

            }

            /* Map HA DMA memory from kernel space */
            if (sd->ha_dma_psize) {
                sd->ha_dma_lbase = bcmlu_ngbde_mmap(sd->ha_dma_pbase,
                                                    sd->ha_dma_psize);
                if (sd->ha_dma_lbase == NULL) {
                    LOG_WARN(BSL_LS_SYS_PCI,
                             (BSL_META_U(unit,
                                         "Unable to map HA DMA memory.\n")));
                }
            }

            if (sd->ha_dma_lbase) {
                if (bcmdrd_dev_ha_dma_mem_set(unit, sd->ha_dma_psize,
                                              sd->ha_dma_lbase,
                                              sd->ha_dma_pbase) < 0) {
                    LOG_WARN(BSL_LS_SYS_PCI,
                             (BSL_META_U(unit,
                                         "Failed to initialize HA DMA\n")));
                }
            }
        }

        /* Initialize interrupt structure */
        memset(&intr, 0, sizeof(intr));

        /* Callback context is local system device */
        intr.devh = sd;

        /*! Connect/disconnect interrupt handler. */
        intr.intr_connect = intr_connect;
        intr.intr_disconnect = intr_disconnect;
        intr.intr_sync = intr_sync;
        intr.intr_configure = intr_configure;
        intr.intr_mask_write = intr_mask_write;

        if (bcmdrd_dev_hal_intr_init(unit, &intr) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META_U(unit,
                                  "Failed to initialize interrupt API\n")));
            continue;
        }

        if (++ndevs >= COUNTOF(sys_devs)) {
            break;
        }
    }

    return ndevs;
}

int
bcma_sys_probe_override(int unit, const char *dev_name)
{
    bcmdrd_dev_id_t id;
    int idx;

    if (SHR_FAILURE(bcmdrd_dev_id_from_name(dev_name, &id))) {
        return -1;
    }

    for (idx = 0; idx < BCMDRD_CONFIG_MAX_UNITS; idx++) {
        if (unit >= 0 && unit != idx) {
            continue;
        }
        sal_memcpy(&sys_devs[idx].override_id, &id, sizeof(bcmdrd_dev_id_t));
    }
    return 0;
}
