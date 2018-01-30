/*! \file bcmtm_drv.h
 *
 * BCMTM Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BD) component.
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

#ifndef BCMTM_DRV_H
#define BCMTM_DRV_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtm/ct/bcmtm_ct_internal.h>
#include <bcmtm/bcmtm_internal_tdm.h>

#define BCMTM_PORT_IS_HG2      (1 << 1)
#define BCMTM_PORT_IS_MGMT     (1 << 2)
#define BCMTM_PORT_IS_OVSB     (1 << 3)

typedef struct bcmtm_int_port_info_s {
    int log_port;
    int phy_port;
    int pipe;
    int idb_port;
    int mmu_port;
} bcmtm_int_port_info_t;

typedef struct bcmtm_drv_info_s {

    int frequency;
    int fabric_port_enable;
    int dpr_clock_frequency;

    bcmtm_int_port_info_t cpu_ports[MAX_NUM_CPU_PORTS];
    bcmtm_int_port_info_t lb_ports[MAX_NUM_LB_MGMT_PORTS];
    bcmtm_int_port_info_t mgmt_ports[MAX_NUM_LB_MGMT_PORTS];

    /*! Logical to physical port mapping */
    int port_l2p_mapping[MAX_NUM_PORTS];
    /*! Physical to logica port mapping. */
    int port_p2l_mapping[MAX_NUM_PORTS];
    /*! Logical to mmu port mapping */
    int port_l2m_mapping[MAX_NUM_PORTS];
    /*! Logical to idb port mapping. */
    int port_l2i_mapping[MAX_NUM_PORTS];
    /*! Physical to mmu port mapping */
    int port_p2m_mapping[MAX_NUM_PORTS];
    /*! Logical port to pipe mapping */
    int port_pipe_mapping[MAX_NUM_PORTS];
    /*! Physical port to mmu local mapping */
    int port_p2m_local_mapping[MAX_NUM_PORTS];
    /*! Logical port to mmu chip port number */
    int port_l2m_chip_mapping[MAX_NUM_PORTS];
    /*! Logical Port flags. */
    int port_flags[MAX_NUM_PORTS];
    /*! Number of multicast queue in a logical port. */
    int num_mc_cosq[MAX_NUM_PORTS];
    /*! Number of unicast queue in a logical port. */
    int num_uc_cosq[MAX_NUM_PORTS];
    /*! Multicast base queue numer for logical port. */
    int mc_cosq_base[MAX_NUM_PORTS];
    /*! Unicast base queue numer for logical port. */
    int uc_cosq_base[MAX_NUM_PORTS];
    /*! Number of lanes in the port macro of logical port. */
    int port_num_lanes[MAX_NUM_PORTS];
    /*! Maximum  speed of logical port. */
    int port_speed_max[MAX_NUM_PORTS];
    /*! Initialised port speed for logical port. */
    int port_init_speed[MAX_NUM_PORTS];
    /*! Current port speed for logical port.*/
    int port_speed_cur[MAX_NUM_PORTS];
    /*! Port scheduler state. */
    soc_port_schedule_state_t port_schedule_state;
    uint32_t oobfc_clock_ns;

    _bcmtm_tomahawk_tdm_t tdm;
} bcmtm_drv_info_t;


/*!
 * \name TM driver functions
 *
 * The function prototypes for \ref bcmtm_drv_t.
 */

typedef void
(*bcmtm_drv_pt_sid_rename_f)(int unit,
                             bcmdrd_sid_t in_sid,
                             int in_inst,
                             bcmdrd_sid_t *out_sid,
                             int *instance);
typedef bcmdrd_sid_t
(*bcmtm_drv_pt_sid_uniq_acc_f)(int unit,
                              bcmdrd_sid_t in_sid,
                              int xpe,
                              int pipe);


typedef int
(*bcmtm_drv_chip_func_f)(int unit);


typedef int
(*bcmtm_drv_port_op_f)(int unit, bcmtm_pport_t port);

typedef bcmpc_tm_handler_t *
(*bcmtm_drv_pc_hdl_get_f)(int unit);

typedef int
(*bcmtm_tdm_drv_f)(int unit, bcmtm_tdm_drv_t *drv);

/*!
 * Function pointer for imm initialization.
 */
typedef int
(*bcmtm_drv_imm_init_f)(int unit);

/*******************************************************************************
* TM driver object
 */

/*!
 * \brief TM driver object
 *
 * TM driver is used to provide the chip specific information.
 *
 * The chip specific information are included,
 * 1. The port block id and PHY address.
 * 2. The driver and access bus of PHY/PMAC.
 * 3. The programming sequence for complex port configuration e.g. port up/down.
 * 4. The linkscan mode.
 *
 * Each device should provide its own instance to BCMTM module by \ref
 * bcmtm_drv_set() from the top layer. BCMTM internally will use this interface
 * to get the corresponding information.
 */
typedef struct bcmtm_drv_s {

    bcmtm_drv_chip_func_f dev_init;
    bcmtm_drv_chip_func_f dev_cleanup;

    /* ! Configure traffic manager components. */
    bcmpc_mmu_update_f tm_configure;

    bcmtm_drv_port_op_f port_ing_down;

    bcmtm_drv_port_op_f port_ing_up;

    bcmtm_drv_port_op_f port_mmu_down;

    bcmtm_drv_port_op_f port_mmu_up;

    bcmtm_drv_port_op_f port_egr_down;

    bcmtm_drv_port_op_f port_egr_up;

    bcmtm_tdm_drv_f tdm_drv;

    /*! PT SID rename function */
    bcmtm_drv_pt_sid_rename_f pt_sid_rename;

    bcmtm_drv_pt_sid_uniq_acc_f pt_sid_uniq_acc;

    bcmtm_drv_pc_hdl_get_f pc_hdl_get;

    /*! TM imm init function. */
    bcmtm_drv_imm_init_f imm_init;

} bcmtm_drv_t;


/*!
 * \brief Set the TM driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv TM driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmtm_drv_set(int unit, bcmtm_drv_t *drv);

/*!
 * \brief Get the TM driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv TM driver to get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmtm_drv_get(int unit, bcmtm_drv_t **drv);

/*!
 * \brief Allocates HA memory for device specific driver info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in allocating driver info.
 */
extern int
bcmtm_drv_info_alloc(int unit);

/*!
 * \brief Free the allocated HA memory for device specific driver info.
 *
 * \param[in] unit Unit number.
 */
extern void
bcmtm_drv_info_free(int unit);

/*!
 * \brief Get the address for device specific driver info.
 *
 * \param [in] unit Unit number.
 * \param [out] drv_info Driver information.
 *
 * \retval SHR_E_NONE No error.
 * \retavl SHR_E_UNAVAIL Driver information not available.
 */
extern int
bcmtm_drv_info_get(int unit,
                   bcmtm_drv_info_t **drv_info);

extern bcmdrd_sid_t
bcmtm_pt_sid_unique_xpe_pipe(int unit, bcmdrd_sid_t sid, int xpe, int pipe);

/*!
 * \brief Get number of multicast queue for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] num_mc_q Number of multicast queues for logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_num_mc_cosq_get(int unit,
        bcmtm_lport_t lport,
        int *num_mc_q);

/*!
 * \brief Get number of unicast queue for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \param [out] num_mc_q Number of multicast queues for logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_num_uc_cosq_get(int unit,
        bcmtm_lport_t lport,
        int *num_uc_q);

/*!
 * \brief Verify logical port for cpu port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is CPU port.
 * \retval FALSE logical port is not CPU port.
 */
extern int
bcmtm_port_is_cpu(int unit,
        bcmtm_lport_t lport);

/*!
 * \brief Verify logical port for loopback port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is loopback port.
 * \retval FALSE logical port is not loopback port.
 */
extern int
bcmtm_port_is_lb(int unit, bcmtm_lport_t lport);

/*!
 * \brief Verify logical port for front panel port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is front panel port.
 * \retval FALSE logical port is not front panel port.
 */
extern int
bcmtm_port_is_fp(int unit, bcmtm_lport_t lport);

/*!
 * \brief Verify logical port for management port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is management port.
 * \retval FALSE logical port is not management port.
 */
extern int
bcmtm_port_is_mgmt(int unit, bcmtm_lport_t lport);

/*!
 * \brief Get global mmu port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out]mport MMU global port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_mmu_port_get(int unit,
        bcmtm_lport_t lport,
        int *mport);

/*!
 * \brief Get physical port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] pport Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_phys_port_get(int unit,
        bcmtm_lport_t lport,
        int *pport);

/*!
 * \brief Get mmu chip port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] mmu_chip_port MMU chip port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_mmu_chip_port_get(int unit,
        bcmtm_lport_t lport,
        int *mmu_chip_port);

/*!
 * \brief Get idb port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] idb_port IDB port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_idb_port_get(int unit,
        bcmtm_lport_t lport,
        int *idb_port);

/*!
 * \brief Get maximum speed for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] max_port_speed Maximum port speed for the logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_speed_max_get(int unit,
        bcmtm_lport_t lport,
        int *max_port_speed);

/*!
 * \brief Get the pipe number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] pipe Pipe number for the logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_pipe_get(int unit,
        bcmtm_lport_t lport,
        int *pipe);

/*!
 * \brief Get mmu local port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] mmu_local MMU local port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_mmu_local_port_get(int unit,
        bcmtm_lport_t lport,
        int *mmu_local);

/*!
 * \brief Get multicast queue base number for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] mc_q_base Multicast base queue number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_mc_cosq_base_get(int unit,
        bcmtm_lport_t lport,
        int *mc_q_base);

/*!
 * \brief Get number of unicast queue base number for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] uc_q_base Unicast base queue number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_port_uc_cosq_base_get(int unit,
        bcmtm_lport_t lport,
        int *uc_q_base);


extern int
bcmtm_port_ing_down(int unit, bcmtm_pport_t port);

extern int
bcmtm_port_ing_up(int unit, bcmtm_pport_t port);

extern int
bcmtm_port_mmu_down(int unit, bcmtm_pport_t port);

extern int
bcmtm_port_mmu_up(int unit, bcmtm_pport_t port);

extern int
bcmtm_port_egr_down(int unit, bcmtm_pport_t port);

extern int
bcmtm_port_egr_up(int unit, bcmtm_pport_t port);

/*!
 * \brief Execute operations of a port configuration operation.
 *
 * This function will handle the TM operations for link up/down.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op_name Operation name.
 * \param [in] op_param Operation parameter.
 */
int
bcmtm_port_op_exec(int unit, bcmtm_pport_t pport,
                   char *op_name, uint32_t op_param);

/*!
 * \brief TM modules regestering for IMM.
 *
 * \param [in] unit   Unit Number.
 */
extern int
bcmtm_imm_reg(int unit);

/*!
 * \brief Get the field value from field array for Logical table field ID.
 *
 * \param [in] unit  Unit number.
 * \param [in] fid   Logical table field ID.
 * \param [in] field_array Field array having field id and field_value.
 * \param [out] fval field value for the logical table field ID.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_NOT_FOUND field ID not found in field array.
 */
extern int
bcmtm_fval_get_from_field_array(int unit,
        bcmlrd_fid_t fid,
        const bcmltd_field_t *field_array,
        uint64_t *fval);

#endif /* BCMTM_DRV_H */
