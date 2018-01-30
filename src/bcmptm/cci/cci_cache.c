/*! \file cci_cache.c
 *
 * Functions for counter cache (SW) sub module
 *
 * This file contains implementation of counter cache (SW) for CCI
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

/******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include "cci_cache.h"

/******************************************************************************
 * Defines
 */
/*!
 * \brief Default size of temporary config map
 */
#define CFG_MAP_DEFAULT_SIZE      512
/*!
 * \brief maximum number of counter Fields per symbol entry
 */
#define CTR_FIELD_NUM_MAX         16

/*!
 * \brief Check if table instance and Index is valid
     [in] map , Pointer to the entry in countr map Table for SID
     [in] tbl_inst, Table instance in Dynamic Info
     [in] index, Index for the instance
*/
#define INDEX_VALID(map , inst, index) \
                (((uint32_t)(index) >= (map)->index_min) && \
                  ((uint32_t)(index) <= (map)->index_max) && \
                 (inst >= 0) && (inst < (map)->tbl_inst))

/*!
 * \brief Get the index of the counter in Counter Table
     [in] map , Pointer to the entry in countr map Table for SID
     [in] tbl_inst, Table instance in Dynamic Info
     [in] index, Index for the instance
     [in] field, Counter Field number in entry for SID
*/

#define CTR_INDEX(map , inst, index, field) \
            (uint32_t)(((map)->ctr_table_offset) + \
                (((map)->index_max) - ((map)->index_min) + (1)) \
                  * (uint32_t)(inst) * (uint32_t)((map)->field_num) + \
                  (uint32_t)(index) * (uint32_t)((map)->field_num) + \
                  (uint32_t)(field))


/******************************************************************************
 * Typedefs
 */


/*!
 * \brief Description of counts provided by Counter
 */

typedef struct cci_count_s {
    uint64_t  hw_count;     /*!< hw_count, Previous hardware Count */
    uint64_t  sw_count;     /*!< sw_count, Accumulated  software Count */
} cci_count_t;

typedef struct cci_field_s {
    uint16_t minbit;        /*! First bit of this field. */
    uint16_t maxbit;        /*! Last bit of this field. */
} cci_field_t;

/*!
 * \brief base counter map object
 */
typedef struct cci_ctr_cache_map_s {
    bcmdrd_sid_t sid;          /*!< Symbol ID */
    uint32_t     offset;       /*!< Fixed part of symbol address */
    int          tbl_inst;     /*!< Number of table instances */
    int          ctrtype;      /*! Counter Type */
    bool         is_mem;       /*!< Sym is memory */
    bool         enable;       /*!< enable to update */
    bool         clr_on_read;  /*!<clear-on-read for sbus reads */
    bool         locked;       /*!< lock status */
    bool         clr_on_ser;   /*! Cache clear from SER event */
    size_t       size;         /*!< size,number of entries in counter Table  */
    uint32_t     index_min;    /*! Minimum valid index */
    uint32_t     index_max;    /*! Maximum valid index */
    uint32_t     ctr_table_offset;   /*!< ctr_table_offset,in counter Table */
    size_t       field_num;            /*!< field_num, # of counter fields */
    uint32_t     pt_entry_wsize;       /*!< physical entry size in words */
    cci_field_t  pt_field[CTR_FIELD_NUM_MAX];   /*!< physcal field*/
    cci_field_t  lt_field[CTR_FIELD_NUM_MAX];   /*!< logical field*/
} cci_ctr_cache_map_t;

/*!
 * \brief Counter Cache  Instance
 */

typedef struct cci_ctr_cache_s {
    /*!< parent handle of parent */
    const cci_context_t  *parent;
    /*!< mutex to wait for cache update */
    sal_mutex_t          mlock;
    /*!< Counter map Table */
    size_t               ctr_map_size;
    cci_ctr_cache_map_t  *ctr_map_table;
    /*!< Counter Table */
    size_t               ctr_table_size;
    cci_count_t          *ctr_table;
} cci_ctr_cache_t;

/*!
 * \brief Config map to be used during configuration of counter map table
 */

typedef struct config_map_s {
    size_t              size;
    cci_ctr_cache_map_t *map;
} config_map_t;


/******************************************************************************
* Private variables
 */
/* Map arrary used at the time of configuration */
static config_map_t config_map[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * cci_pt_read:
 * Read PT Entry
 */
static int
cci_pt_read(int unit,
            bcmptm_cci_ctr_info_t *info,
            uint32_t *buf,
            size_t size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmbd_pt_read(unit, info->sid,
                      &info->dyn_info, info->in_pt_ovrr_info,
                      buf, size));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cci_pt_write:
 * Write PT Entry
 */
static int
cci_pt_write(int unit,
            bcmptm_cci_ctr_info_t *info,
            uint32_t *buf)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmbd_pt_write(unit, info->sid,
                       &info->dyn_info, info->in_pt_ovrr_info,
                       buf));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Copy field  bits between two word arrays.
 *
 * \param [in, out] destination buffer pointer.
 * \param [in] destination field offset (in bit).
 * \param [in] source buffer pointer.
 * \param [in] source field offset (in bits).
 * \param [in] field size bits.
 */
static void
cci_buff32_field_copy(uint32_t *d, int offd, const uint32_t *s,
                     int offs, int n)
{
    uint32_t masks, maskd;
    const uint32_t *ps;
    uint32_t *pd;

    while (n--) {
        masks  = 1 << (offs & 31);
        ps = s + (offs >> 5);
        pd = d + (offd >> 5);
        if (*ps & masks) {
            maskd = 1 << (offd & 31);
            *pd |= maskd;
        } else {
            maskd = ~(1 << (offd & 31));
            *pd &= maskd;
        }
        offs++;
        offd++;
    }
}

/******************************************************************************
 * ctr_field_init:
 * Initializes the PT and LT field information
 */
static int
ctr_field_init(int unit, cci_ctr_cache_map_t *map)
{
    size_t i;
    bcmdrd_sym_field_info_t finfo;
    bcmdrd_fid_t *fid_list = NULL;
    size_t field_num;
    bcmdrd_sid_t sid;
    uint16_t lt_shift = 0, lt_max = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_MEMORY);

    sid = map->sid;
    /* If fileds are formatted , Get format SID */
    SHR_IF_ERR_EXIT(
        bcmptm_pt_cci_frmt_sym_get(unit, &sid));
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &field_num));
    SHR_ALLOC(fid_list, field_num * sizeof(bcmdrd_fid_t), "FID");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    /* Get the field list and identify counter fields only */
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_fid_list_get(unit, sid, field_num,
                                  fid_list, &field_num));
    for (map->field_num = 0, i = 0; i < field_num; i++) {
        if (SHR_FAILURE(
                bcmdrd_pt_field_info_get(unit, sid, fid_list[i], &finfo))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "Error Field Info SID =%d Name = %s\n"),
                                 sid,
                                 bcmdrd_pt_sid_to_name(unit, map->sid)));
                map->pt_field[i].minbit = 0;
                map->pt_field[i].maxbit = 0;
        } else {
            /* In the case field is counter */
            if (bcmdrd_pt_field_is_counter(unit, sid, fid_list[i])) {
                size_t f_num = map->field_num;
                cci_field_t *pt_field = &map->pt_field[f_num];
                cci_field_t *lt_field = &map->lt_field[f_num];

                pt_field->minbit = finfo.minbit;
                pt_field->maxbit = finfo.maxbit;
                /* lt_shift determines the shift in counter lt bits due to shift */
                /* Reset the shift if pt min bit >= lt_max */
                if (pt_field->minbit >= lt_max) {
                    lt_shift = 0;
                }
                lt_field->minbit = lt_shift + pt_field->minbit;
                /* support 64 bit fields */
                lt_field->maxbit = lt_field->minbit + 63;
                lt_max = lt_field->maxbit;
                /* Increment the shift */
                lt_shift += 64 - (pt_field->maxbit -
                            pt_field->minbit + 1);
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "SID =%d Name = %s, \
                             pt-minbit =%u, pt-maxbit = %u \
                             lt-minbit =%u, lt-maxbit = %u\n"),
                             map->sid,
                             bcmdrd_pt_sid_to_name(unit, map->sid),
                             pt_field->minbit,
                             pt_field->maxbit,
                             lt_field->minbit,
                             lt_field->maxbit));
                map->field_num++;
            } else {
                /* Non counter fields will be same width as HW */
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "Field Info SID =%d Field Name = %s, \
                                  field bits = %d\n"),
                                 sid,  finfo.name, (finfo.maxbit -finfo.minbit + 1)));
            }
        }
    }
    if (fid_list) {
        SHR_FREE(fid_list);
        fid_list = NULL;
    }


exit:
    if (fid_list) {
        SHR_FREE(fid_list);
    }
    SHR_FUNC_EXIT();

}
/******************************************************************************
 * ctr_map_table_set:
 * Set the Counter Map Table for all SID's
 */

static void
ctr_map_table_set(int unit,
                  cci_ctr_cache_t *cache,
                  cci_ctr_cache_map_t *map)
{
    uint32_t offset = 0;
    uint32_t i;
    bcmdrd_sid_t sid;

    for(i = 0; i < cache->ctr_map_size; i++) {
        cache->ctr_map_table[i] =  map[i];
        cache->ctr_map_table[i].enable = 1;
        cache->ctr_map_table[i].ctr_table_offset = offset;
        offset += cache->ctr_map_table[i].size;
        sid = cache->ctr_map_table[i].sid;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "sid =%d, Name = %s, Map =%d, ctr offset = 0x%x\n"),
                              sid, bcmdrd_pt_sid_to_name(unit, sid),
                              i, cache->ctr_map_table[i].offset));
    }
}

/******************************************************************************
 * ctr_map_table_search:
 * Search the SID in counter Map Table and Return the index
 */
static int
ctr_map_table_search(int unit,
                     cci_ctr_cache_map_t *map,
                     bcmdrd_sym_info_t *info,
                     uint32_t start,
                     uint32_t end,
                     uint32_t  *map_id)
{
    uint32_t mid = (start + end)/2;
    uint32_t offset = info->offset;
    int rv;

    if (map[mid].offset == offset) {
        bool is_mem = info->flags & BCMDRD_SYMBOL_FLAG_MEMORY;
        /* Memory and Register can have same offset so return accordingly */
        if (is_mem  == map[mid].is_mem) {
            *map_id = mid;
        } else  {
            if (is_mem  == map[mid + 1].is_mem) {
                *map_id = mid + 1;
            } else {
                /* This can happen only for mid > 0 */
                *map_id = mid - 1;
            }
        }
        rv = SHR_E_NONE;
    } else if (start < end) {
       if (offset < map[mid].offset) {
           rv = ctr_map_table_search(unit, map, info, start,
                                     mid - 1, map_id);
       } else {
           rv = ctr_map_table_search(unit, map, info, mid + 1,
                                        end, map_id);
      }
    } else {
       rv = SHR_E_FAIL;
    }

    return rv;
}

/******************************************************************************
 * ctr_map_table_swap:
 * Soritng helper function
 */
static void
config_map_table_swap(cci_ctr_cache_map_t *map,
                      int i,
                      int j)
{
    cci_ctr_cache_map_t t;

    t = map[i];
    map[i] = map[j];
    map[j] = t;
}

/******************************************************************************
 * ctr_map_table_divide:
 * Sorting helper function to divide list into two parts for recursive sort
 */
static int
config_map_table_divide(cci_ctr_cache_map_t *map,
                        int start,
                        int end)
{
   uint32_t pivot = map[end].offset;
   int i, j;

    for (i = start - 1, j = start; j < end; j++)
    {
        if (map[j].offset <= pivot)
        {
            i++;
            config_map_table_swap(map, i, j);
        }
    }
    config_map_table_swap(map, i + 1, end);

    return (i + 1);
}

/******************************************************************************
 * ctr_map_table_sort:
 * Sorting function to sort SID based on offset (Fixed part of Address)]
 */
static void
config_map_table_sort(cci_ctr_cache_map_t *map,
                      int start,
                      int end)
{
    int i;

    if(start < end)
    {
        /* divide and conquer */
        i = config_map_table_divide(map, start, end);
        config_map_table_sort(map, start, i - 1);
        config_map_table_sort(map, i + 1, end);
    }
}

/******************************************************************************
 * config_map_table_alloc:
 * Allocate config Map Table
 */
static int
config_map_table_alloc(int unit,
                       config_map_t *cmap,
                       size_t size)
{
    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(cmap->map,
              sizeof(cci_ctr_cache_map_t) * size,
              "MAP");
    SHR_NULL_CHECK(cmap->map, SHR_E_MEMORY);
    sal_memset(cmap->map, 0,
               sizeof(cci_ctr_cache_map_t) * size);
    cmap->size = size;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * config_map_table_free:
 * Free Config Map Table
 */
static int
config_map_table_free(int unit,
                      config_map_t *cmap)
{
    SHR_FUNC_ENTER(unit);

    if (cmap->map) {
        SHR_FREE(cmap->map);
        cmap->map = NULL;
        cmap->size = 0;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * config_map_table_realloc:
 * Reallocate Config Map Table
 */
static int
config_map_table_realloc(int unit,
                         config_map_t *cmap,
                         size_t size)
{
    cci_ctr_cache_map_t  *tmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(tmap, size * sizeof(cci_ctr_cache_map_t), "T MAP");
    SHR_NULL_CHECK(tmap, SHR_E_MEMORY);
    sal_memcpy(tmap, cmap->map,
               sizeof(cci_ctr_cache_map_t) * cmap->size);
    SHR_FREE(cmap->map);
    cmap->map = tmap;
    cmap->size = size;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cache_tbl_alloc: Allocate
 * Cache tables in HA space
 */
static int
cache_tbl_alloc(int unit, cci_ctr_cache_t *cache)
{
    cci_context_t  *con;
    uint32_t        ha_alloc_size;
    uint32_t        ha_req_size;

    SHR_FUNC_ENTER(unit);
    con = (cci_context_t *)cache->parent;

    /* Allocate Counter Map Table in HA Space, if already allocated, return pointer */
    ha_req_size = cache->ctr_map_size * sizeof(cci_ctr_cache_map_t);
    ha_alloc_size = ha_req_size;
    cache->ctr_map_table = shr_ha_mem_alloc(unit,
                                            BCMMGMT_PTM_COMP_ID,
                                            BCMPTM_HA_SUBID_CCI_MAPID,
                                            &ha_alloc_size);
    SHR_NULL_CHECK(cache->ctr_map_table, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (con->info->ctr_map_size != cache->ctr_map_size) {
        con->info->ctr_map_size = cache->ctr_map_size;
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,"con->info->ctr_map_size %d\n"),
             (int)con->info->ctr_map_size));

    /* Allocate Counter Table in HA Space, if already allocated, return pointer */
    ha_req_size = cache->ctr_table_size * sizeof(cci_count_t);
    ha_alloc_size = ha_req_size;
    cache->ctr_table = shr_ha_mem_alloc(unit,
                                        BCMMGMT_PTM_COMP_ID,
                                        BCMPTM_HA_SUBID_CCI_CTR_CACHE,
                                        &ha_alloc_size);
    SHR_NULL_CHECK(cache->ctr_table, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (con->info->ctr_table_size != cache->ctr_table_size) {
        con->info->ctr_table_size = cache->ctr_table_size;
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,"con->info->ctr_table_size %d\n"),
             (int)con->info->ctr_table_size));
exit:
    if (SHR_FUNC_ERR()) {
        if (cache->ctr_map_table) {
            shr_ha_mem_free(unit, cache->ctr_map_table);
        }
        if (cache->ctr_table) {
            shr_ha_mem_free(unit, cache->ctr_table);
        }
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * cache_tbl_free:
 * Free cache tables from HA space
 */
static void
cache_tbl_free(int unit, cci_ctr_cache_t *cache)
{
    /* No need to release HA mem */
    if (cache->ctr_map_table) {
        cache->ctr_map_table = NULL;
        cache->ctr_map_size = 0;
    }

    if (cache->ctr_table) {
        cache->ctr_table = NULL;
        cache->ctr_table_size = 0;
    }
}


/******************************************************************************
 * Public Functions
 */

/******************************************************************************
* bcmptm_cci_cache_init
 */
int
bcmptm_cci_cache_init(int unit,
                      bool warm,
                      const cci_context_t  *cci_con,
                      cci_handle_t *handle)

{
    cci_ctr_cache_t *cache = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate the counter cache handle */
    SHR_ALLOC(cache, sizeof(cci_ctr_cache_t), "CCI cache");
    SHR_NULL_CHECK(cache, SHR_E_MEMORY);
    sal_memset(cache, 0, sizeof(cci_ctr_cache_t));
    cache->parent = cci_con;
    cache->mlock = sal_mutex_create("CCI CACHE");
    SHR_NULL_CHECK(cache->mlock, SHR_E_MEMORY);
    *handle = cache;

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_cleanup
 */
int
bcmptm_cci_cache_cleanup(int unit,
                         cci_handle_t handle)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);

    cache_tbl_free(unit, cache);
    if (cache->mlock) {
        sal_mutex_destroy(cache->mlock);
    }

    SHR_FREE(cache);

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_ctr_add
 */
extern int
bcmptm_cci_cache_ctr_add(int unit,
                         cci_handle_t handle,
                         bcmdrd_sid_t sid,
                         uint32_t *map_id)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmdrd_sym_info_t sinfo;
    cci_context_t   *con = NULL;
    cci_ctr_cache_map_t  *map = config_map[unit].map;
    size_t size;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    con = (cci_context_t *)cache->parent;
    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(map_id, SHR_E_PARAM);

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

/* phase 1: Store counter SID's in temporary config map table
 * Get the sizes of counter table and Counter Map Table
 * phase 2:  Get the Get map-id from sorted config map table
 * Add the counter in Counter map table in HA Space
*/

    switch (con->info->sub) {
    case 1:
        i = cache->ctr_map_size;
        map[i].sid = sid;
        map[i].offset = sinfo.offset;
        /* Counter Type */
        map[i].ctrtype = bcmptm_pt_cci_ctrtype_get(unit, sid);
        map[i].clr_on_read = 0;
        /* sym is memory */
        map[i].is_mem = sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY;
        map[i].clr_on_ser = 0;
        /* Number of entries in one instance */
        map[i].index_min = sinfo.index_min;
        map[i].index_max = sinfo.index_max;
        map[i].pt_entry_wsize = sinfo.entry_wsize;
        size = (sinfo.index_max - sinfo.index_min + 1);
        SHR_IF_ERR_EXIT(
            ctr_field_init(unit, &map[i]));
        /* Multiplied by total number of fields */
        size *= map[i].field_num;
        /* Total number of entries in all instances */
        map[i].tbl_inst = bcmdrd_pt_num_tbl_inst(unit, sid);
        size *= (map[i].tbl_inst + 1);
        cache->ctr_table_size += size;
        map[i].size = size;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "id = %d, SID =%d, name =%s, offset=0x%x \
                              index_min =%u, index_max = %u,\
                              tbl_instance = %d, ctrtype = %d\n"),
                              i, sid, sinfo.name, map[i].offset,
                              map[i].index_min, map[i].index_max,
                              map[i].tbl_inst, map[i].ctrtype));

        cache->ctr_map_size++;
        /* If number of SID's is more than default reallocate */
        if (cache->ctr_map_size >= config_map[unit].size) {
            SHR_IF_ERR_EXIT(
                config_map_table_realloc(unit, &config_map[unit],
                                         cache->ctr_map_size * 2));
            map = config_map[unit].map;
        }
    break;
    case 2:
        /* search for the sid using symbol address in config map table */
        SHR_IF_ERR_EXIT(
            ctr_map_table_search(unit, cache->ctr_map_table, &sinfo,
                                    0, cache->ctr_map_size, map_id));
        LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "name =%s, Map id=%d offset=0x%x\n"),
                              sinfo.name, *map_id,
                              cache->ctr_map_table[*map_id].offset));
    break;
    default:
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unknown Config Phase = %d\n"),
                              con->info->sub));
    break;
    }

exit:

    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_read
 */
int
bcmptm_cci_cache_read(int unit,
                      cci_handle_t handle,
                      bcmptm_cci_ctr_info_t *info,
                      uint32_t *buf,
                      size_t size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    cci_ctr_cache_map_t *lt_map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    uint16_t n;
    size_t i, lt_fld;
    size_t fld_start, fld_end;

    uint32_t index = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_table[info->map_id];
    lt_map = &cache->ctr_map_table[info->lt_map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;
    /* size in bits */
    size = size * sizeof(uint32_t) * 8;

    /* In case of field transform, only tranformed field will be copied */
    if (info->txfm_fld >= 0) {
        fld_start = info->txfm_fld;
        fld_end = fld_start + 1;
    } else {
        fld_start = 0;
        fld_end = map->field_num;
    }

    for (i = fld_start, lt_fld = 0; i < fld_end; i++, lt_fld++) {
        /* Get number of bits to be copied */
        n = (lt_map->lt_field[lt_fld].maxbit - lt_map->lt_field[lt_fld].minbit + 1);
        /* Check if size of the buffer is enough */
        /* Don't Copy the field if buffer size not sufficent */
        if (lt_map->lt_field[lt_fld].minbit + n > size) {
            LOG_VERBOSE(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Error: Name = %s, bits = %d\n"),
                          bcmdrd_pt_sid_to_name(unit, map->sid), n));
            continue;
        }
        if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
            uint32_t c[2];
            index = CTR_INDEX(map, dyn->tbl_inst, dyn->index, i);
            c[0] = (uint32_t)cache->ctr_table[index].sw_count;
            c[1] = (uint32_t)(cache->ctr_table[index].sw_count >> 32);
            LOG_VERBOSE(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Name = %s, count = 0x%08x_%08x, lt-minbit = %d\n"),
                          bcmdrd_pt_sid_to_name(unit, lt_map->sid),
                          c[1],c[0],
                          lt_map->lt_field[lt_fld].minbit));
            cci_buff32_field_copy(buf, lt_map->lt_field[lt_fld].minbit, c, 0, n);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                          dyn->index, dyn->tbl_inst));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_write
 */
int
bcmptm_cci_cache_write(int unit,
                       cci_handle_t handle,
                       bcmptm_cci_ctr_info_t *info,
                       uint32_t *buf)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    cci_ctr_cache_map_t *lt_map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    int n;
    size_t i, lt_fld;
    size_t fld_start, fld_end;
    uint32_t index;
    uint32_t c[2] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_table[info->map_id];
    lt_map = &cache->ctr_map_table[info->lt_map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;

    /* In case of field transform, only tranformed field will be written */
    if (info->txfm_fld >= 0) {
        fld_start = info->txfm_fld;
        fld_end = fld_start + 1;
    } else {
        fld_start = 0;
        fld_end = map->field_num;
    }

    /* Copy counter fields from bufffer in HW format into LT Format */
    for (i = fld_start, lt_fld = 0; i < fld_end; i++) {
        /* number of bits */
        n = (lt_map->lt_field[lt_fld].maxbit - lt_map->lt_field[lt_fld].minbit + 1);
        /* Get the counter Value  for field */
        cci_buff32_field_copy(c, 0, buf, lt_map->lt_field[i].minbit, n);
        LOG_VERBOSE(BSL_LOG_MODULE,
          (BSL_META_U(unit, "Name = %s, count = 0x%08x%08x, lt-minbit = %d\n"),
                      bcmdrd_pt_sid_to_name(unit, lt_map->sid),
                      c[1],c[0],
                      lt_map->lt_field[lt_fld].minbit));
        /* Go to SID base index in Counter Table and update */
        if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
            index = CTR_INDEX(map, dyn->tbl_inst, dyn->index, i);
            cache->ctr_table[index].sw_count = (((uint64_t)c[1] << 32) | c[0]);
            cache->ctr_table[index].hw_count = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Name = %s Index = %d, Tbl_nst = %d\n"),
                          bcmdrd_pt_sid_to_name(unit, lt_map->sid),
                          dyn->index, dyn->tbl_inst));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();

}


/******************************************************************************
* bcmptm_cci_cache_update
 */
int
bcmptm_cci_cache_update(int unit,
                        cci_handle_t handle,
                        bcmptm_cci_ctr_info_t *info,
                        uint32_t *buf,
                        size_t size)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t *map = NULL;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    int n;
    size_t i;
    uint32_t index;
    uint64_t c;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    map = &cache->ctr_map_table[info->map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;

    /* Skip cache update when SER correction happend */
    if (map->clr_on_ser) {
        map->clr_on_ser = FALSE;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Skip cache update on sid = %s map_id %d\n"),
                    bcmdrd_pt_sid_to_name(unit, map->sid), info->map_id));
        SHR_EXIT();
    }
    /* Extract the counter fields from buffer in HW format */
    for (i = 0; i < map->field_num; i++) {
        uint32_t count[2] = {0};
        /* number of bits */
        n = (map->pt_field[i].maxbit - map->pt_field[i].minbit + 1);
        /* Get the HW counter Value  for field */
        cci_buff32_field_copy(count, 0, buf, map->pt_field[i].minbit, n);
        c = (((uint64_t)count[1] << 32) | count[0]);
        /* Go to SID base index in Counter Table */
        if (INDEX_VALID(map, dyn->tbl_inst, dyn->index)) {
            index = CTR_INDEX(map, dyn->tbl_inst, dyn->index, i);
            if (map->clr_on_read) {
               /* Clear on read enabled */
               cache->ctr_table[index].sw_count += c;
               cache->ctr_table[index].hw_count = 0;
            } else {
                if (c < cache->ctr_table[index].hw_count) {
                   /* Wrap around */
                   cache->ctr_table[index].sw_count +=
                      ((((uint64_t)0x01 << n) -1) -
                       cache->ctr_table[index].hw_count) + c;
                } else {
                   cache->ctr_table[index].sw_count +=
                   c - cache->ctr_table[index].hw_count;
                }
                cache->ctr_table[index].hw_count = c;
            }
        } else {
        LOG_ERROR(BSL_LOG_MODULE,
          (BSL_META_U(unit, "Index = %d, Tbl_nst = %d\n"),
                      dyn->index, dyn->tbl_inst));
          SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();

}

/******************************************************************************
* bcmptm_cci_cache_ctr_info_get
 */
int
bcmptm_cci_cache_ctr_info_get(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id mapid,
                              bcmptm_cci_cache_ctr_info_t *cinfo)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(cinfo, SHR_E_PARAM);

    if (mapid >= cache->ctr_map_size) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    cinfo->sid = cache->ctr_map_table[mapid].sid;
    cinfo->offset = cache->ctr_map_table[mapid].offset;
    cinfo->tbl_inst = cache->ctr_map_table[mapid].tbl_inst;
    cinfo->ctrtype = cache->ctr_map_table[mapid].ctrtype;
    cinfo->is_mem = cache->ctr_map_table[mapid].is_mem;
    cinfo->enable = cache->ctr_map_table[mapid].enable;
    cinfo->index_min = cache->ctr_map_table[mapid].index_min;
    cinfo->index_max = cache->ctr_map_table[mapid].index_max;
    cinfo->field_num = cache->ctr_map_table[mapid].field_num;
    cinfo->pt_entry_wsize = cache->ctr_map_table[mapid].pt_entry_wsize;

exit:
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* Return map table size
 */
int
bcmptm_cci_cache_ctr_map_size(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id *max)

{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;

    SHR_FUNC_ENTER(unit);
    cache = (cci_ctr_cache_t *)handle;
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    *max = cache->ctr_map_size;

exit:
    SHR_FUNC_EXIT();

}

/*****************************************************************************
* bcmptm_cci_cache_ctr_config
 */
int
bcmptm_cci_cache_ctr_config(int unit,
                            cci_handle_t handle,
                            bool warm,
                            bcmptm_sub_phase_t phase)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_context_t   *con = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    con = (cci_context_t *)cache->parent;
    SHR_NULL_CHECK(con, SHR_E_INIT);

    switch(phase) {
    case BCMPTM_SUB_PHASE_1:
        SHR_IF_ERR_EXIT(
            config_map_table_alloc(unit, &config_map[unit],
                                   CFG_MAP_DEFAULT_SIZE));
    break;

    case BCMPTM_SUB_PHASE_2:
        if (DO_CONFIG(warm, con)){
            /* Arrage the temporary map  in order */
            config_map_table_sort(config_map[unit].map, 0,
                                  cache->ctr_map_size - 1);
        } else {
           /* Get the table sizes from HA Space */
           cache->ctr_map_size = con->info->ctr_map_size;
           cache->ctr_table_size = con->info->ctr_table_size;
        }
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,"cache->ctr_map_size %d "
                             "con->info->ctr_map_size %d\n"),
                 (int)cache->ctr_map_size, (int)con->info->ctr_map_size));

        /* Allocate Counter Map Table and Counter Table in HA */
        SHR_IF_ERR_EXIT(
            cache_tbl_alloc(unit, cache));

        /* Set the  Counter Table for counter SIDs */
        if (DO_CONFIG(warm, con)){
            ctr_map_table_set(unit, cache, config_map[unit].map);
            sal_memset(cache->ctr_table, 0,
                   cache->ctr_table_size * sizeof(cci_count_t));
        }

    break;

    case BCMPTM_SUB_PHASE_3:
        if (DO_CONFIG(warm, con)){
            /* All map inforamton is transferred to Counter Map Table in HA
                     * Free temp config table
                     */
            config_map_table_free(unit, &config_map[unit]);
        }
    break;
    default:
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unknown Config Phase = %d\n"),
                              phase));
    break;
    }

exit:
    if (SHR_FUNC_ERR()) {
        config_map_table_free(unit, &config_map[unit]);
        cache_tbl_free(unit, cache);
    }
    SHR_FUNC_EXIT();


}

/*****************************************************************************
* bcmptm_cci_cache_hw_sync
 */
int
bcmptm_cci_cache_hw_sync(int unit,
                         cci_handle_t handle,
                         bcmptm_cci_cache_hw_op_f hw_op,
                         void *param,
                         bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    int rv;
    int retry = CFG_SER_RETRY_COUNT;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_op, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    while (retry--) {
        /* Take The Mutex */
        if (sal_mutex_take(cache->mlock, CFG_CCI_TIME_OUT) == 0) {
            /* Hardware Sync with cache update in case of success*/
            rv = hw_op(unit, handle, param, update_cache);
            /* Release the lock for SER correction to happen,
                    * because same function will be called
                    */
            SHR_IF_ERR_EXIT(sal_mutex_give(cache->mlock));
            if (SHR_SUCCESS(rv)) {
                break;
            } else {
            
            }
       } else {
           rv = SHR_E_TIMEOUT;
           break;
       }
    }
    SHR_RETURN_VAL_EXIT(rv);

exit:
    SHR_FUNC_EXIT();


}

/*****************************************************************************
* bcmptm_cci_cache_pt_req_passthru_write
 */
int
bcmptm_cci_cache_pt_req_passthru_write(int unit,
                                       cci_handle_t handle,
                                       void *param,
                                       bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_cache_pt_param_t *pt_param = NULL;
    bcmptm_cci_ctr_info_t *info = NULL;
    uint32_t  *buff = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* SCHAN PIO request to write physical entry */
    SHR_IF_ERR_EXIT(
        cci_pt_write(unit, info, pt_param->buf));


    if (update_cache) {
        size_t size;

        size = cache->ctr_map_table[info->map_id].field_num *
               sizeof(uint64_t);
        /* Allocate dummy LT buffer and clear */
        SHR_ALLOC(buff, size, "PT BUFF");
        SHR_NULL_CHECK(buff, SHR_E_MEMORY);
        sal_memset(buff, 0, size);
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_write(unit, handle, info, buff));
    }

exit:
    if (buff) {
        SHR_FREE(buff);
    }
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* bcmptm_cci_cache_pt_req_write
 */
int
bcmptm_cci_cache_pt_req_write(int unit,
                              cci_handle_t handle,
                              void *param,
                              bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_cache_pt_param_t *pt_param;
    bcmptm_cci_ctr_info_t *info;
    cci_ctr_cache_map_t *map = NULL;
    cci_ctr_cache_map_t *lt_map = NULL;
    uint32_t  *buff = NULL;
    size_t size;
    int n;
    size_t i;
    size_t fld_start, fld_end;
    uint32_t c[2];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    map = &cache->ctr_map_table[info->map_id];
    lt_map = &cache->ctr_map_table[info->lt_map_id];
    SHR_NULL_CHECK(map, SHR_E_INIT);

    /* Allocate buffer to read hardware */
    size = map->pt_entry_wsize * sizeof(uint32_t);
    SHR_ALLOC(buff, size, "PT BUFF");
    SHR_NULL_CHECK(buff, SHR_E_MEMORY);

    /* SCHAN PIO request to read physical entry */
    SHR_IF_ERR_EXIT(
        cci_pt_read(unit, info, buff, (size/sizeof(uint32_t))));

    /* Reset the counter fields */
    c[1] = c[0] = 0;

    /* In case of field transform, only tranformed field will be reset */
    if (info->txfm_fld >= 0) {
        fld_start = info->txfm_fld;
        fld_end = fld_start + 1;
    } else {
        fld_start = 0;
        fld_end = map->field_num;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "SID = %s, SID-MAP = %s\n"
                 "index = %d, tbl_inst = %d\n"),
                 bcmdrd_pt_sid_to_name(unit, info->sid),
                 bcmdrd_pt_sid_to_name(unit, lt_map->sid),
                 info->dyn_info.index, info->dyn_info.tbl_inst));

    for (i = fld_start; i < fld_end; i++) {
        /* number of bits */
        n = (map->pt_field[i].maxbit - map->pt_field[i].minbit + 1);
        /* Get the HW counter Value  for field */
        cci_buff32_field_copy(buff, map->pt_field[i].minbit, c, 0, n);
    }
    /* Write back to the hardware */
    SHR_IF_ERR_EXIT(
        cci_pt_write(unit, info, buff));

    /* Update the cache with value as input parameter */
    if (update_cache) {
        SHR_IF_ERR_EXIT(
             bcmptm_cci_cache_write(unit, handle, info, pt_param->buf));
    }

exit:
    if (buff) {
        SHR_FREE(buff);
    }
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* bcmptm_cci_cahe_pt_req_read
 */
int
bcmptm_cci_cache_pt_req_read(int unit,
                             cci_handle_t handle,
                             void *param,
                             bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_ctr_info_t *info = NULL;
    bcmptm_cci_cache_pt_param_t *pt_param = NULL;
    void       *entry_words = NULL;
    size_t     entry_size;
    cci_ctr_cache_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    pt_param = (bcmptm_cci_cache_pt_param_t *)param;
    info = pt_param->info;
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Symbol Tansform may have changed entry size */
    map = &cache->ctr_map_table[info->map_id];
    SHR_NULL_CHECK(info, SHR_E_INIT);
    if (map->pt_entry_wsize > pt_param->size) {
        entry_size = map->pt_entry_wsize;
        SHR_ALLOC(entry_words, entry_size * sizeof(uint32_t), "PT_ENTRY");
        SHR_NULL_CHECK(entry_words, SHR_E_MEMORY);
    } else {
        entry_words = pt_param->buf;
        entry_size = pt_param->size;
    }

    /* SCHAN PIO request to read physical entry */
    SHR_IF_ERR_EXIT(
        cci_pt_read(unit, info, entry_words, entry_size));

    /* Update the cache if needed */
    if (update_cache) {
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_update(unit, handle, info, entry_words,
                                    entry_size));
    }

exit:
    if ((map) && (map->pt_entry_wsize >  pt_param->size)) {
        if (entry_words) {
            SHR_FREE(entry_words);
        }
    }
    SHR_FUNC_EXIT();
}

/*****************************************************************************
* bcmptm_cci_cahe_pt_req_read
 */
int
bcmptm_cci_cache_mapid_to_sid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              bcmdrd_sid_t *sid)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);

    if (mapid < cache->ctr_map_size) {
        *sid = cache->ctr_map_table[mapid].sid;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
/*****************************************************************************
* bcmptm_cci_cahe_pt_req_read
 */
bool
bcmptm_cci_cache_mapid_index_valid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              uint32_t index,
                              int tbl_inst)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    cci_ctr_cache_map_t  *ctr_map = NULL;
    bool rv = false;

    if (cache == NULL)
        return rv;

    ctr_map = &cache->ctr_map_table[mapid];

    if (mapid < cache->ctr_map_size) {
        if (index >= ctr_map->index_min &&
            index <= ctr_map->index_max &&
            tbl_inst <= ctr_map->tbl_inst)  {
            if (bcmptm_pt_cci_index_valid(unit,
                    ctr_map->sid, tbl_inst, index)) {
                rv = true;
            }
        }
    }
    return rv;
}
/*****************************************************************************
* bcmptm_cci_cache_lport_to_pport
 */
int
bcmptm_cci_cache_lport_to_pport(int unit,
                                cci_handle_t handle,
                                bcmptm_cci_ctr_info_t *info)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    int pport;
    bcmdrd_port_num_domain_t domain;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    dyn = &info->dyn_info;

    domain = bcmdrd_pt_port_num_domain(unit, info->sid);

    /*
     * Need to change to physical port if domain is BCMDRD_PND_PHYS e.g. MIB
     * No Change in case domain is BCMDRD_PND_LOGIC e.g. IPIPE/EPIPE blocks
     * TBD: Conversion to MMU port
     */

    if (domain == BCMDRD_PND_PHYS) {
        pport = bcmpc_lport_to_pport(unit, dyn->tbl_inst);
        if (pport == BCMPC_INVALID_PPORT) {
            LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "LP[%d] not mapped\n"),
                              dyn->tbl_inst));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Transform LP[%d]--PP[%d]\n"),
                              dyn->tbl_inst, pport));
            dyn->tbl_inst = pport;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Clear of the counters in cache and HW for given Symbol.
 */
int
bcmptm_cci_cache_pt_req_all_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmdrd_sid_t sid;
    uint32_t id;
    uint32_t j;
    uint32_t *buff = NULL;
    int inst;
    size_t size;
    cci_ctr_cache_map_t *map = NULL;
    bcmptm_cci_cache_pt_param_t pt_param;
    bcmptm_cci_ctr_info_t info;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    sid = *(bcmdrd_sid_t *)param;

    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_ctr_info_get(unit, sid, &ptcache_ctr_info));

    id = ptcache_ctr_info.cci_map_id;
    map = &cache->ctr_map_table[id];
    /* Allocate the buffer */
    size = sizeof(uint32_t) * map->pt_entry_wsize;
    SHR_ALLOC(buff, size, "HwEntry");
    SHR_NULL_CHECK(buff, SHR_E_MEMORY);
    sal_memset(buff, 0, size);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "[%s], max-tlb_inst = %d, max-index =%u\n"),
                bcmdrd_pt_sid_to_name(unit, sid),
            map->tbl_inst,  map->index_max));
    for (inst = 0; inst < map->tbl_inst; inst++) {
        for (j = map->index_min; j < map->index_max; j++) {
            sal_memset(&info, 0, sizeof(info));
            info.dyn_info.index = j;
            info.dyn_info.tbl_inst = inst;
            info.sid = sid;
            info.lt_map_id = info.map_id = id;
            info.txfm_fld = -1;
            pt_param.buf = buff;
            pt_param.info = &info;
            /* Reset HW counter and cache */
            LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Clear[%s], tlb_inst = %d, index =%u\n"),
                   bcmdrd_pt_sid_to_name(unit, sid),
                   inst, j));
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_pt_req_passthru_write(unit, handle, &pt_param,
                                              update_cache));
        }
    }
    map->clr_on_read = bcmptm_pt_cci_ctr_evict_is_clr_on_read(unit, sid);



exit:
    if (buff) {
        SHR_FREE(buff);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_cache_pt_req_ser_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    cci_ctr_cache_t *cache = (cci_ctr_cache_t *)handle;
    bcmptm_cci_ctr_info_t *info;
    cci_ctr_cache_map_t *map = NULL;
    uint32_t *ptbuff = NULL, *cbuff = NULL;
    size_t ptsize, csize;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cache, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    info = (bcmptm_cci_ctr_info_t *)param;
    map = &cache->ctr_map_table[info->map_id];

    ptsize = sizeof(uint32_t) * map->pt_entry_wsize;
    SHR_ALLOC(ptbuff, ptsize, "HwEntry");
    SHR_NULL_CHECK(ptbuff, SHR_E_MEMORY);
    sal_memset(ptbuff, 0, ptsize);

    /* SCHAN PIO request to write physical entry */
    SHR_IF_ERR_EXIT(
        cci_pt_write(unit, info, ptbuff));

    if (update_cache) {
        csize = sizeof(uint64_t) * map->field_num;
        SHR_ALLOC(cbuff, csize, "CACHE BUFF");
        SHR_NULL_CHECK(cbuff, SHR_E_MEMORY);
        sal_memset(cbuff, 0, csize);
        SHR_IF_ERR_EXIT(
            bcmptm_cci_cache_write(unit, handle, info, cbuff));
        map->clr_on_ser = TRUE;
    }

exit:
    if (ptbuff) {
        SHR_FREE(ptbuff);
    }
    if (cbuff) {
        SHR_FREE(cbuff);
    }
    SHR_FUNC_EXIT();
}
