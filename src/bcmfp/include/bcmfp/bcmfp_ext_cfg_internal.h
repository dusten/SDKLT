/*! \file bcmfp_ext_cfg_internal.h
 *
 * Defines, Enums and Structures to represent FP extractors
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

#ifndef BCMFP_EXT_CFG_INTERNAL_H
#define BCMFP_EXT_CFG_INTERNAL_H

#include <bcmfp/bcmfp_ext_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>

#define BCMFP_EXT_LEVEL_COUNT 4

#define BCMFP_EXT_NUM_CHUNKS_PER_GRAN 50

#define BCMFP_EXT_NUM_GRAN 5

/*
 * Typedef:
 *     bcmfp_ext_cfg_mode_t
 * Purpose:
 *     Key Generation extractors slice configuration modes.
 */
typedef enum bcmfp_ext_cfg_mode_e {
    BCMFP_EXT_CFG_MODE_SINGLE   = 0,       /* Extractor config for 80b mode.   */
    BCMFP_EXT_CFG_MODE_DBLINTRA = 1,       /* Extractor config for 160b mode.  */
    BCMFP_EXT_CFG_MODE_DBLINTER = 2,       /* Extractor config for 320b mode.  */
    BCMFP_EXT_CFG_MODE_TRIPLE   = 3,       /* Extractor config for 480b mode.  */
    BCMFP_EXT_CFG_MODE_COUNT    = 4        /* Always Last. Not a usable value. */
} bcmfp_ext_cfg_mode_t;

/*
 * Typedef:
 *     bcmfp_ext_sections_t
 * Purpose:
 *     Field sections information structure.
 */
typedef struct bcmfp_ext_section_cfg_s {
    bcmfp_ext_section_t sec;        /* Extractor section information.        */
    int fill_bits;                  /* No. of bits used from this extractor. */
    int drain_bits;                 /* Total bits in this extractor.         */
} bcmfp_ext_section_cfg_t;

typedef struct bcmfp_ext_params_s {
    bcmfp_ext_section_t section;    /* Extractor section.               */
    uint8_t sec_val;                      /* Extractor selector value.        */
    uint16_t bus_offset;                  /* Field offset in input bus.       */
    uint8_t width;                        /* Chunk widths in input bus.       */
    uint8_t chunk_offset;
    uint8_t extracted;                     /* Bits extracted count.            */
    uint8_t bits_used;
    uint8_t size;
} bcmfp_ext_params_t;

typedef struct bcmfp_ext_field_sec_info_s {
    bcmfp_ext_ctrl_sel_t ctrl_sel;
    uint8_t ctrl_sel_val;
    bcmfp_ext_params_t e_params[BCMFP_QUAL_MAX_CHUNKS];
    uint32_t required_bitmap;
    uint8_t extracted;
    uint8_t bits_used;
    uint8_t num_chunks;
    uint8_t size;
    bcmfp_qual_t qid;
    uint16_t chunk_id;
    bcmfp_qset_t qset;
    struct bcmfp_ext_field_sec_info_s *next;
} bcmfp_ext_field_sec_info_t;

typedef struct bcmfp_ibus_field_info_s {
   int num_fields;                      /* Number of qualifiers in input bus. */
   bcmfp_ext_field_sec_info_t **fields; /* Qualifiers to section information. */
} bcmfp_ext_field_info_t;

/*
 * Typedef:
 *     bcmfp_ext_cfg_t
 * Purpose:
 *     Field extractors configuration structure.
 */
typedef struct bcmfp_ext_cfg_s {
    uint32_t ext_id;                          /* Extractor identifier.        */
    int gran;                                 /* Extractor ganularity.        */
    int ext_num;                              /* Extractor number.            */
    bcmfp_ext_section_t in_sec;               /* IN section.                  */
    bcmfp_ext_section_t out_sec;              /* OUT section.                 */
    int bus_offset;                           /* Output bus offset.           */
    uint32_t flags;                           /* Attribtues BCMFP_EXT_ATTR_XXX. */
    uint8_t  in_use;                          /* Extractor inuse.             */
    int chunk_extracted;                      /* Which chunk of attached qual
                                               * is extracted using this
                                               * extractor.
                                               */
    uint8_t chunk_offset;                     /* Chunk offset of chunk_extracted */
    bcmfp_ext_field_sec_info_t *ext_field;    /* bus field extracted using
                                               * this extractor.
                                               */
} bcmfp_ext_cfg_t;

/*
 * Typedef:
 *     bcmfp_ext_cfg_db_t
 * Purpose:
 *     Field extractors configuration and extractor sections information
 *     structure.
 */
typedef struct bcmfp_ext_cfg_db_s {
    bcmfp_ext_cfg_mode_t mode;                       /* Extractor modes.      */
    bcmfp_ext_cfg_t *ext_cfg[BCMFP_EXT_LEVEL_COUNT]; /* Extractor config.     */
    uint16_t conf_size[BCMFP_EXT_LEVEL_COUNT];         /* Configuration size.   */
    bcmfp_ext_section_cfg_t *sec_cfg[BCMFP_EXT_SECTION_COUNT]; /* Sections capacity. */
    uint16_t num_sec;                                  /* Number of sections.   */
} bcmfp_ext_cfg_db_t;

/*
 * Typedef:
 *    BCMFP_EXT_ID_CREATE
 * Purpose:
 *    Extractor ID construction macro.
 */
#define BCMFP_EXT_ID_CREATE(_part_, _level_, _gran_, _ext_num_, _ext_id_)   \
            (_ext_id_) = (((_part_) << 28) | ((_level_) << 18)              \
                            | ((_gran_) << 10) | (_ext_num_))

/*
 * Typedef:
 *    BCMFP_EXT_ID_PARSE
 * Purpose:
 *    Parse an Extractor ID and get the extractor level, granularity and
 *    extractor number information.
 */
#define BCMFP_EXT_ID_PARSE(_ext_id_, _part_, _level_, _gran_, _ext_num_)      \
            (_ext_num_) = ((_ext_id_) & 0x3ff);                               \
            (_gran_) = (((_ext_id_) >> 10) & 0xff);                           \
            (_level_) = (((_ext_id_) >> 18) & 0xf);                           \
            (_part_) = (((_ext_id_) >> 28) & 0x3)

/*
 * Typedef:
 *    BCMFP_EXT_CFG_DECL
 * Purpose:
 *    BCMFP_EXT_CFG_ADD macro needs the following declaration in any function
 *    that uses it.
 */
#define BCMFP_EXT_CFG_DECL            \
    int _rv_;                         \
    bcmfp_ext_cfg_t _fp_ext_cfg_

/*
 * Typedef:
 *    BCMFP_EXT_CFG_ADD
 * Purpose:
 *    Extractor hierarcy construction utility macro.
 */
#define BCMFP_EXT_CFG_ADD(_unit_,                                              \
                          _stage_,                                             \
                          _emode_,                                             \
                          _part_,                                              \
                          _level_,                                             \
                          _gran_,                                              \
                          _ext_num_,                                           \
                          _in_sec_,                                            \
                          _out_sec_,                                           \
                          _offset_,                                            \
                          _flags_)                                             \
            do {                                                               \
                int _ext_id_ = 0;                                              \
                _rv_ = bcmfp_ext_cfg_t_init(unit, &_fp_ext_cfg_);              \
                SHR_IF_ERR_EXIT(_rv_);                                         \
                BCMFP_EXT_ID_CREATE(_part_,                                    \
                                    _level_,                                   \
                                    _gran_,                                    \
                                    _ext_num_,                                 \
                                    _ext_id_);                                 \
                (_fp_ext_cfg_).ext_id = (_ext_id_);                            \
                (_fp_ext_cfg_).gran = (_gran_);                                \
                (_fp_ext_cfg_).ext_num = (_ext_num_);                          \
                (_fp_ext_cfg_).in_sec = (_in_sec_);                            \
                (_fp_ext_cfg_).out_sec = (_out_sec_);                          \
                (_fp_ext_cfg_).bus_offset = (_offset_);                        \
                (_fp_ext_cfg_).flags = (_flags_);                              \
                (_fp_ext_cfg_).in_use = 0;                                     \
                (_fp_ext_cfg_).chunk_extracted = -1;                           \
                (_fp_ext_cfg_).chunk_offset = 0;                               \
                _rv_ = bcmfp_ext_cfg_insert((_unit_),                          \
                                            (_stage_),                         \
                                            (_emode_),                         \
                                            (_level_),                         \
                                            &(_fp_ext_cfg_));                  \
                SHR_IF_ERR_EXIT(_rv_);                                         \
            } while(0)


#define BCMFP_EXT_GRAN_BASE(gran, base)  \
        {                                \
           if (gran == 32) {             \
               base = 0;                 \
           } else if (gran == 16) {      \
               base = 1;                 \
           } else if (gran == 8) {       \
               base = 2;                 \
           } else if (gran == 4) {       \
               base = 3;                 \
           } else if (gran == 2) {       \
               base = 4;                 \
           }                             \
        }

#endif /* BCMFP_EXT_CFG_INTERNAL_H */
