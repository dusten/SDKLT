/*! \file ha_issu_ctrl.c
 *
 * HA ISSU control module manages the components data structures for the
 * purpose of ISSU.
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

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include "ha_internal.h"

#define STRUCTS_PER_BLK  100

/*!
 * \brief Component structure information structure.
 *
 * This structure use to trace the location of the components data structure.
 * This information is only used in case of ISSU.
 */
typedef struct {
    uint64_t struct_id;  /* Unique structure ID */
    uint16_t blk_id;     /* The block ID containing the structure */
    uint16_t offset;     /* The location of the structure within the block */
} comp_struct_info_t;

static bool find_struct_entry(ha_private_t *this,
                              uint16_t blk_id,
                              uint32_t offset,
                              uint64_t id)
{
    ha_mem_comp_struct_ctrl_blk_t *comp_struct = this->init_shr_mem;
    uint8_t *tmp;
    comp_struct_info_t *struct_info;
    uint32_t j;

    /* Go through all the current struct info HA blocks */
    do {
        if (comp_struct->next_blk_section == INVALID_BLOCK_SECTION) {
            return false;
        }
        tmp = (uint8_t *)this->mem_sect[comp_struct->next_blk_section].mem_start;
        tmp += comp_struct->next_blk_offset;
        comp_struct = (ha_mem_comp_struct_ctrl_blk_t *)tmp;
        if (comp_struct->signature != HA_MEM_SIGNATURE) {
            return false;
        }

        tmp = (uint8_t *)comp_struct + sizeof(ha_mem_comp_struct_ctrl_blk_t);
        struct_info = (comp_struct_info_t *)tmp;
        for (j = 0; j < comp_struct->num_of_elements; j++, struct_info++) {
            if (struct_info->struct_id == id && struct_info->blk_id == blk_id &&
                struct_info->offset == offset) {
                return true;
            }
        }
    } while (true);
}

void shr_ha_issu_struct_init(ha_private_t *this)
{
    ha_mem_comp_struct_ctrl_blk_t *comp_struct = this->init_shr_mem;
    ha_mem_blk_hdr_t *blk_hdr;
    uint8_t *tmp;

    blk_hdr = this->mem_sect[0].mem_start;
    do {
        if (blk_hdr->attrib & BLOCK_INTERNAL_USE) {
            /* All blocks will be in section 0*/
            comp_struct->next_blk_section = 0;
            /* Update the offset */
            comp_struct->next_blk_offset = (uint8_t *)blk_hdr -
                (uint8_t *)this->mem_sect[0].mem_start;
            comp_struct->next_blk_offset += sizeof(ha_mem_blk_hdr_t);

            /* We done with the current block so update to the new block */
            tmp = (uint8_t *)blk_hdr + sizeof(ha_mem_blk_hdr_t);
            comp_struct = (ha_mem_comp_struct_ctrl_blk_t *)tmp;
        }
        blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
    } while (blk_hdr < this->mem_sect[0].mem_end);
}

int shr_ha_struct_report_ha(void *private,
                            void *mem,
                            uint32_t offset,
                            uint64_t id)
{
    ha_mem_blk_hdr_t *blk_hdr = (ha_mem_blk_hdr_t *)
        ((uint8_t *)mem - sizeof(ha_mem_blk_hdr_t));
    ha_private_t *this = (ha_private_t *)private;
    ha_mem_comp_struct_ctrl_blk_t *comp_struct = this->init_shr_mem;
    uint8_t *tmp;
    comp_struct_info_t *struct_info;

    if (!mem){
        return SHR_E_PARAM;
    }
    /* Sanity check that the block is valid */
    if ((blk_hdr->signature != HA_MEM_SIGNATURE)||
       (blk_hdr->state !=(uint8_t)HA_BLK_ALLOCATED)) {
        return SHR_E_PARAM;
    }

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);

    /* First search to see if this entry already exist. */
    if (find_struct_entry(this, blk_hdr->blk_id, offset, id)) {
        sal_mutex_give(this->mutex);
        return SHR_E_NONE;
    }

    /* Go through the comp_struct blocks to find the last one */
    while (comp_struct->block_size == comp_struct->num_of_elements) {
        if (comp_struct->next_blk_section == INVALID_BLOCK_SECTION) {
            break;
        }
        tmp = (uint8_t *)this->mem_sect[comp_struct->next_blk_section].mem_start;
        tmp += comp_struct->next_blk_offset;
        comp_struct = (ha_mem_comp_struct_ctrl_blk_t *)tmp;
        /* Verify that the block is coherent */
        if (comp_struct->signature != HA_MEM_SIGNATURE) {
            sal_mutex_give(this->mutex);
            return SHR_E_INTERNAL;
        }
    }
    /* If the last block is full need to allocate another one */
    if (comp_struct->block_size == comp_struct->num_of_elements) {
        uint32_t length = STRUCTS_PER_BLK * sizeof(comp_struct_info_t);
        ha_mem_blk_hdr_t *new_blk_hdr;

        /*
         * Add the block overhead - the comp struct block header.
         */
        length += sizeof(ha_mem_comp_struct_ctrl_blk_t);
        new_blk_hdr = shr_ha_alloc_internal_block(this, length);

        if (!new_blk_hdr) {
            sal_mutex_give(this->mutex);
            return SHR_E_MEMORY;
        }
        new_blk_hdr->attrib = BLOCK_INTERNAL_USE;
        comp_struct->next_blk_section = new_blk_hdr->section;
        comp_struct->next_blk_offset = (uint8_t *)new_blk_hdr -
            (uint8_t *)this->mem_sect[new_blk_hdr->section].mem_start;
        comp_struct->next_blk_offset += sizeof(ha_mem_blk_hdr_t);
        tmp = (uint8_t *)new_blk_hdr + sizeof(ha_mem_blk_hdr_t);
        comp_struct = (ha_mem_comp_struct_ctrl_blk_t *)tmp;
        comp_struct->block_size = STRUCTS_PER_BLK;
        comp_struct->num_of_elements = 0;
        comp_struct->next_blk_section = INVALID_BLOCK_SECTION;
        comp_struct->next_blk_offset = 0;
        comp_struct->signature = HA_MEM_SIGNATURE;
    }

    /* tmp points to where the data structs */
    tmp = (uint8_t *)comp_struct + sizeof(ha_mem_comp_struct_ctrl_blk_t);
    struct_info = (comp_struct_info_t *)tmp;
    struct_info += comp_struct->num_of_elements++;

    /* Finnaly we can write the data */
    struct_info->blk_id = blk_hdr->blk_id;
    struct_info->offset = offset;
    struct_info->struct_id = id;

    sal_mutex_give(this->mutex);
    return SHR_E_NONE;
}

int shr_ha_struct_info_ha(void *private,
                          uint64_t id,
                          void **mem,
                          uint32_t *offset)
{
    ha_private_t *this = (ha_private_t *)private;
    ha_mem_comp_struct_ctrl_blk_t *comp_struct = this->init_shr_mem;
    uint8_t *tmp;
    comp_struct_info_t *struct_info;
    ha_mem_blk_hdr_t *blk;
    uint32_t j;
    int rv = SHR_E_NOT_FOUND;

    if (!mem || !offset) {
        return SHR_E_PARAM;
    }

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);

    /* Go through all the current struct info HA blocks */
    do {
        if (comp_struct->next_blk_section == INVALID_BLOCK_SECTION) {
            break;
        }
        tmp = (uint8_t *)this->mem_sect[comp_struct->next_blk_section].mem_start;
        tmp += comp_struct->next_blk_offset;
        comp_struct = (ha_mem_comp_struct_ctrl_blk_t *)tmp;
        if (comp_struct->signature != HA_MEM_SIGNATURE) {
            break;
        }

        tmp = (uint8_t *)comp_struct + sizeof(ha_mem_comp_struct_ctrl_blk_t);
        struct_info = (comp_struct_info_t *)tmp;
        for (j = 0; j < comp_struct->num_of_elements; j++, struct_info++) {
            if (struct_info->struct_id == id) {
                blk = shr_ha_block_find(this,  struct_info->blk_id);
                if (!blk) {   /* Our internal data is not coherent */
                    rv = SHR_E_INTERNAL;
                    break;
                }
                tmp = (uint8_t *)blk;
                *mem = tmp + sizeof(ha_mem_blk_hdr_t);
                *offset = struct_info->offset;
                rv = SHR_E_NONE;
                break;
            }
        }
    } while (rv == SHR_E_NOT_FOUND);

    sal_mutex_give(this->mutex);
    return rv;
}
