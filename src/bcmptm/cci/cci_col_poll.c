/*! \file cci_col_poll.c
 *
 * Functions for poll based counter collection
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

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_time.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd_cmicd_schan.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <sal/sal_thread.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmptm/bcmptm_internal.h>
#include "cci_col.h"
#include "cci_cache.h"



#define COL_POLL_MSGQ_SIZE           10
#define ADDR_SHIFT                   8

/*!
* \brief Determine if it is time to collect counter block
*/
#define COL_TIME(th, block) \
((((th)->col_time_tick) % (uint64_t)(((th)->config).multiplier[block])) == 0)

typedef enum {
    POL_STATE_STOP = 0,
    POL_STATE_RUN
} cci_pol_state_t;


/*!
* \brief Description of the poll based counter collection structure
*/

typedef struct cci_col_poll_s {
    /*! Context of parent */
    const bcmptm_cci_col_t *parent;

    /*! Number of threads */
    int nthreads;

    /*! Context of thread instance */
    cci_handle_t *hthreads;
} cci_col_poll_t;

/*!
* \brief Counter block meta data
*/

typedef struct col_ctr_block_s {
    size_t wsize;
    uint32_t min_mapid;
    uint32_t max_mapid;
    uint32_t min_offset;
    uint32_t max_offset;
    uint32_t *vaddr;
    uint64_t paddr;
    uint32_t index_min;
    uint32_t index_max;
    int tbl_inst;
    int port;
    int tbl_inst_max;
    int addr_shift;
    uint32_t pt_entry_wsize;
    struct col_ctr_block_s *next;
} col_ctr_block_t;

/*!
* \brief List of counter DMA work list
*/
typedef struct col_ctr_work_s {
    /*! DMA work  for Port Counter Collection */
    bcmbd_sbusdma_work_t sbusdma_work;

    /* map id in counter cache */
    uint32_t mapid;

    /* Soft port */
    int port;

    /* tbl instance */
    int tbl_inst;

    /* Entry Size */
    uint32_t pt_entry_wsize;

    /*! Virtual address of collection buffer */
    uint32_t *vaddr;

   /*! Next work Node */
    struct col_ctr_work_s *next;
}col_ctr_work_t;

/*!
* \brief List of counter collection entities of one type
*/
typedef struct col_ctr_list_s {
    /*! Symbol ID */
    bcmdrd_sid_t sid;
    /*! Number of counter collection blocks */

    int tbl_inst;
    /*! Head of collection meta data list */

    col_ctr_block_t *ctr_head;

    /*! Counter DMA work list */
    col_ctr_work_t *dma_work;

    /*! Next symbol Node */
    struct col_ctr_list_s *next;
} col_ctr_list_t;

/*!
* \brief Description of the poll based counter thread context.
*/

typedef struct col_thread_handle_s {
    /*! logical device id  */
    int unit;

    /*! polling context */
    cci_col_poll_t *col_pol;

    /*! handle to CCI cache */
    cci_handle_t hcache;

    /*! thread id */
    sal_thread_t id;

    /*! thread state  */
    volatile cci_pol_state_t state;

    /*! msgq_hdl handle of message queue */
    sal_msgq_t msgq_hdl;

    /*! wait to handshaking */
    sal_sem_t wait_sem;

    /*! Collection tick */
    uint64_t          col_time_tick;

    /*! number of port counter blocks */
    int num_port_reg_blk;

    /*! Port reg counter blocks */
    col_ctr_block_t port_reg_blk[CCI_CTR_TYPE_NUM];

    /*! DMA work  for Port Counter Collection */
    bcmbd_sbusdma_work_t *port_reg_work[CCI_MAX_PPORT_COUNT];

    /*! max port tbl instances */
    int port_reg_inst_max;

    /*! virtual address of collection buffer */
    uint32_t *vaddr_buf;

    /*! phyiscal address of collection buffer */
    uint64_t paddr_buf;

    /*! size of collection buffer */
    size_t wsize_buf;

    /*! collection configuration */
    cci_config_t config;

    /*!Counter  collection meta data list */
    col_ctr_list_t *ctr_mem_col_list[CCI_CTR_TYPE_NUM];
}  col_thread_handle_t;


/*!
 * copy data between message and configuration
 */
static int
col_poll_config_copy(int unit,
                    cci_config_t *config,
                    col_thread_msg_t *msg,
                    bool to_cfg)
{

    switch (msg->cmd) {
    case MSG_CMD_POL_CONFIG_COL_EN:
        if(to_cfg) {
            config->col_enable = msg->data[0];
        } else {
            msg->data[0] = config->col_enable;
        }
    break;
    case MSG_CMD_POL_CONFIG_INTERVAL:
        if(to_cfg) {
            config->interval = msg->data[0];
        } else {
            msg->data[0] = config->interval;
        }
    break;
    case MSG_CMD_POL_CONFIG_PORT:
    {
        uint32_t i, count, mask;

        count = sizeof(config->pbmp.w)/sizeof(uint32_t);
        /* Copy LSW and then MSW */
        if(to_cfg) {
           mask = ~0x0;
           for (i = 0; i < count/2; i++) {
               config->pbmp.w[2 * i] = msg->data[i] & mask;
               config->pbmp.w[2 * i + 1] =  msg->data[i] >> 32;
           }
        } else{

           for (i = 0; i < count/2; i++) {
               msg->data[i] = config->pbmp.w[2 * i];
               msg->data[i] |= (uint64_t)config->pbmp.w[2 * i + 1] << 32;
           }
        }
    }
    break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_PORT:
        if(to_cfg) {
            config->multiplier[CCI_CTR_TYPE_PORT] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_PORT];
        }
    break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE:
        if(to_cfg) {
            config->multiplier[CCI_CTR_TYPE_EPIPE] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_EPIPE];
        }
    break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE:
        if(to_cfg) {
            config->multiplier[CCI_CTR_TYPE_IPIPE] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_IPIPE];
        }
    break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_TM:
        if(to_cfg) {
            config->multiplier[CCI_CTR_TYPE_TM] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_TM];
        }
    break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT:
        if(to_cfg) {
            config->multiplier[CCI_CTR_TYPE_EVICT] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_EVICT];
        }
    break;
    default:
    break;
    }

    return SHR_E_NONE;

}

/*!
 * Send collection configuration change message
 */
int
bcmptm_cci_col_poll_config(int unit,
                       cci_handle_t handle,
                       cci_pol_cmd_t cmd,
                       cci_config_t *config)
{
    cci_col_poll_t *pol = (cci_col_poll_t *)handle;
    col_thread_handle_t *th;
    col_thread_msg_t msg;

    SHR_FUNC_ENTER(unit);

    if (pol->hthreads) {

        th = (col_thread_handle_t *)pol->hthreads[0];
        SHR_NULL_CHECK(th, SHR_E_PARAM);

        sal_memset(&msg, 0, sizeof(msg));
        msg.cmd = cmd;

        SHR_IF_ERR_EXIT(
            col_poll_config_copy(unit, config, &msg, false));
        if (sal_msgq_post(th->msgq_hdl, (void *)&msg, SAL_MSGQ_NORMAL_PRIORITY,
             MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            SHR_RETURN_VAL_EXIT(SHR_E_TIMEOUT);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Configure SCHAN PIO opcode
 */
static uint32_t
schan_opcode_set(int unit, int op_code, int dst_blk, int acc_type,
                       int data_len, int dma, uint32_t ignore_mask)
{
    uint32_t oc = 0;

    SCMH_OPCODE_SET(oc, op_code);
    SCMH_DSTBLK_SET(oc, dst_blk);
    SCMH_ACCTYPE_SET(oc, acc_type);
    SCMH_DATALEN_SET(oc, data_len);
    SCMH_DMA_SET(oc, dma);
    SCMH_BANK_SET(oc, ignore_mask);

    return oc;
}

/*!
 * Initialize the DMA work items
 */
static int
col_poll_port_reg_dma_work_init(int unit,
                                cci_handle_t handle,
                                int tbl_inst,
                                int *num_items)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    int i;
    int items = 0;
    uint32_t acctype;
    int blknum;
    int lane;
    int rv;
    bcmdrd_sid_t sid;
    bcmdrd_sym_info_t sinfo;
    const bcmdrd_chip_info_t *cinfo;
    col_ctr_block_t *port_blk = NULL;
    bcmbd_sbusdma_data_t *data = NULL;
    bcmdrd_pblk_t pblk;
    uint64_t buff_offset = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    SHR_NULL_CHECK(cinfo, SHR_E_FAIL);

    for (i = 0; i < th->num_port_reg_blk; i++) {
        port_blk = &th->port_reg_blk[i];
        if (tbl_inst <= port_blk->tbl_inst_max) {
            /* Get information about first SID of counter block */
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_mapid_to_sid(unit, th->hcache,
                                              port_blk->min_mapid, &sid));
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_info_get(unit, sid, &sinfo));
            acctype = BCMBD_CMICD_BLKACC2ACCTYPE(sinfo.blktypes);
            sal_memset(&pblk, 0, sizeof(pblk));
            rv = bcmdrd_pt_port_block_lane_get(unit, sid, tbl_inst,
                                               &blknum, &lane);
            if (SHR_FAILURE(rv)) {
                /* Skip invalid ports */
                continue;
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"SID = %s, tbl_inst=%d, blknum=%d lane =%d\n"),
                 bcmdrd_pt_sid_to_name(unit, sid), tbl_inst, blknum, lane));
            /* Populate the work data */
            data = th->port_reg_work[tbl_inst]->data;
            data[items].op_code = schan_opcode_set(unit, READ_REGISTER_CMD_MSG,
                                       blknum, acctype, 0, 0, 0);
            data[items].buf_paddr = th->paddr_buf + buff_offset;
            data[items].start_addr =
                bcmbd_block_port_addr(unit, blknum, lane, sinfo.offset, 0);
            data[items].data_width = port_blk->pt_entry_wsize;
            data[items].addr_gap = ADDR_SHIFT;
            data[items].op_count =
                ((port_blk->max_offset - port_blk->min_offset) >> ADDR_SHIFT) + 1;
            items++;
        }
        /* Update the offset for port block */
        buff_offset += port_blk->wsize * sizeof(uint32_t);
    }
    *num_items = items;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the Port Reg DMA
 */
static int
col_poll_port_reg_dma_init(int unit,
                           cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    int i;
    int items = 0;
    int  tbl_inst;
    col_ctr_block_t *port_blk = NULL;
    bcmbd_sbusdma_data_t *data = NULL;
    bcmbd_sbusdma_work_t *work = NULL;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_FAIL;
    }

    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(th->config.pbmp, tbl_inst) {
        if (tbl_inst >= th->port_reg_inst_max) {
            continue;
        }
        items = 0;
        /* Find the number work items needed for this port */
        for(i = 0; i < th->num_port_reg_blk; i++) {
            port_blk = &th->port_reg_blk[i];
            if (tbl_inst  <= port_blk->tbl_inst_max) {
                items++;
            }
        }
        /* Check applicable items */
        if (items <= 0) {
            continue;
        }
        /* Allocate the memory for work items */
        SHR_ALLOC(work,
                  sizeof(bcmbd_sbusdma_work_t),
                  "DMA WORK");
        SHR_NULL_CHECK(work, SHR_E_MEMORY);
        SHR_ALLOC(data,
                  sizeof(bcmbd_sbusdma_data_t) * items,
                           "WORK ITEM");
        SHR_NULL_CHECK(data, SHR_E_MEMORY);
        sal_memset(data, 0,
                   (sizeof(bcmbd_sbusdma_data_t) * items));
        sal_memset(work, 0, sizeof(bcmbd_sbusdma_work_t));
        /* Populate work meta data */
        work->data = data;
        work->flags = SBUSDMA_WF_READ_CMD;
        th->port_reg_work[tbl_inst] = work;

        /* Populate the work item data */
        SHR_IF_ERR_EXIT(
            col_poll_port_reg_dma_work_init(unit, handle, tbl_inst, &items));
        work->items = items;
        if (work->items > 0) {
            /* Create Batch work */
            SHR_IF_ERR_EXIT(
                bcmbd_sbusdma_batch_work_create(unit, work));
            LOG_VERBOSE(
                BSL_LOG_MODULE,
                (BSL_META_U(unit,"DMA work [%p] for tbl_inst=%d items=%d\n"),
                 (void *)work, tbl_inst, work->items));
        }
        work = NULL;
        data = NULL;
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* Clean up the failed work meta data */
        th->port_reg_work[tbl_inst] = NULL;
        if (data) {
            SHR_FREE(data);
        }
        if (work) {
            SHR_FREE(work);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the DMA work items
 */
static int
col_poll_port_mem_dma_work_init(int unit,
                                cci_handle_t handle,
                                col_ctr_work_t *ctr_work)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    uint32_t acctype;
    int blknum;
    int lane;
    int rv;
    bcmdrd_sid_t sid;
    bcmdrd_sym_info_t sinfo;
    const bcmdrd_chip_info_t *cinfo;
    bcmbd_sbusdma_data_t *data = NULL;
    bcmdrd_pblk_t pblk;
    int index_min, index_max;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_work, SHR_E_PARAM);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    SHR_NULL_CHECK(cinfo, SHR_E_FAIL);
    data = ctr_work->sbusdma_work.data;

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_mapid_to_sid(unit, th->hcache,
                                      ctr_work->mapid, &sid));

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_info_get(unit, sid, &sinfo));

    acctype = BCMBD_CMICD_BLKACC2ACCTYPE(sinfo.blktypes);
    sal_memset(&pblk, 0, sizeof(pblk));

    SHR_IF_ERR_EXIT(
        bcmptm_pt_cci_frmt_index_from_port(unit, sid, ctr_work->port,
                                           &index_min, &index_max));

    rv = bcmdrd_pt_port_block_lane_get(unit, sid, ctr_work->port,
                                               &blknum, &lane);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,"ERROR: SID=%s, port=%d"),
             bcmdrd_pt_sid_to_name(unit, sid), ctr_work->port));
        /* Skip unsupported port */
        SHR_RETURN_VAL_EXIT(SHR_E_PORT);
    }
    /* Populate the work data */
    data->op_code = schan_opcode_set(unit, READ_MEMORY_CMD_MSG,
                                     blknum, acctype,
                                     sinfo.entry_wsize * sizeof(uint32_t),
                                     0, 0);
    data->buf_paddr = th->paddr_buf;
    data->op_count = index_max - index_min + 1;
    data->addr_gap = 0;
    data->start_addr =
        bcmbd_block_port_addr(unit, blknum, -1, sinfo.offset,
                              lane * data->op_count);
    data->data_width = sinfo.entry_wsize;


exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the Port mem DMA
 */
static int
col_poll_port_mem_dma_init(int unit,
                           cci_handle_t handle)
{
    col_thread_handle_t *th;
    int port;
    col_ctr_list_t *port_mem;
    col_ctr_work_t *ctr_work = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    th = (col_thread_handle_t *)handle;
    port_mem = th->ctr_mem_col_list[CCI_CTR_TYPE_PORT];
    while (port_mem) {
        /* Iterate over all enabled ports */
        BCMDRD_PBMP_ITER(th->config.pbmp, port) {
            bcmbd_sbusdma_work_t *sbusdma_work = NULL;
            SHR_ALLOC(ctr_work, sizeof(col_ctr_work_t), "PtmCciDmaMemWork");
            SHR_NULL_CHECK(ctr_work, SHR_E_MEMORY);
            sal_memset(ctr_work, 0, sizeof(col_ctr_work_t));
            ctr_work->port = port;
            ctr_work->mapid = port_mem->ctr_head->min_mapid;
            ctr_work->pt_entry_wsize = port_mem->ctr_head->pt_entry_wsize;
            ctr_work->tbl_inst =
                bcmdrd_pt_tbl_inst_from_port(unit, port_mem->sid, port);
            sbusdma_work = &ctr_work->sbusdma_work;
            sal_memset(sbusdma_work, 0, sizeof(bcmbd_sbusdma_work_t));
            SHR_ALLOC(sbusdma_work->data,
                  sizeof(bcmbd_sbusdma_data_t), "PtmCciDmaMemData");
            SHR_NULL_CHECK(sbusdma_work->data, SHR_E_MEMORY);
            sal_memset(sbusdma_work->data, 0, sizeof(bcmbd_sbusdma_data_t));
            sbusdma_work->flags = SBUSDMA_WF_READ_CMD;
            /* Populate the work item data */
            sbusdma_work->items = 0;
            rv =col_poll_port_mem_dma_work_init(unit, handle, ctr_work);
            if (SHR_SUCCESS(rv)) {
                /* Create Batch work */
                sbusdma_work->items = 1;
                rv = bcmbd_sbusdma_batch_work_create(unit, sbusdma_work);
                if (SHR_FAILURE(rv)) {
                    sbusdma_work->items = 0;
                    LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,"DMA work Error [%p] for port=%d\n"),
                       (void *)sbusdma_work, port));
                }
            }
            ctr_work->next = port_mem->dma_work;
            port_mem->dma_work = ctr_work;
            ctr_work = NULL;
        }
        port_mem = port_mem->next;
    }
exit:
    if (SHR_FUNC_ERR()) {
        /* Free the counter work if not saved in the list */
        if (ctr_work) {
            SHR_FREE(ctr_work);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the Port Mem DMA work
 */
static int
col_poll_port_mem_dma_cleanup(int unit,
                              cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    col_ctr_list_t *port_mem;
    bcmbd_sbusdma_work_t *sbusdma_work;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    th = (col_thread_handle_t *)handle;
    port_mem = th->ctr_mem_col_list[CCI_CTR_TYPE_PORT];

    while (port_mem) {
        col_ctr_work_t *ctr_work = port_mem->dma_work;
        while (ctr_work) {
            col_ctr_work_t *work;
            sbusdma_work = &ctr_work->sbusdma_work;
            if (sbusdma_work->items > 0) {
                bcmbd_sbusdma_batch_work_delete(unit, sbusdma_work);
            }
            if (sbusdma_work->data) {
                SHR_FREE(sbusdma_work->data);
            }
            work = ctr_work;
            ctr_work = ctr_work->next;
            SHR_FREE(work);
        }
        port_mem = port_mem->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the Port Reg DMA work
 */
static int
col_poll_port_reg_dma_cleanup(int unit,
                              cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    int  tbl_inst;
    bcmbd_sbusdma_work_t *work;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(th->config.pbmp, tbl_inst) {
        if (th->port_reg_work[tbl_inst] != NULL) {
            work = th->port_reg_work[tbl_inst];
            if (work->items > 0) {
                bcmbd_sbusdma_batch_work_delete(unit, work);
            }
            th->port_reg_work[tbl_inst] = NULL;
            /* Delete the batch work */
            if (work->data) {
                SHR_FREE(work->data);
            }
            SHR_FREE(work);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the port reg counter collection meta data
 */
static int
poll_col_ctr_port_reg_init(int unit,
                           cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    uint32_t  maxid, i, offset;
    bcmptm_cci_cache_ctr_info_t cinfo;
    int addr_shift = ADDR_SHIFT;
    int index = -1;
    col_ctr_block_t *port_blk = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get maximum map ID */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_ctr_map_size(unit, th->hcache, &maxid));

    offset = 0;
    th->wsize_buf = 0;

    /* Divide the port counter registers into groups
        * so that offsets of member SID's are equispaced for same port.
        * This will help to DMA the counter values all registers in group
        * at the same time.
        * Only the ports set active by user will be collected.
        */
    for(i = 0; i < maxid; i++) {
        /* Get Counter Information */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_ctr_info_get(unit, th->hcache, i, &cinfo));
        if ((cinfo.ctrtype == CCI_CTR_TYPE_PORT) && !cinfo.is_mem) {
            if (bcmptm_pt_cci_ctr_is_bypass(unit, cinfo.sid)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,"IP-EP bypass=%s\n"),
                               bcmdrd_pt_sid_to_name(unit, cinfo.sid)));
                continue;
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"Map id=%d offset=0x%x\n"),
                              i,  cinfo.offset));
           if ((cinfo.offset == 0) ||
                (cinfo.offset > offset + (1 << addr_shift))) {
               /* Discontinuity in offset, next chunk */
               index++;
               port_blk = &th->port_reg_blk[index];
               SHR_NULL_CHECK(port_blk, SHR_E_INIT);
               port_blk->min_mapid = i;
               port_blk->min_offset = cinfo.offset;
               port_blk->wsize = 0;
               port_blk->addr_shift = addr_shift;
               port_blk->pt_entry_wsize = cinfo.pt_entry_wsize;
               port_blk->tbl_inst_max = cinfo.tbl_inst;
               if (th->port_reg_inst_max < port_blk->tbl_inst_max) {
                   th->port_reg_inst_max = port_blk->tbl_inst_max;
               }
               LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit,"Min Map id=%d offset=0x%x tbl_inst=%d\n"),
                              i, cinfo.offset, cinfo.tbl_inst));
               port_blk->max_mapid = i;
               port_blk->wsize += cinfo.pt_entry_wsize;
               th->wsize_buf += cinfo.pt_entry_wsize;
               offset = cinfo.offset;
           } else if (cinfo.offset == offset) {
               /* Duplicate entry ignore */
               continue;
           } else {
               SHR_NULL_CHECK(port_blk, SHR_E_INIT);
               port_blk->max_mapid = i;
               port_blk->wsize += cinfo.pt_entry_wsize;
               th->wsize_buf += cinfo.pt_entry_wsize;
               offset = cinfo.offset;
               port_blk->max_offset = offset;
           }
       }
   }
   th->num_port_reg_blk = index + 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the counter memory. collection meta data
 */
static int
poll_col_ctr_mem_init(int unit,
                      int ctr_type,
                      cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    uint32_t  maxid, i, k;
    bcmptm_cci_cache_ctr_info_t cinfo;
    size_t wsize = 0;
    col_ctr_block_t *block = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get maximum map ID */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_ctr_map_size(unit, th->hcache, &maxid));

    /* find the number of Port memory table symbols */
    for(i = 0; i < maxid; i++) {
        col_ctr_list_t *ctr_list = NULL;
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_ctr_info_get(unit, th->hcache, i, &cinfo));
        if ((cinfo.ctrtype == ctr_type) && cinfo.is_mem) {
            /* Include New counter to the collection list */
            LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,"Name = %s\n"),
                    bcmdrd_pt_sid_to_name(unit, cinfo.sid)));
            SHR_ALLOC(ctr_list, sizeof(col_ctr_list_t), "PtmCciListNode");
            SHR_NULL_CHECK(ctr_list, SHR_E_MEMORY);
            sal_memset(ctr_list , 0, sizeof(col_ctr_list_t));
            ctr_list->next = th->ctr_mem_col_list[ctr_type];
            th->ctr_mem_col_list[ctr_type] = ctr_list;
            ctr_list->tbl_inst = cinfo.tbl_inst;
            ctr_list->sid = cinfo.sid;
            ctr_list->ctr_head = NULL;
            /* Prepare collection meta data */
            for (k = cinfo.index_min; k <= cinfo.index_max; k++) {
                if (bcmptm_pt_cci_index_valid(unit, cinfo.sid, 1, k)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,"Valid: Name = %s index =%d\n"),
                            bcmdrd_pt_sid_to_name(unit, cinfo.sid), k));
                    if (!block) {
                        SHR_ALLOC(block, sizeof(col_ctr_block_t), "PtmCciCtrBlock");
                        SHR_NULL_CHECK(block, SHR_E_MEMORY);
                        sal_memset(block, 0, sizeof(col_ctr_block_t));
                        block->next = ctr_list->ctr_head;
                        ctr_list->ctr_head = block;
                        /* Initialize collection meta data */
                        block->min_mapid = i;
                        block->max_mapid = i;
                        block->min_offset = cinfo.offset;
                        block->max_offset = cinfo.offset;
                        block->addr_shift = 0;
                        block->pt_entry_wsize = cinfo.pt_entry_wsize;
                        block->tbl_inst_max = cinfo.tbl_inst;
                        block->index_min = k;
                        block->index_max = k;
                        block->next = NULL;
                    } else {
                        block->index_max = k;
                    }
                } else {
                    /* Discontinuity, Table has holes, create new block*/
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Invalid: Name = %s, index =%d\n"),
                            bcmdrd_pt_sid_to_name(unit, cinfo.sid), k));
                    if (block) {
                        block->wsize = cinfo.pt_entry_wsize *
                                (block->index_max - block->index_min + 1);
                        block = NULL;
                    }
                }
            }
            /* store the size of the table instance */
            if (block) {
                block->wsize = cinfo.pt_entry_wsize * \
                           (block->index_max - block->index_min + 1);
                if ((block->wsize * block->pt_entry_wsize) > wsize) {
                    wsize = block->wsize * block->pt_entry_wsize;
                }
                block = NULL;
            }
        }
    }

    if (wsize > th->wsize_buf) {
        th->wsize_buf = wsize;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup mem counter collection meta data
 */
static int
poll_col_ctr_mem_cleanup(int unit,
                           int ctr_type,
                           cci_handle_t handle)
{
    col_thread_handle_t *th = NULL;
    col_ctr_list_t *ctr_list = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    th = (col_thread_handle_t *)handle;
    ctr_list = th->ctr_mem_col_list[ctr_type];

    while (ctr_list) {
        /* Free collection meta data */
        col_ctr_block_t *ctr_head = ctr_list->ctr_head;
        while (ctr_head) {
            ctr_list->ctr_head = ctr_head->next;
            SHR_FREE(ctr_head);
            ctr_head = ctr_list->ctr_head;
        }
        ctr_list->tbl_inst = 0;
        th->ctr_mem_col_list[ctr_type] = ctr_list->next;
        SHR_FREE(ctr_list);
        ctr_list = th->ctr_mem_col_list[ctr_type];
    }
    th->ctr_mem_col_list[ctr_type] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the port mem counter collection meta data
 */
static int
col_poll_port_mem_cleanup(int unit,
                           cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        poll_col_ctr_mem_cleanup(unit, CCI_CTR_TYPE_PORT, handle));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * Initialize the Port memory counter memory collection meta data
 */
static int
poll_col_ctr_port_mem_init(int unit,
                           cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        poll_col_ctr_mem_init(unit, CCI_CTR_TYPE_PORT, handle));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the port reg counter collection meta data
 */
static int
poll_col_ctr_port_init(int unit,
                       cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    size_t wsize = 0;
    col_ctr_block_t *port_blk = NULL;
    cci_handle_t cci;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    /* Get handle of CCI Cache */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_handle_get(unit, th->col_pol->parent, &cci));

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_handle_get(unit, cci, &th->hcache));

    SHR_IF_ERR_EXIT(
        poll_col_ctr_port_reg_init(unit, handle));

    SHR_IF_ERR_EXIT(
        poll_col_ctr_port_mem_init(unit, handle));

    /* Find the max buffer size needed */
    for (i = 0; i < th->num_port_reg_blk; i++) {
        port_blk = &th->port_reg_blk[i];
        wsize += port_blk->wsize;
    }

    if (wsize > th->wsize_buf) {
        th->wsize_buf = wsize;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Cleanup the port collection meta data
 */
static void
poll_col_ctr_port_cleanup(int unit,
                          cci_handle_t handle)
{
    col_poll_port_mem_cleanup(unit, handle);
}

/*
 * Collect the block of contiguous port counters
.*  for specified port Reg using non DMA
 */
static int
col_poll_ctr_port_reg_block_nodma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    col_ctr_block_t *port_blk;
    bcmptm_cci_ctr_info_t info;
    bcmbd_pt_dyn_info_t dyn;
    uint32_t i;
    bcmptm_cci_cache_ctr_info_t cinfo;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    port_blk = (col_ctr_block_t *)param;

    for (i = port_blk->min_mapid; i <= port_blk->max_mapid; i++) {
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_ctr_info_get(unit, handle, i, &cinfo));
        /* Check if counter type is Port and register */
        if ((cinfo.ctrtype != CCI_CTR_TYPE_PORT) || cinfo.is_mem) {
            continue;
        }
        dyn.index = 0;
        dyn.tbl_inst = port_blk->tbl_inst;
        /* Check if tbl-inst and index are valid */
        if (!bcmptm_cci_cache_mapid_index_valid(unit, handle, i,
                              dyn.index, dyn.tbl_inst)) {
            continue;
        }

        /* Read Counter value using SCHAN PIO */
        SHR_IF_ERR_EXIT(
            bcmbd_pt_read(unit, cinfo.sid, &dyn, NULL,
                      port_blk->vaddr, port_blk->pt_entry_wsize));
        /* Update the cache with value as input parameter */
        if (update_cache) {
            info.dyn_info = dyn;
            info.map_id = i;
            info.sid = cinfo.sid;
            SHR_IF_ERR_EXIT(
                 bcmptm_cci_cache_update(unit, handle, &info,
                                         port_blk->vaddr,
                                         port_blk->pt_entry_wsize));
        }

   }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Collect the block of contiguous port counters for specified port using SBUS DMA
 */
static int
col_poll_ctr_port_reg_block_dma(int unit,
                                cci_handle_t handle,
                                void *param,
                                bool update_cache)
{
    col_thread_handle_t *th = (col_thread_handle_t *)param;
    bcmptm_cci_cache_ctr_info_t cinfo;
    bcmptm_cci_ctr_info_t info;
    bcmbd_pt_dyn_info_t dyn;
    uint32_t i, offset;
    bcmbd_sbusdma_work_t *work;
    int tbl_inst, j;
    uint64_t buff_offset;
    uint32_t *vaddr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* Execute the DMA work */
    tbl_inst = th->port_reg_blk[0].tbl_inst;
    work = th->port_reg_work[tbl_inst];
    if (work->items == 0) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        bcmbd_sbusdma_work_execute(unit, work));
    /* Update the cache with values collected using DMA */
    if (update_cache) {
        for(j = 0; j < work->items; j++) {
            col_ctr_block_t *port_blk = &th->port_reg_blk[j];
            /* Check if port is valid for counter group */
            if (tbl_inst >= port_blk->tbl_inst_max) {
                continue;
            }
            buff_offset = 0;
            offset = ~0x0;
            /* Update each counter cache */
            for (i = port_blk->min_mapid; i <= port_blk->max_mapid; i++) {
                dyn.index = 0;
                dyn.tbl_inst = tbl_inst;
                info.dyn_info = dyn;
                /* Check if tbl-inst and index are valid */
                if (!bcmptm_cci_cache_mapid_index_valid(unit, handle, i,
                                      dyn.index, dyn.tbl_inst)) {
                    continue;
                }
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_cache_ctr_info_get(unit, handle, i, &cinfo));
                if (cinfo.offset != offset) {
                    info.map_id = i;
                    vaddr = &port_blk->vaddr[buff_offset];
                    SHR_IF_ERR_EXIT(
                         bcmptm_cci_cache_update(unit, handle, &info,
                                                 vaddr,
                                                 port_blk->pt_entry_wsize));
                    buff_offset += port_blk->pt_entry_wsize;
                    offset = cinfo.offset;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect all port reg. counters for all active ports using DMA
 */
static int
col_poll_ctr_port_reg_dma(int unit,
                      cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    int i;
    int tbl_inst;
    uint64_t buff_offset = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    if (th->num_port_reg_blk <= 0) {
        goto exit;
    }
    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(th->config.pbmp, tbl_inst) {
        if (tbl_inst < th->port_reg_inst_max) {
            /* No collection if not mapped to logical port */
            buff_offset = 0;
            if (th->port_reg_work[tbl_inst] != NULL) {
                for(i = 0; i < th->num_port_reg_blk; i++) {
                   col_ctr_block_t *port_blk = &th->port_reg_blk[i];
                   port_blk->vaddr = &th->vaddr_buf[buff_offset];
                   port_blk->tbl_inst = tbl_inst;
                   /* update the offset for port block */
                   buff_offset += port_blk->wsize;
                }
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,"tbl_inst = %d\n"), tbl_inst));
                /* Collect the couter and update the Counter cache */
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_cache_hw_sync(unit, th->hcache,
                                             col_poll_ctr_port_reg_block_dma,
                                             th, TRUE));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the port counter table
.*  for specified port Mem using non DMA
 */
static int
col_poll_ctr_port_mem_block_dma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    bcmptm_cci_ctr_info_t info;
    bcmbd_pt_dyn_info_t dyn;
    col_ctr_work_t *ctr_work;
    bcmbd_sbusdma_work_t *work;
    int i;
    int index_min, index_max;
    bcmdrd_sid_t sid;
    uint32_t *vaddr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    ctr_work = (col_ctr_work_t *)param;
    work = &ctr_work->sbusdma_work;
    SHR_IF_ERR_EXIT(
    bcmptm_cci_cache_mapid_to_sid(unit, handle,
                                      ctr_work->mapid, &sid));

    SHR_IF_ERR_EXIT(
        bcmptm_pt_cci_frmt_index_from_port(unit, sid, ctr_work->port,
                                           &index_min, &index_max));

    if(work->items) {
        SHR_IF_ERR_EXIT(
                bcmbd_sbusdma_work_execute(unit, work));
        /* Update the cache with values collected using DMA */
        if (update_cache) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"SID=%s, port=%d, tbl_inst =%d\n"
                           "index_min=%u, index_max=%u entry_size=%u\n"),
                            bcmdrd_pt_sid_to_name(unit, sid), ctr_work->port,
                            ctr_work->tbl_inst, index_min, index_max,
                            ctr_work->pt_entry_wsize));
            vaddr = ctr_work->vaddr;
            for (i = index_min; i <= index_max; i++) {
                dyn.index = i;
                dyn.tbl_inst = ctr_work->tbl_inst;
                /* Check if tbl-inst and index are valid */
                if (bcmptm_cci_cache_mapid_index_valid(unit, handle,
                                                       ctr_work->mapid,
                                                       dyn.index, dyn.tbl_inst)) {
                    info.dyn_info = dyn;
                    info.map_id = ctr_work->mapid;
                    info.sid = sid;
                    SHR_IF_ERR_EXIT(
                         bcmptm_cci_cache_update(unit, handle, &info,
                                                 vaddr,
                                                 ctr_work->pt_entry_wsize));
                    vaddr += ctr_work->pt_entry_wsize;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect all port mem counters for all active ports using DMA
 */
static int
col_poll_ctr_port_mem_dma(int unit,
                            cci_handle_t handle)
{
    col_thread_handle_t *th;
    col_ctr_list_t *port_mem;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    th = (col_thread_handle_t *)handle;
    port_mem = th->ctr_mem_col_list[CCI_CTR_TYPE_PORT];

    while (port_mem) {
        col_ctr_work_t *ctr_work = port_mem->dma_work;
        /* Iterate over all enabled ports */
        while(ctr_work) {
            ctr_work->vaddr = th->vaddr_buf;
            /* Collect the couter and update the Counter cache */
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_hw_sync(unit, th->hcache,
                                         col_poll_ctr_port_mem_block_dma,
                                         ctr_work, TRUE));
            ctr_work = ctr_work->next;
        }
        port_mem = port_mem->next;
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * Collect all port reg counters for all active ports using SCHAN PIO
 */
static int
col_poll_ctr_port_reg_nodma(int unit,
                            cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    int i;
    int tbl_inst;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    if (th->num_port_reg_blk <= 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(th->config.pbmp, tbl_inst) {
        if (tbl_inst < th->port_reg_inst_max) {
            for(i = 0; i < th->num_port_reg_blk; i++) {
                col_ctr_block_t *port_blk = &th->port_reg_blk[i];
                /* Check if port is valid for counter group */
                if (tbl_inst >= port_blk->tbl_inst_max) {
                    continue;
                }
                port_blk->vaddr = th->vaddr_buf;
                port_blk->tbl_inst = tbl_inst;
                /* Collect the couter and update the Counter cache */
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_cache_hw_sync(unit, th->hcache,
                                             col_poll_ctr_port_reg_block_nodma,
                                             port_blk, TRUE));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the port counter table
.*  for specified port Mem using non DMA
 */
static int
col_poll_ctr_port_mem_block_nodma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    col_ctr_block_t *port_blk = NULL;
    bcmptm_cci_ctr_info_t info;
    bcmbd_pt_dyn_info_t dyn;
    int i;
    int index_min, index_max;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    port_blk = (col_ctr_block_t *)param;

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_mapid_to_sid(unit, handle,
                                      port_blk->min_mapid, &sid));

    SHR_IF_ERR_EXIT(
        bcmptm_pt_cci_frmt_index_from_port(unit, sid, port_blk->port,
                                           &index_min, &index_max));
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"SID=%s, port=%d, tbl_inst =%d\n"
                   "index_min=%u, index_max=%u entry_size=%u\n"),
                    bcmdrd_pt_sid_to_name(unit, sid), port_blk->port,
                    port_blk->tbl_inst, index_min, index_max,
                    port_blk->pt_entry_wsize));

    for (i = index_min; i <= index_max; i++) {
        dyn.index = i;
        dyn.tbl_inst = port_blk->tbl_inst;
        /* Check if tbl-inst and index are valid */
        if (bcmptm_cci_cache_mapid_index_valid(unit, handle,
                                               port_blk->min_mapid,
                                               dyn.index, dyn.tbl_inst)) {
            /* Read Counter value using SCHAN PIO */
            SHR_IF_ERR_EXIT(
                bcmbd_pt_read(unit, sid, &dyn, NULL,
                          port_blk->vaddr, port_blk->pt_entry_wsize));
            /* Update the cache with value as input parameter */
            if (update_cache) {
                info.dyn_info = dyn;
                info.map_id = port_blk->min_mapid;
                info.sid = sid;
                SHR_IF_ERR_EXIT(
                     bcmptm_cci_cache_update(unit, handle, &info,
                                             port_blk->vaddr,
                                             port_blk->pt_entry_wsize));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect all port mem counters for all active ports using SCHAN PIO
 */
static int
col_poll_ctr_port_mem_nodma(int unit,
                            cci_handle_t handle)
{
    col_thread_handle_t *th;
    int port;
    bcmdrd_sid_t sid;
    col_ctr_list_t *port_mem;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    th = (col_thread_handle_t *)handle;

    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(th->config.pbmp, port) {
        port_mem = th->ctr_mem_col_list[CCI_CTR_TYPE_PORT];
        while (port_mem) {
            col_ctr_block_t *port_blk = port_mem->ctr_head;
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_mapid_to_sid(unit, th->hcache,
                                              port_blk->min_mapid, &sid));
            port_blk->vaddr = th->vaddr_buf;
            /* find the table instance from port */
            port_blk->port = port;
            port_blk->tbl_inst =
                bcmdrd_pt_tbl_inst_from_port(unit, sid, port);
            /* Collect the couter and update the Counter cache */
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_hw_sync(unit, th->hcache,
                                         col_poll_ctr_port_mem_block_nodma,
                                         port_blk, TRUE));
            port_mem = port_mem->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the eviction counter block using non DMA
 */
static int
col_poll_ctr_evict_block_nodma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    col_ctr_block_t *block = NULL;
    uint32_t i;
    bcmdrd_sid_t sid;
    bcmptm_cci_ctr_info_t info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    block = (col_ctr_block_t *)param;

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_mapid_to_sid(unit, handle,
                                      block->min_mapid, &sid));

    for (i = block->index_min; i <= block->index_max; i++) {
        bcmbd_pt_dyn_info_t dyn;
        dyn.index = i;
        dyn.tbl_inst = block->tbl_inst;

        /* Read Counter value using SCHAN PIO */
        SHR_IF_ERR_EXIT(
            bcmbd_pt_read(unit, sid, &dyn, NULL,
                      block->vaddr, block->pt_entry_wsize));
        /* Update the cache with value as input parameter */
        if (update_cache) {
            info.dyn_info = dyn;
            info.map_id = block->min_mapid;
            info.sid = sid;
            SHR_IF_ERR_EXIT(
                 bcmptm_cci_cache_update(unit, handle, &info,
                                         block->vaddr,
                                         block->pt_entry_wsize));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * Collect eviction counters using SCHAN PIO
 */
static int
col_poll_ctr_evict_nodma(int unit,
                            cci_handle_t handle)
{
    col_thread_handle_t *th;
    col_ctr_list_t *evict;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    th = (col_thread_handle_t *)handle;;
    evict = th->ctr_mem_col_list[CCI_CTR_TYPE_EVICT];

    while (evict) {
        /* Check if eviction conter is enabled  */
        if (bcmptm_pt_cci_ctr_evict_is_enable(unit, evict->sid)) {
            col_ctr_block_t *ctr_head = evict->ctr_head;
            for (i = 0; i < evict->tbl_inst; i++) {
                while (ctr_head) {
                    /* Collect the couter and update the Counter cache */
                    ctr_head->tbl_inst = i;
                    ctr_head->vaddr = th->vaddr_buf;
                    SHR_IF_ERR_EXIT(
                        bcmptm_cci_cache_hw_sync(unit, th->hcache,
                                                 col_poll_ctr_evict_block_nodma,
                                                 ctr_head, TRUE));
                    ctr_head = ctr_head->next;
                }
            }
        }
        evict = evict->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the counter memory collection meta data
 */
static int
poll_col_ctr_evict_init(int unit,
                           cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        poll_col_ctr_mem_init(unit, CCI_CTR_TYPE_EVICT, handle));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the eviction counter collection meta data
 */
static int
poll_col_ctr_evict_cleanup(int unit,
                           cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        poll_col_ctr_mem_cleanup(unit, CCI_CTR_TYPE_EVICT, handle));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Counter collection dispatch function
 */
static int
col_poll_ctr_dispatch(int unit, cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    /* Collection is not enabled */
    if (!th->config.col_enable) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Port Counters */
    if ((th->config.multiplier[CCI_CTR_TYPE_PORT] != 0) &&
                      COL_TIME(th, CCI_CTR_TYPE_PORT)) {
        if (th->config.dma) {
            SHR_IF_ERR_EXIT(
                col_poll_ctr_port_reg_dma(unit, handle));
            SHR_IF_ERR_EXIT(
                col_poll_ctr_port_mem_dma(unit, handle));
        } else {
            SHR_IF_ERR_EXIT(
                col_poll_ctr_port_reg_nodma(unit, handle));
            SHR_IF_ERR_EXIT(
                col_poll_ctr_port_mem_nodma(unit, handle));
        }
    }

    /* Eviction Counters */
    if ((th->config.multiplier[CCI_CTR_TYPE_EVICT] != 0) &&
                      COL_TIME(th, CCI_CTR_TYPE_EVICT)) {
        SHR_IF_ERR_EXIT(
            col_poll_ctr_evict_nodma(unit, handle));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Counter collection polling cleanup function
 */
static int
poll_col_cleanup(int unit,
                 col_thread_handle_t  *thrd)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(thrd, SHR_E_PARAM);

    if (thrd->config.dma) {
        bcmdrd_hal_dma_free(unit, thrd->wsize_buf * sizeof(uint32_t),
                            thrd->vaddr_buf, thrd->paddr_buf);
        /* Cleanup DMA for Port Reg batch work */
        col_poll_port_reg_dma_cleanup(unit, thrd);
        /* Cleanup DMA for Port mem batch work */
        col_poll_port_mem_dma_cleanup(unit, thrd);
    } else {
        if (thrd->vaddr_buf) {
            SHR_FREE(thrd->vaddr_buf);
        }
    }
    thrd->vaddr_buf = NULL;
    thrd->paddr_buf = 0;

    /* Clean up port counter collection resources */
    poll_col_ctr_port_cleanup(unit, thrd);
    /* Clean up eviction counter collection resources */
    poll_col_ctr_evict_cleanup(unit, thrd);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Counter collection polling init function
 */
static int
poll_col_init(int unit,
              col_thread_handle_t *thrd)
{
    bool psim;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(thrd, SHR_E_PARAM);

    /* Initialize Port collection DMA buffers and meta data */
    SHR_IF_ERR_EXIT(
         poll_col_ctr_port_init(unit, thrd));
    /* Initialize Eviction collection DMA buffers and meta data */
    SHR_IF_ERR_EXIT(
        poll_col_ctr_evict_init(unit, thrd));

   /* Allocate buffer */
    thrd->vaddr_buf = NULL;
    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    if (psim) {
        thrd->config.dma = 0;
    } else {
        thrd->config.dma = 1;
    }

    if (thrd->config.dma) {
       thrd->vaddr_buf =
          bcmdrd_hal_dma_alloc(unit,
                               thrd->wsize_buf * sizeof(uint32_t),
                               "bcmptmCtrDma", &thrd->paddr_buf);

       SHR_NULL_CHECK(thrd->vaddr_buf, SHR_E_MEMORY);
       /* Initalize DMA for batch work */
       SHR_IF_ERR_EXIT(
           col_poll_port_reg_dma_init(unit, thrd));
       SHR_IF_ERR_EXIT(
           col_poll_port_mem_dma_init(unit, thrd));
    } else {
        SHR_ALLOC(thrd->vaddr_buf,
                  thrd->wsize_buf * sizeof(uint32_t),
                  "bcmptmCtrDma");
        SHR_NULL_CHECK(thrd->vaddr_buf, SHR_E_MEMORY);
    }
    sal_memset(thrd->vaddr_buf, 0,
               (thrd->wsize_buf * sizeof(uint32_t)));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Counter collection polling thread function
 */
static void
col_poll_thread_func(cci_handle_t handle)
{
    col_thread_handle_t *th = (col_thread_handle_t *)handle;
    col_thread_msg_t msg;
    bool loop = TRUE;
    int unit = th->unit;

    th->state = POL_STATE_RUN;

    while (loop) {
        /* wait for message and process the collection */
        if (sal_msgq_recv(th->msgq_hdl, &msg, th->config.interval) ==
                          SAL_MSGQ_E_TIMEOUT) {
            th->col_time_tick++;
            /*No message, Do collection */
            col_poll_ctr_dispatch(unit, handle);
        } else {
            switch (msg.cmd) {
            case MSG_CMD_POL_STOP:
                loop = FALSE;
            break;
            case MSG_CMD_FIFODMA_REQ:
            /* FIFO DMA interrupt to collect evicted counters */

            break;
            /* Configuration message processing */
            case MSG_CMD_POL_CONFIG_COL_EN:
            case MSG_CMD_POL_CONFIG_INTERVAL:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_PORT:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_TM:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT:
                col_poll_config_copy(unit, &th->config, &msg, true);
            break;

            case MSG_CMD_POL_CONFIG_PORT:
                /* Cleanup collection resources */
                poll_col_cleanup(unit, handle);
                /* Update configuration */
                col_poll_config_copy(unit, &th->config, &msg, true);
                LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                            "Re-Initialize collection resources\n")));
                /* Initialize collection DMA buffers and meta data */
                if (poll_col_init(unit, handle) != SHR_E_NONE) {
                    LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,
                              "Error in init collection resources\n")));
                    loop = FALSE;
                }
            break;

            default:
                LOG_WARN(BSL_LOG_MODULE,(BSL_META_U(unit,
                                         "Invalid Command = %d\n"),msg.cmd));

            break;
            }
        }
        /* Set up timeout */
    }

    th->state = POL_STATE_STOP;
    sal_sem_give(th->wait_sem);

}

/*
 * Counter collection polling thread init function
 */
static int
col_poll_thread_init(int unit,
                        cci_col_poll_t *col_pol,
                        int prio,
                        cci_handle_t *handle)
{
    col_thread_handle_t    *thrd = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(col_pol, SHR_E_PARAM);

    SHR_ALLOC(thrd, sizeof(col_thread_handle_t), "POLL_COL");
    SHR_NULL_CHECK(thrd, SHR_E_MEMORY);
    sal_memset(thrd, 0, sizeof(col_thread_handle_t));
    *handle = thrd;

    /* Create Message  Queue */
    thrd->msgq_hdl =  sal_msgq_create(sizeof(col_thread_msg_t),
                                      COL_POLL_MSGQ_SIZE, "POLL_QUEUE");
    SHR_NULL_CHECK(thrd->msgq_hdl, SHR_E_MEMORY);

    /* Create Wait Semaphore */
    thrd->wait_sem = sal_sem_create("WAIT SEM", 0, 0);
    SHR_NULL_CHECK(thrd->wait_sem, SHR_E_MEMORY);

    thrd->unit = unit;
    thrd->col_pol = col_pol;
    thrd->state = POL_STATE_STOP;

    /* Configure the thread properties */
    sal_memset(&thrd->config, 0, sizeof(cci_config_t));
    SHR_IF_ERR_EXIT(
        bcmptm_cci_config_get(unit, &thrd->config));

    /* Initialize counter collection resources */
    SHR_IF_ERR_EXIT(
        poll_col_init(unit, thrd));


    /* Create thread */
    thrd->id = sal_thread_create("POLL_THREAD", SAL_THREAD_STKSZ,
                                 prio, col_poll_thread_func, thrd);

    if (thrd->id == SAL_THREAD_ERROR) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* Free the allocated resources */
        if (thrd) {
            poll_col_ctr_port_cleanup(unit, thrd);
            if (thrd->wait_sem) {
                sal_sem_destroy(thrd->wait_sem);
                thrd->wait_sem = NULL;
            }
            if (thrd->msgq_hdl) {
                sal_msgq_destroy(thrd->msgq_hdl);
                thrd->msgq_hdl = NULL;
            }
            SHR_FREE(thrd);
        }
    }
    SHR_FUNC_EXIT();

}

/*
 * Counter collection polling thread cleanup function
 */
static int
col_poll_thread_cleanup(int unit, cci_col_poll_t *col_pol,
                        cci_handle_t handle)
{
    col_thread_handle_t  *thread_handle;
    col_thread_msg_t msg;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    thread_handle = (col_thread_handle_t  *)handle;

    /* signal the thread and wait for it to exit */
    if (thread_handle->msgq_hdl) {
        msg.cmd = MSG_CMD_POL_STOP;
        if (sal_msgq_post(thread_handle->msgq_hdl, &msg, 0,
                          MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            SHR_IF_ERR_EXIT(SHR_E_TIMEOUT);
        }

        /* Wait for thread to be in run state */
        if (sal_sem_take(thread_handle->wait_sem, 2 * CFG_CCI_TIME_OUT) == -1) {
            SHR_IF_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }
    /* Check if still is RUN state */
    if (thread_handle->state == POL_STATE_RUN) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    /* Cleanup counter collection resources */
    SHR_IF_ERR_EXIT(
        poll_col_cleanup(unit, thread_handle));

    /* Destroy wait semaphore */
    if (thread_handle->wait_sem) {
        sal_sem_destroy(thread_handle->wait_sem);
    }

    /* Destroy Message Queue handle */
    if (thread_handle->msgq_hdl) {
        sal_msgq_destroy(thread_handle->msgq_hdl);
    }

    /* clean up the thread context */
    SHR_FREE(handle);
exit:
    SHR_FUNC_EXIT();


}

/*
 * Cleans polling threads and frees the thread instance
 */
static int
cci_col_poll_stop(int unit,
                  cci_col_poll_t *pol)
{
    int i;

    SHR_FUNC_ENTER(unit);
    for (i = 0; i < pol->nthreads; i++) {
        SHR_IF_ERR_EXIT(
            col_poll_thread_cleanup(unit, pol, pol->hthreads[i]));
        pol->nthreads--;
        pol->hthreads[i] = NULL;
    }
    SHR_FREE(pol->hthreads);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize poll based counter collection resources.
 */
int
bcmptm_cci_col_poll_init(int unit,
                         const bcmptm_cci_col_t *con_col,
                         cci_handle_t *handle)
{
    cci_col_poll_t *pol = NULL;
    SHR_FUNC_ENTER(unit);

    /* Allocate the poll base counter collection handle */
    SHR_ALLOC(pol, sizeof(cci_col_poll_t), "CCI Poll");
    SHR_NULL_CHECK(pol, SHR_E_MEMORY);
    sal_memset(pol, 0, sizeof(cci_col_poll_t));
    pol->parent = con_col;
    *handle = pol;

exit:
    SHR_FUNC_EXIT();

}

/*
 * Cleanup poll based counter collection resources.
 */
int
bcmptm_cci_col_poll_cleanup(int unit,
                            cci_handle_t handle)
{
    cci_col_poll_t   *pol = (cci_col_poll_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pol, SHR_E_PARAM);

    SHR_FREE(pol);

exit:
    SHR_FUNC_EXIT();

}

/*
 * Start poll based counter collection.
 */
extern int
bcmptm_cci_col_poll_run(int unit,
                        cci_handle_t handle)
{
    cci_col_poll_t   *pol = (cci_col_poll_t *)handle;
    int th = 1;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pol, SHR_E_PARAM);

    pol->nthreads = 0;

    if (th) {
        SHR_ALLOC(pol->hthreads, sizeof(cci_handle_t)* th,
                                  "POLL THREADS");
        SHR_NULL_CHECK(pol->hthreads, SHR_E_MEMORY);
        sal_memset(pol->hthreads, 0, sizeof(cci_handle_t)*th);
    }

    /* Initialize threads */
    for (i = 0; i < th; i++) {
        SHR_IF_ERR_EXIT(
            col_poll_thread_init(unit, pol, SAL_THREAD_PRIO_DEFAULT,
                                 &pol->hthreads[i]));
        pol->nthreads++;
    }

exit:
    if (SHR_FUNC_ERR()) {
        cci_col_poll_stop(unit, pol);
    }
    SHR_FUNC_EXIT();
}

/*
 * Stop poll based counter collection.
 */
extern int
bcmptm_cci_col_poll_stop(int unit,
                         cci_handle_t handle)
{
    cci_col_poll_t   *pol = (cci_col_poll_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pol, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(
        cci_col_poll_stop(unit, pol));

 exit:
    SHR_FUNC_EXIT();

}

