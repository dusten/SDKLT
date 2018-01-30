/*! \file bcmptm_internal.h
 *
 * Utils, defines shared across BCMPTM sub-modules
 *
 * This file contains utils, defines shared across PTM sub-modules
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

#ifndef BCMPTM_INTERNAL_H
#define BCMPTM_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
 * Defines
 */
/*! Size of buffer allocated to store entry data words for any PT.
 */
#define BCMPTM_MAX_PT_ENTRY_WORDS 40     /* 1280 bit wide mem */

/*! Size of buffer allocated to store field words of an entry for any PT
 */
#define BCMPTM_MAX_PT_FIELD_WORDS BCMPTM_MAX_PT_ENTRY_WORDS

/*! Size of buffer allocated to store words received from LTM for modeled req
 */
#define BCMPTM_MAX_MREQ_ENTRY_WORDS 40     /* 1280 bit wide mem */


/*! Convert number of bits to number of 32 bit words.
 */
#define BCMPTM_BITS2WORDS(x) (((x) + 31) / 32)

/*! Convert number of bytes to number of 32 bit words.
 * - Example usage: determine num of 32 bit words required to store elements of
 * size uint8_t - DFID, etc
 */
#define BCMPTM_BYTES2WORDS(x) (((x) + 3) / 4)

/*! Convert number of double_bytes to number of 32 bit words.
 * - Example usage: determine num of 32 bit words required to store elements of
 * size uint16_t - LTID, DFID, etc
 */
#define BCMPTM_DBYTES2WORDS(x) (((x) + 1) / 2)

/*! Debug macro to allow force display of verbose msgs from PTM.
 * \n For debug, we can change RHS to LOG_ERROR.
 */
#define BCMPTM_LOG_VERBOSE LOG_VERBOSE

/*! Macro to set, clr, test a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_OP(vmap, index, BOOL_OP) \
        ((vmap) BOOL_OP (1 << ((index) % 32)))

/*! Macro to set a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_SET(vmap, index) BCMPTM_WORD_BMAP_OP(vmap, index, |=)

/*! Macro to clr a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_CLR(vmap, index) BCMPTM_WORD_BMAP_OP(vmap, index, &= ~)

/*! Macro to test a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_TST(vmap, index) BCMPTM_WORD_BMAP_OP(vmap, index, &)

/*! Macro to test if SHR error code implies SER event
 */
#define BCMPTM_SHR_FAILURE_SER(_expr) ((_expr) == SHR_E_FAIL)

/*! Max number of Mode_Change groups supported per unit
 */
#define BCMPTM_RM_MC_GROUP_COUNT_MAX 4


/*! HA signatures for PTCACHE_SINFO */
#define BCMPTM_HA_SIGN_PTCACHE_SINFO         0xABCA0001
/*! HA signatures for PTCACHE_VINFO */
#define BCMPTM_HA_SIGN_PTCACHE_VINFO         0xABCA0002

/*! HA signatures for WAL_TRANS */
#define BCMPTM_HA_SIGN_WAL_TRANS             0xABCB1231
/*! HA signatures for WAL_MSG */
#define BCMPTM_HA_SIGN_WAL_MSG               0xABCB1232
/*! HA signatures for WAL_OPS_INFO */
#define BCMPTM_HA_SIGN_WAL_OPS_INFO          0xABCB1233
/*! HA signatures for WAL_UNDO_OPS_INFO */
#define BCMPTM_HA_SIGN_WAL_UNDO_OPS_INFO     0xABCB1234
/*! HA signatures for WAL_UNDO_WORDS_BUF */
#define BCMPTM_HA_SIGN_WAL_UNDO_WORDS_BUF    0xABCB1235
/*! HA signatures for WAL_WSTATE */
#define BCMPTM_HA_SIGN_WAL_WSTATE            0xABCB1236
/*! HA signatures for WAL_WORDS_BUF */
#define BCMPTM_HA_SIGN_WAL_WORDS_BUF         0xABCB1237

/*! HA signatures for LT_OFFSET_INFO */
#define BCMPTM_HA_SIGN_LT_OFFSET_INFO        0xABCD0001


/*******************************************************************************
 * Typedefs
 */
/*! Sub-phases */
typedef enum {
    BCMPTM_SUB_PHASE_1 = 1, /* 1st scan of LTID/SID. Notify RM */
    BCMPTM_SUB_PHASE_2 = 2, /* 2nd scan of LTID/SID. RM will return offset */
    BCMPTM_SUB_PHASE_3 = 3  /* No scan of LTID/SID. RM can do extra init */
} bcmptm_sub_phase_t;

/*! HA sub-module IDs */
typedef enum {
    BCMPTM_HA_SUBID_PTCACHE_SINFO = 1,
    BCMPTM_HA_SUBID_PTCACHE_VINFO = 2,
    BCMPTM_HA_SUBID_LT_INFO_TABLE = 3,
    BCMPTM_HA_SUBID_CCI_INFO = 7,
    BCMPTM_HA_SUBID_CCI_MAPID = 8,
    BCMPTM_HA_SUBID_CCI_CTR_CACHE = 9,
    BCMPTM_HA_SUBID_RMHASH_TRANS_REC = 10,
    BCMPTM_HA_SUBID_BUCKET_STATE_INFO = 11,
    BCMPTM_HA_SUBID_ENTRY_NODE_INFO = 12,
    BCMPTM_HA_SUBID_RMHASH_USD_ENT_CTR = 13,
    BCMPTM_HA_SUBID_RMHASH_UNDO_LIST = 14,
    BCMPTM_HA_SUBID_RMTCAM_BKT_STATE = 16,
    BCMPTM_HA_SUBID_RM_TCAM = 17,
    BCMPTM_HA_SUBID_RM_TCAM_BKP = 18,
    BCMPTM_HA_SUBID_WAL_TRANS      = 32,
    BCMPTM_HA_SUBID_WAL_MSG        = 33,
    BCMPTM_HA_SUBID_WAL_WSTATE     = 34,
    BCMPTM_HA_SUBID_WAL_OPS_INFO   = 35,
    BCMPTM_HA_SUBID_WAL_WORDS_BUF  = 36,
    BCMPTM_HA_SUBID_UNDO_WORDS_BUF = 37,
    BCMPTM_HA_SUBID_UNDO_OPS_INFO  = 38,
    BCMPTM_HA_SUBID_RM_ALPM        = 39,
    BCMPTM_HA_SUBID_RM_ALPM_L1     = 40,
    BCMPTM_HA_SUBID_SER_SLICES_INFO = 41,
    BCMPTM_HA_SUBID_SER_OVERLAY_BITMAP = 42,
    BCMPTM_HA_SUBID_MAX            = 80
} bcmptm_ha_sub_id_t;

/*! Transaction commands for rsrc_mgr */
typedef enum {
    BCMPTM_TRANS_CMD_IDLE_CHECK = 1, /* only for rsrc_mgr */
    BCMPTM_TRANS_CMD_COMMIT = 2,
    BCMPTM_TRANS_CMD_ABORT = 3,
    BCMPTM_TRANS_CMD_CSEQ_ID_STORE = 4 /* only for cmdproc */
} bcmptm_trans_cmd_t;

/*! Tcam slice modes from serc perspective */
typedef enum {
    BCMPTM_TCAM_SLICE_INVALID = 0, /* Not applicable */
    BCMPTM_TCAM_SLICE_DISABLED = 1, /* Disabled */
    BCMPTM_TCAM_SLICE_NARROW = 2, /* Narrow */
    BCMPTM_TCAM_SLICE_WIDE = 3, /* Wide */
} bcmptm_rm_slice_mode_t;

/*! \brief Information passed by RM when changing slice_mode */
typedef struct bcmptm_rm_slice_change_s {
    /*! slice num. Use -1 to indicate invalid slice. */
    int slice_num0;

    /*! slice num. Use -1 to indicate invalid slice */
    int slice_num1;

    /*! New slice_mode */
    bcmptm_rm_slice_mode_t slice_mode;

    /*! Helps identify RM that manages these slices */
    uint32_t slice_group_id;
} bcmptm_rm_slice_change_t;

/*! Resource manager op types */
typedef enum {
    BCMPTM_RM_OP_NORMAL = 0, /* Nothing special */
    BCMPTM_RM_OP_EM_NARROW = 1, /* Entry mode narrow */
    BCMPTM_RM_OP_EM_WIDE = 2, /* Entry mode wide */
    BCMPTM_RM_OP_SLICE_MODE_CHANGE = 3, /* Slice mode change */
} bcmptm_rm_op_t;

/*! \brief Information passed to SER logic when changing entry mode */
typedef struct bcmptm_rm_entry_mode_change_s {
    /*! Index into narrow, wide view */
    uint32_t index;

    /*! Helps identify narrow, wide view.
     * When WAL reader calls entry_mode_change() API, it will NOT provide
     * op_type = NORMAL or SLICE_MODE_CHANGE.
     * If it does, then SERC logic must return SHR_E_UNAVAIL to caller. */
    bcmptm_rm_op_t op_type;
} bcmptm_rm_entry_mode_change_t;


/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Allocate and initialize lt_info_table for a unit
 * \n Must be called every time a new unit is attached and table sizes have been
 * determined, configured.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_lt_info_table_init(int unit, bool warm);

/*!
 * \brief Deallocate lt_info_table for a unit
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_lt_info_table_cleanup(int unit);

/*!
 * \brief Read lt_state_offset from lt_info_table
 *
 * \param [in] unit Logical device id
 * \param [in] req_ltid Enum to specify LTid
 *
 * \param [out] lt_state_offset Offset into lt_state managed by rsrc_mgr
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Offset for requested LTid was not found
 */
extern int
bcmptm_lt_state_offset_read(int unit, bcmltd_sid_t req_ltid,
                            uint32_t *lt_state_offset);

/*!
 * \brief Read a mem or reg internal to SOC - Interactive path.
 * \n Intended for CCI only.
 * \n By default data is read from PTCache for cacheable PTs.
 * \n Will first try reading from PTcache and then from HW.
 * \n Will NOT do TCAM(x,y) to (k,m) conversion.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_entry_words Array large enough to hold read data
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ireq_read_cci(int unit,
                     uint32_t flags,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                     void *pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,

                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags);

/*!
  \brief Init modeled path data structs
  - Alloc, init modeled path data structs

  \param [in] unit Logical device id
  \param [in] warm TRUE implies warmboot

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_init(int unit, bool warm);

/*!
  \brief Stop modeled path
  - Will wait for all msgs in WAL to drain, and then disable WAL rdr thread

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_stop(int unit);

/*!
  \brief Cleanup modeled path
  - De-alloc modeled path data structs

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_cleanup(int unit);

/*!
  \brief Alloc mem for rsrc_mgr traverse

  \param [in] unit Logical device id
  \param [in] sz Number of bytes to alloc
  \param [in] s Text string for tracking purposes
  \param [in] ptr Pointer to allocated mem or NULL on error

  \retval SHR_E_NONE Success */
extern int
bcmptm_rm_traverse_alloc(int unit, size_t sz, char *s, void **ptr);

/*!
  \brief Free mem for rsrc_mgr traverse

  \param [in] unit Logical device id
  \param [in] ptr Pointer to mem block to be freed

  \retval SHR_E_NONE Success */
extern int
bcmptm_rm_traverse_free(int unit, void *ptr);

#endif /* BCMPTM_INTERNAL_H */
