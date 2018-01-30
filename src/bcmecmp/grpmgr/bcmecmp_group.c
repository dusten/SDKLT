/*! \file bcmecmp_group.c
 *
 * This file contains ECMP group management functions.
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

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <bcmecmp/bcmecmp_lth_common.h>
#include <bcmecmp/bcmecmp_lt_utils.h>
#include <bcmecmp/bcmecmp_group.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Initalize bcmecmp_tbl_op_t structure.
 *
 * \param [in] op_info Pointer to bcmecmp_tbl_op_t_init structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_tbl_op_t_init(bcmecmp_tbl_op_t *op_info)
{
    if (op_info) {
        op_info->oper_flags = BCMECMP_OPER_NONE;
        op_info->num_ent = 0;
        op_info->free_idx = BCMECMP_INVALID;
        op_info->tbl_ptr = NULL;
    }
    return;
}

/*!
 * \brief  Returns free indices range from requested hardware table.
 *
 * This function checks for a range of free hardware table indices and
 * returns the start index of the free index range in op_data free_idx
 * structure member variable.
 *
 * \param [in] unit Unit number.
 * \param [in] op_data Pointer to bcmecmp_tbl_op_t structure.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 */
static int
bcmecmp_tbl_free_idx_get(int unit, bcmecmp_tbl_op_t *op_data)
{
    int imin, imax, start_idx, idx;
    uint32_t num_ent;
    uint32_t oper_flags;
    bcmecmp_tbl_prop_t *tbl_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_data, SHR_E_PARAM);
    SHR_NULL_CHECK(op_data->tbl_ptr, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcmecmp_tbl_free_idx_get.\n")));

    tbl_ptr = op_data->tbl_ptr;
    imin = tbl_ptr->imin;
    imax = tbl_ptr->imax;
    num_ent = op_data->num_ent;
    oper_flags = op_data->oper_flags;
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "drd_sid=%d ltd_sid=%d imin=%d imax=%d num_ent=%d"
                          " oper_flags=0x%x.\n"),
              tbl_ptr->drd_sid,
              tbl_ptr->ltd_sid,
              imin,
              imax,
              num_ent,
              oper_flags));

    for (start_idx = imin; start_idx <= imax; start_idx++) {
        /* skip index zero if required by hardware. */
        if (!start_idx && (oper_flags & BCMECMP_OPER_SKIP_ZERO)) {
            continue;
        }

        if (!BCMECMP_TBL_REF_CNT(tbl_ptr, start_idx)) {
            for (idx = start_idx;
                 idx < (start_idx + (int)num_ent) && (idx <= imax);
                 idx++) {
                if (BCMECMP_TBL_REF_CNT(tbl_ptr, idx)) {
                    break;
                }
            }
            if (idx == (start_idx + (int)num_ent)) {
                op_data->free_idx = start_idx;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "free_idx=%d\n"), op_data->free_idx));
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP group next-hop members print debug function.
 *
 * ECMP group next-hop members print debug function.
 *
 * \param [in] unit Unit number.
 * \param [in] pre_sort Set to TRUE to print unsorted next-hop array elements.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_nhops_debug_print(int unit,
                                bool pre_sort,
                                bcmecmp_lt_entry_t *lt_entry)
{
    shr_pb_t *pb = NULL; /* Print buffer. */
    uint32_t idx;        /* Local iterator variable. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Debug print of Pre-Sorted Underlay ECMP Group IDs/NHOP_IDs list. */
    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t%s", pre_sort ? "Pre-Sorted:" : "Sorted:");
    shr_pb_printf(pb,
        "[npaths=%-4d sort=%d]:\n",
        lt_entry->num_paths,
        lt_entry->nhop_sorted);
    shr_pb_printf
        (pb, "\t=============================");
    for (idx = 0; idx < lt_entry->num_paths; idx++) {
        if (!(idx % 5)) {
            shr_pb_printf(pb, "\n\t");
        }
        shr_pb_printf(pb,
                      " %4d) %-5d",
                      idx,
                      (lt_entry->ecmp_nhop
                        ? lt_entry->uecmp_id[idx] : lt_entry->nhop_id[idx]));
    }
    shr_pb_printf(pb, "\n");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Sorts group next-hop array elements.
 *
 * Sorts list of next-hop members belonging to the group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_nhops_sort(int unit, bcmecmp_lt_entry_t *lt_entry)
{
    size_t nmemb; /* Number of members in group. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Debug print of Pre-Sorted Underlay ECMP Group IDs/NHOP_IDs list. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_group_nhops_debug_print(unit, TRUE, lt_entry)));

    /* Get number of elements in the array that's to be sorted. */
    nmemb = (size_t)lt_entry->num_paths;

    /* Check NHOP_ECMP status. */
    if (lt_entry->ecmp_nhop) {
        /* Sort supplied ECMP_UNDERLAY_IDs. */
        sal_qsort((void *)lt_entry->uecmp_id, nmemb,
                  sizeof(lt_entry->uecmp_id[0]), bcmecmp_cmp_int);
    } else {
        /* Sort supplied NHOP_IDs. */
        sal_qsort((void *)lt_entry->nhop_id, nmemb,
                  sizeof(lt_entry->nhop_id[0]), bcmecmp_cmp_int);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_group_nhops_debug_print(unit, FALSE, lt_entry)));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Returns a random member index value within an index range.
 *
 * Generates and returns a random member index value given an index range.
 *
 * \param [in] unit Unit number.
 * \param [in] rand_max Max index value limit for random index generation.
 * \param [out] rand_index Pointer to generated randon index value.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_rand_memb_index_get(int unit,
                               uint32_t rand_max,
                               uint32_t *rand_index)
{
    uint32_t modulus;
    uint32_t rand_seed_shift = 16;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rand_index, SHR_E_PARAM);

    modulus = rand_max + 1;
    if (modulus > (uint32_t)(1 << (32 - rand_seed_shift))) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    BCMECMP_RH_RAND_SEED(unit) = BCMECMP_RH_RAND_SEED(unit) * 1103515245
                                        + 12345;

    *rand_index = (BCMECMP_RH_RAND_SEED(unit) >> rand_seed_shift) % modulus;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Returnes RH member array index where the next-hop value must be set.
 *
 * Calculates and returns the RH member array index where the next-hop member
 * value must be set. Maximum entries and additional overflow entries that a RH
 * group's member can occupy in ECMP member table are used in this calculation
 * for RH member array index selection.
 *
 * \param [in] unit Unit number.
 * \param [in] max_ent_cnt Maximum entries a RH group member can occupy.
 * \param [in] overflow_cnt Surplus entries that RH group members can occupy.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 * \param [out] selected_index Pointer to selected to next-hop member index.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Internal error.
 */
static int
bcmecmp_group_rh_memb_select(int unit,
                             uint32_t max_ent_cnt,
                             uint32_t *overflow_cnt,
                             bcmecmp_lt_entry_t *lt_entry,
                             uint32_t *selected_index)
{
    uint32_t mindex = 0; /* Random member index value. */
    uint32_t nindex = 0; /* New member index value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(overflow_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(selected_index, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcmecmp_group_rh_memb_select.\n")));
    /*
     * NHOP member array index is zero based, hence mindex must be less than
     * "num_paths - 1".
     */
    SHR_IF_ERR_EXIT
        (bcmecmp_rh_rand_memb_index_get
            (unit, (lt_entry->num_paths - 1), &mindex));

    if (lt_entry->rh_memb_ecnt_arr[mindex] < max_ent_cnt) {
        lt_entry->rh_memb_ecnt_arr[mindex] += 1;
        *selected_index = mindex;
    } else {
        nindex = (mindex + 1) % lt_entry->num_paths;
        while (nindex != mindex) {
            if (lt_entry->rh_memb_ecnt_arr[nindex] < max_ent_cnt) {
                lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                *selected_index = nindex;
                break;
            } else {
                nindex = (nindex + 1) % lt_entry->num_paths;
            }
        }
        if (nindex == mindex) {
            if (lt_entry->rh_memb_ecnt_arr[nindex]< (max_ent_cnt + 1)
                && (0 != *overflow_cnt)) {
                lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                *selected_index = nindex;
                *overflow_cnt -= 1;
            } else {
                nindex = (mindex + 1) % lt_entry->num_paths;
                while (nindex != mindex) {
                    if (lt_entry->rh_memb_ecnt_arr[nindex] < (max_ent_cnt + 1)
                        && (0 != *overflow_cnt)) {
                        lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                        *selected_index = nindex;
                        *overflow_cnt -= 1;
                        break;
                    } else {
                        nindex = (nindex + 1) % lt_entry->num_paths;
                    }
                }
                if (nindex == mindex) {
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Distributes group members as per RH load balance algorithm.
 *
 * Distributes NUM_PATHS group members among RH_SIZE member table entries for
 * installation in ECMP hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_rh_entries_set(int unit,
                             bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t max_ent_cnt = 0;  /* Maxiumum entries a member can occupy. */
    uint32_t overflow_cnt = 0; /* Overflow count. */
    uint32_t idx;              /* loop iterator variable. */
    uint32_t selected_index = (uint32_t)BCMECMP_INVALID; /* Selected member
                                                            index. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /*
     * Validate NUM_PATHS value and confirm it's non-zero to perform RH load
     * balancing. If value is zero, there are no members to perform load
     * balancing.
     */
    if (!lt_entry->num_paths) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No members NUM_PATHS=%u.\n"),
                     lt_entry->num_paths));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (lt_entry->num_paths && !lt_entry->rh_entries_arr) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NULL RH_ENTRIES_ARR, NUM_PATHS=%u.\n"),
                     lt_entry->num_paths));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    max_ent_cnt = (lt_entry->rh_entries_cnt / lt_entry->num_paths);
    overflow_cnt = (lt_entry->rh_entries_cnt % lt_entry->num_paths);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[rh_entries_cnt=%-4u, "
                            "num_paths=%-4d, max_ent_cnt:%-4u, ovf_cnt=%u].\n"),
                 lt_entry->rh_entries_cnt,
                 lt_entry->num_paths,
                 max_ent_cnt,
                 overflow_cnt));

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\trh_mindex_arr:\n");
    shr_pb_printf(pb, "\t==============\n");

    for (idx = 0; idx < lt_entry->rh_entries_cnt; idx++) {
        SHR_IF_ERR_EXIT
            (bcmecmp_group_rh_memb_select(unit,
                                          max_ent_cnt,
                                          &overflow_cnt,
                                          lt_entry,
                                          &selected_index));
        if (lt_entry->ecmp_nhop) {
            lt_entry->rh_entries_arr[idx] = lt_entry->uecmp_id[selected_index];
        } else {
            lt_entry->rh_entries_arr[idx] = lt_entry->nhop_id[selected_index];
        }

        /*
         * Store selected next-hop array member index value for use during
         * Add/Delete/Replace operations.
         */
        lt_entry->rh_mindex_arr[idx] = selected_index;

        if (!(idx % 3)) {
            shr_pb_printf(pb, "\n\t");
        }
        shr_pb_printf(pb, "%4d)%5d / midx=%-4u ",
                      idx,
                      lt_entry->rh_entries_arr[idx],
                      lt_entry->rh_mindex_arr[idx]);
    }

    shr_pb_printf(pb, "\n\n");
    shr_pb_printf
        (pb, "\t[npaths=%-4d sort=%d]:\n",
         lt_entry->num_paths,
         lt_entry->nhop_sorted);
    shr_pb_printf
        (pb, "\t=================================\n");
    for (idx = 0; idx < lt_entry->num_paths; idx++) {
        shr_pb_printf
            (pb,
             "\t%d) [NHOP_ID=%-5d RH_ENT_CNT=%-4u]\n",
             idx,
             lt_entry->ecmp_nhop
                ? lt_entry->uecmp_id[idx] : lt_entry->nhop_id[idx],
             lt_entry->rh_memb_ecnt_arr[idx]);
    }
    shr_pb_printf(pb, "\n");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    exit:
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Resilient hash ECMP group data print debug function.
 *
 * Resilient hash ECMP group data print debug function.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_id Resilient hash group identifier.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_data_debug_print(int unit,
                                bcmecmp_id_t ecmp_id,
                                bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t idx; /* Index iterator variable. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb,
                  "\tECMP_ID=%-4d RH_ENTRIES_ARR[rh_entries_cnt==%-4d]:\n",
                  ecmp_id,
                  lt_entry->rh_entries_cnt);
    shr_pb_printf
        (pb, "\t=================================================");
    for (idx = 0; idx < lt_entry->rh_entries_cnt; idx++) {
        if (!(idx % 3)) {
            shr_pb_printf(pb, "\n\t");
        }
        shr_pb_printf(pb,
                      "%4d) %5d / midx=%-4d",
                      idx,
                      lt_entry->rh_entries_arr[idx],
                      lt_entry->rh_mindex_arr[idx]);
    }
    shr_pb_printf(pb, "\n\n");
    shr_pb_printf
        (pb, "\t[npaths=%-4d sort=%d]:\n",
         lt_entry->num_paths,
         lt_entry->nhop_sorted);
    shr_pb_printf
        (pb, "\t=====================================\n");
    for (idx = 0; idx < lt_entry->num_paths; idx++) {
        shr_pb_printf
            (pb,
             "\t%4d) [NHOP_ID=Cur:%-5d New:%-5d / "
             "RH_ENT_CNT=Cur:%-4u New:%-4u]\n",
             idx,
             BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id) ?
                BCMECMP_GRP_UECMP_PTR(unit, ecmp_id)[idx]
                    : BCMECMP_GRP_NHOP_PTR(unit, ecmp_id)[idx],
             lt_entry->ecmp_nhop
                ? lt_entry->uecmp_id[idx] : lt_entry->nhop_id[idx],
             BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx],
             lt_entry->rh_memb_ecnt_arr[idx]);
    }
    shr_pb_printf(pb, "\n");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Add new member to an existing RH-ECMP group.
 *
 * This function adds a new member to an existing RH-ECMP group and re-balances
 * the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] new_mindex New RH group member array index.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_memb_add(int unit,
                        uint32_t new_mindex,
                        bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t ubound = 0, lbound = 0; /* Upper bound and lower bound. */
    uint32_t threshold = 0;          /* Upper threshold value. */
    uint32_t eindex = 0;      /* Selected entry index. */
    uint32_t next_eindex = 0; /* Next entry index. */
    uint32_t *ecount = NULL;  /* New member entries usage count. */
    uint32_t mindex;          /* member index. */
    bool new_memb_sel = FALSE; /* New member selected. */
    int new_nhop_id; /* New NHOP member to be added. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s - n_mindex=%u n_nhop=%d.\n"),
             __func__,
             new_mindex,
             (lt_entry->ecmp_nhop ? lt_entry->uecmp_id[new_mindex] :
                                            lt_entry->nhop_id[new_mindex])));

    lbound = lt_entry->rh_entries_cnt / lt_entry->num_paths;
    ubound = (lt_entry->rh_entries_cnt % lt_entry->num_paths)
                    ? (lbound + 1) : lbound;
    threshold = ubound;

    /*
     * Set local entry count pointer to point to new member's entry usage
     * count array element address.
     */
    ecount = &(lt_entry->rh_memb_ecnt_arr[new_mindex]);

    /*
     * New member initial entries usage count is expected to be zero. If it's
     * non-zero it's an error.
     */
    if (*ecount) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Error: new_mindex=%u non-zero ecount=%u.\n"),
                     new_mindex,
                     *ecount));
        return (SHR_E_INTERNAL);
    }

    /* Get the new next-hop member ID value. */
    if (lt_entry->ecmp_nhop) {
        new_nhop_id = lt_entry->uecmp_id[new_mindex];
    } else {
        new_nhop_id = lt_entry->nhop_id[new_mindex];
    }

    while (*ecount < lbound) {
        /* Select a random entry to replace with new member. */
        SHR_IF_ERR_EXIT
            (bcmecmp_rh_rand_memb_index_get
                (unit, (lt_entry->rh_entries_cnt - 1), &eindex));
        if (lt_entry->rh_mindex_arr[eindex] == new_mindex) {
            new_memb_sel = TRUE;
        } else {
            new_memb_sel = FALSE;
            mindex = lt_entry->rh_mindex_arr[eindex];
        }

        if (!new_memb_sel && (lt_entry->rh_memb_ecnt_arr[mindex] > threshold)) {

            lt_entry->rh_entries_arr[eindex] = new_nhop_id;

            lt_entry->rh_mindex_arr[eindex] = new_mindex;

            /* Decrement entry usage count value for the replaced member. */
            lt_entry->rh_memb_ecnt_arr[mindex] -= 1;

            /* Increment entry usage count value for the new member. */
            *ecount += 1;
        } else {
            /*
             * Arrived here because the randomly selected entry index happened
             * to be the new member or the selected existing member's entry
             * usage count is below the threshold value.
             *
             * In either case, find the next entry index that's either not
             * populated with the new member or existing entry with entry usage
             * count value that's equal to greater than the threshold value.
             */
            next_eindex = (eindex + 1) % lt_entry->rh_entries_cnt;
            while (next_eindex != eindex) {

                mindex = lt_entry->rh_mindex_arr[next_eindex];

                if (mindex == new_mindex) {
                    new_memb_sel = TRUE;
                } else {
                    new_memb_sel = FALSE;
                }

                if (!new_memb_sel
                    && (lt_entry->rh_memb_ecnt_arr[mindex] > threshold)) {
                    /*
                     * Replace existing member with new member next-hop ID
                     * value.
                     */
                    lt_entry->rh_entries_arr[next_eindex] = new_nhop_id;

                    /*
                     * Replace existing member with the new member index
                     * value.
                     */
                    lt_entry->rh_mindex_arr[next_eindex] = new_mindex;

                    /*
                     * Decrement entry usage count value for the replaced
                     * member.
                     */
                    lt_entry->rh_memb_ecnt_arr[mindex] -= 1;

                    /* Increment entry usage count value for the new member. */
                    *ecount += 1;
                    break;
                } else {
                    next_eindex = (next_eindex + 1) % lt_entry->rh_entries_cnt;
                }
            }
            if (next_eindex == eindex) {
                threshold--;
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Add member/s to an existing RH-ECMP group.
 *
 * This function adds member or members to an existing RH-ECMP group
 * and re-balances the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_members_add(int unit,
                           bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t new_mindex; /* New member array index. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_rh_grp_members_add.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->rh_entries_cnt));
    /*
     * Copy RH group's existing entries usage count array into LT entry's RH
     * entries usage count array for update operation.
     */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
               BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * grp_npaths));

    /* RH-Group data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_rh_grp_data_debug_print(unit, ecmp_id, lt_entry)));

    /*
     * Call RH member add function for every new member in the
     * next-hop array. New members must be set in the array after the
     * current next-hop member elements.
     */
    for (new_mindex = grp_npaths; new_mindex < lt_entry->num_paths;
         new_mindex++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_rh_grp_memb_add(unit, new_mindex, lt_entry));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an existing RH-ECMP group.
 *
 * This function deletes a member from an existing RH-ECMP group and
 * re-balances the group with no impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] del_mindex New RH group member array index.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_memb_delete(int unit,
                           uint32_t del_mindex,
                           bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t idx; /* Index iterator variable. */
    uint32_t max_ent_cnt = 0;  /* Maxiumum entries a member can occupy. */
    uint32_t overflow_cnt = 0; /* Overflow count. */
    uint32_t selected_index = (uint32_t)BCMECMP_INVALID; /* Selected member
                                                            index. */
    shr_pb_t *pb = NULL; /* Print buffer. */
    bcmecmp_id_t ecmp_id; /* ECMP group identifier. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_rh_grp_memb_delete: mindex=%u.\n"),
                  del_mindex));

    max_ent_cnt = (lt_entry->rh_entries_cnt / lt_entry->num_paths);
    overflow_cnt = (lt_entry->rh_entries_cnt % lt_entry->num_paths);
    ecmp_id = lt_entry->ecmp_id;

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\trh_mindex_arr:\n");
    shr_pb_printf(pb, "\t==============\n");

    for (idx = 0; idx < lt_entry->rh_entries_cnt; idx++) {
        /*
         * If current member is not the member to be deleted, continue to the
         * next member.
         */
        if (BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] != del_mindex) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmecmp_group_rh_memb_select(unit,
                                          max_ent_cnt,
                                          &overflow_cnt,
                                          lt_entry,
                                          &selected_index));
        if (lt_entry->ecmp_nhop) {
            lt_entry->rh_entries_arr[idx] = lt_entry->uecmp_id[selected_index];
        } else {
            lt_entry->rh_entries_arr[idx] = lt_entry->nhop_id[selected_index];
        }

        /*
         * Delay this update until all members are deleted so that mindex array
         * has old mindex member value for multi member delete operation.
         */
        lt_entry->rh_mindex_arr[idx] = selected_index;

        /* Decrement deleted members entry usage count value.  */
        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[del_mindex] -= 1;

        /* Update debug print buffer. */
        shr_pb_printf(pb, "\t%5d)[n_nhop=%-5d n_midx=%-4u]"
                          " [o_nhop=%-5d o_midx=%4d u_ecnt=%-5d]\n",
                      idx,
                      lt_entry->rh_entries_arr[idx],
                      selected_index,
                      (lt_entry->ecmp_nhop ?
                            BCMECMP_GRP_UECMP_PTR(unit, ecmp_id)[del_mindex]
                            : BCMECMP_GRP_NHOP_PTR(unit, ecmp_id)[del_mindex]),
                      del_mindex,
                      BCMECMP_GRP_RH_MEMB_ENTCNT_PTR
                            (unit, ecmp_id)[del_mindex]);

        /*
         * If all the entries have been replaced, update next-hop value as
         * invalid.
         */
        if (!BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[del_mindex]) {
            if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)) {
                BCMECMP_GRP_UECMP_PTR
                    (unit, ecmp_id)[del_mindex] = BCMECMP_INVALID;
            } else {
                BCMECMP_GRP_NHOP_PTR
                    (unit, ecmp_id)[del_mindex] = BCMECMP_INVALID;
            }
        }
    }
    shr_pb_printf(pb, "\n");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    exit:
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete member/s from an existing RH-ECMP group.
 *
 * This function deletes member or members from an existing RH-ECMP group
 * and re-balances the group with no impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_members_delete(int unit,
                              bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t deleted_cnt = 0; /* Number of members deleted from the group. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t *deleted_mindex = NULL; /* Array of deleted members indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t shrd_new_mindex_cnt = 0; /* Count of shared members with new
                                         mindex. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t del_idx = 0; /* Deleted members array index iterator. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    bcmecmp_id_t ecmp_id; /* ECMP group identifier. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group updated next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_rh_grp_members_delete.\n")));

    ecmp_id = lt_entry->ecmp_id;

    /*
     * Check if num_paths is non-zero in this delete (update) operation. If
     * there are valid members after delete, the remaining members must be
     * re-balanced.
     *
     * If there no members left in the group after this delete operation,
     * re-balance operation is not required. Clear the entries in the member
     * table, clear the mindex value for all entries in this group, reset
     * entries usage count and return.
     */
    if (lt_entry->num_paths) {
        grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

        /* Determine how many members have been deleted. */
        deleted_cnt = (grp_npaths - lt_entry->num_paths);

        /*
         * Initialize member array pointers based on the group's next-hop
         * type.
         */
        if (lt_entry->ecmp_nhop) {
            memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
            umemb_arr = lt_entry->uecmp_id;
        } else {
            memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
            umemb_arr = lt_entry->nhop_id;
        }

        /* Valid member array must be preset, else it's an internal error. */
        SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
        SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);

        /*
         * For update operation, copy existing group RH entries array
         * into LT entry's RH entries array for updation.
         */
        sal_memcpy(lt_entry->rh_entries_arr,
                   BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                   (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                    lt_entry->rh_entries_cnt));
        /*
         * Copy RH group's existing entries mapped member index array into LT
         * entry's RH entries mapped member index array for update operation.
         */
        sal_memcpy(lt_entry->rh_mindex_arr,
                   BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   (sizeof(*(lt_entry->rh_mindex_arr)) *
                    lt_entry->rh_entries_cnt));

        BCMECMP_ALLOC(deleted_mindex,
                      deleted_cnt * sizeof(*deleted_mindex),
                      "bcmecmpRhGrpMembsDelMindexArr");

        BCMECMP_ALLOC(shrd_mindex,
                      (grp_npaths * sizeof(*shrd_mindex)),
                      "bcmecmpRhGrpMembsDelShrdMindexArr");
        sal_memset(shrd_mindex,
                   BCMECMP_INVALID,
                   (grp_npaths * sizeof(*shrd_mindex)));

        /*
         * The set of new group members are a sub-set of the current members
         * list as some of the existing members have been deleted from the list
         * as part of update operation.
         *
         * So, for every member in the existing members array (outer for-loop),
         * check if this member exists in the updated members array
         * (inner for-loop). If it's not present, store this member index in the
         * deleted member indices array.
         */
        for (idx = 0; idx < grp_npaths; idx++) {
            /*
             * num_paths value must be less than grp_npaths as it's member/s
             * delete operation and number of elements in umemb_arr is equal to
             * num_paths.
             */
            for (uidx = 0; uidx < lt_entry->num_paths; uidx++) {
                /*
                 * Compare existing nhop members with members in updated members
                 * array, confirm rh_memb_ecnt_arr value is equal to zero for
                 * the updated nhop member to skip duplicate members.
                 */
                if (memb_arr[idx] == umemb_arr[uidx]
                    && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                    lt_entry->rh_memb_ecnt_arr[uidx] =
                            BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
                    shrd_cnt++;
                    if (idx != uidx) {
                        /*
                         * Save new mindex mapped to the old mindex value for
                         * update after the delete operation.
                         */
                        shrd_mindex[idx] = uidx;
                        shrd_new_mindex_cnt++;

                        /*
                         * Reset the entry usage counter value as this member
                         * will be maintained in the new index location "uidx"
                         * after the delete operation.
                         */
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                    }
                    break;
                }
            }

            /*
             * If the member is present in the updated nhop members array, move
             * to the next member in existing array.
             */
            if (uidx != lt_entry->num_paths) {
                continue;
            }

            /*
             * Member not found in the updated array, add this member index to
             * be deleted members list.
             */
            if (del_idx < deleted_cnt) {
                deleted_mindex[del_idx++] = idx;
            } else {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "del_idx = %u > del_max_idx = %u.\n"),
                            del_idx,
                            (deleted_cnt - 1)));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }

        for (idx = 0; idx < deleted_cnt; idx++) {
            /* RH-Group data print debug function. */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d"),
                        bcmecmp_rh_grp_data_debug_print(unit, ecmp_id,
                                                        lt_entry)));

            /* Delete the member and re-balance the group entries. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_grp_memb_delete(unit,
                                            deleted_mindex[idx],
                                            lt_entry));
        }

        /*
         * For the shared members, check and update the member index value in
         * the LT entry mindex array if it has changed.
         */
        if (shrd_new_mindex_cnt) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "\n\t[Shared mindex update]\n");
            shr_pb_printf(pb, "\t======================\n");
            for (idx = 0; idx < grp_npaths; idx++) {
                new_idx = shrd_mindex[idx];
                if (new_idx == (uint32_t)BCMECMP_INVALID) {
                    continue;
                }
                for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                    if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                        && idx == lt_entry->rh_mindex_arr[uidx]) {
                        shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                      "[n_mindex=%u / n_nhop=%-5d]\n",
                                      idx,
                                      memb_arr[idx],
                                      new_idx,
                                      lt_entry->rh_entries_arr[uidx]);

                        /* Update mindex with new mindex value. */
                        lt_entry->rh_mindex_arr[uidx] = new_idx;
                    }
                }
                shrd_new_mindex_cnt--;
            }
            shr_pb_printf(pb, "\t** Remaining shrd_entries for update=%u **\n",
                          shrd_new_mindex_cnt);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
    } else {
        /* Clear RH-ECMP group member table entries. */
        sal_memset(lt_entry->rh_entries_arr,
                   0,
                   (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                    lt_entry->rh_entries_cnt));

        /* Clear stored RH-ECMP group member indices data. */
        sal_memset(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                    lt_entry->rh_entries_cnt));

        /* Clear stored RH-ECMP group members entry usage count data. */
        sal_memset(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) *
                    lt_entry->max_paths));
    }

    exit:
        if (lt_entry->num_paths) {
            /* Free memory allocated to store mindex array. */
            BCMECMP_FREE(deleted_mindex);
            BCMECMP_FREE(shrd_mindex);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Replace member/s from an existing RH-ECMP group.
 *
 * This function replaces member or members from an existing RH-ECMP group
 * and re-balances the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_members_replace(int unit,
                               bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t idx, uidx;       /* Array index iterator variables. */
    bcmecmp_id_t ecmp_id;     /* ECMP group identifier. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_rh_grp_members_replace.\n")));

    ecmp_id = lt_entry->ecmp_id;

    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * For replace operation, update num_paths must be equal to group's current
     * num_paths value.
     */
    if (grp_npaths - lt_entry->num_paths) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    if (lt_entry->ecmp_nhop) {
        memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->uecmp_id;
    } else {
        memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->nhop_id;
    }

    /* Valid member array must be preset, else it's an internal error. */
    SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
    SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->rh_entries_cnt));

    /* Copy members entry usage count information. */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
               BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * lt_entry->num_paths));

    /* RH-Group data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_rh_grp_data_debug_print(unit, ecmp_id, lt_entry)));

    /*
     * Find and replace the existing nhop member with the new nhop member
     * ID value.
     */
    for (idx = 0; idx < lt_entry->num_paths; idx++) {
        if (memb_arr[idx] != umemb_arr[idx]) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "\n");
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {
                    lt_entry->rh_entries_arr[uidx] = umemb_arr[idx];
                    shr_pb_printf(pb, "\t%5d) o_nhop = %-5d n_nhop=%-5d\n",
                                  uidx,
                                  memb_arr[idx],
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
            shr_pb_printf(pb, "\n");
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Add, replace and re-balance member/s in an existing RH-ECMP group.
 *
 * This function performs member/s add, replace and re-balance operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 * \return SHR_E_INTERNAL Invalid configuration.
 */
static int
bcmecmp_rh_grp_members_add_replace(int unit,
                                   bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t added_cnt = 0; /* Number of members added to the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_mindex_cnt = 0; /* Count of shared members with new mindex. */
    uint32_t rep_idx = 0; /* Added member array index. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group updated next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_rh_grp_members_add_replace.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    if (lt_entry->ecmp_nhop) {
        memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->uecmp_id;
    } else {
        memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->nhop_id;
    }

    /*
     * Current and updated member array pointers must be valid, check and
     * return error if this condition is not met.
     */
    SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
    SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->rh_entries_cnt));

    BCMECMP_ALLOC(replace_mindex,
                  (grp_npaths * sizeof(*replace_mindex)),
                  "bcmecmpRhGrpMembsAddRepReplaceMindexArr");
    BCMECMP_ALLOC(shrd_mindex,
                  (grp_npaths * sizeof(*shrd_mindex)),
                  "bcmecmpRhGrpMembsAddRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMECMP_INVALID,
               (grp_npaths * sizeof(*shrd_mindex)));

    /*
     * Identify the members that are shared between the updated NHOP array and
     * the current NHOP members up to the group's current NUM_PATHS value.
     * Members in the updated array after current NUM_PATHS value will be
     * treated as member add operation.
     * For members up to grp_npaths that are not shared with the updated members
     * array will be candidates for replace
     * operation.
     */
    for (idx = 0; idx < grp_npaths; idx++) {
        for (uidx = 0; uidx < lt_entry->num_paths; uidx++) {
            if ((memb_arr[idx] == umemb_arr[uidx])
                && (lt_entry->rh_memb_ecnt_arr[uidx] == 0)) {
                /* Update entry usage count. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];

                /* Increment shared entry count. */
                shrd_cnt++;

                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_mindex_cnt++;
                }

                /* Clear old entry usage count value. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                break;
            }
        }

        /*
         * Existing member at current index is also part of the updated
         * next-hop members array, continue to the next member.
         */
        if (uidx != lt_entry->num_paths) {
            continue;
        }

        /* Member not shared hence add to replace member elements list. */
        replace_mindex[replace_cnt++] = idx;
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t[grp_npaths=%u Shrd_cnt=%u Rep_cnt=%u]\n",
                  grp_npaths, shrd_cnt, replace_cnt);
    shr_pb_printf(pb, "\t======================================\n");

    if (replace_cnt) {
        BCMECMP_ALLOC(new_mindex,
                      replace_cnt * sizeof(*new_mindex),
                      "bcmecmpRhGrpMembsAddRepNewMindexArr");

        shr_pb_printf(pb, "\t[Members replacment list]\n");
        shr_pb_printf(pb, "\t=========================\n");

        for (idx = 0; idx < replace_cnt; idx++) {

            /* Get the old to be replaced index value. */
            rep_idx = replace_mindex[idx];

            for (uidx = 0; uidx < grp_npaths; uidx++) {

                if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                    /* Already updated, move to next member. */
                    continue;
                }

                /* Update entry count value. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];

                /* Clear old entry usage count value. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Store the new mindex mapped to this old mindex for updating
                 * the mindex and rh_entries_arr arrays.
                 */
                new_mindex[idx] = uidx;

                shr_pb_printf(pb,
                          "\trep_arr.(idx=%u) o_mindex=%u n_mindex=%u\n",
                           idx,
                           rep_idx,
                           uidx);
                break;
            }
        }
    }

    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_mindex_cnt) {
        shr_pb_printf(pb, "\t[Shared mindex update]\n");
        shr_pb_printf(pb, "\t======================\n");
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                    && idx == lt_entry->rh_mindex_arr[uidx]) {

                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  idx,
                                  memb_arr[idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);

                }
            }
            shrd_mindex_cnt--;
        }
        shr_pb_printf(pb, "\t** Remaining shrd_entries for update=%u **\n",
                      shrd_mindex_cnt);
    }

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Replaced mindex update]\n");
        shr_pb_printf(pb, "\t========================\n");

        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];

            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[rep_idx] == lt_entry->rh_entries_arr[uidx]
                    && rep_idx == lt_entry->rh_mindex_arr[uidx]) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    lt_entry->rh_entries_arr[uidx] = umemb_arr[new_idx];

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  rep_idx,
                                  memb_arr[rep_idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));

    for (new_idx = 0; new_idx < lt_entry->num_paths;
         new_idx++) {
        if (lt_entry->rh_memb_ecnt_arr[new_idx]) {
            continue;
        }
        /* RH-Group data print debug function. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    bcmecmp_rh_grp_data_debug_print(unit, ecmp_id, lt_entry)));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_rh_grp_memb_add(unit, new_idx, lt_entry));
        added_cnt++;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\n\tadded_cnt=%u n_mindex=%u n_nhop=%d\n"),
                     added_cnt, new_idx, umemb_arr[new_idx]));
    }

    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
        }
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete, replace and re-balance member/s in an existing RH-ECMP group.
 *
 * This function performs member/s delete, replace and re-balance operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 * \return SHR_E_INTERNAL Invalid configuration.
 */
static int
bcmecmp_rh_grp_members_delete_replace(int unit,
                                      bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id; /* ECMP group identifier. */
    uint32_t grp_npaths = 0; /* Current num_paths value of the group. */
    uint32_t *deleted_mindex = NULL; /* Array of deleted member/s indices. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t deleted_cnt = 0; /* Number of members deleted from the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_new_mindex_cnt = 0; /* Count of shared members with new
                                         mindex. */
    uint32_t del_idx = 0; /* Deleted members array index. */
    uint32_t rep_idx = 0; /* Replace member array index. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group updated next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_rh_grp_members_delete_replace.\n")));

    /*
     * NUM_PATHS must be non-zero when this function is called for member delete
     * operation, check and return if this condition is not met.
     */
    if (!lt_entry->num_paths) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Get the group's ECMP_ID value and current NUM_PATHS values. */
    ecmp_id = lt_entry->ecmp_id;
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * Determine how many members have been deleted based on the updated
     * NUM_PATHS value.
     */
    deleted_cnt = (grp_npaths - lt_entry->num_paths);

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    if (lt_entry->ecmp_nhop) {
        memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->uecmp_id;
    } else {
        memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->nhop_id;
    }

    /*
     * Current and updated member array pointers must be valid, check and
     * return error if this condition is not met.
     */
    SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
    SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);

    /*
     * Copy existing group RH entries array into LT entry RH entries array for
     * members delete + replace operation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));
    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->rh_entries_cnt));

    BCMECMP_ALLOC(deleted_mindex,
                  deleted_cnt * sizeof(*deleted_mindex),
                  "bcmecmpRhGrpMembsDelRepDelMindexArr");

    BCMECMP_ALLOC(replace_mindex,
                  (lt_entry->num_paths) * sizeof(*replace_mindex),
                  "bcmecmpRhGrpMembsDelRepReplaceMindexArr");

    BCMECMP_ALLOC(shrd_mindex,
                  (grp_npaths * sizeof(*shrd_mindex)),
                  "bcmecmpRhGrpMembsDelRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMECMP_INVALID,
               (grp_npaths * sizeof(*shrd_mindex)));

    /*
     * Order of NHOP members in updated NHOP array is not same as the stored
     * NHOP members array and some of the members in the updated array might be
     * shared with the stored members. Identify these shared members, deleted
     * members and members to be replaced.
     */
    for (idx = 0; idx < grp_npaths; idx++) {
        for (uidx = 0; uidx < lt_entry->num_paths; uidx++) {
            /*
             * Identify the common members and copy the stored member entry
             * usage count value to LT entry usage count element at the new
             * member index for delete and rebalance operation.
             */
            if (memb_arr[idx] == umemb_arr[uidx]
                && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
                shrd_cnt++;
                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_new_mindex_cnt++;
                    /*
                     * Reset the entry usage counter value as this member
                     * will be maintained in the new index location "uidx"
                     * after the delete operation.
                     */
                }
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                break;
            }
        }

        /*
         * If the member is a shared member, then continue to the next member
         * in the list.
         */
        if (uidx != lt_entry->num_paths) {
            continue;
        }

        /*
         * Member not found in the updated array, add this old member index
         * value be the deleted members array.
         */
        if (del_idx < deleted_cnt) {
            deleted_mindex[del_idx++] = idx;
        } else {
            /*
             * If this member is not a shared member and members delete count
             * has already been reached, then save this member for replacing
             * with a new member in the updated members array.
             */
            replace_mindex[replace_cnt++] = idx;
        }
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t[Shrd_cnt=%u / Shrd_n_mindex_cnt=%u Del_cnt=%u"
                      " Rep_cnt=%u]\n",
                  shrd_cnt,
                  shrd_new_mindex_cnt,
                  deleted_cnt,
                  replace_cnt);
    shr_pb_printf(pb, "\t=================================================\n");

    /*
     * If there are members to be replaced, identify the members from the
     * updated NHOP members array that will be replacing the current members.
     * Save the new member index value that is replacing a current member and
     * also assign the current members entry usage count value to the new
     * member in LT entry array for use during delete member rebalance
     * calculations.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Members replacment list]\n");
        shr_pb_printf(pb, "\t=========================\n");
        BCMECMP_ALLOC(new_mindex,
                      (replace_cnt) * sizeof(*new_mindex),
                      "bcmecmpRhGrpMembsNewMindexArr");
        for (uidx = 0; uidx < lt_entry->num_paths; uidx++) {
            /*
             * Shared member entry count is already found, skip to next
             * member.
             */
            if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                continue;
            }
            for (idx = 0; idx < replace_cnt; idx++) {
                rep_idx = replace_mindex[idx];
                if (!BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx]) {
                    /*
                     * This replacement member candidate has already been
                     * associate with a new member from the updated array, move
                     * on to the next member in the replace mindex array.
                     */
                    continue;
                }
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Save the new member index value that is replacing the current
                 * replacement member.
                 */
                new_mindex[idx] = uidx;

                shr_pb_printf(pb,
                              "\trep_arr.(idx=%u) o_mindex=%u n_mindex=%u\n",
                              idx,
                              rep_idx,
                              uidx);
                break;

            }
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    pb = NULL;

    /* Perform members delete and rebalance operation. */
    for (idx = 0; idx < deleted_cnt; idx++) {
        /* RH-Group data print debug function. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    bcmecmp_rh_grp_data_debug_print(unit, ecmp_id, lt_entry)));

        /* Delete the member and re-balance the group entries. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_rh_grp_memb_delete(unit,
                                        deleted_mindex[idx],
                                        lt_entry));
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_new_mindex_cnt) {
        shr_pb_printf(pb, "\t[Shared mindex update]\n");
        shr_pb_printf(pb, "\t======================\n");
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                    && idx == lt_entry->rh_mindex_arr[uidx]) {
                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  idx,
                                  memb_arr[idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);

                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                }
            }
            shrd_new_mindex_cnt--;
        }
        shr_pb_printf(pb, "\t** Remaining shrd_entries for update=%u **\n",
                      shrd_new_mindex_cnt);
    }

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Replaced mindex update]\n");
        shr_pb_printf(pb, "\t========================\n");
        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[rep_idx] == lt_entry->rh_entries_arr[uidx]
                    && rep_idx == lt_entry->rh_mindex_arr[uidx]) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    lt_entry->rh_entries_arr[uidx] = umemb_arr[new_idx];

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  rep_idx,
                                  memb_arr[rep_idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));
    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(deleted_mindex);
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
        }
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Determine the type of operation being performed on the RH-ECMP group.
 *
 * This function determines the type of operation being performed on the
 * member/s of a RH-ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 * \param [out] rh_oper Resilient hashing ECMP group members operation type.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 */
static int
bcmecmp_group_rh_oper_get(int unit,
                          bcmecmp_lt_entry_t *lt_entry,
                          bcmecmp_rh_oper_t *rh_oper)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths; /* Group current NUM_PATHS value. */
    uint32_t idx = 0, uidx = 0; /* Next-hop members iterator variables. */
    uint32_t shared_nhop_cnt = 0; /* Shared NHOPs count. */
    uint32_t new_nhop_cnt = 0; /* New NHOPs count. */
    uint32_t alloc_sz = 0, nalloc_sz = 0; /* Alloc size current and new. */
    uint32_t max_shared = 0;
    int *nhop_arr = NULL; /* NHOP members array. */
    int *shrd_nhop_arr = NULL; /* Shared NHOP members array. */
    int *new_nhop_arr = NULL; /* New NHOP members in updated members list. */
    bool mindex_changed = FALSE; /* Member index has changed in NHOP array. */
    const char * const rh_op_str[BCMECMP_RH_OPER_COUNT] = BCMECMP_RH_OPER_NAME;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(rh_oper, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_rh_oper_get.\n")));
    ecmp_id = lt_entry->ecmp_id;


    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    alloc_sz = sizeof(*nhop_arr) * grp_npaths;
    nalloc_sz = sizeof(*shrd_nhop_arr) * lt_entry->num_paths;
    max_shared = ((alloc_sz > nalloc_sz) ? nalloc_sz : alloc_sz);

    BCMECMP_ALLOC(nhop_arr,
                  alloc_sz,
                  "bcmecmpRhOperGetNhopArr");
    BCMECMP_ALLOC(shrd_nhop_arr,
                  max_shared,
                  "bcmecmpRhOperGetUpdtNhopArr");
    BCMECMP_ALLOC(new_nhop_arr,
                  nalloc_sz,
                  "bcmecmpRhOperGetNewNhopArr");

    /*
     * Check if the group next-hop members are regular next-hops or underlay
     * ECMP group next-hops.
     */
    if (lt_entry->ecmp_nhop) {
        /*
         * Identify the shared members and the new members in the updated
         * next-hop members array.
         */
        sal_memcpy(nhop_arr, BCMECMP_GRP_UECMP_PTR(unit, ecmp_id), alloc_sz);

        for (uidx = 0; uidx < lt_entry->num_paths; uidx++) {
            for (idx = 0; idx < grp_npaths; idx++) {
                if ((nhop_arr[idx] != BCMECMP_INVALID)
                     && (nhop_arr[idx] == lt_entry->uecmp_id[uidx])) {
                    /* Old NHOP also member of the updated NHOP array. */
                    shrd_nhop_arr[shared_nhop_cnt++] = lt_entry->uecmp_id[uidx];
                    nhop_arr[idx] = BCMECMP_INVALID;
                    /*
                     * Check if member index has changed in the updated members
                     * array.
                     */
                    if (idx != uidx) {
                        mindex_changed = TRUE;
                    }
                    break;
                }
            }
            if (idx == grp_npaths) {
                new_nhop_arr[new_nhop_cnt++] = lt_entry->uecmp_id[uidx];
            }
        }
    } else {
        /*
         * Identify the shared members and the new members in the updated
         * next-hop members array.
         */
        sal_memcpy(nhop_arr, BCMECMP_GRP_NHOP_PTR(unit, ecmp_id), alloc_sz);

        for (uidx = 0; uidx < lt_entry->num_paths; uidx++) {
            for (idx = 0; idx < grp_npaths; idx++) {
                if ((nhop_arr[idx] != BCMECMP_INVALID)
                     && (nhop_arr[idx] == lt_entry->nhop_id[uidx])) {
                    /* Old NHOP also member of the updated NHOP array. */
                    shrd_nhop_arr[shared_nhop_cnt++] = lt_entry->nhop_id[uidx];
                    nhop_arr[idx] = BCMECMP_INVALID;
                    /*
                     * Check if member index has changed in the updated members
                     * array.
                     */
                    if (idx != uidx) {
                        mindex_changed = TRUE;
                    }
                    break;
                }
            }
            if (idx == grp_npaths) {
                new_nhop_arr[new_nhop_cnt++] = lt_entry->nhop_id[uidx];
            }
        }
    }

    /* Determine the type of operation to be performed on RH group members. */
    if (lt_entry->num_paths > grp_npaths) {
        if ((shared_nhop_cnt == grp_npaths) && !mindex_changed) {
            *rh_oper = BCMECMP_RH_OPER_ADD;
        } else {
            *rh_oper = BCMECMP_RH_OPER_ADD_REPLACE;
        }
    } else if (lt_entry->num_paths < grp_npaths) {
        if (shared_nhop_cnt == lt_entry->num_paths) {
            /*
             * This condition is TRUE when all the members passed in the
             * member/s delete-update operation are already members of this
             * group (Their mindex could have changed but they are still common
             * members between the NHOP members list).
             * This condition is also TRUE when all the members have been
             * deleted in the update operation i.e. 'lt_entry->num_paths == 0',
             * means there are no shared members in the group in this delete
             * operation. As shared_nhop_cnt default value is initialized to '0'
             * at the start of this function, This condition will be TRUE.
             */
            *rh_oper = BCMECMP_RH_OPER_DELETE;
        } else {
            *rh_oper = BCMECMP_RH_OPER_DELETE_REPLACE;
        }
    } else if (lt_entry->num_paths == grp_npaths) {
        *rh_oper = BCMECMP_RH_OPER_REPLACE;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t[grp_npaths=%-4u num_paths=%-4u"
                            "shared_nhop_cnt=%-4u\n"
                            "\t new_nhop_cnt=%-4u mindex_changed=%s "
                            "rh_oper=(%s)]\n"),
                 grp_npaths,
                 lt_entry->num_paths,
                 shared_nhop_cnt,
                 new_nhop_cnt,
                 mindex_changed ? "TRUE" : "FALSE",
                 rh_op_str[*rh_oper]));

    exit:
        BCMECMP_FREE(nhop_arr);
        BCMECMP_FREE(shrd_nhop_arr);
        BCMECMP_FREE(new_nhop_arr);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Distributes group members as per RH load balance algorithm.
 *
 * Distributes NUM_PATHS group members among RH_SIZE member table entries for
 * installation in ECMP hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_rh_entries_update(int unit,
                                bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_rh_oper_t rh_oper = BCMECMP_RH_OPER_INVALID; /* RH oper type. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_rh_entries_update.\n")));

    /* Get the type of RH-ECMP group operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_group_rh_oper_get(unit, lt_entry, &rh_oper));

    /* Invoke operation dependent function. */
    switch (rh_oper) {
        case BCMECMP_RH_OPER_ADD:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_grp_members_add(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_grp_members_delete(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_grp_members_replace(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_ADD_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_grp_members_add_replace(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_DELETE_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_grp_members_delete_replace(unit, lt_entry));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Set NHOP_ID[]/ECMP_UNDERLAY_ID[] elements at unspecified indices.
 *
 * Set unspecifed NHOP_ID[]/ECMP_UNDERLAY_ID[] array element values to default
 * value, based on the logical table entry's group type.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_add_member_fields_set(int unit,
                                    bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    uint32_t idx; /* Index iterator. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_add_member_fields_set.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;

    /*
     * Check the group type and update NHOP_ID[] or ECMP_UNDERLAY_ID[] array
     * elements with default/min value if a value has not been specified by
     * the end-user for this field.
     */
    if (gtype == BCMECMP_GRP_TYPE_OVERLAY && lt_entry->ecmp_nhop) {
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID))
              || (lt_entry->uecmp_ids_count < BCMECMP_LT_MAX_PATHS
                                                    (unit, group, gtype))) {
            /* Set unspecified ECMP_UNDERLAY_ID[] fields to default value. */
            for (idx = lt_entry->uecmp_ids_count;
                 idx < BCMECMP_LT_MAX_PATHS(unit, group, gtype);
                 idx++) {
                lt_entry->uecmp_id[idx] = BCMECMP_LT_MIN_UECMP_ID
                                                (unit, group, gtype);
            }
        }
    } else {
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID))
              || (lt_entry->nhop_ids_count < BCMECMP_LT_MAX_PATHS
                                                (unit, group, gtype))) {
            /*
             * Set unspecified NHOP_ID[] fields to default value based on the
             * group type (LT).
             */
            for (idx = lt_entry->nhop_ids_count; idx < lt_entry->max_paths;
                 idx++) {
                lt_entry->nhop_id[idx] = BCMECMP_LT_MIN_NHOP_ID
                                                (unit, group, gtype);
            }
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Set NHOP_ID[]/ECMP_UNDERLAY_ID[] elements at unspecified indices.
 *
 * Set unspecified NHOP_ID[]/ECMP_UNDERLAY_ID[] array element values from the
 * ECMP group database.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_update_member_fields_set(int unit,
                                       bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;            /* Group ECMP_ID. */
    uint32_t idx; /* Index iterator. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_update_member_fields_set.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;

    /* Initialize group ECMP_ID and group type local variables. */
    ecmp_id = lt_entry->ecmp_id;

    /*
     * Check the group type and update NHOP_ID[] or ECMP_UNDERLAY_ID[] array
     * elements with the stored value, if a value has not been specified by
     * the end-user for this field.
     */
    if (gtype == BCMECMP_GRP_TYPE_OVERLAY && lt_entry->ecmp_nhop) {
        /*
         * If ECMP_UNDERLAY_ID[] field values are not specified (or) if the
         * total count of ECMP_UNDERLAY_ID[] fields set is less than the
         * group's MAX_PATHS value, then for unspecified ECMP_UNDERLAY_ID[]
         * array elements, set the values from the group's stored database.
         */
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID))
              || (lt_entry->uecmp_ids_count < lt_entry->max_paths)) {
            for (idx = lt_entry->uecmp_ids_count;
                 idx < lt_entry->max_paths;
                 idx++) {
                lt_entry->uecmp_id[idx] = (BCMECMP_GRP_UECMP_PTR(unit,
                                                                 ecmp_id)[idx]);
            }
        }
    } else {
        /*
         * If NHOP_ID[] field values are not specified (or) if the total count
         * of NHOP_ID[] fields set is less than the group's MAX_PATHS value,
         * then for unspecified NHOP_ID[] array elements, set the values from
         * the group's stored database.
         */
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID))
              || (lt_entry->nhop_ids_count < lt_entry->max_paths)) {
            for (idx = lt_entry->nhop_ids_count;
                 idx < lt_entry->max_paths;
                 idx++) {
                lt_entry->nhop_id[idx] = (BCMECMP_GRP_NHOP_PTR(unit,
                                                               ecmp_id)[idx]);
            }
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Add an ECMP Group logical table entry to hardware tables.
 *
 * Reserves hardware table resources and installs ECMP logical table entry
 * fields to corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
int
bcmecmp_group_add(int unit,
                  bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bcmecmp_tbl_op_t tbl_op; /* Table operation info. */
    int rv;                  /* Return value. */
    bool rh_lb_mode = FALSE; /* Group's RH-LB_MODE status. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_add.\n")));

    /* Get ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    if (BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d already Exists.\n"), ecmp_id));
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d is New.\n"), ecmp_id));

        /* Set values for NHOP_ID and NUM_PATHS fields if not specified. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_group_add_member_fields_set(unit, lt_entry));

        /* Get RH LB_MODE status for further group configuration. */
        rh_lb_mode = (BCMECMP_LB_MODE_RESILIENT == lt_entry->lb_mode);

        /* Initialize table operation structure and set the operation type. */
        bcmecmp_tbl_op_t_init(&tbl_op);
        tbl_op.oper_flags |= BCMECMP_OPER_ADD;

        /*
         * Initialize number of entries to be reserved in member table based on
         * group's LB_MODE.
         */
        if (rh_lb_mode) {
            tbl_op.num_ent = lt_entry->rh_entries_cnt;
        } else {
            tbl_op.num_ent = lt_entry->max_paths;
        }

        /*
         * For HECMP Single lookup groups that are in Underlay ECMP group ID
         * range, member table entries must be allocated from Underlay member
         * table indicies range in HW.
         */
        if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)
            && (lt_entry->glt_sid == BCMECMP_TBL_LTD_SID
                                        (unit, group, BCMECMP_GRP_TYPE_SINGLE))
            && (ecmp_id >= BCMECMP_TBL_IDX_MIN
                                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY))) {
            tbl_op.tbl_ptr = BCMECMP_TBL_PTR
                                (unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
        } else {
            tbl_op.tbl_ptr = BCMECMP_TBL_PTR(unit, member, gtype);
        }

        /* Get free base index from member table. */
        rv = bcmecmp_tbl_free_idx_get(unit, &tbl_op);
        if (SHR_E_FULL == rv) {
            /* TBD: Compact group entries to create space for new request. */
            SHR_RETURN_VAL_EXIT(rv);
        } else if (SHR_E_NONE != rv) {
            SHR_RETURN_VAL_EXIT(rv);
        }

        /* Update allocated ECMP Group member table Base Index information. */
        lt_entry->mstart_idx = tbl_op.free_idx;

        /*
         * Check if group's LB_MODE is RESILIENT and call the corresponding
         * group install routine.
         */
        if (rh_lb_mode) {
            /*
             * Allocate memory to store the RH balanced nhop members for
             * installation in hardware tables.
             */
            BCMECMP_ALLOC
                (lt_entry->rh_entries_arr,
                 lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_entries_arr)),
                 "bcmecmpRhGrpAddLtEntRhEntArr");

            /*
             * Allocate memory to store the RH group entry's mapped member index
             * data.
             */
            BCMECMP_ALLOC
                (lt_entry->rh_mindex_arr,
                 lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_mindex_arr)),
                 "bcmecmpRhGrpAddLtEntRhMindexArr");

            /*
             * Allocate memory to store the RH group members entries usage count
             * data.
             */
            BCMECMP_ALLOC
                (lt_entry->rh_memb_ecnt_arr,
                 lt_entry->max_paths * sizeof(*(lt_entry->rh_memb_ecnt_arr)),
                 "bcmecmpRhGrpAddLtEntRhMembEntCntArr");

            SHR_IF_ERR_EXIT
                (bcmecmp_group_rh_entries_set(unit, lt_entry));

            /* Install RH-ECMP Group in hardware tables. */
            SHR_IF_ERR_EXIT
                (BCMECMP_FNCALL_EXEC(unit, rh_grp_ins)(unit, lt_entry));

            /*
             * Increment Member table entries reference count by group's
             * RH_SIZE i.e. rh_entries_cnt value.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_tbl_ref_cnt_incr
                    (BCMECMP_TBL_PTR(unit, member, gtype),
                     lt_entry->mstart_idx,
                     lt_entry->rh_entries_cnt));

            BCMECMP_ALLOC(BCMECMP_GRP_RHG_PTR(unit, ecmp_id),
                          sizeof(*BCMECMP_GRP_RHG_PTR(unit, ecmp_id)),
                          "bcmecmpRhGrpAddRhgInfo");
            BCMECMP_ALLOC(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id),
                          sizeof(*BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)),
                          "bcmecmpRhGrpAddRhgInfoBk");

            /* Allocate memory to store RH-ECMP group entries. */
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                 lt_entry->rh_entries_cnt *
                 sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhEntArr");
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                 lt_entry->rh_entries_cnt *
                 sizeof(*(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhEntArrBk");

            /* Copy load balanced RH members array. */
            sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                       lt_entry->rh_entries_arr,
                       sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                              lt_entry->rh_entries_cnt);

            /*
             * Allocate memory to store group RH-ECMP entries mapped member
             * index information.
             */
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                 lt_entry->rh_entries_cnt *
                 sizeof(*(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMindexArr");
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                 lt_entry->rh_entries_cnt *
                 sizeof(*(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMindexArrBk");

            /* Copy group entries mapped member index array. */
            sal_memcpy(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                       lt_entry->rh_mindex_arr,
                       (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                              lt_entry->rh_entries_cnt));

            /*
             * Allocate memory to store RH-ECMP members entries usage count
             * information.
             */
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMembEntCntArr");
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMembEntCntArrBk");

            /* Copy members entry usage count information. */
            sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                       lt_entry->rh_memb_ecnt_arr,
                       (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id))
                        * lt_entry->num_paths));
        } else {
            /* Sort the nhop array elements if it's been requested. */
            if (lt_entry->nhop_sorted) {
                SHR_IF_ERR_EXIT
                    (bcmecmp_group_nhops_sort(unit, lt_entry));
            }

            /* Install ECMP Group in hardware tables. */
            SHR_IF_ERR_EXIT
                (BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));

            /*
             * Increment Member table entries reference count by MAX_PATHS
             * value of the group.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_tbl_ref_cnt_incr
                    (BCMECMP_TBL_PTR(unit, member, gtype),
                     lt_entry->mstart_idx,
                     lt_entry->max_paths));
        }

        /* Store ECMP Group information in Per-Group Info Array. */
        BCMECMP_GRP_LT_SID(unit, ecmp_id) = lt_entry->glt_sid;
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = lt_entry->lb_mode;
        BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) = lt_entry->nhop_sorted;
        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = lt_entry->max_paths;
        BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = lt_entry->num_paths;
        BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id) = lt_entry->ecmp_nhop;
        BCMECMP_GRP_RH_SIZE(unit, ecmp_id) = lt_entry->rh_size_enc;
        BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) = lt_entry->rh_entries_cnt;
        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = lt_entry->mstart_idx;

        if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)) {
            /* Allocate memory to store group underlay ECMP_ID members. */
            BCMECMP_ALLOC
                (BCMECMP_GRP_UECMP_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_GRP_UECMP_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpUecmpArr");
            BCMECMP_ALLOC
                (BCMECMP_GRP_UECMP_BK_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_GRP_UECMP_BK_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpUecmpBkArr");

            /* Copy group underlay ECMP_ID array information. */
            sal_memcpy
                (BCMECMP_GRP_UECMP_PTR(unit, ecmp_id),
                 lt_entry->uecmp_id,
                 sizeof(BCMECMP_GRP_UECMP_PTR(unit, ecmp_id)[0]) *
                        lt_entry->max_paths);
        } else {
            /* Allocate memory to store group NHOP_ID members. */
            BCMECMP_ALLOC
                (BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpNhopArr");
            BCMECMP_ALLOC
                (BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpNhopBkArr");

            /* Copy group NHOP_ID array information. */
            sal_memcpy
                (BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
                 lt_entry->nhop_id,
                 sizeof(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id)[0]) *
                        lt_entry->max_paths);
        }

        /* Increment Group ID reference count. */
        BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id) = 1;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "[ECMP_ID=%d]: Max_Paths=%u Num_Paths=%u "
                                "mstart_idx=%d ecmp_nhop=%d rh_size_enc=%u.\n"),
                    ecmp_id,
                    BCMECMP_GRP_MAX_PATHS(unit, ecmp_id),
                    BCMECMP_GRP_NUM_PATHS(unit, ecmp_id),
                    BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id),
                    BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id),
                    BCMECMP_GRP_RH_SIZE(unit, ecmp_id)));

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add: ECMP_ID=%d Ref_cnt=%d "
                              "SID[%d] != [%d].\n"),
                   ecmp_id,
                   BCMECMP_TBL_REF_CNT
                        (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id),
                   BCMECMP_GRP_LT_SID(unit, ecmp_id),
                   lt_entry->glt_sid));
    }

    exit:
        if (rh_lb_mode) {
            /* Free memory allocated for re-balance operation. */
            BCMECMP_FREE(lt_entry->rh_entries_arr);
            BCMECMP_FREE(lt_entry->rh_mindex_arr);
            BCMECMP_FREE(lt_entry->rh_memb_ecnt_arr);
        }

        /* Check if it's error return case. */
        if (SHR_FUNC_ERR() && (BCMECMP_INVALID != ecmp_id)) {
            /*
             * On error return, free memory allocated for storing group
             * next-hop members.
             */
            if (lt_entry->ecmp_nhop) {
                BCMECMP_FREE(BCMECMP_GRP_UECMP_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_UECMP_BK_PTR(unit, ecmp_id));
            } else {
                BCMECMP_FREE(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id));
            }
            /*
             * On error return, free memory allocated for managing the RH-ECMP
             * group members.
             */
            if (rh_lb_mode) {
                if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
                    if (BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));
                    }
                    if (BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                    }
                    if (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                                    ecmp_id));
                    }
                    BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
                }

                if (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                    if (BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit,
                                                                   ecmp_id));
                    }
                    if (BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit,
                                                                  ecmp_id));
                    }
                    if (BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                                      ecmp_id));
                    }
                    BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
                }
            }
        }

        SHR_FUNC_EXIT();
}

/*!
 * \brief Update installed ECMP logical table entry field values.
 *
 * Update an existing ECMP logical table entry field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
int
bcmecmp_group_update(int unit,
                     bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool rh_lb_mode = FALSE; /* Group's RH-LB_MODE status. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_update.\n")));

    /* Get ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    /* Verify the group is in use. */
    if (!BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not found.\n"), ecmp_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d in use.\n"), ecmp_id));

    /*
     * Use stored group lb_mode value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->lb_mode = BCMECMP_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LB_MODE=%d.\n"), lt_entry->lb_mode));
    }

    /*
     * Use stored group max_paths value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)) {

        /* Retrive and use group max_paths value. */
        lt_entry->max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MAX_PATHS=%u.\n"), lt_entry->max_paths));
    }

    /*
     * Use stored group nhop_sorted value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED)) {

        /* Retrive and use group nhop_sorted value. */
        lt_entry->nhop_sorted = BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NHOP_SORTED=%d.\n"), lt_entry->nhop_sorted));
    }

    /*
     * Use stored group num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)) {

        /* Retrive and use group num_paths value. */
        lt_entry->num_paths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NUM_PATHS=%u.\n"), lt_entry->num_paths));
    }

    /*
     * Use stored group ecmp_nhop value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)) {

        /*
         * Retrive and use group ecmp_nhop value. This field can be TRUE only
         * for Overlay ECMP Groups that have Underlay ECMP Group/s as Next-hops.
         */
        lt_entry->ecmp_nhop = BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_NHOP=%d.\n"), lt_entry->ecmp_nhop));
    }

    /*
     * Use stored group rh_size_enc value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)) {

        /* Retrive and use group rh_size_enc value. */
        lt_entry->rh_size_enc = BCMECMP_GRP_RH_SIZE(unit, ecmp_id);
        lt_entry->rh_entries_cnt = BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_SIZE[CNT=%u]=%u.\n"),
                     lt_entry->rh_entries_cnt,
                     lt_entry->rh_size_enc));
    }

    /* Check and update NHOP_ID[] field values if it's not specified. */
    SHR_IF_ERR_EXIT(bcmecmp_group_update_member_fields_set(unit, lt_entry));

    /* Sort the nhop array elements if it's been requested. */
    if (lt_entry->nhop_sorted) {
        SHR_IF_ERR_EXIT(bcmecmp_group_nhops_sort(unit, lt_entry));
    }

    /* Initialize Start index value used by hardware write operation. */
    lt_entry->mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX
                                (unit, ecmp_id);

    /*
     * Get group's load balancing mode for performing the correct delete
     * function call and member table reference count decrement operations.
     */
    rh_lb_mode = (BCMECMP_LB_MODE_RESILIENT == BCMECMP_GRP_LB_MODE
                                                    (unit, ecmp_id));

    if (rh_lb_mode) {
        /*
         * Allocate memory to store the RH balanced nhop members for
         * installation in hardware tables.
         */
        BCMECMP_ALLOC
            (lt_entry->rh_entries_arr,
             lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_entries_arr)),
             "bcmecmpRhGrpUpdtLtEntRhEntArr");

        /*
         * Allocate memory to store the RH group entry's mapped member index
         * data.
         */
        BCMECMP_ALLOC
            (lt_entry->rh_mindex_arr,
             lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_mindex_arr)),
             "bcmecmpRhGrpUpdtLtEntRhMindexArr");

        /*
         * Allocate memory to store the RH group members entries usage count
         * data.
         */
        BCMECMP_ALLOC
            (lt_entry->rh_memb_ecnt_arr,
             lt_entry->max_paths * sizeof(*(lt_entry->rh_memb_ecnt_arr)),
             "bcmecmpRhGrpUpdtLtEntRhMembEntCntArr");

        /*
         * If NUM_PATHS value for this RH-group was zero prior to this update
         * operation and valid next-hop members are being added to this RH-group
         * in this UPDATE operation i.e. NUM_PATHS > 0, then this UPDATE
         * operation is equivalent to new next-hop members INSERT operation for
         * this group. So, use RH entries SET function for load balance
         * operation.
         */
        if (BCMECMP_LT_FIELD_GET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
                && lt_entry->num_paths
                && !BCMECMP_GRP_NUM_PATHS(unit, ecmp_id)) {
            /* load balance the RH next-hop members. */
            SHR_IF_ERR_EXIT
                (bcmecmp_group_rh_entries_set(unit, lt_entry));
        } else {

            /*
             * This is an existing RH group members Add/Delete/Replace type of
             * operation. So, use RH entries UPDATE function to perform the
             * rebalancing work.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_group_rh_entries_update(unit, lt_entry));
        }

        /* Install RH-ECMP Group in hardware tables. */
        SHR_IF_ERR_EXIT(BCMECMP_FNCALL_EXEC(unit, rh_grp_ins)(unit, lt_entry));

        /* Copy updated group load balanced RH member entries array. */
        sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                   lt_entry->rh_entries_arr,
                   (sizeof(*BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id)) *
                          lt_entry->rh_entries_cnt));

        /* Copy updated group entries mapped member index array. */
        sal_memcpy(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   lt_entry->rh_mindex_arr,
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                          lt_entry->rh_entries_cnt));

        /* Copy updated members entry usage count array. */
        sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   lt_entry->rh_memb_ecnt_arr,
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) *
                    lt_entry->num_paths));
    } else {
        /* Install ECMP Group in hardware tables. */
        SHR_IF_ERR_EXIT(BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));
    }

    /*
     * Compare new num_paths value with stored num_paths value
     * and update stored value if it has changed.
     */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) != lt_entry->num_paths) {
        BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = lt_entry->num_paths;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[EID=%d]: Max_Paths=%u Num_Paths=%u.\n"),
                ecmp_id,
                BCMECMP_GRP_MAX_PATHS(unit, ecmp_id),
                BCMECMP_GRP_NUM_PATHS(unit, ecmp_id)));

    /*
     * Store updated group next-hop members information based on the next-hop
     * members type.
     */
    if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)) {
        sal_memcpy
            (BCMECMP_GRP_UECMP_PTR(unit, ecmp_id),
             lt_entry->uecmp_id,
             sizeof(*(BCMECMP_GRP_UECMP_PTR(unit, ecmp_id))) *
                    lt_entry->max_paths);
    } else {
        sal_memcpy
            (BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
             lt_entry->nhop_id,
             sizeof(*(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id))) *
                    lt_entry->max_paths);
    }

    exit:
        /* Free memory allocated for RH group re-balance operation. */
        if (rh_lb_mode) {

            /* RH-Group data print debug function. */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d"),
                        bcmecmp_rh_grp_data_debug_print(unit, ecmp_id,
                                                        lt_entry)));

            /* Free temp RH group entries array. */
            BCMECMP_FREE(lt_entry->rh_entries_arr);

            /* Free temp RH group entry mapped member index array. */
            BCMECMP_FREE(lt_entry->rh_mindex_arr);

            /* Free temp RH group member entries usage count array. */
            BCMECMP_FREE(lt_entry->rh_memb_ecnt_arr);
        }

        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an ECMP Group logical table entry from hardware tables.
 *
 * Delete an ECMP Group logical table entry from hardware tables. ECMP member
 * table resources consumed by the group are returned to the free pool as part
 * of this delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
int
bcmecmp_group_delete(int unit,
                     bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool rh_lb_mode = FALSE; /* Group's RH-LB_MODE status. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_delete.\n")));

    /* Get ECMP ID value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    /* Check and confirm ECMP_ID exists prior to delete operation. */
    if (!BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not found.\n"), ecmp_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Get group's load balancing mode for performing the correct delete
     * function call and member table reference count decrement operations.
     */
    rh_lb_mode = (BCMECMP_LB_MODE_RESILIENT == BCMECMP_GRP_LB_MODE
                                                    (unit, ecmp_id));

    if (rh_lb_mode) {
        /* Delete RH-ECMP Group from hardware tables. */
        SHR_IF_ERR_EXIT
            (BCMECMP_FNCALL_EXEC(unit, rh_grp_del)(unit, lt_entry));

        /*
         * Decrement Member table entries reference count by group's RH_SIZE
         * i.e. rh_entries_cnt value.
         */
        SHR_IF_ERR_EXIT
            (bcmecmp_tbl_ref_cnt_decr
                (BCMECMP_TBL_PTR(unit, member, gtype),
                 BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id),
                 BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id)));

        /*
         * Free memory allocated for storing per RH entry mapped member's index
         * information.
         */
        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));

        /* Free memory allocated for storing RH load balanced group entries. */
        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));

        /*
         * Free memory allocated for storing per member RH entries usage
         * count.
         */
        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id));

        /* Free memory allocated for storing group's RH info. */
        BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
    } else {
        /* Delete ECMP Group from hardware tables. */
        SHR_IF_ERR_EXIT
            (BCMECMP_FNCALL_EXEC(unit, grp_del)(unit, lt_entry));

        /* Decrement Member table entries reference count. */
        SHR_IF_ERR_EXIT
            (bcmecmp_tbl_ref_cnt_decr
                (BCMECMP_TBL_PTR(unit, member, gtype),
                 BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id),
                 BCMECMP_GRP_MAX_PATHS(unit, ecmp_id)));
    }

    /* Decrement Group ID reference count. */
    BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id) = 0;

    /*
     * Check if next-hop members are Underlay ECMP_ID type next-hops or regular
     * next-hops and free memory allocated for storing these next-hops.
     */
    if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)) {
        /* Free memory allocated for storing underlay ECMP NHOPs. */
        BCMECMP_FREE(BCMECMP_GRP_UECMP_PTR(unit, ecmp_id));
    } else {
        /* Free memory allocated for storing regular ECMP NHOPs. */
        BCMECMP_FREE(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id));
    }

    /* Clear/initialize ECMP Group information in Per-Group Info Array. */
    BCMECMP_GRP_LT_SID(unit, ecmp_id) = BCMLTD_SID_INVALID;
    BCMECMP_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_LB_MODE_REGULAR;
    BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) = FALSE;
    BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = BCMECMP_LT_MAX_PATHS
                                                        (unit, group, gtype);
    BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = 0;
    BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id) = FALSE;
    BCMECMP_GRP_RH_SIZE(unit, ecmp_id) = 0;
    BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) = 0;
    BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = BCMECMP_INVALID;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Get an ECMP Group logical table entry data from hardware tables.
 *
 * Get an ECMP Group logical table entry data from hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
int
bcmecmp_group_get(int unit,
                  bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_get.\n")));

    /* Get ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    if (!BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not found.\n"), ecmp_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d in use.\n"), ecmp_id));

    if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)) {
        /* Copy group Underlay ECMP_ID array information. */
        sal_memcpy
            (lt_entry->uecmp_id,
             BCMECMP_GRP_UECMP_PTR(unit, ecmp_id),
             sizeof(lt_entry->uecmp_id[0]) * BCMECMP_GRP_MAX_PATHS
                                                    (unit, ecmp_id));
    } else {
        /* Copy group NHOP_ID array information. */
        sal_memcpy
            (lt_entry->nhop_id,
             BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
             sizeof(lt_entry->nhop_id[0]) * BCMECMP_GRP_MAX_PATHS
                                                    (unit, ecmp_id));
    }

    lt_entry->lb_mode = BCMECMP_GRP_LB_MODE(unit, ecmp_id);
    lt_entry->num_paths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    lt_entry->ecmp_nhop = BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id);
    lt_entry->max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);
    lt_entry->nhop_sorted = BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id);
    lt_entry->rh_size_enc = BCMECMP_GRP_RH_SIZE(unit, ecmp_id);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[ECMP_ID=%d]: Max_Paths=%u Num_Paths=%u"
                            " LB_MODE=%d SORTED=%d RH_SIZE=%u.\n"),
                lt_entry->ecmp_id,
                lt_entry->max_paths,
                lt_entry->num_paths,
                lt_entry->lb_mode,
                lt_entry->nhop_sorted,
                lt_entry->rh_size_enc));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse specified ECMP logical table and find inserted entries.
 *
 * This function traverses specified ECMP logical table, finds and returns the
 * first valid entry when Get-First operation is performed. For Get-Next
 * operation, it starts searching for next valid entry after the given ECMP_ID.
 * When an entry is found, it returns entry's data field values.
 *
 * When no valid entries are found for Get-First API call this function returns
 * SHR_E_EMPTY error code. For Get-Next API call when no entries are found
 * after the specified entry, it returns SHR_E_NOT_FOUND error code.
 *
 * Get-First vs Get-Next API call is indicated via the 'first' boolean input
 * parameter variable of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] first Indicates get first logical table operation.
 * \param [out] lt_entry Pointer to ECMP logical table entry.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_EMPTY No entries in the logical table.
 * \return SHR_E_NOT_FOUND No entries found in the logical table.
 */
int
bcmecmp_group_find(int unit,
                   bool first,
                   bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_find.\n")));

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    /* Initialize ecmp_id value based on get first or get next operation. */
    for (ecmp_id = (first ? BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype)
                                : (lt_entry->ecmp_id));
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
         ecmp_id++) {
        /*
         * Skip ECMP groups that are not in use or groups that do not match the
         * expected LT SID value.
         */
        if (!BCMECMP_TBL_REF_CNT
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)
            || (BCMECMP_GRP_LT_SID(unit, ecmp_id) != lt_entry->glt_sid)) {

            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Skipped: ECMP_ID=%d Ref_cnt=%d "
                                  "SID[%d] != [%d].\n"),
                       ecmp_id,
                       BCMECMP_TBL_REF_CNT
                            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id),
                       BCMECMP_GRP_LT_SID(unit, ecmp_id),
                       lt_entry->glt_sid));

            continue;
        }

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Found: ECMP_ID=%d Ref_cnt=%d "
                              "SID[%d] != [%d].\n"),
                   ecmp_id,
                   BCMECMP_TBL_REF_CNT
                        (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id),
                   BCMECMP_GRP_LT_SID(unit, ecmp_id),
                   lt_entry->glt_sid));

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d found.\n"), ecmp_id));

        /* Update LT entry ECMP_ID value to be used for Get operation. */
        lt_entry->ecmp_id = ecmp_id;

        /* Retrieve LT entry information. */
        SHR_RETURN_VAL_EXIT(bcmecmp_group_get(unit, lt_entry));
    }

    /*
     * Return entry not found error if no additional entries are found for
     * Get Next request.
     */
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Free memory allocated to store group next-hop members.
 *
 * Free memory allocated to store group next-hop members.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
int
bcmecmp_groups_cleanup(int unit)
{
    bcmecmp_id_t ecmp_id; /* Group identifier. */
    bcmecmp_grp_type_t gtype; /* Group type. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_groups_cleanup.\n")));

    /*
     * Check and perform groups clean-up, only when ECMP feature has been
     * initialized successfuly for this BCM unit.
     */
    if (!(BCMECMP_CTRL(unit)->init)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (gtype = BCMECMP_GRP_TYPE_SINGLE;
            gtype < BCMECMP_GRP_TYPE_COUNT; gtype++ ) {
        if (gtype > BCMECMP_GRP_TYPE_SINGLE
                && BCMECMP_MODE(unit) != BCMECMP_MODE_HIER) {
            break;
        }

        /*
         * Check and skip invalid ECMP group types as no groups can exist at
         * these uninitialized ECMP group levels.
         */
        if (BCMECMP_TBL_LTD_SID(unit, group, gtype) == BCMLTD_SID_INVALID) {
            continue;
        }

        for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype);
             ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
             ecmp_id++) {
            if (BCMECMP_TBL_REF_CNT
                    (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
                if (BCMECMP_LB_MODE_RESILIENT == BCMECMP_GRP_LB_MODE
                                                        (unit, ecmp_id)) {
                    /* Free memory allocated for RH group info. */
                    if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                                    ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
                    }

                    /* Free memory allocated for RH group backup info. */
                    if (BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit,
                                                                   ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit,
                                                                  ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                                  ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
                    }
                }

                if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)) {
                    /* Free memory allocated for storing underlay ECMP NHOPs. */
                    BCMECMP_FREE(BCMECMP_GRP_UECMP_PTR(unit, ecmp_id));
                    if (BCMECMP_GRP_UECMP_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_UECMP_BK_PTR(unit, ecmp_id));
                    }
                } else {
                    /* Free memory allocated for storing regular ECMP NHOPs. */
                    BCMECMP_FREE(BCMECMP_GRP_NHOP_PTR(unit, ecmp_id));
                    if (BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_NHOP_BK_PTR(unit, ecmp_id));
                    }
                }

                /* Clear group reference count value. */
                BCMECMP_TBL_REF_CNT
                    (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id) = 0;
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
}
