/*! \file bcmptm_wal_local.h
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

#ifndef BCMPTM_WAL_LOCAL_H
#define BCMPTM_WAL_LOCAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_internal.h>


/*******************************************************************************
 * Defines
 */
#define BCMPTM_WAL_MSG_BPTR                bcmptm_wal_msg_bptr[unit]
#define BCMPTM_WAL_MSG_DQ_READY_BPTR       bcmptm_wal_msg_dq_ready_bptr[unit]
#define BCMPTM_WAL_SCF_NUM_CHANS           bcmptm_wal_scf_num_chans[unit]


/*******************************************************************************
 * Typedefs
 */
/*! \brief wal_msg_cmd types */
typedef enum {
    BCMPTM_WAL_MSG_CMD_NOP = 0, /* can be used as marker to detect empty WAL, etc */
    BCMPTM_WAL_MSG_CMD_WRITE,
    BCMPTM_WAL_MSG_CMD_SLAM,
    BCMPTM_WAL_MSG_CMD_THREAD_EXIT       /* stop thread */
} bcmptm_wal_msg_cmd_t;

/*! \brief slice_mode encoding in uint32_t */
typedef union bcmptm_wal_slice_mode_info_s {
    struct {
        /*! 1st slice num */
        uint32_t slice_num0:10;

        /*! 2nd slice num */
        uint32_t slice_num1:10;

        /*! new slice_mode */
        uint32_t slice_mode:3;

        /*! 1 => This msg contains slice_mode_change op */
        uint32_t slice_change_en:1;

        /*! Up to 8 groups */
        uint32_t slice_group_id:3;

        /*! 1 => This msg contains entry_mode_change ops */
        uint32_t entry_change_en:1;

        /*! spare */
        uint32_t spare:4;
    } f;
    uint32_t entry;
} bcmptm_wal_slice_mode_info_t;


/*! \brief Info for individual ops to be sent to HW */
typedef struct bcmptm_wal_ops_info_s {
/*! \brief Number of words to be sent to HW for each op
 * - For Write - it includes opcode, addr, data beats
 * - When SCHAN_FIFO mode is disabled, BD will use SCHAN_PIO to drain ops. This
 * param helps BD to delineate individual ops in req_entry_words array.
 * - Dont_care for SLAM */
    uint16_t op_wsize;

/*! \brief Number of hdr words for each op
 * - For Write - it includes opcode, addr
 * - When SCHAN_FIFO mode is disabled, BD will use SCHAN_PIO to drain ops. This
 * param helps BD to delineate individual ops in req_entry_words array.
 * - Dont_care for SLAM */
    uint8_t op_ctrl_wsize;

/*! \brief SID for individual ops
 * - Specifies SID for SLAM cmd
 */
    bcmdrd_sid_t sid;

/*! \brief Dyn_info for individual ops
 * - Specifies index, tbl_inst (pipe) for each op
 * - Specifies Frist_index, tbl_inst (pipe) for SLAM operation
 */
    bcmbd_pt_dyn_info_t dyn_info;



/*! \brief Additional ops_info needed by cbf.
 * - To carry bcmptm_rm_op_t, etc.
 * - Not for SCF use.
 * - Dont_care for SLAM */
    bcmptm_rm_op_t op_type;
} bcmptm_wal_ops_info_t;

/*! \brief Format of msg (group of ops).
 *
 * - Will use offsets for pointers in HA space
 * - Must be able to recreate bcmptm_walr_msg in case of crash
 * - can have additional info to help with release of buffers
 */
typedef struct bcmptm_wal_msg_s {
/*! \brief Refer to defn for wal_slice_mode_info_t */
    bcmptm_wal_slice_mode_info_t smc_word;

/* \brief Index into Undo (prev_data) information */
    uint32_t undo_ops_info_idx;  /* 1st undo_op */

/* \brief 1st undo_word */
    uint32_t undo_words_buf_idx; /* 1st undo_word */

/* \brief Number of undo_words */
    uint32_t undo_num_words;

/* \brief Link to parent trans and thus the link to all msgs of this trans */
    uint32_t trans_idx;

/*! \brief Must be released when msg is sent to HW.
 * Can be really huge when SLAM entry count is large. */
    uint32_t num_words_skipped;

/*! \brief Unique msg ID to help with debug.
 * Non_zero implies msg contents are meaningful.
 */
    uint32_t umsg_idx;

/*! \brief Need for scf to call cbf() BEFORE any op for msg is sent to HW.
 * - Will be set by WAL writer.
 * - Will be cleared by cbf in WAL reader context */
    bool need_cbf_before_msg;

/*! \brief Refer to defn for bcmptm_walr_cmd_t */
    bcmptm_wal_msg_cmd_t cmd;

/*! \brief ptr to req_entry_words (uint32_t array)
 * - For WRITE: it contains {opcode, address, data_words} for multiple op packed
 * consecutively in single DMAable buffer.
 * - For SLAM: it contains {data_words} for multiple locations packed
 * consecutively in single DMAable buffer.
 */
    uint32_t words_buf_idx;

/*! \brief Num of words in req_entry_words array
 * - Specifies number of elements in req_entry_words array.
 * - Contains words_buf_count for SLAM. */
    uint32_t req_entry_wsize;

/*! \brief Num of individual ops in msg.
 * - Specifies number of elements in ops_info array
 *  -# WRITE: Number of elements in ops_info array
 *  -# SLAM: There is only 1 op - so use it to store entry_count. */
    uint32_t num_ops;

/*! \brief Info for ops in msg sent to WAL reader context */
    uint32_t ops_info_idx;

/*! \brief For error checking */
    bool committed;


} bcmptm_wal_msg_t;

/*! \brief Format of status returned once ops for msg are sent to HW */
typedef struct bcmptm_walr_status_s {
/*! \brief rv */
    int rv;

/*! \brief Num of ops that went to HW without problem */
    uint32_t num_good_ops;

/*! \brief Num of req_words that went to HW without problem */
    uint32_t good_req_words_wsize;


} bcmptm_walr_status_t;


/*******************************************************************************
 * External global vars
 */
extern uint64_t bcmptm_wal_words_buf_bpa[BCMDRD_CONFIG_MAX_UNITS];
extern uint32_t *bcmptm_wal_words_buf_bptr[BCMDRD_CONFIG_MAX_UNITS];
extern bcmptm_wal_ops_info_t *bcmptm_wal_ops_info_bptr[BCMDRD_CONFIG_MAX_UNITS];

extern bcmptm_wal_msg_t *bcmptm_wal_msg_bptr[BCMDRD_CONFIG_MAX_UNITS];

extern bool *bcmptm_wal_msg_dq_ready_bptr[BCMDRD_CONFIG_MAX_UNITS];
extern bool *bcmptm_wal_msg_dq_ready_ptr[BCMDRD_CONFIG_MAX_UNITS];

extern uint32_t bcmptm_wal_msg_dq_idx[BCMDRD_CONFIG_MAX_UNITS];
extern bcmptm_wal_msg_t *bcmptm_wal_msg_dq_ptr[BCMDRD_CONFIG_MAX_UNITS];
extern int bcmptm_wal_scf_num_chans[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Function prototypes
 */

extern int
bcmptm_wal_rdr_msg_done(int unit, bcmptm_wal_msg_t *wal_msg_ptr,
                        bcmptm_walr_status_t *scf_status);


extern uint32_t
bcmptm_wal_next_msg_idx(int unit, uint32_t c_msg_idx);

#endif /* BCMPTM_WAL_LOCAL_H */
