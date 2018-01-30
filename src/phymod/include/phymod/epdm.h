/*
 *
 * $Id: epdm.h $
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

#ifndef EPDM_H
#define EPDM_H

#include "epdm_bcm_common_defines.h"
#define __PLP_NOT_USED ((void*)0)

typedef int (*__bcm_plp_mutex_info_set_f) (bcm_plp_access_t phy_info, bcm_plp_mutex_info_t *mutex_info);
typedef int (*__bcm_plp_static_config_set_f)(bcm_plp_access_t phy_info, void* bcm_static_config);
typedef int (*__bcm_plp_static_config_get_f)(bcm_plp_access_t phy_info, void* bcm_static_config);
typedef int (*__bcm_plp_init_f)(bcm_plp_access_t phy_info, int (*read)(void* user_acc,
            unsigned int core_addr, unsigned int reg_addr, unsigned int* val), 
            int (*write)(void* user_acc, unsigned int core_addr, unsigned int reg_addr,
             unsigned int val), bcm_pm_firmware_load_method_t firmware_load_method);

typedef int (*__bcm_plp_cleanup_f)(bcm_plp_access_t phy_info);

typedef int (*__bcm_plp_link_status_get_f)(bcm_plp_access_t phy_info, unsigned int *link_status);

typedef int (*__bcm_plp_mode_config_set_f)(bcm_plp_access_t phy_info, int speed, int if_type,
                            int ref_clk, int interface_mode, void* device_aux_modes);

typedef int (*__bcm_plp_mode_config_get_f)(bcm_plp_access_t phy_info, int *speed, 
                            int *if_type, int *ref_clk, int *interface_mode,
                            void *device_aux_modes);

typedef int (*__bcm_plp_driver_version_get_f)(bcm_plp_access_t phy_info, char *phy_chip_name, unsigned short *major_ver,
                         unsigned short *minor_ver);

typedef int (*__bcm_plp_prbs_set_f)(bcm_plp_access_t phy_info, unsigned int tx_rx, 
                     unsigned int poly, unsigned int invert,
                     unsigned int loopback, unsigned int ena_dis);

typedef int (*__bcm_plp_prbs_get_f)(bcm_plp_access_t phy_info, unsigned int tx_rx,
                     unsigned int *poly, unsigned int *invert,
                     unsigned int *loopback, unsigned int *ena_dis);

typedef int (*__bcm_plp_prbs_rx_stat_f)(bcm_plp_access_t phy_info, unsigned int time);

typedef int (*__bcm_plp_prbs_clear_f)(bcm_plp_access_t phy_info, unsigned int tx_rx);

typedef int (*__bcm_plp_prbs_config_get_f)(bcm_plp_access_t phy_info, unsigned int tx_rx,
                            unsigned int *poly, unsigned int *invert);

typedef int (*__bcm_plp_prbs_status_get_f)(bcm_plp_access_t phy_info, unsigned int *prbs_lock,
                            unsigned int *prbs_lock_loss, unsigned int *error_count);  

typedef int (*__bcm_plp_reg_value_set_f)(bcm_plp_access_t phy_info, unsigned int devaddr,
                          unsigned int regaddr, unsigned int data);

typedef int (*__bcm_plp_reg_value_get_f)(bcm_plp_access_t phy_info, unsigned int devaddr,
                          unsigned int regaddr, unsigned int *data);

typedef int (*__bcm_plp_polarity_set_f)(bcm_plp_access_t phy_info, unsigned int tx_pol,
                         unsigned int rx_pol);

typedef int (*__bcm_plp_polarity_get_f)(bcm_plp_access_t phy_info, unsigned int *tx_pol,
                         unsigned int *rx_pol);

typedef int (*__bcm_plp_rx_pmd_lock_get_f)(bcm_plp_access_t phy_info, unsigned int* rx_pmd_lock);

typedef int (*__bcm_plp_rev_id_f)(bcm_plp_access_t phy_info, unsigned int* rev_id);


typedef int (*__bcm_plp_loopback_set_f)(bcm_plp_access_t phy_info, unsigned int lb_mode,
                         unsigned int enable);

typedef int (*__bcm_plp_loopback_get_f)(bcm_plp_access_t phy_info, unsigned int lb_mode,
                         unsigned int *enable);

typedef int (*__bcm_plp_tx_set_f)(bcm_plp_access_t phy_info, bcm_plp_tx_t* tx);

typedef int (*__bcm_plp_tx_get_f)(bcm_plp_access_t phy_info, bcm_plp_tx_t* tx);

typedef int (*__bcm_plp_rx_set_f)(bcm_plp_access_t phy_info, bcm_plp_rx_t* rx);

typedef int (*__bcm_plp_rx_get_f)(bcm_plp_access_t phy_info, bcm_plp_rx_t* rx);

typedef int (*__bcm_plp_reset_set_f)(bcm_plp_access_t phy_info, unsigned int reset_mode,
                      unsigned int reset_val);

typedef int (*__bcm_plp_phy_lane_reset_set_f)(bcm_plp_access_t phy_info, bcm_plp_pm_phy_reset_t* reset);

typedef int (*__bcm_plp_phy_lane_reset_get_f)(bcm_plp_access_t phy_info, bcm_plp_pm_phy_reset_t* reset);

typedef int (*__bcm_plp_tx_lane_control_set_f)(bcm_plp_access_t phy_info, bcm_pm_phy_tx_lane_control_t tx_control);

typedef int (*__bcm_plp_rx_lane_control_set_f)(bcm_plp_access_t phy_info, bcm_pm_phy_rx_lane_control_t rx_control);

typedef int (*__bcm_plp_tx_lane_control_get_f)(bcm_plp_access_t phy_info, bcm_pm_phy_tx_lane_control_t *tx_control);

typedef int (*__bcm_plp_rx_lane_control_get_f)(bcm_plp_access_t phy_info, bcm_pm_phy_rx_lane_control_t *rx_control);

typedef int (*__bcm_plp_lane_cross_switch_map_set_f)(bcm_plp_access_t phy_info, unsigned int* tx_source_array);

typedef int (*__bcm_plp_lane_cross_switch_map_get_f)(bcm_plp_access_t phy_info, unsigned int *mapped_to);

typedef int (*__bcm_plp_force_tx_training_set_f)(bcm_plp_access_t phy_info, unsigned int enable);

typedef int (*__bcm_plp_force_tx_training_get_f)(bcm_plp_access_t phy_info, unsigned int *enable);

typedef int (*__bcm_plp_force_tx_training_status_get_f)(bcm_plp_access_t phy_info, unsigned int *enabled,
                                         unsigned int *training_failure, unsigned int *trained);

typedef int (*__bcm_plp_cl73_ability_set_f)(bcm_plp_access_t phy_info, unsigned short tech_ability,
                             unsigned short fec_ability, unsigned short pause_ability, bcm_plp_an_config_t an_config);

typedef int (*__bcm_plp_cl73_ability_get_f)(bcm_plp_access_t phy_info, unsigned short *tech_ability,
                             unsigned short *fec_ability, unsigned short *pause_ability, bcm_plp_an_config_t* an_config);
typedef int (*__bcm_plp_cl73_set_f)(bcm_plp_access_t phy_info, unsigned short ena_dis);

typedef int (*__bcm_plp_cl73_get_f)(bcm_plp_access_t phy_info, unsigned int *an,
                     unsigned int *an_done);

typedef int (*__bcm_plp_display_eye_scan_f)(bcm_plp_access_t phy_info);

typedef int (*__bcm_plp_firmware_info_get_f)(bcm_plp_access_t phy_info, unsigned int *fw_version,
                              unsigned int *fw_crc);

typedef int (*__bcm_plp_firmware_set_f)(bcm_plp_access_t phy_info);

typedef int (*__bcm_plp_rxtx_laneswap_set_f)(bcm_plp_access_t phy_info, bcm_plp_laneswap_map_t* laneswap_map);

typedef int (*__bcm_plp_rxtx_laneswap_get_f)(bcm_plp_access_t phy_info, bcm_plp_laneswap_map_t* laneswap_map);

typedef int (*__bcm_plp_pll_sequencer_restart_f)(bcm_plp_access_t phy_info, unsigned char flags,
                                  bcm_pm_sequencer_operation_t operation);

typedef int (*__bcm_plp_fec_enable_set_f)(bcm_plp_access_t phy_info, unsigned int enable);

typedef int (*__bcm_plp_fec_enable_get_f)(bcm_plp_access_t phy_info, unsigned int* enable);

typedef int (*__bcm_plp_phy_status_dump_f)(bcm_plp_access_t phy_info);

typedef int (*__bcm_plp_phy_diagnostics_get_f)(bcm_plp_access_t phy_info, bcm_plp_pm_phy_diagnostics_t* diag);

typedef int (*__bcm_plp_intr_status_get_f)(bcm_plp_access_t phy_info, unsigned int intr_type,
                            unsigned int* intr_status);

typedef int (*__bcm_plp_intr_enable_set_f)(bcm_plp_access_t phy_info, unsigned int intr_type,
                            unsigned int enable);

typedef int (*__bcm_plp_intr_enable_get_f)(bcm_plp_access_t phy_info, unsigned int intr_type,
                            unsigned int* enable);

typedef int (*__bcm_plp_intr_status_clear_f)(bcm_plp_access_t phy_info, unsigned int intr_type);

typedef int (*__bcm_plp_fc_pcs_chkr_enable_set_f)(bcm_plp_access_t phy_info, unsigned int fcpcs_chkr_mode,
                                   unsigned int enable);

typedef int (*__bcm_plp_fc_pcs_chkr_enable_get_f)(bcm_plp_access_t phy_info, unsigned int fcpcs_chkr_mode,
                                   unsigned int* enable);

typedef int (*__bcm_plp_fc_pcs_chkr_status_get_f)(bcm_plp_access_t phy_info, unsigned int *lock_status,
                                   unsigned int* lock_lost_lh, unsigned int* error_count);
#ifdef SERDES_API_FLOATING_POINT
typedef int (*__bcm_plp_eye_margin_proj_f)(bcm_plp_access_t phy_info, double rate,
                            unsigned char ber_scan_mode, unsigned char timer_control,
                            unsigned char max_error_control);
#else 
typedef int (*__bcm_plp_eye_margin_proj_f)(bcm_plp_access_t phy_info, int rate,
                            unsigned char ber_scan_mode, unsigned char timer_control,
                            unsigned char max_error_control);
#endif

typedef int (*__bcm_plp_repeater_mode_get_f)(bcm_plp_access_t phy_info, unsigned int *ena_dis);

typedef int (*__bcm_plp_repeater_mode_set_f)(bcm_plp_access_t phy_info, unsigned int ena_dis);

typedef int (*__bcm_plp_module_read_f)(bcm_plp_access_t phy_info, unsigned int slv_addr,
                        unsigned int start_addr, unsigned int no_of_bytes,
                        unsigned char *read_data);

typedef int (*__bcm_plp_module_write_f)(bcm_plp_access_t phy_info, unsigned int slv_addr,
                         unsigned int start_addr, unsigned int no_of_bytes,
                         unsigned char *write_data);

typedef int (*__bcm_plp_cfg_gpio_pin_set_f)(bcm_plp_access_t phy_info, unsigned int gpio_pin_number,
                             unsigned int cfg_direction, unsigned int cfg_pull,
                             unsigned int pin_value);

typedef int (*__bcm_plp_cfg_gpio_pin_get_f)(bcm_plp_access_t phy_info, unsigned int gpio_pin_number,
                             unsigned int *cfg_direction, unsigned int *cfg_pull,
                             unsigned int *pin_value);

typedef int (*__bcm_plp_power_set_f)(bcm_plp_access_t phy_info,  unsigned int power_rx, unsigned int power_tx);

typedef int (*__bcm_plp_power_get_f)(bcm_plp_access_t phy_info,  unsigned int *power_rx, unsigned int *power_tx);

typedef int (*__bcm_plp_firmware_lane_config_set_f)(bcm_plp_access_t phy_info, bcm_plp_pm_firmware_lane_config_t* firmware_lane_config);

typedef int (*__bcm_plp_firmware_lane_config_get_f)(bcm_plp_access_t phy_info, bcm_plp_pm_firmware_lane_config_t* firmware_lane_config);
typedef int (*__bcm_plp_init_fw_bcast_f)(bcm_plp_access_t phy_info,
                  int (*read)(void* user_acc, unsigned int core_addr, unsigned int reg_addr, unsigned int* val),
                  int (*write)(void* user_acc, unsigned int core_addr, unsigned int reg_addr, unsigned int val),
                  bcm_plp_firmware_load_type_t *firmware_load_type,
                  bcm_plp_firmware_broadcast_method_t broadcast_method);
typedef int (*__bcm_plp_failover_mode_set_f)(bcm_plp_access_t phy_info,unsigned int failover_mode);
typedef int (*__bcm_plp_failover_mode_get_f)(bcm_plp_access_t phy_info,unsigned int *failover_mode);

typedef int (*__bcm_plp_edc_config_set_f)(bcm_plp_access_t phy_info, unsigned int edc_method, unsigned int edc_value );
typedef int (*__bcm_plp_edc_config_get_f)(bcm_plp_access_t phy_info, unsigned int *edc_method, unsigned int *edc_value );

typedef int (*__bcm_plp_fec_corrected_error_counter_f)(bcm_plp_access_t phy_info, bcm_plp_fec_type_t fec_type, unsigned int *count);
typedef int (*__bcm_plp_fec_uncorrected_error_counter_f)(bcm_plp_access_t phy_info, bcm_plp_fec_type_t fec_type, unsigned int *count);

typedef int (*__bcm_plp_event_status_get)(bcm_plp_access_t phy_info, unsigned int event_type, unsigned int *event_status);

typedef int (*__bcm_plp_base_t_autoneg_ability_set_f)(bcm_plp_access_t phy_info, bcm_plp_base_t_ability_t *ability);
typedef int (*__bcm_plp_base_t_autoneg_ability_get_f)(bcm_plp_access_t phy_info, bcm_plp_base_t_ability_t *ability);
typedef int (*__bcm_plp_base_t_autoneg_set_f)(bcm_plp_access_t phy_info, int  enable);
typedef int (*__bcm_plp_base_t_autoneg_get_f)(bcm_plp_access_t phy_info, int *enable, int *an_done);
typedef int (*__bcm_plp_base_t_eee_set_f)(bcm_plp_access_t phy_info, bcm_plp_base_t_eee_t *eee_conf);
typedef int (*__bcm_plp_base_t_eee_get_f)(bcm_plp_access_t phy_info, bcm_plp_base_t_eee_t *eee_conf);
typedef int (*__bcm_plp_base_t_power_mode_set_f)(bcm_plp_access_t phy_info, bcm_plp_base_t_power_mode_t  power_mode);
typedef int (*__bcm_plp_base_t_power_mode_get_f)(bcm_plp_access_t phy_info, bcm_plp_base_t_power_mode_t *power_mode);
typedef int (*__bcm_plp_base_t_cable_diag_f)(bcm_plp_access_t phy_info, bcm_plp_base_t_cable_diag_t *cdiag);
typedef int (*__bcm_plp_base_t_state_mfg_diag_f)(bcm_plp_access_t phy_info, unsigned int inst,
                                                 bcm_plp_base_t_diag_ctrl_type_t op_type,
                                                 bcm_plp_base_t_diag_ctrl_cmd_t  op_cmd, void *arg);
typedef int (*__bcm_plp_l1_intr_status_get_f)(bcm_plp_access_t phy_info, bcm_plp_l1_intr_status_t *l1_intr_status);

typedef int (*__bcm_plp_timesync_config_set)(bcm_plp_access_t phy_info, bcm_plp_timesync_config_t* config);
typedef int (*__bcm_plp_timesync_config_get)(bcm_plp_access_t phy_info, bcm_plp_timesync_config_t* config);
typedef int (*__bcm_plp_timesync_enable_set)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int enable);
typedef int (*__bcm_plp_timesync_enable_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int* enable);
typedef int (*__bcm_plp_timesync_nco_addend_set)(bcm_plp_access_t phy_info, unsigned int flags,  unsigned int freq_step);
typedef int (*__bcm_plp_timesync_nco_addend_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int* freq_step);
typedef int (*__bcm_plp_timesync_framesync_mode_set)(bcm_plp_access_t phy_info, unsigned int flags, bcm_plp_timesync_framesync_t* framesync);
typedef int (*__bcm_plp_timesync_framesync_mode_get)(bcm_plp_access_t phy_info, unsigned int flags, bcm_plp_timesync_framesync_t* framesync);
typedef int (*__bcm_plp_timesync_local_time_set)(bcm_plp_access_t phy_info, unsigned int flags, unsigned long long local_time);
typedef int (*__bcm_plp_timesync_local_time_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned long long* local_time);
typedef int (*__bcm_plp_timesync_load_ctrl_set)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int load_once, unsigned int load_always);
typedef int (*__bcm_plp_timesync_load_ctrl_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int* load_once, unsigned int* load_always);
typedef int (*__bcm_plp_timesync_tx_timestamp_offset_set)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int ts_offset);
typedef int (*__bcm_plp_timesync_tx_timestamp_offset_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int* ts_offset);
typedef int (*__bcm_plp_timesync_rx_timestamp_offset_set)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int ts_offset);
typedef int (*__bcm_plp_timesync_rx_timestamp_offset_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned int* ts_offset);
typedef int (*__bcm_plp_timesync_capture_timestamp_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned long long* cap_ts);
typedef int (*__bcm_plp_timesync_heartbeat_timestamp_get)(bcm_plp_access_t phy_info, unsigned int flags, unsigned long long* hb_ts);
typedef int (*__bcm_plp_timesync_do_sync)(bcm_plp_access_t phy_info);

typedef struct __plp__dispatch__s__ {
    __bcm_plp_static_config_set_f __static_config_set; 
    __bcm_plp_static_config_get_f __static_config_get; 
    __bcm_plp_init_f __init; 
    __bcm_plp_cleanup_f __cleanup; 
    __bcm_plp_link_status_get_f __link_status_get; 
    __bcm_plp_mode_config_set_f __mode_config_set; 
    __bcm_plp_mode_config_get_f __mode_config_get; 
    __bcm_plp_driver_version_get_f __driver_version_get; 
    __bcm_plp_prbs_set_f __prbs_set; 
    __bcm_plp_prbs_get_f __prbs_get; 
    __bcm_plp_prbs_rx_stat_f __prbs_rx_stat; 
    __bcm_plp_prbs_clear_f __prbs_clear; 
    __bcm_plp_prbs_config_get_f __prbs_config_get; 
    __bcm_plp_prbs_status_get_f __prbs_status_get; 
    __bcm_plp_reg_value_set_f __reg_value_set; 
    __bcm_plp_reg_value_get_f __reg_value_get; 
    __bcm_plp_polarity_set_f __polarity_set; 
    __bcm_plp_polarity_get_f __polarity_get; 
    __bcm_plp_rx_pmd_lock_get_f __rx_pmd_lock_get; 
    __bcm_plp_rev_id_f __rev_id; 
    __bcm_plp_loopback_set_f __loopback_set; 
    __bcm_plp_loopback_get_f __loopback_get; 
    __bcm_plp_tx_set_f __tx_set; 
    __bcm_plp_tx_get_f __tx_get; 
    __bcm_plp_rx_set_f __rx_set; 
    __bcm_plp_rx_get_f __rx_get; 
    __bcm_plp_reset_set_f __reset_set; 
    __bcm_plp_phy_lane_reset_set_f __phy_lane_reset_set; 
    __bcm_plp_phy_lane_reset_get_f __phy_lane_reset_get; 
    __bcm_plp_tx_lane_control_set_f __tx_lane_control_set; 
    __bcm_plp_rx_lane_control_set_f __rx_lane_control_set; 
    __bcm_plp_tx_lane_control_get_f __tx_lane_control_get; 
    __bcm_plp_rx_lane_control_get_f __rx_lane_control_get; 
    __bcm_plp_lane_cross_switch_map_set_f __lane_cross_switch_map_set; 
    __bcm_plp_lane_cross_switch_map_get_f __lane_cross_switch_map_get; 
    __bcm_plp_force_tx_training_set_f __force_tx_training_set; 
    __bcm_plp_force_tx_training_get_f __force_tx_training_get; 
    __bcm_plp_force_tx_training_status_get_f __force_tx_training_status_get; 
    __bcm_plp_cl73_ability_set_f __cl73_ability_set; 
    __bcm_plp_cl73_ability_get_f __cl73_ability_get; 
    __bcm_plp_cl73_set_f __cl73_set; 
    __bcm_plp_cl73_get_f __cl73_get; 
    __bcm_plp_display_eye_scan_f __display_eye_scan; 
    __bcm_plp_firmware_info_get_f __firmware_info_get; 
    __bcm_plp_firmware_set_f __firmware_set; 
    __bcm_plp_rxtx_laneswap_set_f __rxtx_laneswap_set; 
    __bcm_plp_rxtx_laneswap_get_f __rxtx_laneswap_get; 
    __bcm_plp_pll_sequencer_restart_f __pll_sequencer_restart; 
    __bcm_plp_fec_enable_set_f __fec_enable_set; 
    __bcm_plp_fec_enable_get_f __fec_enable_get; 
    __bcm_plp_phy_status_dump_f __phy_status_dump; 
    __bcm_plp_phy_diagnostics_get_f __phy_diagnostics_get; 
    __bcm_plp_intr_status_get_f __intr_status_get; 
    __bcm_plp_intr_enable_set_f __intr_enable_set; 
    __bcm_plp_intr_enable_get_f __intr_enable_get; 
    __bcm_plp_intr_status_clear_f __intr_status_clear; 
    __bcm_plp_fc_pcs_chkr_enable_set_f __fc_pcs_chkr_enable_set; 
    __bcm_plp_fc_pcs_chkr_enable_get_f __fc_pcs_chkr_enable_get; 
    __bcm_plp_fc_pcs_chkr_status_get_f __fc_pcs_chkr_status_get;
    __bcm_plp_eye_margin_proj_f __eye_margin_proj;
    __bcm_plp_repeater_mode_get_f __repeater_mode_get; 
    __bcm_plp_repeater_mode_set_f __repeater_mode_set; 
    __bcm_plp_module_read_f __module_read; 
    __bcm_plp_module_write_f __module_write; 
    __bcm_plp_cfg_gpio_pin_set_f __cfg_gpio_pin_set; 
    __bcm_plp_cfg_gpio_pin_get_f __cfg_gpio_pin_get; 
    __bcm_plp_power_set_f __power_set; 
    __bcm_plp_power_get_f __power_get; 
    __bcm_plp_firmware_lane_config_set_f __firmware_lane_config_set; 
    __bcm_plp_firmware_lane_config_get_f __firmware_lane_config_get; 
    __bcm_plp_init_fw_bcast_f __init_fw_bcast; 
    __bcm_plp_failover_mode_set_f __failover_mode_set; 
    __bcm_plp_failover_mode_get_f __failover_mode_get; 
    __bcm_plp_edc_config_set_f __edc_config_set; 
    __bcm_plp_edc_config_get_f __edc_config_get;
    __bcm_plp_fec_corrected_error_counter_f __fec_corrected_counter_get;
    __bcm_plp_fec_uncorrected_error_counter_f __fec_uncorrected_counter_get;
    __bcm_plp_event_status_get __event_status_get;
    __bcm_plp_base_t_autoneg_ability_set_f  __base_t_autoneg_ability_set;
    __bcm_plp_base_t_autoneg_ability_get_f  __base_t_autoneg_ability_get;
    __bcm_plp_base_t_autoneg_set_f  __base_t_autoneg_set;
    __bcm_plp_base_t_autoneg_get_f  __base_t_autoneg_get;
    __bcm_plp_base_t_eee_set_f  __base_t_eee_set;
    __bcm_plp_base_t_eee_get_f  __base_t_eee_get;
    __bcm_plp_base_t_power_mode_set_f  __base_t_power_mode_set;
    __bcm_plp_base_t_power_mode_get_f  __base_t_power_mode_get;
    __bcm_plp_timesync_config_set __timesync_config_set;
    __bcm_plp_timesync_config_get __timesync_config_get;
    __bcm_plp_timesync_enable_set __timesync_enable_set;
    __bcm_plp_timesync_enable_get __timesync_enable_get;
    __bcm_plp_timesync_nco_addend_set __timesync_nco_addend_set;
    __bcm_plp_timesync_nco_addend_get __timesync_nco_addend_get;
    __bcm_plp_timesync_framesync_mode_set  __timesync_framesync_mode_set;
    __bcm_plp_timesync_framesync_mode_get  __timesync_framesync_mode_get;
    __bcm_plp_timesync_local_time_set   __timesync_local_time_set;
    __bcm_plp_timesync_local_time_get   __timesync_local_time_get;
    __bcm_plp_timesync_load_ctrl_set    __timesync_load_ctrl_set;
    __bcm_plp_timesync_load_ctrl_get    __timesync_load_ctrl_get;
    __bcm_plp_timesync_tx_timestamp_offset_set  __timesync_tx_timestamp_offset_set;
    __bcm_plp_timesync_tx_timestamp_offset_get  __timesync_tx_timestamp_offset_get;
    __bcm_plp_timesync_rx_timestamp_offset_set  __timesync_rx_timestamp_offset_set;
    __bcm_plp_timesync_rx_timestamp_offset_get  __timesync_rx_timestamp_offset_get;
    __bcm_plp_timesync_capture_timestamp_get    __timesync_capture_timestamp_get;
    __bcm_plp_timesync_heartbeat_timestamp_get  __timesync_heartbeat_timestamp_get;
    __bcm_plp_timesync_do_sync __timesync_do_sync;
    __bcm_plp_mutex_info_set_f __mutex_info_set;
    __bcm_plp_l1_intr_status_get_f __l1_intr_status_get;
    __bcm_plp_base_t_cable_diag_f  __base_t_cable_diag;
    __bcm_plp_base_t_state_mfg_diag_f  __base_t_state_mfg_diag;
} __plp__dispatch__t__; 

#define bcm_plp_l1_intr_status_get(_pd, _a, _b) \
    PLP_CALL((_pd), __l1_intr_status_get, ((_a), (_b)))
#define bcm_plp_mutex_info_set(_pd, _a, _b) \
    PLP_CALL((_pd), __mutex_info_set, ((_a), (_b)))
#define bcm_plp_static_config_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __static_config_set, ((_a), (_b)))
#define bcm_plp_static_config_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __static_config_get, ((_a), (_b)))
#define bcm_plp_init(_pd, _a, _b, _c, _d ) \
    PLP_CALL((_pd), __init, ((_a), (_b), (_c), (_d)))
#define bcm_plp_cleanup(_pd, _a ) \
    PLP_CALL((_pd), __cleanup, ((_a)))
#define bcm_plp_link_status_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __link_status_get, ((_a), (_b)))
#define bcm_plp_mode_config_set(_pd, _a, _b, _c, _d, _e, _f ) \
    PLP_CALL((_pd), __mode_config_set, ((_a), (_b), (_c), (_d), (_e), (_f)))
#define bcm_plp_mode_config_get(_pd, _a, _b, _c, _d, _e, _f  ) \
    PLP_CALL((_pd), __mode_config_get, ((_a), (_b), (_c), (_d), (_e), (_f)))
#define bcm_plp_driver_version_get(_pd, _a, _b, _c, _d) \
    PLP_CALL((_pd), __driver_version_get, ((_a), (_b), (_c), (_d)))
#define bcm_plp_prbs_set(_pd, _a, _b, _c, _d, _e, _f ) \
    PLP_CALL((_pd), __prbs_set, ((_a), (_b), (_c), (_d), (_e), (_f)))
#define bcm_plp_prbs_get(_pd, _a, _b, _c, _d, _e, _f ) \
    PLP_CALL((_pd), __prbs_get, ((_a), (_b), (_c), (_d), (_e), (_f)))
#define bcm_plp_prbs_rx_stat(_pd, _a, _b ) \
    PLP_CALL((_pd), __prbs_rx_stat, ((_a), (_b)))
#define bcm_plp_prbs_clear(_pd, _a, _b ) \
    PLP_CALL((_pd), __prbs_clear, ((_a), (_b)))
#define bcm_plp_prbs_config_get(_pd, _a, _b, _c, _d ) \
    PLP_CALL((_pd), __prbs_config_get, ((_a), (_b), (_c), (_d)))
#define bcm_plp_prbs_status_get(_pd, _a, _b, _c, _d ) \
    PLP_CALL((_pd), __prbs_status_get, ((_a), (_b), (_c), (_d)))
#define bcm_plp_reg_value_set(_pd, _a, _b, _c, _d ) \
    PLP_CALL((_pd), __reg_value_set, ((_a), (_b), (_c), (_d)))
#define bcm_plp_reg_value_get(_pd, _a, _b, _c, _d ) \
    PLP_CALL((_pd), __reg_value_get, ((_a), (_b), (_c), (_d)))
#define bcm_plp_polarity_set(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __polarity_set, ((_a), (_b), (_c)))
#define bcm_plp_polarity_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __polarity_get, ((_a), (_b), (_c)))
#define bcm_plp_rx_pmd_lock_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __rx_pmd_lock_get, ((_a), (_b)))
#define bcm_plp_rev_id(_pd, _a, _b ) \
    PLP_CALL((_pd), __rev_id, ((_a), (_b)))
#define bcm_plp_loopback_set(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __loopback_set, ((_a), (_b), (_c)))
#define bcm_plp_loopback_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __loopback_get, ((_a), (_b), (_c)))
#define bcm_plp_tx_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __tx_set, ((_a), (_b)))
#define bcm_plp_tx_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __tx_get, ((_a), (_b)))
#define bcm_plp_rx_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __rx_set, ((_a), (_b)))
#define bcm_plp_rx_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __rx_get, ((_a), (_b)))
#define bcm_plp_reset_set(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __reset_set, ((_a), (_b), (_c)))
#define bcm_plp_phy_lane_reset_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __phy_lane_reset_set, ((_a), (_b)))
#define bcm_plp_phy_lane_reset_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __phy_lane_reset_get, ((_a), (_b)))
#define bcm_plp_tx_lane_control_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __tx_lane_control_set, ((_a), (_b)))
#define bcm_plp_rx_lane_control_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __rx_lane_control_set, ((_a), (_b)))
#define bcm_plp_tx_lane_control_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __tx_lane_control_get, ((_a), (_b)))
#define bcm_plp_rx_lane_control_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __rx_lane_control_get, ((_a), (_b)))
#define bcm_plp_lane_cross_switch_map_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __lane_cross_switch_map_set, ((_a), (_b)))
#define bcm_plp_lane_cross_switch_map_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __lane_cross_switch_map_get, ((_a), (_b)))
#define bcm_plp_force_tx_training_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __force_tx_training_set, ((_a), (_b)))
#define bcm_plp_force_tx_training_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __force_tx_training_get, ((_a), (_b)))
#define bcm_plp_force_tx_training_status_get(_pd, _a, _b, _c, _d ) \
    PLP_CALL((_pd), __force_tx_training_status_get, ((_a), (_b), (_c), (_d)))
#define bcm_plp_cl73_ability_set(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __cl73_ability_set, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_cl73_ability_get(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __cl73_ability_get, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_cl73_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __cl73_set, ((_a), (_b)))
#define bcm_plp_cl73_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __cl73_get, ((_a), (_b), (_c)))
#define bcm_plp_display_eye_scan(_pd, _a ) \
    PLP_CALL((_pd), __display_eye_scan, ((_a)))
#define bcm_plp_firmware_info_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __firmware_info_get, ((_a), (_b), (_c)))
#define bcm_plp_firmware_set(_pd, _a) \
    PLP_CALL((_pd), __firmware_set, ((_a)))
#define bcm_plp_rxtx_laneswap_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __rxtx_laneswap_set, ((_a), (_b)))
#define bcm_plp_rxtx_laneswap_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __rxtx_laneswap_get, ((_a), (_b)))
#define bcm_plp_pll_sequencer_restart(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __pll_sequencer_restart, ((_a), (_b), (_c)))
#define bcm_plp_fec_enable_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __fec_enable_set, ((_a), (_b)))
#define bcm_plp_fec_enable_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __fec_enable_get, ((_a), (_b)))
#define bcm_plp_phy_status_dump(_pd, _a ) \
    PLP_CALL((_pd), __phy_status_dump, ((_a)))
#define bcm_plp_phy_diagnostics_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __phy_diagnostics_get, ((_a), (_b)))
#define bcm_plp_intr_status_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __intr_status_get, ((_a), (_b), (_c)))
#define bcm_plp_intr_enable_set(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __intr_enable_set, ((_a), (_b), (_c)))
#define bcm_plp_intr_enable_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __intr_enable_get, ((_a), (_b), (_c)))
#define bcm_plp_intr_status_clear(_pd, _a, _b ) \
    PLP_CALL((_pd), __intr_status_clear, ((_a), (_b)))
#define bcm_plp_fc_pcs_chkr_enable_set(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __fc_pcs_chkr_enable_set, ((_a), (_b), (_c)))
#define bcm_plp_fc_pcs_chkr_enable_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __fc_pcs_chkr_enable_get, ((_a), (_b), (_c)))
#define bcm_plp_fc_pcs_chkr_status_get(_pd, _a, _b, _c, _d ) \
    PLP_CALL((_pd), __fc_pcs_chkr_status_get, ((_a), (_b), (_c), (_d)))
#define bcm_plp_eye_margin_proj(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __eye_margin_proj, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_repeater_mode_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __repeater_mode_get, ((_a), (_b)))
#define bcm_plp_repeater_mode_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __repeater_mode_set, ((_a), (_b)))
#define bcm_plp_module_read(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __module_read, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_module_write(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __module_write, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_cfg_gpio_pin_set(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __cfg_gpio_pin_set, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_cfg_gpio_pin_get(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __cfg_gpio_pin_get, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_power_set(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __power_set, ((_a), (_b), (_c)))
#define bcm_plp_power_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __power_get, ((_a), (_b), (_c)))
#define bcm_plp_firmware_lane_config_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __firmware_lane_config_set, ((_a), (_b)))
#define bcm_plp_firmware_lane_config_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __firmware_lane_config_get, ((_a), (_b)))
#define bcm_plp_init_fw_bcast(_pd, _a, _b, _c, _d, _e ) \
    PLP_CALL((_pd), __init_fw_bcast, ((_a), (_b), (_c), (_d), (_e)))
#define bcm_plp_failover_mode_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __failover_mode_set, ((_a), (_b)))
#define bcm_plp_failover_mode_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __failover_mode_get, ((_a), (_b)))
#define bcm_plp_edc_config_set(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __edc_config_set, ((_a), (_b), (_c)))
#define bcm_plp_edc_config_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __edc_config_get, ((_a), (_b), (_c)))
#define bcm_plp_fec_corrected_error_counter(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __fec_corrected_counter_get, ((_a), (_b),(_c)))
#define bcm_plp_fec_uncorrected_error_counter(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __fec_uncorrected_counter_get, ((_a), (_b), (_c)))
#define bcm_plp_event_status_get(_pd, _a, _b , _c) \
    PLP_CALL((_pd), __event_status_get, ((_a), (_b), (_c)))

#ifdef BCM_PLP_BASE_T_PHY
#define bcm_plp_base_t_autoneg_ability_set(_pd, _a, _b) \
    PLP_CALL((_pd), __base_t_autoneg_ability_set, ((_a), (_b)))
#define bcm_plp_base_t_autoneg_ability_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __base_t_autoneg_ability_get, ((_a), (_b)))
#define bcm_plp_base_t_autoneg_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __base_t_autoneg_set, ((_a), (_b)))
#define bcm_plp_base_t_autoneg_get(_pd, _a, _b, _c ) \
    PLP_CALL((_pd), __base_t_autoneg_get, ((_a), (_b), (_c)))
#define bcm_plp_base_t_eee_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __base_t_eee_set, ((_a), (_b)))
#define bcm_plp_base_t_eee_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __base_t_eee_get, ((_a), (_b)))
#define bcm_plp_base_t_power_mode_set(_pd, _a, _b ) \
    PLP_CALL((_pd), __base_t_power_mode_set, ((_a), (_b)))
#define bcm_plp_base_t_power_mode_get(_pd, _a, _b ) \
    PLP_CALL((_pd), __base_t_power_mode_get, ((_a), (_b)))
#define bcm_plp_base_t_cable_diag(_pd, _a, _b) \
     PLP_CALL((_pd), __base_t_cable_diag, ((_a), (_b)))
#define bcm_plp_base_t_state_mfg_diag(_pd, _a, _b, _c, _d, _e) \
     PLP_CALL((_pd), __base_t_state_mfg_diag, ((_a), (_b), (_c), (_d), (_e)))
#endif /* BCM_PLP_BASE_T_PHY */

#define bcm_plp_timesync_config_set(_pd, _a, _b) \
    PLP_CALL((_pd), __timesync_config_set, ((_a), (_b)))
#define bcm_plp_timesync_config_get(_pd, _a, _b) \
    PLP_CALL((_pd), __timesync_config_get, ((_a), (_b)))
#define bcm_plp_timesync_enable_set(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_enable_set, ((_a), (_b), (_c)))
#define bcm_plp_timesync_enable_get(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_enable_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_nco_addend_set(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_nco_addend_set, ((_a), (_b), (_c)))
#define bcm_plp_timesync_nco_addend_get(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_nco_addend_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_framesync_mode_set(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_framesync_mode_set, ((_a), (_b), (_c)))
#define bcm_plp_timesync_framesync_mode_get(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_framesync_mode_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_local_time_set(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_local_time_set, ((_a), (_b), (_c)))
#define bcm_plp_timesync_local_time_get(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_local_time_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_load_ctrl_set(_pd,  _a, _b, _c, _d) \
    PLP_CALL((_pd), __timesync_load_ctrl_set, ((_a), (_b), (_c), (_d)))
#define bcm_plp_timesync_load_ctrl_get(_pd,  _a, _b, _c, _d) \
    PLP_CALL((_pd), __timesync_load_ctrl_get, ((_a), (_b), (_c), (_d)))
#define bcm_plp_timesync_tx_timestamp_offset_set(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_tx_timestamp_offset_set, ((_a), (_b), (_c)))
#define bcm_plp_timesync_tx_timestamp_offset_get(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_tx_timestamp_offset_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_rx_timestamp_offset_set(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_rx_timestamp_offset_set, ((_a), (_b), (_c)))
#define bcm_plp_timesync_rx_timestamp_offset_get(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_rx_timestamp_offset_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_capture_timestamp_get(_pd,   _a, _b, _c) \
    PLP_CALL((_pd), __timesync_capture_timestamp_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_heartbeat_timestamp_get(_pd, _a, _b, _c) \
    PLP_CALL((_pd), __timesync_heartbeat_timestamp_get, ((_a), (_b), (_c)))
#define bcm_plp_timesync_do_sync(_pd, _a) \
    PLP_CALL((_pd), __timesync_do_sync, (_a))

extern __plp__dispatch__t__ plp_evora_dispatch;
extern __plp__dispatch__t__ plp_quadra28_dispatch;
extern __plp__dispatch__t__ plp_dino_dispatch;
extern __plp__dispatch__t__ plp_koi_dispatch;
extern __plp__dispatch__t__ plp_orca_dispatch;

#define _PLP_FALSE (0)

#define PLP_CHECK_PF(__pf, __pa) ((__pf == __PLP_NOT_USED) ? _PLP_API_UNAVAILABLE : (__pf __pa))

#ifdef PLP_EVORA_SUPPORT
    #define PLP_EVORA_COMPARE(_N, _pf, _pa) ((strcmp(_N, "evora")) == 0) ? PLP_CHECK_PF(plp_evora_dispatch._pf, _pa)
#else
    #define PLP_EVORA_COMPARE(_N, _pf, _pa) _PLP_FALSE ? _PLP_FALSE
#endif

#ifdef PLP_DINO_SUPPORT
    #define PLP_DINO_COMPARE(_N, _pf, _pa) ((strcmp(_N, "dino")) == 0) ? PLP_CHECK_PF(plp_dino_dispatch._pf, _pa)
#else
    #define PLP_DINO_COMPARE(_N, _pf, _pa) _PLP_FALSE ? _PLP_FALSE
#endif

#ifdef PLP_QUADRA28_SUPPORT
    #define PLP_QUADRA28_COMPARE(_N, _pf, _pa) ((strcmp(_N, "quadra28")) == 0) ? PLP_CHECK_PF(plp_quadra28_dispatch._pf, _pa)
#else
    #define PLP_QUADRA28_COMPARE(_N, _pf, _pa) _PLP_FALSE ? _PLP_FALSE
#endif

#ifdef PLP_KOI_SUPPORT
    #define PLP_KOI_COMPARE(_N, _pf, _pa) ((strcmp(_N, "koi")) == 0) ? PLP_CHECK_PF(plp_koi_dispatch._pf, _pa)
#else
    #define PLP_KOI_COMPARE(_N, _pf, _pa) _PLP_FALSE ? _PLP_FALSE
#endif

#ifdef PLP_ORCA_SUPPORT
    #define PLP_ORCA_COMPARE(_N, _pf, _pa) ((strcmp(_N, "orca")) == 0) ? PLP_CHECK_PF(plp_orca_dispatch._pf, _pa)
#else
    #define PLP_ORCA_COMPARE(_N, _pf, _pa) _PLP_FALSE ? _PLP_FALSE
#endif

#define PLP_CALL(_N, _pf, _pa) (PLP_DINO_COMPARE(_N, _pf, _pa)  :\
                               (PLP_EVORA_COMPARE(_N, _pf, _pa) :\
                               (PLP_QUADRA28_COMPARE(_N, _pf, _pa):\
                               (PLP_KOI_COMPARE(_N, _pf, _pa):\
                               (PLP_ORCA_COMPARE(_N, _pf, _pa):\
                                _PLP_API_UNAVAILABLE)))))

#endif /* EPDM_H */

