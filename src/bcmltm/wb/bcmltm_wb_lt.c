/*! \file bcmltm_wb_lt.c
 *
 * Logical Table Manager - Logical Table Working Buffer.
 *
 * This file contains the working buffer information
 * for Logical Tables, such as the working buffer handles
 * for the logical tables.
 *
 * The working buffer handle is needed through the metadata construction
 * of a given table in order to properly assign the offsets into
 * the working buffer space used during LTM operation, FA and EE stages.
 *
 * The assumption (throughout the LTM component) is that the
 * logical table IDs (LRD symbold ID) are defined in a compact numbering
 * space [0.. n-1], where 'n' is the total number of LRD symbols.
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

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Helper macros
 */
/* Check that info list has been initialized */
#define WB_INFO_INIT(_u)                        \
    (WB_INFO(_u) != NULL)

/* Check that symbol id is valid */
#define WB_INFO_SID_VALID(_u, _sid)             \
    ((_sid) < WB_INFO_SID_MAX_COUNT(_u))

/* Check that unit and symbol id are valid */
#define WB_INFO_VALID(_u, _sid)                         \
    (WB_INFO_INIT(_u) && WB_INFO_SID_VALID(_u, _sid))


/*
 * Accessor macros for Logical Tables info
 */
#define WB_INFO(_u)                      wb_info[_u]
#define WB_INFO_SID_MAX_COUNT(_u)        WB_INFO(_u)->sid_max_count
#define WB_INFO_HANDLE(_u, _sid)         WB_INFO(_u)->handle[_sid]


/*!
 * \brief Working Buffer Information.
 */
typedef struct wb_info_list_s {
    /*! Total number of symbols. */
    uint32_t sid_max_count;

    /*!
     * Array of Working Buffer Handler Pointers.
     *
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    bcmltm_wb_handle_t *handle[];
} wb_info_list_t;


/* Logical Table Working Buffer Information */
static wb_info_list_t *wb_info[BCMLTM_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */
static int
lt_info_valid(int unit, uint32_t sid)
{
    SHR_FUNC_ENTER(unit);

    if (!WB_INFO_INIT(unit)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM working buffer list has been "
                              "not initialized: sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    if (!WB_INFO_SID_VALID(unit, sid)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM working buffer: invalid sid=%d "
                              "(max_sid=%d)\n"),
                   sid, WB_INFO_SID_MAX_COUNT(unit) - 1));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create a working buffer handle for given table ID.
 *
 * This routine allocates and initializes a working buffer handle
 * for the specified table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_wb_lt_handle_create(int unit, uint32_t sid)
{
    bcmltm_wb_handle_t *handle = NULL;

    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (WB_INFO_HANDLE(unit, sid) != NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM working buffer handle is already "
                              "created: sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    SHR_ALLOC(handle, sizeof(*handle), "LTM LT working buffer handle");
    SHR_NULL_CHECK(handle, SHR_E_MEMORY);
    sal_memset(handle, 0, sizeof(*handle));

    /* Initialize handle */
    SHR_IF_ERR_EXIT(bcmltm_wb_handle_init(sid, handle));

    WB_INFO_HANDLE(unit, sid) = handle;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_wb_handle_cleanup(handle);
        SHR_FREE(handle);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy  working buffer handle for given table ID.
 *
 * This routine frees any memory allocated during the
 * construction of the working buffer handle for the specified table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
bcmltm_wb_lt_handle_destroy(int unit, uint32_t sid)
{
    if (!WB_INFO_VALID(unit, sid)) {
        return;
    }

    if (WB_INFO_HANDLE(unit, sid) == NULL) {
        return;
    }

    /* Cleanup handle */
    bcmltm_wb_handle_cleanup(WB_INFO_HANDLE(unit, sid));

    /* Free handle */
    SHR_FREE(WB_INFO_HANDLE(unit, sid));
    WB_INFO_HANDLE(unit, sid) = NULL;

    return;
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_lt_info_init(int unit, uint32_t sid_max_count)
{
    wb_info_list_t *info = NULL;
    unsigned int size;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    size_t sid;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (WB_INFO_INIT(unit)) {
        SHR_EXIT();
    }

    /* Allocate */
    size = sizeof(*info) + (sizeof(bcmltm_wb_handle_t*) * sid_max_count);
    SHR_ALLOC(info, size, "LTM Logical Table Working Buffer info");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, size);
    WB_INFO(unit) = info;

    /* Initialize information */
    info->sid_max_count = sid_max_count;

     /* Allocate sid list array */
    size = sizeof(*sid_list) * sid_max_count;
    SHR_ALLOC(sid_list, size, "LTM Logical Table sid_list");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    rv = bcmlrd_table_list_get(unit, sid_max_count, sid_list, &num_sid);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            /* No tables are defined */
            num_sid = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Could not get table ID list rv=%d\n"),
                       rv));
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    /* Create WB handle for each table */
    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];

        /* Create WB handle */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmltm_wb_lt_handle_create(unit, sid), SHR_E_UNAVAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_wb_lt_info_cleanup(unit);
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}


void
bcmltm_wb_lt_info_cleanup(int unit)
{
    uint32_t sid;
    uint32_t sid_max;

    if (!WB_INFO_INIT(unit)) {
        return;
    }

    /*
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    sid_max = WB_INFO_SID_MAX_COUNT(unit) - 1;

    /* Destroy internal information */
    for (sid = 0; sid <= sid_max; sid++) {
        /* LRD symbol ID is the same */
        bcmltm_wb_lt_handle_destroy(unit, sid);
    }

    SHR_FREE(WB_INFO(unit));
    WB_INFO(unit) = NULL;

    return;
}


bcmltm_wb_handle_t *
bcmltm_wb_lt_handle_get(int unit, uint32_t sid)
{
    /* Sanity check */
    if (!WB_INFO_VALID(unit, sid)) {
        return NULL;
    }

    return WB_INFO_HANDLE(unit, sid);
}


const bcmltm_wb_block_t *
bcmltm_wb_lt_block_get(int unit, uint32_t sid,
                       bcmltm_wb_block_id_t block_id)
{
    /* Sanity check */
    if (!WB_INFO_VALID(unit, sid)) {
        return NULL;
    }

    return bcmltm_wb_block_find(WB_INFO_HANDLE(unit, sid), block_id);
}


int
bcmltm_wb_lt_wsize_get(int unit, uint32_t sid,
                       uint32_t *size)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *size = bcmltm_wb_wsize_get(WB_INFO_HANDLE(unit, sid));

 exit:
    SHR_FUNC_EXIT();
}

