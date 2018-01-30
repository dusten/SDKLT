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
 */

#include <phymod/phymod.h>
#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#include <phymod/chip/null.h>

#ifdef PHYMOD_NULL_SUPPORT

int null_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{

    /* Place your code here */
    return phymod_core_info_t_init(info);
}


int null_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{

    /* Place your code here */
    return phymod_lane_map_t_init(lane_map);
}


int null_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{

    /* Place your code here */
    *direction = 0;

    return PHYMOD_E_NONE;

}


int null_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{

    /* Place your code here */
    return phymod_core_firmware_info_t_init(fw_info);
}


int null_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_core_config)
{

    /* Place your code here */
    return phymod_firmware_core_config_t_init(fw_core_config);
}


int null_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{

    /* Place your code here */
    return phymod_firmware_lane_config_t_init(fw_lane_config);

}


int null_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_rx_restart(const phymod_phy_access_t* phy)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{

    /* Place your code here */
    return phymod_polarity_t_init(polarity);

}


int null_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{

    /* Place your code here */
    return phymod_tx_t_init(tx);

}


int null_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{

    /* Place your code here */
    return phymod_tx_t_init(tx);

}


int null_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{

    /* Place your code here */
    return phymod_tx_override_t_init(tx_override);

}


int null_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{

    /* Place your code here */
    return phymod_rx_t_init(rx);

}


int null_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{

    /* Place your code here */
    return phymod_phy_reset_t_init(reset);

}


int null_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{

    /* Place your code here */
    return phymod_phy_power_t_init(power);

}


int null_phy_hg2_codec_control_set(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t hg2_codec)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_hg2_codec_control_get(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t* hg2_codec)
{

    /* Place your code here */
    *hg2_codec = 0;


    return PHYMOD_E_NONE;

}


int null_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{

    /* Place your code here */
    *tx_control = 0;


    return PHYMOD_E_NONE;

}


int null_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{

    /* Place your code here */
    *rx_control = 0;


    return PHYMOD_E_NONE;

}


int null_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{

    /* Place your code here */
    *enable = 0;


    return PHYMOD_E_NONE;

}


int null_phy_autoneg_oui_set(const phymod_phy_access_t* phy, phymod_autoneg_oui_t an_oui)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_oui_get(const phymod_phy_access_t* phy, phymod_autoneg_oui_t* an_oui)
{

    /* Place your code here */
    return phymod_autoneg_oui_t_init(an_oui);

}


int null_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{

    /* Place your code here */
    *enable = 0;


    return PHYMOD_E_NONE;

}


int null_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{

    /* Place your code here */
    return phymod_phy_inf_config_t_init(config);

}


int null_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{

    /* Place your code here */
    *cl72_en = 0;


    return PHYMOD_E_NONE;

}


int null_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{

    /* Place your code here */
    return phymod_cl72_status_t_init(status);

}


int null_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{

    /* Place your code here */
    return phymod_autoneg_ability_t_init(an_ability_get_type);

}


int null_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{

    /* Place your code here */
    return phymod_autoneg_ability_t_init(an_ability_get_type);

}


int null_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{

    /* Place your code here */
    *an_done = 1;
    return phymod_autoneg_control_t_init(an);
}


int null_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{

    /* Place your code here */
    return phymod_autoneg_status_t_init(status);

}


int null_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{

    /* Place your code here */
    *core_vco_pll_multiplier = 0;


    return PHYMOD_E_NONE;

}


int null_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{

    /* Place your code here */
    *enable = 0;


    return PHYMOD_E_NONE;

}


int null_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{

    /* Place your code here */
    *rx_pmd_locked = 1;


    return PHYMOD_E_NONE;

}


int null_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* rx_signal_detect)
{

    /* Place your code here */
    *rx_signal_detect = 1;

    return PHYMOD_E_NONE;

}


int null_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{

    /* Place your code here */
    *link_status = 1;


    return PHYMOD_E_NONE;

}


int null_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, phymod_pcs_userspeed_config_t* config)
{

    /* Place your code here */
    return phymod_pcs_userspeed_config_t_init(config);

}


int null_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int null_phy_eye_margin_est_get(const phymod_phy_access_t* phy, phymod_eye_margin_mode_t eye_margin_mode, uint32_t* value)
{

    /* Place your code here */
    *value = 0;


    return PHYMOD_E_NONE;

}


int null_phy_sw_autoneg_enable(const phymod_phy_access_t* phy, int enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_speed_config_set(const phymod_phy_access_t* phy,
                              const phymod_phy_speed_config_t* speed_config,
                              const phymod_phy_pll_state_t* old_pll_state,
                              phymod_phy_pll_state_t* new_pll_state)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_tx_taps_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_tx_t* tx)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_lane_config_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_firmware_lane_config_t* lane_config)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_advert_ability_set(const phymod_phy_access_t* phy,
                                         const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                         const phymod_phy_pll_state_t* old_pll_adv_state,
                                         phymod_phy_pll_state_t* new_pll_adv_state)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_advert_ability_get(const phymod_phy_access_t* phy,
                                         phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy,
                                                phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

int null_phy_plls_reconfig(const phymod_phy_access_t* phy, uint32_t num_plls, const uint32_t* plls)
{
    /* Place your code here */


    return PHYMOD_E_NONE;

}

#endif /* PHYMOD_NULL_SUPPORT */
