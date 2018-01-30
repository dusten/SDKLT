/*! \file ha.c
 *
 * HA API implementation.
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
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include "ha_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define HA_MAX_UNITS    16     

/* instance for every unit */
static void *ha_inst[HA_MAX_UNITS];

static const shr_ha_drv_t *ha_mem[HA_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
static void *
loc_alloc(void *private,
          uint8_t mod_id,
          uint8_t sub_id,
          uint32_t *length)
{
    return sal_alloc(*length, "shrHaAlloc");
}

static void *
loc_realloc(void *private,
            void *mem,
            uint32_t length)
{
    void *tmp = sal_alloc(length, "shrHaRealloc");
    if (!tmp) {
        return NULL;
    }
    sal_memcpy(tmp, mem, length);   
    sal_free(mem);
    return tmp;
}

static int
loc_free(void *private, void *mem)
{
    sal_free(mem);
    return 0;
}

static int
loc_struct_report(void *private,
                  void *mem,
                  uint32_t offset,
                  uint64_t id)
{
    return 0;
}

static int
loc_struct_info_get(void *private,
                    uint64_t id,
                    void **mem,
                    uint32_t *offset)
{
    return SHR_E_NOT_FOUND;
}

static const shr_ha_drv_t local_drv = {
    .alloc = loc_alloc,
    .realloc = loc_realloc,
    .free = loc_free,
    .struct_report = loc_struct_report,
    .struct_info = loc_struct_info_get
};

/*******************************************************************************
 * Public functions
 */

void *
shr_ha_mem_alloc(int unit,
                 uint8_t comp_id,
                 uint8_t sub_id,
                 uint32_t *length)
{
    if ((unit < 0) || (unit >= HA_MAX_UNITS) || (!ha_mem[unit])) {
        return NULL;
    }
    return ha_mem[unit]->alloc(ha_inst[unit],
                               comp_id,
                               sub_id,
                               length);
}

void *
shr_ha_mem_realloc(int unit, void *mem, uint32_t length)
{
    if ((unit < 0) || (unit >= HA_MAX_UNITS) || (!ha_mem[unit])) {
        return NULL;
    }
    return ha_mem[unit]->realloc(ha_inst[unit], mem, length);
}

int
shr_ha_mem_free(int unit, void *mem)
{
    if ((unit < 0) || (unit >= HA_MAX_UNITS) || (!ha_mem[unit])) {
        return SHR_E_UNIT;
    }
    return ha_mem[unit]->free(ha_inst[unit], mem);
}

int
shr_ha_struct_report(int unit,
                     void *mem,
                     uint32_t offset,
                     uint64_t id)
{
    if ((unit < 0) || (unit >= HA_MAX_UNITS) || (!ha_mem[unit])) {
        return SHR_E_UNIT;
    }
    return ha_mem[unit]->struct_report(ha_inst[unit], mem, offset, id);
}

int
shr_ha_struct_info_get(int unit,
                       uint64_t id,
                       void **mem,
                       uint32_t *offset)
{
    if ((unit < 0) || (unit >= HA_MAX_UNITS) || (!ha_mem[unit])) {
        return SHR_E_UNIT;
    }
    return ha_mem[unit]->struct_info(ha_inst[unit], id, mem, offset);
}

int
shr_ha_init(int unit,
            void *mmap,
            int size,
            bool enabled,
            bool new_file,
            shr_ha_resize_f resize_f,
            void *ctx)
{
    int rv;

    if ((unit < 0) || (unit >= HA_MAX_UNITS) || (ha_mem[unit])) {
        return SHR_E_UNIT;
    }
    if (enabled) {
        ha_mem[unit] = &shr_ha_drv;
        rv = ha_mem[unit]->create(mmap, size, new_file, resize_f,
                                  ctx, &ha_inst[unit]);
        if (rv != SHR_E_NONE) {
            return rv;
        }
        return ha_mem[unit]->init(ha_inst[unit]);
    }
    else {
        ha_mem[unit] = &local_drv;
        return SHR_E_NONE;
    }
}

int
shr_ha_close(int unit, shr_ha_blk_free_f blk_free_cb)
{
    if (unit < 0 || unit >= HA_MAX_UNITS || !ha_mem[unit]) {
        return SHR_E_UNIT;
    }
    if (ha_mem[unit]->delete) {
        ha_mem[unit]->delete(ha_inst[unit], blk_free_cb);
    }
    ha_mem[unit] = NULL;
    ha_inst[unit] = NULL;
    return SHR_E_NONE;
}


