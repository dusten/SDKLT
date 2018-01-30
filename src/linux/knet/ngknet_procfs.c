/*! \file ngknet_procfs.c
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

#include <lkm/lkm.h>
#include <lkm/ngknet_ioctl.h>
#include "ngknet_main.h"
#include "ngknet_extra.h"

extern struct ngknet_dev ngknet_devices[];
extern void bcmcnet_pdma_rx_ring_dump(struct pdma_dev *dev, int queue);
extern void bcmcnet_pdma_tx_ring_dump(struct pdma_dev *dev, int queue);
extern int bcmcnet_pdma_rx_queue_reg_dump(struct pdma_dev *dev, int queue);
extern int bcmcnet_pdma_tx_queue_reg_dump(struct pdma_dev *dev, int queue);

static struct proc_dir_entry *proc_root = NULL;

static int
proc_debug_level_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Debug level: 0x%x\n", ngknet_debug_level_get());

    return 0;
}

static int
proc_debug_level_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_debug_level_show, NULL);
}

static ssize_t
proc_debug_level_write(struct file *file, const char *buf,
                       size_t count, loff_t *loff)
{
    char level_str[11] = {0};
    int debug_level;

    if (copy_from_user(level_str, buf, sizeof(level_str) - 1)) {
        return -EFAULT;
    }
    debug_level = simple_strtol(level_str, NULL, 16);

    ngknet_debug_level_set(debug_level);
    printk("Debug level set to: 0x%x\n", debug_level);

    return count;
}

static int
proc_debug_level_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

static struct file_operations proc_debug_level_fops = {
    owner:      THIS_MODULE,
    open:       proc_debug_level_open,
    read:       seq_read,
    write:      proc_debug_level_write,
    llseek:     seq_lseek,
    release:    proc_debug_level_release,
};

static int
proc_device_info_show(struct seq_file *m, void *v)
{
    struct ngknet_dev *dev;
    struct bcmcnet_dev_info *info;
    int di, qi, ai = 0;
    int rv;

    for (di = 0; di < NUM_PDMA_DEV_MAX; di++) {
        dev = &ngknet_devices[di];
        if (!(dev->flags & NGKNET_DEV_ACTIVE)) {
            continue;
        }
        ai++;

        rv = bcmcnet_pdma_dev_info_get(&dev->pdma_dev);
        if (SHR_FAILURE(rv)) {
            printk("ngknet: get device%d info failed\n", di);
            break;
        }

        info = &dev->pdma_dev.info;
        seq_printf(m, "dev_no:         %d\n",   di);
        seq_printf(m, "dev_name:       %s\n",   info->dev_name);
        seq_printf(m, "dev_id:         0x%x\n", info->dev_id);
        seq_printf(m, "dev_type:       %d\n",   info->dev_type);
        seq_printf(m, "max_groups:     %d\n",   info->max_groups);
        seq_printf(m, "max_queues:     %d\n",   info->max_queues);
        seq_printf(m, "bm_groups:      0x%x\n", info->bm_groups);
        seq_printf(m, "bm_rx_queues:   0x%x\n", info->bm_rx_queues);
        seq_printf(m, "bm_tx_queues:   0x%x\n", info->bm_tx_queues);
        seq_printf(m, "nb_groups:      %d\n",   info->nb_groups);
        seq_printf(m, "nb_rx_queues:   %d\n",   info->nb_rx_queues);
        seq_printf(m, "nb_tx_queues:   %d\n",   info->nb_tx_queues);
        seq_printf(m, "rx_desc_size:   %d\n",   info->rx_desc_size);
        seq_printf(m, "tx_desc_size:   %d\n",   info->tx_desc_size);
        seq_printf(m, "rx_ph_size:     %d\n",   info->rx_ph_size);
        seq_printf(m, "tx_ph_size:     %d\n",   info->tx_ph_size);
        for (qi = 0; qi < info->nb_rx_queues; qi++) {
            seq_printf(m, "rx_buf_sz[%d]:   %d\n", qi, info->rx_buf_size[qi]);
        }
        for (qi = 0; qi < info->nb_rx_queues; qi++) {
            seq_printf(m, "nb_rx_desc[%d]:  %d\n", qi, info->nb_rx_desc[qi]);
        }
        for (qi = 0; qi < info->nb_tx_queues; qi++) {
            seq_printf(m, "nb_tx_desc[%d]:  %d\n", qi, info->nb_tx_desc[qi]);
        }
    }

    if (!ai) {
        seq_printf(m, "%s\n", "No active device");
    } else {
        seq_printf(m, "------------------------\n");
        seq_printf(m, "Total %d devices\n", ai);
    }

    return 0;
}

static int
proc_device_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_device_info_show, NULL);
}

static int
proc_device_info_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

static struct file_operations proc_device_info_fops = {
    owner:      THIS_MODULE,
    open:       proc_device_info_open,
    read:       seq_read,
    llseek:     seq_lseek,
    release:    proc_device_info_release,
};

static int
proc_filter_info_show(struct seq_file *m, void *v)
{
    struct ngknet_dev *dev;
    ngknet_filter_t filt = {0};
    int di, fd, ai = 0;
    int rv;

    for (di = 0; di < NUM_PDMA_DEV_MAX; di++) {
        dev = &ngknet_devices[di];
        if (!(dev->flags & NGKNET_DEV_ACTIVE)) {
            continue;
        }
        ai++;

        do {
            rv = ngknet_filter_get_next(dev, &filt);
            if (SHR_FAILURE(rv)) {
                printk("ngknet: get device%d filter failed\n", di);
                break;
            }

            seq_printf(m, "dev_no:         %d\n",   di);
            seq_printf(m, "id:             %d\n",   filt.id);
            seq_printf(m, "next:           %d\n",   filt.next);
            seq_printf(m, "type:           %d\n",   filt.type);
            seq_printf(m, "flags:          0x%x\n", filt.flags);
            seq_printf(m, "prio:           %d\n",   filt.priority);
            seq_printf(m, "chan:           %d\n",   filt.chan);
            seq_printf(m, "desc:           %s\n",   filt.desc);
            seq_printf(m, "dest_type:      %d\n",   filt.dest_type);
            seq_printf(m, "dest_id:        %d\n",   filt.dest_id);
            seq_printf(m, "dest_proto:     0x%x\n", filt.dest_proto);
            seq_printf(m, "mirror_type:    %d\n",   filt.mirror_type);
            seq_printf(m, "mirror_id:      %d\n",   filt.mirror_id);
            seq_printf(m, "mirror_proto:   0x%x\n", filt.mirror_proto);
            seq_printf(m, "oob_offset:     %d\n",   filt.oob_data_offset);
            seq_printf(m, "oob_size:       %d\n",   filt.oob_data_size);
            seq_printf(m, "pkt_offset:     %d\n",   filt.pkt_data_offset);
            seq_printf(m, "pkt_size:       %d\n",   filt.pkt_data_size);
            seq_printf(m, "filt_data:      ");
            for (fd = 0; fd < filt.oob_data_size + filt.pkt_data_size; fd++) {
                seq_printf(m, "%02x ", filt.data.b[fd]);
                if ((fd + 1) % 32 == 0 ||
                    (fd + 1) == (filt.oob_data_size + filt.pkt_data_size)) {
                    seq_printf(m, "\n");
                    if ((fd + 1) < (filt.oob_data_size + filt.pkt_data_size)) {
                        seq_printf(m, "                ");
                    }
                }
            }
            if (fd == 0) {
                seq_printf(m, "\n");
            }
            seq_printf(m, "filt_mask:      ");
            for (fd = 0; fd < filt.oob_data_size + filt.pkt_data_size; fd++) {
                seq_printf(m, "%02x ", filt.mask.b[fd]);
                if ((fd + 1) % 32 == 0 ||
                    (fd + 1) == (filt.oob_data_size + filt.pkt_data_size)) {
                    seq_printf(m, "\n");
                    if ((fd + 1) < (filt.oob_data_size + filt.pkt_data_size)) {
                        seq_printf(m, "                ");
                    }
                }
            }
            if (fd == 0) {
                seq_printf(m, "\n");
            }
        } while (filt.next);
    }

    if (!ai) {
        seq_printf(m, "%s\n", "No active device");
    } else {
        seq_printf(m, "------------------------\n");
        seq_printf(m, "Total %d devices\n", ai);
    }

    return 0;
}

static int
proc_filter_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_filter_info_show, NULL);
}

static int
proc_filter_info_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

static struct file_operations proc_filter_info_fops = {
    owner:      THIS_MODULE,
    open:       proc_filter_info_open,
    read:       seq_read,
    llseek:     seq_lseek,
    release:    proc_filter_info_release,
};

static int
proc_pkt_stats_show(struct seq_file *m, void *v)
{
    struct ngknet_dev *dev;
    struct bcmcnet_dev_stats *stats;
    int di, qi, ai = 0;
    int rv;

    for (di = 0; di < NUM_PDMA_DEV_MAX; di++) {
        dev = &ngknet_devices[di];
        if (!(dev->flags & NGKNET_DEV_ACTIVE)) {
            continue;
        }
        ai++;

        rv = bcmcnet_pdma_dev_stats_get(&dev->pdma_dev);
        if (SHR_FAILURE(rv)) {
            printk("ngknet: get device%d stats failed\n", di);
            break;
        }

        stats = &dev->pdma_dev.stats;
        seq_printf(m, "rx_packets:     %llu\n", (unsigned long long)stats->rx_packets);
        seq_printf(m, "rx_bytes:       %llu\n", (unsigned long long)stats->rx_bytes);
        for (qi = 0; qi < dev->pdma_dev.ctrl.nb_rxq; qi++) {
            seq_printf(m, "rx_packets[%d]:  %llu\n", qi, (unsigned long long)stats->rxq_packets[qi]);
            seq_printf(m, "rx_bytes[%d]:    %llu\n", qi, (unsigned long long)stats->rxq_bytes[qi]);
        }
        seq_printf(m, "rx_dropped:     %llu\n", (unsigned long long)stats->rx_dropped);
        seq_printf(m, "rx_errors:      %llu\n", (unsigned long long)stats->rx_errors);
        seq_printf(m, "rx_nomems:      %llu\n", (unsigned long long)stats->rx_nomems);
        seq_printf(m, "tx_packets:     %llu\n", (unsigned long long)stats->tx_packets);
        seq_printf(m, "tx_bytes:       %llu\n", (unsigned long long)stats->tx_bytes);
        for (qi = 0; qi < dev->pdma_dev.ctrl.nb_txq; qi++) {
            seq_printf(m, "tx_packets[%d]:  %llu\n", qi, (unsigned long long)stats->txq_packets[qi]);
            seq_printf(m, "tx_bytes[%d]:    %llu\n", qi, (unsigned long long)stats->txq_bytes[qi]);
        }
        seq_printf(m, "tx_dropped:     %llu\n", (unsigned long long)stats->tx_dropped);
        seq_printf(m, "tx_errors:      %llu\n", (unsigned long long)stats->tx_errors);
        seq_printf(m, "tx_xoffs:       %llu\n", (unsigned long long)stats->tx_xoffs);
        seq_printf(m, "interrupts:     %llu\n", (unsigned long long)stats->intrs);
    }

    if (!ai) {
        seq_printf(m, "%s\n", "No active device");
    } else {
        seq_printf(m, "------------------------\n");
        seq_printf(m, "Total %d devices\n", ai);
    }

    return 0;
}

static int
proc_pkt_stats_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_pkt_stats_show, NULL);
}

static int
proc_pkt_stats_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

static struct file_operations proc_pkt_stats_fops = {
    owner:      THIS_MODULE,
    open:       proc_pkt_stats_open,
    read:       seq_read,
    llseek:     seq_lseek,
    release:    proc_pkt_stats_release,
};

static int
proc_rate_limit_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Rx rate limit: %d pps\n", ngknet_rx_rate_limit_get());

    return 0;
}

static int
proc_rate_limit_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_rate_limit_show, NULL);
}

static ssize_t
proc_rate_limit_write(struct file *file, const char *buf,
                      size_t count, loff_t *loff)
{
    char limit_str[9] = {0};
    int rate_limit;

    if (copy_from_user(limit_str, buf, sizeof(limit_str) - 1)) {
        return -EFAULT;
    }
    rate_limit = simple_strtol(limit_str, NULL, 10);

    ngknet_rx_rate_limit_set(rate_limit);
    printk("Rx rate limit set to: %d pps\n", rate_limit);

    return count;
}

static int
proc_rate_limit_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

static struct file_operations proc_rate_limit_fops = {
    owner:      THIS_MODULE,
    open:       proc_rate_limit_open,
    read:       seq_read,
    write:      proc_rate_limit_write,
    llseek:     seq_lseek,
    release:    proc_rate_limit_release,
};

static int
proc_reg_status_show(struct seq_file *m, void *v)
{
    struct ngknet_dev *dev;
    int di, qi, ai = 0;

    for (di = 0; di < NUM_PDMA_DEV_MAX; di++) {
        dev = &ngknet_devices[di];
        if (!(dev->flags & NGKNET_DEV_ACTIVE)) {
            continue;
        }
        ai++;
        for (qi = 0; qi < dev->pdma_dev.ctrl.nb_rxq; qi++) {
            bcmcnet_pdma_rx_queue_reg_dump(&dev->pdma_dev, qi);
        }
        for (qi = 0; qi < dev->pdma_dev.ctrl.nb_txq; qi++) {
            bcmcnet_pdma_tx_queue_reg_dump(&dev->pdma_dev, qi);
        }
    }

    if (!ai) {
        seq_printf(m, "%s\n", "No active device");
    } else {
        seq_printf(m, "------------------------\n");
        seq_printf(m, "Total %d devices\n", ai);
    }

    return 0;
}

static int
proc_reg_status_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_reg_status_show, NULL);
}

static int
proc_reg_status_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

static struct file_operations proc_reg_status_fops = {
    owner:      THIS_MODULE,
    open:       proc_reg_status_open,
    read:       seq_read,
    llseek:     seq_lseek,
    release:    proc_reg_status_release,
};

static int
proc_ring_status_show(struct seq_file *m, void *v)
{
    struct ngknet_dev *dev;
    int di, qi, ai = 0;

    for (di = 0; di < NUM_PDMA_DEV_MAX; di++) {
        dev = &ngknet_devices[di];
        if (!(dev->flags & NGKNET_DEV_ACTIVE)) {
            continue;
        }
        ai++;
        seq_printf(m, "%s-%d, ", "Unit", di);
        for (qi = 0; qi < dev->pdma_dev.ctrl.nb_rxq; qi++) {
            bcmcnet_pdma_rx_ring_dump(&dev->pdma_dev, qi);
        }
        seq_printf(m, "%s%d, ", "Rx queues: ", qi);
        for (qi = 0; qi < dev->pdma_dev.ctrl.nb_txq; qi++) {
            bcmcnet_pdma_tx_ring_dump(&dev->pdma_dev, qi);
        }
        seq_printf(m, "%s%d. ", "Tx queues: ", qi);
        seq_printf(m, "\n");
    }

    if (!ai) {
        seq_printf(m, "%s\n", "No active device");
    } else {
        seq_printf(m, "------------------------\n");
        seq_printf(m, "Total %d devices\n", ai);
    }

    return 0;
}

static int
proc_ring_status_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_ring_status_show, NULL);
}

static int
proc_ring_status_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

static struct file_operations proc_ring_status_fops = {
    owner:      THIS_MODULE,
    open:       proc_ring_status_open,
    read:       seq_read,
    llseek:     seq_lseek,
    release:    proc_ring_status_release,
};

int
ngknet_procfs_init(void)
{
    struct proc_dir_entry *entry = NULL;

    proc_root = proc_mkdir(NGKNET_MODULE_NAME, NULL);
    if (proc_root == NULL) {
        printk(KERN_ERR "ngknet: proc_mkdir failed\n");
        return -1;
    }

    PROC_CREATE(entry, "debug_level", 0666, proc_root, &proc_debug_level_fops);
    if (entry == NULL) {
        printk(KERN_ERR "ngknet: proc_create failed\n");
        return -1;
    }

    PROC_CREATE(entry, "device_info", 0444, proc_root, &proc_device_info_fops);
    if (entry == NULL) {
        printk(KERN_ERR "ngknet: proc_create failed\n");
        return -1;
    }

    PROC_CREATE(entry, "filter_info", 0444, proc_root, &proc_filter_info_fops);
    if (entry == NULL) {
        printk(KERN_ERR "ngknet: proc_create failed\n");
        return -1;
    }

    PROC_CREATE(entry, "pkt_stats", 0444, proc_root, &proc_pkt_stats_fops);
    if (entry == NULL) {
        printk(KERN_ERR "ngknet: proc_create failed\n");
        return -1;
    }

    PROC_CREATE(entry, "rate_limit", 0666, proc_root, &proc_rate_limit_fops);
    if (entry == NULL) {
        printk(KERN_ERR "ngknet: proc_create failed\n");
        return -1;
    }

    PROC_CREATE(entry, "reg_status", 0444, proc_root, &proc_reg_status_fops);
    if (entry == NULL) {
        printk(KERN_ERR "ngknet: proc_create failed\n");
        return -1;
    }

    PROC_CREATE(entry, "ring_status", 0444, proc_root, &proc_ring_status_fops);
    if (entry == NULL) {
        printk(KERN_ERR "ngknet: proc_create failed\n");
        return -1;
    }

    return 0;
}

int
ngknet_procfs_cleanup(void)
{
    remove_proc_entry("debug_level", proc_root);
    remove_proc_entry("device_info", proc_root);
    remove_proc_entry("filter_info", proc_root);
    remove_proc_entry("pkt_stats", proc_root);
    remove_proc_entry("rate_limit", proc_root);
    remove_proc_entry("reg_status", proc_root);
    remove_proc_entry("ring_status", proc_root);

    remove_proc_entry(NGKNET_MODULE_NAME, NULL);

    return 0;
}

