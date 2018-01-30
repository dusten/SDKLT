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

#include <phymod/phymod.h>
#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_TSCE_SUPPORT

#include <phymod/chip/tsce.h>

#include <phymod/chip/eagle.h>

__phymod__dispatch__t__ phymod_tsce_driver = {

    tsce_core_identify,
    tsce_core_info_get,
    tsce_core_lane_map_set,
    tsce_core_lane_map_get,
    tsce_core_reset_set,
    tsce_core_reset_get,
    tsce_core_firmware_info_get,
    tsce_phy_firmware_core_config_set,
    tsce_phy_firmware_core_config_get,
    tsce_phy_firmware_lane_config_set,
    tsce_phy_firmware_lane_config_get,
    tsce_core_pll_sequencer_restart,
    tsce_core_wait_event,
    tsce_phy_rx_restart,
    tsce_phy_polarity_set,
    tsce_phy_polarity_get,
    tsce_phy_tx_set,
    tsce_phy_tx_get,
    eagle_phy_media_type_tx_get,
    tsce_phy_tx_override_set,
    tsce_phy_tx_override_get,
    NULL, /* phymod_phy_txpi_config_set */
    NULL, /* phymod_phy_txpi_config_get */
    tsce_phy_rx_set,
    tsce_phy_rx_get,
    tsce_phy_rx_adaptation_resume,
    tsce_phy_reset_set,
    tsce_phy_reset_get,
    tsce_phy_power_set,
    tsce_phy_power_get,
    tsce_phy_hg2_codec_control_set,
    tsce_phy_hg2_codec_control_get,
    tsce_phy_tx_lane_control_set,
    tsce_phy_tx_lane_control_get,
    tsce_phy_rx_lane_control_set,
    tsce_phy_rx_lane_control_get,
    tsce_phy_fec_enable_set,
    tsce_phy_fec_enable_get,
    NULL, /* phymod_phy_forced_speed_line_side_set */
    NULL, /* phymod_phy_forced_speed_line_side_get */
    NULL, /* phymod_phy_autoneg_line_side_set */
    NULL, /* phymod_phy_autoneg_line_side_get */
    NULL, /* phymod_phy_flow_control_mode_set */
    NULL, /* phymod_phy_flow_control_mode_get */
    NULL, /* phymod_phy_psm_cos_bmp_set */
    NULL, /* phymod_phy_psm_cos_bmp_get */
    tsce_phy_autoneg_oui_set,
    tsce_phy_autoneg_oui_get,
    tsce_phy_eee_set,
    tsce_phy_eee_get,
    tsce_phy_interface_config_set,
    tsce_phy_interface_config_get,
    tsce_phy_cl72_set,
    tsce_phy_cl72_get,
    tsce_phy_cl72_status_get,
    tsce_phy_autoneg_ability_set,
    tsce_phy_autoneg_ability_get,
    tsce_phy_autoneg_remote_ability_get,
    tsce_phy_autoneg_set,
    tsce_phy_autoneg_get,
    tsce_phy_autoneg_restart_set,
    tsce_phy_autoneg_status_get,
    tsce_core_init,
    tsce_phy_pll_multiplier_get,
    tsce_phy_init,
    tsce_phy_loopback_set,
    tsce_phy_loopback_get,
    tsce_phy_rx_pmd_locked_get,
    tsce_phy_rx_signal_detect_get,
    tsce_phy_link_status_get,
    NULL, /* phymod_phy_status_dump */
    tsce_phy_pcs_userspeed_set,
    tsce_phy_pcs_userspeed_get,
    tsce_phy_reg_read,
    tsce_phy_reg_write,
    NULL, /* phymod_phy_private_read */
    NULL, /* phymod_phy_private_write */
    NULL, /* phymod_phy_rev_id */
    NULL, /* phymod_phy_lane_cross_switch_map_set */
    NULL, /* phymod_phy_lane_cross_switch_map_get */
    NULL, /* phymod_phy_intr_enable_set */
    NULL, /* phymod_phy_intr_enable_get */
    NULL, /* phymod_phy_intr_status_get */
    NULL, /* phymod_phy_intr_status_clear */
    NULL, /* phymod_phy_i2c_read */
    NULL, /* phymod_phy_i2c_write */
    NULL, /* phymod_phy_gpio_config_set */
    NULL, /* phymod_phy_gpio_config_get */
    NULL, /* phymod_phy_gpio_pin_value_set */
    NULL, /* phymod_phy_gpio_pin_value_get */
    NULL, /* phymod_timesync_config_set */
    NULL, /* phymod_timesync_config_get */
    NULL, /* phymod_timesync_enable_set */
    NULL, /* phymod_timesync_enable_get */
    NULL, /* phymod_timesync_nco_addend_set */
    NULL, /* phymod_timesync_nco_addend_get */
    NULL, /* phymod_timesync_framesync_mode_set */
    NULL, /* phymod_timesync_framesync_mode_get */
    NULL, /* phymod_timesync_local_time_set */
    NULL, /* phymod_timesync_local_time_get */
    NULL, /* phymod_timesync_load_ctrl_set */
    NULL, /* phymod_timesync_load_ctrl_get */
    NULL, /* phymod_timesync_tx_timestamp_offset_set */
    NULL, /* phymod_timesync_tx_timestamp_offset_get */
    NULL, /* phymod_timesync_rx_timestamp_offset_set */
    NULL, /* phymod_timesync_rx_timestamp_offset_get */
    NULL, /* phymod_timesync_capture_timestamp_get */
    NULL, /* phymod_timesync_heartbeat_timestamp_get */
    NULL, /* phymod_timesync_do_sync */
    NULL, /* phymod_timesync_offset_set */
    NULL, /* phymod_timesync_adjust_set */
    NULL, /* phymod_edc_config_set */
    NULL, /* phymod_edc_config_get */
    NULL, /* phymod_phy_core_mode_set */
    NULL, /* phymod_phy_core_mode_get */
    NULL, /* phymod_failover_mode_set */
    NULL, /* phymod_failover_mode_get */
    NULL, /* phymod_port_init */
    NULL, /* phymod_phy_autoneg_try_enable */
    NULL, /* phymod_debug_ether */
    NULL, /* phymod_reset_interface */
    NULL, /* phymod_phy_short_chn_mode_enable_set */
    NULL, /* phymod_phy_short_chn_mode_enable_get */
    NULL, /* phymod_port_enable_set */
    NULL, /* phymod_port_enable_get */
    tsce_phy_eye_margin_est_get,
    NULL, /* phymod_phy_multi_get */
    NULL, /* phymod_phy_rescal_set */
    NULL, /* phymod_phy_rescal_get */
    NULL, /* phymod_phy_sw_an_control_status_get */
    NULL, /* phymod_phy_sw_an_base_page_exchange_handler */
    NULL, /* phymod_phy_sw_an_lp_page_rdy_handler */
    NULL, /* phymod_phy_sw_an_advert_set */
    NULL, /* phymod_phy_sw_autoneg_enable */
    NULL, /* phymod_phy_speed_config_set */
    NULL, /* phymod_phy_speed_config_get */
    NULL, /* phymod_phy_op_mode_get */
    NULL, /* phymod_phy_tx_taps_default_get */
    NULL, /* phymod_phy_lane_config_default_get */
    NULL, /* phymod_phy_firmware_load_info_get */
    NULL, /* phymod_phy_autoneg_advert_ability_set */
    NULL, /* phymod_phy_autoneg_advert_ability_get */
    NULL, /* phymod_phy_autoneg_remote_advert_ability_get */
    NULL, /* phymod_phy_bond_in_pwrdn_override */
    NULL, /* phymod_phy_tx_pam4_precoder_enable_set */
    NULL, /* phymod_phy_tx_pam4_precoder_enable_get */
    NULL, /* phymod_phy_tx_phase_lock_set */
    NULL, /* phymod_phy_plls_reconfig */
    NULL, /* phymod_phy_linkdown_transmit_set */
    NULL, /* phymod_phy_linkdown_transmit_get */
};

#endif /* PHYMOD_TSCE_SUPPORT */
