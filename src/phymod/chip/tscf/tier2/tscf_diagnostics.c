/*
 *         
 * $Id: tscf_diagnostics.c,v 1.1.2.8 2013/08/07 11:03:37 mlarsen Exp $
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
 */

#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/tscf_diagnostics.h>
#include "../../tscf/tier1/tefmod_enum_defines.h" 
#include "../../tscf/tier1/tefmod.h" 
#include "../../falcon/tier1/falcon_tsc_common.h" 
#include "../../falcon/tier1/falcon_tsc_interface.h" 
#include "../../falcon/tier1/falcon_cfg_seq.h"
#include "../../falcon/tier1/falcon_tsc_debug_functions.h" 


#define PATTERN_MAX_LENGTH 240
#define TSCF_NOF_LANES_IN_CORE  (4)

#ifdef PHYMOD_TSCF_SUPPORT



/*phymod, internal enum mappings*/
STATIC
int _tscf_prbs_poly_phymod_to_falcon(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *falcon_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *falcon_poly = PRBS_7;
        break;
    case phymodPrbsPoly9:
        *falcon_poly = PRBS_9;
        break;
    case phymodPrbsPoly11:
        *falcon_poly = PRBS_11;
        break;
    case phymodPrbsPoly15:
        *falcon_poly = PRBS_15;
        break;
    case phymodPrbsPoly23:
        *falcon_poly = PRBS_23;
        break;
    case phymodPrbsPoly31:
        *falcon_poly = PRBS_31;
        break;
    case phymodPrbsPoly58:
        *falcon_poly = PRBS_58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for tscf %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tscf_prbs_poly_tscf_to_phymod(falcon_prbs_polynomial_type_t tscf_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(tscf_poly){
    case FALCON_PRBS_POLYNOMIAL_7:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case FALCON_PRBS_POLYNOMIAL_9:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case FALCON_PRBS_POLYNOMIAL_11:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case FALCON_PRBS_POLYNOMIAL_15:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case FALCON_PRBS_POLYNOMIAL_23:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case FALCON_PRBS_POLYNOMIAL_31:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case FALCON_PRBS_POLYNOMIAL_58:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), tscf_poly));
    }
    return PHYMOD_E_NONE;
}

#ifdef PHYMOD_TO_TSCE_OS_MODE_TRANS
STATIC
int _tscf_os_mode_phymod_to_tscf(phymod_osr_mode_t phymod_os_mode, tefmod_os_mode_type *tscf_os_mode)
{
    switch(phymod_os_mode){
    case phymodOversampleMode1:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_1;
        break;
    case phymodOversampleMode2:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_2;
        break;
    case phymodOversampleMode3:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_3;
        break;
    case phymodOversampleMode3P3:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_3_3;
        break;
    case phymodOversampleMode4:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_4;
        break;
    case phymodOversampleMode5:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_5;
        break;
    case phymodOversampleMode8:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_8;
        break;
    case phymodOversampleMode8P25:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_8_25;
        break;
    case phymodOversampleMode10:
        *tscf_os_mode = TEFMOD_PMA_OS_MODE_10;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported over sample mode for tscf %u"), phymod_os_mode));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tscf_os_mode_tscf_to_phymod(tefmod_os_mode_type tscf_os_mode, phymod_osr_mode_t *phymod_os_mode)
{
    switch(tscf_os_mode){
    case TEFMOD_PMA_OS_MODE_1:
        *phymod_os_mode = phymodOversampleMode1;
        break;
    case TEFMOD_PMA_OS_MODE_2:
        *phymod_os_mode = phymodOversampleMode2;
        break;
    case TEFMOD_PMA_OS_MODE_3:
        *phymod_os_mode = phymodOversampleMode3;
        break;
    case TEFMOD_PMA_OS_MODE_3_3:
        *phymod_os_mode = phymodOversampleMode3P3;
        break;
    case TEFMOD_PMA_OS_MODE_4:
        *phymod_os_mode = phymodOversampleMode4;
        break;
    case TEFMOD_PMA_OS_MODE_5:
        *phymod_os_mode = phymodOversampleMode5;
        break;
    case TEFMOD_PMA_OS_MODE_8:
        *phymod_os_mode = phymodOversampleMode8;
        break;
    case TEFMOD_PMA_OS_MODE_8_25:
        *phymod_os_mode = phymodOversampleMode8P25;
        break;
    case TEFMOD_PMA_OS_MODE_10:
        *phymod_os_mode = phymodOversampleMode10;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown os mode %u"), tscf_os_mode));
    }
    return PHYMOD_E_NONE;
}
#endif

/*diagnotics functions*/

int tscf_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}

int tscf_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int tscf_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}

int tscf_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum falcon_poly;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    PHYMOD_IF_ERR_RETURN(_tscf_prbs_poly_phymod_to_falcon(prbs->poly, &falcon_poly));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check which direction */
    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_config_rx_prbs(&phy_copy.access, falcon_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
        }
    } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_config_tx_prbs(&phy_copy.access, falcon_poly, prbs->invert));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_config_rx_prbs(&phy_copy.access, falcon_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
            PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_config_tx_prbs(&phy_copy.access, falcon_poly, prbs->invert));
        }
    }         
    return PHYMOD_E_NONE;
}

int tscf_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_prbs_t config_tmp;
    falcon_prbs_polynomial_type_t tscf_poly;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_poly_get(&phy->access, &tscf_poly));
        PHYMOD_IF_ERR_RETURN(_tscf_prbs_poly_tscf_to_phymod(tscf_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_poly_get(&phy->access, &tscf_poly));
        PHYMOD_IF_ERR_RETURN(_tscf_prbs_poly_tscf_to_phymod(tscf_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_poly_get(&phy->access, &tscf_poly));
        PHYMOD_IF_ERR_RETURN(_tscf_prbs_poly_tscf_to_phymod(tscf_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return PHYMOD_E_NONE;
}



int tscf_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(falcon_tsc_tx_prbs_en(&phy_copy.access, enable));
        }
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(falcon_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(falcon_tsc_tx_prbs_en(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(falcon_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    }
    return PHYMOD_E_NONE;   
}

int tscf_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t *enable)
{
    uint32_t enable_tmp;
    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_enable_get(&phy->access, &enable_tmp));
        *enable &= enable_tmp;
    }

    return PHYMOD_E_NONE;
}


int tscf_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    uint8_t status = 0;
    uint32_t prbs_err_count = 0;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    prbs_status->prbs_lock = 0;
    prbs_status->error_count = 0;
    prbs_status->prbs_lock_loss = 0;
    prbs_status->prbs_lock = 1;

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(falcon_tsc_prbs_chk_lock_state(&phy_copy.access, &status));
    if (status) {
        /*next check the lost of lock and error count */
        status = 0;
        PHYMOD_IF_ERR_RETURN
                (falcon_tsc_prbs_err_count_state(&phy_copy.access, &prbs_err_count, &status));
        PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS Error count :: %d\n", i, prbs_err_count));
        if (status) {
        /*temp lost of lock */
            prbs_status->prbs_lock_loss = 1;
        } else {
                prbs_status->error_count += prbs_err_count;
        }
    } else {
            PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS not locked\n", i ));
            prbs_status->prbs_lock = 0;
            /* return PHYMOD_E_NONE; */
        }
    }
    return PHYMOD_E_NONE;
}


int tscf_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
    int i,j = 0, bit;
    char patt[PATTERN_MAX_LENGTH+1];
    for (i=0; i< PATTERN_MAX_SIZE; i++)
    {
      for (j=0;j<32 && i*32+j <= PATTERN_MAX_LENGTH; j++)
      {
          if (i*32+j == (int)pattern->pattern_len) {
              break;
          }
          bit = pattern->pattern[i] >> j & 00000001;
          switch (bit) {
          case (1):
              patt[i*32+j] = '1';
              break;
          default:
              patt[i*32+j] = '0';
              break;
          }
      }
      if (i*32+j == (int)pattern->pattern_len && i*32+j <= PATTERN_MAX_LENGTH) {
          /* coverity[overrun-local] */
          patt[i*32+j] = '\0';
          break;
      }
    }
    PHYMOD_IF_ERR_RETURN
            (falcon_tsc_config_shared_tx_pattern_idx_set(&phy->access,
                                  &pattern->pattern_len ));
    PHYMOD_IF_ERR_RETURN 
            (falcon_tsc_config_shared_tx_pattern (&phy->access, 
				(uint8_t) pattern->pattern_len, (const char *) patt));
    return PHYMOD_E_NONE;
}




int tscf_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_config_shared_tx_pattern_idx_get(&phy->access,
                                  &pattern->pattern_len, 
                                  pattern->pattern));
    return PHYMOD_E_NONE;
}

int tscf_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_tx_shared_patt_gen_en(&phy->access, (uint8_t) enable, (uint8_t)pattern->pattern_len));

    return PHYMOD_E_NONE;
}

int tscf_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN
       (falcon_tsc_tx_shared_patt_gen_en_get(&phy->access, (uint8_t *) enable));
        
    return PHYMOD_E_NONE;
}

int tscf_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
    int16_t temperature;
    falcon_tsc_read_die_temperature(&core->access,&temperature); 
    diag->temperature = temperature ;
    falcon_tsc_read_pll_range(&core->access, &diag->pll_range);
        
    return PHYMOD_E_NONE;
    
}

int tscf_phy_pmd_info_dump(const phymod_phy_access_t* phy, char *arg)
{
    int start_lane, num_lane;
    int tmp_lane_mask;
    phymod_phy_access_t phy_copy;
    struct falcon_tsc_detailed_lane_status_st lane_st[4] = {{0}};
    int i = 0;
    int j = 0;
    int tmp_ln_msk;
    char *cmd_str = (char*)arg;
    uint32_t type;
    uint8_t trace_mem[768];
    int is_in_reset = 0;

    if (!cmd_str) {
        type = (uint32_t)TEFMOD_DIAG_DSC;
    } else if((!PHYMOD_STRCMP(cmd_str,"ber"))||(!PHYMOD_STRCMP(cmd_str,"Ber"))||(!PHYMOD_STRCMP(cmd_str,"BER"))) {
        type = (uint32_t)TEFMOD_DIAG_BER;
    } else if((!PHYMOD_STRCMP(cmd_str,"config"))||(!PHYMOD_STRCMP(cmd_str,"Config"))||(!PHYMOD_STRCMP(cmd_str,"CONFIG"))) {
        type = (uint32_t)TEFMOD_DIAG_CFG;
    } else if((!PHYMOD_STRCMP(cmd_str,"cl72"))||(!PHYMOD_STRCMP(cmd_str,"Cl72"))||(!PHYMOD_STRCMP(cmd_str,"CL72"))) {
        type = (uint32_t)TEFMOD_DIAG_CL72;
    } else if((!PHYMOD_STRCMP(cmd_str,"debug"))||(!PHYMOD_STRCMP(cmd_str,"Debug"))||(!PHYMOD_STRCMP(cmd_str,"DEBUG"))) {
        type = (uint32_t)TEFMOD_DIAG_DEBUG;
    } else if((!PHYMOD_STRCMP(cmd_str,"state"))||(!PHYMOD_STRCMP(cmd_str,"State"))||(!PHYMOD_STRCMP(cmd_str,"STATE"))) {
        type = (uint32_t)TEFMOD_DIAG_STATE;
    } else if((!PHYMOD_STRCMP(cmd_str,"verbose"))||(!PHYMOD_STRCMP(cmd_str,"Verbose"))||(!PHYMOD_STRCMP(cmd_str,"VERBOSE"))) {
        type = (uint32_t)TEFMOD_DIAG_ALL;
    } else if (!PHYMOD_STRCMP(cmd_str,"STD")){
        type = (uint32_t)TEFMOD_DIAG_DSC_STD;
    } else {
        type = (uint32_t)TEFMOD_DIAG_STATE;
    }

    PHYMOD_DEBUG_ERROR((" %s:%d type = %d laneMask  = 0x%X, Address = 0x%X\n", __func__, __LINE__, type, phy->access.lane_mask, phy->access.addr));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if(type==TEFMOD_DIAG_DSC) {
       PHYMOD_IF_ERR_RETURN
           (falcon_tsc_display_core_state(&phy_copy.access));
       PHYMOD_IF_ERR_RETURN
           (falcon_tsc_display_lane_state_hdr(&phy_copy.access)); 
       for (i = 0; i < 4; i++) {
            phy_copy.access.lane_mask = 0x1 << i ;
            PHYMOD_IF_ERR_RETURN
                (tefmod_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if(is_in_reset) {
                PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", i));
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_display_lane_state(&phy_copy.access));
       }
    } else if(type==TEFMOD_DIAG_DSC_STD) {
        PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
        PHYMOD_DIAG_OUT(("    | DSC Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy->access.addr, phy->access.lane_mask));
        PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
        PHYMOD_IF_ERR_RETURN
            (falcon_tsc_display_core_state(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (falcon_tsc_display_lane_state_hdr(&phy_copy.access)); 
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (tefmod_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if(is_in_reset) {
                PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", (i + start_lane)));
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_display_lane_state(&phy_copy.access));
        }
        tmp_ln_msk = phy_copy.access.lane_mask;
        for (j = 0; j < 4; j++) {
            phy_copy.access.lane_mask = 0x1 << j;
            PHYMOD_IF_ERR_RETURN
                (tefmod_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if(is_in_reset) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_log_full_pmd_state(&phy_copy.access, &lane_st[j]));
        }
        phy_copy.access.lane_mask = tmp_ln_msk;
        PHYMOD_IF_ERR_RETURN
            (falcon_tsc_disp_full_pmd_state(&phy_copy.access, lane_st, 4));
        PHYMOD_IF_ERR_RETURN
            (falcon_tsc_read_event_log((&phy_copy.access), trace_mem, 2));
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            
            switch(type) {
            case TEFMOD_DIAG_CFG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if(i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (falcon_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_config(&phy_copy.access));
                break;
                
            case TEFMOD_DIAG_CL72:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_cl93n72_status(&phy_copy.access));
                break;
                
            case TEFMOD_DIAG_DEBUG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_debug_status(&phy_copy.access));
                break;
                
            case TEFMOD_DIAG_BER:
                PHYMOD_DEBUG_ERROR((" %s:%d type = BER\n", __func__, __LINE__));
                break;
                
            /*
             * COVERITY
             *
             * TEFMOD_DIAG_ALL branch involve information in TEFMOD_DIAG_STATE branch
             */
            /* coverity[unterminated_case] */
            case TEFMOD_DIAG_ALL:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if(i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (falcon_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_config(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_cl93n72_status(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_debug_status(&phy_copy.access));

            case TEFMOD_DIAG_STATE:
            default:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DEF\n", __func__, __LINE__));
                 PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_core_state_hdr(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_core_state_line(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_core_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_state_hdr(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_state(&phy_copy.access));
                tmp_ln_msk = phy_copy.access.lane_mask;
                for (j = 0; j < 4; j++) {
                     phy_copy.access.lane_mask = 0x1 << j;
                     PHYMOD_IF_ERR_RETURN
                         (tefmod_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
                     if(is_in_reset) {
                         PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", j));
                         continue;
                     }
                     PHYMOD_IF_ERR_RETURN
                         (falcon_tsc_log_full_pmd_state(&phy_copy.access, &lane_st[j]));
                 }
                phy_copy.access.lane_mask = tmp_ln_msk;
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_disp_full_pmd_state(&phy_copy.access, lane_st, 4));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_read_event_log((&phy_copy.access), trace_mem, 2));
                break;
            }
        }
    }
    return PHYMOD_E_NONE;
}

int tscf_phy_pcs_info_dump(const phymod_phy_access_t* phy, char *arg)
{
    phymod_phy_access_t phy_copy;


    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN (tefmod_diag_disp(&phy_copy.access, (char*)arg));
 
    return PHYMOD_E_NONE;
}


int tscf_phy_fec_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    uint32_t lane_count;
    uint32_t sum = 0;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(tefmod_fec_correctable_counter_get(&phy_copy.access, &lane_count));
        /* Check overflow */
        if (lane_count > 0xffffffff - sum) {
            sum = 0xffffffff;
            break;
        } else {
            sum += lane_count;
        }
    }

    *count = sum;

    return PHYMOD_E_NONE;
}

int tscf_phy_fec_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    uint32_t lane_count;
    uint32_t sum = 0;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(tefmod_fec_uncorrectable_counter_get(&phy_copy.access, &lane_count));
        /* Check overflow */
        if (lane_count > 0xffffffff - sum) {
            sum = 0xffffffff;
            break;
        } else {
            sum += lane_count;
        }
    }

    *count = sum;

    return PHYMOD_E_NONE;
}

int tscf_phy_fec_cl91_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_fec_cl91_correctable_counter_get(&phy_copy.access, count));

    return PHYMOD_E_NONE;
}

int tscf_phy_fec_cl91_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_fec_cl91_uncorrectable_counter_get(&phy_copy.access, count));

    return PHYMOD_E_NONE;
}

int tscf_phy_stat_get(const phymod_phy_access_t* phy, phymod_phy_stat_t* stat)
{
    phymod_phy_access_t phy_copy;
    tefmod_encode_mode encode_mode;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < TSCF_NOF_LANES_IN_CORE; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, i)) {
            continue;
        }

        phy_copy.access.lane_mask = 0x1 << i;

        PHYMOD_IF_ERR_RETURN(tefmod_encode_mode_get(&phy_copy.access, &encode_mode));
        if (encode_mode == TEFMOD_ENCODE_MODE_CL49) {
            /* CL49 has no BIP count */
            stat->pcs_bip_err_count[i] = 0;
            PHYMOD_IF_ERR_RETURN(tefmod_cl49_ber_counter_get(&phy_copy.access, &stat->pcs_ber_count[i]));
        }
        else if (encode_mode == TEFMOD_ENCODE_MODE_CL82) {
            PHYMOD_IF_ERR_RETURN(tefmod_bip_error_counter_get(&phy_copy.access, &stat->pcs_bip_err_count[i]));
            PHYMOD_IF_ERR_RETURN(tefmod_cl82_ber_counter_get(&phy_copy.access, &stat->pcs_ber_count[i]));
        }
        else {
            stat->pcs_bip_err_count[i] = 0;
            stat->pcs_ber_count[i]     = 0;
        }
    }

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_TSCF_SUPPORT */
