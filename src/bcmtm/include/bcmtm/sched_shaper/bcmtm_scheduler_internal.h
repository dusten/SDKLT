/*! \file bcmtm_scheduler_internal.h
 *
 * Definitions for Shapers.
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

#ifndef BCMTM_SCHEDULER_INTERNAL_H
#define BCMTM_SCHEDULER_INTERNAL_H

#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>

#define MAX_TM_SCHEDULER_NODE_QUEUE 12

typedef struct bcmtm_scheduler_profile_s {
    /*! Number of unicast queue */
    uint8_t num_ucq;

    /*! Number of multicast queue. */
    uint8_t num_mcq;

    /*! Enable flow control for unicast. */
    uint8_t fc_uc;

    /*! Operational state of the profile node. */
    uint8_t opcode;
}bcmtm_scheduler_profile_t;

typedef struct bcmtm_scheduler_cfg_s {
    /*! Scheduling mode. */
    uint8_t sched_mode;

    /*! Weight of the node. */
    uint32_t weight;

    /*! Scheduler Profile ID. */
    uint8_t profile_id;

    /*! WRR enable for the port. */
    uint8_t wrr_enable;

    /*! Parent node ID. */
    uint8_t parent_node;

    /*! Delete configuration. */
    uint8_t delete_en;
} bcmtm_scheduler_cfg_t;

/*!
 * Device specific function pointer for physical table configuration
 * for port scheduler.
 */
typedef int (*bcmtm_scheduler_port_pt_set_f) (int unit,
        bcmltd_sid_t ltid,
        bcmtm_lport_t lport,
        bcmtm_scheduler_profile_t *profile,
        bcmtm_scheduler_cfg_t *scheduler_cfg);

/*!
 * Device specific function pointer for physical table configuration
 * for non-cpu node schedulers.
 */
typedef int (*bcmtm_scheduler_node_pt_set_f) (int unit,
        bcmtm_lport_t lport,
        uint32_t sched_node_id,
        uint32_t sched_level,
        bcmltd_sid_t ltid,
        bcmtm_scheduler_cfg_t *scheduler_cfg);
/*!
 * Device specific function pointer for physical table configuration
 * for CPU Node schedulers.
 */
typedef int (*bcmtm_scheduler_cpu_node_pt_set_f) (int unit,
        uint32_t sched_node_id,
        uint32_t sched_level,
        bcmltd_sid_t ltid,
        bcmtm_scheduler_cfg_t *scheduler_cfg);

/*!
 * Device specific function pointer for profile configuration
 * for scheduler profiles.
 */
typedef int (*bcmtm_scheduler_profile_pt_set_t) (int unit,
        bcmltd_sid_t ltid,
        uint8_t profile_id,
        bcmtm_scheduler_profile_t *scheduler_profile);

/*!
 * Device specific function pointer for node profile validation.
 */
typedef int (*bcmtm_scheduler_node_profile_validate_t) (int unit,
        int node_id,
        bcmtm_scheduler_profile_t *scheduler_profile);

/*!
 * Device specific function pointer for profile validation.
 */
typedef int (*bcmtm_scheduler_profile_validate_t) (int unit,
        bcmtm_scheduler_profile_t *scheduler_profile);

/*! Device specific funtions. */
typedef struct bcmtm_scheduler_hdlr_s{
    bcmtm_scheduler_port_pt_set_f port_set;
    bcmtm_scheduler_node_pt_set_f node_set;
    bcmtm_scheduler_cpu_node_pt_set_f cpu_node_set;
    bcmtm_scheduler_profile_pt_set_t profile_set;
    bcmtm_scheduler_node_profile_validate_t node_profile_validate;
    bcmtm_scheduler_profile_validate_t profile_validate;
}bcmtm_scheduler_hdlr_t;


/*!
 * \brief Gets the scheduler profile for a given profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Profile ID.
 * \param [out] profile  Scheduler profile.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY Memory not available for creating field list.
 */
extern int
bcmtm_scheduler_profile_get(int unit,
                            uint8_t profile_id,
                            bcmtm_scheduler_profile_t *profile);

/*!
 * \brief Validates the scheduler profile.
 *
 * \param [in] unit Unit number.
 * \param [in] node_id Scheduler node id.
 * \param [in] profile Scheduler profile.
 * \param [out] status Profile status.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
extern int
bcmtm_scheduler_profile_validate(int unit,
                                 int node_id,
                                 bcmtm_scheduler_profile_t *profile,
                                 uint8_t *status);

#endif /* BCMTM_SCHEDULER_INTERNAL_H */

