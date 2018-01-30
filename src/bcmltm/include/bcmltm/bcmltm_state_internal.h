/*! \file bcmltm_state_internal.h
 *
 * Logical Table Manager State Internal Definitions.
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

#ifndef BCMLTM_STATE_INTERNAL_H
#define BCMLTM_STATE_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>

#include <bcmdrd_config.h>

#include <bcmltd/bcmltd_handler.h>

#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_ha_internal.h>


/*
 * LTM state for a single LT
 */

/*!
 * \brief LTM Extended State formats.
 *
 * The per-LT state extensions are selected from these types
 */
typedef enum bcmltm_state_type_e {
    /*!
     * Local in-use entries bitmap for Index LTs.
     * Each bit indicates an entry in the Logical Table.  The bit position
     * within the bitmap matches the Tracking Index for the entry.
     */
    BCMLTM_STATE_TYPE_INUSE_BITMAP = 0,

    /*!
     * Current valid entries bitmap for Index LTs.
     * Each bit indicates a Tracking Index matching a set of
     * key fields that are a legal combination according to the
     * currently configured API Key field bounds in TABLE_FIELD_CONTROL.
     */
    BCMLTM_STATE_TYPE_VALID_BITMAP = 1,

    /*!
     * Global in-use entries bitmap for overlay tables.
     * One table out of the set of LTs sharing entries by index
     * is selected to manage this global bitmap.  Other LTs
     * reference this table by SID.
     */
    BCMLTM_STATE_TYPE_GLOBAL_BITMAP = 2,

    /*!
     * Current value bounds for LT Key API fields.
     * The set of valid key values is used to generate the valid bitmap.
     */
    BCMLTM_STATE_TYPE_KEY_BOUNDS = 3,

    BCMLTM_STATE_TYPE_COUNT   /*!< Total extended state type count. */
} bcmltm_state_type_t;

/*!
 * \brief Per-LT additional state data.
 *
 * Some Logical Tables require state beyond the basic elements for all LTs
 * This structure links the different categories of the LT state for cases
 * where each is needed.
 * All state is stored in HA memory, so HA pointers are used to
 * reference the structure links.
 */
typedef struct bcmltm_lt_state_data_s {
    /*! Size in bytes of any additional table state. */
    uint32_t state_data_size;

    /*! Format of the extended state. */
    bcmltm_state_type_t state_data_type;

    /*! HA Pointer to a this table state. */
    bcmltm_ha_ptr_t state_data_hap;

    /*! Optional HA pointer to additional table state. */
    bcmltm_ha_ptr_t next_state_data_hap;
} bcmltm_lt_state_data_t;

/*!
 * \brief Per-Key field value boundary information.
 *
 * This structure containes the current minimum and maximum values
 * for Index LT Key fields, as assigned (or initialized) via the
 * TABLE_FIELD_CONTROL LT.
 */
typedef struct bcmltm_key_bound_info_s {
    /*! API-facing field identifier. */
    uint32_t api_field_id;

    /*!
     * The index of the field in an array.
     */
    uint32_t field_idx;

    /*! Currently configured minimum value for this Index LT Key field. */
    uint64_t current_key_min;

    /*! Currently configured maximum value for this Index LT Key field. */
    uint64_t current_key_max;
} bcmltm_key_bound_info_t;

/*!
 * \brief Index LT Key field value boundary information.
 *
 * This structure containes the Index LT Key fields' boundaries,
 * as assigned (or initialized) via the TABLE_FIELD_CONTROL LT.
 * These boundaries are used to determine the valid bitmap of
 * Index with Allocation LTs, and to verify the API value of Index LT
 * Key fields for all operations.
 */
typedef struct bcmltm_index_key_bounds_s {
    /*! The number of API Key fields for this Index LT. */
    uint32_t num_keys;

    /*!
     * An array of size num_keys containing the boundary information
     * for each API key field of this Index LT.
     */
    bcmltm_key_bound_info_t key_bounds[];
} bcmltm_index_key_bounds_t;

/*!
 * \brief Per-LT ongoing state.
 *
 * This structure assembles the runtime state of a single LT.  For
 * deployments that enable HA, this state will be updated at each
 * entry or transaction commit.
 */
typedef struct bcmltm_lt_state_s {
    /* Common LT state. */

    /*! State is initialized - used in HA recovery. */
    bool initialized;

    /*! Logical Table ID - HA key. */
    uint32_t ltid;

    
    /*! Currently configured minimum index for this LT. */
    uint32_t current_index_min;

    /*! Currently configured maximum index for this LT. */
    uint32_t current_index_max;

    /*!
     * Currently configured minimum memory parameter value
     * permitted for this memory parameter.
     */
    uint32_t current_param_min;

    /*!
     * Currently configured maximum memory parameter value
     * permitted for this memory parameter.
     */
    uint32_t current_param_max;
    

    /*! Current count of entries in this LT. */
    uint32_t entry_count;
    /* ... */

    /*! Optional HA pointer to a structure tracking additional table state. */
    bcmltm_ha_ptr_t table_state_data_hap;

    /*! Enable PT status update for LT operation. */
    bool table_op_pt_info_enable;
} bcmltm_lt_state_t;

/*!
 * \brief Define format to retrieve total state of a LT
 *
 * This is the function prototype for fetching the total working state
 * of a particular Logical Table.  The state is managed within the high
 * level LTM submodule controling metadata.  Some state-related
 * operations and tables require access to the state data of a table.
 * This callback permits access to the LT state structure.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_state_p Pointer to the pointer for state for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 typedef int (*bcmltm_state_lt_get_f)(int unit,
                                      uint32_t ltid,
                                      bcmltm_lt_state_t **lt_state_p);

/*!
 * \brief Define format to retrieve LT info values.
 *
 * This is the function prototype for fetching the information values of
 * a Logical Table.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [in] field_id The ID number for this TABLE_* field.
 * \param [in] field_idx The array index for this TABLE_* field.
 * \param [out] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
 typedef int (*bcmltm_info_lt_get_f)(int unit,
                                     uint32_t table_id,
                                     uint32_t field_id,
                                     uint32_t field_idx,
                                     uint64_t *data_value);


/*!
 * \brief LTM state management information per-unit.
 *
 * This structure assembles the runtime state of a single LT.  For
 * deployments that enable HA, this state will be updated at each
 * entry or transaction commit.
 */
typedef struct bcmltm_state_mgmt_s {
    /*!
     * Maximum size of a single LT bcmltm_lt_state_t structure.
     * This is included in calculating the size of the
     * ROLLBACK_STATE HA block.
     */
    uint32_t maximum_state_size;

    /*!
     * The maximum number of LT state clones in a transaction.
     */
    uint32_t lt_trans_max;

    /*!
     * The current number of LT state clones in a transaction.
     */
    uint32_t lt_clone_count;

    /*!
     * Maximum size of a single LT's combined state data.
     * This is included in calculating the size of the
     * ROLLBACK_STATE_DATA HA block.
     */
    uint32_t maximum_state_data_size;

    /*!
     * An array of HA pointers to the space reserved to contain
     * rollback copies of bcmltm_lt_state_t structures during
     * a transaction.
     */
    bcmltm_ha_ptr_t *lt_state_rollback_chunk_hap;

    /*!
     * An array of HA pointers to the space reserved to contain
     * rollback copies of a single LT's combined state data during
     * a transaction.
     */
    bcmltm_ha_ptr_t *lt_state_data_rollback_chunk_hap;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LT state for a given Logical Table.
     */
    bcmltm_state_lt_get_f lt_state_get_cb;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LT info for a given Logical Table.
     */
    bcmltm_info_lt_get_f lt_info_get_cb;
} bcmltm_state_mgmt_t;

/*!
 * \brief Retrieve the LTM state management structure for a unit.
 *
 * The LTM state management structure contains a several critical
 * elements required for proper transaction operation.  The context for
 * these elements exist at higher layers of the LTM submodule tree,
 * so this function permits those higher layers to initialize the
 * information in the structure for use within the state management
 * domain.
 *
 * \param [in] unit Logical device id.
 *
 * \retval None
 */
extern bcmltm_state_mgmt_t *
bcmltm_state_mgmt_get(int unit);


/*!
 * \brief Register a callback for retrieving LT state.
 *
 * Some LT node functions require access to the LT state of a LT
 * other than the LTID specified in the entry structure.  Any LT
 * with a key including LTID will need this capability.  To preserve
 * the layering heirarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs working state is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state_get_cb Callback function to retrieve LT state.
 *
 * \retval None
 */
extern void
bcmltm_state_lt_get_register(int unit,
                             bcmltm_state_lt_get_f lt_state_get_cb);

/*!
 * \brief Retrieve total state of a LT.
 *
 * Fetch the total working state of a particular Logical Table.
 * This function provides an interface to the registered state
 * retrieval callback which may be used by other LTM submodules.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_state_p Pointer to the state for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int bcmltm_state_lt_get(int unit,
                                uint32_t ltid,
                                bcmltm_lt_state_t **lt_state_p);

/*!
 * \brief Register a callback for retrieving LT info values.
 *
 * Some LT node functions require access to the LT metadata of a LT
 * other than the LTID specified in the entry structure.  Any LT
 * with a key including LTID will need this capability.  To preserve
 * the layering heirarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs info values is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id
 * \param [in] lt_info_get_cb Callback function to retrieve LT info.
 *
 * \retval None
 */
extern void
bcmltm_info_lt_get_register(int unit,
                            bcmltm_info_lt_get_f lt_info_get_cb);

/*!
 * \brief Retrieve info values for a LT.
 *
 * Fetch the specific info value of a particular Logical Table.
 * This function provides an interface to the registered info value
 * retrieval callback which may be used by other LTM submodules.
 *
 * \param [in] unit Logical device id.
 * \param [in] table_id Target Logical Table ID.
 * \param [in] field_id The ID number for this TABLE_INFO field.
 * \param [in] field_idx The array index for this TABLE_INFO field.
 * \param [out] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int bcmltm_info_lt_get(int unit,
                               uint32_t table_id,
                               uint32_t field_id,
                               uint32_t field_idx,
                               uint64_t *data_value);

/*!
 * \brief Retrieve specific state of a LT
 *
 * Search a Logical Table's LT state records for the particular
 * category of state data.
 * Return a pointer to the data if it exists, an error if not.
 * Internally this will traverse the HA pointers, but the input
 * and output are both live pointers.  This is safe because this
 * function is used only within the LT node functions.  The HA pointer
 * to LT state or rollback state are resolved in the encompasing
 * tree wrappers.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] state_type Category of state data.
 * \param [out] state_data Pointer to state data of selected type
 *                         for the Logical Table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
extern int bcmltm_state_data_get(int unit,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_state_type_t state_type,
                                 void **state_data);

/*!
 * \brief Duplicate the state of a LT
 *
 * Copy the source LT state to the destination LT state.
 * The destination should already be allocated to be of sufficient
 * size to hold all of the chained state.  This calculation is
 * performed by bcmltm_state_size.  Further, the linkages between the
 * structures must already be populated, and match in size.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state_src_hap LT state source.
 * \param [in,out] lt_state_dst_hap LT state destination.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
extern int bcmltm_state_copy(int unit,
                             bcmltm_ha_ptr_t lt_state_src_hap,
                             bcmltm_ha_ptr_t lt_state_dst_hap);

/*!
 * \brief Duplicate the state of a LT
 *
 * Create a copy the source LT state.
 * The destination will be allocated and returned in the provided
 * destination pointer.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_idx LT record number within this transaction.
 * \param [in] lt_state_src_hap LT state source.
 * \param [out] lt_state_dst_hap_p Pointer to the LT state destination.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
extern int bcmltm_state_clone(int unit,
                              uint8_t lt_idx,
                              bcmltm_ha_ptr_t lt_state_src_hap,
                              bcmltm_ha_ptr_t *lt_state_dst_hap_p);

/*!
 * \brief Initialize handle for PT status IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_table_op_pt_info_init(int unit);

/*!
 * \brief Cleanup handle for PT status IMM opertions.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_table_op_pt_info_cleanup(int unit);

#endif /* BCMLTM_STATE_INTERNAL_H */
