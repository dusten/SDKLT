/*! \file bcmpkt_unet.h
 *
 * Interfaces for NET (BCMPKT TX and TX) management access.
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

#ifndef BCMPKT_UNET_H
#define BCMPKT_UNET_H

#include <bcmpkt/bcmpkt_dev.h>
#include <shr/shr_pb.h>

/*!
 * UNET driver types.
 * BCMPKT_DEV_DRV_T_AUTO type is for attach function ONLY. When it's input type,
 * Packet I/O module will select an appropriate UNET driver.
 */
typedef enum bcmpkt_unet_drv_types_e {
    BCMPKT_UNET_DRV_T_NONE = BCMPKT_DEV_DRV_T_NONE, /*! No UNET driver. */
    BCMPKT_UNET_DRV_T_AUTO = BCMPKT_DEV_DRV_T_AUTO, /*! Automatically select driver. */
    BCMPKT_UNET_DRV_T_TPKT = BCMPKT_DEV_DRV_T_KNET, /*! Packet_mmap UNET. */
    BCMPKT_UNET_DRV_T_RAWSOCK, /*! Raw socket UNET. */
    BCMPKT_UNET_DRV_T_COUNT /*! Must be end. */
} bcmpkt_unet_drv_types_t;

/*!
 * \brief UNET module initial parameters.
 */
typedef struct bcmpkt_unet_init_s {

    /*! Reserved for future. */
    uint32_t    flags;

    /*! UNET device name. */
    char        name[BCMPKT_DEV_NAME_MAX];

} bcmpkt_unet_init_t;

/*!
 * \brief UNET module initial function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] init UNET module initial handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Check parameters failed.
 */
typedef int (*bcmpkt_unet_init_f) (int unit, const bcmpkt_unet_init_t *init);

/*!
 * \brief UNET module clean up function.
 *
 * This function is to be called for UNET module clean up.
 * - Destroy all existed UNETs on the device network interfaces;
 * - Free UNET module resource;
 *
 * \param [in] unit Switch unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Cleanup failed.
 */
typedef int (*bcmpkt_unet_cleanup_f) (int unit);

/*!
 * \brief UNET Create function.
 *
 * Create UNET onto a netif.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 * \retval SHR_E_EXISTS UNET exists on the network interface.
 * \retval SHR_E_FAIL Other failure.
 */
typedef int (*bcmpkt_unet_create_f)(int unit, int netif_id);

/*!
 * \brief UNET destroy function.
 *
 * Destory UNET from a netif.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 */
typedef int (*bcmpkt_unet_destroy_f)(int unit, int netif_id);

/*!
 * \brief UNET status get function.
 *
 * Get UNET status information. Return 1 if UNET was created on the netif.
 * Otherwise return 0.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval 1 Exists.
 * \retval 0 Not exists.
 */
typedef int (*bcmpkt_unet_exists_f)(int unit, int netif_id);

/*!
 * \brief Dump UNET statistics dump into \c pb.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support UNET.
 */
typedef int (*bcmpkt_unet_stats_dump_f) (int unit, int netif_id, shr_pb_t *pb);

/*!
 * \brief UNET statistics clear function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support UNET.
 */
typedef int (*bcmpkt_unet_stats_clear_f) (int unit, int netif_id);

/*!
 * \brief UNET vectors.
 */
typedef struct bcmpkt_unet_s {

    /*! Inited flag: 0 - uninitialized 1 - initialized. */
    int inited;

    /*! Driver name, such as "TPacket". */
    char driver_name[128];

    /*! UNET driver type. */
    bcmpkt_unet_drv_types_t driver_type;

    /*! UNET initial function. */
    bcmpkt_unet_init_f init;

    /*! UNET cleanup function */
    bcmpkt_unet_cleanup_f cleanup;

    /*! Create UNET onto a netif. */
    bcmpkt_unet_create_f create;

    /*! Destroy UNET from a netif. */
    bcmpkt_unet_destroy_f destroy;

    /*! Get the status if the netif supports UNET. */
    bcmpkt_unet_exists_f exists;

    /*! Dump a UNET's statistics. */
    bcmpkt_unet_stats_dump_f stats_dump;

    /*! Clear a UNET's statistics. */
    bcmpkt_unet_stats_clear_f stats_clear;

} bcmpkt_unet_t;

/*!
 * \brief UNET driver register function.
 *
 * This function is used for registering a low level UNET driver.
 * For each device, the active UNET driver is set by \ref bcmpkt_unet_drv_attach.
 *
 * \param [in] unet_drv UNET driver handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_CONFIG Driver configuration mistake.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_unet_drv_register(bcmpkt_unet_t *unet_drv);

/*!
 * \brief Unregister a UNET driver.
 *
 * \param [in] type UNET driver type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid type.
 * \retval SHR_E_BUSY The driver is in using.
 */
extern int
bcmpkt_unet_drv_unregister(bcmpkt_unet_drv_types_t type);

/*!
 * \brief Attach a UNET drivre onto a device.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] type UNET driver type.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid type.
 * \retval SHR_E_EXISTS Driver exists.
 * \retval SHR_E_FAIL Attach failed.
 */
extern int
bcmpkt_unet_drv_attach(int unit, bcmpkt_unet_drv_types_t type);

/*!
 * \brief Detach UNET driver from a device.
 *
 * \param [in] unit Switch unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 */
extern int
bcmpkt_unet_drv_detach(int unit);

/*!
 * \brief Get UNET driver type.
 *
 * \param [in] unit Switch unit number.
 * \param [out] type UNET driver type.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_unet_drv_type_get(int unit, bcmpkt_unet_drv_types_t *type);

/*!
 * \brief UNET module initial function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] init UNET module initial handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_unet_init(int unit, const bcmpkt_unet_init_t *init);

/*!
 * \brief UNET module clean up function.
 *
 * This function is to be called for UNET module clean up.
 * - Destroy all existed UNETs on the device network interfaces;
 * - Free UNET module resource;
 *
 * \param [in] unit Switch unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_unet_cleanup(int unit);

/*!
 * \brief UNET Create function.
 *
 * Create UNET onto a netif.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 * \retval SHR_E_EXISTS UNET exists on the network interface.
 * \retval SHR_E_FAIL Other failure.
 */
extern int
bcmpkt_unet_create(int unit, int netif_id);

/*!
 * \brief UNET destroy function.
 *
 * Destory UNET from a netif.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 */
extern int
bcmpkt_unet_destroy(int unit, int netif_id);

/*!
 * \brief Dump UNET statistics into \c pb.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support UNET.
 */
extern int
bcmpkt_unet_stats_dump(int unit, int netif_id, shr_pb_t *pb);

/*!
 * \brief UNET statistics clear function.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Invalid netif_id.
 * \retval SHR_E_UNAVAIL Not support UNET.
 */
extern int
bcmpkt_unet_stats_clear(int unit, int netif_id);

#endif /* BCMPKT_UNET_H */
