/*! \file bcmlrd_internal.h
 *
 * \brief Logical Resource Database internal definitions
 *
 * Internal data structures and definitions
 *
 * Not intended for callers outside of LRD.
 *
 * Not optimized for space.
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

#ifndef BCMLRD_INTERNAL_H
#define BCMLRD_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_internal.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_default_table_conf.h>

/*!
 * \brief Device configuration structure.
 *
 * Per-device map configuration.
 */
typedef struct bcmlrd_dev_conf_s {
    /*! Map configuration. */
    const bcmlrd_map_conf_rep_t *conf;
} bcmlrd_dev_conf_t;

/*!
 * \brief Global Device configuration.
 *
 * Per-device device map configurations. This interface is for unit
 * test purposes only, not for production use.
 */
extern bcmlrd_dev_conf_t const *bcmlrd_dev_conf;

/*!
 * \brief Default Global Device configuration.
 *
 * Default per-device device map configurations.
 */
extern bcmlrd_dev_conf_t bcmlrd_default_dev_conf[];

/*!
 * \brief LRD device configuration structure.
 *
 * Per-device LRD configuration.
 */
typedef struct bcmlrd_device_conf_s {
    /*! Device map configuration. */
    bcmlrd_dev_conf_t   *lrd_dev_conf;

    /*! Map configuration. */
    const bcmltd_table_conf_t *ltd_table_conf;
} bcmlrd_device_conf_t;

extern bcmlrd_device_conf_t bcmlrd_default_device_conf;

/*!
 * \brief Get a configuration for a unit.
 *
 * Get a pointer to a configuration for the given unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval non-NULL     Pointer to configuration.
 * \retval NULL         Unit number invalid.
 */

extern const bcmlrd_map_conf_rep_t *
bcmlrd_unit_conf_get(int unit);

/*!
 * \brief Probe attached units for a configuration
 *
 * \retval SHR_E_NONE   No errors during probe.
 */
extern int
bcmlrd_unit_init(void);

/*!
 * \brief Cleanup LRD attached unit information.
 *
 * \retval SHR_E_NONE   No errors during cleanup.
 */
extern int
bcmlrd_unit_cleanup(void);

/*!
 * \brief Configuration/Unit map.
 *
 * Configurations for each attached unit, or NULL if there is no configuration
 */

extern const bcmlrd_map_conf_rep_t *
bcmlrd_unit_conf[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Return the index for a table name.
 *
 * Get a pointer to a configuration for the given unit.
 *
 * \param [in] table_name       Logical Table name.
 *
 * \retval >= 0                 Table index.
 * \retval SHR_E_NOT_FOUND      Table not found.
 */

extern int
bcmlrd_table_name_to_idx(const char *table_name);

/*!
 * \brief Return the table type flags a table.
 *
 * \param [in] sid              Table index.
 *
 * \retval                      Table type.
 */

static inline uint32_t
bcmlrd_table_type(bcmlrd_sid_t sid)
{
    return (bcmltd_table_conf->table[sid]->flags & BCMLRD_TABLE_F_TYPE_MASK);
}

/*!
 * \brief Prepare a logical table configuration.
 *
 * Prepare a logical table configuration for the given unit by applying
 * any conditional mappings.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Base logical table configuration.
 * \param [out] out             Edited logical table configuration.
 *
 * \retval 0            No error.
 * \retval !0           Error.
 */

extern int
bcmlrd_unit_conf_prepare(int unit,
                         const bcmlrd_map_conf_rep_t *in,
                         const bcmlrd_map_conf_rep_t **out);


/*!
 * \brief Cleanup unit configuration preparation.
 *
 * Clean up memory allocations and related items associated with
 * configuration preparation.
 */

extern void
bcmlrd_unit_conf_cleanup(void);

/*!
 * \brief Field data type.
 */
typedef enum bcmlrd_field_data_type_e {
    /*! Min. */
    BCMLRD_FIELD_DATA_TYPE_MIN,

    /*! Max. */
    BCMLRD_FIELD_DATA_TYPE_MAX,

    /*! Default. */
    BCMLRD_FIELD_DATA_TYPE_DEFAULT,

} bcmlrd_field_data_type_t;

/*!
 * \brief Get field data by data type.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  type            Field data type.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_def       Pointer to field default data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_data_get_by_type(int unit,
                              const bcmlrd_sid_t sid,
                              const bcmlrd_fid_t fid,
                              const bcmlrd_field_data_type_t type,
                              const uint32_t  num_alloc,
                              uint64_t* field_def,
                              uint32_t* num_actual);

/*!
 * \brief Check if logical field is unmapped.
 *
 * This routine checks if given logical field ID is unmapped.
 * It assumes that the field ID is valid (i.e. within range).
 *
 * \param [in] map     Table map.
 * \param [in] fid     Field ID.
 *
 * \retval TRUE  Field is not mapped.
 * \retval FALSE Field is mapped.
 */
static inline bool
bcmlrd_field_is_unmapped_internal(const bcmlrd_map_t *map,
                                  bcmlrd_fid_t fid)
{
    return (!!(map->field_data->field[fid].flags & BCMLRD_FIELD_F_UNMAPPED));
}

/*!
 * \brief Set dev conf data.
 *
 * Set dev conf data.
 *
 * \param [in]  bcmlrd_dev_conf_t* dev conf.
 *
 * \retval bcmlrd_dev_conf_t* Pointer to the old dev conf data.
 */
const bcmlrd_dev_conf_t*
bcmlrd_dev_conf_set(const bcmlrd_dev_conf_t *dev_conf);

#endif /* BCMLRD_INTERNAL_H */
