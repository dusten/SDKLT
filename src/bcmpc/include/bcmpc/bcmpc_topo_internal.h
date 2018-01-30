/*! \file bcmpc_topo_internal.h
 *
 * BCMPC TOPO Internal Functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
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

#ifndef BCMPC_TOPO_INTERNAL_H
#define BCMPC_TOPO_INTERNAL_H

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pm_drv.h>


/*!
 * \brief PM operating mode validation function pointer type.
 *
 * This function is used to validate the chip-specific PM operating mode if any.
 * The generic PM-spcific mode is handled by the PM driver, \ref bcmpc_pm_drv_t.
 *
 * Take PM4x10 for example, it supports the port with 1, 2 or 4 lanes generally,
 * while in BCM56960, we only allow 1-lane port according to the chip design.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_mode PM mode.
 *
 * \retval SHR_E_NONE The \c pm_mode is valid.
 * \retval SHR_E_FAIL The \c pm_mode is invalid.
 */
typedef int (*bcmpc_topo_pm_mode_validate_f)(int unit,
                                             bcmpc_pm_mode_t *pm_mode);

/*!
 * \brief Port mode validation function pointer type.
 *
 * This function is used to validate the chip-specific port operating mode if
 * any. The generic PM-specific port operating mode is handled by the PM driver,
 * \ref bcmpc_pm_drv_t.
 *
 * Take PM4x10 for example, the 1-lane port supports 1G, 5G and 10G generally,
 * while in BCM56960, we only allow 10G according to the chip design.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] port_mode Port mode.
 *
 * \retval SHR_E_NONE The \c port_mode is valid.
 * \retval SHR_E_FAIL The \c port_mode is invalid.
 */
typedef int (*bcmpc_topo_port_mode_validate_f)(int unit, bcmpc_pport_t pport,
                                               bcmpc_port_opmode_t port_mode);

/*!
 * \brief Port macro type structure
 *
 * This data structure defines the information for a port macro type.
 *
 * The number of lanes (\c num_lanes) and ports (\c num_ports) are in physical
 * view. For example,
 * TSCE has 4 physical lanes and occupies 4 physical ports.
 * QTC/QSGMII has 4 lanes and occupies 16 physical ports.
 *
 * For an aggregated PM, the number of aggregated entities (\c num_aggr) is
 * required.
 * An aggregated PM means the PM has more than one sub-cores, and each core
 * could work independently as several individual units or together as one unit.
 * For example, PM12x10 could either work as 3 independent units (i.e. PM4x10)
 * or aggregating all resources to become one control unit. For this case, we
 * would set \c num_aggr = 3.
 *
 * We suppose that each sub-unit is identical, i.e. the lanes and ports of
 * an aggregated PM will be equally distributed to each sub-unit.
 */
typedef struct bcmpc_topo_type_s {

    /*! PortMacro type name. */
    const char *name;

    /*! The number of physical lanes. */
    int num_lanes;

    /*! The number of physical ports. */
    int num_ports;

    /*! The number of aggregated entities. */
    int num_aggr;

    /*! PM driver. */
    bcmpc_pm_drv_t *pm_drv;

    /*! Chip-specific PM mode validation function */
    bcmpc_topo_pm_mode_validate_f pm_mode_validate;

    /*! Chip-specific port mode validation function */
    bcmpc_topo_port_mode_validate_f port_mode_validate;

} bcmpc_topo_type_t;


/*!
 * \brief Port macro instance structure
 *
 * This data structure defines the information for a port macro instance.
 *
 * The base port means the first physical switch port number of the PM.
 * For example, a PM is associated with physical port 2, 3, 4 and 5, the
 * \c base_port should be set to 2.
 *
 * The invalid physical PM port bit map, \c invalid_pbmp, indicates which
 * physical PM ports are not associated with a physical device port.
 * By default all physical PM ports are valid i.e. invalid_pbmp is 0.
 */
typedef struct bcmpc_topo_s {

    /*! PortMacro type ID. */
    int tid;

    /*! Base physical port number. */
    bcmpc_pport_t base_pport;

    /*! Invalid physical PM port bit map. */
    SHR_BITDCLNAME(invalid_pbmp, BCMPC_NUM_PPORTS_PER_PM_MAX);

} bcmpc_topo_t;

/*!
 * \brief Initalize PM type data structure.
 *
 * \param [in] tinfo Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_topo_type_t_init(bcmpc_topo_type_t *tinfo);


/*!
 * \brief Create a port macro type.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity to create. The value should be unique.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_create(int unit, int tid);


/*!
 * \brief Destroy a port macro type.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity to destroy.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_destroy(int unit, int tid);


/*!
 * \brief Set port macro type information.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity to set.
 * \param [in] tinfo PM type info.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_set(int unit, int tid, bcmpc_topo_type_t *tinfo);


/*!
 * \brief Get port macro type information.
 *
 * \param [in] unit Unit number.
 * \param [in] tid type identity to get
 * \param [out] tinfo type info
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_get(int unit, int tid, bcmpc_topo_type_t *tinfo);


/*!
 * \brief Callback function for type traversing.
 *
 * The traversal function to invoke on each created type.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity.
 * \param [in] tinfo PM type info.
 * \param [in] cookie User data for this traverse function.
 */
typedef int (*bcmpc_topo_type_traverse_cb_f)(int unit, int tid,
                                             bcmpc_topo_type_t *tinfo,
                                             void *cookie);


/*!
 * \brief Traverse created types.
 *
 * Use the provided traversal function and cookie on each type.
 *
 * \param [in] unit Unit number.
 * \param [in] cb_func Callback function to invoke.
 * \param [in] cookie User data for this traverse function.
 */
extern int
bcmpc_topo_type_traverse(int unit, bcmpc_topo_type_traverse_cb_f cb_func,
                         void *cookie);


/*!
 * \brief Initalize PM instance data structure.
 *
 * \param [in] pm_info Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_topo_t_init(bcmpc_topo_t *pm_info);


/*!
 * \brief Create a port macro instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity to create. The value should be unique.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_create(int unit, int pm_id);


/*!
 * \brief Destroy a port macro instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity to destroy.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_destroy(int unit, int pm_id);


/*!
 * \brief Set port macro instance information.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity to set.
 * \param [in] pm_info PM info.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_set(int unit, int pm_id, bcmpc_topo_t *pm_info);


/*!
 * \brief Get port macro instance information.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity to get.
 * \param [out] pm_info PM info.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_get(int unit, int pm_id, bcmpc_topo_t *pm_info);


/*!
 * \brief Callback function for instance traversal.
 *
 * The traversal function to invoke on each created instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity.
 * \param [in] pm_info PM info.
 * \param [in] cookie User data for this traverse function.
 */
typedef int (*bcmpc_topo_traverse_cb_f)(int unit, int pm_id,
                                        bcmpc_topo_t *pm_info,
                                        void *cookie);


/*!
 * \brief Traverse created instances.
 *
 * Use the provided traversal function and cookie on each instance.
 *
 * \param [in] unit Unit number.
 * \param [in] cb_func Callback function to invoke.
 * \param [in] cookie User data for this traverse function.
 */
extern int
bcmpc_topo_traverse(int unit, bcmpc_topo_traverse_cb_f cb_func,
                    void *cookie);

/*!
 * \brief Get port macro instance ID for the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The base physical port number of a logical port.
 * \param [out] pm_id Instance ID.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_id_get(int unit, bcmpc_pport_t pport, int *pm_id);

#endif /* BCMPC_TOPO_INTERNAL_H */
