/*! \file bcmfp_action_cfg_internal.h
 *
 * Structures, defines to hold and access FP action configurations.
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

#ifndef BCMFP_ACTION_CFG_INTERNAL_H
#define BCMFP_ACTION_CFG_INTERNAL_H

#include <bcmfp/bcmfp_action_internal.h>

/*! Max number of values to be programmed in h/w(Policy Table)
 *\n to enable an action. Not all actions need to be programmed
 *\n max number of values in hardware.
 */
#define BCMFP_ACTION_PARAM_SZ           (6)

/*! Invalid Action Value. */
#define BCMFP_INVALID_ACTION_VALUE     (-1)

/* Actions not supported in EM */
#define BCMFP_ACTION_NO_EM_SUPPORT      0x1

/* Actions not supported in IFP */
#define BCMFP_ACTION_NO_IFP_SUPPORT      0x10

typedef struct bcmfp_action_offset_cfg_s {
    uint16_t    offset[BCMFP_ACTION_PARAM_SZ]; /* Action offset.    */
    uint8_t     width[BCMFP_ACTION_PARAM_SZ];  /* Action width.     */
    int         value[BCMFP_ACTION_PARAM_SZ];  /* Action Value.     */
} bcmfp_action_offset_cfg_t;

typedef struct bcmfp_action_cfg_s {
    uint32_t    flags;     /* Action flags BCMFP_ACTION_XXX. */
    bcmfp_action_set_t aset; /* Action set */
    bcmfp_action_offset_cfg_t offset_cfg; /* Action offset information. */
    struct bcmfp_action_cfg_s *next; /* Next offset information of same action. */
} bcmfp_action_cfg_t;

typedef struct bcmfp_action_cfg_db_s {
    uint32_t num_actions;
    bcmfp_action_cfg_t *action_cfg[BCMFP_ACTION_COUNT];
} bcmfp_action_cfg_db_t;

/* BCMFP_ACTION_* macros require the following declaration
 * in any function which uses them.
 */
#define BCMFP_ACTION_CFG_DECL \
    int _rv_;         \
    bcmfp_action_cfg_t _action_cfg_

/* Add Action ifno to stage. */
#define BCMFP_ACTION_CFG_ADD(_unit_,                                             \
                             _stage_,                                            \
                             _action_,                                           \
                             _flags_,                                            \
                             _aset_,                                             \
                             _offset1_,                                          \
                             _width1_,                                           \
                             _value1_,                                           \
                             _offset2_,                                          \
                             _width2_,                                           \
                             _value2_,                                           \
                             _offset3_,                                          \
                             _width3_,                                           \
                             _value3_,                                           \
                             _offset4_,                                          \
                             _width4_,                                           \
                             _value4_,                                           \
                             _offset5_,                                          \
                             _width5_,                                           \
                             _value5_)                                           \
            do {                                                                 \
                _rv_ = bcmfp_action_cfg_t_init(unit, &(_action_cfg_));           \
                SHR_IF_ERR_EXIT(_rv_);                                           \
                (_action_cfg_).flags                 |= (_flags_);               \
                (_action_cfg_).aset                   = (_aset_);                \
                (_action_cfg_).offset_cfg.offset[0]   = (_offset1_);             \
                (_action_cfg_).offset_cfg.width[0]    = (_width1_);              \
                (_action_cfg_).offset_cfg.value[0]    = (_value1_);              \
                (_action_cfg_).offset_cfg.offset[1]   = (_offset2_);             \
                (_action_cfg_).offset_cfg.width[1]    = (_width2_);              \
                (_action_cfg_).offset_cfg.value[1]    = (_value2_);              \
                (_action_cfg_).offset_cfg.offset[2]   = (_offset3_);             \
                (_action_cfg_).offset_cfg.width[2]    = (_width3_);              \
                (_action_cfg_).offset_cfg.value[2]    = (_value3_);              \
                (_action_cfg_).offset_cfg.offset[3]   = (_offset4_);             \
                (_action_cfg_).offset_cfg.width[3]    = (_width4_);              \
                (_action_cfg_).offset_cfg.value[3]    = (_value4_);              \
                (_action_cfg_).offset_cfg.offset[4]   = (_offset5_);             \
                (_action_cfg_).offset_cfg.width[4]    = (_width5_);              \
                (_action_cfg_).offset_cfg.value[4]    = (_value5_);              \
                (_action_cfg_).next      = NULL;                                 \
                _rv_ = bcmfp_action_cfg_insert((_unit_),                         \
                                               (_stage_),                        \
                                               (_action_),                       \
                                               &(_action_cfg_));                 \
                SHR_IF_ERR_EXIT(_rv_);                                           \
            } while (0)

/* Add single offset, width, value for action. */
#define BCMFP_ACTION_CFG_ADD_ONE(_unit_, _stage_, _action_, _flags_, _aset_,     \
                                 _offset1_, _width1_, _value1_)                  \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                          (_flags_), (_aset_),                   \
                                          (_offset1_), (_width1_),               \
                                          (_value1_), 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                                          0, 0, 0);

#define BCMFP_ACTION_CFG_ADD_TWO(_unit_, _stage_, _action_, _flags_, _aset_,     \
                                 _offset1_, _width1_, _value1_,                  \
                                 _offset2_, _width2_, _value2_)                  \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                          (_flags_), (_aset_),                   \
                                          (_offset1_), (_width1_),               \
                                          (_value1_), (_offset2_), (_width2_),   \
                                          (_value2_), 0, 0, 0, 0, 0, 0,          \
                                          0, 0, 0);

#define BCMFP_ACTION_CFG_ADD_THREE(_unit_, _stage_, _action_, _flags_, _aset_,   \
                                        _offset1_, _width1_, _value1_,           \
                                        _offset2_, _width2_, _value2_,           \
                                        _offset3_, _width3_, _value3_)           \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                          (_flags_), (_aset_),                   \
                                          (_offset1_), (_width1_),               \
                                          (_value1_), (_offset2_), (_width2_),   \
                                          (_value2_), (_offset3_), (_width3_),   \
                                          (_value3_), 0, 0, 0,                   \
                                          0, 0, 0);

#define BCMFP_ACTION_CFG_ADD_FOUR(_unit_, _stage_, _action_, _flags_, _aset_,    \
                                        _offset1_, _width1_, _value1_,           \
                                        _offset2_, _width2_, _value2_,           \
                                        _offset3_, _width3_, _value3_,           \
                                        _offset4_, _width4_, _value4_)           \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                      (_flags_), (_aset_),                       \
                                      (_offset1_), (_width1_),                   \
                                      (_value1_), (_offset2_), (_width2_),       \
                                      (_value2_), (_offset3_), (_width3_),       \
                                      (_value3_), (_offset4_), (_width4_),       \
                                      (_value4_), 0, 0, 0)

#define BCMFP_ACTION_CFG_ADD_FIVE(_unit_, _stage_, _action_, _flags_, _aset_,    \
                                        _offset1_, _width1_, _value1_,           \
                                        _offset2_, _width2_, _value2_,           \
                                        _offset3_, _width3_, _value3_,           \
                                        _offset4_, _width4_, _value4_,           \
                                        _offset5_, _width5_, _value5_)           \
                     BCMFP_ACTION_CFG_ADD((_unit_), (_stage_),(_action_),        \
                                      (_flags_), (_aset_),                       \
                                      (_offset1_), (_width1_),                   \
                                      (_value1_), (_offset2_), (_width2_),       \
                                      (_value2_), (_offset3_), (_width3_),       \
                                      (_value3_), (_offset4_), (_width4_),       \
                                      (_value4_), (_offset5_), (_width5_),       \
                                      (_value5_))

/* Action conflict check macro. */
#define BCMFP_ACTIONS_CONFLICT(_val_)\
    if (action == _val_) {\
        return (SHR_E_CONFIG);\
    }

#define BCMFP_ACTION_CHECK(width, param)\
    if ((unsigned)((1 << width) - 1) < param) {\
        return SHR_E_PARAM;\
    }

#define BCMFP_ACTION_OFFSET_GET(_unit_, _stage_, _action_, _a_offset_)\
    do {\
        SHR_IF_ERR_EXIT(bcmfp_action_offset_get((_unit_), (_stage_),\
                    (_action_), (_a_offset_)));\
    } while (0)

#define BCMFP_ACTION_WIDTH_GET(width, action, stage)\
    if (NULL != stage) {\
        width = stage->action_cfg_db->\
        action_cfg[action]->offset_cfg.width[0];\
    } else {\
        return SHR_E_FAIL;\
    }

#define BCMFP_ACTION_SET(_unit_, _buf_, _a_offset_)\
    do {\
        SHR_IF_ERR_EXIT(bcmfp_action_value_set((_unit_), (_buf_),\
                    (_a_offset_)));\
    } while (0)


#endif /* BCMFP_ACTION_CFG_INTERNAL_H */

