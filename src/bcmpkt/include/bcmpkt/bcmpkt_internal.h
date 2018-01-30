/*! \file bcmpkt_internal.h
 *
 * Internal common head file.
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

#ifndef BCMPKT_INTERNAL_H
#define BCMPKT_INTERNAL_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpkt/bcmpkt_net.h>

/*! Get a field from a PMD buffer. */
typedef uint32_t (*bcmpkt_field_get_f)(uint32_t *data);

/*! Set a field within a PMD buffer. */
typedef void (*bcmpkt_field_set_f)(uint32_t *data, uint32_t val);

/*! Get a complex field pointer or other attributions. */
typedef uint32_t (*bcmpkt_ifield_get_f)(uint32_t *data, uint32_t **addr);

/*!
 * \brief RX callback information structure.
 */
typedef struct bcmpkt_rx_cb_info_s {

    /*! Next callback info handle. */
    struct bcmpkt_rx_cb_info_s *cb_next;

    /*! Callback flags. */
    uint32_t flags;

    /*! Callback function. */
    bcmpkt_rx_cb_f cb_func;

    /*! Callback application contex. */
    void *cb_data;

    /*! True: Pending in callback unregistering state. */
    bool cb_pending;

} bcmpkt_rx_cb_info_t;

/*!
 * \brief Packet metadata information structure.
 */
typedef struct bcmpkt_pmd_view_info_s {

    /*! View type list. */
    shr_enum_map_t *view_types;

    /*!
     * Each field's view code.
     * -2 means unavailable field.
     * -1 means common field.
     * others are correspondent view codes defined in view types.
     */
    int *view_infos;

    /*! View type get function. */
    bcmpkt_field_get_f view_type_get;

} bcmpkt_pmd_view_info_t;

/*!
 * \brief Check if a registered device driver is being actively used.
 *
 * \param [in] type Device driver type.
 *
 * \retval 1 Some device is using the driver.
 * \retval 0 No device is using the driver.
 */
extern int
bcmpkt_dev_drv_inuse(bcmpkt_dev_drv_types_t type);

/*!
 * \brief Check if a registered UNET driver is being actively used
 *
 * \param [in] type UNET driver type.
 *
 * \retval 1 Some device is using the driver.
 * \retval 0 No device is using the driver.
 */
extern int
bcmpkt_unet_drv_inuse(bcmpkt_unet_drv_types_t type);

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
extern int
bcmpkt_unet_exists(int unit, int netif_id);

/*!
 * \brief Get device ID.
 *
 * \param [in] unit Unit number.
 * \param [out] id Device ID.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input len is too small.
 * \retval SHR_E_NOT_FOUND Devuce name not found.
 */
extern int
bcmpkt_dev_id_get(int unit, uint32_t *id);

/*!
 * \brief Attach CNET device driver.
 *
 * CNET driver will be attached when call bcmpkt_dev_drv_attach() function
 * automatically. Not open to user now.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_CONFIG Invalid driver.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_EXISTS Driver registered already.
 */
extern int
bcmpkt_dev_drv_cnet_attach(void);

/*!
 * \brief Detach CNET device driver.
 *
 * Not use now. Leave it for future.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid type.
 * \retval SHR_E_BUSY The driver is in using.
 */
extern int
bcmpkt_dev_drv_cnet_detach(void);

/*!
 * \brief Get RCPU header's configuration handle.
 *
 * \param [in] unit Switch unit number.
 *
 * \retval RCPU header's configuration handle.
 */
extern bcmpkt_rcpu_hdr_t *
bcmpkt_rcpu_hdr(int unit);

/*!
 * \brief Dump data into \c pb.
 *
 * \param [in,out] pb Print buffer handle.
 * \param [in] data Data to be printed.
 * \param [in] size print bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid type.
 */
extern int
bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size);

/*!
 * \brief Dump data buffer into \c pb.
 *
 * \param [in,out] pb Print buffer handle.
 * \param [in] dbuf Data buffer to be printed.
 *
 * \retval None.
 */
extern void
bcmpkt_data_buf_dump(bcmpkt_data_buf_t *dbuf, shr_pb_t *pb);

#endif /* BCMPKT_INTERNAL_H */
