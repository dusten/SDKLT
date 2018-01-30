/*! \file bcmfp_presel_qual_cfg_internal.h
 *
 * Structures, defines to hold and access FP presel qualifier configurations.
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

#ifndef BCMFP_PRESEL_QUAL_CFG_INTERNAL_H
#define BCMFP_PRESEL_QUAL_CFG_INTERNAL_H

#include <bcmfp/bcmfp_ext_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>

#define BCMFP_PRESEL_QUAL_CFG_DECL                \
    int _rv_;                                     \
    bcmfp_qual_cfg_t _qual_cfg_

/*
 * Typedef:
 *    BCMFP_QUAL_CFG_ADD
 * Purpose:
 *    Qualifier ibus information add macros.
 */
#define BCMFP_PRESEL_QUAL_CFG_ADD(_unit_,                              \
                           _stage_,                                    \
                           _qual_id_,                                  \
                           _qual_flags_,                               \
                           _ctrl_sel_,                                 \
                           _ctrl_sel_val_,                             \
                           _sec0_, _sec0_val_,                         \
                           _sec1_, _sec1_val_,                         \
                           _sec2_, _sec2_val_,                         \
                           _sec3_, _sec3_val_,                         \
                           _sec4_, _sec4_val_,                         \
                           _sec5_, _sec5_val_,                         \
                           _sec6_, _sec6_val_,                         \
                           _sec7_, _sec7_val_,                         \
                           _width0_, _width1_,                         \
                           _width2_, _width3_,                         \
                           _width4_, _width5_,                         \
                           _width6_, _width7_,                         \
                           _chunk_offset0_, _chunk_offset1_,           \
                           _chunk_offset2_, _chunk_offset3_,           \
                           _chunk_offset4_, _chunk_offset5_,           \
                           _chunk_offset6_, _chunk_offset7_,           \
                           _bus_offset0_, _bus_offset1_,               \
                           _bus_offset2_, _bus_offset3_,               \
                           _bus_offset4_, _bus_offset5_,               \
                           _bus_offset6_, _bus_offset7_,               \
                           _num_chunks_)                               \
            do {                                                             \
                _rv_ = bcmfp_presel_qual_cfg_t_init(_unit_, &(_qual_cfg_));  \
                if (SHR_FAILURE(_rv_)) {                                     \
                    SHR_RETURN_VAL_EXIT(_rv_);                               \
                }                                                            \
                (_qual_cfg_).ctrl_sel                   = (_ctrl_sel_);      \
                (_qual_cfg_).ctrl_sel_val               = (_ctrl_sel_val_);  \
                (_qual_cfg_).e_params[0].section      = (_sec0_);            \
                (_qual_cfg_).e_params[0].sec_val      = (_sec0_val_);        \
                (_qual_cfg_).e_params[0].chunk_offset = (_chunk_offset0_);   \
                (_qual_cfg_).e_params[0].width        = (_width0_);          \
                (_qual_cfg_).e_params[0].bus_offset   = (_bus_offset0_);     \
                (_qual_cfg_).e_params[1].section      = (_sec1_);            \
                (_qual_cfg_).e_params[1].sec_val      = (_sec1_val_);        \
                (_qual_cfg_).e_params[1].chunk_offset = (_chunk_offset1_);   \
                (_qual_cfg_).e_params[1].width        = (_width1_);          \
                (_qual_cfg_).e_params[1].bus_offset   = (_bus_offset1_);     \
                (_qual_cfg_).e_params[2].section      = (_sec2_);            \
                (_qual_cfg_).e_params[2].sec_val      = (_sec2_val_);        \
                (_qual_cfg_).e_params[2].chunk_offset = (_chunk_offset2_);   \
                (_qual_cfg_).e_params[2].width        = (_width2_);          \
                (_qual_cfg_).e_params[2].bus_offset   = (_bus_offset2_);     \
                (_qual_cfg_).e_params[3].section      = (_sec3_);            \
                (_qual_cfg_).e_params[3].sec_val      = (_sec3_val_);        \
                (_qual_cfg_).e_params[3].chunk_offset = (_chunk_offset3_);   \
                (_qual_cfg_).e_params[3].width        = (_width3_);          \
                (_qual_cfg_).e_params[3].bus_offset   = (_bus_offset3_);     \
                (_qual_cfg_).e_params[4].section      = (_sec4_);            \
                (_qual_cfg_).e_params[4].sec_val      = (_sec4_val_);        \
                (_qual_cfg_).e_params[4].chunk_offset = (_chunk_offset4_);   \
                (_qual_cfg_).e_params[4].width        = (_width4_);          \
                (_qual_cfg_).e_params[4].bus_offset   = (_bus_offset4_);     \
                (_qual_cfg_).e_params[5].section      = (_sec5_);            \
                (_qual_cfg_).e_params[5].sec_val      = (_sec5_val_);        \
                (_qual_cfg_).e_params[5].chunk_offset = (_chunk_offset5_);   \
                (_qual_cfg_).e_params[5].width        = (_width5_);          \
                (_qual_cfg_).e_params[5].bus_offset   = (_bus_offset5_);     \
                (_qual_cfg_).e_params[6].section      = (_sec6_);            \
                (_qual_cfg_).e_params[6].sec_val      = (_sec6_val_);        \
                (_qual_cfg_).e_params[6].chunk_offset = (_chunk_offset6_);   \
                (_qual_cfg_).e_params[6].width        = (_width6_);          \
                (_qual_cfg_).e_params[6].bus_offset   = (_bus_offset6_);     \
                (_qual_cfg_).e_params[7].section      = (_sec7_);            \
                (_qual_cfg_).e_params[7].sec_val      = (_sec7_val_);        \
                (_qual_cfg_).e_params[7].chunk_offset = (_chunk_offset7_);   \
                (_qual_cfg_).e_params[7].width        = (_width7_);          \
                (_qual_cfg_).e_params[7].bus_offset   = (_bus_offset7_);     \
                (_qual_cfg_).size = ((_width0_) + (_width1_) + (_width2_) +  \
                                    (_width3_) + (_width4_) + (_width5_) +   \
                                    (_width6_) + (_width7_));                \
                (_qual_cfg_).num_chunks = (_num_chunks_);                    \
                _rv_ = bcmfp_presel_qual_cfg_insert((_unit_),                \
                                             (_stage_),                      \
                                             (_qual_id_),                    \
                                             &(_qual_cfg_));                 \
                if (SHR_FAILURE(_rv_)) {                                     \
                    SHR_RETURN_VAL_EXIT(_rv_);                               \
                }                                                            \
            } while(0)

#define BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(_unit_,                  \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_)                \
            BCMFP_PRESEL_QUAL_CFG_ADD(_unit_,                         \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               BCMFP_EXT_CTRL_SEL_DISABLE, 0,         \
                               _sec0_,  _sec0_val_,                   \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _width0_,                              \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _chunk_offset0_,                       \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _bus_offset0_,                         \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               1)

#endif /* BCMFP_PRESEL_QUAL_CFG_INTERNAL_H */
