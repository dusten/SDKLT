/*! \file bcmltm_md_pthru.c
 *
 * Logical Table Manager Physical Table Pass Through (Thru) Metadata.
 *
 * This is the most basic case. Each physical symbol will have a
 * 'logical table' representation, with a 1-1 mapping of all its
 * physical fields to logical fields for a given table.
 *
 * The Physical Table Pass Thru metadata uses the symbol IDs from the
 * DRD component.  These symbol IDs are the identification for all
 * the physical tables (register and memories) defined on a given
 * device.
 *
 * The DRD symbol IDs are defined in a compact numbering
 * space [0.. n-1], where 'n' is the total number of DRD symbols for
 * a given device.
 *
 * The design of this component is based on the DRD symbol ID attribute
 * define above.   The symbol ID is used as the array index into
 * several of the table metadata data structure.
 *
 * The DRD symbol ID value is used as the LTM table ID.
 *
 * PT Pass Thru Metadata Memory:
 *
 * There are currently 2 ways in which the memory for the metadata is
 * allocated:
 * a) Memory pool:  These memories are pre-allocated to fit information
 *    for a large number of objects.  The memory is freed only when
 *    when the entire LTM metadata is destroyed (not when each table is
 *    destroyed).  There is no 'cleanup' or free of memory at the table level.
 * b) Allocation on-the-fly:  These memories are allocated as the table
 *    metadata is created.  These memory must be freed when the
 *    corresponding table and corresponding data is destroyed.
 * After more cases are developed, a final common approach shall be decided.
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

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>

#include <bcmltd/bcmltd_table.h>

#include <bcmptm/bcmptm.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include <bcmltm/bcmltm_md_pthru_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/* Maximum number of field indexes for a wide field */
#define MAX_FIELD_IDXS             10


/*
 * Buffer memory layout for PT Pass Thru:
 *     +---------------------------------------------+
 *     | uint32 sbus_data_count                      |
 *     +---------------------------------------------+
 *     | uint32_t index                              |
 *     +---------------------------------------------+
 *     | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] |
 *     +---------------------------------------------+
 *
 * Number of PT operations for any Physical Table is 1.
 */

/* Number of roots for 'set' operation */
#define PTHRU_OP_SET_NUM_ROOTS   1

/* Number of roots for 'modify' operation */
#define PTHRU_OP_MODIFY_NUM_ROOTS   2

/* Number of roots for 'get' operation */
#define PTHRU_OP_GET_NUM_ROOTS   2

/* Number of roots for 'lookup' operation */
#define PTHRU_OP_LOOKUP_NUM_ROOTS   2

/* Number of PT views required for a LT (symbol) */
#define PTHRU_NUM_PT_VIEWS       1

/* Number of PT ops required for a PT view */
#define PTHRU_NUM_PT_OPS      1

/* SBUS Data Count (sum of all PT's ops for a logical entry) */
#define PTHRU_SBUS_DATA_COUNT    1

/* Types of fields */
#define PTHRU_FIELD_TYPE_KEY     0
#define PTHRU_FIELD_TYPE_VALUE   1

/*
 * Special Field IDs for Keys
 *
 * The DRD does not provide field IDs for the index (memory/register array)
 * or port (port-based register) keys.
 */
/* Index key for memories and array registers */
#define PTHRU_FIELD_KEY_INDEX       (BCMLTM_PT_FIELD_KEY_INDEX)
/* Port key for Port-based reg */
#define PTHRU_FIELD_KEY_PORT        (BCMLTM_PT_FIELD_KEY_PORT)
/* Instance key to access table instances (i.e. pipe) */
#define PTHRU_FIELD_KEY_INSTANCE    (BCMLTM_PT_FIELD_KEY_INSTANCE)

/* Special Key Field Min and Max bits */
#define PTHRU_FIELD_KEY_INDEX_MINBIT        0
#define PTHRU_FIELD_KEY_INDEX_MAXBIT       31
#define PTHRU_FIELD_KEY_PORT_MINBIT         0
#define PTHRU_FIELD_KEY_PORT_MAXBIT        31
#define PTHRU_FIELD_KEY_INSTANCE_MINBIT     0
#define PTHRU_FIELD_KEY_INSTANCE_MAXBIT    31

/* Special Key Field Names */
#define PTHRU_FIELD_KEY_INDEX_NAME     "BCMLTM_PT_INDEX"
#define PTHRU_FIELD_KEY_PORT_NAME      "BCMLTM_PT_PORT"
#define PTHRU_FIELD_KEY_INSTANCE_NAME  "BCMLTM_PT_INSTANCE"


/* Default number of elements */
#define PTHRU_POOL_BLOCK_DEF_COUNT    1000


/*
 * Accessor macros for PT Pass Thru info
 */
#define PTHRU_INFO(_u)                 pthru_info[_u]
#define PTHRU_LTM_MD(_u)               PTHRU_INFO(_u)->ltm_md
#define PTHRU_SID_MAX_COUNT(_u)        PTHRU_INFO(_u)->sid_max_count
#define PTHRU_POOL(_u)                 PTHRU_INFO(_u)->pool
#define PTHRU_PT_LIST(_u)              PTHRU_INFO(_u)->pt_list
#define PTHRU_PT_LIST_SID(_u, _sid)    PTHRU_INFO(_u)->pt_list[_sid]
#define PTHRU_PTM_MAPPING_KEYS(_u)              \
    PTHRU_INFO(_u)->ptm_mapping_keys
#define PTHRU_PTM_MAPPING_KEYS_SID(_u, _sid)    \
    PTHRU_INFO(_u)->ptm_mapping_keys[_sid]
#define PTHRU_PTM_MAPPING_VALUES(_u)            \
    PTHRU_INFO(_u)->ptm_mapping_values
#define PTHRU_PTM_MAPPING_VALUES_SID(_u, _sid)  \
    PTHRU_INFO(_u)->ptm_mapping_values[_sid]


/*
 * Helper macros
 */
/* Check that PT info has been initialized */
#define PTHRU_INFO_INIT(_u)                     \
    (PTHRU_INFO(_u) != NULL)

/* Check that symbold id (physical table id) is valid */
#define PTHRU_SID_VALID(_u, _sid)                           \
    ((_sid) < PTHRU_SID_MAX_COUNT(_u))

/*
 * The following memory alloc/free macros are defined
 * to isolate the underneath method in which a given
 * memory needed during the LTM metadata creation is obtained.
 *
 * Using these macros will make it easier to transition in case
 * the underneath implementation in how memory is obtained and released
 * changes.
 *
 * For memory pool, the memory is only freed when the entire
 * LTM metadata on a unit is cleanup, so the free for the individual
 * object is a no-op.
 */
/* (bcmltm_lt_md_t *) */
#define PTHRU_LT_MD_ALLOC(_u, _sid)             \
    &PTHRU_POOL(_u)->lt_md[_sid];

#define PTHRU_LT_MD_FREE(_ptr)      /* N/A since memory is from pool */

/* (bcmltm_lt_params_t *) */
#define PTHRU_PARAMS_ALLOC(_u, _sid)            \
    &PTHRU_POOL(unit)->params[sid];

#define PTHRU_PARAMS_FREE(_ptr)     /* N/A since memory is from pool */

/* (bcmltm_lt_op_md_t *) */
#define PTHRU_OP_MD_ALLOC(_u, _op, _sid)        \
    &PTHRU_POOL(_u)->op_md[_op][_sid];

#define PTHRU_OP_MD_FREE(_ptr)      /* N/A since memory is from pool */

/* (bcmltm_node_t **) */
#define PTHRU_NODE_PTRS_ALLOC(_u, _count)       \
    pthru_pool_node_ptrs_alloc(_u, _count)

#define PTHRU_NODE_PTRS_FREE(_ptr)  /* N/A since memory is from pool */


/*
 * PT Pass Thru Memory Pools
 *
 * This adaption of memory pool management is simple and fits
 * the PT Pass Thru needs.
 *
 * The memories are allocated as big blocks to fit the data
 * for all the physical tables data for a given object type.
 * A chunk of allocated memory within the pool is given as
 * the memory is requested.
 *
 * The assumption is that the LTM metadata for all the physical tables
 * in a unit is created once during the device initialization and then
 * destroyed (all) when the device is detached.  Tables are not
 * created or destroyed constantly.  As a result, there is not a
 * routine that frees or releases a given chunk back to the pool.
 *
 * As the metadata is constructed, the memory from the different pools
 * is given through various ways (based on the nature of the data):
 * - Memory block using the SID as index
 * - Next available block in the pool (for a given fixed size)
 */

/*!
 * Memory Pool Block.
 *
 * This is a helper management pool interface to handle a generic
 * data size.  Callers need to provide corresponding definition and
 * wrapper for a given data type that requires a given size.
 */
typedef struct pthru_pool_block_s {
    /*! Next block of memory. */
    struct pthru_pool_block_s *next;

    /*! Total elements allocated. */
    size_t max_count;

    /*! Current number of elements used. */
    size_t used_count;

    /*! Buffer pool. */
    void *buffer;
} pthru_pool_block_t;

/*!
 * Memory Pool Container.
 *
 * This contains a series of pools for different types of
 * fixed-size memory chunks (elements for a given data size).
 */
typedef struct pthru_pool_s {
    /*! LT metadata, indexed by SID. */
    bcmltm_lt_md_t *lt_md;

    /*! Params, indexed by SID. */
    bcmltm_lt_params_t *params;

    /*! OP metadata, indexed by SID. */
    bcmltm_lt_op_md_t *op_md[BCMLT_PT_OPCODE_NUM];

    /*! Pointer to node pointers, use pool_block. */
    pthru_pool_block_t *node_ptrs;
} pthru_pool_t;


/*!
 * PT Pass Thru Information.
 */
typedef struct pthru_info_s {
    /*! Total number of symbols. */
    size_t sid_max_count;

    /*! Memory pool container. */
    pthru_pool_t *pool;

    /*!
     * Array of pointers to bcmltm_pt_list used as the EE node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     *
     * The index into the list is the SID of the table.
     */
    bcmltm_pt_list_t **pt_list;


    /*!
     * Array of pointers to bcmltm_to_ptm_mapping_t for key fields used
     * as the FA Keys node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     *
     * The index into the list is the SID of the table.
     */
    bcmltm_to_ptm_mapping_t **ptm_mapping_keys;

    /*!
     * Array of pointers to bcmltm_to_ptm_mapping_t for value fields used
     * as the FA Values node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     *
     * The index into the list is the SID of the table.
     */
    bcmltm_to_ptm_mapping_t **ptm_mapping_values;

    /*! LTM device metadata (last) */
    bcmltm_md_t *ltm_md;

} pthru_info_t;


/* PT Pass Thru Information Data */
static pthru_info_t *pthru_info[BCMLTM_MAX_UNITS];



/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocate dynamic memory of given buffer size.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] size Size to allocate.
 * \param [in] str String display for alloc function.
 * \param [out] pool_ptr Pointer to allocated buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pool_alloc(int unit, unsigned int size,
                 char *str, void **pool_ptr)
{
    void *pool = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pool block */
    SHR_ALLOC(pool, size, str);
    SHR_NULL_CHECK(pool, SHR_E_MEMORY);
    sal_memset(pool, 0, size);

    *pool_ptr = pool;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s alloc=0x%lx size=%d\n"),
                 str, (long unsigned int)pool, size));

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free dynamic memory.
 *
 * Free dynamic memory for given pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] pool Pointer of buffer to free.
 * \param [in] str String display for free function.
 */
static void
pthru_pool_free(int unit, void *pool, char *str)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s free=0x%lx\n"),
                 str, (long unsigned int)pool));
    SHR_FREE(pool);
}


/*!
 * \brief Free a memory pool block.
 *
 * Free dynamic memory for a pool block type.
 *
 * \param [in] unit Unit number.
 * \param [in] block_head Head of the memory pool block type to free.
 * \param [in] sub_str String display for free function.
 */
static void
pthru_pool_block_destroy(int unit, pthru_pool_block_t *block_head,
                         const char *sub_str)
{
    pthru_pool_block_t *block = block_head;
    pthru_pool_block_t *next;
    char str[80];

    sal_sprintf(str,  "LTM PT Pass Thru block pool %s", sub_str);
    while (block != NULL) {
        next = block->next;
        pthru_pool_free(unit, (void *)block, str);
        block = next;
    }

    return;
}


/*!
 * \brief Allocate a memory pool block.
 *
 * Allocate a block of memory and add it to the list of memory pool block.
 * A memory pool block type is a link list of blocks of memory.
 *
 * A block of memory contains 'n' number of chunks.  The usage is that
 * the chunks within a block are of the same size.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [out] block_head Head of list to add allocated memory block to.
 * \param [in] data_size Size of the block chunk.
 * \param [in] max_count Number of chunks allocated.
 * \param [in] sub_str String display for alloc function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pool_block_create(int unit, pthru_pool_block_t **block_head,
                        size_t data_size, size_t max_count,
                        const char *sub_str)
{
    unsigned int size;
    pthru_pool_block_t *block = NULL;
    void *alloc_ptr = NULL;
    char str[80];

    SHR_FUNC_ENTER(unit);

     /* Allocate pool */
    size = sizeof(pthru_pool_block_t) + (data_size * max_count);
    sal_sprintf(str,  "LTM PT Pass Thru block pool %s", sub_str);
    SHR_IF_ERR_EXIT
        (pthru_pool_alloc(unit, size, str, &alloc_ptr));

    block = (pthru_pool_block_t *)alloc_ptr;
    block->max_count = max_count;
    block->used_count = 0;
    block->next = *block_head;
    block->buffer = (void *)&block[1];
    *block_head = block;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free memory allocated for the LT metadata.
 *
 * Free memory allocated for the LT metadata.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_pool_lt_md_destroy(int unit)
{
    pthru_pool_free(unit, (void *)PTHRU_POOL(unit)->lt_md,
                    "LTM PT Pass Thru lt_md pool");
    PTHRU_POOL(unit)->lt_md = NULL;

    return;
}


/*!
 * \brief Allocate memory to store the LT metadata.
 *
 * Allocate memory to store the LT metadata bcmltm_lt_md_t
 * for all tables IDs.
 *
 * This routine is used to pre-allocate memory which is later
 * used when the LTM metadata is built.  It does not populate the metadata.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *   - Total number of table IDs is known during initialization and
 *     does not change.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pool_lt_md_create(int unit)
{
    unsigned int size;
    size_t max_count;
    void *alloc_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    max_count = PTHRU_SID_MAX_COUNT(unit);

    /* Allocate pool */
    size = sizeof(bcmltm_lt_md_t) * max_count;
    SHR_IF_ERR_EXIT
        (pthru_pool_alloc(unit, size,
                          "LTM PT Pass Thru lt_md pool",
                          &alloc_ptr));

    PTHRU_POOL(unit)->lt_md = (bcmltm_lt_md_t *)alloc_ptr;
 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free memory allocated for LT metadata params.
 *
 * Free memory allocated for the LT metadata params.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_pool_params_destroy(int unit)
{
    pthru_pool_free(unit, (void *)PTHRU_POOL(unit)->params,
                    "LTM PT Pass Thru params pool");
    PTHRU_POOL(unit)->params = NULL;

    return;
}


/*!
 * \brief Allocate memory to store the LT metadata params.
 *
 * Allocate memory to store the LT metadata params bcmltm_lt_params_t
 * for all tables IDs.
 *
 * This routine is used to pre-allocate memory which is later
 * used when the LTM metadata is built.  It does not populate the metadata.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *   - Total number of table IDs is known during initialization and
 *     does not change.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pool_params_create(int unit)
{
    unsigned int size;
    size_t max_count;
    void *alloc_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    max_count = PTHRU_SID_MAX_COUNT(unit);

    /* Allocate pool */
    size = sizeof(bcmltm_lt_params_t) * max_count;
    SHR_IF_ERR_EXIT
        (pthru_pool_alloc(unit, size,
                          "LTM PT Pass Thru params pool",
                          &alloc_ptr));

    PTHRU_POOL(unit)->params = (bcmltm_lt_params_t *)alloc_ptr;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free memory allocated for LT metadata operations.
 *
 * Free memory allocated for the LT metadata operations.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_pool_op_md_destroy(int unit)
{
    bcmlt_pt_opcode_t opcode;
    char str[80];

    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        sal_sprintf(str, "LTM PT Pass Thru lt_op_md %d pool", opcode);
        pthru_pool_free(unit, (void *)PTHRU_POOL(unit)->op_md[opcode], str);
        PTHRU_POOL(unit)->op_md[opcode] = NULL;
    }

    return;
}


/*!
 * \brief Allocate memory to store the LT metadata operations.
 *
 * Allocate memory to store the LT metadata operations bcmltm_lt_op_md_t
 * for all operations for all tables IDs.
 *
 * This routine is used to pre-allocate memory which is later
 * used when the LTM metadata is built.  It does not populate the metadata.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *   - Total number of table IDs is known during initialization and
 *     does not change.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pool_op_md_create(int unit)
{
    unsigned int size;
    size_t max_count;
    bcmlt_pt_opcode_t opcode;
    char str[80];
    void *alloc_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    max_count = PTHRU_SID_MAX_COUNT(unit);

    /* Allocate pool */
    size = sizeof(bcmltm_lt_op_md_t) * max_count;
    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        if (opcode == BCMLT_PT_OPCODE_NOP) {
            continue;
        }
        sal_sprintf(str, "LTM PT Pass Thru lt_op_md %d pool", opcode);
        SHR_IF_ERR_EXIT
            (pthru_pool_alloc(unit, size,
                              str,
                              &alloc_ptr));

        PTHRU_POOL(unit)->op_md[opcode] = (bcmltm_lt_op_md_t *)alloc_ptr;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free memory allocated for the LT node pointers.
 *
 * Free memory allocated for the LT node pointers.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_pool_node_ptrs_destroy(int unit)
{
    pthru_pool_block_destroy(unit,
                             PTHRU_POOL(unit)->node_ptrs,
                             "node_ptrs");
    PTHRU_POOL(unit)->node_ptrs = NULL;

    return;
}


/*!
 * \brief Allocate memory for storing LT node pointers.
 *
 * Allocate memory to store the LT node pointers.
 *
 * This routine is used to pre-allocate memory which is later
 * used when the LTM metadata is built.  It does not populate the metadata.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *
 * \param [in] unit Unit number.
 * \param [in] max_count Number of node pointers to allocate.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pool_node_ptrs_create(int unit, size_t max_count)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate pool */
    if (max_count <= 0) {
        max_count = PTHRU_POOL_BLOCK_DEF_COUNT;
    }
    SHR_IF_ERR_EXIT
        (pthru_pool_block_create(unit,
                                 &PTHRU_POOL(unit)->node_ptrs,
                                 sizeof(bcmltm_node_t *), max_count,
                                 "node_ptrs"));

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Return requested memory to hold an array of LT node pointers.
 *
 * Return requested memory for array of LT node pointers.
 *
 * This routine will first get the pointer from the LT node pointers
 * pre-allocate memory.  If not available, it will call appropriate
 * low level routine to allocate necessary memory.
 *
 * Assumes:
 *   - unit is valid.
 *   - Main memory pool container has been created.
 *
 * \param [in] unit Unit number.
 * \param [in] count Number of node pointers needed.
 *
 * \retval Pointer to array of node pointers, of NULL if error.
 */
static bcmltm_node_t **
pthru_pool_node_ptrs_alloc(int unit, size_t count)
{
    int rv = SHR_E_NONE;
    pthru_pool_block_t *block = PTHRU_POOL(unit)->node_ptrs;
    bcmltm_node_t **ptr;
    size_t free_count;
    size_t alloc_count;

    free_count = block->max_count - block->used_count;

    if (free_count < count) {
        alloc_count = (count > PTHRU_POOL_BLOCK_DEF_COUNT) ?
            count : PTHRU_POOL_BLOCK_DEF_COUNT;
        rv = pthru_pool_node_ptrs_create(unit, alloc_count);
        if (SHR_FAILURE(rv)) {
            return NULL;
        }
        block = PTHRU_POOL(unit)->node_ptrs;
    }

    ptr = &((bcmltm_node_t **)block->buffer)[block->used_count];
    block->used_count += count;

    return ptr;
}


/*!
 * \brief Free all the memory allocated for the memory pool.
 *
 * Free memory allocated in the memory pool.
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_pool_destroy(int unit)
{
    if (PTHRU_POOL(unit) == NULL) {
        return;
    }

    /* Free various memory pools */
    pthru_pool_node_ptrs_destroy(unit);
    pthru_pool_op_md_destroy(unit);
    pthru_pool_params_destroy(unit);
    pthru_pool_lt_md_destroy(unit);

    /* Free main memory pool container */
    pthru_pool_free(unit, (void *)PTHRU_POOL(unit),
                    "LTM PT Pass Thru memory pool");
    PTHRU_POOL(unit) = NULL;

    return;
}


/*!
 * \brief Allocate sets of memory pools to use for the LTM metadata.
 *
 * This routine is used to pre-allocate a series of memory pools
 * which is later used when the LTM metadata is built.
 * It does not populate the metadata.
 *
 * Assumes:
 *   - unit is valid.
 *   - Total number of table IDs is known during initialization and
 *     does not change.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Total number of tables.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pool_create(int unit, size_t sid_max_count)
{
    unsigned int size;
    void *alloc_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    size = sizeof(pthru_pool_t);
    SHR_IF_ERR_EXIT
        (pthru_pool_alloc(unit, size,
                          "LTM PT Pass Thru memory pool",
                          &alloc_ptr));

    PTHRU_POOL(unit) = (pthru_pool_t *)alloc_ptr;

    /* Create memory pools for different objects */
    SHR_IF_ERR_EXIT(pthru_pool_lt_md_create(unit));
    SHR_IF_ERR_EXIT(pthru_pool_params_create(unit));
    SHR_IF_ERR_EXIT(pthru_pool_op_md_create(unit));
    SHR_IF_ERR_EXIT(pthru_pool_node_ptrs_create(unit, sid_max_count));

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pool_destroy(unit);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given table PT mapping.
 *
 * Destroy the given table PT mapping.
 *
 * \param [in] pt_map Pointer to table PT mapping to destroy.
 */
static void
pthru_pt_map_destroy(bcmltm_map_pt_t *pt_map)
{
    if (pt_map == NULL) {
        return;
    }

    SHR_FREE(pt_map);

    return;
}


/*!
 * \brief Create the table PT mapping for given table.
 *
 * Create the table PT mapping for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] wb_handle Working buffer handle.
 * \param [out] pt_map_ptr Returning pointer to table PT mapping.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_map_create(int unit,
                    bcmdrd_sid_t sid,
                    bcmltm_wb_handle_t *wb_handle,
                    bcmltm_map_pt_t **pt_map_ptr)
{
    unsigned int size;
    bcmltm_map_pt_t *pt_map = NULL;
    const size_t num_pt = 1;  /* PT Pass Thru is 1-1 */
    uint32_t flags;
    uint32_t num_ops = PTHRU_NUM_PT_OPS;
    bcmltm_wb_block_id_t wb_block_id;

    SHR_FUNC_ENTER(unit);

    *pt_map_ptr = NULL;

    flags = bcmltm_wb_ptm_op_flags_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_ptm_pthru_add(unit, sid, flags, num_ops,
                                       wb_handle, &wb_block_id, NULL));

    size = sizeof(*pt_map) + (sizeof(bcmltm_map_pt_info_t) * num_pt);
    SHR_ALLOC(pt_map, size, "LTM PT Pass Thru PT map");
    SHR_NULL_CHECK(pt_map, SHR_E_MEMORY);
    sal_memset(pt_map, 0, size);

    pt_map->num_pt = num_pt;
    pt_map->info[0].sid = sid;
    pt_map->info[0].flags = flags;
    pt_map->info[0].num_ops = num_ops;
    pt_map->info[0].wb_block_id = wb_block_id;

    *pt_map_ptr = pt_map;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_map_destroy(pt_map);
        *pt_map_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given params metadata.
 *
 * Destroy the given params metadata.
 *
 * \param [in] params Pointer to params metadata to destroy.
 */
static void
pthru_params_destroy(bcmltm_lt_params_t *params)
{
    COMPILER_REFERENCE(params);
    PTHRU_PARAMS_FREE(params);
    return;
}


/*!
 * \brief Create the LT params metadata.
 *
 * Create the LT params metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid, ltid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] params_ptr Returning pointer to LT params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_params_create(int unit, bcmdrd_sid_t sid,
                    bcmltm_lt_params_t **params_ptr)
{
    bcmltm_lt_params_t *params = NULL;
    int index_min;
    int index_max;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    params = PTHRU_PARAMS_ALLOC(unit, sid);
    SHR_NULL_CHECK(params, SHR_E_MEMORY);

    index_min = bcmdrd_pt_index_min(unit, sid);
    index_max = bcmdrd_pt_index_max(unit, sid);

    /* Sanity check */
    if ((index_min < 0) || (index_max < 0) || (index_min > index_max)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM metadata params create: "
                              "invalid sid=%d "
                              "index_min=%d index_max=%d\n"),
                   sid, index_min, index_max));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Fill data */
    params->ltid = BCMLTM_SID_TO_LTID(sid);
    params->index_min = index_min;
    params->index_max = index_max;
    params->mem_param_min = 0;
    params->mem_param_max = bcmdrd_pt_num_tbl_inst(unit, sid) - 1;
    /* Not required for PT Pass Thru */
    BCMLTM_HA_PTR_INVALID_SET(params->lt_state_hap);

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_params_destroy(params);
        *params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT param_offsets metadata.
 *
 * Destroy the given PT param_offsets metadata.
 *
 * \param [in] param_offsets Pointer to PT param_offsets metadata to destroy.
 */
static void
pthru_pt_param_offsets_destroy(uint32_t *param_offsets)
{
    if (param_offsets == NULL) {
        return;
    }

    SHR_FREE(param_offsets);

    return;
}


/*!
 * \brief Create the PT param_offsets metadata.
 *
 * Create the PT parameter indexes metadata for the given PT.
 *
 * Assumes arguments are valid (i.e. unit, pt_info, etc.)
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [out] param_offsets_ptr Returning pointer to PT param_offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_param_offsets_create(int unit,
                              const bcmltm_map_pt_info_t *pt_info,
                              const bcmltm_wb_block_t *wb_block,
                              uint32_t **param_offsets_ptr)
{
    uint32_t *param_offsets = NULL;
    unsigned int size;
    uint32_t op_idx;
    uint32_t num_ops = pt_info->num_ops;

    SHR_FUNC_ENTER(unit);

    /* Allocate for all PT ops needed for this PT view */
    size = sizeof(uint32_t) * num_ops;
    SHR_ALLOC(param_offsets, size, "LTM PT param_offsets");
    SHR_NULL_CHECK(param_offsets, SHR_E_MEMORY);
    sal_memset(param_offsets, 0, size);

    for (op_idx = 0; op_idx < num_ops; op_idx++) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_param_base_offset_get(wb_block,
                                                 op_idx,
                                                 &param_offsets[op_idx]));
    }

    *param_offsets_ptr = param_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_param_offsets_destroy(param_offsets);
        *param_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT buffer offsets metadata.
 *
 * Destroy the given PT buffer offsets metadata.
 *
 * \param [in] buffer_offsets Pointer to PT buffer offsets to destroy
 */
static void
pthru_pt_buffer_offsets_destroy(uint32_t *buffer_offsets)
{
    if (buffer_offsets == NULL) {
        return;
    }

    SHR_FREE(buffer_offsets);

    return;
}


/*!
 * \brief Create the PT buffer offsets metadata.
 *
 * Create the PT buffer offsets metadata for the given PT.
 *
 * Assumes arguments are valid (i.e. unit, pt_info, etc.)
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [out] buffer_offsets_ptr Returning pointer to PT buffer offsets md.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_buffer_offsets_create(int unit,
                               const bcmltm_map_pt_info_t *pt_info,
                               const bcmltm_wb_block_t *wb_block,
                               uint32_t **buffer_offsets_ptr)
{
    uint32_t *buffer_offsets = NULL;
    unsigned int size;
    uint32_t op_idx;
    size_t num_ops = pt_info->num_ops;

    SHR_FUNC_ENTER(unit);

    /* Allocate for all PT (ops) needed for this PT view */
    size = sizeof(uint32_t) * num_ops;
    SHR_ALLOC(buffer_offsets, size, "LTM PT buffer_offsets");
    SHR_NULL_CHECK(buffer_offsets, SHR_E_MEMORY);
    sal_memset(buffer_offsets, 0, size);

    for (op_idx = 0; op_idx < num_ops; op_idx++) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_entry_base_offset_get(wb_block,
                                                 op_idx,
                                                 &buffer_offsets[op_idx]));
    }

    *buffer_offsets_ptr = buffer_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_buffer_offsets_destroy(buffer_offsets);
        *buffer_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT op list metadata.
 *
 * Destroy the given PT op list metadata.
 *
 * \param [in] op_list Pointer to PT op list metadata to destroy.
 */
static void
pthru_pt_op_list_destroy(bcmltm_pt_op_list_t *op_list)
{
    if (op_list == NULL) {
        return;
    }

    pthru_pt_buffer_offsets_destroy(op_list->buffer_offsets);
    pthru_pt_param_offsets_destroy(op_list->param_offsets);

    SHR_FREE(op_list);
    return;
}


/*!
 * \brief Create the PT op list metadata.
 *
 * Create the PT op list metadata for the given PT.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, pt_info are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_list_ptr Returning pointer to PT op list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_op_list_create(int unit,
                        const bcmltm_map_pt_info_t *pt_info,
                        const bcmltm_wb_handle_t *wb_handle,
                        bcmltm_pt_op_list_t **op_list_ptr)
{
    bcmltm_pt_op_list_t *op_list = NULL;
    bcmdrd_sid_t sid;
    const bcmltm_wb_block_t *wb_block = NULL;

    SHR_FUNC_ENTER(unit);

    sid = pt_info->sid;

    /* Get working buffer block for PT */
    wb_block = bcmltm_wb_block_find(wb_handle,
                                    pt_info->wb_block_id);
    if (wb_block == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not find working buffer block: "
                              "sid=%d wb_block_id=%d\n"),
                   pt_info->sid, pt_info->wb_block_id));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    SHR_ALLOC(op_list, sizeof(*op_list),
              "LTM PT Pass Thru pt_op_list");
    SHR_NULL_CHECK(op_list, SHR_E_MEMORY);
    sal_memset(op_list, 0, sizeof(*op_list));

    op_list->flags = pt_info->flags;
    op_list->num_pt_ops = pt_info->num_ops;
    op_list->word_size = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (pthru_pt_param_offsets_create(unit, pt_info, wb_block,
                                       &op_list->param_offsets));
    SHR_IF_ERR_EXIT
        (pthru_pt_buffer_offsets_create(unit, pt_info, wb_block,
                                        &op_list->buffer_offsets));

    *op_list_ptr = op_list;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_op_list_destroy(op_list);
        *op_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given list of pointers to PT op list metadata.
 *
 * Destroy the given list of pointers to PT op list metadata.
 *
 * \param [in] num_pt_views Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT op list.
 */
static void
pthru_pt_op_list_ptrs_destroy(size_t num_pt_views,
                              bcmltm_pt_op_list_t **op_list_ptrs)
{
    size_t pt_view;

    if (op_list_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (op_list_ptrs[pt_view] == NULL) {
            continue;
        }
        pthru_pt_op_list_destroy(op_list_ptrs[pt_view]);
    }

    SHR_FREE(op_list_ptrs);
    return;
}


/*!
 * \brief Create the list of pointers to PT op list metadata.
 *
 * Create the list of pointers to PT op list metadata for the given
 * list of physical table IDs.
 *
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit and list of table IDs are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_list_ptrs_ptr Returning pointer to list of PT op list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_op_list_ptrs_create(int unit,
                             const bcmltm_map_pt_t *pt_map,
                             const bcmltm_wb_handle_t *wb_handle,
                             bcmltm_pt_op_list_t ***op_list_ptrs_ptr)
{
    unsigned int size;
    size_t pt_view;
    size_t num_pt_views = pt_map->num_pt;
    bcmltm_pt_op_list_t **op_list_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    size = sizeof(bcmltm_pt_op_list_t *) * num_pt_views;
    SHR_ALLOC(op_list_ptrs, size,
              "LTM PT Pass Thru op_list ptrs");
    SHR_NULL_CHECK(op_list_ptrs, SHR_E_MEMORY);
    sal_memset(op_list_ptrs, 0, size);

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        pthru_pt_op_list_create(unit, &pt_map->info[pt_view], wb_handle,
                                &op_list_ptrs[pt_view]);
    }

    *op_list_ptrs_ptr = op_list_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_op_list_ptrs_destroy(num_pt_views, op_list_ptrs);
        *op_list_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT dynamic info metadata.
 *
 * Destroy the given PT dynamic info metadata.
 *
 * \param [in] pt_dyn_info Pointer to PT dynamic info to destroy.
 */
static void
pthru_pt_dyn_info_destroy(bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    SHR_FREE(pt_dyn_info);
    return;
}


/*!
 * \brief Create the PT dynamic info metadata.
 *
 * Create the PT op list metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_dyn_info_ptr Returning pointer to PT dynamic info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_dyn_info_create(int unit, bcmbd_pt_dyn_info_t **pt_dyn_info_ptr)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt_dyn_info, sizeof(*pt_dyn_info),
              "LTM PT Pass Thru pt_dyn_info");
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_MEMORY);
    sal_memset(pt_dyn_info, 0, sizeof(*pt_dyn_info));

    /* Fill data */
    pt_dyn_info->index = 0;
    pt_dyn_info->tbl_inst = -1;

    *pt_dyn_info_ptr = pt_dyn_info;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_dyn_info_destroy(pt_dyn_info);
        *pt_dyn_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given Hash PT dynamic info metadata.
 *
 * Destroy the given Hash PT dynamic info metadata.
 *
 * \param [in] pt_dyn_hash_info Pointer to Hash PT dynamic info to destroy.
 */
static void
pthru_pt_dyn_hash_info_destroy(bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info)
{
    SHR_FREE(pt_dyn_hash_info);
    return;
}


/*!
 * \brief Create the Hash PT dynamic info metadata.
 *
 * Create the Hash PT op list metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_dyn_hash_info_ptr Returning pointer to Hash PT dynamic info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_dyn_hash_info_create(int unit,
                              bcmbd_pt_dyn_hash_info_t **pt_dyn_hash_info_ptr)
{
    bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt_dyn_hash_info, sizeof(*pt_dyn_hash_info),
              "LTM PT Pass Thru pt_dyn_hash_info");
    SHR_NULL_CHECK(pt_dyn_hash_info, SHR_E_MEMORY);
    sal_memset(pt_dyn_hash_info, 0, sizeof(*pt_dyn_hash_info));

    /* Fill data */
    pt_dyn_hash_info->tbl_inst = -1;
    pt_dyn_hash_info->bank = 0;

    *pt_dyn_hash_info_ptr = pt_dyn_hash_info;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_dyn_hash_info_destroy(pt_dyn_hash_info);
        *pt_dyn_hash_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT mem args metadata.
 *
 * Destroy the given PT mem args metadata.
 *
 * \param [in] mem_args Pointer to PT mem args metadata to destroy.
 */
static void
pthru_pt_mem_args_destroy(bcmltm_pt_mem_args_t *mem_args)
{
    if (mem_args == NULL) {
        return;
    }

    pthru_pt_dyn_hash_info_destroy(mem_args->pt_dyn_hash_info);
    pthru_pt_dyn_info_destroy(mem_args->pt_dyn_info);
    SHR_FREE(mem_args);

    return;
}


/*!
 * \brief Create the PT mem args metadata.
 *
 * Create the PT mem args metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [out] mem_args_ptr Returning pointer to PT mem args metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_mem_args_create(int unit, bcmdrd_sid_t sid,
                         bcmltm_pt_mem_args_t **mem_args_ptr)
{
    bcmltm_pt_mem_args_t *mem_args = NULL;
    uint32_t flags = 0;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(mem_args, sizeof(*mem_args),
              "LTM PT Pass Thru pt_mem_args");
    SHR_NULL_CHECK(mem_args, SHR_E_MEMORY);
    sal_memset(mem_args, 0, sizeof(*mem_args));

    /* Indicates PTM access is for PT PassThru (rather than LT interactive) */
    flags |= BCMPTM_REQ_FLAGS_PASSTHRU;

    /* Fill data */
    mem_args->flags = flags;
    mem_args->pt = sid;
    mem_args->pt_static_info = NULL;

    SHR_IF_ERR_EXIT
        (pthru_pt_dyn_info_create(unit, &mem_args->pt_dyn_info));

    if (bcmdrd_pt_attr_is_hashed(unit, sid)) {
        SHR_IF_ERR_EXIT
            (pthru_pt_dyn_hash_info_create(unit, &mem_args->pt_dyn_hash_info));
    } else {
        mem_args->pt_dyn_hash_info = NULL;
    }

    *mem_args_ptr = mem_args;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_mem_args_destroy(mem_args);
        *mem_args_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given list of pointers to PT mem args metadata.
 *
 * Destroy the given list of pointers to PT mem args metadata.
 *
 * \param [in] num_pt_views Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT mem args list.
 */
static void
pthru_pt_mem_args_ptrs_destroy(size_t num_pt_views,
                               bcmltm_pt_mem_args_t **mem_args_ptrs)
{
    size_t pt_view;

    if (mem_args_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (mem_args_ptrs[pt_view] == NULL) {
            continue;
        }
        pthru_pt_mem_args_destroy(mem_args_ptrs[pt_view]);
    }

    SHR_FREE(mem_args_ptrs);
    return;
}


/*!
 * \brief Create the list of pointers to PT mem args metadata.
 *
 * Create the list of pointers to PT mem args metadata for the given
 * list of physical table IDs.
 *
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit and list of table IDs are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map Table PT mapping information.
 * \param [out] mem_args_ptrs_ptr Returning pointer to list of PT mem args list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_mem_args_ptrs_create(int unit,
                              const bcmltm_map_pt_t *pt_map,
                              bcmltm_pt_mem_args_t ***mem_args_ptrs_ptr)
{
    unsigned int size;
    size_t pt_view;
    size_t num_pt_views = pt_map->num_pt;
    bcmltm_pt_mem_args_t **mem_args_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    size = sizeof(bcmltm_pt_mem_args_t *) * num_pt_views;
    SHR_ALLOC(mem_args_ptrs, size,
              "LTM PT Pass Thru mem_args ptrs");
    SHR_NULL_CHECK(mem_args_ptrs, SHR_E_MEMORY);
    sal_memset(mem_args_ptrs, 0, size);

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        pthru_pt_mem_args_create(unit, pt_map->info[pt_view].sid,
                                 &mem_args_ptrs[pt_view]);
    }

    *mem_args_ptrs_ptr = mem_args_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_mem_args_ptrs_destroy(num_pt_views, mem_args_ptrs);
        *mem_args_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT list metadata.
 *
 * Destroy the given PT list metadata.
 *
 * \param [in] pt_list List of PT metadata.
 */
static void
pthru_pt_list_destroy(bcmltm_pt_list_t *pt_list)
{
    if (pt_list == NULL) {
        return;
    }

    pthru_pt_op_list_ptrs_destroy(pt_list->num_pt_view, pt_list->memop_lists);
    pthru_pt_mem_args_ptrs_destroy(pt_list->num_pt_view, pt_list->mem_args);

    SHR_FREE(pt_list);

    return;
}


/*!
 * \brief Create the PT list metadata.
 *
 * Create the PT list metadata for the given list of physical table IDs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] pt_list_ptr Returning pointer PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_list_create(int unit, const bcmltm_map_pt_t *pt_map,
                     const bcmltm_wb_handle_t *wb_handle,
                     bcmltm_pt_list_t **pt_list_ptr)
{
    bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt_list, sizeof(*pt_list),
              "LTM PT Pass Thru pt_list");
    SHR_NULL_CHECK(pt_list, SHR_E_MEMORY);
    sal_memset(pt_list, 0, sizeof(*pt_list));

    /* Fill data */
    pt_list->num_pt_view = pt_map->num_pt;
    SHR_IF_ERR_EXIT
        (pthru_pt_mem_args_ptrs_create(unit, pt_map,
                                       &pt_list->mem_args));
    SHR_IF_ERR_EXIT
        (pthru_pt_op_list_ptrs_create(unit, pt_map, wb_handle,
                                      &pt_list->memop_lists));

    *pt_list_ptr = pt_list;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_pt_list_destroy(pt_list);
        *pt_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve the PT list metadata for given table.
 *
 * Retrieve the PT list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] pt_list_ptr Returning pointer to PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_pt_list_retrieve(int unit, bcmdrd_sid_t sid,
                       const bcmltm_pt_list_t **pt_list_ptr)
{
    SHR_FUNC_ENTER(unit);

    if (!PTHRU_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (!PTHRU_SID_VALID(unit, sid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (PTHRU_PT_LIST(unit) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    *pt_list_ptr = PTHRU_PT_LIST_SID(unit, sid);

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy EE node data.
 *
 * Destroy EE node data.
 *
 * \param [in] ee_node EE node data to destroy.
 */
static void
pthru_ee_node_data_destroy(bcmltm_ee_node_t *ee_node)
{
    if (ee_node == NULL) {
        return;
    }

    SHR_FREE(ee_node);

    return;
}


/*!
 * \brief Create EE node data for set operation.
 *
 * Create EE node data for set operation.  This is used in the
 * as the node_data for the Execution Engine tree.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ee_node_ptr Returning pointer to EE node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ee_node_data_set_create(int unit,
                              bcmdrd_sid_t sid,
                              bcmltm_ee_node_t **ee_node_ptr)
{
    bcmltm_ee_node_t *ee_node = NULL;
    const bcmltm_pt_list_t *pt_list_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(ee_node, sizeof(*ee_node), "LTM PT Pass Thru ee_node write");
    SHR_NULL_CHECK(ee_node, SHR_E_MEMORY);
    sal_memset(ee_node, 0, sizeof(*ee_node));

    /* Fill data */
    ee_node->node_function = bcmltm_ee_node_schan_write;
    SHR_IF_ERR_EXIT
        (pthru_pt_list_retrieve(unit, sid, &pt_list_ptr));

    ee_node->node_cookie = (void *)pt_list_ptr;

    *ee_node_ptr = ee_node;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_ee_node_data_destroy(ee_node);
        *ee_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create EE node data for get operation.
 *
 * Create EE node data for get operation.  This is used in the
 * as the node_data for the Execution Engine tree.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ee_node_ptr Returning pointer to EE node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ee_node_data_get_create(int unit,
                              bcmdrd_sid_t sid,
                              bcmltm_ee_node_t **ee_node_ptr)
{
    bcmltm_ee_node_t *ee_node = NULL;
    const bcmltm_pt_list_t *pt_list_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(ee_node, sizeof(*ee_node), "LTM PT Pass Thru ee_node read");
    SHR_NULL_CHECK(ee_node, SHR_E_MEMORY);
    sal_memset(ee_node, 0, sizeof(*ee_node));

    /* Fill data */
    ee_node->node_function = bcmltm_ee_node_schan_read;
    SHR_IF_ERR_EXIT
        (pthru_pt_list_retrieve(unit, sid, &pt_list_ptr));

    ee_node->node_cookie = (void *)pt_list_ptr;

    *ee_node_ptr = ee_node;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_ee_node_data_destroy(ee_node);
        *ee_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create EE node data for lookup operation.
 *
 * Create EE node data for lookup operation.  This is used in the
 * as the node_data for the Execution Engine tree.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ee_node_ptr Returning pointer to EE node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ee_node_data_lookup_create(int unit,
                                 bcmdrd_sid_t sid,
                                 bcmltm_ee_node_t **ee_node_ptr)
{
    bcmltm_ee_node_t *ee_node = NULL;
    const bcmltm_pt_list_t *pt_list_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(ee_node, sizeof(*ee_node), "LTM PT Pass Thru ee_node lookup");
    SHR_NULL_CHECK(ee_node, SHR_E_MEMORY);
    sal_memset(ee_node, 0, sizeof(*ee_node));

    /* Fill data */
    ee_node->node_function = bcmltm_ee_node_pt_hash_lookup;
    SHR_IF_ERR_EXIT
        (pthru_pt_list_retrieve(unit, sid, &pt_list_ptr));

    ee_node->node_cookie = (void *)pt_list_ptr;

    *ee_node_ptr = ee_node;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_ee_node_data_destroy(ee_node);
        *ee_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy all the EE node data for the given tree.
 *
 * Destroy all the EE node data for the given tree.
 *
 * The function traverses the binary tree, and destroys the node_data for
 * each node in the tree.  This uses post-order recursive traversal of the tree.
 *
 * \param [in] node EE tree to destroy.
 */
static void
pthru_ee_tree_node_data_destroy(bcmltm_node_t *node)
{
    if (node == NULL) {
        return;
    }

    if (node->left != NULL) {
        pthru_ee_tree_node_data_destroy(node->left);
    }

    if (node->right != NULL) {
        pthru_ee_tree_node_data_destroy(node->right);
    }

    if (node->node_data != NULL) {
        pthru_ee_node_data_destroy(node->node_data);
        node->node_data = NULL;
    }

    return;
}


/*!
 * \brief Destroy the list of given EE tree roots.
 *
 * Destroy the list of given EE tree roots.
 *
 * This function destroys the node_data stored in each tree along
 * with the tree itself.
 *
 * \param [in] num_roots Number of tree roots.
 * \param [in] ee_node_roots List of tree roots to destroy.
 */
static void
pthru_ee_node_roots_destroy(size_t num_roots,
                            bcmltm_node_t **ee_node_roots)
{
    size_t root;

    if (ee_node_roots == NULL) {
        return;
    }

    for (root = 0; root < num_roots; root++) {
        if (ee_node_roots[root] == NULL) {
            continue;
        }

        /*
         * Destroy node data.
         * The function bcmltm_node_destroy() does not destroy
         * the node_data so this needs to be explicitly done.
         */
        pthru_ee_tree_node_data_destroy(ee_node_roots[root]);

        /* Destroy tree */
        bcmltm_tree_destroy(ee_node_roots[root]);
    }

    PTHRU_NODE_PTRS_FREE(ee_node_roots);

    return;
}


/*!
 * \brief Create the EE tree roots for a set operation.
 *
 * Create the EE tree roots for a set operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ee_node_roots_set_create(int unit,
                               bcmdrd_sid_t sid,
                               bcmltm_node_t ***ee_node_roots_ptr)
{
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    ee_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_SET_NUM_ROOTS);
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);

    /* A 'set' operation only needs one root (tree) */

    /*
     * Create first tree
     */
    /* Create node data */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_data_set_create(unit, sid, &node_data));

    /* Create tree and install node */
    ee_node_roots[0] = bcmltm_tree_node_create((void *)node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((ee_node_roots[0] == NULL) && (node_data != NULL)) {
            pthru_ee_node_data_destroy(node_data);
        }

        pthru_ee_node_roots_destroy(PTHRU_OP_SET_NUM_ROOTS, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create the EE tree roots for a MODIFY operation.
 *
 * Create the EE tree roots for a MODIFY operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ee_node_roots_modify_create(int unit,
                                  bcmdrd_sid_t sid,
                                  bcmltm_node_t ***ee_node_roots_ptr)
{
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_t *write_node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    ee_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_MODIFY_NUM_ROOTS);
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);

    /*
     * MODIFY
     *
     * EE Trees Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (PTM read operation)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (PTM write operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute PTM read operation.
     */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_data_get_create(unit, sid, &read_node_data));
    ee_node_roots[0] = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Execute PTM write operation.
     */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_data_set_create(unit, sid, &write_node_data));
    ee_node_roots[1] = bcmltm_tree_node_create((void *)write_node_data);
    SHR_NULL_CHECK(ee_node_roots[1], SHR_E_MEMORY);

    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((ee_node_roots[0] == NULL) && (read_node_data != NULL)) {
            pthru_ee_node_data_destroy(read_node_data);
        }
        if ((ee_node_roots[1] == NULL) && (write_node_data != NULL)) {
            pthru_ee_node_data_destroy(write_node_data);
        }

        pthru_ee_node_roots_destroy(PTHRU_OP_MODIFY_NUM_ROOTS, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create the EE tree roots for a get operation.
 *
 * Create the EE tree roots for a get operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ee_node_roots_get_create(int unit,
                               bcmdrd_sid_t sid,
                               bcmltm_node_t ***ee_node_roots_ptr)
{
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    ee_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_GET_NUM_ROOTS);
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);

    /* A 'get' operation only needs one root, so 2nd root is null */

    /*
     * Create first tree
     */
    /* Create node data */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_data_get_create(unit, sid, &node_data));

    /* Create tree and install node */
    ee_node_roots[0] = bcmltm_tree_node_create((void *)node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

    /*
     * Create second tree
     */
    ee_node_roots[1] = NULL;  /* Not used */

    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((ee_node_roots[0] == NULL) && (node_data != NULL)) {
            pthru_ee_node_data_destroy(node_data);
        }

        pthru_ee_node_roots_destroy(PTHRU_OP_GET_NUM_ROOTS, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create the EE tree roots for a LOOKUP operation.
 *
 * Create the EE tree roots for a LOOKUP operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ee_node_roots_lookup_create(int unit,
                                  bcmdrd_sid_t sid,
                                  bcmltm_node_t ***ee_node_roots_ptr)
{
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    ee_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_LOOKUP_NUM_ROOTS);
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);

    /* A LOOKUP operation only needs one root, so 2nd root is null */

    /*
     * Create first tree
     */
    /* Create node data */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_data_lookup_create(unit, sid, &node_data));

    /* Create tree and install node */
    ee_node_roots[0] = bcmltm_tree_node_create((void *)node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

    /*
     * Create second tree
     */
    ee_node_roots[1] = NULL;  /* Not used */

    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((ee_node_roots[0] == NULL) && (node_data != NULL)) {
            pthru_ee_node_data_destroy(node_data);
        }

        pthru_ee_node_roots_destroy(PTHRU_OP_LOOKUP_NUM_ROOTS, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Get the minimum and maximum bit positions for given field.
 *
 * Return the minimum and maximum bit positions for given field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 * \param [out] minbit Returning minimum bit position.
 * \param [out] maxbit Returning maximum bit position.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_field_min_max_bit_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                            uint16_t *minbit, uint16_t *maxbit)
{
    *minbit = 0;
    *maxbit = 0;

    if (fid == PTHRU_FIELD_KEY_INDEX) {
        *minbit = PTHRU_FIELD_KEY_INDEX_MINBIT;
        *maxbit = PTHRU_FIELD_KEY_INDEX_MAXBIT;
    } else if (fid == PTHRU_FIELD_KEY_PORT) {
        *minbit = PTHRU_FIELD_KEY_PORT_MINBIT;
        *maxbit = PTHRU_FIELD_KEY_PORT_MAXBIT;
    } else if (fid == PTHRU_FIELD_KEY_INSTANCE) {
        *minbit = PTHRU_FIELD_KEY_INSTANCE_MINBIT;
        *maxbit = PTHRU_FIELD_KEY_INSTANCE_MAXBIT;
    } else {
        int min, max;

        min = bcmdrd_pt_field_minbit(unit, sid, fid);
        max = bcmdrd_pt_field_maxbit(unit, sid, fid);

        if ((min < 0) || (max < 0)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid field bit positions: "
                                  "ptid=%d fid=%d minbit=%d maxbit=%d\n"),
                       sid, fid, *minbit, *maxbit));
            return SHR_E_INTERNAL;
        }

        *minbit = min;
        *maxbit = max;
    }

    return SHR_E_NONE;
}


/*!
 * \brief Get the total number of field elements for given field.
 *
 * This routine returns the number of field index elements required
 * to fit the width of the given field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 *
 * \retval Number of field indexes (> 0), if successful.
 *         Otherwise, 0 on failure.
 */
static uint32_t
pthru_field_idx_count_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    uint32_t width;
    uint16_t minbit;
    uint16_t maxbit;
    uint32_t idx_count;

    rv = pthru_field_min_max_bit_get(unit, sid, fid, &minbit, &maxbit);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    width = maxbit - minbit + 1;
    idx_count = bcmltd_field_idx_count_get(width);

    return idx_count;
}


/*!
 * \brief Get the list of key fields IDs for given table ID.
 *
 * Get the list of key fields IDs for given table ID.
 *
 * The function will always return the total number of field IDs
 * in num_fid, irrespective of the value of list_max, i.e. if
 * num_sid is greater than list_max, then the returned fid_list was
 * truncated.
 *
 * If list_max is zero, then the number of field IDs is returned
 * in num_fid, but the fid_list is not updated (can be
 * specified as NULL). The returned num_fid can then be used to
 * allocate a sufficiently large sid_list array.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] list_max Size of allocated entries in list.
 * \param [out] fid_list Returning list of field IDs.
 * \param [out] num_fid Total number of field IDs returned.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pthru_pt_fid_list_key_get(int unit,
                          bcmdrd_sid_t sid, size_t list_max,
                          bcmdrd_fid_t *fid_list, size_t *num_fid)
{
    size_t idx = 0;

    /* Check register/memory array */
    if (bcmdrd_pt_index_max(unit, sid) > 0) {
        if (idx < list_max) {
            fid_list[idx] = PTHRU_FIELD_KEY_INDEX;
        }
        idx++;
    }

    /* Check port-based register */
    if (bcmdrd_pt_is_port_reg(unit, sid) ||
        bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        if (idx < list_max) {
            fid_list[idx] = PTHRU_FIELD_KEY_PORT;
        }
        idx++;
    } else {
        /* Check table instance */
        /*
         * The DRD function that gets the number of table instances
         * returns the number of ports for port-based symbols.
         *
         * Also, symbols without UNIQUE access has instance of 1.
         *
         * As such, add virtual field INSTANCE only on non-port-based symbols
         * with table instances more than 1.
         */
        int tbl_inst;
        tbl_inst = bcmdrd_pt_num_tbl_inst(unit, sid);

        if (tbl_inst > 1) {
            if (idx < list_max) {
                fid_list[idx] = PTHRU_FIELD_KEY_INSTANCE;
            }
            idx++;
        }
    }

    *num_fid = idx;

    return SHR_E_NONE;
}


/*!
 * \brief Get the key field information.
 *
 * Get the key field information for the given table and field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 * \param [out] finfo Field information to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_key_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                         bcmdrd_sym_field_info_t *finfo)
{
    bcmltm_ptm_field_list_t *key_list;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(finfo, 0, sizeof(*finfo));

    key_list = PTHRU_PTM_MAPPING_KEYS_SID(unit, sid)->field_list;

    /* Check that key exist for given table */
    for (idx = 0; idx < key_list->num_fields; idx++) {
        if (fid == key_list->field_maps[idx].api_field_id) {
            break;
        }
    }
    if (idx >= key_list->num_fields) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    finfo->fid = fid;
    finfo->view = 0;
    finfo->minbit = key_list->field_maps[idx].wbc.minbit;
    finfo->maxbit = key_list->field_maps[idx].wbc.maxbit;
    finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_KEY;

    if (fid == PTHRU_FIELD_KEY_INDEX) {
        finfo->name = PTHRU_FIELD_KEY_INDEX_NAME;
    } else if (fid == PTHRU_FIELD_KEY_PORT) {
        finfo->name = PTHRU_FIELD_KEY_PORT_NAME;
    } else if (fid == PTHRU_FIELD_KEY_INSTANCE) {
        finfo->name = PTHRU_FIELD_KEY_INSTANCE_NAME;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the field map metadata for given field ID.
 *
 * Get the field map(s) information for the given field ID.
 * If the field is wide, i.e. width is larger than what a field
 * element can fit (64 bits), then the necessary number of field
 * maps will be filled out.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] wb_block Working buffer block.
 * \param [in] fid Field ID.
 * \param [in] field_map_size Array count of field map.
 * \param [out] field_map Start of field map array to fill.
 * \param [out] num_field_maps Number of field maps returned.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pthru_ptm_field_map_get(int unit, bcmdrd_sid_t sid,
                        const bcmltm_wb_block_t *wb_block,
                        bcmdrd_fid_t fid,
                        uint32_t field_map_size,
                        bcmltm_ptm_field_map_t *field_map,
                        uint32_t *num_field_maps)
{
    uint32_t wbc_offset;
    uint16_t minbit;
    uint16_t maxbit;
    uint32_t idx;
    uint32_t idx_count;
    uint16_t idx_minbit[MAX_FIELD_IDXS];
    uint16_t idx_maxbit[MAX_FIELD_IDXS];

    SHR_FUNC_ENTER(unit);

    *num_field_maps = 0;

    /* Get working buffer offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_pthru_offset_get(wb_block,
                                              fid,
                                              &wbc_offset));

    /*
     * Get corresponding minimum and maximum bit positions for
     * each field element.
     */
    SHR_IF_ERR_EXIT
        (pthru_field_min_max_bit_get(unit, sid, fid, &minbit, &maxbit));

    SHR_IF_ERR_EXIT
        (bcmltm_nc_field_to_fidx_min_max(unit, fid,
                                         minbit, maxbit,
                                         MAX_FIELD_IDXS,
                                         idx_minbit, idx_maxbit,
                                         &idx_count));
    /* Sanity check */
    if (idx_count > field_map_size) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM PT Pass Thru field map array size "
                              "not large enough: "
                              "sid=%d fid=%d size=%d need=%d\n"),
                   sid, fid, field_map_size, idx_count));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < idx_count; idx++) {
        field_map[idx].api_field_id = fid;
        field_map[idx].field_idx = idx;
        field_map[idx].wbc.buffer_offset = wbc_offset;
        field_map[idx].wbc.minbit = idx_minbit[idx];
        field_map[idx].wbc.maxbit = idx_maxbit[idx];
        field_map[idx].pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
    }

    *num_field_maps = idx_count;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the list of field maps metadata.
 *
 * Destroy the list of field maps metadata.
 *
 * \param [in] field_maps Field maps metadata to destroy.
 */
static void
pthru_ptm_field_maps_destroy(bcmltm_ptm_field_map_t *field_maps)
{
    SHR_FREE(field_maps);
    return;
}


/*!
 * \brief Create the list of field maps metadata.
 *
 * Create the list of field maps metadata.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [in] field_type Indicates key or value field.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ptm_field_maps_create(int unit,
                            bcmdrd_sid_t sid,
                            const bcmltm_map_pt_t *pt_map,
                            const bcmltm_wb_handle_t *wb_handle,
                            int field_type,
                            bcmltm_ptm_field_map_t **field_maps_ptr,
                            size_t *num_fields)
{
    bcmltm_ptm_field_map_t *field_maps = NULL;
    bcmdrd_fid_t *fid_list = NULL;
    unsigned int size;
    size_t fid_max_count;
    size_t num_fid;
    size_t total_field_maps = 0;
    size_t i;
    uint32_t pt_view = 0; /* PT Pass Thru only has 1 view */
    const bcmltm_wb_block_t *wb_block = NULL;
    uint32_t num_field_maps = 0;
    uint32_t field_maps_count = 0;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of fields in symbol */
    if (field_type == PTHRU_FIELD_TYPE_KEY) {
        /* Keys */
        SHR_IF_ERR_EXIT
            (pthru_pt_fid_list_key_get(unit, sid, 0, NULL, &fid_max_count));
    } else {
        /* Values */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &fid_max_count));
    }

    if (fid_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate fid list array */
    size = sizeof(*fid_list) * fid_max_count;
    SHR_ALLOC(fid_list, size,
              "LTM PT Pass Thru fid_list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, size);

    /* Get list of Field IDs */
    if (field_type == PTHRU_FIELD_TYPE_KEY) {
        /* Keys */
        SHR_IF_ERR_EXIT
            (pthru_pt_fid_list_key_get(unit, sid, fid_max_count,
                                       fid_list, &num_fid));
    } else {
        /* Values */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, sid, fid_max_count,
                                    fid_list, &num_fid));
    }

    if (fid_max_count != num_fid) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM Number of fields mismatch: "
                              "sid=%d expected=%d actual=%d\n"),
                   sid, (int) fid_max_count, (int) num_fid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Get total field maps required (unique fid,idx) */
    for (i = 0; i < fid_max_count; i++) {
        total_field_maps += pthru_field_idx_count_get(unit, sid, fid_list[i]);
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * total_field_maps;
    SHR_ALLOC(field_maps, size,
              "LTM PT Pass Thru ptm_field_map");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);


    /* PT Pass Thru only has 1 pt view */
    wb_block = bcmltm_wb_block_find(wb_handle,
                                    pt_map->info[pt_view].wb_block_id);
    if (wb_block == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not find working buffer block: "
                              "sid=%d wb_block_id=%d\n"),
                   sid, pt_map->info[pt_view].wb_block_id));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < fid_max_count; i++) {
        SHR_IF_ERR_EXIT
            (pthru_ptm_field_map_get(unit, sid, wb_block, fid_list[i],
                                     total_field_maps - num_field_maps,
                                     &field_maps[num_field_maps],
                                     &field_maps_count));
        num_field_maps += field_maps_count;
    }

    *num_fields = num_field_maps;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_ptm_field_maps_destroy(field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the field list metadata.
 *
 * Destroy the field list metadata.
 *
 * \param [in] field_list Field list metadata to destroy.
 */
static void
pthru_ptm_field_list_destroy(bcmltm_ptm_field_list_t *field_list)
{
    if (field_list == NULL) {
        return;
    }

    pthru_ptm_field_maps_destroy(field_list->field_maps);

    SHR_FREE(field_list);

    return;
}


/*!
 * \brief Create the field list metadata.
 *
 * Create the field list metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [in] field_type Indicates key or value field.
 * \param [out] field_list_ptr Returning pointer to field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ptm_field_list_create(int unit,
                            bcmdrd_sid_t sid,
                            const bcmltm_map_pt_t *pt_map,
                            const bcmltm_wb_handle_t *wb_handle,
                            int field_type,
                            bcmltm_ptm_field_list_t **field_list_ptr)
{
    bcmltm_ptm_field_list_t *field_list = NULL;
    size_t num_fields;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list),
              "LTM PT Pass Thru ptm_field_list");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));


    /* Get field map */
    SHR_IF_ERR_EXIT
        (pthru_ptm_field_maps_create(unit, sid, pt_map, wb_handle, field_type,
                                     &field_list->field_maps,
                                     &num_fields));
    field_list->num_fields = num_fields;

    /* Not applicable */
    field_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_ptm_field_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the field mapping metadata.
 *
 * Destroy the field mapping metadata.
 *
 * \param [in] ptm_mapping Field mapping metadata to destroy.
 */
static void
pthru_ptm_mapping_destroy(bcmltm_to_ptm_mapping_t *ptm_mapping)
{
    if (ptm_mapping == NULL) {
        return;
    }

    pthru_ptm_field_list_destroy(ptm_mapping->field_list);

    SHR_FREE(ptm_mapping);

    return;
}


/*!
 * \brief Create the field mapping metadata.
 *
 * Create the field mapping metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] wb_handle Working buffer handle.
 * \param [in] field_type Indicates key or value field.
 * \param [out] ptm_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ptm_mapping_create(int unit,
                         bcmdrd_sid_t sid,
                         const bcmltm_map_pt_t *pt_map,
                         const bcmltm_wb_handle_t *wb_handle,
                         int field_type,
                         bcmltm_to_ptm_mapping_t **ptm_mapping_ptr)
{
    bcmltm_to_ptm_mapping_t *ptm_mapping = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(ptm_mapping, sizeof(*ptm_mapping),
              "LTM PT Pass Thru ptm_mapping");
    SHR_NULL_CHECK(ptm_mapping, SHR_E_MEMORY);
    sal_memset(ptm_mapping, 0, sizeof(*ptm_mapping));

    /* Fill data */
    SHR_IF_ERR_EXIT
        (pthru_ptm_field_list_create(unit, sid, pt_map, wb_handle, field_type,
                                     &ptm_mapping->field_list));

    *ptm_mapping_ptr = ptm_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_ptm_mapping_destroy(ptm_mapping);
        *ptm_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve the field mapping metadata.
 *
 * Retrieve the field mapping metadata for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Indicates key or value field.
 * \param [out] ptm_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ptm_mapping_retrieve(int unit,
                           bcmdrd_sid_t sid,
                           int field_type,
                           const bcmltm_to_ptm_mapping_t **ptm_mapping_ptr)
{
    SHR_FUNC_ENTER(unit);

    if (!PTHRU_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (!PTHRU_SID_VALID(unit, sid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (field_type == PTHRU_FIELD_TYPE_KEY) {
        if (PTHRU_PTM_MAPPING_KEYS(unit) == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        *ptm_mapping_ptr = PTHRU_PTM_MAPPING_KEYS_SID(unit, sid);
    } else {
        if (PTHRU_PTM_MAPPING_VALUES(unit) == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        *ptm_mapping_ptr = PTHRU_PTM_MAPPING_VALUES_SID(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy FA node data.
 *
 * Destroy FA node data.
 *
 * \param [in] fa_node FA node data to destroy.
 */
static void
pthru_fa_node_data_destroy(bcmltm_fa_node_t *fa_node)
{
    if (fa_node == NULL) {
        return;
    }

    SHR_FREE(fa_node);

    return;
}


/*!
 * \brief Create FA node data for set operation.
 *
 * Create FA node data for set operation.  This is used in the
 * as the node_data for the Field Adaptation tree.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fa_node_ptr Returning pointer to FA node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_fa_node_data_set_create(int unit,
                              bcmdrd_sid_t sid,
                              int field_type,
                              bcmltm_fa_node_t **fa_node_ptr)
{
    bcmltm_fa_node_t *fa_node = NULL;
    const bcmltm_to_ptm_mapping_t *ptm_map = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(fa_node, sizeof(*fa_node), "LTM PT Pass Thru fa_node set");
    SHR_NULL_CHECK(fa_node, SHR_E_MEMORY);
    sal_memset(fa_node, 0, sizeof(*fa_node));

    /* Fill data */
    fa_node->node_function = bcmltm_fa_node_api_value_fields_to_wb;

    SHR_IF_ERR_EXIT
        (pthru_ptm_mapping_retrieve(unit, sid, field_type, &ptm_map));

    fa_node->node_cookie = (void *)ptm_map;

    *fa_node_ptr = fa_node;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_fa_node_data_destroy(fa_node);
        *fa_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create FA node data for get operation.
 *
 * Create FA node data for get operation.  This is used in the
 * as the node_data for the Field Adaptation tree.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fa_node_ptr Returning pointer to FA node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_fa_node_data_get_create(int unit,
                              bcmdrd_sid_t sid,
                              int field_type,
                              bcmltm_fa_node_t **fa_node_ptr)
{
    bcmltm_fa_node_t *fa_node = NULL;
    const bcmltm_to_ptm_mapping_t *ptm_map = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(fa_node, sizeof(*fa_node), "LTM PT Pass Thru fa_node get");
    SHR_NULL_CHECK(fa_node, SHR_E_MEMORY);
    sal_memset(fa_node, 0, sizeof(*fa_node));

    /* Fill data */
    if (field_type == PTHRU_FIELD_TYPE_KEY) {
        fa_node->node_function = bcmltm_fa_node_api_value_fields_to_wb;
    } else {
        fa_node->node_function = bcmltm_fa_node_wb_to_api_fields;
    }

    SHR_IF_ERR_EXIT
        (pthru_ptm_mapping_retrieve(unit, sid, field_type, &ptm_map));

    fa_node->node_cookie = (void *)ptm_map;

    *fa_node_ptr = fa_node;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_fa_node_data_destroy(fa_node);
        *fa_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create FA node data for lookup operation.
 *
 * Create FA node data for lookup operation.  This is used in the
 * as the node_data for the Field Adaptation tree.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fa_node_ptr Returning pointer to FA node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_fa_node_data_lookup_create(int unit,
                                 bcmdrd_sid_t sid,
                                 int field_type,
                                 bcmltm_fa_node_t **fa_node_ptr)
{
    bcmltm_fa_node_t *fa_node = NULL;
    const bcmltm_to_ptm_mapping_t *ptm_map = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(fa_node, sizeof(*fa_node), "LTM PT Pass Thru fa_node lookup");
    SHR_NULL_CHECK(fa_node, SHR_E_MEMORY);
    sal_memset(fa_node, 0, sizeof(*fa_node));

    /* Fill data */
    if (field_type == PTHRU_FIELD_TYPE_KEY) {
        fa_node->node_function = bcmltm_fa_node_api_value_fields_to_wb;
    } else {
        fa_node->node_function = bcmltm_fa_node_wb_to_api_fields;
    }

    /* Since it is unknown whether a field is a Hash Key or not,
     * the node cookie used for KEYS lookup need to contain
     * all the PT fields which includes PT Hash Key and Data fields.
     *
     * The function pthru_ptm_mapping_retrieve() with PTHRU_FIELD_TYPE_VALUE
     * returns all the PT fields.
     */
    SHR_IF_ERR_EXIT
        (pthru_ptm_mapping_retrieve(unit, sid, PTHRU_FIELD_TYPE_VALUE,
                                    &ptm_map));

    fa_node->node_cookie = (void *)ptm_map;

    *fa_node_ptr = fa_node;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_fa_node_data_destroy(fa_node);
        *fa_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy all the FA node data for the given tree.
 *
 * Destroy all the FA node data for the given tree.
 *
 * The function traverses the binary tree, and destroys the node_data for
 * each node in the tree.  This uses post-order recursive traversal of the tree.
 *
 * \param [in] node FA tree to destroy.
 */
static void
pthru_fa_tree_node_data_destroy(bcmltm_node_t *node)
{
    if (node == NULL) {
        return;
    }

    if (node->left != NULL) {
        pthru_fa_tree_node_data_destroy(node->left);
    }

    if (node->right != NULL) {
        pthru_fa_tree_node_data_destroy(node->right);
    }

    if (node->node_data != NULL) {
        pthru_fa_node_data_destroy(node->node_data);
        node->node_data = NULL;
    }

    return;
}


/*!
 * \brief Destroy the list of given FA tree roots.
 *
 * Destroy the list of given FA tree roots.
 *
 * This function destroys the node_data stored in each tree along
 * with the tree itself.
 *
 * \param [in] num_roots Number of tree roots.
 * \param [in] fa_node_roots List of tree roots to destroy.
 */
static void
pthru_fa_node_roots_destroy(size_t num_roots,
                            bcmltm_node_t **fa_node_roots)
{
    size_t root;

    if (fa_node_roots == NULL) {
        return;
    }

    for (root = 0; root < num_roots; root++) {
        if (fa_node_roots[root] == NULL) {
            continue;
        }

        /*
         * Destroy node data.
         * The function bcmltm_node_destroy() does not destroy
         * the node_data so this needs to be explicitly done.
         */
        pthru_fa_tree_node_data_destroy(fa_node_roots[root]);

        /* Destroy tree */
        bcmltm_tree_destroy(fa_node_roots[root]);
    }

    PTHRU_NODE_PTRS_FREE(fa_node_roots);

    return;
}


/*!
 * \brief Create the FA tree roots for a set operation.
 *
 * Create the FA tree roots for a set operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_fa_node_roots_set_create(int unit,
                               bcmdrd_sid_t sid,
                               bcmltm_node_t ***fa_node_roots_ptr)
{
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *new_node = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    fa_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_SET_NUM_ROOTS);
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);

    /*
     * A 'set' operation only needs one root (tree).
     * The tree has 2 nodes, one for keys and another for values.
     */

    /*
     * Create first tree
     */
    /* Create values node data */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_set_create(unit, sid, PTHRU_FIELD_TYPE_VALUE,
                                       &values_node_data));
    /* Create tree and install values node */
    fa_node_roots[0] = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    /* Create keys node data */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_set_create(unit, sid, PTHRU_FIELD_TYPE_KEY,
                                       &keys_node_data));
    /* Insert keys node to tree */
    new_node = bcmltm_tree_left_node_create(fa_node_roots[0],
                                            (void *)keys_node_data);
    SHR_NULL_CHECK(new_node, SHR_E_MEMORY);

    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (fa_node_roots[0] == NULL) {
            if (values_node_data != NULL) {
                pthru_fa_node_data_destroy(values_node_data);
            }
        }
        if (new_node == NULL) {
            if (keys_node_data != NULL) {
                pthru_fa_node_data_destroy(keys_node_data);
            }
        }

        pthru_fa_node_roots_destroy(PTHRU_OP_SET_NUM_ROOTS, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a MODIFY operation.
 *
 * Create the FA tree roots for a MODIFY operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_fa_node_roots_modify_create(int unit,
                                  bcmdrd_sid_t sid,
                                  bcmltm_node_t ***fa_node_roots_ptr)
{
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    fa_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_MODIFY_NUM_ROOTS);
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);

    /*
     * MODIFY
     *
     * FA Trees Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert API Keys)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (Convert API Values)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_set_create(unit, sid, PTHRU_FIELD_TYPE_KEY,
                                       &keys_node_data));
    fa_node_roots[0] = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Convert API Value fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_set_create(unit, sid, PTHRU_FIELD_TYPE_VALUE,
                                       &values_node_data));
    fa_node_roots[1] = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(fa_node_roots[1], SHR_E_MEMORY);

    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((fa_node_roots[0] == NULL) && (keys_node_data != NULL)) {
            pthru_fa_node_data_destroy(keys_node_data);
        }
        if ((fa_node_roots[1] == NULL) && (values_node_data != NULL)) {
            pthru_fa_node_data_destroy(values_node_data);
        }

        pthru_fa_node_roots_destroy(PTHRU_OP_MODIFY_NUM_ROOTS, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a get operation.
 *
 * Create the FA tree roots for a get operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_fa_node_roots_get_create(int unit,
                               bcmdrd_sid_t sid,
                               bcmltm_node_t ***fa_node_roots_ptr)
{
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    fa_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_GET_NUM_ROOTS);
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);

    /*
     * A 'get' operation has 2 roots (trees).
     *
     * The first FA tree is used to adapt the API Key fields into
     * the working buffer to be used to set the HW entry data.
     * Note that this is 'optional' if the symbol does not have a key field.
     *
     * The second tree is used to transport the information read
     * from the EE stage into the returning list of value fields.
     */

    /*
     * Create first tree
     */
    /* Create keys node data */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_get_create(unit, sid, PTHRU_FIELD_TYPE_KEY,
                                       &keys_node_data));
    /* Create tree and install keys node */
    fa_node_roots[0] = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    /*
     * Create second tree
     */
    /* Create values node data */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_get_create(unit, sid, PTHRU_FIELD_TYPE_VALUE,
                                       &values_node_data));
    /* Create tree */
    fa_node_roots[1] = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(fa_node_roots[1], SHR_E_MEMORY);

    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (fa_node_roots[0] == NULL) {
            if (keys_node_data != NULL) {
                pthru_fa_node_data_destroy(keys_node_data);
            }
        }
        if (fa_node_roots[1] == NULL) {
            if (values_node_data != NULL) {
                pthru_fa_node_data_destroy(values_node_data);
            }
        }

        pthru_fa_node_roots_destroy(PTHRU_OP_GET_NUM_ROOTS, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a lookup operation.
 *
 * Create the FA tree roots for a lookup operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_fa_node_roots_lookup_create(int unit,
                                  bcmdrd_sid_t sid,
                                  bcmltm_node_t ***fa_node_roots_ptr)
{
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    fa_node_roots = PTHRU_NODE_PTRS_ALLOC(unit,
                                          PTHRU_OP_LOOKUP_NUM_ROOTS);
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);

    /*
     * A 'lookup' operation has 2 roots (trees).
     *
     * The first FA tree is used to adapt the API fields into
     * the working buffer to be used to set the HW entry data.
     *
     * Since it is unknown whether a field is a Hash Key or not,
     * this first FA tree will contain all the PT fields which includes
     * the PT Hash Key and Data fields.  It is expected
     * that the caller will pass the right fields (i.e. Keys) for
     * this operation to succeed.
     *
     * The second tree is used to transport the information read
     * from the EE stage into the returning list of value fields.
     */

    /*
     * Create first tree
     */
    /* Create key node data with all PT fields */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_lookup_create(unit, sid, PTHRU_FIELD_TYPE_KEY,
                                          &keys_node_data));
    /* Create tree and install keys node */
    fa_node_roots[0] = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    /*
     * Create second tree
     */
    /* Create values node data */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_data_lookup_create(unit, sid, PTHRU_FIELD_TYPE_VALUE,
                                          &values_node_data));
    /* Create tree */
    fa_node_roots[1] = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(fa_node_roots[1], SHR_E_MEMORY);

    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (fa_node_roots[0] == NULL) {
            if (keys_node_data != NULL) {
                pthru_fa_node_data_destroy(keys_node_data);
            }
        }
        if (fa_node_roots[1] == NULL) {
            if (values_node_data != NULL) {
                pthru_fa_node_data_destroy(values_node_data);
            }
        }

        pthru_fa_node_roots_destroy(PTHRU_OP_LOOKUP_NUM_ROOTS, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy given operation metadata.
 *
 * Destroy given operation metadata.
 *
 * \param [in] op_md Operation metadata to destroy.
 */
static void
pthru_op_md_destroy(bcmltm_lt_op_md_t *op_md)
{
    if (op_md == NULL) {
        return;
    }

    pthru_fa_node_roots_destroy(op_md->num_roots,
                                op_md->fa_node_roots);

    pthru_ee_node_roots_destroy(op_md->num_roots,
                                op_md->ee_node_roots);

    PTHRU_OP_MD_FREE(op_md);

    return;
}


/*!
 * \brief Create operation metadata for the set opcode.
 *
 * Create the operation metadata for given table ID to use for the set opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_op_md_set_create(int unit, bcmdrd_sid_t sid,
                       bcmltm_wb_handle_t *wb_handle,
                       bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get memory from pool */
    op_md = PTHRU_OP_MD_ALLOC(unit, BCMLT_PT_OPCODE_SET, sid);
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);

    /* Fill data */
    op_md->num_roots = PTHRU_OP_SET_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_roots_set_create(unit, sid,
                                        &op_md->ee_node_roots));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_roots_set_create(unit, sid,
                                        &op_md->fa_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_op_md_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create operation metadata for the MODIFY opcode.
 *
 * Create the operation metadata for given table ID to use for the
 * MODIFY opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_op_md_modify_create(int unit, bcmdrd_sid_t sid,
                          bcmltm_wb_handle_t *wb_handle,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get memory from pool */
    op_md = PTHRU_OP_MD_ALLOC(unit, BCMLT_PT_OPCODE_MODIFY, sid);
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);

    /* Fill data */
    op_md->num_roots = PTHRU_OP_MODIFY_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_roots_modify_create(unit, sid,
                                           &op_md->ee_node_roots));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_roots_modify_create(unit, sid,
                                           &op_md->fa_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_op_md_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create operation metadata for the get opcode.
 *
 * Create the operation metadata for given table ID to use for the get opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_op_md_get_create(int unit, bcmdrd_sid_t sid,
                       bcmltm_wb_handle_t *wb_handle,
                       bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);


    /* Get memory from pool */
    op_md = PTHRU_OP_MD_ALLOC(unit, BCMLT_PT_OPCODE_GET, sid);
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);

    /* Fill data */
    op_md->num_roots = PTHRU_OP_GET_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_roots_get_create(unit, sid,
                                        &op_md->ee_node_roots));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_roots_get_create(unit, sid,
                                        &op_md->fa_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_op_md_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create operation metadata for the lookup opcode.
 *
 * Create the operation metadata for given table ID to use for the
 * lookup opcode.
 *
 * The LOOKUP opcode is only available on Hash tables.  The routine
 * will not create any metadata for non-Hash tables.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_op_md_lookup_create(int unit, bcmdrd_sid_t sid,
                          bcmltm_wb_handle_t *wb_handle,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Do not create opcode metadata if table is not Hash */
    if (!bcmdrd_pt_attr_is_hashed(unit, sid)) {
        SHR_EXIT();
    }

    /* Get memory from pool */
    op_md = PTHRU_OP_MD_ALLOC(unit, BCMLT_PT_OPCODE_LOOKUP, sid);
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);

    /* Fill data */
    op_md->num_roots = PTHRU_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (pthru_ee_node_roots_lookup_create(unit, sid,
                                           &op_md->ee_node_roots));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (pthru_fa_node_roots_lookup_create(unit, sid,
                                           &op_md->fa_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_op_md_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create operation metadata for the given opcode.
 *
 * Create the operation metadata for given table ID to use for the
 * specified opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [in] opcode Opcode.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_op_md_create(int unit, bcmdrd_sid_t sid,
                   bcmltm_wb_handle_t *wb_handle,
                   bcmlt_pt_opcode_t opcode,
                   bcmltm_lt_op_md_t **op_md_ptr)
{
    SHR_FUNC_ENTER(unit);

    *op_md_ptr = NULL;

    switch(opcode) {
    case BCMLT_PT_OPCODE_NOP:
        SHR_EXIT();
        break;
    case BCMLT_PT_OPCODE_SET:   /* Update the entry contents */
        SHR_IF_ERR_EXIT
            (pthru_op_md_set_create(unit, sid, wb_handle, op_md_ptr));
        SHR_EXIT();
        break;
    case BCMLT_PT_OPCODE_MODIFY:/* Modify the specified fields of an entry */
        SHR_IF_ERR_EXIT
            (pthru_op_md_modify_create(unit, sid, wb_handle, op_md_ptr));
        SHR_EXIT();
        break;
    case BCMLT_PT_OPCODE_GET:   /* Retrieve the contents of an entry */
        SHR_IF_ERR_EXIT
            (pthru_op_md_get_create(unit, sid, wb_handle, op_md_ptr));
        SHR_EXIT();
        break;
    case BCMLT_PT_OPCODE_CLEAR: /* Restore entry to default values */
        /* TBD */
        *op_md_ptr = NULL;
        SHR_EXIT();
        break;
    case BCMLT_PT_OPCODE_LOOKUP:
        SHR_IF_ERR_EXIT
            (pthru_op_md_lookup_create(unit, sid, wb_handle, op_md_ptr));
        SHR_EXIT();
        break;
    case BCMLT_PT_OPCODE_FIFO_POP: /* Other PT operations */
    case BCMLT_PT_OPCODE_FIFO_PUSH:
    case BCMLT_PT_OPCODE_INSERT:
    case BCMLT_PT_OPCODE_DELETE:
        /* TBD */
        SHR_EXIT();
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        break;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_op_md_destroy(*op_md_ptr);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the LT metadata.
 *
 * Destroy the LT metadata for given table.
 *
 * Assumes:
 *   - unit and sid are valid
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_md LT metadata to destroy.
 */
static void
pthru_lt_destroy(int unit, bcmdrd_sid_t sid,
                 bcmltm_lt_md_t *lt_md)
{
    bcmlt_pt_opcode_t opcode;

    if (lt_md == NULL) {
        return;
    }

    /* Destroy ops */
    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        pthru_op_md_destroy(lt_md->op[opcode]);
    }

    /* Destroy params */
    pthru_params_destroy(lt_md->params);

    /* Destroy FA node cookies: Keys, Values */
    pthru_ptm_mapping_destroy(PTHRU_PTM_MAPPING_VALUES_SID(unit, sid));
    PTHRU_PTM_MAPPING_VALUES_SID(unit, sid) = NULL;
    pthru_ptm_mapping_destroy(PTHRU_PTM_MAPPING_KEYS_SID(unit, sid));
    PTHRU_PTM_MAPPING_KEYS_SID(unit, sid) = NULL;

    /* Destroy EE node cookies for EE stage */
    pthru_pt_list_destroy(PTHRU_PT_LIST_SID(unit, sid));
    PTHRU_PT_LIST_SID(unit, sid) = NULL;

    PTHRU_LT_MD_FREE(lt_md);

    return;
}


/*!
 * \brief Create the LT metadata.
 *
 * Create the LT metadata for given table.
 *
 * Assumes:
 *   - unit and sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_md_ptr Returning pointer to LT metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_lt_create(int unit,
                bcmdrd_sid_t sid,
                bcmltm_lt_md_t **lt_md_ptr)
{
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_params_t *params = NULL;
    bcmltm_lt_op_md_t *op_md = NULL;
    bcmlt_pt_opcode_t opcode;
    bcmltm_map_pt_t *pt_map = NULL;
    bcmltm_wb_handle_t wb_handle;

    SHR_FUNC_ENTER(unit);

    /* Initialize Working Buffer handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_init(sid, &wb_handle));

    /* Create the table PT mapping */
    SHR_IF_ERR_EXIT
        (pthru_pt_map_create(unit, sid, &wb_handle, &pt_map));

    /* Get memory from pool for LT metadata */
    lt_md = PTHRU_LT_MD_ALLOC(unit, sid);
    SHR_NULL_CHECK(lt_md, SHR_E_MEMORY);

    /* Create EE node cookie */
    /* The PT list is 1 and the PT ID is same as the SID for Pass Thru */
    SHR_IF_ERR_EXIT
        (pthru_pt_list_create(unit, pt_map, &wb_handle,
                              &PTHRU_PT_LIST_SID(unit, sid)));

    /* Create FA node cookies: Keys, Values */
    SHR_IF_ERR_EXIT
        (pthru_ptm_mapping_create(unit, sid, pt_map, &wb_handle,
                                  PTHRU_FIELD_TYPE_KEY,
                                  &PTHRU_PTM_MAPPING_KEYS_SID(unit, sid)));
    SHR_IF_ERR_EXIT
        (pthru_ptm_mapping_create(unit, sid, pt_map, &wb_handle,
                                  PTHRU_FIELD_TYPE_VALUE,
                                  &PTHRU_PTM_MAPPING_VALUES_SID(unit, sid)));


    /* Create params */
    SHR_IF_ERR_EXIT(pthru_params_create(unit, sid, &params));
    lt_md->params = params;

    /* Create ops */
    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        SHR_IF_ERR_EXIT
            (pthru_op_md_create(unit, sid, &wb_handle, opcode, &op_md));
        lt_md->op[opcode] = op_md;
    }

    *lt_md_ptr = lt_md;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_lt_destroy(unit, sid, lt_md);
        *lt_md_ptr = NULL;
    }

    /* Destroy the table PT mapping */
    pthru_pt_map_destroy(pt_map);

    /* Cleanup Working Buffer handle */
    bcmltm_wb_handle_cleanup(&wb_handle);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the LTM metadata.
 *
 * Destroy the LTM metadata for given unit.
 *
 * This implementation is based upon the symbols IDs
 * being in a compact numbering space from [0 .. (sid_max_count-1)]
 *
 * Assumes:
 *   - unit is valid
 *
 * \param [in] unit Unit number.
 * \param [in] ltm_md LTM metadata to destroy.
 */
static void
pthru_ltm_destroy(int unit, bcmltm_md_t *ltm_md)
{
    bcmdrd_sid_t sid;
    bcmdrd_sid_t sid_max;

    if (ltm_md == NULL) {
        return;
    }

    sid_max = ltm_md->lt_max - 1;

    /* Destroy LT metadata for each table */
    for (sid = 0; sid <= sid_max; sid++) {
        pthru_lt_destroy(unit, sid, ltm_md->lt_md[sid]);
        ltm_md->lt_md[sid] = NULL;
    }

    /* Free LTM metadata */
    SHR_FREE(ltm_md);

    return;
}


/*!
 * \brief Create the LTM metadata for given unit.
 *
 * Create the LTM metadata for given unit.
 *
 * This implementation is based upon the symbols IDs
 * being in a compact numbering space from [0 .. (sid_max_count-1)]
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of tables IDs.
 * \param [out] ltm_md_ptr Returning pointer to LT metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_ltm_create(int unit,
                 size_t sid_max_count,
                 bcmltm_md_t **ltm_md_ptr)
{
    unsigned int size;
    bcmdrd_sid_t sid;
    bcmdrd_sid_t sid_max;
    bcmltm_md_t *ltm_md = NULL;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmdrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    size_t opx;

    SHR_FUNC_ENTER(unit);

    /* SID max value */
    sid_max = sid_max_count - 1;

    /* Allocate sid list array */
    size = sizeof(*sid_list) * sid_max_count;
    SHR_ALLOC(sid_list, size,
              "LTM PT Pass Thru sid_list");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_sid_list_get(unit, sid_max_count, sid_list, &num_sid));

    /* Verify that SID value fits design criteria [0 .. (max_count-1)] */
    for (idx = 0; idx < num_sid; idx++) {
        if (sid_list[idx] > sid_max) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid SID %d, "
                                  "SID should not be larger than %d\n"),
                       sid_list[idx], sid_max));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Allocate LTM metadata structure */
    size = sizeof(bcmltm_md_t) +
        (sizeof(bcmltm_lt_md_t *) * sid_max_count);
    SHR_ALLOC(ltm_md, size, "LTM Metadata");
    SHR_NULL_CHECK(ltm_md, SHR_E_MEMORY);
    sal_memset(ltm_md, 0, size);

    ltm_md->lt_max = sid_max_count;
    ltm_md->wb_max_modeled = 0;
    ltm_md->wb_max_interactive = 0;

    /* Create LT metadata for each table */
    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];
        /*
         * Skip invalid SIDs.
         * SID list from DRD may contain symbols that are not valid
         * for the current device configuration.
         */
        if (!bcmdrd_pt_is_valid(unit, sid)) {
            continue;
        }

        SHR_IF_ERR_EXIT(pthru_lt_create(unit, sid, &lt_md));
        ltm_md->lt_md[sid] = lt_md;
        ltm_md->lt_num++;

        /* Check for maximum Working Buffer size */
        for (opx = BCMLT_PT_OPCODE_NOP; opx < BCMLT_PT_OPCODE_NUM; opx++) {
            if (lt_md->op[opx] != NULL) {
                if (lt_md->op[opx]->working_buffer_size >
                    ltm_md->wb_max_interactive) {
                    ltm_md->wb_max_interactive =
                        lt_md->op[opx]->working_buffer_size;
                }
            }
        }
    }

    *ltm_md_ptr = ltm_md;

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_ltm_destroy(unit, ltm_md);
        *ltm_md_ptr = NULL;
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup the array of pointers to PT list.
 *
 * Free the memory allocated for the array of pointers to PT list
 * for given unit.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_info_pt_list_cleanup(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_sid_t sid_max;

    if (PTHRU_PT_LIST(unit) == NULL) {
        return;
    }

    /*
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    sid_max = PTHRU_SID_MAX_COUNT(unit) - 1;

    for (sid = 0; sid <= sid_max; sid++) {
        /* DRD symbol id is the same */
        pthru_pt_list_destroy(PTHRU_PT_LIST_SID(unit, sid));
    }

    SHR_FREE(PTHRU_PT_LIST(unit));
    PTHRU_PT_LIST(unit) = NULL;

    return;
}


/*!
 * \brief Allocate array of pointers to PT list metadata.
 *
 * Allocate array of pointers to store the PT list metadata for
 * given number of table IDs.
 *
 * Note that this function does not populate the information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Number of pointers to allocate.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_info_pt_list_init(int unit, size_t sid_max_count)
{
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    size = sizeof(bcmltm_pt_list_t *) * sid_max_count;

    /* Allocate PT List pointers */
    SHR_ALLOC(PTHRU_PT_LIST(unit), size,
              "LTM PT Pass Thru info pt_list ptrs");
    SHR_NULL_CHECK(PTHRU_PT_LIST(unit), SHR_E_MEMORY);
    sal_memset(PTHRU_PT_LIST(unit), 0, size);

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_info_pt_list_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup the array of pointers to field mapping.
 *
 * Free the memory allocated for the array of pointers to field mapping
 * for given unit.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_info_ptm_mapping_cleanup(int unit)
{
    bcmdrd_sid_t sid;
    bcmdrd_sid_t sid_max;

    /*
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    sid_max = PTHRU_SID_MAX_COUNT(unit) - 1;

    /* Cleanup Keys Mapping */
    if (PTHRU_PTM_MAPPING_KEYS(unit) != NULL) {
        for (sid = 0; sid <= sid_max; sid++) {
            /* DRD symbol id is the same */
            pthru_ptm_mapping_destroy(PTHRU_PTM_MAPPING_KEYS_SID(unit, sid));
        }

        SHR_FREE(PTHRU_PTM_MAPPING_KEYS(unit));
        PTHRU_PTM_MAPPING_KEYS(unit) = NULL;
    }

    /* Cleanup Values Mapping */
    if (PTHRU_PTM_MAPPING_VALUES(unit) != NULL) {
        for (sid = 0; sid <= sid_max; sid++) {
            /* DRD symbol id is the same */
            pthru_ptm_mapping_destroy(PTHRU_PTM_MAPPING_VALUES_SID(unit, sid));
        }

        SHR_FREE(PTHRU_PTM_MAPPING_VALUES(unit));
        PTHRU_PTM_MAPPING_VALUES(unit) = NULL;
    }
    return;
}


/*!
 * \brief Allocate array of pointers for field mapping metadata.
 *
 * Allocate array of pointers to store key and value field mapping
 * metadata for given number of table IDs.
 *
 * Note that this function does not populate the information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Number of pointers to allocate.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_info_ptm_mapping_init(int unit, size_t sid_max_count)
{
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    size = sizeof(bcmltm_to_ptm_mapping_t *) * sid_max_count;

    /* Allocate Key Mapping pointers */
    SHR_ALLOC(PTHRU_PTM_MAPPING_KEYS(unit), size,
              "LTM PT Pass Thru info ptm_mapping key ptrs");
    SHR_NULL_CHECK(PTHRU_PTM_MAPPING_KEYS(unit), SHR_E_MEMORY);
    sal_memset(PTHRU_PTM_MAPPING_KEYS(unit), 0, size);

    /* Allocate Value Mapping pointers */
    SHR_ALLOC(PTHRU_PTM_MAPPING_VALUES(unit), size,
              "LTM PT Pass Thru info ptm_mapping values ptrs");
    SHR_NULL_CHECK(PTHRU_PTM_MAPPING_VALUES(unit), SHR_E_MEMORY);
    sal_memset(PTHRU_PTM_MAPPING_VALUES(unit), 0, size);

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_info_ptm_mapping_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup internal metadata information.
 *
 * Free all pre-allocated memory for given unit.
 *
 * \param [in] unit Unit number.
 */
static void
pthru_info_cleanup(int unit)
{
    if (!PTHRU_INFO_INIT(unit)) {
        return;
    }

    /* Cleanup internal information */
    pthru_info_ptm_mapping_cleanup(unit);
    pthru_info_pt_list_cleanup(unit);
    pthru_pool_destroy(unit);

    SHR_FREE(PTHRU_INFO(unit));
    PTHRU_INFO(unit) = NULL;

    return;
}


/*!
 * \brief Create and initialize internal metadata information.
 *
 * Create and initialize internal metadata information for given unit.
 * This includes pre-allocating memory for a different set of information
 * required by the LTM metadata structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of table IDs.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pthru_info_init(int unit, size_t sid_max_count)
{
    pthru_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (PTHRU_INFO_INIT(unit)) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "LTM PT Pass Thru info");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));
    PTHRU_INFO(unit) = info;

    /* Initialize information */
    info->sid_max_count = sid_max_count;
    SHR_IF_ERR_EXIT
        (pthru_pool_create(unit, sid_max_count));
    SHR_IF_ERR_EXIT
        (pthru_info_pt_list_init(unit, sid_max_count));

    SHR_IF_ERR_EXIT
        (pthru_info_ptm_mapping_init(unit, sid_max_count));

 exit:
    if (SHR_FUNC_ERR()) {
        pthru_info_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_pthru_create(int unit)
{
    size_t sid_max_count;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM PT Pass Thru metadata create\n")));

    /* Check if metadata has been initialized */
    if (PTHRU_INFO_INIT(unit)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "LTM PT Pass Thru metadata is already "
                             "created\n")));
        SHR_EXIT();
    }

    /* Get total table symbol IDs count */
    SHR_IF_ERR_EXIT(bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_max_count));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total PT Pass Thru tables is %d\n"),
                 (int)sid_max_count));

    /* Initialize internal information */
    SHR_IF_ERR_EXIT
        (pthru_info_init(unit, sid_max_count));

    /* Create LTM metadata */
    SHR_IF_ERR_EXIT
        (pthru_ltm_create(unit, sid_max_count, &PTHRU_LTM_MD(unit)));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_pthru_destroy(unit);
    }

    SHR_FUNC_EXIT();
}


int
bcmltm_md_pthru_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (!PTHRU_INFO_INIT(unit)) {
        SHR_EXIT();
    }

    /* Destroy LTM metadata */
    pthru_ltm_destroy(unit, PTHRU_LTM_MD(unit));
    PTHRU_LTM_MD(unit) = NULL;

    /* Cleanup PT Pass Thru info data structure */
    pthru_info_cleanup(unit);

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pthru_lt_retrieve(int unit,
                            uint32_t ltid,
                            bcmltm_lt_md_t **ltm_md_ptr)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!PTHRU_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    sid = BCMLTM_LTID_TO_SID(ltid);

    if (!PTHRU_SID_VALID(unit, sid)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid table id %d (sid=%d)\n"),
                   ltid, sid));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    *ltm_md_ptr = PTHRU_LTM_MD(unit)->lt_md[sid];

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_wb_max_get(int unit,
                           uint32_t *wb_max)
{
    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!PTHRU_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    *wb_max = PTHRU_LTM_MD(unit)->wb_max_interactive;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pthru_key_field_list_retrieve(int unit, bcmdrd_sid_t sid,
                                        const bcmltm_ptm_field_list_t **list)
{
    SHR_FUNC_ENTER(unit);

    if (!PTHRU_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    if (!PTHRU_SID_VALID(unit, sid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    *list = PTHRU_PTM_MAPPING_KEYS_SID(unit, sid)->field_list;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pthru_value_field_list_retrieve(int unit, bcmdrd_sid_t sid,
                                          const bcmltm_ptm_field_list_t **list)
{
    SHR_FUNC_ENTER(unit);

    if (!PTHRU_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    if (!PTHRU_SID_VALID(unit, sid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    *list = PTHRU_PTM_MAPPING_VALUES_SID(unit, sid)->field_list;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pthru_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                               bcmdrd_sym_field_info_t *finfo)
{

    SHR_FUNC_ENTER(unit);

    if (!PTHRU_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    if (!PTHRU_SID_VALID(unit, sid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check if field is a key */
    if ((fid == BCMLTM_PT_FIELD_KEY_INDEX) ||
        (fid == BCMLTM_PT_FIELD_KEY_PORT) ||
        (fid == BCMLTM_PT_FIELD_KEY_INSTANCE)) {
        SHR_IF_ERR_EXIT(pthru_key_field_info_get(unit, sid, fid, finfo));
    } else {
        /* Value field, get info from DRD */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_info_get(unit, sid, fid, finfo));
    }

 exit:
    SHR_FUNC_EXIT();
}

