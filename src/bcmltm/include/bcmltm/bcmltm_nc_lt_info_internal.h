/*! \file bcmltm_nc_lt_info_internal.h
 *
 * Logical Table Manager - Logical Table Information.
 *
 * This file contains definitions to create and retrieve
 * information used to construct the metadata for the FA and EE node cookies.
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

#ifndef BCMLTM_NC_LT_INFO_INTERNAL_H
#define BCMLTM_NC_LT_INFO_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmptm/bcmptm_types.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>


/*!
 * \brief Field Transform Type : Forward Key Transform.
 */
#define BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM    0

/*!
 * \brief Field Transform Type : Forward Value Transform.
 */
#define BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM  1

/*!
 * \brief Field Transform Type : Reverse Key Transform.
 */
#define BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM    2

/*!
 * \brief Field Transform Type : Reverse Value Transform.
 */
#define BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM  3

/*!
 * \brief Initialize LT information data structure.
 *
 * Initialize and populate LT information for given unit.
 * This includes allocating memory required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of table IDs on unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_init(int unit, uint32_t sid_max_count);


/*!
 * \brief Cleanup LT information.
 *
 * Free all allocated memory for given unit.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmltm_nc_lt_info_cleanup(int unit);


/*!
 * \brief Get the table attributes for given table ID.
 *
 * This routine gets the table attribues for given table ID as
 * defined in the logical table definition map file.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] attr Table attributes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_table_attr_get(int unit, bcmlrd_sid_t sid,
                                 bcmltm_table_attr_t *attr);


/*!
 * \brief Get the table type for the given logical table ID.
 *
 * This function gets the table type for given table ID as specified in
 * the logical table definition.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] type Table type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_table_type_get(int unit, bcmlrd_sid_t sid,
                                 bcmltm_table_type_t *type);


/*!
 * \brief Retrieve the groups map for given table.
 *
 * Retrieve the groups map definition for given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] map_ptr Returning pointer to groups map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_nc_lt_info_map_groups_retrieve(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_map_groups_t **map_ptr);


/*!
 * \brief Retrieve the PT group map for given table.
 *
 * Retrieve the PT group map for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_map_ptr Returning pointer to PT group map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_map_pt_retrieve(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_map_pt_t **pt_map_ptr);


/*!
 * \brief Retrieve the PT list metadata for given table.
 *
 * Retrieve the PT list metadata for given table.
 * This is used as the EE node cookie in the EE stage for
 * index PTM operations on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] pt_list_ptr Returning pointer to PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_pt_list_retrieve(int unit, bcmlrd_sid_t sid,
                                   const bcmltm_pt_list_t **pt_list_ptr);


/*!
 * \brief Retrieve the PT keyed information metadata for given table.
 *
 * Retrieve the PT keyed operations information metadata for given table
 * and PTM operation.
 *
 * This is used as the EE node cookie in the EE stage for
 * keyed PTM operations on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] ptm_op PTM operation.
 * \param [in] pt_keyed_ptr Returning pointer for PT keyed op info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_pt_keyed_retrieve(int unit, bcmlrd_sid_t sid,
                                    bcmptm_op_type_t ptm_op,
                              const bcmltm_pt_keyed_op_info_t **pt_keyed_ptr);


/*!
 * \brief Retrieve the FA LT Index metadata for given table.
 *
 * Retrieve the FA LT Index metadata for given table.
 * This is used as the FA node cookie in the FA stage for LTM index
 * resource operations on Index LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] index_ptr Returning pointer to FA Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_fa_lt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                       const bcmltm_fa_lt_index_t **index_ptr);


/*!
 * \brief Retrieve the EE LT Index metadata for given table.
 *
 * Retrieve the EE LT Index metadata for given table.
 * This is used as the EE node cookie in the EE stage for LTM index
 * resource operations on Index LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] index_ptr Returning pointer to EE Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_ee_lt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                 const bcmltm_ee_index_info_t **index_ptr);


/*!
 * \brief Retrieve the LT to PT index WB copy list metadata.
 *
 * Retrieve the LT to PT index WB copy list metadata for given table ID.
 * This is used as the FA node cookie in the FA stage to copy
 * the next index into the corresponding PTM WB during a TRAVERSE
 * operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] list_ptr Returning pointer to LT to PT copy list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_lt_to_pt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                    const bcmltm_wb_copy_list_t **list_ptr);


/*!
 * \brief Retrieve the field mapping metadata.
 *
 * Retrieve the field mapping metadata for given table ID.
 * This is used as the FA node cookie in the FA stage for PTM operations
 * on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] ptm_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_ptm_mapping_retrieve(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_field_type_t field_type,
                                       bcmltm_field_dir_t field_dir,
                                       const bcmltm_to_ptm_mapping_t
                                       **ptm_mapping_ptr);


/*!
 * \brief Retrieve the key field mapping metadata for IwA LTs.
 *
 * Retrieve the key field mapping metadata for direct maps on given IwA LT.
 * This is used as the FA node cookie in the FA stage for INSERT operations
 * on Index with Allocation LTs.
 *
 * This routine can return these field lists:
 * - Keys required during INSERT on IwA LTs.
 * - Keys optional during INSERT on IwA LTs (allocated if absent).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] required If TRUE, return the required key fields;
 *                      otherwise, return the optional key fields.
 * \param [out] ptm_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_ptm_mapping_keys_alloc_retrieve(int unit,
                                                  bcmlrd_sid_t sid,
                                                  bool required,
                                                  const bcmltm_to_ptm_mapping_t
                                                  **ptm_mapping_ptr);

/*!
 * \brief Retrieve the fixed fields metadata.
 *
 * Retrieve the fixed field mapping metadata for given table ID.
 * This is used as the FA node cookie in the FA stage for PTM operations
 * on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fixed_fields_ptr Returning pointer to fixed fields metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_info_fixed_fields_retrieve(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_fixed_field_list_t
                                        **fixed_fields_ptr);

/*!
 * \brief Get the number of fields which requires field validation
 *        in a LT.
 *
 * Get the number of fields which requires field validation in a LT.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type The type of field KEY or VALUE.
 * \param [out] num Number of fields requires field validation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_validate_field_num_get(int unit, bcmlrd_sid_t sid,
                                     bcmltm_field_type_t field_type,
                                     uint32_t *num);

/*!
 * \brief Retrieve the LTA Field Validation parameters.
 *
 * Retrieve the LTA Field Validation parameters.
 * This is used as the FA node cookie in the FA stage for the
 * Field Validation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Field Type.
 * \param [in] fld_idx Index of Field.
 * \param [out] table_params_ptr Returning pointer to LTA
 *              Field Validation parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_validate_params_retrieve(int unit, bcmlrd_sid_t sid,
                                       bcmltm_field_type_t field_type,
                                       uint32_t fld_idx,
                                       const bcmltm_lta_validate_params_t
                                       **table_params_ptr);

/*!
 * \brief Retrieve the LTA src field list for Field Validation.
 *
 * Retrieve the LTA in field list for Field Validation.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Field Type.
 * \param [in] fld_idx Index of Field.
 * \param [out] fields_ptr Field src field list pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_validate_src_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                           bcmltm_field_type_t field_type,
                                           uint32_t fld_idx,
                                           const bcmltm_lta_field_list_t
                                           **fields_ptr);

/*!
 * \brief Retrieve the LTA copy field list for Field Validation.
 *
 * Retrieve the LTA in field list for Field Validation copy from
 * a given LTA reverse transform.  This is required on UPDATE to
 * use the existing field values when combined with any changed
 * values to validate the fields.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Field Type.
 * \param [in] validate_idx Index of validate handler.
 * \param [in] from_xfrm_type From list Field Transform Type.
 * \param [in] from_xfrm_idx From list index of transform handler.
 * \param [out] fields_ptr Field src field list pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_validate_copy_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                            bcmltm_field_type_t field_type,
                                            uint32_t validate_idx,
                                            int from_xfrm_type,
                                            uint32_t from_xfrm_idx,
                                            const bcmltm_lta_field_list_t
                                            **fields_ptr);

/*!
 * \brief Get the number of Field Transform Handler.
 *
 * Get the number of Field Transform Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field Transform Type.
 * \param [out] num Number of Field Transform.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_xfrm_num_get(int unit, bcmlrd_sid_t sid,
                           int xfrm_type,
                           uint32_t *num);

/*!
 * \brief Retrieve the LTA Field Transform parameters.
 *
 * Retrieve the LTA Field Transform parameters.
 * This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Field Transform Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field Transform Type.
 * \param [in] xfrm_idx of transform handler.
 * \param [out] table_params_ptr Returning pointer to LTA
 *              Field Transform parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_xfrm_params_retrieve(int unit, bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   uint32_t xfrm_idx,
                                   const bcmltm_lta_transform_params_t
                                   **table_params_ptr);

/*!
 * \brief Retrieve the LTA Field Transform in field list.
 *
 * Retrieve the LTA in field list which is API LT fields list.
 * In forward transform case, this in field list is src field.
 * In reverse transform case, this in field list is dst field.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field Transform Type.
 * \param [in] xfrm_idx The index of transform handler.
 * \param [out] fields_ptr Field Transform in field list pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_xfrm_in_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                      int xfrm_type,
                                      uint32_t xfrm_idx,
                                      const bcmltm_lta_field_list_t
                                      **fields_ptr);

/*!
 * \brief Retrieve the LTA Field Transform dst field list.
 *
 * Retrieve the LTA dst field list which is PTM fields list.
 * In forward transform case, this out field list is dst field.
 * In reverse transform case, this out field list is src field.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field Transform Type.
 * \param [in] xfrm_idx of transform handler.
 * \param [out] fields_ptr Field Transform dst field list pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_xfrm_out_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                       int xfrm_type,
                                       uint32_t xfrm_idx,
                                       const bcmltm_lta_field_list_t
                                       **fields_ptr);

/*!
 * \brief Retrieve the LTA Field LTA Copy field list.
 *
 * Retrieve the LTA Copy field list which is API fields list.
 * The only current use case is reverse transform output to
 * forward transform input.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field Transform Type.
 * \param [in] xfrm_idx Index of transform handler.
 * \param [in] from_xfrm_type From list Field Transform Type.
 * \param [in] from_xfrm_idx From list index of transform handler.
 * \param [out] fields_ptr LTA Copy field list pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_xfrm_copy_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                        int xfrm_type,
                                        uint32_t xfrm_idx,
                                        int from_xfrm_type,
                                        uint32_t from_xfrm_idx,
                                        const bcmltm_lta_field_list_t
                                        **fields_ptr);

/*!
 * \brief Retrieve the Table Commit handler list.
 *
 * This routine retrieves the Table Commit Handler list for the given
 * logical table.  If the logical table does not have any Table Commit
 * handlers, the returned pointer is NULL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] tc_list_ptr Returning pointer to Table Commit handler list,
 *                         or NULL if table does not have any.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_table_commit_list_retrieve(int unit, bcmlrd_sid_t sid,
                               const bcmltm_table_commit_list_t **tc_list_ptr);

/*!
 * \brief Retrieve the LTA CTH table handler.
 *
 * This routine retrieves the Cusotm Table Handler interface for the given
 * logical table.  If the logical table does not have CTH implementation,
 * the returned pointer is NULL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] handler_ptr Returning pointer to CTH handler, or NULL if
 *                          table is not CTH.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_cth_handler_retrieve(int unit, bcmlrd_sid_t sid,
                                   const bcmltd_table_handler_t **handler_ptr);
/*!
 * \brief Retrieve the LTA CTH table parameters.
 *
 * Retrieve the LTA CTH table parameters.
 * This is used as the EE node cookie in the EE stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] table_params_ptr Returning pointer to LTA CTH table parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_cth_table_params_retrieve(int unit, bcmlrd_sid_t sid,
                                        const bcmltm_lta_table_params_t
                                        **table_params_ptr);

/*!
 * \brief Retrieve the LTA CTH Input field list.
 *
 * Retrieve the LTA CTH Input field list.
 * This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to LTA CTH Input field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_cth_in_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                     const bcmltm_lta_field_list_t
                                     **fields_ptr);

/*!
 * \brief Retrieve the LTA CTH Input key field list.
 *
 * Retrieve the LTA CTH Input key field list.
 * This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to LTA CTH Input key field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_cth_in_key_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                         const bcmltm_lta_field_list_t
                                         **fields_ptr);

/*!
 * \brief Retrieve the LTA CTH Output field list.
 *
 * Retrieve the LTA CTH Output field list.
 * This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to LTA CTH Output field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_cth_out_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_lta_field_list_t
                                      **fields_ptr);

/*!
 * \brief Retrieve the LTM group FA LT Index metadata for given table.
 *
 * Retrieve the LTM FA LT Index metadata for given table.
 * This is used as the FA node cookie in the FA stage for LTM based
 * LTs (such as config or stats) to access the LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] index_ptr Returning pointer to FA Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_ltm_fa_lt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_fa_lt_index_t **index_ptr);

/*!
 * \brief Retrieve the LTM group key.
 *
 * This routine retrieves the LTM group key for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ltm_key_ptr Returning pointer to LTM group key.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_ltm_key_retrieve(int unit, bcmlrd_sid_t sid,
                              const bcmltm_to_ptm_mapping_t **ltm_key_ptr);

/*!
 * \brief Retrieve the LTM group field spec list.
 *
 * This routine retrieves the LTM group key for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fs_list_ptr Returning pointer to field spec.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_ltm_fs_list_retrieve(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_field_spec_list_t **fs_list_ptr);

/*!
 * \brief Display LT info contents for given logical table ID.
 *
 * Display LT info contents for given logical table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 */
extern void
bcmltm_nc_lt_info_dump(int unit, bcmlrd_sid_t sid);


/*!
 * \brief Display LT info contents for all logical tables in given unit.
 *
 * Display LT info contents for all logical tables in given unit.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmltm_nc_lt_info_all_dump(int unit);


/*!
 * \brief  Display LT info contents for given logical table name.
 *
 * Display LT info contents for given logical table name.
 *
 * \param [in] unit Unit number.
 * \param [in] name Logical table name.
 */
extern void
bcmltm_nc_lt_info_dump_by_name(int unit, const char *name);

#endif /* BCMLTM_NC_LT_INFO_INTERNAL_H */
