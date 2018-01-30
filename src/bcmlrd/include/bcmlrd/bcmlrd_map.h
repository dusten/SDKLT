/*! \file bcmlrd_map.h
 *
 * Logical Table Mapping
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

#ifndef BCMLRD_MAP_H
#define BCMLRD_MAP_H

#include <sal/sal_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_internal.h>

/*!
 * \brief Get mapping info for a table and device.
 *
 * Get the mapping info for a table and device.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  sid      Table ID.
 * \param [out] map      Map info.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit, invalid sid, sid not defined for
 * device, mapping not available, etc.
 */
extern int
bcmlrd_map_get(int unit, bcmlrd_sid_t sid, bcmlrd_map_t const **map);

/*!
 * \brief List of all tables referenced by a logical table.
 *
 * Get a list of all  tables referenced by a logical table.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  sid      Logical Table ID.
 * \param [in]  count    Number of Destination Table Desc allocated.
 * \param [out] dst      List of Destination Table Descriptors.
 * \param [out] actual   Number of Table Descriptors returned.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit, invalid sid, sid not defined for
 * device, mapping not available, etc.
 */

extern int
bcmlrd_mapped_dest_get(int unit,
                       bcmlrd_sid_t sid,
                       size_t count,
                       bcmlrd_table_desc_t *dst,
                       size_t *actual);

/*!
 * \brief Get number of table init IDs.
 *
 * Get the number of table init IDs for the given unit.
 * The table init ID is the index into bcmltd_table_lifecycle_handler_t
 * array.
 *
 * \param [in]  unit     Unit number.
 * \param [out] count    Number of table init IDs.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit,
 */
extern int
bcmlrd_map_table_init_id_count_get(int unit, uint32_t *count);

/*!
 * \brief Get number of table init IDs.
 *
 * Get the number of table init IDs for the given unit.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  init_idx Table init index.
 * \param [out] init_id  Table init id.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit,
 */
extern int
bcmlrd_map_table_init_id_get(int unit, uint32_t init_idx, uint32_t *init_id);

/*!
 * \brief Get map table attribute value
 *
 * Get map table attribute value
 *
 * \param [in]  unit     Unit number.
 * \param [in]  sid      Table id.
 * \param [in]  name     Table attribute name.
 * \param [out] value    Table attribute value.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit,
 */
extern int
bcmlrd_map_table_attr_get(int unit,
                          bcmlrd_sid_t sid,
                          bcmlrd_map_table_attr_name_t name,
                          uint32_t *value);

/*!
 * \brief Function to check if the logical field in unmapped.
 *
 * This routine checks if the logical field is unmapped
 * for the given map and fid.
 *
 * \param [in]  map             Pointer to map structure.
 * \param [in]  fid             Logical Field ID.
 *
 * \retval true if the field is unmapped.
 * \retval false if the field is not unmapped.
 */

static inline bool
bcmlrd_map_field_is_unmapped(const bcmlrd_map_t *map, bcmlrd_fid_t fid)
{
    return bcmlrd_field_is_unmapped_internal(map, fid);
}

#endif /* BCMLRD_MAP_H */
