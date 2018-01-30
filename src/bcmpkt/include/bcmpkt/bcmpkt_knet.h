/*! \file bcmpkt_knet.h
 *
 * KNET access interfaces, including virtual network interface (netif)
 * management and KNET packet filter.
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

#ifndef BCMPKT_KNET_H
#define BCMPKT_KNET_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmpkt/bcmpkt_rxpmd.h>

/*! Maximum chars are allowed in network interface name.  */
#define BCMPKT_NETIF_NAME_MAX           16

/*!
 * Maximum Network interface ID number.
 * Valid ID is from 1 to 63. 0 is reserved.
 */
#define BCMPKT_NETIF_MAX_IDS            64

/*! Check if netif ID number is within the netif supported range.
 * 0 is used for device management.
 */
#define BCMPKT_NETIF_VALID(_n) ((_n > 0) && (_n < BCMPKT_NETIF_MAX_IDS))

/**
 * \name Network interface creation flags.
 * \anchor BCMPKT_NETIF_F_XXX
 */
/*! \{ */
/*!
 * RCPU header encapsulation is used for deliver packet metadata to/from
 * applications.
 * Applications should encapsulate RCPU header when send packets to the netif,
 * and may parser RCPU header when receive packets from the netif. UNET could be
 * created on such netif, which supports RCPU encapsulation, only.
 */
#define BCMPKT_NETIF_F_RCPU_ENCAP       (1 << 0)
/*!
 * Bind this netif with a RX DMA channel. All packets from
 * the DMA channel will be forwarded to this netif.
 */
#define BCMPKT_NETIF_F_BIND_RX_CH       (1 << 1)

/*!
 * Bind this netif with a switch front port for transmition.
 * All packets from this netif will be forwarded to the bound local switch port.
 */
#define BCMPKT_NETIF_F_BIND_TX_PORT     (1 << 2)
/*! \} */

/*!
 * \brief Packet network interface structure.
 *
 * This structure defines a network interface configuration.
 *
 * "id" is allocated by \ref bcmpkt_netif_create_f function.
 *
 * "mac_addr" and "vlan" are the network interface's Layer 2 configuration.
 * "port", "port_encap" and "port_queue" are for binding a switch front port.
 *
 * "dma_chan_id" works for binding a DMA channel with a netif.
 */
typedef struct bcmpkt_netif_s {

    /*! Network interface ID.  */
    int id;

    /*! Creation flag. Refer to \ref BCMPKT_NETIF_F_XXX flags. */
    uint32_t flags;

    /*! MAC address associated with this network interface. */
    shr_mac_t mac_addr;

    /*! VLAN TCI associated with this network interface. */
    uint16_t vlan;

    /*!
    * Bind with a network interface for transmit destination port. All packet
    * sent from the netif will be forwarded to this port directly.
    */
    int port;

    /*!
    * "port_encap" works with "port" to tell the encapsulation type of the port.
    * "ieee", "higig" or "higig2".
    *  Only support "ieee" currently. NULL string is taken as "ieee".
    */
    char port_encap[10];

    /*!
    * "port_queue" works with "port" to tell egress queue number.
    */
    int port_queue;

    /*!
    * This parameter works with \ref BCMPKT_NETIF_F_BIND_RX_CH to bind a RX DMA
    * channel with this netif.
    */
    int dma_chan_id;

    /*!
    * Max packet size is for this netif. Any packet which size is bigger than
    * this setting will be dropped.
    */
    uint32_t max_frame_size;

    /*! Network interface name. */
    char name[BCMPKT_NETIF_NAME_MAX];

} bcmpkt_netif_t;

/*!
 * \brief Network interface traverse callback function type.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif Network interface.
 * \param [in] cb_data Application-provided context for callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_FAIL Failed.
 */
typedef int (*bcmpkt_netif_traverse_cb_f) (int unit,
                                           const bcmpkt_netif_t *netif,
                                           void *cb_data);

/*!
 * \brief Network interface create function.
 *
 * This function is used for creating a new network interface.
 *
 * The bcmpkt_netif_t.id is an output parameter.
 *
 * \param [in] unit Switch unit number.
 * \param [in,out] netif Network interface handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameter failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
typedef int (*bcmpkt_netif_create_f) (int unit, bcmpkt_netif_t *netif);

/*!
 * \brief Network interface get function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface index.
 * \param [out] netif Network interface handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_BADID Invalid network interface ID.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 * \retval SHR_E_NOT_FOUND Not found the entry.
 */
typedef int (*bcmpkt_netif_get_f) (int unit, int netif_id,
                                   bcmpkt_netif_t *netif);

/*!
 * \brief Network interface traverse function.
 *
 * This function is for traversing all network interfaces' configuration.
 * The callback function will be called for each present netif. Refer to
 * \ref bcmpkt_netif_traverse_cb_f for callback function type.
 *
 * \param [in] unit Switch unit number.
 * \param [in] cb_func Network interface traverse callback function.
 * \param [in] cb_data Application-provided context for callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
typedef int (*bcmpkt_netif_traverse_f) (int unit,
                                        bcmpkt_netif_traverse_cb_f cb_func,
                                        void *cb_data);

/*!
 * \brief Network destroy function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface index.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_BADID Invalid network interface ID.
 * \retval SHR_E_FAIL Access driver failed.
 * \retval SHR_E_RESOURCE Not allowed to destroy a UNET netif.
 */
typedef int (*bcmpkt_netif_destroy_f) (int unit, int netif_id);

/**
 * \name KNET filter types.
 * \anchor BCMPKT_FILTER_T_XXX
 */
/*! \{ */
/*! Packet filter for RX direction. */
#define BCMPKT_FILTER_T_RX_PKT    1
/*! \} */

/**
 * \name KNET filter flags.
 * \anchor BCMPKT_FILTER_F_XXX
 */
/*! \{ */
/*! Strip VLAN tag from packets sent to vitual network interfaces. */
#define BCMPKT_FILTER_F_STRIP_TAG 0x00000001
/*! \} */

/**
 * \name KNET filter destination types.
 * \anchor BCMPKT_DEST_T_XXX
 */
/*! \{ */
/*! Null destination (drop packet). */
#define BCMPKT_DEST_T_NULL        0

/*! Send packet to virtual network interface. */
#define BCMPKT_DEST_T_NETIF       1

/*! Send packet to BCM Rx API. */
#define BCMPKT_DEST_T_BCM_RX_API  2

/*! Send packet to kernel  call-back function. */
#define BCMPKT_DEST_T_CALLBACK    3
/*! \} */

/**
 * \name KNET filter match flags.
 * \anchor BCMPKT_FILTER_M_XXX
 */
/*! \{ */
/*! Match raw packet data */
#define BCMPKT_FILTER_M_RAW           0x00000001
/*! Match VLAN ID */
#define BCMPKT_FILTER_M_VLAN          0x00000002
/*! Match local ingress port */
#define BCMPKT_FILTER_M_INGPORT       0x00000004
/*! Match source module port */
#define BCMPKT_FILTER_M_SRC_MODPORT   0x00000008
/*! Match source module ID */
#define BCMPKT_FILTER_M_SRC_MODID     0x00000010
/*! Match copy-to-CPU reason code */
#define BCMPKT_FILTER_M_REASON        0x00000020
/*! Match filter processor rule number */
#define BCMPKT_FILTER_M_FP_RULE       0x00000040
/*! Match error bit */
#define BCMPKT_FILTER_M_ERROR         0x00000080
/*! Match CPU queue (rx queue) */
#define BCMPKT_FILTER_M_CPU_QUEUE     0x00000100
/*! \} */

/*! Filter description maximum size */
#define BCMPKT_FILTER_DESC_MAX    32

/*! Packet raw data maximum size */
#define BCMPKT_FILTER_SIZE_MAX    256

/*! \brief Packet filter structure.
 */
typedef struct bcmpkt_filter_s {
    /*! Filter ID. */
    int id;
    /*! Filter type. Refer to \ref BCMPKT_FILTER_T_XXX. */
    uint32_t type;
    /*! Filter flags. Refer to \ref BCMPKT_FILTER_F_XXX. */
    uint32_t flags;
    /*! Filter priority (0 is highest). */
    uint32_t priority;
    /*! Destination type. Refer to \ref BCMPKT_DEST_T_XXX. */
    uint32_t dest_type;
    /*! Filter destination ID. */
    int dest_id;
    /*!
    * If non-zero this value overrides the default protocol type when
    * matching packet is passed to network stack.
    */
    uint16_t dest_proto;
    /*! Destination type. Refer to \ref BCMPKT_DEST_T_XXX. */
    uint32_t mirror_type;
    /*! Mirror destination ID. */
    int mirror_id;
    /*!
    * If non-zero this value overrides the default protocol type when
    * matching packet is passed to network stack.
    */
    uint16_t mirror_proto;

    /*! Source RX DMA channel to match. */
    int dma_chan;

    /*! Refer to \ref BCMPKT_FILTER_M_XXX. */
    uint32_t match_flags;

    /*! Filter description (optional) */
    char desc[BCMPKT_FILTER_DESC_MAX];
    /*! VLAN ID to match. */
    uint16_t m_vlan;
    /*! Local ingress port to match. */
    int m_ingport;
    /*! Source module port to match. */
    int m_src_modport;
    /*! Source module ID to match. */
    int m_src_modid;
    /*! Source CPU port queue to match. */
    int m_cpu_queue;
    /*! Copy-to-CPU reason to match. */
    bcmpkt_rx_reasons_t m_reason;
    /*! Filter processor rule to match. */
    uint32_t m_fp_rule;
    /*! Size of valid raw data and mask. */
    uint32_t raw_size;
    /*! Raw data to match. */
    uint8_t m_raw_data[BCMPKT_FILTER_SIZE_MAX];
    /*! Raw data mask for match. */
    uint8_t m_raw_mask[BCMPKT_FILTER_SIZE_MAX];
} bcmpkt_filter_t;

/*!
 * \brief Network filter traverse callback function type.
 *
 * \param [in] unit Switch unit number.
 * \param [in] filter KNET filter handle.
 * \param [in] cb_data Application-provided context for callback function.
 *
 * \retval SHR_E_NONE No Error.
 * \retval SHR_E_FAIL Failed.
 */
typedef int (*bcmpkt_filter_traverse_cb_f) (int unit,
                                            const bcmpkt_filter_t *filter,
                                            void *cb_data);

/*!
 * \brief Network filter create function.
 *
 * This function is to be called for creating a new network filter.
 * The filter may be used for dispatching matched packets to a specific
 * destination, and/or mirror those to another destination, or drop those
 * directly.
 *
 * The bcmpkt_filter_t.id is an output parameter.
 *
 * \param [in] unit Switch unit number.
 * \param [in,out] filter KNET filter handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
typedef int (*bcmpkt_filter_create_f) (int unit, bcmpkt_filter_t *filter);

/*!
 * \brief Network filter get function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] filter_id KNET filter ID.
 * \param [out] filter KNET filter handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 * \retval SHR_E_NOT_FOUND Not found the entry.
 */
typedef int (*bcmpkt_filter_get_f) (int unit, int filter_id,
                                    bcmpkt_filter_t *filter);

/*!
 * \brief Network filter traverse function.
 *
 * This function for traversing all network filter configuration.
 *
 * \param [in] unit Switch unit number.
 * \param [in] cb_func KNET filter traverse callback function.
 * \param [in] cb_data Application-provided context for callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
typedef int (*bcmpkt_filter_traverse_f) (int unit,
                                         bcmpkt_filter_traverse_cb_f cb_func,
                                         void *cb_data);

/*!
 * \brief Network filter destroy function.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] filter_id KNET filter ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_FAIL Access driver failed.
 */
typedef int (*bcmpkt_filter_destroy_f) (int unit, int filter_id);

/*!
 * \brief Network interface operation vector.
 */
typedef struct bcmpkt_netif_ops_s {

    /*! Create a netif. */
    bcmpkt_netif_create_f create;

    /*! Get a netif's configuration. */
    bcmpkt_netif_get_f get;

    /*! Traverse netifs. */
    bcmpkt_netif_traverse_f traverse;

    /*! Destroy a netif. */
    bcmpkt_netif_destroy_f destroy;
} bcmpkt_netif_ops_t;

/*!
 * \brief KNET filter operation vector.
 */
typedef struct bcmpkt_filter_ops_s {

    /*! Create a filter. */
    bcmpkt_filter_create_f create;

    /*! Get a filter configuration. */
    bcmpkt_filter_get_f get;

    /*! Traverse filter configurations. */
    bcmpkt_filter_traverse_f traverse;

    /*! Destroy a filter. */
    bcmpkt_filter_destroy_f destroy;

} bcmpkt_filter_ops_t;

/*!
 * \brief KNET vector.
 */
typedef struct bcmpkt_knet_s {
    /*! Inited flag: 0 - uninitialized 1 - initialized. */
    int inited;

    /*! KNET driver name, such as "KNET". */
    char driver_name[128];

     /*! Netif operations vector. */
    bcmpkt_netif_ops_t netif_ops;

    /*! KNET filter operations vector. */
    bcmpkt_filter_ops_t filter_ops;

} bcmpkt_knet_t;

/*!
 * \brief Register KNET operations.
 *
 * Enable KNET operations.
 *
 * \param [in] knet_drv KNET vectors.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_CONFIG Invalid driver.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_knet_drv_register(bcmpkt_knet_t *knet_drv);

/*!
 * \brief Unregister KNET driver.
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY The driver is in using.
 */
extern int
bcmpkt_knet_drv_unregister(void);

/*!
 * \brief Network interface create function.
 *
 * This function is used for creating a new network interface.
 *
 * The bcmpkt_netif_t.id is an output parameter.
 *
 * \param [in] unit Switch unit number.
 * \param [in,out] netif Network interface handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameter failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
extern int
bcmpkt_netif_create(int unit, bcmpkt_netif_t *netif);

/*!
 * \brief Network interface get function.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface index.
 * \param [out] netif Network interface handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_BADID Invalid network interface ID.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 * \retval SHR_E_NOT_FOUND Not found the entry.
 */
extern int
bcmpkt_netif_get(int unit, int netif_id, bcmpkt_netif_t *netif);

/*!
 * \brief Network interface traverse function.
 *
 * This function is for traversing all network interfaces' configuration.
 * The callback function will be called for each present netif. Refer to
 * \ref bcmpkt_netif_traverse_cb_f for callback function type.
 *
 * \param [in] unit Switch unit number.
 * \param [in] cb_func Network interface traverse callback function.
 * \param [in] cb_data Application-provided context for callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
extern int
bcmpkt_netif_traverse(int unit, bcmpkt_netif_traverse_cb_f cb_func,
                      void *cb_data);

/*!
 * \brief Network destroy function.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface index.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_BADID Invalid network interface ID.
 * \retval SHR_E_FAIL Access driver failed.
 * \retval SHR_E_RESOURCE Not allowed to destroy a UNET netif.
 */
extern int
bcmpkt_netif_destroy(int unit, int netif_id);

/*!
 * \brief Network filter create function.
 *
 * This function is to be called for creating a new network filter.
 * The filter may be used for dispatching matched packets to a specific
 * destination, and/or mirror those to another destination, or drop those
 * directly.
 *
 * The bcmpkt_filter_t.id is an output parameter.
 *
 * \param [in] unit Switch unit number.
 * \param [in,out] filter KNET filter handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
extern int
bcmpkt_filter_create(int unit, bcmpkt_filter_t *filter);

/*!
 * \brief Network filter get function.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] filter_id KNET filter ID.
 * \param [out] filter KNET filter handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 * \retval SHR_E_NOT_FOUND Not found the entry.
 */
extern int
bcmpkt_filter_get(int unit, int filter_id, bcmpkt_filter_t *filter);

/*!
 * \brief Network filter traverse function.
 *
 * This function for traversing all network filter configuration.
 *
 * \param [in] unit Switch unit number.
 * \param [in] cb_func KNET filter traverse callback function.
 * \param [in] cb_data Application-provided context for callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Access driver failed.
 */
extern int
bcmpkt_filter_traverse(int unit, bcmpkt_filter_traverse_cb_f cb_func,
                       void *cb_data);

/*!
 * \brief Network filter destroy function.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] filter_id KNET filter ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_CONFIG Network device was not initialed.
 * \retval SHR_E_FAIL Access driver failed.
 */
extern int
bcmpkt_filter_destroy(int unit, int filter_id);

#endif  /* !BCMPKT_KNET_H */
