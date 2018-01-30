/*! \file bcmtm_internal_tdm.h
 *
 * TM internal TDM calendar.
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

#ifndef BCMTM_INTERNAL_TDM_H
#define BCMTM_INTERNAL_TDM_H

#include <bcmtm/tdm/bcmtm_tdm_top.h>
#include <bcmtm/bcmtm_types.h>


/*
 * Lane Information
 *
 * Contains information on a given lane for a port.
 *
 * A port that uses 'x' number of lanes will have 'x' soc_port_lane_info_t
 * elements, one for each of the lanes that the port will use.
 */
typedef struct soc_port_lane_info_s {
    int pgw;           /* PGW instance where lane resides */
    int xlp;           /* XLP number within PGW */
    int tsc;           /*  TSC number (Same as XLP) */
    int port_index;    /* Index within XLP */
} soc_port_lane_info_t;


/*
 * Port Resource
 *
 * Contains port resource configuration used for FlexPort operations.
 */
typedef struct soc_port_resource_s {
    uint32_t flags;
    uint32_t op;              /* BCMI_XGS5_PORT_RESOURCE_OP_XXX */
    int logical_port;       /* Logical port associated to physical port */
    int physical_port;      /* Physical port associated logical port */
    int mmu_port;           /* MMU port associated to logical port */
    int idb_port;           /* IDB port associated to logical port */
    int pipe;               /* Pipe number of the port */
    int speed;              /* Initial speed after FlexPort operation */
    int mode;               /* Port mode: single, dual, quad, tri012, ... */
    int num_lanes;          /* Number of PHY lanes */
    soc_port_lane_info_t *lane_info[SOC_PORT_RESOURCE_LANES_MAX];
                            /* Lane information array */
    bcmtm_port_encap_mode_t encap; /* Encapsulation mode for port */
    int oversub;            /* Indicates if port has oversub enabled */
    uint16_t prio_mask;       /* Packet priority to priority group mapping mask */
    uint8_t hsp;              /* Indicates if port is high speed port */
    uint32_t cl91_enabled;    /* Indicates if port has CL91 enabled */
} soc_port_resource_t;



#define MAX_SCH_SLICES        2
#define MAX_NUM_FLEXPORTS     256
#define MAX_CAL_LEN           512
#define MAX_NUM_OVS_GRPS      32
#define MAX_OVS_GRP_LEN       64
#define MAX_NUM_OVS_SPC_GRP   32
#define MAX_OVS_SPC_LEN       160

#define MAX_FLEX_PHASES       25

/*
 * ASF (Cut-Through forwarding)
 */

/* ASF Modes */
typedef enum {
    _SOC_ASF_MODE_SAF          = 0,  /* store-and-forward */
    _SOC_ASF_MODE_SAME_SPEED   = 1,  /* same speed CT  */
    _SOC_ASF_MODE_SLOW_TO_FAST = 2,  /* slow to fast CT   */
    _SOC_ASF_MODE_FAST_TO_SLOW = 3,  /* fast to slow CT   */
    _SOC_ASF_MODE_CFG_UPDATE   = 4,  /* internal cfg updates */
    _SOC_ASF_MODE_UNSUPPORTED  = 5
} soc_asf_mode_e;

/* ASF MMU Cell Buffer Allocation Profiles */
typedef enum {
    _SOC_ASF_MEM_PROFILE_NONE      = 0,  /* No cut-through support */
    _SOC_ASF_MEM_PROFILE_SIMILAR   = 1,  /* Similar Speed Alloc Profile  */
    _SOC_ASF_MEM_PROFILE_EXTREME   = 2,  /* Extreme Speed Alloc Profile */
    _SOC_ASF_MEM_PROFILE_END       = 3
} soc_asf_mem_profile_e;

typedef struct soc_asf_prop_s {
    int                    switch_bypass_mode; /* Indicate latency modes. */
    soc_asf_mode_e         asf_modes[MAX_NUM_PORTS]; /* Per-port CT mode. */
    soc_asf_mem_profile_e  asf_mem_prof; /* MMU cell buffer allocation profile. */
} soc_asf_prop_t;


typedef struct soc_port_map_type_s {
    tdm_port_speed_t log_port_speed[MAX_NUM_PORTS]; /* Logical port speed. */

    /* Physical-to-logical port map. From soc_info_t. */
    int     port_p2l_mapping[MAX_NUM_PORTS];
    /* Logical-to-physical port map. From soc_info_t. */
    int     port_l2p_mapping[MAX_NUM_PORTS];
    /* Physical-to-MMU port map. From soc_info_t. */
    int     port_p2m_mapping[MAX_NUM_PORTS];
    /* MMU-to-physical port map. From soc_info_t. */
    int     port_m2p_mapping[MAX_NUM_PORTS];
    /* Logical-to-IDB port map. From soc_info_t. */
    int     port_l2i_mapping[MAX_NUM_PORTS];
    /* Logical port to number of SERDES lanes information. From soc_info_t. */
    int     port_num_lanes[MAX_NUM_PORTS];
    /* Physical port to port-block (PBLK, a permutation of PM instances) */
    /* instance mapping for the full-chip. N PMs per pipe. */
    int     port_p2PBLK_inst_mapping[MAX_NUM_PORTS];

    pbmp_t  physical_pbm;      /* Physical port bitmap. From soc_info_t. */
    pbmp_t  hg2_pbm;           /* HiGig2 port bitmap. From soc_info_t. */
    pbmp_t  management_pbm;    /* Management port bitmap. From soc_info_t. */
    pbmp_t  oversub_pbm;       /* Oversubscribed port bitmap. From soc_info_t. */

} soc_port_map_type_t;


typedef struct soc_tdm_schedule_s {
    int cal_len;                        /* Length of linerate TDM calendar. */
    int linerate_schedule[MAX_CAL_LEN]; /* One array holding the TDM calendar. */

    int num_ovs_groups;                 /* Number of oversubscription groups. */
    int ovs_group_len;                  /* Oversubscription group length. */
                                        /* An array of arrays holding the oversubscription        */
                                        /* schedule for multiple oversubscription groups.         */
    int oversub_schedule[MAX_NUM_OVS_GRPS][MAX_OVS_GRP_LEN];

    int num_pkt_sch_or_ovs_space;       /* Number of packet scheduler or spacing groups.          */
    int pkt_sch_or_ovs_space_len;       /* Packet scheduler or spacing group length.              */
                                        /* An array of arrays holding either the oversubscription */
                                        /* spacings for multiple oversubscription groups or       */
                                        /* the packet scheduling calendar information.            */
    int pkt_sch_or_ovs_space[MAX_NUM_OVS_SPC_GRP][MAX_OVS_SPC_LEN];
} soc_tdm_schedule_t;


typedef struct soc_tdm_schedule_pipe_s {
    int num_slices;     /* Set to 1 where the chip does not have separate */
                        /* physical tables for half-pipes (or slices). */
    soc_tdm_schedule_t   tdm_schedule_slice[MAX_SCH_SLICES]; /* TDM schedule per-slice. */

} soc_tdm_schedule_pipe_t;


typedef struct soc_port_schedule_state_s {
    int                  nport;         /* Number of ports to be flexed. From BCM. RO. */
    soc_port_resource_t  resource[MAX_NUM_FLEXPORTS]; /* Port reconfiguration information. From BCM. RO. */
    int                  frequency;     /* Core clock frequency. From soc_info_t. RO. */
    int                  bandwidth;     /* Maximum core bandwidth. From soc_info_t. RO. */
    int                  io_bandwidth;  /* Maximum IO bandwidth. From soc_info_t. RO. */
    int                  lossless;      /* Configurations - 1: Lossless; 0: Lossy. RO. */
    int                  is_flexport;   /* Call context - 1: FlexPort; 0: Initial TDM. RO. */
    int                  calendar_type; /* TDM calendar type - 0: Universal; 1: Ethernet optimized*/

    soc_asf_prop_t       cutthru_prop;  /* Cut-Through setting */
    soc_port_map_type_t  in_port_map;   /* Input port map. From soc_info_t. RO. */
    soc_port_map_type_t  out_port_map;  /* Output port map. From soc_info_t. */

    soc_tdm_schedule_pipe_t tdm_ingress_schedule_pipe[MAX_NUM_PIPES];
    soc_tdm_schedule_pipe_t tdm_egress_schedule_pipe[MAX_NUM_PIPES];
                                  /* Scratch pad locals. Caller must not allocate. */
                                  /* Callee allocates at entry and deallocates     */
                                  /* before return.                                */
    void *cookie;
} soc_port_schedule_state_t;

typedef struct _bcmtm_tomahawk_tdm_pblk_info_s {

    int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
} _bcmtm_tomahawk_tdm_pblk_info_t;


#define BCMTM_INT_TDM_LENGTH                 256
#define BCMTM_INT_OVS_HPIPE_COUNT_PER_PIPE   1
#define BCMTM_INT_OVS_GROUP_COUNT_PER_HPIPE  6
#define BCMTM_INT_OVS_GROUP_TDM_LENGTH       12
#define BCMTM_INT_PKT_SCH_LENGTH             160


/*  General Physical port */
#define BCMTM_INT_PORTS_PER_PBLK             4
#define BCMTM_INT_PBLKS_PER_PIPE             8
#define BCMTM_INT_PIPES_PER_DEV              4
#define BCMTM_INT_GPHY_PORTS_PER_PIPE        \
    (BCMTM_INT_PORTS_PER_PBLK * BCMTM_INT_PBLKS_PER_PIPE)
#define BCMTM_INT_PHY_PORTS_PER_PIPE         (BCMTM_INT_GPHY_PORTS_PER_PIPE + 2)
#define BCMTM_INT_PBLKS_PER_DEV              \
    (BCMTM_INT_PBLKS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)
#define BCMTM_INT_PHY_PORTS_PER_DEV          \
    (BCMTM_INT_PHY_PORTS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)


/*  Device port
 *   * 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define BCMTM_INT_GDEV_PORTS_PER_PIPE        32
#define BCMTM_INT_DEV_PORTS_PER_PIPE         (BCMTM_INT_GDEV_PORTS_PER_PIPE + 2)
#define BCMTM_INT_DEV_PORTS_PER_DEV          \
    (BCMTM_INT_DEV_PORTS_PER_PIPE * BCMTM_INT_PIPES_PER_DEV)

typedef struct _bcmtm_tomahawk_tdm_pipe_s {
    int idb_tdm[BCMTM_INT_TDM_LENGTH];
    int mmu_tdm[BCMTM_INT_TDM_LENGTH];
    int ovs_tdm[BCMTM_INT_OVS_HPIPE_COUNT_PER_PIPE][BCMTM_INT_OVS_GROUP_COUNT_PER_HPIPE][BCMTM_INT_OVS_GROUP_TDM_LENGTH];
    int pkt_shaper_tdm[BCMTM_INT_OVS_HPIPE_COUNT_PER_PIPE][BCMTM_INT_PKT_SCH_LENGTH];
} _bcmtm_tomahawk_tdm_pipe_t;

typedef struct _bcmtm_tomahawk_tdm_s {
    _bcmtm_tomahawk_tdm_pipe_t tdm_pipe[BCMTM_INT_PIPES_PER_DEV];
    _bcmtm_tomahawk_tdm_pblk_info_t pblk_info[BCMTM_INT_PHY_PORTS_PER_DEV];
    int port_ratio[BCMTM_INT_PBLKS_PER_DEV];
    int ovs_ratio_x1000[BCMTM_INT_PIPES_PER_DEV][BCMTM_INT_OVS_HPIPE_COUNT_PER_PIPE];
} _bcmtm_tomahawk_tdm_t;


#if 0 
typedef struct bcmtm_drv_info_s {

    int frequency;
    int fabric_port_enable;

    /*! Logical to physical port mapping */
    int port_l2p_mapping[SOC_MAX_NUM_PORTS];
    int port_p2l_mapping[SOC_MAX_NUM_PORTS];

    /*! Logical to mmu port mapping */
    int port_l2m_mapping[SOC_MAX_NUM_PORTS];

    /*! Physical to mmu port mapping */
    int port_p2m_mapping[SOC_MAX_NUM_PORTS];

    /*! Max speed */
    int port_speed_max[SOC_MAX_NUM_PORTS];
    int port_init_speed[SOC_MAX_NUM_PORTS];

    /*! Current speed */
    int port_speed_cur[SOC_MAX_NUM_PORTS];

#if 0
    /*! TDM database */
    bcmtm_drv_tdm_t tdm_info;
#endif
    soc_port_schedule_state_t port_schedule_state;

} bcmtm_drv_info_t;
#endif

#endif /* BCMTM_INTERNAL_TDM_H */

