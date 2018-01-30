/*! \file rm_alpm_bucket.c
 *
 * Resource Manager for ALPM Level-N bucket
 *
 * This file contains the resource manager for ALPM Level-N bucket
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
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_util.h>

#include "rm_alpm_common.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include "rm_alpm_device.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_level1.h"


/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE              BSL_LS_BCMPTM_RMALPM

#define BKT_MGMT(u)             (bucket_mgmt[u])
#define BKT_MGMT_EXEC(u, fn)    (BKT_MGMT(u)->fn)

/*******************************************************************************
 * Private Functions (declaration)
 */

static bucket_mgmt_t *bucket_mgmt[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_bkt_hdl_init(int u, bool recover)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bkt_hdl_init)) {
        return BKT_MGMT_EXEC(u, bkt_hdl_init)(u, recover);
    }
    return SHR_E_UNAVAIL;
}

void
bcmptm_rm_alpm_bkt_hdl_cleanup(int u)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bkt_hdl_cleanup)) {
        BKT_MGMT_EXEC(u, bkt_hdl_cleanup)(u);
    }
}

int
bcmptm_rm_alpm_bkt_hdl_get(int u, int db, int ln, bkt_hdl_t **bkt_hdl)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bkt_hdl_get)) {
        return BKT_MGMT_EXEC(u, bkt_hdl_get)(u, db, ln, bkt_hdl);
    }
    return SHR_E_UNAVAIL;
}

void
bcmptm_rm_alpm_adata_encode(int u, int ln, alpm_arg_t *arg, void *adata)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, alpm_data_encode)) {
        BKT_MGMT_EXEC(u, alpm_data_encode)(u, ln, arg, adata);
    }
}

void
bcmptm_rm_alpm_bucket_pivot_set(int u, int db, int ln, log_bkt_t group,
                                alpm_pivot_t *pivot)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_pivot_set)) {
        BKT_MGMT_EXEC(u, bucket_pivot_set)(u, db, ln, group, pivot);
    }
}

alpm_pivot_t *
bcmptm_rm_alpm_bucket_pivot_get(int u, int db, int ln, ln_index_t ln_index)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_pivot_get)) {
        return BKT_MGMT_EXEC(u, bucket_pivot_get)(u, db, ln, ln_index);
    }
    return NULL;
}


int
bcmptm_rm_alpm_bucket_free(int u, int db, int ln, log_bkt_t group)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_free)) {
        return BKT_MGMT_EXEC(u, bucket_free)(u, db, ln, group);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_alloc(int u, int ln, alpm_arg_t *arg, log_bkt_t *group)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_alloc)) {
        return BKT_MGMT_EXEC(u, bucket_alloc)(u, ln, arg, group);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_delete(int u, int db, int ln, alpm_pivot_t *pivot,
                             ln_index_t ent_idx)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_delete)) {
        return BKT_MGMT_EXEC(u, bucket_delete)(u, db, ln, pivot, ent_idx);
    }
    return SHR_E_UNAVAIL;

}
int
bcmptm_rm_alpm_bucket_insert(int u, int ln, alpm_arg_t *arg,
                             alpm_pivot_t *pivot)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_insert)) {
        return BKT_MGMT_EXEC(u, bucket_insert)(u, ln, arg, pivot);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_split(int u, int ln, alpm_arg_t *arg,
                            alpm_pivot_t *parent_pivot,
                            bkt_split_aux_t *split_aux)

{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_split)) {
        return BKT_MGMT_EXEC(u, bucket_split)(u, ln, arg, parent_pivot, split_aux);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_recover(int u, int ln, alpm_arg_t *arg)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_recover)) {
        return BKT_MGMT_EXEC(u, bucket_recover)(u, ln, arg);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_bucket_recover_done(int u, int db, int ln)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_recover_done)) {
        return BKT_MGMT_EXEC(u, bucket_recover_done)(u, db, ln);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_entry_read(int u, int ln, alpm_arg_t *arg)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, entry_read)) {
        return BKT_MGMT_EXEC(u, entry_read)(u, ln, arg);
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_entry_hw_idx_get(int u, int db, int ln, ln_index_t ent_idx,
                                uint32_t *hw_idx)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, entry_hw_idx_get)) {
        *hw_idx = BKT_MGMT_EXEC(u, entry_hw_idx_get)(u, db, ln, ent_idx);
        return SHR_E_NONE;
    }
    return SHR_E_UNAVAIL;
}

int
bcmptm_rm_alpm_entry_write(int u, int ln, alpm_arg_t *arg)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, entry_write)) {
        return BKT_MGMT_EXEC(u, entry_write)(u, ln, arg);
    }
    return SHR_E_UNAVAIL;

}

void
bcmptm_rm_alpm_bucket_sanity(int u, int db, int ln, log_bkt_t log_bkt)
{
    if (BKT_MGMT(u) && BKT_MGMT_EXEC(u, bucket_sanity)) {
        BKT_MGMT_EXEC(u, bucket_sanity)(u, db, ln, log_bkt);
    }
}

void
bcmptm_rm_alpm_all_buckets_sanity(int u, int db, int ln)
{
#if 0
    bkt_hdl_t *bkt_hdl;
    shr_dq_t *elem;
    rbkt_t *curr;
    int count = 0;
    int v_rbkts = 0, f_rbkts = 0;
    bcmptm_rm_alpm_bkt_hdl_get(u, db, ln, &bkt_hdl);
    SHR_DQ_TRAVERSE(&bkt_hdl->global_group_list, elem)
        curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        v_rbkts += curr->valid_banks;
        f_rbkts += curr->free_banks;
        bcmptm_rm_alpm_bucket_sanity(u, db, ln, curr->index);
        count++;
    SHR_DQ_TRAVERSE_END(&bkt_hdl->global_group_list, elem);
    assert(v_rbkts == bkt_hdl->rbkts_used);
    assert(count == bkt_hdl->groups_used);
#endif
}

int
bcmptm_rm_alpm_bucket_init(int u)
{
    alpm_dev_info_t *dev_info;
    SHR_FUNC_ENTER(u);

    ALPM_ALLOC(bucket_mgmt[u], sizeof(bucket_mgmt_t), "Bucket callbacks");
    sal_memset(bucket_mgmt[u], 0, sizeof(bucket_mgmt_t));

    dev_info = bcmptm_rm_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    switch (dev_info->alpm_ver) {
        case ALPM_VERSION_1:
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_ver1_register(u, bucket_mgmt[u]));
            break;
        case ALPM_VERSION_0:
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_ver0_register(u, bucket_mgmt[u]));
            break;
        default:
            SHR_RETURN(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_bucket_cleanup(int u)
{
    SHR_FREE(bucket_mgmt[u]);
}



