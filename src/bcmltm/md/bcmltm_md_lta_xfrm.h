/*! \file bcmltm_md_lta_xfrm.h
 *
 * Logical Table Manager - LTA Field Transform.
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

#ifndef BCMLTM_MD_LTA_XFRM_H
#define BCMLTM_MD_LTA_XFRM_H


#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>


/*!
 * \brief Field Transform Default Types.
 *
 * This type specifies different ways in which the LTA input list
 * can be populated.
 *
 * BCMLTM_MD_XFRM_DEFAULT_NONE:
 *   LTM API entry input fields are copied to the LTA field list.
 * BCMLTM_MD_XFRM_DEFAULT_UPDATE:
 *   LTM API entry input fields are copied onto an existing LTA field list.
 * BCMLTM_MD_XFRM_DEFAULT_ADD:
 *   LTM API entry input fields are copied to the LTA field list and,
 *   in addition, field default values are added to the LTA list for
 *   those fields which are not present in the input list.
 * BCMLTM_MD_XFRM_DEFAULT_ALL:
 *   The LTA field list is populated with all default field values.
 * BCMLTM_MD_XFRM_DEFAULT_ALLOC:
 *   The LTA key field list contains optional fields.
 *   Normally, all fields in a key field list are required.
 *   This option allows keys to be absent from the API input list,
 *   and if so, a flag is set to indicate they need to be allocated.
 *   This is applicable for Index with Allocation LTs.
 */
typedef enum bcmltm_md_xfrm_default_e {
    /*! LTA field list from API input list only. */
    BCMLTM_MD_XFRM_DEFAULT_NONE = 0,

    /*! LTA field list from API input list only, to existing LTA field. */
    BCMLTM_MD_XFRM_DEFAULT_UPDATE = 1,

    /*! Add default to LTA field list if field is not present in input list. */
    BCMLTM_MD_XFRM_DEFAULT_ADD = 2,

    /*! LTA field list with all defaults. */
    BCMLTM_MD_XFRM_DEFAULT_ALL = 3,

    /*! LTA key field list have optional fields (for IwA index). */
    BCMLTM_MD_XFRM_DEFAULT_ALLOC = 4,
} bcmltm_md_xfrm_default_t;


/*!
 * \brief Create the Forward Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Forward Field Transforms
 * for the given LT and transform type.
 *
 * This can be used by any operation that requires a forward transform
 * from LT source to PT destination.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] field_def Indicates how to populate the LTA input field list.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_fwd_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmltm_md_xfrm_default_t field_def,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_node_t **next_chain);


/*!
 * \brief Create the Reverse Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Reverse Field Transforms
 * for the given LT and transform type.
 *
 * This can be used by any operation that requires a reverse transform
 * from LT source to PT destination, such as LOOKUP.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_rev_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_node_t **next_chain);

/*!
 * \brief Create the Key Allocation Forward Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Key Forward
 * Field Transforms to be used for INSERT on IwA LTs.
 * It installs both, transforms with required keys as well as
 * transforms with optional keys.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_fwd_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_node_t **next_chain);

/*!
 * \brief Create the Key Allocation Reverse Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Key Reverse
 * Field Transforms to be used for INSERT on IwA LTs.
 * It installs transforms that contain optional keys.
 *
 * If optional keys are absent during an INSERT, they are allocated
 * and returned in the API field list.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_rev_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_node_t **next_chain);

#endif /* BCMLTM_MD_LTA_XFRM_H */
