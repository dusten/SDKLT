/*! \file shr_avl.h
 *
 * Defines a generic AVL tree data structure.
 *
 * On creating a tree, the user specifies the size of the datum and the
 * FIXED maximum number of entries.  On insert/delete/lookup, the user
 * supplies the datum compare function.
 *
 * NOTE: These routines are NOT thread-safe.  If multiple threads could
 * access an AVL tree, external locks must be used to mutually exclude
 * each call to the shr_avl routines, including shr_avl_traverse().
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

#ifndef SHR_AVL_H
#define SHR_AVL_H

/*! Opaque type for raw AVL datum (actual datum size is variable) */
typedef int shr_avl_datum_t;

/*!
 * \brief Comparison function for AVL tree operations.
 *
 * <long-description>
 *
 * \param [in] user_data Application-provided context.
 * \param [in] datum1 First datum of comparison.
 * \param [in] datum2 Second datum of comparison.
 *
 * \retval 1 datum1 is greater than datum2
 * \retval 0 datum1 is equal to datum2
 * \retval -1 datum1 is lesser than datum2
 */
typedef int (*shr_avl_compare_fn)(void *user_data,
                                  shr_avl_datum_t *datum1,
                                  shr_avl_datum_t *datum2);

/*!
 * \brief Comparison function for AVL lookup operations.
 *
 * <long-description>
 *
 * \param [in] user_data Application-provided context for AVL tree.
 * \param [in] datum1 First datum of comparison.
 * \param [in] datum2 Second datum of comparison.
 *
 * \retval 1 datum1 is greater than datum2
 * \retval 0 datum1 is equal to datum2
 * \retval -1 datum1 is lesser than datum2
 */
typedef int (*shr_avl_compare_fn_lkupdata)(void *user_data,
                                           shr_avl_datum_t *datum1,
                                           shr_avl_datum_t *datum2,
                                           void *lkupdata);

/*!
 * \brief Call-back function for AVL tree traverse operation.
 *
 * The application-provided traverse function will be called for each
 * datum in the AVL tree upon request.
 *
 * \param [in] user_data Application-provided context for AVL tree.
 * \param [in] datum Current traversed datum.
 * \param [in] trav_data Application-provided context for traverse.
 */
typedef int (*shr_avl_traverse_fn)(void *user_data,
                                   shr_avl_datum_t *datum,
                                   void *trav_data);

/*!
 * \brief Call-back function for copying AVL entries.
 *
 * The application-provided traverse function will be called for all
 * entries in the AVL tree upon request.
 *
 * \param [in] user_data Application-provided context for AVL tree.
 * \param [in] datum1 Destination datum.
 * \param [in] trav_data Source datum.
 */
typedef int (*shr_avl_datum_copy_fn)(void *user_data,
                                     shr_avl_datum_t *datum1,
                                     shr_avl_datum_t *datum2);

/*!
 * \brief AVL tree node structure.
 *
 * In addition to the raw AVL datum, this structure contains all
 * necessary control information for the entry.
 */
typedef struct shr_avl_entry_s {

    /*! Pointer to node's left subtree. */
    struct shr_avl_entry_s *left;

    /*! Pointer to node's right subtree. */
    struct shr_avl_entry_s *right;

    /*! Current AVL balance factor. */
    int balance;

    /*! Raw AVL datum (opaque type - real size is variable). */
    shr_avl_datum_t datum;

} shr_avl_entry_t;

/*!
 * \brief AVL tree data structure.
 *
 * This structure is used for all AVL operations.
 */
typedef struct shr_avl_s {

    /*! Application-provided context for call-backs. */
    void *user_data;

    /*! Size of each datum stored in the tree. */
    int datum_bytes;

    /*! Maximum number of entries in AVL tree. */
    int datum_max;

    /*! Size of AVL entry, i.e. raw datum + control information */
    int entry_bytes;

    /*! Allocated memory block for storing AVL entries. */
    char *datum_base;

    /*! Root of AVL tree. */
    shr_avl_entry_t *root;

    /*! List of unused AVL entries. */
    shr_avl_entry_t *free_list;

    /*! Number entries in tree */
    int count;

    /*! Function for copying AVL data */
    shr_avl_datum_copy_fn datum_copy_fn;

} shr_avl_t;

/*!
 * \brief Create an empty AVL tree.
 *
 * Use this function to create a new AVL tree.
 *
 * \param [out] avl_ptr Return pointer (handle) to new AVL tree.
 * \param [in] user_data Arbitrary user data passed to compare routines.
 * \param [in] datum_bytes Size of datum being stored.
 * \param [in] datum_max Maximum number of nodes in tree.
 *
 * \retval 0 No errors.
 * \retval -1 Memory allocation error.
 */
extern int
shr_avl_create(shr_avl_t **avl_ptr,
               void *user_data,
               int datum_bytes,
               int datum_max);

/*!
 * \brief Release the space occupied by AVL tree structure.
 *
 * \param [in] avl AVL tree handle.
 *
 * \retval 0 No errors.
 */
extern int
shr_avl_destroy(shr_avl_t *avl);

/*!
 * \brief Insert a datum into the AVL tree.
 *
 * A duplicate key overwrites the old contents of the datum.
 *
 * \param [in] avl AVL tree handle.
 * \param [in] cmp_fn Datum comparison function.
 * \param [in] datum Datum to insert.
 *
 * \retval 0 No errors.
 * \retval -1 Tree is full.
 */
extern int
shr_avl_insert(shr_avl_t *avl,
               shr_avl_compare_fn cmp_fn,
               shr_avl_datum_t *datum);

/*!
 * \brief Delete an AVL tree entry.
 *
 * \param [in] avl AVL tree handle
 * \param [in] key_cmp_fn Call-back function for entry comparison
 * \param [in] datum Datum of AVL entry to be deleted
 *
 * \retval 0 Success, did not find datum.
 * \retval 1 Success, found and deleted datum.
 */
extern int
shr_avl_delete(shr_avl_t *avl,
               shr_avl_compare_fn key_cmp_fn,
               shr_avl_datum_t *datum);

/*!
 * \brief Reset an AVL tree to empty.
 *
 * Also used during tree creation to initialize free list.
 *
 * \param [in] avl AVL tree handle.
 *
 * \retval 0 No errors.
 */
extern int
shr_avl_delete_all(shr_avl_t *avl);

/*!
 * \brief Return the number of data in an AVL tree.
 *
 * \param [in] avl AVL tree handle
 *
 * \return Number of entries in AVL tree (0 if avl is NULL).
 */
extern int
shr_avl_count(shr_avl_t *avl);

/*!
 * \brief Look up a datum in the AVL tree.
 *
 * \param [in] avl AVL tree handle
 * \param [in] key_cmp_fn Compare function which should compare only
 *		          the key portion of the datum.
 * \param [in,out] datum As input, datum in which key portion is
 *                       valid. As output, datum in which key and data
 *                       portion is valid if found
 *
 * \retval 0 Success, did not find datum.
 * \retval 1 Success, found key and updated rest of datum.
 */
extern int
shr_avl_lookup(shr_avl_t *avl,
               shr_avl_compare_fn key_cmp_fn,
               shr_avl_datum_t *datum);

/*!
 * \brief <brief-description>
 *
 * <long-description>
 *
 * \param [in] avl AVL tree handle.
 * \param [in] key_cmp_fn Compare function which should compare only
 *                        the key portion of the datum.
 * \param [in,out] datum As input, datum in which key portion is
 *                       valid. As output, datum in which key and data
 *                       portion is valid if found.
 * \param [in] lkupdata Application-provided data passed to the
 *                      comparison function
 *
 * \retval 0 Success, did not find datum.
 * \retval 1 Success, found key and updated rest of datum.
 */
extern int
shr_avl_lookup_lkupdata(shr_avl_t *avl,
                        shr_avl_compare_fn_lkupdata key_cmp_fn,
                        shr_avl_datum_t *datum,
                        void *lkupdata);

/*!
 * \brief Return the minimum element in the tree.
 *
 * Return the minimum elelment as per application-provided comparison
 * function during AVL tree creation.
 *
 * \param [in] avl AVL tree handle.
 * \param [out] datum Datum to receive element.
 *
 * \retval 0 Success, found and retrieved datum.
 * \retval -1 Tree empty.
 */
extern int
shr_avl_lookup_min(shr_avl_t *avl,
                   shr_avl_datum_t *datum);

/*!
 * \brief Return the maximum element in the tree.
 *
 * Return the maximum elelment as per application-provided comparison
 * function during AVL tree creation.
 *
 * \param [in] avl AVL tree handle.
 * \param [out] datum Datum to receive element.
 *
 * \retval 0 Success, found and retrieved datum.
 * \retval -1 Tree empty.
 */
extern int
shr_avl_lookup_max(shr_avl_t *avl,
                   shr_avl_datum_t *datum);

/*!
 * \brief Call a callback function for each datum in an AVL tree.
 *
 * The callback routine may not modify the AVL tree being traversed.
 *
 * \param [in] avl AVL tree handle.
 * \param [in] trav_fn User callback function, called once per datum.
 * \param [in] trav_data Arbitrary user data passed to callback routine.
 *
 * \return If the user callback ever returns a negative value, the
 *	   traversal is stopped and that value is returned.  Otherwise
 *	   0 is returned.
 */
extern int
shr_avl_traverse(shr_avl_t *avl,
                 shr_avl_traverse_fn trav_fn,
                 void *trav_data);

#endif  /* SHR_AVL_H */
