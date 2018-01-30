/*! \file ngknet_linux.h
 *
 * Data structure and macro definitions for Linux kernel APIs abstraction.
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

#ifndef NGKNET_LINUX_H
#define NGKNET_LINUX_H

#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>

/*!
 * Kernel abstraction
 */

#define MODULE_PARAM(n, t, p)   module_param(n, t, p)

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
#define kal_vlan_hwaccel_put_tag(skb, proto, tci) \
    __vlan_hwaccel_put_tag(skb, tci)
#define NETIF_F_HW_VLAN_CTAG_RX NETIF_F_HW_VLAN_RX
#define NETIF_F_HW_VLAN_CTAG_TX NETIF_F_HW_VLAN_TX
#else
#define kal_vlan_hwaccel_put_tag(skb, proto, tci) \
    __vlan_hwaccel_put_tag(skb, htons(proto), tci)
#endif /* KERNEL_VERSION(3,10,0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0)
static inline int
kal_support_paged_skb(void)
{
    return false;
}
#else
static inline int
kal_support_paged_skb(void)
{
    return true;
}
#endif /* KERNEL_VERSION(3,6,0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0)
static inline struct page *
kal_dev_alloc_page(void)
{
    return NULL;
}
#elif LINUX_VERSION_CODE < KERNEL_VERSION(3,19,0)
static inline struct page *
kal_dev_alloc_page(void)
{
    return alloc_pages(GFP_ATOMIC | __GFP_ZERO | __GFP_COLD |
                       __GFP_COMP | __GFP_MEMALLOC, 0);
}
#else
static inline struct page *
kal_dev_alloc_page(void)
{
    return dev_alloc_page();
}
#endif /* KERNEL_VERSION(3,6,0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0)
static inline struct sk_buff *
kal_build_skb(void *data, unsigned int frag_size)
{
    return NULL;
}
#else
static inline struct sk_buff *
kal_build_skb(void *data, unsigned int frag_size)
{
    return build_skb(data, frag_size);
}
#endif /* KERNEL_VERSION(3,6,0) */

/*!
 * System abstraction
 */

static inline void *
sal_alloc(unsigned int sz, char *s)
{
    return kmalloc(sz, GFP_KERNEL);
}

static inline void
sal_free(void *addr)
{
    kfree(addr);
}

static inline void *
sal_memset(void *dest, int c, size_t cnt)
{
    return memset(dest, c, cnt);
}

static inline void *
sal_memcpy(void *dest, const void *src, size_t cnt)
{
    return memcpy(dest, src, cnt);
}

static inline char *
sal_strncpy(char *dest, const char *src, size_t cnt)
{
    return strncpy(dest, src, cnt);
}

/*!
 * Time
 */

extern unsigned long
sal_time_usecs(void);

/*!
 * Synchronization
 */

typedef struct sal_sem_s {
    char semaphore_opaque_type;
} *sal_sem_t;

typedef struct sal_spinlock_s {
    char spinlock_opaque_type;
} *sal_spinlock_t;

#define SAL_SEM_FOREVER         -1
#define SAL_SEM_BINARY          1
#define SAL_SEM_COUNTING        0

extern sal_sem_t
sal_sem_create(char *desc, int binary, int count);

extern void
sal_sem_destroy(sal_sem_t sem);

extern int
sal_sem_take(sal_sem_t sem, int usec);

extern int
sal_sem_give(sal_sem_t sem);

extern sal_spinlock_t
sal_spinlock_create(char *desc);

extern void
sal_spinlock_destroy(sal_spinlock_t lock);

extern int
sal_spinlock_lock(sal_spinlock_t lock);

extern int
sal_spinlock_unlock(sal_spinlock_t lock);

#endif /* NGKNET_LINUX_H */

