/*! \file bcma_bcmpktcmd_internal.h
 *
 * Definitions intended for bcmpktcmd internal use only.
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

#ifndef BCMA_BCMPKTCMD_INTERNAL_H
#define BCMA_BCMPKTCMD_INTERNAL_H

#include <bcma/cli/bcma_cli.h>
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_pb.h>
#include <sal/sal_time.h>
#include <bcmpkt/bcmpkt_net.h>

/*! FIXME: change to the number from config file */
#define BCMA_PORT_MAP_PORT_MAX  255

/*! Maximum default packet size. */
#define MAX_FRAME_SIZE_DEF      (1536)

/*! Default netif MAC address. */
#define BCMA_BCMPKT_DEF_NETIF_MAC {0x00, 0xbc, 0x00, 0x00, 0x00, 0x00}

#define ENET_TYPE_NONE          0
#define ENET_TYPE_ETHERNET_II   1
#define ENET_TYPE_8023_RAW      2
#define ENET_TYPE_SNAP          3

#define ENET_MAC_SIZE           6
#define ENET_FCS_SIZE           4
#define ENET_MIN_PKT_SIZE       60 /* Does not include CRC */
#define ENET_TPID               0x8100
#define ENET_ETHERTYPE          0xFFFF
#define ENET_TAG_SIZE           4

#define BCMPKT_TXPMD_START_IHEADER        2
#define BCMPKT_TXPMD_HEADER_TYPE_FROM_CPU 1

#define BCMPKT_TXPMD_START_HIGIG  3

#define BCMPKT_LBHDR_START_CODE 0xFB

/* Show Packet Data in Packet Watcher */
#define WATCHER_DEBUG_SPD       (0x1 << 0)
/* Loopback Packet Data to TX in Packet Watcher */
#define WATCHER_DEBUG_LD        (0x1 << 1)
/* Show RX packet data Rate in Packet Watcher */
#define WATCHER_DEBUG_SRR       (0x1 << 2)
/* Loopback Packet with time */
#define WATCHER_DEBUG_LDT       (0x1 << 3)

/* CLI parameter not configured flag */
#define PX_NOT_CONFIGURED       -1

#define NETIF_DEFID             1
#define RX_DMA_CHAN_DEFID       1

#define BCMPKT_API_HANDLER_CHECK(_h, _ls) \
    if (_h == NULL || _h->inited != 1) {\
        LOG_ERROR(BSL_LS_APPL_PKTDEV,\
                  (BSL_META("%s handler was not found!\n"),\
                    #_h));\
        return -1;\
    }

typedef struct bcma_txpmd_s {
    bcmdrd_dev_type_t dev_type;
    uint32_t *data;
} bcma_txpmd_t;

typedef struct bcma_lbhdr_s {
    bcmdrd_dev_type_t dev_type;
    uint32_t *data;
} bcma_lbhdr_t;

typedef struct higig_symbl_info_s {
    struct higig_symbl_info_s *next;

   int unit;
   uint32_t sflags; /*! save symbol flags*/
   char *name;
   uint16_t minbit;
   uint16_t maxbit;
} higig_symbl_info_t;

typedef struct bcma_pbmp_s {
    SHR_BITDCLNAME(pbits, BCMA_PORT_MAP_PORT_MAX);
} bcma_pbmp_t;

/*!
 * Packet generation parameters.
 * len is used for clear file/data (len > 64).
 */
typedef struct bcma_bcmpkt_pktgen_cfg_s {
    uint32_t pkttype;    /*! NONE, ETH2, 802.3RAW, SNAP */
    uint32_t len;        /* packet length */
    uint16_t ethertype;  /* EtherType */

    uint16_t tpid;
    bool untagged;
    uint8_t dei;
    uint8_t pcp;
    uint16_t vlan;        /* VLAN ID */

    uint32_t pattern;    /* Specific 32-bit data pattern */
    uint32_t pat_inc;    /*Value by which each word of the data
                         pattern is incremented*/
    bool pat_random; /* use random pattern */
    bool per_port_smac; /*different source mac for each source port*/
    shr_mac_t smac;       /* source MAC */
    uint32_t smac_inc;   /* source MAC increment value */

    shr_mac_t dmac;       /* destination MAC */
    uint32_t dmac_inc;   /* destination MAC increment value */

} bcma_bcmpkt_pktgen_cfg_t;

typedef struct watcher_data_s {
    struct watcher_data_s *next;
    int netif_id;
    uint32_t debug_mode;
    /*! For RX rate debug. */
    uint32_t rx_packets;
    sal_usecs_t start_time;
} watcher_data_t;

/*!
 * \brief Packet watcher function.
 *
 * Packet watcher.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network device index.
 * \param [in] packet Packet structure.
 * \param [in] cookie Not used here.
 *
 * \retval SHR_E_NONE success
 * \retval SHR_E_UNIT Invalid unit number
 * \retval SHR_E_PARAM A parameter was invalid
 */
extern int
bcma_bcmpkt_watcher(int unit, int netif_id, bcmpkt_packet_t *packet,
                    void *cookie);

extern int
bcma_bcmpkt_parse_data_add(void);

extern int
bcma_bcmpkt_load_data_from_istr(char *istr, uint8_t *buf, uint32_t buf_size,
                                uint32_t *data_size);

extern void
bcma_bcmpkt_macaddr_inc(shr_mac_t macaddr, int amount);

extern int
bcma_bcmpkt_packet_payload_fill(bcma_bcmpkt_pktgen_cfg_t *cfg,
                                bcmpkt_data_buf_t *dbuf);

extern int
bcma_bcmpkt_packet_generate(bcma_bcmpkt_pktgen_cfg_t *cfg,
                            bcmpkt_data_buf_t *dbuf);

extern int
bcma_bcmpkt_load_data_from_file(char *fname, bcmpkt_data_buf_t *dbuf);

extern int
bcma_bcmpkt_lmatch_check(const char *dst, const char *src, int size);

extern void
bcma_bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size);

extern void
bcma_bcmpktcmd_watcher_destroy(int unit, int netif_id);

extern int
bcma_bcmpkt_txpmd_init(uint32_t dev_type, uint32_t *txpmd);

extern int
bcma_bcmpkt_lbhdr_init(uint32_t dev_type, uint32_t *lbhdr);

extern void
bcma_bcmpkt_flag_set( uint32_t  *flags, uint32_t this_flag, int do_set);

extern bool
bcma_bcmpkt_flag_status_get(const uint32_t flags, uint32_t this_flag);

extern int
bcma_bcmpkt_netif_defid_get(int unit);

extern int
bcma_bcmpkt_netif_defid_set(int unit, int netif_id);

#endif /* BCMA_BCMPKTCMD_INTERNAL_H */
