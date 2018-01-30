/*
 *
 * $Id: epdm_sec.h $
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

#ifndef EPDM_SEC_H
#define EPDM_SEC_H

#include "epdm_bcm_common_sec_defines.h"
#include "epdm_bcm_common_defines.h"
#define __PLP_NOT_USED ((void*)0)

/* CFYE */

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_device_init_f)(bcm_plp_sec_phy_access_t *pa, const bcm_plp_cfye_init_t *init_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_device_uninit_f)(bcm_plp_sec_phy_access_t *pa);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_vport_add_f)(bcm_plp_sec_phy_access_t *pa,
                                             bcm_plp_cfye_vport_handle_t *vport_handle_p,
                                             const bcm_plp_cfye_vport_t *vport_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_vport_remove_f)(bcm_plp_sec_phy_access_t *pa,
                                                const bcm_plp_cfye_vport_handle_t vport_handle);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_vport_update_f)(bcm_plp_sec_phy_access_t *pa,
                                                const bcm_plp_cfye_vport_handle_t vport_handle,
                                                const bcm_plp_cfye_vport_t *vport_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_add_f)(bcm_plp_sec_phy_access_t *pa,
                                            const bcm_plp_cfye_vport_handle_t vport_handle,
                                            bcm_plp_cfye_rule_handle_t *rule_handle_p,
                                            const bcm_plp_cfye_rule_t *rule_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_remove_f)(bcm_plp_sec_phy_access_t *pa,
                                               const bcm_plp_cfye_rule_handle_t rule_handle);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_update_f)(bcm_plp_sec_phy_access_t *pa,
                                               const bcm_plp_cfye_rule_handle_t rule_handle,
                                               const bcm_plp_cfye_rule_t *rule_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_enable_f)(bcm_plp_sec_phy_access_t *pa,
                                               const bcm_plp_cfye_rule_handle_t rule_handle,
                                               const unsigned char fsync);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_disable_f)(bcm_plp_sec_phy_access_t *pa,
                                                const bcm_plp_cfye_rule_handle_t rule_handle,
                                                const unsigned char fsync);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_enable_disable_f)(bcm_plp_sec_phy_access_t *pa,
                                                      const bcm_plp_cfye_rule_handle_t rule_handle_disable,
                                                      const bcm_plp_cfye_rule_handle_t rule_handle_enable,
                                                      const unsigned char enable_all,
                                                      const unsigned char disable_all,
                                                      const unsigned char fsync);
typedef unsigned int (*__bcm_plp_cfye_vport_id_get_f)(bcm_plp_sec_phy_access_t *pa,
                                    const bcm_plp_cfye_vport_handle_t vport_handle);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_device_limits_f)(bcm_plp_sec_phy_access_t *pa,
                                                 unsigned int *max_channel_count_p,
                                                 unsigned int *maxv_port_count_p,
                                                 unsigned int *max_rule_count_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_device_update_f)(bcm_plp_sec_phy_access_t *pa,
                                                 const bcm_plp_cfye_device_t *control_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_statistics_read_f)(bcm_plp_sec_phy_access_t *pa,
                                                   const unsigned int stat_index,
                                                   bcm_plp_cfye_statistics_t *stat_p,
                                                   const unsigned char fsync);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_statistics_summary_read_f)(bcm_plp_sec_phy_access_t *pa,
                                                           const unsigned int start_offset,
                                                           unsigned int *summary_p,
                                                           const unsigned int Count);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_statistics_summary_channel_read_f)(bcm_plp_sec_phy_access_t *pa,
                                                                   bcm_plp_cfye_statistics_channel_summary_t *ch_summary_p);


typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_vport_handle_issame_f)(bcm_plp_sec_phy_access_t *pa,
                                      const bcm_plp_cfye_vport_handle_t handle1_p,
                                      const bcm_plp_cfye_vport_handle_t handle2_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_handle_issame_f)(bcm_plp_sec_phy_access_t *pa,
                                     const bcm_plp_cfye_rule_handle_t handle1_p,
                                     const bcm_plp_cfye_rule_handle_t handle2_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_intr_enable_set_f)(bcm_plp_sec_phy_access_t *pa, const bcm_plp_cfye_intr_t *cfye_intr);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_intr_enable_get_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_cfye_intr_t *cfye_intr);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_intr_status_get_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_cfye_intr_t *cfye_intr_sts);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_intr_status_clear_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_cfye_intr_t *cfye_intr_clear);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_event_status_get_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_cfye_intr_t *cfye_event_sts);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_handle_get_f)(bcm_plp_sec_phy_access_t *pa, const unsigned int rule_index, bcm_plp_cfye_rule_handle_t * const rule_handle_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_rule_index_get_f)(bcm_plp_sec_phy_access_t *pa, const bcm_plp_cfye_rule_handle_t rule_handle, unsigned int * const rule_index_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_vport_handle_get_f)(bcm_plp_sec_phy_access_t *pa, const unsigned int vport_index, bcm_plp_cfye_vport_handle_t * const vport_handle_p);

typedef bcm_plp_cfye_status_t (*__bcm_plp_cfye_vport_index_get_f)(bcm_plp_sec_phy_access_t *pa, const bcm_plp_cfye_vport_handle_t vport_handle, unsigned int * const vport_index_p);

/* SECY */

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_init_f)(bcm_plp_sec_phy_access_t *pa,
                         const bcm_plp_secy_settings_t * const settings_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_uninit_f)(bcm_plp_sec_phy_access_t *pa);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_add_f)(bcm_plp_sec_phy_access_t *pa,
                    const unsigned int vport,
                    bcm_plp_secy_sa_handle_t * const sa_handle_p,
                    const bcm_plp_secy_sa_t * const sa_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_update_f)(bcm_plp_sec_phy_access_t *pa,
                       const bcm_plp_secy_sa_handle_t sa_handle,
                       const bcm_plp_secy_sa_t * const sa_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_read_f)(bcm_plp_sec_phy_access_t *pa,
                     const bcm_plp_secy_sa_handle_t sa_handle,
                     const unsigned int word_offset,
                     const unsigned int word_count,
                     unsigned int * transform_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_remove_f)(bcm_plp_sec_phy_access_t *pa,
                       const bcm_plp_secy_sa_handle_t sa_handle);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_rules_sectag_update_f)(bcm_plp_sec_phy_access_t *pa,
                                 const bcm_plp_secy_rule_sectag_t * const sec_tag_rules_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_bypass_get_f)(bcm_plp_sec_phy_access_t *pa,
                                unsigned char * const fbypass);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_bypass_set_f)(bcm_plp_sec_phy_access_t *pa,
                                const unsigned char fbypass);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_config_set_f)(bcm_plp_sec_phy_access_t *pa);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_limits_f)(bcm_plp_sec_phy_access_t *pa,
                           unsigned int * const max_channel_count_p,
                           unsigned int * const maxv_port_count_p,
                           unsigned int * const max_sa_count_p,
                           unsigned int * const max_sc_count_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_secy_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                            const unsigned int secy_index,
                                                            unsigned int * const count_summary_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_pifc_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                    unsigned int ** const ifc_indexes_pp,
                                                    unsigned int * const num_ifc_indexes_p);
typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_ifc_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                   const unsigned int ifc_index,
                                                   unsigned int * const count_summary_ifc_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_pifc1_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                     unsigned int ** const ifc_indexes_pp,
                                                     unsigned int * const num_ifc_indexes_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_ifc1_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                    const unsigned int ifc_index,
                                                    unsigned int * const count_summary_ifc_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_prxcam_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                      unsigned int ** const rxcam_indexes_pp,
                                                      unsigned int * const num_rxcam_indexes_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_psa_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                   unsigned int ** const sa_indexes_pp,
                                                   unsigned int * const num_sa_indexes_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_sa_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                  const unsigned int sa_index,
                                                  unsigned int * const count_summary_sa_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_channel_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                       unsigned int ** const channel_summary_pp,
                                                       unsigned int * const num_channel_summaries_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_handle_issame_f)(bcm_plp_sec_phy_access_t *pa,
                                   const bcm_plp_secy_sa_handle_t * const handle1_p,
                                   const bcm_plp_secy_sa_handle_t * const handle2_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_handle_sa_index_issame_f)(bcm_plp_sec_phy_access_t *pa,
                                           const bcm_plp_secy_sa_handle_t sa_handle,
                                           const unsigned int sa_index);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_crypt_auth_bypass_len_update_f)(bcm_plp_sec_phy_access_t *pa,
                                        const unsigned int bypass_length);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_nextpn_update_f)(bcm_plp_sec_phy_access_t *pa,
                              const bcm_plp_secy_sa_handle_t sa_handle,
                              const unsigned int next_pn_lo,
                              const unsigned int next_pn_hi,
                              unsigned char * const fnext_pn_written_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_chain_f)(bcm_plp_sec_phy_access_t *pa,
                      const bcm_plp_secy_sa_handle_t active_sa_handle,
                      bcm_plp_secy_sa_handle_t * const new_sa_handle_p,
                      const bcm_plp_secy_sa_t * const new_sa_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_switch_f)(bcm_plp_sec_phy_access_t *pa,
                       const bcm_plp_secy_sa_handle_t active_sa_handle,
                       const bcm_plp_secy_sa_handle_t new_sa_handle,
                       const bcm_plp_secy_sa_t * const new_sa_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_statistics_e_get_f)(bcm_plp_sec_phy_access_t *pa,
                                 const bcm_plp_secy_sa_handle_t sa_handle,
                                 bcm_plp_secy_sa_stat_e_t * const stat_p,
                                 const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_statistics_i_get_f)(bcm_plp_sec_phy_access_t *pa,
                                 const bcm_plp_secy_sa_handle_t sa_handle,
                                 bcm_plp_secy_sa_stat_i_t * const stats_p,
                                 const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_ifc_statistics_e_get_f)(bcm_plp_sec_phy_access_t *pa,
                                  const unsigned int vport,
                                  bcm_plp_secy_ifc_stat_e_t * const stat_p,
                                  const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_ifc_statistics_i_get_f)(bcm_plp_sec_phy_access_t *pa,
                                  unsigned int vport,
                                  bcm_plp_secy_ifc_stat_i_t * const stat_p,
                                  const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_rxcam_statistics_get_f)(bcm_plp_sec_phy_access_t *pa,
                                  const unsigned int scindex,
                                  bcm_plp_secy_rxcam_stat_t * const stats_p,
                                  const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_pnthr_summary_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                           unsigned int ** const sa_indexes_pp,
                                           unsigned int * const num_sa_indexes_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_expired_summary_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                             unsigned int ** const sa_indexes_pp,
                                             unsigned int * const num_sa_indexes_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_rules_mtucheck_update_f)(bcm_plp_sec_phy_access_t *pa,
                                   const unsigned int scindex,
                                   const bcm_plp_secy_sc_rule_mtu_check_t * const mtu_check_rule_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_channel_statistics_get_f)(bcm_plp_sec_phy_access_t *pa,
                                    bcm_plp_secy_channel_stat_t * const stats_p,
                                    const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_channel_packets_inflight_get_f)(bcm_plp_sec_phy_access_t *pa,
                                         unsigned int * const in_flight_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_secy_statistics_e_get_f)(bcm_plp_sec_phy_access_t *pa,
                              const unsigned int vport,
                              bcm_plp_secy_secy_stat_e_t *stats_p,
                              const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_secy_statistics_i_get_f)(bcm_plp_sec_phy_access_t *pa,
                              unsigned int vport,
                              bcm_plp_secy_secy_stat_i_t *stats_p,
                              const unsigned char fsync);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_count_summary_psecy_checkandclear_f)(bcm_plp_sec_phy_access_t *pa,
                                                             unsigned int **secy_indexes_pp,
                                                             unsigned int *num_secy_indexes_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_intr_enable_set_f)(bcm_plp_sec_phy_access_t *pa, const bcm_plp_secy_intr_t *secy_intr);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_intr_enable_get_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_secy_intr_t *secy_intr);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_intr_status_get_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_secy_intr_t *secy_intr_status);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_intr_status_clear_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_secy_intr_t *secy_intr_clear);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_event_status_get_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_secy_intr_t *secy_event_status);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_handle_get_f)(bcm_plp_sec_phy_access_t *pa, const unsigned int sa_index, bcm_plp_secy_sa_handle_t * const sa_handle_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_sa_index_get_f)(bcm_plp_sec_phy_access_t *pa,
                                                               const bcm_plp_secy_sa_handle_t sa_handle,
                                                               unsigned int * const sa_index_p,
                                                               unsigned int * const sc_index_p);

typedef bcm_plp_secy_status_t (*__bcm_plp_secy_device_update_f)(bcm_plp_sec_phy_access_t *pa, const bcm_plp_secy_device_params_t * const control_p);

typedef bcm_plp_sa_builder_status_t (*__bcm_plp_secy_build_transform_record_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_sa_builder_params_t *params, unsigned int *sa_buffer_p);

typedef int (*__bcm_plp_mac_sec_burst_reg_value_get_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_sec_reg_access_t *reg_acc);
typedef int (*__bcm_plp_mac_sec_burst_reg_value_set_f)(bcm_plp_sec_phy_access_t *pa, bcm_plp_sec_reg_access_t *reg_acc);
/* MAC */

typedef int (*__bcm_plp_mac_loopback_set_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_lb_type_t lb_type, unsigned int enable);
typedef int (*__bcm_plp_mac_loopback_get_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_lb_type_t lb_type, unsigned int *enable);
typedef int (*__bcm_plp_mac_fault_option_set_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_fault_option_t fault_option);
typedef int (*__bcm_plp_mac_fault_option_get_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_fault_option_t *fault_option);
typedef int (*__bcm_plp_mac_flow_control_set_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_flow_control_t flow_option);
typedef int (*__bcm_plp_mac_flow_control_get_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_flow_control_t *flow_option);
typedef int (*__bcm_plp_mac_store_and_forward_mode_set_f)(bcm_plp_mac_access_t mac_info, int enable);
typedef int (*__bcm_plp_mac_store_and_forward_mode_get_f)(bcm_plp_mac_access_t mac_info, int *is_enable);
typedef int (*__bcm_plp_reg64_value_set_f)(bcm_plp_access_t phy_info, unsigned int devaddr, unsigned int regaddr, unsigned long long data);
typedef int (*__bcm_plp_reg64_value_get_f)(bcm_plp_access_t phy_info, unsigned int devaddr, unsigned int regaddr, unsigned long long *data);
typedef int (*__bcm_plp_mac_cleanup_f)(bcm_plp_mac_access_t mac_info);
typedef int (*__bcm_plp_mac_max_packet_size_set_f)(bcm_plp_mac_access_t mac_info, int pkt_size);
typedef int (*__bcm_plp_mac_max_packet_size_get_f)(bcm_plp_mac_access_t mac_info, int *pkt_size);
typedef int (*__bcm_plp_mac_runt_threshold_set_f)(bcm_plp_mac_access_t mac_info, int threshold);
typedef int (*__bcm_plp_mac_runt_threshold_get_f)(bcm_plp_mac_access_t mac_info, int *threshold);
typedef int (*__bcm_plp_mac_pad_size_set_f)(bcm_plp_mac_access_t mac_info, int pad_size);
typedef int (*__bcm_plp_mac_pad_size_get_f)(bcm_plp_mac_access_t mac_info, int *pad_size);
typedef int (*__bcm_plp_tx_mac_sa_set_f)(bcm_plp_mac_access_t mac_info, unsigned char mac_sa[6]);
typedef int (*__bcm_plp_tx_mac_sa_get_f)(bcm_plp_mac_access_t mac_info, unsigned char mac_sa[6]);
typedef int (*__bcm_plp_rx_mac_sa_set_f)(bcm_plp_mac_access_t mac_info, unsigned char mac_sa[6]);
typedef int (*__bcm_plp_rx_mac_sa_get_f)(bcm_plp_mac_access_t mac_info, unsigned char mac_sa[6]);
typedef int (*__bcm_plp_mac_tx_avg_ipg_set_f)(bcm_plp_mac_access_t mac_info, int avg_ipg);
typedef int (*__bcm_plp_mac_tx_avg_ipg_get_f)(bcm_plp_mac_access_t mac_info, int *avg_ipg);
typedef int (*__bcm_plp_mac_tx_preamble_length_set_f)(bcm_plp_mac_access_t mac_info, int preamble_length);
typedef int (*__bcm_plp_mac_tx_preamble_length_get_f)(bcm_plp_mac_access_t mac_info, int *preamble_length);
typedef int (*__bcm_plp_mac_pause_control_set_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_pause_control_t pause_control);
typedef int (*__bcm_plp_mac_pause_control_get_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_pause_control_t *pause_control);
typedef int (*__bcm_plp_mac_pfc_control_set_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_pfc_control_t pfc_ctrl);
typedef int (*__bcm_plp_mac_pfc_control_get_f)(bcm_plp_mac_access_t mac_info, bcm_plp_mac_pfc_control_t *pfc_ctrl);
typedef int (*__bcm_plp_mac_diagnostic_dump_f)(bcm_plp_mac_access_t mac_info);

typedef struct __plp__sec__dispatch__s__ {
    __bcm_plp_cfye_device_init_f __cfye_device_init;
    __bcm_plp_cfye_device_uninit_f __cfye_device_uninit;
    __bcm_plp_cfye_vport_add_f __cfye_vport_add;
    __bcm_plp_cfye_vport_remove_f __cfye_vport_remove;
    __bcm_plp_cfye_vport_update_f __cfye_vport_update;
    __bcm_plp_cfye_rule_add_f __cfye_rule_add;
    __bcm_plp_cfye_rule_remove_f __cfye_rule_remove;
    __bcm_plp_cfye_rule_update_f __cfye_rule_update;
    __bcm_plp_cfye_rule_enable_f __cfye_rule_enable;
    __bcm_plp_cfye_rule_disable_f __cfye_rule_disable;
    __bcm_plp_cfye_rule_enable_disable_f __cfye_rule_enable_disable;
    __bcm_plp_cfye_vport_id_get_f __cfye_vport_id_get;
    __bcm_plp_cfye_device_limits_f __cfye_device_limits;
    __bcm_plp_cfye_device_update_f __cfye_device_update;
    __bcm_plp_cfye_statistics_read_f __cfye_statistics_read;
    __bcm_plp_cfye_statistics_summary_read_f __cfye_statistics_summary_read;
    __bcm_plp_cfye_statistics_summary_channel_read_f __cfye_statistics_summary_channel_read;
    __bcm_plp_cfye_vport_handle_issame_f __cfye_vport_handle_issame;
    __bcm_plp_cfye_rule_handle_issame_f __cfye_rule_handle_issame;
    __bcm_plp_cfye_intr_enable_set_f __cfye_intr_enable_set;
    __bcm_plp_cfye_intr_enable_get_f __cfye_intr_enable_get;
    __bcm_plp_cfye_intr_status_get_f __cfye_intr_status_get;
    __bcm_plp_cfye_intr_status_clear_f __cfye_intr_status_clear;
    __bcm_plp_cfye_event_status_get_f __cfye_event_status_get;
    __bcm_plp_cfye_rule_handle_get_f __cfye_rule_handle_get;
    __bcm_plp_cfye_rule_index_get_f __cfye_rule_index_get;
    __bcm_plp_cfye_vport_handle_get_f __cfye_vport_handle_get;
    __bcm_plp_cfye_vport_index_get_f __cfye_vport_index_get;
    void* reserved6;
    void* reserved7;
    void* reserved8;
    void* reserved9;
    void* reserved10;
    __bcm_plp_secy_device_init_f __secy_device_init;
    __bcm_plp_secy_device_uninit_f __secy_device_uninit;
    __bcm_plp_secy_sa_add_f __secy_sa_add;
    __bcm_plp_secy_sa_update_f __secy_sa_update;
    __bcm_plp_secy_sa_read_f __secy_sa_read;
    __bcm_plp_secy_sa_remove_f __secy_sa_remove;
    __bcm_plp_secy_rules_sectag_update_f __secy_rules_sectag_update;
    __bcm_plp_secy_bypass_get_f __secy_bypass_get;
    __bcm_plp_secy_bypass_set_f __secy_bypass_set;
    __bcm_plp_secy_config_set_f __secy_config_set;
    __bcm_plp_secy_device_limits_f __secy_device_limits;
    __bcm_plp_secy_device_count_summary_secy_checkandclear_f __secy_device_count_summary_secy_checkandclear;
    __bcm_plp_secy_device_count_summary_pifc_checkandclear_f __secy_device_count_summary_pifc_checkandclear;
    __bcm_plp_secy_device_count_summary_ifc_checkandclear_f __secy_device_count_summary_ifc_checkandclear;
    __bcm_plp_secy_device_count_summary_pifc1_checkandclear_f __secy_device_count_summary_pifc1_checkandclear;
    __bcm_plp_secy_device_count_summary_ifc1_checkandclear_f __secy_device_count_summary_ifc1_checkandclear;
    __bcm_plp_secy_device_count_summary_prxcam_checkandclear_f __secy_device_count_summary_prxcam_checkandclear;
    __bcm_plp_secy_device_count_summary_psa_checkandclear_f __secy_device_count_summary_psa_checkandclear;
    __bcm_plp_secy_device_count_summary_sa_checkandclear_f __secy_device_count_summary_sa_checkandclear;
    __bcm_plp_secy_device_count_summary_channel_checkandclear_f __secy_device_count_summary_channel_checkandclear;
    __bcm_plp_secy_sa_handle_issame_f __secy_sa_handle_issame;
    __bcm_plp_secy_sa_handle_sa_index_issame_f __secy_sa_handle_sa_index_issame;
    __bcm_plp_secy_crypt_auth_bypass_len_update_f __secy_crypt_auth_bypass_len_update;
    __bcm_plp_secy_sa_nextpn_update_f __secy_sa_nextpn_update;
    __bcm_plp_secy_sa_chain_f __secy_sa_chain;
    __bcm_plp_secy_sa_switch_f __secy_sa_switch;
    __bcm_plp_secy_sa_statistics_e_get_f __secy_sa_statistics_e_get;
    __bcm_plp_secy_sa_statistics_i_get_f __secy_sa_statistics_i_get;
    __bcm_plp_secy_ifc_statistics_e_get_f __secy_ifc_statistics_e_get;
    __bcm_plp_secy_ifc_statistics_i_get_f __secy_ifc_statistics_i_get;
    __bcm_plp_secy_rxcam_statistics_get_f __secy_rxcam_statistics_get;
    __bcm_plp_secy_sa_pnthr_summary_checkandclear_f __secy_sa_pnthr_summary_checkandclear;
    __bcm_plp_secy_sa_expired_summary_checkandclear_f __secy_sa_expired_summary_checkandclear;
    __bcm_plp_secy_rules_mtucheck_update_f __secy_rules_mtucheck_update;
    __bcm_plp_secy_channel_statistics_get_f __secy_channel_statistics_get;
    __bcm_plp_secy_channel_packets_inflight_get_f __secy_channel_packets_inflight_get;
    __bcm_plp_secy_secy_statistics_e_get_f __secy_secy_statistics_e_get;
    __bcm_plp_secy_secy_statistics_i_get_f __secy_secy_statistics_i_get;
    __bcm_plp_secy_device_count_summary_psecy_checkandclear_f __secy_device_count_summary_psecy_checkandclear;
    __bcm_plp_secy_intr_enable_set_f __secy_intr_enable_set;
    __bcm_plp_secy_intr_enable_get_f __secy_intr_enable_get;
    __bcm_plp_secy_intr_status_get_f __secy_intr_status_get;
    __bcm_plp_secy_intr_status_clear_f __secy_intr_status_clear;
    __bcm_plp_secy_event_status_get_f __secy_event_status_get;
    __bcm_plp_secy_sa_handle_get_f __secy_sa_handle_get;
    __bcm_plp_secy_sa_index_get_f __secy_sa_index_get;
    __bcm_plp_secy_device_update_f __secy_device_update;
    __bcm_plp_secy_build_transform_record_f __build_transform_record;
    __bcm_plp_mac_sec_burst_reg_value_get_f __mac_sec_burst_reg_value_get;
    __bcm_plp_mac_sec_burst_reg_value_set_f __mac_sec_burst_reg_value_set;
    void* reserved14;
    void* reserved15;
    void* reserved16;
    void* reserved17;
    void* reserved18;
    void* reserved19;
    void* reserved20;
    __bcm_plp_mac_loopback_set_f __mac_loopback_set;
    __bcm_plp_mac_loopback_get_f __mac_loopback_get;
    __bcm_plp_mac_fault_option_set_f __mac_fault_option_set;
    __bcm_plp_mac_fault_option_get_f __mac_fault_option_get;
    __bcm_plp_mac_flow_control_set_f __mac_flow_control_set;
    __bcm_plp_mac_flow_control_get_f __mac_flow_control_get;
    __bcm_plp_mac_store_and_forward_mode_set_f __mac_store_and_forward_mode_set;
    __bcm_plp_mac_store_and_forward_mode_get_f __mac_store_and_forward_mode_get;
    __bcm_plp_mac_cleanup_f __mac_cleanup;
    __bcm_plp_mac_max_packet_size_set_f __mac_max_packet_size_set;
    __bcm_plp_mac_max_packet_size_get_f __mac_max_packet_size_get;
    __bcm_plp_mac_runt_threshold_set_f __mac_runt_threshold_set;
    __bcm_plp_mac_runt_threshold_get_f __mac_runt_threshold_get;
    __bcm_plp_mac_pad_size_set_f __mac_pad_size_set;
    __bcm_plp_mac_pad_size_get_f __mac_pad_size_get;
    __bcm_plp_tx_mac_sa_set_f __tx_mac_sa_set;
    __bcm_plp_tx_mac_sa_get_f __tx_mac_sa_get;
    __bcm_plp_rx_mac_sa_set_f __rx_mac_sa_set;
    __bcm_plp_rx_mac_sa_get_f __rx_mac_sa_get;
    __bcm_plp_mac_tx_avg_ipg_set_f __tx_avg_ipg_set;
    __bcm_plp_mac_tx_avg_ipg_get_f __tx_avg_ipg_get;
    __bcm_plp_mac_tx_preamble_length_set_f __mac_tx_preamble_length_set;
    __bcm_plp_mac_tx_preamble_length_get_f __mac_tx_preamble_length_get;
    __bcm_plp_mac_pause_control_set_f __mac_pause_control_set;
    __bcm_plp_mac_pause_control_get_f __mac_pause_control_get;
    __bcm_plp_mac_pfc_control_set_f __mac_pfc_control_set;
    __bcm_plp_mac_pfc_control_get_f __mac_pfc_control_get;
    __bcm_plp_reg64_value_set_f __reg64_value_set;
    __bcm_plp_reg64_value_get_f __reg64_value_get;
    __bcm_plp_mac_diagnostic_dump_f  __mac_diag_dump;
    void* reserved22;
    void* reserved23;
    void* reserved24;
    void* reserved25;
    void* reserved26;
    void* reserved27;
    void* reserved28;
    void* reserved29;
    void* reserved30;
} __plp__sec__dispatch__t__; 

/* CFYE */ 
#define bcm_plp_cfye_device_init(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_device_init, ((_a), (_b)))

#define bcm_plp_cfye_device_uninit(_pd, a) \
    PLP_SEC_CALL((_pd), __cfye_device_uninit, ((a)))

#define bcm_plp_cfye_vport_add(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_vport_add, ((_a), (_b), (_c)))

#define bcm_plp_cfye_vport_remove(_pd, _a, _b) \
   PLP_SEC_CALL((_pd), __cfye_vport_remove, ((_a), (_b)))

#define bcm_plp_cfye_vport_update(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_vport_update, ((_a), (_b), (_c)))

#define bcm_plp_cfye_rule_add(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __cfye_rule_add, ((_a), (_b), (_c), (_d)))

#define bcm_plp_cfye_rule_remove(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_rule_remove, ((_a), (_b)))

#define bcm_plp_cfye_rule_update(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_rule_update, ((_a), (_b), (_c)))

#define bcm_plp_cfye_rule_enable(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_rule_enable, ((_a), (_b), (_c)))

#define bcm_plp_cfye_rule_disable(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_rule_disable, ((_a), (_b), (_c)))

#define bcm_plp_cfye_rule_enable_disable(_pd, _a, _b, _c, _d, _e, _f) \
     PLP_SEC_CALL((_pd), __cfye_rule_enable_disable, ((_a), (_b), (_c), (_d), (_e), (_f)))

#define bcm_plp_cfye_vport_id_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_vport_id_get, ((_a), (_b)))

#define bcm_plp_cfye_device_limits(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __cfye_device_limits, ((_a), (_b), (_c), (_d)))

#define bcm_plp_cfye_device_update(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_device_update, ((_a), (_b)))

#define bcm_plp_cfye_statistics_read(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __cfye_statistics_read, ((_a), (_b), (_c), (_d)))

#define bcm_plp_cfye_statistics_summary_read(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __cfye_statistics_summary_read, ((_a), (_b), (_c), (_d)))

#define bcm_plp_cfye_statistics_summary_channel_read(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_statistics_summary_channel_read, ((_a), (_b)))

#define bcm_plp_cfye_vport_handle_issame(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_vport_handle_issame, ((_a), (_b), (_c)))

#define bcm_plp_cfye_rule_handle_issame(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_rule_handle_issame, ((_a), (_b), (_c)))

#define bcm_plp_cfye_intr_enable_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_intr_enable_set, ((_a), (_b)))

#define bcm_plp_cfye_intr_enable_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_intr_enable_get, ((_a), (_b)))

#define bcm_plp_cfye_intr_status_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_intr_status_get, ((_a), (_b)))

#define bcm_plp_cfye_intr_status_clear(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_intr_status_clear, ((_a), (_b)))

#define bcm_plp_cfye_event_status_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __cfye_event_status_get, ((_a), (_b)))

#define bcm_plp_cfye_rule_handle_get(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_rule_handle_get, ((_a), (_b), (_c))) 

#define bcm_plp_cfye_rule_index_get(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_rule_index_get, ((_a), (_b), (_c)))

#define bcm_plp_cfye_vport_handle_get(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_vport_handle_get, ((_a), (_b), (_c)))

#define bcm_plp_cfye_vport_index_get(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __cfye_vport_index_get, ((_a), (_b), (_c)))

/* SECY */
#define  bcm_plp_secy_device_init(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_device_init, ((_a), (_b)))

#define  bcm_plp_secy_device_uninit(_pd, _a) \
    PLP_SEC_CALL((_pd), __secy_device_uninit, ((_a)))

#define  bcm_plp_secy_sa_add(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_sa_add, ((_a), (_b), (_c), (_d))) 

#define  bcm_plp_secy_sa_update(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_sa_update, ((_a), (_b), (_c)))

#define  bcm_plp_secy_sa_read(_pd, _a, _b, _c, _d, _e) \
    PLP_SEC_CALL((_pd), __secy_sa_read, ((_a), (_b), (_c), (_d), (_e)))

#define  bcm_plp_secy_sa_remove(_pd, _a, _b) \
    PLP_SEC_CALL((_pd),  __secy_sa_remove, ((_a), (_b)))    

#define  bcm_plp_secy_rules_sectag_update(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_rules_sectag_update, ((_a), (_b)))

#define  bcm_plp_secy_bypass_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_bypass_get, ((_a), (_b)))

#define  bcm_plp_secy_bypass_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_bypass_set, ((_a), (_b)))

#define  bcm_plp_secy_config_set(_pd, _a) \
    PLP_SEC_CALL((_pd), __secy_config_set, ((_a)))

#define  bcm_plp_secy_device_limits(_pd, _a, _b, _c, _d, _e) \
    PLP_SEC_CALL((_pd), __secy_device_limits, ((_a), (_b), (_c), (_d), (_e)))

#define  bcm_plp_secy_device_count_summary_secy_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_secy_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_pifc_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_pifc_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_ifc_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_ifc_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_pifc1_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_pifc1_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_ifc1_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_ifc1_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_prxcam_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_prxcam_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_psa_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_psa_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_sa_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_sa_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_device_count_summary_channel_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_channel_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_sa_handle_issame(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_sa_handle_issame, ((_a), (_b), (_c)))

#define  bcm_plp_secy_sa_handle_sa_index_issame(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_sa_handle_sa_index_issame, ((_a), (_b), (_c))) 

#define  bcm_plp_secy_crypt_auth_bypass_len_update(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_crypt_auth_bypass_len_update, ((_a), (_b)))

#define  bcm_plp_secy_sa_nextpn_update(_pd, _a, _b, _c, _d, _e) \
    PLP_SEC_CALL((_pd), __secy_sa_nextpn_update, ((_a), (_b), (_c), (_d), (_e)))

#define  bcm_plp_secy_sa_chain(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_sa_chain, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_sa_switch(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_sa_switch, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_sa_statistics_e_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_sa_statistics_e_get, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_sa_statistics_i_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_sa_statistics_i_get, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_ifc_statistics_e_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_ifc_statistics_e_get, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_ifc_statistics_i_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_ifc_statistics_i_get, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_rxcam_statistics_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_rxcam_statistics_get, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_sa_pnthr_summary_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_sa_pnthr_summary_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_sa_expired_summary_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_sa_expired_summary_checkandclear, ((_a), (_b), (_c)))

#define  bcm_plp_secy_rules_mtucheck_update(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_rules_mtucheck_update, ((_a), (_b), (_c)))

#define  bcm_plp_secy_channel_statistics_get(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_channel_statistics_get, ((_a), (_b), (_c)))

#define  bcm_plp_secy_channel_packets_inflight_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_channel_packets_inflight_get, ((_a), (_b)))

#define  bcm_plp_secy_secy_statistics_e_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_secy_statistics_e_get, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_secy_statistics_i_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_secy_statistics_i_get, ((_a), (_b), (_c), (_d)))

#define  bcm_plp_secy_device_count_summary_psecy_checkandclear(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_device_count_summary_psecy_checkandclear, ((_a), (_b), (_c)))

#define bcm_plp_secy_intr_enable_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_intr_enable_set, ((_a), (_b)))

#define bcm_plp_secy_intr_enable_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_intr_enable_get, ((_a), (_b)))

#define bcm_plp_secy_intr_status_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_intr_status_get, ((_a), (_b)))

#define bcm_plp_secy_intr_status_clear(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_intr_status_clear, ((_a), (_b)))

#define bcm_plp_secy_event_status_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_event_status_get, ((_a), (_b)))

#define bcm_plp_secy_sa_handle_get(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __secy_sa_handle_get, ((_a), (_b), (_c)))

#define bcm_plp_secy_sa_index_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __secy_sa_index_get, ((_a), (_b), (_c), (_d)))

#define bcm_plp_secy_device_update(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __secy_device_update, ((_a), (_b)))

#define bcm_plp_secy_build_transform_record(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __build_transform_record, ((_a), (_b), (_c)))

#define bcm_plp_mac_sec_burst_reg_value_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_sec_burst_reg_value_get, ((_a), (_b)))

#define bcm_plp_mac_sec_burst_reg_value_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_sec_burst_reg_value_set, ((_a), (_b)))
/* MAC */

#define bcm_plp_mac_loopback_set(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __mac_loopback_set, ((_a), (_b), (_c)))

#define bcm_plp_mac_loopback_get(_pd, _a, _b, _c) \
    PLP_SEC_CALL((_pd), __mac_loopback_get, ((_a), (_b), (_c)))

#define bcm_plp_mac_fault_option_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_fault_option_set, ((_a), (_b)))

#define bcm_plp_mac_fault_option_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_fault_option_get, ((_a), (_b)))

#define bcm_plp_mac_flow_control_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_flow_control_set, ((_a), (_b)))

#define bcm_plp_mac_flow_control_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_flow_control_get, ((_a), (_b)))

#define bcm_plp_mac_store_and_forward_mode_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_store_and_forward_mode_set, ((_a), (_b)))

#define bcm_plp_mac_store_and_forward_mode_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_store_and_forward_mode_get, ((_a), (_b)))

#define bcm_plp_reg64_value_set(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __reg64_value_set, ((_a), (_b), (_c), (_d)))

#define bcm_plp_reg64_value_get(_pd, _a, _b, _c, _d) \
    PLP_SEC_CALL((_pd), __reg64_value_get, ((_a), (_b), (_c), (_d)))

#define bcm_plp_mac_cleanup(_pd, _a) \
    PLP_SEC_CALL((_pd), __mac_cleanup, ((_a)))

#define bcm_plp_mac_max_packet_size_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_max_packet_size_set, ((_a), (_b)))

#define bcm_plp_mac_max_packet_size_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_max_packet_size_get, ((_a), (_b)))

#define bcm_plp_mac_runt_threshold_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_runt_threshold_set, ((_a), (_b)))

#define bcm_plp_mac_runt_threshold_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_runt_threshold_get, ((_a), (_b)))

#define bcm_plp_mac_pad_size_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_pad_size_set, ((_a), (_b)))

#define bcm_plp_mac_pad_size_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_pad_size_get, ((_a), (_b)))

#define bcm_plp_tx_mac_sa_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __tx_mac_sa_set, ((_a), (_b)))

#define bcm_plp_tx_mac_sa_get(_pd, _a, _b)\
    PLP_SEC_CALL((_pd), __tx_mac_sa_get, ((_a), (_b)))

#define bcm_plp_rx_mac_sa_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __rx_mac_sa_set, ((_a), (_b)))

#define bcm_plp_rx_mac_sa_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __rx_mac_sa_get, ((_a), (_b)))

#define bcm_plp_mac_tx_avg_ipg_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __tx_avg_ipg_set, ((_a), (_b)))

#define bcm_plp_mac_tx_avg_ipg_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __tx_avg_ipg_get, ((_a), (_b)))

#define bcm_plp_mac_tx_preamble_length_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_tx_preamble_length_set, ((_a), (_b)))

#define bcm_plp_mac_tx_preamble_length_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_tx_preamble_length_get, ((_a), (_b)))

#define bcm_plp_mac_pause_control_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_pause_control_set, ((_a), (_b)))

#define bcm_plp_mac_pause_control_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_pause_control_get, ((_a), (_b)))

#define bcm_plp_mac_pfc_control_set(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_pfc_control_set, ((_a), (_b)))

#define bcm_plp_mac_pfc_control_get(_pd, _a, _b) \
    PLP_SEC_CALL((_pd), __mac_pfc_control_get, ((_a), (_b)))

#define bcm_plp_mac_diagnostic_dump(_pd, _a) \
    PLP_SEC_CALL((_pd), __mac_diag_dump, (_a))

extern __plp__sec__dispatch__t__ plp_evora_sec_dispatch;

#define _PLP_FALSE (0)

#define PLP_CHECK_PF(__pf, __pa) ((__pf == __PLP_NOT_USED) ? _PLP_API_UNAVAILABLE : (__pf __pa))

#ifdef PLP_MACSEC_SUPPORT
    #define PLP_EVORA_SEC_COMPARE(_N, _pf, _pa) (strcmp(_N, "evora")) == 0 ? PLP_CHECK_PF(plp_evora_sec_dispatch._pf, _pa)
#else
    #define PLP_EVORA_SEC_COMPARE(_N, _pf, _pa) _PLP_FALSE ? _PLP_FALSE
#endif

#define PLP_SEC_CALL(_N, _pf, _pa) (PLP_EVORA_SEC_COMPARE(_N, _pf, _pa)  :\
                                _PLP_API_UNAVAILABLE)

#endif /* EPDM_SEC_H */
