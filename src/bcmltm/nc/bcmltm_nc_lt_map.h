/*! \file bcmltm_nc_lt_map.h
 *
 * Logical Table Manager - Logical Table Mapping Information Definitions.
 *
 * This file contains routines that parse the LRD mapping information.
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

#ifndef BCMLTM_NC_LT_MAP_H
#define BCMLTM_NC_LT_MAP_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>


/*!
 * \brief Parse the LRD table attributes for a given LT.
 *
 * This routine parses the table attributes defined in the LT
 * map file.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] attr Table attributes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_map_attr_parse(int unit, bcmlrd_sid_t sid,
                            bcmltm_table_attr_t *attr);


/*!
 * \brief Parse the LRD map groups for a given LT.
 *
 * Parse the LRD map groups for a given LT.
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [out] groups Group map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_map_groups_parse(int unit, bcmlrd_sid_t sid,
                              const bcmltm_table_attr_t *attr,
                              bcmltm_map_groups_t *groups);

/*!
 * \brief Free the given map groups memory.
 *
 * Free the given map groups memory.
 *
 * \param [in] groups Pointer to memory to free.
 */
extern void
bcmltm_nc_lt_map_groups_cleanup(bcmltm_map_groups_t *groups);

/*!
 * \brief Get the LT mapping information for a given table ID.
 *
 * Get the LT mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_CUSTOM
 *     Map Entry Type: { BCMLRD_MAP_ENTRY_MAPPED_VALUE,
 *                       BCMLRD_MAP_ENTRY_FIXED_VALUE }
 *
 * Returning arrays will be allocated the max possible size in order
 * to walk the LRD map entries just once.  A later stage will
 * copy this information into the static data structure using
 * the right array sizes (the number of elements will be known by the
 * end of parsing).
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_map_group PT map groups for the LT.
 * \param [out] lt_map Returning LT mapping entries information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_map_entries_pt_parse(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_table_attr_t *attr,
                                  const bcmltm_map_pt_t *pt_map_group,
                                  bcmltm_map_entries_pt_t *lt_map);

/*!
 * \brief Cleanup LT mapping information.
 *
 * Free any internal memory allocated for the LT mapping information.
 *
 * \param [out] lt_map LT mapping information to clean.
 */
extern void
bcmltm_nc_lt_map_entries_pt_cleanup(bcmltm_map_entries_pt_t *lt_map);

/*!
 * \brief Get the Field Validation mapping information for a given LT.
 *
 * Get the LT Field Valiation mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_VALIDATION
 *     Map Entry Type: BCMLRD_MAP_ENTRY_KEY/VALUE_FIELD_VALIDATION
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] map_group FV map groups for the LT.
 * \param [out] lt_map Returning LT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_map_entries_fv_parse(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_map_fv_t *map_group,
                                  bcmltm_map_entries_fv_t *lt_map);


/*!
 * \brief Cleanup LT Field Valiation map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * Field Validation map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
extern void
bcmltm_nc_lt_map_entries_fv_cleanup(bcmltm_map_entries_fv_t *lt_map);


/*!
 * \brief Get the Table Commit mapping information for a given LT.
 *
 * Get the LT Table Commit mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_TABLE_COMMIT
 *     Map Entry Type: BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_map Returning LT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_map_entries_tc_parse(int unit, bcmlrd_sid_t sid,
                                  bcmltm_map_entries_tc_t *lt_map);


/*!
 * \brief Cleanup LT Table Commit map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * Table Commit map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
extern void
bcmltm_nc_lt_map_entries_tc_cleanup(bcmltm_map_entries_tc_t *lt_map);


/*!
 * \brief Get the Custom Table Handler mapping information for a given LT.
 *
 * Get the LT Custom Table Handler mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_CUSTOM
 *     Map Entry Type: BCMLRD_MAP_ENTRY_TABLE_HANDLER
 *
 * This support table definitions with only only 1 custom table handler
 * group and 1 map entry handler in the group per LT.
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_map Returning LT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_map_entries_cth_parse(int unit, bcmlrd_sid_t sid,
                                   bcmltm_map_entries_cth_t *lt_map);


/*!
 * \brief Cleanup LT Customer Table Handler map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * Custom Table Handler map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
extern void
bcmltm_nc_lt_map_entries_cth_cleanup(bcmltm_map_entries_cth_t *lt_map);

/*!
 * \brief Get the LTM group mapping information for a given LT.
 *
 * Get the LTM group mapping information for a given table ID.
 *
 * This routine parses the LRD map group information for:
 *     Map Group Kind: BCMLRD_MAP_LTM
 *
 * This support table definitions with only only 1 LTM group.
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_map Returning LT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lt_map_entries_ltm_parse(int unit, bcmlrd_sid_t sid,
                                   bcmltm_map_entries_ltm_t *lt_map);


/*!
 * \brief Cleanup LTM group map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * LTM group map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
extern void
bcmltm_nc_lt_map_entries_ltm_cleanup(bcmltm_map_entries_ltm_t *lt_map);

#endif /* BCMLTM_NC_LT_MAP_H */
