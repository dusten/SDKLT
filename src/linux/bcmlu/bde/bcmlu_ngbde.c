/*! \file ngbde_user.c
 *
 * Linux user mode connector for kernel mode BDE.
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

/* Include sys/types.h before ngbde_ioctl.h to avoid type conflicts */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <lkm/ngbde_ioctl.h>
#include <bcmlu/bcmlu_ngbde.h>

#define DEV_FNAME       "/dev/linux_ngbde"
#define DEV_FMODE       (O_RDWR | O_SYNC)

#define MOD_FNAME       "linux_ngbde.ko"

static int devfd = -1;

static int
dev_open(void)
{
    char *shcmd;

    if (devfd >= 0) {
        /* Already open */
        return 0;
    }

    devfd = open(DEV_FNAME, DEV_FMODE);
    if (devfd >= 0) {
        /* Successfully opened */
        return 0;
    }

    /* If device file cannot be opened, the try to create it */
    shcmd = "f=" DEV_FNAME "; if [ ! -e $f ]; then mknod $f c 120 0; fi";
    if (system(shcmd) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to create device node %s.\n"),
                   DEV_FNAME));
        return -1;
    }
    /* If successfully created, try opening device file again */
    devfd = open(DEV_FNAME, DEV_FMODE);
    if (devfd >= 0) {
        /* Successfully opened */
        return 0;
    }

    /* If device file still cannot be opened, the try loading BDE module */
    shcmd = "/sbin/insmod " MOD_FNAME;
    if (system(shcmd) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to load kernel module %s.\n"),
                   MOD_FNAME));
        return -1;
    }

    /* If BDE module loaded successfully, try opening device file again */
    devfd = open(DEV_FNAME, DEV_FMODE);
    if (devfd >= 0) {
        /* Successfully opened */
        return 0;
    }

    /* Give up */
    LOG_ERROR(BSL_LS_SYS_PCI,
              (BSL_META("Unable to open BDE device.\n")));
    return -1;
}

static int
dev_close(void)
{
    if (devfd >= 0) {
        close(devfd);
        devfd = -1;
    }
    return 0;
}

static int
do_ioctl(int cmd, struct ngbde_ioc_cmd_s *ioc)
{
    if (ioctl(devfd, cmd, ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("BDE IOCTL command %d failed.\n"), cmd));
        return -1;
    }
    return 0;
}

int
bcmlu_ngbde_init(void)
{
    struct ngbde_ioc_cmd_s ioc;

    if (dev_open() < 0) {
        return -1;
    }

    memset(&ioc, 0, sizeof(ioc));
    if (do_ioctl(NGBDE_IOC_MOD_INFO, &ioc) < 0) {
        return -1;
    }
    if (ioc.op.mod_info.version != NGBDE_IOC_VERSION) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Kernel BDE version mismatch.\n")));
        dev_close();
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_cleanup(void)
{
    dev_close();
    return 0;
}

int
bcmlu_ngbde_num_dev_get(int *ndev)
{
    struct ngbde_ioc_cmd_s ioc;

    if (ndev == NULL) {
        return -1;
    }

    memset(&ioc, 0, sizeof(ioc));
    if (do_ioctl(NGBDE_IOC_PROBE_INFO, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }
    *ndev = ioc.op.probe_info.num_swdev;

    return ioc.rc;
}

int
bcmlu_ngbde_dev_info_get(int devid, bcmlu_dev_info_t *di)
{
    struct ngbde_ioc_cmd_s ioc;
    int idx;

    if (di == NULL) {
        return -1;
    }

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    if (do_ioctl(NGBDE_IOC_DEV_INFO, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }
    di->type = ioc.op.dev_info.type;
    di->vendor_id = ioc.op.dev_info.vendor_id;
    di->device_id = ioc.op.dev_info.device_id;
    di->revision = ioc.op.dev_info.revision;
    if (ioc.op.dev_info.flags & NGBDE_DEV_F_MSI) {
        di->flags |= BCMLU_DEV_F_MSI;
    }

    for (idx = 0; idx < 2; idx++) {
        memset(&ioc, 0, sizeof(ioc));
        ioc.devid = devid;
        ioc.op.rsrc_id.type = NGBDE_IO_RSRC_DEV_IO;
        ioc.op.rsrc_id.inst = idx;
        if (do_ioctl(NGBDE_IOC_PHYS_ADDR, &ioc) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META("Unable to connect to kernel BDE.\n")));
            return -1;
        }
        di->iowin[idx].addr = ioc.op.phys_addr.addr;
        di->iowin[idx].size = ioc.op.phys_addr.size;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_dma_info_get(int devid, bcmlu_dma_info_t *dmi)
{
    struct ngbde_ioc_cmd_s ioc;

    if (dmi == NULL) {
        return -1;
    }

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.rsrc_id.type = NGBDE_IO_RSRC_DMA_MEM;
    if (do_ioctl(NGBDE_IOC_PHYS_ADDR, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }
    dmi->addr = ioc.op.phys_addr.addr;
    dmi->size = ioc.op.phys_addr.size;

    return ioc.rc;
}

int
bcmlu_ngbde_intr_connect(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_CONN;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_disconnect(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_DISC;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_wait(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_WAIT;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_stop(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_STOP;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_regs_clr(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_REGS_CLR;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_reg_add(int devid, int irq_num, uint32_t status_reg,
                         uint32_t mask_reg, uint32_t kmask)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.irq_reg_add.irq_num = irq_num;
    ioc.op.irq_reg_add.status_reg = status_reg;
    ioc.op.irq_reg_add.mask_reg = mask_reg;
    ioc.op.irq_reg_add.kmask = kmask;
    if (do_ioctl(NGBDE_IOC_IRQ_REG_ADD, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_mask_write(int devid, int irq_num,
                            uint32_t offs, uint32_t val)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.irq_mask_wr.irq_num = irq_num;
    ioc.op.irq_mask_wr.offs = offs;
    ioc.op.irq_mask_wr.val = val;
    if (do_ioctl(NGBDE_IOC_IRQ_MASK_WR, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_pio_map(int devid, uint64_t addr, uint32_t size)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.pio_win.addr = addr;
    ioc.op.pio_win.size = size;
    if (do_ioctl(NGBDE_IOC_PIO_WIN_MAP, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_iio_map(int devid, uint64_t addr, uint32_t size)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.pio_win.addr = addr;
    ioc.op.pio_win.size = size;
    if (do_ioctl(NGBDE_IOC_IIO_WIN_MAP, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

void *
bcmlu_ngbde_mmap(uint64_t offset, size_t size)
{
    dev_open();

    return bcmlu_mmap2(devfd, offset >> 12, size);
}

int
bcmlu_ngbde_munmap(void *addr, size_t size)
{
    return munmap(addr, size);
}
