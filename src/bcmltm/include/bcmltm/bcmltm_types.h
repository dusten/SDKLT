/*! \file bcmltm_types.h
 *
 * Logical Table Manager Types.
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

#ifndef BCMLTM_TYPES_H
#define BCMLTM_TYPES_H

#include <shr/shr_error.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltm/lt_types.h>

/*!
 * \brief LT STATS catagories.
 *
 * Translation of the LRD values because of prior implementation.
 */

typedef enum bcmltm_field_stats_e {
    BCMLRD_FIELD_STATS_LT_ID = TABLE_STATSt_TABLE_IDf,
    BCMLRD_FIELD_LT_ERROR_COUNT = TABLE_STATSt_TABLE_ERROR_CNTf,
    BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT = \
           TABLE_STATSt_FIELD_SIZE_ERROR_CNTf,
    BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT = \
           TABLE_STATSt_FIELD_LIST_ERROR_CNTf,
    BCMLRD_FIELD_VALIDATE_ERROR_COUNT = TABLE_STATSt_VALIDATE_ERROR_CNTf,
    BCMLRD_FIELD_TRANSFORM_ERROR_COUNT = TABLE_STATSt_TRANSFORM_ERROR_CNTf,
    BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT = \
           TABLE_STATSt_TABLE_HANDLER_ERROR_CNTf,
    BCMLRD_FIELD_LT_INSERT_COUNT = TABLE_STATSt_TABLE_INSERT_CNTf,
    BCMLRD_FIELD_LT_LOOKUP_COUNT = TABLE_STATSt_TABLE_LOOKUP_CNTf,
    BCMLRD_FIELD_LT_DELETE_COUNT = TABLE_STATSt_TABLE_DELETE_CNTf,
    BCMLRD_FIELD_LT_UPDATE_COUNT = TABLE_STATSt_TABLE_UPDATE_CNTf,
    BCMLRD_FIELD_LT_TRAVERSE_COUNT = TABLE_STATSt_TABLE_TRAVERSE_CNTf,
    BCMLRD_FIELD_LT_INSERT_ERROR_COUNT = \
           TABLE_STATSt_TABLE_INSERT_ERROR_CNTf,
    BCMLRD_FIELD_LT_LOOKUP_ERROR_COUNT = \
           TABLE_STATSt_TABLE_LOOKUP_ERROR_CNTf,
    BCMLRD_FIELD_LT_DELETE_ERROR_COUNT = \
           TABLE_STATSt_TABLE_DELETE_ERROR_CNTf,
    BCMLRD_FIELD_LT_UPDATE_ERROR_COUNT = \
           TABLE_STATSt_TABLE_UPDATE_ERROR_CNTf,
    BCMLRD_FIELD_LT_TRAVERSE_ERROR_COUNT = \
           TABLE_STATSt_TABLE_TRAVERSE_ERROR_CNTf,
    BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT = \
           TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf,
    BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT = \
           TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf,
    BCMLRD_FIELD_STATS_NUM = TABLE_STATSt_FIELD_COUNT
} bcmltm_field_stats_t;

/*!
 * \brief Logical Table Manager Types.
 *
 * The public types required by the LTM module which may be employed
 * by other modules upstream.
 */

/*! Maximum number of bits for a single field value (Field ID, index). */
#define BCMLTM_FIELD_SIZE_BITS           64

/*!
 * \brief LTM field list.
 *
 * This data structure defines a list of fields in an entry.
 */
typedef bcmlt_field_list_t bcmltm_field_list_t;

/*!
 * \brief LTM field list allocation callback.
 *
 * This function is provided by a higher layer than the LTM to create
 * field list elements which may be employed to return data
 * for operations which generate output field values.
 */
typedef bcmltm_field_list_t * (*bcmlt_field_list_alloc_f)(void);

/*!
 * \brief Logical Table Entry opcode.
 *
 * This data structure is a union of logical table opcode and
 * pass through opcode.
 */
typedef union {
    /*! Logical table opcode. */
    bcmlt_opcode_t lt_opcode;

    /*! Passthrough opcode. */
    bcmlt_pt_opcode_t pt_opcode;
} bcmltm_combined_opcode_t;


/*!
 * \name LTM per-entry flags.
 *
 * Flags and options for additional context in a LTM entry.
 */
/*! \{ */
/*! PT Pass Thru. */
#define BCMLTM_ENTRY_FLAG_PASSTHRU 0x1

/*! Retrieve contents directly from HW (bypass PT cache). */
#define BCMLTM_ENTRY_FLAG_HW_GET   0x2

/*! Initiate a Traverse operation. */
#define BCMLTM_ENTRY_FLAG_TRAVERSE_START   0x4

/*! Exclude default values in lookup/traverse operations */
#define BCMLTM_ENTRY_FLAG_EXC_DEF   0x8
/*! \} */


/*!
 * \brief Logical Table Manager Entry.
 *
 * The data structure which contains the information necessary to
 * fully specify a Logical Table Operation.
 */
typedef struct bcmltm_entry_s {
    /*! Unique rollup counter identify this entry. */
    uint32_t                  entry_id;

    /*!
     * Transaction ID for the LT entry specificaion of this operation.
     * Note that multiple entries may have the same ID.
     * That will boundle the entries into atomic operation.
     */
    uint32_t                  trans_id;

    /*! ID selecting the LT in which this entry operation will take place. */
    uint32_t                  table_id;

    /*! Device identifier. */
    int                       unit;

    /*! Entry detail flags for special indications. */
    uint32_t                  flags;

    /*!
     * Entry operation code. The pt flag determines which field
     * from the union is relevant.
     */
    bcmltm_combined_opcode_t  opcode;

    /*!
     * Externally provided allocation function for adding field
     * elements to the out_fields list when the operation retrieves
     * field values from the device or PT cache.
     */
    bcmlt_field_list_alloc_f  field_alloc_cb;

    /*!
     * List of field values provided by the application.
     * List is NULL terminated.
     */
    bcmltm_field_list_t       *in_fields;

    /*!
     * An application-allocated list of field values for the return
     * of data from the LTM. List is NULL terminated.
     */
    bcmltm_field_list_t       *out_fields;

    /*! Link pointer to the next entry in the list. */
    struct bcmltm_entry_s     *next;
} bcmltm_entry_t;


/*! Depth of KEY/DATA SID and INDEX arrays */
#define BCMLTM_TABLE_OP_PT_INFO_MAX_INDEX 4
/*!
 * \brief PT status per LT operation.
 *
 * This structure contains the status for a single PT operation.
 */
typedef struct bcmltm_table_op_pt_info_s {
    /*! Enum of Logical table under operation. */
    uint32_t lt_id;

    /*! Aggregate view check. */
    bool aggregate_view;

    /*! Instance. */
    uint32_t pt_instance;

    /*! Valid entries in pt_sid array */
    uint32_t pt_sid_cnt;

    /*! Physical SID. */
    uint32_t pt_sid[BCMLTM_TABLE_OP_PT_INFO_MAX_INDEX];

    /*! Valid entries in pt_index array */
    uint32_t pt_index_cnt;

    /*! Physical index. */
    uint32_t pt_index[BCMLTM_TABLE_OP_PT_INFO_MAX_INDEX];

    /*! Valid entries in pt_sid_data array */
    uint32_t pt_sid_data_cnt;

    /*! Physical SID data only. */
    uint32_t pt_sid_data[BCMLTM_TABLE_OP_PT_INFO_MAX_INDEX];

    /*! Valid entries in pt_index_data array */
    uint32_t pt_index_data_cnt;

    /*! Physical index data only. */
    uint32_t pt_index_data[BCMLTM_TABLE_OP_PT_INFO_MAX_INDEX];

} bcmltm_table_op_pt_info_t;

/*!
 * \brief Define transaction operation status and callback.
 *
 * All logical table transaction operations result in a status
 * to be returned to the caller. This callback function is invoked
 * upon completion of a transaction.  The LTM module passes this
 * callback onward to the PTM so that module may notify when all portions
 * of a modelled transaction are complete.
 *
 * \param [in] event Indicate the reason the callback being called.
 * \param [in] trans_id Transaction ID of this notification.
 * \param [in] status Operation status indication.
 * \param [in] context Cookie supplied when callback registered.
 *
 * \return void None.
 */
typedef void (*bcmltm_trans_cb_f)(bcmlt_notif_option_t event,
                                  uint32_t trans_id,
                                  shr_error_t status,
                                  void *context);

#endif /* BCMLTM_TYPES_H */
