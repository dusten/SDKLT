/*! \file bcmtm_tdm_defines.h
 *
 * Defines and Typedefs for TDM core algorithm
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

#ifndef BCMTM_TDM_DEFINES_H
#define BCMTM_TDM_DEFINES_H


/********************************************************************
 * Defines
 */
/*! Number of limited ports per pipe. */
#define TDM_AUX_SIZE 128

/*! Number of max speed classes. */
#define TDM_MAX_SPEED_TYPES 32

/*! Number of max calendars per device. */
#define TDM_NUM_CALENDARS 8

/*! TDM BOOL values. */
#define TDM_TRUE 1
#define TDM_FALSE 0
#define TDM_PASS 1
#define TDM_FAIL 0

/*! TDM slot checking direction in calendar. */
#define TDM_DIR_DN 1
#define TDM_DIR_UP 0

/*! TDM calendar mode. */
#define TDM_CAL_CURR 0
#define TDM_CAL_PREV 1

/*! DEVICE ID MASK. */
#define TDM_DEV_ID_MASK 0xfff0

/*! Max number of general (front panel) ports. */
#define TDM_MAX_NUM_GPORTS 512
/*! Max number of PMs. */
#define TDM_MAX_NUM_PMS 128

/*! Max length of linerate calendar. */
#define TDM_MAX_CAL_LEN 512

/*! Max number of oversub speed groups. */
#define TDM_MAX_GRP_WID 12

/*! Max length of each oversub speed group. */
#define TDM_MAX_GRP_LEN 12

/*! Max number of oversub groups per calendar. */
#define TDM_MAX_NUM_GRPS TDM_MAX_GRP_WID

/*! Max number of pipes. */
#define TDM_MAX_NUM_CALS 8

/*! Max number of subpipes per pipe. */
#define TDM_MAX_NUM_SUBPIPES 2

/*! Max number of groups per packet shaper scheduler. */
#define TDM_MAX_SHP_WID 2

/*! Max length of each packet shaper group. */
#define TDM_MAX_SHP_LEN 160

/*! Max number of PMs per packet shaper scheduler. */
#define TDM_MAX_PMS_PER_SHP 32

/*! Max number of ports per pipe. */
#define TDM_MAX_PORTS_PER_PIPE 64

/*! Pintout width of linerate calendars. */
#define TDM_LR_CAL_PRINT_WID 20

/********************************************************************
 * Typedefs: TDM speed types
 */
/*!
 * \brief TDM port speed types.
 *
 * This structure is used to define TDM supported port speed types.
 */
typedef enum tdm_port_speed_e {
    TDM_SPEED_0     = 0,
    TDM_SPEED_1G    = 1000,
    TDM_SPEED_1P25G = 1250,
    TDM_SPEED_2P5G  = 2500,
    TDM_SPEED_5G    = 5000,
    TDM_SPEED_7P5G  = 7500,
    TDM_SPEED_10G   = 10000,
    TDM_SPEED_11G   = 11000,
    TDM_SPEED_12P5G = 12500,
    TDM_SPEED_13G   = 13000,
    TDM_SPEED_15G   = 15000,
    TDM_SPEED_16G   = 16000,
    TDM_SPEED_20G   = 20000,
    TDM_SPEED_21G   = 21000,
    TDM_SPEED_25G   = 25000,
    TDM_SPEED_27G   = 27000,
    TDM_SPEED_40G   = 40000,
    TDM_SPEED_42G   = 42000,
    TDM_SPEED_50G   = 50000,
    TDM_SPEED_53G   = 53000,
    TDM_SPEED_75G   = 75000,
    TDM_SPEED_100G  = 100000,
    TDM_SPEED_106G  = 106000,
    TDM_SPEED_120G  = 120000,
    TDM_SPEED_127G  = 127000,
    TDM_SPEED_200G  = 200000,
    TDM_SPEED_400G  = 400000
} tdm_port_speed_t;

/*!
 * \brief TDM port state types.
 *
 * This structure is used to define TDM supported port state types.
 */
typedef enum tmd_port_state_e {
    /*! Ethernet types. */
    TDM_STATE_DISABLED = 0x0,
    TDM_STATE_LINERATE = 0x1,
    TDM_STATE_OVERSUB  = 0x2,
    TDM_STATE_COMBINE  = 0x3,

    /*! Higig types. */
    TDM_STATE_LINERATE_HG = 0x5,
    TDM_STATE_OVERSUB_HG  = 0x6,
    TDM_STATE_COMBINE_HG  = 0x7,

    /*! Management port types. */
    TDM_STATE_MGMT    = 0x9,
    TDM_STATE_MGMT_HG = 0xd,

    /*! Flex port types */
    TDM_STATE_FLEX_UP = 0x10,
    TDM_STATE_FLEX_DN = 0x20,
    TDM_STATE_FLEX_CH = 0x40
} tdm_port_state_t;

/*!
 * \brief TDM speed index types.
 *
 * This structure is used to define TDM supported speed index types.
 */
typedef enum tdm_speed_idx_e {
    TDM_SPEED_IDX_0=0,
    TDM_SPEED_IDX_1G,
    TDM_SPEED_IDX_1P25G,
    TDM_SPEED_IDX_2P5G,
    TDM_SPEED_IDX_5G,
    TDM_SPEED_IDX_10G,
    TDM_SPEED_IDX_12P5G,
    TDM_SPEED_IDX_20G,
    TDM_SPEED_IDX_25G,
    TDM_SPEED_IDX_40G,
    TDM_SPEED_IDX_50G,
    TDM_SPEED_IDX_100G,
    TDM_SPEED_IDX_120G,
    TDM_SPEED_IDX_200G,
    TDM_SPEED_IDX_400G,
    TDM_SPEED_IDX_NUM
} tdm_speed_idx_t;

/*!
 * \brief TDM slot units.
 *
 * This structure is used to define TDM supported slot unit in Mbps/slot.
 */
typedef enum tdm_slot_unit_e {
    TDM_SLOT_UNIT_0     = 0,
    TDM_SLOT_UNIT_1G    = TDM_SPEED_1G,
    TDM_SLOT_UNIT_1P25G = TDM_SPEED_1P25G,
    TDM_SLOT_UNIT_2P5G  = TDM_SPEED_2P5G,
    TDM_SLOT_UNIT_5G    = TDM_SPEED_5G,
    TDM_SLOT_UNIT_10G   = TDM_SPEED_10G,
    TDM_SLOT_UNIT_25G   = TDM_SPEED_25G,
    TDM_SLOT_UNIT_50G   = TDM_SPEED_50G
} tdm_slot_unit_t;

/*!
 * \brief TDM core executive driver index.
 *
 * This structure is used to define TDM core executive driver index.
 */
typedef enum tdm_drv_e {
    TDM_DRV_INIT = 0,
    TDM_DRV_CFG,
    TDM_DRV_RUN,
    TDM_DRV_FREE,
    TDM_CORE_INIT,
    TDM_CORE_FREE,
    TDM_CORE_POST,
    TDM_CORE_ALLOC,
    TDM_CORE_SCHEDULER,
    TDM_CORE_SCHEDULER_OVS,
    TDM_CORE_EXTRACT,
    TDM_CORE_FILTER,
    TDM_CORE_ACCESSORIZE,
    TDM_CORE_VMAP_PREALLOC,
    TDM_CORE_VMAP_ALLOC,
    TDM_CORE_ENCAP_SCAN,
    TDM_CORE_PICK_VEC,
    TDM_CORE_PM_SCAN,
    TDM_DRV_SIZE
} tdm_drv_t;


/**************************************************************************
 * Typedefs : User data
 */
 /*!
 * \brief TDM user configuration structure.
 *
 * This structure is used to define the user interface of TDM algorithm
 * in order to generate TDM calendars for certian port configuration.
 */
typedef struct tdm_cfg_s {
    /*! SDK unit. */
    int unit;

    /*! Device id. */
    int dev_id;

    /*! Core clock frequency. */
    int clk_freq_core;

    /*! Dpp clock frequency. */
    int clk_freq_dpp;

    /*! Number of general ports: FP+CPU+GMGT+LPBK. */
    int num_gports;

    /*! Flexport operation indicator: 0->disable, 1->enable. */
    int flex_en;

    /*! TDM checker indicator: 0->disable, 1->enable. */
    int chk_en;

    /*! Gport speed list. */
    int port_speeds[TDM_MAX_NUM_GPORTS];

    /*! Gport state list. */
    int port_states[TDM_MAX_NUM_GPORTS];

    /*! Reserved interface for chip specific variables. */
    void *chip_vars;

    /*! FIXME: temporal var */
    int mgmt_mode;
} tdm_cfg_t;

/*!
 * \brief TDM user access data for main calendar (per pipe).
 */
typedef struct tdm_cal_main_s {
    /*! calendar enable indicator, 1->enable, 0->disable (default). */
    int cal_en;

    /*! calendar validity, 1->valid, 0->invalid (default). */
    int cal_valid;

    /*! calendar length */
    int cal_len;

    /*! calendar */
    int cal[TDM_MAX_CAL_LEN];
} tdm_cal_main_t;

/*!
 * \brief TDM user access data for oversub group calendar (per pipe).
 */
typedef struct tdm_cal_ovsb_s {
    /*! calendar enable indicator, 1->enable, 0->disable (default). */
    int cal_en;

    /*! calendar validity, 1->valid, 0->invalid (default). */
    int cal_valid;

    /*! num of groups */
    int grp_num;

    /*! length of each group */
    int grp_len;

    /*! calendar */
    int cal[TDM_MAX_GRP_WID][TDM_MAX_GRP_LEN];
} tdm_cal_ovsb_t;

/*!
 * \brief TDM user access data for packet shaper calendar (per pipe).
 */
typedef struct tdm_cal_shaper_s {
    /*! calendar enable indicator, 1->enable, 0->disable (default). */
    int cal_en;

    /*! calendar validity, 1->valid, 0->invalid (default). */
    int cal_valid;

    /*! number of sub-pipes/groups per packet shaper */
    int cal_wid;

    /*! calendar length (num of rows) */
    int cal_len;

    /*! calendar */
    int cal[TDM_MAX_SHP_WID][TDM_MAX_SHP_LEN];

    /*! length of pm_to_portblock array */
    int pm2pblk_len;

    /*! pm_to_porkblock array */
    int pm2pblk_map[TDM_MAX_NUM_PMS];
} tdm_cal_shaper_t;

/*!
 * \brief TDM user access data for all calendars (per pipe).
 *
 * This structure stores the TDM output data (i.e. TDM calendars)
 * generated by TDM algorithm for a single pipe.
 */
typedef struct tdm_cal_s {
    /*! calendar enable indicator: 1->enable, 0->disable (default). */
    char cal_en;

    /*! linerate main calendar */
    tdm_cal_main_t lr;

    /*! oversub group calendar */
    tdm_cal_ovsb_t ovsb;

    /*! packet shaper calendar */
    tdm_cal_shaper_t shp;
} tdm_cal_t;

/*!
 * \brief TDM user access interface.
 *
 * This structure stores TDM user input and output data.
 * TDM user input data contains chip and port configuration info,
 * TDM user output data contains all calendars generated by TDM algorithm.
 */
typedef struct tdm_user_data_s {
    /*! TDM configuration profile */
    tdm_cfg_t cfg;

    /*! TDM calendars */
    tdm_cal_t cals[TDM_MAX_NUM_CALS];
} tdm_user_data_t;


/**************************************************************************
 * Typedefs : chip data
 */
/*!
 * \brief TDM chip interface maintained by chip-specific TDM logic.
 */
typedef struct tdm_chip_cfg_s {
    /*! lowest front-panel/physical port number */
    int fp_port_lo;

    /*! highest front-panel/physical port number */
    int fp_port_hi;

    /*! number of pipes */
    int num_pipes;

    /*! number of front-panel pms */
    int num_pms;

    /*! number of lanes per pm */
    int num_pm_lanes;

    /*! minimum space for same port slots */
    int min_space_same;

    /*! minimum space sister port slots */
    int min_space_sister;

    /*! Physical port to PM mapping */
    int port2pm_map[TDM_MAX_NUM_GPORTS];
} tdm_chip_cfg_t;

/*!
 * \brief TDM chip struct for tokenized value.
 */
typedef struct tdm_chip_token_s {
    /*! token of ancillary slots */
    int token_ancl;

    /*! token of empty slots */
    int token_empty;

    /*! token of oversub slots */
    int token_ovsb;

    /*! token of CMIC/CPU port slots */
    int token_cmic;

    /*! token of IDLE1 slots */
    int token_idl1;

    /*! token of IDLE2 slots */
    int token_idl2;

    /*! token of IDLE slots */
    int token_idle;

    /*! token of opportunistic-1 slots */
    int token_opp1;

    /*! token of opportunistic-2 slots */
    int token_opp2;

    /*! token of NULL slots */
    int token_null;
} tdm_chip_token_t;

/*!
 * \brief TDM chip variables.
 */
typedef struct tdm_chip_var_s {
    /*! PM enable indicator, 1->enable, 0->disable (default) */
    char pm_en[TDM_MAX_NUM_PMS];

    /*! PM to subpipe map */
    char pm2spipe_map[TDM_MAX_NUM_PMS];
} tdm_chip_var_t;

/*!
 * \brief TDM chip interface maintained by chip-specific TDM logic.
 */
typedef struct tdm_chip_data_s {
    /*! chip parameters */
    tdm_chip_cfg_t cfg;

    /*! chip parameters */
    tdm_chip_token_t token;

    /*! chip variables */
    tdm_chip_var_t var;
} tdm_chip_data_t;


/**************************************************************************
 * Typedefs : pipe data
 */
/*!
 * \brief TDM core access data sturct for pipe configuration.
 *
 * This structure stores configurable pipe information.
 */
typedef struct tdm_pipe_cfg_s {
    /*! slot granularity for main calendar (in Mbps) */
    int slot_unit;

    /*! pipe id */
    int pipe_id;

    /*! lowest physical port number in pipe */
    int pipe_port_lo;

    /*! highest physical port number in pipe */
    int pipe_port_hi;

    /*! Max number of linerate slots in main calendar. */
    int num_lr_limit;

    /*! Max number of ancillary slots in main calendar. */
    int num_ancl_limit;
} tdm_pipe_cfg_t;

/*!
 * \brief TDM core access data sturct for pipe variables.
 *
 * This structure stores variables for linerate and oversub ports in a pipe.
 */
typedef struct tdm_pipe_var_s {
    /*! linerate port enable indicator */
    char lr_en;

    /*! oversub port enable indicator */
    char os_en;

    /*! linerate port count */
    int lr_cnt;

    /*! oversub port count */
    int os_cnt;

    /*! number of speed classes for linerate ports */
    int lr_spd_types;

    /*! number of speed classes for oversub ports */
    int os_spd_types;

    /*! linerate port buffer */
    int lr_buff[TDM_MAX_PORTS_PER_PIPE];

    /*! oversub port buffer */
    int os_buff[TDM_MAX_PORTS_PER_PIPE];

    /*! port count per speed class for linerate ports */
    int lr_spd_prt_cnt[TDM_MAX_SPEED_TYPES];

    /*! port count per speed class for oversub ports */
    int os_spd_prt_cnt[TDM_MAX_SPEED_TYPES];
} tdm_pipe_var_t;

/*!
 * \brief TDM core side miscellanenous variables.
 *
 * This structure stores variables to help TDM core functions to access
 * TDM chip-side functions.
 */
typedef struct tdm_core_misc_s {
    /*! Physical port. */
    int port;
} tdm_core_misc_t;

/*!
 * \brief TDM core interface used by TDM library.
 */
typedef struct tdm_core_data_s {
    /*! pipe parameters */
    tdm_pipe_cfg_t cfg;

    /*! pipe variables */
    tdm_pipe_var_t var;

    /*! Core miscellaneous variables for chip usage */
    tdm_core_misc_t misc;
} tdm_core_data_t;



/**************************************************************************
 * Typedefs: TDM global structure
 */

/*!
 * \brief TDM global structure.
 */
typedef struct tdm_mod_s {
    /*! TDM user data */
    tdm_user_data_t user_data;

    /*! TDM user data for FlexPort context */
    tdm_user_data_t prev_user_data;

    /*! TDM chip side data */
    tdm_chip_data_t chip_data;

    /*! TDM core side data */
    tdm_core_data_t core_data;

    /*! TDM core driver functions */
    int (*core_drv[TDM_DRV_SIZE])(struct tdm_mod_s *);
} tdm_mod_t;

#endif /* BCMTM_TDM_DEFINES_H */
