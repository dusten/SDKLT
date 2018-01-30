/*! \file ser_tcam_scan.c
 *
 * Functions for SER TCAM S/W scan.
 *
 * Include SER TCAM S/W scan.
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

/******************************************************************************
 * Includes
 */
#include <sal/sal_sem.h>
#include <sal/sal_mutex.h>
#include <sal/sal_thread.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>

#include "ser_tcam_scan.h"
#include "ser_bd.h"
#include "ser_config.h"
/******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Maximum number of work per DMA batch read */
#define MAX_NUM_WORK_DMA_BATCH_READ     16

/* Take slice mode mutex if TCAM has  */
#define SLICE_MODE_MUTEX_TAKE(slice_mode_mutex) \
    do { \
        if (slice_mode_mutex != NULL) {  \
            (void)sal_mutex_take(slice_mode_mutex, SAL_MUTEX_FOREVER); \
        } \
    } while(0)

/* give slice mode mutex if TCAM has  */
#define SLICE_MODE_MUTEX_GIVE(slice_mode_mutex) \
    do { \
        if (slice_mode_mutex != NULL) {  \
            (void)sal_mutex_give(slice_mode_mutex); \
        } \
    } while(0)

/*!
  * \brief Count of entries of one PT
  */
#define BCMPTM_SER_INDEX_COUNT(unit, sid) \
    (1 + bcmdrd_pt_index_max(unit, sid) - bcmdrd_pt_index_min(unit, sid))

#define INT_TO_PTR(x)    ((void *)(unsigned long)(x))
#define PTR_TO_INT(x)    ((unsigned long)(x))

/******************************************************************************
 * Typedefs
 */

/*! Cache table-specific info to avoid recalculating on each pass */
typedef struct bcmptm_ser_mem_scan_table_info_s {
    /*! SID */
    bcmdrd_sid_t mem;
    /*! read only during runtime */
    uint32_t     ser_flags;
    /*! min index */
    int          index_min;
    /*! max index */
    int          index_max;
    /*! can change during run-time */
    uint32_t     ser_dynamic_state;
} bcmptm_ser_mem_scan_table_info_t;

/*! Information can be used to help Tcam scan */
typedef struct bcmptm_ser_mem_scan_info_s {
    /*! tcam num */
    int num_tables;
    /*! information of every tcam */
    bcmptm_ser_mem_scan_table_info_t *table_info;
    /*! interval semaphore */
    sal_sem_t  tcam_scan_interval;
    /*! tcam scan dma buffer */
    uint32_t   *tcam_scan_buf;
    /*! physical address of dma buffer */
    uint64_t   tcam_scan_buf_paddr;
    /*! tcam scan dma size */
    uint32_t   tcam_scan_buf_size;
    /*! Flag whether tcam thread is stopped or not */
    bool       tcam_scan_stopped;
    /*! Notify tcam scan thread had been stopped */
    sal_sem_t  tcam_scan_stop_notify;
    /*! Tcam scan thread ID */
    sal_thread_t tcam_scan_thread_id;
} bcmptm_ser_mem_scan_info_t;

/******************************************************************************
 * Private variables
 */
/*! Cache used to save scanning information for all TCAMs */
static bcmptm_ser_mem_scan_info_t *tcam_scan_info[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Defines
 */

/*! Cache table-specific info to avoid recalculating on each pass */
#define TCAM_MEM_SCAN_TABLE_INFO(unit, table_index) \
    (&(tcam_scan_info[unit]->table_info[table_index]))

/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 * bcmptm_ser_tcam_index_range_get, used to get valid index ranges
 */
static int
bcmptm_ser_tcam_index_range_get(int unit, bcmptm_ser_mem_scan_table_info_t *table_info,
                     int begin_index, int chunk_size, int max_range_num,
                     int *start_indexes, int *index_ranges, int *actual_range_num,
                     int *index_scan_num, int *begin_index_next)
{
    bcmdrd_sid_t mem = INVALIDm;
    int rv = SHR_E_NONE;
    int index_start = 0, index_count = 0;
    int i = 0, index_range_sum = 0;

    mem = table_info->mem;

    *begin_index_next = table_info->index_max + 1;
    *index_scan_num = 0;
    *actual_range_num = 0;

    index_count = 0;
    index_range_sum = 0;
    index_count = 1;
    for (i = begin_index; i <= table_info->index_max; ) {
        index_start = i;
        if (table_info->ser_flags & BCMPTM_SER_FLAG_OVERLAY) {
            /* L3_DEFIPm or L3_DEFIP_PAIR_128m */
            rv = bcmptm_ser_overlay_index_stride_get(unit, mem,
                                                     index_start, &index_count);
            if (SHR_FAILURE(rv)) {
                /* There is failure, skip scan this memory */
                *begin_index_next = table_info->index_max + 1;
                break;
            } else if (index_count >= 1) {
                if (index_count > chunk_size) {
                    index_count = chunk_size;
                }
                start_indexes[0] = index_start;
                index_ranges[0] = index_count;
                *actual_range_num = 1;
                *begin_index_next = index_start + index_count;
                *index_scan_num = index_count;
                break;
            }
            i += 1;
        } else {
            /* tcam has slices or tcam has no slices*/
            rv = bcmptm_ser_slice_index_adjust(unit, mem,
                                               &index_start, &index_count);
            if (rv == SHR_E_NOT_FOUND) {
                /* tcam has no slices */
                index_count = table_info->index_max - index_start + 1;
                rv = SHR_E_NONE;
            }
            if (index_count <= 0 || SHR_FAILURE(rv)) {
                /* all valid indexes are scanned, index_start = max_index + 1 */
                *begin_index_next = table_info->index_max + 1;
                break;
            } else if (index_count > chunk_size) {
                index_count = chunk_size;
            }
            index_range_sum += index_count;
            if (index_range_sum > chunk_size) {
                break;
            }
            *index_scan_num = index_range_sum;
            start_indexes[*actual_range_num] = index_start;
            index_ranges[*actual_range_num] = index_count;
            *begin_index_next = index_start + index_count;
            (*actual_range_num)++;
            if (*actual_range_num >= max_range_num) {
                break;
            }
            i = *begin_index_next;
        }
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_tcam_batch_scan, used to scan TCAM memory tables by S/W
 */
static int
bcmptm_ser_tcam_batch_scan(int unit, bcmptm_ser_mem_scan_table_info_t *table_info,
                           int pipe_no, int begin_index, int chunk_size, uint64_t buf_paddr,
                           int *index_num_scanned, int *begin_index_next)
{
    int actual_range_num = 0, rv = SHR_E_NONE;
    int start_indexes[MAX_NUM_WORK_DMA_BATCH_READ];
    int index_ranges[MAX_NUM_WORK_DMA_BATCH_READ], retry_time = 0;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];

retry:
    /* value of index_num_scanned may be less than value of chunk_size */
    rv = bcmptm_ser_tcam_index_range_get(unit, table_info,
                                         begin_index, chunk_size,
                                         MAX_NUM_WORK_DMA_BATCH_READ,
                                         start_indexes, index_ranges,
                                         &actual_range_num,
                                         index_num_scanned,
                                         begin_index_next);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get indexes to be scanned!\n")));
        return rv;
    }
    if (actual_range_num > 0) {
        if (*index_num_scanned == 1) {
            /* only one entry need to be read */
            rv = bcmptm_ser_pt_read(unit, table_info->mem, start_indexes[0], pipe_no,
                                    NULL, entry_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        } else {
            rv = bcmptm_ser_batch_sbusdma_read(unit, table_info->mem, pipe_no,
                                               NULL, NULL, start_indexes,
                                               index_ranges, actual_range_num,
                                               buf_paddr, 0);
        }
        if (SHR_SUCCESS(rv)) {
            return rv;
        } else if (retry_time < 10) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "SER error is triggered.\n")));
            rv = SHR_E_NONE;
            retry_time++;
            /* wait for SERC thread to correct SER error */
            sal_usleep(1000000);
            /*
            * there may be multiple ser errors in DMA range,
            * re-scan this DMA range to trigger other SER errors.
            */
            goto retry;
        } else {
            LOG_FATAL(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to read TCAM[%s] again and again: %d\n"),
                       bcmdrd_pt_sid_to_name(unit, table_info->mem), rv));
            rv = SHR_E_NONE;
        }
    }
    return rv;
}

/******************************************************************************
 * bcmptm_ser_tcam_scan_buff_adjust, user can modify "tcam scan chunk size" automatically.
 */
static void
bcmptm_ser_tcam_scan_buff_adjust(int unit, uint32_t new_size)
{
    uint64_t dma_addr = 0;
    uint32_t *read_buf = NULL;
    bcmptm_ser_mem_scan_info_t *scan_info = tcam_scan_info[unit];

    if (scan_info->tcam_scan_buf_size == new_size) {
        return;
    }
    if (scan_info->tcam_scan_buf != NULL) {
        read_buf = scan_info->tcam_scan_buf;
        dma_addr = scan_info->tcam_scan_buf_paddr;
        bcmdrd_hal_dma_free(unit, 0, read_buf, dma_addr);
        scan_info->tcam_scan_buf = NULL;
        scan_info->tcam_scan_buf_size = 0;
        scan_info->tcam_scan_buf_paddr = 0;
    }
    /* used to free old DMA buffer */
    if (new_size == 0) {
        return;
    }
    scan_info->tcam_scan_buf_size = new_size;
    read_buf = bcmdrd_hal_dma_alloc(unit, new_size, "memscan", &dma_addr);
    if (read_buf == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to allocate tcam scan read buffer,"
                             " size of buffer is [%d]\n"),
                  new_size));
        scan_info->tcam_scan_buf_size = 0;
        dma_addr = 0;
    }
    scan_info->tcam_scan_buf_paddr = dma_addr;
    /* can be NULL */
    scan_info->tcam_scan_buf = read_buf;
    return;
}

/******************************************************************************
* bcmptm_ser_tcam_scan_info_deinit: release cache used to save scanning information for all TCAMs
 */
static void
bcmptm_ser_tcam_scan_info_deinit(int unit)
{
    bcmptm_ser_mem_scan_info_t *scan_info_ptr = NULL;

    if (tcam_scan_info[unit] == NULL) {
        return;
    }
    scan_info_ptr = tcam_scan_info[unit];
    tcam_scan_info[unit] = NULL;

    if (scan_info_ptr->table_info != NULL) {
        sal_free(scan_info_ptr->table_info);
        scan_info_ptr->table_info = NULL;
    }

    if (scan_info_ptr->tcam_scan_interval) {
        sal_sem_destroy(scan_info_ptr->tcam_scan_interval);
        scan_info_ptr->tcam_scan_interval = NULL;
    }
    if (scan_info_ptr->tcam_scan_stop_notify) {
        sal_sem_destroy(scan_info_ptr->tcam_scan_stop_notify);
        scan_info_ptr->tcam_scan_stop_notify = NULL;
    }
    sal_free(scan_info_ptr);
    scan_info_ptr = NULL;
}

/******************************************************************************
* bcmptm_ser_tcam_scan_info_init: init cache used to save scanning information for all TCAMs
 */
static int
bcmptm_ser_tcam_scan_info_init(int unit)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL;
    int tcam_info_num = 0, rv = SHR_E_NONE, alloc_size = 0;
    bcmptm_ser_mem_scan_info_t *scan_info_ptr = NULL;
    int ser_idx = 0;
    bcmptm_ser_mem_scan_table_info_t *table_info = NULL;
    bcmdrd_sid_t mem_sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

    if (tcam_scan_info[unit] != NULL) {
        return SHR_E_NONE;
    }
    alloc_size = sizeof(bcmptm_ser_mem_scan_info_t);
    scan_info_ptr = sal_alloc(alloc_size, "tcam scan info");
    SHR_NULL_CHECK(scan_info_ptr, SHR_E_MEMORY);

    tcam_scan_info[unit] = scan_info_ptr;
    sal_memset(scan_info_ptr, 0, alloc_size);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, tcam_info_get_fn)(unit, &tcam_ser_info, &tcam_info_num);
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    scan_info_ptr->num_tables = tcam_info_num;
    alloc_size = tcam_info_num * sizeof(bcmptm_ser_mem_scan_table_info_t);
    scan_info_ptr->table_info = sal_alloc(alloc_size, "tcam scan table info");
    if ( scan_info_ptr->table_info == NULL) {
        sal_free(scan_info_ptr);
        scan_info_ptr = NULL;
        SHR_NULL_CHECK(NULL, SHR_E_MEMORY);
    }
    sal_memset(scan_info_ptr->table_info, 0, alloc_size);

    for (ser_idx = 0; ser_idx < scan_info_ptr->num_tables; ser_idx++) {
        table_info = &scan_info_ptr->table_info[ser_idx];
        mem_sid = tcam_ser_info[ser_idx].mem;
        table_info->ser_flags = tcam_ser_info[ser_idx].ser_flags;
        table_info->ser_dynamic_state = tcam_ser_info[ser_idx].ser_dynamic_state;

        table_info->mem = mem_sid;
        table_info->index_max = bcmdrd_pt_index_max(unit, mem_sid);
        table_info->index_min = bcmdrd_pt_index_min(unit, mem_sid);
    }
    scan_info_ptr->tcam_scan_interval = sal_sem_create("memscan timer", SAL_SEM_BINARY, 0);
    scan_info_ptr->tcam_scan_stop_notify = sal_sem_create("memscan stop notify", SAL_SEM_BINARY, 0);
    if (scan_info_ptr->tcam_scan_interval == NULL ||
        scan_info_ptr->tcam_scan_stop_notify == NULL) {
        bcmptm_ser_tcam_scan_info_deinit(unit);
        SHR_NULL_CHECK(NULL, SHR_E_MEMORY);
    }
    scan_info_ptr->tcam_scan_thread_id = SAL_THREAD_ERROR;
    scan_info_ptr->tcam_scan_stopped = TRUE;

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to init tcam scan information!\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_tcam_scan_thread, used to scan TCAM memory tables by S/W
 */
static void
bcmptm_ser_tcam_scan_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    uint32_t interval = 0;
    int index_num_not_scanned, index_num_scanned;
    uint32_t chunk_size_old = 0, chunk_size_new, chunk_byte_size = 0;
    int num_tables = 0, ser_idx = 0;
    bcmptm_ser_mem_scan_table_info_t *table_info = NULL;
    bcmdrd_sid_t mem = INVALIDm;
    sal_mutex_t slice_mode_mutex;
    int rv = SHR_E_NONE, pipe_no = -1, pipe_num = 0, skip_num = 0;
    int begin_index = 0, begin_index_next = 0;
    uint64_t buf_paddr = 0;
    bool     tcam_scan_stopped = TRUE;
    bcmptm_ser_mem_scan_info_t *scan_info_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    scan_info_ptr = tcam_scan_info[unit];

    tcam_scan_stopped = scan_info_ptr->tcam_scan_stopped;
    if (tcam_scan_stopped) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    index_num_not_scanned = BCMPTM_SER_CONTROL_TCAM_ENTRIES_READ_PER_INTERVAL(unit);
    chunk_size_old = BCMPTM_SER_CONTROL_TCAM_SCAN_CHUNK_SIZE(unit);
    chunk_byte_size = chunk_size_old * sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE;
    /* allocate dma read buffer */
    bcmptm_ser_tcam_scan_buff_adjust(unit, chunk_byte_size);
    buf_paddr = scan_info_ptr->tcam_scan_buf_paddr;
    if (buf_paddr == 0) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    num_tables = scan_info_ptr->num_tables;
    ser_idx = 0;
    for (; ; ser_idx++) {
        /* Nothing needs to be scanned */
        if (skip_num >= num_tables) {
            rv = SHR_E_NONE;
            SHR_RETURN_VAL_EXIT(rv);
        }
        /* new round */
        if (ser_idx == num_tables) {
            skip_num = 0;
            ser_idx = 0;
        }
        table_info = TCAM_MEM_SCAN_TABLE_INFO(unit, ser_idx);
        mem = table_info->mem;
        if (BCMPTM_SER_CONTROL_PT_ECC_PARITY_CHECK(unit, mem) == 0 ||
            BCMPTM_SER_CONTROL_PT_SW_SCAN(unit, mem) == 0) {
            skip_num++;
            continue;
        }
        if (0 == (table_info->ser_dynamic_state & BCMPTM_SER_FLAG_SER_ENGINE)) {
            skip_num++;
            continue;
        }
        if (((table_info->ser_flags & BCMPTM_MEM_ADDR_ACC_TYPE_MASK) !=
              BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0) &&
             ((table_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK) == 0)) {
            skip_num++;
            continue;
        }
        pipe_num = 0;
        if (table_info->ser_flags & BCMPTM_SER_FLAG_MULTI_PIPE) {
            rv = bcmptm_ser_tbl_inst_num_get(unit, mem, &pipe_num);
            SHR_IF_ERR_EXIT(rv);
        }
        rv = bcmptm_ser_slice_mode_mutex_get(unit, mem, &slice_mode_mutex);
        if (SHR_FAILURE(rv)) {
            slice_mode_mutex = NULL;
        }
        begin_index = table_info->index_min;
        pipe_no = 0;
        while(pipe_no < pipe_num) {
            SLICE_MODE_MUTEX_TAKE(slice_mode_mutex);

            rv = bcmptm_ser_tcam_batch_scan(unit, table_info, pipe_no,
                                            begin_index, chunk_size_old, buf_paddr,
                                            &index_num_scanned,
                                            &begin_index_next);
            SLICE_MODE_MUTEX_GIVE(slice_mode_mutex);
            SHR_IF_ERR_EXIT(rv);

            tcam_scan_stopped = scan_info_ptr->tcam_scan_stopped;
            if (tcam_scan_stopped) {
                rv = SHR_E_NONE;
                SHR_RETURN_VAL_EXIT(rv);
            }
            index_num_not_scanned -= index_num_scanned;
            if (index_num_not_scanned <= 0) {
                /* interval should be more than 1000000, refer to definition of LT SER_CONTROLt */
                interval = BCMPTM_SER_CONTROL_TCAM_SCAN_INTERVAL(unit);
                /* Fall asleep */
                (void)sal_sem_take(scan_info_ptr->tcam_scan_interval, interval);
                /* wake up */
                /* update latest data from LT SER_CONTROLt */
                chunk_size_new = BCMPTM_SER_CONTROL_TCAM_SCAN_CHUNK_SIZE(unit);
                if (chunk_size_new != chunk_size_old) {
                    chunk_byte_size = chunk_size_new * sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE;
                    /* adjust size of dma read buffer */
                    bcmptm_ser_tcam_scan_buff_adjust(unit, chunk_byte_size);
                    buf_paddr = scan_info_ptr->tcam_scan_buf_paddr;
                    /* Fail to re-allocate DMA read buffer */
                    if (buf_paddr == 0) {
                        rv = SHR_E_MEMORY;
                        SHR_IF_ERR_EXIT(rv);
                    }
                    chunk_size_old = chunk_size_new;
                }
                index_num_not_scanned = BCMPTM_SER_CONTROL_TCAM_ENTRIES_READ_PER_INTERVAL(unit);
                /* Maybe need to exit */
                tcam_scan_stopped = scan_info_ptr->tcam_scan_stopped;
                if (tcam_scan_stopped) {
                    rv = SHR_E_NONE;
                    SHR_RETURN_VAL_EXIT(rv);
                }
            }
            begin_index = begin_index_next;
            if (begin_index > table_info->index_max) {
                pipe_no++;
                begin_index = table_info->index_min;
            }
        }
    }

exit:
    if (0 == SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " There is failure in TCAM memory scan thread!\n")));
    }
    /* need to free tcam_scan_info[unit]->tcam_scan_buf */
    bcmptm_ser_tcam_scan_buff_adjust(unit, 0);
    scan_info_ptr->tcam_scan_thread_id = SAL_THREAD_ERROR;
    scan_info_ptr->tcam_scan_stopped = TRUE;
    (void)sal_sem_give(scan_info_ptr->tcam_scan_stop_notify);
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         " TCAM memory scan thread exited!\n")));
    sal_thread_exit(0);
}

/******************************************************************************
 * Public Functions
 */


/******************************************************************************
* bcmptm_ser_tcam_scan_start: start tcam scan thread. If user wants to stop this thread,
* user can configure SER_CONTROLr to do that.
 */
int
bcmptm_ser_tcam_scan_start(int unit)
{
    sal_thread_t thread_id = SAL_THREAD_ERROR;
    uint32_t tcam_scan_enable = BCMPTM_SER_CONTROL_TCAM_SCAN(unit);
    bcmptm_ser_mem_scan_info_t *scan_info = NULL;
    int rv = SHR_E_NONE;

    if (tcam_scan_enable == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " TCAM_SCANf of SER_CONTROLt is disable,"
                             " so TCAM scan thread is not started!\n")));
        return SHR_E_NONE;
    }
    rv = bcmptm_ser_tcam_scan_info_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    scan_info = tcam_scan_info[unit];
    if (scan_info->tcam_scan_stopped == FALSE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " TCAM scan thread have been started or is starting!\n")));
        return SHR_E_NONE;
    } else if (scan_info->tcam_scan_thread_id != SAL_THREAD_ERROR) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " TCAM scan thread is exiting!\n")));
        return SHR_E_NONE;
    }
    scan_info->tcam_scan_stopped = FALSE;
    thread_id = sal_thread_create("SER TCAM thread",
                                  SAL_THREAD_STKSZ,
                                  50,
                                  bcmptm_ser_tcam_scan_thread,
                                  INT_TO_PTR(unit));
    if (thread_id == SAL_THREAD_ERROR) {
        scan_info->tcam_scan_stopped = TRUE;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to create TCAM scan thread!\n")));
        return SHR_E_MEMORY;
    } else {
        scan_info->tcam_scan_thread_id = thread_id;
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " TCAM scan thread is started!\n")));
    }

    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_tcam_scan_stop: stop tcam scan thread.
 */
int
bcmptm_ser_tcam_scan_stop(int unit, int free_resource)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_mem_scan_info_t *scan_info = tcam_scan_info[unit];

    if (scan_info == NULL || scan_info->tcam_scan_stopped == TRUE) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " TCAM scan thread have been stopped or is exiting!\n")));
        return SHR_E_NONE;
    }
    scan_info->tcam_scan_stopped = TRUE;
    /* notify tcam thread to exit */
    (void)sal_sem_give(scan_info->tcam_scan_interval);

    do {
        rv = sal_sem_take(scan_info->tcam_scan_stop_notify, 1000000);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " TCAM scan thread is exiting!\n")));
        }
    } while (SHR_FAILURE(rv));
    if (free_resource) {
        bcmptm_ser_tcam_scan_info_deinit(unit);
    }
    return rv;
}

