/*! \file ser_sram_scan.c
 *
 * Functions for SER SRAM S/W scan.
 *
 * Include SER SRAM S/W scan.
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
#include <sal/sal_thread.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <sal/sal_libc.h>

#include "ser_sram_scan.h"
#include "ser_tcam_scan.h"
#include "ser_bd.h"
#include "ser_config.h"
/******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER


#define INT_TO_PTR(x)    ((void *)(unsigned long)(x))
#define PTR_TO_INT(x)    ((unsigned long)(x))

/******************************************************************************
 * Typedefs
 */
/*! Information can be used to help sram scan */
typedef struct bcmptm_ser_sram_scan_info_s {
    /*! interval semaphore */
    sal_sem_t  sram_scan_interval;
    /*! sram scan dma buffer */
    uint32_t   *sram_scan_buf;
    /*! physical address of dma buffer */
    uint64_t   sram_scan_buf_paddr;
    /*! sram scan dma size */
    uint32_t   sram_scan_buf_size;
    /*! Flag whether sram thread is stopped or not */
    bool       sram_scan_stopped;
    /*! Notify sram scan thread had been stopped */
    sal_sem_t  sram_scan_stop_notify;
    /*! Sram scan thread ID */
    sal_thread_t sram_scan_thread_id;
    /*! Sram scan thread will scan this PT firstly */
    bcmdrd_sid_t begin_sid;
} bcmptm_ser_sram_scan_info_t;

/******************************************************************************
 * Private variables
 */
/*! Cache used to save sram scanning information */
static bcmptm_ser_sram_scan_info_t *sram_scan_info[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private Functions
 */
/******************************************************************************
 * bcmptm_ser_sram_scan_buff_adjust, user can modify "sram scan chunk size" automatically.
 */
static void
bcmptm_ser_sram_scan_buff_adjust(int unit, uint32_t new_size)
{
    uint64_t dma_addr = 0;
    uint32_t *read_buf = NULL;
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];

    if (scan_info->sram_scan_buf_size == new_size) {
        return;
    }
    if (scan_info->sram_scan_buf != NULL) {
        read_buf = scan_info->sram_scan_buf;
        dma_addr = scan_info->sram_scan_buf_paddr;
        bcmdrd_hal_dma_free(unit, 0, read_buf, dma_addr);
        scan_info->sram_scan_buf = NULL;
        scan_info->sram_scan_buf_size = 0;
        scan_info->sram_scan_buf_paddr = 0;
    }
    /* used to free old DMA buffer */
    if (new_size == 0) {
        return;
    }
    scan_info->sram_scan_buf_size = new_size;
    read_buf = bcmdrd_hal_dma_alloc(unit, new_size, "memscan", &dma_addr);
    if (read_buf == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to allocate sram scan read buffer,"
                             " size of buffer is [%d]\n"),
                  new_size));
        scan_info->sram_scan_buf_size = 0;
        dma_addr = 0;
    }
    scan_info->sram_scan_buf_paddr = dma_addr;
    /* can be NULL */
    scan_info->sram_scan_buf = read_buf;
    return;
}
/******************************************************************************
* bcmptm_ser_sram_scan_info_deinit: free cache used to save sram scanning information
 */
static void
bcmptm_ser_sram_scan_info_deinit(int unit)
{
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];

    if (scan_info == NULL) {
        return;
    }
    sram_scan_info[unit] = NULL;
    if (scan_info->sram_scan_interval != NULL) {
        sal_sem_destroy(scan_info->sram_scan_interval);
        scan_info->sram_scan_interval = NULL;
    }
    if (scan_info->sram_scan_stop_notify != NULL) {
        sal_sem_destroy(scan_info->sram_scan_stop_notify);
        scan_info->sram_scan_stop_notify = NULL;
    }
    sal_free(scan_info);
    scan_info = NULL;
}

/******************************************************************************
* bcmptm_ser_sram_scan_info_init: init cache used to save sram scanning information
 */
static int
bcmptm_ser_sram_scan_info_init(int unit)
{
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];
    int rv = SHR_E_NONE, reg_num = 1;
    size_t alloc_size = 0;
    bcmptm_ser_ctrl_reg_info_t *iss_bank_config_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (scan_info != NULL) {
        return SHR_E_NONE;
    }
    alloc_size = sizeof(bcmptm_ser_sram_scan_info_t);
    scan_info = sal_alloc(alloc_size, "sram mem scan info");
    SHR_NULL_CHECK(scan_info, SHR_E_MEMORY);

    sram_scan_info[unit] = scan_info;

    sal_memset(scan_info, 0, alloc_size);

    scan_info->sram_scan_thread_id = SAL_THREAD_ERROR;
    scan_info->sram_scan_stopped = TRUE;
    scan_info->begin_sid = 0;

    scan_info->sram_scan_interval = sal_sem_create("sram scan timer", SAL_SEM_BINARY, 0);
    if (scan_info->sram_scan_interval == NULL) {
        bcmptm_ser_sram_scan_info_deinit(unit);
        SHR_NULL_CHECK(NULL, SHR_E_MEMORY);
    }
    scan_info->sram_scan_stop_notify = sal_sem_create("sram scan stop notify", SAL_SEM_BINARY, 0);
    if (scan_info->sram_scan_stop_notify == NULL) {
        bcmptm_ser_sram_scan_info_deinit(unit);
        SHR_NULL_CHECK(NULL, SHR_E_MEMORY);
    }
    /* read value of ISS_BANK_CONFIGr, in order to get actual size of L2Xm/L3_ENTRY etc. */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, iss_bank_config_info_get_fn)(unit, &iss_bank_config_info, &reg_num);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, iss_bank_config_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to init sram scan information!\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_sram_scan, used to scan sram memory tables by S/W
 */
static int
bcmptm_ser_sram_scan(int unit, bcmdrd_sid_t sid, int tbl_inst, int begin_index,
                     int index_num, uint64_t buf_paddr, bool is_port_reg)
{
    int rv = SHR_E_NONE, retry_time = 0;
    bcmdrd_pt_ser_info_t info;

retry:
    if (is_port_reg) {
        rv = bcmptm_ser_batch_sbusdma_read(unit, sid, -1,
                                           &begin_index, &index_num,
                                           NULL, NULL, 1,
                                           buf_paddr, 1);
    } else {
        rv = bcmptm_ser_batch_sbusdma_read(unit, sid, tbl_inst, NULL, NULL,
                                           &begin_index, &index_num, 1,
                                           buf_paddr, 0);
    }
    if (SHR_SUCCESS(rv)) {
        return rv;
    } else if (retry_time < 10) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SRAM SER error is triggered.\n")));
        info.resp = BCMDRD_SER_RESP_NONE;
        rv = bcmdrd_pt_ser_info_get(unit, sid, &info);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        if (info.resp == BCMDRD_SER_RESP_NONE) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Recovery type of [%s] is NONE, so disable S/W scan for this PT.\n"),
                     bcmdrd_pt_sid_to_name(unit, sid)));
            /* Disable S/W memory scan */
            BCMPTM_SER_IMM_CONTROL_PT_SW_SCAN_SET(unit, sid, 0);
            return rv;
        }
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
                              "Fail to read SRAM[%s] again and again. ERR NO: %d\n"),
                   bcmdrd_pt_sid_to_name(unit, sid), rv));
        rv = SHR_E_NONE;
    }
    return rv;
}

/******************************************************************************
 * bcmptm_ser_sram_is_eligible, used to check whether one PT can be scanned or not.
 */
static int
bcmptm_ser_sram_is_eligible(int unit, bcmdrd_sid_t sid)
{
    bool skip = 0;

    if (bcmdrd_pt_is_valid(unit, sid) == FALSE) {
        return FALSE;
    }
    if (BCMPTM_SER_CONTROL_PT_ECC_PARITY_CHECK(unit, sid) == 0 ||
        BCMPTM_SER_CONTROL_PT_SW_SCAN(unit, sid) == 0) {
        return FALSE;
    }
    /* TCAM view and AGGR view is not scanned, but DATA_ONLY view is scanned. */
    if ((bcmdrd_pt_attr_is_cam(unit, sid) == TRUE) ||
        (bcmdrd_pt_attr_is_fifo(unit, sid) == TRUE)) {
        return FALSE;
    }
    skip = 0;
    (void)BCMPTM_SER_DATA_DRIVER_CB(unit, sram_scan_mem_skip_fn)(unit, sid, &skip);
    if (skip) {
        BCMPTM_SER_IMM_CONTROL_PT_SW_SCAN_SET(unit, sid, 0);
        return FALSE;
    }
    return TRUE;
}

/******************************************************************************
 * bcmptm_ser_sram_scan_thread, used to scan sram memory tables by S/W
 */
static void
bcmptm_ser_sram_scan_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    int rv = SHR_E_NONE;
    uint32_t chunk_byte_size = 0, chunk_size_new = 0;
    int chunk_size_old = 0;
    uint64_t buf_paddr = 0;
    int inst = 0, inst_num = 0, is_port_reg = FALSE;
    int begin_index = 0, max_index = 0, min_index = 0, index_num = 0;
    int index_num_not_scanned = 0, interval = 0;
    bool sram_scan_stopped = FALSE;
    bcmptm_ser_sram_scan_info_t *scan_info_ptr = sram_scan_info[unit];
    int min_port = 0, max_port = 0;
    size_t sid_count, sid = scan_info_ptr->begin_sid;

    SHR_FUNC_ENTER(unit);

    sram_scan_stopped = scan_info_ptr->sram_scan_stopped;
    if (sram_scan_stopped) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    index_num_not_scanned = BCMPTM_SER_CONTROL_SRAM_ENTRIES_READ_PER_INTERVAL(unit);

    chunk_size_old = BCMPTM_SER_CONTROL_SRAM_SCAN_CHUNK_SIZE(unit);
    chunk_byte_size = chunk_size_old * sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE;
    /* allocate dma read buffer */
    bcmptm_ser_sram_scan_buff_adjust(unit, chunk_byte_size);
    buf_paddr = scan_info_ptr->sram_scan_buf_paddr;
    if (buf_paddr == 0) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_count);
    SHR_IF_ERR_EXIT(rv);

    for (; ; sid++) {
        /* new round */
        if (sid == sid_count) {
            sid = 0;
        }
        rv = bcmptm_ser_sram_is_eligible(unit, sid);
        if (rv == FALSE) {
            continue;
        }
        is_port_reg = PT_IS_PORT_OR_SW_PORT_REG(unit, sid);

        inst_num = 0;
        rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &inst_num);
        SHR_IF_ERR_EXIT(rv);

        if (is_port_reg) {
            min_port = 0;
            begin_index = min_port;
            /* instance number */
            max_port = inst_num - 1;
            inst_num = 1;
        } else {
            min_index = bcmdrd_pt_index_min(unit, sid);
            rv = bcmptm_ser_sram_index_valid(unit, sid, min_index, &max_index);
            if (rv == FALSE) {
                continue;
            }
            begin_index = min_index;
        }
        inst = 0;
        while (inst < inst_num) {
            if (is_port_reg) {
                index_num = ((begin_index + chunk_size_old) < max_port) ?
                    chunk_size_old : (max_port - begin_index + 1);
            } else {
                index_num = ((begin_index + chunk_size_old) < max_index) ?
                    chunk_size_old : (max_index - begin_index + 1);
            }
            if (index_num <= 0) {
                inst++;
                begin_index = (is_port_reg) ? min_port: min_index;
                continue;
            }

            rv = bcmptm_ser_sram_scan(unit, sid, inst, begin_index,
                                      index_num, buf_paddr, is_port_reg);
            SHR_IF_ERR_EXIT(rv);
            begin_index += index_num;

            sram_scan_stopped = scan_info_ptr->sram_scan_stopped;
            if (sram_scan_stopped) {
                rv = SHR_E_NONE;
                SHR_RETURN_VAL_EXIT(rv);
            }
            index_num_not_scanned -= index_num;
            if (index_num_not_scanned <= 0) {
                /* interval should be more than 1000000, refer to definition of LT SER_CONTROLt */
                interval = BCMPTM_SER_CONTROL_SRAM_SCAN_INTERVAL(unit);
                /* Fall asleep */
                (void)sal_sem_take(scan_info_ptr->sram_scan_interval, interval);
                /* wake up */
                /* update latest data from LT SER_CONTROLt */
                chunk_size_new = BCMPTM_SER_CONTROL_SRAM_SCAN_CHUNK_SIZE(unit);
                if (chunk_size_new != (uint32_t)chunk_size_old) {
                    chunk_byte_size = chunk_size_new * sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE;
                    /* adjust size of dma read buffer */
                    bcmptm_ser_sram_scan_buff_adjust(unit, chunk_byte_size);
                    buf_paddr = scan_info_ptr->sram_scan_buf_paddr;
                    /* Fail to re-allocate DMA read buffer */
                    if (buf_paddr == 0) {
                        rv = SHR_E_MEMORY;
                        SHR_RETURN_VAL_EXIT(rv);
                    }
                    chunk_size_old = chunk_size_new;
                }
                index_num_not_scanned = BCMPTM_SER_CONTROL_SRAM_ENTRIES_READ_PER_INTERVAL(unit);
                /* Maybe need to exit */
                sram_scan_stopped = scan_info_ptr->sram_scan_stopped;
                if (sram_scan_stopped) {
                    rv = SHR_E_NONE;
                    SHR_RETURN_VAL_EXIT(rv);
                }
            }
        }
    }
exit:
    if (0 == SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " There is failure in SRAM memory scan thread!\n")));
    }
    /* need to free sram_scan_info[unit]->sram_scan_buf */
    bcmptm_ser_sram_scan_buff_adjust(unit, 0);
    scan_info_ptr->sram_scan_thread_id = SAL_THREAD_ERROR;
    scan_info_ptr->sram_scan_stopped = TRUE;
    /* Keep the SID  */
    scan_info_ptr->begin_sid = sid;
    (void)sal_sem_give(scan_info_ptr->sram_scan_stop_notify);
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "SRAM memory scan thread exited!\n")));
    sal_thread_exit(0);
}

/******************************************************************************
 * Public Functions
 */
/******************************************************************************
* bcmptm_ser_sram_scan_start: start sram scan thread. If user wants to stop this thread,
* user can configure SER_CONTROLr to do that.
 */
int
bcmptm_ser_sram_scan_start(int unit, bcmdrd_sid_t beginning_sid)
{
    sal_thread_t thread_id = SAL_THREAD_ERROR;
    uint32_t sram_scan_enable = BCMPTM_SER_CONTROL_SRAM_SCAN(unit);
    bcmptm_ser_sram_scan_info_t *scan_info = NULL;
    int rv = SHR_E_NONE;

    if (sram_scan_enable == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " SRAM_SCANf of SER_CONTROLt is disable,"
                             " so SRAM scan thread is not started!\n")));
        return SHR_E_NONE;
    }
    rv = bcmptm_ser_sram_scan_info_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    scan_info = sram_scan_info[unit];
    if (scan_info->sram_scan_stopped == FALSE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " SRAM scan thread have been started or is starting!\n")));
        return SHR_E_NONE;
    } else if (scan_info->sram_scan_thread_id != SAL_THREAD_ERROR) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " SRAM scan thread is exiting!\n")));
        return SHR_E_NONE;
    }
    scan_info->sram_scan_stopped = FALSE;
    if (beginning_sid != 0) {
        scan_info->begin_sid = beginning_sid;
    }
    thread_id = sal_thread_create("SER SRAM thread",
                                  SAL_THREAD_STKSZ,
                                  50,
                                  bcmptm_ser_sram_scan_thread,
                                  INT_TO_PTR(unit));
    if (thread_id == SAL_THREAD_ERROR) {
        scan_info->sram_scan_stopped = TRUE;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to create SRAM scan thread!\n")));
        return SHR_E_MEMORY;
    } else {
        scan_info->sram_scan_thread_id = thread_id;
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " SRAM scan thread is started!\n")));
    }

    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_sram_scan_stop: stop sram scan thread.
 */
int
bcmptm_ser_sram_scan_stop(int unit, int free_resource)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];

    if (scan_info == NULL || scan_info->sram_scan_stopped == TRUE) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " SRAM scan thread have been stopped or is exiting!\n")));
        return SHR_E_NONE;
    }
    scan_info->sram_scan_stopped = TRUE;
    /* notify SRAM thread to exit */
    (void)sal_sem_give(scan_info->sram_scan_interval);

    do {
        rv = sal_sem_take(scan_info->sram_scan_stop_notify, 1000000);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " SRAM scan thread is exiting!\n")));
        }
    } while (SHR_FAILURE(rv));

    if (free_resource) {
        bcmptm_ser_sram_scan_info_deinit(unit);
    }
    return rv;
}

/******************************************************************************
* bcmptm_ser_sram_scan_resume: wake up sram scan thread by unittest code.
 */
int
bcmptm_ser_sram_scan_resume(int unit)
{
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];

    if (scan_info == NULL || scan_info->sram_scan_stopped == TRUE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " SRAM scan thread have been stopped or is exiting!\n")));
        return SHR_E_FAIL;
    }

    (void)sal_sem_give(scan_info->sram_scan_interval);
    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_serc_table_size_update
 */
int
bcmptm_serc_table_size_update(int unit, const char *resource_grp, uint32_t share_banks)
{
    return BCMPTM_SER_DATA_DRIVER_CB(unit, uft_mem_resize_fn)(unit, resource_grp, (int)share_banks);
}


/******************************************************************************
* bcmptm_ser_sram_index_valid
 */
int
bcmptm_ser_sram_index_valid(int unit, bcmdrd_sid_t sid, int index, int *max_index)
{
    int dedicated_bank = 0, rv = SHR_E_NONE;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, uft_mem_index_check_fn)(unit, sid, (uint32_t)index,
                                        (uint32_t *)max_index, &dedicated_bank);
    if (rv == SHR_E_UNAVAIL) {
        /* not uft memory table */
        *max_index = bcmdrd_pt_index_max(unit, sid);
    } else if (SHR_FAILURE(rv) || (*max_index == 0)) {
        return FALSE;
    }
    if (index <= *max_index) {
        return TRUE;
    }
    return FALSE;
}

