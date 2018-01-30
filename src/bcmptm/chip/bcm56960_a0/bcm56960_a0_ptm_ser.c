/*! \file bcm56960_a0_ptm_ser.c
 *
 * <description>
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
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmbd/chip/bcm56960_a0_mmu_intr.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>
#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmbd/chip/bcm56960_a0_cmic_intr.h>
#include <bcmbd/bcmbd_cmicd_intr.h>

#include "./bcm56960_a0_ptm_ser.h"

/*******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Id of XPE0, XPE1, XPE2, XEP3 */
#define  MMU_XPE0  0
#define  MMU_XPE1  1
#define  MMU_XPE2  2
#define  MMU_XPE3  3

/* Id of SC0, SC1 */
#define  MMU_SC0  0
#define  MMU_SC1  1

/* ACC_TYPE     => $ACC_TYPE{SINGLE} */
#define ACC_TYPE_SINGLE          20

/* ACC_TYPE     => $ACC_TYPE{UNIQUE} */
#define ACC_TYPE_UNIQUE          31

/* ACC_TYPE     => $ACC_TYPE{DUPLICATE} */
#define ACC_TYPE_DUPLICATE             9

/* ACC_TYPE     => $ACC_TYPE{DATA_SPLIT} */
#define ACC_TYPE_DATA_SPLIT            14

/* ACC_TYPE     => $ACC_TYPE{ADDR_SPLIT} */
#define ACC_TYPE_ADDR_SPLIT            10

/* ACC_TYPE     => $ACC_TYPE{ADDR_SPLIT_SPLIT} */
#define ACC_TYPE_ADDR_SPLIT_SPLIT      12


/* BASE_TYPE => $BASE_TYPE{XPE} */
#define BT_XPE                   5

/* Segment bits of 56960_a0 */
#define TH_MMU_ADDR_ZERO_OUT_SEG_FIELD 0xfffc7fff /* [17:15] = 3'b000 */
#define TH_MMU_ADDR_SEG0 0x00000000 /* [17:15] = 3'b000 */
#define TH_MMU_ADDR_SEG1 0x00008000 /* [17:15] = 3'b001 */
#define TH_MMU_ADDR_SEG2 0x00010000 /* [17:15] = 3'b010 */
#define TH_MMU_ADDR_SEG3 0x00018000 /* [17:15] = 3'b011 */

/* 17, 16, 15 bits: offset in bucket */
#define TH_ALPM_MODE0_BKT_OFFSET_MASK                   0x38000
/* 16, 15, 14 bits: offset in bucket */
#define TH_ALPM_MODE1_BKT_OFFSET_MASK                   0x1C000


/* Refer to bcmbd_cmicd_schan.h */
#define READ_MEMORY_CMD_MSG           0x07
#define READ_REGISTER_CMD_MSG         0x0b
#define WRITE_MEMORY_CMD_MSG          0x09
#define WRITE_REGISTER_CMD_MSG        0x0d

#define BCMPTM_SER_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define BCMPTM_SER_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define BCMPTM_SER_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define BCMPTM_SER_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define BCMPTM_SER_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define BCMPTM_SER_OPCODE_SET(d,v)    F32_SET(d,26,6,v)

/* Assign value to val[BCMDRD_CONFIG_MAX_UNITS] */
#define ASSIGN_ARRAY_VAL(v)   { v, v, v, v, v, v, v, v }

/* Code of access type, string name of access type */
typedef struct bcmptm_ser_acctype_info_s {
    uint32_t    acctype;
    const char *acctype_name;
} bcmptm_ser_acctype_info_t;


/* Base information for LP and XOR hash memory tables */
/* sid_reported,  offset, entries_per_ram, ram_count, sid_corrected, continous */
typedef struct bcmptm_ser_sram_base_info_s {
    bcmdrd_sid_t    sid_reported;
    uint32_t        offset;
    uint32_t        entries_per_ram;
    uint32_t        ram_count;
    bcmdrd_sid_t    sid_corrected;
    uint32_t        continous;
} bcmptm_ser_sram_base_info_t;

/* Get uft bank number */
typedef int (*fn_pt_bank_num_get)(int unit);

typedef struct bcmptm_ser_uft_info_s {
    /* SID can use UFT */
    bcmdrd_sid_t              sid;
    /* entry number per bucket */
    uint32_t                  dedicated_entry_num;
    /* entry number per bucket */
    uint32_t                  entry_num_per_bucket;
    /* get bank number which is used by SID */
    fn_pt_bank_num_get        fn_bank_num_get;
} bcmptm_ser_uft_info_t;


/*******************************************************************************
 * Private variables
 */
/* List of PTs which are needed to be cleared before enabling SER parity */
static bcmptm_ser_mem_clr_list_t bcm56960_a0_mem_clr_lst[] = {
    { MMU_CBPDATA0m, 1, 0 },
    { MMU_CBPDATA15m, 1, 0 },
    { MMU_CBPDATA30m, 1, 0 },
    { MMU_CBPDATA45m, 1, 0 },

    { EGR_1588_LINK_DELAY_64r, 1, -1 }
};

/* In order to clear ING SER FIFO, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t ing_ser_fifo_ctrl_fld_lst[] = {
    {FIFO_RESETf, ASSIGN_ARRAY_VAL(1), 0},
    {FIFO_RESETf, ASSIGN_ARRAY_VAL(0), 0}, /* reset */
};

/* In order to clear EGR SER FIFO, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t egr_ser_fifo_ctrl_fld_lst[] = {
    {FIFO_RESETf, ASSIGN_ARRAY_VAL(1), 0},
    {FIFO_RESETf, ASSIGN_ARRAY_VAL(0), 0}, /* reset */
};

/* In order to clear CLPORT MIB, CLPORT_MIB_RESETr the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t clport_mib_reset_fld_lst[] = {
    {CLR_CNTf, ASSIGN_ARRAY_VAL(0xf), 0},
    {CLR_CNTf, ASSIGN_ARRAY_VAL(0), 0}, /* reset */
};

/* In order to init idb memory tables, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t idb_hw_ctrl_fld_lst[] = {
    {IS_MEM_INITf, ASSIGN_ARRAY_VAL(1), 0},
    {IS_MEM_INITf, ASSIGN_ARRAY_VAL(0), 0}, /* reset */
};

/* In order to init mmu(gcfg) memory tables, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t mmu_gcfg_miscconfig_fld_lst[] = {
    {INIT_MEMf, ASSIGN_ARRAY_VAL(1), 0},
    {PARITY_ENf, ASSIGN_ARRAY_VAL(1), 0},
    {INIT_MEMf, ASSIGN_ARRAY_VAL(0), 0}, /* reset */
    {PARITY_ENf, ASSIGN_ARRAY_VAL(0), 0},
    {REFRESH_ENf, ASSIGN_ARRAY_VAL(0), 0},
    {REFRESH_ENf, ASSIGN_ARRAY_VAL(1), 0}
};

/*
 * List of control registers which are needed to be configured, in order to clear memory tables
 * Those memory tables should be cleared before enabling SER
 */
static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_clear_mem_before_ser_enable_info[] = {
     /* used to clear ING SER FIFO */
    {ING_SER_FIFO_CTRLr, 0, &ing_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {ING_SER_FIFO_CTRLr, 0, &ing_ser_fifo_ctrl_fld_lst[1], 1, 0},
     /* used to clear EGR SER FIFO */
    {EGR_SER_FIFO_CTRLr, 0, &egr_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {EGR_SER_FIFO_CTRLr, 0, &egr_ser_fifo_ctrl_fld_lst[1], 1, 0},
     /* used to clear CLPORT MIB */
    {CLPORT_MIB_RESETr, 0, &clport_mib_reset_fld_lst[0], 1, 0},
    {CLPORT_MIB_RESETr, 0, &clport_mib_reset_fld_lst[1], 1, 0},
    /* disable REFRESH_EN */
    {MMU_GCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[4], 1, 0}
     /* add new items here */
};

/*
 * List of control registers which are needed to be configured, in order to clear memory tables
 * The memory tables should be cleared after enabling SER
 */
static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_clear_mem_after_ser_enable_info[] = {
     /* used to clear memory tables belonged to IDB */
    {IDB_HW_CONTROLr, 0, &idb_hw_ctrl_fld_lst[0], 1, 0},
    {IDB_HW_CONTROLr, 0, &idb_hw_ctrl_fld_lst[1], 1, 0},
     /* used to clear memory tables belonged to MMU GCFG */
    {MMU_GCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[0], 2, 0},
    {MMU_GCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[2], 1, 20},
    {MMU_GCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[3], 1, 0},
    {MMU_GCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[5], 1, 0}
     /* add new items here */
};

/* In order to enable intr of l2_mgmt */
static bcmptm_ser_fld_info_t l2_mgmt_intr_mask_fld_lst[] = {
    {SER_FIFO_NOT_EMPTYf, ASSIGN_ARRAY_VAL(1), 0},
};


/*
 * List of control registers which are used to enable interrupt source of L2_MGMT
 */
static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_enable_l2_mgmt_intr_info[] = {
    { L2_MGMT_INTR_MASKr, 0, l2_mgmt_intr_mask_fld_lst, COUNTOF(l2_mgmt_intr_mask_fld_lst) },
     /* add new items here */
};

/* In order to configure SER_ACC_TYPE_MAPm, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t ser_acc_type_map_fld_lst[] = {
    {COMPRESSED_ACC_TYPEf, ASSIGN_ARRAY_VAL(0), 0}, /* for index 0 */
    {COMPRESSED_ACC_TYPEf, ASSIGN_ARRAY_VAL(1), 0}, /* for index 1 */
    {COMPRESSED_ACC_TYPEf, ASSIGN_ARRAY_VAL(2), 0}, /* for index 2 */
    {COMPRESSED_ACC_TYPEf, ASSIGN_ARRAY_VAL(3), 0}, /* for index 3 */
};

/*
  * COMPRESSED_ACC_TYPEf of SER_ACC_TYPE_MAPm[n]
  * will be configure as n (n = 0, 1, 2, 3).
  */
static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_ser_acc_type_map_lst[] = {
    {SER_ACC_TYPE_MAPm, 0, &ser_acc_type_map_fld_lst[0], 1},
    {SER_ACC_TYPE_MAPm, 1, &ser_acc_type_map_fld_lst[1], 1},
    {SER_ACC_TYPE_MAPm, 2, &ser_acc_type_map_fld_lst[2], 1},
    {SER_ACC_TYPE_MAPm, 3, &ser_acc_type_map_fld_lst[3], 1},
};

/* In order to start cmic SER engine, the following field of SER_RING_ERR_CTRL should be set as 'val'  */
static bcmptm_ser_fld_info_t ser_ring_err_ctrl_fld_lst[] = {
    {ENABLEf, ASSIGN_ARRAY_VAL(0xf), 0},
};

/*
 * In order to start cmic SER engine, the following field of SER_RANGE_ENABLE should be set,
 * SER_RANGE_ENABLEf will be configured
 * by routine bcm56960_a0_cmic_ser_engine_regs_info_sync
 */
static bcmptm_ser_fld_info_t ser_range_enable_fld_lst[] = {
    {SER_RANGE_ENABLEf, ASSIGN_ARRAY_VAL(0), 0},
};

/* In order to start cmic SER engine, the following field of SER_CONFIG_REG should be set as "val"  */
static bcmptm_ser_fld_info_t ser_config_reg_fld_lst[] = {
    {CORRECT_1Bf, ASSIGN_ARRAY_VAL(1), 0},
    {INT_ENABLEf, ASSIGN_ARRAY_VAL(1), 0},
    {SER_BYPASSf, ASSIGN_ARRAY_VAL(0), 0},
};

/*
  * cmic registers should be configured to start cmic ser engine
  */
static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_cmic_ser_engine_reg_lst[] = {
    {SER_RING_ERR_CTRLr, 0, ser_ring_err_ctrl_fld_lst, COUNTOF(ser_ring_err_ctrl_fld_lst)},
    {SER_RANGE_ENABLEr,  0, ser_range_enable_fld_lst, COUNTOF(ser_range_enable_fld_lst)},
    {SER_CONFIG_REGr,    0, ser_config_reg_fld_lst, COUNTOF(ser_config_reg_fld_lst)},
};

static bcmptm_ser_tcam_generic_info_t bcm56960_a0_tcam_ser_info[] = {
    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_TUNNEL_ONLYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_TUNNEL_ONLYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_TUNNEL_ONLYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_TUNNEL_ONLYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { ING_SNATm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { ING_SNATm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { ING_SNATm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { ING_SNATm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { VLAN_SUBNETm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VLAN_SUBNETm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { VLAN_SUBNETm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { VLAN_SUBNETm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { MY_STATION_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { MY_STATION_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { MY_STATION_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { MY_STATION_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { IP_MULTICAST_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IP_MULTICAST_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IP_MULTICAST_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IP_MULTICAST_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { L2_USER_ENTRYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L2_USER_ENTRYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L2_USER_ENTRYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L2_USER_ENTRYm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIPm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_NARROW_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIPm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_NARROW_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_DEFIPm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_NARROW_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_DEFIPm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_NARROW_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIP_PAIR_128m, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_WIDE_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIP_PAIR_128m, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_WIDE_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_DEFIP_PAIR_128m, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_WIDE_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_DEFIP_PAIR_128m, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_OVERLAY |
      BCMPTM_SER_FLAG_WIDE_VIEW |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* Global view */
    { SRC_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { SRC_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { SRC_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { SRC_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { SRC_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { DST_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { DST_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { DST_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { DST_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { DST_COMPRESSIONm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { FP_UDF_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { FP_UDF_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { FP_UDF_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { FP_UDF_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { FP_UDF_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { VFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { VFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { VFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { VFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, BCMPTM_SER_TYPE_PARITY,
      BCMPTM_SER_PARITY_8BITS, BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { EFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { EFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { EFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { EFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_TCAMm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_TCAM_WIDEm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAM_WIDEm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_TCAM_WIDEm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_TCAM_WIDEm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE |
      BCMPTM_SER_FLAG_XY_READ | BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_TCAM_WIDEm, INVALIDm, BCMPTM_SER_TYPE_PARITY, BCMPTM_SER_PARITY_8BITS,
      BCMPTM_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      BCMPTM_SER_FLAG_MULTI_PIPE | BCMPTM_SER_FLAG_XY_READ |
      BCMPTM_SER_FLAG_ACC_TYPE_CHK | BCMPTM_SER_FLAG_VIEW_DISABLE, 4 },
};

/*!
 *\brief  can't enable parity checking for
 * some  memory tables or some bus or buffer spaces
 * such as: on TH+, OBM0_QUEUE_FIFO_1b, OBM0_QUEUE_FIFO_2b etc.
 */
static bcmdrd_sid_t  mem_ser_mute[] = {
    TRUNK_RR_CNTm,
};


static bcmdrd_fid_t bcm56960_a0_mmu_xpe_enable_fields[] = {
    THDM_PARITY_ENf,
    ENQX_PARITY_ENf,
    EPRG_PARITY_ENf,
    RQE_PARITY_ENf,
    THDI_PARITY_ENf,
    THDU_PARITY_ENf,
    PQE_PARITY_ENf,
    WRED_PARITY_ENf,
    DQX_PARITY_ENf,
    CCP_PARITY_ENf,
    CTR_PARITY_ENf,
    INVALIDf
};

static bcmdrd_fid_t bcm56960_a0_mmu_sc_enable_fields[] = {
    MB_A_PARITY_ENf,
    MB_B_PARITY_ENf,
    TDM_PARITY_ENf,
    ENQS_PARITY_ENf,
    MTRO_PARITY_ENf,
    VBS_PARITY_ENf,
    DQS_PARITY_ENf,
    TOQ_B_PARITY_ENf,
    TOQ_A_PARITY_ENf,
    CFAP_B_PARITY_ENf,
    CFAP_A_PARITY_ENf,
    INVALIDf
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56960_a0_idb_status_reg[] = {
    /* OBM queue buffer */
    { IDB_OBM0_QUEUE_ECC_STATUSr, "IDB OBM0 queue FIFO memory", 0, 0 },
    { IDB_OBM1_QUEUE_ECC_STATUSr, "IDB OBM1 queue FIFO memory", 0, 1 },
    { IDB_OBM2_QUEUE_ECC_STATUSr, "IDB OBM2 queue FIFO memory", 0, 2 },
    { IDB_OBM3_QUEUE_ECC_STATUSr, "IDB OBM3 queue FIFO memory", 0, 3 },
    { IDB_OBM4_QUEUE_ECC_STATUSr, "IDB OBM4 queue FIFO memory", 0, 4 },
    { IDB_OBM5_QUEUE_ECC_STATUSr, "IDB OBM5 queue FIFO memory", 0, 5 },
    { IDB_OBM6_QUEUE_ECC_STATUSr, "IDB OBM6 queue FIFO memory", 0, 6 },
    { IDB_OBM7_QUEUE_ECC_STATUSr, "IDB OBM7 queue FIFO memory", 0, 7 },
    /* OBM data */
    { IDB_OBM0_DATA_ECC_STATUSr, "IDB OBM0 packet data memory", 1, 0 },
    { IDB_OBM1_DATA_ECC_STATUSr, "IDB OBM1 packet data memory", 1, 1 },
    { IDB_OBM2_DATA_ECC_STATUSr, "IDB OBM2 packet data memory", 1, 2 },
    { IDB_OBM3_DATA_ECC_STATUSr, "IDB OBM3 packet data memory", 1, 3 },
    { IDB_OBM4_DATA_ECC_STATUSr, "IDB OBM4 packet data memory", 1, 4 },
    { IDB_OBM5_DATA_ECC_STATUSr, "IDB OBM5 packet data memory", 1, 5 },
    { IDB_OBM6_DATA_ECC_STATUSr, "IDB OBM6 packet data memory", 1, 6 },
    { IDB_OBM7_DATA_ECC_STATUSr, "IDB OBM7 packet data memory", 1, 7 },
    { IDB_OBM0_CA_ECC_STATUSr, "IDB OBM0 CA packet buffer memory", 2, 0 },
    { IDB_OBM1_CA_ECC_STATUSr, "IDB OBM1 CA packet buffer memory", 2, 1 },
    { IDB_OBM2_CA_ECC_STATUSr, "IDB OBM2 CA packet buffer memory", 2, 2 },
    { IDB_OBM3_CA_ECC_STATUSr, "IDB OBM3 CA packet buffer memory", 2, 3 },
    { IDB_OBM4_CA_ECC_STATUSr, "IDB OBM4 CA packet buffer memory", 2, 4 },
    { IDB_OBM5_CA_ECC_STATUSr, "IDB OBM5 CA packet buffer memory", 2, 5 },
    { IDB_OBM6_CA_ECC_STATUSr, "IDB OBM6 CA packet buffer memory", 2, 6 },
    { IDB_OBM7_CA_ECC_STATUSr, "IDB OBM7 CA packet buffer memory", 2, 7 },
    { IDB_CA_CPU_ECC_STATUSr,  "CA-CPU packet buffer memory", 3, 0 },
    { IDB_CA_LPBK_ECC_STATUSr, "CA-LPBK packet buffer memory", 4, 0 },
    { IDB_IS_TDM_CAL_ECC_STATUSr, "IDB IS_TDM0,1 calendar", -1, -1 },
    { INVALIDr } /* must */
};

/* Field list of status registers for  BCMPTM_SER_PARITY_TYPE_ECC */
static bcmdrd_fid_t bcm56960_a0_ecc_status_fields[] = {
    MULTIPLE_ERRf,
    ENTRY_IDXf,
    DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

/* Field list of status registers for  BCMPTM_SER_PARITY_TYPE_PARITY */
/*
static bcmdrd_fid_t bcm56960_a0_ecc_parity_fields[] = {
    MULTIPLE_ERRf,
    ENTRY_IDXf,
    INVALIDf
};
*/
/* Field list of status registers for  BCMPTM_SER_PARITY_TYPE_MAC_TX_CDC */
static bcmdrd_fid_t bcm56960_a0_pm_tx_cdc_status_fields[] = {
    TX_CDC_SINGLE_BIT_ERRf,
    TX_CDC_DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

/* Field list of status registers for  BCMPTM_SER_PARITY_TYPE_MAC_RX_CDC */
static bcmdrd_fid_t bcm56960_a0_pm_rx_cdc_status_fields[] = {
    RX_CDC_SINGLE_BIT_ERRf,
    RX_CDC_DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

/* Field list of status registers for  BCMPTM_SER_PARITY_TYPE_MAC_RX_TS */
static bcmdrd_fid_t bcm56960_a0_pm_rx_ts_status_fields[] = {
    RX_CDC_SINGLE_BIT_ERRf,
    RX_CDC_DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56960_a0_pm_rx_ts_clear_field[] = {
    CLEAR_RX_TS_MEM_SINGLE_BIT_ERRf,
    CLEAR_RX_TS_MEM_DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56960_a0_pm_rx_cdc_clear_field[] = {
    CLEAR_RX_CDC_SINGLE_BIT_ERRf,
    CLEAR_RX_CDC_DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56960_a0_pm_tx_cdc_clear_field[] = {
    CLEAR_TX_CDC_SINGLE_BIT_ERRf,
    CLEAR_TX_CDC_DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56960_a0_pm_clp_rx_status_reg[] = {
    { CLPORT_MIB_RSC0_ECC_STATUSr, "CLP MIB RSC0 ECC", -1, -1 },
    { CLPORT_MIB_RSC1_ECC_STATUSr, "CLP MIB RSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56960_a0_pm_clp_tx_status_reg[] = {
    { CLPORT_MIB_TSC0_ECC_STATUSr, "CLP MIB TSC0 ECC", -1, -1 },
    { CLPORT_MIB_TSC1_ECC_STATUSr, "CLP MIB TSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56960_a0_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC", -1, -1 },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56960_a0_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC", -1, -1 },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

/* should not enable interrupt registers, need to call API 'bcmbd_mmu_intr_enable' to do so */
static bcmptm_ser_low_level_info_t bcm56960_a0_mmu_low_ser_info[] = {
    { BCMPTM_SER_PARITY_TYPE_MMU_GLB, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU GLB MEM PAR",
        MMU_GCFG_PARITY_ENr, INTFO_PARITY_ENf, NULL,
        INVALIDr /*MMU_GCFG_GLB_CPU_INT_ENr*/, INVALIDf /*MEM_PAR_ERR_ENf*/, NULL,
        INVALIDr /*MMU_GCFG_GLB_CPU_INT_STATr*/, NULL, INVALIDf /*MEM_PAR_ERR_STATf*/, NULL,
        INVALIDr /*MMU_GCFG_GLB_CPU_INT_CLEARr*/, INVALIDf /*MEM_PAR_ERR_CLRf*/, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_MMU_XPE, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU XPE MEM PAR",
        MMU_XCFG_PARITY_ENr, INVALIDf, bcm56960_a0_mmu_xpe_enable_fields,
        INVALIDr /*MMU_XCFG_XPE_CPU_INT_ENr*/, INVALIDf /*MEM_PAR_ERR_ENf*/, NULL,
        INVALIDr /*MMU_XCFG_XPE_CPU_INT_STATr*/, NULL, INVALIDf /* MEM_PAR_ERR_STATf */, NULL,
        INVALIDr /*MMU_XCFG_XPE_CPU_INT_CLEARr*/, INVALIDf /*MEM_PAR_ERR_CLRf*/, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_MMU_SC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SC MEM PAR",
        MMU_SCFG_PARITY_ENr, INVALIDf, bcm56960_a0_mmu_sc_enable_fields,
        INVALIDr /*MMU_SCFG_SC_CPU_INT_ENr*/, INVALIDf, NULL /*bcm56960_a0_mmu_sc_int_enable_fields*/,
        INVALIDr /*MMU_SCFG_SC_CPU_INT_STATr*/, NULL, INVALIDf /*MEM_PAR_ERR_STATf*/,
        NULL/*_soc_th_mmu_sc_status_fields*/,
        INVALIDr /*MMU_SCFG_SC_CPU_INT_CLEARr*/, INVALIDf/*MEM_PAR_ERR_CLRf*/,
        NULL/*_soc_th_mmu_sc_clr_fields*/
    },
    { BCMPTM_SER_PARITY_TYPE_NONE } /* table terminator */
};
 /*
  * fields of MMU_SCFG_MEM_FAIL_ADDR_64m/ MMU_XCFG_MEM_FAIL_ADDR_64m/
  * MMU_GCFG_MEM_FAIL_ADDR_64m.
  * Sequence of fields cannot be changed.
  */
static bcmdrd_fid_t mem_fail_addr_flds[] = {
    ERR_MULTf,
    ERR_TYPEf,
    EADDRf
};

static bcmptm_ser_mmu_intr_mapping_t bcm56960_a0_mmu_intr_map[] = {
    { MMU_INTR_GLB_MEM_PAR_ERR, BCMPTM_SER_PARITY_TYPE_MMU_GLB,
      MMU_GCFG_MEM_SER_FIFO_STSr, EMPTYf, MMU_GCFG_MEM_FAIL_ADDR_64m,
      mem_fail_addr_flds, MMU_GCFG_MEM_FAIL_INT_CTRr, "MMU_GLB" },

    { MMU_INTR_XPE_MEM_PAR_ERR, BCMPTM_SER_PARITY_TYPE_MMU_XPE,
      MMU_XCFG_MEM_SER_FIFO_STSr, EMPTYf, MMU_XCFG_MEM_FAIL_ADDR_64m,
      mem_fail_addr_flds, MMU_XCFG_MEM_FAIL_INT_CTRr, "MMU_XPE" },

    { MMU_INTR_SC_MEM_PAR_ERR, BCMPTM_SER_PARITY_TYPE_MMU_SC,
      MMU_SCFG_MEM_SER_FIFO_STSr, EMPTYf, MMU_SCFG_MEM_FAIL_ADDR_64m,
      mem_fail_addr_flds, MMU_SCFG_MEM_FAIL_INT_CTRr, "MMU_SC" }
};

static bcmptm_ser_low_level_info_t bcm56960_a0_mmu_top_ser_info[] = {
    { BCMPTM_SER_PARITY_TYPE_MMU_SER, bcm56960_a0_mmu_low_ser_info, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf, NULL,
        INVALIDr /*MMU_GCFG_ALL_CPU_INT_ENr*/, INVALIDf,
        NULL, /* bcm56960_a0_mmu_top_int_enable_fields */
        INVALIDr /*MMU_GCFG_ALL_CPU_INT_STATr*/,
        NULL, INVALIDf, NULL /*bcm56960_a0_mmu_top_int_status_fields*/,
        INVALIDr, INVALIDf, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_NONE } /* table terminator */
};

static bcmptm_ser_low_level_info_t bcm56960_a0_idb_ser_info[] = {
    { BCMPTM_SER_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "IP IDB memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, bcm56960_a0_idb_status_reg, ECC_ERRf, bcm56960_a0_ecc_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_NONE } /* table terminator */
};

static bcmptm_ser_low_level_info_t bcm56960_a0_pm_clp_ser_info[] = {
    { BCMPTM_SER_PARITY_TYPE_MAC_RX_TS, NULL, 0,
        MAC_RX_TS_CDC_MEM_ERRf, MAC_RX_TS_CDC_MEM_ERRf,
        INVALIDm, "MAC RX TimeStamp CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_RX_TS_MEM_ECC_STATUSr, NULL, INVALIDf, bcm56960_a0_pm_rx_ts_status_fields,
        CLMAC_CLEAR_ECC_STATUSr, INVALIDf, bcm56960_a0_pm_rx_ts_clear_field
    },
    { BCMPTM_SER_PARITY_TYPE_MAC_RX_CDC, NULL, 0,
        MAC_RX_CDC_MEM_ERRf, MAC_RX_CDC_MEM_ERRf,
        INVALIDm, "MAC RX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_RX_CDC_ECC_STATUSr, NULL, INVALIDf, bcm56960_a0_pm_rx_cdc_status_fields,
        CLMAC_CLEAR_ECC_STATUSr, INVALIDf, bcm56960_a0_pm_rx_cdc_clear_field
    },
    { BCMPTM_SER_PARITY_TYPE_MAC_TX_CDC, NULL, 0,
        MAC_TX_CDC_MEM_ERRf, MAC_TX_CDC_MEM_ERRf,
        INVALIDm, "MAC TX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_TX_CDC_ECC_STATUSr, NULL, INVALIDf, bcm56960_a0_pm_tx_cdc_status_fields,
        CLMAC_CLEAR_ECC_STATUSr, INVALIDf, bcm56960_a0_pm_tx_cdc_clear_field
    },
    { BCMPTM_SER_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, bcm56960_a0_pm_clp_rx_status_reg, ECC_ERRf, bcm56960_a0_ecc_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, bcm56960_a0_pm_clp_tx_status_reg, ECC_ERRf, bcm56960_a0_ecc_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_NONE } /* table terminator */
};

static bcmptm_ser_low_level_info_t bcm56960_a0_pm_xlp_ser_info[] = {
    { BCMPTM_SER_PARITY_TYPE_MAC_RX_CDC, NULL, 0,
        MAC_RX_CDC_MEM_ERRf, MAC_RX_CDC_MEM_ERRf,
        INVALIDm, "MAC RX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_RX_CDC_ECC_STATUSr, NULL, INVALIDf, bcm56960_a0_pm_rx_cdc_status_fields,
        XLMAC_CLEAR_ECC_STATUSr, INVALIDf, bcm56960_a0_pm_rx_cdc_clear_field
    },
    { BCMPTM_SER_PARITY_TYPE_MAC_TX_CDC, NULL, 0,
        MAC_TX_CDC_MEM_ERRf, MAC_TX_CDC_MEM_ERRf,
        INVALIDm, "MAC TX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_TX_CDC_ECC_STATUSr, NULL, INVALIDf, bcm56960_a0_pm_tx_cdc_status_fields,
        XLMAC_CLEAR_ECC_STATUSr, INVALIDf, bcm56960_a0_pm_tx_cdc_clear_field
    },
    { BCMPTM_SER_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, bcm56960_a0_pm_xlp_rx_status_reg, ECC_ERRf, bcm56960_a0_ecc_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, bcm56960_a0_pm_xlp_tx_status_reg, ECC_ERRf, bcm56960_a0_ecc_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { BCMPTM_SER_PARITY_TYPE_NONE } /* table terminator */
};

static bcmptm_ser_top_level_route_info_t bcm56960_a0_ser_route_blocks[] = {
    { 3, 0x00000010, /* MMU_TO_CMIC_MEMFAIL_INTR */
      BCMPTM_SER_BLK_MMU, -1, INVALIDr, INVALIDr, INVALIDf,
      bcm56960_a0_mmu_top_ser_info, 0 },
    /* xsm: not using MMU_GLB or MMU_XPE or MMU_SC is on purpose */
    { 3, 0x00000020, /* PIPE0_EP2_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_EPIPE, 0, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000040, /* PIPE1_EP2_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_EPIPE, 1, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000080, /* PIPE2_EP2_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_EPIPE, 2, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000100, /* PIPE3_EP2_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_EPIPE, 3, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x200, /* PIPE0_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      BCMPTM_SER_BLK_IPIPE_IDB, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      bcm56960_a0_idb_ser_info, 0 },
    { 3, 0x400, /* PIPE1_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      BCMPTM_SER_BLK_IPIPE_IDB, 1, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      bcm56960_a0_idb_ser_info, 0 },
    { 3, 0x800, /* PIPE2_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      BCMPTM_SER_BLK_IPIPE_IDB, 2, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      bcm56960_a0_idb_ser_info, 0 },
    { 3, 0x1000, /* PIPE3_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      BCMPTM_SER_BLK_IPIPE_IDB, 3, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      bcm56960_a0_idb_ser_info, 0 },
    { 3, 0x00002000, /* PIPE0_IP5_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00004000, /* PIPE1_IP5_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_IPIPE, 1, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00008000, /* PIPE2_IP5_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_IPIPE, 2, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00010000, /* PIPE3_IP5_TO_CMIC_PERR_INTR */
      BCMPTM_SER_BLK_IPIPE, 3, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 4, 0x00000001, /* PM0_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 0 },
    { 4, 0x00000002, /* PM1_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 1 },
    { 4, 0x00000004, /* PM2_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 2 },
    { 4, 0x00000008, /* PM3_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 3 },
    { 4, 0x00000010, /* PM4_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 4 },
    { 4, 0x00000020, /* PM5_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 5 },
    { 4, 0x00000040, /* PM6_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 6 },
    { 4, 0x00000080, /* PM7_INTR */
      BCMPTM_SER_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 7 },
    { 4, 0x00000100, /* PM8_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 8 },
    { 4, 0x00000200, /* PM9_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 9 },
    { 4, 0x00000400, /* PM10_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 10 },
    { 4, 0x00000800, /* PM11_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 11 },
    { 4, 0x00001000, /* PM12_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 12 },
    { 4, 0x00002000, /* PM13_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 13 },
    { 4, 0x00004000, /* PM14_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 14 },
    { 4, 0x00008000, /* PM15_INTR */
      BCMPTM_SER_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 15 },
    { 4, 0x00010000, /* PM16_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 16 },
    { 4, 0x00020000, /* PM17_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 17 },
    { 4, 0x00040000, /* PM18_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 18 },
    { 4, 0x00080000, /* PM19_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 19 },
    { 4, 0x00100000, /* PM20_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 20 },
    { 4, 0x00200000, /* PM21_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 21 },
    { 4, 0x00400000, /* PM22_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 22 },
    { 4, 0x00800000, /* PM23_INTR */
      BCMPTM_SER_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 23 },
    { 4, 0x01000000, /* PM24_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 24 },
    { 4, 0x02000000, /* PM25_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 25 },
    { 4, 0x04000000, /* PM26_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 26 },
    { 4, 0x08000000, /* PM27_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 27 },
    { 4, 0x10000000, /* PM28_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 28 },
    { 4, 0x20000000, /* PM29_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 29 },
    { 4, 0x40000000, /* PM30_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 30 },
    { 4, 0x80000000, /* PM31_INTR */
      BCMPTM_SER_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_clp_ser_info, 31 },
    { 5, 0x00000001, /* PM32_INTR */
      BCMPTM_SER_BLK_XLPORT, 1, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      bcm56960_a0_pm_xlp_ser_info, 0 },
    /* must keep this entry */
    { 0 } /* table terminator */
};

static bcmdrd_sid_t bcm56960_a0_range_cfg[] = {
    SER_RANGE_0_CONFIGr, SER_RANGE_1_CONFIGr, SER_RANGE_2_CONFIGr,
    SER_RANGE_3_CONFIGr,
    SER_RANGE_4_CONFIGr, SER_RANGE_5_CONFIGr, SER_RANGE_6_CONFIGr,
    SER_RANGE_7_CONFIGr,
    SER_RANGE_8_CONFIGr, SER_RANGE_9_CONFIGr, SER_RANGE_10_CONFIGr,
    SER_RANGE_11_CONFIGr,
    SER_RANGE_12_CONFIGr, SER_RANGE_13_CONFIGr, SER_RANGE_14_CONFIGr,
    SER_RANGE_15_CONFIGr,
    SER_RANGE_16_CONFIGr, SER_RANGE_17_CONFIGr, SER_RANGE_18_CONFIGr,
    SER_RANGE_19_CONFIGr,
    SER_RANGE_20_CONFIGr, SER_RANGE_21_CONFIGr, SER_RANGE_22_CONFIGr,
    SER_RANGE_23_CONFIGr,
    SER_RANGE_24_CONFIGr, SER_RANGE_25_CONFIGr, SER_RANGE_26_CONFIGr,
    SER_RANGE_27_CONFIGr,
    SER_RANGE_28_CONFIGr, SER_RANGE_29_CONFIGr, SER_RANGE_30_CONFIGr,
    SER_RANGE_31_CONFIGr
};

static bcmdrd_fid_t bcm56960_a0_range_cfg_flds[] = {
    INTERLEAVE_MODEf,
    PROT_MODEf,
    PROT_TYPEf,
    ACC_TYPE_MODEf,
    BLOCKf,
    INVALIDf /*must*/
};

static bcmdrd_sid_t bcm56960_a0_range_start[] = {
    SER_RANGE_0_STARTr, SER_RANGE_1_STARTr, SER_RANGE_2_STARTr,
    SER_RANGE_3_STARTr,
    SER_RANGE_4_STARTr, SER_RANGE_5_STARTr, SER_RANGE_6_STARTr,
    SER_RANGE_7_STARTr,
    SER_RANGE_8_STARTr, SER_RANGE_9_STARTr, SER_RANGE_10_STARTr,
    SER_RANGE_11_STARTr,
    SER_RANGE_12_STARTr, SER_RANGE_13_STARTr, SER_RANGE_14_STARTr,
    SER_RANGE_15_STARTr,
    SER_RANGE_16_STARTr, SER_RANGE_17_STARTr, SER_RANGE_18_STARTr,
    SER_RANGE_19_STARTr,
    SER_RANGE_20_STARTr, SER_RANGE_21_STARTr, SER_RANGE_22_STARTr,
    SER_RANGE_23_STARTr,
    SER_RANGE_24_STARTr, SER_RANGE_25_STARTr, SER_RANGE_26_STARTr,
    SER_RANGE_27_STARTr,
    SER_RANGE_28_STARTr, SER_RANGE_29_STARTr, SER_RANGE_30_STARTr,
    SER_RANGE_31_STARTr
};

static bcmdrd_sid_t bcm56960_a0_range_end[] = {
    SER_RANGE_0_ENDr, SER_RANGE_1_ENDr, SER_RANGE_2_ENDr, SER_RANGE_3_ENDr,
    SER_RANGE_4_ENDr, SER_RANGE_5_ENDr, SER_RANGE_6_ENDr, SER_RANGE_7_ENDr,
    SER_RANGE_8_ENDr, SER_RANGE_9_ENDr, SER_RANGE_10_ENDr, SER_RANGE_11_ENDr,
    SER_RANGE_12_ENDr, SER_RANGE_13_ENDr, SER_RANGE_14_ENDr, SER_RANGE_15_ENDr,
    SER_RANGE_16_ENDr, SER_RANGE_17_ENDr, SER_RANGE_18_ENDr, SER_RANGE_19_ENDr,
    SER_RANGE_20_ENDr, SER_RANGE_21_ENDr, SER_RANGE_22_ENDr, SER_RANGE_23_ENDr,
    SER_RANGE_24_ENDr, SER_RANGE_25_ENDr, SER_RANGE_26_ENDr, SER_RANGE_27_ENDr,
    SER_RANGE_28_ENDr, SER_RANGE_29_ENDr, SER_RANGE_30_ENDr, SER_RANGE_31_ENDr
};

static bcmdrd_sid_t bcm56960_a0_range_result[] = {
    SER_RANGE_0_RESULTr, SER_RANGE_1_RESULTr, SER_RANGE_2_RESULTr,
    SER_RANGE_3_RESULTr,
    SER_RANGE_4_RESULTr, SER_RANGE_5_RESULTr, SER_RANGE_6_RESULTr,
    SER_RANGE_7_RESULTr,
    SER_RANGE_8_RESULTr, SER_RANGE_9_RESULTr, SER_RANGE_10_RESULTr,
    SER_RANGE_11_RESULTr,
    SER_RANGE_12_RESULTr, SER_RANGE_13_RESULTr, SER_RANGE_14_RESULTr,
    SER_RANGE_15_RESULTr,
    SER_RANGE_16_RESULTr, SER_RANGE_17_RESULTr, SER_RANGE_18_RESULTr,
    SER_RANGE_19_RESULTr,
    SER_RANGE_20_RESULTr, SER_RANGE_21_RESULTr, SER_RANGE_22_RESULTr,
    SER_RANGE_23_RESULTr,
    SER_RANGE_24_RESULTr, SER_RANGE_25_RESULTr, SER_RANGE_26_RESULTr,
    SER_RANGE_27_RESULTr,
    SER_RANGE_28_RESULTr, SER_RANGE_29_RESULTr, SER_RANGE_30_RESULTr,
    SER_RANGE_31_RESULTr
};

static bcmdrd_sid_t bcm56960_a0_prot_word[] = {
    SER_RANGE_0_PROT_WORD_0r, SER_RANGE_0_PROT_WORD_1r,
    SER_RANGE_0_PROT_WORD_2r, SER_RANGE_0_PROT_WORD_3r,
    SER_RANGE_1_PROT_WORD_0r, SER_RANGE_1_PROT_WORD_1r,
    SER_RANGE_1_PROT_WORD_2r, SER_RANGE_1_PROT_WORD_3r,
    SER_RANGE_2_PROT_WORD_0r, SER_RANGE_2_PROT_WORD_1r,
    SER_RANGE_2_PROT_WORD_2r, SER_RANGE_2_PROT_WORD_3r,
    SER_RANGE_3_PROT_WORD_0r, SER_RANGE_3_PROT_WORD_1r,
    SER_RANGE_3_PROT_WORD_2r, SER_RANGE_3_PROT_WORD_3r,
    SER_RANGE_4_PROT_WORD_0r, SER_RANGE_4_PROT_WORD_1r,
    SER_RANGE_4_PROT_WORD_2r, SER_RANGE_4_PROT_WORD_3r,
    SER_RANGE_5_PROT_WORD_0r, SER_RANGE_5_PROT_WORD_1r,
    SER_RANGE_5_PROT_WORD_2r, SER_RANGE_5_PROT_WORD_3r,
    SER_RANGE_6_PROT_WORD_0r, SER_RANGE_6_PROT_WORD_1r,
    SER_RANGE_6_PROT_WORD_2r, SER_RANGE_6_PROT_WORD_3r,
    SER_RANGE_7_PROT_WORD_0r, SER_RANGE_7_PROT_WORD_1r,
    SER_RANGE_7_PROT_WORD_2r, SER_RANGE_7_PROT_WORD_3r,
    SER_RANGE_8_PROT_WORD_0r, SER_RANGE_8_PROT_WORD_1r,
    SER_RANGE_8_PROT_WORD_2r, SER_RANGE_8_PROT_WORD_3r,
    SER_RANGE_9_PROT_WORD_0r, SER_RANGE_9_PROT_WORD_1r,
    SER_RANGE_9_PROT_WORD_2r, SER_RANGE_9_PROT_WORD_3r,
    SER_RANGE_10_PROT_WORD_0r, SER_RANGE_10_PROT_WORD_1r,
    SER_RANGE_10_PROT_WORD_2r, SER_RANGE_10_PROT_WORD_3r,
    SER_RANGE_11_PROT_WORD_0r, SER_RANGE_11_PROT_WORD_1r,
    SER_RANGE_11_PROT_WORD_2r, SER_RANGE_11_PROT_WORD_3r,
    SER_RANGE_12_PROT_WORD_0r, SER_RANGE_12_PROT_WORD_1r,
    SER_RANGE_12_PROT_WORD_2r, SER_RANGE_12_PROT_WORD_3r,
    SER_RANGE_13_PROT_WORD_0r, SER_RANGE_13_PROT_WORD_1r,
    SER_RANGE_13_PROT_WORD_2r, SER_RANGE_13_PROT_WORD_3r,
    SER_RANGE_14_PROT_WORD_0r, SER_RANGE_14_PROT_WORD_1r,
    SER_RANGE_14_PROT_WORD_2r, SER_RANGE_14_PROT_WORD_3r,
    SER_RANGE_15_PROT_WORD_0r, SER_RANGE_15_PROT_WORD_1r,
    SER_RANGE_15_PROT_WORD_2r, SER_RANGE_15_PROT_WORD_3r,
    SER_RANGE_16_PROT_WORD_0r, SER_RANGE_16_PROT_WORD_1r,
    SER_RANGE_16_PROT_WORD_2r, SER_RANGE_16_PROT_WORD_3r,
    SER_RANGE_17_PROT_WORD_0r, SER_RANGE_17_PROT_WORD_1r,
    SER_RANGE_17_PROT_WORD_2r, SER_RANGE_17_PROT_WORD_3r,
    SER_RANGE_18_PROT_WORD_0r, SER_RANGE_18_PROT_WORD_1r,
    SER_RANGE_18_PROT_WORD_2r, SER_RANGE_18_PROT_WORD_3r,
    SER_RANGE_19_PROT_WORD_0r, SER_RANGE_19_PROT_WORD_1r,
    SER_RANGE_19_PROT_WORD_2r, SER_RANGE_19_PROT_WORD_3r,
    SER_RANGE_20_PROT_WORD_0r, SER_RANGE_20_PROT_WORD_1r,
    SER_RANGE_20_PROT_WORD_2r, SER_RANGE_20_PROT_WORD_3r,
    SER_RANGE_21_PROT_WORD_0r, SER_RANGE_21_PROT_WORD_1r,
    SER_RANGE_21_PROT_WORD_2r, SER_RANGE_21_PROT_WORD_3r,
    SER_RANGE_22_PROT_WORD_0r, SER_RANGE_22_PROT_WORD_1r,
    SER_RANGE_22_PROT_WORD_2r, SER_RANGE_22_PROT_WORD_3r,
    SER_RANGE_23_PROT_WORD_0r, SER_RANGE_23_PROT_WORD_1r,
    SER_RANGE_23_PROT_WORD_2r, SER_RANGE_23_PROT_WORD_3r,
    SER_RANGE_24_PROT_WORD_0r, SER_RANGE_24_PROT_WORD_1r,
    SER_RANGE_24_PROT_WORD_2r, SER_RANGE_24_PROT_WORD_3r,
    SER_RANGE_25_PROT_WORD_0r, SER_RANGE_25_PROT_WORD_1r,
    SER_RANGE_25_PROT_WORD_2r, SER_RANGE_25_PROT_WORD_3r,
    SER_RANGE_26_PROT_WORD_0r, SER_RANGE_26_PROT_WORD_1r,
    SER_RANGE_26_PROT_WORD_2r, SER_RANGE_26_PROT_WORD_3r,
    SER_RANGE_27_PROT_WORD_0r, SER_RANGE_27_PROT_WORD_1r,
    SER_RANGE_27_PROT_WORD_2r, SER_RANGE_27_PROT_WORD_3r,
    SER_RANGE_28_PROT_WORD_0r, SER_RANGE_28_PROT_WORD_1r,
    SER_RANGE_28_PROT_WORD_2r, SER_RANGE_28_PROT_WORD_3r,
    SER_RANGE_29_PROT_WORD_0r, SER_RANGE_29_PROT_WORD_1r,
    SER_RANGE_29_PROT_WORD_2r, SER_RANGE_29_PROT_WORD_3r,
    SER_RANGE_30_PROT_WORD_0r, SER_RANGE_30_PROT_WORD_1r,
    SER_RANGE_30_PROT_WORD_2r, SER_RANGE_30_PROT_WORD_3r,
    SER_RANGE_31_PROT_WORD_0r, SER_RANGE_31_PROT_WORD_1r,
    SER_RANGE_31_PROT_WORD_2r, SER_RANGE_31_PROT_WORD_3r
};
static bcmdrd_fid_t bcm56960_a0_range_prot_flds[] = {
    DATA_STARTf,
    DATA_ENDf,
    INVALIDf /*must*/
};

static bcmdrd_sid_t bcm56960_a0_range_addr_bits[] = {
    SER_RANGE_0_ADDR_BITSr, SER_RANGE_1_ADDR_BITSr,
    SER_RANGE_2_ADDR_BITSr, SER_RANGE_3_ADDR_BITSr,
    SER_RANGE_4_ADDR_BITSr, SER_RANGE_5_ADDR_BITSr,
    SER_RANGE_6_ADDR_BITSr, SER_RANGE_7_ADDR_BITSr,
    SER_RANGE_8_ADDR_BITSr, SER_RANGE_9_ADDR_BITSr,
    SER_RANGE_10_ADDR_BITSr, SER_RANGE_11_ADDR_BITSr,
    SER_RANGE_12_ADDR_BITSr, SER_RANGE_13_ADDR_BITSr,
    SER_RANGE_14_ADDR_BITSr, SER_RANGE_15_ADDR_BITSr,
    SER_RANGE_16_ADDR_BITSr, SER_RANGE_17_ADDR_BITSr,
    SER_RANGE_18_ADDR_BITSr, SER_RANGE_19_ADDR_BITSr,
    SER_RANGE_20_ADDR_BITSr, SER_RANGE_21_ADDR_BITSr,
    SER_RANGE_22_ADDR_BITSr, SER_RANGE_23_ADDR_BITSr,
    SER_RANGE_24_ADDR_BITSr, SER_RANGE_25_ADDR_BITSr,
    SER_RANGE_26_ADDR_BITSr, SER_RANGE_27_ADDR_BITSr,
    SER_RANGE_28_ADDR_BITSr, SER_RANGE_29_ADDR_BITSr,
    SER_RANGE_30_ADDR_BITSr, SER_RANGE_31_ADDR_BITSr
};

static bcmdrd_sid_t bcm56960_a0_range_disable[] = {
    SER_RANGE_0_DISABLEr, SER_RANGE_1_DISABLEr,
    SER_RANGE_2_DISABLEr, SER_RANGE_3_DISABLEr,
    SER_RANGE_4_DISABLEr, SER_RANGE_5_DISABLEr,
    SER_RANGE_6_DISABLEr, SER_RANGE_7_DISABLEr,
    SER_RANGE_8_DISABLEr, SER_RANGE_9_DISABLEr,
    SER_RANGE_10_DISABLEr, SER_RANGE_11_DISABLEr,
    SER_RANGE_12_DISABLEr, SER_RANGE_13_DISABLEr,
    SER_RANGE_14_DISABLEr, SER_RANGE_15_DISABLEr,
    SER_RANGE_16_DISABLEr, SER_RANGE_17_DISABLEr,
    SER_RANGE_18_DISABLEr, SER_RANGE_19_DISABLEr,
    SER_RANGE_20_DISABLEr, SER_RANGE_21_DISABLEr,
    SER_RANGE_22_DISABLEr, SER_RANGE_23_DISABLEr,
    SER_RANGE_24_DISABLEr, SER_RANGE_25_DISABLEr,
    SER_RANGE_26_DISABLEr, SER_RANGE_27_DISABLEr,
    SER_RANGE_28_DISABLEr, SER_RANGE_29_DISABLEr,
    SER_RANGE_30_DISABLEr, SER_RANGE_31_DISABLEr
};

static bcmptm_ser_fld_info_t bcm56960_a0_ser_result_fld[] = {
    { RANGEf, ASSIGN_ARRAY_VAL(0), 1},
    { SBUS_ADDRf, ASSIGN_ARRAY_VAL(0), 1},
    { ACC_TYPEf, ASSIGN_ARRAY_VAL(0), 1}
};

static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_ser_result[] = {
    { SER_RESULT_0m, 0, bcm56960_a0_ser_result_fld, COUNTOF(bcm56960_a0_ser_result_fld), 0},
    { SER_RESULT_1m, 0, bcm56960_a0_ser_result_fld, COUNTOF(bcm56960_a0_ser_result_fld), 0}
};

static bcmptm_ser_fld_info_t bcm56960_a0_ser_error_0_fld[] = {
    { ERROR_0_VALf, ASSIGN_ARRAY_VAL(0), 1},
};
static bcmptm_ser_fld_info_t bcm56960_a0_ser_error_1_fld[] = {
    { ERROR_1_VALf, ASSIGN_ARRAY_VAL(0), 1},
};

static bcmptm_ser_fld_info_t bcm56960_a0_ser_miss_event_fld[] = {
    { COUNTf, ASSIGN_ARRAY_VAL(0), 1}
};

static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_ser_error[] = {
    { SER_ERROR_0r, 0, bcm56960_a0_ser_error_0_fld, 1, 0},
    { SER_ERROR_1r, 0, bcm56960_a0_ser_error_1_fld, 1, 0},
    { SER_MISSED_EVENTr, 0, bcm56960_a0_ser_miss_event_fld, 1, 0}
};

/* field list of memory ING_SER_FIFOm and EGR_SER_FIFOm and L2_MGMT_SER_FIFOm */
bcmdrd_fid_t  bcm56960_a0_ser_ing_egr_fifo_fld[] = {
    VALIDf,
    ECC_PARITYf,
    MEM_TYPEf,
    ADDRESSf,
    PIPE_STAGEf,
    INSTRUCTION_TYPEf,
    DROPf,
    MULTIPLEf,
    NON_SBUSf,
    MEMBASEf,
    MEMINDEXf,
    REGBASEf,
    REGINDEXf
};

/*
 * If 'stage id' is 0xFF, that means 'membase' can identify buffer or bus.
 * The 'stage id' reported by H/W is ignored.
 * If 'stage id' is less than 0xFF, that means two or more buffers or buses share the
 * same 'membase', so both 'membase' and 'stage id' should be used to
 * identify buffer or bus.
 */
static bcmptm_ser_buffer_bus_info_t bcm56960_a0_ser_hwmem_base_info[] = {
    /* IDB */
    { "OBM0_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 0},
    { "OBM1_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 1},
    { "OBM2_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 2},
    { "OBM3_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 3},
    { "OBM4_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 4},
    { "OBM5_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 5},
    { "OBM6_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 6},
    { "OBM7_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 7},
    { "OBM0_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 0},
    { "OBM1_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 1},
    { "OBM2_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 2},
    { "OBM3_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 3},
    { "OBM4_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 4},
    { "OBM5_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 5},
    { "OBM6_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 6},
    { "OBM7_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 7},
    { "CELL_ASSEM_BUFFER0", "CELL_ASSEM_BUFFER - In Idb", 0x2, 0},
    { "CELL_ASSEM_BUFFER1", "CELL_ASSEM_BUFFER - In Idb", 0x2, 1},
    { "CELL_ASSEM_BUFFER2", "CELL_ASSEM_BUFFER - In Idb", 0x2, 2},
    { "CELL_ASSEM_BUFFER3", "CELL_ASSEM_BUFFER - In Idb", 0x2, 3},
    { "CELL_ASSEM_BUFFER4", "CELL_ASSEM_BUFFER - In Idb", 0x2, 4},
    { "CELL_ASSEM_BUFFER5", "CELL_ASSEM_BUFFER - In Idb", 0x2, 5},
    { "CELL_ASSEM_BUFFER6", "CELL_ASSEM_BUFFER - In Idb", 0x2, 6},
    { "CELL_ASSEM_BUFFER7", "CELL_ASSEM_BUFFER - In Idb", 0x2, 7},
    { "CPU_CELL_ASSEM_BUFFER", "CPU_CELL_ASSEM_BUFFER - In Idb", 0x3, 0xff},
    { "LPBK_CELL_ASSEM_BUFFER", "LPBK_CELL_ASSEM_BUFFER - In Idb", 0x4, 0xff},
    /* Non-IDB */
    { "IP_PACKET_BUFFER", "INGRESS_PACKET_BUFFER - In Ipars", 0x7, 0xff},
    { "ISW3_EOP_BUFFER_A", "ISW3_EOP_BUFFER_A - In Isw3", 0x8, 0xff},
    { "ISW3_EOP_BUFFER_B", "ISW3_EOP_BUFFER_B - In Isw3", 0x9, 0xff},
    { "BUBBLE_MOP", "BUBBLE_MOP - In Isw2", 0xa, 0},
    { "IPARS_IVP_PT_BUS_PARITY_EN", "IPARS_IVP_PT_BUS - In Ivp, Passthru Highway bus", 0xb, 0xff},
    { "IVP_ISW1_PT_BUS_PARITY_EN", "IVP_ISW1_PT_BUS - In Isw1, Passthru Highway bus", 0xc, 0xff},
    { "IFWD_ISW1_PT_BUS_PARITY_EN", "IFWD_ISW1_PT_BUS - In Isw1, Passthru Highway bus", 0xd, 0xff},
    { "ISW1_ISW2_PT_BUS_PARITY_EN", "ISW1_ISW2_PT_BUS - In Isw2, Passthru Highway bus", 0xf, 0x14},
    { "NONE", "ICFG_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x10, 0xff},
    { "IPARS_BUS_PARITY_EN", "IPARS_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x11, 0xff},
    { "IVXLT_BUS_PARITY_EN", "IVXLT_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x12, 0xff},
    { "VP_BUS_PAR_EN", "IVP_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x13, 0xff},
    { "IFWD_BUS_PARITY_EN", "IFWD_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x14, 0xff},
    { "IRSEL1_BUS_PARITY_EN", "IRSEL1_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x15, 0xff},
    { "ISW1_BUS_PARITY_EN", "ISW1_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x16, 0xff},
    /*
     * According to regsfile, membase of IFP_BUS should be 0x17,
     * however, in TH chip, when IFP_BUS is injected SER error,
     * the membase reported by H/W is 0xF.
     */
    { "IFP_BUS_PARITY_EN", "IFP_BUS - PassThrough bus parity check, MEMINDEX is 0", 0xf, 0x12},
    { "IRSEL2_BUS_PARITY_EN", "IRSEL2_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x18, 0xff},
    { "ISW2_BUS_PARITY_EN", "ISW2_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x19, 0xff},
    { "LEARN_FIFO", "LEARN_FIFO - In L2MM", 0x30, 0xff},
    { "CENTRAL_EVICTION_FIFO", "CENTRAL_EVICTION_FIFO - In CEV", 0x38, 0xff},
    /*
     * If IP_PACKET_BUFFER is injected SER error, The membase reported by H/W is 0x40.
     * Packet data ECC in Ipipe is not checked, so the checking and detection is in EL3 of Epipe.
     * IP_PACKET_BUFFER and EP_MPB_DATA can share the same control register.
     */
    { "IP_PACKET_BUFFER", "EP_MPB_DATA - In El3", 0x40, 0xff},
    { "EP_INITBUF_RAM_WRAPPER", "EP_INITBUF - In Ehcpm", 0x41, 0xff},
    { "EP_EDATABUF_CM_MEM", "CM_DATA_BUFFER - In Edatabuf", 0x42, 0xff},
    { "EP_EDATABUF_PM0_MEM", "PM0_DATA_BUFFER - In Edatabuf", 0x43, 0xff},
    { "EP_EDATABUF_PM1_MEM", "PM1_DATA_BUFFER - In Edatabuf", 0x44, 0xff},
    { "EP_EDATABUF_PM2_MEM", "PM2_DATA_BUFFER - In Edatabuf", 0x45, 0xff},
    { "EP_EDATABUF_PM3_MEM", "PM3_DATA_BUFFER - In Edatabuf", 0x46, 0xff},
    { "EP_EDATABUF_PM4_MEM", "PM4_DATA_BUFFER - In Edatabuf", 0x47, 0xff},
    { "EP_EDATABUF_PM5_MEM", "PM5_DATA_BUFFER - In Edatabuf", 0x48, 0xff},
    { "EP_EDATABUF_PM6_MEM", "PM6_DATA_BUFFER - In Edatabuf", 0x49, 0xff},
    { "EP_EDATABUF_PM7_MEM", "PM7_DATA_BUFFER - In Edatabuf", 0x4a, 0xff},
    { "EP_EDATABUF_LBP_MEM", "LBP_DATA_BUFFER - In Edatabuf", 0x4b, 0xff},
    { "EP_EDATABUF_PM_RESI_BUFFER", "PM_RESI0_DATA_BUFFER - In Edatabuf", 0x4c, 0xff},
    { "EP_EDATABUF_PM_RESI_BUFFER", "PM_RESI1_DATA_BUFFER - In Edatabuf", 0x4d, 0xff},
    { "EP_EDATABUF_CM_CTRL", "CM_EDB_CTRL_BUFFER - In Edatabuf", 0x50, 0xff},
    { "EP_EDATABUF_PM0_CTRL", "PM0_EDB_CTRL_BUFFER - In Edatabuf", 0x51, 0xff},
    { "EP_EDATABUF_PM1_CTRL", "PM1_EDB_CTRL_BUFFER - In Edatabuf", 0x52, 0xff},
    { "EP_EDATABUF_PM2_CTRL", "PM2_EDB_CTRL_BUFFER - In Edatabuf", 0x53, 0xff},
    { "EP_EDATABUF_PM3_CTRL", "PM3_EDB_CTRL_BUFFER - In Edatabuf", 0x54, 0xff},
    { "EP_EDATABUF_PM4_CTRL", "PM4_EDB_CTRL_BUFFER - In Edatabuf", 0x55, 0xff},
    { "EP_EDATABUF_PM5_CTRL", "PM5_EDB_CTRL_BUFFER - In Edatabuf", 0x56, 0xff},
    { "EP_EDATABUF_PM6_CTRL", "PM6_EDB_CTRL_BUFFER - In Edatabuf", 0x57, 0xff},
    { "EP_EDATABUF_PM7_CTRL", "PM7_EDB_CTRL_BUFFER - In Edatabuf", 0x58, 0xff},
    { "EP_EDATABUF_LBP_CTRL", "LBP_EDB_CTRL_BUFFER - In Edatabuf", 0x59, 0xff},
    { "EVLAN_BUS_PARITY_EN", "EGR_VLAN_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x5a, 0xff},
    { "EHCPM_BUS_PARITY_EN", "EGR_HCPM_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x5b, 0xff},
    { "EPMOD_BUS_PARITY_EN", "EGR_PMOD_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x5c, 0xff},
    { "EFPPARS_BUS_PARITY_EN", "EGR_FPPARS_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x5d, 0xff},
    { "EFP_BUS_PARITY_EN", "EFP_BUS - PassThrough bus parity check, MEMINDEX is 0", 0x5e, 0xff},
    { "EP_PT_BUS_PARITY_EN", "EP_PT_BUS - In Esw, Passthru Highway bus", 0x5f, 0xff}
};

/* In order to clear AUX_ARB_CONTROLr the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t aux_arb_control_fld_lst[] = {
    {FP_REFRESH_ENABLEf, ASSIGN_ARRAY_VAL(0), 0},
};

/* In order to clear EFP_METER_CONTROLr the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t efp_meter_control_fld_lst[] = {
    {EFP_REFRESH_ENABLEf, ASSIGN_ARRAY_VAL(0), 0},
};

static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_clear_refresh_registers[] = {
     /* used to clear field FP_REFRESH_ENABLEf of  AUX_ARB_CONTROLr */
    {AUX_ARB_CONTROLr, 0, &aux_arb_control_fld_lst[0], 1, 0},
     /* used to clear field EFP_REFRESH_ENABLEf of  EFP_METER_CONTROLr */
    {EFP_METER_CONTROLr, 0, &efp_meter_control_fld_lst[0], 1, 0},
     /* used to clear field REFRESH_ENf of  MMU_GCFG_MISCCONFIGr */
    {MMU_GCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[4], 1, 0}
};

/*
* In order to read L2_ENTRY_BANK_CONFIGf, L3_ENTRY_BANK_CONFIGf,
* FPEM_ENTRY_BANK_CONFIGf and ALPM_ENTRY_BANK_CONFIGf of ISS_BANK_CONFIGr.
*/
static bcmptm_ser_fld_info_t iss_bank_config_fld_lst[] = {
    { L2_ENTRY_BANK_CONFIGf, ASSIGN_ARRAY_VAL(0), 1},
    { L3_ENTRY_BANK_CONFIGf, ASSIGN_ARRAY_VAL(0), 1},
    { FPEM_ENTRY_BANK_CONFIGf, ASSIGN_ARRAY_VAL(0), 1},
    { ALPM_ENTRY_BANK_CONFIGf, ASSIGN_ARRAY_VAL(0), 1},
};

static bcmptm_ser_ctrl_reg_info_t bcm56960_a0_iss_bank_config_reg[] = {
    { ISS_BANK_CONFIGr, 0, &iss_bank_config_fld_lst[0], 4, 0 },
};

static bcmptm_ser_acctype_info_t bcm56960_a0_acctype_info[] = {
    { ACC_TYPE_SINGLE , ACC_TYPE_SINGLE_STR },
    { ACC_TYPE_UNIQUE, ACC_TYPE_UNIQUE_STR },
    { ACC_TYPE_DUPLICATE, ACC_TYPE_DUPLICATE_STR},
    { ACC_TYPE_DATA_SPLIT, ACC_TYPE_DATA_SPLIT_STR},
    { ACC_TYPE_ADDR_SPLIT, ACC_TYPE_ADDR_SPLIT_STR},
    { ACC_TYPE_ADDR_SPLIT_SPLIT, ACC_TYPE_ADDR_SPLIT_SPLIT_STR},
    { 0, "unknown" }
};

static int
bcm56960_a0_ser_l2_share_bank_num_get(int unit)
{
    int l2_bank_num = 0;

    l2_bank_num = (int)iss_bank_config_fld_lst[0].val[unit];

    return l2_bank_num;
}

static void
bcm56960_a0_ser_l2_share_bank_num_set(int unit, int bank_num)
{
    iss_bank_config_fld_lst[0].val[unit] = bank_num;
}

static int
bcm56960_a0_ser_l3_share_bank_num_get(int unit)
{
    int l3_bank_num = 0;

    l3_bank_num = (int)iss_bank_config_fld_lst[1].val[unit];

    return l3_bank_num;
}

static void
bcm56960_a0_ser_l3_share_bank_num_set(int unit, int bank_num)
{
    iss_bank_config_fld_lst[1].val[unit] = bank_num;
}

static int
bcm56960_a0_ser_fpem_bank_num_get(int unit)
{
    int fpem_bank_num = 0;

    fpem_bank_num = (int)iss_bank_config_fld_lst[2].val[unit];

    return fpem_bank_num;
}

static void
bcm56960_a0_ser_fpem_bank_num_set(int unit, int bank_num)
{
    iss_bank_config_fld_lst[2].val[unit] = bank_num;
}

static int
bcm56960_a0_ser_alpm_bank_num_get(int unit)
{
    int alpm_bank_num = 0;

    alpm_bank_num = (int)iss_bank_config_fld_lst[3].val[unit];

    return alpm_bank_num;
}

static void
bcm56960_a0_ser_alpm_bank_num_set(int unit, int bank_num)
{
    iss_bank_config_fld_lst[3].val[unit] = bank_num;
}

static int
bcm56960_a0_ser_iss_bank_config_get(int unit,
                                bcmptm_ser_ctrl_reg_info_t **reg_config_lst,
                                int *reg_num)
{
    *reg_config_lst = bcm56960_a0_iss_bank_config_reg;
    *reg_num = COUNTOF(bcm56960_a0_iss_bank_config_reg);

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_uft_mem_resize(int unit, const char *group_id_str, int bank_num)
{
    /* refer to bcm56960_a0_DEVICE_EM_GROUP.map.ltl */
    const char *group_name[] = {
        "L2_HASH_GROUP",
        "L3_HASH_GROUP",
        "FP_EM_HASH_GROUP",
        "ALPM_LEVEL2_GROUP"
    };
    int group_id = 0, group_num = COUNTOF(group_name);

    if (bank_num < 0 || bank_num > 4) {
        return SHR_E_PARAM;
    }
    for (group_id = 0; group_id < group_num; group_id++) {
        if (sal_strcmp(group_id_str, group_name[group_id]) == 0) {
            break;
        }
    }
    switch(group_id) {
        case 0:
            bcm56960_a0_ser_l2_share_bank_num_set(unit, bank_num);
            break;
        case 1:
            bcm56960_a0_ser_l3_share_bank_num_set(unit, bank_num);
            break;
        case 2:
            bcm56960_a0_ser_fpem_bank_num_set(unit, bank_num);
            break;
        case 3:
            bcm56960_a0_ser_alpm_bank_num_set(unit, bank_num);
            break;
        default:
            return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_mem_clr_list_get(int unit,
                                       bcmptm_ser_mem_clr_list_t **mem_list,
                                       int *entry_num)
{
    *mem_list = bcm56960_a0_mem_clr_lst;
    *entry_num = COUNTOF(bcm56960_a0_mem_clr_lst);

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_clear_mem_before_ser_enable(int unit,
                                            bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                            int *reg_num)
{
    *reg_ctrl_lst = bcm56960_a0_clear_mem_before_ser_enable_info;
    *reg_num = COUNTOF(bcm56960_a0_clear_mem_before_ser_enable_info);
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_clear_mem_after_ser_enable(int unit,
                                           bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                           int *reg_num)
{
    *reg_ctrl_lst = bcm56960_a0_clear_mem_after_ser_enable_info;
    *reg_num = COUNTOF(bcm56960_a0_clear_mem_after_ser_enable_info);

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_l2_mgmt_ser_intr_enable(int unit,
                                        bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                        int *reg_num, int *intr_num)
{
    *reg_ctrl_lst = bcm56960_a0_enable_l2_mgmt_intr_info;
    *reg_num = COUNTOF(bcm56960_a0_enable_l2_mgmt_intr_info);

    *intr_num = L2_MGMT_TO_CMIC_INTR;

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_idb_scan_ctrl_reg_lst_get(int unit,
                                       bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                       int *reg_num)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56960_a0_ser_acc_type_map_lst_get(int unit,
                                     bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                     int *reg_num)
{
    *reg_ctrl_lst = bcm56960_a0_ser_acc_type_map_lst;
    *reg_num = COUNTOF(bcm56960_a0_ser_acc_type_map_lst);

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_tcam_ser_info_get(int unit,
                                  bcmptm_ser_tcam_generic_info_t **tcam_ser_info,
                                  int *tcam_info_num)
{
    *tcam_ser_info = bcm56960_a0_tcam_ser_info;
    *tcam_info_num = COUNTOF(bcm56960_a0_tcam_ser_info);

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_mem_mute_list_get(int unit,
                                  bcmdrd_sid_t **mem_ser_mute_list,
                                  int *mem_ser_mute_num)
{
    *mem_ser_mute_list = mem_ser_mute;
    *mem_ser_mute_num = COUNTOF(mem_ser_mute);

    return SHR_E_NONE;
}


static int
bcm56960_a0_ser_route_info_get(int unit,
                               bcmptm_ser_top_level_route_info_t **ser_route_blocks,
                               int *route_num)
{
    *ser_route_blocks = bcm56960_a0_ser_route_blocks;
    *route_num = COUNTOF(bcm56960_a0_ser_route_blocks);

    *route_num -= 1;

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_mmu_intr_map_get(int unit,
                                 uint32_t *cmic_lvl_intr_num,
                                 bcmptm_ser_mmu_intr_mapping_t **ser_mmu_intr_map,
                                 int *intr_map_num)
{
    *cmic_lvl_intr_num = MMU_TO_CMIC_MEMFAIL_INTR;
    *ser_mmu_intr_map = bcm56960_a0_mmu_intr_map;
    *intr_map_num = COUNTOF(bcm56960_a0_mmu_intr_map);

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_blk_type_map(int unit,
                             bcmptm_ser_blk_type_t ser_logical_type,
                             int *blk_type)
{
    /*
         * value of BLKTYPE_* is different between different chips,
         * but value of BCMPTM_SER_BLK is shared by all chips.
         */
    uint32_t blk_type_mapping[][2] = {
        { BCMPTM_SER_BLK_EPIPE, BLKTYPE_EPIPE },
        { BCMPTM_SER_BLK_IPIPE, BLKTYPE_IPIPE },
        { BCMPTM_SER_BLK_IPIPE_IDB, BLKTYPE_IPIPE },
        { BCMPTM_SER_BLK_MMU_GLB, BLKTYPE_MMU_GLB },
        { BCMPTM_SER_BLK_MMU_XPE, BLKTYPE_MMU_XPE },
        { BCMPTM_SER_BLK_MMU_SC,  BLKTYPE_MMU_SC },
        { BCMPTM_SER_BLK_CLPORT, BLKTYPE_CLPORT },
        { BCMPTM_SER_BLK_XLPORT, BLKTYPE_XLPORT },
    };
    int count = COUNTOF(blk_type_mapping);
    int i = 0;

    for (i = 0; i < count; i++) {
        if (ser_logical_type == blk_type_mapping[i][0])
        {
            *blk_type = blk_type_mapping[i][1];
            return SHR_E_NONE;

        }
    }
    *blk_type = -1;
    return SHR_E_NOT_FOUND;
}

static int
bcm56960_a0_ser_mmu_intr_check(int unit, int reg_id, int offset)
{
    bcmptm_ser_top_level_route_info_t *top_route_blocks = bcm56960_a0_ser_route_blocks;
    int i = 0, blocks_num = COUNTOF(bcm56960_a0_ser_route_blocks);
    int mmu_block = FALSE;

    for (i = 0; i < blocks_num; i++) {
        if (top_route_blocks[i].blocktype == BCMPTM_SER_BLK_MMU) {
            if ((reg_id == top_route_blocks[i].cmic_reg) &&
                (top_route_blocks[i].cmic_bit & (1 << offset))) {
                mmu_block = TRUE;
            }
            break;
        }
    }
    return mmu_block;
}

static void
bcm56960_a0_ser_cmic_intr_enable(int unit, int cmc, int reg_id,
                                 int offset, int enable)
{
    uint32_t intr_num = reg_id * 32 + offset;
    int mmu_block = FALSE;
    bcmptm_ser_mmu_intr_mapping_t *intr_map = bcm56960_a0_mmu_intr_map;
    int i = 0, intr_map_num = COUNTOF(bcm56960_a0_mmu_intr_map);

    mmu_block = bcm56960_a0_ser_mmu_intr_check(unit, reg_id, offset);

    if (mmu_block) {
        /* enable MMU_GLB/MMU_XPE/MMU_SC sub-blocks */
        for (i = 0; i < intr_map_num; i++) {
            if (enable) {
                (void)bcmbd_mmu_intr_enable(unit, intr_map[i].mmu_intr_num);
            } else {
                (void)bcmbd_mmu_intr_disable(unit, intr_map[i].mmu_intr_num);
            }
        }
    } else {
        if (enable) {
            bcmbd_cmicd_intr_enable(unit, 0, intr_num);
        } else {
            bcmbd_cmicd_intr_disable_deferred(unit, 0, intr_num);
        }
    }
}

static int
bcm56960_a0_ser_cmic_intr_func_set(int unit, int cmc, unsigned int intr_num,
                                   bcmbd_ser_intr_f intr_func,
                                   bcmbd_ser_intr_f mmu_intr_func)
{
    int mmu_block = FALSE;
    bcmptm_ser_mmu_intr_mapping_t *intr_map = bcm56960_a0_mmu_intr_map;
    int i = 0, intr_map_num = COUNTOF(bcm56960_a0_mmu_intr_map);
    int rv = SHR_E_NONE;

    mmu_block = bcm56960_a0_ser_mmu_intr_check(unit, intr_num / 32, intr_num % 32);
    if (mmu_block) {
        /* Register interrupt callback routines for MMU_GLB/MMU_XPE/MMU_SC sub-blocks */
        for (i = 0; i < intr_map_num; i++) {
            rv = bcmbd_mmu_intr_func_set(unit, intr_map[i].mmu_intr_num,
                                         (bcmbd_intr_f)mmu_intr_func,
                                         intr_map[i].mmu_intr_num);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
        return rv;
    } else {
        return bcmbd_cmicd_intr_func_set(unit, cmc, intr_num,
                                         (bcmbd_intr_f)intr_func, intr_num);
    }
}

static int
bcm56960_a0_ser_cmic_ser_engine_regs_get(int unit,
                                         bcmdrd_sid_t **range_cfg_list,
                                         bcmdrd_sid_t **range_start_list,
                                         bcmdrd_sid_t **range_end_list,
                                         bcmdrd_sid_t **range_result_list,
                                         bcmdrd_sid_t **prot_list,
                                         bcmdrd_sid_t **range_addr_bits_list,
                                         bcmdrd_sid_t **range_disable_list,
                                         int *group_num,
                                         bcmdrd_fid_t  **range_cfg_fid_list,
                                         bcmdrd_fid_t  **prot_fid_list,
                                         bcmdrd_sid_t  *ser_memory_sid)
{
    *range_cfg_list = bcm56960_a0_range_cfg;
    *range_start_list = bcm56960_a0_range_start;
    *range_end_list = bcm56960_a0_range_end;
    *range_result_list = bcm56960_a0_range_result;
    *prot_list = bcm56960_a0_prot_word;
    *range_addr_bits_list = bcm56960_a0_range_addr_bits;
    *range_disable_list = bcm56960_a0_range_disable;
    *group_num = COUNTOF(bcm56960_a0_range_cfg);
    *ser_memory_sid = SER_MEMORYm;
    *range_cfg_fid_list = bcm56960_a0_range_cfg_flds;
    *prot_fid_list = bcm56960_a0_range_prot_flds;

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_cmic_ser_engine_enable(int unit,
                                       bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                       int *reg_num, int *intr_num)
{
    *reg_ctrl_lst = bcm56960_a0_cmic_ser_engine_reg_lst;
    *reg_num = COUNTOF(bcm56960_a0_cmic_ser_engine_reg_lst);

    *intr_num = CMICD_IRQ_SER_INTR;
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_cmic_ser_engine_update(int unit,
                                       uint32_t bit_offset, int enable)
{
    int i;
    int fld_num = COUNTOF(ser_range_enable_fld_lst);
    uint32_t ser_range_en_val = 0;

    if (bit_offset >= 32) {
        return SHR_E_PARAM;
    }
    for (i = 0; i < fld_num; i++) {
        if (ser_range_enable_fld_lst[i].ctrl_fld == SER_RANGE_ENABLEf) {
            ser_range_en_val = ser_range_enable_fld_lst[i].val[unit];
            break;
        }
    }
    if (i >= fld_num) {
        return SHR_E_CONFIG;
    }
    if (enable == 0) {
        ser_range_en_val &= ~(1 << bit_offset);
    } else {
        ser_range_en_val |= 1 << bit_offset;
    }
    ser_range_enable_fld_lst[i].val[unit] = ser_range_en_val;
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_cmic_ser_result_get(int unit,
                                    bcmptm_ser_ctrl_reg_info_t **ser_error_lst,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_0,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_1,
                                    int *err_num)
{
    *ser_error_lst = bcm56960_a0_ser_error;
    *err_num = COUNTOF(bcm56960_a0_ser_error);

    *ser_result_0 = &bcm56960_a0_ser_result[0];
    *ser_result_1 = &bcm56960_a0_ser_result[1];

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_cmicd_adext_get(int unit, bcmptm_cmic_adext_info_t adext_info,
                                uint32_t *adext)
{
    BCMBD_CMICD_ADEXT_ACCTYPE_SET(*adext, adext_info.acc_type);
    BCMBD_CMICD_ADEXT_BLOCK_SET(*adext, adext_info.blk_num);
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_acc_type_get(int unit, bcmdrd_sid_t sid, uint32_t *acc_type,
                             const char **acctype_str)
{
    int rv = SHR_E_NONE;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t blkacc = 0, acctype_tmp = 0;
    int i = 0, acctype_num = COUNTOF(bcm56960_a0_acctype_info);
    bcmptm_ser_acctype_info_t *acctype_info = bcm56960_a0_acctype_info;

    rv = bcmdrd_pt_info_get(unit, sid, sinfo);
    if (SHR_FAILURE(rv)) {
        return SHR_E_PARAM;
    }

    blkacc = sinfo->blktypes;
    acctype_tmp = BCMBD_CMICD_BLKACC2ACCTYPE(blkacc);

    *acc_type = acctype_tmp;
    for (i = 0; i < acctype_num; i++) {
        if (acctype_info[i].acctype == acctype_tmp) {
            *acctype_str = acctype_info[i].acctype_name;
            break;
        }
    }
    /* last one is "unknown" */
    if (i == acctype_num) {
        *acctype_str = acctype_info[i - 1].acctype_name;
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_cmicd_schan_opcode_get(int unit, int cmd, int blk_num,
                                       int blk_types, int tbl_inst, int data_len,
                                       int dma, uint32_t ignore_mask,
                                       uint32_t *opcode)
{
    uint32_t oc = 0;
    int cmd_map[][2] = {
        { BCMPTM_SER_READ_MEMORY_CMD_MSG, READ_MEMORY_CMD_MSG },
        { BCMPTM_SER_READ_REGISTER_CMD_MSG, READ_REGISTER_CMD_MSG },
        { BCMPTM_SER_WRITE_MEMORY_CMD_MSG, WRITE_MEMORY_CMD_MSG },
        { BCMPTM_SER_WRITE_REGISTER_CMD_MSG, WRITE_REGISTER_CMD_MSG }
    };
    int i = 0, map_num = COUNTOF(cmd_map), acc_type = 0;
    const bcmdrd_chip_info_t *cinfo = NULL;
    int blktype = -1, blkinst = 0, rv = SHR_E_NONE;

    if (tbl_inst > 3) {
        return SHR_E_PARAM;
    }
    for (i = 0; i < map_num; i++) {
        if (cmd_map[i][0] == cmd) {
            break;
        }
    }
    if (i == map_num) {
        return SHR_E_PARAM;
    }
    acc_type = BCMBD_CMICD_BLKACC2ACCTYPE(blk_types);

    if(acc_type == ACC_TYPE_UNIQUE) {
        if (tbl_inst >= 0) {
            acc_type = tbl_inst;
        } else {
            return SHR_E_PARAM;
        }
    }
    cinfo = bcmdrd_dev_chip_info_get(unit);
    rv = bcmdrd_chip_block_type(cinfo, blk_num, &blktype, &blkinst);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }
    if ((blktype == BLKTYPE_EPIPE || blktype == BLKTYPE_IPIPE) &&
        acc_type == ACC_TYPE_DUPLICATE && tbl_inst >= 0) {
        acc_type = tbl_inst;
    }
    BCMPTM_SER_OPCODE_SET(oc, cmd_map[i][1]);
    BCMPTM_SER_DSTBLK_SET(oc, blk_num);
    BCMPTM_SER_ACCTYPE_SET(oc, acc_type);
    BCMPTM_SER_DATALEN_SET(oc, data_len);
    BCMPTM_SER_DMA_SET(oc, dma);
    BCMPTM_SER_BANK_SET(oc, ignore_mask);
    *opcode = oc;

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_mac_err_fid_check(int unit, bcmdrd_fid_t err_fid,
                                  int *is_double_err_fid)
{
    bcmdrd_fid_t single_bit_err_fid[] = {
                                            TX_CDC_SINGLE_BIT_ERRf,
                                            RX_CDC_SINGLE_BIT_ERRf
                                        };
    bcmdrd_fid_t double_bit_err_fid[] = {
                                            TX_CDC_DOUBLE_BIT_ERRf,
                                            RX_CDC_DOUBLE_BIT_ERRf
                                        };
    int i = 0;
    int single_fid_num = COUNTOF(single_bit_err_fid);
    int double_fid_num = COUNTOF(double_bit_err_fid);

    for (i = 0; i < single_fid_num; i++) {
        if (err_fid == single_bit_err_fid[i]) {
            *is_double_err_fid = 0;
            return SHR_E_NONE;
        }
    }
    for (i = 0; i < double_fid_num; i++) {
        if (err_fid == double_bit_err_fid[i]) {
            *is_double_err_fid = 1;
            return SHR_E_NONE;
        }
    }
    return SHR_E_NOT_FOUND;
}


static int
bcm56960_a0_ser_ing_egr_fifo_get(int unit, bcmdrd_sid_t *ing_fifo,
                                 bcmdrd_sid_t *egr_fifo,
                                 bcmdrd_sid_t *egr_intr_statr,
                                 bcmdrd_sid_t *l2_mgmt_fifo,
                                 bcmdrd_sid_t *l2_mgmt_intr,
                                 bcmdrd_fid_t **fid_list,
                                 int *fid_num,
                                 bcmdrd_fid_t *l2_mgmt_fifo_not_empty,
                                 bcmdrd_sid_t *l2_mod_fifo)
{
    *ing_fifo = ING_SER_FIFOm;
    *egr_fifo = EGR_SER_FIFOm;
    *l2_mgmt_fifo = L2_MGMT_SER_FIFOm;

    *egr_intr_statr = EGR_INTR_STATUSr;
    *l2_mgmt_intr = L2_MGMT_INTRr;
    *l2_mgmt_fifo_not_empty = SER_FIFO_NOT_EMPTYf;

    *fid_list = bcm56960_a0_ser_ing_egr_fifo_fld;
    *fid_num = COUNTOF(bcm56960_a0_ser_ing_egr_fifo_fld);
    *l2_mod_fifo = L2_MOD_FIFOm;
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_hwmem_base_info_get(int unit, int membase, int stage_id,
                                    char **bd_bb_name, char **bb_name)
{
    int base_num = COUNTOF(bcm56960_a0_ser_hwmem_base_info), i = 0;
    bcmptm_ser_buffer_bus_info_t *hwmem_info = bcm56960_a0_ser_hwmem_base_info;

    for (i = 0; i < base_num; i++) {
        if (hwmem_info[i].membase == membase) {
            if (hwmem_info[i].stage_id == 0xff ||
                hwmem_info[i].stage_id == stage_id) {
                *bd_bb_name = hwmem_info[i].bd_name;
                *bb_name = hwmem_info[i].membase_info;
                return SHR_E_NONE;
            }
        }
    }
    return SHR_E_NOT_FOUND;
}

static void
bcm56960_a0_ser_bus_addr_translate(int unit, int membase, int address, int *non_sbus)
{
    /* H/W will reports address of HG_TRUNK_GROUPm, when ISW1_ISW2_PT_BUS has SER error. */
    if (membase != 0xF) {
        return;
    }
    /* Address range of HG_TRUNK_GROUPm */
    if (address < 0x503c0000 || address > 0x503c003f) {
        return;
    }
    *non_sbus = 1;

    return;
}

static uint32_t
bcm56960_a0_ser_flex_ctr_index_remap(int unit, uint32_t address, int blk_type,
                                     uint32_t stage, uint32_t index)
{
    uint32_t index2 = index;
    if ((BLKTYPE_IPIPE == blk_type) && (21 == stage)) {
        switch (address & 0xfffff000) {
            /* pool 0 */
            case 0x56800000:
            case 0x56801000:
            case 0x56802000:
            case 0x56803000:

            case 0x56804000:
            case 0x56808000:
            case 0x5680c000:
            case 0x56810000:

            /* pool 1 */
            case 0x56820000:
            case 0x56821000:
            case 0x56822000:
            case 0x56823000:

            case 0x56824000:
            case 0x56828000:
            case 0x5682c000:
            case 0x56830000:

            /* pool 2 */
            case 0x56840000:
            case 0x56841000:
            case 0x56842000:
            case 0x56843000:

            case 0x56844000:
            case 0x56848000:
            case 0x5684c000:
            case 0x56850000:

            /* pool 3 */
            case 0x56880000:
            case 0x56881000:
            case 0x56882000:
            case 0x56883000:

            case 0x56884000:
            case 0x56888000:
            case 0x5688c000:
            case 0x56890000:

            /* pool 4 */
            case 0x568a0000:
            case 0x568a1000:
            case 0x568a2000:
            case 0x568a3000:

            case 0x568a4000:
            case 0x568a8000:
            case 0x568ac000:
            case 0x568b0000:
                index2 = address & 0xfff;
                break; /* 4K depth */
            default:
                break;
        }
    } else if ((BLKTYPE_EPIPE == blk_type) && (10 == stage)) {
        switch (address & 0xfffff000) {
            /* pool 0 */
            case 0x2a800000:
            case 0x2a801000:
            case 0x2a802000:
            case 0x2a803000:

            case 0x2a804000:
            case 0x2a808000:
            case 0x2a80c000:
            case 0x2a810000:
                index2 = address & 0xfff;
                break; /* 4K depth */
            default:
                break;
        }
    }
    return index2;
}

/* JIRA - TH-3787
* if we decode mem as _PIPE0, error could be in _PIPE0,1
* if we decode mem as _PIPE2, error could be in _PIPE2,3
*
* blocks => xpe, acc_type => duplicate,
* base_type => epipe
*/
static int
bcm56960_a0_ser_mmu_wred_mem_remap(int unit, bcmptm_ser_correct_info_t *spci)
{
    bcmdrd_sid_t wred_pipe0_sid_mapping[][2] = {
        {MMU_WRED_CONFIG_MMU_XPE0_EPIPE0m, MMU_WRED_CONFIG_MMU_XPE0_EPIPE1m},
        {MMU_WRED_CONFIG_MMU_XPE2_EPIPE0m, MMU_WRED_CONFIG_MMU_XPE2_EPIPE1m},
        {MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE0_EPIPE0m, MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE0_EPIPE1m},
        {MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE2_EPIPE0m, MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE2_EPIPE1m},
        {MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE0_EPIPE0m, MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE0_EPIPE1m},
        {MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE2_EPIPE0m, MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE2_EPIPE1m},
        {MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE0_EPIPE0m, MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE0_EPIPE1m},
        {MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE2_EPIPE0m, MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE2_EPIPE1m},
        {MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE0_EPIPE0m, MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE0_EPIPE1m},
        {MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE2_EPIPE0m, MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE2_EPIPE1m}
    };

    bcmdrd_sid_t wred_pipe2_sid_mapping[][2] = {
        {MMU_WRED_CONFIG_MMU_XPE1_EPIPE2m, MMU_WRED_CONFIG_MMU_XPE1_EPIPE3m},
        {MMU_WRED_CONFIG_MMU_XPE3_EPIPE2m, MMU_WRED_CONFIG_MMU_XPE3_EPIPE3m},
        {MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE1_EPIPE2m, MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE1_EPIPE3m},
        {MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE3_EPIPE2m, MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE3_EPIPE3m},
        {MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE1_EPIPE2m, MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE1_EPIPE3m},
        {MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE3_EPIPE2m, MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE3_EPIPE3m},
        {MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE1_EPIPE2m, MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE1_EPIPE3m},
        {MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE3_EPIPE2m, MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE3_EPIPE3m},
        {MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE1_EPIPE2m, MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE1_EPIPE3m},
        {MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE3_EPIPE2m, MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE3_EPIPE3m},
    };
    int wred_pipe0_mapping_num = COUNTOF(wred_pipe0_sid_mapping);
    int wred_pipe2_mapping_num = COUNTOF(wred_pipe2_sid_mapping);
    int pipe0_idx = 0, pipe2_idx = 0;

    for (pipe0_idx = 0; pipe0_idx < wred_pipe0_mapping_num; pipe0_idx++) {
        if (spci->sid == wred_pipe0_sid_mapping[pipe0_idx][0]) {
            spci->second_sid = wred_pipe0_sid_mapping[pipe0_idx][1];
            spci->second_addr = spci->addr & TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            spci->second_addr |= TH_MMU_ADDR_SEG1;
            break;
        }
    }
    for (pipe2_idx = 0; pipe2_idx < wred_pipe2_mapping_num; pipe2_idx++) {
        if (spci->sid == wred_pipe2_sid_mapping[pipe2_idx][0]) {
            spci->second_sid = wred_pipe2_sid_mapping[pipe2_idx][1];
            spci->second_addr = spci->addr & TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            spci->second_addr |= TH_MMU_ADDR_SEG3;
            break;
        }
    }
    if (pipe0_idx >= wred_pipe0_mapping_num && pipe2_idx >= wred_pipe2_mapping_num) {
        /* not found */
        spci->second_sid = INVALIDm;
        return SHR_E_NONE;
    }
    spci->second_index = spci->index;
    spci->second_tbl_inst = spci->tbl_inst;

    return SHR_E_NONE;
}

/* LHS views are physical views of mem instances which encounter error and
 * are thus reported by HW.
 *
 * RHS views are logical views for which SW maintains cache and does
 * ser_correction writes.
 *
 * remap function thus maps physical mem views to logical mem views.
 */
static int
bcm56960_a0_ser_mmu_phy2log_mem_remap(int unit, bcmptm_ser_correct_info_t *spci)
{
    bcmdrd_sid_t phy_to_log_mapping[][2] = {
        /* WRED */
        {MMU_WRED_DROP_CURVE_PROFILE_0_Am, MMU_WRED_DROP_CURVE_PROFILE_0m},
        {MMU_WRED_DROP_CURVE_PROFILE_0_Bm, MMU_WRED_DROP_CURVE_PROFILE_0m},
        {MMU_WRED_DROP_CURVE_PROFILE_1_Am, MMU_WRED_DROP_CURVE_PROFILE_1m},
        {MMU_WRED_DROP_CURVE_PROFILE_1_Bm, MMU_WRED_DROP_CURVE_PROFILE_1m},
        {MMU_WRED_DROP_CURVE_PROFILE_2_Am, MMU_WRED_DROP_CURVE_PROFILE_2m},
        {MMU_WRED_DROP_CURVE_PROFILE_2_Bm, MMU_WRED_DROP_CURVE_PROFILE_2m},
        {MMU_WRED_DROP_CURVE_PROFILE_3_Am, MMU_WRED_DROP_CURVE_PROFILE_3m},
        {MMU_WRED_DROP_CURVE_PROFILE_3_Bm, MMU_WRED_DROP_CURVE_PROFILE_3m},
        {MMU_WRED_DROP_CURVE_PROFILE_4_Am, MMU_WRED_DROP_CURVE_PROFILE_4m},
        {MMU_WRED_DROP_CURVE_PROFILE_4_Bm, MMU_WRED_DROP_CURVE_PROFILE_4m},
        {MMU_WRED_DROP_CURVE_PROFILE_5_Am, MMU_WRED_DROP_CURVE_PROFILE_5m},
        {MMU_WRED_DROP_CURVE_PROFILE_5_Bm, MMU_WRED_DROP_CURVE_PROFILE_5m},
        {MMU_WRED_DROP_CURVE_PROFILE_6_Am, MMU_WRED_DROP_CURVE_PROFILE_6m},
        {MMU_WRED_DROP_CURVE_PROFILE_6_Bm, MMU_WRED_DROP_CURVE_PROFILE_6m},
        {MMU_WRED_DROP_CURVE_PROFILE_7_Am, MMU_WRED_DROP_CURVE_PROFILE_7m},
        {MMU_WRED_DROP_CURVE_PROFILE_7_Bm, MMU_WRED_DROP_CURVE_PROFILE_7m},
        {MMU_WRED_DROP_CURVE_PROFILE_8_Am, MMU_WRED_DROP_CURVE_PROFILE_8m},
        {MMU_WRED_DROP_CURVE_PROFILE_8_Bm, MMU_WRED_DROP_CURVE_PROFILE_8m},
        /* THDU */
        {MMU_THDU_RESUME_PORT0m, MMU_THDU_RESUME_PORTm},
        {MMU_THDU_RESUME_PORT1m, MMU_THDU_RESUME_PORTm},
        {MMU_THDU_RESUME_PORT2m, MMU_THDU_RESUME_PORTm},

        {MMU_THDU_CONFIG_PORT0m, MMU_THDU_CONFIG_PORTm},
        {MMU_THDU_CONFIG_PORT1m, MMU_THDU_CONFIG_PORTm},

        {MMU_THDU_CONFIG_QGROUP0m, MMU_THDU_CONFIG_QGROUPm},
        {MMU_THDU_CONFIG_QGROUP1m, MMU_THDU_CONFIG_QGROUPm},

        {MMU_THDU_OFFSET_QGROUP0m, MMU_THDU_OFFSET_QGROUPm},
        {MMU_THDU_OFFSET_QGROUP1m, MMU_THDU_OFFSET_QGROUPm},

        {MMU_THDU_CONFIG_QUEUE0m, MMU_THDU_CONFIG_QUEUEm},
        {MMU_THDU_CONFIG_QUEUE1m, MMU_THDU_CONFIG_QUEUEm},

        {MMU_THDU_OFFSET_QUEUE0m, MMU_THDU_OFFSET_QUEUEm},
        {MMU_THDU_OFFSET_QUEUE1m, MMU_THDU_OFFSET_QUEUEm},

        {MMU_THDU_Q_TO_QGRP_MAP0m, MMU_THDU_Q_TO_QGRP_MAPm},
        {MMU_THDU_Q_TO_QGRP_MAP1m, MMU_THDU_Q_TO_QGRP_MAPm},
        {MMU_THDU_Q_TO_QGRP_MAP2m, MMU_THDU_Q_TO_QGRP_MAPm},
        /* THDI */
        {THDI_PORT_SP_CONFIG0m, THDI_PORT_SP_CONFIGm},
        {THDI_PORT_SP_CONFIG1m, THDI_PORT_SP_CONFIGm},
        {THDI_PORT_SP_CONFIG2m, THDI_PORT_SP_CONFIGm},
        /* MTRO */
        {MMU_MTRO_EGRMETERINGCONFIG_MEM_Am, MMU_MTRO_EGRMETERINGCONFIG_MEMm},
        {MMU_MTRO_EGRMETERINGCONFIG_MEM_Bm, MMU_MTRO_EGRMETERINGCONFIG_MEMm},
        /* THDM */
        {MMU_THDM_MCQE_QUEUE_CONFIG_Am, MMU_THDM_MCQE_QUEUE_CONFIGm},
        {MMU_THDM_MCQE_QUEUE_CONFIG_Bm, MMU_THDM_MCQE_QUEUE_CONFIGm},
        {MMU_THDM_MCQE_QUEUE_CONFIG_Cm, MMU_THDM_MCQE_QUEUE_CONFIGm},

        {MMU_THDM_DB_QUEUE_CONFIG_Am, MMU_THDM_DB_QUEUE_CONFIGm},
        {MMU_THDM_DB_QUEUE_CONFIG_Bm, MMU_THDM_DB_QUEUE_CONFIGm},
        {MMU_THDM_DB_QUEUE_CONFIG_Cm, MMU_THDM_DB_QUEUE_CONFIGm},

        {MMU_THDM_MCQE_PORTSP_CONFIG_Am, MMU_THDM_MCQE_PORTSP_CONFIGm},
        {MMU_THDM_MCQE_PORTSP_CONFIG_Bm, MMU_THDM_MCQE_PORTSP_CONFIGm},
        {MMU_THDM_MCQE_PORTSP_CONFIG_Cm, MMU_THDM_MCQE_PORTSP_CONFIGm},

        {MMU_THDM_DB_PORTSP_CONFIG_Am, MMU_THDM_DB_PORTSP_CONFIGm},
        {MMU_THDM_DB_PORTSP_CONFIG_Bm, MMU_THDM_DB_PORTSP_CONFIGm},
        {MMU_THDM_DB_PORTSP_CONFIG_Cm, MMU_THDM_DB_PORTSP_CONFIGm},

        {MMU_THDM_MCQE_QUEUE_OFFSET_Am, MMU_THDM_MCQE_QUEUE_OFFSETm},
        {MMU_THDM_MCQE_QUEUE_OFFSET_Bm, MMU_THDM_MCQE_QUEUE_OFFSETm},
        {MMU_THDM_MCQE_QUEUE_OFFSET_Cm, MMU_THDM_MCQE_QUEUE_OFFSETm},

        {MMU_THDM_DB_QUEUE_OFFSET_Am, MMU_THDM_DB_QUEUE_OFFSETm},
        {MMU_THDM_DB_QUEUE_OFFSET_Bm, MMU_THDM_DB_QUEUE_OFFSETm},
        {MMU_THDM_DB_QUEUE_OFFSET_Cm, MMU_THDM_DB_QUEUE_OFFSETm},
         /* RQE */
        {MMU_REPL_GROUP_INITIAL_COPY_COUNT0m, MMU_REPL_GROUP_INITIAL_COPY_COUNTm},
        {MMU_REPL_GROUP_INITIAL_COPY_COUNT1m, MMU_REPL_GROUP_INITIAL_COPY_COUNTm}
    };
    int num_mapping = COUNTOF(phy_to_log_mapping), i = 0;

    for (i = 0; i < num_mapping; i++) {
        if (spci->sid == phy_to_log_mapping[i][0]) {
            spci->reported_sid = spci->sid;
            spci->sid = phy_to_log_mapping[i][1];
            return SHR_E_NONE;
        }
    }
    spci->reported_sid = INVALIDm;
    return SHR_E_NONE;
}
/* block type is MMU_XPE, base type is MMU_XPE, access-type is unique */
static int
bcm56960_a0_ser_mmu_xpe_mem_remap(int unit, bcmptm_ser_correct_info_t *spci,
                                  uint32_t adext, int blk_inst)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    int blk_type, base_type, acc_type;
    const bcmdrd_symbols_t *symbols;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t adext_unique = 0;
    bcmptm_cmic_adext_info_t adext_info;

    base_type = ((spci->addr) >> 23) & 0x7;
    if ((base_type == BT_XPE) &&
        (blk_inst >= MMU_XPE1 && blk_inst <= MMU_XPE3)) {
        spci->addr &= TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;

        adext_info.acc_type = blk_inst;
        adext_info.blk_num = BCMBD_CMICD_ADEXT2BLOCK(adext);
        (void)bcm56960_a0_ser_cmicd_adext_get(unit, adext_info, &adext_unique);

        rv = bcmbd_pt_addr_decode(unit, adext_unique, spci->addr,
                                  BCMDRD_SYMBOL_FLAG_MEMORY, &sid, &dyn_info);
        if(SHR_FAILURE(rv)) {
            return rv;
        }
        symbols = bcmdrd_dev_symbols_get(unit, 0);
        if (bcmdrd_sym_info_get(symbols, sid, sinfo) == NULL) {
            return SHR_E_INTERNAL;
        }
        blk_type = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
        acc_type = bcmdrd_pt_acctype_get(unit, sinfo->sid);

        if ((blk_type == BLKTYPE_MMU_XPE) && (acc_type >= MMU_XPE1) && (acc_type <= MMU_XPE3)) {
            spci->tbl_inst = blk_inst;
            spci->index = dyn_info.index;
            spci->sid = sid;
        } else {
            return SHR_E_INTERNAL;
        }
    } else {
        return SHR_E_INTERNAL;
    }
    return SHR_E_NONE;
}

/* block type is MMU_SC, base type is MMU_XPE, access-type is single */
static int
bcm56960_a0_ser_mmu_sc_mem_remap(int unit, bcmptm_ser_correct_info_t *spci,
                                 uint32_t adext, int blk_inst)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    int blk_type, base_type, acc_type;
    const bcmdrd_symbols_t *symbols;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_info_get(symbols, spci->sid, sinfo) == NULL) {
        return SHR_E_INTERNAL;
    }
    base_type = ((sinfo->offset) >> 23) & 0x7;
    blk_type = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
    acc_type = bcmdrd_pt_acctype_get(unit, sinfo->sid);

    if ((blk_type == BLKTYPE_MMU_SC) && (base_type == BT_XPE) &&
        (acc_type == ACC_TYPE_SINGLE)) {
        if (blk_inst == MMU_SC1) {
            spci->addr &= TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            spci->addr |= TH_MMU_ADDR_SEG1;
        } else {
            spci->addr &= TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            spci->addr |= TH_MMU_ADDR_SEG0;
        }

        rv = bcmbd_pt_addr_decode(unit, adext, spci->addr,
                                  BCMDRD_SYMBOL_FLAG_MEMORY, &sid, &dyn_info);
        if(SHR_FAILURE(rv)) {
            return rv;
        }
        spci->tbl_inst = dyn_info.tbl_inst;
        spci->index = dyn_info.index;
        spci->sid = sid;

        if (blk_inst == MMU_SC1) {
            spci->second_addr = spci->addr & TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            spci->second_addr |= TH_MMU_ADDR_SEG3;
        } else {
            spci->second_addr = spci->addr & TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            spci->second_addr |= TH_MMU_ADDR_SEG2;
        }

        rv = bcmbd_pt_addr_decode(unit, adext, spci->second_addr,
                                  BCMDRD_SYMBOL_FLAG_MEMORY, &sid, &dyn_info);
        if(SHR_FAILURE(rv)) {
            return rv;
        }
        spci->second_tbl_inst = dyn_info.tbl_inst;
        spci->second_index = dyn_info.index;
        spci->second_sid = sid;
    }

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_mmu_mem_remap(int unit, bcmptm_ser_correct_info_t *spci,
                              uint32_t adext, int blk_inst)
{
    int rv = SHR_E_NONE;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;

    /* JIRA - TH-3797
    * seg_id reported is always 0 even when error is for
    * instances in XPE1, XPE3
    * Thus if we decode the mem as _SC0 and blocks are
    * XPE1 or XPE3 then we should make correction to _SC1
    *
    * For this mem:
    * blocks => xpe, acc_type => duplicate,
    * base_type => slice
    */
    if ((spci->sid == MMU_REPL_GROUP_INITIAL_COPY_COUNT0m ||
         spci->sid == MMU_REPL_GROUP_INITIAL_COPY_COUNT1m)) {
        if (blk_inst == MMU_XPE1 || blk_inst == MMU_XPE3) {
            spci->addr &= TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            spci->addr |= TH_MMU_ADDR_SEG1;
            rv = bcmbd_pt_addr_decode(unit, adext, spci->addr,
                                      BCMDRD_SYMBOL_FLAG_MEMORY, &sid, &dyn_info);
            if(SHR_FAILURE(rv)) {
                return rv;
            }
            spci->tbl_inst = dyn_info.tbl_inst;
            spci->index = dyn_info.index;
            spci->sid = sid;
            spci->second_sid = INVALIDm;
        }
    } else if (spci->sid == INVALIDm) {
        rv = bcm56960_a0_ser_mmu_xpe_mem_remap(unit, spci, adext, blk_inst);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    } else {
        rv = bcm56960_a0_ser_mmu_wred_mem_remap(unit, spci);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        rv = bcm56960_a0_ser_mmu_sc_mem_remap(unit, spci, adext, blk_inst);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    rv = bcm56960_a0_ser_mmu_phy2log_mem_remap(unit, spci);
    return rv;
}

/* PTcache and WAL just support L3_TUNNEL_DATA_ONLYm and L3_TUNNEL_ONLYm */
static int
bcm56960_a0_ser_ip_ep_sram_remap(int unit, bcmdrd_sid_t sid, bcmdrd_sid_t *remap_sid)
{
    bcmdrd_sid_t mapping[][2] = {
        { L3_TUNNELm, L3_TUNNEL_DATA_ONLYm }
    };
    int i = 0, num_mapping = COUNTOF(mapping);

    *remap_sid = sid;
    for (i = 0; i < num_mapping; i++) {
        if (sid == mapping[i][0]) {
            *remap_sid = mapping[i][1];
            break;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_ing_egr_sbs_control_get(int unit, bcmdrd_sid_t *ing_sbs_control,
                                        bcmdrd_sid_t *egr_sbs_control, bcmdrd_fid_t *pipe_select_fid)
{
    bcmdrd_sid_t sid[2] = {SBS_CONTROLr, EGR_SBS_CONTROLr};
    *ing_sbs_control = sid[0];
    *egr_sbs_control = sid[1];
    *pipe_select_fid = PIPE_SELECTf;
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_data_split_mem_test_field_get(int unit, bcmdrd_sid_t sid,
                                              int acc_type, bcmdrd_fid_t *test_fid)
{
    uint32_t mem_test_fields[][3] = {
        /* mem with acc_type=data_split, has separate parity field for each pipe */
        { EGR_VLAN_STGm, 0, EVEN_EGR_VLAN_STG_PARITY_P0f },
        { EGR_VLAN_STGm, 1, EVEN_EGR_VLAN_STG_PARITY_P1f },
        { EGR_VLAN_STGm, 2, EVEN_EGR_VLAN_STG_PARITY_P2f },
        { EGR_VLAN_STGm, 3, EVEN_EGR_VLAN_STG_PARITY_P3f },
        /* mem with acc_type=data_split, has separate parity field for each pipe */
        { SRC_MODID_INGRESS_BLOCKm, 0, EVEN_PBM_PARITY_P0f },
        { SRC_MODID_INGRESS_BLOCKm, 1, EVEN_PBM_PARITY_P1f },
        { SRC_MODID_INGRESS_BLOCKm, 2, EVEN_PBM_PARITY_P2f },
        { SRC_MODID_INGRESS_BLOCKm, 3, EVEN_PBM_PARITY_P3f },
        /* mem with acc_type=data_split, has separate parity field for each pipe */
        { VLAN_TABm, 0, EVEN_PBM_PARITY_P0f },
        { VLAN_TABm, 1, EVEN_PBM_PARITY_P1f },
        { VLAN_TABm, 2, EVEN_PBM_PARITY_P2f },
        { VLAN_TABm, 3, EVEN_PBM_PARITY_P3f },
        /* mem with acc_type=data_split, has separate parity field for each pipe */
        { EGR_VLANm, 0, EVEN_PARITY_P0f },
        { EGR_VLANm, 1, EVEN_PARITY_P1f },
        { EGR_VLANm, 2, EVEN_PARITY_P2f },
        { EGR_VLANm, 3, EVEN_PARITY_P3f },
        /* mem with acc_type=data_split, has separate parity field for each pipe */
        {L3_TUNNEL_DATA_ONLYm, 0, EVEN_PBM_PARITY_P0f},
        {L3_TUNNEL_DATA_ONLYm, 1, EVEN_PBM_PARITY_P1f},
        {L3_TUNNEL_DATA_ONLYm, 2, EVEN_PBM_PARITY_P2f},
        {L3_TUNNEL_DATA_ONLYm, 3, EVEN_PBM_PARITY_P3f}

    };
    int num = COUNTOF(mem_test_fields), i = 0;
    uint32_t acc_type_u = (acc_type < 0) ? 0 : acc_type;

    for (i = 0; i < num; i++) {
        if (mem_test_fields[i][0] == sid &&
            mem_test_fields[i][1] == acc_type_u) {
            *test_fid = mem_test_fields[i][2];
            return SHR_E_NONE;
        }
    }
    return SHR_E_NOT_FOUND;
}

static int
bcm56960_a0_ser_alpm_info_remap(int unit, bcmdrd_sid_t sid_fv,
                                int index_fv, bcmdrd_sid_t *sid,
                                int *index, int *ecc_checking)
{
    bcmdrd_sid_t l3_defip_alpm_views[] = {
        L3_DEFIP_ALPM_IPV4m,
        L3_DEFIP_ALPM_IPV4_1m,
        L3_DEFIP_ALPM_IPV6_64m,
        L3_DEFIP_ALPM_IPV6_64_1m,
        L3_DEFIP_ALPM_IPV6_128m,
        L3_DEFIP_ALPM_RAWm
    };
    int index_map_ipv4[] = { 0, 1, 1, 2, 3, 3 };
    int index_map_ipv6_64_1[] = { 0, 2, 3 };
    int num = COUNTOF(l3_defip_alpm_views), i = 0;
    int bank_num, bucket_mask, bit_shift, offset_in_bucket_fv;
    int offset_in_bucket_ecc, bucket_bank;

    for (i = 0; i < num; i++) {
        if (sid_fv == l3_defip_alpm_views[i]) {
            break;
        }
    }
    if (i >= num) {
        return SHR_E_NOT_FOUND;
    }
    if (sid == NULL || index == NULL) {
        return SHR_E_NONE;
    }
    bank_num = bcm56960_a0_ser_alpm_bank_num_get(unit);
    if (bank_num == 0) {
        return SHR_E_PARAM;
    }
    /* just for TH */
    bit_shift = (bank_num == 4) ? 15 : 14;
    bucket_mask = (1 << bit_shift) - 1;
    bucket_bank = index_fv & bucket_mask;
    offset_in_bucket_fv = index_fv >> bit_shift;

    if (sid_fv == L3_DEFIP_ALPM_IPV4m) { /* 6:4 */
        /* 6 entries per backet */
        if (offset_in_bucket_fv >= 6) {
            return SHR_E_PARAM;
        }
        /* 0 -> 0; 1 -> 1; 2 -> 1; 3 -> 2; 4 -> 3; 5 -> 3 */
        offset_in_bucket_ecc = index_map_ipv4[offset_in_bucket_fv];
    } else if (sid_fv == L3_DEFIP_ALPM_IPV4_1m ||
               sid_fv == L3_DEFIP_ALPM_IPV6_64m) { /* 1:1 */
        /* 4 entries per backet */
        if (offset_in_bucket_fv >= 4) {
            return SHR_E_PARAM;
        }
        /* 0 -> 0; 1 -> 1; 2 -> 2; 3 -> 3 */
        offset_in_bucket_ecc = offset_in_bucket_fv;
    } else if (sid_fv == L3_DEFIP_ALPM_IPV6_64_1m) { /* 3:4 */
        /* 3 entries per backet */
        if (offset_in_bucket_fv >= 3) {
            return SHR_E_PARAM;
        }
        /* 0 -> 0; 1 -> 1; 2 -> 2 */
        offset_in_bucket_ecc = index_map_ipv6_64_1[offset_in_bucket_fv];
    } else if (sid_fv == L3_DEFIP_ALPM_IPV6_128m) {
        /* 2 entries per backet */
        if (offset_in_bucket_fv >= 2) {
            return SHR_E_PARAM;
        }
        /* 0 -> 0; 1 -> 2 */
        offset_in_bucket_ecc = offset_in_bucket_fv * 2;
    } else if (sid_fv == L3_DEFIP_ALPM_RAWm) {
        /* 1 entries per backet */
        if (offset_in_bucket_fv >= 1) {
            return SHR_E_PARAM;
        }
        /* 0 -> 0; */
        offset_in_bucket_ecc = 0;
    } else {
        return SHR_E_PARAM;
    }
    *ecc_checking = 1;
    *sid = L3_DEFIP_ALPM_ECCm;
    *index = (offset_in_bucket_ecc << bit_shift) + bucket_bank;
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_alpm_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                              bcmptm_ser_sram_info_t *sram_info)
{
    int bank_num = 0;
    uint32_t bucket_mask, bit_shift;
    uint32_t base_bucket, bank, bank_mask, bank_shift, bkt_offset;
    uint32_t i = 0, ram_count = 4, bucket_num_per_ram = 2048;
    uint32_t offset_in_bucket = 0, offset_mask;

    bank_num = bcm56960_a0_ser_alpm_bank_num_get(unit);
    if (bank_num == 0) {
        return SHR_E_INTERNAL;
    }
    /* just for TH */
    bit_shift = (bank_num == 2) ? 14 : 15;
    bank_mask = (bank_num == 2) ? 0x1 : 0x3;
    bank_shift = (bank_num == 2) ? 0x1 : 0x2;
    offset_mask = (bank_num == 2) ? TH_ALPM_MODE1_BKT_OFFSET_MASK :
                  TH_ALPM_MODE0_BKT_OFFSET_MASK;

    bucket_mask = (1 << bit_shift) - 1;
    base_bucket = (index & bucket_mask) >> bank_shift;
    bank = index & bank_mask;
    base_bucket = base_bucket % bucket_num_per_ram;
    offset_in_bucket = index & offset_mask;
    sram_info->ram_count = ram_count;
    for (i = 0; i < ram_count; i++) {
        sram_info->pt_view[i] = sid;
        sram_info->index_count[i] = 1;
        bkt_offset = base_bucket + bucket_num_per_ram * i;
        sram_info->indexes[i][0] = offset_in_bucket |
                                   (bkt_offset << bank_shift) | bank;
    }
    sram_info->hw_recovery = FALSE;
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_mem_ecc_info_get(int unit, bcmdrd_sid_t sid_fv, int index_fv,
                                 bcmdrd_sid_t *sid, int *index, int *ecc_checking)
{
    uint32_t mem_map_info[][3] = {
        { VLAN_XLATEm,  VLAN_XLATE_ECCm, 1 }, /* 1 : 1 */
        { VLAN_MACm, VLAN_XLATE_ECCm, 1 }, /* 1 : 1 */
        { EGR_VLAN_XLATEm, EGR_VLAN_XLATE_ECCm, 1 }, /* 1 : 1 */
        { MPLS_ENTRYm, MPLS_ENTRY_ECCm, 1 }, /* 1 : 1 */
        { L2Xm, L2_ENTRY_ONLY_ECCm, 1 }, /* 1 : 1 */
        { L2_ENTRY_ONLY_TILEm, L2_ENTRY_ONLY_ECCm, 1 }, /* 1 : 1 */
        { L2_ENTRY_TILEm, L2_ENTRY_ONLY_ECCm, 1 }, /* 1 : 1 */
        { L3_ENTRY_ONLYm, L3_ENTRY_ONLY_ECCm, 1 }, /* 1 : 1 */
        { L3_ENTRY_IPV4_UNICASTm, L3_ENTRY_ONLY_ECCm, 1 }, /* 1 : 1 */
        { L3_ENTRY_IPV4_MULTICASTm, L3_ENTRY_ONLY_ECCm, 2 }, /* 1 : 2 */
        { L3_ENTRY_IPV6_UNICASTm, L3_ENTRY_ONLY_ECCm, 2 }, /* 1 : 2 */
        { L3_ENTRY_IPV6_MULTICASTm, L3_ENTRY_ONLY_ECCm, 4 }, /* 1 : 4 */
        { EXACT_MATCH_2m, FPEM_ECCm, 2 }, /* 1 : 2 */
        { EXACT_MATCH_2_ENTRY_ONLYm, FPEM_ECCm, 2 }, /* 1 : 2 */
        { EXACT_MATCH_4m, FPEM_ECCm, 4 }, /* 1 : 4 */
        { EXACT_MATCH_4_ENTRY_ONLYm, FPEM_ECCm, 4 } /* 1 : 4 */
    };
    int num = COUNTOF(mem_map_info), i = 0;
    int rv = SHR_E_NONE;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;

    if (ecc_checking == NULL) {
        return SHR_E_PARAM;
    }
    rv = bcm56960_a0_ser_alpm_info_remap(unit, sid_fv, index_fv,
                                         sid, index, ecc_checking);
    if (rv != SHR_E_NOT_FOUND) {
        return rv;
    }
    if (sid != NULL && index != NULL) {
        *sid = sid_fv;
        *index = index_fv;
    }
    *ecc_checking = 0;

    for (i = 0; i < num; i++) {
        if (mem_map_info[i][0] == sid_fv) {
            *ecc_checking = 1;
            break;
        }
    }
    /* found */
    if (*ecc_checking == 1 && sid != NULL && index != NULL) {
        *sid = mem_map_info[i][1];
        *index = index_fv * mem_map_info[i][2];
    }
    if (*ecc_checking == 0) {
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_ECC1B, sid_fv, &ctrl_info);
        if (SHR_FAILURE(rv)) {
            *ecc_checking = 0;
        } else {
            *ecc_checking = 1;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_tcam_only_remap(int unit, bcmdrd_sid_t tcam_only_sid,
                                bcmdrd_sid_t *mapped_sid, int *inject_err_to_key_fld)
{
    /* TCAM_ONLY view, AGGR view */
    uint32_t mem_map_info[][2] = {
        /* SER engine scans TCAM_ONLY view, please refer to bcm56960_a0_tcam_ser_info */
        { L3_TUNNEL_ONLYm, L3_TUNNEL_ONLYm },
        /* SER engine scans AGGR view, please refer to bcm56960_a0_tcam_ser_info */
        { EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm, EXACT_MATCH_LOGICAL_TABLE_SELECTm },
        { IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm, IFP_LOGICAL_TABLE_SELECTm },
        { DST_COMPRESSION_TCAM_ONLYm, DST_COMPRESSIONm },
        { ING_SNAT_ONLYm, ING_SNATm },
        { L2_USER_ENTRY_ONLYm, L2_USER_ENTRYm },
        { L3_DEFIP_ONLYm, L3_DEFIPm },
        { L3_DEFIP_PAIR_128_ONLYm, L3_DEFIP_PAIR_128m },
        { MY_STATION_TCAM_ENTRY_ONLYm, MY_STATION_TCAMm },
        { SRC_COMPRESSION_TCAM_ONLYm, SRC_COMPRESSIONm },
        { DST_COMPRESSION_TCAM_ONLYm, DST_COMPRESSIONm },
        { VLAN_SUBNET_ONLYm, VLAN_SUBNETm }
    };
    int num = COUNTOF(mem_map_info), i = 0;

    *mapped_sid = tcam_only_sid;
    *inject_err_to_key_fld = 0;
    for (i = 0; i < num; i++) {
        if (mem_map_info[i][0] == tcam_only_sid) {
            *mapped_sid = mem_map_info[i][1];
            *inject_err_to_key_fld = 1;
            break;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_refresh_regs_info_get(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **mem_refresh_info,
                                      uint32_t *info_num)
{
    *mem_refresh_info = bcm56960_a0_clear_refresh_registers;
    *info_num = COUNTOF(bcm56960_a0_clear_refresh_registers);

    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_is_tdm_calendar_get(int unit,
                                    bcmdrd_sid_t idb_ecc_status_reg,
                                    uint32_t *entry_index, bcmdrd_sid_t *is_tdm_calendar)
{
    if (idb_ecc_status_reg != IDB_IS_TDM_CAL_ECC_STATUSr) {
        return SHR_E_NONE;
    }
    if (is_tdm_calendar == NULL) {
        return SHR_E_PARAM;
    }
    if (*entry_index & 0x80) {
        *is_tdm_calendar = IS_TDM_CALENDAR1m;
        *entry_index &= 0x7F;
    } else {
        *is_tdm_calendar = IS_TDM_CALENDAR0m;
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_fpem_lp_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                                 bcmptm_ser_sram_info_t *sram_info)
{
    uint32_t lp_entries_per_ram = 2048, lp_entries_per_bank = 8192;
    uint32_t fv_entries_per_bank = 0, bank_id = 0;
    uint32_t offset_fv = 0, offset_lp = 0, base = 0, i = 0;

    sram_info->ram_count = 4;
    base = index % lp_entries_per_ram;
    bank_id = index / lp_entries_per_bank;
    for (i = 0; i < sram_info->ram_count; i++) {
        sram_info->index_count[i] = 1;
        if (i < 2) {
            /* bank == 0, 1, 2, 3 */
            /* indexes ((8K * bank) + 0K) to ((8K * bank) + 4K-1) correspond to em2 type entries */
            /* each index in FPEM_LP corresponds to 4 consecutive em2 entries */
            fv_entries_per_bank = 16 * 1024;
            offset_fv = fv_entries_per_bank * bank_id;
            sram_info->pt_view[i] = EXACT_MATCH_2m;
            sram_info->indexes[i][0] = offset_fv + 4 * (base + i * lp_entries_per_ram);
        } else if (i == 2) {
            /* indexes ((8K * bank) + 4K) to ((8K * bank) + 6K-1) correspond to em4 type entries */
            /* each index in FPEM_LP corresponds to 4 consecutive em4 entries */
            fv_entries_per_bank = 8 * 1024;
            offset_fv = fv_entries_per_bank * bank_id;
            sram_info->pt_view[i] = EXACT_MATCH_4m;
            sram_info->indexes[i][0] = offset_fv + 4 * base;
        } else {
            /* indexes ((8K * bank) + 6K) to ((8K * bank) + 8K-1) of FPEM_LPm is not used */
            offset_lp = lp_entries_per_bank * bank_id;
            sram_info->pt_view[i] = sid;
            sram_info->indexes[i][0] = offset_lp + base + i * lp_entries_per_ram;
        }
    }
    sram_info->hw_recovery = FALSE;
    return SHR_E_NONE;
}

/* LPs use H/W recovery type to correct SER error. */
static int
bcm56960_a0_ser_lp_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                            bcmptm_ser_sram_info_t *sram_info)
{
    bcmdrd_sid_t lp_pts[] = {
        L2_ENTRY_ISS_LPm,
        L3_ENTRY_ISS_LPm,
        L2_ENTRY_LPm,
        L3_ENTRY_LPm,
        VLAN_XLATE_LPm,
        EGR_VLAN_XLATE_LPm
    };
    int i = 0, lp_pts_num = COUNTOF(lp_pts), found = 0;

    for(i = 0; i <lp_pts_num; i++) {
        if (sid == lp_pts[i]) {
            found = 1;
            break;
        }
    }
    if (found) {
        sram_info->ram_count = 1;
        sram_info->pt_view[0] = sid;
        sram_info->index_count[0] = 1;
        sram_info->indexes[0][0] = index;
        sram_info->hw_recovery = TRUE;
        return SHR_E_NONE;
    } else {
        return SHR_E_UNAVAIL;
    }
}

static int
bcm56960_a0_ser_mem_xor_lp_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                                    bcmptm_ser_sram_info_t *sram_info)
{
    /* sid_reported,  offset, entries_per_ram, ram_count, sid_corrected, continous */
    bcmptm_ser_sram_base_info_t sram_info_map[] = {
        /* dedicated L2 entries - hash table */
        { L2_ENTRY_ONLY_ECCm,  0,    8192, 1, L2_ENTRY_ONLY_ECCm, 0 },
        /* hash_xor mems, stride 8K */
        { L2_ENTRY_ONLY_ECCm,  8192, 8192, 4, L2_ENTRY_ONLY_ECCm, 0 },
        /* dedicated L2 entries - hash table */
        { L3_ENTRY_ONLY_ECCm,  0,    8192, 1, L3_ENTRY_ONLY_ECCm, 0 },
        /* hash_xor mems, stride 8K */
        { L3_ENTRY_ONLY_ECCm,  8192, 8192, 4, L3_ENTRY_ONLY_ECCm, 0 },
        /* hash_xor mems, stride 8K */
        { FPEM_ECCm,           0,    8192, 4, FPEM_ECCm, 0 },
        /* hash_xor mems, stride 8K, only one bank */
        { ING_L3_NEXT_HOPm,    0,    8192, 4, ING_L3_NEXT_HOPm, 0 },
        /* Contiguous index, "offset" and "entries_per_ram" is not used */
        /* hash_xor mems, stride 1 */
        { VLAN_XLATE_ECCm,     0,    0,    4, VLAN_XLATE_ECCm, 1 },
        /* hash_xor mems, stride 1 */
        { EGR_VLAN_XLATE_ECCm, 0,    0,    4, EGR_VLAN_XLATE_ECCm, 1 }
    };
    bcmptm_ser_sram_base_info_t    *base_info_ptr = NULL;
    int i = 0, info_map_num = 0, found = 0;
    uint32_t entries_num_per_bank = 0, stride = 0, ram_num = 0, j = 0;
    uint32_t offset = 0, base_index = 0, base = 0, continuous = 0;
    bcmdrd_sid_t sid_reported = INVALIDm, sid_corrected = INVALIDm;
    int rv = SHR_E_UNAVAIL;

    info_map_num = COUNTOF(sram_info_map);
    for (i = 0; i < info_map_num; i++) {
        base_info_ptr = &sram_info_map[i];
        sid_reported = base_info_ptr->sid_reported;
        if ((sid_reported == sid) && (base_info_ptr->offset <= index)) {
            found = 1;
            offset = base_info_ptr->offset;
            ram_num = base_info_ptr->ram_count;
            sid_corrected = base_info_ptr->sid_corrected;
            continuous = base_info_ptr->continous;
            stride = continuous ? 1 : base_info_ptr->entries_per_ram;
            /* not break, find the last one, for L2_ENTRY_ONLY_ECCm and L3_ENTRY_ONLY_ECCm */
            continue;
        }
    }
    if (found == 1) {
        if (continuous == 0) {
            /* stride is equal to entries_per_ram */
            entries_num_per_bank = stride * ram_num;
            base_index = index - offset;
            base = (base_index / entries_num_per_bank) * entries_num_per_bank;
            base = base + offset + base_index % stride;
        } else {
            base = (index / ram_num) * ram_num;
        }
        sram_info->ram_count = ram_num;
        sram_info->indexes[0][0] = base;
        sram_info->pt_view[0] = sid_corrected;
        sram_info->index_count[0] = 1;
        for (j = 1; j < sram_info->ram_count; j++) {
            sram_info->pt_view[j] = sid_corrected;
            sram_info->index_count[j] = 1;
            /* stride can be 1, also can be entries_per_ram */
            sram_info->indexes[j][0] = sram_info->indexes[j - 1][0] + stride;
        }
        sram_info->hw_recovery = FALSE;
        rv = SHR_E_NONE;
    } else if (L3_DEFIP_ALPM_ECCm == sid) {
        rv = bcm56960_a0_ser_alpm_info_get(unit, sid, index, sram_info);
    } else if (FPEM_LPm == sid) {
        rv = bcm56960_a0_ser_fpem_lp_info_get(unit, sid, index, sram_info);
    } else {
        rv = bcm56960_a0_ser_lp_info_get(unit, sid, index, sram_info);
    }
    return rv;
}

static int
bcm56960_a0_ser_uft_valid_index_check(int unit, bcmdrd_sid_t sid,
                                      uint32_t index, uint32_t *max_index,
                                      int *dedicated_bank)
{
    bcmptm_ser_uft_info_t check_sid_info[] = {
        /* L3_DEFIP_ALPM_ECCm */
        { L3_DEFIP_ALPM_ECCm, 0, 4, bcm56960_a0_ser_alpm_bank_num_get },
        { L3_DEFIP_ALPM_IPV4m, 0, 6, bcm56960_a0_ser_alpm_bank_num_get },
        { L3_DEFIP_ALPM_IPV4_1m, 0, 4, bcm56960_a0_ser_alpm_bank_num_get },
        { L3_DEFIP_ALPM_IPV6_64m, 0, 4, bcm56960_a0_ser_alpm_bank_num_get },
        { L3_DEFIP_ALPM_IPV6_64_1m, 0, 3, bcm56960_a0_ser_alpm_bank_num_get },
        { L3_DEFIP_ALPM_IPV6_128m, 0, 2, bcm56960_a0_ser_alpm_bank_num_get },
        { L3_DEFIP_ALPM_RAWm, 0, 1, bcm56960_a0_ser_alpm_bank_num_get },
         /* L2Xm */
        { L2Xm, 8192, 4, bcm56960_a0_ser_l2_share_bank_num_get },
        { L2_ENTRY_TILEm, 8192, 4, bcm56960_a0_ser_l2_share_bank_num_get },
        { L2_ENTRY_ONLY_TILEm, 8192, 4, bcm56960_a0_ser_l2_share_bank_num_get },
        { L2_ENTRY_ONLY_ECCm, 8192, 4, bcm56960_a0_ser_l2_share_bank_num_get },
        { L2_ENTRY_ISS_LPm, 0, 1, bcm56960_a0_ser_l2_share_bank_num_get },
        /* L3_ENTRY_ONLY_ECCm */
        { L3_ENTRY_ONLY_ECCm, 8192, 4, bcm56960_a0_ser_l3_share_bank_num_get },
        { L3_ENTRY_ONLYm, 8192, 4, bcm56960_a0_ser_l3_share_bank_num_get },
        { L3_ENTRY_IPV4_UNICASTm, 8192, 4, bcm56960_a0_ser_l3_share_bank_num_get },
        { L3_ENTRY_IPV4_MULTICASTm, 4096, 2, bcm56960_a0_ser_l3_share_bank_num_get },
        { L3_ENTRY_IPV6_UNICASTm, 4096, 2, bcm56960_a0_ser_l3_share_bank_num_get },
        { L3_ENTRY_IPV6_MULTICASTm, 2048, 1, bcm56960_a0_ser_l3_share_bank_num_get },
        { L3_ENTRY_ISS_LPm, 0, 1, bcm56960_a0_ser_l3_share_bank_num_get },
        /* FPEM_ECCm */
        { FPEM_ECCm, 0, 4, bcm56960_a0_ser_fpem_bank_num_get },
        { EXACT_MATCH_2m, 0, 2, bcm56960_a0_ser_fpem_bank_num_get },
        { EXACT_MATCH_2_ENTRY_ONLYm, 0, 2, bcm56960_a0_ser_fpem_bank_num_get },
        { EXACT_MATCH_4m, 0, 1, bcm56960_a0_ser_fpem_bank_num_get },
        { EXACT_MATCH_4_ENTRY_ONLYm, 0, 1, bcm56960_a0_ser_fpem_bank_num_get },
        { FPEM_LPm, 0, 1, bcm56960_a0_ser_fpem_bank_num_get }
    };
    int i = 0, sid_check_num = COUNTOF(check_sid_info), bank_num = 0;
    uint32_t bucket_num_per_bank = 0, entry_num_per_bucket = 0;
    uint32_t entry_num = 0, dedicated_entry_num = 0;

    for ( i = 0; i < sid_check_num; i++) {
        if (sid == check_sid_info[i].sid) {
            bank_num = check_sid_info[i].fn_bank_num_get(unit);
            entry_num_per_bucket = check_sid_info[i].entry_num_per_bucket;
            dedicated_entry_num = check_sid_info[i].dedicated_entry_num;
            entry_num = dedicated_entry_num;
            bucket_num_per_bank = 8192 * 4 / 4;
            entry_num += bucket_num_per_bank * bank_num * entry_num_per_bucket;
            *max_index = entry_num - 1;
            *dedicated_bank = 1;
            if (index >= dedicated_entry_num) {
                *dedicated_bank = 0;
            }
            if (index >= entry_num) {
                return SHR_E_CONFIG;
            } else {
                return SHR_E_NONE;
            }
        }
    }
    return SHR_E_UNAVAIL;
}

static int
bcm56960_a0_ser_sram_ctrl_register_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                                       bcmptm_ser_control_reg_info_t *sram_ctrl_reg_info)
{
    /* Disables PRE_MEMWR parity check. */
    uint32_t wr_check_reg_map[][3] = {
        { L2Xm, L2_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L2_ENTRY_ONLY_ECCm, L2_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L2_ENTRY_TILEm, L2_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L2_ENTRY_ONLY_TILEm, L2_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_ENTRY_ONLY_ECCm, L3_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_ENTRY_ONLYm, L3_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_ENTRY_IPV4_UNICASTm, L3_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_ENTRY_IPV4_MULTICASTm, L3_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_ENTRY_IPV6_UNICASTm, L3_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_ENTRY_IPV6_MULTICASTm, L3_ENTRY_PARITY_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        /* { FPEM_LPm, FPEM_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf }, */
        { FPEM_ECCm, FPEM_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { EXACT_MATCH_2m, FPEM_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { EXACT_MATCH_2_ENTRY_ONLYm, FPEM_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { EXACT_MATCH_4m, FPEM_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { EXACT_MATCH_4_ENTRY_ONLYm, FPEM_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_DEFIP_ALPM_ECCm, ILPM_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_DEFIP_ALPM_IPV4m, ILPM_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_DEFIP_ALPM_IPV4_1m, ILPM_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_DEFIP_ALPM_IPV6_64m, ILPM_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_DEFIP_ALPM_IPV6_64_1m, ILPM_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_DEFIP_ALPM_IPV6_128m, ILPM_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { L3_DEFIP_ALPM_RAWm, ILPM_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { EGR_VLAN_XLATEm, EGR_VLAN_XLATE_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { EGR_VLAN_XLATE_ECCm, EGR_VLAN_XLATE_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { VLAN_XLATEm, VLAN_XLATE_DBGCTRL_0r, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { VLAN_MACm, VLAN_XLATE_DBGCTRL_0r, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { VLAN_XLATE_ECCm, VLAN_XLATE_DBGCTRL_0r, DISABLE_SBUS_MEMWR_PARITY_CHECKf },
        { ING_L3_NEXT_HOPm, IRSEL2_SER_CONTROLr, DISABLE_SBUS_MEMWR_PARITY_CHECK_FOR_ING_L3_NEXT_HOPf }
    };
    /*
    * If there is SER error in XOR bank, and force register is enabled,
    * when we read corresponding table bank, the XOR SER error can be triggered.
    */
    uint32_t force_reg_map[][4] = {
        /* for dedicated banks, dedicated banks for L2 and L3 is SP banks but XOR banks */
        /*
        * { L2Xm, L2_ENTRY_CONTROL_6r, FORCE_XOR_GENERATIONf, 1 },
        * { L2_ENTRY_ONLY_ECCm, L2_ENTRY_CONTROL_6r, FORCE_XOR_GENERATIONf, 1 },
        * { L2_ENTRY_ONLY_TILEm, L2_ENTRY_CONTROL_6r, FORCE_XOR_GENERATIONf, 1 },
        * { L2_ENTRY_TILEm, L2_ENTRY_CONTROL_6r, FORCE_XOR_GENERATIONf, 1 },
        * { L3_ENTRY_ONLY_ECCm, L3_ENTRY_CONTROL_10r, FORCE_XOR_GENERATIONf, 1 },
        * { L3_ENTRY_ONLYm, L3_ENTRY_CONTROL_10r, FORCE_XOR_GENERATIONf, 1 },
        * { L3_ENTRY_IPV4_UNICASTm, L3_ENTRY_CONTROL_10r, FORCE_XOR_GENERATIONf, 1 },
        * { L3_ENTRY_IPV4_MULTICASTm, L3_ENTRY_CONTROL_10r, FORCE_XOR_GENERATIONf, 1 },
        * { L3_ENTRY_IPV6_UNICASTm, L3_ENTRY_CONTROL_10r, FORCE_XOR_GENERATIONf, 1 },
        * { L3_ENTRY_IPV6_MULTICASTm, L3_ENTRY_CONTROL_10r, FORCE_XOR_GENERATIONf, 1 },
        */
        { EGR_VLAN_XLATEm, EGR_VLAN_XLATE_CONTROLr, FORCE_XOR_GENf, 1 },
        { EGR_VLAN_XLATE_ECCm, EGR_VLAN_XLATE_CONTROLr, FORCE_XOR_GENf, 1 },
        { VLAN_XLATEm, VLAN_XLATE_DBGCTRL_0r, FORCE_XOR_GENf, 1 },
        { VLAN_MACm, VLAN_XLATE_DBGCTRL_0r, FORCE_XOR_GENf, 1 },
        { VLAN_XLATE_ECCm, VLAN_XLATE_DBGCTRL_0r, FORCE_XOR_GENf, 1 },
        { ING_L3_NEXT_HOPm, RSEL2_RAM_DBGCTRL5r, FORCE_XOR_GEN_NHOPf, 1 },
        /* for share banks */
        { L2Xm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L2_ENTRY_ONLY_ECCm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L2_ENTRY_ONLY_TILEm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L2_ENTRY_TILEm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_ENTRY_ONLY_ECCm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_ENTRY_ONLYm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_ENTRY_IPV4_UNICASTm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_ENTRY_IPV4_MULTICASTm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_ENTRY_IPV6_UNICASTm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_ENTRY_IPV6_MULTICASTm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { FPEM_ECCm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { EXACT_MATCH_2m, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { EXACT_MATCH_2_ENTRY_ONLYm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { EXACT_MATCH_4m, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { EXACT_MATCH_4_ENTRY_ONLYm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_DEFIP_ALPM_ECCm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_DEFIP_ALPM_IPV4m, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_DEFIP_ALPM_IPV4_1m, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_DEFIP_ALPM_IPV6_64m, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_DEFIP_ALPM_IPV6_64_1m, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_DEFIP_ALPM_IPV6_128m, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 },
        { L3_DEFIP_ALPM_RAWm, ISS_MEMORY_CONTROL_84r, FORCE_XOR_GENERATIONf, 0 }
    };
    /* Disables XOR bank write. Allows SW to inject parity error on Table Banks or XOR Bank. */
    uint32_t xor_write_reg_map[][4] = {
        /* for dedicated banks, dedicated banks for L2 and L3 is SP banks but XOR banks */
        /*
        * { L2Xm, L2_ENTRY_CONTROL_6r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L2_ENTRY_ONLY_ECCm, L2_ENTRY_CONTROL_6r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L2_ENTRY_ONLY_TILEm, L2_ENTRY_CONTROL_6r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L2_ENTRY_TILEm, L2_ENTRY_CONTROL_6r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L3_ENTRY_ONLY_ECCm, L3_ENTRY_CONTROL_10r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L3_ENTRY_ONLYm, L3_ENTRY_CONTROL_10r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L3_ENTRY_IPV4_UNICASTm, L3_ENTRY_CONTROL_10r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L3_ENTRY_IPV4_MULTICASTm, L3_ENTRY_CONTROL_10r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L3_ENTRY_IPV6_UNICASTm, L3_ENTRY_CONTROL_10r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        * { L3_ENTRY_IPV6_MULTICASTm, L3_ENTRY_CONTROL_10r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        */
        { EGR_VLAN_XLATEm, EGR_VLAN_XLATE_CONTROLr, DEBUG_DISABLE_XOR_WRITEf, 1 },
        { EGR_VLAN_XLATE_ECCm, EGR_VLAN_XLATE_CONTROLr, DEBUG_DISABLE_XOR_WRITEf, 1 },
        { VLAN_XLATEm, VLAN_XLATE_DBGCTRL_0r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        { VLAN_MACm, VLAN_XLATE_DBGCTRL_0r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        { VLAN_XLATE_ECCm, VLAN_XLATE_DBGCTRL_0r, DEBUG_DISABLE_XOR_WRITEf, 1 },
        { ING_L3_NEXT_HOPm, RSEL2_RAM_DBGCTRL5r, DEBUG_DISABLE_XOR_WRITE_NHOPf, 1 },
        /* for share banks */
        { L2Xm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L2_ENTRY_ONLY_ECCm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L2_ENTRY_ONLY_TILEm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L2_ENTRY_TILEm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_ENTRY_ONLY_ECCm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_ENTRY_ONLYm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_ENTRY_IPV4_UNICASTm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_ENTRY_IPV4_MULTICASTm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_ENTRY_IPV6_UNICASTm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_ENTRY_IPV6_MULTICASTm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { FPEM_ECCm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { EXACT_MATCH_2m, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { EXACT_MATCH_2_ENTRY_ONLYm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { EXACT_MATCH_4m, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { EXACT_MATCH_4_ENTRY_ONLYm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_DEFIP_ALPM_ECCm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_DEFIP_ALPM_IPV4m, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_DEFIP_ALPM_IPV4_1m, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_DEFIP_ALPM_IPV6_64m, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_DEFIP_ALPM_IPV6_64_1m, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_DEFIP_ALPM_IPV6_128m, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 },
        { L3_DEFIP_ALPM_RAWm, ISS_MEMORY_CONTROL_84r, DEBUG_DISABLE_XOR_WRITEf, 0 }
    };
    /* Extra controlling register */
    uint32_t extra_reg_map[][4] = {
        /* H/W freeze L2Xm */
        { L2Xm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_NEWf, 1 },
        { L2Xm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_MOVEf, 1 },
        { L2_ENTRY_ONLY_ECCm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_NEWf, 1 },
        { L2_ENTRY_ONLY_ECCm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_MOVEf, 1 },
        { L2_ENTRY_ONLY_TILEm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_NEWf, 1 },
        { L2_ENTRY_ONLY_TILEm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_MOVEf, 1 },
        { L2_ENTRY_TILEm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_NEWf, 1 },
        { L2_ENTRY_TILEm, ING_MISC_CONFIG2r, CML_OVERRIDE_ENABLE_MOVEf, 1 }
    };
    uint32_t i = 0, map_num, j;
    uint32_t max_index, dedicated = 1;
    int rv = SHR_E_NONE;

    /* initialize */
    sram_ctrl_reg_info->force_field = INVALIDf;
    sram_ctrl_reg_info->force_reg = INVALIDr;
    sram_ctrl_reg_info->wr_check_reg = INVALIDr;
    sram_ctrl_reg_info->wr_check_field = INVALIDf;
    sram_ctrl_reg_info->xor_wr_reg = INVALIDr;
    sram_ctrl_reg_info->xor_wr_field = INVALIDf;

    for (j = 0; j < BCMPTM_SER_EXTRA_REG_NUM; j++) {
        sram_ctrl_reg_info->extra_reg[j] = INVALIDr;
        sram_ctrl_reg_info->extra_field[j] = INVALIDf;
    }

    rv = bcm56960_a0_ser_uft_valid_index_check(unit, sid, index, &max_index, (int *)(&dedicated));
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
    }
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* find corresponding controlling regsters */
    map_num = COUNTOF(wr_check_reg_map);
    for (i = 0; i < map_num; i++) {
        if (sid == wr_check_reg_map[i][0]) {
            sram_ctrl_reg_info->wr_check_reg = wr_check_reg_map[i][1];
            sram_ctrl_reg_info->wr_check_field = wr_check_reg_map[i][2];
            break;
        }
    }
    map_num = COUNTOF(force_reg_map);
    for (i = 0; i < map_num; i++) {
        if (sid == force_reg_map[i][0] && dedicated == force_reg_map[i][3]) {
            sram_ctrl_reg_info->force_reg = force_reg_map[i][1];
            sram_ctrl_reg_info->force_field = force_reg_map[i][2];
            break;
        }
    }
    map_num = COUNTOF(xor_write_reg_map);
    for (i = 0; i < map_num; i++) {
        if (sid == xor_write_reg_map[i][0] && dedicated == xor_write_reg_map[i][3]) {
            sram_ctrl_reg_info->xor_wr_reg = xor_write_reg_map[i][1];
            sram_ctrl_reg_info->xor_wr_field = xor_write_reg_map[i][2];
            break;
        }
    }
    map_num = COUNTOF(extra_reg_map);
    for (i = 0, j = 0; j < BCMPTM_SER_EXTRA_REG_NUM && i < map_num; i++) {
        if (sid == extra_reg_map[i][0]) {
            sram_ctrl_reg_info->extra_reg[j] = extra_reg_map[i][1];
            sram_ctrl_reg_info->extra_field[j] = extra_reg_map[i][2];
            sram_ctrl_reg_info->extra_field_val[j] = extra_reg_map[i][3];
            j++;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56960_a0_ser_sram_scan_mem_skip(int unit, bcmdrd_sid_t sid, bool *skip)
{
    bcmdrd_sid_t sid_skip_list[] = {
        /* scan L2Xm */
        L2_ENTRY_ONLY_ECCm,
        L2_ENTRY_ONLY_TILEm,
        L2_ENTRY_TILEm,

        L2_BULKm,
        L2_LEARN_INSERT_FAILUREm,
        /* scan L3_ENTRY_ONLY_ECCm */
        L3_ENTRY_IPV4_UNICASTm,
        L3_ENTRY_IPV4_MULTICASTm,
        L3_ENTRY_IPV6_UNICASTm,
        L3_ENTRY_IPV6_MULTICASTm,
        L3_ENTRY_ONLYm,
        /* scan L3_DEFIP_ALPM_ECCm */
        L3_DEFIP_ALPM_IPV4m,
        L3_DEFIP_ALPM_IPV4_1m,
        L3_DEFIP_ALPM_IPV6_64m,
        L3_DEFIP_ALPM_IPV6_64_1m,
        L3_DEFIP_ALPM_IPV6_128m,
        L3_DEFIP_ALPM_RAWm,
        /* scan FPEM_ECCm */
        EXACT_MATCH_2m,
        EXACT_MATCH_4m,
        EXACT_MATCH_2_ENTRY_ONLYm,
        EXACT_MATCH_4_ENTRY_ONLYm,
        /* scan functional view */
        MPLS_ENTRY_ECCm,
        VLAN_XLATE_ECCm,
        EGR_VLAN_XLATE_ECCm,
        /*
        * scan DATA_ONLY view if has. If there is no SER protection for DATA_ONLY,
        * BCMPTM_SER_CONTROL_PT_SW_SCAN will return false.
        * All TCAM_ONLY view and AGGR view is not scanned by SRAM memory scan thread.
        * Because bcmdrd_pt_attr_is_cam(unit, sid) return TRUE for TCAM_ONLY view
        * and AGGR view.
        */
    };
    int i = 0, sid_num = COUNTOF(sid_skip_list);

    *skip = FALSE;
    for (i = 0; i < sid_num; i++) {
        if (sid_skip_list[i] == sid) {
            *skip = TRUE;
            break;
        }
    }
    return SHR_E_NONE;
}

static const bcmptm_ser_data_driver_t bcm56960_a0_data_driver = {

    .mem_clr_list_get_fn = bcm56960_a0_ser_mem_clr_list_get,

    .clear_mem_after_ser_enable_fn = bcm56960_a0_ser_clear_mem_after_ser_enable,

    .clear_mem_before_ser_enable_fn = bcm56960_a0_ser_clear_mem_before_ser_enable,

    .l2_mgmt_ser_enable_info_get_fn = bcm56960_a0_ser_l2_mgmt_ser_intr_enable,

    .tcam_idb_ser_scan_reg_info_get_fn = bcm56960_a0_ser_idb_scan_ctrl_reg_lst_get,

    .acc_type_map_info_get_fn = bcm56960_a0_ser_acc_type_map_lst_get,

    .tcam_info_get_fn = bcm56960_a0_ser_tcam_ser_info_get,

    .mute_mem_list_get_fn = bcm56960_a0_ser_mem_mute_list_get,

    .route_info_get_fn = bcm56960_a0_ser_route_info_get,

    .mmu_intr_map_get_fn = bcm56960_a0_ser_mmu_intr_map_get,

    .blk_type_map_fn = bcm56960_a0_ser_blk_type_map,

    .cmic_intr_enable_fn = bcm56960_a0_ser_cmic_intr_enable,

    .cmic_intr_func_set_fn = bcm56960_a0_ser_cmic_intr_func_set,

    .cmic_ser_engine_regs_get_fn = bcm56960_a0_ser_cmic_ser_engine_regs_get,

    .cmic_ser_engine_enable_fn = bcm56960_a0_ser_cmic_ser_engine_enable,

    .cmic_ser_engine_update_fn = bcm56960_a0_ser_cmic_ser_engine_update,

    .cmic_ser_result_get_fn = bcm56960_a0_ser_cmic_ser_result_get,

    .cmic_adext_get_fn = bcm56960_a0_ser_cmicd_adext_get,

    .pt_acctype_get_fn = bcm56960_a0_ser_acc_type_get,

    .cmic_schan_opcode_get_fn = bcm56960_a0_ser_cmicd_schan_opcode_get,

    .mac_err_fid_check_fn = bcm56960_a0_ser_mac_err_fid_check,

    .ing_egr_fifo_get_fn = bcm56960_a0_ser_ing_egr_fifo_get,

    .hwmem_base_info_get_fn = bcm56960_a0_ser_hwmem_base_info_get,

    .bus_addr_translate_fn = bcm56960_a0_ser_bus_addr_translate,

    .flex_ctr_index_remap_fn = bcm56960_a0_ser_flex_ctr_index_remap,

    .mmu_mem_remap_fn = bcm56960_a0_ser_mmu_mem_remap,

    .ip_ep_sram_remap_fn = bcm56960_a0_ser_ip_ep_sram_remap,

    .ing_egr_sbs_control_get_fn = bcm56960_a0_ser_ing_egr_sbs_control_get,

    .data_split_mem_test_fid_get_fn = bcm56960_a0_ser_data_split_mem_test_field_get,

    .mem_ecc_info_get_fn = bcm56960_a0_ser_mem_ecc_info_get,

    .tcam_only_remap_fn = bcm56960_a0_ser_tcam_only_remap,

    .refresh_regs_info_get_fn = bcm56960_a0_ser_refresh_regs_info_get,

    .is_tdm_calendar_sid_get_fn = bcm56960_a0_ser_is_tdm_calendar_get,

    .sram_ctrl_reg_get_fn = bcm56960_a0_ser_sram_ctrl_register_get,

    .xor_lp_mem_info_get_fn = bcm56960_a0_ser_mem_xor_lp_info_get,

    .uft_mem_index_check_fn = bcm56960_a0_ser_uft_valid_index_check,

    .iss_bank_config_info_get_fn = bcm56960_a0_ser_iss_bank_config_get,

    .uft_mem_resize_fn = bcm56960_a0_ser_uft_mem_resize,

    .sram_scan_mem_skip_fn = bcm56960_a0_ser_sram_scan_mem_skip
};

const bcmptm_ser_data_driver_t *
bcm56960_a0_ptm_ser_data_driver_register(int unit)
{
    return &bcm56960_a0_data_driver;
}

