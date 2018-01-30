/*! \file tomahawk_mmu_flex_init.h
 *
 * MMU flex init function prototypes.
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

#ifndef TOMAHAWK_MMU_FLEX_INIT_H
#define TOMAHAWK_MMU_FLEX_INIT_H



/*! @file tomahawk_mmu_flex_init.h
 *  @brief Mmu FlexPort functions and structures.
 *  Details are shown below.
 */


/* Some functions from MMU_CONFIG */
extern int soc_tomahawk_mmu_get_num_l0_nodes_per_port(int unit, int lcl_port,
                                                       int pipe,
                                                       int *number_l0_nodes);


extern int soc_tomahawk_mmu_get_num_l1_uc_nodes_per_port(
    int unit, int lcl_port, int pipe, int *number_l1_uc_nodes);


extern int soc_tomahawk_mmu_get_num_l1_mc_nodes_per_port(
    int unit, int lcl_port, int pipe, int *number_l1_mc_nodes);


extern int soc_tomahawk_mmu_get_l0_base_offset_for_port(int unit, int lcl_port,
                                                         int *l0_offset);


extern int soc_tomahawk_mmu_get_l1_base_offset_for_port(int unit, int lcl_port,
                                                         int uc_bit,
                                                         int *l1_offset);

extern int soc_tomahawk_mmu_get_valid_ipipes_for_xpe(int unit, int xpe,
                                                      int *ipipes);


extern int soc_tomahawk_mmu_get_valid_epipes_for_xpe(int unit, int xpe,
                                                      int *epipes);

extern int soc_tomahawk_mmu_is_xpe_valid(int unit, int xpe, int *valid);


extern int soc_tomahawk_get_min_max_src_ct_speed(int unit,int speed_decode,
                                                  int ct_mode, int ct_profile,
                                                  uint64 *min_speed,
                                                  uint64 *max_speed);


extern int soc_tomahawk_get_asf_xmit_start_count(int unit, int dst_decode,
                                                  int ct_mode,
                                                  uint64 *asf_count);


extern int soc_tomahawk_mmu_get_bst_mode(int unit, int *bst_mode);


extern int soc_tomahawk_mmu_get_pktstat_mode(int unit, int *pktstat_mode);


extern int soc_tomahawk_mmu_get_ct_ep_credit_low_high_threshold(
    int unit, int speed_decode, int frequency, int oversub, int oversub_ratio,
    uint64 *ep_credit_low, uint64 *ep_credit_hi);


extern int soc_tomahawk_get_ct_fifo_threshold_depth(int unit, int dst_decode,
                                                     int ct_mode, int oversub,
                                                     uint64 *fifo_thresh,
                                                     uint64 *fifo_depth);


extern int soc_tomahawk_mmu_return_dip_delay_amount(int unit, int port_speed,
                                                     uint64 *dip_amount);


extern int soc_tomahawk_mmu_thdi_get_pg_hdrm_setting(int unit, int speed,
                                                      int line_rate,
                                                      uint32 *setting);


/*PORT FLUSH Functions from RQE_VBS_MTRO */
extern int soc_tomahawk_mmu_vbs_port_flush(
    int unit, soc_port_resource_t *port_resource_t, uint64 set_val);


extern int soc_tomahawk_mmu_rqe_port_flush(
    int unit, soc_port_resource_t *port_resource_t, uint64 set_val);


extern int soc_tomahawk_mmu_mtro_port_flush(
    int unit, soc_port_resource_t *port_resource_t, uint64 set_val);


/* THDI/WRED Clear Functions */
extern int soc_tomahawk_mmu_thdi_bst_clr(int                  unit,
                                          soc_port_resource_t *port_resource_t);


extern int soc_tomahawk_mmu_wred_clr(int                  unit,
                                      soc_port_resource_t *port_resource_t);


extern int soc_tomahawk_mmu_get_xpe_from_pipe(int pipe, pipe_t direction,
                                               int* xpe_offs);


extern int soc_tomahawk_mmu_ctr_clr(int                  unit,
                                     soc_port_resource_t *port_resource_t);


/* THDI Setup function */
extern int soc_tomahawk_mmu_thdi_setup(int                  unit,
                                        soc_port_resource_t *port_resource_t,
                                        int                  lossy);


/* TDM Port EP Credit Clear function*/
extern int soc_tomahawk_mmu_clear_prev_ep_credits(
    int unit, soc_port_resource_t *port_resource_t);


/*PORT MAPPING */
extern int soc_tomahawk_mmu_set_mmu_to_phy_port_mapping(
    int unit,soc_port_resource_t *port_resource_t);


/*VBS Credit Clear Functions */
extern int soc_tomahawk_mmu_clear_vbs_credit_memories(
    int unit,soc_port_resource_t *port_resource_t);


/* MTRO Credit Clear Functions */
extern int soc_tomahawk_mmu_clear_mtro_bucket_mems(
    int unit, soc_port_resource_t *port_resource_t);


/*CT Setting Functions */
extern int soc_tomahawk_mmu_reinit_ct_setting(
    int unit,soc_port_resource_t *port_resource_t, soc_asf_mode_e ct_mode,
    soc_asf_mem_profile_e ct_profile, int frequency,
    int exact_speed, int oversub_ratio);


/* Port Up Sequence */
extern int soc_tomahawk_flex_mmu_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state_t);


/* Port Down Sequence */
extern int soc_tomahawk_flex_mmu_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state_t);


/* Reconfigure Sequence Phase 1 Phase 2 */
extern int soc_tomahawk_flex_mmu_reconfigure_phase1(
    int unit, soc_port_schedule_state_t *port_schedule_state_t);


extern int soc_tomahawk_flex_mmu_reconfigure_phase2(
    int unit, soc_port_schedule_state_t *port_schedule_state_t);


/* DIP/ForceSAF Sequence */
extern int soc_tomahawk_mmu_reinit_dip_fsaf_setting(
    int unit, soc_port_resource_t *port_resource_t);


/* THDU functions */
extern int soc_tomahawk_mmu_thdu_bst_clr(
    int                  unit,
    soc_port_resource_t *port_resource_t
    );


extern int soc_tomahawk_mmu_thdu_pktstat_clr(
    int                  unit,
    soc_port_resource_t *port_resource_t
    );


extern int soc_tomahawk_mmu_thdu_qgrp_min_limit_config(
    int                  unit,
    soc_port_resource_t *port_resource_t,
    int                  lossy
    );


/*THDM functions */
extern int soc_tomahawk_mmu_thdm_bst_clr (
    int                  unit,
    soc_port_resource_t *port_resource_t
    );


#endif /* TOMAHAWK_MMU_FLEX_INIT_H */
