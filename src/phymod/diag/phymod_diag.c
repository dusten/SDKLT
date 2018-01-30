/* 
 * $Id: phymod_diag.c $ 
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
#include <phymod/phymod_diag.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>

#ifdef PHYMOD_QSGMIIE_SUPPORT
#include <phymod/chip/bcmi_qsgmiie_serdes_sym.h>
#endif /*PHYMOD_QSGMIIE_SUPPORT  */
#ifdef PHYMOD_TSCE_SUPPORT
#include <phymod/chip/bcmi_tsce_xgxs_sym.h>
#endif /*PHYMOD_TSCE_SUPPORT  */
#ifdef PHYMOD_TSCF_SUPPORT
#include <phymod/chip/bcmi_tscf_xgxs_sym.h> 
#include <phymod/chip/bcmi_tscf_xgxs_defs.h> 
#include <phymod/chip/bcmi_tscf_xgxs_gen2_sym.h>
#endif /*PHYMOD_TSCF_SUPPORT  */
#if 0 /* #ifdef PHYMOD_PHY8806X_SUPPORT */
#include <phymod/chip/bcmi_phy8806x_xgxs_sym.h>
#endif /*PHYMOD_PHY8806X_SUPPORT  */
#ifdef PHYMOD_EAGLE_SUPPORT
#include <phymod/chip/bcmi_eagle_xgxs_sym.h>
#endif /*PHYMOD_EAGLE_SUPPORT  */
#ifdef PHYMOD_FALCON_SUPPORT
#include <phymod/chip/bcmi_falcon_xgxs_sym.h>
#endif /*PHYMOD_FALCON_SUPPORT  */
#ifdef PHYMOD_VIPER_SUPPORT
#include <phymod/chip/bcmi_viper_xgxs_defs.h>
#include <phymod/chip/bcmi_viper_xgxs_sym.h>
#include <phymod/chip/bcmi_sgmiip2x4_serdes_sym.h>
#endif /*PHYMOD_VIPER_SUPPORT  */
#ifdef PHYMOD_TSCF16_SUPPORT
#include <phymod/chip/bcmi_tscf_16nm_xgxs_sym.h>
#endif /*PHYMOD_TSCF16_SUPPORT  */
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
#include <phymod/chip/bcmi_tsce_dpll_xgxs_sym.h>
#endif /*PHYMOD_TSCE_DPLL_SUPPORT  */
#ifdef PHYMOD_BLACKHAWK_SUPPORT
#include <phymod/chip/bcmi_blackhawk_xgxs_sym.h>
#endif /*PHYMOD_BLACKHAWK_SUPPORT  */
#ifdef PHYMOD_TSCE16_SUPPORT
#include <phymod/chip/bcmi_tsce16_xgxs_sym.h>
#endif
#ifdef PHYMOD_TSCBH_SUPPORT
#include <phymod/chip/bcmi_tscbh_xgxs_sym.h>
#endif /*PHYMOD_TSCBH_SUPPORT  */
#ifdef PHYMOD_TSCE16_SUPPORT
#include <phymod/chip/bcmi_tsce16_xgxs_sym.h>
#endif
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
#include <phymod/chip/bcmi_tscf_gen3_xgxs_sym.h>
#endif /*PHYMOD_TSCF_GEN3_SUPPORT  */
#ifdef PHYMOD_BLACKHAWK_SUPPORT
#include <phymod/chip/bcmi_blackhawk_xgxs_sym.h>
#endif /*PHYMOD_BLACKHAWK_SUPPORT  */

#define _PHYMOD_PHY_MSG(phy, string) ("Phy 0x%x lanes 0x%02x: " string), (phy)->access.addr, (phy)->access.lane_mask
#define _PHYMOD_CORE_MSG(core, string) ("Core 0x%x: " string), (core)->access.addr

#define PHYMOD_DIAG_PRINT_FUNC_VALIDATE if(phymod_diag_print_func == NULL) return PHYMOD_E_IO

print_func_f phymod_diag_print_func = NULL;


int phymod_diag_symbols_table_get(phymod_phy_access_t *phy, phymod_symbols_t **symbols)
{
    switch(phy->type){
#ifdef PHYMOD_QSGMIIE_SUPPORT
    case phymodDispatchTypeQsgmiie:
        *symbols = &bcmi_qsgmiie_serdes_symbols;
        break;
#endif /*PHYMOD_QSGMIIE_SUPPORT  */
#ifdef PHYMOD_TSCE_SUPPORT
    case phymodDispatchTypeTsce:
        *symbols = &bcmi_tsce_xgxs_symbols;
        break;
#endif /*PHYMOD_TSCE_SUPPORT  */
#ifdef PHYMOD_TSCF_SUPPORT
    case phymodDispatchTypeTscf:
    {
        /* first need to get the tscf model number */
      MAIN0_SERDESIDr_t serdesid;
      uint32_t model; 

      /* next check serdes ID to see if gen2 or not */
      READ_MAIN0_SERDESIDr(&phy->access, &serdesid);
      model = MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid);

      /* check if tscf_gen2 version */
      if (model == 0x15)  {
        *symbols = &bcmi_tscf_xgxs_gen2_symbols;
      } else {
        *symbols = &bcmi_tscf_xgxs_symbols;
      }
        break;
    }
#endif /*PHYMOD_TSCF_SUPPORT  */
#if 0 /* #ifdef PHYMOD_PHY8806X_SUPPORT */
    case phymodDispatchTypePhy8806x:
        *symbols = &bcmi_phy8806x_xgxs_symbols;
        break;
#endif /*PHYMOD_PHY8806X_SUPPORT  */
#ifdef PHYMOD_EAGLE_SUPPORT
    case phymodDispatchTypeEagle:
        *symbols = &bcmi_eagle_xgxs_symbols;
        break;
#endif /*PHYMOD_EAGLE_SUPPORT  */
#ifdef PHYMOD_FALCON_SUPPORT
    case phymodDispatchTypeFalcon:
        *symbols = &bcmi_falcon_xgxs_symbols;
        break;
#endif /*PHYMOD_FALCON_SUPPORT  */
#ifdef PHYMOD_VIPER_SUPPORT
    case phymodDispatchTypeViper:
    {
        SERDESID0r_t serdes_id;
        uint32_t model = 0;

        READ_SERDESID0r(&phy->access, &serdes_id);
        model = SERDESID0r_MODEL_NUMBERf_GET(serdes_id);

        /* check if SGMIIPLUS2x4 core */
        if (model == 0xf) {
            *symbols = &bcmi_sgmiip2x4_serdes_symbols;
        } else {
            *symbols = &bcmi_viper_xgxs_symbols;
        }

        break;
    }
#endif /*PHYMOD_VIPER_SUPPORT  */
#ifdef PHYMOD_TSCF16_SUPPORT
    case phymodDispatchTypeTscf16:
        *symbols = &bcmi_tscf_16nm_xgxs_symbols;
        break;
#endif /*PHYMOD_TSCF16_SUPPORT  */
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
    case phymodDispatchTypeTsce_dpll:
        *symbols = &bcmi_tsce_dpll_xgxs_symbols;
        break;
#endif
#ifdef PHYMOD_TSCE16_SUPPORT
    case phymodDispatchTypeTsce16:
        *symbols = &bcmi_tsce16_xgxs_symbols;
        break;
#endif
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
    case phymodDispatchTypeTscf_gen3:
        *symbols = &bcmi_tscf_gen3_xgxs_symbols;
        break;
#endif /*PHYMOD_TSCF_GEN3_SUPPORT  */
#ifdef PHYMOD_BLACKHAWK_SUPPORT
    case phymodDispatchTypeBlackhawk:
        *symbols = &bcmi_blackhawk_xgxs_symbols;
        break;
#endif
#ifdef PHYMOD_TSCBH_SUPPORT
    case phymodDispatchTypeTscbh:
        *symbols = &bcmi_tscbh_xgxs_symbols;
        break;
#endif
    default:
        PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(phy,"unsupported SerDes type(%d) for symbolic access \n"), phy->type));
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

/******************************************************************************
 Firmware load
 */

int phymod_diag_firmware_load(phymod_core_access_t *cores, int array_size, char *firwmware_file){
    int i = 0;

    /*unsigned short firmware_crc;
    unsigned short firmware_verision;
    unsigned short firmware_length;*/
    unsigned short phy_type = -1;
    
    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;
    
    
      
    for(i = 0 ; i < array_size ; i++){
        if(cores[i].type != phy_type){
            PHYMOD_DEBUG_ERROR((_PHYMOD_CORE_MSG(&cores[i],"The firmware is not compatible with the core\n")));
            return PHYMOD_E_FAIL;
        }
    }
    return PHYMOD_E_NONE;
}


/******************************************************************************
 Register read/write
 */
int phymod_diag_reg_read(phymod_phy_access_t *phys, int array_size, uint32_t reg_addr){
    uint32_t val = 0;
    int rv;
    int i;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;
    
    for(i = 0 ; i < array_size ; i++){
       rv = phymod_phy_reg_read(&phys[i], reg_addr, &val);
       if(rv == PHYMOD_E_NONE){
           PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Reg 0x%08x: 0x%04x\n"), reg_addr, val));
       }
       else{
           PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Reg 0x%08x: Error read register!\n"), reg_addr));
       }
    }
    return PHYMOD_E_NONE;
}


int phymod_diag_reg_write(phymod_phy_access_t *phys, int array_size, uint32_t reg_addr, uint32_t val){
    int i;
    int rv;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;
    
    for(i = 0 ; i < array_size ; i++){
       rv = phymod_phy_reg_write(&phys[i], reg_addr, val);
       if(rv != PHYMOD_E_NONE){
           PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Reg 0x%08x: Error write register!\n"), reg_addr));
       }
    }
    return PHYMOD_E_NONE;
}


/******************************************************************************
 PRBS
 */
STATIC
void phymod_diag_prbs_get_results_print(phymod_phy_access_t *phy, phymod_prbs_status_t *status) {
    if (status->prbs_lock == 0) {
        if (status->error_count) {
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG
                                       (phy, "prbs unlocked with %d errors during PRBS test\n"),
                                        status->error_count));   
        } else {
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(phy, "prbs unlocked\n")));
        } 
    } else if (status->prbs_lock_loss == 1) {
        PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(phy, "prbs unlocked during the PRBS test\n")));
    } else {
        if (status->error_count) {
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG
                                       (phy, "prbs locked with %d errors during PRBS test\n"),
                                        status->error_count));   
        } else { 
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(phy, "prbs locked\n")));
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(phy, "PRBS OK!\n")));
        }
    }
}


STATIC
int phymod_diag_prbs_set(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_set_args_t *params){
    int i = 0;
    int rv;
    
    phymod_prbs_t_validate(&params->prbs_options);
    
       
    if(params->loopback == 1){
        for(i = 0 ; i < array_size ; i++){
            rv = phymod_phy_loopback_set(&phys[i], phymodLoopbackGlobal, 1);
            if(rv != PHYMOD_E_NONE){
                PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Failed set loopback\n")));
                return rv;
            }
        }
    }    
    for(i = 0 ; i < array_size ; i++){
        if(phymod_phy_prbs_config_set(&phys[i], params->flags, &params->prbs_options) != PHYMOD_E_NONE){
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Failed set PRBS\n")));
            return PHYMOD_E_FAIL;
        }
        if(phymod_phy_prbs_enable_set(&phys[i], params->flags,  params->enable) != PHYMOD_E_NONE){
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Failed enable PRBS\n")));
            return PHYMOD_E_FAIL;
        }

    }
    return PHYMOD_E_NONE;
}



STATIC
int phymod_diag_prbs_get(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_get_args_t *params){
    int i = 0;
    int rv;
    phymod_prbs_status_t status;
    uint32_t enable;
    uint32_t flags = 0;

    
    for(i = 0 ; i < array_size ; i++){

        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        rv = phymod_phy_prbs_enable_get(&phys[i], flags, &enable);
        if(rv != PHYMOD_E_NONE){
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Failed get PRBS configuration\n")));
            return rv;
        }
        if(enable == 0){
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"PRBS status get can not be called when rx PRBS is disabled\n")));
            return PHYMOD_E_FAIL;
        }
        /*clear the status*/
        rv = phymod_phy_prbs_status_get(&phys[i], PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ, &status);
        if(rv != PHYMOD_E_NONE){
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"PRBS clear status failed\n")));
            return rv;
        }
    }
    /*wait*/
    PHYMOD_SLEEP(params->time);

    /*read the status*/
    for(i = 0 ; i < array_size ; i++){
        rv = phymod_phy_prbs_status_get(&phys[i], PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ, &status);
        if(rv != PHYMOD_E_NONE){
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"PRBS status read failed\n")));
            return rv;
        }
        phymod_diag_prbs_get_results_print(&phys[i], &status);
    }
    return PHYMOD_E_NONE;
}


STATIC
int phymod_diag_prbs_clear(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_clear_args_t *params){
    int i = 0;
    int rv;
    
    for(i = 0 ; i < array_size ; i++){
        rv = phymod_phy_prbs_enable_set(&phys[i], params->flags, 0);
        if(rv  != PHYMOD_E_NONE){
            PHYMOD_DEBUG_ERROR((_PHYMOD_PHY_MSG(&phys[i],"Failed clear prbs\n")));
            return rv;
        }
    }
    return PHYMOD_E_NONE;
}


int phymod_diag_prbs(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_args_t *prbs_diag_args){
    int rv = PHYMOD_E_NONE;

    PHYMOD_DIAG_PRINT_FUNC_VALIDATE;

    switch(prbs_diag_args->prbs_cmd){
    case PhymodDiagPrbsClear:
        rv = phymod_diag_prbs_clear(phys, array_size, &prbs_diag_args->args.clear_params);
        break;
    case PhymodDiagPrbsGet:
        rv = phymod_diag_prbs_get(phys, array_size, &prbs_diag_args->args.get_params);
        break;
    case PhymodDiagPrbsSet:
        rv = phymod_diag_prbs_set(phys, array_size, &prbs_diag_args->args.set_params);
        break;
    default:
        PHYMOD_DEBUG_ERROR(("Failed parsing PRBS command type\n"));
        rv = PHYMOD_E_FAIL;
    }
    return rv;
}


/******************************************************************************
DSC
 */
int phymod_diag_dsc(phymod_phy_access_t *phys, int array_size){

    int i, rv = PHYMOD_E_NONE;
    uint32_t lane, lane_index;

    for(i = 0 ; i < array_size ; i++){
        lane = phys[i].access.lane_mask;
        for( lane_index = 0 ; lane_index < PHYMOD_MAX_LANES_PER_CORE; lane_index++){
            /*if lane is not selected pass*/
            if( (lane & (1<<lane_index)) == 0){
                continue;
            }
            phys[i].access.lane_mask = lane & (1<<lane_index);
            /*collect info*/
            rv = phymod_phy_pmd_info_dump(&(phys[i]), NULL); 

            if(rv != PHYMOD_E_NONE) {
                break;
            }
        }
        phys[i].access.lane_mask = lane;
        if(rv != PHYMOD_E_NONE) {
            break;
        }
    }
    return rv;
}

/******************************************************************************
DSC
 */
int phymod_diag_dsc_std(phymod_phy_access_t *phys, int array_size){

    int i, rv = PHYMOD_E_NONE;
    uint32_t lane, lane_index;

    for(i = 0 ; i < array_size ; i++){
        lane = phys[i].access.lane_mask;
        for( lane_index = 0 ; lane_index < PHYMOD_MAX_LANES_PER_CORE; lane_index++){
            /*if lane is not selected pass*/
            if( (lane & (1<<lane_index)) == 0){
                continue;
            }
            phys[i].access.lane_mask |= lane & (1<<lane_index);

        }
        if (phys[i].access.lane_mask) {
            rv = phymod_phy_pmd_info_dump(&(phys[i]), "STD"); 

            if(rv != PHYMOD_E_NONE) {
                break;
            }
        }
        phys[i].access.lane_mask = lane; 
    }
    return rv;
}

/******************************************************************************
DSC
 */
int phymod_diag_dsc_config(phymod_phy_access_t *phys, int array_size){

    int i, rv = PHYMOD_E_NONE;
    uint32_t lane, lane_index;

    for(i = 0 ; i < array_size ; i++){
        lane = phys[i].access.lane_mask;
        for( lane_index = 0 ; lane_index < PHYMOD_MAX_LANES_PER_CORE; lane_index++){
            /*if lane is not selected pass*/
            if( (lane & (1<<lane_index)) == 0){
                continue;
            }
            phys[i].access.lane_mask = lane & (1<<lane_index);
            /*collect info*/
            rv = phymod_phy_pmd_info_dump(&(phys[i]), "config"); 

            if(rv != PHYMOD_E_NONE) {
                break;
            }
        }
        phys[i].access.lane_mask = lane;
        if(rv != PHYMOD_E_NONE) {
            break;
        }
    }
    return rv;
}


/******************************************************************************
Eyescan
 */
int phymod_diag_eyescan_run( 
    phymod_phy_access_t *phys,
    int unit,
    int* port_ids,
    int* line_rates, 
    int num_phys,
    phymod_eyescan_mode_t mode,
    phymod_phy_eyescan_options_t* eyescan_options
    )   
{
    const phymod_phy_access_t    *pa;
    int                 idx, rc = PHYMOD_E_NONE, rc_done = PHYMOD_E_NONE;
    uint32_t            flags;
    if(eyescan_options) {
      /* new code for BER projection */
      if(mode == phymodEyescanModeBERProj) {
        PHYMOD_DIAG_OUT((" ber_scan_mode   = %d\n", eyescan_options->ber_proj_scan_mode));
        PHYMOD_DIAG_OUT((" timer_control   = %d\n", eyescan_options->ber_proj_timer_cnt));
        PHYMOD_DIAG_OUT((" max_err_control = %d\n", eyescan_options->ber_proj_err_cnt));
      } else {
        PHYMOD_DIAG_OUT((" Timeout = %d ms\n",  eyescan_options->timeout_in_milliseconds));
        PHYMOD_DIAG_OUT((" Hmax    = %d\n",     eyescan_options->horz_max));
        PHYMOD_DIAG_OUT((" Hmin    = %d\n",     eyescan_options->horz_min));
        PHYMOD_DIAG_OUT((" Hstep   = %d\n",     eyescan_options->hstep));
        PHYMOD_DIAG_OUT((" Vmax    = %d\n",     eyescan_options->vert_max));
        PHYMOD_DIAG_OUT((" Vmin    = %d\n",     eyescan_options->vert_min));
        PHYMOD_DIAG_OUT((" Vstep   = %d\n",     eyescan_options->vstep));
        PHYMOD_DIAG_OUT((" mode    = %d\n",     eyescan_options->mode));
      }
    }

    /*enable eyescan*/
    flags = 0;
    PHYMOD_EYESCAN_F_ENABLE_SET(flags);
    PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_SET(flags);

    for (idx = 0; idx < num_phys; idx++) {
        pa = &(phys[idx]);
        rc = PHYMOD_E_NONE;
        if (pa->access.lane_mask){
            rc = phymod_phy_eyescan_run(pa, flags, mode, NULL);
        }
        if (rc != PHYMOD_E_NONE) {
            PHYMOD_DIAG_OUT(("Failed in phymod_phy_eyescan_run (enable) \n"));
            goto exit;
        }
    }

    PHYMOD_USLEEP(100000);

    /*process eyescan*/
    flags = 0;
    PHYMOD_EYESCAN_F_PROCESS_SET(flags);

    for (idx = 0; idx < num_phys; idx++) {
        pa = &(phys[idx]);

        if(eyescan_options && line_rates) {
            eyescan_options->linerate_in_khz = line_rates[idx];
        }

        PHYMOD_DIAG_OUT(("for u=%0d p=%0d lane_mask=%0x:\n", unit, port_ids[idx], pa->access.lane_mask));
        rc = phymod_phy_eyescan_run(pa, flags, mode, eyescan_options);
        if (rc != PHYMOD_E_NONE) {
            PHYMOD_DIAG_OUT(("Failed in phymod_phy_eyescan_run (process) \n"));
        }
    }

exit:    
    /*disable eyescan mode*/
    flags = 0;
    PHYMOD_EYESCAN_F_DONE_SET(flags);

    for (idx = 0; idx < num_phys; idx++) {

        pa = &(phys[idx]);

        rc_done = phymod_phy_eyescan_run(pa, flags, mode, eyescan_options);
        if (rc_done != PHYMOD_E_NONE) {
            PHYMOD_DIAG_OUT(("Failed in phymod_phy_eyescan_run (done) \n"));
            if(rc == PHYMOD_E_NONE) { /* Update returned error code */
                rc = rc_done;
            }
        }
    }

    return rc;
}
