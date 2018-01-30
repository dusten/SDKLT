/*! \file bcmltm_stats_internal.h
 *
 * Logical Table Manager Statistics Internal Definitions.
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

#ifndef BCMLTM_STATS_INTERNAL_H
#define BCMLTM_STATS_INTERNAL_H

#include <bcmdrd_config.h>

#include <bcmltm/bcmltm_types.h>

/*!
 * \brief Define format to retrieve statistics array of a LT
 *
 * This is the function prototype for fetching the statistics array
 * of a particular Logical Table, which is managed within the high
 * level LTM submodule.  Some  operations and tables require access
 * to the statistics of a table other than the access LT.
 * This callback permits access to another LT's metadata structure.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_stats_p Pointer to the pointer for statistics array
 *                         for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 typedef int (*bcmltm_stats_lt_get_f)(int unit,
                                      uint32_t ltid,
                                      uint32_t **lt_stats_p);

/*!
 * \brief LTM statistics management information per-unit.
 */
typedef struct bcmltm_stats_mgmt_s {
    /*!
     * Size of statistic types indexed arrays on this unit.
     */
    uint32_t stats_array_size;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LT info for a given Logical Table.
     */
    bcmltm_stats_lt_get_f stats_lt_get_cb;
} bcmltm_stats_mgmt_t;

/*!
 * \brief Register a callback for retrieving LT statistics array.
 *
 * The TABLE_STATS node functions require access to the LT statistics array
 * of a LT other than the LTID specified in the entry structure.  To preserve
 * the layering heirarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs statistics array is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id.
 * \param [in] stats_lt_get_cb Callback function to retrieve LT
 *                             statistics array.
 *
 * \retval None
 */
extern void
bcmltm_stats_lt_get_register(int unit,
                             bcmltm_stats_lt_get_f stats_lt_get_cb);

/*!
 * \brief Retrieve statistics array of a LT.
 *
 * Fetch the statistics array of a particular Logical Table.
 * This function provides an interface to the registered statistics
 * array retrieval callback which may be used by other LTM submodules.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_stats_p Pointer to the statistics array for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int bcmltm_stats_lt_get(int unit,
                                uint32_t ltid,
                                uint32_t **lt_stats_p);

/*!
 * \brief Increment LT_STATS field
 *
 * Wrapper for auto-incrementing various LT_STATS values.
 *
 * \param [in] lt_stats Pointer to the statistics array for this LT.
 * \param [in] stat_field FID indicating which stat is to increment.
 */
extern void
bcmltm_stats_increment(uint32_t *lt_stats,
                       uint32_t stat_field);

#endif /* BCMLTM_STATS_INTERNAL_H */
