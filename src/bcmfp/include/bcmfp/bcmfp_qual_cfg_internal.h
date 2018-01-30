/*! \file bcmfp_qual_cfg_internal.h
 *
 * Defines, Enums and Structures to represent FP qualifiers
 * related information.
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

#ifndef BCMFP_QUAL_CFG_INTERNAL_H
#define BCMFP_QUAL_CFG_INTERNAL_H

#include <bcmfp/bcmfp_ext_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmdrd/bcmdrd_types.h>

#define BCMFP_QUAL_MAX_CHUNKS 128

#define BCMFP_QUAL_OFFSETS_MAX 128

/* Inports has the maximum bitmap size */
#define BCMFP_QUAL_BITMAP_MAX_WORDS  BCMDRD_PBMP_WORD_MAX

typedef enum bcmfp_qual_flags_e {

    BCMFP_QUAL_FLAGS_PMUX = 0,

    BCMFP_QUAL_FLAGS_NOT_IN_SINGLE,

    BCMFP_QUAL_FLAGS_NOT_IN_DBLINTRA,

    BCMFP_QUAL_NO_EM_SUPPORT,

    BCMFP_QUAL_NO_IFP_SUPPORT,

    BCMFP_QUAL_FLAGS_COUNT,

} bcmfp_qual_flags_t;

typedef struct bcmfp_qual_flags_bmp_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_QUAL_FLAGS_COUNT)];
} bcmfp_qual_flags_bmp_t;

typedef struct bcmfp_qual_offset_cfg_s {

    bcmfp_ext_section_t section;

    uint16_t sec_val;

    uint16_t width;

    uint16_t offset;

    uint16_t bus_offset;

    uint16_t chunk_offset;

    uint8_t extracted;

    uint8_t bits_used;

    uint8_t size;

} bcmfp_qual_offset_cfg_t;

typedef struct bcmfp_qual_cfg_s {

    bcmfp_qual_flags_bmp_t qual_flags;

    bcmfp_ext_ctrl_sel_t ctrl_sel;

    uint16_t ctrl_sel_val;

    uint8_t intraslice;

    uint8_t secondary;

    bcmfp_ext_section_t section;

    uint16_t sec_val;

    bcmfp_ext_ctrl_sel_t pri_ctrl_sel;

    uint16_t pri_ctrl_sel_val;

    bcmfp_ext_ctrl_sel_t sec_ctrl_sel;

    uint16_t sec_ctrl_sel_val;

    bcmfp_qual_offset_cfg_t e_params[BCMFP_QUAL_MAX_CHUNKS];

    uint32_t size;

    uint8_t num_chunks;

    uint8_t update_count;

} bcmfp_qual_cfg_t;

typedef struct bcmfp_qual_cfg_info_s {

     bcmfp_qual_t qual;

     bcmfp_qual_cfg_t *qual_cfg_arr;

     /* Will specify the number of available combinations
      * in qual_cfg_arr array.
      */
     uint8_t num_qual_cfg;

} bcmfp_qual_cfg_info_t;

typedef struct bcmfp_qual_cfg_info_db_s {

    int num_quals;

    bcmfp_qual_cfg_info_t *qual_cfg_info[BCMFP_QUAL_COUNT];

} bcmfp_qual_cfg_info_db_t;

typedef struct bcmfp_qual_bitmap_s {

    bcmfp_qual_t qual;

    uint32_t bitmap[BCMFP_QUAL_BITMAP_MAX_WORDS];

    struct bcmfp_qual_bitmap_s *next;

} bcmfp_qual_bitmap_t;

/* Qualifier data/mask buffer. */
typedef uint32_t bcmfp_qual_data_t[BCMFP_ENTRY_WORDS_MAX];

#define BCMFP_QUAL_DATA_CLEAR(_data_) \
        sal_memset((_data_), 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t))

#define BCMFP_GROUP_QUAL_WIDTH_GET(_q_offset_, _width_)            \
       do {                                                        \
            int i;                                                 \
            _width_ = 0;                                           \
            for (i = 0; i < (_q_offset_)->num_offsets; i++)  {     \
                (_width_) = (_width_) + (_q_offset_)->width[i];    \
            }                                                      \
       } while(0)

/*
 * Typedef:
 *    BCMFP_QUAL_CFG_DECL
 * Purpose:
 *    BCMFP_QUAL_CFG_XXX macros require the following declaration in any
 *    function which uses them.
 */
#define BCMFP_QUAL_CFG_DECL                                                \
    int _rv_;                                                              \
    bcmfp_qual_cfg_t _qual_cfg_

/*
 * Typedef:
 *   BCMFP_QUAL_CFG_INIT
 * Purpose:
 *    To re-init the bcmfp_qual_cfg_t structure.
 */
#define BCMFP_QUAL_CFG_INIT(_unit_)                                        \
    _rv_ = bcmfp_qual_cfg_t_init(_unit_, &(_qual_cfg_));                   \
    if (SHR_FAILURE(_rv_)) {                                               \
        SHR_RETURN_VAL_EXIT(_rv_);                                         \
    }

/*
 * Typedef:
 *   BCMFP_QUAL_CFG_OFFSET_ADD
 * Purpose:
 *   To update the selectors, offset and width of the
 *   bcmfp_qual_cfg_t variable based on the index.
 */
#define BCMFP_QUAL_CFG_OFFSET_ADD(_index_,                                 \
                                   _section_,                              \
                                   _sec_val_,                              \
                                   _offset_,                               \
                                   _width_)                                \
                (_qual_cfg_).section                        = (_section_); \
                (_qual_cfg_).sec_val                        = (_sec_val_); \
                (_qual_cfg_).e_params[_index_].section      = (_section_); \
                (_qual_cfg_).e_params[_index_].sec_val      = (_sec_val_); \
                (_qual_cfg_).e_params[_index_].bus_offset   = (_offset_);  \
                (_qual_cfg_).e_params[_index_].chunk_offset = (_offset_);  \
                (_qual_cfg_).e_params[_index_].offset       = (_offset_);  \
                (_qual_cfg_).e_params[_index_].width        = (_width_);   \
                (_qual_cfg_).size                           += (_width_);  \
                (_qual_cfg_).num_chunks++;

/*
 * Typedef:
 *   BCMFP_QUAL_CFG_CTRL_SEL_ADD
 * Purpose:
 *   To update the control selectors and its value in the
 *   bcmfp_qual_cfg_t structure.
 */
#define BCMFP_QUAL_CFG_CTRL_SEL_ADD(_ctrl_sel_, _ctrl_sel_val_)               \
                (_qual_cfg_).pri_ctrl_sel     = (_ctrl_sel_);                 \
                (_qual_cfg_).pri_ctrl_sel_val = (_ctrl_sel_val_);             \
                (_qual_cfg_).ctrl_sel         = (_ctrl_sel_);                 \
                (_qual_cfg_).ctrl_sel_val     = (_ctrl_sel_val_);

/*
 * Typedef:
 *  BCMFP_QUAL_CFG_INSERT()
 * Purpose:
 *   To update the control selectors and its value in the
 *   bcmfp_qual_cfg_t structure.
 */
#define BCMFP_QUAL_CFG_INSERT(_unit_, _stage_, _qual_id_, _qual_flags_)  \
                _rv_ = bcmfp_qual_cfg_insert((_unit_),                   \
                                             (_stage_),                  \
                                             (_qual_id_),                \
                                             &(_qual_cfg_));             \
                if (SHR_FAILURE(_rv_)) {                                 \
                    SHR_RETURN_VAL_EXIT(_rv_);                           \
                }


/*
 * Typedef:
 *    BCMFP_QUAL_CFG_ADD
 * Purpose:
 *    Qualifier ibus information add macros.
 */

#define BCMFP_QUAL_CFG_ADD(_unit_,                                     \
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
                           _intraslice_, _secondary_,                  \
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
                _rv_ = bcmfp_qual_cfg_t_init(_unit_, &(_qual_cfg_));         \
                if (SHR_FAILURE(_rv_)) {                                     \
                    SHR_RETURN_VAL_EXIT(_rv_);                               \
                }                                                            \
                sal_memcpy(&((_qual_cfg_).qual_flags), &(_qual_flags_),      \
                           sizeof(bcmfp_qual_flags_bmp_t));                  \
                (_qual_cfg_).ctrl_sel                 = (_ctrl_sel_);        \
                (_qual_cfg_).ctrl_sel_val             = (_ctrl_sel_val_);    \
                (_qual_cfg_).intraslice               = (_intraslice_);      \
                (_qual_cfg_).secondary                = (_secondary_);       \
                (_qual_cfg_).section                  = (_sec0_);            \
                (_qual_cfg_).sec_val                  = (_sec0_val_);        \
                (_qual_cfg_).pri_ctrl_sel             = (_ctrl_sel_);        \
                (_qual_cfg_).pri_ctrl_sel_val         = (_ctrl_sel_val_);    \
                (_qual_cfg_).e_params[0].section      = (_sec0_);            \
                (_qual_cfg_).e_params[0].sec_val      = (_sec0_val_);        \
                (_qual_cfg_).e_params[0].chunk_offset = (_chunk_offset0_);   \
                (_qual_cfg_).e_params[0].width        = (_width0_);          \
                (_qual_cfg_).e_params[0].bus_offset   = (_bus_offset0_);     \
                (_qual_cfg_).e_params[0].offset       = (_bus_offset0_);     \
                (_qual_cfg_).e_params[1].section      = (_sec1_);            \
                (_qual_cfg_).e_params[1].sec_val      = (_sec1_val_);        \
                (_qual_cfg_).e_params[1].chunk_offset = (_chunk_offset1_);   \
                (_qual_cfg_).e_params[1].width        = (_width1_);          \
                (_qual_cfg_).e_params[1].bus_offset   = (_bus_offset1_);     \
                (_qual_cfg_).e_params[1].offset       = (_bus_offset1_);     \
                (_qual_cfg_).e_params[2].section      = (_sec2_);            \
                (_qual_cfg_).e_params[2].sec_val      = (_sec2_val_);        \
                (_qual_cfg_).e_params[2].chunk_offset = (_chunk_offset2_);   \
                (_qual_cfg_).e_params[2].width        = (_width2_);          \
                (_qual_cfg_).e_params[2].bus_offset   = (_bus_offset2_);     \
                (_qual_cfg_).e_params[2].offset       = (_bus_offset2_);     \
                (_qual_cfg_).e_params[3].section      = (_sec3_);            \
                (_qual_cfg_).e_params[3].sec_val      = (_sec3_val_);        \
                (_qual_cfg_).e_params[3].chunk_offset = (_chunk_offset3_);   \
                (_qual_cfg_).e_params[3].width        = (_width3_);          \
                (_qual_cfg_).e_params[3].bus_offset   = (_bus_offset3_);     \
                (_qual_cfg_).e_params[3].offset       = (_bus_offset3_);     \
                (_qual_cfg_).e_params[4].section      = (_sec4_);            \
                (_qual_cfg_).e_params[4].sec_val      = (_sec4_val_);        \
                (_qual_cfg_).e_params[4].chunk_offset = (_chunk_offset4_);   \
                (_qual_cfg_).e_params[4].width        = (_width4_);          \
                (_qual_cfg_).e_params[4].bus_offset   = (_bus_offset4_);     \
                (_qual_cfg_).e_params[4].offset       = (_bus_offset4_);     \
                (_qual_cfg_).e_params[5].section      = (_sec5_);            \
                (_qual_cfg_).e_params[5].sec_val      = (_sec5_val_);        \
                (_qual_cfg_).e_params[5].chunk_offset = (_chunk_offset5_);   \
                (_qual_cfg_).e_params[5].width        = (_width5_);          \
                (_qual_cfg_).e_params[5].bus_offset   = (_bus_offset5_);     \
                (_qual_cfg_).e_params[5].offset       = (_bus_offset5_);     \
                (_qual_cfg_).e_params[6].section      = (_sec6_);            \
                (_qual_cfg_).e_params[6].sec_val      = (_sec6_val_);        \
                (_qual_cfg_).e_params[6].chunk_offset = (_chunk_offset6_);   \
                (_qual_cfg_).e_params[6].width        = (_width6_);          \
                (_qual_cfg_).e_params[6].bus_offset   = (_bus_offset6_);     \
                (_qual_cfg_).e_params[6].offset       = (_bus_offset6_);     \
                (_qual_cfg_).e_params[7].section      = (_sec7_);            \
                (_qual_cfg_).e_params[7].sec_val      = (_sec7_val_);        \
                (_qual_cfg_).e_params[7].chunk_offset = (_chunk_offset7_);   \
                (_qual_cfg_).e_params[7].width        = (_width7_);          \
                (_qual_cfg_).e_params[7].bus_offset   = (_bus_offset7_);     \
                (_qual_cfg_).e_params[7].offset       = (_bus_offset7_);     \
                (_qual_cfg_).size = ((_width0_) + (_width1_) + (_width2_) +  \
                                    (_width3_) + (_width4_) + (_width5_) +   \
                                    (_width6_) + (_width7_));                \
                (_qual_cfg_).num_chunks = (_num_chunks_);                    \
                _rv_ = bcmfp_qual_cfg_insert((_unit_),                       \
                                             (_stage_),                      \
                                             (_qual_id_),                    \
                                             &(_qual_cfg_));                 \
                if (SHR_FAILURE(_rv_)) {                                     \
                    SHR_RETURN_VAL_EXIT(_rv_);                               \
                }                                                            \
            } while(0)

#define BCMFP_QUAL_CFG_ONE_OFFSET_ADD(_unit_,                         \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_)                \
            BCMFP_QUAL_CFG_ADD(_unit_,                                \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               BCMFP_EXT_CTRL_SEL_DISABLE, 0,         \
                               _sec0_, _sec0_val_,                    \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0,                                  \
                               _width0_,                              \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _chunk_offset0_,                       \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _bus_offset0_,                         \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               1)



#define BCMFP_QUAL_CFG_TWO_OFFSET_ADD(_unit_,                         \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_,                \
                                      _sec1_,                         \
                                      _sec1_val_,                     \
                                      _width1_,                       \
                                      _bus_offset1_,                  \
                                      _chunk_offset1_)                \
            BCMFP_QUAL_CFG_ADD(_unit_,                                \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               BCMFP_EXT_CTRL_SEL_DISABLE, 0,         \
                               _sec0_, _sec0_val_,                    \
                               _sec1_, _sec1_val_,                    \
                               0, 0, 0, 0, 0, 0,                      \
                               0, 0, 0, 0, 0, 0,                      \
                               0, 0,                                  \
                               _width0_, _width1_,                    \
                               0, 0, 0, 0, 0, 0,                      \
                               _chunk_offset0_,                       \
                               _chunk_offset1_,                       \
                               0, 0, 0, 0, 0, 0,                      \
                               _bus_offset0_,                         \
                               _bus_offset1_,                         \
                               0, 0, 0, 0, 0, 0,                      \
                               2)

#define BCMFP_QUAL_CFG_THREE_OFFSET_ADD(_unit_,                       \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_,                \
                                      _sec1_,                         \
                                      _sec1_val_,                     \
                                      _width1_,                       \
                                      _bus_offset1_,                  \
                                      _chunk_offset1_,                \
                                      _sec2_,                         \
                                      _sec2_val_,                     \
                                      _width2_,                       \
                                      _bus_offset2_,                  \
                                      _chunk_offset2_)                \
            BCMFP_QUAL_CFG_ADD(_unit_,                                \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               BCMFP_EXT_CTRL_SEL_DISABLE, 0,         \
                               _sec0_, _sec0_val_,                    \
                               _sec1_, _sec1_val_,                    \
                               _sec2_, _sec2_val_,                    \
                               0, 0, 0, 0,                            \
                               0, 0, 0, 0, 0, 0,                      \
                               0, 0,                                  \
                               _width0_, _width1_,                    \
                               _width2_,                              \
                               0, 0, 0, 0, 0,                         \
                               _chunk_offset0_,                       \
                               _chunk_offset1_,                       \
                               _chunk_offset2_,                       \
                               0, 0, 0, 0, 0,                         \
                               _bus_offset0_,                         \
                               _bus_offset1_,                         \
                               _bus_offset2_,                         \
                               0, 0, 0, 0, 0,                         \
                               3)

#define BCMFP_QUAL_CFG_FOUR_OFFSET_ADD(_unit_,                        \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_,                \
                                      _sec1_,                         \
                                      _sec1_val_,                     \
                                      _width1_,                       \
                                      _bus_offset1_,                  \
                                      _chunk_offset1_,                \
                                      _sec2_,                         \
                                      _sec2_val_,                     \
                                      _width2_,                       \
                                      _bus_offset2_,                  \
                                      _chunk_offset2_,                \
                                      _sec3_,                         \
                                      _sec3_val_,                     \
                                      _width3_,                       \
                                      _bus_offset3_,                  \
                                      _chunk_offset3_)                \
            BCMFP_QUAL_CFG_ADD(_unit_,                                \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               BCMFP_EXT_CTRL_SEL_DISABLE, 0,         \
                               _sec0_, _sec0_val_,                    \
                               _sec1_, _sec1_val_,                    \
                               _sec2_, _sec2_val_,                    \
                               _sec3_, _sec3_val_,                    \
                               0, 0,                                  \
                               0, 0, 0, 0, 0, 0,                      \
                               0, 0,                                  \
                               _width0_, _width1_,                    \
                               _width2_, _width3_,                    \
                               0, 0, 0, 0,                            \
                               _chunk_offset0_,                       \
                               _chunk_offset1_,                       \
                               _chunk_offset2_,                       \
                               _chunk_offset3_,                       \
                               0, 0, 0, 0,                            \
                               _bus_offset0_,                         \
                               _bus_offset1_,                         \
                               _bus_offset2_,                         \
                               _bus_offset3_,                         \
                               0, 0, 0, 0,                            \
                               4)

#define BCMFP_QUAL_CFG_EIGHT_OFFSET_ADD(_unit_,                       \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_,                \
                                      _sec1_,                         \
                                      _sec1_val_,                     \
                                      _width1_,                       \
                                      _bus_offset1_,                  \
                                      _chunk_offset1_,                \
                                      _sec2_,                         \
                                      _sec2_val_,                     \
                                      _width2_,                       \
                                      _bus_offset2_,                  \
                                      _chunk_offset2_,                \
                                      _sec3_,                         \
                                      _sec3_val_,                     \
                                      _width3_,                       \
                                      _bus_offset3_,                  \
                                      _chunk_offset3_,                \
                                      _sec4_,                         \
                                      _sec4_val_,                     \
                                      _width4_,                       \
                                      _bus_offset4_,                  \
                                      _chunk_offset4_,                \
                                      _sec5_,                         \
                                      _sec5_val_,                     \
                                      _width5_,                       \
                                      _bus_offset5_,                  \
                                      _chunk_offset5_,                \
                                      _sec6_,                         \
                                      _sec6_val_,                     \
                                      _width6_,                       \
                                      _bus_offset6_,                  \
                                      _chunk_offset6_,                \
                                      _sec7_,                         \
                                      _sec7_val_,                     \
                                      _width7_,                       \
                                      _bus_offset7_,                  \
                                      _chunk_offset7_)                \
            BCMFP_QUAL_CFG_ADD(_unit_,                                \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               BCMFP_EXT_CTRL_SEL_DISABLE, 0,         \
                               _sec0_, _sec0_val_,                    \
                               _sec1_, _sec1_val_,                    \
                               _sec2_, _sec2_val_,                    \
                               _sec3_, _sec3_val_,                    \
                               _sec4_, _sec4_val_,                    \
                               _sec5_, _sec5_val_,                    \
                               _sec6_, _sec6_val_,                    \
                               _sec7_, _sec7_val_,                    \
                               0, 0,                                  \
                               _width0_, _width1_,                    \
                               _width2_, _width3_,                    \
                               _width4_, _width5_,                    \
                               _width6_, _width7_,                    \
                               _chunk_offset0_,                       \
                               _chunk_offset1_,                       \
                               _chunk_offset2_,                       \
                               _chunk_offset3_,                       \
                               _chunk_offset4_,                       \
                               _chunk_offset5_,                       \
                               _chunk_offset6_,                       \
                               _chunk_offset7_,                       \
                               _bus_offset0_,                         \
                               _bus_offset1_,                         \
                               _bus_offset2_,                         \
                               _bus_offset3_,                         \
                               _bus_offset4_,                         \
                               _bus_offset5_,                         \
                               _bus_offset6_,                         \
                               _bus_offset7_,                         \
                               8)
#define BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(_unit_,                \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_,                \
                                      _ctrl_sel_,                     \
                                      _ctrl_sel_val_)                 \
              BCMFP_QUAL_CFG_ADD(_unit_,                              \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               _ctrl_sel_, _ctrl_sel_val_,            \
                               _sec0_, _sec0_val_,                    \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0,                                  \
                               _width0_,                              \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _chunk_offset0_,                       \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _bus_offset0_,                         \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               1)

#define BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(_unit_,               \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_,                \
                                      _ctrl_sel_,                     \
                                      _ctrl_sel_val_)                 \
              BCMFP_QUAL_CFG_ADD(_unit_,                              \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                               _ctrl_sel_, _ctrl_sel_val_,            \
                               _sec0_, _sec0_val_,                    \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               1, 0,                                  \
                               _width0_,                              \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _chunk_offset0_,                       \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               _bus_offset0_,                         \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               1)

#define BCMFP_QUAL_CFG_TWO_OFFSET_CTRL_SEL_ADD(_unit_,                \
                                      _stage_,                        \
                                      _qual_id_,                      \
                                      _qual_flags_,                   \
                                      _sec0_,                         \
                                      _sec0_val_,                     \
                                      _width0_,                       \
                                      _bus_offset0_,                  \
                                      _chunk_offset0_,                \
                                      _sec1_,                         \
                                      _sec1_val_,                     \
                                      _width1_,                       \
                                      _bus_offset1_,                  \
                                      _chunk_offset1_,                \
                                      _ctrl_sel_,                     \
                                      _ctrl_sel_val_)                 \
            BCMFP_QUAL_CFG_ADD(_unit_,                                \
                               _stage_,                               \
                               _qual_id_,                             \
                               _qual_flags_,                          \
                                _ctrl_sel_, _ctrl_sel_val_,           \
                               _sec0_, _sec0_val_,                    \
                               _sec1_, _sec1_val_,                    \
                               0, 0, 0, 0, 0,                         \
                               0, 0, 0, 0, 0, 0, 0,                   \
                               0, 0,                                  \
                               _width0_,_width1_,                     \
                               0, 0, 0, 0, 0, 0,                      \
                               _chunk_offset0_, _chunk_offset1_,      \
                               0, 0, 0, 0, 0, 0,                      \
                               _bus_offset0_, _bus_offset1_,          \
                               0, 0, 0, 0, 0, 0,                      \
                               2)

#endif /* BCMFP_QUAL_CFG_INTERNAL_H */
