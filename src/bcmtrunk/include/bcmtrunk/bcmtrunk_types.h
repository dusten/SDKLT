/*! \file bcmtrunk_types.h
 *
 * This file contains data types used by TRUNK custom handler component.
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

#ifndef BCMTRUNK_TYPES_H
#define BCMTRUNK_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmtrunk/generated/trunk_ha.h>


/*! Max trunk groups supported. */
#define BCMTRUNK_MAX_TRUNK        (1024)

/*!
 * Max trunk members supported in normal trunk mode.
 * Max members is fixed as 8 in fast trunk mode.
 */
#define BCMTRUNK_MAX_MEMBERS      (256)

/*! Mim trunk members supported in normal trunk mode. */
#define BCMTRUNK_MIN_MEMBERS      (1)

/*! Max trunk members supported in fast trunk mode. */
#define BCMTRUNK_FAST_MAX_MEMBERS (64)

/*! Max failover port members for a port. */
#define BCMTRUNK_FAILOVER_MAX_MEMBERS  (8)

/*! HA subcomponent id for trunk group. */
#define BCMTRUNK_GRP_SUB_COMP_ID      (1)
/*! HA subcomponent id for trunk fast group. */
#define BCMTRUNK_FAST_GRP_SUB_COMP_ID (2)
/*! HA subcomponent id for trunk failover entry. */
#define BCMTRUNK_FAILOVER_SUB_COMP_ID (3)
/*! HA subcomponent id for trunk member bitmap. */
#define BCMTRUNK_MEMBER_BMP_SUB_COMP_ID (4)


/*! Encap base tunnel structure version. */
#define BCMTRUNK_MEMBER_BMAP_T_VER            1

/*! Encap base tunnel structure signature. */
#define BCMTRUNK_MEMBER_BMAP_T_SIG            0x12261ee5fd9a1111


/*! Trunk Hg LB mode. */
typedef enum bcmtrunk_lb_mode_e {
    /*! Regular hash mode. */
    BCMTRUNK_LB_HASH = 0,

    /*! Random mode. */
    BCMTRUNK_LB_RANDOM = 1,

    /*! Round robin mode. */
    BCMTRUNK_LB_RR = 2,

    BCMTRUNK_LB_INVALID = 4
} bcmtrunk_lb_mode_t;

/*! Bookkeeping info for TRUNK LT. */
typedef struct bcmtrunk_lth_grp_bk_s {
    /*! Trunk group info. */
    bcmtrunk_group_t    *grp;

    /*! Total member bitmap for TRUNK_MEMBER resource. */
    SHR_BITDCL          *mbmp;

    /*! Size of TRUNK_MEMBER table. */
    int                 member_size;
} bcmtrunk_lth_grp_bk_t;

/*!
 * It is used to save parameters from LTM while doing parsing LT fields.
 * It is used for TRUNK LT.
 */
typedef struct bcmtrunk_group_param_s {
    /*! Trunk id. */
    uint32_t      id;

    /*! Trunk id valid. */
    bool          id_valid;

    /*! Trunk load balancing mode(regualr hash, random or roud robin). */
    uint8_t       lb_mode;

    /*! Load balancing valid */
    bool          lb_mode_valid;

    /*! Hash rule tag for member selection. */
    uint8_t       uc_rtag;

    /*! Uc_rtag valid. */
    bool          uc_rtag_valid;

    /*!
     * Max possible members of this trunk assumed by end-users.
     * SDK will reserve TRUNK_MEMBER resource for this trunk group.
     * It can improve performance to processing trunk change, but may
     * waste some TRUNK_MEMBER resources.
     * It is not supported in fast trunk mode.
     * Max members for fast trunk group is fixed as 8.
     */
    uint16_t      uc_max_members;

    /*! Uc_max_members valid. */
    bool          uc_max_members_valid;

    /*! Actual egress member count. */
    uint16_t      uc_cnt;

    /*! Uc_cnt valid. */
    bool          uc_cnt_valid;

    /*! Module ID array of egress members for unicast packets. */
    uint8_t       uc_modid[BCMTRUNK_MAX_MEMBERS];

    /*! Uc_modid valid. */
    bool          uc_modid_valid[BCMTRUNK_MAX_MEMBERS];

    /*! Port ID array of egress members for unicast packets. */
    uint8_t       uc_modport[BCMTRUNK_MAX_MEMBERS];

    /*! Uc_modport valid. */
    bool          uc_modport_valid[BCMTRUNK_MAX_MEMBERS];

    /*! AGM id. */
    uint8_t       uc_agm_id;

    /*! Uc_agm_id valid. */
    bool          uc_agm_id_valid;

    /*! Egress member count for non-unicast packets. */
    uint16_t      nonuc_cnt;

    /*! Nonuc_cnt valid. */
    bool          nonuc_cnt_valid;

    /*! Module ID array of egress members for non-unicast packets. */
    uint8_t       nonuc_modid[BCMTRUNK_MAX_MEMBERS];

    /*! Nonuc_modid valid. */
    bool          nonuc_modid_valid[BCMTRUNK_MAX_MEMBERS];


    /*! Port ID array of egress members for non-unicast packets. */
    uint8_t       nonuc_modport[BCMTRUNK_MAX_MEMBERS];

    /*! Nonuc_modport valid. */
    bool          nonuc_modport_valid[BCMTRUNK_MAX_MEMBERS];

} bcmtrunk_group_param_t;

/*! Bookkeeping info for TRUNK FAST LT. */
typedef struct bcmtrunk_lth_fgrp_bk_s {
    /*! Trunk group info. */
    bcmtrunk_fast_group_t    *grp;

    /*! Different device has different max members. */
    uint32_t max_members;

} bcmtrunk_lth_fgrp_bk_t;

/*!
 * It is used to save parameters from LTM while doing parsing LT fields.
 * It is used for TRUNK_FAST LT.
 */
typedef struct bcmtrunk_fast_group_param_s {
    /*! Trunk id. */
    uint32_t      id;

    /*! Trunk id valid. */
    bool          id_valid;

    /*! Trunk load balancing mode(regualr hash, random or roud robin). */
    uint8_t       lb_mode;

    /*! Load balancing valid */
    bool          lb_mode_valid;

    /*! Hash rule tag for member selection. */
    uint8_t       uc_rtag;

    /*! Uc_rtag valid. */
    bool          uc_rtag_valid;

    /*! Actual egress member count. */
    uint16_t      uc_cnt;

    /*! Uc_cnt valid. */
    bool          uc_cnt_valid;

    /*! Module ID array of egress members for unicast packets. */
    uint8_t       uc_modid[BCMTRUNK_MAX_MEMBERS];

    /*! Uc_modid valid. */
    bool          uc_modid_valid[BCMTRUNK_MAX_MEMBERS];

    /*! Port ID array of egress members for unicast packets. */
    uint8_t       uc_modport[BCMTRUNK_MAX_MEMBERS];

    /*! Uc_modport valid. */
    bool          uc_modport_valid[BCMTRUNK_MAX_MEMBERS];

    /*! AGM id. */
    uint8_t       uc_agm_id;

    /*! Uc_agm_id valid. */
    bool          uc_agm_id_valid;

    /*! Egress member count for non-unicast packets. */
    uint16_t      nonuc_cnt;

    /*! Nonuc_cnt valid. */
    bool          nonuc_cnt_valid;

    /*! Module ID array of egress members for non-unicast packets. */
    uint8_t       nonuc_modid[BCMTRUNK_FAST_MAX_MEMBERS];

    /*! Nonuc_modid valid. */
    bool          nonuc_modid_valid[BCMTRUNK_FAST_MAX_MEMBERS];


    /*! Port ID array of egress members for non-unicast packets. */
    uint8_t       nonuc_modport[BCMTRUNK_FAST_MAX_MEMBERS];

    /*! Nonuc_modport valid. */
    bool          nonuc_modport_valid[BCMTRUNK_FAST_MAX_MEMBERS];

} bcmtrunk_fast_group_param_t;


/*! Save parameters from LTM while parsing TRUNK_FAILOVER lt fields. */
typedef struct bcmtrunk_failover_param_s {
    /*! Failover port id. */
    uint32_t      id;

    /*! Failover port id valid */
    bool          id_valid;

    /*! Hash rule tag for failover. */
    uint8_t       rtag;

    /*! Rtag valid. */
    bool          rtag_valid;

    /*! Actual failover list count. */
    uint16_t      failover_cnt;

    /*! Failover cnt valid. */
    bool          failover_cnt_valid;

    /*! Module ID array of failover ports. */
    uint8_t       failover_modid[BCMTRUNK_FAILOVER_MAX_MEMBERS];

    /*! Module id valid. */
    bool          failover_modid_valid[BCMTRUNK_FAILOVER_MAX_MEMBERS];

    /*! Port ID array of failover ports. */
    uint8_t       failover_modport[BCMTRUNK_FAILOVER_MAX_MEMBERS];

    /*! Modport id valid. */
    bool          failover_modport_valid[BCMTRUNK_FAILOVER_MAX_MEMBERS];

} bcmtrunk_failover_param_t;

/*! Bookkeeping info for TRUNK_FALIOVER LT. */
typedef struct bcmtrunk_failover_bk_s {
    /*! Trunk failover info. */
    bcmtrunk_failover_t    *entry;

    /*! Maximum number of logical ports. */
    uint16_t                max_ports;

} bcmtrunk_failover_bk_t;


#endif /* BCMTRUNK_TYPES_H */

