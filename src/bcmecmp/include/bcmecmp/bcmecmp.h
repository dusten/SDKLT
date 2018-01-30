/*! \file bcmecmp.h
 *
 * Top-level ECMP device functions.
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

#ifndef BCMECMP_H
#define BCMECMP_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>

/*!
 * \brief Structure for ECMP physical table operation.
 *
 * This structure is used to supply PTM API attributes used during ECMP logical
 * table entry Read/Write operations to hardware/physical tables.
 */
typedef struct bcmecmp_pt_op_info_s {

    /*! Device Resource Database Symbol ID. */
    bcmdrd_sid_t drd_sid;

    /*! Request Logical Table Source ID information. */
    bcmltd_sid_t req_lt_sid;

    /*! Response Logical Table Source ID information. */
    bcmltd_sid_t rsp_lt_sid;

    /*! Logical table operation transaction identifier. */
    uint32_t trans_id;

    /*! Request PTM API option flags. */
    uint32_t req_flags;

    /*! Response PTM API option flags. */
    uint32_t rsp_flags;

    /*! PTM operation type. */
    bcmptm_op_type_t op;

    /*! Physical Table Entry Size in number of words. */
    size_t wsize;

    /*! Enable DMA. */
    bool dma;

    /*! Total number of entries for DMA (SLAM/write or TDMA/read) operation. */
    uint32_t ecount;

    /*!
     * Dynamic address information of the physical table.
     * Physical Table Entry Hardware Index and Pipe Instances to Install Info.
     */
    bcmbd_pt_dyn_info_t dyn_info;
} bcmecmp_pt_op_info_t;

/*!
 * \brief Structure for ECMP device specific function list.
 */
typedef struct bcmecmp_drv_s {

    /*! Device type. */
    bcmdrd_dev_type_t dev_type;

    /*! ECMP device specific data initialization function. */
    int (*info_init)(int unit, bool warm);

    /*! ECMP device specific data cleanup function. */
    int (*info_cleanup)(int unit);

    /*! Configures device ECMP resolution mode. */
    int (*mode_config)(int unit);

    /*! ECMP group install in hardware device function. */
    int (*grp_ins)(int unit, void *info);

    /*! ECMP group delete from hardware device function. */
    int (*grp_del)(int unit, void *info);

    /*! ECMP group get from hardware device function. */
    int (*grp_get)(int unit, void *info);

    /*! Function installs a RH-ECMP group entry into hardware tables. */
    int (*rh_grp_ins)(int unit, void *info);

    /*! Function deletes a RH-ECMP group entry from hardware tables. */
    int (*rh_grp_del)(int unit, void *info);
} bcmecmp_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_ecmp_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmecmp_attach(int unit);

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_ecmp_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated by \ref bcmecmp_attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmecmp_detach(int unit);

/*!
 * \brief Get pointer to ECMP base driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern bcmecmp_drv_t *
bcmecmp_drv_get(int unit);

#endif /* BCMECMP_H */
