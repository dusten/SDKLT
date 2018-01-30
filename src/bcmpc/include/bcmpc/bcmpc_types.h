/*! \file bcmpc_types.h
 *
 * BCMPC Exported Types.
 *
 * Declaration of the structures, enumerations which are exported by
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

#ifndef BCMPC_TYPES_H
#define BCMPC_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*! Physical device port number, a.k.a. physical port. */
typedef shr_port_t bcmpc_pport_t;

/*! Logical switch port number, a.k.a. logical port. */
typedef shr_port_t bcmpc_lport_t;

/*! MAC address type. */
typedef shr_mac_t bcmpc_mac_t;

/*! Bitmap of ports of a particular type or properties. */
typedef bcmdrd_pbmp_t bcmpc_pbmp_t;

/*! Invalid logical port number. */
#define BCMPC_INVALID_LPORT ((bcmpc_lport_t)-1)

/*! Invalid physical port number. */
#define BCMPC_INVALID_PPORT ((bcmpc_pport_t)-1)

/*! \brief Invalid Profile ID.
 *
 * \c BCMPC_INVALID_PROF_ID is a special value, which is not associated with
 * any real profile. Essentially this means that index \c BCMPC_INVALID_PROF_ID
 * cannot be used in any of the profile tables.
 *
 * If the user does not specify the profile ID during an INSERT, the profile ID
 * will be set to \c BCMPC_INVALID_PROF_ID by default, but the configuration
 * will not be submitted to HW (because it is incomplete).
 */
#define BCMPC_INVALID_PROF_ID (0)

/*! The max number of units per system */
#define BCMPC_NUM_UNITS_MAX BCMDRD_CONFIG_MAX_UNITS

/*! The max number of physical ports per chip */
#define BCMPC_NUM_PPORTS_PER_CHIP_MAX BCMDRD_CONFIG_MAX_PORTS

/*! The maximum number of physical ports per PM */
#define BCMPC_NUM_PPORTS_PER_PM_MAX (16)

/*! The maximum number of lanes per PM */
#define BCMPC_NUM_LANES_PER_PM_MAX (16)

/*! The maximum number of aggreated entities per PM */
#define BCMPC_NUM_AGGR_CORES_PER_PM_MAX (4)

/*! The maximum number of port operating modes per device */
#define BCMPC_NUM_OPMODES_PER_UNIT_MAX (16)

/*! The maximum number of RX DEF TAPs */
#define BCMPC_NUM_RX_DFE_TAPS_MAX (16)

/*!
 * \brief Port Macro operating mode.
 *
 * For some PM types, a physical port may be associated with multiple
 * interfaces. For example, a PM with QTC and GPHY, a physical port can
 * be associated with an SGMII interface (BCMPC_PM_OPMODE_DEFAULT), a QSGMII
 * interface (BCMPC_PM_OPMODE_QSGMII), or a GPHY interface (BCMPC_PM_OPMODE_GPHY
 * ) for that matter, but only one interface at a time. The operating mode is
 * used to define the working mode as well.
 *
 * Note that the order of this enum should be same as PC_PM_OPMODE_T.
 */
typedef enum bcmpc_pm_opmode_e {

    /*! Default port mode. */
    BCMPC_PM_OPMODE_DEFAULT = 0,

    /*! Configure Gigabit core for QSGMII mode. */
    BCMPC_PM_OPMODE_QSGMII,

    /*! Configure Gigabit core to use internal copper PHYs. */
    BCMPC_PM_OPMODE_GPHY,

    /*! Must be the last. */
    BCMPC_PM_OPMODE_COUNT

} bcmpc_pm_opmode_t;

/*!
 * \brief Linkscan modes.
 */
typedef enum bcmpc_ls_mode_e {

    /*! Linkscan is disabled. */
    BCMPC_LS_MODE_DISABLED,

    /*! Linkscan is enabled with software linkscan mode. */
    BCMPC_LS_MODE_SW,

    /*! Linkscan is enabled with other linkscan mode, e.g. HW or FW. */
    BCMPC_LS_MODE_OTHER

} bcmpc_ls_mode_t;

/*!
 * \brief Port macro configuration.
 *
 * This structure determines how a port macro is divided into logical
 * ports. Please refer to the documentation for the specific port
 * macro to see which lane configurations are supported.
 *
 * Note that some port macro contain multiple cores (e.g. PM12X10) in
 * which case the operating mode must be specified for all the cores.
 *
 * Each logical port must also be assigned a maximum speed, which is
 * used by the MMU to reserve packet (cell) buffers.
 *
 * The \c SPEED_MAX array is indexed by the port macro physical port
 * number associated with a logical port. For example for a PM4X10
 * port macro in DUAL mode (2 x 20 Gbps), the array would be assigned
 * like this:
 *
 * \verbatim
 *     SPEED_MAX[0] = 20000
 *     SPEED_MAX[1] = 0
 *     SPEED_MAX[2] = 20000
 *     SPEED_MAX[3] = 0
 *     LANE_MAP[0] = 0x3
 *     LANE_MAP[1] = 0
 *     LANE_MAP[2] = 0xc
 *     LANE_MAP[3] = 0
 * \endverbatim
 */
typedef struct bcmpc_pm_mode_s {

    /*! Port macro operating mode. */
    bcmpc_pm_opmode_t opmode[BCMPC_NUM_AGGR_CORES_PER_PM_MAX];

    /*! PM port maximum speed. */
    uint32_t speed_max[BCMPC_NUM_PPORTS_PER_PM_MAX];

    /*! PM port lane map. */
    uint32_t lane_map[BCMPC_NUM_PPORTS_PER_PM_MAX];

} bcmpc_pm_mode_t;

/*!
 * A special value to indicate the param is don't care to the operation,
 * /ref bcmpc_operation_t.
 */
#define BCMPC_OP_PARAM_NONE ((uint32_t)-1)

/*!
 * \brief BCMPC operation description.
 *
 * This structure is used to specify a driver operation which is used in \ref
 * bcmpc_cfg_profile_t.
 *
 * For a operation, in addition to the name of the operation, it also contains
 * a driver name and an integer parameter for the operation.
 */
typedef struct bcmpc_operation_s {

    /*! Drive name. */
    char *drv;

    /*! Operation name. */
    char *name;

    /*! Operation parameter. */
    uint32_t param;

} bcmpc_operation_t;

#endif /* BCMPC_TYPES_H */
