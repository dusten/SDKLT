/*! \file ser_bd.c
 *
 * APIs of SER sub-component can write or read H/W by BD routine.
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
#include <shr/shr_error.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_ser_internal.h>

#include "ser_bd.h"
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

/******************************************************************************
 * Private Functions
 */
/******************************************************************************
* bcmptm_ser_ip_ep_dup_check used to check whether access type of one PT belonging to IP/EP
* is duplicate.
 */
static int
bcmptm_ser_ip_ep_dup_check(int unit, int blktype, const char *str)
{
    int rv = 0;
    int blktype_ip = -1, blktype_ep = -1;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, blk_type_map_fn)(unit, BCMPTM_SER_BLK_EPIPE, &blktype_ep);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to get blk type of EPIPE\n")));
        return FALSE;
    }
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, blk_type_map_fn)(unit, BCMPTM_SER_BLK_IPIPE, &blktype_ip);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to get blk type of IPIPE\n")));
        return FALSE;
    }
    if ((blktype == blktype_ip || blktype == blktype_ep) &&
        PT_IS_DUPLICATE_ACC_TYPE(str)) {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************
* bcmptm_ser_batch_sbusdma_oper used to read or write PTs by DMA mode.
 */
static int
bcmptm_ser_batch_sbusdma_oper(int unit, bcmdrd_sid_t sid, int tbl_inst,
                              int *start_ports, int *port_ranges,
                              int *start_indexes, int *index_ranges, int work_num,
                              uint64_t buf_paddr, bool is_port_reg, uint32_t work_flags)
{
    bcmdrd_sym_info_t sinfo = {0};
    int rv = SHR_E_NONE, i = 0, need_delete_work = 0;
    int blknum = 0, data_len = 0, size_work_data = 0;
    uint32_t opcode = 0;
    bcmbd_sbusdma_data_t *work_data = NULL;
    bcmbd_sbusdma_work_t work;
    int cmd = 0;

    SHR_FUNC_ENTER(unit);

    if (buf_paddr == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to allocate tcam scan read buffer!\n")));
        rv = SHR_E_MEMORY;
        SHR_IF_ERR_EXIT(rv);
    }
    rv = bcmdrd_pt_info_get(unit, sid, &sinfo);
    SHR_IF_ERR_EXIT(rv);

    if (is_port_reg) {
        rv = bcmptm_ser_blknum_get(unit, sid, start_ports[0], -1, &blknum);
        SHR_IF_ERR_EXIT(rv);
    } else {
        rv = bcmptm_ser_blknum_get(unit, sid, tbl_inst, -1, &blknum);
        SHR_IF_ERR_EXIT(rv);
    }
    /* Structure the work data */
    size_work_data = sizeof(bcmbd_sbusdma_data_t) * work_num;
    work_data = sal_alloc(size_work_data, "work_data");
    if (work_data == NULL) {
        rv = SHR_E_MEMORY;
        SHR_IF_ERR_EXIT(rv);
    }
    sal_memset(work_data, 0, size_work_data);

    data_len = sizeof(uint32_t) * sinfo.entry_wsize;

    if (is_port_reg) {
        if (work_flags & SBUSDMA_WF_WRITE_CMD) {
            cmd = BCMPTM_SER_WRITE_REGISTER_CMD_MSG;
        } else {
            cmd = BCMPTM_SER_READ_REGISTER_CMD_MSG;
        }
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_schan_opcode_get_fn)(unit, cmd, blknum,
                                          sinfo.blktypes, -1, data_len,
                                          0, 0, &opcode);
        SHR_IF_ERR_EXIT(rv);
    } else {
        if (work_flags & SBUSDMA_WF_WRITE_CMD) {
            cmd = BCMPTM_SER_WRITE_MEMORY_CMD_MSG;
        } else {
            cmd = BCMPTM_SER_READ_MEMORY_CMD_MSG;
        }
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_schan_opcode_get_fn)(unit, cmd, blknum,
                                          sinfo.blktypes, tbl_inst, data_len,
                                          0, 0, &opcode);
        SHR_IF_ERR_EXIT(rv);
    }

    /* Structure the work data */
    for (i = 0; i < work_num; i++) {
        if (is_port_reg) {
            rv = bcmptm_ser_pt_addr_get(unit, sid, start_ports[0], 0, &(work_data[i].start_addr));
            SHR_IF_ERR_EXIT(rv);

            work_data[i].op_count = port_ranges[i];
        } else {
            rv = bcmptm_ser_pt_addr_get(unit, sid, tbl_inst, start_indexes[i], &(work_data[i].start_addr));
            SHR_IF_ERR_EXIT(rv);

            work_data[i].op_count = index_ranges[i];
        }
        work_data[i].op_code = opcode;
        work_data[i].attrs = SBUSDMA_DA_APND_BUF;
        work_data[i].data_width = sinfo.entry_wsize;
        work_data[i].addr_gap = 0;
    }
    work_data[0].buf_paddr = buf_paddr;

    /* Structure the read work */
    sal_memset(&work, 0, sizeof(work));

    work.data = work_data;
    work.items = work_num;
    work.flags = work_flags;
    /* work.flags |= SBUSDMA_WF_INT_MODE; */

    /* Create the work */
    if (work_num == 1) {
        rv = bcmbd_sbusdma_light_work_init(unit, &work);
    } else {
        rv = bcmbd_sbusdma_batch_work_create(unit, &work);
    }
    SHR_IF_ERR_EXIT(rv);

    need_delete_work = 1;

    rv = bcmbd_sbusdma_work_execute(unit, &work);
    SHR_IF_ERR_EXIT(rv);

exit:
    /* Delete the work */
    if (work_num > 1 && need_delete_work) {
        (void)bcmbd_sbusdma_batch_work_delete(unit, &work);
    }
    if (work_data != NULL) {
        sal_free(work_data);
        work_data = NULL;
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to sbusdma read/write [%s]!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
/******************************************************************************
 * bcmptm_ser_pt_write
 */
int
bcmptm_ser_pt_write(int unit, bcmdrd_sid_t  sid,
                    int index, int tbl_inst, void *ovrr_info,
                    uint32_t *data, int flags)
{
    int rv = SHR_E_NONE;
    bcmbd_pt_dyn_info_t  dyn_info;
    int ovrr_info_inst = -1;
    void *ovrr_info_ptr = ovrr_info;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;
    int blktype = -1;

    SHR_FUNC_ENTER(unit);

    dyn_info.index = index;
    dyn_info.tbl_inst = tbl_inst;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, sid, &acc_type, &acctype_str);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to get access type of PT(%s)\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        SHR_IF_ERR_EXIT(rv);
    }
    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    if (PT_IS_PORT_OR_SW_PORT_REG(unit, sid)) {
        dyn_info.tbl_inst = (tbl_inst < 0) ? 0 : tbl_inst;
    } else if (bcmptm_ser_ip_ep_dup_check(unit, blktype, acctype_str) &&
        tbl_inst >= 0 && ovrr_info == NULL) {
        ovrr_info_inst = tbl_inst;
        ovrr_info_ptr = (void *)(&ovrr_info_inst);
        dyn_info.tbl_inst = -1;
    }
    rv = bcmptm_scor_write(unit, flags, sid, &dyn_info, ovrr_info_ptr, data);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to write memory (%s) instance [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), dyn_info.tbl_inst));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_pt_read
 */
int
bcmptm_ser_pt_read(int unit, bcmdrd_sid_t  sid,
                   int index, int tbl_inst, void *ovrr_info,
                   uint32_t *data, size_t wsize, int flags)
{
    int rv = SHR_E_NONE;
    bcmbd_pt_dyn_info_t  dyn_info;
    int ovrr_info_inst = -1, blktype = -1;
    void *ovrr_info_ptr = ovrr_info;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    SHR_FUNC_ENTER(unit);

    dyn_info.index = index;
    dyn_info.tbl_inst = tbl_inst;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, sid, &acc_type, &acctype_str);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to get access type of PT(%s)\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        SHR_IF_ERR_EXIT(rv);
    }
    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    if (PT_IS_PORT_OR_SW_PORT_REG(unit, sid)) {
        dyn_info.tbl_inst = (tbl_inst < 0) ? 0 : tbl_inst;
    } else if (bcmptm_ser_ip_ep_dup_check(unit, blktype, acctype_str) &&
        tbl_inst >= 0 && ovrr_info == NULL) {
        if (flags & BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE) {
            ovrr_info_inst = tbl_inst;
            ovrr_info_ptr = (void *)(&ovrr_info_inst);
            dyn_info.tbl_inst = -1;
        } else {
            dyn_info.tbl_inst = 0;
        }
    }

    rv = bcmptm_scor_read(unit, flags, sid, &dyn_info, ovrr_info_ptr, wsize, data);
    /* PTcache is not used for SID */
    if ((flags & BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE) == 0 &&
        rv == SHR_E_UNAVAIL) {
        sal_memset(data, 0, 4 * wsize);
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to read memory(%s)\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_tbl_inst_num_get
 */
int
bcmptm_ser_tbl_inst_num_get(int unit, bcmdrd_sid_t sid, int *inst_num)
{
    int rv = SHR_E_NONE;
    int blk_type = 0;
    const char *acctype_str;
    uint32_t acc_type = 0;
    int is_port_reg = 0;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, sid, &acc_type, &acctype_str);
    SHR_IF_ERR_EXIT(rv);

    is_port_reg = PT_IS_PORT_OR_SW_PORT_REG(unit, sid);

    blk_type = bcmdrd_pt_blktype_get(unit, sid, 0);

    if (bcmptm_ser_ip_ep_dup_check(unit, blk_type, acctype_str) && !is_port_reg) {
        *inst_num = bcmdrd_pt_num_pipe_inst(unit, sid);
    } else {
        *inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to get instance number of PT [%s]!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_batch_sbusdma_read
 */
int
bcmptm_ser_batch_sbusdma_read(int unit, bcmdrd_sid_t sid, int tbl_inst,
                              int *start_ports, int *port_ranges,
                              int *start_indexes, int *index_ranges, int work_num,
                              uint64_t buf_paddr, bool is_port_reg)
{
    return bcmptm_ser_batch_sbusdma_oper(unit, sid, tbl_inst,
                                         start_ports, port_ranges,
                                         start_indexes, index_ranges, work_num,
                                         buf_paddr, is_port_reg, SBUSDMA_WF_READ_CMD);
}

/******************************************************************************
 * bcmptm_ser_batch_sbusdma_write
 */
int
bcmptm_ser_batch_sbusdma_write(int unit, bcmdrd_sid_t sid, int tbl_inst,
                               int *start_ports, int *port_ranges,
                               int *start_indexes, int *index_ranges, int work_num,
                               uint64_t buf_paddr, bool is_port_reg, uint32_t work_flags)
{
    return bcmptm_ser_batch_sbusdma_oper(unit, sid, tbl_inst,
                                         start_ports, port_ranges,
                                         start_indexes, index_ranges, work_num,
                                         buf_paddr, is_port_reg, work_flags);
}

/******************************************************************************
* bcmptm_ser_ctrl_reg_operate write or read controlling register or memory as appointed value
 */
int
bcmptm_ser_ctrl_reg_operate(int unit,
                            bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info,
                            int reg_num)
{
    int  reg_idx = 0, write_time = 0;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], fld_idx = 0, fld_value = 0;
    bcmdrd_fid_t fid = 0;
    int  rv = SHR_E_NONE;
    int  index = 0, inst = 0, inst_num = 1, i = 0;

    SHR_FUNC_ENTER(unit);

    if (regs_ctrl_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
    for (reg_idx = 0; reg_idx < reg_num; reg_idx++) {
        if (regs_ctrl_info[reg_idx].ctrl_reg == INVALIDr ||
            regs_ctrl_info[reg_idx].ctrl_reg == INVALIDm) {
            break;
        }
        inst_num = bcmdrd_pt_num_tbl_inst(unit, regs_ctrl_info[reg_idx].ctrl_reg);
        for (i = 0; i < inst_num; i++) {
            if (inst_num == 1) {
                inst = -1;
            } else {
                inst = i;
            }
            sal_memset(entry_data, 0, sizeof(entry_data));
            index = regs_ctrl_info[reg_idx].ctrl_reg_index;
            rv = bcmptm_ser_pt_read(unit, regs_ctrl_info[reg_idx].ctrl_reg,
                                    index, inst, NULL, entry_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            write_time = 0;
            /* write fields */
            for (fld_idx = 0; fld_idx < regs_ctrl_info[reg_idx].num_fld; fld_idx++) {
                if (regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].rd == 1) {
                    continue;
                }
                write_time++;
                fid = regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].ctrl_fld;
                fld_value = regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].val[unit];
                rv = bcmdrd_pt_field_set(unit, regs_ctrl_info[reg_idx].ctrl_reg,
                                         entry_data, fid, &fld_value);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            if (write_time) {
                rv = bcmptm_ser_pt_write(unit, regs_ctrl_info[reg_idx].ctrl_reg,
                                         index, inst, NULL, entry_data,
                                         BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            if (regs_ctrl_info[reg_idx].utime > 0) {
                sal_usleep(regs_ctrl_info[reg_idx].utime);
            }
            /* read fields */
            sal_memset(entry_data, 0, sizeof(entry_data));
            rv = bcmptm_ser_pt_read(unit, regs_ctrl_info[reg_idx].ctrl_reg,
                                    index, inst, NULL, entry_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            for (fld_idx = 0; fld_idx < regs_ctrl_info[reg_idx].num_fld; fld_idx++) {
                if (regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].rd == 0) {
                    continue;
                }
                fld_value = 0;
                fid = regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].ctrl_fld;
                rv = bcmdrd_pt_field_get(unit, regs_ctrl_info[reg_idx].ctrl_reg,
                                         entry_data, fid, &fld_value);
                regs_ctrl_info[reg_idx].ctrl_flds_val[fld_idx].val[unit] = fld_value;
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to write or read controlling register[%s]\n"),
                             bcmdrd_pt_sid_to_name(unit, regs_ctrl_info[reg_idx].ctrl_reg)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_reg_multi_fields_modify used to modify multiple fields of one register
 */
int
bcmptm_ser_reg_multi_fields_modify(int unit, bcmdrd_sid_t sid,
                                   bcmdrd_fid_t *field_list, uint32_t new_fld_value,
                                   uint32_t *new_fld_val_list, uint32_t *old_fld_val_list)
{
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE] = {0}, fval;
    int rv = SHR_E_NONE, f;
    bcmdrd_sym_field_info_t finfo;
    int inst_num = 0, i = 0;

    SHR_FUNC_ENTER(unit);

    if (field_list == NULL || sid == INVALIDr) {
        return SHR_E_PARAM;
    }

    sal_memset(entry_data, 0, sizeof(entry_data));
    rv = bcmptm_ser_pt_read(unit, sid, 0, -1, NULL, entry_data,
                            BCMDRD_MAX_PT_WSIZE, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (f = 0; ;f++) {
        if (field_list[f] == INVALIDf) {
            break;
        }
        if (new_fld_val_list == NULL) {
            fval = new_fld_value;
        } else {
            fval = new_fld_val_list[f];
        }
        rv = bcmdrd_pt_field_info_get(unit, sid, field_list[f], &finfo);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "\t Can't find field [%d] of reg/mem [%s]\n"),
                      field_list[f],
                      bcmdrd_pt_sid_to_name(unit, sid)));
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        if (old_fld_val_list != NULL) {
            rv = bcmdrd_pt_field_get(unit, sid, entry_data, field_list[f], &old_fld_val_list[f]);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\t Try to modify field [%s] of reg/mem [%s] as [0x%x]\n"),
                     finfo.name,
                     bcmdrd_pt_sid_to_name(unit, sid), fval));
        rv = bcmdrd_pt_field_set(unit, sid, entry_data, field_list[f], &fval);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    if (inst_num == 1) {
        rv = bcmptm_ser_pt_write(unit, sid, 0, -1, NULL, entry_data,
                                 BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        /* for unique */
        for (i = 0; i < inst_num; i++) {
            rv = bcmptm_ser_pt_write(unit, sid, 0, i, NULL, entry_data,
                                     BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\t Fail to modify reg/mem [%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_blknum_get, get physical block number of  SID
 */
int
bcmptm_ser_blknum_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                      int blk_type, int *blk_num)
{
    int blktype_tmp;
    const bcmdrd_chip_info_t *cinfo = NULL;
    bcmdrd_sym_info_t sinfo;
    int port, inst;
    bcmdrd_pblk_t pblk;
    const bcmdrd_symbols_t *symbols = NULL;
    bcmdrd_pipe_info_t drd_pi, *pi = &drd_pi;
    int rv = SHR_E_NONE;
    uint32_t acctype = 0;
    const char *acctype_str = NULL;
    uint32_t linst = 0, pmask = 0;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }
    sal_memset(&sinfo, 0, sizeof(sinfo));

    if (bcmdrd_pt_is_valid(unit, sid)) {
        blktype_tmp = bcmdrd_pt_blktype_get(unit, sid, 0);

        symbols = bcmdrd_dev_symbols_get(unit, 0);
        if (bcmdrd_sym_info_get(symbols, sid, &sinfo) == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, sid, &acctype, &acctype_str);
        SHR_IF_ERR_EXIT(rv);

        sal_memset(pi, 0, sizeof(*pi));
        pi->offset = sinfo.offset;
        pi->acctype = acctype;
        pi->blktype = blktype_tmp;
        pi->baseidx = -1;
        pmask = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_PMASK);
        linst = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);

        if (pmask != 0 || linst != 0) {
            /* Use instance 0 as block instance for per-pipe table. */
            inst = 0;
        } else if(bcmptm_ser_ip_ep_dup_check(unit, blktype_tmp, acctype_str)) {
            /* for ip/ep duplicate access type memory tables */
            inst = 0;
        } else {
            inst = tbl_inst;
        }
    } else if (blk_type >= 0) {
        blktype_tmp = blk_type;
        inst = tbl_inst;
    } else {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid blktype [%d] and invalid SID [%d]\n"),
                             blk_type, sid));
    }

    if (sinfo.flags & BCMDRD_SYMBOL_FLAG_PORT) {
        if (!bcmdrd_pt_is_valid(unit, sid)) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "Invalid port based SID [%d]\n"),
                                 sid));
        }
        /* Iterate over blocks until matching port is found */
        port = tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype_tmp, &pblk) == 0) {
                break;
            }
            blktype_tmp = bcmdrd_pt_blktype_get(unit, sid, blktype_tmp + 1);
        } while (blktype_tmp >= 0);

        if (blktype_tmp < 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit, "port = %d\n"), port));
        }
        *blk_num = pblk.blknum;
    } else {
        if (inst < 0) {
            inst = 0;
        }
        *blk_num = bcmdrd_chip_block_number(cinfo, blktype_tmp, inst);
        if (*blk_num < 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_pt_addr_get, get physical address of one memory or register
 */
int
bcmptm_ser_pt_addr_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                       int index, uint32_t *addr)
{
    int rv = SHR_E_NONE;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    const bcmdrd_chip_info_t *cinfo = NULL;
    int blknum = 0, lane = -1, offset = 0;
    int port = 0, blktype = 0, blkinst = 0;
    bcmdrd_pblk_t pblk = {0};
    uint32_t linst = 0, pmask = 0, sec_size = 0, sect_shft = 0;
    uint32_t acctype = 0;
    const char *acctype_str = NULL;
    bcmdrd_pipe_info_t drd_pi, *pi = &drd_pi;
    int index_tmp = 0;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    rv = bcmdrd_pt_info_get(unit, sid, sinfo);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid sid [%d]\n"), sid));
    }
    offset = sinfo->offset;
    index_tmp = index * sinfo->step_size;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, sid, &acctype, &acctype_str);
    SHR_IF_ERR_EXIT(rv);

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = offset;
    pi->acctype = acctype;
    pi->blktype = blktype;
    pi->baseidx = -1;

    pmask = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_PMASK);
    linst = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);
    sect_shft = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_SECT_SHFT);
    sec_size = (sect_shft == 0) ? 0 : (1 << sect_shft);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_PORT) {
        /* Iterate over blocks until matching port is found */
        port = tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk) == 0) {
                break;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, blktype + 1);
        } while (blktype >= 0);
        if (blktype < 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "blktype = %d, blkinst = %d\n"),
                                 blktype, tbl_inst));
        }
        blknum = pblk.blknum;
        lane = pblk.lane;
    } else {
        if (pmask != 0 || linst != 0) {
            /* Use instance 0 as block instance for per-pipe table. */
            blkinst = 0;
        } else if(bcmptm_ser_ip_ep_dup_check(unit, blktype, acctype_str)) {
            /* for duplicate access type memory tables */
            blkinst = 0;
        } else {
            blkinst = tbl_inst;
        }
        if (blkinst < 0) {
            blkinst = 0;
        }
        blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (blknum < 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "blknum = %d, blkinst = %d\n"),
                                 blknum, blkinst));
        }
        /*
         * The SIDs with UNIQUE access type and BASE_TYPE instance > 1
         * should have been separate to per-pipe SIDs.
         */
        if (pmask != 0 && linst > 1) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                                (BSL_META_U(unit,
                                            "pmask = %d, linst = %d\n"),
                                 pmask, linst));
        }
        if (pmask == 0) {
            if (linst != 0) {
                lane = tbl_inst;
            } else {
                lane = 0;
            }
        }
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        if (linst != 0 && sec_size != 0) {
           offset += tbl_inst * sec_size;
        }
        lane = -1;
    }
    *addr = bcmbd_block_port_addr(unit, blknum, lane, offset, index_tmp);
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_non_mmu_status_reg_inst_get used to get instance number of status registers
 */
int
bcmptm_ser_non_mmu_status_reg_inst_get(int unit,
                                       bcmptm_ser_top_level_route_info_t *top_rb_info,
                                       bcmdrd_sid_t reg,
                                       int *begin_inst_id, int *end_inst_id)
{
    const bcmdrd_chip_info_t *cinfo = NULL;
    int blk_type = 0, blknum = 0, rv = SHR_E_NONE;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, reg, &acc_type, &acctype_str);
    SHR_IF_ERR_EXIT(rv);

    if (top_rb_info->blocktype == BCMPTM_SER_BLK_IPIPE_IDB &&
        PT_IS_UNIQUE_ACC_TYPE(acctype_str)) {
        *begin_inst_id = top_rb_info->pipe;
        *end_inst_id = top_rb_info->pipe;
    } else if (bcmdrd_pt_is_soft_port_reg(unit, reg)) {
        /* Registers can be per PM, such as CLPORT_MIB_RSC1_ECC_STATUSr,
        *  CLPORT_ECC_CONTROLr, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, etc.
        */
        *begin_inst_id = top_rb_info->blk_inst;
        *end_inst_id = top_rb_info->blk_inst;
    } else if (bcmdrd_pt_is_port_reg(unit, reg)) {
        /* Registers can be per port, such as CLMAC_RX_TS_MEM_ECC_STATUSr,
        * CLMAC_CLEAR_ECC_STATUSr etc.
        */
        cinfo = bcmdrd_dev_chip_info_get(unit);
        if (!cinfo) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, blk_type_map_fn)(unit, top_rb_info->blocktype, &blk_type);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        blknum = bcmdrd_chip_block_number(cinfo, blk_type, top_rb_info->blk_inst);
        *begin_inst_id = bcmdrd_chip_port_number(cinfo, blknum, 0);
        *end_inst_id = *begin_inst_id + 3; /* 4 ports per PM */
    } else {
        *begin_inst_id = -1;
        *end_inst_id = -1;
    }
exit:
    SHR_FUNC_EXIT();
}

