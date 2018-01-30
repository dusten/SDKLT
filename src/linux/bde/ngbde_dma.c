/*! \file ngbde_dma.c
 *
 * This module handles allocation of DMA memory pools.
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
static int dma_debug = 0;
module_param(dma_debug, int, 0);
MODULE_PARM_DESC(dma_debug,
"DMA debug output enable (default 0).");
/*! \endcond */

/*! Default size of of DMA memory pools (in MB). */
#define DMAPOOL_SIZE_DEFAULT    12

/*! Default number of DMA memory pools per device. */
#define NUM_DMAPOOL_DEFAULT     1

/*! \cond */
static int dma_size = DMAPOOL_SIZE_DEFAULT;
module_param(dma_size, int, 0);
MODULE_PARM_DESC(dma_size,
"Size of of DMA memory pools in MB (default 8 MB).");
/*! \endcond */

/*! \cond */
static char *dma_alloc;
module_param(dma_alloc, charp, 0);
MODULE_PARM_DESC(dma_alloc,
"DMA allocation method auto|kapi|pgmem (default auto)");
/*! \endcond */

/*! \cond */
static int dma_pools = NUM_DMAPOOL_DEFAULT;
module_param(dma_pools, int, 0);
MODULE_PARM_DESC(dma_pools,
"Number of DMA memory pools to pre-allocate per device (default 1).");
/*! \endcond */

/*!
 * \brief Allocate DMA memory via kernel API.
 *
 * \param [in] dmactrl DMA allocation control.
 * \param [out] dmamem DMA allocation result.
 *
 * \return Nothing.
 */
static void
ngbde_dmamem_kapi_alloc(ngbde_dmactrl_t *dmactrl, ngbde_dmamem_t *dmamem)
{
    void *vaddr;
    dma_addr_t paddr;

    vaddr = dma_alloc_coherent(dmactrl->dev, dmactrl->size, &paddr,
                               dmactrl->flags);
    if (vaddr) {
        /* Store allocation information in dmamem structure */
        dmamem->vaddr = vaddr;
        dmamem->paddr = paddr;
        dmamem->dev = dmactrl->dev;
        dmamem->size = dmactrl->size;
        dmamem->type = NGBDE_DMA_T_KAPI;

        /* Write small signature for debug purposes */
        strcpy((char *)vaddr, "DMA_KAPI");

        if (dma_debug) {
            printk("DMA: Allocated %d KB of KAPI memory\n",
                   (int)(dmamem->size / ONE_KB));
        }
    } else {
        if (dma_debug) {
            printk("DMA: Failed to allocate KAPI memory\n");
        }
    }
}

/*!
 * \brief Allocate DMA memory via page allocator.
 *
 * \param [in] dmactrl DMA allocation control.
 * \param [out] dmamem DMA allocation result.
 *
 * \return Nothing.
 */
static void
ngbde_dmamem_pgmem_alloc(ngbde_dmactrl_t *dmactrl, ngbde_dmamem_t *dmamem)
{
    void *vaddr;

    vaddr = ngbde_pgmem_alloc(dmactrl->size, dmactrl->flags);
    if (vaddr) {
        /* Store allocation information in dmamem structure */
        dmamem->vaddr = vaddr;
        dmamem->paddr = virt_to_phys(vaddr);
        dmamem->size = dmactrl->size;
        dmamem->type = NGBDE_DMA_T_PGMEM;

        /* Write small signature for debug purposes */
        strcpy((char *)vaddr, "DMA_PGMEM");

        if (dma_debug) {
            printk("DMA: Allocated %d KB of PGMEM memory\n",
                   (int)(dmamem->size / ONE_KB));
        }
    } else {
        if (dma_debug) {
            printk("DMA: Failed to allocate PGMEM memory\n");
        }
    }
}

/*!
 * \brief Allocate DMA memory.
 *
 * Depending on the DMA allocation control parameters, we select one
 * of several DMA memory allocation methods.
 *
 * \param [in] dmactrl DMA allocation control.
 * \param [out] dmamem DMA allocation result.
 *
 * \return Nothing.
 */
static int
ngbde_dmamem_alloc(ngbde_dmactrl_t *dmactrl, ngbde_dmamem_t *dmamem)
{
    int kapi = 0;

#ifdef CONFIG_CMA
    if (dmactrl->size <= CONFIG_CMA_SIZE_MBYTES) {
        kapi = 1;
    }
#else
    if (dmactrl->size <= (1 << (MAX_ORDER - 1 + PAGE_SHIFT))) {
        kapi = 1;
    }
#endif

    /* Allocation via kernel DMA API (if allowed) */
    if (kapi) {
        switch (dmactrl->pref_type) {
        case NGBDE_DMA_T_AUTO:
        case NGBDE_DMA_T_KAPI:
            ngbde_dmamem_kapi_alloc(dmactrl, dmamem);
            break;
        default:
            break;
        }
    }

    /* Allocation via private page allocator */
    if (dmamem->vaddr == NULL) {
        switch (dmactrl->pref_type) {
        case NGBDE_DMA_T_AUTO:
        case NGBDE_DMA_T_PGMEM:
            ngbde_dmamem_pgmem_alloc(dmactrl, dmamem);
            break;
        default:
            break;
        }
    }

    if (dmamem->vaddr == NULL) {
        printk("DMA: Failed to allocate DMA memory\n");
        return -1;
    }

    return 0;
}

/*!
 * \brief Free DMA memory.
 *
 * Free DMA memory allocated via \ref ngbde_dmamem_alloc.
 *
 * \param [in] dmamem DMA allocation result from \ref ngbde_dmamem_alloc.
 *
 * \return Nothing.
 */
static int
ngbde_dmamem_free(ngbde_dmamem_t *dmamem)
{
    switch (dmamem->type) {
    case NGBDE_DMA_T_KAPI:
        if (dma_debug) {
            printk("DMA: Freeing %d KB of KAPI memory\n",
                   (int)(dmamem->size / ONE_KB));
        }
        dma_free_coherent(dmamem->dev, dmamem->size,
                          dmamem->vaddr, dmamem->paddr);
        memset(dmamem, 0, sizeof(*dmamem));
        break;
    case NGBDE_DMA_T_PGMEM:
        if (dma_debug) {
            printk("DMA: Freeing %d KB of PGMEM memory\n",
                   (int)(dmamem->size / ONE_KB));
        }
        ngbde_pgmem_free(dmamem->vaddr);
        memset(dmamem, 0, sizeof(*dmamem));
        break;
    case NGBDE_DMA_T_NONE:
        /* Nothing to free */
        break;
    default:
        printk("DMA: Unable to free unknown DMA memory type\n");
        break;
    }
    return 0;
}

/*!
 * \brief Free all DMA memory pools for all devices.
 *
 * \return Nothing.
 */
void
ngbde_dma_cleanup(void)
{
    struct ngbde_dev_s *swdev;
    unsigned int num_swdev, idx;
    unsigned int pool;

    ngbde_swdev_get_all(&swdev, &num_swdev);

    for (idx = 0; idx < num_swdev; idx++) {
        for (pool = 0; pool < NGBDE_NUM_DMAPOOL_MAX; pool++) {
            ngbde_dmamem_free(&swdev[idx].dmapool[pool].dmamem);
        }
    }
}

/*!
 * \brief Allocate DMA memory pools for all devices.
 *
 * \return Nothing.
 */
int
ngbde_dma_init(void)
{
    int rv;
    struct ngbde_dev_s *swdev;
    unsigned int num_swdev, idx;
    int dma_type = NGBDE_DMA_T_AUTO;
    struct ngbde_dmapool_s *dmapool;
    unsigned int pool;

    ngbde_swdev_get_all(&swdev, &num_swdev);

    for (idx = 0; idx < num_swdev; idx++) {

        /* Default DMA memory size per device */
        if (dma_size < 0) {
            dma_size = DMAPOOL_SIZE_DEFAULT;
        }

        /* Check for forced DMA allocation method */
        if (dma_alloc) {
            if (strcmp(dma_alloc, "kapi") == 0) {
                dma_type = NGBDE_DMA_T_KAPI;
            } else if (strcmp(dma_alloc, "pgmem") == 0) {
                dma_type = NGBDE_DMA_T_PGMEM;
            } else {
                printk("PROBE: Unknown DMA type: %s\n", dma_alloc);
            }
        }

        /* Set DMA allocation parameters */
        for (pool = 0; pool < NGBDE_NUM_DMAPOOL_MAX; pool++) {
            dmapool = &swdev->dmapool[pool];
            dmapool->dmactrl.dev = swdev->dma_dev;
            dmapool->dmactrl.size = dma_size * ONE_MB;
            dmapool->dmactrl.pref_type = dma_type;
            dmapool->dmactrl.flags = GFP_KERNEL | GFP_DMA32;
        }

        /* Number of DMA memory pools per device */
        if ((unsigned int)dma_pools >= NGBDE_NUM_DMAPOOL_MAX) {
            dma_pools = NUM_DMAPOOL_DEFAULT;
        }

        /* Allocate DMA pools */
        for (pool = 0; pool < dma_pools; pool++) {
            dmapool = &swdev[idx].dmapool[pool];
            rv = ngbde_dmamem_alloc(&dmapool->dmactrl, &dmapool->dmamem);
            if (rv < 0) {
                printk(KERN_WARNING "%s: Unable to allocate DMA pool %d %d\n",
                       MOD_NAME, idx, pool);
            }
        }
    }
    return 0;
}
