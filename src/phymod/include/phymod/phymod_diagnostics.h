/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 2013/09/12 10:43:06 nirf Exp $
 * 
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
 * 
 * 
 *         
 *     
 * DO NOT EDIT THIS FILE!
 */

#ifndef _PHYMOD_DIAGNOSTICS_H__H_
#define _PHYMOD_DIAGNOSTICS_H__H_

#include <phymod/phymod_definitions.h>
#include <phymod/phymod.h>
typedef struct phymod_slicer_position_s {
    uint32_t horizontal;
    uint32_t vertical;
} phymod_slicer_position_t;

/* phymod_slicer_position_t initialization and validation */
int phymod_slicer_position_t_validate(const phymod_slicer_position_t* phymod_slicer_position);
int phymod_slicer_position_t_init(phymod_slicer_position_t* phymod_slicer_position);

/*! 
 * @brief Flags for phymod_phy_rx_slicer_position_set/get APIs 
 */ 
#define PHYMOD_SLICER_F_LIVE_LINK 0x1 /**< Use the live-link slicer */


/*! 
 * phymod_phy_rx_slicer_position_set
 *
 * @brief Set/get slicer position 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [in]  position        - Slicer location to set
 */
int phymod_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position);
/*! 
 * phymod_phy_rx_slicer_position_get
 *
 * @brief Set/get slicer position 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [out]  position        - Current slicer position
 */
int phymod_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position);

/*! 
 * phymod_phy_rx_slicer_position_max_get
 *
 * @brief Get slicer range limitation 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [in]  position_min    - Slicer minimum position
 * @param [in]  position_max    - Slicer maxumum position
 */
int phymod_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max);


/*!
 * @enum phymod_prbs_poly_e
 * @brief PRBS polynomial 
 */ 
typedef enum phymod_prbs_poly_e {
    phymodPrbsPoly7 = 0,
    phymodPrbsPoly9,
    phymodPrbsPoly11,
    phymodPrbsPoly15,
    phymodPrbsPoly23,
    phymodPrbsPoly31,
    phymodPrbsPoly58,
    phymodPrbsPoly49,
    phymodPrbsPoly10,
    phymodPrbsPoly20,
    phymodPrbsPoly13,
    phymodPrbsPolyQ13,
    phymodPrbsPolyCount
} phymod_prbs_poly_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_prbs_poly_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_prbs_poly_t validation */
int phymod_prbs_poly_t_validate(phymod_prbs_poly_t phymod_prbs_poly);
/*! 
 * @brief RX/TX selector. no flags means both 
 */ 
#define PHYMOD_PRBS_DIRECTION_RX 0x1 /**< Config RX */
#define PHYMOD_PRBS_DIRECTION_TX 0x2 /**< Config TX */

#define PHYMOD_PRBS_DIRECTION_RX_SET(flags) (flags |= PHYMOD_PRBS_DIRECTION_RX)
#define PHYMOD_PRBS_DIRECTION_TX_SET(flags) (flags |= PHYMOD_PRBS_DIRECTION_TX)

#define PHYMOD_PRBS_DIRECTION_RX_CLR(flags) (flags &= ~PHYMOD_PRBS_DIRECTION_RX)
#define PHYMOD_PRBS_DIRECTION_TX_CLR(flags) (flags &= ~PHYMOD_PRBS_DIRECTION_TX)

#define PHYMOD_PRBS_DIRECTION_RX_GET(flags) (flags & PHYMOD_PRBS_DIRECTION_RX ? 1 : 0)
#define PHYMOD_PRBS_DIRECTION_TX_GET(flags) (flags & PHYMOD_PRBS_DIRECTION_TX ? 1 : 0)


/*!
 * @struct phymod_prbs_s
 * @brief PRBS control 
 */ 
typedef struct phymod_prbs_s {
    phymod_prbs_poly_t poly;
    uint32_t invert;
} phymod_prbs_t;

/* phymod_prbs_t initialization and validation */
int phymod_prbs_t_validate(const phymod_prbs_t* phymod_prbs);
int phymod_prbs_t_init(phymod_prbs_t* phymod_prbs);

/*! 
 * phymod_phy_prbs_config_set
 *
 * @brief set/get PRBS configuration 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [in]  prbs            - PRBS configuration to set
 */
int phymod_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs);
/*! 
 * phymod_phy_prbs_config_get
 *
 * @brief set/get PRBS configuration 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [out]  prbs            - PRBS configuration
 */
int phymod_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs);

/*! 
 * phymod_phy_prbs_enable_set
 *
 * @brief Set/get PRBS enable state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [in]  enable          - Enable \ disable PRBS
 */
int phymod_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable);
/*! 
 * phymod_phy_prbs_enable_get
 *
 * @brief Set/get PRBS enable state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [out]  enable          - PRBS state
 */
int phymod_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable);


/*!
 * @struct phymod_prbs_status_s
 * @brief PRBS control 
 */ 
typedef struct phymod_prbs_status_s {
    uint32_t prbs_lock; /**< Whether PRBS is currently locked */
    uint32_t prbs_lock_loss; /**< Whether PRBS was unlocked since last call */
    uint32_t error_count; /**< PRBS errors count */
} phymod_prbs_status_t;

/* phymod_prbs_status_t initialization and validation */
int phymod_prbs_status_t_validate(const phymod_prbs_status_t* phymod_prbs_status);
int phymod_prbs_status_t_init(phymod_prbs_status_t* phymod_prbs_status);

/*! 
 * @brief Flags for phymod_phy_prbs_status_get API 
 */ 
#define PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ 0x1 /**< Clear error counter on read */


/*! 
 * phymod_phy_prbs_status_get
 *
 * @brief Get PRBS Status 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [out]  prbs_status     - PRBS status
 */
int phymod_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status);

typedef struct phymod_pattern_s {
    uint32_t pattern_len;
    uint32_t* pattern; /**< Pattern to set */
} phymod_pattern_t;

/* phymod_pattern_t initialization and validation */
int phymod_pattern_t_validate(const phymod_pattern_t* phymod_pattern);
int phymod_pattern_t_init(phymod_pattern_t* phymod_pattern);

/*! 
 * phymod_phy_pattern_config_set
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  pattern         - Pattern Configuration
 */
int phymod_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern);
/*! 
 * phymod_phy_pattern_config_get
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [inout] pattern         - Pattern Configuration
 */
int phymod_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern);

/*! 
 * phymod_phy_pattern_enable_set
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  enable          - Enable \ disable pattern send \ receive
 * @param [in]  pattern         - Pattern Configuration
 */
int phymod_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern);
/*! 
 * phymod_phy_pattern_enable_get
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  enable          - Pattern state
 */
int phymod_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable);

typedef struct phymod_core_diagnostics_s {
    uint32_t temperature;
    uint32_t pll_range;
} phymod_core_diagnostics_t;

/* phymod_core_diagnostics_t initialization and validation */
int phymod_core_diagnostics_t_validate(const phymod_core_diagnostics_t* phymod_core_diagnostics);
int phymod_core_diagnostics_t_init(phymod_core_diagnostics_t* phymod_core_diagnostics);

/*! 
 * phymod_core_diagnostics_get
 *
 * @brief Get core diagnostics information 
 *
 * @param [in]  core            - core access information
 * @param [out]  diag            - Core diagnostics information
 */
int phymod_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag);

typedef struct phymod_diag_slicer_offset_s {
    uint32_t offset_pe;
    uint32_t offset_ze;
    uint32_t offset_me;
    uint32_t offset_po;
    uint32_t offset_zo;
    uint32_t offset_mo;
} phymod_diag_slicer_offset_t;

/* phymod_diag_slicer_offset_t initialization and validation */
int phymod_diag_slicer_offset_t_validate(const phymod_diag_slicer_offset_t* phymod_diag_slicer_offset);
int phymod_diag_slicer_offset_t_init(phymod_diag_slicer_offset_t* phymod_diag_slicer_offset);

typedef struct phymod_diag_eyescan_s {
    uint32_t heye_left;
    uint32_t heye_right;
    uint32_t veye_upper;
    uint32_t veye_lower;
} phymod_diag_eyescan_t;

/* phymod_diag_eyescan_t initialization and validation */
int phymod_diag_eyescan_t_validate(const phymod_diag_eyescan_t* phymod_diag_eyescan);
int phymod_diag_eyescan_t_init(phymod_diag_eyescan_t* phymod_diag_eyescan);


/*!
 * @enum phymod_pmd_mode_e
 * @brief pmd modes 
 */ 
typedef enum phymod_pmd_mode_e {
    phymodPmdModeOs = 0,
    phymodPmdModeOsDfe,
    phymodPmdModeBrDfe,
    phymodPmdModeCount
} phymod_pmd_mode_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_pmd_mode_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_pmd_mode_t validation */
int phymod_pmd_mode_t_validate(phymod_pmd_mode_t phymod_pmd_mode);
typedef struct phymod_phy_diagnostics_s {
    uint32_t signal_detect;
    uint32_t vga_bias_reduced;
    uint32_t postc_metric;
    phymod_osr_mode_t osr_mode;
    phymod_pmd_mode_t pmd_mode;
    uint32_t rx_lock;
    uint32_t rx_ppm;
    uint32_t tx_ppm;
    uint32_t clk90_offset;
    uint32_t clkp1_offset;
    uint32_t p1_lvl;
    uint32_t m1_lvl;
    uint32_t dfe1_dcd;
    uint32_t dfe2_dcd;
    uint32_t slicer_target;
    phymod_diag_slicer_offset_t slicer_offset;
    phymod_diag_eyescan_t eyescan;
    uint32_t state_machine_status;
    uint32_t link_time;
    int pf_main;
    int pf_hiz;
    int pf_bst;
    int pf_low;
    int pf2_ctrl;
    int vga;
    int dc_offset;
    int p1_lvl_ctrl;
    int dfe1;
    int dfe2;
    int dfe3;
    int dfe4;
    int dfe5;
    int dfe6;
    int txfir_pre;
    int txfir_main;
    int txfir_post1;
    int txfir_post2;
    int txfir_post3;
    int tx_amp_ctrl;
    uint8_t br_pd_en;
} phymod_phy_diagnostics_t;

/* phymod_phy_diagnostics_t initialization and validation */
int phymod_phy_diagnostics_t_validate(const phymod_phy_diagnostics_t* phymod_phy_diagnostics);
int phymod_phy_diagnostics_t_init(phymod_phy_diagnostics_t* phymod_phy_diagnostics);

/*! 
 * phymod_phy_diagnostics_get
 *
 * @brief Get phy diagnostics information 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  diag            - 
 */
int phymod_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag);

/*! 
 * phymod_phy_pmd_info_dump
 *
 * @brief Get phy diagnostics information 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  type            - 
 */
int phymod_phy_pmd_info_dump(const phymod_phy_access_t* phy, char* type);

/*! 
 * phymod_phy_pcs_info_dump
 *
 * @brief Get phy diagnostics information 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  type            - 
 */
int phymod_phy_pcs_info_dump(const phymod_phy_access_t* phy, char* type);

typedef struct phymod_phy_eyescan_options_s {
    uint32_t linerate_in_khz;
    uint16_t timeout_in_milliseconds;
    int8_t horz_max;
    int8_t horz_min;
    int8_t hstep;
    int8_t vert_max;
    int8_t vert_min;
    int8_t vstep;
    uint8_t ber_proj_scan_mode; /**< For BER projection Controls the direction and polarity of the test. */
    uint8_t ber_proj_timer_cnt; /**< BER proj total measurement time in units of ~1.3 seconds. */
    uint8_t ber_proj_err_cnt; /**< BER proj the error threshold it uses to step to next measurement in units of 16. */
    int8_t mode;
} phymod_phy_eyescan_options_t;

/* phymod_phy_eyescan_options_t initialization and validation */
int phymod_phy_eyescan_options_t_validate(const phymod_phy_eyescan_options_t* phymod_phy_eyescan_options);
int phymod_phy_eyescan_options_t_init(phymod_phy_eyescan_options_t* phymod_phy_eyescan_options);


/*!
 * @enum phymod_eyescan_mode_e
 * @brief Eyescan modes 
 */ 
typedef enum phymod_eyescan_mode_e {
    phymodEyescanModeFast = 0, /**< Fast eyescan resoultion. */
    phymodEyescanModeLowBER, /**< Allow more aquirate BER resolution. */
    phymodEyescanModeBERProj, /**< Do BER projection. */
    phymodEyescanModeCount
} phymod_eyescan_mode_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_eyescan_mode_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_eyescan_mode_t validation */
int phymod_eyescan_mode_t_validate(phymod_eyescan_mode_t phymod_eyescan_mode);
/*! 
 * @brief Eyescan flags 
 */ 
#define PHYMOD_EYESCAN_F_ENABLE 0x1 /**< Enable eyescan */
#define PHYMOD_EYESCAN_F_PROCESS 0x2 /**< Process eyescan */
#define PHYMOD_EYESCAN_F_DONE 0x4 /**< Eyescan done */
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT 0x8 /**< Don't wait to finish enable operation */

#define PHYMOD_EYESCAN_F_ENABLE_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_ENABLE)
#define PHYMOD_EYESCAN_F_PROCESS_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_PROCESS)
#define PHYMOD_EYESCAN_F_DONE_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_DONE)
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT)

#define PHYMOD_EYESCAN_F_ENABLE_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_ENABLE)
#define PHYMOD_EYESCAN_F_PROCESS_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_PROCESS)
#define PHYMOD_EYESCAN_F_DONE_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_DONE)
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT)

#define PHYMOD_EYESCAN_F_ENABLE_GET(flags) ((flags) & PHYMOD_EYESCAN_F_ENABLE ? 1 : 0)
#define PHYMOD_EYESCAN_F_PROCESS_GET(flags) ((flags) & PHYMOD_EYESCAN_F_PROCESS ? 1 : 0)
#define PHYMOD_EYESCAN_F_DONE_GET(flags) ((flags) & PHYMOD_EYESCAN_F_DONE ? 1 : 0)
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags) ((flags) & PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT ? 1 : 0)

/*! 
 * phymod_phy_eyescan_run
 *
 * @brief Display eyescan information 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [in]  mode            - Select eyescan mode
 * @param [in]  eyescan_options   - Eyescan options. Set to NULL for eyescan types without options, or to use defaults.
 */
int phymod_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options);


/*!
 * @enum phymod_link_monitor_mode_e
 * @brief Link monitor modes 
 */ 
typedef enum phymod_link_monitor_mode_e {
    phymodLinkMonPCS49_1x10G = 0,
    phymodLinkMonPCS82_4x10G,
    phymodLinkMonPCS82_2x25G,
    phymodLinkMonPCS82_4x25G,
    phymodLinkMonFC4,
    phymodLinkMonFC8,
    phymodLinkMonFC16,
    phymodLinkMonFC32,
    phymodLinkMonCount
} phymod_link_monitor_mode_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_link_monitor_mode_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_link_monitor_mode_t validation */
int phymod_link_monitor_mode_t_validate(phymod_link_monitor_mode_t phymod_link_monitor_mode);

/*!
 * @enum phymod_diag_type_e
 * @brief diag types 
 */ 
typedef enum phymod_diag_type_e {
    phymod_diag_GENERAL = 0x00000001, /**< General device wide information.  */
    phymod_diag_TOPOLOGY = 0x00000002, /**< General device wide information.  */
    phymod_diag_LINK  = 0x00000004, /**< General device wide information.  */
    phymod_diag_SPEED = 0x00000008, /**< General device wide information.  */
    phymod_diag_ANEG = 0x00000010, /**< General device wide information.  */
    phymod_diag_TFC = 0x00000020, /**< General device wide information.  */
    phymod_diag_AN_TIMERS = 0x00000040, /**< General device wide information.  */
    phymod_diag_STATE = 0x00000080, /**< General device wide information.  */
    phymod_diag_DEBUG = 0x00000100, /**< General device wide information.  */
    phymod_diag_IEEE = 0x00000200, /**< General device wide information.  */
    phymod_diag_EEE = 0x00000400, /**< General device wide information.  */
    phymod_diag_BER = 0x00000800, /**< General device wide information.  */
    phymod_diag_CFG = 0x00001000, /**< General device wide information.  */
    phymod_diag_CL72 = 0x00002000, /**< General device wide information.  */
    phymod_diag_DSC = 0x00004000, /**< General device wide information.  */
    phymod_diag_DSC_STD = 0x00008000, /**< General device wide information.  */
    phymod_diag_ALL = 0x00010000, /**< General device wide information.  */
    phymod_diag_Count
} phymod_diag_type_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_diag_type_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_diag_type_t validation */
int phymod_diag_type_t_validate(phymod_diag_type_t phymod_diag_type);
/*! 
 * phymod_phy_link_mon_enable_set
 *
 * @brief Get/Set link monitor enable/disable 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  link_mon_mode   - 
 * @param [in]  enable          - 
 */
int phymod_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t enable);
/*! 
 * phymod_phy_link_mon_enable_get
 *
 * @brief Get/Set link monitor enable/disable 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  link_mon_mode   - 
 * @param [out]  enable          - 
 */
int phymod_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t* enable);

/*! 
 * phymod_phy_link_mon_status_get
 *
 * @brief Get link monitor Status 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  lock_status     - link mon status
 * @param [out]  lock_lost_lh    - Whether link mon was unlocked since last call
 * @param [out]  error_count     - link mon error count
 */
int phymod_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count);

/*! 
 * phymod_phy_fec_correctable_counter_get
 *
 * @brief Get CL74 FEC Corrected Codeword Counter 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  count           - CL74 FEC corrected codeword counter
 */
int phymod_phy_fec_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

/*! 
 * phymod_phy_fec_uncorrectable_counter_get
 *
 * @brief Get CL74 FEC Uncorrected Codeword Counter 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  count           - CL74 FEC uncorrected codeword counter
 */
int phymod_phy_fec_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

typedef struct phymod_phy_stat_s {
    uint32_t pcs_bip_err_count[PHYMOD_MAX_LANES_PER_CORE]; /**< BIP error count per lane */
    uint32_t pcs_ber_count[PHYMOD_MAX_LANES_PER_CORE]; /**< BER count per lane */
} phymod_phy_stat_t;

/* phymod_phy_stat_t initialization and validation */
int phymod_phy_stat_t_validate(const phymod_phy_stat_t* phymod_phy_stat);
int phymod_phy_stat_t_init(phymod_phy_stat_t* phymod_phy_stat);

/*! 
 * phymod_phy_stat_get
 *
 * @brief Get PHY Stat Counter 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  stat            - PHY stat, such as BIP error count, BER count and so on
 */
int phymod_phy_stat_get(const phymod_phy_access_t* phy, phymod_phy_stat_t* stat);

/*! 
 * phymod_phy_fec_cl91_correctable_counter_get
 *
 * @brief Get CL91 FEC Corrected Codeword Counter 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  count           - CL91 FEC corrected codeword counter
 */
int phymod_phy_fec_cl91_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

/*! 
 * phymod_phy_fec_cl91_uncorrectable_counter_get
 *
 * @brief Get CL91 FEC Uncorrected Codeword Counter 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  count           - CL91 FEC uncorrected codeword counter
 */
int phymod_phy_fec_cl91_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);


/*!
 * @enum phymod_PAM4_tx_pattern_e
 * @brief PAM4 tx pattern 
 */ 
typedef enum phymod_PAM4_tx_pattern_e {
    phymod_PAM4TxPattern_JP03B = 1, /**< JP03B tx pattern enable */
    phymod_PAM4TxPattern_Linear, /**< PAM4 linear tx pattern  */
    phymod_Count
} phymod_PAM4_tx_pattern_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_PAM4_tx_pattern_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_PAM4_tx_pattern_t validation */
int phymod_PAM4_tx_pattern_t_validate(phymod_PAM4_tx_pattern_t phymod_PAM4_tx_pattern);
/*! 
 * phymod_phy_PAM4_tx_pattern_enable_set
 *
 * @brief PAM4 tx pattern set/get 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  pattern_type    - 
 * @param [in]  enable          - enable
 */
int phymod_phy_PAM4_tx_pattern_enable_set(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t enable);
/*! 
 * phymod_phy_PAM4_tx_pattern_enable_get
 *
 * @brief PAM4 tx pattern set/get 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  pattern_type    - 
 * @param [out]  enable          - enable
 */
int phymod_phy_PAM4_tx_pattern_enable_get(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t* enable);

typedef struct phymod_phy_ber_proj_options_s {
    uint16_t ber_proj_fec_size; /**< FEC frame size in bits. */
    uint8_t ber_proj_hist_errcnt_thresh; /**< Histogram error threshold. */
    uint32_t ber_proj_timeout_s; /**< Measurement time that PRBS errors are accumulated */
} phymod_phy_ber_proj_options_t;

/* phymod_phy_ber_proj_options_t initialization and validation */
int phymod_phy_ber_proj_options_t_validate(const phymod_phy_ber_proj_options_t* phymod_phy_ber_proj_options);
int phymod_phy_ber_proj_options_t_init(phymod_phy_ber_proj_options_t* phymod_phy_ber_proj_options);


/*!
 * @enum phymod_ber_proj_mode_e
 * @brief BER projection mode 
 */ 
typedef enum phymod_ber_proj_mode_e {
    phymodBERProjModePostFEC = 1, /**< Post-fec error projection using PRBS analyzer */
    phymodBERProjModeCount
} phymod_ber_proj_mode_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_ber_proj_mode_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_ber_proj_mode_t validation */
int phymod_ber_proj_mode_t_validate(phymod_ber_proj_mode_t phymod_ber_proj_mode);
/*! 
 * phymod_phy_ber_proj
 *
 * @brief  BER Projection 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  mode            - BER projection mode
 * @param [in]  options         - BER projection options
 */
int phymod_phy_ber_proj(const phymod_phy_access_t* phy, phymod_ber_proj_mode_t mode, const phymod_phy_ber_proj_options_t* options);

#endif /*_PHYMOD_DIAGNOSTICS_H_*/
