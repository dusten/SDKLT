/*! \file tomahawk_tdm.h
 *
 * Tomahawk tdm function prototypes.
 */
/*
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

#ifndef TOMAHAWK_TDM_H
#define TOMAHAWK_TDM_H

extern int _soc_tomahawk_tdm_mmu_calendar_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int _soc_tomahawk_tdm_idb_calendar_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int _soc_tomahawk_tdm_oversub_group_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int _soc_tomahawk_tdm_oversub_group_set_sel(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int set_idb, int set_mmu);

extern void _soc_tomahawk_speed_to_ovs_class_mapping(int  unit,
                                                      int  speed,
                                                      int *ovs_class);

extern void soc_tomahawk_get_hsp_info(int  unit,
                                        int  speed,
                                        int  is_idb,
                                        int  is_mmu,
                                        int *is_hsp,
                                        int *same_spacing);

extern void soc_tomahawk_port_ratio_get(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int clport, int *mode, int prev_or_new);

extern int _soc_tomahawk_tdm_pkt_shaper_calendar_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int _soc_tomahawk_tdm_idb_opportunistic_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state, int cpu_op_en,
    int lb_opp_en, int opp1_port_en, int opp2_port_en, int opp_ovr_sub_en);

extern int _soc_tomahawk_tdm_mmu_opportunistic_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int cpu_op_en, int lb_opp_en, int opp1_port_en, int opp2_port_en,
    int opp_ovr_sub_en);

extern int _soc_tomahawk_tdm_idb_hsp_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int _soc_tomahawk_tdm_mmu_hsp_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int _soc_tomahawk_tdm_idb_ppe_credit_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int full_credit_threshold_0,
    int opp_credit_threshold_0,
    int full_credit_threshold_1,
    int opp_credit_threshold_1 );

extern int _soc_tomahawk_tdm_calculation(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    bcmtm_tdm_drv_t *th_tdm_drv);

extern void soc_tomahawk_pipe_map_get(
    int unit,
    soc_port_schedule_state_t *port_schedule_state, uint32 *pipe_map);

extern int soc_tomahawk_tdm_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    bcmtm_tdm_drv_t *th_tdm_drv);

extern int _soc_tomahawk_tdm_idb_dpp_ctrl_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int credits);

extern void soc_tomahawk_ovs_pipe_map_get(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    uint32 *ovs_pipe_map);

extern void soc_print_port_schedule_state(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern void soc_print_port_map(int unit, soc_port_map_type_t *port_map);

extern void soc_print_tdm_schedule_slice( int unit,
    soc_tdm_schedule_t *tdm_schedule, int hpipe, int is_ing);

extern void soc_print_port_resource(int unit,
    soc_port_resource_t *port_resource, int entry_num);

extern void
soc_tomahawk_port_schedule_speed_remap(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

#endif /* TOMAHAWK_TDM_H */
