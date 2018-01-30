/*! \file ngknet_dev.h
 *
 * NGKNET device definitions.
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

#ifndef NGKNET_DEV_H
#define NGKNET_DEV_H

/*! Device name length */
#define NGKNET_DEV_NAME_MAX     16

/*! Maximum number of virtual network devices */
#define NUM_VDEV_MAX            64

/*! Maximum number of filters */
#define NUM_FILTER_MAX          256

/*!
 * \brief System network interface
 *
 * Network interface types:
 *
 *  NGKNET_NETIF_T_VLAN
 *  Transmits to this interface will go to ingress PIPE of switch
 *  CPU port using specified VLAN ID. Packet will be switched.
 *
 *  NGKNET_NETIF_T_PORT
 *  Transmits to this interface will go to unmodified to specified
 *  physical switch port. All switching logic is bypassed. Meta data
 *  should be provided when this interface is created.
 *
 *  NGKNET_NETIF_T_META
 *  Transmits to this interface will be done using raw meta data
 *  as DMA descriptors.
 *
 * Network interface flags:
 *
 *  NGKNET_NETIF_F_RCPU_ENCAP
 *  Use RCPU encapsulation for packets that enter and exit this
 *  interface.
 *
 *  NGKNET_NETIF_F_ADD_TAG
 *  Add VLAN tag to packets sent directly to physical port.
 */
/*! Max network interface name length */
#define NGKNET_NETIF_NAME_MAX       16
/*! Max network interface meta bytes */
#define NGKNET_NETIF_META_MAX       16

/*! Send packets to switch */
#define NGKNET_NETIF_T_VLAN         0
/*! Send packets to port */
#define NGKNET_NETIF_T_PORT         1
/*! Send packets with matadata attached */
#define NGKNET_NETIF_T_META         2

/*! Send packets with RCPU encapsulation */
#define NGKNET_NETIF_F_RCPU_ENCAP   (1U << 0)
/*! Send packets with vlan tag */
#define NGKNET_NETIF_F_ADD_TAG      (1U << 1)
/*! Bind network interface to channel */
#define NGKNET_NETIF_F_BIND_CHAN    (1U << 2)

/*!
 * \brief Network interface description.
 */
typedef struct ngknet_netif_s {
    /*! This network interface ID */
    uint16_t id;

    /*! Next network interface ID */
    uint16_t next;

    /*! Network interface type */
    uint16_t type;

    /*! Network interface flags */
    uint16_t flags;

    /*! Network interface VLAN ID */
    uint16_t vlan;

    /*! Network interface MAC address */
    uint8_t macaddr[6];

    /*! Network interface MTU */
    uint32_t mtu;

    /*! Network interface bound to channel */
    uint32_t chan;

    /*! Network interface name */
    char name[NGKNET_NETIF_NAME_MAX];

    /*! Metadata length used to send packets to port */
    uint32_t meta_len;

    /*! Metadata used to send packets to port */
    uint8_t meta_data[NGKNET_NETIF_META_MAX];
} ngknet_netif_t;

/*!
 * \brief Packet filters
 *
 * Filters work like software TCAMs where a mask is applied to the
 * source data, and the result is then compared to the filter data.
 *
 * Filters are checked in priority order with the lowest priority
 * values being checked first (i.e. 0 is the highest priority).
 *
 * Filter types:
 *
 *  NGKNET_FILTER_T_RX_PKT
 *  Filter data and mask are applied to the Rx DMA control block
 *  as well as to the Rx packet contents.
 *
 * Destination types:
 *
 *  NGKNET_FILTER_DEST_T_NULL
 *  Packet is dropped.
 *
 *  NGKNET_FILTER_DEST_T_NETIF
 *  Packet is sent to network interface with ID <dest_id>.
 *
 * Filter flags:
 *
 *  NGKNET_FILTER_F_ANY_DATA
 *  When this flags is set the filter will match any packet on
 *  the associated unit.
 *
 *  NGKNET_FILTER_F_STRIP_TAG
 *  Strip VLAN tag before packet is sent to destination.
 */
/*! Roundup to word */
#define BYTES2WORDS(bytes)          ((bytes + 3) / 4)

/*! Max filter description length */
#define NGKNET_FILTER_DESC_MAX      32
/*! Max filter bytes size */
#define NGKNET_FILTER_BYTES_MAX     256
/*! Max filter words size */
#define NGKNET_FILTER_WORDS_MAX     BYTES2WORDS(NGKNET_FILTER_BYTES_MAX)

/*! Drop packet */
#define NGKNET_FILTER_DEST_T_NULL   0
/*! Send packet to netif */
#define NGKNET_FILTER_DEST_T_NETIF  1

/*! Filter to Rx */
#define NGKNET_FILTER_T_RX_PKT      1

/*! Match any data */
#define NGKNET_FILTER_F_ANY_DATA    (1U << 0)
/*! Strip vlan tag */
#define NGKNET_FILTER_F_STRIP_TAG   (1U << 1)

/*!
 * \brief Filter description.
 */
typedef struct ngknet_filter_s {
    /*! This filter ID */
    uint16_t id;

    /*! Next filter ID */
    uint16_t next;

    /*! Filter type */
    uint16_t type;

    /*! Filter flags */
    uint16_t flags;

    /*! Filter priority */
    uint32_t priority;

    /*! Filter belong to */
    uint32_t chan;

    /*! Filter description */
    char desc[NGKNET_FILTER_DESC_MAX];

    /*! Destination type */
    uint16_t dest_type;

    /*! Destination network interface ID */
    uint16_t dest_id;

    /*! Destination network interface protocol type */
    uint16_t dest_proto;

    /*! Mirror type */
    uint16_t mirror_type;

    /*! Mirror network interface ID */
    uint16_t mirror_id;

    /*! Mirror network interface protocol type */
    uint16_t mirror_proto;

    /*! Out band data offset */
    uint16_t oob_data_offset;

    /*! Out band data size */
    uint16_t oob_data_size;

    /*! Packet data offset */
    uint16_t pkt_data_offset;

    /*! Packet data size */
    uint16_t pkt_data_size;

    /*! Filtering data */
    union {
        uint8_t b[NGKNET_FILTER_BYTES_MAX];
        uint32_t w[NGKNET_FILTER_WORDS_MAX];
    } data;

    /*! Filtering mask */
    union {
        uint8_t b[NGKNET_FILTER_BYTES_MAX];
        uint32_t w[NGKNET_FILTER_WORDS_MAX];
    } mask;
} ngknet_filter_t;

/*!
 * \brief Device configure structure.
 */
typedef struct ngknet_dev_cfg_s {
    /*! Device name */
    char name[NGKNET_DEV_NAME_MAX];

    /*! Device type string */
    char type_str[NGKNET_DEV_NAME_MAX];

    /*! Device ID */
    uint32_t dev_id;

    /*! Reserved */
    uint32_t rsvd;

    /*! Number of groups */
    uint32_t nb_grp;

    /*! Bitmap of groups */
    uint32_t bm_grp;

    /*! Base network interface */
    ngknet_netif_t base_netif;
} ngknet_dev_cfg_t;

/*!
 * \brief Channel configure structure.
 */
typedef struct ngknet_chan_cfg_s {
    /*! Channel number */
    int chan;

    /*! Number of descriptors */
    uint32_t nb_desc;

    /*! Rx buffer size */
    uint32_t rx_buf_size;

    /*! Channel control */
    uint32_t chan_ctrl;
    /*! Packet_byte_swap */
#define NGKNET_PKT_BYTE_SWAP    (1 << 0)
    /*! Non packet_byte_swap */
#define NGKNET_OTH_BYTE_SWAP    (1 << 1)
    /*! Header_byte_swap */
#define NGKNET_HDR_BYTE_SWAP    (1 << 2)

    /*! Rx or Tx */
    int dir;
    /*! Rx channel */
#define NGKNET_RX_CHAN          0
    /*! Tx channel */
#define NGKNET_TX_CHAN          1
} ngknet_chan_cfg_t;

/*!
 * \brief RCPU header structure.
 */
struct ngknet_rcpu_hdr {
    /*! Destination MAC address */
    uint8_t dst_mac[6];

    /*! Source MAC address */
    uint8_t src_mac[6];

    /*! VLAN TPID */
    uint16_t vlan_tpid;

    /*! VLAN TCI */
    uint16_t vlan_tci;

    /*! Ethernet type */
    uint16_t eth_type;

    /*! Packet signature */
    uint16_t pkt_sig;

    /*! Operation code */
    uint8_t op_code;

    /*! Flags */
    uint8_t flags;

    /*! Transaction number */
    uint16_t trans_id;

    /*! Packet data length */
    uint16_t data_len;

    /*! Expected data length */
    uint16_t rep_len;

    /*! packet meta data length */
    uint8_t meta_len;

    /*! Transmission queue number */
    uint8_t queue_id;

    /*! Reserved must be 0 */
    uint16_t reserved;
};

/*! RCPU Rx operation */
#define RCPU_OPCODE_RX          0x10
/*! RCPU Tx operation */
#define RCPU_OPCODE_TX          0x20

/*! RCPU purge flag */
#define RCPU_FLAG_PURGE         (1 << 0)
/*! RCPU pause flag */
#define RCPU_FLAG_PAUSE         (1 << 1)
/*! RCPU modhdr flag */
#define RCPU_FLAG_MODHDR        (1 << 2)
/*! RCPU bind queue flag */
#define RCPU_FLAG_BIND_QUE      (1 << 3)

#endif /* NGKNET_DEV_H */

