/*! \file ser_tcam.c
 *
 * Functions for cmic TCAM SER.
 *
 * Include cmic TCAM SER init etc.
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
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_time.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>

#include "ser_tcam.h"
#include "ser_intr_dispatch.h"
#include "ser_bd.h"
#include "ser_config.h"
/******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER


/******************************************************************************
 * Typedefs
 */

/******************************************************************************
 * Private variables
 */
/*! Used to save interrupt number for CMIC SER engine number */
static int cmic_ser_engine_intr_num[BCMDRD_CONFIG_MAX_UNITS];


/******************************************************************************
 * Private Functions
 */
/******************************************************************************
 * bcmptm_ser_granularity_get, get usage granularity per memory table entry
 */
static int
bcmptm_ser_granularity_get(bcmptm_ser_protection_type_t prot_type,
                           bcmptm_ser_protection_mode_t prot_mode,
                           bcmptm_ser_start_end_bits_t *bits)
{
    int i, words = 0, ser_mem_gran;

    for (i = 0; i < BCMPTM_NUM_PROT_SECTIONS; i++) {
        if (bits[i].start_bit >= 0) {
            words++;
        }
    }
    if (prot_type == BCMPTM_SER_TYPE_PARITY) {
        switch (prot_mode) {
        case BCMPTM_SER_PARITY_2BITS:
            ser_mem_gran = 2;
            break;
        case BCMPTM_SER_PARITY_4BITS:
            ser_mem_gran = 4;
            break;
        case BCMPTM_SER_PARITY_8BITS:
            ser_mem_gran = 8;
            break;
        case BCMPTM_SER_PARITY_1BIT:
        default:
            ser_mem_gran = 1;
            break;
        }
        return ser_mem_gran;
    } else {
        switch (prot_mode) {
        case BCMPTM_SER_ECC_2FLD:
            ser_mem_gran = 2;
            break;
        case BCMPTM_SER_ECC_4FLD:
            ser_mem_gran = 4;
            break;
        case BCMPTM_SER_ECC_1FLD:
        default:
            ser_mem_gran = 1;
            break;
        }
        return ser_mem_gran * words * 9;
    }
}

/******************************************************************************
 * bcmptm_ser_at_map_init, init SER_ACC_TYPE_MAPm for chips own four IP/EP pipes
 */
static int
bcmptm_ser_at_map_init(int unit)
{
    bcmptm_ser_ctrl_reg_info_t *mem_info = NULL;
    int  reg_num = 0;
    int  rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, acc_type_map_info_get_fn)(unit, &mem_info, &reg_num);

    if (rv != SHR_E_NONE && rv != SHR_E_UNAVAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = bcmptm_ser_ctrl_reg_operate(unit, mem_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_cmic_engine_config, config CMIC SER engine
 */
static int
bcmptm_ser_cmic_engine_config(int unit, int hw_ser_ix,
                              bcmptm_ser_tcam_generic_info_t *cur_ser_info,
                              uint32_t mem_start_addr, uint32_t mem_end_addr)
{
    int rv = SHR_E_NONE;
    int i = 0, group_num, blknum;
    bcmdrd_sid_t *range_cfg = NULL, *range_start = NULL;
    bcmdrd_sid_t *range_end = NULL, *range_result = NULL;
    bcmdrd_sid_t *prot_word = NULL;
    bcmdrd_sid_t *range_addr_bits = NULL, *range_disable = NULL;
    bcmdrd_sid_t ser_memory_sid, sid;
    uint32_t     field_val_list[5], entry_data[BCMDRD_MAX_PT_WSIZE];
    bcmdrd_fid_t *range_cfg_flds = NULL, *range_prot_flds = NULL;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_ser_engine_regs_get_fn)(unit,
                                                                      &range_cfg, &range_start, &range_end,
                                                                      &range_result, &prot_word,
                                                                      &range_addr_bits, &range_disable,
                                                                      &group_num, &range_cfg_flds, &range_prot_flds,
                                                                      &ser_memory_sid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (cur_ser_info->ser_section_end >= bcmptm_pt_index_count(unit, ser_memory_sid)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ser_section_end [%d] is larger than depth [%d]"
                                " of memory [%s]\n"),
                     cur_ser_info->ser_section_end,
                     bcmptm_pt_index_count(unit, ser_memory_sid),
                     bcmdrd_pt_sid_to_name(unit, ser_memory_sid)));
        rv = SHR_E_RESOURCE;
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    /* Clear SER_MEMORYt */
    sal_memset(entry_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
    for (i = cur_ser_info->ser_section_start;
         i <= (int)cur_ser_info->ser_section_end; i++) {
        rv = bcmptm_ser_pt_write(unit, ser_memory_sid,
                                 i, -1, NULL, entry_data,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    if (group_num < hw_ser_ix || hw_ser_ix < 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "hw_ser_ix[%d] is not within"
                                "the range of [0] to [%d]\n"),
                     hw_ser_ix, group_num));
    }
    rv = bcmptm_ser_pt_write(unit, range_start[hw_ser_ix],
                             0, -1, NULL, &mem_start_addr,
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, range_end[hw_ser_ix],
                             0, -1, NULL, &mem_end_addr,
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_RANGE_DISABLE) {
        rv = bcmptm_ser_pt_write(unit, range_addr_bits[hw_ser_ix],
                                 0, -1, NULL, &cur_ser_info->addr_start_bit,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_pt_write(unit, range_disable[hw_ser_ix],
                                 0, -1, NULL, &cur_ser_info->addr_mask,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmptm_ser_pt_write(unit, range_result[hw_ser_ix],
                             0, -1, NULL, &(cur_ser_info->ser_section_start),
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (i = 0; i < BCMPTM_NUM_PROT_SECTIONS; i++) {
        if (cur_ser_info->start_end_bits[i].start_bit != -1) {
            sid = prot_word[hw_ser_ix * BCMPTM_NUM_PROT_SECTIONS + i];
            field_val_list[0] = cur_ser_info->start_end_bits[i].start_bit;
            field_val_list[1] = cur_ser_info->start_end_bits[i].end_bit;
            rv = bcmptm_ser_reg_multi_fields_modify(unit, sid, range_prot_flds,
                                                    0, field_val_list, NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
    sid = range_cfg[hw_ser_ix];
    field_val_list[0] = cur_ser_info->intrlv_mode;
    field_val_list[1] = cur_ser_info->prot_mode;
    field_val_list[2] = cur_ser_info->prot_type;
    field_val_list[3] = (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK)? 1 : 0;

    rv = bcmptm_ser_blknum_get(unit, cur_ser_info->mem, 0, -1, &blknum);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    field_val_list[4] = (uint32_t)blknum;
    rv = bcmptm_ser_reg_multi_fields_modify(unit, sid,
                                            range_cfg_flds, 0, field_val_list, NULL);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Range[%2dth](0x%04x, 0x%04x): %s "
                            "Sbus range (0x%08x-0x%08x)\n"),
                 hw_ser_ix, cur_ser_info->ser_section_start,
                 cur_ser_info->ser_section_end,
                 bcmdrd_pt_sid_to_name(unit, cur_ser_info->mem),
                 mem_start_addr, mem_end_addr));

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to configure range[%2dth](0x%04x, 0x%04x) for"
                             " %s owning Sbus range (0x%08x-0x%08x)\n"),
                  hw_ser_ix, cur_ser_info->ser_section_start,
                  cur_ser_info->ser_section_end,
                  bcmdrd_pt_sid_to_name(unit, cur_ser_info->mem),
                  mem_start_addr, mem_end_addr));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/******************************************************************************
 * bcmptm_ser_tcam_cmic_scan_init used to start cmic SER engine to scan TCAM tables
 */
int
bcmptm_ser_tcam_cmic_scan_init(int unit)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL, *cur_ser_info = NULL;
    int tcam_info_num = 0, rv = SHR_E_NONE;
    int info_ix, hw_ser_ix, i;
    bool alias;
    int index_min, index_max, ser_mem_count, ser_mem_gran;
    uint32_t ser_mem_addr = 0, ser_mem_total = 0;
    uint32_t start_addr, end_addr;
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int reg_num = 0;
    int intr_num = 0;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, tcam_info_get_fn)(unit, &tcam_ser_info, &tcam_info_num);
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* some chips have no CMIC SER scan engine */
    if (tcam_info_num == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    rv = bcmptm_ser_at_map_init(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    info_ix = 0;
    hw_ser_ix = 0;
    while (info_ix < tcam_info_num) {
        alias = FALSE;
        cur_ser_info = &(tcam_ser_info[info_ix]);
        if (!bcmdrd_pt_attr_is_cam(unit, cur_ser_info->mem)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        cur_ser_info->ser_hw_index = -1;
        index_min = bcmdrd_pt_index_min(unit, cur_ser_info->mem);
        index_max = bcmdrd_pt_index_max(unit, cur_ser_info->mem);
        ser_mem_count = index_max - index_min + 1;
        if (!ser_mem_count) {
            /* We want to keep constant map
             * between info_ix and hw_ser_ix. In other words, if some mem
             * is not enabled in lower latency mode, we want to keep the
             * bit position for that mem in SER_RANGE_ENABLE constant
             * - because in tomahawk/ser.c, this bit position is hard-coded
             * based on max_latency_mode. Otherwise, we will have to keep
             * track of this position for mem based on latency modes.
             */
            hw_ser_ix++;
            info_ix++;
            continue;
        }
        rv =bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem, -1,
                                   index_min, &start_addr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv =bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem, -1,
                                   index_max, &end_addr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        ser_mem_gran = bcmptm_ser_granularity_get(cur_ser_info->prot_type,
                                                  cur_ser_info->prot_mode,
                                                  cur_ser_info->start_end_bits);
        ser_mem_count *= ser_mem_gran;
        /* Width of each entry of SER_MEMORYm is 32 */
        ser_mem_count = ((ser_mem_count + 31) / 32);
        if (cur_ser_info->alias_mem != INVALIDm) {
            /* See if the aliased memory has already been configured */
            for (i = 0; i < info_ix; i++) {
                if (tcam_ser_info[i].mem == cur_ser_info->alias_mem) {
                    alias = TRUE;
                    break;
                }
            }
        }
        if (alias) {
            cur_ser_info->ser_section_start = tcam_ser_info[i].ser_section_start;
            cur_ser_info->ser_section_end = tcam_ser_info[i].ser_section_end;
            cur_ser_info->ser_hw_index = hw_ser_ix;
        } else {
            if (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK) {
                ser_mem_count *= cur_ser_info->num_instances;
            } else if ((cur_ser_info->ser_flags & BCMPTM_SER_FLAG_MULTI_PIPE) &&
                       (info_ix > 0) && (tcam_ser_info[info_ix - 1].mem == cur_ser_info->mem)) {
                /* Copy info for 1/2/3 instance of  the same mem */
                i = info_ix - 1;
                cur_ser_info->ser_section_start = tcam_ser_info[i].ser_section_start;
                cur_ser_info->ser_section_end = tcam_ser_info[i].ser_section_end;
                cur_ser_info->ser_hw_index = tcam_ser_info[i].ser_hw_index;
                cur_ser_info->ser_dynamic_state |= BCMPTM_SER_FLAG_SER_ENGINE;
                info_ix++;
                continue;
            }

            ser_mem_addr = ser_mem_total;
            ser_mem_total += ser_mem_count;

            /* Record section for mem_clear use */
            cur_ser_info->ser_section_start = ser_mem_addr;
            cur_ser_info->ser_section_end = ser_mem_total - 1;
            cur_ser_info->ser_hw_index = hw_ser_ix;
        }
        rv = bcmptm_ser_cmic_engine_config(unit, hw_ser_ix, cur_ser_info,
                                           start_addr, end_addr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (!(cur_ser_info->ser_flags & BCMPTM_SER_FLAG_VIEW_DISABLE)) {
            cur_ser_info->ser_dynamic_state |= BCMPTM_SER_FLAG_SER_ENGINE;
            /* PT is protected by unique mode */
            rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_ser_engine_update_fn)(unit, hw_ser_ix, 1);
            SHR_IF_ERR_EXIT(rv);
            /* Disable global mode protection */
            if (cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK) {
                rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_ser_engine_update_fn)(unit, (hw_ser_ix - 1), 0);
                SHR_IF_ERR_EXIT(rv);
            }
            BCMPTM_SER_IMM_CONTROL_PT_ECC_PARITY_CHECK_SET(unit, cur_ser_info->mem, 1);
            BCMPTM_SER_IMM_CONTROL_PT_SW_SCAN_SET(unit, cur_ser_info->mem, 1);
        }
        hw_ser_ix++;
        info_ix++;
    }
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_ser_engine_enable_fn)(unit, &regs_ctrl_info,
                                                                    &reg_num, &intr_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_intr_enable_fn)(unit, 0, intr_num / 32, intr_num % 32, 1);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_intr_func_set_fn)(unit, 0, intr_num,
                                                                bcmptm_ser_notify,
                                                                bcmptm_ser_mmu_notify);
    SHR_IF_ERR_EXIT(rv);

    cmic_ser_engine_intr_num[unit] = intr_num;

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to configure CMIC SER engine\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_tcam_cmic_intr_num_get used to get interrupt source number of cmic SER engine
 */
int
bcmptm_ser_tcam_cmic_intr_num_get(int unit)
{
    return cmic_ser_engine_intr_num[unit];
}

/******************************************************************************
 * bcmptm_ser_tcam_fifo_pop, get SER information from SER_RESULT_0m or SER_RESULT_1m
 */
int
bcmptm_ser_tcam_fifo_pop(int unit, uint32_t intr_param,
                         bcmptm_ser_top_level_route_info_t *top_rb_info,
                         bcmptm_ser_raw_info_t *crt_info)
{
    int rv = SHR_E_NONE, err_num = 0;
    bcmptm_ser_ctrl_reg_info_t *ser_error_list;
    bcmptm_ser_ctrl_reg_info_t *ser_result_0, *ser_result_1;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], count = 0;
    bcmptm_ser_tcam_crt_info_t *tcam_crt_info = (bcmptm_ser_tcam_crt_info_t *)(&crt_info->serc_info);

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_ser_result_get_fn)(unit, &ser_error_list, &ser_result_0,
                                                                 &ser_result_1, &err_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, ser_error_list, err_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sal_memset(entry_data, 0, sizeof(entry_data));
    crt_info->valid = 1;

    if (ser_error_list[0].ctrl_flds_val[0].val[unit] != 0) {
        rv = bcmptm_ser_ctrl_reg_operate(unit, ser_result_0, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        tcam_crt_info->hw_idx = ser_result_0->ctrl_flds_val[0].val[unit];
        tcam_crt_info->sbus_addr = ser_result_0->ctrl_flds_val[1].val[unit];
        tcam_crt_info->acc_type = ser_result_0->ctrl_flds_val[2].val[unit];
        /* clear ser_result_0m, also clear interrupt */
        rv = bcmptm_ser_pt_write(unit, ser_error_list[0].ctrl_reg,
                                 0, -1, NULL, entry_data, 0);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else if (ser_error_list[1].ctrl_flds_val[0].val[unit] != 0) {
        rv = bcmptm_ser_ctrl_reg_operate(unit, ser_result_1, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        tcam_crt_info->hw_idx = ser_result_1->ctrl_flds_val[0].val[unit];
        tcam_crt_info->sbus_addr = ser_result_1->ctrl_flds_val[1].val[unit];
        tcam_crt_info->acc_type = ser_result_1->ctrl_flds_val[2].val[unit];
        /* clear ser_result_1m, also clear interrupt */
        rv = bcmptm_ser_pt_write(unit, ser_error_list[1].ctrl_reg,
                                 0, -1, NULL, entry_data, 0);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        /* All SER information have read */
        crt_info->valid = 0;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Data come from register SER_MISSED_EVENTr */
    count = ser_error_list[2].ctrl_flds_val[0].val[unit];
    if (count != 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Overflow events: %d.\n"), count));
    }
exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to pop data from SER_RESULT_0/1m !\n")));
        crt_info->valid = 0;
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_tcam_fifo_entry_parse
 */
int
bcmptm_ser_tcam_fifo_entry_parse(int unit,
                                 bcmptm_ser_raw_info_t *crt_info,
                                 bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL, *cur_ser_info = NULL;
    int tcam_info_num = 0, info_idx, rv = SHR_E_NONE;
    int phys_index_min, phys_index_max;
    int ser_mem_count = 0, pipe_num = -1;
    uint32_t start_addr, end_addr;
    bcmptm_ser_tcam_crt_info_t *tcam_crt_info = (bcmptm_ser_tcam_crt_info_t *)(&crt_info->serc_info);
    uint32_t hw_idx = tcam_crt_info->hw_idx;
    int acc_type = (int)tcam_crt_info->acc_type;
    uint32_t sbus_addr = tcam_crt_info->sbus_addr;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, tcam_info_get_fn)(unit, &tcam_ser_info, &tcam_info_num);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (tcam_info_num <= 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_EMPTY);
    }

    info_idx = 0;
    while (info_idx < tcam_info_num) {
        if (tcam_ser_info[info_idx].ser_hw_index == hw_idx) {
            break;
        }
        info_idx++;
    }
    if (info_idx == tcam_info_num) {
        rv = SHR_E_NOT_FOUND;
        SHR_RETURN_VAL_EXIT(rv);
    }
    cur_ser_info = &(tcam_ser_info[info_idx]);

    if (!(cur_ser_info->ser_flags & BCMPTM_SER_FLAG_ACC_TYPE_CHK)) {
        pipe_num = -1;
    } else {
        pipe_num = acc_type;
    }

    phys_index_min = bcmdrd_pt_index_min(unit, cur_ser_info->mem);

    phys_index_max = bcmdrd_pt_index_max(unit, cur_ser_info->mem);

    if (BCMPTM_SER_FLAG_REMAP_READ & cur_ser_info->ser_flags) {
        
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER index remapping not supported\n")));
    }
    ser_mem_count = phys_index_max - phys_index_min + 1;
    if (!ser_mem_count) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER parity failure with unavailable mem range\n")));
        rv = SHR_E_FAIL;
        SHR_RETURN_VAL_EXIT(rv);
    }
    rv =bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem,pipe_num,
                               phys_index_min, &start_addr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_addr_get(unit, cur_ser_info->mem, pipe_num,
                                phys_index_max, &end_addr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if ((sbus_addr < start_addr) || (sbus_addr > end_addr)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER parity failure with invalid mem SBUS addr[0x%x]"
                             " start addr[0x%x] end addr[0x%x]!!\n"),
                  sbus_addr, start_addr, end_addr));
        rv = SHR_E_FAIL;
        SHR_RETURN_VAL_EXIT(rv);
    }
    spci->addr = sbus_addr;
    spci->tbl_inst = pipe_num;
    spci->sid = cur_ser_info->mem;
    spci->index = (sbus_addr - start_addr) + phys_index_min;
    spci->flags |= BCMPTM_SER_FLAG_PARITY;
    spci->flags |= BCMPTM_SER_FLAG_TCAM;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Table[%s] instance[%d] index[%d] error at address 0x%08x\n"),
               bcmdrd_pt_sid_to_name(unit, spci->sid), spci->tbl_inst,
               spci->index, spci->addr));
exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to parse SER information for TCAM!\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_tcam_pt_ser_enable
 */
int
bcmptm_ser_tcam_pt_ser_enable(int unit, bcmdrd_sid_t sid, int enable)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL, *cur_ser_info = NULL;
    int tcam_info_num = 0, rv = SHR_E_NONE;
    int info_ix = 0, hw_ser_ix = 0;
    uint32_t ser_engine_flag = 0;
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int reg_num = 0;
    int intr_num = 0;

    SHR_FUNC_ENTER(unit);

    if (sid == INVALIDm) {
        rv = SHR_E_PARAM;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, tcam_info_get_fn)(unit, &tcam_ser_info, &tcam_info_num);
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* some chips have no CMIC SER scan engine */
    if (tcam_info_num == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    while (info_ix < tcam_info_num) {
        cur_ser_info = &(tcam_ser_info[info_ix]);
        ser_engine_flag = (cur_ser_info->ser_dynamic_state & BCMPTM_SER_FLAG_SER_ENGINE) ? 1 : 0;
        if ((cur_ser_info->mem == sid || cur_ser_info->alias_mem == sid)
            && ser_engine_flag) {
            break;
        }
        info_ix++;
    }
    if (info_ix == tcam_info_num) {
        rv = SHR_E_UNAVAIL;
        SHR_RETURN_VAL_EXIT(rv);
    }
    hw_ser_ix = cur_ser_info->ser_hw_index;
    /* Update cache of field SER_RANGE_ENABLEf of reg SER_RANGE_ENABLEr */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_ser_engine_update_fn)(unit, hw_ser_ix, enable);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_ser_engine_enable_fn)(unit, &regs_ctrl_info,
                                                                    &reg_num, &intr_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to enable or disable TCAM SER checking!\n")));
    }
    SHR_FUNC_EXIT();
}

