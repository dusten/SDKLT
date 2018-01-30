/*----------------------------------------------------------------------
 * $Id: eagle_cfg_seq.h,v 1.1.2.2 2013/09/17 21:11:10 wniu Exp $ 
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
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : eagle_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 *  $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  $Id$
*/


#ifndef EAGLE_CFG_SEQ_H 
#define EAGLE_CFG_SEQ_H

#include "common/srds_api_err_code.h"
#include "eagle_tsc_enum.h"

typedef struct {
  int8_t pll_pwrdn;
  int8_t tx_s_pwrdn;
  int8_t rx_s_pwrdn;
} power_status_t;

typedef struct {
  int8_t revid_model;
  int8_t revid_process;
  int8_t revid_bonding;
  int8_t revid_rev_number;
  int8_t revid_rev_letter;
} eagle_rev_id0_t;


typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp;
  int8_t revid_pir;
  int8_t revid_cl72;
  int8_t revid_micro;
  int8_t revid_mdio;
  int8_t revid_multiplicity;
} eagle_rev_id1_t;


typedef enum {
    EAGLE_PRBS_POLYNOMIAL_7 = 0,
    EAGLE_PRBS_POLYNOMIAL_9,
    EAGLE_PRBS_POLYNOMIAL_11,
    EAGLE_PRBS_POLYNOMIAL_15,
    EAGLE_PRBS_POLYNOMIAL_23,
    EAGLE_PRBS_POLYNOMIAL_31,
    EAGLE_PRBS_POLYNOMIAL_58,
    EAGLE_PRBS_POLYNOMIAL_TYPE_COUNT 
}eagle_prbs_polynomial_type_t;

#define PATTERN_MAX_SIZE 8

err_code_t eagle_tsc_identify(const phymod_access_t *pa, eagle_rev_id0_t *rev_id0, eagle_rev_id1_t *rev_id1);

err_code_t eagle_uc_active_set(const phymod_access_t *pa, uint32_t enable);           /* set microcontroller active or not  */
err_code_t eagle_uc_active_get(const phymod_access_t *pa, uint32_t *enable);          /* get microcontroller active or not  */
err_code_t eagle_uc_reset(const phymod_access_t *pa, uint32_t enable);           /* set dw8501 reset  */
err_code_t eagle_prbs_tx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t eagle_prbs_rx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t eagle_prbs_tx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly);
err_code_t eagle_prbs_rx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly);
err_code_t eagle_prbs_tx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_prbs_rx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_pll_mode_set(const phymod_access_t *pa, int pll_mode);   /* PLL divider set */
err_code_t eagle_core_soft_reset_release(const phymod_access_t *pa, uint32_t enable);   /* release the pmd core soft reset */
err_code_t eagle_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable);   /* release the pmd core soft reset */
err_code_t eagle_pram_firmware_enable(const phymod_access_t *pa, int enable);   /* release the pmd core soft reset */
err_code_t eagle_pmd_force_signal_detect(const phymod_access_t *pa, int enable); /*force the signal detect */   
err_code_t eagle_pmd_loopback_get(const phymod_access_t *pa, uint32_t *enable);   
err_code_t eagle_uc_init(const phymod_access_t *pa);
err_code_t eagle_pram_flop_set(const phymod_access_t *pa, int value);
err_code_t eagle_tsc_ucode_init( const phymod_access_t *pa );
err_code_t eagle_pmd_ln_h_rstb_pkill_override( const phymod_access_t *pa, uint16_t val); 
err_code_t eagle_pmd_cl72_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_pmd_cl72_receiver_status(const phymod_access_t *pa, uint32_t *status);
err_code_t eagle_tx_pi_control_get( const phymod_access_t *pa,  int16_t *value);
   

err_code_t eagle_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol);
err_code_t eagle_tx_rx_polarity_get(const phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol);
err_code_t eagle_force_tx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t eagle_force_tx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t eagle_force_rx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t eagle_force_rx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t eagle_pll_mode_get(const phymod_access_t *pa, uint32_t *pll_mode);
err_code_t eagle_osr_mode_set(const phymod_access_t *pa, int osr_mode);
err_code_t eagle_osr_mode_get(const phymod_access_t *pa, int *osr_mode);
err_code_t eagle_tsc_dig_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t eagle_tsc_rmt_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t eagle_core_soft_reset(const phymod_access_t *pa);
err_code_t eagle_tsc_pwrdn_set(const phymod_access_t *pa, int tx_rx, int pwrdn);
err_code_t eagle_tsc_pwrdn_get(const phymod_access_t *pa, power_status_t *pwrdn);
err_code_t eagle_pcs_lane_swap_tx(const phymod_access_t *pa, uint32_t tx_lane_map); 
err_code_t eagle_pmd_lane_swap (const phymod_access_t *pa, uint32_t lane_map); 
err_code_t eagle_pmd_lane_swap_tx_get (const phymod_access_t *pa, uint32_t *lane_map); 
err_code_t eagle_lane_hard_soft_reset_release(const phymod_access_t *pa, uint32_t enable); 
err_code_t eagle_clause72_control(const phymod_access_t *pa, uint32_t cl_72_en);          
err_code_t eagle_clause72_control_get(const phymod_access_t *pa, uint32_t *cl_72_en);    
err_code_t eagle_pmd_reset_seq(const phymod_access_t *pa, int pmd_touched);
err_code_t eagle_pll_reset_enable_set (const phymod_access_t *pa, int enable);
err_code_t eagle_tsc_read_pll_range (const phymod_access_t *pa, uint32_t *pll_range);
err_code_t eagle_tsc_signal_detect (const phymod_access_t *pa, uint32_t *signal_detect);
err_code_t eagle_tsc_ladder_setting_to_mV(const phymod_access_t *pa, int8_t y, int16_t* level);
err_code_t eagle_pll_config_get(const phymod_access_t *pa, enum eagle_tsc_pll_enum *pll_mode);
err_code_t eagle_tx_lane_control_set(const phymod_access_t *pa, uint32_t en);
err_code_t eagle_tx_lane_control_get(const phymod_access_t *pa, uint32_t* en);
err_code_t eagle_rx_lane_control_set(const phymod_access_t *pa, uint32_t en);
err_code_t eagle_rx_lane_control_get(const phymod_access_t *pa, uint32_t* en);
err_code_t eagle_tsc_get_vco (const phymod_phy_inf_config_t* config, uint32_t *vco_rate, enum eagle_tsc_pll_enum *new_pll_div, int16_t *new_os_mode);
err_code_t eagle_pmd_tx_disable_pin_dis_set(const phymod_access_t *pa, uint32_t enable);
err_code_t eagle_pmd_tx_disable_pin_dis_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_tsc_tx_shared_patt_gen_en_get( const phymod_access_t *pa, uint8_t *enable);
err_code_t eagle_tsc_config_shared_tx_pattern_idx_set( const phymod_access_t *pa, const uint32_t *pattern_len);
err_code_t eagle_tsc_config_shared_tx_pattern_idx_get( const phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern);

#endif /* PHY_TSC_IBLK_H */
