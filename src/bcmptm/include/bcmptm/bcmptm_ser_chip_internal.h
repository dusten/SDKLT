/*! \file bcmptm_ser_chip_internal.h
 *
 * Interface for SER APIs defined in bcmptm_common.c
 * Those APIs only can be used in SER sub-component.
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

#ifndef BCMPTM_SER_CHIP_INTERNAL_H
#define BCMPTM_SER_CHIP_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <sal/sal_types.h>
#include <sal/sal_time.h>

/*******************************************************************************
 * Defines
 */
/*!\brief  string name of single access types */
#define ACC_TYPE_SINGLE_STR                "SINGLE"

/*! \brief  string name of unique access types */
#define ACC_TYPE_UNIQUE_STR                "UNIQUE"

/*!\brief  string name of duplicate access types */
#define ACC_TYPE_DUPLICATE_STR             "DUPLICATE"

/*!\brief  string name of data-split access types */
#define ACC_TYPE_DATA_SPLIT_STR            "DATA_SPLIT"

/*!\brief  string name of addr-split access types */
#define ACC_TYPE_ADDR_SPLIT_STR            "ADDR_SPLIT"

/*!\brief  string name of addr-split-split access types */
#define ACC_TYPE_ADDR_SPLIT_SPLIT_STR      "ADDR_SPLIT_SPLIT"

/*!\brief SINGLE access type */
#define PT_IS_SINGLE_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_SINGLE_STR, str) == 0)

/*!\brief UNIQUE access type */
#define PT_IS_UNIQUE_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_UNIQUE_STR, str) == 0)

/*!\brief DUPLICATE access type */
#define PT_IS_DUPLICATE_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_DUPLICATE_STR, str) == 0)

/*!\brief DATA-SPLIT access type */
#define PT_IS_DATA_SPLIT_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_DATA_SPLIT_STR, str) == 0)

/*!\brief ADDR-SPLIT access type */
#define PT_IS_ADDR_SPLIT_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_ADDR_SPLIT_STR, str) == 0)

/*!\brief ADDR-SPLIT-SPLIT access type */
#define PT_IS_ADDR_SPLIT_SPLIT_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_ADDR_SPLIT_SPLIT_STR, str) == 0)

/*!\brief HW multi-pipe table */
#define BCMPTM_SER_FLAG_MULTI_PIPE     0x00400

/*!\brief Read table with XY format */
#define BCMPTM_SER_FLAG_XY_READ        0x00800

/*!\brief Some tables have holes, so can't use dma to read them */
#define BCMPTM_SER_FLAG_NO_DMA         0x01000

/*!\brief Read remapped indexes */
#define BCMPTM_SER_FLAG_REMAP_READ     0x02000

/*!\brief Memories shared in HW */
#define BCMPTM_SER_FLAG_OVERLAY        0x04000

/*!\brief HW multi-pipe table */
#define BCMPTM_SER_FLAG_NARROW_VIEW    0x08000

/*!\brief HW multi-pipe table */
#define BCMPTM_SER_FLAG_WIDE_VIEW      0x10000

/*!\brief Unique pipe acc type mode */
#define BCMPTM_SER_FLAG_ACC_TYPE_CHK   0x20000

/*!\brief Use 'addr_mask' as range disable bitmap */
#define BCMPTM_SER_FLAG_RANGE_DISABLE  0x40000

/*!
 * \brief Create entry for this mem view but don't enable the SER engine range.
 * So, memscan will ignore such entry.
 */
#define BCMPTM_SER_FLAG_VIEW_DISABLE   0x80000

/*!\brief Memory protected by SER engine */
#define BCMPTM_SER_FLAG_SER_ENGINE     0x400000


/*!\brief SBUS acc type bits for pre-extended SBUS address format */
#define BCMPTM_MEM_ADDR_ACC_TYPE_MASK       0x7

/*!\brief access pipe 0 */
#define BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_0     0

/*!\brief access pipe 1 */
#define BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_1     1

/*!\brief access pipe 2 */
#define BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_2     2

/*!\brief access pipe 3 */
#define BCMPTM_MEM_ADDR_ACC_TYPE_PIPE_3     3

/*!\brief SBUS memory DMA read operation */
#define BCMPTM_SER_READ_MEMORY_CMD_MSG        1

/*!\brief SBUS register DMA read operation */
#define BCMPTM_SER_READ_REGISTER_CMD_MSG      2

/* SBUS Memory write operation */
#define BCMPTM_SER_WRITE_MEMORY_CMD_MSG       3
/* SBUS Register write operation */
#define BCMPTM_SER_WRITE_REGISTER_CMD_MSG     4

/*!
 * \brief Defined for MMU memory tables which are needed to cleared before enabling SER parity
 */
typedef struct bcmptm_ser_mem_list_s {
    bcmdrd_sid_t  mem_sid;         /*!< SID of mem */
    bool          multi_insts;     /*!< 1: memory tables have multiple instances */
    int           index;           /*!< -1: clear all entries; >=0: clear entry at index */
} bcmptm_ser_mem_clr_list_t;

/*!
 * \brief Field and its value used to initialize registers during SER initialization.
 */
typedef struct bcmptm_fld_info_s {
    bcmdrd_fid_t  ctrl_fld;                      /*!< FID of control register */
    uint32_t      val[BCMDRD_CONFIG_MAX_UNITS];  /*!< Value of FID */
    uint32_t      rd;                            /*! < field needed to read */
} bcmptm_ser_fld_info_t;

/*!
 * \brief Used to initialize registers during SER initialization.
 */
typedef struct bcmptm_ser_ctrl_reg_s {
    /*!< SID of control register or mem */
    bcmdrd_sid_t                 ctrl_reg;
    /*!< index of control register or mem */
    int                          ctrl_reg_index;
    /*!< fields and their value */
    bcmptm_ser_fld_info_t       *ctrl_flds_val;
    /*! < number of fields of SID */
    uint32_t                     num_fld;
    /*! < sleep for utime */
    uint32_t                     utime;
} bcmptm_ser_ctrl_reg_info_t;


/*!
 * \brief SER protection mode of cmic SER engine
 */
typedef enum {
    /*!< Parity 1bit protection mode */
    BCMPTM_SER_PARITY_1BIT,
    /*!< ECC 1bit protection mode */
    BCMPTM_SER_ECC_1FLD = BCMPTM_SER_PARITY_1BIT,
    /*!< Parity 2bit protection mode */
    BCMPTM_SER_PARITY_2BITS,
    /*!< ECC 2bit protection mode */
    BCMPTM_SER_ECC_2FLD = BCMPTM_SER_PARITY_2BITS,
    /*!< Parity 4bit protection mode */
    BCMPTM_SER_PARITY_4BITS,
    /*!< ECC 4bit protection mode */
    BCMPTM_SER_ECC_4FLD = BCMPTM_SER_PARITY_4BITS,
    /*!< Parity 8bit protection mode */
    BCMPTM_SER_PARITY_8BITS
} bcmptm_ser_protection_mode_t;

/*!
 * \brief SER protection type of cmic SER engine
 */
typedef enum {
    /*!< Parity protection mode */
    BCMPTM_SER_TYPE_PARITY,
    /*!< ECC protection mode */
    BCMPTM_SER_TYPE_ECC
} bcmptm_ser_protection_type_t;

/*!
 * \brief interleave type of cmic SER engine
 */
typedef enum {
    /*!< None interleave */
    BCMPTM_SER_INTERLEAVE_NONE,
    /*!< 1bit interleave */
    BCMPTM_SER_INTERLEAVE_MOD2,
    /*!< 2bit interleave */
    BCMPTM_SER_INTERLEAVE_MOD4
} bcmptm_ser_interleave_type_t;

/*!
 * \brief SER info types (for all chips)
 */
typedef enum {
    BCMPTM_SER_PARITY_TYPE_NONE,
    /*!< ECC */
    BCMPTM_SER_PARITY_TYPE_ECC,
    /*!< Parity */
    BCMPTM_SER_PARITY_TYPE_PARITY,
    /*!< MMU */
    BCMPTM_SER_PARITY_TYPE_MMU_SER,
    /*!< MMU GLB */
    BCMPTM_SER_PARITY_TYPE_MMU_GLB,
    /*!< MMU XPE */
    BCMPTM_SER_PARITY_TYPE_MMU_XPE,
    /*!< MMU SC */
    BCMPTM_SER_PARITY_TYPE_MMU_SC,
    /*!< MMU SED */
    BCMPTM_SER_PARITY_TYPE_MMU_SED,
    /*!< CLMAC*/
    BCMPTM_SER_PARITY_TYPE_CLMAC,
    /*!< MAC RX CDC */
    BCMPTM_SER_PARITY_TYPE_MAC_RX_CDC,
    /*!< MAC TX CDC */
    BCMPTM_SER_PARITY_TYPE_MAC_TX_CDC,
    /*!< MAC RX TS */
    BCMPTM_SER_PARITY_TYPE_MAC_RX_TS
    /* Add new parity type here */
} bcmptm_ser_info_type_t;

/*!
 * \brief SER logical block types (for all chips).
 * Just can be used in SER sub-component.
 */
typedef enum {
    /*!< EPIPE */
    BCMPTM_SER_BLK_EPIPE,
    /*!< IPIPE */
    BCMPTM_SER_BLK_IPIPE,
    /*!< IPIPE IDB */
    BCMPTM_SER_BLK_IPIPE_IDB,
    /*!< MMU */
    BCMPTM_SER_BLK_MMU,
    /*!< MMU GLB */
    BCMPTM_SER_BLK_MMU_GLB,
    /*!< MMU XPE */
    BCMPTM_SER_BLK_MMU_XPE,
    /*!< MMU SC */
    BCMPTM_SER_BLK_MMU_SC,
    /*!< MMU SED */
    BCMPTM_SER_BLK_MMU_SED,
    /*!< CLPORT */
    BCMPTM_SER_BLK_CLPORT,
    /*!< XLPORT */
    BCMPTM_SER_BLK_XLPORT,
    /*!< SER engine for TCAM*/
    BCMPTM_SER_BLK_SER_ENGINE,
    /*!< L2_MGMTm */
    BCMPTM_SER_BLK_L2_MGMT,
    /*!< Add new ser logical block type here */
    BCMPTM_SER_BLK_INVAID
} bcmptm_ser_blk_type_t;

/*!
 *\brief Specifies the start bit position and
 * end bit position for the subfield within the entry
 */
typedef struct bcmptm_ser_start_end_bits_s {
    /*!< start bit */
    int start_bit;
    /*!< end bit */
    int end_bit;
} bcmptm_ser_start_end_bits_t;

/*!\brief Max number of subfield per entry, for CMIC SW scan */
#define BCMPTM_NUM_PROT_SECTIONS  4

/*!
 *\brief  TCAM SER information used to configure CMIC SER engine.
 */
typedef struct bcmptm_ser_tcam_generic_info_s {
    /*!< parity data of mem */
    bcmdrd_sid_t                  mem;
    /*!< share parity data with alias mem */
    bcmdrd_sid_t                  alias_mem;
    /*!< SER protection type */
    bcmptm_ser_protection_type_t  prot_type;
    /*!< SER protection mode */
    bcmptm_ser_protection_mode_t  prot_mode;
    /*!< SER interleave type */
    bcmptm_ser_interleave_type_t  intrlv_mode;
    /*!< the start and end bit position for the subfield within the entry */
    bcmptm_ser_start_end_bits_t   start_end_bits[BCMPTM_NUM_PROT_SECTIONS];
    /*!< Range disable bitmap to skip memory holes, valid only for BCMPTM_SER_FLAG_RANGE_DISABLE */
    uint32_t                      addr_mask;
    /*!< The start offset of SER_MEMORY for one SER range */
    uint32_t                      ser_section_start;
    /*!< The end offset of SER_MEMORY fo one SER range */
    uint32_t                      ser_section_end;
    /*!< Used to index H/W registers */
    uint32_t                      ser_hw_index;
    /*!< SER flags of mem, read-only */
    uint32_t                      ser_flags;
    /*!< number of instance of mem, Valid only for BCMPTM_SER_FLAG_ACC_TYPE_CHK */
    uint8_t                       num_instances;
    /*!< Start bit position to skip memory holes, valid only for BCMPTM_SER_FLAG_RANGE_DISABLE */
    uint32_t                      addr_start_bit;
    /*!< SER state can be changed during runtime */
    uint32_t                      ser_dynamic_state;
} bcmptm_ser_tcam_generic_info_t;

/*!
 *\brief Controlling register and name of buffer or bus.
 */
typedef struct bcmptm_ser_buf_bus_ctrl_reg_s {
    /*!< Controlling register */
    bcmdrd_sid_t reg;
    /*!< Name of buffer or bus */
    char         *mem_str;
    /*!< Membase of buffer or bus */
    int          membase;
    /*!< Stage ID of buffer or bus */
    int          stage_id;
} bcmptm_ser_buf_bus_ctrl_reg_t;

/*!
 *\brief Top level of SER controlling information
 */
typedef struct bcmptm_ser_top_level_info_s {
    /*!< ID of intr status reg, eg. 3: intr3, 4: intr4, 5: intr5 */
    uint8_t               cmic_reg;
    /*!< offset of intr status reg */
    uint32_t              cmic_bit;
    /*!< Block type */
    bcmptm_ser_blk_type_t blocktype;
    /*!< Table instance */
    int                   pipe;
    /*!< SID of enabling register */
    bcmdrd_sid_t          enable_reg;
    /*!< SID of status register */
    bcmdrd_sid_t          status_reg;
    /*!< FID of enabling register */
    bcmdrd_fid_t          enable_field;
    /*!< The second level of SER controlling information */
    void                  *info;
    /*!< Block number */
    uint8_t               blk_inst;
} bcmptm_ser_top_level_route_info_t;

/*!
 *\brief The low level of SER controlling information
 */
typedef struct bcmptm_ser_low_level_info_s {
    /*!< ser information type */
    bcmptm_ser_info_type_t    type;
    /*!< The third level of SER controlling information */
    struct bcmptm_ser_low_level_info_s *info;
    /*!< Block number */
    int                       id;
    /*!< Field of top level enabling register */
    bcmdrd_fid_t              group_reg_enable_field;
    /*!< Field of top level status register */
    bcmdrd_fid_t              group_reg_status_field;
    
    bcmdrd_sid_t              mem;
    /*!< Debug information */
    char                      *mem_str;
    /*!< Low level of SER enabling register  */
    bcmdrd_sid_t              enable_reg;
    /*!< Low level of field of SER enabling register  */
    bcmdrd_fid_t              enable_field;
    /*!< Low level of fields of SER enabling register  */
    bcmdrd_fid_t              *enable_field_list;
    /*!< Low level of SER intr controlling register  */
    bcmdrd_sid_t              intr_enable_reg;
    /*!< Low level of SER field of intr controlling register  */
    bcmdrd_fid_t              intr_enable_field;
    /*!< Low level of SER fields of intr controlling register  */
    bcmdrd_fid_t              *intr_enable_field_list;
    /*!< Low level of SER intr status register  */
    bcmdrd_sid_t              intr_status_reg;
    /*!< Low level of SER intr status register list  */
    bcmptm_ser_buf_bus_ctrl_reg_t *intr_status_reg_list;
    /*!< Low level of field of SER intr status register  */
    bcmdrd_fid_t              intr_status_field;
    /*!< Low level of fields of SER intr status register  */
    bcmdrd_fid_t              *intr_status_field_list;
    /*!< Low level of SER intr clear register */
    bcmdrd_sid_t              intr_clr_reg;
    /*!< Low level of SER field of intr clear register */
    bcmdrd_fid_t              intr_clr_field;
    /*!< Low level of SER fields of intr clear register */
    bcmdrd_fid_t              *intr_clr_field_list;
} bcmptm_ser_low_level_info_t;

/*!\brief information of buffer or bus */
typedef struct bcmptm_ser_buffer_bus_info_s {
    /*!< Name of buffer or bus from BD */
    char          *bd_name;
    /*!< Membase information from SER_STATUS_BUS format in regsfile */
    char          *membase_info;
    /*!< Membase of buffer or bus */
    int          membase;
    /*!< Stage ID of buffer or bus */
    int          stage_id;
} bcmptm_ser_buffer_bus_info_t;

/*!\brief handler for interrupts, its format is equal to bcmbd_intr_f */
typedef void (*bcmbd_ser_intr_f)(int unit, uint32_t intr_param);

/*!
 *\brief MMU SER controlling information
 */
typedef struct bcmptm_ser_mmu_intr_mapping_s {
    /*!< Interrupt number of XPE, SC, GLB or SED defined by BD */
    uint32_t               mmu_intr_num;
    /*!< ser information type */
    bcmptm_ser_info_type_t type;
    /*!< MEM_SER_FIFO_STSr of XPE, SC, GLB or SED */
    bcmdrd_sid_t           fifo_status_reg;
    /*!< EMPTYf of MEM_SER_FIFO_STSr */
    bcmdrd_fid_t           empty_fid;
    /*!< MEM_FAIL_ADDR_64m of XPE, SC, GLB or SED */
    bcmdrd_sid_t           fifo_mem;
    /*!< ERR_MULTf, ERR_TYPEf, EADDRf of MEM_FAIL_ADDR_64m*/
    bcmdrd_fid_t           *fifo_mem_fids;
    /*!< MEM_FAIL_INT_CTRr of XPE, SC, GLB or SED */
    bcmdrd_sid_t           ser_fail_ctr_reg;
    /*!< block name */
    char                   *blk_name;
} bcmptm_ser_mmu_intr_mapping_t;

/*!
 *\brief Get adext for one chips according to the members of following structure.
 */
typedef struct bcmptm_cmic_adext_info_s {
    /*!< access type */
    int acc_type;
    /*!< block number */
    int blk_num;
    /* Add new member here */
} bcmptm_cmic_adext_info_t;

/*!\brief SER flags */
typedef enum {
    /*! Multi-bits SER error */
    BCMPTM_SER_FLAG_MULTIBIT  = 1 << 1,
    /*! SER type. 1: 1bit SER checking; 0: parity/2bit SER checking */
    BCMPTM_SER_FLAG_DOUBLEBIT = 1 << 2,
    /*! ECC SER error */
    BCMPTM_SER_FLAG_ECC = 1 << 3,
    /*! Parity SER error */
    BCMPTM_SER_FLAG_PARITY = 1 << 4,
    /*! SBUS can't be accessible */
    BCMPTM_SER_FLAG_NON_SBUS = 1 << 5,
    /*! SER caused packet drop */
    BCMPTM_SER_FLAG_DROP = 1 << 6,
    /*! EP/IP: SER error in SOP cell.  */
    BCMPTM_SER_FLAG_ERR_IN_SOP = 1 << 7,
    /*! EP/IP: SER error in MOP cell. */
    BCMPTM_SER_FLAG_ERR_IN_MOP = 1 << 8,
    /*! EP/IP: SER error in EOP cell. */
    BCMPTM_SER_FLAG_ERR_IN_EOP = 1 << 9,
    /*! EP/IP: SER error in sbus transaction. */
    BCMPTM_SER_FLAG_ERR_IN_SBUS = 1 << 10,
    /*! EP/IP: SER error in transaction - refresh, aging etc. */
    BCMPTM_SER_FLAG_ERR_IN_AGE = 1 << 11,
    /*! EP/IP: SER error in other transaction. */
    BCMPTM_SER_FLAG_ERR_IN_OTHER = 1 << 12,
    /*! MMU: SER error in MMU transaction. */
    BCMPTM_SER_FLAG_ERR_IN_MMU = 1 << 13,
    /*! PORT: SER error in PORT transaction. */
    BCMPTM_SER_FLAG_ERR_IN_PORT = 1 << 14,
    /*! TCAM SER error. */
    BCMPTM_SER_FLAG_TCAM = 1 << 15,
    /*! L2mod SER error. */
    BCMPTM_SER_FLAG_L2MOD = 1 << 16,
} bcmptm_ser_log_flag_t;

/*!
 * \brief information can be used to correct SER error.
 */
typedef struct bcmptm_ser_correct_info_s {
    /*! flags of SER correction */
    uint32_t        flags;
    /*! SID of memory or register owning SER error, use it to correct SER error */
    bcmdrd_sid_t    sid;
    /*! Second SID of memory or register need to be corrected, such as MMU_WRED_CONFIG */
    bcmdrd_sid_t    second_sid;
    /*! SID of memory or register owning SER error, reporting view */
    bcmdrd_sid_t    reported_sid;
    /*! Id of table instance owning SER error */
    int             tbl_inst;
    /*! Id of table instance of second SID owning SER error */
    int             second_tbl_inst;
    /*! sbus address of one entry owning SER error */
    uint32_t        addr;
    /*! sbus address of one entry of second SID owning SER error */
    uint32_t        second_addr;
    /*! entry index owning SER error */
    uint32_t        index;
    /*! entry index of  second SID owning SER error */
    uint32_t        second_index;
    /*! block type of mem reporting SER error */
    int             blk_type;
    /*! detect_time */
    sal_usecs_t     detect_time;
} bcmptm_ser_correct_info_t;

/*!\brief In order to correct a SER error, SERC needs to write several PTs.  */
#define BCMPTM_SER_MAX_SRAMS              16

/*!\brief Max entries number per bucket.  */
#define BCMPTM_SER_MAX_ENTRIES_PER_BKT    8

/*!\brief Used to correct SER in XOR or LP PTs */
typedef struct bcmptm_ser_sram_info_s {
    /*! RAM number */
    uint32_t      ram_count;
    /*! PT ID */
    bcmdrd_sid_t  pt_view[BCMPTM_SER_MAX_SRAMS];
    /*! Number of indexes */
    uint32_t      index_count[BCMPTM_SER_MAX_SRAMS];
    /*! Indexes */
    uint32_t      indexes[BCMPTM_SER_MAX_SRAMS][BCMPTM_SER_MAX_ENTRIES_PER_BKT];
    /*! H/W recovery for LP tables */
    bool          hw_recovery;
} bcmptm_ser_sram_info_t;

/*!\brief number of extra controlling register */
#define BCMPTM_SER_EXTRA_REG_NUM     2

/*!\brief controlling register info for XOR PTs */
typedef struct bcmptm_ser_control_reg_info_s {
    /*! SID: Disables PRE_MEMWR parity check. */
    bcmdrd_sid_t  wr_check_reg;
    /*! FID: Disables PRE_MEMWR parity check. */
    bcmdrd_fid_t  wr_check_field;
    /*! SID: force XOR generation per table bank for reads */
    bcmdrd_sid_t  force_reg;
    /*! SID: force XOR generation per table bank for reads */
    bcmdrd_fid_t  force_field;
    /*! SID: Disables XOR bank write. */
    bcmdrd_sid_t  xor_wr_reg;
    /*! FID: Disables XOR bank write. */
    bcmdrd_fid_t  xor_wr_field;
    /*! SID: can be used to Freeze L2. */
    bcmdrd_sid_t  extra_reg[BCMPTM_SER_EXTRA_REG_NUM];
    /*! FID: can be used to Freeze L2. */
    bcmdrd_fid_t  extra_field[BCMPTM_SER_EXTRA_REG_NUM];
    /*! FVAL: can be used to Freeze L2. */
    uint32_t      extra_field_val[BCMPTM_SER_EXTRA_REG_NUM];
} bcmptm_ser_control_reg_info_t;

/*!
 * \brief Get MMU memory tables which should be cleared before enabling SER parity.
 *
 * \param [in] unit       Unit number.
 * \param [out] mem_list  List of PTs which should be cleared
 * \param [out] num       Number of PTs
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_mem_clr_list_get_t)(int unit,
                                 bcmptm_ser_mem_clr_list_t **mem_list,
                                 int *entry_num);

/*!
 * \brief Get control registers' info. Those registers can be used to clear memory tables,
 * \n Those tables should be cleared after enabling SER checking.
 *
 * \param [in] unit           Unit number.
 * \param [out] reg_ctrl_lst  Data list of controlling registers
 * \param [out] reg_num       Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_clear_mem_after_ser_enable_t)(int unit,
                                           bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                           int *reg_num);

/*!
 * \brief Get info to enable ser interrupt of l2_mgmt
 *
 * \param [in] unit          Unit number.
 * \param [out] reg_ctrl_lst Data list of controlling registers used to enable SER interrupt of L2_mgmt
 * \param [out] reg_num      Number of item in data list
 * \param [out] intr_num     CMIC level interrupt number
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_l2_mgmt_ser_enable_info_get_t)(int unit,
                                            bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                            int *reg_num, int *intr_num);

/*!
 * \brief Get control registers' info. Those registers can be used to clear memory tables.
 * \n Those tables should be cleared before enabling SER checking.
 *
 * \param [in] unit          Unit number.
 * \param [out] reg_ctrl_lst Data list of controlling registers used to clear memory tables
 * \param [out] reg_num      Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_clear_mem_before_ser_enable_t)(int unit,
                                            bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                            int *reg_num);

/*!
 * \brief Get control registers' info used to start IDB SER H/W scan engine for TCAMs.
 *
 * \param [in] unit          Unit number.
 * \param [out] reg_ctrl_lst Data list of controlling registers
 * \param [out] reg_num      Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_tcam_idb_ser_scan_reg_info_get_t)(int unit,
                                               bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                               int *reg_num);

/*!
 * \brief Get ser_acc_type_map info. The info can be used to configure SER_ACC_TYPE_MAPm.
 *
 * \param [in] unit               Unit number.
 * \param [out] ser_acc_type_map  Data list of SER_ACC_TYPE_MAPm
 * \param [out] reg_num           Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_acc_type_map_info_get_t)(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **ser_acc_type_map,
                                      int *reg_num);

/*!
 * \brief Get TCAM SER scan info. This info can be used during TCAM S/W scan.
 *
 * \param [in] unit             Unit number.
 * \param [out] tcam_ser_info   Data list of TCAM SER scan info
 * \param [out] tcam_info_num   Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_tcam_info_get_t)(int unit,
                              bcmptm_ser_tcam_generic_info_t **tcam_ser_info,
                              int *tcam_info_num);

/*!
 * \brief Get SER mute memory tables or buffer or bus, their parity checking should not be enabled
 *
 * \param [in] unit                Unit number.
 * \param [out] mem_ser_mute_list  Memory table list.
 * \param [out] mem_ser_mute_num   Number of memory tables
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_mute_mem_list_get_t)(int unit,
                                  bcmdrd_sid_t **mem_ser_mute_list,
                                  int *mem_ser_mute_num);

/*!
 * \brief Get SER route information used to correct SER error.
 *
 * \param [in] unit              Unit number.
 * \param [out] ser_route_blocks Data list used to correct SER error.
 * \param [out] route_num        Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_route_info_get_t)(int unit,
                               bcmptm_ser_top_level_route_info_t **ser_route_blocks,
                               int *route_num);

/*!
 * \brief Get ser MMU interrupt status register mapping.
 * \n SERC thread uses the information to know which block of MMU has SER error.
 *
 * \param [in] unit                   Unit number.
 * \param [out] cmic_level_intr_id    CMIC level interrupt number.
 * \param [out] ser_mmu_intr_map Data List of low level interrupt status registers.
 * \param [out] intr_map_num          Number of item of data list.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_mmu_intr_map_get_t)(int unit,
                                 uint32_t *cmic_level_intr_id,
                                 bcmptm_ser_mmu_intr_mapping_t **ser_mmu_intr_map,
                                 int *intr_map_num);

/*!
 * \brief Get block types (per chips) according to SER logical block types (global).
 *
 * \param [in] unit          Unit number.
 * \param [in] ser_blk_type  SER logical block types
 * \param [out] blk_type     Block types (per chips) defined by BD.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_blk_type_map_t)(int unit,
                             bcmptm_ser_blk_type_t ser_blk_type,
                             int *blk_type);

/*!
 * \brief Enable or disable cmic interrupt for SER.
 *
 * \param [in] unit         Unit number.
 * \param [in] cmc          CMC.
 * \param [in] reg_id       ID of CMIC level enable interrupt register.
 * \param [in] offset       Bit offset of CMIC level enable interrupt register.
 * \param [in] enable       1: enable, 0: disable.
 *
 * \return NONE.
 */
typedef void
(*bcmptm_ser_cmic_intr_enable_t)(int unit, int cmc, int reg_id,
                                 int offset, int enable);

/*!
 * \brief Register CMIC level interrupt CB routine to handle SER event reported by interrupt source 'intr_num'.
 *
 * \param [in] unit             Unit number.
 * \param [in] cmc              CMC.
 * \param [in] intr_num         Number of interrupt source.
 * \param [in] intr_func        CB routine for non-MMU blocks
 * \param [in] mmu_intr_func    CB routine for MMU block
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_cmic_intr_func_set_t)(int unit, int cmc, unsigned int intr_num,
                                   bcmbd_ser_intr_f intr_func,
                                   bcmbd_ser_intr_f mmu_intr_func);

/*!
 * \brief Get registers such as:
 *
 *\n SER_RANGE_X_CONFIGr, SER_RANGE_X_STARTr, SER_RANGE_X_ENDr,
 *\n SER_RANGE_X_RESULTr, SER_RANGE_X_ADDR_BITSr, SER_RANGE_X_DISABLEr and
 *\n SER_RANGE_X_PROT_WORD_Yr
 *\n 'X= 0, 1, 2....31', 'Y = 0, 1, 2, 3'
 *
 *\n Those registers can be used to configure CMIC SER engine.
 *
 * \param [in] unit                   Unit number.
 * \param [out] range_cfg_list        List of SER_RANGE_x_CONFIGr.
 * \param [out] range_start_list      List of SER_RANGE_x_STARTr.
 * \param [out] range_end_list        List of SER_RANGE_x_ENDr.
 * \param [out] range_result_list     List of SER_RANGE_x_RESULTr.
 * \param [out] prot_list             List of SER_RANGE_x_PROT_WORD_0r.
 * \param [out] range_addr_bits_list  List of SER_RANGE_x_ADDR_BITSr
 * \param [out] range_disable_list    List of SER_RANGE_x_DISABLEr.
 * \param [out] group_num             Number of item in above lists.
 * \param [out] range_cfg_fid_list    Field list of SER_RANGE_x_CONFIGr.
 * \param [out] prot_fid_list         Field list of SER_RANGE_x_PROT_WORD_0r.
 * \param [out] ser_memory_sid        SER_MEMORYm
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_cmic_ser_engine_regs_get_t)(int unit,
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
                                         bcmdrd_sid_t  *ser_memory_sid);

/*!
 * \brief Get info of regs SER_RING_ERR_CTRLr/SER_RANGE_ENABLEr/SER_CONFIG_REGr,
 * \n in order to start CMIC SER engine.
 *
 * \param [in] unit           Unit number.
 * \param [out] reg_ctrl_lst  Data list of controlling registers.
 * \param [out] reg_num       Number of item in data list.
 * \param [out] intr_num      Interrupt number of CMIC SER engine.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_cmic_ser_engine_enable_t)(int unit,
                                       bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                       int *reg_num, int *intr_num);

/*!
 * \brief Update cache of field SER_RANGE_ENABLEf of SER_RANGE_ENABLEr.
 *
 * \param [in] unit        Unit number.
 * \param [in] bit_offset  Bit offset of field SER_RANGE_ENABLEf.
 * \param [in] enable      Set value indexed by 'bit_offset' as '1' or '0'.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_cmic_ser_engine_update_t)(int unit, uint32_t bit_offset, int enable);

/*!
 * \brief Get info of regs SER_ERROR_0r/SER_ERROR_1r/SER_RESULT_0m/SER_RESULT_1m.
 * \n Those registers can be used to get SER error information.
 *
 * \param [in] unit            Unit number.
 * \param [out] ser_error_lst  Data list of SER_ERROR_0r and SER_ERROR_1r.
 * \param [out] ser_result_0   Data list of SER_RESULT_0m.
 * \param [out] ser_result_1   Data list of SER_RESULT_1m.
 * \param [out] err_num        Number of item in 'ser_error_lst'.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_cmic_ser_result_get_t)(int unit,
                                    bcmptm_ser_ctrl_reg_info_t **ser_error_lst,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_0,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_1,
                                    int *err_num);

/*!
 * \brief Get adext according to adext_info.
 *
 * \param [in] unit        Unit number.
 * \param [in] adext_info  Acc_type and block number.
 * \param [out] adext      Magical data from acc_type and block number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_cmic_adext_get_t)(int unit, bcmptm_cmic_adext_info_t adext_info,
                               uint32_t *adext);

/*!
 * \brief Get access type according to sid.
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          PT ID.
 * \param [out] acc_type    ID of access type (per chip).
 * \param [out] acctype_str String name of access type (all chips).
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_pt_acctype_get_t)(int unit, bcmdrd_sid_t sid, uint32_t *acc_type,
                               const char **acctype_str);

/*!
 * \brief Get schan operation_code for DMA access.
 *
 * \param [in] unit         Unit number.
 * \param [in] cmd          Command defined by SER, e.g: BCMPTM_SER_READ_MEMORY_CMD_MSG.
 * \param [in] dst_blk      Destination block number.
 * \param [in] blk_types    Block types.
 * \param [in] data_len     Length of data.
 * \param [in] dma
 * \param [in] ignore_mask
 * \param [out] opcode      DMA opcode.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_cmic_schan_opcode_get_t)(int unit, int cmd, int dst_blk,
                                      int blk_types, int tbl_inst, int data_len,
                                      int dma, uint32_t ignore_mask,
                                      uint32_t *opcode);

/*!
 * \brief Check whether MAC error fid belongs to double bit error fids or belongs to single bit error fids.
 *
 * \param [in] unit                Unit number.
 * \param [in] bcmdrd_fid_t        Field ID.
 * \param [out] is_double_err_fid  1: double bit error field, 0: single bit error field.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_mac_err_fid_check_t)(int unit, bcmdrd_fid_t err_fid,
                                  int *is_double_err_fid);

/*!
 * \brief Get SID of ING_SER_FIFOm, EGR_SER_FIFOm, L2_MGMT_SER_FIFOm,  EGR_INTR_STATUSr,.
 * \n and L2_MGMT_INTRr, and get FID list of ING_SER_FIFOm, EGR_SER_FIFOm, L2_MGMT_SER_FIFOm.
 *
 * \param [in] unit                     Unit number.
 * \param [out] ing_fifo                ING_SER_FIFOm
 * \param [out] egr_fifo                EGR_SER_FIFOm
 * \param [out] egr_intr_statr          EGR_INTR_STATUSr
 * \param [out] l2_mgmt_fifo            L2_MGMT_SER_FIFOm
 * \param [out] l2_mgmt_intr            L2_MGMT_INTRr
 * \param [out] fid_list                Field list of ING_SER_FIFOm or EGR_SER_FIFOm
 * \param [out] fid_num                 Number of field
 * \param [out] l2_mgmt_fifo_not_empty  Field ID of SER_FIFO_NOT_EMPTYf
 * \param [out] l2_mod_fifo             L2_MOD_FIFOm
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_ing_egr_fifo_get_t)(int unit, bcmdrd_sid_t *ing_fifo,
                                 bcmdrd_sid_t *egr_fifo,
                                 bcmdrd_sid_t *egr_intr_statr,
                                 bcmdrd_sid_t *l2_mgmt_fifo,
                                 bcmdrd_sid_t *l2_mgmt_intr,
                                 bcmdrd_fid_t **fid_list,
                                 int *fid_num,
                                 bcmdrd_fid_t *l2_mgmt_fifo_not_empty,
                                 bcmdrd_sid_t *l2_mod_fifo);

/*!
 * \brief Get HW memory base information.
 *
 * \param [in] unit         Unit number.
 * \param [in] membase      Membase of buffer or bus.
 * \param [in] stage_id     Stage id of buffer or bus.
 * \param [out] bd_bb_nm    BD name of base information.
 * \param [out] base_info   String of base information  e.g: "OBM_QUEUE_FIFO - In Idb" etc.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_hwmem_base_info_get_t)(int unit,
                                     int membase, int stage_id,
                                     char **bd_bb_nm, char **base_info);

/*!
 * \brief Remap index of Flex ctr memory table
 *
 * \param [in] unit          Unit number.
 * \param [in] address       Address of PT
 * \param [in] blk_type      Block type
 * \param [in] stage         Stage ID
 * \param [in] index         Index of PT
 *
 * \return index remapped
 */
typedef uint32_t
(*bcmptm_ser_flex_ctr_index_remap_t)(int unit, uint32_t address, int blk_type,
                                     uint32_t stage, uint32_t index);

/*!
 * \brief Remap index or SID and address of MMU memory tables.
 * \n Due to H/W fault, MMU reports wrong SID or address, need this routine to correct them.
 *
 * \param [in] unit           Unit number.
 * \param [in/out] spci       Old/new SID, address, and other information.
 * \param [in] adext          Adext.
 * \param [in] blk_inst       Block instance.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_mmu_mem_remap_t)(int unit, bcmptm_ser_correct_info_t *spci,
                               uint32_t adext, int blk_inst);

/*!
 * \brief  Remap AGGR SID of IP/EP memory tables to DATA-ONLY SID.
 *\n RM only uses data-only view instead of AGGR view.
 *\n There is no PTcache for AGGR view if data-only view is used only.
 *
 * \param [in] unit        Unit number.
 * \param [in] spci        AGGR SID.
 * \param [out] remap_sid  DATA-ONLY SID.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_ip_ep_sram_remap_t)(int unit, bcmdrd_sid_t sid,
                                 bcmdrd_sid_t *remap_sid);

/*!
 * \brief  Get ING and EGR SBS_CONTROLr, in order to make sure defualt pipe.
 *
 * \param [in] unit               Unit number.
 * \param [out] ing_sbs_control   SBS_CONTROLr.
 * \param [out] egr_sbs_control   EGR_SBS_CONTROLr.
 * \param [out] pipe_select_fid   Field id.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_ing_egr_sbs_control_get_t)(int unit, bcmdrd_sid_t *ing_sbs_control,
                                        bcmdrd_sid_t *egr_sbs_control, bcmdrd_fid_t *pipe_select_fid);

/*!
 * \brief  Get test field ID to be injected SER error for data-split memory table
 *
 * \param [in] unit          Unit number.
 * \param [out] sid          Data-split memory table id.
 * \param [out] acc_type     Pipe number.
 * \param [out] test_fid     Field id existed only in one pipe which id is equal to 'acc_type'.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_data_split_mem_test_fid_get_t)(int unit, bcmdrd_sid_t sid,
                                            int acc_type, bcmdrd_fid_t *test_fid);

/*!
 * \brief  Remap functional SID to ECC view SID, and remap functional index to ECC view index.
 *
 * \param [in] unit             Unit number.
 * \param [in] sid_fv           Functional PT ID.
 * \param [in] index_fv         Functional PT index.
 * \param [out] sid             ECC view PT ID, can be NULL
 * \param [out] index           ECC view PT index, can be NULL
 * \param [out] ecc_checking    1: Functional PT has ECC view, cannot be NULL
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_mem_ecc_info_get_t)(int unit, bcmdrd_sid_t sid_fv, int index_fv,
                                 bcmdrd_sid_t *sid, int *index, int *ecc_checking);

/*!
 * \brief Map TCAM-ONLY view to the view protected by TCAM SER engine.
 *
 * \param [in] unit                     Unit number.
 * \param [in] only_sid                 TCAM-ONLY PT ID.
 * \param [out] sid                     Functional or TCAM-ONLY PT ID.
 * \param [out] inject_err_to_key_fld   1: Inject SER error to TCAM fields.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmptm_ser_tcam_only_remap_t)(int unit, bcmdrd_sid_t only_sid, bcmdrd_sid_t *sid, int *inject_err_to_key_fld);

/*!
* \brief Get refresh register information. Such as: REFRESH_ENf of MMU_GCFG_MISCCONFIGr
*
* \param [in] unit                 Unit number.
* \param [out] mem_refresh_info    Data list of refresh registers.
* \param [out] info_num            Number of item of data list.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_refresh_regs_info_get_t)(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **mem_refresh_info,
                                      uint32_t *info_num);

/*!
* \brief For some devices, if found SER event in IDB_IS_TDM_CAL_ECC_STATUSr, need to correct
* IS_TDM_CALENDAR1m or IS_TDM_CALENDAR0m.
*
* \param [in] unit                   Unit number.
* \param [in] idb_ecc_status_reg     IDB_IS_TDM_CAL_ECC_STATUSr
* \param [out] entry_index           Index of IS_TDM_CALENDAR1m or IS_TDM_CALENDAR0m.
* \param [out] is_tdm_calendar       IS_TDM_CALENDAR1m or IS_TDM_CALENDAR0m.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_is_tdm_calendar_sid_get_t)(int unit,
                                        bcmdrd_sid_t idb_ecc_status_reg,
                                        uint32_t *entry_index, bcmdrd_sid_t *is_tdm_calendar);

/*!
* \brief Get controlling registers used to disables PRE_MEMWR parity check,
* \n force XOR generation per table bank for reads, disables XOR bank write and others.
*
* \param [in] unit                 Unit number.
* \param [in] sid                  XOR PT ID.
* \param [in] index                Index of PT.
* \param [out] sram_ctrl_reg_info  Controlling registers' information.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_sram_ctrl_reg_get_t)(int unit, bcmdrd_sid_t sid, uint32_t index,
                                  bcmptm_ser_control_reg_info_t *sram_ctrl_reg_info);

/*!
* \brief If found SER event in HASH_XOR memory tables, needs to correct SER error by
* writing corresponding indexes on every RAM. If found SER event in LP memory tables,
* needs to correct SER error by writing corresponding functional memory views.
*
* \param [in] unit       Unit number.
* \param [in] sid        XOR or LP PT ID.
* \param [in] index      Index of PT.
* \param [out] sram_info Information including SIDs, indexes of functional view.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_xor_lp_mem_info_get_t)(int unit, bcmdrd_sid_t sid, uint32_t index,
                                    bcmptm_ser_sram_info_t *sram_info);

/*!
* \brief Some PTs use UFT, so their entry number can be changed according to UFT configuration.
* \n Need to check whether index of the PTs is valid or not.
*
* \param [in] unit            Unit number.
* \param [in] sid             PT ID using UFT.
* \param [in] index           Index of PT.
* \param [out] max_index      Current maximum index of PT ID.
* \param [out] dedicated_bank 1: 'index' is on dedicated bank of PT ID.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_uft_mem_index_check_t)(int unit, bcmdrd_sid_t sid, uint32_t index,
                                    uint32_t *max_index, int *dedicated_bank);

/*!
* \brief Read ISS_BANK_CONFIGr to know the size of tables which can use UFT banks.
*
* \param [in] unit            Unit number.
* \param [out] reg_ctrl_lst   Data list of ISS_BANK_CONFIGr.
* \param [out] reg_num        Number of item in data list.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_iss_bank_config_info_get_t)(int unit,
                                         bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                         int *reg_num);

/*!
* \brief Update bank number of tables which can use UFT banks..
*
* \param [in] unit          Unit number.
* \param [in] group_id      Corresponding L2/L3/ALPM/L3_DEFIP.
* \param [in] bank_num      Number of UFT bank used by 'group_id'.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_uft_mem_resize_t)(int unit, const char *group_id, int bank_num);

/*!
* \brief Some tables do not need to be scanned..
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [out] skip    1: 'sid' does not need to be scanned.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef int
(*bcmptm_ser_sram_scan_mem_skip_t)(int unit, bcmdrd_sid_t sid, bool *skip);

/*!
* \brief When there is SER error in bus ISW1_ISW2_PT_BUS,
* H/W reports address of HG_TRUNK_GROUPm
*
* \param [in] unit          Unit number.
* \param [in] membase       Membase of ISW1_ISW2_PT_BUS.
* \param [in] address       Address of HG_TRUNK_GROUPm.
* \param [out] non_sbus     Non_sbus, 1:buffer or bus has SER error, 0: PT has SER error.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
typedef void
(*bcmptm_ser_bus_addr_translate_t)(int unit, int membase, int address, int *non_sbus);

/*!\brief Routines in chip specific code */
typedef struct bcmptm_ser_per_chip_data_driver_s {
    bcmptm_ser_mem_clr_list_get_t mem_clr_list_get_fn;

    bcmptm_ser_clear_mem_after_ser_enable_t clear_mem_after_ser_enable_fn;

    bcmptm_ser_l2_mgmt_ser_enable_info_get_t l2_mgmt_ser_enable_info_get_fn;

    bcmptm_ser_clear_mem_before_ser_enable_t clear_mem_before_ser_enable_fn;

    bcmptm_ser_tcam_idb_ser_scan_reg_info_get_t tcam_idb_ser_scan_reg_info_get_fn;

    bcmptm_ser_acc_type_map_info_get_t acc_type_map_info_get_fn;

    bcmptm_ser_tcam_info_get_t tcam_info_get_fn;

    bcmptm_ser_mute_mem_list_get_t mute_mem_list_get_fn;

    bcmptm_ser_route_info_get_t route_info_get_fn;

    bcmptm_ser_mmu_intr_map_get_t mmu_intr_map_get_fn;

    bcmptm_ser_blk_type_map_t blk_type_map_fn;

    bcmptm_ser_cmic_intr_enable_t cmic_intr_enable_fn;

    bcmptm_ser_cmic_intr_func_set_t cmic_intr_func_set_fn;

    bcmptm_ser_cmic_ser_engine_regs_get_t cmic_ser_engine_regs_get_fn;

    bcmptm_ser_cmic_ser_engine_enable_t cmic_ser_engine_enable_fn;

    bcmptm_ser_cmic_ser_engine_update_t cmic_ser_engine_update_fn;

    bcmptm_ser_cmic_ser_result_get_t cmic_ser_result_get_fn;

    bcmptm_ser_cmic_adext_get_t cmic_adext_get_fn;

    bcmptm_ser_pt_acctype_get_t pt_acctype_get_fn;

    bcmptm_ser_cmic_schan_opcode_get_t cmic_schan_opcode_get_fn;

    bcmptm_ser_mac_err_fid_check_t mac_err_fid_check_fn;

    bcmptm_ser_ing_egr_fifo_get_t ing_egr_fifo_get_fn;

    bcmptm_ser_hwmem_base_info_get_t hwmem_base_info_get_fn;

    bcmptm_ser_bus_addr_translate_t  bus_addr_translate_fn;

    bcmptm_ser_flex_ctr_index_remap_t flex_ctr_index_remap_fn;

    bcmptm_ser_mmu_mem_remap_t mmu_mem_remap_fn;

    bcmptm_ser_ip_ep_sram_remap_t ip_ep_sram_remap_fn;

    bcmptm_ser_ing_egr_sbs_control_get_t ing_egr_sbs_control_get_fn;

    bcmptm_ser_data_split_mem_test_fid_get_t data_split_mem_test_fid_get_fn;

    bcmptm_ser_mem_ecc_info_get_t mem_ecc_info_get_fn;

    bcmptm_ser_tcam_only_remap_t tcam_only_remap_fn;

    bcmptm_ser_refresh_regs_info_get_t refresh_regs_info_get_fn;

    bcmptm_ser_is_tdm_calendar_sid_get_t is_tdm_calendar_sid_get_fn;

    bcmptm_ser_sram_ctrl_reg_get_t sram_ctrl_reg_get_fn;

    bcmptm_ser_xor_lp_mem_info_get_t xor_lp_mem_info_get_fn;

    bcmptm_ser_uft_mem_index_check_t uft_mem_index_check_fn;

    bcmptm_ser_iss_bank_config_info_get_t iss_bank_config_info_get_fn;

    bcmptm_ser_uft_mem_resize_t uft_mem_resize_fn;

    bcmptm_ser_sram_scan_mem_skip_t sram_scan_mem_skip_fn;

} bcmptm_ser_data_driver_t;

/*!
* \brief Get data driver from chip specific code.
*
* \param [in] unit     Logical device id.
*
* \return Data driver routines.
*/
extern const bcmptm_ser_data_driver_t*
bcmptm_ser_data_driver_register(int unit);


/*!\brief Pointer to routines in chip specific code */
extern const bcmptm_ser_data_driver_t* bcmptm_ser_data_cb[BCMDRD_CONFIG_MAX_UNITS];

/*!\brief Call routines in chip specific code */
#define BCMPTM_SER_DATA_DRIVER_CB(_unit, _fn_name) \
    bcmptm_ser_data_cb[_unit]->_fn_name


#endif /* BCMPTM_SER_CHIP_INTERNAL_H */
