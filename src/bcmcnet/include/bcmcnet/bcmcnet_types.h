/*! \file bcmcnet_types.h
 *
 * BCMCNET public data structure and macro definitions.
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

#ifndef BCMCNET_TYPES_H
#define BCMCNET_TYPES_H

#include <bcmcnet/bcmcnet_dep.h>

/*! Maximum length of device name */
#define DEV_NAME_LEN_MAX    16

/*! Maximum number of groups supported each device */
#define NUM_GRP_MAX         4

/*! Maximum number of queues supported each group */
#define NUM_Q_PER_GRP       8

/*! Maximum number of queues supported each device */
#define NUM_QUE_MAX         (NUM_GRP_MAX * NUM_Q_PER_GRP)

/*! Maximum length of jumbo frame */
#define JUMBO_FRAME_LEN_MAX 0xffff

/*! Maximum Rx buffer size */
#define RX_BUF_SIZE_MAX     JUMBO_FRAME_LEN_MAX

/*! Minimum Rx buffer size */
#define RX_BUF_SIZE_MIN     68

/*! Default Rx buffer size */
#define RX_BUF_SIZE_DFLT    9216

/*! CMICD name */
#define CMICD_DEV_NAME      "cmicd"

/*! CMICX name */
#define CMICX_DEV_NAME      "cmicx"

/*!
 * \brief Transmission direction.
 */
enum pdma_dir {
    PDMA_Q_RX = 0,
    PDMA_Q_TX
};

/*!
 * \brief Device information.
 */
struct bcmcnet_dev_info {
    /*! Device name */
    char dev_name[DEV_NAME_LEN_MAX];

    /*! Device ID */
    uint32_t dev_id;

    /*! Device type */
    uint32_t dev_type;

    /*! Maximum number of groups */
    uint32_t max_groups;

    /*! Maximum number of queues */
    uint32_t max_queues;

    /*! Bitmap of groups at work */
    uint32_t bm_groups;

    /*! Bitmap of Rx queues at work */
    uint32_t bm_rx_queues;

    /*! Bitmap of Tx queues at work */
    uint32_t bm_tx_queues;

    /*! Number of groups at work */
    uint32_t nb_groups;

    /*! Number of Rx queues at work */
    uint32_t nb_rx_queues;

    /*! Number of Tx queues at work */
    uint32_t nb_tx_queues;

    /*! Rx descriptor size */
    uint32_t rx_desc_size;

    /*! Tx descriptor size */
    uint32_t tx_desc_size;

    /*! Rx packet header size */
    uint32_t rx_ph_size;

    /*! Tx packet header size */
    uint32_t tx_ph_size;

    /*! Rx buffer size */
    uint32_t rx_buf_dflt;

    /*! Number of descriptors for a queue */
    uint32_t nb_desc_dflt;

    /*! Rx buffer size per queue */
    uint32_t rx_buf_size[NUM_QUE_MAX];

    /*! Number of Rx descriptors per queue */
    uint32_t nb_rx_desc[NUM_QUE_MAX];

    /*! Number of Tx descriptors per queue */
    uint32_t nb_tx_desc[NUM_QUE_MAX];
};

/*!
 * \brief Device statistics.
 */
struct bcmcnet_dev_stats {
    /*! Number of successfully received packets */
    uint64_t rx_packets;

    /*! Number of successfully received bytes */
    uint64_t rx_bytes;

    /*! Number of dropped packets */
    uint64_t rx_dropped;

    /*! Number of erroneous received packets */
    uint64_t rx_errors;

    /*! Number of error head packets */
    uint64_t rx_head_errors;

    /*! Number of error data packets */
    uint64_t rx_data_errors;

    /*! Number of error cell packets */
    uint64_t rx_cell_errors;

    /*! Number of RX pktbuf allocation failures */
    uint64_t rx_nomems;

    /*! Number of successfully transmitted packets */
    uint64_t tx_packets;

    /*! Number of successfully transmitted bytes */
    uint64_t tx_bytes;

    /*! Number of dropped packets */
    uint64_t tx_dropped;

    /*! Number of failed transmitted packets */
    uint64_t tx_errors;

    /*! Number of suspended transmission */
    uint64_t tx_xoffs;

    /*! Number of interrupts */
    uint64_t intrs;

    /*! Number of successfully received packets per queue */
    uint64_t rxq_packets[NUM_QUE_MAX];

    /*! Number of successfully received bytes per queue */
    uint64_t rxq_bytes[NUM_QUE_MAX];

    /*! Number of dropped packets per queue */
    uint64_t rxq_dropped[NUM_QUE_MAX];

    /*! Number of erroneous received packets per queue */
    uint64_t rxq_errors[NUM_QUE_MAX];

    /*! Number of error head packets per queue */
    uint64_t rxq_head_errors[NUM_QUE_MAX];

    /*! Number of error data packets per queue */
    uint64_t rxq_data_errors[NUM_QUE_MAX];

    /*! Number of error cell packets per queue */
    uint64_t rxq_cell_errors[NUM_QUE_MAX];

    /*! Number of RX pktbuf allocation failures per queue */
    uint64_t rxq_nomems[NUM_QUE_MAX];

    /*! Number of successfully transmitted bytes per queue */
    uint64_t txq_packets[NUM_QUE_MAX];

    /*! Number of successfully transmitted bytes per queue */
    uint64_t txq_bytes[NUM_QUE_MAX];

    /*! Number of dropped packets per queue */
    uint64_t txq_dropped[NUM_QUE_MAX];

    /*! Number of failed transmitted packets per queue */
    uint64_t txq_errors[NUM_QUE_MAX];

    /*! Number of suspended transmission per queue */
    uint64_t txq_xoffs[NUM_QUE_MAX];
};

#endif /* BCMCNET_TYPES_H */

