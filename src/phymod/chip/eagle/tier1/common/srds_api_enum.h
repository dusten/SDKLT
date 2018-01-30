/******************************************************************************
******************************************************************************
*  Revision      :  $Id: srds_api_enum.h 977 2015-03-19 02:24:29Z kirand $ *
*                                                                            *
*  Description   :  Enum types used by Serdes API functions                  *
*                                                                            *
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
*                                                       *
*  No portions of this material may be reproduced in any form without        *
*  the written permission of:                                                *
*      Broadcom Corporation                                                  *
*      5300 California Avenue                                                *
*      Irvine, CA  92617                                                     *
*                                                                            *
*  All information contained in this document is Broadcom Corporation        *
*  company private proprietary, and trade secret.                            *
 */

/** @file srds_api_enum.h
 * Enum types used by Serdes API functions
   This file contains Enum's which are common to all serdes cores
 */

#ifndef SRDS_API_ENUM_H
#define SRDS_API_ENUM_H

/** PRBS Polynomial Enum */
enum srds_prbs_polynomial_enum {
	PRBS_7  = 0,
	PRBS_9  = 1,
	PRBS_11 = 2,
	PRBS_15 = 3,
	PRBS_23 = 4,
	PRBS_31 = 5,
	PRBS_58 = 6
};


/** PRBS Checker Mode Enum */
enum srds_prbs_checker_mode_enum {
	PRBS_SELF_SYNC_HYSTERESIS       = 0,
	PRBS_INITIAL_SEED_HYSTERESIS    = 1,
	PRBS_INITIAL_SEED_NO_HYSTERESIS = 2
};


/** TX_PI Fixed Frequency and Jitter Generation Modes Enum */
enum srds_tx_pi_freq_jit_gen_enum {
	TX_PI_FIXED_FREQ      = 0,
	TX_PI_SSC_LOW_FREQ    = 1,
	TX_PI_SSC_HIGH_FREQ   = 2,
	TX_PI_SJ              = 3
};

/** RX AFE Settings Enum */
enum srds_rx_afe_settings_enum {
	RX_AFE_PF,
	RX_AFE_PF2,
	RX_AFE_VGA,
	RX_AFE_DFE1,
	RX_AFE_DFE2,
	RX_AFE_DFE3,
	RX_AFE_DFE4,
	RX_AFE_DFE5,
	RX_AFE_DFE6,
	RX_AFE_DFE7,
	RX_AFE_DFE8,
	RX_AFE_DFE9,
	RX_AFE_DFE10,
	RX_AFE_DFE11,
	RX_AFE_DFE12,
	RX_AFE_DFE13,
	RX_AFE_DFE14
};

/** TX AFE Settings Enum */
enum srds_tx_afe_settings_enum {
	TX_AFE_PRE,
	TX_AFE_MAIN,
	TX_AFE_POST1,
	TX_AFE_POST2,
	TX_AFE_POST3,
	TX_AFE_AMP,
	TX_AFE_DRIVERMODE
};

/** AFE Drivermode Enum */
enum srds_afe_drivermode_enum {
	DM_DEFAULT = 0,
	DM_HALF_AMPLITUDE_HI_IMPED,
	DM_HALF_AMPLITUDE,
	DM_NOT_SUPPORTED
};

/** Repeater Mode Enum */
enum srds_rptr_mode_enum {
	DATA_IN_SIDE,
	DIG_LPBK_SIDE,
	RMT_LPBK_SIDE
};

/** Core Powerdown Modes Enum */
enum srds_core_pwrdn_mode_enum {
	PWR_ON,
	PWRDN,
	PWRDN_DEEP,
	PWRDN_TX,
	PWRDN_RX
};

/** Evenet Log Display Mode Enum */
enum srds_event_log_display_mode_enum {
	EVENT_LOG_HEX,
	EVENT_LOG_DECODED,
	EVENT_LOG_HEX_AND_DECODED,
	EVENT_LOG_MODE_MAX
};

/** Eye Scan Direction Enum */
enum srds_eye_scan_dir_enum {
	EYE_SCAN_HORIZ = 0,
	EYE_SCAN_VERTICAL = 1
};


/* Used by #serdes_display_diag_data to select diag_level             */
/* example:                                                           */
/*  diag_level = SRDS_DIAG_LANE | SRDS_DIAG_CORE | SRDS_DIAG_EVENT    */
enum srds_diag_level_enum {
	SRDS_DIAG_LANE = 1,
	SRDS_DIAG_CORE = 1 << 1,
	SRDS_DIAG_EVENT = 1 << 2,
	SRDS_DIAG_EYE = 1 << 3,
	SRDS_DIAG_REG_CORE = 1 << 4,
	SRDS_DIAG_REG_LANE = 1 << 5,
	SRDS_DIAG_UC_CORE = 1 << 6,
	SRDS_DIAG_UC_LANE = 1 << 7,
	SRDS_DIAG_LANE_DEBUG = 1 << 8,
	SRDS_DIAG_BER_VERT = 1 << 9,
	SRDS_DIAG_BER_HORZ = 1 << 10
};

/* Used by #serdes_diag_access to select type of access */
enum srds_diag_access_enum {
	SRDS_REG_READ,
	SRDS_REG_RMW,
	SRDS_CORE_RAM_READ_BYTE,
	SRDS_CORE_RAM_RMW_BYTE,
	SRDS_CORE_RAM_READ_WORD,
	SRDS_CORE_RAM_RMW_WORD,
	SRDS_LANE_RAM_READ_BYTE,
	SRDS_LANE_RAM_RMW_BYTE,
	SRDS_LANE_RAM_READ_WORD,
	SRDS_LANE_RAM_RMW_WORD,
	SRDS_GLOB_RAM_READ_BYTE,
	SRDS_GLOB_RAM_RMW_BYTE,
	SRDS_GLOB_RAM_READ_WORD,
	SRDS_GLOB_RAM_RMW_WORD,
	SRDS_UC_CMD,
	SRDS_EN_BREAKPOINT,
	SRDS_GOTO_BREAKPOINT,
	SRDS_RD_BREAKPOINT,
	SRDS_DIS_BREAKPOINT,
	SRDS_BER_PROJ_DATA
};

#endif
