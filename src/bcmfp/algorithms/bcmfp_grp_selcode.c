/*! \file bcmfp_grp_selcode.c
 *
 * APIs for selcode algorithm.
 *
 * Selcode algorithm is used to allocate selcodes for the given QSET.
 * bcmfp_allocate_extractors is the top function to use. In addition to this
 * function and all the other required utility functions to allocate selcodes
 * are part of this file.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include "bcmfp_grp_selcode.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

static int
bcmfp_group_selectors_init(int unit, bcmfp_group_t *fg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    fg->ext_codes[0].intraslice = BCMFP_EXT_SELCODE_DONT_USE;
    if (fg->flags & BCMFP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE) {
            fg->ext_codes[2].intraslice = BCMFP_EXT_SELCODE_DONT_USE;
        } else {
            fg->ext_codes[1].intraslice = BCMFP_EXT_SELCODE_DONT_USE;
        }
    }

    fg->ext_codes[0].secondary = BCMFP_EXT_SELCODE_DONT_USE;
    if (fg->flags & BCMFP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE) {
            fg->ext_codes[1].secondary = BCMFP_EXT_SELCODE_DONT_USE;
            fg->ext_codes[2].secondary = BCMFP_EXT_SELCODE_DONT_USE;
        }
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qualifiers_free(int unit, bcmfp_group_t *fg, int idx)
{
    bcmfp_group_qual_offset_info_t   *q_arr;       /* Qualifiers array.        */
    int                              entry_type;   /* holds type of an entry   */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    if ((idx < 0) || (idx >= BCMFP_ENTRY_PARTS_MAX)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    for (entry_type = 0; entry_type < BCMFP_ENTRY_TYPE_COUNT; entry_type++) {
        q_arr = &(fg->qual_arr[entry_type][idx]);

        /* Free qualifiers arrays. */
        SHR_FREE(q_arr->qid_arr);
        SHR_FREE(q_arr->offset_arr);

        /* Reset arrays size. */
        q_arr->size = 0;
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_selcode_qual_arr_free(int unit,
                            bcmfp_qual_cfg_info_t ***f_qual_arr,
                            uint16_t qual_count)
{
    int   qual_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(f_qual_arr, SHR_E_PARAM);

    if (*f_qual_arr) {
        for (qual_idx = 0; qual_idx < qual_count ; qual_idx++) {
            if ((*f_qual_arr)[qual_idx]) {
                SHR_FREE((*f_qual_arr)[qual_idx]->qual_cfg_arr);
                SHR_FREE((*f_qual_arr)[qual_idx]);
            }
        }
        SHR_FREE(*f_qual_arr);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_qual_info_cmp(const void *a, const void *b)
{
    bcmfp_qual_cfg_info_t    **first;     /* First compared qualifier.  */
    bcmfp_qual_cfg_info_t    **second;    /* Second compared qualifier. */

    first = (bcmfp_qual_cfg_info_t **)a;
    second = (bcmfp_qual_cfg_info_t **)b;

    if ((*first)->num_qual_cfg < (*second)->num_qual_cfg) {
        return (-1);
    } else if ((*first)->num_qual_cfg > (*second)->num_qual_cfg) {
        return (1);
    }
    return (0);
}


static int
bcmfp_selector_update_cmp(const void *a, const void *b)
{
    bcmfp_qual_cfg_t    *first;     /* First compared selector.  */
    bcmfp_qual_cfg_t    *second;    /* Second compared selector. */

    first = (bcmfp_qual_cfg_t *)a;
    second = (bcmfp_qual_cfg_t *)b;

    if (first->update_count <
            second->update_count) {
        return (-1);
    } else if (first->update_count >
            second->update_count) {
        return (1);
    }
    return (0);
}

static int
bcmfp_qualifiers_info_get(int unit, bcmfp_stage_t *stage,
                          bcmfp_qset_t *qset_req,
                          bcmfp_qual_cfg_info_t ***f_qual_arr,
                          uint16_t qual_arr_size)
{
    uint16_t    idx;                     /* Qualifier set iterator.   */
    uint16_t    arr_idx    = 0;          /* Qualifiers array index.   */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(qset_req, SHR_E_PARAM);
    SHR_NULL_CHECK(f_qual_arr, SHR_E_PARAM);

    /* Allocate & initialize group select codes array. */
    BCMFP_ALLOC((*f_qual_arr),
                (qual_arr_size * sizeof(bcmfp_qual_cfg_info_t *)),
                "Field qualifiers");

    /* Copy individual qualifiers information from the requested qset */
    SHR_BIT_ITER(qset_req->w, BCMFP_QUAL_COUNT, idx) {
        /* Qualifiers support check. */
        SHR_NULL_CHECK(stage->qual_cfg_info_db->qual_cfg_info[idx],
                       SHR_E_RESOURCE);

        /* Make a copy of the qualifier combination for sorting
         * instead of using pointer to the data in stage
         * this will avoid sorting of qualifiers in stage
         * and make group create independent of previously
         * created groups.
         */
        BCMFP_ALLOC(((*f_qual_arr)[arr_idx]),
                    (sizeof(bcmfp_qual_cfg_info_t)),
                    "Field qualifiers");

        (*f_qual_arr)[arr_idx]->qual =
                   stage->qual_cfg_info_db->qual_cfg_info[idx]->qual;
        (*f_qual_arr)[arr_idx]->num_qual_cfg =
                   stage->qual_cfg_info_db->qual_cfg_info[idx]->num_qual_cfg;

        /* Allocate memory for qualifer config array */
        BCMFP_ALLOC(((*f_qual_arr)[arr_idx]->qual_cfg_arr),
                    (stage->qual_cfg_info_db->qual_cfg_info[idx]->num_qual_cfg *
                     sizeof(bcmfp_qual_cfg_t)),
                    "Field qualifiers");

        /* Copy qual info from stage fc to new memory allocated */
        sal_memcpy((*f_qual_arr)[arr_idx]->qual_cfg_arr,
                   stage->qual_cfg_info_db->qual_cfg_info[idx]->qual_cfg_arr,
                   (stage->qual_cfg_info_db->qual_cfg_info[idx]->num_qual_cfg *
                    sizeof(bcmfp_qual_cfg_t)));

        arr_idx++;
        if (arr_idx == (qual_arr_size)) {
            break;
        }
    }

    /* Qualifier array elements are sorted so less frequently present
       qualifiers appear first. */
    sal_qsort((*f_qual_arr), qual_arr_size, sizeof(bcmfp_qual_cfg_info_t *),
              bcmfp_qual_info_cmp);

    SHR_FUNC_EXIT();

exit:

    if (*f_qual_arr) {
        SHR_FREE(((*f_qual_arr)[arr_idx]));
    }

    (void)bcmfp_selcode_qual_arr_free(unit, f_qual_arr, arr_idx);

    SHR_FUNC_EXIT();
}

static int
bcmfp_selector_diff(int unit,
                    bcmfp_ext_sel_t *sel_arr,
                    int part_idx,
                    bcmfp_qual_cfg_t *selector,
                    uint8_t *diff_count)
{
    uint8_t            count;            /* Required updates count. */
    bcmfp_ext_sel_t    *tcam_part;       /* Tcam part selectors.    */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(sel_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(selector, SHR_E_PARAM);
    SHR_NULL_CHECK(diff_count, SHR_E_PARAM);

    count = 0;
    tcam_part = sel_arr + part_idx;

    /* Qualifier is available in the second part of intraslice entry */
    if (selector->intraslice) {
        if (BCMFP_EXT_SELCODE_DONT_USE == tcam_part->intraslice) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
    } else {
        if (BCMFP_EXT_SELCODE_DONT_USE != tcam_part->intraslice) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
    }

    /* Qualifier is available only in the secondary part of a paired
       entry */

    if (selector->secondary) {
        if (BCMFP_EXT_SELCODE_DONT_USE == tcam_part->secondary) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
    }

    /* Primary slice selector. */
    switch (selector->e_params[0].section) {
        case BCMFP_EXT_SECTION_DISABLE:
            break;
        case BCMFP_EXT_SECTION_FPF1:
            if (tcam_part->fpf1 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf1 != selector->e_params[0].sec_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_SECTION_FPF2:
            if (tcam_part->fpf2 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf2 != selector->e_params[0].sec_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_SECTION_FPF3:
            if (tcam_part->fpf3 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf3 != selector->e_params[0].sec_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_SECTION_FPF4:
            if (tcam_part->fpf4 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf4 != selector->e_params[0].sec_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Secondary slice selector. */
    /* Per slice selectors are inserted into primary slice only. */
    if ((selector->intraslice) && (0 != part_idx)) {
        tcam_part = sel_arr + (part_idx - 1);
    }

    switch (selector->pri_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        case BCMFP_EXT_CTRL_SEL_SRC:
            if (tcam_part->src_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->src_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_IP_HEADER:
            if (tcam_part->ip_header_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->ip_header_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID:
            if (tcam_part->egr_key1_classid_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key1_classid_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID:
            if (tcam_part->egr_key2_classid_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key2_classid_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASSID:
            if (tcam_part->egr_key3_classid_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key3_classid_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASSID:
            if (tcam_part->egr_key4_classid_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key4_classid_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID:
            if (tcam_part->egr_key6_classid_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key6_classid_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID:
            if (tcam_part->egr_key7_classid_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key7_classid_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID:
            if (tcam_part->egr_key8_classid_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key8_classid_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP:
            if (tcam_part->egr_key4_dvp_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key4_dvp_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP:
            if (tcam_part->egr_key8_dvp_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key8_dvp_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_MDL:
            if (tcam_part->egr_key4_mdl_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key4_mdl_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR:
            if (tcam_part->egr_ip_addr_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                ++count;
            } else if (tcam_part->egr_ip_addr_sel != selector->pri_ctrl_sel_val) {
                SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
            }
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Tertiary slice selector */
    switch (selector->sec_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    *diff_count = count;

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_qual_selectors_sort(int unit,
                          bcmfp_ext_sel_t *candidate,
                          uint8_t parts_count,
                          bcmfp_qual_cfg_info_t *f_qual)
{
    uint8_t            idx;                      /* Tcam parts iterator.               */
    uint8_t            conf_idx;                 /* Configurations iterator.           */
    uint8_t            update_count_min;         /* Minimum selectors update count.    */
    uint8_t            update_count;             /* Number of updates required
                                                    to accommodate qualifier.          */
    int                rv;                       /* Operation return status.           */
    bcmfp_qual_cfg_t   *selector;                /* Qualifier select codes config.     */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(candidate, SHR_E_PARAM);
    SHR_NULL_CHECK(f_qual, SHR_E_PARAM);

    /* Don't sort if only one configuration is available. */
    if (f_qual->num_qual_cfg <= 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Get number of updates required for each qualifier configuration. */
    for (conf_idx = 0; conf_idx < f_qual->num_qual_cfg; conf_idx++) {
        update_count_min = -1;
        selector = &(f_qual->qual_cfg_arr[conf_idx]);

        for (idx = 0; idx < parts_count; idx++) {
            update_count = 0;

            /* Test if selector can be inserted in a specific tcam part. */
            rv = bcmfp_selector_diff(unit, candidate, idx,
                                      selector, &update_count);
            if (SHR_FAILURE(rv)) {
                continue;
            }

            if (update_count < update_count_min) {
                update_count_min = update_count;
            }
        }
        /* Keep minimum number of required updates. */
        selector->update_count  =  update_count_min;
    }

    /* Qualifier configurations sort based on number of updates required.  */
    sal_qsort(f_qual->qual_cfg_arr, f_qual->num_qual_cfg,
              sizeof(bcmfp_qual_cfg_t), bcmfp_selector_update_cmp);

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_selector_insert(int unit,
                      bcmfp_ext_sel_t *sel_arr,
                      int part_idx,
                      bcmfp_qual_cfg_t *selector)
{
    bcmfp_ext_sel_t    *tcam_part;   /* Tcam part selectors. */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(sel_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(selector, SHR_E_PARAM);

    tcam_part = sel_arr + part_idx;

    /* Primary slice selector. */
    switch (selector->e_params[0].section) {
        case BCMFP_EXT_SECTION_DISABLE:
            break;
        case BCMFP_EXT_SECTION_FPF1:
            tcam_part->fpf1 = selector->e_params[0].sec_val;
            break;
        case BCMFP_EXT_SECTION_FPF2:
            tcam_part->fpf2 = selector->e_params[0].sec_val;
            break;
        case BCMFP_EXT_SECTION_FPF3:
            tcam_part->fpf3 = selector->e_params[0].sec_val;
            break;
        case BCMFP_EXT_SECTION_FPF4:
            tcam_part->fpf4 = selector->e_params[0].sec_val;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Per slice selectors are inserted into primary slice only. */
    if ((selector->intraslice) && (0 != part_idx)) {
        tcam_part = sel_arr + (part_idx - 1);
    }

    /* Secondary slice selector. */
    switch (selector->pri_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        case BCMFP_EXT_CTRL_SEL_SRC:
            tcam_part->src_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_IP_HEADER:
            tcam_part->ip_header_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID:
            tcam_part->egr_key1_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID:
            tcam_part->egr_key2_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASSID:
            tcam_part->egr_key3_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASSID:
            tcam_part->egr_key4_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID:
            tcam_part->egr_key6_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID:
            tcam_part->egr_key7_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID:
            tcam_part->egr_key8_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP:
            tcam_part->egr_key4_dvp_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP:
            tcam_part->egr_key8_dvp_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_MDL:
            tcam_part->egr_key4_mdl_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR:
            tcam_part->egr_ip_addr_sel = selector->pri_ctrl_sel_val;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Tertiary slice selector */
    switch (selector->sec_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_qual_offsets_copy(int unit,
                        bcmfp_group_t *fg,
                        uint16_t qual_count,
                        bcmfp_qual_cfg_info_t **f_qual_arr,
                        uint8_t *qual_conf_idx)
{
    bcmfp_qual_cfg_t                 *qual_config  = NULL;         /* Qualifier configuration.       */
    bcmfp_group_qual_offset_info_t   *q_arr        = NULL;         /* Qualifiers array.              */
    bcmfp_qset_t                     qset_assigned;                /* Assigned to a specific         */
    int                              qual_idx;                     /* Qualifiers iteration index.    */
    int                              chunk_idx;                    /* Chunk iteration index.         */
    int                              part_idx;                     /* Tcam parts iteration index.    */
    uint8_t                          parts_count   = 0;            /* Number of entry parts.         */
    uint8_t                          conf_idx;                     /* Qualifiers configuration index.*/
    uint8_t                          diff;                         /* Selector diff .                */
    bcmfp_qual_t                     *qual_arr      = NULL;         /* New qualifiers array.          */
    bcmfp_qual_offset_info_t         *offset_arr   = NULL;         /* New qualifier offsets array.   */
    uint16_t                         size          = 0;            /* Qualifiers array size.         */
    int                              rv;                           /* Operation return status.       */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(f_qual_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_conf_idx, SHR_E_PARAM);

    BCMFP_QSET_INIT(qset_assigned);

    /* Get number of entry parts based on the field group flags. */
    SHR_IF_ERR_EXIT(bcmfp_group_parts_count(unit,
                    fg->flags, &parts_count));

    /* Get currently assigned qualifiers. */
    for (part_idx = 0; part_idx < parts_count; part_idx++) {
        q_arr = &(fg->qual_arr[BCMFP_ENTRY_TYPE_ALL][part_idx]);
        for (qual_idx = 0; qual_idx < q_arr->size; qual_idx++) {
            BCMFP_QSET_ADD(qset_assigned, q_arr->qid_arr[qual_idx]);
        }
    }

    for (part_idx = 0; part_idx < parts_count; part_idx++) {
        /* Initialization. */
        offset_arr = NULL;
        qual_arr = NULL;

        /* Get qualifiers array pointer. */
        q_arr = &(fg->qual_arr[BCMFP_ENTRY_TYPE_ALL][part_idx]);
        size = q_arr->size;

        for (qual_idx = 0; qual_idx < qual_count; qual_idx++) {
            /* Check if qualifier was already assigned. */
            if (BCMFP_QSET_TEST(qset_assigned, f_qual_arr[qual_idx]->qual)) {
                continue;
            }

            /* Get qualifier configuration index. */
            conf_idx = qual_conf_idx[qual_idx];
            /* Get qualifier configuration for the index. */
            qual_config = f_qual_arr[qual_idx]->qual_cfg_arr + conf_idx;
            /* Test if selector was inserted in a specific tcam part. */
            rv = bcmfp_selector_diff(unit, fg->ext_codes, part_idx,
                                     qual_config,
                                     &diff);
            if (SHR_FAILURE(rv) || diff) {
                continue;
            }
            size++;
        }

        /* Allocate qualifiers id and offsets array. */
        BCMFP_ALLOC(qual_arr, (size * sizeof (bcmfp_qual_t)), "Group qual id");

        BCMFP_ALLOC(offset_arr, (size *
                    sizeof (bcmfp_qual_offset_info_t)), "Group qual offset");

        /* Copy whatever qualifiers the group already have into the new array */
        if (q_arr->size) {
            sal_memcpy(qual_arr, q_arr->qid_arr,
                    q_arr->size * sizeof (bcmfp_qual_t));
            sal_memcpy(offset_arr, q_arr->offset_arr,
                    q_arr->size * sizeof(bcmfp_qual_offset_info_t));
        }
        size = q_arr->size;

        /* Free old group qualifiers. */
        SHR_IF_ERR_EXIT(bcmfp_group_qualifiers_free(unit, fg, part_idx));

        /* Now let the group point to the new and bigger array */
        q_arr->qid_arr = qual_arr;
        q_arr->offset_arr = offset_arr;
        q_arr->size = size;

        for (qual_idx = 0; qual_idx < qual_count; qual_idx++) {
            /* Check if qualifier was already assigned. */
            if (BCMFP_QSET_TEST(qset_assigned, f_qual_arr[qual_idx]->qual)) {
                continue;
            }

            /* Get qualifier configuration index. */
            conf_idx = qual_conf_idx[qual_idx];
            /* Get qualifier configuration for the index. */
            qual_config = f_qual_arr[qual_idx]->qual_cfg_arr + conf_idx;
            /* Test if selector was inserted in a specific tcam part. */
            rv = bcmfp_selector_diff(unit,
                                     fg->ext_codes,
                                     part_idx,
                                     qual_config,
                                     &diff);

            if (SHR_FAILURE(rv) || diff) {
                continue;
            }
            q_arr->qid_arr[q_arr->size] = f_qual_arr[qual_idx]->qual;
            q_arr->offset_arr[q_arr->size].num_offsets = qual_config->num_chunks;
            for (chunk_idx = 0; chunk_idx < qual_config->num_chunks; chunk_idx++) {
                q_arr->offset_arr[q_arr->size].offset[chunk_idx] =
                    qual_config->e_params[chunk_idx].offset;
                q_arr->offset_arr[q_arr->size].width[chunk_idx] =
                    qual_config->e_params[chunk_idx].width;
            }
            q_arr->size++;
            BCMFP_QSET_ADD(qset_assigned, f_qual_arr[qual_idx]->qual);
        }
    }

    SHR_FUNC_EXIT();

exit:

    SHR_FREE(qual_arr);
    SHR_FREE(offset_arr);

    SHR_FUNC_EXIT();
}


static int
bcmfp_qual_selectors_insert(int unit,
                            bcmfp_ext_sel_t *candidate,
                            uint8_t parts_count,
                            bcmfp_qual_cfg_t *selector)
{
    uint8_t    idx;                      /* Tcam parts iterator.            */
    uint8_t    selected_part;            /* Tcam part selected for
                                            qualifier installation.         */
    uint8_t    update_count_min;         /* Minimum selectors update count. */
    uint8_t    update_count;             /* Number of updates required
                                            to accommodate qualifier.       */
    int        rv;                       /* Operation return status.        */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(candidate, SHR_E_PARAM);
    SHR_NULL_CHECK(selector, SHR_E_PARAM);

    update_count_min = -1;
    selected_part = -1;

    for (idx = 0; idx < parts_count; idx++) {
        update_count = 0;

        /* Test if selector can be inserted in a specific tcam part. */
        rv = bcmfp_selector_diff(unit, candidate, idx, selector, &update_count);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        if (update_count < update_count_min) {
            update_count_min = update_count;
            selected_part = idx;
        }
    }

    /* Check if any tcam part can accommodate all the required selectors. */
    if ((uint8_t)-1 == selected_part) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

    /* Insert selector configuration. */
    SHR_IF_ERR_EXIT(bcmfp_selector_insert(unit, candidate,
                                          selected_part, selector));

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_group_selcode_allocate(int unit,
                             bcmfp_stage_t *stage,
                             bcmfp_group_t *fg,
                             int qual_count)
{
    bcmfp_qual_cfg_info_t     **f_qual_arr;                    /* Qualifiers information.         */
    bcmfp_qual_cfg_t          *qual_config;                    /* Qualifier configuration.        */
    uint8_t                   *qual_conf_idx;                  /* Array of iterators over each
                                                                  individual qual configurations. */
    bcmfp_ext_sel_t           *sel_arr;                        /* Selectors status before each
                                                                  individual qualifier
                                                                  configuration was added.        */
    bcmfp_ext_sel_t           *candidate;                      /* Candidate select codes set.     */
    uint8_t                   conf_idx       = 0;              /* Qualifiers configuration index. */
    uint8_t                   parts_count    = 0;              /* Number of tcam parts.           */
    int                       qual_idx       = 0;              /* Qualifiers iteration index.     */
    int                       sel_entry_sz   = 0;              /* Byte size of sel_arr entity.    */
    int                       rv             = SHR_E_NONE;     /* Operation return status.        */

    SHR_FUNC_ENTER(unit);

    /* Initialization. */
    f_qual_arr    = NULL;
    qual_config   = NULL;
    qual_conf_idx = NULL;
    sel_arr       = NULL;
    candidate     = NULL;

    /* Input parameters check. */
    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Update group selectors based on group flags. */
    SHR_IF_ERR_EXIT(bcmfp_group_selectors_init(unit, fg));

    /* Get qualifiers configuration.          */
    /* NOTE: Must free f_qual_arr at the end. */
    SHR_IF_ERR_EXIT(bcmfp_qualifiers_info_get(unit,
                                               stage,
                                               &fg->qset,
                                               &f_qual_arr,
                                               qual_count));

    /* Allocate and initialize qualifier configurations iterators. */
    BCMFP_ALLOC(qual_conf_idx, (qual_count * sizeof(uint8_t)),
                "Field qualifier configuration");

    /* Get number of entry parts. */
    SHR_IF_ERR_EXIT(bcmfp_group_parts_count(unit,
                                             fg->flags,
                                             &parts_count));

    /* Allocate and initialize select codes status before           */
    /* individual qualifier configuration was applied.              */
    /* Candidate select codes are in the last element of the array. */
    sel_entry_sz = parts_count * sizeof(bcmfp_ext_sel_t);

    BCMFP_ALLOC(sel_arr, ((1 + qual_count) * sel_entry_sz), \
                "Field group select codes");

    /* Initialize first qualifier select codes to group select codes. */
    candidate = sel_arr + (qual_count * parts_count);
    sal_memcpy(candidate, fg->ext_codes, sel_entry_sz);

    /* Initialize first qualifier select codes to a group defaults */
    sal_memcpy(sel_arr, candidate, sel_entry_sz);

    /* Main select codes selection loop over caller requested qualifier set. */
    for (qual_idx = 0; qual_idx < qual_count;) {

        /* Get qualifier selcodes configuration index. */
        conf_idx = qual_conf_idx[qual_idx];

        /*
         * Sort qualifier configurations so minimum
         * change configuration attempted first.
         */
        if (0 == conf_idx) {
            SHR_IF_ERR_EXIT(bcmfp_qual_selectors_sort(unit,
                                                      candidate,
                                                      parts_count,
                                                      f_qual_arr[qual_idx]));
        }

        /*
         * Set default return value, in case all configuration
         * options were exhausted.
         */
        rv = SHR_E_RESOURCE;

        /* Loop over possible qualifier configurations. */
        for (;conf_idx < f_qual_arr[qual_idx]->num_qual_cfg; conf_idx++)  {

            /* Get qualifier configuration for the index. */
            qual_config = f_qual_arr[qual_idx]->qual_cfg_arr + conf_idx;

            /* Skip conflicting configurations. */
            if ((uint8_t) -1 ==
                    qual_config->update_count) {
                continue;
            }

            /*
             *  Attempt to accommodate specific qualifier
             *  configuration into candidate select codes.
             */
            rv = bcmfp_qual_selectors_insert(unit,
                                             candidate,
                                             parts_count,
                                             qual_config);

            if (SHR_SUCCESS(rv) || (SHR_E_RESOURCE != rv)) {
                break;
            }
        }

        /*
         * If code failed to insert qualifier configuration
         * attempt a previous qualifier in the list using alternative
         * configuration.
         */
        if (SHR_FAILURE(rv)) {
            if ((qual_idx == 0) || (SHR_E_RESOURCE != rv)) {
                break;
            }
            /* Previous qualifier has to use next available configuration. */
            while (qual_idx > 0) {
                /* This qualifier configuration will be re-selected. */
                qual_conf_idx[qual_idx] = 0;
                /* Back to the previous qualifier. */
                qual_idx--;
                /* Check select codes were modified by qualifier. */
                qual_config = f_qual_arr[qual_idx]->qual_cfg_arr +
                              qual_conf_idx[qual_idx];
                /* If modified -> reverse changes and proceed to next config.*/
                if ((0 == qual_idx) ||
                        (0 != qual_config->update_count)) {
                    /* Go to the next configuration */
                    qual_conf_idx[qual_idx]++;
                    /* Restore original select codes state. */
                    sal_memcpy(candidate,
                            sel_arr + (qual_idx * parts_count),
                            sel_entry_sz);
                    break;
                }
            }
        } else {
            /* Preserve current qualifier configuration index. */
            qual_conf_idx[qual_idx] = conf_idx;
            /* Proceed to the next qualifier. */
            qual_idx++;
            /* Set selection base line for the next qualifier */
            if (qual_idx < qual_count) {
                sal_memcpy(sel_arr + (qual_idx * parts_count),
                        candidate, sel_entry_sz);
            }
        }
    }
    if (SHR_SUCCESS(rv)) {
        /* Copy selected codes to the group configuration. */
        sal_memcpy(fg->ext_codes, candidate, sel_entry_sz);
        /* Copy qualifiers offset to group tcam parts. */
        rv = bcmfp_qual_offsets_copy(unit,
                                     fg,
                                     qual_count,
                                     f_qual_arr,
                                     qual_conf_idx);
    }
    SHR_RETURN_VAL_EXIT(rv);

exit:

    SHR_FREE(qual_conf_idx);
    SHR_FREE(sel_arr);
    (void)bcmfp_selcode_qual_arr_free(unit, &f_qual_arr, qual_count);

    SHR_FUNC_EXIT();
}

int
bcmfp_group_selcode_assign(int unit,
                           bcmfp_group_t *fg,
                           bcmfp_stage_t *stage,
                           int qual_count,
                           uint8_t reset_flag)
{
    int                 idx;                  /* Generic iteration index.       */
    int                 rv  = SHR_E_NONE;     /* Operation return status.       */

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Get stage FP control info. */
    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, fg->stage_id, &stage));

    /*
     * Clear the group's slice Qsets.
     *   Selcodes need not be cleared if this function is called from group_set
     */
    for (idx = 0; (idx < BCMFP_ENTRY_PARTS_MAX) && reset_flag ; idx++) {
        bcmfp_ext_codes_init(unit, &(fg->ext_codes[idx]));
        SHR_IF_ERR_EXIT(bcmfp_group_qualifiers_free(unit, fg, idx));
    }

    /* Allocate select codes based on group flags. */
    rv = bcmfp_group_selcode_allocate(unit, stage, fg, qual_count);

    SHR_RETURN_VAL_EXIT(rv);
exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_group_slice_mode_to_group_mode(int unit,
                                     bcmfp_group_slice_mode_t grp_slice_mode,
                                     bcmfp_group_mode_t *grp_mode)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(grp_mode, SHR_E_PARAM);

    switch (grp_slice_mode) {
        case BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_SINGLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_SINGLE_WIDE:
            *grp_mode = BCMFP_GROUP_MODE_SINGLE;
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_DOUBLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_DOUBLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_ALT_DOUBLE_WIDE:
            *grp_mode = BCMFP_GROUP_MODE_DBLINTER;
            break;

        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

exit:

    SHR_FUNC_EXIT();
}
