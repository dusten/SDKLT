/*! \file ngknet_ioctl.h
 *
 * NGKNET I/O control definitions.
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

#ifndef NGKNET_IOCTL_H
#define NGKNET_IOCTL_H

/*! Module information */
#define NGKNET_MODULE_NAME      "linux_ngknet"
#define NGKNET_MODULE_MAJOR     121

/*! Must be updated if backward compatibility is broken */
#define NGKNET_IOC_VERSION      1

/*! Max number of input arguments */
#define NGKNET_IOC_IARG_MAX     2

#define NGKNET_IOC_MAGIC        'K'

#define NGKNET_VERSION_GET      _IOR(NGKNET_IOC_MAGIC,  0xa0, unsigned int)
#define NGKNET_RX_RATE_LIMIT    _IOWR(NGKNET_IOC_MAGIC, 0xa1, unsigned int)
#define NGKNET_DEV_INIT         _IOWR(NGKNET_IOC_MAGIC, 0xb0, unsigned int)
#define NGKNET_DEV_DEINIT       _IOWR(NGKNET_IOC_MAGIC, 0xb1, unsigned int)
#define NGKNET_DEV_SUSPEND      _IOWR(NGKNET_IOC_MAGIC, 0xb2, unsigned int)
#define NGKNET_DEV_RESUME       _IOWR(NGKNET_IOC_MAGIC, 0xb3, unsigned int)
#define NGKNET_QUEUE_CONFIG     _IOWR(NGKNET_IOC_MAGIC, 0xc0, unsigned int)
#define NGKNET_QUEUE_QUERY      _IOR(NGKNET_IOC_MAGIC,  0xc1, unsigned int)
#define NGKNET_RCPU_CONFIG      _IOWR(NGKNET_IOC_MAGIC, 0xc2, unsigned int)
#define NGKNET_RCPU_GET         _IOR(NGKNET_IOC_MAGIC,  0xc3, unsigned int)
#define NGKNET_NETIF_CREATE     _IOWR(NGKNET_IOC_MAGIC, 0xd0, unsigned int)
#define NGKNET_NETIF_DESTROY    _IOWR(NGKNET_IOC_MAGIC, 0xd1, unsigned int)
#define NGKNET_NETIF_GET        _IOR(NGKNET_IOC_MAGIC,  0xd2, unsigned int)
#define NGKNET_NETIF_NEXT       _IOR(NGKNET_IOC_MAGIC,  0xd3, unsigned int)
#define NGKNET_FILT_CREATE      _IOWR(NGKNET_IOC_MAGIC, 0xe0, unsigned int)
#define NGKNET_FILT_DESTROY     _IOWR(NGKNET_IOC_MAGIC, 0xe1, unsigned int)
#define NGKNET_FILT_GET         _IOR(NGKNET_IOC_MAGIC,  0xe2, unsigned int)
#define NGKNET_FILT_NEXT        _IOR(NGKNET_IOC_MAGIC,  0xe3, unsigned int)
#define NGKNET_FILT_PN_SET      _IOWR(NGKNET_IOC_MAGIC, 0xe4, unsigned int)
#define NGKNET_INFO_GET         _IOR(NGKNET_IOC_MAGIC,  0xf0, unsigned int)
#define NGKNET_STATS_GET        _IOR(NGKNET_IOC_MAGIC,  0xf1, unsigned int)
#define NGKNET_STATS_RESET      _IOWR(NGKNET_IOC_MAGIC, 0xf2, unsigned int)

/*! Kernel module information. */
struct ngknet_ioc_mod_info {
    /*! IOCTL version used by kernel module */
    uint32_t version;
};

/*! Data transmission */
struct ngknet_ioc_data_xmit {
    /*! Data buffer address */
    uint64_t buf;

    /*! Data buffer length */
    uint32_t len;
};

/*! IOCTL operations */
union ngknet_ioc_op {
    /*! Get module info */
    struct ngknet_ioc_mod_info info;
    /*! Transmit data */
    struct ngknet_ioc_data_xmit data;
};

/*!
 * \brief NGKNET IOCTL command message.
 */
struct ngknet_ioctl {
    /*! Device number */
    uint32_t unit;

    /*! Return code (0 means success) */
    uint32_t rc;

    /*! Input arguments */
    int iarg[NGKNET_IOC_IARG_MAX];

    /*! IOCTL operation */
    union ngknet_ioc_op op;
};

#endif /* NGKNET_IOCTL_H */

