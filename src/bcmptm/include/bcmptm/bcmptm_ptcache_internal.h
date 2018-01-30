/*! \file bcmptm_ptcache_internal.h
 *
 * Utils, defines in ptcache
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

#ifndef BCMPTM_PTCACHE_INTERNAL_H
#define BCMPTM_PTCACHE_INTERNAL_H

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
/*! Specify 'get TCAM CORRUPT bit' operation.
 *  \n tc_cmd for use in bcmptm_ptcache_tc_cmd()
 */
#define BCMPTM_TCAM_CORRUPT_CMD_GET 0

/*! Specify 'set TCAM CORRUPT bit' operation.
 *  \n tc_cmd for use in bcmptm_ptcache_tc_cmd()
 */
#define BCMPTM_TCAM_CORRUPT_CMD_SET 1

/*! Specify 'clear TCAM CORRUPT bit' operation.
 *  \n tc_cmd for use in bcmptm_ptcache_tc_cmd()
 */
#define BCMPTM_TCAM_CORRUPT_CMD_CLR 2

/*! Identifies 'ctl' type tcam_info_word
 */
#define BCMPTM_TCAM_IW_TYPE_CW 0

/*! Identifies 'key' type tcam_info_word
 */
#define BCMPTM_TCAM_IW_TYPE_FI_KEY 1

/*! Identifies 'mask' type tcam_info_word
 */
#define BCMPTM_TCAM_IW_TYPE_FI_MASK 2

/*! Max value of 'minbit' field in tcam_info_word of type 'field'
 *  \n (encoded with 12 bits
 */
#define BCMPTM_TCAM_IW_LIMIT_MINBIT 4095

/*! Max value of 'width' field in tcam_info_word of type 'field'
 *  \n (encoded with 12 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_WIDTH 4095

/*! Max value of 'count' field in tcam_info_word of type 'ctl'
 *  \n (encoded with 8 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_COUNT 255

/*! value of LTID when entry is read from HW
 * - never written in cache */
#define BCMPTM_LTID_RSP_HW                             0xFFFFFFFF

/*! value of LTID when cache is not allocated for this PT
 * - never written in cache */
#define BCMPTM_LTID_RSP_NOCACHE                        0xFFFFFFFE

/*! value of LTID when LTID is not supported for this PT
 * - never written in cache */
#define BCMPTM_LTID_RSP_NOT_SUPPORTED                  0xFFFFFFFD

/*! value of LTID when entry is not valid in cache
 * - never written in cache */
#define BCMPTM_LTID_RSP_INVALID                        0xFFFFFFFC

/*! Reserved value of LTID
 * - never written in cache
 * - rsrc_mgr must consider values >= LTID_RSVD as illegal */
#define BCMPTM_LTID_RSVD                               BCMPTM_LTID_RSP_INVALID

/*! Reserved value of LTID - 16b version
 * - only used by ptcache */
#define BCMPTM_LTID_RSVD16                             (BCMPTM_LTID_RSVD & 0xFFFF)

/*! value of DFID when entry is read from HW
 * - never written in cache */
#define BCMPTM_DFID_RSP_HW                             0xFFFF

/*! value of DFID when cache is not allocated for this PT
 * - PTCACHE_TYPE is NO_CACHE or CCI_ONLY
 * - never written in cache */
#define BCMPTM_DFID_RSP_NOCACHE                        0 /* 0xFFFE */

/*! value of DFID when DFID is not supported for this PT
 * - dfid_seg_en = 0 for this PT or this PT is of type SE (eg: Register)
 * - never written in cache */
#define BCMPTM_DFID_RSP_NOT_SUPPORTED                  0 /* 0xFFFD */

/*! value of DFID when entry is not valid in cache
 * - never written in cache and is of type ME (multiple entries)  */
#define BCMPTM_DFID_RSP_INVALID                        0 /* 0xFFFC */

/*! value of DFID from modelled path
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_MREQ                               0xFFFB

/*! value of DFID from interactive path
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_IREQ                               0xFFFA

/*! value of DFID from modeled path - 8b version
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_MREQ8                              (BCMPTM_DFID_MREQ & 0xFF)

/*! value of DFID from interactive path - 8b version
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_IREQ8                              (BCMPTM_DFID_IREQ & 0xFF)


/*! Reserved value of DFID
 * - rsrc_mgr must consider values >= DFID_RSVD as illegal */
#define BCMPTM_DFID_RSVD                               BCMPTM_DFID_IREQ

/*! Reserved value of DFID - 8b version
 * - only used by ptcache */
#define BCMPTM_DFID_RSVD8                              (BCMPTM_DFID_IREQ & 0xFF)


/*******************************************************************************
 * Typedefs
 */
/*! Definition of bits in info_words for tcam type pt
 * \n Needed when converting (key, mask) to XY format and vice-versa.
 */
typedef union bcmptm_tcam_iw_fi_s {
    /* usage: store field info */

    /*! Format for field info word */
    struct fi_s {
        /*! type of field in this word */
        uint32_t type:3;

        /*! 1 => spare */
        uint32_t spare:5;

        /*! first bit for this field in entry */
        uint32_t minbit:12;

        /*! width of this field */
        uint32_t width:12;
    } fi; /*!< field info word */

    /*! Format for ctl info word */
    struct cw_s {
        /*! type of info in this word */
        uint32_t type:3;

        /*! 1 => spare */
        uint32_t spare:21;

        /*! num of words in info_array */
        uint32_t count:8;
    } cw; /*!< ctl info word */

    /*! Format for raw entry word */
    uint32_t entry;
} bcmptm_tcam_iw_fi_t;

/*! Information for counter mems from ptcache */
typedef struct bcmptm_ptcache_ctr_info_s {
    /*! TRUE if sid is counter */
    bool pt_is_ctr;

    /*!  TRUE if counter sid is CCI_ONLY */
    bool ctr_is_cci_only;

    /*! cci_map_id */
    uint32_t cci_map_id;
} bcmptm_ptcache_ctr_info_t;


/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Allocate and initialize PTcache for a unit
 * \n Must be called every time a new unit is attached and table sizes have been
 * determined, configured.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_init(int unit, bool warm);

/*!
 * \brief Deallocate PTcache for a unit
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_cleanup(int unit);

/*!
 * \brief Determine if PT is ctr, whether it is completely managed by CCI
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] ctr_info Refer to bcmptm_ptcache_ctr_info_t defn
 *
 * \retval SHR_E_NONE Success. May or may not be counter.
 */
extern int
bcmptm_ptcache_ctr_info_get(int unit,
                            bcmdrd_sid_t sid,
                            bcmptm_ptcache_ctr_info_t *ctr_info);

/*!
 * \brief Read a mem or reg from PTcache
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n If (tbl_inst < 0) data is returned from one of the instances
 * \param [in] ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] entry_count Number of consecutive entries to fetch.
 * rsp_cache_vbit is returned only for first entry in the list.
 * \n Must be 1 for tables for which have single entry / LTID / DFID or are
 * narrow overlay views of wider tables.
 * \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
 *
 * \param [out] rsp_entry_words Array large enough to hold read data
 * \param [out] rsp_entry_cache_vbit 'cache_vbit (vbit)' state of entry in cache
 * \param [out] rsp_entry_dfid dfield_format_id of entry in cache. Meaningful
 * only if rsp_entry_cache_vbit is TRUE.
 * \param [out] rsp_entry_ltid LTid of previous writer. Meaningful only if
 * rsp_entry_cache_vbit is TRUE.
 *
 * \retval SHR_E_NONE Success (entry may be valid or not valid in cache)
 * \retval SHR_E_UNAVAIL Cache is not allocated for this PT. rsp_entry_words are
 * not correct.
 */
extern int
bcmptm_ptcache_read(int unit,
                    bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    void *ovrr_info,
                    uint32_t entry_count,
                    size_t rsp_entry_wsize,
                    uint32_t *rsp_entry_words,
                    bool *rsp_entry_cache_vbit,
                    uint16_t *rsp_entry_dfid,
                    bcmltd_sid_t *rsp_entry_ltid);

/*!
 * \brief Write a mem or reg in PTcache
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n If (tbl_inst < 0) data is written to ALL instances
 * \param [in] ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] entry_count Number of consecutive entries to update.
 * set_cache_vbit applies to all consecutive entries. User must ensure it has
 * filled entry words correctly.
 * \n Must be 1 for tables for which have single entry / LTID / DFID or are
 * narrow overlay views of wider tables.
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] set_cache_vbit 1 => Make cache_vbit TRUE for this cache_entry
 *                         \n 0 => Make cache_vbit FALSE for this cache_entry
 * \param [in] req_ltid LTid of Current writer
 * \param [in] req_dfid Data field format id
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Cache is not allocated for this PT
 */
extern int
bcmptm_ptcache_update(int unit,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      void *ovrr_info,
                      uint32_t entry_count,
                      uint32_t *entry_words,
                      bool set_cache_vbit,
                      bcmltd_sid_t req_ltid,
                      uint16_t req_dfid);

/*!
 * \brief Write a mem or reg in PTcache on Interactive path
 * Will first read entry in cache. If entry is valid in cache, then it will
 * preserve LTID,DFID. If entry is not valid in cache, it will write req_ltid,
 * req_dfid in cache.
 * Is needed to allow modification of modeled SIDs on interactive path (for
 * debug).
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n If (tbl_inst < 0) data is written to ALL instances
 * \param [in] ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] set_cache_vbit 1 => Make cache_vbit TRUE for this cache_entry
 *                         \n 0 => Make cache_vbit FALSE for this cache_entry
 * \param [in] req_ltid LTid of Current writer
 * \param [in] req_dfid Data field format id
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Cache is not allocated for this PT
 */
extern int
bcmptm_ptcache_update_ireq(int unit,
                           bcmdrd_sid_t sid,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           void *ovrr_info,
                           uint32_t *entry_words,
                           bool set_cache_vbit,
                           bcmltd_sid_t req_ltid,
                           uint16_t req_dfid);

/*!
 * \brief Access TCAM CORRUPT BIT for a mem in PTcache
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n For Writes, (tbl_inst < 0) => bit is updated for ALL instances
 *             \n For Reads, (tbl_inst < 0) => bit is read from last instance
 * \param [in] ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] tc_cmd Refer to encoding of BCMPTM_TCAM_CORRUPT_CMD_XXX
 *
 * \param [out] prev_tc_rdata TCAM CORRUPT BIT data (before update, if any)
 *                            \n NonZero => 1, Zero => 0
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Cache or TCAM CORRUPT bits is not allocated for this PT
 */
extern int
bcmptm_ptcache_tc_cmd(int unit,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      void *ovrr_info,
                      uint32_t tc_cmd,
                      uint32_t *prev_tc_rdata);

/*!
 * \brief Fetch base_index for info_seg in ptcache
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \param [out] iw_base_index Absolute base_index for info_seg in ptcache.vinfo
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL info_words are not allocated for this sid
 */
extern int
bcmptm_ptcache_iw_base(int unit,
                       bcmdrd_sid_t sid,
                       uint32_t *iw_base_index);

/*!
 * \brief Read info_word from info_seg
 *
 * \param [in] unit Logical device id
 * \param [in] iw_index Index (absolute) into info_seg
 *
 * \param [out] iw_rdata Info word from info_seg
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_iw_read(int unit,
                       uint32_t iw_index,
                       uint32_t *iw_rdata);

/*!
 * \brief Write info_word to info_seg
 *
 * \param [in] unit Logical device id
 * \param [in] iw_index Index (absolute) into info_seg
 * \param [in] iw_wdata Info word to be written to info_seg
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_iw_write(int unit,
                        uint32_t iw_index,
                        uint32_t iw_wdata);


/*!
 * \brief Convert (key, mask) fields of TCAM table to HW specific XY format.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [in] entry_count Num of consecutive entries to converted
 * \param [in] km_entry Ptr to 1st entry in (key, mask) format
 *
 * \param [out] xy_entry Ptr to 1st entry in (x, y) format
 *              \n xy_entry can be same as km_entry (REPLACE)
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_pt_tcam_km_to_xy(int unit,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *km_entry,
                        uint32_t *xy_entry);

/*!
 * \brief Convert (key, mask) fields of TCAM from HW specific XY to (key, mask) format.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [in] entry_count Num of consecutive entries to converted
 * \param [in] xy_entry Ptr to 1st entry in (x, y) format
 *
 * \param [out] km_entry Ptr to 1st entry in (key, mask) format
 *              \n km_entry can be same as xy_entry (REPLACE)
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_pt_tcam_xy_to_km(int unit,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *xy_entry,
                        uint32_t *km_entry);


/*!
 * \brief Find info_word count required for a PT
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [out] iw_count Non-zero value means info_words needed.
 *
 * Returns:
 * \retval SHR_E_NONE
 */
extern int
bcmptm_pt_iw_count(int unit,
                   bcmdrd_sid_t sid, uint32_t *iw_count);

/*!
 * \brief Fill info_seg with info_words
 * \n
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [in] iw_base_index Absolute base_index for info_word_seg in ptcach.vinfo
 *
 * Returns:
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_pt_iw_fill(int unit,
                  bcmdrd_sid_t sid,
                  uint32_t iw_base_index);

#endif /* BCMPTM_PTCACHE_INTERNAL_H */
