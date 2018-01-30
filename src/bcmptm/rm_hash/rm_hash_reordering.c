/*! \file rm_hash_reordering.c
 *
 * Non-recurrsive Hash Reordering for Hash Table Resource management
 *
 * This file contains the resource manager for physical hash tables
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "rm_hash_internal.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH
#define RM_HASH_MAX_DST_BUCKET_NUM ((RM_HASH_MAX_BANK_COUNT - 1) * RM_HASH_MAX_ENTRY_NUM_PER_BUCKET)
#define RM_HASH_ENTRY_MOVE_OPER         1
#define RM_HASH_ENTRY_MOVE_LIST_DEF_NUM 128
#define RM_HASH_ENTRY_MOVE_LIST_INC_NUM 100

/*******************************************************************************
 * Typedefs
 */
/*!
 * \brief Information about a potential entry move item
 *
 * This data structure describes the sequence number of an entry and
 * its SLB (software logical bucket), and the valid base bucket bitmap in
 * the SLB.
 */
typedef struct rm_hash_ptnl_move_info_s {
    /*! \brief Sequence number in the entry node list of a slb */
    uint8_t e_seq;

    /*! \brief SLB info */
    rm_hash_ent_slb_info_t slb_info;
} rm_hash_ptnl_move_info_t;

/*!
 * \brief Information about an entry which is to be moved
 *
 * This data structure describes an entry which is to be moved among its valid
 * bucket. It contains relevant attributes of an entry.
 * To be more specific, it contains the bucket mode of the entry, the base bucket
 * bitmap indicating which base bucket this entry resides currently, the base
 * entry and logical entry bitmap within the base bucket. And it also contains
 * the size of the entry and sid of the entry.
 */
typedef struct rm_hash_move_ent_info_s {
    /*! \brief Bucket mode of the entry */
    rm_hash_bm_t bm;

    /*! \brief Location of the entry */
    rm_hash_ent_loc_t e_loc;

    /*! \brief size of the entry */
    uint8_t e_size;

    /*! \brief sid of the entry */
    bcmdrd_sid_t sid;
} rm_hash_move_ent_info_t;

/*!
 * \brief Information about an entry move operation.
 *
 * This data structure describes an entry move operation. For example
 * the size of the moving entry, the bucket mode of the entry, etc.
 */
typedef struct rm_hash_ent_move_op_s {
    /*! \brief SID of the entry to be moved */
    bcmdrd_sid_t sid;

    /*! \brief Number of base entries used by the entry to be moved */
    uint8_t e_size;

    /*! \brief Entry bucket mode */
    rm_hash_bm_t e_bm;

    /*! \brief Source SLB where the entry will be moved from */
    uint32_t s_slb;

    /*! \brief Destination SLB where the entry will be moved to */
    uint32_t d_slb;

    /*! \brief Source entry location info */
    rm_hash_ent_loc_t se_loc;

    /*! \brief Destination entry location info */
    rm_hash_ent_loc_t de_loc;

    /*! \brief Indicates if the move really provide the room to accomdate the entry */
    bool slot_provider;
} rm_hash_ent_move_op_t;

/*!
 * \brief Information about an entry move operation
 *
 * This data structure describes the elements in a stack frame. A stack frame
 * must correspond to a SLB.
 */
typedef struct rm_hash_ent_move_stack_s {
    /*! \brief The bkt that needs to be analyzed. Either it can provide some free
     * space to accomadate the moved entry coming from the last stack frame, or
     * it will have to select one of its entry to move outside.
     */
    uint32_t slb;

    /*! \brief Info about requested entry */
    rm_hash_req_ent_info_t req_e_info;

    /*! \brief Number of eligible potential entry move operations */
    uint8_t num_epm;

    /*! \brief Eligible potential entry move operations list */
    rm_hash_ptnl_move_info_t epm_info[RM_HASH_MAX_DST_BUCKET_NUM];

    /*! \brief Index of next alternative entry move operation */
    uint8_t curr_epm_idx;

    /*! \brief Actual SLB entry bitmap plus applying the pending move ops */
    rm_hash_slb_ent_bmp_t e_bmp;

    /*! \brief The info regarding the entry to be moved out */
    rm_hash_move_ent_info_t move_ent;

    /*! \brief Indicates if this bkt actually provide the space eventually. */
    bool  slot_provider;
} rm_hash_ent_move_stack_t;

/*!
 * \brief Information about entry move operation list
 *
 * This data structure describes the information about the entry move info list.
 */
typedef struct rm_hash_ent_move_op_list_s {
    /*! \brief Indicates the start address of the entry move info list. */
    rm_hash_ent_move_op_t *e_move_op;

    /*! \brief Indicates the count of entry move info currently. */
    uint32_t count;
} rm_hash_ent_move_op_list_t;

/*******************************************************************************
 * Private variables
 */
static const char bmp_string[16][5] = {"0000", "0001", "0010", "0011",
                                       "0100", "0101", "0110", "0111",
                                       "1000", "1001", "1010", "1011",
                                       "1100", "1101", "1110", "1111"};

static rm_hash_ent_move_op_list_t e_move_op_list[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t e_move_num[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_ent_size_map_t e_size_map [16] = {
    {FALSE, 0x00}, /* 0000 */
    {TRUE,  0x01}, /* 0001 */
    {TRUE,  0x01}, /* 0010 */
    {TRUE,  0x02}, /* 0011 */
    {TRUE,  0x01}, /* 0100 */
    {FALSE, 0x00}, /* 0101 */
    {FALSE, 0x00}, /* 0110 */
    {FALSE, 0x00}, /* 0111 */
    {TRUE,  0x01}, /* 1000 */
    {FALSE, 0x00}, /* 1001 */
    {FALSE, 0x00}, /* 1010 */
    {FALSE, 0x00}, /* 1011 */
    {TRUE,  0x02}, /* 1100 */
    {FALSE, 0x00}, /* 1101 */
    {FALSE, 0x00}, /* 1110 */
    {TRUE,  0x04}, /* 1111 */
};
/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Save the information about an entry move operation into the array of the
 * device.
 *
 * \param [in] unit Unit number.
 * \param [in] src_bkt Source bucket of the entry move.
 * \param [in] sent_bmp Source entry bitmap of the entry move.
 * \param [in] soffset Source entry offset of the entry move.
 * \param [in] dst_bkt Destnation bucket of the entry move.
 * \param [in] dent_bmp Destnation entry bitmap of the entry move.
 * \param [in] doffset Destnation entry offset of the entry move.
 * \param [in] ent_size Entry size of the entry to be moved.
 * \param [in] slot_provider Flag indicating if this move provides the real
 *             space.
 * \param [in] sid Physical table sid of the entry to be moved.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_op_save(int unit,
                         uint32_t s_slb,
                         rm_hash_ent_loc_t se_loc,
                         uint32_t d_slb,
                         rm_hash_ent_loc_t de_loc,
                         uint8_t e_size,
                         rm_hash_bm_t e_bm,
                         bcmdrd_sid_t sid,
                         bool slot_provider)
{
    uint32_t move_no = e_move_num[unit];
    uint32_t list_len = e_move_op_list[unit].count;
    rm_hash_ent_move_op_t *move_op = NULL;
    size_t info_size = 0;

    SHR_FUNC_ENTER(unit);

    if (move_no >= list_len) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rm_hash_entry_move_list for unit %d is full.\n"),
                                unit));
        /*
         * There is no free space in the existing move info list.
         * The extra space is required.
         */
        info_size = sizeof(rm_hash_ent_move_op_t) *
                    (list_len + RM_HASH_ENTRY_MOVE_LIST_INC_NUM);
        move_op = sal_alloc(info_size, "entry_move info");
        if (move_op == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
        /*
         * Copy existing entry move info from the existing list to the
         * new one.
         */
        sal_memcpy(move_op, e_move_op_list[unit].e_move_op,
                   sizeof(rm_hash_ent_move_op_t) * list_len);
        sal_free(e_move_op_list[unit].e_move_op);
        e_move_op_list[unit].e_move_op = move_op;
        e_move_op_list[unit].count += RM_HASH_ENTRY_MOVE_LIST_INC_NUM;
    }
    move_op = e_move_op_list[unit].e_move_op + move_no;
    move_op->s_slb = s_slb;
    move_op->se_loc = se_loc;
    move_op->d_slb = d_slb;
    move_op->de_loc = de_loc;
    move_op->slot_provider = slot_provider;
    move_op->e_size = e_size;
    move_op->e_bm = e_bm;
    move_op->sid = sid;
    e_move_num[unit]++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry move information from the pending move list.
 *
 * \param [in] unit Unit number.
 * \param [in] move_no Index into the pending move list.
 * \param [out] pmove_info Pointer to the buffer for recording the address
 *              of rm_hash_ent_move_info_t object.
 *
 * \retval TRUE The entry move information has been got successfully.
 * \retval FALSE The entry move information has not been got.
 */
static bool
rm_hash_ent_move_op_get(int unit,
                        uint32_t move_no,
                        rm_hash_ent_move_op_t **pmove_op)
{
    if (move_no >= e_move_num[unit]) {
        return FALSE;
    }
    *pmove_op = &e_move_op_list[unit].e_move_op[move_no];
    return TRUE;
}

/*!
 * \brief Get the number of entry move operations stored in the operation list.
 *
 * \param [in] unit Unit number.
 *
 * \retval The number of pending entry move
 */
static uint32_t
rm_hash_ent_move_num_get(int unit)
{
    return e_move_num[unit];
}

/*!
 * \brief Clear the entry move info list.
 *
 * \param [in] unit Unit number.
 */
static void
rm_hash_ent_move_info_clear(int unit)
{
    sal_memset(e_move_op_list[unit].e_move_op,
               0,
               sizeof(rm_hash_ent_move_op_t) *
               e_move_op_list[unit].count);
    e_move_num[unit] = 0;
}

/*!
 * \brief Dump the details of an entry move.
 *
 * \param [in] unit Unit number.
 */
static void
rm_hash_ent_move_op_dump(int unit, rm_hash_ent_move_op_t *move_op)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "move entry sid[%s] \n"
                            "move entry size[%d] \n"
                            "move entry bucket mode[%d] \n"
                            "move entry source slb[%d] \n"
                            "move entry destination slb[%d] \n"
                            "move entry source base bucket bmp[%s] \n"
                            "move entry destination base bucket bmp[%s] \n"),
                bcmdrd_pt_sid_to_name(unit, move_op->sid),
                move_op->e_size,
                move_op->e_bm,
                move_op->s_slb,
                move_op->d_slb,
                bmp_string[move_op->se_loc.bb_bmp],
                bmp_string[move_op->de_loc.bb_bmp]));
}

/*!
 * \brief Clear the entry move operation list.
 *
 * \param [in] unit Unit number.
 * \param [in] bkt_id Logical bucket index.
 * \param [in] depth The depth of hash reordering.
 */
static void
rm_hash_ent_move_trace_clear(int unit, uint32_t slb, int depth)
{
    int idx = 0;
    rm_hash_ent_move_op_t *move_op = NULL, *prev_move_op = NULL;

    if (e_move_num[unit] == 0) {
        return;
    }
    idx = e_move_num[unit] - 1;
    move_op = &e_move_op_list[unit].e_move_op[idx];
    if (move_op->s_slb != slb) {
        return;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Need to clear move path of SLB[%d], "
                            "operation number [%d]\n"),
                 slb, idx));
    for (; idx > 0; idx--) {
        move_op = &e_move_op_list[unit].e_move_op[idx];
        prev_move_op = &e_move_op_list[unit].e_move_op[idx - 1];
        if (move_op->slot_provider == TRUE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Clear movement operation[%d] "
                                    "source bkt[%d], destination bkt[%d]\n"),
                         idx, move_op->s_slb, move_op->d_slb));
            sal_memset(move_op, 0, sizeof(rm_hash_ent_move_op_t));
            if (prev_move_op->s_slb != slb) {
                break;
            } else {
                continue;
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Clear movement operation[%d] source bkt[%d], "
                                "destination bkt[%d]\n"),
                     idx, move_op->s_slb, move_op->d_slb));
        sal_memset(move_op, 0, sizeof(rm_hash_ent_move_op_t));
    }
    e_move_num[unit] = idx;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "push iteration depth [%d]: all destination bkts "
                            "of slb [%d] were traversed\n"),
                 depth, slb));
}


/*!
 * \brief Filter the invalid destination bkts from the destination bkt list,
 *  and save all the valid destination bkts into the stack.
 *
 * \param [in] unit Unit number.
 * \param [in] orig_alt_info Original alternative entry move info.
 * \param [in] num_emc Number of alternative entry move info.
 * \param [in] depth Depth of stack.
 * \param [out] stack_base Stack of the entry move.
 */
static void
rm_hash_ptnl_move_info_filter(int unit,
                              rm_hash_ptnl_move_info_t *ptnl_info,
                              int num_epm,
                              int depth,
                              rm_hash_ent_move_stack_t *stack_base)
{
    int layer = 0, epm_idx;
    uint32_t slb = 0;
    rm_hash_ptnl_move_info_t *epm_info = NULL;
    uint8_t idx = 0;

    epm_info = stack_base[depth].epm_info;
    for (epm_idx = 0; epm_idx < num_epm; epm_idx++) {
        slb = ptnl_info[epm_idx].slb_info.slb;
        for (layer = 0; layer < depth; layer++) {
            if (slb == stack_base[layer].slb) {
                break;
            }
        }
        if (layer == depth) {
            /*
             * If this SLB doesn't appear in the stack, save it and
             * its entry id onto the top of the stack.
             */
            epm_info[idx].e_seq = ptnl_info[epm_idx].e_seq;
            epm_info[idx].slb_info = ptnl_info[epm_idx].slb_info;
            idx++;
        }
    }
    stack_base[depth].num_epm = idx;
}



/*!
 * \brief Allocate a fresh new stack frame for a bucket.
 *
 * Essentially, a specified bucket need to provide some free space to hold
 * an entry. This entry has the size specified via ent_size.
 * When a frame is to be allocated, we will save the following items into the
 * stack: bucket id, the free space this bucket needs to provide, candidate
 * entry move <entry, bucket> tuple list, and the total number of elements in
 * the list.

 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to lt control structure of the LT.
 * \param [in] lbkt Logical bucket.
 * \param [in] depth Depth of entry move stack.
 * \param [out] stack_base Stack of entry move.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_move_stk_frm_alloc(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint32_t slb,
                           rm_hash_req_ent_info_t e_info,
                           int frm_idx,
                           rm_hash_ent_move_stack_t *stack_base)
{
    rm_hash_slb_state_t *b_state = NULL;
    uint32_t e_node_offset = 0;
    rm_hash_dual_ent_node_t *de_node = NULL;
    rm_hash_shr_ent_node_t *se_node = NULL;
    uint8_t lbank = 0, num_epm = 0, e_idx = 0;
    uint8_t num_lbanks = pt_info->num_lbanks;
    rm_hash_ptnl_move_info_t pre_filter_info[RM_HASH_MAX_DST_BUCKET_NUM];

    SHR_FUNC_ENTER(unit);

    b_state = pt_info->b_state + slb;
    e_node_offset = b_state->e_node_offset;
    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_get(unit,
                                              pt_info,
                                              e_node_offset,
                                              &de_node));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_get(unit,
                                             pt_info,
                                             e_node_offset,
                                             &se_node));
    }
    stack_base[frm_idx].slb = slb;
    stack_base[frm_idx].req_e_info = e_info;
    stack_base[frm_idx].num_epm = 0;
    /*
     * slb is the software logical bucket to be analyzed. First of all
     * it should be checked if it really contains valid entries.
     */
    if (b_state->ve_cnt == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /*
     * A slb may contain entries of multiple bucket modes. For example,
     * a BM2 slb may contain bm0, bm1 and bm2 entries. So we need to
     * figure out which entries are helpful to accomadate the requested
     * entry with a specific bucket mode.
     */
    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        for (e_idx = 0; e_idx < b_state->ve_cnt; e_idx++) {
            if (de_node->e_loc.bb_bmp & e_info.vbb_bmp) {
                for (lbank = 0; lbank < num_lbanks; lbank++) {
                    if (de_node->slb_info[lbank].slb == slb) {
                        /*
                         * Currently, the entry resides on the specified slb, so the slb
                         * is not a valid move destination, igore it.
                         */
                        continue;
                    }
                    /*
                     * The entry occupies some space in the base bucket. So it is possible
                     * that if we move the entry into another bank, this may help to
                     * accomadate the requested entry.
                     * We will record the following info:
                     * 1. The sequence number of entry in the slb entry node list
                     * 2. The slb and vbb bitmap on other banks.
                     */
                    pre_filter_info[num_epm].e_seq = e_idx;
                    pre_filter_info[num_epm].slb_info = de_node->slb_info[lbank];
                    num_epm++;
                }
            }
            de_node++;
        }
    } else {
        for (e_idx = 0; e_idx < b_state->ve_cnt; e_idx++) {
            if (se_node->e_loc.bb_bmp & e_info.vbb_bmp) {
                for (lbank = 0; lbank < num_lbanks; lbank++) {
                    if (se_node->slb_info[lbank].slb == slb) {
                        /*
                         * Currently, the entry resides on the specified slb, so the slb
                         * is not a valid move destination, igore it.
                         */
                        continue;
                    }
                    /*
                     * The entry occupies some space in the base bucket. So it is possible
                     * that if we move the entry into another bank, this may help to
                     * accomadate the requested entry.
                     * We will record the following info:
                     * 1. The sequence number of entry in the slb entry node list
                     * 2. The slb and vbb bitmap on other banks.
                     */
                    pre_filter_info[num_epm].e_seq   = e_idx;
                    pre_filter_info[num_epm].slb_info = se_node->slb_info[lbank];
                    num_epm++;
                }
            }
            se_node++;
        }
    }

    rm_hash_ptnl_move_info_filter(unit,
                                  pre_filter_info,
                                  num_epm,
                                  frm_idx,
                                  stack_base);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Save the information of the entry that is selected to be moved out
 * into the stack frame.
 *
 * Essentially, a specified bucket will have multiple candidate entry move
 * <entry, bucket> tuple list. At a given time, one entry in the bucket will
 * be selected to move out. Correspondingly, the information about the selected
 * entry needs to recorded into the stack frame.
 */
static void
rm_hash_move_ent_info_set(rm_hash_ent_move_stack_t *stack,
                          rm_hash_move_ent_info_t *me_info)
{
    stack->move_ent = *me_info;
}

/*!
 * \brief Compute empty bucket bitmap for a bucket if it appears in
 * the pending list.
 *
 * \param [in] unit Unit number.
 * \param [in] bkt_id logical bucket index.
 * \param [in] bkt_size logical bucket size.
 * \param [out] empty_bmp Empty entry bitmap of the logical bucket.
 * \param [out] src_bmp Source base entries to be moved.
 */
static int
rm_hash_slb_ent_bmp_update(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint32_t slb,
                           rm_hash_slb_ent_bmp_t *slb_e_bmp,
                           rm_hash_slb_ent_bmp_t *src_e_bmp)
{
    uint32_t idx;
    rm_hash_ent_move_op_t *move_op = NULL;
    uint16_t be_bmp = 0, le_bmp = 0;
    uint32_t move_num = rm_hash_ent_move_num_get(unit);

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < move_num; idx++) {
        (void)rm_hash_ent_move_op_get(unit, idx, &move_op);
        if (slb == move_op->d_slb) {
            /*
             * slb appears as the destination of an entry move operation.
             * the base entry and logical entry bitmap from the slb point
             * of view need to be reflected as well.
             * First, get the base entry bitmap of the "comming in" entry.
             * Second, get the logical entry bitmap and update these bitmaps
             * into slb state.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &move_op->de_loc,
                                                        &be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &move_op->de_loc,
                                                        &le_bmp));
            /* The be and le info need to be updated into slb state */
            slb_e_bmp->be_bmp |= be_bmp;
            slb_e_bmp->le_bmp[move_op->e_bm] |= le_bmp;
        }
        if (slb == move_op->s_slb) {
            /*
             * slb appears as the source of an entry move operation.
             * the base entry and logical entry bitmap change due to
             * the entry moving out should be reflected in slb state as
             * well.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &move_op->se_loc,
                                                        &be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &move_op->se_loc,
                                                        &le_bmp));
            slb_e_bmp->be_bmp &= ~be_bmp;
            slb_e_bmp->le_bmp[move_op->e_bm] &= ~le_bmp;
            /*
             * Record be and le bitmap of all the moved out entries
             * from slb point of view.
             */
            if (src_e_bmp != NULL) {
                src_e_bmp->be_bmp |= be_bmp;
                src_e_bmp->le_bmp[move_op->e_bm] |= le_bmp;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compute how many entry have been analyzed
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb logical bucket
 * \param [out] ent_cnt Entry count
 */
static int
rm_hash_analyzed_ment_cnt_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              uint32_t slb,
                              uint8_t *ent_cnt)
{
    uint8_t e_seq = 0;
    rm_hash_slb_state_t *b_state = NULL;
    uint32_t e_node_offset = 0;
    uint16_t be_bmp = 0, le_bmp = 0;
    rm_hash_dual_ent_node_t *de_node = NULL;
    rm_hash_shr_ent_node_t *se_node = NULL;
    rm_hash_slb_ent_bmp_t slb_e_bmp;
    rm_hash_slb_ent_bmp_t src_e_bmp;

    SHR_FUNC_ENTER(unit);

    sal_memset(&slb_e_bmp, 0, sizeof(slb_e_bmp));
    sal_memset(&src_e_bmp, 0, sizeof(src_e_bmp));

    b_state = pt_info->b_state + slb;
    e_node_offset = b_state->e_node_offset;

    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_get(unit,
                                              pt_info,
                                              e_node_offset,
                                              &de_node));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_get(unit,
                                             pt_info,
                                             e_node_offset,
                                             &se_node));
    }

    slb_e_bmp = b_state->e_bmp;
    SHR_IF_ERR_EXIT
        (rm_hash_slb_ent_bmp_update(unit,
                                    pt_info,
                                    slb,
                                    &slb_e_bmp,
                                    &src_e_bmp));

    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        while ((e_seq < b_state->ve_cnt) && (src_e_bmp.be_bmp != 0)) {
            /*
             * There may be multiple entries with multiple BM. For each entry,
             * we need to determine if it appears in the pending move list.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &de_node->e_loc,
                                                        &be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &de_node->e_loc,
                                                        &le_bmp));
            /*
             * be and le for this entry node has been retrieved.
             * Check if this entry node has been put into pending move list.
             */
            if (((be_bmp & src_e_bmp.be_bmp) == be_bmp) &&
                ((le_bmp & src_e_bmp.le_bmp[de_node->bm]) == le_bmp)) {
                src_e_bmp.be_bmp &= ~(be_bmp);
                src_e_bmp.le_bmp[de_node->bm] &= ~(le_bmp);
            }
            de_node++;
            e_seq++;
        }
    } else {
        while ((e_seq < b_state->ve_cnt) && (src_e_bmp.be_bmp != 0)) {
            /*
             * There may be multiple entries with multiple BM. For each entry,
             * we need to determine if it appears in the pending move list.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &se_node->e_loc,
                                                        &be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &se_node->e_loc,
                                                        &le_bmp));
            /*
             * be and le for this entry node has been retrieved.
             * Check if this entry node has been put into pending move list.
             */
            if (((be_bmp & src_e_bmp.be_bmp) == be_bmp) &&
                ((le_bmp & src_e_bmp.le_bmp[se_node->bm]) == le_bmp)) {
                src_e_bmp.be_bmp &= ~(be_bmp);
                src_e_bmp.le_bmp[se_node->bm] &= ~(le_bmp);
            }
            se_node++;
            e_seq++;
        }
    }
    *ent_cnt = e_seq;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if all the pending move entries have been analyzed.
 *
 * \param [in] stack_ptr Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] ana_e_cnt Analyzed entry count
 */
static bool
rm_hash_move_curr_bkt_idx_valid(rm_hash_ent_move_stack_t *stack_ptr,
                                uint8_t ana_e_cnt)
{
    uint8_t idx = stack_ptr->num_epm - 1;

    if (stack_ptr->epm_info[idx].e_seq < ana_e_cnt) {
        /* All the entries have been placed into the pending move list */
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Get used base entry bitmap for an slb
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb software logical bucket
 * \param [out] slb_e_bmp software logical bucket used base entry bitmap
 */
static int
rm_hash_slb_used_bmp_get(int unit,
                         rm_hash_pt_info_t *pt_info,
                         uint32_t slb,
                         rm_hash_slb_ent_bmp_t *slb_e_bmp)
{
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_slb_ent_bmp_t e_bmp;

    SHR_FUNC_ENTER(unit);

    b_state = pt_info->b_state + slb;

    /* Initial value retrieval */
    e_bmp = b_state->e_bmp;
    /* Update the value according to the move list */
    SHR_IF_ERR_EXIT
        (rm_hash_slb_ent_bmp_update(unit,
                                    pt_info,
                                    slb,
                                    &e_bmp,
                                    NULL));
    *slb_e_bmp = e_bmp;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get used base entry bitmap for an slb
 *
 * \param [in] unit Unit number.
 * \param [in] stack_frm Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] ana_ent_cnt Analyzed entry count
 * \param [in] curr_idx Current index
 * \param [out] next_idx Next index
 */
static void
rm_hash_next_epm_index_get(int unit,
                           rm_hash_ent_move_stack_t *stack_frm,
                           uint8_t ana_ent_cnt,
                           uint8_t curr_idx,
                           uint8_t *next_idx)
{
    uint8_t epm_idx = curr_idx + 1;

    while ((stack_frm->epm_info[epm_idx].e_seq < ana_ent_cnt) &&
           (epm_idx < stack_frm->num_epm)) {
        epm_idx++;
    }
    *next_idx = epm_idx;
}

/*!
 * \brief Get information about the entry to be moved.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] stack_frm Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] epm_idx Current index
 * \param [out] move_ent_info Info about the entry to be moved.
 */
static int
rm_hash_move_ent_info_get(int unit,
                          rm_hash_pt_info_t *pt_info,
                          rm_hash_ent_move_stack_t *stack_frm,
                          uint8_t epm_idx,
                          rm_hash_move_ent_info_t *move_ent_info)
{
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_shr_ent_node_t *se_node = NULL, *se_header = NULL;
    rm_hash_dual_ent_node_t *de_node = NULL, *de_header = NULL;
    uint32_t slb = 0, e_node_offset = 0;

    SHR_FUNC_ENTER(unit);

    slb = stack_frm->slb;
    b_state = pt_info->b_state + slb;
    e_node_offset = b_state->e_node_offset;
    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_get(unit,
                                              pt_info,
                                              e_node_offset,
                                              &de_node));
        de_header = de_node;
        de_node = de_header + stack_frm->epm_info[epm_idx].e_seq;
        if ((de_node != NULL) && ((de_node - de_header) < b_state->ve_cnt)) {
            move_ent_info->e_loc  = de_node->e_loc;
            move_ent_info->sid    = de_node->sid;
            move_ent_info->bm     = de_node->bm;
            move_ent_info->e_size = e_size_map[de_node->e_loc.be_bmp].size;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_get(unit,
                                                 pt_info,
                                                 e_node_offset,
                                                 &se_node));
        se_header = se_node;
        se_node = se_header + stack_frm->epm_info[epm_idx].e_seq;
        if ((se_node != NULL) && ((se_node - se_header) < b_state->ve_cnt)) {
            move_ent_info->e_loc  = se_node->e_loc;
            move_ent_info->sid    = se_node->sid;
            move_ent_info->bm     = se_node->bm;
            move_ent_info->e_size = e_size_map[se_node->e_loc.be_bmp].size;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the entry move info in the stack
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] stack_frm Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] curr_epm_idx Current move index
 * \param [in] e_bmp SLB bitmap
 * \param [out] ment_found If entry has been found
 */
static int
rm_hash_move_stk_frm_ment_record(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 rm_hash_ent_move_stack_t *stack_frm,
                                 uint8_t curr_epm_idx,
                                 rm_hash_slb_ent_bmp_t e_bmp,
                                 bool *ment_found)
{
    uint8_t ament_cnt = 0, act_epm_idx = 0;
    rm_hash_move_ent_info_t ment_info;
    uint16_t be_bmp = 0, le_bmp = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ment_info, 0, sizeof(ment_info));

    /* Determine how many entries should be skipped */
    SHR_IF_ERR_EXIT
        (rm_hash_analyzed_ment_cnt_get(unit,
                                       pt_info,
                                       stack_frm->slb,
                                       &ament_cnt));
    /*
     * The provided tuple index exceeds the maximum value, Or all the entries
     * have been analyzed.
     */
    if ((curr_epm_idx >= stack_frm->num_epm) ||
        (!rm_hash_move_curr_bkt_idx_valid(stack_frm, ament_cnt))){
        *ment_found = FALSE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /*
     * If the entry indexed by the provided tuple index has been already
     * analyzed, we need to increase the tuple index until a un-analyzed one
     */
    if (stack_frm->epm_info[curr_epm_idx].e_seq < ament_cnt) {
        rm_hash_next_epm_index_get(unit,
                                   stack_frm,
                                   ament_cnt,
                                   curr_epm_idx,
                                   &act_epm_idx);
        curr_epm_idx = act_epm_idx;
    }
    if (curr_epm_idx >= stack_frm->num_epm) {
        *ment_found = FALSE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Get the move entry based on tuple index */
    SHR_IF_ERR_EXIT
        (rm_hash_move_ent_info_get(unit,
                                   pt_info,
                                   stack_frm,
                                   curr_epm_idx,
                                   &ment_info));
    /* Update the stack frame accordingly */
    rm_hash_move_ent_info_set(stack_frm, &ment_info);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                &ment_info.e_loc,
                                                &be_bmp));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                &ment_info.e_loc,
                                                &le_bmp));

    /* Update ent bitmap */
    e_bmp.be_bmp &= ~be_bmp;
    e_bmp.le_bmp[ment_info.bm] &= ~le_bmp;
    stack_frm->e_bmp = e_bmp;

    stack_frm->curr_epm_idx = curr_epm_idx;
    stack_frm->slot_provider = FALSE;
    *ment_found = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Push the bucket information onto the bucket stack
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to lt control structure of the LT.
 * \param [in] bkt Logical bucket index.
 * \param [in] stack Stack of entry move.
 * \param [in] orig_need Original request size of the entry.
 * \param [in] start_depth Start depth of the entry move stack.
 * \param [in] end_depth End depth of the entry move stack.
 * \param [out] push_depth Actual depth of the push operation.
 * \param [out] slot_found Flag indicating if free slot has been found.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_bkt_stack_push(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint32_t slb,
                                rm_hash_ent_move_stack_t *stack,
                                rm_hash_req_ent_info_t req_e_info,
                                uint8_t sfrm_idx,
                                uint8_t frm_num_limit,
                                uint8_t *frm_num,
                                bool *slot_found)
{
    int fidx = 0, epm_idx = 0;
    uint8_t curr_epm_idx = 0;
    bool accom = FALSE, ment_found = FALSE;
    rm_hash_slb_ent_bmp_t slb_e_bmp;
    rm_hash_ent_loc_t e_loc;
    rm_hash_req_ent_info_t e_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&slb_e_bmp, 0, sizeof(slb_e_bmp));
    sal_memset(&e_loc, 0, sizeof(e_loc));
    fidx = sfrm_idx;
    e_info = req_e_info;
    if (fidx > 0) {
        curr_epm_idx = stack[fidx - 1].curr_epm_idx;
        SHR_IF_ERR_EXIT
            (rm_hash_slb_used_bmp_get(unit,
                                      pt_info,
                                      stack[fidx - 1].slb,
                                      &slb_e_bmp));
        /* Try to find an entry that can be moved out. */
        SHR_IF_ERR_EXIT
            (rm_hash_move_stk_frm_ment_record(unit,
                                              pt_info,
                                              &stack[fidx - 1],
                                              curr_epm_idx,
                                              slb_e_bmp,
                                              &ment_found));
        if (ment_found == FALSE) {
             rm_hash_ent_move_trace_clear(unit, stack[fidx - 1].slb, fidx - 1);
             *frm_num = fidx - 1;
             *slot_found = FALSE;
             SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    for (; fidx < frm_num_limit; fidx++) {
        if (fidx > 0) {
            epm_idx = stack[fidx - 1].curr_epm_idx;
            slb     = stack[fidx - 1].epm_info[epm_idx].slb_info.slb;
            e_info.e_size  = stack[fidx - 1].move_ent.e_size;
            e_info.vbb_bmp = stack[fidx - 1].epm_info[epm_idx].slb_info.vbb_bmp;
            e_info.e_bm = stack[fidx - 1].move_ent.bm;
            stack[fidx - 1].curr_epm_idx += 1;
        }
        rm_hash_move_stk_frm_alloc(unit,
                                   pt_info,
                                   slb,
                                   e_info,
                                   fidx,
                                   stack);
        SHR_IF_ERR_EXIT
            (rm_hash_slb_used_bmp_get(unit,
                                      pt_info,
                                      slb,
                                      &slb_e_bmp));
        stack[fidx].e_bmp = slb_e_bmp;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                                   pt_info,
                                                   &slb_e_bmp,
                                                   &e_info,
                                                   &accom,
                                                   &e_loc));
        if (accom == TRUE) {
            *frm_num = fidx + 1;
            *slot_found = TRUE;
            stack[fidx].slot_provider = TRUE;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        if ((fidx == (frm_num_limit - 1)) && accom == FALSE) {
            *frm_num = fidx;
            *slot_found = FALSE;
            stack[fidx].slot_provider = FALSE;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        curr_epm_idx = 0;
        SHR_IF_ERR_EXIT
            (rm_hash_move_stk_frm_ment_record(unit,
                                              pt_info,
                                              &stack[fidx],
                                              curr_epm_idx,
                                              slb_e_bmp,
                                              &ment_found));
        if (ment_found == FALSE) {
            rm_hash_ent_move_trace_clear(unit, stack[fidx].slb, fidx);
            *frm_num = fidx;
            *slot_found = FALSE;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pop the stack
 *
 * The stack to be popped will contain 1 or multiple frames. The total frame number
 * of the stack is described by the parameter cur_frm_num. Starting from the top of
 * stack, this routine will analyze whether current frame can accommodate the requested
 * entry size. The requested entry comes from last frame.
 *
 * If the current frame can accommodate the requested entry size, that means the entry
 * in current - 1 frame can be moved into current frame. So an entry move operation
 * will be recorded in the move pending list.
 *
 * In the process of popping from top of the stack to the bottom of the stack, once
 * a frame at layer n cannot accommodate its requested entry, the pop operation will
 * terminate and the stack depth will be updated as n. in another word, it consists
 * of frame 0.. n-1.
 *
 * \param [in] unit Unit number.
 * \param [in] bkt_size Size of logical bucket.
 * \param [in] stack Entry move stack.
 * \param [in] cur_frm_num Current depth of entry move stack.
 * \param [out] new_frm_num Depth of entry move stack after the pop is completed.
 * \param [out] slot_found Flag indicating if free slot has been found.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_bkt_stack_pop(int unit,
                               rm_hash_pt_info_t *pt_info,
                               rm_hash_ent_move_stack_t *stack,
                               uint8_t cur_frm_num,
                               uint8_t *new_frm_num,
                               bool *slot_found)
{
    int frm_idx;
    uint32_t s_slb = 0, d_slb = 0;
    bcmdrd_sid_t sid;
    bool accom = FALSE;
    rm_hash_ent_loc_t se_loc, de_loc;
    rm_hash_bm_t e_bm;
    rm_hash_req_ent_info_t e_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&se_loc, 0, sizeof(se_loc));
    sal_memset(&de_loc, 0, sizeof(de_loc));
    for (frm_idx = cur_frm_num - 1; frm_idx >= 0; frm_idx--) {
        e_info = stack[frm_idx].req_e_info;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                                   pt_info,
                                                   &stack[frm_idx].e_bmp,
                                                   &e_info,
                                                   &accom,
                                                   &de_loc));

        if (accom == TRUE) {
            if (frm_idx == 0) {
                *slot_found = TRUE;
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
            s_slb =  stack[frm_idx - 1].slb;
            se_loc = stack[frm_idx - 1].move_ent.e_loc;
            sid =    stack[frm_idx - 1].move_ent.sid;
            e_bm =   stack[frm_idx - 1].move_ent.bm;
            d_slb =  stack[frm_idx].slb;
            SHR_IF_ERR_EXIT
                (rm_hash_ent_move_op_save(unit,
                                          s_slb,
                                          se_loc,
                                          d_slb,
                                          de_loc,
                                          stack[frm_idx].req_e_info.e_size,
                                          e_bm,
                                          sid,
                                          stack[frm_idx].slot_provider));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "movement operation save: bkt_id [%d] \
                                     bkt_pair_id [%d] num [%d]\n"),
                                    s_slb, d_slb,
                                    stack[frm_idx].req_e_info.e_size));
        } else {
            *new_frm_num = frm_idx;
            *slot_found = FALSE;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry move path
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to lt control structure of the LT.
 * \param [in] lbkt Logical bucket.
 * \param [in] ent_size Entry size.
 * \param [in] trying_depth Depth of entry move stack.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_path_get(int unit,
                          rm_hash_pt_info_t *pt_info,
                          uint32_t slb,
                          rm_hash_req_ent_info_t req_e_info,
                          uint8_t trying_depth)
{
    rm_hash_ent_move_stack_t ent_move_stack[BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH + 1];
    uint8_t frmnum_pushed = 0, pop_depth = 0;
    bool slot_found_in_push = FALSE, path_avail = FALSE;

    SHR_FUNC_ENTER(unit);

    rm_hash_ent_move_info_clear(unit);
    sal_memset(ent_move_stack, 0,
               sizeof(rm_hash_ent_move_stack_t) *
               (BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH + 1));
    while (1) {
        SHR_IF_ERR_EXIT
            (rm_hash_ent_move_bkt_stack_push(unit,
                                             pt_info,
                                             slb,
                                             ent_move_stack,
                                             req_e_info,
                                             pop_depth,
                                             trying_depth,
                                             &frmnum_pushed,
                                             &slot_found_in_push));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "slot_found_in_push:[%d], start depth:[%d],"
                                 "end depth [%d]\n"),
                                slot_found_in_push, pop_depth, frmnum_pushed));
        if (slot_found_in_push == FALSE) {
            if (frmnum_pushed == 0) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "empty entry is not found\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_FULL);
            } else {
                pop_depth = frmnum_pushed;
            }
        } else {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_move_bkt_stack_pop(unit,
                                                pt_info,
                                                ent_move_stack,
                                                frmnum_pushed,
                                                &pop_depth,
                                                &path_avail));
            if (path_avail == TRUE) {
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            } else {
                pop_depth += 1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move an entry node from a bkt to another
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to lt control structure of the PT.
 * \param [in] src_lbkt Source bucket of an entry move.
 * \param [in] dst_lbkt Destination bucket of an entry move.
 * \param [in] src_bmp Bitmap of the scource entry to be moved.
 * \param [in] dst_bmp Bitmap of the destination entry to be moved.
 * \param [in] doffset Offset of the destination entry to be moved.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_node_move(int unit,
                      rm_hash_pt_info_t *pt_info,
                      uint32_t s_slb,
                      uint32_t d_slb,
                      rm_hash_ent_loc_t se_loc,
                      rm_hash_ent_loc_t de_loc)
{
    rm_hash_slb_state_t *s_b_state = NULL, *d_b_state = NULL;
    rm_hash_dual_ent_node_t *move_dent = NULL, *dent_node = NULL;
    rm_hash_shr_ent_node_t *move_sent = NULL, *sent_node = NULL;
    uint8_t dst_lbank = 0;
    uint32_t ent_index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_lbank_get(unit, pt_info, d_slb, &dst_lbank));
    s_b_state = pt_info->b_state + s_slb;
    d_b_state = pt_info->b_state + d_slb;
    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_delete(unit,
                                                 pt_info,
                                                 s_b_state,
                                                 se_loc.bb_bmp,
                                                 se_loc.be_bmp,
                                                 &move_dent));
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
         SHR_IF_ERR_EXIT
             (bcmptm_rm_hash_trans_ulist_dual_ent_insert(unit,
                                                         s_slb,
                                                         pt_info->e_ctr_offset,
                                                         move_dent,
                                                         BCMPTM_RM_HASH_UNDO_ENT_INSERT));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_allocate(unit,
                                                   pt_info,
                                                   d_b_state,
                                                   &dent_node));
        *dent_node = *move_dent;
        dent_node->e_loc = de_loc;
        dent_node->e_loc.lbank = dst_lbank;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_index_get(unit,
                                          pt_info,
                                          d_slb,
                                          &de_loc,
                                          e_size_map[dent_node->e_loc.be_bmp].size,
                                          &ent_index));
        dent_node->e_idx = ent_index;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_insert(unit,
                                                 pt_info,
                                                 d_b_state,
                                                 dent_node));
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
         SHR_IF_ERR_EXIT
             (bcmptm_rm_hash_trans_ulist_dual_ent_insert(unit,
                                                         d_slb,
                                                         pt_info->e_ctr_offset,
                                                         dent_node,
                                                         BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_delete(unit,
                                                pt_info,
                                                s_b_state,
                                                se_loc.bb_bmp,
                                                se_loc.be_bmp,
                                                &move_sent));
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
         SHR_IF_ERR_EXIT
             (bcmptm_rm_hash_trans_ulist_shr_ent_insert(unit,
                                                        s_slb,
                                                        pt_info->e_ctr_offset,
                                                        move_sent,
                                                        BCMPTM_RM_HASH_UNDO_ENT_INSERT));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_allocate(unit,
                                                  pt_info,
                                                  d_b_state,
                                                  &sent_node));
        *sent_node = *move_sent;
        sent_node->e_loc = de_loc;
        sent_node->e_loc.lbank = dst_lbank;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_index_get(unit,
                                          pt_info,
                                          d_slb,
                                          &sent_node->e_loc,
                                          e_size_map[sent_node->e_loc.be_bmp].size,
                                          &ent_index));
        sent_node->e_idx = ent_index;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_insert(unit,
                                                pt_info,
                                                d_b_state,
                                                sent_node));
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
         SHR_IF_ERR_EXIT
             (bcmptm_rm_hash_trans_ulist_shr_ent_insert(unit,
                                                        d_slb,
                                                        pt_info->e_ctr_offset,
                                                        sent_node,
                                                        BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update software bkt state according to move operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to lt control structure of the LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_bkt_state_update(int unit, rm_hash_pt_info_t *pt_info)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    int idx;
    bool ret = FALSE;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; ; idx++) {
        ret = rm_hash_ent_move_op_get(unit, idx, &move_op);
        if (ret == FALSE) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }

        SHR_IF_ERR_EXIT
            (rm_hash_ent_node_move(unit,
                                   pt_info,
                                   move_op->s_slb,
                                   move_op->d_slb,
                                   move_op->se_loc,
                                   move_op->de_loc));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Examine if a bucket has enough space to store an entry
 *
 * \param [in] unit Unit number.
 * \param [in] req_e_size Requested entry size.
 * \param [in] vbb_bmp Valid base bucket bitmap for the requested entry.
 * \param [in] req_e_bm Bucket mode of the requested entry.
 * \param [in] b_state Software logical bucket state.
 * \param [out] accom Flag indicating if free space is available.
 * \param [out] e_loc Detailed information about location.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_can_hold(int unit,
                     rm_hash_pt_info_t *pt_info,
                     uint8_t req_e_size,
                     uint8_t vbb_bmp,
                     rm_hash_bm_t req_e_bm,
                     rm_hash_slb_state_t *b_state,
                     bool *accom,
                     rm_hash_ent_loc_t *e_loc)
{
    rm_hash_slb_ent_bmp_t e_bmp;
    rm_hash_bm_t bm_idx;
    rm_hash_req_ent_info_t e_info;

    SHR_FUNC_ENTER(unit);

    e_bmp.be_bmp = b_state->e_bmp.be_bmp;
    for (bm_idx = RM_HASH_BM_N; bm_idx < RM_HASH_BM_CNT; bm_idx++) {
        e_bmp.le_bmp[bm_idx] = b_state->e_bmp.le_bmp[bm_idx];
    }

    e_info.e_size = req_e_size;
    e_info.vbb_bmp = vbb_bmp;
    e_info.e_bm = req_e_bm;

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                               pt_info,
                                               &e_bmp,
                                               &e_info,
                                               accom,
                                               e_loc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct memory update according to move operation list.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Request flags for the hash operation.
 * \param [in] req_ltid Request ltid of the hash operation.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] sid SID of the hash table.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lbkt Logical bucket index.
 * \param [in] ent_size Entry size.
 * \param [in] lt_ctrl Pointer to rm_hash_pt_info_t structure.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure for recording.
 *              response info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_mem_update(int unit,
                            uint32_t req_flags,
                            bcmltd_sid_t req_ltid,
                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                            bcmdrd_sid_t sid,
                            bcmptm_rm_hash_req_t *req_info,
                            uint32_t slb,
                            uint8_t vbb_bmp,
                            uint8_t req_e_size,
                            rm_hash_bm_t req_e_bm,
                            rm_hash_pt_info_t *pt_info,
                            bcmptm_rm_hash_rsp_t *rsp_info)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    int i = 0;
    bool ret = FALSE, rsp_ent_valid, accom = FALSE;
    uint8_t ent_num_move = 0;
    uint32_t ent_index, dst_ent_index, ent_from[BCMPTM_MAX_PT_ENTRY_WORDS];
    bcmdrd_sid_t sid_base = sid;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};
    bcmbd_pt_dyn_info_t ent_dyn_info = {-1, -1};
    rm_hash_ent_loc_t se_loc, de_loc;
    rm_hash_slb_state_t *b_state_base = NULL;
    uint32_t cmdproc_rsp_flags = 0, rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    uint16_t rsp_dfield_format_id;

    SHR_FUNC_ENTER(unit);

    b_state_base = pt_info->b_state;
    for (i = 0; ; i++) {
        ret = rm_hash_ent_move_op_get(unit, i, &move_op);
        if (ret == FALSE) {
            if (i == 0) {
                SHR_IF_ERR_EXIT
                    (rm_hash_slb_can_hold(unit,
                                          pt_info,
                                          req_e_size,
                                          vbb_bmp,
                                          req_e_bm,
                                          &b_state_base[slb],
                                          &accom,
                                          &de_loc));
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_index_get(unit,
                                                  pt_info,
                                                  slb,
                                                  &de_loc,
                                                  req_e_size,
                                                  &ent_index));
            } else {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_index_get(unit,
                                                  pt_info,
                                                  slb,
                                                  &se_loc,
                                                  req_e_size,
                                                  &ent_index));
            }
            dyn_info = *pt_dyn_info;
            dyn_info.index = ent_index;
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "LTID:[%d] "
                                   "mem[%s] index[%d]. \n"),
                                   req_ltid,
                                   bcmdrd_pt_sid_to_name(unit, sid),
                                   ent_index));
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_write(unit,
                                      req_flags,
                                      0,
                                      sid,
                                      &dyn_info,
                                      NULL,
                                      NULL, /* misc_info */
                                      req_info->entry_words,
                                      TRUE,
                                      TRUE,
                                      TRUE,
                                      BCMPTM_RM_OP_NORMAL, NULL,
                                      req_ltid,
                                      req_info->dfield_format_id,
                                      &cmdproc_rsp_flags));
            rsp_info->rsp_entry_sid = sid;
            rsp_info->rsp_entry_index = ent_index;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        rm_hash_ent_move_op_dump(unit, move_op);
        ent_num_move = move_op->e_size;
        sid_base = move_op->sid;
        se_loc = move_op->se_loc;
        de_loc = move_op->de_loc;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_index_get(unit,
                                          pt_info,
                                          move_op->s_slb,
                                          &se_loc,
                                          ent_num_move,
                                          &ent_index));
        ent_dyn_info.index = ent_index;
        /*
         * By default, the hit bits of the entry will NOT be preserved
         * during entry move. The content of the entries to be moved
         * will be retrieved from PTcache, not from HW table.
         * In the future, a configurable variable may be considered
         * to control if the hit bits need to be preserved. It should
         * be noted that reading HW table to get the hit bit will
         * completely stall Modeled path.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_read(unit,
                                 BCMPTM_REQ_FLAGS_NO_FLAGS,
                                 0,
                                 sid_base,
                                 &ent_dyn_info,
                                 NULL,
                                 NULL, /* misc_info */
                                 COUNTOF(ent_from),
                                 ent_from,
                                 &rsp_ent_valid,
                                 &rsp_ltid,
                                 &rsp_dfield_format_id,
                                 &rsp_flags));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_index_get(unit,
                                          pt_info,
                                          move_op->d_slb,
                                          &de_loc,
                                          ent_num_move,
                                          &dst_ent_index));
        ent_dyn_info.index = dst_ent_index;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                              "LTID:[%d] "
                              "mem[%s] index[%d]. \n"),
                              rsp_ltid,
                              bcmdrd_pt_sid_to_name(unit, sid_base),
                              dst_ent_index));
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_write(unit,
                                  req_flags,
                                  0,
                                  sid_base,
                                  &ent_dyn_info,
                                  NULL,
                                  NULL, /* misc_info */
                                  ent_from,
                                  TRUE,
                                  TRUE,
                                  TRUE,
                                  BCMPTM_RM_OP_NORMAL, NULL,
                                  rsp_ltid,
                                  rsp_dfield_format_id,
                                  &cmdproc_rsp_flags));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_insert_with_ent_move(int unit,
                                    uint32_t req_flags,
                                    bcmltd_sid_t req_ltid,
                                    bcmdrd_sid_t ptsid,
                                    rm_hash_pt_info_t *pt_info,
                                    int depth,
                                    const bcmptm_rm_hash_more_info_t *more_info,
                                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                                    bcmptm_rm_hash_req_t *req_info,
                                    uint8_t lbank,
                                    rm_hash_ent_slb_info_t *slb_info_list,
                                    uint8_t ent_size,
                                    rm_hash_bm_t e_bm,
                                    bcmptm_rm_hash_rsp_t *rsp_info)
{
    rm_hash_slb_state_t *b_state_base = NULL;
    rm_hash_req_ent_info_t req_e_info;
    uint32_t ent_index = 0, ent_sig = 0;
    int tmp_rv;
    rm_hash_ent_loc_t e_loc;
    bool accom = FALSE;

    SHR_FUNC_ENTER(unit);

    if (depth <= 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    }

    b_state_base = pt_info->b_state;
    req_e_info.e_size = ent_size;
    req_e_info.vbb_bmp = slb_info_list[lbank].vbb_bmp;
    req_e_info.e_bm = e_bm;
    tmp_rv = rm_hash_ent_move_path_get(unit,
                                       pt_info,
                                       slb_info_list[lbank].slb,
                                       req_e_info,
                                       depth + 1);
    if (tmp_rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(tmp_rv);
    }
    SHR_IF_ERR_EXIT
        (rm_hash_ent_move_mem_update(unit,
                                     req_flags,
                                     req_ltid,
                                     pt_dyn_info,
                                     ptsid,
                                     req_info,
                                     slb_info_list[lbank].slb,
                                     slb_info_list[lbank].vbb_bmp,
                                     ent_size,
                                     e_bm,
                                     pt_info,
                                     rsp_info));
    SHR_IF_ERR_EXIT
        (rm_hash_ent_move_bkt_state_update(unit, pt_info));

    SHR_IF_ERR_EXIT
        (rm_hash_slb_can_hold(unit,
                              pt_info,
                              ent_size,
                              slb_info_list[lbank].vbb_bmp,
                              e_bm,
                              &b_state_base[slb_info_list[lbank].slb],
                              &accom,
                              &e_loc));
    e_loc.lbank = lbank;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      slb_info_list[lbank].slb,
                                      &e_loc,
                                      ent_size,
                                      &ent_index));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_sig_generate(unit,
                                         more_info,
                                         req_info->entry_words,
                                         &ent_sig));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_state_set(unit,
                                      pt_info,
                                      &e_loc,
                                      slb_info_list,
                                      ptsid,
                                      ent_index,
                                      ent_sig,
                                      e_bm,
                                      ent_size));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_move_info_init(int unit)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    size_t op_size = 0;

    SHR_FUNC_ENTER(unit);

    op_size = sizeof(rm_hash_ent_move_op_t) * RM_HASH_ENTRY_MOVE_LIST_DEF_NUM;
    move_op = sal_alloc(op_size, "entry_move info");
    if (move_op == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    sal_memset(move_op, 0, op_size);
    e_move_op_list[unit].e_move_op = move_op;
    e_move_op_list[unit].count = RM_HASH_ENTRY_MOVE_LIST_DEF_NUM;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_move_info_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (e_move_op_list[unit].e_move_op != NULL) {
        sal_free(e_move_op_list[unit].e_move_op);
        e_move_op_list[unit].e_move_op = NULL;
        e_move_op_list[unit].count = 0;
    }

    SHR_FUNC_EXIT();
}
