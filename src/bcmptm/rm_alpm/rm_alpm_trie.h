/*! \file rm_alpm_trie.h
 *
 * Trie routines internal to RM ALPM
 *
 * This file contains trie routines which are internal to
 * ALPM Resource Manager
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

#ifndef RM_ALPM_TRIE_H
#define RM_ALPM_TRIE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_lmem_mgr.h>

/*******************************************************************************
 * Defines
 */
/*
 * This macro is a tidy way of performing subtraction to move from a
 * pointer within an object to a pointer to the object.
 *
 * Arguments are:
 *    type of object to recover
 *    pointer to object from which to recover element pointer
 *    pointer to an object of type t
 *    name of the trie node field in t through which the object is linked on
 *    trie
 * Returns:
 *    a pointer to the object, of type t
 */
#define TRIE_ELEMENT(t, p, ep, f) \
    ((t) (((char *) (p)) - (((char *) &((ep)->f)) - ((char *) (ep)))))

/*
 * TRIE_ELEMENT_GET performs the same function as TRIE_ELEMENT, but does not
 * require a pointer of type (t).  This form is preferred as TRIE_ELEMENT
 * typically generate Coverity errors, and the (ep) argument is unnecessary.
 *
 * Arguments are:
 *    type of object to recover
 *    pointer to object from which to recover element pointer
 *    name of the trie node field in t through which the object is linked on
 *    trie
 * Returns:
 *    a pointer to the object, of type t
 */
#define TRIE_ELEMENT_GET(t, p, f) \
    ((t) (((char *) (p)) - (((char *) &(((t)(0))->f)))))


#define CLONE_TRIE_NODE(dest, src) \
    sal_memcpy((dest), (src), sizeof(rm_alpm_trie_node_t))


typedef enum trie_propagate_op_e {
    PROPOGATE_INSERT = 0,
    PROPOGATE_DELETE = 1,
    PROPOGATE_UPDATE = 2
} trie_propagate_op_t;

/*******************************************************************************
 * Typedefs
 */
typedef struct rm_alpm_trie_node_s rm_alpm_trie_node_t;
typedef enum rm_alpm_node_type_e {
    INTERNAL, PAYLOAD, MAX
} rm_alpm_node_type_e_t;

typedef struct child_node_s {
    rm_alpm_trie_node_t *child_node;
} rm_alpm_child_node_t;

struct rm_alpm_trie_node_s {
    /* rm_alpm_trie_node_t     *trie_node; */
#define MAX_CHILD     (2)
    rm_alpm_child_node_t    child[MAX_CHILD];
    rm_alpm_node_type_e_t   type;
    uint32_t        skip_len;
    uint32_t        skip_addr;
    uint32_t        count;     /* number of payload node counts */
    uint32_t        bpm;       /* best prefix match bit map - 32 bits */
    rm_alpm_trie_node_t *next;
};

typedef struct rm_alpm_trie_s {
    rm_alpm_trie_node_t *trie;     /* trie root pointer */
    uint32_t    v6_key;    /* support 144 bits key, otherwise expect 48 bits key */
    uint32_t    mkl;       /* max_key_len*/
    struct rm_alpm_trie_s *next;
} rm_alpm_trie_t;

typedef struct rm_alpm_trie_bpm_cb_info_s {
    uint32_t        *pfx;     /* prefix buffer pointer from caller space */
    uint32_t        len;      /* prefix length */
    void            *user_data;
} rm_alpm_trie_bpm_cb_info_t;

typedef enum rm_alpm_trie_traverse_order_e_s {
    TRAVERSE_PREORDER,  /* root, left, right */
    TRAVERSE_INORDER,   /* left, root, right */
    TRAVERSE_POSTORDER, /* left, right, root */
    TRAVERSE_BFS,       /* Level by level */
    TRAVERSE_ORDER_MAX
} rm_alpm_trie_traverse_order_e_t;

typedef enum rm_alpm_trie_traverse_states_e_s {
    TRAVERSE_STATE_NONE,
    TRAVERSE_STATE_DELETED,
    TRAVERSE_STATE_DONE,
    TRAVERSE_STATE_MAX
} rm_alpm_trie_traverse_states_e_t;

typedef int (*rm_alpm_trie_callback_f)(rm_alpm_trie_node_t *trie, void *datum);

typedef int (*rm_alpm_trie_propagate_cb_f)(rm_alpm_trie_node_t *trie,
                                           rm_alpm_trie_bpm_cb_info_t *info);

typedef int (*rm_alpm_trie_callback_ext_f)(rm_alpm_trie_node_t *ptrie,
                                           rm_alpm_trie_node_t *trie,
                                           rm_alpm_trie_traverse_states_e_t *state,
                                           void *info);
typedef int (*rm_alpm_trie_clone_callback_f)(rm_alpm_trie_node_t *trie,
                                             rm_alpm_trie_node_t **clone_trie,
                                             void *info);

typedef int (*rm_alpm_trie_repart_callback_f)(rm_alpm_trie_node_t *ptrie,
                                              rm_alpm_trie_node_t *trie,
                                              rm_alpm_trie_traverse_states_e_t *state,
                                              void *info,
                                              rm_alpm_trie_node_t **new_ptrie);

/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */


/*
 * Function:
 *     bcmptm_rm_alpm_trie_create
 * Purpose:
 *     allocates a trie & initializes it
 */
extern int bcmptm_rm_alpm_trie_create(uint32_t       max_key_len,
                                    rm_alpm_trie_t **ptrie);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_destroy
 * Purpose:
 *     destroys a trie
 */
extern int bcmptm_rm_alpm_trie_destroy(rm_alpm_trie_t *trie);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_insert
 * Purpose:
 *     Inserts provided prefix/length in to the trie
 */
extern int bcmptm_rm_alpm_trie_insert(rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            *bpm,
                                      uint32_t            length,
                                      rm_alpm_trie_node_t *payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_delete
 * Purpose:
 *     Deletes provided prefix/length in to the trie
 */
extern int bcmptm_rm_alpm_trie_delete(rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            length,
                                      rm_alpm_trie_node_t **payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_search
 * Purpose:
 *     Search the given trie for provided prefix/length
 */
extern int bcmptm_rm_alpm_trie_search(rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            length,
                                      rm_alpm_trie_node_t **payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_search_verbose
 * Purpose:
 *     Search the given trie for provided prefix/length,
 *     return the matched prefix/length
 */
extern int bcmptm_rm_alpm_trie_search_verbose(rm_alpm_trie_t      *trie,
                                              uint32_t            *key,
                                              uint32_t            length,
                                              rm_alpm_trie_node_t **payload,
                                              uint32_t            *result_key,
                                              uint32_t            *result_len);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_dump
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
extern int bcmptm_rm_alpm_trie_dump(rm_alpm_trie_t          *trie,
                                    rm_alpm_trie_callback_f cb,
                                    void                    *user_data);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_lpm
 * Purpose:
 *     Find the longest prefix matched with given prefix
 */
extern int bcmptm_rm_alpm_trie_find_lpm(rm_alpm_trie_t      *trie,
                                        uint32_t            *key,
                                        uint32_t            length,
                                        rm_alpm_trie_node_t **payload,
                                        uint32_t            *lpm_pfx,
                                        uint32_t            *lpm_len);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_lpm2
 * Purpose:
 *     Find the longest prefix matched with given prefix, exclude itself.
 */
extern int bcmptm_rm_alpm_trie_find_lpm2(rm_alpm_trie_t      *trie,
                                         uint32_t            *key,
                                         uint32_t            length,
                                         rm_alpm_trie_node_t **payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_pm
 * Purpose:
 *     Find the prefix matched nodes with given prefix and callback
 *     with specified callback funtion and user data
 */
extern int bcmptm_rm_alpm_trie_find_pm(rm_alpm_trie_t          *trie,
                                       uint32_t                *key,
                                       uint32_t                length,
                                       rm_alpm_trie_callback_f cb,
                                       void                    *user_data);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_prefix_bpm
 * Purpose:
 *    Given a key/length return the Best prefix match length
 *    key/bpm_pfx_len will be the BPM for the key/length
 */
extern int bcmptm_rm_alpm_trie_find_prefix_bpm(rm_alpm_trie_t *trie,
                                               uint32_t       *key,
                                               uint32_t       length,
                                               uint32_t       *bpm_pfx_len);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_bpm_mask_get
 * Purpose:
 *     Get the bpm mask of target key. This key is already in the trie.
 */
extern int bcmptm_rm_alpm_trie_bpm_mask_get(rm_alpm_trie_t *trie,
                                            uint32_t       *key,
                                            uint32_t       length,
                                            uint32_t       *bpm_mask);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_split
 * Purpose:
 *     Split the trie into 2 based on optimum pivot
 */
extern int bcmptm_rm_alpm_trie_split(rm_alpm_trie_t      *trie,
                                     const uint32_t      max_split_len,
                                     uint32_t            *pivot,
                                     uint32_t            *length,
                                     rm_alpm_trie_node_t **split_trie_root,
                                     uint32_t            *bpm,
                                     /* if set split will strictly split only on
                                      * payload nodes, if not set splits at
                                      * optimal point on the trie
                                      */
                                     uint32_t            payload_node_split,
                                     int                 max_split_count);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_merge
 * Purpose:
 *     unsplit or fuse the child trie with parent trie
 */
extern int bcmptm_rm_alpm_trie_merge(rm_alpm_trie_t      *parent_trie,
                                     rm_alpm_trie_node_t *child_trie,
                                     uint32_t            *child_pivot,
                                     uint32_t            length);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_split2
 * Purpose:
 *     Split the trie such that the new sub trie covers given prefix/length.
 *     Basically this is a reverse of bcmptm_rm_alpm_trie_merge.
 */
extern int bcmptm_rm_alpm_trie_split2(rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            key_len,
                                      uint32_t            *pivot,
                                      uint32_t            *pivot_len,
                                      rm_alpm_trie_node_t **split_trie_root);

/*!
 * \brief Clone a new trie
 *
 * \param [in] trie_src Trie cloned from
 * \param [in] clone_cb Payload clone callback
 * \param [in] user_data User data
 * \param [out] trie_dst Trie cloned to
 * \param [out] clone_hdl Local memory handle
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_trie_clone_create(rm_alpm_trie_t *trie_src,
                                 rm_alpm_trie_clone_callback_f clone_cb,
                                 void *user_data,
                                 rm_alpm_trie_t **trie_dst,
                                 shr_lmm_hdl_t *clone_hdl);

/*!
 * \brief Destroy a cloned trie
 *
 * \param [in] trie Trie to be destroyed
 * \param [in] clone_hdl Local memory handle
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_trie_clone_destroy(rm_alpm_trie_t *trie,
                                  shr_lmm_hdl_t clone_hdl);

/*!
 * \brief Return payload count of a trie
 *
 * \param [in] trie Trie
 *
 * \return Trie count
 */
extern int
bcmptm_rm_alpm_trie_count(rm_alpm_trie_t *trie);

extern int bcmptm_rm_alpm_trie_compare(rm_alpm_trie_t *trie_src,
                                       rm_alpm_trie_t *trie_dst,
                                       int            *equal);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_traverse
 * Purpose:
 *     Traverse the trie & call the application callback with user data
 */
extern int bcmptm_rm_alpm_trie_traverse(rm_alpm_trie_t                  *trie,
                                        rm_alpm_trie_callback_f         cb,
                                        void                            *user_data,
                                        rm_alpm_trie_traverse_order_e_t order);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_traverse2
 * Purpose:
 *     Traverse the trie (PAYLOAD) & call the extended application callback
 *     which has current node's PAYLOAD parent node with user data.
 */
extern int bcmptm_rm_alpm_trie_traverse2(rm_alpm_trie_t                  *trie,
                                         rm_alpm_trie_callback_ext_f     cb,
                                         void                            *user_data,
                                         rm_alpm_trie_traverse_order_e_t order);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_repartition
 * Purpose:
 *     Traverse the trie & call the extended application callback
 *     which has current node's parent node with user data.
 */
extern int bcmptm_rm_alpm_trie_repartition(rm_alpm_trie_t *trie,
                                           rm_alpm_trie_repart_callback_f cb,
                                           void *user_data,
                                           rm_alpm_trie_traverse_order_e_t order);


/*
 * Function:
 *     bcmptm_rm_alpm_trie_pivot_propagate_prefix
 * Purpose:
 *   Propogate prefix BPM from a given pivot.
 */
extern int bcmptm_rm_alpm_trie_propagate(rm_alpm_trie_t              *trie,
                                         uint32_t                    *pfx,
                                         uint32_t                    len,
                                         trie_propagate_op_t         op,
                                         rm_alpm_trie_propagate_cb_f cb,
                                         rm_alpm_trie_bpm_cb_info_t  *cb_info,
                                         bool                        bpm_aware);

extern int bcmptm_rm_alpm_trie_pivot_propagate(uint32_t mkl,
                                               rm_alpm_trie_node_t *pivot,
                                               uint32_t pivot_len,
                                               uint32_t *pfx,
                                               uint32_t len,
                                               trie_propagate_op_t op,
                                               rm_alpm_trie_propagate_cb_f cb,
                                               rm_alpm_trie_bpm_cb_info_t *cb_info,
                                               bool bpm_aware);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_iter_get_first
 * Purpose:
 *     Traverse the trie & return pointer to first payload node
 */
extern int bcmptm_rm_alpm_trie_iter_get_first(rm_alpm_trie_t      *trie,
                                              rm_alpm_trie_node_t **payload);


/*!
 * \brief Initiliaze ALPM trie sub module.
 *
 * \param [in]  multi_thread is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise
 *
 * \return SHR_E_NONE/SHR_E_MEMORY.
 */

extern int bcmptm_rm_alpm_trie_init(bool multi_thread);

/*!
 * \brief Cleanup ALPM trie sub module.
 *
 * \return nothing.
 */
extern void bcmptm_rm_alpm_trie_cleanup(void);

#endif /* RM_ALPM_TRIE_H */

