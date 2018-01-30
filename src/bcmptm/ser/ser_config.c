/*! \file ser_config.c
 *
 * Interface functions for SER
 *
 * This file contains the initialization code of SER.
 * This file also contains SER code for public use
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
#include <shr/shr_debug.h>
#include <bcmdrd_config.h>
#include <shr/shr_error.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>

#include "ser_config.h"
#include "ser_intr_dispatch.h"
#include "ser_tcam.h"
#include "ser_bd.h"
 /******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*!
  * \brief max number of interrupt register
  */
#define MAX_NUM_INTR_REGS        12

/******************************************************************************
 * Typedefs
 */

/******************************************************************************
 * Private variables
 */
/*! Used to save interrupt number for L2_MGMG */
static int l2_mgmt_intr_num[BCMDRD_CONFIG_MAX_UNITS];

/*! Used to call routines in CTH sub-component to update/get data to/from IMM LTs */
bcmptm_ser_cth_cb_t *bcmptm_ser_cth_cb = NULL;

/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 * bcmptm_ser_mem_clear should be called before enabling SER in order to clear some PTs
 */
static int
bcmptm_ser_mem_clear(int unit)
{
    bcmptm_ser_mem_clr_list_t *mem_list = NULL;
    int  num_mem = 0;
    int  rv = SHR_E_NONE;
    int  num_insts = 0, i = 0, inst = -1;
    const uint32_t *default_entry_data = NULL;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], ent_size, work_flags = 0;
    int flag = 0;
    bool cache_en = 0;
    bool is_port_reg = FALSE;
    int start_ports, port_ranges, start_indexes, index_ranges;
    uint64_t dma_addr;
    void *buf_paddr = NULL;
    size_t max_entry_size = 0;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, mem_clr_list_get_fn)(unit, &mem_list, &num_mem);

    if (rv == SHR_E_UNAVAIL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Device need not to clear MMU "
                              "memory table specifically\n")));
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    max_entry_size = BCMDRD_MAX_PT_WSIZE * sizeof(uint32_t);
    buf_paddr = bcmdrd_hal_dma_alloc(unit, max_entry_size, "clear_pt", &dma_addr);
    if (NULL == buf_paddr) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to allocate DMA buffer used to clear PTs\n")));
        rv = SHR_E_MEMORY;
        SHR_RETURN_VAL_EXIT(rv);
    }

    /* Those MMU memory buffer must be cleared, because its data can be cleared when MMU block is reset */
    for (i = 0; i < num_mem; i++) {
        default_entry_data = bcmdrd_pt_default_value_get(unit, mem_list[i].mem_sid);
        ent_size = bcmdrd_pt_entry_size(unit, mem_list[i].mem_sid);
        if (default_entry_data != NULL) {
            sal_memcpy(entry_data, default_entry_data, ent_size);
            sal_memcpy(buf_paddr, default_entry_data, ent_size);
        } else {
            sal_memset(entry_data, 0, ent_size);
            sal_memset(buf_paddr, 0, ent_size);
        }
        rv = bcmptm_pt_attr_is_cacheable(unit, mem_list[i].mem_sid, &cache_en);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        flag = (cache_en) ? BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE : 0;

        is_port_reg = PT_IS_PORT_OR_SW_PORT_REG(unit, mem_list[i].mem_sid);

        num_insts = bcmdrd_pt_num_tbl_inst(unit, mem_list[i].mem_sid);
        inst = -1;
        if (mem_list[i].multi_insts) {
            for (inst = 0; inst < num_insts; inst++) {
                if (mem_list[i].index >= 0) {
                    rv = bcmptm_ser_pt_write(unit, mem_list[i].mem_sid,
                                             mem_list[i].index,
                                             inst, NULL, entry_data, flag);
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                } else {
                    work_flags = SBUSDMA_WF_WRITE_CMD | SBUSDMA_WF_SGL_DATA;
                    if (is_port_reg) {
                        start_ports = 0;
                        port_ranges = num_insts;
                        rv = bcmptm_ser_batch_sbusdma_write(unit, mem_list[i].mem_sid, 0,
                                                            &start_ports, &port_ranges,
                                                            NULL, NULL, 1,
                                                            dma_addr, is_port_reg,
                                                            work_flags);
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                        /* clear all instance of port registers, so 'break' */
                        break;
                    } else {
                        start_indexes = bcmdrd_pt_index_min(unit, mem_list[i].mem_sid);
                        index_ranges = bcmdrd_pt_index_max(unit, mem_list[i].mem_sid) + 1;
                        rv = bcmptm_ser_batch_sbusdma_write(unit, mem_list[i].mem_sid, inst,
                                                            NULL, NULL, &start_indexes,
                                                            &index_ranges, 1,
                                                            dma_addr, is_port_reg,
                                                            work_flags);
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                    }
                }
            }
        } else {
            if (mem_list[i].index >= 0) {
                rv = bcmptm_ser_pt_write(unit, mem_list[i].mem_sid,
                                         mem_list[i].index,
                                         inst, NULL, entry_data, flag);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } else {
                work_flags = SBUSDMA_WF_WRITE_CMD | SBUSDMA_WF_SGL_ADDR;
                if (is_port_reg) {
                    start_ports = 0;
                    port_ranges = num_insts;
                    rv = bcmptm_ser_batch_sbusdma_write(unit, mem_list[i].mem_sid, 0,
                                                        &start_ports, &port_ranges,
                                                        NULL, NULL, 1,
                                                        dma_addr, is_port_reg,
                                                        work_flags);
                } else {
                    start_indexes = bcmdrd_pt_index_min(unit, mem_list[i].mem_sid);
                    index_ranges = bcmdrd_pt_index_max(unit, mem_list[i].mem_sid) + 1;
                    rv = bcmptm_ser_batch_sbusdma_write(unit, mem_list[i].mem_sid, inst,
                                                        NULL, NULL, &start_indexes,
                                                        &index_ranges, 1,
                                                        dma_addr, is_port_reg,
                                                        work_flags);
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    if (buf_paddr != NULL) {
        bcmdrd_hal_dma_free(unit, 0, buf_paddr, dma_addr);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to clear MMU memory tables\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mem_init should be called after enabling SER to clear memories
 */
static int
bcmptm_ser_mem_init_after_ser_enable(int unit)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int reg_num = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, clear_mem_after_ser_enable_fn)(unit, &regs_ctrl_info, &reg_num);

    if (rv == SHR_E_UNAVAIL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Device may need to clear "
                             "memory tables after enabling SER checking\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to init memory tables"
                             " after enabling SER checking\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mem_init should be called before enabling SER to clear memories
 */
static int
bcmptm_ser_mem_init_before_ser_enable(int unit)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int  reg_num = 0;
    int  rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, clear_mem_before_ser_enable_fn)(unit, &regs_ctrl_info, &reg_num);

    if (rv == SHR_E_UNAVAIL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Device may need to clear "
                             "memory tables before enabling SER checking\n")));
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to init memory tables"
                             " before enabling SER checking\n")));
    }
    SHR_FUNC_EXIT();
}


/******************************************************************************
* bcmptm_ser_tcam_non_cmic_scan_init used to start IDB H/W to scan TCAMs
 */
static int
bcmptm_ser_tcam_idb_scan_init(int unit)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int  reg_num = 0;
    int  rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, tcam_idb_ser_scan_reg_info_get_fn)(unit, &regs_ctrl_info, &reg_num);

    if (rv == SHR_E_UNAVAIL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Device may need not to enable IDB SER scan\n")));
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to start IDB SER scan\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_ctrl_reg_write used to write parity controlling reg to
* enable or disable all physical tables SER checking
 */
static int
bcmptm_ser_ctrl_reg_write(int unit, bcmdrd_sid_t  reg_sid,
                          bcmdrd_fid_t reg_fid, uint32_t fmask, int enable)
{
    uint32_t reg_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t actual_fmask = 0;
    int rv = SHR_E_NONE;
    int num_inst = 0, inst = -1;

    SHR_FUNC_ENTER(unit);

    sal_memset(reg_data, 0, sizeof(reg_data));

    rv = bcmptm_ser_pt_read(unit, reg_sid, 0, -1, NULL,
                            reg_data, BCMDRD_MAX_PT_WSIZE, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, reg_sid, reg_data, reg_fid, &actual_fmask);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (enable) {
        actual_fmask |= fmask;
    } else {
        actual_fmask &= ~fmask;
    }
    rv = bcmdrd_pt_field_set(unit, reg_sid, reg_data, reg_fid, &actual_fmask);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* access type of MMU_ENQX_ENABLE_ECCP_MEM is duplicate, but table instance is 2 */
    num_inst = bcmdrd_pt_num_tbl_inst(unit, reg_sid);
    if (num_inst == 1) {
        inst = -1;
        rv = bcmptm_ser_pt_write(unit, reg_sid, 0, inst, NULL,
                                 reg_data,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        for (inst = 0; inst < num_inst; inst++) {
            rv = bcmptm_ser_pt_write(unit, reg_sid, 0, inst, NULL,
                                     reg_data,
                                     BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to write controlling reg [%s] to %s "
                             "parity or ecc checking\n"),
                  bcmdrd_pt_sid_to_name(unit, reg_sid),
                  enable ? "enable": "disable"));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_pts_ser_enable used to enable or disable all physical tables SER checking
 */
static void
bcmptm_ser_pts_ser_enable(int unit, bcmdrd_ser_en_type_t ctrl_type, int enable)
{
    bcmdrd_sid_t sid;
    int rv = SHR_E_NONE;
    size_t sid_count = 0;

    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_count);
    if (SHR_FAILURE(rv)) {
        return;
    }
    for (sid = 0; sid < sid_count; sid++) {
        rv = bcmptm_ser_pt_ser_enable(unit, sid, ctrl_type, enable);
        if (SHR_SUCCESS(rv)) {
            if (ctrl_type == BCMDRD_SER_EN_TYPE_EC) {
                (void)BCMPTM_SER_CTH_CB(ser_control_pt_imm_insert)(unit, sid, enable);
            }
        } else {
            if (ctrl_type == BCMDRD_SER_EN_TYPE_EC) {
                (void)BCMPTM_SER_CTH_CB(ser_control_pt_imm_insert)(unit, sid, 0);
            }
        }
    }
}

/******************************************************************************
* bcmptm_ser_all_buffer_bus_enable used to enable or disable all buffers or buses SER checking
 */
static void
bcmptm_ser_all_buffer_bus_enable(int unit,
                                 bcmdrd_ser_en_type_t ctrl_type, int enable)
{
    bcmdrd_ser_rmr_id_t rid;
    int rv = SHR_E_NONE;
    size_t num_rid = 0, i;
    bcmdrd_ser_rmr_id_t *rid_list = NULL;

    rv = bcmdrd_pt_ser_rmr_id_list_get(unit, 0, NULL, &num_rid);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                           "\t Fail to get information of controlling register from drd\n")));
        return;
    }
    rid_list = sal_alloc(sizeof(bcmdrd_ser_rmr_id_t) * (num_rid + 1), "ser_rmr_id_list");
    if (rid_list == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                           "\t Fail to allocate memory space\n")));
        return;
    }

    (void)bcmdrd_pt_ser_rmr_id_list_get(unit, num_rid, rid_list, &num_rid);

    for (i = 0; i < num_rid; i++) {
        rid = rid_list[i];

        (void)bcmptm_ser_buf_bus_enable(unit, rid, ctrl_type, enable);
    }
    sal_free(rid_list);
}

/******************************************************************************
* bcmptm_ser_mmu_blocks_enable used to enable or disable block parity checking and interrupt of mmu
 */
static int
bcmptm_ser_mmu_blocks_enable(int unit,
                             const bcmptm_ser_low_level_info_t *info_list,
                             uint32_t enable)
{
    const bcmptm_ser_low_level_info_t *info;
    int info_index, rv = SHR_E_NONE;
    bcmdrd_sid_t  reg;
    bcmdrd_fid_t  *field_list, fields[2];
    uint32_t en = enable ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == BCMPTM_SER_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }
        rv = SHR_E_NONE;

        switch (info->type) {
            case BCMPTM_SER_PARITY_TYPE_MMU_SER:
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "(MMU) Enable MMU SER interrupts\n")));
                /* Interrupt enable */
                reg = info->intr_enable_reg;
                if (reg != INVALIDr) {
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "Can't write register to enable"
                                         " MMU SER interrupt directly,"
                                         " suggest you to enable MMU interrupt"
                                         " by API bcmbd_mmu_intr_enable\n")));
                    rv = SHR_E_INTERNAL;
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }

                rv = bcmptm_ser_mmu_blocks_enable(unit, info->info, en);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
                break;
            case BCMPTM_SER_PARITY_TYPE_MMU_GLB:
            case BCMPTM_SER_PARITY_TYPE_MMU_XPE:
            case BCMPTM_SER_PARITY_TYPE_MMU_SC:
            case BCMPTM_SER_PARITY_TYPE_MMU_SED:
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "(MMU) Enable MMU SER parity or ECC checking\n")));
                /* Parity enable */
                reg = info->enable_reg;
                field_list = info->enable_field_list;
                if (reg != INVALIDr) {
                    if (field_list == NULL) {
                        field_list = &fields[0];
                        fields[0] = info->enable_field;
                        fields[1] = INVALIDf;
                    }
                    rv = bcmptm_ser_reg_multi_fields_modify(unit, reg, field_list, en, NULL, NULL);
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }

                /* Interrupt enable */
                reg = info->intr_enable_reg;
                if (reg != INVALIDr) {
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "Can't write register to enable"
                                         " MMU SER interrupt directly,"
                                         " suggest you to enable MMU interrupt"
                                         " by API bcmbd_mmu_intr_enable\n")));
                    rv = SHR_E_INTERNAL;
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
                break;
            default:
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "(MMU)Some cases are not taken into account\n")));
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mmu_blocks_enable used to enable or disable blocks(except for mmu) parity checking
 */
static int
bcmptm_ser_non_mmu_blocks_enable(int unit,
                             bcmptm_ser_top_level_route_info_t *top_rb,
                             uint32_t enable)
{
    bcmptm_ser_low_level_info_t *info_list = (bcmptm_ser_low_level_info_t *)top_rb->info;
    const bcmptm_ser_low_level_info_t *info;
    int info_index, rv = SHR_E_NONE, port, port_end;
    bcmdrd_sid_t  reg;
    bcmdrd_fid_t  field;
    uint32_t      data_entry[BCMDRD_MAX_PT_WSIZE] = {0}, rval;

    SHR_FUNC_ENTER(unit);

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == BCMPTM_SER_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }
        switch (info->type) {
            case BCMPTM_SER_PARITY_TYPE_ECC:
            case BCMPTM_SER_PARITY_TYPE_PARITY:
            case BCMPTM_SER_PARITY_TYPE_CLMAC:
            case BCMPTM_SER_PARITY_TYPE_MAC_RX_TS:
            case BCMPTM_SER_PARITY_TYPE_MAC_RX_CDC:
            case BCMPTM_SER_PARITY_TYPE_MAC_TX_CDC:
                reg = info->enable_reg;
                field = info->enable_field;
                if (reg == INVALIDr || field == INVALIDf) {
                    break;
                }
                rv = bcmptm_ser_non_mmu_status_reg_inst_get(unit, top_rb,
                                                            reg, &port, &port_end);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
                for (; port <= port_end; port++) {
                    rv = bcmptm_ser_pt_read(unit, reg, 0, port, NULL,
                                            data_entry, BCMDRD_MAX_PT_WSIZE,
                                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
                    SHR_IF_ERR_VERBOSE_EXIT(rv);

                    rval = enable ? 1 : 0;
                    rv = bcmdrd_pt_field_set(unit, reg, data_entry, field, &rval);
                    SHR_IF_ERR_VERBOSE_EXIT(rv);

                    rv = bcmptm_ser_pt_write(unit, reg, 0, port, NULL, data_entry,
                                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
                break;
            default:
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "\t(non MMU)new SER type[%d] is not taken into account\n"
                                     "The new SER type should be add to routine bcmptm_ser_non_mmu_blocks_enable"
                                     " and bcmptm_ser_mac_status_reg_read_clear\n"),
                          info->type));
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_group_reg_enable used to enable or disable top level register
 */
static int
bcmptm_ser_group_reg_enable(int unit,
                            const bcmptm_ser_low_level_info_t *info_list,
                            bcmdrd_sid_t group_enable_reg,
                            int port, uint32_t enable)
{
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t rval;
    int rv = SHR_E_NONE, info_idx;
    bcmdrd_fid_t group_enable_field;

    SHR_FUNC_ENTER(unit);
    rv = bcmptm_ser_pt_read(unit, group_enable_reg, 0, port, NULL,
                            entry_data, BCMDRD_MAX_PT_WSIZE, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (info_idx = 0; ; info_idx++) {
        if (info_list[info_idx].type == BCMPTM_SER_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }
        group_enable_field = info_list[info_idx].group_reg_enable_field;
        if (group_enable_field != INVALIDf) {
            /* Enable the info entry in the group register */
            rval = enable ? 1 : 0;
            rv = bcmdrd_pt_field_set(unit, group_enable_reg, entry_data,
                                     group_enable_field, &rval);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
    rv = bcmptm_ser_pt_write(unit, group_enable_reg,
                             0, port, NULL, entry_data,
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_cmic_reg_enable used to enable or disable cmic level register
 */
static void
bcmptm_ser_cmic_reg_enable(int unit,
                            uint32_t *cmic_rval, int size, uint32_t enable)
{
    int cmic_id = 0, i, rv = SHR_E_NONE;
    uint32_t intr_num = 0;
    int max_regs_num = (size < MAX_NUM_INTR_REGS) ? size : MAX_NUM_INTR_REGS;
    int en = enable ? 1 : 0;

    for (cmic_id = 0; cmic_id < max_regs_num; cmic_id++) {
        if (cmic_rval[cmic_id] == 0) {
            continue;
        }
        for (i = 0; i < 32; i++) {
            if ((cmic_rval[cmic_id] & (1 << i)) == 0) {
                continue;
            }
            BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_intr_enable_fn)(unit, 0, cmic_id, i, en);
            intr_num = cmic_id * 32 + i;
            if (en == 0) {
                continue;
            }
            rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_intr_func_set_fn)(unit, 0, intr_num,
                                                                        bcmptm_ser_notify,
                                                                        bcmptm_ser_mmu_notify);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "\t Fail to register SER callback routine\n")));
            }
        }
    }
}

/******************************************************************************
* bcmptm_ser_blocks_enable used to enable or disable SER function of all blocks
 */
static int
bcmptm_ser_blocks_enable(int unit, int enable)
{
    bcmptm_ser_top_level_route_info_t *top_rb = NULL, *cur_top_rb;
    int i = 0, route_num = 0, rv = SHR_E_NONE;
    uint32_t cmic_bit = 0;
    uint32_t cmic_rval[MAX_NUM_INTR_REGS] = {0}, cmic_id;
    bcmptm_ser_blk_type_t ser_blk_type = 0;
    int port = -1;
    const bcmptm_ser_low_level_info_t *info_list = NULL;
    bcmdrd_fid_t field_list[2] = {INVALIDf, INVALIDf};

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, route_info_get_fn)(unit, &top_rb, &route_num);
    if (rv == SHR_E_UNAVAIL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Can't get SER route information\n")));
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sal_memset(cmic_rval, 0, sizeof(cmic_rval));

    for (i = 0; i < route_num; i++) {
        cur_top_rb = &top_rb[i];
        cmic_bit = cur_top_rb->cmic_bit;
        if (0 == cmic_bit) {
            /* end */
            break;
        }
        cmic_id = cur_top_rb->cmic_reg;
        if (cmic_id < MAX_NUM_INTR_REGS) {
            cmic_rval[cmic_id] |= cmic_bit;
        } else {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "\t ID of cmic interrupt register is invalid\n")));
        }
        if (cur_top_rb->enable_reg != INVALIDr &&
            cur_top_rb->enable_field != INVALIDf) {
            field_list[0] = cur_top_rb->enable_field;
            field_list[1] = INVALIDf;
            rv = bcmptm_ser_reg_multi_fields_modify(unit, cur_top_rb->enable_reg,
                                                    field_list, enable ? 1 : 0, NULL, NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        if (cur_top_rb->info == NULL) {
            continue;
        }
        info_list = (bcmptm_ser_low_level_info_t *)cur_top_rb->info;
        port= -1;

        ser_blk_type = cur_top_rb->blocktype;
        if (ser_blk_type == BCMPTM_SER_BLK_MMU) {
            rv = bcmptm_ser_mmu_blocks_enable(unit, info_list, enable);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            rv = bcmptm_ser_non_mmu_blocks_enable(unit, cur_top_rb, enable);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        if (cur_top_rb->enable_reg != INVALIDr) {
            if (bcmdrd_pt_is_soft_port_reg(unit, cur_top_rb->enable_reg)) {
                port = cur_top_rb->blk_inst;
            } else if (bcmdrd_pt_is_port_reg(unit, cur_top_rb->enable_reg)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "\t can't access to per port group enable register\n")));
            }
            rv = bcmptm_ser_group_reg_enable(unit, info_list,
                                             cur_top_rb->enable_reg, port, enable);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
    /* enable cmic interrupt registers */
    bcmptm_ser_cmic_reg_enable(unit, cmic_rval, MAX_NUM_INTR_REGS, enable);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to enable ser blocks\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_l2_mgmt_intr_enable used to enable or disable interrupt of l2_mgmt
 */
static int
bcmptm_ser_l2_mgmt_intr_enable(int unit, uint32_t enable)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int reg_num, rv = SHR_E_NONE, intr_num = 0;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, l2_mgmt_ser_enable_info_get_fn)(unit, &regs_ctrl_info,
                                                                         &reg_num, &intr_num);
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_intr_enable_fn)(unit, 0, intr_num / 32, intr_num % 32, enable);
    if (enable == 0) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_intr_func_set_fn)
            (unit, 0, intr_num, bcmptm_ser_notify, bcmptm_ser_mmu_notify);
    SHR_IF_ERR_EXIT(rv);

    l2_mgmt_intr_num[unit] = intr_num;

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to enable l2_mgmt ser\n")));
    }
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public Functions
 */

/******************************************************************************
 * bcmptm_ser_l2_mgmt_intr_num_get used to get interrupt source number of l2_mgmt
 */
int
bcmptm_ser_l2_mgmt_intr_num_get(int unit)
{
    return l2_mgmt_intr_num[unit];
}

/******************************************************************************
 * bcmptm_pt_clear
 */
int
bcmptm_pt_clear(int unit, int flags)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (flags & PT_CLEAR_BEFORE_SER_ENABLE) {
        rv = bcmptm_ser_mem_clear(unit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_mem_init_before_ser_enable(unit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (flags & PT_CLEAR_AFTER_SER_ENABLE) {
        rv = bcmptm_ser_mem_init_after_ser_enable(unit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_comp_config
 */
int
bcmptm_ser_comp_config(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    bool enable = SER_CHECKING_ENABLE;

    SHR_FUNC_ENTER(unit);

    enable = BCMPTM_SER_CTH_CB(ser_checking_start)(unit);
    if (enable == SER_CHECKING_DISABLE) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SER function is disabled\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    if (warm) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SER Warmboot boot!!\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Welcome to SER configuration\n")));

    rv = bcmptm_pt_clear(unit, PT_CLEAR_BEFORE_SER_ENABLE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_l2_mgmt_intr_enable(unit, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* enable parity or ecc checking of memory tables, registers */
    bcmptm_ser_pts_ser_enable(unit, BCMDRD_SER_EN_TYPE_EC, 1);

    rv = bcmptm_ser_tcam_cmic_scan_init(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* REPORT_SINGLE_BIT_ECC: Default value is true */
    if (BCMPTM_SER_CONTROL_REPORT_SINGLE_BIT_ECC(unit)) {
        bcmptm_ser_pts_ser_enable(unit, BCMDRD_SER_EN_TYPE_ECC1B, 1);
    }
    /* enable parity or ecc checking of buffers and buses */
    bcmptm_ser_all_buffer_bus_enable(unit, BCMDRD_SER_EN_TYPE_EC, 1);
    bcmptm_ser_all_buffer_bus_enable(unit, BCMDRD_SER_EN_TYPE_ECC1B, 1);

    rv = bcmptm_ser_blocks_enable(unit, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_pt_clear(unit, PT_CLEAR_AFTER_SER_ENABLE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_tcam_idb_scan_init(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_pt_ser_enable used to enable or disable one physical table SER checking
 */
int
bcmptm_ser_pt_ser_enable(int unit, bcmdrd_sid_t sid,
                         bcmdrd_ser_en_type_t ctrl_type, int enable)
{
    int rv = SHR_E_NONE;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    bcmdrd_sid_t *mem_ser_mute_list = NULL;
    int mem_ser_mute_num = 0;
    int i;

    SHR_FUNC_ENTER(unit);
    /* skip appointed memory tables */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, mute_mem_list_get_fn)(unit, &mem_ser_mute_list, &mem_ser_mute_num);
    for (i = 0; i < mem_ser_mute_num; i++) {
        if (sid == mem_ser_mute_list[i]) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\t Need not to enable memory %s "
                                    "parity or ecc checking[type==%d]\n"),
                         bcmdrd_pt_sid_to_name(unit, sid),
                         ctrl_type));
            rv = SHR_E_UNAVAIL;
            SHR_RETURN_VAL_EXIT(rv);
        }
    }
    sal_memset(&ctrl_info, 0, sizeof(ctrl_info));

    rv = bcmdrd_pt_ser_en_ctrl_get(unit, ctrl_type, sid, &ctrl_info);

    if (SHR_FAILURE(rv)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t memory[%s] may not have capability "
                              "of [type==%d] parity checking\n"),
                   bcmdrd_pt_sid_to_name(unit, sid), ctrl_type));
        rv = SHR_E_UNAVAIL;
        SHR_RETURN_VAL_EXIT(rv);
    }
    rv = bcmdrd_pt_is_valid(unit, ctrl_info.sid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (ctrl_info.fmask == 0) {
        ctrl_info.fmask = 1;
    }
    rv = bcmptm_ser_ctrl_reg_write(unit, ctrl_info.sid, ctrl_info.fid,
                                   ctrl_info.fmask, enable);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        if (SHR_E_UNAVAIL != rv) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "\t Fail to %s memory [%s] parity or ecc checking\n"),
                       enable ? "enable": "disable",
                       bcmdrd_pt_sid_to_name(unit, sid)));
        }
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_buf_bus_enable used to enable or disable SER checking of one buffer or bus
 */
int
bcmptm_ser_buf_bus_enable(int unit, bcmdrd_ser_rmr_id_t rid,
                          bcmdrd_ser_en_type_t ctrl_type, int enable)
{
    int rv = SHR_E_NONE;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ctrl_info, 0, sizeof(ctrl_info));

    rv = bcmdrd_pt_ser_rmr_en_ctrl_get(unit, ctrl_type, rid, &ctrl_info);
     if (SHR_FAILURE(rv)) {
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "\t Fail to get buffer or bus [%d]"
                                 " controlling information\n"), rid));
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }

     rv = bcmdrd_pt_is_valid(unit, ctrl_info.sid);
     if (SHR_FAILURE(rv)) {
         LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Controlling SID [%d] is invalid\n"),
                   ctrl_info.sid));
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }
     if (ctrl_info.fmask == 0) {
         ctrl_info.fmask = 1;
     }
     LOG_DEBUG(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "\t [%s], its control type [%d],"
                           " control register [%s], control field [%d]\n"),
                bcmdrd_pt_ser_rmr_to_name(unit, rid), ctrl_type,
                bcmdrd_pt_sid_to_name(unit, ctrl_info.sid), ctrl_info.fid));

     rv = bcmptm_ser_ctrl_reg_write(unit, ctrl_info.sid, ctrl_info.fid,
                                    ctrl_info.fmask, enable);
     if (SHR_FAILURE(rv)) {
         LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to write controlling register\n")));
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_cth_cb_register used to record callback routines in cth/ser
 */
void
bcmptm_ser_cth_cb_register(bcmptm_ser_cth_cb_t *cth_cb_fn)
{
    bcmptm_ser_cth_cb = cth_cb_fn;
}

