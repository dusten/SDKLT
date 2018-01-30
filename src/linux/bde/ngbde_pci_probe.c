/*! \file ngbde_pci_probe.c
 *
 * NG BDE probe for PCI devices.
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
static int use_msi = 0;
module_param(use_msi, int, 0);
MODULE_PARM_DESC(use_msi,
"Use MSI interrupts if supported by the kernel (default 0).");
/*! \endcond */

/*!
 * Use BCMDRD_DEVLIST_ENTRY macro to generate a device list based on
 * supported/installed devices.
 */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _vn, _dv, PCI_ANY_ID, PCI_ANY_ID },

/*! Include all chip variants in the list of supported devices. */
#define BCMDRD_DEVLIST_INCLUDE_ALL

static struct pci_device_id pci_id_table[] = {
#include <bcmdrd/bcmdrd_devlist.h>
    { BROADCOM_VENDOR_ID, 0xb524, PCI_ANY_ID, PCI_ANY_ID },
    { BROADCOM_VENDOR_ID, 0xb684, PCI_ANY_ID, PCI_ANY_ID },
    { 0, 0, 0, 0 }
};

static int
pci_probe(struct pci_dev *pci_dev, const struct pci_device_id *ent)
{
    int rv;
    int cmic_bar = 0;
    uint8_t rev;
    struct ngbde_dev_s ngbde_dev, *nd = &ngbde_dev;

    if (PCI_FUNC(pci_dev->devfn) > 0) {
        return 0;
    }

    memset(nd, 0, sizeof(*nd));
    nd->pci_dev = pci_dev;
    nd->dma_dev = &pci_dev->dev;
    nd->vendor_id = pci_dev->vendor;
    nd->device_id = pci_dev->device;
    nd->irq_line = pci_dev->irq;

    /* PCI revision must extracted "manually */
    pci_read_config_byte(pci_dev, PCI_REVISION_ID, &rev);
    nd->revision = rev;

    if (pci_enable_device(pci_dev)) {
        printk("Cannot enable PCI device : vendor_id = %x, device_id = %x\n",
               pci_dev->vendor, pci_dev->device);
    }
    pci_set_master(pci_dev);

    /* Check for iProc */
    if (pci_resource_len(pci_dev, 2)) {
        nd->iowin[1].addr = pci_resource_start(pci_dev, 0);
        nd->iowin[1].size = pci_resource_len(pci_dev, 0);
        cmic_bar = 2;
    }
    nd->iowin[0].addr = pci_resource_start(pci_dev, cmic_bar);
    nd->iowin[0].size = pci_resource_len(pci_dev, cmic_bar);

    spin_lock_init(&nd->lock);

#if defined(CONFIG_PCI_MSI)
    if (use_msi < 0) {
        use_msi = 1;
    }
    nd->use_msi = use_msi;
#endif

    rv = ngbde_swdev_add(nd);

    return rv;
}

static void
pci_remove(struct pci_dev* pci_dev)
{
}

static struct pci_driver pci_driver = {
    .name = MOD_NAME,
    .probe = pci_probe,
    .remove = pci_remove,
    .id_table = pci_id_table,
  /* The rest are dynamic */
};

int
ngbde_pci_probe(void)
{
    if (pci_register_driver(&pci_driver) < 0) {
        return -ENODEV;
    }

    return 0;
}

int
ngbde_pci_cleanup(void)
{
    pci_unregister_driver(&pci_driver);

    return 0;
}
