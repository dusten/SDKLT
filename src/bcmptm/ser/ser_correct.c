/*! \file ser_correct.c
 *
 * Interface functions for SER correction
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
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_thread.h>
#include <bcmbd/bcmbd.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <sal/sal_types.h>
#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>

#include "ser_correct.h"
#include "ser_intr_dispatch.h"
#include "ser_tcam.h"
#include "ser_tcam_scan.h"
#include "ser_sram_scan.h"
#include "ser_bd.h"
#include "ser_config.h"
/******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER


/******************************************************************************
 * Typedefs
 */
/*!
* When SERC is paused, we need to cache ser_intr_msg to ser_msg_cache,
* in order to prevent intr_dispatch_msg_q overflow.
* This is depth of ser_msg_cache.
*/
#define SER_MSG_CACHE_DEPTH    16

/*!
* Function used to get SER information from SER FIFOs
*/
typedef int(* bcmptm_ser_fifo_pop_fn)(int unit, uint32_t intr_param,
                                bcmptm_ser_top_level_route_info_t *top_rb_info,
                                bcmptm_ser_raw_info_t *crt_info);
/*!
* Function used to parse SER information from SER FIFOs
*/
typedef int(* bcmptm_ser_fifo_parse_fn)(int unit,
                                 bcmptm_ser_raw_info_t *crt_info,
                                 bcmptm_ser_correct_info_t *spci);
/*!
* This struct used to map ser_blk_type with Popping and parsing functions
*/
typedef struct bcmptm_ser_pop_parse_fn_map_s {
    /* SER blk type */
    bcmptm_ser_blk_type_t     ser_blk_type;
    /* Popping function */
    bcmptm_ser_fifo_pop_fn    ser_fifo_pop_fn;
    /* Parsing function */
    bcmptm_ser_fifo_parse_fn  ser_fifo_parse_fn;
} bcmptm_ser_pop_parse_fn_map_t;

/*!
* This struct used to save field value of NG_SER_FIFOm or EGR_SER_FIFOm
* or L2_MGMT_SER_FIFOm
*/
typedef struct bcmptm_ser_ing_egr_fifo_fld_val_s {
    uint32_t valid;      /* VALIDf */
    uint32_t ecc_parity; /* ECC_PARITYf */
    uint32_t mem_type;   /* MEM_TYPEf */
    uint32_t address;    /* ADDRESSf */
    uint32_t pipe_stage; /* PIPE_STAGEf */
    uint32_t intruction; /* INSTRUCTION_TYPEf */
    uint32_t drop;       /* DROPf */
    uint32_t multiple;   /* MULTIPLEf */
    uint32_t non_sbus;   /* NON_SBUSf */
    uint32_t membase;    /* MEMBASEf */
    uint32_t memindex;   /* MEMINDEXf */
    uint32_t regbase;    /* REGBASEf */
    uint32_t regindex;   /* REGINDEXf */
} bcmptm_ser_ing_egr_fifo_fld_val_t;


#define BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, ecc_field_name)  \
    do { \
        const bcmdrd_symbol_t *symbol = NULL; \
        bcmdrd_sym_field_info_t finfo; \
        uint32_t fval = 0;             \
        const bcmdrd_symbols_t *symbols = NULL; \
        symbols = bcmdrd_dev_symbols_get(unit, 0); \
        symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid, ecc_field_name, &finfo); \
        if (symbol) { \
            (void)bcmdrd_pt_field_set(unit, sid, hw_entry, finfo.fid, &fval); \
            (void)bcmdrd_pt_field_set(unit, sid, sw_entry, finfo.fid, &fval); \
        } \
    } while(0);

#define INT_TO_PTR(x)    ((void *)(unsigned long)(x))
#define PTR_TO_INT(x)    ((unsigned long)(x))
#define INVALID_MEM_ADDR    0xFFFFFFFF
/******************************************************************************
 * Private variables
 */
/*! Used to save thread ID of SERC */
static sal_thread_t serc_thread_id[BCMDRD_CONFIG_MAX_UNITS] = {
    SAL_THREAD_ERROR, SAL_THREAD_ERROR, SAL_THREAD_ERROR, SAL_THREAD_ERROR,
    SAL_THREAD_ERROR, SAL_THREAD_ERROR, SAL_THREAD_ERROR, SAL_THREAD_ERROR };
/*! FALSE: SERC correct SER, TRUE: SERC does not correct SER */
static bool serc_thread_pause[BCMDRD_CONFIG_MAX_UNITS];
/*! FALSE: SERC is running, TRUE: SERC is stopped */
static bool serc_thread_stop[BCMDRD_CONFIG_MAX_UNITS];
/*! Tell user the SERC thread had been exited */
static sal_sem_t  serc_terminate_sem[BCMDRD_CONFIG_MAX_UNITS];

/*! Cache of intr_dispatch_msg_q, in order to prevent intr_dispatch_msg_q overflow */
static bcmptm_ser_intr_msg_q_info_t ser_msg_cache[BCMDRD_CONFIG_MAX_UNITS][SER_MSG_CACHE_DEPTH];
/*! Index of  ser_msg_cache */
static int ser_msg_cache_idx = 0;

#define SERC_TERMINATE_FLAG(ser_msg) (serc_thread_stop[ser_msg.unit] &&\
    ser_msg.reg_id == 0 && ser_msg.reg_id == 0)

/*! Recent 32 time reported SID */
#define  SERC_CACHE_MAX_NUM                 32
/*!
 * If more than one SER error in the same buffer or PT are reported within
 * the specified time interval, there may be H/W or configuration issue in the buffer or PT.
 * \ref SERC_CACHE_SEVERITY_THRES is used to guarantee
 * buffer or PT owns H/W or configuration issue.
 */
#define  SERC_CACHE_SEVERITY_THRES          16

/*! Clear ser_corrected_sids every \ref SERC_CACHE_CLEAR_TIME_INTERVAL */
#define  SERC_CACHE_CLEAR_TIME_INTERVAL     5000000

static bcmdrd_sid_t ser_corrected_sids[BCMDRD_CONFIG_MAX_UNITS][SERC_CACHE_MAX_NUM];
static int ser_corrected_index[BCMDRD_CONFIG_MAX_UNITS];
static int ser_corrected_count[BCMDRD_CONFIG_MAX_UNITS];
static int ser_sf_checking_disable[BCMDRD_CONFIG_MAX_UNITS];
static sal_usecs_t ser_sf_checking_latest_time[BCMDRD_CONFIG_MAX_UNITS];

/*! assign SID to ring cache */
#define ASSIGN_NEW_SID_TO_RING_CACHE(_unit, _sid) \
    do { \
        ser_corrected_sids[_unit][ser_corrected_index[_unit]] = _sid; \
        if (ser_corrected_index[_unit] < (SERC_CACHE_MAX_NUM - 1)) { \
            ser_corrected_index[_unit]++; \
        } else { \
            ser_corrected_index[_unit] = 0; \
        } \
        if (ser_corrected_count[_unit] < SERC_CACHE_MAX_NUM) { \
            ser_corrected_count[_unit]++; \
        } \
    } while(0)

/*! Create SID of buffer or bus */
#define BB_MAGIC_SID_CREATE(_membase, _stage_id) \
            ((1 << 31) | (_membase & 0xFF) | ((_stage_id & 0xFF) << 8))
/*! GET membase from SID of buffer or bus */
#define BB_MAGIC_SID_MEMBASE_GET(_sid_magic) \
            ((_sid_magic & 0xFF))
/*! GET ID from SID of buffer or bus */
#define BB_MAGIC_SID_STAGE_ID_GET(_sid_magic) \
            ((_sid_magic & 0xFF00) >> 8)
/******************************************************************************
 * Private Functions
 */
/******************************************************************************
* bcmptm_ser_msg_cache_push can be only used by SERC thread
 */
static int
bcmptm_ser_msg_cache_push(int unit, bcmptm_ser_intr_msg_q_info_t *ser_msg)
{
    if (ser_msg_cache_idx >= SER_MSG_CACHE_DEPTH) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "ser_msg_cache is full\n")));
        return SHR_E_FULL;
    }
    sal_memcpy(&ser_msg_cache[unit][ser_msg_cache_idx], ser_msg, sizeof(bcmptm_ser_intr_msg_q_info_t));
    ser_msg_cache_idx++;
    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_msg_cache_pop can be only used by SERC thread
 */
static int
bcmptm_ser_msg_cache_pop(int unit, bcmptm_ser_intr_msg_q_info_t *ser_msg)
{
    int i = 0;
    if (ser_msg_cache_idx <= 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ser_msg_cache is empty\n")));
        return SHR_E_EMPTY;
    }
    sal_memcpy(ser_msg, &ser_msg_cache[unit][0], sizeof(bcmptm_ser_intr_msg_q_info_t));

    ser_msg_cache_idx--;
    for (i = 0; i < ser_msg_cache_idx; i++) {
        sal_memcpy(&ser_msg_cache[unit][i], &ser_msg_cache[unit][i + 1],
                   sizeof(bcmptm_ser_intr_msg_q_info_t));
    }
    sal_memset(&ser_msg_cache[unit][ser_msg_cache_idx], 0,
               sizeof(bcmptm_ser_intr_msg_q_info_t));

    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_mem_ecc_protection
 */
static bool
bcmptm_ser_mem_ecc_protection(int unit, bcmdrd_sid_t sid)
{
    int ecc_checking = 0;

    (void)BCMPTM_SER_DATA_DRIVER_CB(unit, mem_ecc_info_get_fn)(unit, sid, 0, NULL, NULL, &ecc_checking);

    if (ecc_checking == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
}

 /******************************************************************************
 * bcmptm_ser_mmu_intr_mapping_get
 */
static int
bcmptm_ser_mmu_intr_mapping_get(int unit, uint32_t mmu_intr_num, int inst,
                                bcmptm_ser_mmu_intr_mapping_t **intr_map)
{
    int rv = SHR_E_NONE;
    int intr_map_num = 0, i = 0;
    bcmptm_ser_mmu_intr_mapping_t *ser_mmu_intr_map = NULL;
    uint32_t cmic_intr_id = 0;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, mmu_intr_map_get_fn)(unit, &cmic_intr_id, &ser_mmu_intr_map, &intr_map_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (i = 0; i < intr_map_num; i++) {
        if (mmu_intr_num == ser_mmu_intr_map[i].mmu_intr_num) {
            break;
        }
    }
    if (i == intr_map_num) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Not found MMU interrupt\n")));
        *intr_map = NULL;
        rv = SHR_E_NOT_FOUND;
        SHR_RETURN_VAL_EXIT(rv);
    }
    *intr_map = &ser_mmu_intr_map[i];

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to get MMU SER mapping info\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mmu_cfg_fail_mem_pop used to pop memory MMU_XCFG_MEM_FAIL_ADDR_64m/
* MMU_SCFG_MEM_FAIL_ADDR_64m/MMU_GCFG_MEM_FAIL_ADDR_64m
 */
static int
bcmptm_ser_mmu_cfg_fail_mem_pop(int unit, bcmdrd_sid_t fifo_mem, int inst,
                                bcmdrd_fid_t multi_bits_fid,
                                bcmdrd_fid_t err_type_fid,
                                bcmdrd_fid_t eaddr_fid,
                                bcmptm_ser_blk_type_t ser_blk_type,
                                bcmptm_ser_mmu_crt_info_t *ser_mmu_info)
{
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
    bcmbd_pt_dyn_info_t dyn_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry_data, 0, BCMDRD_MAX_PT_WSIZE * sizeof(uint32_t));
    dyn_info.index = 0;
    dyn_info.tbl_inst = inst;
    /* get block instance */
    ser_mmu_info->blk_inst = inst;
    /* get block type */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, blk_type_map_fn)(unit, ser_blk_type, &(ser_mmu_info->blk_type));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* pop fifo mem */
    rv = bcmbd_pt_pop(unit, fifo_mem, &dyn_info, NULL, entry_data, BCMDRD_MAX_PT_WSIZE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* read data from ERR_MULTf */
    rv = bcmdrd_pt_field_get(unit, fifo_mem, entry_data,
                             multi_bits_fid, &(ser_mmu_info->multi_bits));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* read data from ERR_TYPEf */
    rv = bcmdrd_pt_field_get(unit, fifo_mem, entry_data,
                             err_type_fid, &(ser_mmu_info->err_type));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* read data from EADDRf */
    rv = bcmdrd_pt_field_get(unit, fifo_mem, entry_data,
                             eaddr_fid, &(ser_mmu_info->eaddr));
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mmu_fifo_pop used to pop SER information from
 */
static int
bcmptm_ser_mmu_fifo_pop(int unit, uint32_t intr_param,
                        bcmptm_ser_top_level_route_info_t *top_rb_info,
                        bcmptm_ser_raw_info_t *crt_info)
{
    int rv = SHR_E_NONE;
    uint32_t fifo_stat_val = 0, fld_val = 0;
    bcmptm_ser_mmu_intr_mapping_t *intr_map = NULL;
    bcmptm_ser_mmu_crt_info_t *ser_mmu_info = (bcmptm_ser_mmu_crt_info_t *)(&crt_info->serc_info);
    uint32_t mmu_intr_num = MMU_INTR_NUM_GET(intr_param);
    int inst = MMU_INTR_INST_GET(intr_param);
    uint32_t intr_cnt = 0;

    SHR_FUNC_ENTER(unit);

    crt_info->valid = 0;

    rv = bcmptm_ser_mmu_intr_mapping_get(unit, mmu_intr_num, inst, &intr_map);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_read(unit, intr_map->fifo_status_reg, 0,
                            inst, NULL, &fifo_stat_val, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, intr_map->fifo_status_reg, &fifo_stat_val,
                             intr_map->empty_fid, &fld_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* FIFO memories are empty */
    if (fld_val == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Value of EMPTYf of fifo status register[%s] is TRUE\n"),
                     bcmdrd_pt_sid_to_name(unit, intr_map->fifo_status_reg)));
        rv = bcmbd_mmu_intr_clear(unit, intr_param);
        /* exit */
        SHR_RETURN_VAL_EXIT(rv);
    }
    rv = bcmptm_ser_mmu_cfg_fail_mem_pop(unit, intr_map->fifo_mem, inst,
                                         intr_map->fifo_mem_fids[0],
                                         intr_map->fifo_mem_fids[1],
                                         intr_map->fifo_mem_fids[2],
                                         intr_map->type,
                                         ser_mmu_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    crt_info->valid = 1;

    rv = bcmptm_ser_pt_read(unit, intr_map->ser_fail_ctr_reg, 0,
                            inst, NULL, &intr_cnt, 1,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    LOG_WARN(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Found MMU interrupt from [%s%d], count [%d]\n"),
              intr_map->blk_name, inst, intr_cnt));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "%s %s error at address 0x%08x\n"),
               ser_mmu_info->multi_bits ? "multiple" : "",
               (ser_mmu_info->err_type == 1) ? "1bit": "parity/2bit",
               ser_mmu_info->eaddr));
exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to pop information from MMU FIFO memory\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_mmu_fifo_entry_parse
 */
static int
bcmptm_ser_mmu_fifo_entry_parse(int unit,
                                bcmptm_ser_raw_info_t *crt_info,
                                bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_mmu_crt_info_t *mmu_crt_info = (bcmptm_ser_mmu_crt_info_t *)(&crt_info->serc_info);
    int rv = SHR_E_NONE;
    uint32_t adext = 0;
    bcmptm_cmic_adext_info_t adext_info;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;
    int blktype = mmu_crt_info->blk_type;
    int blkinst = mmu_crt_info->blk_inst;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    /* access type of mmu memory may be duplicate, unique or single */
    /*
    * try to use unique acc_type to find SID,
    * if not found, then try to use default acc_type to find SID.
 */
    adext_info.acc_type = blkinst;
    for (i = 0; i < 2; i++) {
        rv = bcmptm_ser_blknum_get(unit, INVALIDm, 0, blktype,
                                   &(adext_info.blk_num));
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_adext_get_fn)(unit, adext_info, &adext);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* SER checking only for MMU memories */
        rv = bcmbd_pt_addr_decode(unit, adext, mmu_crt_info->eaddr,
                                  BCMDRD_SYMBOL_FLAG_MEMORY, &sid, &dyn_info);
        if(SHR_FAILURE(rv)) {
            adext_info.acc_type = -1;
        } else {
            break;
        }
    }
    if (SHR_FAILURE(rv)) {
        sid = INVALIDm;
    }
    spci->addr = mmu_crt_info->eaddr;
    spci->sid = sid;
    spci->index = dyn_info.index;
    spci->tbl_inst = dyn_info.tbl_inst;

    spci->second_sid = INVALIDm;
    spci->second_addr = INVALID_MEM_ADDR;

    /* spci will be modified in this routine */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, mmu_mem_remap_fn)(unit, spci, adext, mmu_crt_info->blk_inst);

    if (rv != SHR_E_UNAVAIL && SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to remap index or sid of mmu memory table\n")));
        SHR_RETURN_VAL_EXIT(rv);
    }
    if (mmu_crt_info->err_type > 1) {
        spci->flags |= BCMPTM_SER_FLAG_DOUBLEBIT;
    }
    if (mmu_crt_info->multi_bits) {
        spci->flags |= BCMPTM_SER_FLAG_MULTIBIT;
    }
    if (TRUE == bcmptm_ser_mem_ecc_protection(unit, sid)) {
        spci->flags |= BCMPTM_SER_FLAG_ECC;
    } else {
        spci->flags |= BCMPTM_SER_FLAG_PARITY;
    }
    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_MMU;
exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to parse information from MMU FIFO memory\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mac_status_reg_read_clear used to get information from status registers of
* BCMPTM_SER_PARITY_TYPE_CLMAC etc.
* After that, we need to clear status registers
 */
static int
bcmptm_ser_mac_status_reg_read_clear(int unit,
                                     bcmdrd_sid_t clr_reg, bcmdrd_fid_t *clr_fid_ptr,
                                     int clr_fid_idx,
                                     int reg_index, int tbl_inst,
                                     bcmptm_ser_low_level_info_t *low_info,
                                     bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem)
{
    int rv = SHR_E_NONE;
    uint32_t rval = 0, fval = 0;
    bcmptm_ser_info_type_t ser_info_types[] = {
        BCMPTM_SER_PARITY_TYPE_MAC_RX_TS,
        BCMPTM_SER_PARITY_TYPE_MAC_TX_CDC,
        BCMPTM_SER_PARITY_TYPE_MAC_RX_CDC,
        BCMPTM_SER_PARITY_TYPE_CLMAC
    };
    int i = 0, num = COUNTOF(ser_info_types);
    int is_double_bit_err = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num; i++) {
        if (low_info->type == ser_info_types[i]) {
            break;
        }
    }
    if (i >= num) {
        rv = SHR_E_NOT_FOUND;
        SHR_RETURN_VAL_EXIT(rv);
    }

    if (clr_reg == INVALIDr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "The interrupt clear register should not be invalid for MAC\n")));
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }
    for (i = 0; clr_fid_ptr[i] != INVALIDf; i++); /* get number of fids */

    if (i <= clr_fid_idx) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fields number of clear register should"
                              " be equal to the number of status register\n")));
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, mac_err_fid_check_fn)(unit, clr_fid_ptr[clr_fid_idx], &is_double_bit_err);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The field[%d] must be registered to routine bcmptm_ser_mac_err_fid_check\n"),
                  clr_fid_ptr[clr_fid_idx]));
    }
    /* ECC SER type checking */
    if (is_double_bit_err) {
        /* 2bit ECC error */
        ser_misc_mem->err_type = 1;
    } else {
        /* 1bit ECC error */
        ser_misc_mem->err_type = 0;
    }
    /* clear interrupt status register */
    rv = bcmptm_ser_pt_read(unit, clr_reg, reg_index, tbl_inst,
                            NULL, &rval, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    fval = 0;
    rv = bcmdrd_pt_field_set(unit, clr_reg, &rval,
                             clr_fid_ptr[clr_fid_idx], &fval);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, clr_reg, reg_index, tbl_inst,
                             NULL, &rval, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    fval = 1;
    rv = bcmdrd_pt_field_set(unit, clr_reg, &rval,
                             clr_fid_ptr[clr_fid_idx], &fval);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, clr_reg, reg_index, tbl_inst,
                             NULL, &rval, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_ecc_parity_status_reg_get used to get status registers of
* BCMPTM_SER_PARITY_TYPE_ECC/BCMPTM_SER_PARITY_TYPE_PARITY
 */
static int
bcmptm_ser_ecc_parity_status_reg_get(int unit,
                                     bcmdrd_sid_t reg,
                                     uint32_t *status_entry_data,
                                     bcmdrd_fid_t *fld_list,
                                     int *fval_multiple_err,
                                     int *fval_entry_idx,
                                     int *fval_double_bit_err)
{
    int rv = SHR_E_NONE, field_idx = 0;
    int fval[3] = {-1, -1, -1};
    uint32_t unsigned_fval = 0;

    SHR_FUNC_ENTER(unit);

    for (field_idx = 0; fld_list[field_idx] != INVALIDf; field_idx++) {
        rv = bcmdrd_pt_field_get(unit, reg, status_entry_data,
                                 fld_list[field_idx], &unsigned_fval);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ECC or parity status register[%s] may not have field[%d]\n"),
                       bcmdrd_pt_sid_to_name(unit, reg), fld_list[field_idx]));
            SHR_RETURN_VAL_EXIT(rv);
        }
        fval[field_idx] = (int)unsigned_fval;
    }
    /* -1 means that the fields are not used */
    *fval_multiple_err = fval[0];
    *fval_entry_idx = fval[1];
    *fval_double_bit_err = fval[2]; /* parity status registers have no this field */

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_ecc_parity_status_reg_read_clear used to get information from status registers of
* BCMPTM_SER_PARITY_TYPE_ECC or BCMPTM_SER_PARITY_TYPE_PARITY.
* After that, we need to clear status registers
 */
static int
bcmptm_ser_ecc_parity_status_reg_read_clear(int unit,
                                            bcmdrd_sid_t status_reg,
                                            int index, int tbl_inst, uint32_t *entry_data,
                                            bcmptm_ser_low_level_info_t *low_info,
                                            bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem)
{
    int rv = SHR_E_NONE;
    uint32_t rval = 0;
    bcmptm_ser_info_type_t ser_info_types[] = {
        BCMPTM_SER_PARITY_TYPE_ECC,
        BCMPTM_SER_PARITY_TYPE_PARITY
    };
    int i = 0, num = COUNTOF(ser_info_types);

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num; i++) {
        if (low_info->type == ser_info_types[i]) {
            break;
        }
    }
    if (i >= num) {
        rv = SHR_E_NOT_FOUND;
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Not found ser information type %d\n"),
                   low_info->type));
        SHR_RETURN_VAL_EXIT(rv);
    }

    if (low_info->intr_status_field == INVALIDf ||
        low_info->intr_status_field_list == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "[intr_status_field] should be assigned by ECC_ERRf/PAR_ERRf\n"
                              " [intr_status_field_list] should be assigned by other FIDs"
                              " of status register[%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, status_reg)));
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }
    rv = bcmptm_ser_ecc_parity_status_reg_get(unit, status_reg, entry_data,
                                              low_info->intr_status_field_list,
                                              &ser_misc_mem->multi_bits,
                                              &ser_misc_mem->entry_idx,
                                              &ser_misc_mem->err_type);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ser_misc_mem->status_reg = status_reg;
    rval = 0;
    /* clear interrupt status register */
    rv = bcmptm_ser_pt_write(unit, status_reg, index, tbl_inst,
                             NULL, &rval, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}
/******************************************************************************
* bcmptm_ser_group_intr_status_reg_clear used  to clear group interrupt status register
 */
static int
bcmptm_ser_group_intr_status_reg_clear(int unit,
                                       bcmptm_ser_top_level_route_info_t *top_rb_info,
                                       int tbl_inst,
                                       bcmptm_ser_low_level_info_t *low_rb_info)
{
    uint32_t rval = 0, fval = 0;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t  group_enable_reg = top_rb_info->enable_reg;
    bcmdrd_fid_t  group_enable_fid = low_rb_info->group_reg_enable_field;

    SHR_FUNC_ENTER(unit);

    /* read status register */
    rv = bcmptm_ser_pt_read(unit, group_enable_reg, 0, tbl_inst,
                            NULL, &rval, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    fval = 0;

    rv = bcmdrd_pt_field_set(unit, group_enable_reg, &rval,
                             group_enable_fid, &fval);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, group_enable_reg, 0, tbl_inst,
                             NULL, &rval, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    fval = 1;

    rv = bcmdrd_pt_field_set(unit, group_enable_reg, &rval,
                             group_enable_fid, &fval);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, group_enable_reg, 0, tbl_inst,
                             NULL, &rval, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to clear interrupt status register for %s\n"),
                   low_rb_info->mem_str));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_group_intr_statu_reg_check used  to check which SER error type was triggered
* according to value of group interrupt status register
 */
static int
bcmptm_ser_group_intr_status_reg_check(int unit,
                                       bcmptm_ser_top_level_route_info_t *top_rb_info,
                                       int tbl_inst,
                                       bcmptm_ser_low_level_info_t **low_rb_info)
{
    uint32_t rval = 0, fval =0;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t  group_status_reg = top_rb_info->status_reg;
    bcmptm_ser_low_level_info_t *low_rb_cur_info = (bcmptm_ser_low_level_info_t *)top_rb_info->info;
    bcmdrd_fid_t  status_fid;

    SHR_FUNC_ENTER(unit);

    /* read status register */
    rv = bcmptm_ser_pt_read(unit, group_status_reg, 0, tbl_inst,
                            NULL, &rval, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for( ; low_rb_cur_info->type != BCMPTM_SER_PARITY_TYPE_NONE; low_rb_cur_info++)
    {
        status_fid = low_rb_cur_info->group_reg_status_field;

        rv = bcmdrd_pt_field_get(unit, group_status_reg, &rval,
                                 status_fid, &fval);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        if (fval) {
            *low_rb_info = low_rb_cur_info;
            rv = SHR_E_NONE;
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to check group status register[%s] instance[%d] value [0x%08x]\n"),
                   bcmdrd_pt_sid_to_name(unit, group_status_reg),
                   tbl_inst, rval));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_low_level_intr_status_regs_get  get interrupt status registers' ID and their FIDs
 */
static int
bcmptm_ser_low_level_intr_status_regs_get(int unit,
                                          bcmptm_ser_low_level_info_t *low_info,
                                          bcmptm_ser_buf_bus_ctrl_reg_t *status_reg_array,
                                          bcmdrd_fid_t *status_fid_array,
                                          bcmptm_ser_buf_bus_ctrl_reg_t **status_reg_ptr,
                                          bcmdrd_fid_t **status_fid_ptr)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* get SIDs */
    if (low_info->intr_status_reg != INVALIDr) {
        status_reg_array[0].reg = low_info->intr_status_reg;
        status_reg_array[0].mem_str = NULL;
        status_reg_array[1].reg = INVALIDr;
        *status_reg_ptr = status_reg_array;
    } else if (low_info->intr_status_reg_list != NULL) {
        *status_reg_ptr = low_info->intr_status_reg_list;
    } else {
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }

    /* get FIDs, the registers have the same FIDs*/
    if (low_info->intr_status_field != INVALIDf) {
        status_fid_array[0] = low_info->intr_status_field;
        status_fid_array[1] = INVALIDf;
        *status_fid_ptr = status_fid_array;
    } else if (low_info->intr_status_field_list != NULL) {
        *status_fid_ptr = low_info->intr_status_field_list;
    } else {
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get intr status register and its field\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_low_level_intr_clr_regs_get  get interrupt clear register's ID and their FIDs
 */
static int
bcmptm_ser_low_level_intr_clr_regs_get(int unit,
                                       bcmptm_ser_low_level_info_t *low_info,
                                       bcmdrd_sid_t *clr_reg,
                                       bcmdrd_fid_t *clr_fid_array,
                                       bcmdrd_fid_t **clr_fid_ptr)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    /* get SIDs */
    if (low_info->intr_clr_reg != INVALIDr) {
        *clr_reg = low_info->intr_clr_reg;
    } else {
        /* can be INVALIDr */
        *clr_reg = INVALIDr;
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    /* get FIDs, the registers have the same FIDs*/
    if (low_info->intr_status_field != INVALIDf) {
        /* intr_clr_field is paired with intr_status_field */
        if (low_info->intr_clr_field == INVALIDf) {
            rv = SHR_E_CONFIG;
            SHR_RETURN_VAL_EXIT(rv);
        }
        clr_fid_array[0] = low_info->intr_clr_field;
        clr_fid_array[1] = INVALIDf;
        *clr_fid_ptr = clr_fid_array;
    } else if (low_info->intr_status_field_list != NULL) {
        /* intr_clr_field is paired with intr_status_field */
        if (low_info->intr_clr_field_list == NULL) {
            rv = SHR_E_CONFIG;
            SHR_RETURN_VAL_EXIT(rv);
        }
        *clr_fid_ptr = low_info->intr_clr_field_list;
    } else {
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get intr clr register and its field\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_non_mmu_process used to handle SER errors which need not to be corrected.
* Incluing ecc/parity/mac etc.
 */
static int
bcmptm_ser_non_mmu_process(int unit,
                           bcmptm_ser_top_level_route_info_t *top_rb_info,
                           bcmptm_ser_raw_info_t *crt_info)
{
    bcmptm_ser_buf_bus_ctrl_reg_t status_reg_array[2], *status_reg_ptr = NULL;
    bcmdrd_sid_t clr_reg;
    int rv = SHR_E_NONE;
    bcmdrd_fid_t intr_status_fid[2], *intr_status_fid_ptr = NULL;
    bcmdrd_fid_t intr_clr_fid[2], *intr_clr_fid_ptr = NULL;
    bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem = (bcmptm_ser_misc_blk_crt_info_t *)(&crt_info->serc_info);
    int reg_idx = 0, field_idx = 0;
    bcmdrd_sid_t status_reg, group_status_reg;
    bcmbd_pt_dyn_info_t dyn_info = {0, -1}, group_dyn_info = {0, -1};
    int inst, inst_end;
    uint32_t rval = 0, fval =0;
    int blk_type = 0;
    bcmptm_ser_low_level_info_t *low_route_info = (bcmptm_ser_low_level_info_t *)top_rb_info->info;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    SHR_FUNC_ENTER(unit);

    crt_info->valid = 1;

    if (low_route_info == NULL) {
        rv = SHR_E_PARAM;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    group_status_reg = top_rb_info->status_reg;
    if (group_status_reg == INVALIDr) {
        rv = SHR_E_FAIL;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, group_status_reg, &acc_type, &acctype_str);
    SHR_IF_ERR_EXIT(rv);
    if (top_rb_info->blocktype == BCMPTM_SER_BLK_IPIPE_IDB &&
        PT_IS_UNIQUE_ACC_TYPE(acctype_str)) {
        group_dyn_info.tbl_inst = top_rb_info->pipe;
    } else if (bcmdrd_pt_is_soft_port_reg(unit, group_status_reg)) {
        group_dyn_info.tbl_inst = top_rb_info->blk_inst;
    }
    rv = bcmptm_ser_group_intr_status_reg_check(unit, top_rb_info,
                                                group_dyn_info.tbl_inst, &low_route_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (low_route_info->type == BCMPTM_SER_PARITY_TYPE_NONE) {
        /* This condition should not be satisfied */
        crt_info->valid = 0;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "All SER events on %s instance[%d] had been processed\n"),
                  low_route_info->mem_str, group_dyn_info.tbl_inst));
        /* exit */
        rv = SHR_E_NONE;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    rv = bcmptm_ser_low_level_intr_status_regs_get(unit, low_route_info, status_reg_array,
                                                   intr_status_fid, &status_reg_ptr,
                                                   &intr_status_fid_ptr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_low_level_intr_clr_regs_get(unit, low_route_info, &clr_reg,
                                                intr_clr_fid, &intr_clr_fid_ptr);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, blk_type_map_fn)(unit, top_rb_info->blocktype, &blk_type);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (reg_idx = 0; status_reg_ptr[reg_idx].reg != INVALIDr; reg_idx++) {
        status_reg = status_reg_ptr[reg_idx].reg;

        if (status_reg_ptr[reg_idx].membase >= 0 &&
            status_reg_ptr[reg_idx].stage_id >= 0) {
            ser_misc_mem->magic_sid =
                BB_MAGIC_SID_CREATE(status_reg_ptr[reg_idx].membase,
                                    status_reg_ptr[reg_idx].stage_id);
        } else {
            ser_misc_mem->magic_sid = INVALIDr;
        }
        rv = bcmptm_ser_non_mmu_status_reg_inst_get(unit, top_rb_info, status_reg,
                                                    &inst, &inst_end);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        for ( ; inst <= inst_end; inst++) {
            /* read status register */
            rval = 0;
            dyn_info.tbl_inst = inst;
            rv = bcmptm_ser_pt_read(unit, status_reg, dyn_info.index, dyn_info.tbl_inst,
                                    NULL, &rval, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            if (rval == 0) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Value of instance [%d] of status register[%s] is zero\n"),
                           inst, bcmdrd_pt_sid_to_name(unit, status_reg)));
                continue;
            }
            fval = 0;
            for (field_idx = 0; intr_status_fid_ptr[field_idx] != INVALIDf; field_idx++) {
                rv = bcmdrd_pt_field_get(unit, status_reg, &rval,
                                         intr_status_fid_ptr[field_idx], &fval);
                if (SHR_FAILURE(rv)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          " Status register[%s] may not have field[%d]\n"),
                               bcmdrd_pt_sid_to_name(unit, status_reg),
                               intr_status_fid_ptr[field_idx]));
                    SHR_RETURN_VAL_EXIT(rv);
                }
                if (fval) {
                    break;
                }
            }
            /* All status fields needed to check are zero, go to check the next register */
            /* Value of register can be non-zero, but value of all status fields can be zero */
            if (intr_status_fid_ptr[field_idx] == INVALIDf) {
                continue;
            }
            ser_misc_mem->blk_type = blk_type;
            ser_misc_mem->tbl_inst = dyn_info.tbl_inst;
            /* when value is -1, it means the members are not used */
            ser_misc_mem->entry_idx = -1;
            ser_misc_mem->err_type = -1;
            ser_misc_mem->multi_bits = -1;
            ser_misc_mem->status_reg = INVALIDr;
            /* get information from low level status register, and clear low level status register */
            rv = bcmptm_ser_mac_status_reg_read_clear(unit, clr_reg, intr_clr_fid_ptr,
                                                      field_idx, dyn_info.index, dyn_info.tbl_inst,
                                                      low_route_info, ser_misc_mem);
            if (rv != SHR_E_NOT_FOUND) {
                /* exit */
                /* handle only one error at a time */
                SHR_RETURN_VAL_EXIT(rv);
            } else {
                /* ECC SER error */
                rv = bcmptm_ser_ecc_parity_status_reg_read_clear(unit, status_reg,
                                                                 dyn_info.index,
                                                                 dyn_info.tbl_inst, &rval,
                                                                 low_route_info, ser_misc_mem);
                /* handle only one error at a time */
                SHR_RETURN_VAL_EXIT(rv);
            }
        }
    }
    crt_info->valid = 0;
    /* low level status register had been cleared, so clear top level interrupt status register */
    rv = bcmptm_ser_group_intr_status_reg_clear(unit, top_rb_info,
                                                group_dyn_info.tbl_inst, low_route_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "All SER events on %s instance[%d] have been corrected\n"),
               low_route_info->mem_str, group_dyn_info.tbl_inst));
exit:
    if (SHR_FAILURE(rv)) {
        crt_info->valid = 0;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to process SER event for non mmu memory\n")));
    } else if (crt_info->valid == 1) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " %s instance[%d] has %s %s error at index %08d\n"),
                  low_route_info->mem_str,
                  dyn_info.tbl_inst,
                  (ser_misc_mem->multi_bits > 0) ? "multiple" : "single",
                  (ser_misc_mem->err_type > 0) ? "2bit": "1bit",
                  ser_misc_mem->entry_idx));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_ipipe_idb_process used to handle IPIPE_NEW SER errors.
* Incluing ecc/parity etc.
 */
static int
bcmptm_ser_ipipe_idb_process(int unit, uint32_t intr_param,
                             bcmptm_ser_top_level_route_info_t *top_rb_info,
                             bcmptm_ser_raw_info_t *crt_info)
{
    return bcmptm_ser_non_mmu_process(unit, top_rb_info, crt_info);
}

/******************************************************************************
* bcmptm_ser_port_process used to handle PORT SER errors.
* Incluing ecc/parity/mac etc.
 */
static int
bcmptm_ser_port_process(int unit, uint32_t intr_param,
                        bcmptm_ser_top_level_route_info_t *top_rb_info,
                        bcmptm_ser_raw_info_t *crt_info)
{
    return bcmptm_ser_non_mmu_process(unit, top_rb_info, crt_info);
}

/******************************************************************************
* bcmptm_ser_non_mmu_info_parse
 */
static int
bcmptm_ser_non_mmu_info_parse(int unit,
                              bcmptm_ser_raw_info_t *crt_info,
                              bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_misc_blk_crt_info_t *ser_misc_mem = (bcmptm_ser_misc_blk_crt_info_t *)(&crt_info->serc_info);
    bcmdrd_sid_t is_tdm_calendar = INVALIDm;

    SHR_FUNC_ENTER(unit);

    spci->tbl_inst = ser_misc_mem->tbl_inst;
    spci->blk_type = ser_misc_mem->blk_type;
    if (ser_misc_mem->entry_idx >= 0) {
        spci->index = ser_misc_mem->entry_idx;
    }
    if (ser_misc_mem->multi_bits > 0) {
        spci->flags |= BCMPTM_SER_FLAG_MULTIBIT;
    }
    if (ser_misc_mem->err_type > 0) {
        spci->flags |= BCMPTM_SER_FLAG_DOUBLEBIT;
    }
    if (ser_misc_mem->err_type >= 0) {
        spci->flags |= BCMPTM_SER_FLAG_ECC;
    } else {
        /* should not be parity error */
        spci->flags |= BCMPTM_SER_FLAG_PARITY;
    }
    spci->sid = INVALIDm;
    spci->reported_sid = INVALIDm;
    spci->second_sid = INVALIDm;
    
    is_tdm_calendar = INVALIDm;
    /* spci->index may be changed */
    (void)BCMPTM_SER_DATA_DRIVER_CB(unit, is_tdm_calendar_sid_get_fn)(unit, ser_misc_mem->status_reg,
                                             &spci->index, &is_tdm_calendar);
    if (is_tdm_calendar != INVALIDm) {
        spci->sid = is_tdm_calendar;
    } else {
        if (ser_misc_mem->magic_sid != INVALIDr) {
            spci->sid = ser_misc_mem->magic_sid;
            spci->flags |= BCMPTM_SER_FLAG_NON_SBUS;
        }
    }

    SHR_IF_ERR_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_ipipe_idb_parse
 */
static int
bcmptm_ser_ipipe_idb_parse(int unit,
                           bcmptm_ser_raw_info_t *crt_info,
                           bcmptm_ser_correct_info_t *spci)
{
    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_OTHER;
    return bcmptm_ser_non_mmu_info_parse(unit, crt_info, spci);
}

/******************************************************************************
* bcmptm_ser_port_info_parse
 */
static int
bcmptm_ser_port_info_parse(int unit,
                           bcmptm_ser_raw_info_t *crt_info,
                           bcmptm_ser_correct_info_t *spci)
{
    spci->flags |= BCMPTM_SER_FLAG_ERR_IN_PORT;
    return bcmptm_ser_non_mmu_info_parse(unit, crt_info, spci);
}

/******************************************************************************
* bcmptm_ser_ing_egr_fifo_parse
 */
static int
bcmptm_ser_ing_egr_fifo_parse(int unit,
                              bcmptm_ser_raw_info_t *crt_info,
                              bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_ing_egr_crt_info_t *ing_egr_info = (bcmptm_ser_ing_egr_crt_info_t *)(&crt_info->serc_info);
    uint32_t adext = 0, type;
    bcmptm_cmic_adext_info_t adext_info;
    bcmbd_pt_dyn_info_t dyn_info = {0, 0};
    int rv = SHR_E_NONE;
    bcmdrd_sid_t sid;
    bcmptm_ser_log_flag_t err_type[] = { BCMPTM_SER_FLAG_ERR_IN_SOP,
                                         BCMPTM_SER_FLAG_ERR_IN_MOP,
                                         BCMPTM_SER_FLAG_ERR_IN_EOP,
                                         BCMPTM_SER_FLAG_ERR_IN_SBUS,
                                         BCMPTM_SER_FLAG_ERR_IN_AGE
                                       };
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    SHR_FUNC_ENTER(unit);

    if (ing_egr_info->multi) {
        spci->flags |= BCMPTM_SER_FLAG_MULTIBIT;
    }
    if (ing_egr_info->drop) {
        spci->flags |= BCMPTM_SER_FLAG_DROP;
    }
    if (ing_egr_info->ecc_parity > 0) {
        spci->flags |= BCMPTM_SER_FLAG_ECC;
        if (ing_egr_info->ecc_parity > 1) {
            spci->flags |= BCMPTM_SER_FLAG_DOUBLEBIT;
        }
    } else {
        spci->flags |= BCMPTM_SER_FLAG_PARITY;
    }
    type = ing_egr_info->instruction_type;
    if (type < COUNTOF(err_type)) {
        spci->flags |= err_type[type];
    } else {
        spci->flags |= BCMPTM_SER_FLAG_ERR_IN_OTHER;
    }

    spci->addr = ing_egr_info->address;
    spci->blk_type = ing_egr_info->blk_type;

    if (ing_egr_info->non_sbus) {
        spci->flags |= BCMPTM_SER_FLAG_NON_SBUS;
        spci->sid = BB_MAGIC_SID_CREATE(ing_egr_info->reg_mem_base,
                                        ing_egr_info->pipe_stage);
    } else {
        adext_info.acc_type = -1;
        rv = bcmptm_ser_blknum_get(unit, INVALIDm, 0,
                                   ing_egr_info->blk_type, &(adext_info.blk_num));
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_adext_get_fn)(unit, adext_info, &adext);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        if (ing_egr_info->reg) {
            rv = bcmbd_pt_addr_decode(unit, adext, ing_egr_info->address,
                                      BCMDRD_SYMBOL_FLAG_REGISTER, &sid, &dyn_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            spci->sid = sid;
        } else {
            rv = bcmbd_pt_addr_decode(unit, adext, ing_egr_info->address,
                                      BCMDRD_SYMBOL_FLAG_MEMORY, &sid, &dyn_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            dyn_info.index =
                BCMPTM_SER_DATA_DRIVER_CB(unit, flex_ctr_index_remap_fn)(unit,
                                                ing_egr_info->address,
                                                ing_egr_info->blk_type,
                                                ing_egr_info->pipe_stage,
                                                ing_egr_info->index);
            /* map L3_TUNNELm to L3_TUNNEL_DATA_ONLYm*/
            rv = BCMPTM_SER_DATA_DRIVER_CB(unit, ip_ep_sram_remap_fn)(unit, sid, &spci->sid);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        spci->index = dyn_info.index;
        /* for port registers, table instance is equal to port number */
        spci->tbl_inst = dyn_info.tbl_inst;

        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, sid, &acc_type, &acctype_str);
        SHR_IF_ERR_EXIT(rv);
        /* cannot get table instance number from bcmptm_ser_flex_ctr_index_remap for unique */
        if (!PT_IS_PORT_OR_SW_PORT_REG(unit, sid) &&
            (PT_IS_UNIQUE_ACC_TYPE(acctype_str) ||
             PT_IS_DUPLICATE_ACC_TYPE(acctype_str))) {
            spci->tbl_inst = ing_egr_info->blk_inst;
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to parse information from ING or EGR FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_ing_egr_fifo_val_set used to set value of fields of ING_SER_FIFOm and
* EGR_SER_FIFOm and L2_MGMT_SER_FIFOm to variable
 */
static int
bcmptm_ser_ing_egr_fifo_val_set(int unit, bcmdrd_sid_t sid, uint32_t *entry_data,
                                bcmdrd_fid_t *fid_list,
                                bcmptm_ser_ing_egr_fifo_fld_val_t *flds_val)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[0], &(flds_val->valid));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[1], &(flds_val->ecc_parity));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[2], &(flds_val->mem_type));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[3], &(flds_val->address));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[4], &(flds_val->pipe_stage));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[5], &(flds_val->intruction));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[6], &(flds_val->drop));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[7], &(flds_val->multiple));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[8], &(flds_val->non_sbus));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[9], &(flds_val->membase));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[10], &(flds_val->memindex));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[11], &(flds_val->regbase));
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid_list[12], &(flds_val->regindex));
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get field value from ING or EGR FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_ing_egr_fifo_pop used to pop SER information from FIFO
 */
static int
bcmptm_ser_ing_egr_fifo_pop(int unit, uint32_t intr_param,
                            bcmptm_ser_top_level_route_info_t *top_rb_info,
                            bcmptm_ser_raw_info_t *crt_info)
{
    int rv = SHR_E_NONE;
    bcmdrd_sid_t ing_fifo, egr_fifo, egr_intr_statr, l2_mgmt_fifo, l2_mgmt_intr;
    bcmdrd_fid_t *fid_list = NULL;
    int fid_num;
    bcmptm_ser_ing_egr_fifo_fld_val_t fifo_fld_val;
    int is_reg, non_sbus, blk_type;
    bcmdrd_sid_t fifo_sid, l2_mod_fifo_sid;
    uint32_t entry_data[2], rval = 0;
    bcmbd_pt_dyn_info_t dyn_info;
    bcmptm_ser_ing_egr_crt_info_t *ing_egr_info = (bcmptm_ser_ing_egr_crt_info_t *)(&crt_info->serc_info);
    bcmdrd_fid_t l2_mgmt_fifo_not_empty;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, ing_egr_fifo_get_fn)(unit, &ing_fifo, &egr_fifo, &egr_intr_statr,
                                     &l2_mgmt_fifo, &l2_mgmt_intr, &fid_list,
                                     &fid_num, &l2_mgmt_fifo_not_empty, &l2_mod_fifo_sid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    fifo_sid = (top_rb_info->blocktype == BCMPTM_SER_BLK_IPIPE)? ing_fifo: egr_fifo;
    dyn_info.index = 0;
    dyn_info.tbl_inst = top_rb_info->pipe;

    crt_info->valid = 0;

    if (top_rb_info->blocktype == BCMPTM_SER_BLK_IPIPE) {
        rv = bcmbd_pt_pop(unit, fifo_sid, &dyn_info, NULL, entry_data, 2);
        /* IPIPE FIFO is empty */
        if (SHR_FAILURE(rv)) {
            rv = SHR_E_NONE;
            SHR_RETURN_VAL_EXIT(rv);
        }
    } else {
        rv = bcmptm_ser_pt_read(unit, egr_intr_statr, 0, top_rb_info->pipe,
                                NULL, &rval, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        /* EPIPE FIFO is empty */
        if (rval == 0) {
            rv = SHR_E_NONE;
            SHR_RETURN_VAL_EXIT(rv);
        }
        rv = bcmbd_pt_pop(unit, fifo_sid, &dyn_info, NULL, entry_data, 2);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    sal_memset(&fifo_fld_val, 0, sizeof(fifo_fld_val));
    rv = bcmptm_ser_ing_egr_fifo_val_set(unit, fifo_sid, entry_data,
                                         fid_list, &fifo_fld_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    is_reg = fifo_fld_val.mem_type;
    non_sbus = fifo_fld_val.non_sbus;
    /* get block type */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, blk_type_map_fn)(unit, top_rb_info->blocktype, &blk_type);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (!is_reg) {
        ing_egr_info->index = fifo_fld_val.memindex;
        ing_egr_info->reg = 0;
        ing_egr_info->reg_mem_base = fifo_fld_val.membase;
        /* non_sbus may be changed to 1 from 0 */
        BCMPTM_SER_DATA_DRIVER_CB(unit, bus_addr_translate_fn)
            (unit, fifo_fld_val.membase, fifo_fld_val.address, &non_sbus);
        ing_egr_info->non_sbus = non_sbus;
    } else {
        ing_egr_info->index = fifo_fld_val.regindex;
        ing_egr_info->reg = 1;
        ing_egr_info->reg_mem_base = fifo_fld_val.regbase;
        ing_egr_info->non_sbus = non_sbus;
    }

    ing_egr_info->address = fifo_fld_val.address;
    ing_egr_info->blk_inst = top_rb_info->pipe;
    ing_egr_info->blk_type = blk_type;
    ing_egr_info->drop = fifo_fld_val.drop;
    ing_egr_info->ecc_parity = fifo_fld_val.ecc_parity;
    ing_egr_info->instruction_type = fifo_fld_val.intruction;
    ing_egr_info->multi = fifo_fld_val.multiple;
    ing_egr_info->pipe_stage = fifo_fld_val.pipe_stage;

    crt_info->valid = 1;
exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to pop information from ING or EGR FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_l2_mgmt_fifo_pop used to pop SER information from L2_MGMT_SER_FIFOm
 */
static int
bcmptm_ser_l2_mgmt_fifo_pop(int unit, uint32_t intr_param,
                            bcmptm_ser_top_level_route_info_t *top_rb_info,
                            bcmptm_ser_raw_info_t *crt_info)
{
    int rv = SHR_E_NONE;
    bcmdrd_sid_t ing_fifo, egr_fifo, egr_intr_statr, l2_mgmt_fifo, l2_mgmt_intr;
    bcmdrd_sid_t l2_mod_fifo_sid;
    bcmdrd_fid_t *fid_list = NULL;
    int fid_num, blk_type;
    uint32_t entry_data[2], rval = 0, fval = 0;
    bcmbd_pt_dyn_info_t dyn_info;
    bcmptm_ser_ing_egr_fifo_fld_val_t fifo_fld_val;
    bcmptm_ser_ing_egr_crt_info_t *ing_egr_info = (bcmptm_ser_ing_egr_crt_info_t *)(&crt_info->serc_info);
    bcmdrd_fid_t l2_mgmt_fifo_not_empty;

    SHR_FUNC_ENTER(unit);

    crt_info->valid = 0;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, ing_egr_fifo_get_fn)(unit, &ing_fifo, &egr_fifo, &egr_intr_statr,
                                     &l2_mgmt_fifo, &l2_mgmt_intr, &fid_list,
                                     &fid_num, &l2_mgmt_fifo_not_empty, &l2_mod_fifo_sid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_read(unit, l2_mgmt_intr, 0, -1, NULL, &rval, 1,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, l2_mgmt_intr, &rval,
                             l2_mgmt_fifo_not_empty, &fval);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (fval == 0) {
        /* ser correctiong has completed */
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    dyn_info.index = 0;
    dyn_info.tbl_inst = 0;

    rv = bcmbd_pt_pop(unit, l2_mgmt_fifo, &dyn_info, NULL, entry_data, 2);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sal_memset(&fifo_fld_val, 0, sizeof(fifo_fld_val));
    rv = bcmptm_ser_ing_egr_fifo_val_set(unit, l2_mgmt_fifo, entry_data,
                                         fid_list, &fifo_fld_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, blk_type_map_fn)(unit, BCMPTM_SER_BLK_IPIPE, &blk_type);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ing_egr_info->address = fifo_fld_val.address;
    ing_egr_info->blk_inst = 0;
    ing_egr_info->blk_type = blk_type;
    ing_egr_info->index = fifo_fld_val.memindex;
    crt_info->valid = 1;

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to pop information from L2_MGMT_SER_FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_l2_mgmt_fifo_parse
 */
static int
bcmptm_ser_l2_mgmt_fifo_parse(int unit,
                              bcmptm_ser_raw_info_t *crt_info,
                              bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_ing_egr_crt_info_t *ing_egr_info = (bcmptm_ser_ing_egr_crt_info_t *)(&crt_info->serc_info);
    int rv = SHR_E_NONE;
    bcmdrd_sid_t ing_fifo, egr_fifo, egr_intr_statr, l2_mgmt_fifo, l2_mgmt_intr;
    bcmdrd_sid_t l2_mod_fifo_sid;
    bcmdrd_fid_t *fid_list = NULL, l2_mgmt_fifo_not_empty;
    int fid_num;
    bcmptm_ser_log_flag_t err_type[] = { BCMPTM_SER_FLAG_ERR_IN_SOP,
                                         BCMPTM_SER_FLAG_ERR_IN_MOP,
                                         BCMPTM_SER_FLAG_ERR_IN_EOP,
                                         BCMPTM_SER_FLAG_ERR_IN_SBUS,
                                         BCMPTM_SER_FLAG_ERR_IN_AGE
                                       };
    uint32_t type;

    SHR_FUNC_ENTER(unit);

    if (ing_egr_info->ecc_parity > 0) {
        spci->flags |= BCMPTM_SER_FLAG_ECC;
        if (ing_egr_info->ecc_parity > 1) {
            spci->flags |= BCMPTM_SER_FLAG_DOUBLEBIT;
        }
    } else {
        spci->flags |= BCMPTM_SER_FLAG_PARITY;
    }
    type = ing_egr_info->instruction_type;
    if (type < COUNTOF(err_type)) {
        spci->flags |= err_type[type];
    } else {
        spci->flags |= BCMPTM_SER_FLAG_ERR_IN_OTHER;
    }
    spci->flags |= BCMPTM_SER_FLAG_L2MOD;
    spci->addr = ing_egr_info->address;
    spci->tbl_inst = ing_egr_info->blk_inst;
    spci->blk_type = ing_egr_info->blk_type;
    spci->index = ing_egr_info->index;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, ing_egr_fifo_get_fn)(unit, &ing_fifo, &egr_fifo, &egr_intr_statr,
                                     &l2_mgmt_fifo, &l2_mgmt_intr, &fid_list,
                                     &fid_num, &l2_mgmt_fifo_not_empty, &l2_mod_fifo_sid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    spci->sid = l2_mod_fifo_sid;
    /* should be l2_mod_fifo */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "%s parity error\n"),
               bcmdrd_pt_sid_to_name(unit, l2_mod_fifo_sid)));
exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to parse information from L2_MGMT_SER_FIFO\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_intr_num_check used to check where a SER interrupt comes from
 */
static int
bcmptm_ser_intr_num_check(int unit, int intr_num,
                          bcmptm_ser_blk_type_t *ser_blk_type,
                          bcmptm_ser_top_level_route_info_t **hit_top_rb)
{
    bcmptm_ser_top_level_route_info_t *top_rb = NULL, *cur_top_rb;
    int i = 0, route_num = 0, rv = SHR_E_NONE;
    uint32_t cmic_bit = 0, cmic_id = 0;
    uint32_t intr_off_bit = intr_num % 32, intr_id = intr_num / 32;

    SHR_FUNC_ENTER(unit);
    /* interrupt for SER engine */
    if (bcmptm_ser_tcam_cmic_intr_num_get(unit) == intr_num) {
        *ser_blk_type = BCMPTM_SER_BLK_SER_ENGINE;
        *hit_top_rb = NULL;
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    /* interrupt for L2_MGMTm */
    if (bcmptm_ser_l2_mgmt_intr_num_get(unit) == intr_num) {
        *ser_blk_type = BCMPTM_SER_BLK_L2_MGMT;
        *hit_top_rb = NULL;
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    /* IP/EP/MMU/PORT etc. */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, route_info_get_fn)(unit, &top_rb, &route_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (i = 0; i < route_num; i++) {
        cur_top_rb = &top_rb[i];
        cmic_bit = cur_top_rb->cmic_bit;
        if (0 == cmic_bit) {
            /* end */
            rv = SHR_E_NOT_FOUND;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        cmic_id = cur_top_rb->cmic_reg;

        if ((intr_id == cmic_id)  &&  (uint32_t)(1 << intr_off_bit) == cmic_bit) {
            *ser_blk_type = cur_top_rb->blocktype;
            *hit_top_rb = cur_top_rb;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Interrupt comes from SER block [%d]\n"),
                         *ser_blk_type));
            break;
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Don't know where interrupt comes from\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_fifo_pop_and_parse pop and parse one entry from relevent memory tables
 */
static int
bcmptm_ser_fifo_pop_and_parse(int unit, uint32_t intr_param,
                              bcmptm_ser_blk_type_t ser_blk_type,
                              bcmptm_ser_top_level_route_info_t *hit_top_rb,
                              bcmptm_ser_correct_info_t *spci)
{
    bcmptm_ser_raw_info_t crt_info;
    int rv = SHR_E_NONE;
    bcmptm_ser_pop_parse_fn_map_t pop_parse_mapping[] = {
        { BCMPTM_SER_BLK_SER_ENGINE, bcmptm_ser_tcam_fifo_pop, bcmptm_ser_tcam_fifo_entry_parse},
        { BCMPTM_SER_BLK_MMU, bcmptm_ser_mmu_fifo_pop, bcmptm_ser_mmu_fifo_entry_parse},
        { BCMPTM_SER_BLK_EPIPE, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse},
        { BCMPTM_SER_BLK_IPIPE, bcmptm_ser_ing_egr_fifo_pop, bcmptm_ser_ing_egr_fifo_parse},
        { BCMPTM_SER_BLK_CLPORT, bcmptm_ser_port_process, bcmptm_ser_port_info_parse},
        { BCMPTM_SER_BLK_XLPORT, bcmptm_ser_port_process, bcmptm_ser_port_info_parse},
        { BCMPTM_SER_BLK_IPIPE_IDB, bcmptm_ser_ipipe_idb_process, bcmptm_ser_ipipe_idb_parse},
        { BCMPTM_SER_BLK_L2_MGMT, bcmptm_ser_l2_mgmt_fifo_pop, bcmptm_ser_l2_mgmt_fifo_parse}
    };
    int i = 0, num_mapping = COUNTOF(pop_parse_mapping);

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_mapping; i++) {
        if (pop_parse_mapping[i].ser_blk_type != ser_blk_type) {
            continue;
        }
        spci->detect_time = sal_time_usecs();
        if (pop_parse_mapping[i].ser_fifo_pop_fn == NULL) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        rv = pop_parse_mapping[i].ser_fifo_pop_fn(unit, intr_param, hit_top_rb, &crt_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* FIFO is empty, and interrupt status registers are cleared */
        if (rv == SHR_E_NONE && crt_info.valid == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_EMPTY);
        }

        if (pop_parse_mapping[i].ser_fifo_parse_fn == NULL) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        rv = pop_parse_mapping[i].ser_fifo_parse_fn(unit, &crt_info, spci);
        /* we reserve SHR_E_EMPTY to tell caller that the SER interrupt status is cleared */
        if (rv == SHR_E_EMPTY) {
            rv = SHR_E_FAIL;
        }
        /* exit */
        SHR_RETURN_VAL_EXIT(rv);
    }
    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "SER has on handler for SER block type[%d]\n"),
               ser_blk_type));
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    if (rv != SHR_E_EMPTY && SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error in SER block type[%d]\n"),
                   ser_blk_type));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_parity_field_clear
 */
static void
bcmptm_ser_parity_field_clear(int unit, bcmdrd_sid_t sid,
                              uint32_t *hw_entry, uint32_t *sw_entry)
{
    /* Heuristic to clear parity/ecc bits */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_A");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_B");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_LOWER");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_UPPER");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP5");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP6");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP7");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_4");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PBM_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EVEN_EGR_VLAN_STG_PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_P0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_P1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_P2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_P3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_PBM_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_PBM_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_PBM_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_PBM_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_PBM_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_PBM_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_PBM_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_PBM_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_PBM_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_PBM_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_PBM_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_PBM_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "TCAM_PARITY_KEY");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "TCAM_PARITY_MASK");

    /* Clear hit bits */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITDA_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITDA_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITDA_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITDA_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITSA_0");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITSA_1");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITSA_2");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITSA_3");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITDA");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HITSA");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "B0_HIT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "B1_HIT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "L3_HIT_DCM");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "L3_HIT_PM");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "HIT_BITS");
    /* Clear hardware-modified fields in meter tables  */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "BUCKETCOUNT");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "REFRESHCOUNT");

    /* Clear parity/ecc bits on svm_meter_table */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_EXCESS");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_EXCESS");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_EXCESS");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_COMMITTED");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_COMMITTED");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_COMMITTED");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_LRN");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_LRN");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_LRN");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECCP_CONFIG");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "PARITY_CONFIG");
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "ECC_CONFIG");
    /* Clear hardware-modified fields in svm_meter_table */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "LRN");         /* LAST_REFRESH_NUMBERf */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "COMMITTED");   /* COMMITTED_BUCKETCOUNTf */
    BCMPTM_ENTRY_PARITY_CLEAR(unit, sid, "EXCESS");      /* EXCESS_BUCKETCOUNTf */

}

/******************************************************************************
* bcmptm_ser_log_print used to print detailed SER information
 */
static void
bcmptm_ser_log_print(int unit, bcmdrd_sid_t pt_id, uint32_t index,
                     uint32_t inst, uint32_t address, uint32_t ser_type,
                     uint32_t ser_recovery, uint32_t ser_instruction,
                     uint32_t detect_utime, uint32_t correct_utime,
                     uint32_t correct, uint32_t drop)
{
    char *err_instruct_string[] = {
            "Error in: SOP cell EP/IP.",
            "Error in: MOP cell EP/IP.",
            "Error in: EOP cell EP/IP.",
            "Error in: SBUS transaction. EP/IP",
            "Error in: transaction - refresh, aging etc EP/IP.",
            "Error in: SER error in TCAM or unknown transaction EP/IP.",
            "Error in: MMU.",
            "Error in: PORT."
        };
    char *err_type_string[] = {
            "ser_err_parity",
            "ser_err_ecc_1bit",
            "ser_err_ecc_2bit"
        };
    char *err_resp_string[] = {
            "ser_cache_restore",
            "ser_entry_clear",
            "ser_no_operation"
        };
    uint32_t instruct_num = COUNTOF(err_instruct_string);
    uint32_t err_type_num = COUNTOF(err_type_string);
    uint32_t err_resp_num = COUNTOF(err_resp_string);

    if (bcmdrd_pt_is_valid(unit, pt_id)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "%s: index[%d] instance[%d] "
                             "address[0x%08x] has SER error\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id),
                  index, inst, address));
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Buffer or bus index[%d] "
                             "address[0x%08x] has SER error\n"),
                  index, address));
    }
    if (drop) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SER caused packet drop.\n")));
    }
    LOG_WARN(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "SER type [%s], SER recovery type[%s], %s\n"),
             (ser_type >= err_type_num) ? "unknown" : err_type_string[ser_type],
             (ser_recovery >= err_resp_num) ? "unknown" : err_resp_string[ser_recovery],
             (ser_instruction >= instruct_num) ? "unknown" : err_instruct_string[ser_instruction]));

    LOG_WARN(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "The SER is detected at time(0x%08x) %s corrected at time(0x%08x)."
                         " System took (0x%08x)us to process it\n"),
              detect_utime, correct ? "and is": "but is not",
              correct_utime, (correct_utime - detect_utime)));
}

/******************************************************************************
* bcmptm_ser_log
 */
static void
bcmptm_ser_log(int unit, bcmptm_ser_correct_info_t *spci, uint32_t corrected)
{
    bcmdrd_sid_t pt_id = spci->sid;
    uint32_t address = spci->addr, index = spci->index;
    uint32_t inst = (spci->tbl_inst < 0)? 0 : spci->tbl_inst;
    uint32_t ser_type = 0, ser_recovery = 0, ser_instruction = 0;
    uint32_t instruction_map[][2] = {
            { BCMPTM_SER_FLAG_ERR_IN_SOP, SER_INSTRUCTION_SOP },
            { BCMPTM_SER_FLAG_ERR_IN_MOP, SER_INSTRUCTION_MOP },
            { BCMPTM_SER_FLAG_ERR_IN_EOP, SER_INSTRUCTION_EOP },
            { BCMPTM_SER_FLAG_ERR_IN_SBUS, SER_INSTRUCTION_SBUS },
            { BCMPTM_SER_FLAG_ERR_IN_AGE, SER_INSTRUCTION_AGE },
            { BCMPTM_SER_FLAG_ERR_IN_OTHER, SER_INSTRUCTION_OTHER },
            { BCMPTM_SER_FLAG_ERR_IN_MMU, SER_INSTRUCTION_MMU },
            { BCMPTM_SER_FLAG_ERR_IN_PORT, SER_INSTRUCTION_PORT },
        };
    int map_num = 0, i = 0, rv = SHR_E_NONE;
    bcmdrd_pt_ser_info_t info;
    uint32_t detect_utime = 0, correct_utime = 0;
    uint32_t drop = (spci->flags & BCMPTM_SER_FLAG_DROP) ? 1 : 0;

    correct_utime = (uint32_t)sal_time_usecs();
    detect_utime = (uint32_t)spci->detect_time;

    BCMPTM_SER_STATS_TIMESTAMP_SET(unit, &correct_utime);

    if (spci->flags & BCMPTM_SER_FLAG_PARITY) {
        ser_type = SER_ERR_PARITY;
    } else {
        if (spci->flags & BCMPTM_SER_FLAG_DOUBLEBIT) {
            ser_type = SER_ERR_ECC_2BIT;
        } else {
            ser_type = SER_ERR_ECC_1BIT;
        }
    }

    ser_instruction = SER_INSTRUCTION_OTHER;
    map_num = COUNTOF(instruction_map);
    for (i = 0; i < map_num; i++) {
        if (spci->flags & instruction_map[i][0]) {
            ser_instruction = instruction_map[i][1];
        }
    }
    rv = bcmdrd_pt_ser_info_get(unit, pt_id, &info);
    if (SHR_SUCCESS(rv)) {
        if (BCMDRD_SER_RESP_NONE == info.resp) {
            ser_recovery = SER_NO_OPER;
            corrected = 0;
        } else if (BCMDRD_SER_RESP_ENTRY_CLEAR == info.resp) {
            ser_recovery = SER_ENTRY_CLEAR;
        } else {
            ser_recovery = SER_CACHE_RESTORE;
        }
    } else {
        if (!bcmdrd_pt_is_valid(unit, pt_id)) {
            /* bus or buffer */
            pt_id = INVALID_PT_ID;
            ser_recovery = SER_NO_OPER;
            corrected = 0;
        } else {
            ser_recovery = SER_CACHE_RESTORE;
        }
    }
    BCMPTM_SER_STATS_SER_RECOVERY_TYPE_SET(unit, &ser_recovery);
    BCMPTM_SER_STATS_PT_ID_SET(unit, &pt_id);
    BCMPTM_SER_STATS_PT_INDEX_SET(unit, &index);
    BCMPTM_SER_STATS_PT_INSTANCE_SET(unit, &inst);
    BCMPTM_SER_STATS_SER_ERR_TYPE_SET(unit, &ser_type);
    BCMPTM_SER_STATS_SER_INSTRUCT_TYPE_SET(unit, &ser_instruction);
    BCMPTM_SER_STATS_SER_ERR_CORRECTED_SET(unit, &corrected);

    if (BCMPTM_SER_CONTROL_SER_LOGGING(unit)) {
        bcmptm_ser_log_print(unit, pt_id, spci->index,
                             inst, address, ser_type,
                             ser_recovery, ser_instruction,
                             detect_utime, correct_utime, corrected, drop);
    }
}

/******************************************************************************
* bcmptm_ser_statistics
 */
static void
bcmptm_ser_statistics(int unit, bcmptm_ser_correct_info_t *spci)
{
    uint32_t flags = spci->flags;
    bcmdrd_sid_t pt_id = spci->sid;
    bool is_reg = 0, is_cci = 0;

    BCMPTM_SER_STATS_TOTAL_ERR_CNT_INC(unit);

    if (flags & BCMPTM_SER_FLAG_NON_SBUS) {
        BCMPTM_SER_STATS_ECC_PARITY_ERR_INT_BUS_CNT_INC(unit);
        return;
    }
    if (!bcmdrd_pt_is_valid(unit, pt_id)) {
        BCMPTM_SER_STATS_ECC_PARITY_ERR_INT_MEM_CNT_INC(unit);
        return;
    }
    /* physical tables */
    is_reg = bcmdrd_pt_is_reg(unit, pt_id);
    is_cci = bcmdrd_pt_is_counter(unit, pt_id);

    if (flags & BCMPTM_SER_FLAG_TCAM) {
        BCMPTM_SER_STATS_PARITY_ERR_TCAM_CNT_INC(unit);
    } else if (flags & BCMPTM_SER_FLAG_PARITY) {
        if (is_cci) {
            BCMPTM_SER_STATS_PARITY_ERR_CTR_CNT_INC(unit);
        } else if (is_reg) {
            BCMPTM_SER_STATS_PARITY_ERR_REG_CNT_INC(unit);
        } else {
            BCMPTM_SER_STATS_PARITY_ERR_MEM_CNT_INC(unit);
        }
    } else if (flags & BCMPTM_SER_FLAG_DOUBLEBIT) {
        if (is_cci) {
            BCMPTM_SER_STATS_ECC_DBE_CTR_CNT_INC(unit);
        } else if (is_reg) {
            BCMPTM_SER_STATS_ECC_DBE_REG_CNT_INC(unit);
        } else {
            BCMPTM_SER_STATS_ECC_DBE_MEM_CNT_INC(unit);
        }
    } else {
        if (is_cci) {
            BCMPTM_SER_STATS_ECC_SBE_CTR_CNT_INC(unit);
        }
        if (is_reg) {
            BCMPTM_SER_STATS_ECC_SBE_REG_CNT_INC(unit);
        } else {
            BCMPTM_SER_STATS_ECC_SBE_MEM_CNT_INC(unit);
        }
    }
}

/******************************************************************************
* bcmptm_ser_error_entry_record
 */
static int
bcmptm_ser_error_entry_record(int unit, bcmdrd_sid_t pt_id, int tbl_inst, int index)
{
    int rv = SHR_E_NONE, rv_en = SHR_E_EMPTY;
    uint32_t error_entry[BCMDRD_MAX_PT_WSIZE];
    int is_tcam = 0;

    SHR_FUNC_ENTER(unit);

    if (BCMPTM_SER_CONTROL_ERRONEOUS_ENTRIES_LOGGING(unit) == 0 ||
        pt_id == INVALIDm || pt_id == INVALIDr) {
        sal_memset(error_entry, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
        BCMPTM_SER_STATS_ERR_ENTRY_CONTENT_SET(unit, error_entry, BCMDRD_MAX_PT_WSIZE);
        return rv;
    }
    /* disable PT SER checking */
    is_tcam = bcmdrd_pt_attr_is_cam(unit,  pt_id);
    if (!is_tcam) {
        rv_en = bcmptm_ser_pt_ser_enable(unit, pt_id, BCMDRD_SER_EN_TYPE_EC, 0);
    } else {
        rv_en = bcmptm_ser_tcam_pt_ser_enable(unit, pt_id, 0);
    }
    rv = rv_en;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sal_memset(error_entry, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
    /* read from H/W */
    rv = bcmptm_ser_pt_read(unit, pt_id, index, tbl_inst, NULL,
                            error_entry, BCMDRD_MAX_PT_WSIZE, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    BCMPTM_SER_STATS_ERR_ENTRY_CONTENT_SET(unit, error_entry, BCMDRD_MAX_PT_WSIZE);

exit:
    /* re-enable parity or ecc checking */
    if (rv_en == SHR_E_NONE &&
        BCMPTM_SER_CONTROL_PT_ECC_PARITY_CHECK(unit, pt_id)) {
        if (!is_tcam) {
            (void)bcmptm_ser_pt_ser_enable(unit, pt_id, BCMDRD_SER_EN_TYPE_EC, 1);
        } else {
            (void)bcmptm_ser_tcam_pt_ser_enable(unit, pt_id, 1);
        }
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to record error entry of [%s] to cache!\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
    }
    SHR_FUNC_EXIT();
}
/******************************************************************************
* bcmptm_ser_control_pt_update if we disable ecc checking for MMU_WRED_UC_QUEUE_TOTAL_COUNT
* MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE is disabled, too.
 */
static void
bcmptm_ser_control_pt_update(int unit, bcmdrd_sid_t pt_id, int enable)
{
    bcmdrd_pt_ser_en_ctrl_t ctrl_info, ctrl_info_cur;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t sid;
    size_t sid_count = 0;

    BCMPTM_SER_IMM_CONTROL_PT_ECC_PARITY_CHECK_SET(unit, pt_id, 0);
    BCMPTM_SER_IMM_CONTROL_PT_SW_SCAN_SET(unit, pt_id, 0);
    BCMPTM_SER_IMM_CONTROL_PT_HW_SCAN_SET(unit, pt_id, 0);
    rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, pt_id, &ctrl_info);
    if (SHR_FAILURE(rv)) {
        return;
    }
    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_count);
    if (SHR_FAILURE(rv)) {
        return;
    }
    for (sid = 0; sid < sid_count; sid++) {
        if (sid == pt_id) {
            continue;
        }
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, sid, &ctrl_info_cur);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        if (ctrl_info_cur.sid == ctrl_info.sid &&
            ctrl_info_cur.fid == ctrl_info.fid &&
            ctrl_info_cur.fmask == ctrl_info.fmask) {
            BCMPTM_SER_IMM_CONTROL_PT_ECC_PARITY_CHECK_SET(unit, sid, 0);
            BCMPTM_SER_IMM_CONTROL_PT_SW_SCAN_SET(unit, sid, 0);
            BCMPTM_SER_IMM_CONTROL_PT_HW_SCAN_SET(unit, sid, 0);
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Parity or ECC checking for [%s] is disabled!!\n"),
                      bcmdrd_pt_sid_to_name(unit, sid)));
        }
    }
    return;
}

/******************************************************************************
* bcmptm_ser_hf_severity_record
 */
static void
bcmptm_ser_hf_severity_record(int unit, bcmdrd_sid_t pt_id, int hf_flag, uint32_t flags)
{
    uint32_t hf = 1, severity = 1;
    int rv = SHR_E_NONE;

    if (hf_flag == 1) {
        BCMPTM_SER_STATS_HW_FAULT_SET(unit, &hf);
        BCMPTM_SER_STATS_HW_FAULT_CNT_INC(unit);
    } else {
        BCMPTM_SER_STATS_HIGH_SEVERITY_ERR_SET(unit, &severity);
        BCMPTM_SER_STATS_HIGH_SEVERITY_ERR_CNT_INC(unit);
    }
    if ((flags & BCMPTM_SER_FLAG_TCAM) == 0) {
        /* If 1bit error, just need to disable 1bit-reporting */
        if (flags & BCMPTM_SER_FLAG_DOUBLEBIT) {
            rv = bcmptm_ser_pt_ser_enable(unit, pt_id, BCMDRD_SER_EN_TYPE_EC, 0);
            /* record to IMM LT SER_CONTROL_PT */
            bcmptm_ser_control_pt_update(unit, pt_id, 0);
        } else {
            rv = bcmptm_ser_pt_ser_enable(unit, pt_id, BCMDRD_SER_EN_TYPE_ECC1B, 0);
            /* only supports 1bit parity checking */
            if (SHR_FAILURE(rv)) {
                rv = bcmptm_ser_pt_ser_enable(unit, pt_id, BCMDRD_SER_EN_TYPE_EC, 0);
                bcmptm_ser_control_pt_update(unit, pt_id, 0);
            }
        }
    } else {
        /* Reported by CMIC SER engine */
        rv = bcmptm_ser_tcam_pt_ser_enable(unit, pt_id, 0);
        /* record to IMM LT SER_CONTROL_PT */
        bcmptm_ser_control_pt_update(unit, pt_id, 0);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to disable parity or ECC checking for [%s]!!\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Disable parity or ECC checking for [%s]!!\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
    }

    /* notify users there is HW fault */
    if (hf_flag) {
        BCMPTM_SER_IMM_CONTROL_PT_HW_FAULT_SET(unit, pt_id, 1);
        (void)BCMPTM_SER_CTH_CB(ser_hw_fault_notify)(unit, pt_id, hf, 0, 0);
    } else {
        (void)BCMPTM_SER_CTH_CB(ser_hw_fault_notify)(unit, pt_id, 0, severity, 0);
    }
}

/******************************************************************************
* bcmptm_ser_same_pt_check: check two sids are the same PT but different instance.
* such as:MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE0_EPIPE0m and
* MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE0_EPIPE1m;
* MMU_ENQS_PBI_DB_MMU_SC0_IPIPE0m and MMU_ENQS_PBI_DB_MMU_SC0_IPIPE1m
* MMU_PQE0_MEM_MMU_XPE0_XPE0m and MMU_PQE0_MEM_MMU_XPE1_XPE1m
 */
static int
bcmptm_ser_same_pt_check(int unit, bcmdrd_sid_t sid0, bcmdrd_sid_t sid1)
{
    bcmdrd_pt_ser_en_ctrl_t en_ctrl_0, en_ctrl_1;
    int rv = SHR_E_NONE;

    rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, sid0, &en_ctrl_0);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }
    rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, sid1, &en_ctrl_1);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }
    if (en_ctrl_0.sid == en_ctrl_1.sid &&
        en_ctrl_0.fid == en_ctrl_1.fid &&
        en_ctrl_0.fmask == en_ctrl_1.fmask) {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************
* bcmptm_ser_severity_fault_check_init: only can be used by SERC thread
 */
static void
bcmptm_ser_severity_fault_check_init(int unit)
{
    sal_usecs_t past_time = ser_sf_checking_latest_time[unit];
    sal_usecs_t latest_time = sal_time_usecs();
    sal_usecs_t distance = 0;

    if (past_time < latest_time) {
        distance = latest_time - past_time;
    } else {
        distance = (sal_usecs_t)(-1) - past_time + latest_time;
    }
    if (distance <= SERC_CACHE_CLEAR_TIME_INTERVAL && past_time) {
        return;
    }
    /* If latest_time is zero, assign it as 1, initialization value of ser_sf_checking_latest_time is 0. */
    latest_time = latest_time ? latest_time : 1;

    ser_sf_checking_latest_time[unit] = latest_time;

    ser_corrected_index[unit] = 0;
    ser_corrected_count[unit] = 0;
}

/******************************************************************************
* bcmptm_ser_severity_fault_check: only can be used by SERC thread, to check whether
* SER event of one PT or bus or buffer is reported again and again.
 */
static int
bcmptm_ser_severity_fault_check(int unit, bcmdrd_sid_t  sid, uint32_t flags)
{
    int time = 0, i;
    bcmdrd_sid_t sid_cached;

    if (sid == INVALIDm) {
        return FALSE;
    }
    /* unittest needs to disable this checking */
    if (ser_sf_checking_disable[unit]) {
        return FALSE;
    }
    bcmptm_ser_severity_fault_check_init(unit);

    ASSIGN_NEW_SID_TO_RING_CACHE(unit, sid);

    for (i = 0; i < ser_corrected_count[unit]; i++) {
        sid_cached = ser_corrected_sids[unit][i];
        if (sid == sid_cached) {
            time++;
        } else if (bcmptm_ser_same_pt_check(unit, sid, sid_cached)) {
            time++;
        }
    }
    if (time < SERC_CACHE_SEVERITY_THRES) {
        return FALSE;
    }
    /* SER error of buffer or bus or PTs will be reported again and again */
    if (bcmdrd_pt_is_valid(unit, sid)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Severity fault detected (read) at: [%s]!!\n"),
                   bcmdrd_pt_sid_to_name(unit, sid)));
        if (BCMPTM_SER_CONTROL_PT_ECC_PARITY_CHECK(unit, sid) != 0) {
            bcmptm_ser_hf_severity_record(unit, sid, 0, flags);
        }
    }
    return TRUE;
}

/******************************************************************************
* bcmptm_ser_hard_fault_check
 */
static int
bcmptm_ser_hard_fault_check(int unit, bcmdrd_sid_t  sid, int index, int tbl_inst,
                            uint32_t *sw_entry, size_t wsize, uint32_t flags)
{
    int rv = SHR_E_NONE, hf = FALSE, i, entry_size;
    uint32_t hw_entry[BCMDRD_MAX_PT_WSIZE];

    /* don't need to do HW checking */
    if (0 == BCMPTM_SER_CONTROL_HW_FAULT(unit)) {
        return FALSE;
    }
    sal_memset(hw_entry, 0, sizeof(hw_entry));
    /* read from H/W */
    rv = bcmptm_ser_pt_read(unit, sid, index, tbl_inst, NULL,
                            hw_entry, wsize, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);

    if (SHR_FAILURE(rv)) {
        /* Obvious hard fault */
        hf = TRUE;
    } else if (sw_entry != NULL) {
        bcmptm_ser_parity_field_clear(unit, sid, hw_entry, sw_entry);

        if (sal_memcmp(sw_entry, hw_entry, sizeof(hw_entry)) != 0) {
            hf = TRUE;
        }
    }
    if (hf) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Hard fault detected (read) at: %s instance[%d] index[%d]!!\n"),
                   bcmdrd_pt_sid_to_name(unit, sid), tbl_inst, index));

        entry_size = bcmdrd_pt_entry_wsize(unit, sid);
        for (i = 0; i < entry_size; i++) {
            if (sw_entry != NULL) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "SW data[%d]: 0x%08"PRIx32","
                                      " HW data[%d]: 0x%08"PRIx32"\n"),
                           i, sw_entry[i], i, hw_entry[i]));
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "HW data[%d]: 0x%08"PRIx32"\n"),
                           i, hw_entry[i]));
            }
        }
        bcmptm_ser_hf_severity_record(unit, sid, 1, flags);
    }
    return hf;
}

/******************************************************************************
* bcmptm_ser_correct
 */
static int
bcmptm_ser_correct(int unit, bcmdrd_sid_t sid, bcmbd_pt_dyn_info_t *pt_dyn_info,
                   uint32_t *correct_data, size_t wsize, int ser_resp)
{
    int rv = SHR_E_NONE;
    int index, tbl_inst;

    SHR_FUNC_ENTER(unit);

    index = pt_dyn_info->index;
    tbl_inst = pt_dyn_info->tbl_inst;

    if (ser_resp == BCMDRD_SER_RESP_NONE) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    } else if (ser_resp == BCMDRD_SER_RESP_ENTRY_CLEAR) {
        sal_memset(correct_data, 0, sizeof(uint32_t) * wsize);
    } else {
        sal_memset(correct_data, 0, sizeof(uint32_t) * wsize);
        rv = bcmptm_wal_serc_data_find(unit, 0, sid, pt_dyn_info, NULL,
                                       wsize,
                                       -1, BCMPTM_TCAM_SLICE_INVALID,
                                       correct_data);
        if (SHR_FAILURE(rv)) {
            /* read from PTcache */
            rv = bcmptm_ser_pt_read(unit, sid, index, tbl_inst, NULL,
                                    correct_data, wsize, 0);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
    rv = bcmptm_ser_pt_write(unit, sid, index, tbl_inst, NULL, correct_data, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error of %s index [%d] instance [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), index, tbl_inst));
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Correct SER error of %s index [%d] instance [%d] successfully\n"),
                 bcmdrd_pt_sid_to_name(unit, sid), index, tbl_inst));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_cci_ser_temp_req
 */
static int
bcmptm_ser_cci_ser_temp_req(int unit, bcmptm_ser_correct_info_t *spci)
{
    bcmdrd_sid_t sid;
    int rv = SHR_E_NONE, index, tbl_inst;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t sw_entry_data[BCMDRD_MAX_PT_WSIZE];
    bcmdrd_ser_resp_t ser_resp = BCMDRD_SER_RESP_CACHE_RESTORE;
    bcmdrd_pt_ser_info_t info;

    SHR_FUNC_ENTER(unit);

    sid = spci->sid;
    index = spci->index;
    tbl_inst = spci->tbl_inst;

    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = tbl_inst;

    rv = bcmdrd_pt_ser_info_get(unit, sid, &info);
    if (SHR_SUCCESS(rv)) {
        ser_resp = info.resp;
    }

    rv = bcmptm_ser_correct(unit, sid, &pt_dyn_info,
                            sw_entry_data, BCMDRD_MAX_PT_WSIZE, ser_resp);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to correct CCI SER error\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mem_direct_correct
 */
static int
bcmptm_ser_mem_direct_correct(int unit, bcmdrd_sid_t sid, bcmbd_pt_dyn_info_t *pt_dyn_info,
                              int ser_resp, uint32_t flags)
{
    int rv = SHR_E_NONE;
    int hf = FALSE;
    uint32_t sw_entry_data[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_correct(unit, sid, pt_dyn_info,
                            sw_entry_data, BCMDRD_MAX_PT_WSIZE, ser_resp);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error in %s\n"),
                   bcmdrd_pt_sid_to_name(unit, sid)));
        SHR_IF_ERR_EXIT(rv);
    }
    hf = bcmptm_ser_hard_fault_check(unit, sid, pt_dyn_info->index,
                                     pt_dyn_info->tbl_inst, sw_entry_data,
                                     BCMDRD_MAX_PT_WSIZE, flags);
    if (hf == TRUE) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_lp_hw_correct LP memory tables except FPEM_LPm use H/W recovery type
 */
static int
bcmptm_ser_lp_hw_correct(int unit, bcmdrd_sid_t reported_sid,
                                int reported_inst,  int reported_index,
                                bcmptm_ser_sram_info_t *sram_info, uint32_t flags)
{
    bcmdrd_sid_t pt_view = INVALIDm;
    uint32_t corrected_index = 0;
    uint32_t ram = 0, index_count = 0, i = 0;
    int rv = SHR_E_NONE, hf = FALSE;
    uint32_t recovery_data[BCMDRD_MAX_PT_WSIZE];
    int corrected_inst = (reported_inst == 0) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    for (ram = 0; ram < sram_info->ram_count; ram++) {
        pt_view = sram_info->pt_view[ram];
        if (pt_view == INVALIDm) {
            break;
        }
        index_count = sram_info->index_count[ram];
        if (index_count == 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Number of index should not be zero\n")));
            break;
        }
        for (i = 0; i < index_count; i++) {
            corrected_index = sram_info->indexes[ram][i];
            sal_memset(recovery_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
            /* read correction data from pipe which has no SER error. */
            rv = bcmptm_ser_pt_read(unit, pt_view, corrected_index, corrected_inst, NULL,
                                    recovery_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_EXIT(rv);
            /* write correction data to pipe which has SER error. */
            rv = bcmptm_ser_pt_write(unit, pt_view, corrected_index, reported_inst,
                                     NULL, recovery_data, 0);
            SHR_IF_ERR_EXIT(rv);
        }
    }
    hf = bcmptm_ser_hard_fault_check(unit, reported_sid, reported_index,
                                     reported_inst, NULL, 0, flags);
    if (hf == TRUE) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error of [%s] by H/W recovery type\n"),
                   bcmdrd_pt_sid_to_name(unit, reported_sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_mem_special_correct
 */
static int
bcmptm_ser_mem_special_correct(int unit, bcmdrd_sid_t reported_sid,
                               int reported_inst, int reported_index,
                               bcmptm_ser_sram_info_t *sram_info,
                               int ser_resp, uint32_t flags)
{
    uint32_t ram = 0, index_count = 0, i;
    bcmdrd_sid_t pt_view;
    int rv = SHR_E_NONE, hf = FALSE;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t extral_fld_old_val[BCMPTM_SER_EXTRA_REG_NUM] = { 0, 0 };
    uint32_t extral_fld_new_val = 0;
    uint32_t wr_check_fld_old_val = 0;
    bcmptm_ser_control_reg_info_t ctrl_reg_info;
    bcmdrd_sid_t ctrl_reg = INVALIDr;
    bcmdrd_fid_t fld_list[2];
    uint32_t recovery_data[BCMDRD_MAX_PT_WSIZE];
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, sram_ctrl_reg_get_fn)(unit, reported_sid, reported_index, &ctrl_reg_info);
    SHR_IF_ERR_EXIT(rv);

    /* Disable parity checking when write HASH_XOR memory PTs */
    ctrl_reg = ctrl_reg_info.wr_check_reg;
    if (ctrl_reg != INVALIDr) {
        fld_list[0] = ctrl_reg_info.wr_check_field;
        fld_list[1] = INVALIDf; /* must */
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, fld_list,
                                                1, NULL, &wr_check_fld_old_val);
        SHR_IF_ERR_EXIT(rv);
    }
    /* Freeze l2x H/W operations, etc. */
    for (i = 0; i < BCMPTM_SER_EXTRA_REG_NUM; i++) {
        ctrl_reg = ctrl_reg_info.extra_reg[i];
        if (ctrl_reg == INVALIDr) {
            break;
        }
        fld_list[0] = ctrl_reg_info.extra_field[i];
        fld_list[1] = INVALIDf; /* must */
        extral_fld_new_val = ctrl_reg_info.extra_field[i];
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, fld_list,
                                                extral_fld_new_val,
                                                NULL, &extral_fld_old_val[i]);
        SHR_IF_ERR_EXIT(rv);
    }
    for (ram = 0; ram < sram_info->ram_count; ram++) {
        pt_view = sram_info->pt_view[ram];
        if (pt_view == INVALIDm) {
            break;
        }
        pt_dyn_info.tbl_inst = reported_inst;
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, pt_view, &acc_type, &acctype_str);
        SHR_IF_ERR_EXIT(rv);

        if (PT_IS_DATA_SPLIT_ACC_TYPE(acctype_str)) {
            pt_dyn_info.tbl_inst = 0;
        }
        index_count = sram_info->index_count[ram];
        if (index_count == 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Number of index should not be zero\n")));
            break;
        }
        for (i = 0; i < index_count; i++) {
            pt_dyn_info.index = sram_info->indexes[ram][i];
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Correct XOR memory table index [%d] in Ram [%d]\n"),
                       pt_dyn_info.index, ram));
            rv = bcmptm_ser_correct(unit, pt_view, &pt_dyn_info,
                                    recovery_data, BCMDRD_MAX_PT_WSIZE, ser_resp);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to correct SER error in %s\n"),
                           bcmdrd_pt_sid_to_name(unit, pt_view)));
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }
    /* Restore parity checking when write HASH_XOR memory PTs */
    ctrl_reg = ctrl_reg_info.wr_check_reg;
    if (ctrl_reg != INVALIDr) {
        ctrl_reg = ctrl_reg_info.wr_check_reg;
        fld_list[0] = ctrl_reg_info.wr_check_field;
        fld_list[1] = INVALIDf; /* must */

        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, fld_list,
                                                wr_check_fld_old_val, NULL, NULL);
        SHR_IF_ERR_EXIT(rv);
    }
    /* Thaw l2x H/W operations, etc. */
    for (i = 0; i < BCMPTM_SER_EXTRA_REG_NUM; i++) {
        ctrl_reg = ctrl_reg_info.extra_reg[i];
        if (ctrl_reg == INVALIDr) {
            break;
        }
        fld_list[0] = ctrl_reg_info.extra_field[i];
        fld_list[1] = INVALIDf; /* must */
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_reg, fld_list,
                                                extral_fld_old_val[i],
                                                NULL, NULL);
        SHR_IF_ERR_EXIT(rv);
    }
    hf = bcmptm_ser_hard_fault_check(unit, reported_sid, reported_index,
                                     reported_inst, NULL, 0, flags);
    if (hf == TRUE) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to correct SER error of [%s] by special type\n"),
                   bcmdrd_pt_sid_to_name(unit, reported_sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* bcmptm_ser_buffer_bus_ser_req
 */
static void
bcmptm_ser_buffer_bus_ser_req(int unit, bcmptm_ser_correct_info_t *spci)
{
    int sf = FALSE, rv = SHR_E_NONE;
    char *base_info = NULL, *bd_bb_name = NULL;
    int  addrbase = -1, stage_id = -1;
    bcmdrd_ser_rmr_id_t rid = 0;

    addrbase = BB_MAGIC_SID_MEMBASE_GET(spci->sid);
    stage_id = BB_MAGIC_SID_STAGE_ID_GET(spci->sid);
    rv = BCMPTM_SER_DATA_DRIVER_CB
        (unit, hwmem_base_info_get_fn)(unit, addrbase, stage_id,
                                       &bd_bb_name, &base_info);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER logical does not know Mem hwbase[0x%x]\n"),
                  addrbase));
        return;
    }
    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\"%s\", Mem hwbase[0x%x] stage id[0x%x] has SER error...\n"),
               base_info, addrbase, stage_id));

    sf = bcmptm_ser_severity_fault_check(unit, spci->sid, spci->flags);
    if (sf == FALSE) {
        return;
    }
    if (sal_strcmp(bd_bb_name, "NONE") == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SER logical does not know how to"
                             "disable SER checking for this buffer or bus\n")));
        return;
    }
    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\"%s\", Mem hwbase[0x%x] has severity SER error, so"
                          " disable its SER checking\n"),
               base_info, addrbase));
    rv = bcmdrd_pt_ser_name_to_rmr(unit, bd_bb_name, &rid);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Can't find ID of [%s] from BD data base\n"),
                   bd_bb_name));
        return;
    }
    rv = bcmptm_ser_buf_bus_enable(unit, rid, BCMDRD_SER_EN_TYPE_EC, 0);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to disable SER checking of [%s]\n"),
                   bd_bb_name));
    }
    return;
}

/******************************************************************************
* bcmptm_ser_non_cci_ser_req
 */
static int
bcmptm_ser_non_cci_ser_req(int unit, bcmptm_ser_correct_info_t *spci)
{
    bcmdrd_sid_t sid[2];
    int rv = SHR_E_NONE, severity_error = FALSE;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    int index[2], tbl_inst[2], i, valid = 0;
    bcmdrd_ser_resp_t ser_resp = BCMDRD_SER_RESP_CACHE_RESTORE;
    bcmdrd_pt_ser_info_t info;
    bcmptm_ser_sram_info_t *sram_info = NULL;
    int max_index = 0;

    SHR_FUNC_ENTER(unit);

    sid[0] = spci->sid;
    sid[1] = spci->second_sid;
    index[0] = spci->index;
    index[1] = spci->second_index;
    tbl_inst[0] = spci->tbl_inst;
    tbl_inst[1] = spci->second_tbl_inst;

    rv = bcmptm_scor_locks_take(unit, sid[0]);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to take locks\n")));
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    for (i = 0; i < 2; i++) {
        if (sid[i] == INVALIDr || sid[i] == INVALIDm) {
            break;
        }
        /*
        * Disable SER checking for some PTs reported again and again
        * 1/ Some PTs' SER recovery type is BCMDRD_SER_RESP_NONE or
        * 2/ There H/W fault in PTs but H/W fault checking is not enabled.
        * such as: MMU_WRED_PORT_SP_SHARED_COUNTm
        */
        severity_error = bcmptm_ser_severity_fault_check(unit, sid[i], spci->flags);
        if (severity_error == TRUE) {
            rv = SHR_E_INTERNAL;
            break;
        }
        valid = 0;
        rv = bcmptm_ser_paired_tcam_index_valid(unit, sid[i], index[i], &valid);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Fail to check validation of index[%d] of memory table[%s]\n"),
                     index[i], bcmdrd_pt_sid_to_name(unit, sid[i])));
            continue;
        }
        /* slice mode or entry mode(narrow/wide) had been changed when bcmptm_scor_locks_take */
        if (valid == 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "index[%d] of memory table[%s] is invalid,"
                                 "slice mode or entry mode is changed!\n"),
                     index[i], bcmdrd_pt_sid_to_name(unit, sid[i])));
            continue;
        }
        valid = bcmptm_ser_sram_index_valid(unit, sid[i], index[i], &max_index);
        /* Size of UFT memory tables is updated */
        if (valid == 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "index[%d] of memory table[%s] is invalid,"
                                 "size of UFT PTs is updates!\n"),
                     index[i], bcmdrd_pt_sid_to_name(unit, sid[i])));
            continue;
        }
        ser_resp = BCMDRD_SER_RESP_CACHE_RESTORE;

        rv = bcmdrd_pt_ser_info_get(unit, sid[i], &info);
        if (SHR_SUCCESS(rv)) {
            ser_resp = info.resp;
        }
        if (BCMDRD_SER_RESP_NONE == ser_resp) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "SER recovery of [%s] inst[%d] is [ser_no_operation],"
                                 " so ser error is not corrected\n"),
                     bcmdrd_pt_sid_to_name(unit, sid[i]), tbl_inst[i]));
        } else {
            sram_info = sal_alloc(sizeof(bcmptm_ser_sram_info_t), "sram info");
            rv = BCMPTM_SER_DATA_DRIVER_CB(unit, xor_lp_mem_info_get_fn)(unit, sid[i], (uint32_t)index[i], sram_info);
            if (SHR_SUCCESS(rv)) {
                /* LP tables reports SER error, except FPEM_LPm */
                if (sram_info->hw_recovery) {
                    rv = bcmptm_ser_lp_hw_correct(unit, sid[i], tbl_inst[i],
                                                  index[i], sram_info,
                                                  spci->flags);
                } else {
                    /* XOR tables reports SER error */
                    rv = bcmptm_ser_mem_special_correct(unit, sid[i], tbl_inst[i],
                                                        index[i], sram_info,
                                                        ser_resp, spci->flags);
                }
                if (SHR_FAILURE(rv)) {
                    break;
                }
            } else if (rv == SHR_E_UNAVAIL) {
                /* non xor or lp reports SER error */
                pt_dyn_info.index = index[i];
                pt_dyn_info.tbl_inst = tbl_inst[i];
                rv = bcmptm_ser_mem_direct_correct(unit, sid[i], &pt_dyn_info,
                                                   ser_resp, spci->flags);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to get sram information of HASH_XOR or LP table [%s]\n"),
                           bcmdrd_pt_sid_to_name(unit, sid[i])));
                break;
            }
        }
    }
    (void)bcmptm_scor_locks_give(unit, sid[0]);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (sram_info != NULL) {
        sal_free(sram_info);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to correct non CCI SER error\n")));
    }
    SHR_FUNC_EXIT();
}
/******************************************************************************
* bcmptm_ser_spci_init
 */
static void
bcmptm_ser_spci_init(int unit, bcmptm_ser_correct_info_t *spci)
{
    spci->flags = 0;
    spci->reported_sid = INVALIDm;
    spci->second_sid = INVALIDm;
    spci->sid = INVALIDm;
    spci->tbl_inst = -1;
    spci->blk_type = -1;
    spci->addr = -1;
    spci->second_addr = -1;
    spci->index = -1;
    spci->second_index = -1;
}

/******************************************************************************
* bcmptm_serc_process
 */
static void
bcmptm_serc_process(void *unit_vp)
{
    int rv = SHR_E_NONE, unit = PTR_TO_INT(unit_vp);
    bcmptm_ser_intr_msg_q_info_t ser_msg;
    int intr_num;
    bcmptm_ser_blk_type_t ser_blk_type = BCMPTM_SER_BLK_INVAID;
    bcmptm_ser_top_level_route_info_t *hit_top_rb = NULL;
    bcmptm_ser_correct_info_t spci;
    uint32_t corrected = 1;

    while (1) {
        sal_memset(&ser_msg, 0, sizeof(ser_msg));

        if (bcmptm_ser_msg_count(unit) == 0) {
            (void)bcmptm_scor_done(unit);
        }
        if (serc_thread_pause[unit]) {
            rv = bcmptm_ser_msg_recieve(unit, &ser_msg, SAL_MSGQ_FOREVER);
            if (SERC_TERMINATE_FLAG(ser_msg)) {
                break;
            } else {
                (void)bcmptm_ser_msg_cache_push(unit, &ser_msg);
                continue;
            }
        } else {
            rv = bcmptm_ser_msg_cache_pop(unit, &ser_msg);
            if (rv == SHR_E_EMPTY) {
                rv = bcmptm_ser_msg_recieve(unit, &ser_msg, SAL_MSGQ_FOREVER);
            }
            if (SERC_TERMINATE_FLAG(ser_msg)) {
                break;
            }
        }

        intr_num = ser_msg.reg_id * 32 + ser_msg.bit_offset;
        rv = bcmptm_ser_intr_num_check(unit, intr_num, &ser_blk_type, &hit_top_rb);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        while (1) {
            corrected = 1;
            bcmptm_ser_spci_init(unit, &spci);
            rv = bcmptm_ser_fifo_pop_and_parse(unit, ser_msg.intr_param, ser_blk_type, hit_top_rb, &spci);
            /* FIFO is empty */
            if (SHR_FAILURE(rv)) {
                break;
            }
            if (spci.flags & BCMPTM_SER_FLAG_NON_SBUS) {
                bcmptm_ser_buffer_bus_ser_req(unit, &spci);
                corrected = 0;
            } else if (spci.sid != INVALIDm && spci.sid != INVALIDr) {
                (void)bcmptm_ser_error_entry_record(unit, spci.sid, spci.tbl_inst, spci.index);
                if (bcmdrd_pt_is_counter(unit, spci.sid)) {
                    
                    rv = bcmptm_ser_cci_ser_temp_req(unit, &spci);
                } else {
                    rv = bcmptm_ser_non_cci_ser_req(unit, &spci);
                }
                if (SHR_FAILURE(rv)) {
                    corrected = 0;
                }
            } else {
                /* SER comes from IDB or Port */
                corrected = 0;
            }
            bcmptm_ser_log(unit, &spci, corrected);
            bcmptm_ser_statistics(unit, &spci);
            /* write to IMM LT */
            rv = BCMPTM_SER_CTH_CB(ser_stats_imm_update)(unit, 0);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Fail to write cache data to IMM LT\n")));
            }
        }
        
        if (ser_blk_type != BCMPTM_SER_BLK_XLPORT && ser_blk_type != BCMPTM_SER_BLK_CLPORT) {
            BCMPTM_SER_DATA_DRIVER_CB(unit, cmic_intr_enable_fn)(ser_msg.unit, 0, ser_msg.reg_id, ser_msg.bit_offset, 1);
        }
    }
    serc_thread_id[unit] = SAL_THREAD_ERROR;
    (void)sal_sem_give(serc_terminate_sem[unit]);
    sal_thread_exit(0);
}

 /******************************************************************************
 * Public Functions
 */

/******************************************************************************
* bcmptm_ser_serc_start Start SERC thread
 */
int
bcmptm_ser_serc_start(int unit)
{
    if (!BCMPTM_SER_CTH_CB(ser_checking_start)(unit)) {
        return SHR_E_NONE;
    }
    if (bcmptm_ser_intr_msg_q_create(unit) != SHR_E_NONE) {
        return SHR_E_INIT;
    }
    if (serc_thread_id[unit] != SAL_THREAD_ERROR) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SERC thread already have been started.\n")));
        return SHR_E_NONE;
    }
    serc_terminate_sem[unit] = sal_sem_create("serc terminate", SAL_SEM_BINARY, 0);
    if (serc_terminate_sem[unit] == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to create semaphore.\n")));
        return SHR_E_INTERNAL;
    }
    serc_thread_stop[unit] = FALSE;

    serc_thread_id[unit] = sal_thread_create("SER correct",
                                             SAL_THREAD_STKSZ,
                                             50,
                                             bcmptm_serc_process,
                                             INT_TO_PTR(unit));
    if (serc_thread_id[unit] == SAL_THREAD_ERROR) {
        serc_thread_stop[unit] = TRUE;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Could not start SERC thread!\n")));
        return SHR_E_MEMORY;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "SERC thread is running.\n")));
    if (bcmptm_ser_tcam_scan_start(unit) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "FAIL to start TCAM scan thread!\n")));
        /* Do not return ERROR */
    }
    if (bcmptm_ser_sram_scan_start(unit, 0) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "FAIL to start SRAM scan thread!\n")));
        /* Do not return ERROR */
    }
    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_serc_stop terminate SERC thread
 */
int
bcmptm_ser_serc_stop(int unit)
{
    if (!BCMPTM_SER_CTH_CB(ser_checking_start)(unit)) {
        return SHR_E_NONE;
    }
    if (serc_thread_id[unit] == SAL_THREAD_ERROR) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SERC thread already have been terminated\n")));
        return SHR_E_NONE;
    }
    if (bcmptm_ser_tcam_scan_stop(unit, 1) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "FAIL to stop TCAM scan thread!\n")));
        /* Do not return ERROR */
    }
    if (bcmptm_ser_sram_scan_stop(unit, 1) != SHR_E_NONE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "FAIL to stop SRAM scan thread!\n")));
        /* Do not return ERROR */
    }
    serc_thread_stop[unit] = TRUE;
    bcmptm_ser_notify(unit, 0);/* Tell SERC to terminate */

    if (serc_terminate_sem[unit] != NULL) {
        (void)sal_sem_take(serc_terminate_sem[unit], SAL_SEM_FOREVER);
        sal_sem_destroy(serc_terminate_sem[unit]);
        serc_terminate_sem[unit] = NULL;
    }

    bcmptm_ser_intr_msg_q_destroy(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "SERC thread is terminated\n")));
    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_serc_pause  Pause SERC thread, may be used during SER injection and test
 */
int
bcmptm_ser_serc_pause(int unit)
{
    serc_thread_pause[unit] = TRUE;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Pause SERC thread\n")));

    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_serc_resume  Resume SERC thread, may be used during SER injection and test
 */
int
bcmptm_ser_serc_resume(int unit)
{
    serc_thread_pause[unit] = FALSE;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Resume SERC thread\n")));

    return SHR_E_NONE;
}

/******************************************************************************
* bcmptm_ser_severity_fault_checking_disable: may be used during SER injection and test
 */
void
bcmptm_ser_severity_fault_checking_disable(int unit, int disable)
{
    ser_sf_checking_disable[unit] = disable;
}

