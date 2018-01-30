/*! \file bcmpc_drv.h
 *
 * BCMPC Driver Object.
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BCMBD) module.
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

#ifndef BCMPC_DRV_H
#define BCMPC_DRV_H

#include <sal/sal_types.h>
#include <phymod/phymod.h>
#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_tm.h>

/*!
 * \brief BCMPC user access data for PHY access bus.
 *
 * This structure is used as \c user_acc of \c phymod_access_t.
 *
 * This structure stores the extra data according to the host CPU interface to
 * construct the hardware dependent address.
 */
typedef struct bcmpc_phy_access_data_s {

    /*! Unit Number. */
    int unit;

} bcmpc_phy_access_data_t;


/*!
 * \brief BCMPC user access data for PMAC access bus.
 *
 * This structure is used as \c user_acc of \c bcmpmac_access_t.
 *
 * PMAC address only contains the information of register offset, register type
 * and access type. For CMICM/CMICD S-Channel operations, besides PMAC address,
 * it requires unit and block ID information as well.
 *
 * This structure stores the extra data according to the host CPU interface to
 * construct the hardware dependent address.
 */
typedef struct bcmpc_pmac_access_data_s {

    /*! Unit ID */
    int unit;

    /*! Block ID */
    int blk_id;

} bcmpc_pmac_access_data_t;

/*!
 * \brief BCMPC configuration profile.
 *
 * Some port configurations e.g. port up/down are device-specific and are
 * complex in the sense that they involve access to the ingress/egress pipelines
 * , the MMU, the port block, the MAC and the SerDes.
 *
 * This structure is used to define the chip specific programming sequence for
 * some events e.g. port up/down.
 *
 * The entire programming sequence is defined as an array of operations \c ops.
 */
typedef struct bcmpc_cfg_profile_s {

    /*! The count of operations. */
    int op_cnt;

    /*! Operations array. */
    bcmpc_operation_t *ops;

} bcmpc_cfg_profile_t;



/*******************************************************************************
 * PC driver functions
 */

/*!
 * \name PC driver functions
 *
 * The function prototypes for \ref bcmpc_drv_t.
 */

/*! \{ */

/*!
 * \brief Device-specific topology initialization.
 *
 * This function will be called when SYSM starts the PC component. It is used to
 * initialize the chip-specific topology, i.e. PM types and instances
 * population. It should be a pure software operation for writing the topology
 * information into the database.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_topo_init_f)(int unit);

/*!
 * \brief Device-specific port initialization.
 *
 * This function will be called when SYSM starts the PC component. It is used to
 * initialize the chip-specific port configurations. it could include software
 * and hardware operations, ex. internal ports population and PHY firmware
 * download.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set for warm boot, otherwise cold boot.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_dev_init_f)(int unit, bool warm);

/*!
 * \brief Device-specific cleanup.
 *
 * This function will be called when SYSM stops the PC component. It is used to
 * cleanup the chip-specific software resources.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_dev_cleanup_f)(int unit);

/*!
 * \brief PHY MDIO address retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PHY address of \c pport if no errors, otherwise -1.
 */
typedef int
(*bcmpc_phy_addr_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Port block ID retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The block number/ID of \c pport if no errors, otherwise -1.
 */
typedef int
(*bcmpc_blk_id_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PHY bus driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PHYMOD bus driver of \c pport if no errors, otherwise NULL.
 */
typedef phymod_bus_t *
(*bcmpc_phy_bus_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PMAC bus driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PMAC bus driver of \c pport if no errors, otherwise NULL.
 */
typedef bcmpmac_reg_access_t *
(*bcmpc_pmac_bus_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PHY device driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PHYMOD dispatch type of \c pport if no errors, otherwise -1.
 */
typedef phymod_dispatch_type_t
(*bcmpc_phy_drv_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PMAC device driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PMAC driver of \c pport if no errors, otherwise NULL.
 */
typedef bcmpmac_drv_t *
(*bcmpc_pmac_drv_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Port up programming sequence retrieval function pointer type.
 *
 * To get the port up programming sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The port up profile of \c pport if no errors, otherwise NULL.
 */
typedef bcmpc_cfg_profile_t *
(*bcmpc_port_up_profile_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Port down programming sequence retrieval function pointer type.
 *
 * To get the port down programming sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The port down profile of \c pport if no errors, otherwise NULL.
 */
typedef bcmpc_cfg_profile_t *
(*bcmpc_port_down_profile_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Execute operations of a port configuration operation.
 *
 * All operations except for e.g. the MMU operations are owned by the Port
 * Control framework (BCMPC + BCMPMAC + PHYMOD). The MMU operations are owned by
 * the Traffic Manager (BCMTM) eventually.
 *
 * This is a temporary function for execute the port configuration operation
 * which are not handled by PC. And will be removed when the other module are
 * ready.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op Operation.
 */
typedef int
(*bcmpc_op_exec_f)(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op);

/*!
 * \brief Oversub port bit map get function pointer type.
 *
 * This function is used to get the oversub port bit map.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] lport Logical port number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_oversub_pbmp_get_f)(int unit, bcmpc_pbmp_t *pbmp);

/*! \} */

/*******************************************************************************
 * PC driver object
 */

/*!
 * \brief Port Control driver object
 *
 * Port Control driver is used to provide the chip specific information.
 *
 * The chip specific information are included,
 * 1. The port block id and PHY address.
 * 2. The driver and access bus of PHY/PMAC.
 * 3. The programming sequence for complex port configuration e.g. port up/down.
 * 4. The linkscan mode.
 *
 * Each device should provide its own instance to BCMPC module by \ref
 * bcmpc_drv_set() from the top layer. BCMPC internally will use this interface
 * to get the corresponding information.
 */
typedef struct bcmpc_drv_s {

    /*! Initialize device-specific topology. */
    bcmpc_topo_init_f topology_init;

    /*! Initialize device-specific port settings. */
    bcmpc_dev_init_f dev_init;

    /*! Cleanup device-specific resources. */
    bcmpc_dev_cleanup_f dev_cleanup;

    /*! Get PHY MDIO address. */
    bcmpc_phy_addr_get_f phy_addr_get;

    /*! Get port block ID. */
    bcmpc_blk_id_get_f blk_id_get;

    /*! Get PHY Bus Driver. */
    bcmpc_phy_bus_get_f phy_bus_get;

    /*! Get PMAC Bus Driver. */
    bcmpc_pmac_bus_get_f pmac_bus_get;

    /*! Get PHY Driver. */
    bcmpc_phy_drv_get_f phy_drv_get;

    /*! Get PMAC Driver. */
    bcmpc_pmac_drv_get_f pmac_drv_get;

    /*! Get Port Up Programming sequence. */
    bcmpc_port_up_profile_get_f port_up_prof_get;

    /*! Get Port Down Programming sequence. */
    bcmpc_port_down_profile_get_f port_down_prof_get;

    
    /*! Execute the non-port-block operation. */
    bcmpc_op_exec_f op_exec;

    /*! Get the oversub port bit map. */
    bcmpc_oversub_pbmp_get_f oversub_pbmp_get;

} bcmpc_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_pc_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize PC device driver.
 *
 * Install PC driver functions and initialize device features.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmpc_attach(int unit);

/*!
 * \brief Clean up PC driver.
 *
 * Release any resources allocated by \ref bcmpc_attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmpc_detach(int unit);

#endif /* BCMPC_DRV_H */
