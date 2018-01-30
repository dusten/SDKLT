/*! \file be_internals.h
 *
 * In-Memory back-end internal API.
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

#ifndef BE_INTERNALS_H
#define BE_INTERNALS_H

/*!
 * \brief Element pointer size.
 *
 * sub component code (bits 24-31) - That is the HA block identifier.
 * occupied bit (bit 17) - (only for entry) indicates if the entry is in use.
 * locked bit (bit 16) - (only for entry) indicates if the entry is locked.
 * offset (bits 0-15) - That is the offset of the element in the HA block.
 * The offset size allows for up to 16K entries per HA block.
 */
#define BCMIMM_ELEM_PTR_SIZE      4
#define BCMIMM_SUB_OFFST        24  /*!< Sub offset in HA pointer */
#define BCMIMM_LOCK_BIT         16  /*!< Lock bit number in an entry pointer */
#define BCMIMM_OCCUPY_BIT       17  /*!< Occupy bit in an entry pointer */
#define BCMIMM_OFFSET_MASK      0xFFFF  /*!< mask for the offset bits */
#define BCMIMM_PTR_CTRL_MASK    (0xF << 16)

/*!
 * \brief Reference count size
 *
 * This indicates the size of the reference count (in bytes). The
 * reference count is an unsigned number that can reach 64K
 */
#define BCMIMM_ELEM_REF_CNT_SIZE    2

/*!
 * \brief HA block signature.
 *
 * Every HA block, once assigned and set, will have a signature field. The
 * in-mem back-end validate the integrity of an HA block using the signature.
 * An unsigned HA block is considered new block while signed block considered
 * blocks that recovered during warm boot.
 */
#define BCMIMM_BLK_SIGNATURE   0xe68a24bb

/*!
 * \brief HA pointer constructor macro.
 *
 * \param [out] x This is the pointer value.
 * \param [in] sub This is the sub module ID.
 * \param [in] offset This is the offset of the element from the begining
 * of the block.
 */
#define BCMIMM_SET_PTR(_x, _sub, _offset) \
    _x |= ((_sub) << BCMIMM_SUB_OFFST) | ((_offset) & BCMIMM_OFFSET_MASK)

/*!
 * \brief HA pointer assignment macro.
 *
 * It is important to notice that the control portion is beeing preserved at
 * the destination.
 *
 * \param [in,out] ptr_d This is the destination pointer to copy to.
 * \param [in] ptr_s This is the source pointer to copy from.
 */
#define BCMIMM_ASSIGN_PTR(_ptr_d,_ptr_s) \
    _ptr_d = ((_ptr_s) & (~BCMIMM_PTR_CTRL_MASK)) | \
              (_ptr_d & BCMIMM_PTR_CTRL_MASK)

/*!
 * \brief Set occupied flag macro.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_OCCUPIED_SET(_x) _x |= (1 << BCMIMM_OCCUPY_BIT)

/*!
 * \brief Clear occupied flag macro.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_OCCUPIED_CLEAR(_x) _x &= (~(1 << BCMIMM_OCCUPY_BIT))

/*!
 * \brief Is occupied flag set macro.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_IS_OCCUPIED(_x) (_x & (1 << BCMIMM_OCCUPY_BIT))

/*!
 * \brief Is the pointer 'NULL' macro.
 *
 * Ignore occupied and lock bits
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_IS_NULL_PTR(_x) (((_x) & (~BCMIMM_PTR_CTRL_MASK)) == 0)

/*!
 * \brief Obtain the element offset from a pointer.
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_OFFSET_GET(_x) (_x) & 0x7FFF

/*!
 * \brief Obtain the sub module value from a pointer.
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_SUB_GET(_x) ((_x) >> BCMIMM_SUB_OFFST) & 0xFF

/*!
 * \brief Set the sub module value into a pointer.
 *
 * \param [out] x This is the pointer value.
 * \param [in] sub This is the sub module value.
 */
#define BCMIMM_SUB_SET(_x, _sub) _x |= ((_sub) << BCMIMM_SUB_OFFST)

/*!
 * \brief Mark the pointer of an entry as locked.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_ENTRY_LOCK(_x) _x |= 1 << BCMIMM_LOCK_BIT

/*!
 * \brief Mark the pointer of an entry as unlocked.
 *
 * \param [out] x This is the pointer value.
 */
#define BCMIMM_ENTRY_UNLOCK(_x) _x &= ~(1 << BCMIMM_LOCK_BIT)

/*!
 * \brief Does the pointer of an entry locked.
 *
 * \param [in] x This is the pointer value.
 */
#define BCMIMM_IS_LOCKED(_x) (_x) & (1 << BCMIMM_LOCK_BIT)

/*!
 * \brief Obtain the value of the entry lock reference.
 *
 * The entry structure is that the last two words (4 bytes each) are the
 * entry pointer and field pointers. The two bytes prior are the lock
 * reference count.
 *
 * \param [in] _ent Is the entry to check.
 * \param [in] _ent_len Is the table (and entry) entry length (bytes).
 *
 * \return Pointer to the element reference lock count.
 */
#define BCMIMM_ENTRY_LOCK_CNT_GET(_ent, _ent_len) \
    (uint16_t *)((_ent) + (_ent_len) -              \
    2 * BCMIMM_ELEM_PTR_SIZE - sizeof(uint16_t))

/*!
 * \brief Return a pointer to the entry pointer field.
 *
 * \param [in] ent This is the entry pointer.
 * \param [in] end_len This is the entry length (in bytes).
 */
#define BCMIMM_ENTRY_PTR_GET(_ent, _ent_len) \
    (uint32_t *)((_ent) + (_ent_len) - BCMIMM_ELEM_PTR_SIZE)

/*!
 * \brief Return a pointer to the field's pointer field.
 *
 * \param [in] ent This is the entry pointer.
 * \param [in] data_len This is the data length (in bytes) as it was
 * specified during the table create phase.
 */
#define BCMIMM_ENTRY_FLD_PTR_GET(_ent, _data_len) \
    (uint32_t *)((_ent) + (_data_len))

/*!
 * \brief Data structure maintains the element list for particular element size.
 *
 * All the element blocks are HA memory blocks. The in-mem back-end allocates
 * HA blocks to maintain fixed amount of elements per block. These elements
 * are used to store the data fields or the entries in the linked list.
 * If all the elements in the HA blocked had been consumed the in-mem back-end
 * allocates an addition HA block and makes more data elements available.
 * The newly allocated HA block will be pointed by few blocks as follows:
 * a. The previous HA block maintains the sub module ID of this block.
 * b. The pointer to the block will be stored in the \c blk array. The
 * index used in the \c blk array is the block index. The HA pointers
 * maintains this index. Note that the index is an index in the array of
 * a regular memory. Therefore it is important to recreate this structure
 * with the exact way it was in previous run.
 * The following diagram illustrates the way the free blocks are organized.
 *
 *   First block sub=1
 *   |---------------|  |--> |---------------------|  |-->...
 *   | FL | ext |next|--|    | FL  | ext=0|  next--|--|
 *|--|-|----|--------|       |---------------------|
 *|  |<-  |ptr|> |ptr|       |                     |
 *|  |---------------|       |---------------------|
 *|  |      .        |       |                     |
 *|  |      .        |       |                     |
 *|  |---------------|       |---------------------|
 *|
 *|->|---------------|
 *   |   | ext |     |
 *|--|------|--------|
 *|  |      .        |
 *|  |      .        |
 *|  |---------------|
 *|
 *|
 *|->|---------------|
 *   |   |ext=0|     |
 *   |---------------|
 *   |      .        |
 *   |      .        |
 *   |---------------|
 *
 *   Each one of these vertical blocks is an HA memory. The HA blocks on the
 *   same vertical column named chain. All blocks in the same chain provides
 *   free list (FL) elements of the same size. The ext field in every HA block
 *   is the sub component ID of the block underneath it. A pointer
 *   (real pointer) for each of the blocks of a chain is stored in the \c blk
 *   array of the \ref bcmimm_free_list_t. The block pointer retained in the
 *   \c blk array in the location (index) that matches its order apperance in
 *   the chain. The first chain top block uses known sub module number (1).
 *   Therefore, to recreate the structure \ref bcmimm_free_list_t during warm
 *   boot, the in-mem back-end starts to allocate HA blocks using sub=1. The
 *   first block will have two pointers:
 *   a. ext contains the sub of the next block in the chain.
 *   b. next contains the sub of the top first block of the next element size.
 *
 *   Upon allocation of the first HA block, the in-mem back-end allocates a
 *   structure of type \ref bcmimm_free_list_t. It also allocates a fixed
 *   size of the \c blk array. The field \c blk_array_size is set to the
 *   initial size of the \c blk array. The value for the field \c data_len
 *   is taken from the HA block header. The in-mem back-end continues to
 *   follow the entire chain started from block 1 and keep updating
 *   the structure \ref bcmimm_free_list_t. Once the chain had been processed
 *   the in-mem back-end starts to process another chain by using the next
 *   sub ID of the first block. A new data structure of type
 *   \ref bcmimm_free_list_t is allocated and linked to the \c next field
 *   of the first \ref bcmimm_free_list_t structure.
 */
typedef struct bcmimm_free_list_s {
    uint32_t data_len;       /*!< The element size represented by this chain.*/
    element_list_head_t *master_blk; /*!< The chain master block. */
    int unit;           /*!< The unit involved with this chain. */
    sal_mutex_t lock;     /*!< Multi thread access protection for this list. */
    /*! Point to the next chain of element blocks. */
    struct bcmimm_free_list_s *next;
} bcmimm_free_list_t;

/*!
 * \brief Data structure associated with every table created by the API.
 *
 * Every table associated with the actual table block and two free lists.
 * One list for fields and the other for entries. This data structure being
 * kept in regular memory and being constructed during table create.
 */
typedef struct bcmimm_tbl_info_s {
    /*! Pointer to the table to manage. */
    tbl_header_t *tbl;
    /*! The unit associated with this table. */
    int unit;
    uint32_t fid_array_size;
    /*! Field ID array. Use to sort the fields ID. Contains indexs of fields.*/
    uint32_t *sorted_fid;
    /*! Pointer to the field free list structure. */
    bcmimm_free_list_t *field_free_list;
    /*! Pointer to the entry free list structure. */
    bcmimm_free_list_t *entry_free_list;
    /*! Table read/write lock. */
    sal_rwlock_t lock;
} bcmimm_tbl_info_t;

/*!
 * \brief The bcmimm back-end component ID.
 */
extern uint8_t bcmimm_be_comp_id;

/*!
 * \brief Counter for used sub component values per unit.
 */
extern uint8_t bcmimm_available_subs[BCMDRD_CONFIG_MAX_UNITS];

/* Maintains a link list of all the free fields (of all sizes) */
extern bcmimm_free_list_t *bcmimm_free_elem[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Free list element get.
 *
 * This function obtains an element from the free list.
 *
 * \param [in] unit The device number associated with this request.
 * \param [in] elem_fr_lst Pointer to the element free list.
 * \param [out] ptr HA pointer to the newly allocated element.
 *
 * \return Pointer to the element on success and NULL otherwise.
 */
extern uint8_t *bcmimm_free_list_elem_get(int unit,
                                       bcmimm_free_list_t *elem_fr_lst,
                                       uint32_t *ptr);

/*!
 * \brief Return element list to the free list
 *
 * This function returns previously allocated elements (allocated using
 * \ref bcmimm_free_list_elem_get()) into the free list so these elements
 * can be used again. The variable \c ptr is an HA pointer to the first
 * element in the list. The function go through the list pointed by \c ptr
 * and finds the last element. It then set the pointer of the last element
 * to point to the first element in the free list. Last, it sets the free
 * list pointer (obtained from the parameter \c elem_fr_lst) to the begining
 * of the list (the original value of the parameter \c ptr).
 *
 * \param [in] elem_fr_lst This is the element free list control structure.
 * \param [in] ptr This is HA pointer pointing to the first element in the
 * list to free.
 *
 * \return None.
 */
extern void bcmimm_free_list_elem_put(bcmimm_free_list_t *elem_fr_lst,
                                   uint32_t ptr);

/*!
 * \brief Search for the free list to match the required size.
 *
 * This function seaches the master list for a free list that can be used
 * to allocate element of the desired size. To reduce the number of free
 * lists, this function accepts lists that are larger by up to 2 bytes
 * from the desired size. If no list is found to satisfy the required size
 * the function will allocate a new list to match the desired size.
 *
 * \param [in] unit The device number associated with this request.
 * \param [in] size This is the desired (minimal) element size.
 * \param [in] list This is a pointer to the master list. The master
 * list will be updated if the function allocates a new free-list.
 *
 * \return Pointer to the desired free list on success and NULL otherwise.
 */
extern bcmimm_free_list_t *bcmimm_free_list_find(int unit,
                                                size_t size,
                                                bcmimm_free_list_t **list);

/*!
 * \brief Obtains an entry from a free list and HA pointer.
 *
 * This function obtains an entry from a free list using HA pointer value.
 * The HA pointer points to the desired entry within the element list.
 *
 * \param [in] elem_lst This is the element list containing the desired entry.
 * \param [in] ptr This is HA pointer, pointing to the desired entry inside
 * the element list
 *
 * \return Pointer to the entry on success and NULL otherwise.
 */
extern uint8_t *bcmimm_entry_get (bcmimm_free_list_t *elem_lst, uint32_t ptr);

/*!
 * \brief Inserts data field into an entry.
 *
 * This function converts the data array with data_element field into a linked
 * list of fields. Each field has a fixed size determine at table creation.
 * The function allocates memory fields from its associated free list
 * (association was established at table creation). The first element should
 * be pointed by the input parameter ent_ptr. All the rest of the fields being
 * pointed by the previous field. It is important to maintain the same order
 * of the data with the linked list.
 *
 * \param [in] t_info This is the table information containing the entry.
 * \param [in] ent_ptr This is the HA pointer in the entry that points to the
 * list of data fields.
 * \param [in] in This is a structure containing the fields values and IDs.
 * The structure \c in contains a field named \c count that indicates the
 * number of fields in the input. The fields information being partition into
 * two synchronized arrays. One with the field IDs and the other with the
 * field data. The arrays synchronized in a way that the j'th element of the
 * \c fid array correspond to the data of the j'th element of \c fdata array.
 *
 * \return SHR_E_NONE success, SHR_E_MEMORY if failed to allocate internal
 * memory.
 */
extern int bcmimm_data_field_insert(bcmimm_tbl_info_t *t_info,
                                  uint32_t *ent_ptr,
                                  const bcmimm_be_fields_t *in);

/*!
 * \brief Retrieve linked list data fields into data array.
 *
 * This function performs the inverse functionality of the function
 * \ref bcmimm_data_field_insert. It parses the fields linked list and copied the
 * data into the buffer provided by the caller.
 *
 * \param [in] t_info This is the table information containing the entry.
 * \param [in] ent_ptr This is the HA pointer to the first data field of the
 * list.
 * \param [out] out This is a structure where the entry data should be copied
 * into. The \c out structure contains two arrays and a count field indicating
 * the size of each array. Upon return the count field will include the number
 * of elements that were actually copied into the \c out structure.
 *
 * \return SHR_E_NONE success, SHR_E_FULL if there is insufficient space in the
 * output buffer (\c out).
 */
extern int bcmimm_data_field_get(bcmimm_tbl_info_t *t_info,
                               uint32_t ent_ptr,
                               bcmimm_be_fields_t *out);

/*!
 * \brief Updates linked list data fields.
 *
 * This function updates the fields provided in the input parameter to their
 * desired value. Fields that are not specified will be kept. The input
 * parameter contains an action for every field. The action can be to add
 * delete or modify the field data.
 * This function uses the fact that the link list is sorted based on field ID
 * to achieve faster performance. The alternative will require search for
 * every input field in the linked list (O(n^2) vs. O(n log n)).
 *
 * \param [in] t_info This is the table information containing the entry.
 * \param [in] ent_ptr This is the HA pointer to the first data field of the
 * list.
 * \param [in] in This is a structure containing the fields values and IDs.
 * The structure \c in contains a field named \c count that indicates the
 * number of entry fields in the input. The entry fields information being
 * partition into three synchronized arrays. One with the field IDs, one
 * with the desired field operation and the last with the entry
 * field data. The arrays synchronized in a way that the j'th element of the
 * \c fid array correspond to the data of the j'th element of \c fdata array.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_data_field_update(bcmimm_tbl_info_t *t_info,
                                  uint32_t *ent_ptr,
                                  const bcmimm_be_fields_upd_t *in);

/*!
 * \brief Search for an entry in the table.
 *
 * This function search for the entry with a matching key inside the table
 * pointed by \c t_info. If found, the function will also provide the entry
 * pointer field of the entry. If the parameter \c prev_ent is not NULL, the
 * function will also return a pointer to the previous entry (if one exists)
 * that points to the entry that was searched.
 *
 * \param [in] t_info This is point to the table information.
 * \param [in] key This is the key value to be used during the search.
 * \param [out] ent_ptr This will point to the entry pointer field in
 * the searched entry.
 * \param [out] prev_ent This pointer points to the entry that preceded the
 * searched entry.
 *
 * \return Pointer to the searched entry if entry was found or NULL otherwise.
 */
extern uint8_t *bcmimm_entry_find(bcmimm_tbl_info_t *t_info,
                                const void *key,
                                uint32_t **ent_ptr,
                                uint8_t **prev_ent);

/*!
 * \brief Retrieve the free element blocks in warm boot.
 *
 * This function starts from a given sub module value and retrieve all the
 * element blocks (free and allocated) from the previous run. The function
 * parses the higher row of the element blocks by following the \c next field
 * of the \ref bcmimm_free_list_t structure. For each block found, it allocates
 * the entire chain. See \ref bcmimm_free_list_t for more details.
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] sub This is the sub module to start with.
 *
 * \return SHR_E_NONE success, SHR_E_MEMORY on error.
 */
extern int bcmimm_free_lists_retrieve(int unit, uint8_t sub);

/*!
 * \brief Clear the table from its content.
 *
 * This function free all the entries of the table. The state of the table
 * after the clear will match the state of the table after it had just been
 * created during cold boot.
 *
 * \param [in] t_info This is a pointer to all the relevant information
 * associated with the table.
 *
 * \return None.
 */
extern void bcmimm_table_content_clear(bcmimm_tbl_info_t *t_info);

/*!
 * \brief Finds the next entry in a table.
 *
 * This function searches for the next entry in the table using linear search.
 * The search starts from the index and pointer provided as inputs. As a result
 * of the search, these parameters will be updated.
 * Note that the search for the entry starts from the entry that follows the
 * \c start_idx or \c start_ptr. Typically, the \c start_ptr is essentially
 * the entry->next pointer (HA pointer), so if not NULL the next entry will
 * be the entry pointed by \c start_ptr. If \c start_ptr is NULL the function
 * increments the index \c start_idx and starts searching from there.
 *
 * \param [in] t_info This is a pointer to all the relevant information
 * associated with the table.
 * \param [in] first Indicates if this is get first or not.
 * \param [in] start_idx Start the search of an entry from this index
 * (slot).
 * \param [in,out] start_ptr Start the search from this pointer HA pointer.
 * This pointer is not 0 in case multiple entries are at the same slot.
 *
 * \return Pointer to the next entry if found and NULL if no more entries
 * are available.
 */
extern uint8_t *bcmimm_next_entry_find(bcmimm_tbl_info_t *t_info,
                                       bool first,
                                       uint32_t start_idx,
                                       uint32_t *start_ptr);

#endif /* BE_INTERNALS_H */
