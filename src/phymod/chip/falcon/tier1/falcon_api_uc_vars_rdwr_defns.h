/********************************************************************************************
********************************************************************************************
*                                                                                          *
*  Revision      :  $Id: falcon_api_uc_vars_rdwr_defns.h 1187 2015-10-19 20:35:15Z kirand $ *
*                                                                                          *
*  Description   :  Read/Write macros to Falcon IP's uC RAM's User variables               *
*                                                                                          *
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
*                                                                     *
*  No portions of this material may be reproduced in any form without                      *
*  the written permission of:                                                              *
*      Broadcom Corporation                                                                *
*      5300 California Avenue                                                              *
*      Irvine, CA  92617                                                                   *
*                                                                                          *
*  All information contained in this document is Broadcom Corporation                      *
*  company private proprietary, and trade secret.                                          *
 */

/** @file falcon_api_uc_vars_rdwr_defns.h
 * Read/Write macros to Falcon IP's uC RAM's User variables
 */

/* THIS FILE IS GENERATED USING AN AUTOMATED SCRIPT... PLEASE DO NOT EDIT THIS FILE DIRECTLY !!! */


#ifndef FALCON_API_UC_VARS_RDWR_DEFNS_H
#define FALCON_API_UC_VARS_RDWR_DEFNS_H

/************************************/
/*  Serdes IP RAM access functions  */
/************************************/

/* RAM access functions for lane_var_struct variables */
#define rdv_config_word()                                            falcon_tsc_rdwl_uc_var(pa, __ERR, 0x0)
#define wrv_config_word(wr_val)                                      falcon_tsc_wrwl_uc_var(pa, 0x0, wr_val)
#define rdv_usr_ctrl_retune_after_restart()                          falcon_tsc_rdbl_uc_var(pa, __ERR, 0x2)
#define wrv_usr_ctrl_retune_after_restart(wr_val)                    falcon_tsc_wrbl_uc_var(pa, 0x2, wr_val)
#define rdv_usr_ctrl_clk90_offset_adjust()                           falcon_tsc_rdbls_uc_var(pa, __ERR, 0x3)
#define wrv_usr_ctrl_clk90_offset_adjust(wr_val)                     falcon_tsc_wrbls_uc_var(pa, 0x3, wr_val)
#define rdv_usr_ctrl_clk90_offset_override()                         falcon_tsc_rdbl_uc_var(pa, __ERR, 0x4)
#define wrv_usr_ctrl_clk90_offset_override(wr_val)                   falcon_tsc_wrbl_uc_var(pa, 0x4, wr_val)
#define rdv_usr_ctrl_lane_event_log_level()                          falcon_tsc_rdbl_uc_var(pa, __ERR, 0x5)
#define wrv_usr_ctrl_lane_event_log_level(wr_val)                    falcon_tsc_wrbl_uc_var(pa, 0x5, wr_val)
#define rdv_usr_ctrl_vertical_threshold_adjust()                     falcon_tsc_rdbls_uc_var(pa, __ERR, 0x6)
#define wrv_usr_ctrl_vertical_threshold_adjust(wr_val)               falcon_tsc_wrbls_uc_var(pa, 0x6, wr_val)
#define rdv_usr_ctrl_cl93n72_frc_byte()                              falcon_tsc_rdbl_uc_var(pa, __ERR, 0x7)
#define wrv_usr_ctrl_cl93n72_frc_byte(wr_val)                        falcon_tsc_wrbl_uc_var(pa, 0x7, wr_val)
#define rdv_usr_ctrl_disable_startup_functions_word()                falcon_tsc_rdwl_uc_var(pa, __ERR, 0x8)
#define wrv_usr_ctrl_disable_startup_functions_word(wr_val)          falcon_tsc_wrwl_uc_var(pa, 0x8, wr_val)
#define rdv_usr_ctrl_disable_steady_state_functions_word()           falcon_tsc_rdwl_uc_var(pa, __ERR, 0xa)
#define wrv_usr_ctrl_disable_steady_state_functions_word(wr_val)     falcon_tsc_wrwl_uc_var(pa, 0xa, wr_val)
#define rdv_usr_ctrl_disable_startup_dfe_functions_byte()            falcon_tsc_rdbl_uc_var(pa, __ERR, 0xc)
#define wrv_usr_ctrl_disable_startup_dfe_functions_byte(wr_val)      falcon_tsc_wrbl_uc_var(pa, 0xc, wr_val)
#define rdv_usr_ctrl_disable_steady_state_dfe_functions_byte()       falcon_tsc_rdbl_uc_var(pa, __ERR, 0xd)
#define wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(wr_val) falcon_tsc_wrbl_uc_var(pa, 0xd, wr_val)
#define rdv_usr_sts_restart_counter()                                falcon_tsc_rdbl_uc_var(pa, __ERR, 0xe)
#define wrv_usr_sts_restart_counter(wr_val)                          falcon_tsc_wrbl_uc_var(pa, 0xe, wr_val)
#define rdv_usr_sts_reset_counter()                                  falcon_tsc_rdbl_uc_var(pa, __ERR, 0xf)
#define wrv_usr_sts_reset_counter(wr_val)                            falcon_tsc_wrbl_uc_var(pa, 0xf, wr_val)
#define rdv_usr_sts_pmd_lock_counter()                               falcon_tsc_rdbl_uc_var(pa, __ERR, 0x10)
#define wrv_usr_sts_pmd_lock_counter(wr_val)                         falcon_tsc_wrbl_uc_var(pa, 0x10, wr_val)
#define rdv_usr_sts_heye_left()                                      falcon_tsc_rdbl_uc_var(pa, __ERR, 0x11)
#define wrv_usr_sts_heye_left(wr_val)                                falcon_tsc_wrbl_uc_var(pa, 0x11, wr_val)
#define rdv_usr_sts_heye_right()                                     falcon_tsc_rdbl_uc_var(pa, __ERR, 0x12)
#define wrv_usr_sts_heye_right(wr_val)                               falcon_tsc_wrbl_uc_var(pa, 0x12, wr_val)
#define rdv_usr_sts_veye_upper()                                     falcon_tsc_rdbl_uc_var(pa, __ERR, 0x13)
#define wrv_usr_sts_veye_upper(wr_val)                               falcon_tsc_wrbl_uc_var(pa, 0x13, wr_val)
#define rdv_usr_sts_veye_lower()                                     falcon_tsc_rdbl_uc_var(pa, __ERR, 0x14)
#define wrv_usr_sts_veye_lower(wr_val)                               falcon_tsc_wrbl_uc_var(pa, 0x14, wr_val)
#define rdv_usr_sts_micro_stopped()                                  falcon_tsc_rdbl_uc_var(pa, __ERR, 0x15)
#define wrv_usr_sts_micro_stopped(wr_val)                            falcon_tsc_wrbl_uc_var(pa, 0x15, wr_val)
#define rdv_usr_sts_link_time()                                      falcon_tsc_rdwl_uc_var(pa, __ERR, 0x16)
#define wrv_usr_sts_link_time(wr_val)                                falcon_tsc_wrwl_uc_var(pa, 0x16, wr_val)
#define rdv_usr_diag_status()                                        falcon_tsc_rdwl_uc_var(pa, __ERR, 0x18)
#define wrv_usr_diag_status(wr_val)                                  falcon_tsc_wrwl_uc_var(pa, 0x18, wr_val)
#define rdv_usr_diag_rd_ptr()                                        falcon_tsc_rdbl_uc_var(pa, __ERR, 0x1a)
#define wrv_usr_diag_rd_ptr(wr_val)                                  falcon_tsc_wrbl_uc_var(pa, 0x1a, wr_val)
#define rdv_usr_diag_mode()                                          falcon_tsc_rdbl_uc_var(pa, __ERR, 0x1b)
#define wrv_usr_diag_mode(wr_val)                                    falcon_tsc_wrbl_uc_var(pa, 0x1b, wr_val)
#define rdv_usr_main_tap_est()                                       falcon_tsc_rdwls_uc_var(pa, __ERR, 0x1c)
#define wrv_usr_main_tap_est(wr_val)                                 falcon_tsc_wrwls_uc_var(pa, 0x1c, wr_val)
#define rdv_usr_sts_phase_hoffset()                                  falcon_tsc_rdbls_uc_var(pa, __ERR, 0x1e)
#define wrv_usr_sts_phase_hoffset(wr_val)                            falcon_tsc_wrbls_uc_var(pa, 0x1e, wr_val)

/* RAM access functions for core_var_struct variables */
#define rdcv_config_word()                                           falcon_tsc_rdwc_uc_var(pa, __ERR, 0x0)
#define wrcv_config_word(wr_val)                                     falcon_tsc_wrwc_uc_var(pa, 0x0, wr_val)
#define rdcv_temp_frc_val()                                          falcon_tsc_rdwc_uc_var(pa, __ERR, 0x2)
#define wrcv_temp_frc_val(wr_val)                                    falcon_tsc_wrwc_uc_var(pa, 0x2, wr_val)
#define rdcv_common_ucode_version()                                  falcon_tsc_rdwc_uc_var(pa, __ERR, 0x4)
#define wrcv_common_ucode_version(wr_val)                            falcon_tsc_wrwc_uc_var(pa, 0x4, wr_val)
#define rdcv_avg_tmon_reg13bit()                                     falcon_tsc_rdwc_uc_var(pa, __ERR, 0x6)
#define wrcv_avg_tmon_reg13bit(wr_val)                               falcon_tsc_wrwc_uc_var(pa, 0x6, wr_val)
#define rdcv_trace_mem_rd_idx()                                      falcon_tsc_rdwc_uc_var(pa, __ERR, 0x8)
#define wrcv_trace_mem_rd_idx(wr_val)                                falcon_tsc_wrwc_uc_var(pa, 0x8, wr_val)
#define rdcv_trace_mem_wr_idx()                                      falcon_tsc_rdwc_uc_var(pa, __ERR, 0xa)
#define wrcv_trace_mem_wr_idx(wr_val)                                falcon_tsc_wrwc_uc_var(pa, 0xa, wr_val)
#define rdcv_temp_idx()                                              falcon_tsc_rdbc_uc_var(pa, __ERR, 0xc)
#define wrcv_temp_idx(wr_val)                                        falcon_tsc_wrbc_uc_var(pa, 0xc, wr_val)
#define rdcv_usr_ctrl_core_event_log_level()                         falcon_tsc_rdbc_uc_var(pa, __ERR, 0xd)
#define wrcv_usr_ctrl_core_event_log_level(wr_val)                   falcon_tsc_wrbc_uc_var(pa, 0xd, wr_val)
#define rdcv_common_ucode_minor_version()                            falcon_tsc_rdbc_uc_var(pa, __ERR, 0xe)
#define wrcv_common_ucode_minor_version(wr_val)                      falcon_tsc_wrbc_uc_var(pa, 0xe, wr_val)
#define rdcv_afe_hardware_version()                                  falcon_tsc_rdbc_uc_var(pa, __ERR, 0xf)
#define wrcv_afe_hardware_version(wr_val)                            falcon_tsc_wrbc_uc_var(pa, 0xf, wr_val)
#define rdcv_status_byte()                                           falcon_tsc_rdbc_uc_var(pa, __ERR, 0x10)
#define wrcv_status_byte(wr_val)                                     falcon_tsc_wrbc_uc_var(pa, 0x10, wr_val)
#define rdcv_diag_max_time_control()                                 falcon_tsc_rdbc_uc_var(pa, __ERR, 0x11)
#define wrcv_diag_max_time_control(wr_val)                           falcon_tsc_wrbc_uc_var(pa, 0x11, wr_val)
#define rdcv_diag_max_err_control()                                  falcon_tsc_rdbc_uc_var(pa, __ERR, 0x12)
#define wrcv_diag_max_err_control(wr_val)                            falcon_tsc_wrbc_uc_var(pa, 0x12, wr_val)
#define rdcv_misc_ctrl_byte()                                        falcon_tsc_rdbc_uc_var(pa, __ERR, 0x13)
#define wrcv_misc_ctrl_byte(wr_val)                                  falcon_tsc_wrbc_uc_var(pa, 0x13, wr_val)
#define rdcv_config_pll1_word()                                      falcon_tsc_rdwc_uc_var(pa, __ERR, 0x14)
#define wrcv_config_pll1_word(wr_val)                                falcon_tsc_wrwc_uc_var(pa, 0x14, wr_val)

#endif
