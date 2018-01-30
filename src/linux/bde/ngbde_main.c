/*! \file ngbde_main.c
 *
 * NGBDE module entry.
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
MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("NG BDE Module");
MODULE_LICENSE("GPL");
/*! \endcond */

/*!
 * \brief Remap user space DMA memory to non-cached area.
 *
 * Since we cannot flush and invalidate DMA memory from user space,
 * the DMA memory pools need to be cache-coherent, even if this means
 * that we need to remap the DMA memory as non-cached.
 *
 * If undefined, we set this value according to kernel configuration.
 */
#ifndef REMAP_DMA_NONCACHED
#  ifdef CONFIG_DMA_NONCOHERENT
#    define REMAP_DMA_NONCACHED 1
#  else
#    define REMAP_DMA_NONCACHED 0
#  endif
#endif

static int
ngbde_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int
ngbde_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*!
 * \brief Check if memory range is within existing DMA memory pools.
 *
 * \param [in] paddr Physical start address of memory range.
 * \param [in] size Size of memory range.
 *
 * \retval 0 No errors
 */
static int
ngbde_dma_range_valid(unsigned long paddr, unsigned long size)
{
    struct ngbde_dev_s *swdev;
    unsigned int num_swdev, idx;
    struct ngbde_dmamem_s *dmamem;
    unsigned int pool;

    ngbde_swdev_get_all(&swdev, &num_swdev);

    for (idx = 0; idx < num_swdev; idx++) {
        for (pool = 0; pool < NGBDE_NUM_DMAPOOL_MAX; pool++) {
            dmamem = &swdev[idx].dmapool[pool].dmamem;
            if (paddr >= dmamem->paddr &&
                (paddr + size) <= (dmamem->paddr + dmamem->size)) {
                return 1;
            }
        }
    }
    return 0;
}

/*
 * Some kernels are configured to prevent mapping of kernel RAM memory
 * into user space via the /dev/mem device.
 *
 * The function below provides a backdoor to mapping the DMA pool to
 * user space via the BDE device file.
 */
static int
ngbde_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long paddr = vma->vm_pgoff << PAGE_SHIFT;
    unsigned long size = vma->vm_end - vma->vm_start;

    if (!ngbde_dma_range_valid(paddr, size)) {
        printk("BDE: Invalid mmap range 0x%08lx/0x%lx\n", paddr, size);
        return -EINVAL;
    }

#if REMAP_DMA_NONCACHED
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
#endif

    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                        size, vma->vm_page_prot)) {
        printk("BDE: Failed to mmap phys range 0x%lx-0x%lx to 0x%lx-0x%lx\n",
               paddr, paddr + size, vma->vm_start, vma->vm_end);
        return -EAGAIN;
    }

    return 0;
}

static struct file_operations fops = {
    .open = ngbde_open,
    .release = ngbde_release,
    .unlocked_ioctl = ngbde_ioctl,
    .compat_ioctl = ngbde_ioctl,
    .mmap = ngbde_mmap,
};

/*!
 * \brief Standard module cleanup.
 *
 * \return Nothing.
 */
void __exit
cleanup_module(void)
{
    ngbde_intr_cleanup();
    ngbde_iio_cleanup();
    ngbde_pio_cleanup();
    ngbde_dma_cleanup();
    ngbde_procfs_cleanup();
    unregister_chrdev(MOD_MAJOR, MOD_NAME);
    ngbde_pci_cleanup();
    printk(KERN_INFO "Broadcom NGBDE unloaded successfully\n");
}

/*!
 * \brief Standard module initialization.
 *
 * \return Nothing.
 */
int __init
init_module(void)
{
    int rv;

    rv = register_chrdev(MOD_MAJOR, MOD_NAME, &fops);
    if (rv < 0) {
	printk(KERN_WARNING "%s: can't get major %d\n",
	       MOD_NAME, MOD_MAJOR);
	return rv;
    }

    rv = ngbde_pci_probe();
    if (rv < 0) {
	printk(KERN_WARNING "%s: No devices found\n",
	       MOD_NAME);
    }

    rv = ngbde_procfs_init();
    if (rv < 0) {
	printk(KERN_WARNING "%s: Unable to initialize proc files\n",
	       MOD_NAME);
	return rv;
    }

    rv = ngbde_dma_init();
    if (rv < 0) {
	printk(KERN_WARNING "%s: Error initializing DMA memory\n",
	       MOD_NAME);
        ngbde_procfs_cleanup();
	return rv;
    }

    printk(KERN_INFO "Broadcom NGBDE loaded successfully\n");
    return 0;
}
