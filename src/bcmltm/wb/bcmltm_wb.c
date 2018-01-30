/*! \file bcmltm_wb.c
 *
 * Logical Table Manager Working Buffer.
 *
 * This file contains routines to manage the working buffer space
 * required for table operations to assemble the data, such as
 * PTM format entry buffers, PTM memory parameters, field transforms input
 * and output, etc.
 *
 * Coordination of the working buffer space is required among different
 * binary tree function nodes called during the FA and EE stages
 * of a table operation.
 *
 * Working Buffer Handle:
 * The working buffer handle maintains the total working buffer
 * space required for a table.  It contains a list of working
 * buffer blocks for a given table.
 *
 * Working Buffer Block:
 * Working buffer blocks are added during the LTM metadata contruction
 * based on the table needs.  Each block has a type with a well
 * defined buffer layout.
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

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_wb_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Display information for given PTM working buffer block.
 *
 * This routine displays the working buffer information for given PTM block.
 *
 * \param [in] block PTM working buffer block.
 * \param [in] type PTM block type.
 * \param [in] prefix Prefix indentation for output.
 */
static void
block_ptm_dump(const bcmltm_wb_block_ptm_t *block,
               bcmltm_wb_block_type_t type,
               char *prefix)
{
    char *ptm_str;

    if (type == BCMLTM_WB_BLOCK_TYPE_PTM_INDEX) {
        ptm_str = "PTM_INDEX";
    } else if (type == BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU) {
        ptm_str = "PTM_KEYED_PTHRU";
    } else { /* BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT */
        ptm_str = "PTM_KEYED_LT";
    }

    cli_out("%s%s:\n", prefix, ptm_str);

    cli_out("%s  ptid=%d num_ops=%d\n",
            prefix, block->ptid, block->num_ops);
    cli_out("%s  param_wsize=%d entry_wsize=%d op_wsize(1-op)=%d\n",
            prefix, block->param_wsize, block->entry_wsize, block->op_wsize);
    cli_out("%s  ofs={param=%-5d entry=%-5d}\n",
            prefix, block->offset.param, block->offset.entry);

    return;
}


/*!
 * \brief Display information for given LT Index working buffer block.
 *
 * This routine displays the working buffer information for given
 * LT Index block.
 *
 * \param [in] block LT Index working buffer block.
 * \param [in] prefix Prefix indentation for output.
 */
static void
block_lt_index_dump(const bcmltm_wb_block_lt_index_t *block,
                    char *prefix)
{
    char *block_str;

    block_str = "LT_INDEX";

    cli_out("%s%s:\n", prefix, block_str);

    cli_out("%s  index_ofs={track=%-5d index_absent=%-5d}\n",
            prefix,
            block->offset.index.track,
            block->offset.index.index_absent);

    return;
}


/*!
 * \brief Display information for given LTA working buffer block.
 *
 * This routine displays the working buffer information for given LTA
 *  block.
 *
 * \param [in] block LTA working buffer block.
 * \param [in] type LTA block type.
 * \param [in] prefix Prefix indentation for output.
 */
static void
block_lta_dump(const bcmltm_wb_block_lta_t *block,
               bcmltm_wb_block_type_t type,
               char *prefix)
{
    char *block_str;

    if (type == BCMLTM_WB_BLOCK_TYPE_LTA_CTH) {
        block_str = "LTA_CTH";
    } else if (type == BCMLTM_WB_BLOCK_TYPE_LTA_XFRM) {
        block_str = "LTA_XFRM";
    } else { /* BCMLTM_WB_BLOCK_TYPE_LTA_VAL */
        block_str = "LTA_VAL";
    }

    cli_out("%s%s:\n", prefix, block_str);

    cli_out("%s  in_num_fields=%d out_num_fields=%d\n",
            prefix, block->in_num_fields, block->out_num_fields);
    cli_out("%s  in_ofs ={param=%-5d ptrs=%-5d field=%-5d}\n",
            prefix, block->offset.in.param, block->offset.in.ptrs,
            block->offset.in.field);
    cli_out("%s  out_ofs={param=%-5d ptrs=%-5d field=%-5d}\n",
            prefix, block->offset.out.param, block->offset.out.ptrs,
            block->offset.out.field);

    return;
}


/*!
 * \brief Display information for given working buffer block.
 *
 * This routine displays the working buffer information for given block.
 *
 * \param [in] block Working buffer block.
 * \param [in] prefix Prefix indentation for output.
 */
static void
block_dump(const bcmltm_wb_block_t *block, char *prefix)
{
    char new_prefix[30];

    cli_out("%sBlock ID: %d\n", prefix, block->id);

    sal_sprintf(new_prefix, "%s  ", prefix);
    cli_out("%swsize=%d  base_ofs=%d  type=%d\n",
            new_prefix, block->wsize, block->base_offset, block->type);

    switch(block->type) {
    case BCMLTM_WB_BLOCK_TYPE_PTM_INDEX:
    case BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU:
    case BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT:
        block_ptm_dump(&block->u.ptm, block->type, new_prefix);
        break;

    case BCMLTM_WB_BLOCK_TYPE_LT_INDEX:
        block_lt_index_dump(&block->u.lt_index, new_prefix);
        break;

    case BCMLTM_WB_BLOCK_TYPE_LTA_CTH:
    case BCMLTM_WB_BLOCK_TYPE_LTA_XFRM:
    case BCMLTM_WB_BLOCK_TYPE_LTA_VAL:
        block_lta_dump(&block->u.lta, block->type, new_prefix);
        break;

    default:
        break;
    }

    return;
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_handle_init(uint32_t sid,
                      bcmltm_wb_handle_t *handle)
{
    if  (handle == NULL) {
        return SHR_E_PARAM;
    }

    sal_memset(handle, 0, sizeof(*handle));

    /*
     * Working buffer reserves 1 word.
     *
     * This is to done to reserve the offset value of 0 as invalid.
     * It also indicates handle has been initialized (> 0).
     */
    handle->wsize = 1;         /* Reserve 1 word */
    handle->sid = sid;
    handle->num_blocks = 0;
    handle->blocks = NULL;

    return SHR_E_NONE;
}


void
bcmltm_wb_handle_cleanup(bcmltm_wb_handle_t *handle)
{
    bcmltm_wb_block_t *block;
    bcmltm_wb_block_t *next;

    if  (handle == NULL) {
        return;
    }

    block = handle->blocks;
    while (block != NULL) {
        next = block->next;
        SHR_FREE(block);
        block = next;
    }

    handle->wsize = 0;
    handle->sid = 0;
    handle->num_blocks = 0;
    handle->blocks = NULL;
}


int
bcmltm_wb_handle_validate(bcmltm_wb_handle_t *handle)
{
    if  (handle == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid null working buffer handle\n")));
        return SHR_E_INTERNAL;
    }

    if (handle->wsize == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM working buffer handle has not been "
                            "initialized\n")));
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}


uint32_t
bcmltm_wb_wsize_get(const bcmltm_wb_handle_t *handle)
{
    if  (handle == NULL) {
        return 0;
    }

    return handle->wsize;
}


bcmltm_wb_block_t *
bcmltm_wb_block_add(uint32_t wsize, bcmltm_wb_block_type_t type,
                    bcmltm_wb_handle_t *handle)
{
    bcmltm_wb_block_t *block = NULL;

    if  (handle == NULL) {
        return NULL;
    }

    block = sal_alloc(sizeof(*block), "LTM working buffer block");
    if (block == NULL) {
        return NULL;
    }

    sal_memset(block, 0, sizeof(*block));

    /* Set generic block information */
    block->id = handle->num_blocks;
    block->wsize = wsize;
    block->base_offset = handle->wsize;
    block->type = type;
    block->next = NULL;

    /* Add block to end of list */
    if (handle->blocks == NULL) {
        handle->blocks = block;
    } else {
        bcmltm_wb_block_t *last_block = handle->blocks;

        while (last_block->next != NULL) {
            last_block = last_block->next;
        }
        last_block->next = block;
    }

    /* Update handler info */
    handle->wsize += block->wsize;
    handle->num_blocks++;

    return block;
}


const bcmltm_wb_block_t *
bcmltm_wb_block_find(const bcmltm_wb_handle_t *handle,
                     bcmltm_wb_block_id_t block_id)
{
    const bcmltm_wb_block_t *block = NULL;

    if  (handle == NULL) {
        return NULL;
    }

    block = handle->blocks;
    while (block != NULL) {
        if (block->id == block_id) {
            break;
        }
        block = block->next;
    }

    return block;
}


void
bcmltm_wb_dump(const bcmltm_wb_handle_t *handle)
{
    bcmltm_wb_block_t *block = NULL;
    char *prefix;

    cli_out("--- Working Buffer ---\n");

    if  (handle == NULL) {
        return;
    }

    cli_out("Total Size (words): %d\n", handle->wsize);
    cli_out("Table ID: %d\n", handle->sid);
    cli_out("Blocks: %d\n", handle->num_blocks);

    prefix = "  ";
    block = handle->blocks;
    while (block != NULL) {
        block_dump(block, prefix);
        block = block->next;
    }

    return;
}

