/*! \file bcmfp_lt_util.h
 *
 * APIs, defines for FP LT utility functions
 *
 * This file contains APIs, defines for FP common APIs
 * to be used irrespective of FP stage
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

#ifndef BCMFP_LT_UTIL_H
#define BCMFP_LT_UTIL_H

/*******************************************************************************
  Includes
 */
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_lt_internal.h>


/*******************************************************************************
  Defines
 */
/*! Maximum number of words for LT field data (including raw field data). */
#define BCMFP_MAX_WSIZE                     (BCMDRD_PBMP_WORD_MAX)

/*! Maximum uint8 data division for field data (including raw field data) */
#define BCMFP_LT_FIELD_DATA_U8_COUNT        (32)

/*! Maximum uint64 data division for field data (including raw field data) */
#define BCMFP_LT_FIELD_DATA_U64_COUNT       (4)

/*! Maximum number of presel Entry */
#define BCMFP_GROUP_MAX_PRESEL              (32)

/*! Maximum number of Egress Groups */
#define BCMFP_GROUP_MAX_EGRESS              (48)

/*! Maximum number of Vlan Groups */
#define BCMFP_GROUP_MAX_VFP                 (16)

/*!
 * Macro return with exit code rv if expression evaluates
 * to true
 */
#define BCMFP_IF_TRUE_EXIT_WITH_VAL(_expr_, _rv_) \
    do {                                          \
        if ((_expr_) == TRUE) {                   \
            SHR_RETURN_VAL_EXIT((_rv_));          \
        }                                         \
    }while(0)

/*!
 * Macro return with exit code rv if expression evaluates
 * to false
 */
#define BCMFP_IF_FALSE_EXIT_WITH_VAL(_expr_, _rv_) \
    do {                                           \
        if ((_expr_) == FALSE) {                   \
            SHR_RETURN_VAL_EXIT((_rv_));           \
        }                                          \
    }while(0)

#define BCMFP_RET_VAL_ASSIGN(_expr_)               \
    _func_rv = (_expr_)

#define BCMFP_RET_VAL_EXIT()                       \
    SHR_RETURN_VAL_EXIT(_func_rv)

/*! Type conversion from uint64 to uint32 */
#define BCMFP_UINT64_TO_UINT32(_u64val_, _u32val_) \
    (_u32val_) = (uint32_t) (_u64val_)

/*! Type conversion from uint32 to uint64 */
#define BCMFP_UINT32_TO_UINT64(_u32val_, _u64val_) \
    (_u64val_) = (uint64_t) (_u32val_)

/*! Type conversion from uint64 to uint8 */
#define BCMFP_UINT64_TO_UINT8(_u64val_, _u8val_) \
    (_u8val_) = (uint8_t) (_u64val_)

/*! Type conversion from uint8 to uint64 */
#define BCMFP_UINT8_TO_UINT64(_u8val_, _u64val_) \
    (_u64val_) = (uint64_t) (_u8val_)

/*! Type conversion from uint64 to bool */
#define BCMFP_UINT64_TO_BOOL(_u64val_, _bval_)     \
    (_bval_) = (bool) (_u64val_)

/*! Type conversion from bool to uint64 */
#define BCMFP_BOOL_TO_UINT64(_bval_, _u64val_)     \
    (_u64val_) = (uint64_t) (_bval_)

/*! New field added via update */
#define BCMFP_LT_FIELD_NEW    (1 << 0)

/*! Field removed via update */
#define BCMFP_LT_FIELD_REMOVED (1 << 1)


/*******************************************************************************
 Function Prototype
 */
/*!
 * \brief Retrieve number of group configured in the stage
 *
 * This API retrieves the number of groups configured in
 * a FP stage on a particular device
 *
 * \param [in] unit device number for the table of interest.
 * \param [in] stage_id stage in FP
 * \param [out] num_groups pointer to number of group configured
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_group_entry_count_get(int unit,
                               bcmfp_stage_id_t stage_id,
                               size_t *num_groups);

/*!
 * \brief Checks if meter is present
 *
 * This API checks if meter is configured on a particular device
 *
 * \param [in] unit device number for the table of interest.
 * \param [in] stage_id Stage in FP
 * \param [in] meter_id Meter Identifier
 *
 * \return SHR_E_NONE if Meter Exists,
 *         !SHR_E_NONE otherwise failure in obtaining information
 */
extern int
bcmfp_meter_entry_exists(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint64_t meter_id);

/*!
 * \brief Checks if counter is present
 *
 * This API checks if counter is configured on a particular device
 *
 * \param [in] unit device number for the table of interest.
 * \param [in] stage_id Stage in FP
 * \param [in] counter_id counter Identifier
 *
 * \return SHR_E_NONE if counter exists,
 *         !SHR_E_NONE  otherwise failure in obtaining information
 */
extern int
bcmfp_counter_entry_exists(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint64_t counter_id);

/*!
 * \brief Retrieves FP logical table field information
 *
 * This API retrieves field infomation in the FP logical
 * table on a particular device.
 *
 * \param [in] unit device number for the table of interest.
 * \param [in] ltid logical table identifier
 * \param [in] fid logical table field identifier
 * \param [out] field_info Pointer to Buffer to store field metadata
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_info_get(int unit,
                        uint32_t fid,
                        bcmfp_lt_info_t *lt_info,
                        bcmfp_lt_field_info_t **field_info);


/*!
 * \brief Initialize FP logical Table global information structure.
 *
 * This API initialize the FP logical Table global information with all
 * relevent configuration.
 *
 * \param [in] unit device number for the table of interest.
 * \param [in] lt_info Pointer to FP logical Table global information structure.
 *
 * \return SHR_E_NONE success, otherwise failure in initialization.
 */
extern int
bcmfp_lt_lrd_info_init(int unit, bcmfp_lt_info_t *lt_info);

/*!
 * \brief Clean up the FP logical Table global information structure.
 *
 * This API clean up all the configuration in  FP logical Table global
 * information structure which are populated during init time.
 *
 * \param [in] unit device number for the table of interest.
 * \param [in] lt_info Pointer to FP logical Table global information structure.
 *
 * \return SHR_E_NONE success.
 */
extern int
bcmfp_lt_lrd_info_cleanup(int unit, bcmfp_lt_info_t *lt_info);

/*!
 * \brief Validate data for all fields fit in specified width
 *
 * This API validates data for all fields fit in specified width
 * of a logical table on a particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] ltid     Logical table identifier
 * \param [in] opcode   BCMLT opcode
 * \param [in] in       Buffer where all fields and its data
 *                      are present
 *
 * \retval SHR_E_NONE success, otherwise failure in validation
 */
extern int
bcmfp_lt_field_data_validate(int unit,
                             bcmlt_opcode_t opcode,
                             bcmltd_sid_t ltid,
                             const bcmltd_fields_t *in);

/*!
 * \brief Validate data for array field to check duplicate
 *
 * This API validates data for array field to check for duplicity
 * of data for the field of a logical table on a particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] multi_fields     List of array field data
 * \param [in] inmem_multi_fields     List of array field data from inmemory
 *
 * \retval SHR_E_NONE success, otherwise failure in validation
 */
int
bcmfp_lt_field_duplicate_check(int unit,
                               const bcmltd_fields_t *multi_fields,
                               const bcmltd_fields_t *inmem_multi_fields);
/*!
 * \brief Retrieves data for FP logical table field
 *
 * This API retrieves data for scalar field in the FP logical
 * table on a particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] fid      Logical table field identifier
 * \param [in] in       Buffer from where data for field to be retrieved
 * \param [out] data    Pointer to memory where data information
 *                      is to be copied
 *
 * \return SHR_E_NONE success,
 *         SHR_E_NOT_FOUND if field is not present in 'in',
 *         otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_data_get(int unit,
                        uint32_t fid,
                        const bcmltd_fields_t *in,
                        uint64_t *data);

/*!
 * \brief Retrieves data for FP logical table field from the data buffer
 *
 * This API retrieves data for scalar field in the FP logical
 * table on a particular device.
 *
 * \param [in] unit      Device number for the table of interest.
 * \param [in] fid       Logical table field identifier
 * \param [in] data      Buffer from where data for field to be retrieved
 * \param [out] data_val Pointer to memory where data information
 *                      is to be copied
 *
 * \return SHR_E_NONE success,
 *         SHR_E_NOT_FOUND if field is not present in 'in',
 *         otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_data_get_from_data_array(int unit,
                                        uint32_t fid,
                                        const bcmltd_field_t *data,
                                        uint64_t *data_val);

/*!
 * \brief Retrieves data for FP logical table raw field from the data buffer
 *
 * This API retrieves data for scalar field in the FP logical
 * table on a particular device.
 *
 * \param [in] unit      Device number for the table of interest.
 * \param [in] fid       Logical table field identifier
 * \param [in] data      Buffer from where data for field to be retrieved
 * \param [out] multi_fields   Pointer to memory where data information
 *                      is to be copied
 *
 * \return SHR_E_NONE success,
 *         SHR_E_NOT_FOUND if field is not present in 'in',
 *         otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_multi_data_get_from_data_array(int unit,
                                              uint32_t fid,
                                              const bcmltd_field_t *data,
                                              bcmltd_fields_t *multi_fields);

/*!
 * \brief Retrieves key for FP logical table field from the key buffer
 *
 * This API retrieves data for scalar field in the FP logical
 * table on a particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] fid      Logical table field identifier
 * \param [in] key      Buffer from where data for field to be retrieved
 * \param [out] key_val Pointer to memory where data information
 *                      is to be copied
 *
 * \return SHR_E_NONE success,
 *         SHR_E_NOT_FOUND if field is not present in 'in',
 *         otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_key_get_from_key_array(int unit,
                                      uint32_t fid,
                                      const bcmltd_field_t *key,
                                      uint64_t *key_val);

/*!
 * \brief Retrieves multi data for FP logical table field
 *
 * This API retrieves data for array/raw fields in the FP logical
 * table on a particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] ltid     Logical table identifier
 * \param [in] fid      Logical table field identifier
 * \param [in] in       Buffer from where data for field to be retrieved
 * \param [out] multi_fields   Pointer to memory where data information
 *                      is to be copied
 *
 * \return SHR_E_NONE success,
 *         SHR_E_NOT_FOUND if field is not present in 'in',
 *         otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_multi_data_get(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t fid,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *multi_fields);
/*!
 * \brief Allocate memory for bcmltd_fields_t data
 *
 * This API allocates memory for bcmltd_fields_t data
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] ltid     Logical table identifier
 * \param [inout] in    Pointer to bcmltd_fields_t data
*
 * \return SHR_E_NONE success, otherwise failure with information
 */
extern int
bcmfp_lt_bcmltd_fields_buff_alloc(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmltd_fields_t *in);

/*!
 * \brief Free memory allocated for bcmltd_fields_t data
 *
 * This API free memory allocated  for bcmltd_fields_t data
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] fields   Number of fields in bcmltd_fields_t data
 * \param [in] in       Pointer to bcmltd_fields_t data

 * \return SHR_E_NONE success, otherwise failure
 */
extern int
bcmfp_lt_bcmltd_fields_buff_free(int unit,
                                 uint32_t fields,
                                 bcmltd_fields_t *in);

/*!
 * \brief Sets data in field for FP logical table
 *
 * This API sets data for scalar field in the FP logical
 * table on a particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] ltid     Logical table identifier
 * \param [in] fid      Logical table field identifier
 * \param [in] data     Pointer to fid data to be copied to out
 * \param [out] out     Pointer to memory where fid data will be copied
 *
 * \return SHR_E_NONE success,
 *         SHR_E_NOT_FOUND if field is not present in 'out',
 *         otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_data_set(int unit,
                        bcmltd_sid_t ltid,
                        uint32_t fid,
                        uint64_t *data,
                        bcmltd_fields_t *out);

/*!
 * \brief Convert field data to unit32 array
 *
 * This API converts field data to unit32 array
 *
 * \param [in] unit         Device number for the table of interest.
 * \param [in] field        Pointer to bcmltd_field_t Field
 * \param [in] field_info   Logical table field information
 * \param [out] field_val   uint32 Pointer where output is copied
 *
 * \return SHR_E_NONE success,
 *         !SHR_E_NONE if failure in conversion
 */
extern int
bcmfp_lt_field_data_to_uint32_array(int unit,
                                    bcmltd_field_t *field,
                                    bcmfp_lt_field_info_t *finfo,
                                    uint32_t *field_val);

/*!
 * \brief Add new field to LT fields for logical table on a patricular device
 *
 * This API ads new LT field to field list for the given logical table on a
 * particular device. Meomory is already allocated before calling this API.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] ltid     Logical table identifier
 * \param [in] fid      field Identifier
 * \param [in] out_fields Total memory allocated
 * \param [in] im_fields  User provided fields stored in in-mem DB
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
int
bcmfp_lt_field_add(int unit,
                   bcmltd_sid_t ltid,
                   uint32_t fid,
                   uint32_t out_fields,
                   bcmltd_fields_t *imm_fields);

/*!
 * \brief Update LT field data in in-mem DB
 *
 * This API updates LT fields for the given logical table on a particular
 * device. The updated LT field data are written back to in-mem DB
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] ltid     Logical table identifier
 * \param [in] key_ltid Key field Identifier
 * \param [in] in       User provided fields
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_field_update(int unit,
                      bcmltd_sid_t ltid,
                      uint32_t key_fid,
                      const bcmltd_fields_t *in);

#endif /* BCMFP_LT_UTIL_H */
