/*! \file bcmptm_chip_internal.h
 *
 * Chip-specific utils, defines shared across BCMPTM sub-modules
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

#ifndef BCMPTM_CHIP_INTERNAL_H
#define BCMPTM_CHIP_INTERNAL_H

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


/*******************************************************************************
 * Typedefs
 */
/*! Decoded response info for HW_lookup, insert, delete operations.
 */
typedef struct bcmptm_rsp_info_s {
    /*! response entry index */
    uint32_t entry_index;

    /*! key for entry to be inserted existed, so data replaced */
    bool replaced;

    /*! entry_index corresponds to 2x view */
    bool entry_index_view_2x;

    /*! entry_index corresponds to 4x view */
    bool entry_index_view_4x;

    /*! error in lp_word */
    bool error_in_lp_word;

    /*! error in action_table_a */
    bool error_in_action_table_a;

    /*! error in remap_table_a */
    bool error_in_remap_table_a;

    /*! error in action_table_b */
    bool error_in_action_table_b;

    /*! error in remap_table_b */
    bool error_in_remap_table_b;

    /*! Must do ser correction and retry */
    bool ser_retry;

} bcmptm_rsp_info_t;

/*!
 * \brief Format of {key, mask} stored in TCAM.
 * - During reads, TCAM will return key, mask in this format.
 * - During writes, either SW or HW needs to convert standard (key, mask) into
 * tcam-specific format
 */
typedef enum {
    BCMPTM_TCAM_KM_FORMAT_IS_KM = 0, /* tcam format is KM - meaning no conv needed */
    BCMPTM_TCAM_KM_FORMAT_ONLY_CLEAR_DCD, /* only clear dont_care D bits */
    BCMPTM_TCAM_KM_FORMAT_40NM_XY,
    BCMPTM_TCAM_KM_FORMAT_X_YBAR,
    BCMPTM_TCAM_KM_FORMAT_LPT,
    BCMPTM_TCAM_KM_FORMAT_LPT_7NM
} bcmptm_pt_tcam_km_format_t;

/*! Raw field info (used in constant init struct) */
typedef struct bcmptm_field_info_raw_s {
    /*! Start bit for the field */
    uint16_t start_bit;

    /*! width of the field */
    uint16_t width;
} bcmptm_field_info_raw_t;

/*!
 * \brief Format of {key, mask} stored in TCAM.
 * - During reads, TCAM will return key, mask in this format.
 * - During writes, either SW or HW needs to convert standard (key, mask) into
 * tcam-specific format
 */
typedef enum {
    BCMPTM_OINFO_MODE_NONE = 0,
        /* Not an overlay */

    BCMPTM_OINFO_MODE_HASH = 1,
        /* multiple NEs in one WE,
         * All NEs of same width, start_bits of NEs are implicit */

    BCMPTM_OINFO_MODE_TCAM_DATA = 2,
        /* NE is TCAM_DATA_ONLY view of TCAM_AGGR view */

} bcmptm_oinfo_mode_t;

/*! Information for overlay (narrow) mems */
typedef struct bcmptm_overlay_info_s {
    /*! Overlay info mode. See defn for bcmptm_oinfo_mode_t */
    bcmptm_oinfo_mode_t mode;

    /*! TRUE for alpm type tables */
    bool is_alpm;

    /*! narrow view */
    bcmdrd_sid_t n_sid;

    /*! widest view */
    bcmdrd_sid_t w_sid;

    /*! width of narrow entries
     * - Also see defn of num_we_field below */
    uint16_t width_ne;

    /*! num narrow entries in widest view */
    uint8_t num_ne;

    /*! Start bit for narrow entry in widest view
     * Meaningful only if (mode == TCAM_DATA) && (num_we_field == 1) */
    uint16_t start_bit_ne_in_we;

    /*! Number of wide_entry_fields to be extracted to construct narrow_entry
     * If num_we_field = 0, This cannot be TCAM_DATA type overlay.
     *                      'width_ne' provides width of narrow_entry
     *                      we_field_array will be NULL
     *
     * If num_we_field = 1, This has to be TCAM_DATA type overlay
     *                      'width_ne' provides width of 1st wef(wide_entry_field),
     *                      'start_bit_ne_we' provides start_bit of 1st wef
     *                      we_field_array will be NULL
     *                      This covers most TCAM_DATA type overlays.
     *
     * If num_we_field > 1, This also has to be TCAM_DATA type overlay
     *                      'width_ne' provides width of 1st wef(wide_entry_field),
     *                      'start_bit_ne_we' provides start_bit of 1st wef
     *                      we_field_array will be non-NULL and provides
     *                      width, start_bit for 2nd, 3rd, and so on wefs
     *                      This is mode is intended for cases like L3_DEFIP
     *                      where several wefs must be extracted to assemble 1 ne
     */
    uint8_t num_we_field;

    /*! Points to array that holds start_bit, width info for fields in Wide
     * entry that need to be accessed when accessing narrow entry.
     * When !NULL, num_we_field must be > 1 */
    bcmptm_field_info_raw_t *we_field_array;
} bcmptm_overlay_info_t;


/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Allocate and initialize 'function pointers' for a unit
 * \n Must be called every time a new unit is attached.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_fnptr_init(int unit);

/*!
 * \brief Deallocate 'function pointer' space allocated  for a unit
 * \n Must be called every time a unit is detached.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_fnptr_cleanup(int unit);

/*!
 * \brief Find index_count for a PT
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 *
 * Returns:
 * \retval index_count (index_max - index_min + 1) if no errors
 * \retval 0 if errors (eg: sid is illegal, etc)
 */
extern uint32_t
bcmptm_pt_index_count(int unit,
                      bcmdrd_sid_t sid);

/*!
 * \brief Deallocate 'function pointer' space allocated  for a unit
 * \n Must be called every time a unit is detached.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \retval ser_resp type. Refer to definition of bcmdrd_ser_resp_t
 */
extern bcmdrd_ser_resp_t bcmptm_pt_ser_resp(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Decode rsp_info for HW ops: lookup, insert, delete
 *
 * \param [in] unit Logical device id
 * \param [in] req_op Only LOOKUP, INSERT, DELETE supported - see defn for
 * bcmptm_op_type_t
 * \param [in] bd_rsp_info Response info from HW - see defn for
 * bcmbd_tbl_resp_info_t
 *
 * \param [out] rsp_info HW independent response info - see defn for
 * bcmptm_rsp_info_t
 *
 * \retval SHR_E_NONE - Interpretation successful. Nothing unexpected.
 *                 \n - May need SER correction
 * \retval Else       - Error/Unexpected
 */
extern int
bcmptm_rsp_info_get(int unit,
                    bcmptm_op_type_t req_op,
                    bcmbd_tbl_resp_info_t *bd_rsp_info,
                    bcmptm_rsp_info_t *rsp_info); /* out */

/*!
 * \brief Find if PT is ALPM view
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] is_alpm TRUE or FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_attr_is_alpm(int unit, bcmdrd_sid_t sid, bool *is_alpm);

/*!
 * \brief Find overlay properties for a PT
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \param [out] oinfo Overlay info. See defn of bcmptm_overlay_info_t
 *
 * \retval SHR_E_NONE Success
 */
extern int bcmptm_pt_overlay_info_get(int unit, bcmdrd_sid_t sid,
                                      const bcmptm_overlay_info_t **oinfo);

/*!
 * \brief Find key,mask format used by PT
 * - can be different for different PTs on same chip
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] km_format See enums for bcmptm_pt_tcam_km_format_t
 *
 * \retval SHR_E_NONE
 */
extern int
bcmptm_pt_tcam_km_format(int unit, bcmdrd_sid_t sid,
                         bcmptm_pt_tcam_km_format_t *km_format);

/*!
 * \brief Find if HW converts key,mask to HW specific xy format
 * - In future devices HW can convert key,mask to HW specific xy format during
 * writes because parity generation, checking are done in TCAM wrapper. This was
 * not possible in older devices where parity gen, check logic was centralized
 * in CMIC block.
 *
 * \param [in] unit Logical device id
 * \param [out] hw_does_km_to_xy TRUE or FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_hw_does_km_to_xy(int unit, bool *hw_does_km_to_xy);

/*!
 * \brief Find if SW needs to store tcam_corrupt_bits
 * - TRUE for devices where TCAM parity error reporting is not via SER_FIFO
 *
 * \param [in] unit Logical device id
 * \param [out] tc_en TRUE or FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_tcam_corrupt_bits_enable(int unit, bool *tc_en);

/*!
 * \brief Find if PT needs to store logical_table_id for every entry
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] ltid_en TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_needs_ltid(int unit, bcmdrd_sid_t sid, bool *ltid_en);

/*!
 * \brief Find if PT needs to store dfield_format_id for every entry
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] dfid_bytes Only one byte or two byte dfid supported. Legal
 * values are thus: 0, 1, 2
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_needs_dfid(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes);

/*!
 * \brief Find if PT is in 'must_cache' exception list
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] must_cache TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_in_must_cache_list(int unit, bcmdrd_sid_t sid,
                                        bool *must_cache);

/*!
 * \brief Find if PT is in 'dont_cache' exception list
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] dont_cache TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_in_dont_cache_list(int unit, bcmdrd_sid_t sid,
                                        bool *dont_cache);

/*!
 * \brief Find if PT is cacheable
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] cache_en TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_attr_is_cacheable(int unit, bcmdrd_sid_t sid,
                                       bool *cache_en);

/*!
 * \brief Enable, Disable PTM to process mreq, ireq API calls.
 *
 * \param [in] unit Logical device id
 * \param [in] ready TRUE implies allow PTM to process mreq, ireq API calls.
 *
 * \retval SHR_E_NONE
 */
extern void bcmptm_ptm_ready_set(int unit, bool ready);

/*!
 * \brief Check if PTM is ready to accept mreq, ireq API calls.
 *
 * \param [in] unit Logical device id
 *
 * \retval TRUE - Yes PTM is ready to accept mreq, ireq API calls.
 */
extern bool bcmptm_ptm_is_ready(int unit);

#endif /* BCMPTM_CHIP_INTERNAL_H */
