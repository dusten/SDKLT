/*! \file bcmptm_ptcache.c
 *
 * Miscellaneous utilities for PTM
 *
 * This file implements PTcache (SW cache) for physical tables
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmdrd_new.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CACHE

/* #define PTCACHE_REGS_DISABLE */
    /* Note: regs protected by ser will still be cached */

#define PTCACHE_SENTINEL_ME_ENABLE
    /* Enable SENTINEL also for multi_entry_OR_cam_SIDs */

#define PTCACHE_SENTINEL_SE_ENABLE
    /* Enable SENTINEL also for single_entry_AND_non_cam_SIDs
     * For these type of SIDs, info is stored directly in varinfo_array without
     * any data_seg, valid_seg. Most of these SIDs are reg32, reg64, so Sentinel
     * will add 100% or 50% overhead respectively */

#define PTCACHE_VINFO_INDEX_HASH_WORD 0
    /* reserve word_0 in vinfo_array to store hash */
#define PTCACHE_VINFO_INDEX_FLAGS_WORD 1
    /* reserve word_1 in vinfo_array to store flags */
#define PTCACHE_VINFO_INDEX_SID_COUNT_WORD 2
    /* reserve word_2 in vinfo_array to store sid_count */
#define PTCACHE_VINFO_INDEX_CW_COUNT_WORD 3
    /* reserve word_3 in vinfo_array to store num of words in ctl section */
#define PTCACHE_VINFO_INDEX_TW_COUNT_WORD 4
    /* reserve word_4 in vinfo_array to store total num of words in vinfo */
#define PTCACHE_VINFO_INDEX_SIGN_WORD 5
    /* reserve word_5 in vinfo_array to store vinfo_signature word */
#define PTCACHE_INIT_VINFO_ARRAY_INDEX 6

#define PTCACHE_FLAGS_LTID_SIZE16 0x00000001

#define SINFO_PTR (ptcache_sinfo_ptr[unit])
#define VINFO_PTR (ptcache_vinfo_ptr[unit])
#define VINFO_DATA(i) *(VINFO_PTR + (i))
#define SINFO_DATA(i) *(SINFO_PTR + (i))

#define VINFO_INDEX_WIDTH 26
#define VINFO_INDEX_CTL_WORD_MAX (1 << VINFO_INDEX_WIDTH)
    /* Needed to ensure that sinfo_word will be able to store
     * index into ctl_words section
     * vinfo_index_ctl_words */

/* Defn for PTCACHE_TYPE in sinfo_word */
#define PTCACHE_TYPE_NO_CACHE 0
#define PTCACHE_TYPE_ME 1            /* multi entry - data_seg, valid_seg */
#define PTCACHE_TYPE_ME_CCI 2        /* counter which also has cfg fields and is
                                        therefore also cached */
#define PTCACHE_TYPE_SE_NOT_VALID 3  /* single entry, not_valid */
#define PTCACHE_TYPE_SE_VALID 4      /* single entry, valid */
#define PTCACHE_TYPE_CCI_ONLY 5      /* completely managed by CCI */

/* Max value of SID that can be stored in overlay_info word */
#define OVERLAY_INFO_SID_WIDTH 20 /* 1 M SIDs */
#define OVERLAY_INFO_SID_MAX (1 << OVERLAY_INFO_SID_WIDTH)
/* Number of narrow view entries in one widest view entry */
#define OVERLAY_INFO_NUM_NE_WIDTH 5 /* 32 NEs in 1 WE */
#define OVERLAY_INFO_NUM_NE_MAX (1 << OVERLAY_INFO_NUM_NE_WIDTH)
/* Number of wide_entry_fields needed to create narrow_entry */
#define OVERLAY_INFO_WEF_WIDTH 6 /* 64 wide_entry_fields in 1 narrow entry */
#define OVERLAY_INFO_NUM_WEF_MAX (1 << OVERLAY_INFO_WEF_WIDTH)

/* Max value of narrow_view_width that can be stored in overlay_info word */
#define OVERLAY_INFO_NE_WIDTH 15 /* 32K bits Narrow entry width */
#define OVERLAY_INFO_NE_WIDTH_MAX (1 << OVERLAY_INFO_NE_WIDTH)

/* Max value of start_bit that can be stored in overlay_info word */
#define OVERLAY_INFO_SB_WIDTH 15 /* 32K start_bit position */
#define OVERLAY_INFO_SB_MAX (1 << OVERLAY_INFO_SB_WIDTH)

/* wsize of overlay_info */
#define OVERLAY_INFO_WSIZE 2 /* num uint32_t words to store overlay info  */

#define XINFO_CW_OFFSET_VS_WIDTH 8
#define XINFO_CW_OFFSET_VS_MAX ((1 << XINFO_CW_OFFSET_VS_WIDTH) - 4)
/*
 * To support 1 (width, start_bit) words,   we need 2   xinfo_words and must set cw_offset_vs =2
 * To support 2 (width, start_bit) words,   we need 3   xinfo_words and must set cw_offset_vs =3
 * To support N (width, start_bit) words,   we need N+1 xinfo_words and must set cw_offset_vs = N+1
 * To support 254 (width, start_bit) words, we need 255 xinfo_words and must set cw_offset_vs = 255
 *
 * 254 = (2^8 - 2)
 *
 * I reserved more by using (2^8 - 4) just to be safe and
 * also illustrate how we can reserve xinfo_space for other uses
 * besides (width, start_bit) pairs
 */

#define TBL_INST_COUNT_SET(unit, sid)\
do {\
    int tinst_count;\
    tinst_count = bcmdrd_pt_num_tbl_inst(unit, sid);\
    SHR_IF_ERR_EXIT((tinst_count < 0) ? SHR_E_INTERNAL : SHR_E_NONE);\
    tbl_inst_count = tinst_count;\
} while (0)

#define TBL_INST_MAX_SET(unit, sid)\
do {\
    int tinst_max;\
    tinst_max = bcmdrd_pt_num_tbl_inst(unit, sid) - 1;\
    SHR_IF_ERR_EXIT((tinst_max < 0) ?\
                    SHR_E_INTERNAL : SHR_E_NONE);\
    tbl_inst_max = tinst_max;\
} while (0)

/*******************************************************************************
 * Typedefs
 */
/*! Definition of bits in sinfo_word.
 */
typedef union sinfo_s {
    struct {
        /*! index into vinfo array */
        uint32_t vinfo_cw_index:VINFO_INDEX_WIDTH;

        /*! 1 => cw_index points to extra info_word (and not ds_index_word)
         *  0 => no ltid_seg, dfid_seg, info_seg, tc_seg for this sid.
         *       vinfo_cw_index points to vs
         *       vinfo_cw_index+1 points to ds
         */
        uint32_t xinfo_en:1;

        /*! 0 => ignore index in pt_dyn_info */
        uint32_t index_en:1;

        /*! 0 => ignore tbl_inst in pt_dyn_info */
        uint32_t tbl_inst_en:1;

        /*! pt_cache_type */
        uint32_t ptcache_type:3;
    } f;
    uint32_t entry;
} sinfo_t;

/*! Definition of bits in xinfo_word.
 */
typedef union xinfo_s {
    struct {
        /*! 1 => data_seg avail */
        uint32_t data_seg_en:1;

        /*! 1 => info_seg avail */
        uint32_t info_seg_en:1;

        /*! 1 => tcam_corrupt_seg avail */
        uint32_t tc_seg_en:1;

        /*! 1,2 => dfid_seg avail */
        uint32_t dfid_seg_en:2;

        /*! 1 => ltid_seg avail */
        uint32_t ltid_seg_en:1;

        /*! non-zero => is overlay (narrow view) of another (wider view) sid and
         * implies there is no data_seg for this sid.
         * Immediately two next cw are overlay_info_words */
        uint32_t overlay_mode:3;

        /*! index into valid_seg vinfo array.
         *  This means we can have up to 255 additional ctl_words */
        uint32_t cw_offset_vs:XINFO_CW_OFFSET_VS_WIDTH; /* 8 */

        /*! reserved bits for future exp */
        uint32_t reserved:15;
    } fme; /* f for me type */
    uint32_t entry;
} xinfo_t;

/*! Definition of bits in overlay_info_word0
 */
typedef union ptcache_oinfo_w0_s {
    struct {
        /*! sid of widest view */
        uint32_t sid:OVERLAY_INFO_SID_WIDTH;

        /*! num of narrow_view_entries in one widest_view_entry */
        uint32_t num_ne:OVERLAY_INFO_NUM_NE_WIDTH;

        /*! num of wide_entry_fields in one narrow_entry */
        uint32_t num_wef:OVERLAY_INFO_WEF_WIDTH;

        /*! sid is of type alpm - needs special address handling */
        uint32_t addr_mode_alpm:1;
    } f;
    uint32_t entry;
} ptcache_oinfo_w0_t;

/*! Definition of bits in overlay_info_word1
 */
typedef union ptcache_oinfo_wn_s {
    struct {
        /*! Bit-width of overlay (narrow) view
         *  For Hash type overlay, oinfo_w1.width - represents width_ne
         *  For Tcam type overlay, oinfo_w1.width - represents width of
         *  wide_entry_field */
        uint32_t width:OVERLAY_INFO_NE_WIDTH;

        /*! Start Bit of NE in WE
         *  Meaningful only for Tcam type overlay. Specifies start position of
         *  field in wide entry */
        uint32_t start_bit:OVERLAY_INFO_SB_WIDTH;
    } f;
    uint32_t entry;
} ptcache_oinfo_wn_t;


/*******************************************************************************
 * Private variables
 */
static uint32_t *ptcache_sinfo_ptr[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t *ptcache_vinfo_ptr[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Detect reg that is protected by SER
 */
#if defined(PTCACHE_REGS_DISABLE)
static bool
ptcache_pt_is_reg_with_ser(unit, sid)
{
    bool is_reg;
    bcmdrd_ser_resp_t ser_resp;

    is_reg = bcmdrd_pt_is_reg(unit, sid);
    ser_resp = bcmptm_pt_ser_resp(unit, sid);
    return (is_reg &&
            (ser_resp == BCMDRD_SER_RESP_CACHE_RESTORE) &&
            (ser_resp == BCMDRD_SER_RESP_ECC_CORRECTABLE) &&
            (ser_resp == BCMDRD_SER_RESP_WRITE_CACHE_RESTORE)
           );
}
#endif /* PTCACHE_REGS_DISABLE */

/*!
 * \brief Compute the sentinel word for sid
 * \n - one sentinel word is allocated for every data_seg, valid_seg,
 * tcam_corrupt_seg
 * \n - sentinel words are stored in vinfo array just before the word0 of
 * data_seg, valid_seg, tcam_corrupt_seg
 * \n - one sentinel word is allocated for each SE type PT and is stored just
 * before the data word0 of SE
 * \n - sentinel word will be unique for each segment of PT
 */
static uint32_t
ptcache_sentinel_calc(int unit, bcmdrd_sid_t sid, uint32_t offset)
{
    return (sid << unit) + unit + offset;
    /* Purpose of offset is to create unique sentinel word for each segment of
     * same sid.
     * Similarly, using 'unit' helps uniquify sentinel word for same sid on
     * different units
     */
}

/*!
 * \brief Verify the sentinel word for sid
 */
static int
ptcache_sentinel_check(int unit, bcmdrd_sid_t sid, uint32_t offset,
                       const char *sentinel_type)
{
    uint32_t exp_sentinel, obs_sentinel;
    const char *pt_name = "INVALIDm";

    pt_name = bcmdrd_pt_sid_to_name(unit, sid);
    exp_sentinel = ptcache_sentinel_calc(unit, sid, offset);
    obs_sentinel = VINFO_DATA(offset);

    if (exp_sentinel != obs_sentinel) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Sentinel %s Mismatch for sid=%0d, pt=%s, "
                        "exp_sentinel=0x%8x, obs_sentinel=0x%8x, "
                        "vinfo_sw_index=%0d\n"),
             sentinel_type, sid, pt_name, exp_sentinel, obs_sentinel,
             offset));
        return SHR_E_FAIL;
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Sentinel %s ok for sid=%0d, pt=%s, "
                        "exp_sentinel=0x%8x, obs_sentinel=0x%8x, "
                        "vinfo_sw_index=%0d\n"),
             sentinel_type, sid, pt_name, exp_sentinel, obs_sentinel,
             offset));
        return SHR_E_NONE;
    }
}

/*!
 * \brief find overlay info for a given sid
 */
static int
ptcache_overlay_info_set(int unit, bcmdrd_sid_t sid,
                         const bcmptm_overlay_info_t *overlay_info,
                         uint32_t vinfo_cw_index,
                         uint32_t *overlay_num_cw)
{
    ptcache_oinfo_w0_t oinfo_w0;
    ptcache_oinfo_wn_t oinfo_w1;
    ptcache_oinfo_wn_t oinfo_wn;
    bcmptm_field_info_raw_t *wef_array;
    uint32_t i = 0, j = 0;
    SHR_FUNC_ENTER(unit);

    assert(overlay_num_cw != NULL);

    SHR_IF_ERR_EXIT((overlay_info->w_sid >= OVERLAY_INFO_SID_MAX)?
                    SHR_E_INTERNAL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((overlay_info->num_ne >= OVERLAY_INFO_NUM_NE_MAX)?
                    SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_EXIT((overlay_info->num_we_field >= OVERLAY_INFO_NUM_WEF_MAX)?
                    SHR_E_PARAM : SHR_E_NONE);

    SHR_IF_ERR_EXIT((overlay_info->width_ne >= OVERLAY_INFO_NE_WIDTH_MAX)?
                    SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_EXIT((overlay_info->start_bit_ne_in_we >= OVERLAY_INFO_SB_MAX)?
                    SHR_E_PARAM : SHR_E_NONE);

    oinfo_w0.entry = 0;
    oinfo_w0.f.sid = overlay_info->w_sid;
    oinfo_w0.f.num_ne = overlay_info->num_ne;
    oinfo_w0.f.num_wef = overlay_info->num_we_field;
    oinfo_w0.f.addr_mode_alpm = (overlay_info->is_alpm? 1 : 0);
    VINFO_DATA(vinfo_cw_index + i) = oinfo_w0.entry; /* @ i = 0 */
    i++;

    oinfo_w1.entry = 0;
    oinfo_w1.f.width = overlay_info->width_ne;
    oinfo_w1.f.start_bit = overlay_info->start_bit_ne_in_we;
    VINFO_DATA(vinfo_cw_index + i) = oinfo_w1.entry; /* @ i = 1 */
    i++;

    if (overlay_info->num_we_field <= 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    wef_array = overlay_info->we_field_array;
    SHR_NULL_CHECK(wef_array, SHR_E_INTERNAL);
    for(j = 0; (j + 1) < overlay_info->num_we_field; j++) {
        SHR_IF_ERR_EXIT((wef_array[j].width >= OVERLAY_INFO_NE_WIDTH_MAX)?
                        SHR_E_PARAM : SHR_E_NONE);
        SHR_IF_ERR_EXIT((wef_array[j].start_bit >= OVERLAY_INFO_SB_MAX)?
                        SHR_E_PARAM : SHR_E_NONE);
        oinfo_wn.entry = 0;
        oinfo_wn.f.width = wef_array[j].width;
        oinfo_wn.f.start_bit = wef_array[j].start_bit;

        VINFO_DATA(vinfo_cw_index + i) = oinfo_wn.entry; /* @ i = 2 + j */
        i++;

        /* Overlay cw0, cw1 are already written
         *
         * num_we_field   jay     i = 2+j (overlay cw)
         *     2          0       2
         *     3          0,1     2,3
         *     4          0,1,2   2,3,4
         *     ...        ...     ...
         */
    } /* write info for remaining fields */

exit:
    *overlay_num_cw = i; /* total num of overlay cw that were written */
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find narrow entry details from wide entry.
 * - For given index of narrow entry, find index of corresponding wide_entry
 * - Find start_bit, end_bit for narrow entry in wide entry
 * - Common to both ptcache_read and ptcache_update functions
 */
static int
ptcache_w_entry_calc(int unit, ptcache_oinfo_w0_t *oinfo_w0,
                     ptcache_oinfo_wn_t *oinfo_w1,
                     uint32_t tbl_inst, uint32_t n_index,
                     uint32_t w_entry_wsize,
                     uint32_t *w_index, /* out */
                     uint32_t *w_sbit, uint32_t *w_ebit, /* out */
                     uint32_t *w_vinfo_ds_index)    /* in, out */
{
    uint32_t w_pt_index_count, w_sub_index, w_entry_num;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(w_index, SHR_E_PARAM);

    w_pt_index_count = bcmptm_pt_index_count(unit, oinfo_w0->f.sid);

    /* For given n_index within table instance of narrow view,
     * find within table instance of wide view,
     * a. row (w_index) in wide view, and
     * b. sub_index (w_sub_index) within that row of wide view */
    if (oinfo_w0->f.addr_mode_alpm) {
        *w_index = n_index & (w_pt_index_count - 1);
        SHR_IF_ERR_EXIT((*w_index >= w_pt_index_count)? SHR_E_INTERNAL :
                                                        SHR_E_NONE);
        SHR_IF_ERR_EXIT(w_pt_index_count <= 0? SHR_E_INTERNAL : SHR_E_NONE);
        w_sub_index = n_index / w_pt_index_count;
        SHR_IF_ERR_EXIT((w_sub_index >= oinfo_w0->f.num_ne)? SHR_E_INTERNAL
                                                           : SHR_E_NONE);
    } else { /* non_alpm */
        *w_index = n_index / oinfo_w0->f.num_ne;
        SHR_IF_ERR_EXIT((*w_index >= w_pt_index_count)? SHR_E_INTERNAL
                                                      : SHR_E_NONE);
        w_sub_index = n_index % oinfo_w0->f.num_ne;
        /* Example:
         * num_ne = 1 :there are 1 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0          0          0
         * 1          1          0
         * ....       ....       ....
         *
         * num_ne = 2 :there are 2 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0,1        0          0,1
         * 2,3        1          0,1
         * ....       ....       ....
         *
         * num_ne = 3 :there are 3 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0,1,2      0          0,1,2
         * 3,4,5      1          0,1,2
         * ....       ....       ....
         *
         * num_ne = 4 :there are 4 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0,1,2,3    0          0,1,2,3
         * 4,5,6,7    1          0,1,2,3
         * ....       ....       ....
         */
    }

    w_entry_num = (tbl_inst * w_pt_index_count) + (*w_index);

    if (w_vinfo_ds_index != NULL) {
        *w_vinfo_ds_index += w_entry_num * w_entry_wsize;
        /* points to 1st uint32_t word of wide entry */
    }

    if ((w_sbit != NULL) && (w_ebit != NULL)) {
        *w_sbit = oinfo_w1->f.width * w_sub_index;
        /* Example, assume a 420b entry:
         * width    w_sub_index     sbit
         * 70       0,1,2,3,4,5     0,70,140,210,280,350
         * 105      0,1,2,3         0,105,210,315
         * 140      0,1,2           0,140, 280
         * 210      0,1             0,210
         */
        *w_ebit = *w_sbit + oinfo_w1->f.width - 1;
        /* Above ASSUMES that HW always concatenates narrow view entries in one wide
         * view entry - WITHOUT ANY HIT_BITs/ECC_BITs in between */
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Read narrow entry from wide entry for TCAM_DATA type overlay
 * - This is overlay type where we need to assemble narrow entry from one or
 * more fields in wide entry
 */
static int
ptcache_overlay_tcam_data_read(int unit,
                               ptcache_oinfo_w0_t *oinfo_w0,
                               uint32_t vinfo_cw_index,
                               uint32_t tbl_inst,
                               uint32_t n_index,
                               uint32_t w_entry_wsize,

                               uint32_t *rsp_entry_words,  /* out */
                               uint32_t *w_vinfo_ds_index) /* in, out */
{
    uint32_t w_pt_index_count, w_index, w_entry_num;
    uint32_t w_sbit, w_ebit, n_sbit, n_ebit, i;
    ptcache_oinfo_wn_t oinfo_wn;
    uint32_t w_field_buf[BCMPTM_MAX_PT_FIELD_WORDS];
    SHR_FUNC_ENTER(unit);

    assert(oinfo_w0 != NULL);
    /* assert(rsp_entry_words != NULL); ok to be NULL */
    assert(w_vinfo_ds_index != NULL);

    w_pt_index_count = bcmptm_pt_index_count(unit, oinfo_w0->f.sid);
    w_index = n_index; /* for tcam_data overlays */
    SHR_IF_ERR_EXIT((w_index >= w_pt_index_count)? SHR_E_INTERNAL
                                                 : SHR_E_NONE);
    w_entry_num = (tbl_inst * w_pt_index_count) + w_index;
    *w_vinfo_ds_index += w_entry_num * w_entry_wsize;
        /* points to 1st uint32_t word of wide entry from which we need to
         * extract different fields to construct narrow entry */

    /* if caller does not need rsp data, exit */
    if (rsp_entry_words == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Following assumes:
     * - Wide_entry_fields can be non-consecutive
     * - Narrow_entry_fields are filled consecutively
     * - Order of Wide_entry_field info in narrow view's overlay_info cw must be
     *   such that we can fill narrow_entry_fields consecutively.
     *   This is why we dont need to store start_bit for narrow_entry_fields.
     *
     * - vinfo_cw_index is already pointing to cw which holds first (start,
     * width) field info
     */
    n_sbit = 0;
    for (i = 0; i < oinfo_w0->f.num_wef; i++) {
        oinfo_wn.entry = VINFO_DATA(vinfo_cw_index + i);

        /* sal_memset(w_field_buf, 0, sizeof(w_field_buf)); */

        /* Copy wide_entry_field into tmp field buffer */
        w_sbit = oinfo_wn.f.start_bit;
        w_ebit = w_sbit + oinfo_wn.f.width - 1;
        bcmdrd_field_get(VINFO_PTR + (*w_vinfo_ds_index), /* w_entry_buf */
                         w_sbit, w_ebit, w_field_buf);

        /* Copy field from tmp field buffer to 'right place' in rsp_entry_words */
        n_ebit = n_sbit + oinfo_wn.f.width - 1;
        bcmdrd_field_set(rsp_entry_words, n_sbit, n_ebit, w_field_buf);
        n_sbit = n_ebit + 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write narrow entry into wide entry for TCAM_DATA type overlay
 */
static int
ptcache_overlay_tcam_data_write(int unit,
                                ptcache_oinfo_w0_t *oinfo_w0,
                                uint32_t vinfo_cw_index,
                                uint32_t tbl_inst,
                                uint32_t n_index,
                                uint32_t w_entry_wsize,

                                uint32_t *req_entry_words,  /* out */
                                uint32_t *w_vinfo_ds_index) /* in, out */
{
    uint32_t w_pt_index_count, w_index, w_entry_num;
    uint32_t w_sbit, w_ebit, n_sbit, n_ebit, i;
    ptcache_oinfo_wn_t oinfo_wn;
    uint32_t n_field_buf[BCMPTM_MAX_PT_FIELD_WORDS];
    SHR_FUNC_ENTER(unit);

    assert(oinfo_w0 != NULL);
    /* assert(req_entry_words != NULL); ok for req_entry_words to be null */
    assert(w_vinfo_ds_index != NULL);

    w_pt_index_count = bcmptm_pt_index_count(unit, oinfo_w0->f.sid);
    w_index = n_index; /* for tcam_data overlays */
    SHR_IF_ERR_EXIT((w_index >= w_pt_index_count)? SHR_E_INTERNAL
                                                 : SHR_E_NONE);
    w_entry_num = (tbl_inst * w_pt_index_count) + w_index;
    *w_vinfo_ds_index += w_entry_num * w_entry_wsize;
        /* points to 1st uint32_t word of wide entry where we need to
         * assemble narrow_entry_fields to construct wide entry */

    /* if caller does not want write of entry_words, exit */
    if (req_entry_words == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Following assumes:
     * - Narrow_entry_fields are extracted consecutively
     * - Wide_entry_fields can be non-consecutive
     * - Order of Wide_entry_field info in narrow view's overlay_info cw must be
     *   such that we can extract narrow_entry_fields consecutively.
     *   This is why we dont need to store start_bit for narrow_entry_fields.
     *
     * - vinfo_cw_index is already pointing to cw which holds first (start,
     * width) field_info.
     */
    n_sbit = 0;
    for (i = 0; i < oinfo_w0->f.num_wef; i++) {
        oinfo_wn.entry = VINFO_DATA(vinfo_cw_index + i);

        /* sal_memset(n_field_buf, 0, sizeof(n_field_buf)); */

        /* Extract n_field from n_entry into n_field buffer */
        n_ebit = n_sbit + oinfo_wn.f.width - 1;
        bcmdrd_field_get(req_entry_words, /* n_entry_buf */
                         n_sbit, n_ebit, n_field_buf);
        n_sbit = n_ebit + 1;


        /* Copy n_field into 'right place' in w_entry */
        w_sbit = oinfo_wn.f.start_bit;
        w_ebit = w_sbit + oinfo_wn.f.width - 1;
        bcmdrd_field_set(VINFO_PTR + (*w_vinfo_ds_index), /* w_entry_buf */
                         w_sbit, w_ebit, n_field_buf);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Is size of ltid 16 bits or 32 bits
 */
static int
ptcache_ltid_size_get(int unit, bool *ltid_size16b)
{
    size_t ltid_count;
    SHR_FUNC_ENTER(unit);

    ltid_count = BCMLTD_TABLE_COUNT;
    /* There may be 65K tables with LTIDs 0-(65K-1) in multi-unit system.
     * Unit may support only 1K tables but with LTIDs ranging from 64K - (65K-1)
     * BCMLTD_TABLE_COUNT will be 65K in this case and bcmlrd_table_count_get()
     * will be 1K for this unit. We will need 32 bits to store LTIDs for this
     * unit.
     */

    

    assert(ltid_size16b != NULL);
    *ltid_size16b = FALSE; /* default - use 32b ltid */

    if (ltid_count < ((1 << 16) - 8)) {
        /* leave few (8) spots for things like BCMPTM_LTID_RSVD, etc */
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Number of ltid = 0x%0x, ptcache will use 16b LTID\n"),
             (uint32_t)ltid_count));
        *ltid_size16b = TRUE; /* can use 16b ltid */
    } else {
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Number of ltid = 0x%0x, ptcache will use 32b LTID\n"),
             (uint32_t)ltid_count));
    }
/* exit: */
    SHR_FUNC_EXIT();
}

/*!
 * \brief ltid_get helper function for ptcache_read()
 */
static int
ptcache_ltid_get(int unit,
                 bcmdrd_sid_t sid,
                 bool ltid_size16b,
                 uint32_t entry_num,
                 uint32_t vinfo_ltid_seg_index,
                 bcmltd_sid_t *rsp_entry_ltid)
{
    uint32_t tmp_word, ltid_seg_index;
    SHR_FUNC_ENTER(unit);

    assert(rsp_entry_ltid != NULL);

    ltid_seg_index = vinfo_ltid_seg_index;
    if (ltid_size16b) { /* 16b ltid */
        ltid_seg_index += entry_num/2;
        tmp_word = VINFO_DATA(ltid_seg_index);
        if (entry_num % 2) {
            *rsp_entry_ltid = tmp_word >> 16;    /* upper 16b */
        } else {
            *rsp_entry_ltid = tmp_word & 0xFFFF; /* lower 16b */
        }
        SHR_IF_ERR_EXIT((*rsp_entry_ltid >= BCMPTM_LTID_RSVD16)?
                        SHR_E_INTERNAL : SHR_E_NONE);
    } else { /* 32b ltid */
        ltid_seg_index += entry_num;
        *rsp_entry_ltid = VINFO_DATA(ltid_seg_index);
        SHR_IF_ERR_EXIT((*rsp_entry_ltid >= BCMPTM_LTID_RSVD)?
                        SHR_E_INTERNAL : SHR_E_NONE);
    } /* 32b ltid */
    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "sid = %0d, vinfo_ltid_seg_index=%0d\n"),
         sid, ltid_seg_index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief dfid_get helper function for ptcache_read()
 */
static int
ptcache_dfid_get(int unit,
                 bcmdrd_sid_t sid,
                 uint8_t dfid_bytes,
                 uint32_t entry_num,
                 uint32_t vinfo_dfid_seg_index,
                 uint16_t *rsp_entry_dfid)
{
    uint32_t tmp_word, dfid_seg_index;
    SHR_FUNC_ENTER(unit);

    assert(dfid_bytes != 0);
    assert(dfid_bytes != 3);
    assert(rsp_entry_dfid != NULL);

    dfid_seg_index = vinfo_dfid_seg_index;

    if (dfid_bytes == 2) { /* 16b dfid */
        dfid_seg_index += entry_num/2;
        tmp_word = VINFO_DATA(dfid_seg_index);
        if (entry_num % 2) {
            *rsp_entry_dfid = tmp_word >> 16;    /* upper 16b */
        } else {
            *rsp_entry_dfid = tmp_word & 0xFFFF; /* lower 16b */
        }
        SHR_IF_ERR_EXIT((*rsp_entry_dfid > BCMPTM_DFID_MREQ)?
                        SHR_E_INTERNAL : SHR_E_NONE);
        /* usage of > in above allows DFID_IREQ, MREQ to be read
         * from cache */
    } else { /* 8b dfid */
        dfid_seg_index += entry_num/4;
        tmp_word = VINFO_DATA(dfid_seg_index);
        switch (entry_num % 4) {
            case 0: /* [7:0] */
                *rsp_entry_dfid = tmp_word & 0xFF;
                break;
            case 1: /* [15:8] */
                *rsp_entry_dfid = (tmp_word >> 8) & 0xFF;
                break;
            case 2: /* [23:16] */
                *rsp_entry_dfid = (tmp_word >> 16) & 0xFF;
                break;
            default: /* [31:24] */
                *rsp_entry_dfid = (tmp_word >> 24) & 0xFF;
                break;
        }
        SHR_IF_ERR_EXIT((*rsp_entry_dfid > BCMPTM_DFID_MREQ8)?
                        SHR_E_INTERNAL : SHR_E_NONE);
        /* usage of > in above allows DFID_IREQ, MREQ to be read
         * from cache */
        if (*rsp_entry_dfid == BCMPTM_DFID_IREQ8) {
            *rsp_entry_dfid = BCMPTM_DFID_IREQ;
        } else if (*rsp_entry_dfid == BCMPTM_DFID_MREQ8) {
            *rsp_entry_dfid = BCMPTM_DFID_MREQ;
        }
    } /* 8b dfid */
    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "dfid_seg_index=%0d\n"),
         dfid_seg_index));
    if (*rsp_entry_dfid == BCMPTM_DFID_IREQ) {
        LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Reserved Data Format ID (interactive) was read "
                        "for sid=%0d !!\n"),
             sid));
    } else if (*rsp_entry_dfid == BCMPTM_DFID_MREQ) {
        LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Reserved Data Format ID (modeled) was read "
                        "for sid=%0d !!\n"),
             sid));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ltid_set helper function for ptcache_update()
 */
static int
ptcache_ltid_set(int unit,
                 bcmdrd_sid_t sid,
                 bool ltid_size16b,
                 uint32_t entry_num,
                 uint32_t vinfo_ltid_seg_index,
                 bcmltd_sid_t req_ltid)
{
    uint32_t tmp_word, ltid_seg_index, req_ltid_chk;
    SHR_FUNC_ENTER(unit);
    ltid_seg_index = vinfo_ltid_seg_index;

                    if (ltid_size16b) { /* 16b ltid */
                        SHR_IF_ERR_EXIT((req_ltid > 0xFFFF)? /* range chk */
                                        SHR_E_INTERNAL : SHR_E_NONE);
                        req_ltid_chk = req_ltid | 0xFFFF0000; /* rsvd_val chk */
                        SHR_IF_ERR_EXIT((req_ltid_chk >= BCMPTM_LTID_RSVD)?
                                        SHR_E_INTERNAL : SHR_E_NONE);
                        /* Now, we know that req_ltid is within 16b range for
                         * sure, so no need to use (req_ltid & 0xFFFF) */
                        ltid_seg_index += entry_num/2;
                        if (entry_num % 2) { /* 31:16 */
                            tmp_word = (VINFO_DATA(ltid_seg_index) &
                                        0x0000FFFF) | (req_ltid << 16);
                        } else { /* 15:0 */
                            tmp_word = (VINFO_DATA(ltid_seg_index) &
                                        0xFFFF0000) | req_ltid;
                        }
                    } else { /* 32b ltid */
                        SHR_IF_ERR_EXIT((req_ltid >= BCMPTM_LTID_RSVD)?
                                        SHR_E_INTERNAL : SHR_E_NONE);
                        ltid_seg_index += entry_num;
                        tmp_word = req_ltid;
                    }
                    VINFO_DATA(ltid_seg_index) = tmp_word;

                    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "ltid_seg_index=%0d\n"),
                         ltid_seg_index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief dfid_set helper function for ptcache_update()
 */
static int
ptcache_dfid_set(int unit,
                 bcmdrd_sid_t sid,
                 uint8_t dfid_bytes,
                 uint32_t entry_num,
                 uint32_t vinfo_dfid_seg_index,
                 uint16_t req_dfid)
{
    uint32_t tmp_word, dfid_seg_index;
    SHR_FUNC_ENTER(unit);

    assert(dfid_bytes != 0);
    assert(dfid_bytes != 3);

    dfid_seg_index = vinfo_dfid_seg_index;

    if (req_dfid == BCMPTM_DFID_IREQ) {
        LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Reserved Data Format ID is being written from "
                        "Interactive path for sid=%0d !!\n"),
             sid));
    } else if (req_dfid == BCMPTM_DFID_MREQ) {
        LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Reserved Data Format ID is being written from "
                        "Modeled path for sid=%0d !!\n"),
             sid));
    }
    if (dfid_bytes == 2) { /* 16b dfid */
        SHR_IF_ERR_EXIT((req_dfid > BCMPTM_DFID_MREQ)?
                        SHR_E_INTERNAL : SHR_E_NONE);
        /* usage of > in above allows DFID_IREQ, MREQ to be
         * written in cache */
        dfid_seg_index += entry_num/2;
        if (entry_num % 2) { /* 31:16 */
            tmp_word = (VINFO_DATA(dfid_seg_index) &
                        0x0000FFFF) | (req_dfid << 16);
        } else { /* 15:0 */
            tmp_word = (VINFO_DATA(dfid_seg_index) &
                        0xFFFF0000) | req_dfid;
        }
    } else { /* 8b dfid */
        uint16_t req_dfid_chk, req_dfid2;
        if ((req_dfid != BCMPTM_DFID_IREQ) &&
            (req_dfid != BCMPTM_DFID_MREQ)) {
            SHR_IF_ERR_EXIT((req_dfid > 0xFF)? /* range chk */
                            SHR_E_INTERNAL : SHR_E_NONE);
            req_dfid_chk = req_dfid | 0xFF00; /* rsvd_val chk */
            SHR_IF_ERR_EXIT((req_dfid_chk > BCMPTM_DFID_MREQ)?
                            SHR_E_INTERNAL : SHR_E_NONE);
            /* usage of > in above allows DFID_IREQ, MREQ to be
             * written in cache */
            req_dfid2 = req_dfid;
        } else {
            req_dfid2 = req_dfid & 0xFF;
        }
        /* Now, we know that req_dfid is within 8b range for
         * sure, so no need to use (req_dfid2 & 0xFF) */
        dfid_seg_index += entry_num/4;
        switch (entry_num % 4) {
            case 0: /* [7:0] */
                tmp_word = (VINFO_DATA(dfid_seg_index) &
                            0xFFFFFF00) | req_dfid2;
                break;
            case 1: /* [15:8] */
                tmp_word = (VINFO_DATA(dfid_seg_index) &
                            0xFFFF00FF) | (req_dfid2 << 8);
                break;
            case 2: /* [23:16] */
                tmp_word = (VINFO_DATA(dfid_seg_index) &
                           0xFF00FFFF) | (req_dfid2 << 16);
                break;
            default: /* [31:24] */
                tmp_word = (VINFO_DATA(dfid_seg_index) &
                           0x00FFFFFF) | (req_dfid2 << 24);
                break;
        }
    }
    VINFO_DATA(dfid_seg_index) = tmp_word;

    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "dfid_seg_index=%0d\n"),
         dfid_seg_index));

exit:
    SHR_FUNC_EXIT();
}

static int
ptcache_sid_chk(int unit, bcmdrd_sid_t sid)
{
    uint32_t sid_count;
    if (SINFO_PTR == NULL || VINFO_PTR == NULL) {
        return SHR_E_UNAVAIL;
    }
    sid_count = VINFO_DATA(PTCACHE_VINFO_INDEX_SID_COUNT_WORD);
    if (sid >= sid_count) {
        return SHR_E_INTERNAL;
    } else {
        return SHR_E_NONE;
    }
}

static int
fill_null_entry(int unit, bcmdrd_sid_t sid, uint32_t vinfo_dw_index)
{
    uint32_t i, j, entry_wsize, pt_entry_count, tbl_inst_count;
    const uint32_t *null_entryp;
    SHR_FUNC_ENTER(unit);

    null_entryp = bcmdrd_pt_default_value_get(unit, sid);
    entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
    if (null_entryp == NULL) {
        return SHR_E_INTERNAL;
    }
    entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
    pt_entry_count = bcmptm_pt_index_count(unit, sid);
    TBL_INST_COUNT_SET(unit, sid);

    for (i = 0; i < tbl_inst_count; i++) {
        for (j = 0; j < pt_entry_count; j++) {
            sal_memcpy(VINFO_PTR + vinfo_dw_index,
                       null_entryp, 4 * entry_wsize);
            vinfo_dw_index += entry_wsize;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
cache_vbit_update(int unit, bool set_cache_vbit,
                  uint32_t vinfo_cw_index_vs,
                  uint32_t f_vinfo_vs_index,
                  uint32_t f_entry_num,
                  uint32_t entry_count)
{
    uint32_t tmp_word, f_entry_bit, l_entry_num, l_entry_bit,
             vinfo_vs_index_base, l_vinfo_vs_index, vinfo_vs_index_count;

    if (entry_count > 1) {
        f_entry_bit = f_entry_num % 32;

        l_entry_num = f_entry_num + entry_count - 1;

        vinfo_vs_index_base = VINFO_DATA(vinfo_cw_index_vs);
            /* LHS points to 1st word in valid_seg */
        l_vinfo_vs_index = vinfo_vs_index_base + (l_entry_num / 32);

        l_entry_bit = l_entry_num % 32;

        vinfo_vs_index_count = l_vinfo_vs_index - f_vinfo_vs_index + 1;

        if (vinfo_vs_index_count == 1) {
            int n;
            /* Set bits [l:f] in tmp_word */
            n = l_entry_bit - f_entry_bit + 1; /* n will be >= 1 */
            assert(n > 0); /* n must be >= 1 */
            if (n < 32) { /* left-shift of 32 bit var by 32 is unpredictable */
                tmp_word = (1 << n) - 1; /* set n LSBs */
                tmp_word = tmp_word << f_entry_bit; /* insert 'f' zeros at
                                                       bits (f-1):0 */
            } else {
                tmp_word = 0xFFFFFFFF;
            }

            if (set_cache_vbit) {
                VINFO_DATA(f_vinfo_vs_index) |= tmp_word;
            } else {
                VINFO_DATA(f_vinfo_vs_index) &= ~tmp_word;
            }
        } else { /* multiple vinfo_vs words */
            /* First vinfo_vs entry */
            /* Need to set/clr bits [31:f_entry_bit] */
            tmp_word = 0xFFFFFFFF << f_entry_bit;
            if (set_cache_vbit) {
                VINFO_DATA(f_vinfo_vs_index) |= tmp_word;
            } else {
                VINFO_DATA(f_vinfo_vs_index) &= ~tmp_word;
            }

            /* Intermediate vinfo_vs entries */
            if (vinfo_vs_index_count > 2) {
                sal_memset(VINFO_PTR + f_vinfo_vs_index + 1,
                           set_cache_vbit ? 0xFF : 0,
                           (vinfo_vs_index_count - 2) * 4);
            }

            /* Last entry */
            /* Need to set/clr bits [l_entry_bit:0] */
            tmp_word = 0xFFFFFFFF << l_entry_bit;
            tmp_word = tmp_word << 1;
            /* As per Coverity Left shift of 32 bit var by 32 can lead
             * to unpredictable results */
            if (set_cache_vbit) {
                VINFO_DATA(l_vinfo_vs_index) |= ~tmp_word;
            } else {
                VINFO_DATA(l_vinfo_vs_index) &= tmp_word;
            }
        } /* multiple vinfo_vs words */
    } else { /* entry_count = 1 */
        if (set_cache_vbit) {
            BCMPTM_WORD_BMAP_SET(VINFO_DATA(f_vinfo_vs_index), f_entry_num);
        } else { /* clear cache_vbit */
            BCMPTM_WORD_BMAP_CLR(VINFO_DATA(f_vinfo_vs_index), f_entry_num);
        }
    }
}

static int
ptcache_warm_check(int unit, uint32_t *ref_sinfo_sign, uint32_t *ref_vinfo_sign,
                   uint32_t *ptcache_flags_word, uint32_t *drd_hash_word,
                   size_t *sid_count, uint32_t *sinfo_base_ptr)
{
    uint32_t tmp_w, s_cache_word_count, vinfo_req_size, vinfo_alloc_size;
    SHR_FUNC_ENTER(unit);

    /* Jingle */
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Warmboot: for PTcache\n")));

    /* Check HA signature for SINFO array */
    tmp_w = *sinfo_base_ptr;
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != *ref_sinfo_sign ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(unit, "WB: PTcache.SINFO array signature mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         *ref_sinfo_sign, tmp_w));

    /* Get vinfo_req_size */
    s_cache_word_count = *(sinfo_base_ptr + 1);

    /* Get new VINFO_PTR */
    vinfo_req_size = s_cache_word_count * sizeof(uint32_t);
    vinfo_alloc_size = vinfo_req_size;
    VINFO_PTR = shr_ha_mem_alloc(unit,
                                 BCMMGMT_PTM_COMP_ID,
                                 BCMPTM_HA_SUBID_PTCACHE_VINFO,
                                 &vinfo_alloc_size);
    SHR_NULL_CHECK(VINFO_PTR, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(vinfo_alloc_size < vinfo_req_size ?
                    SHR_E_MEMORY : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "VINFO_ARRAY: requested bytes = %0u, "
                          "allocated bytes = %0u, \n"),
         vinfo_req_size, vinfo_alloc_size));

    /* Check HA signature for VINFO array */
    tmp_w = VINFO_DATA(PTCACHE_VINFO_INDEX_SIGN_WORD);
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != *ref_vinfo_sign ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(unit, "WB: PTcache.VINFO array signature mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         *ref_vinfo_sign, tmp_w));

    /* flags_word check (includes ltid_size size) */
    tmp_w = VINFO_DATA(PTCACHE_VINFO_INDEX_FLAGS_WORD);
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != *ptcache_flags_word ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(unit, "WB: PTcache.VINFO array flags_word mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         *ptcache_flags_word, tmp_w));
    

    /* VINFO_ARRAY sid_count check */
    tmp_w = VINFO_DATA(PTCACHE_VINFO_INDEX_SID_COUNT_WORD);
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != *sid_count ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(unit, "WB: PTcache.VINFO array sid_count mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         (uint32_t)(*sid_count), tmp_w));

    /* VINFO_ARRAY cache_word_count check */
    tmp_w = VINFO_DATA(PTCACHE_VINFO_INDEX_TW_COUNT_WORD);
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != s_cache_word_count ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(unit, "WB: PTcache.VINFO array cache_word_count mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         s_cache_word_count, tmp_w));

    /* VINFO_ARRAY hash_word check */
    tmp_w = VINFO_DATA(PTCACHE_VINFO_INDEX_HASH_WORD);
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != *drd_hash_word ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(unit, "WB: PTcache.VINFO array hash_word mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         *drd_hash_word, tmp_w));

    /* Verify Sentinels */
    SHR_IF_ERR_MSG_EXIT(
        bcmptm_ptcache_verify(unit),
        (BSL_META_U(unit, "WB: PTcache sentinel verification failed\n")));

    /* Skip rest of init - PTcache is in sync with HW tables. */
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_ptcache_init(int unit, bool warm)
{
    bcmdrd_sid_t sid;
    size_t sid_count = 0;
    uint32_t sinfo_req_size, sinfo_alloc_size, vinfo_req_size, vinfo_alloc_size;
    uint32_t csid; /* compact, contiguous sid space */
    uint32_t vinfo_index = PTCACHE_INIT_VINFO_ARRAY_INDEX;
    const char *pt_name = "INVALIDm";
    uint32_t pt_cache_count = 0, pt_skip_count = 0, pt_error_count = 0;
    uint32_t cache_word_count = 0; /* num of dw to store data,valid, corrupt bits */
    uint32_t vinfo_dw_index;
    uint32_t vinfo_cw_index = 0;
    uint32_t exp_vinfo_cw_index = PTCACHE_INIT_VINFO_ARRAY_INDEX;
    bool ltid_size16b = FALSE;
    uint32_t drd_hash_word = 0;
    uint32_t ptcache_flags_word = 0;
    uint32_t ref_sinfo_sign, ref_vinfo_sign, *sinfo_base_ptr;
    sinfo_t sinfo_word;
    SHR_FUNC_ENTER(unit);

    /* First alloc space for fixarray_info
     * - fixarray_info stores 32b for each sid irrespective of whether sid
     *   requires cache or not.
     * - This value contains offset/index into vinfo_array and
     *   also hints about how to interpret index, tbl_inst
     * - zero index value means variable info is not needed, so not available
     * - Non-zero index value means var info is available
     */
    SHR_IF_ERR_EXIT(bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_count));
    SHR_IF_ERR_EXIT(bcmdrd_hash(unit, &drd_hash_word));
    SHR_IF_ERR_EXIT(ptcache_ltid_size_get(unit, &ltid_size16b));
    if (ltid_size16b) {
        ptcache_flags_word |= PTCACHE_FLAGS_LTID_SIZE16;
    }
    ref_sinfo_sign = BCMPTM_HA_SIGN_PTCACHE_SINFO + sid_count;
    ref_vinfo_sign = BCMPTM_HA_SIGN_PTCACHE_VINFO + sid_count;

    sinfo_req_size = sid_count * sizeof(uint32_t);
    sinfo_req_size += 4 * sizeof(uint32_t);
        /* Note: Reserve first 4 words to store
         *       sinfo_sign, vinfo_word_count, etc */
    sinfo_alloc_size = sinfo_req_size;
    sinfo_base_ptr = shr_ha_mem_alloc(unit,
                                      BCMMGMT_PTM_COMP_ID,
                                      BCMPTM_HA_SUBID_PTCACHE_SINFO,
                                      &sinfo_alloc_size);
    SHR_NULL_CHECK(sinfo_base_ptr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(sinfo_alloc_size < sinfo_req_size ?
                    SHR_E_MEMORY : SHR_E_NONE);
    SINFO_PTR = (uint32_t *)(sinfo_base_ptr + 4); /* skip reserved words */
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "SINFO_ARRAY: Number of sid = %0u, requested bytes = %0u, "
                          "allocated bytes = %0u, \n"),
         (uint32_t)sid_count, sinfo_req_size, sinfo_alloc_size));

    if (warm) {
        SHR_IF_ERR_EXIT(
            ptcache_warm_check(unit, &ref_sinfo_sign, &ref_vinfo_sign,
                               &ptcache_flags_word, &drd_hash_word,
                               &sid_count, sinfo_base_ptr));
    } else {
        sal_memset(sinfo_base_ptr, 0, sinfo_alloc_size);
            /* For all sid set:
             * - ptcache_type = NO_CACHE,
             * - index_en = 0, tbl_inst_en = 0
             *
             * Also zero out signature words - these will get initialized at end.
             */
    }

    /* Pass_1:
     * a. Build sinfo_array which stores uint32_t for every sid
     * b. Count num of words needed to store offsets to cache_data, valid_bits
     *    (into vinfo_array)
     * c. Count num of words needed to store cache_data, valid_bits,
     *    corrupt_bits in vinfo_array
     */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_comp_config(unit, warm, BCMPTM_SUB_PHASE_1));

    for (sid = 0; sid < sid_count; sid++) {
        uint32_t pt_entry_count = 0, tbl_inst_count = 0;
        uint32_t pt_word_count = 0;
        uint32_t pt_iw_count = 0;
        uint32_t cw_count = 0;
        const bcmptm_overlay_info_t *overlay_info;
        bool ltid_seg_en = FALSE;
        uint8_t dfid_seg_en = 0;
        bool pt_attr_is_cacheable = FALSE;
        bool pt_is_ctr_with_cfg_fields = FALSE;
        bool tc_seg_en = FALSE;
        uint32_t cci_map_id = 0;

        pt_name = bcmdrd_pt_sid_to_name(unit, sid);
        csid = sid;

        pt_entry_count = bcmptm_pt_index_count(unit, sid);
        TBL_INST_COUNT_SET(unit, sid);

        sinfo_word.entry = 0; /* clear all fields */

        if (pt_entry_count == 0 || tbl_inst_count == 0) {
            /* Ret val of bcmptm_pt_index_count() is always >= 0
             * Ret val of 0 implies case where PT could be disabled due to
             * latency mode and no cache should be allocated
             */
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "pt=%s, sid=%0d, "
                            "Caching SKIPPED as pt_entry_count=%0d, "
                            "tbl_inst_count = %0d\n"),
                 pt_name, sid, pt_entry_count, tbl_inst_count));
            pt_error_count++;
            continue;
        } else {
            if (pt_entry_count > 1) {
               sinfo_word.f.index_en = 1;
            }
            if (tbl_inst_count > 1) {
                pt_entry_count *= tbl_inst_count;
                sinfo_word.f.tbl_inst_en = 1;
            }
        }

        SHR_IF_ERR_EXIT(bcmptm_pt_attr_is_cacheable(unit, sid,
                        &pt_attr_is_cacheable));
        SHR_IF_ERR_EXIT(bcmptm_pt_needs_ltid(unit, sid, &ltid_seg_en));
        SHR_IF_ERR_EXIT(bcmptm_pt_needs_dfid(unit, sid, &dfid_seg_en));
        SHR_IF_ERR_EXIT(bcmptm_pt_overlay_info_get(unit, sid, &overlay_info));

        SHR_IF_ERR_EXIT(bcmptm_tcam_corrupt_bits_enable(unit, &tc_seg_en));
        SHR_IF_ERR_EXIT(bcmptm_pt_iw_count(unit, sid, &pt_iw_count));


        /* Counters */
        if (bcmdrd_pt_is_valid(unit, sid) &&
            bcmdrd_pt_is_counter(unit, sid)) {
            int tmp_rv = SHR_E_NONE;

            /* ignore cci_map_id in Pass_1 */
            tmp_rv = bcmptm_cci_ctr_reg(unit, sid, &cci_map_id);
            if (SHR_FAILURE(tmp_rv)) {
                sinfo_word.f.ptcache_type = PTCACHE_TYPE_NO_CACHE;
                sinfo_word.f.vinfo_cw_index = 0;
                SINFO_DATA(csid) = sinfo_word.entry;
                pt_skip_count++;
                LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Caching SKIPPED for counter pt=%s, "
                                "sid=%0d as cci_ctr_reg failed (%0d)\n"),
                     pt_name, sid, tmp_rv));
                continue; /* goto process next sid */
            }

            
            pt_is_ctr_with_cfg_fields = pt_attr_is_cacheable;

            /* Do we need to cache this in PTcache ? */
            if (pt_is_ctr_with_cfg_fields) { /* must cache this */

                BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "pt=%s, sid=%0d, "
                                "Caching enabled for COUNTER\n"),
                     pt_name, sid));

                /* Prep vars to re-use PTCACHE_ME code */
                /* overlay_info->mode will be BCMPTM_OINFO_MODE_NONE */
                dfid_seg_en = FALSE;
                ltid_seg_en = FALSE;
                tc_seg_en = FALSE;
                pt_iw_count = 1; /* to store cci_map_id */

                /* Actual work will be done in PTCACHE_ME code Pass_1 below */

            } else { /* cci_only - no need to cache in PTcache */
                sinfo_word.f.ptcache_type = PTCACHE_TYPE_CCI_ONLY;
                sinfo_word.f.vinfo_cw_index = 0;
                SINFO_DATA(csid) = sinfo_word.entry;

                pt_skip_count++;
                BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "pt=%s, sid=%0d, "
                                "Caching SKIPPED for CCI_ONLY COUNTER\n"),
                     pt_name, sid));
                continue; /* goto process next sid */
            } /* cci_only */

        } /* counters */

        if (bcmdrd_pt_is_valid(unit, sid)
            && (pt_attr_is_cacheable || pt_is_ctr_with_cfg_fields)
            && pt_entry_count /* alloc only if pt_entry_count > 0 */
#if defined(PTCACHE_REGS_DISABLE)
            && (bcmdrd_pt_is_mem(unit, sid) ||
                ptcache_pt_is_reg_with_ser(unit, sid))
#endif /* PTCACHE_REGS_DISABLE */
           ) {
            if ((pt_entry_count == 1) && !bcmdrd_pt_attr_is_cam(unit, sid) &&
                !pt_is_ctr_with_cfg_fields) {
                sinfo_word.f.ptcache_type = PTCACHE_TYPE_SE_NOT_VALID;
                /* Covers REGs (and other single entry MEMs, if any) for which
                 * during cache_acc, we can ignore index, tbl_inst
                 *
                 * - For such PTs, there are no control words in
                 *   vinfo_array. We directly store entry_data. Thus there is
                 *   only one level of indirection.
                 *
                 *   For all other PTs, sinfo_word points to ctl_words in
                 *   vinfo_array - which then point to actual data, valid segs
                 *   in vinfo_array - so there is 2 levels of indirection
                 *
                 * - Change of state from SE_NOT_VALID to SE_VALID will
                 *   happen during write access and from SE_VALID to
                 *   SE_NOT_VALID will happen by cache_invalidate
                 *
                 * - Can we infer this ptcache_type from:
                 *   (!index_en && !tbl_inst_en)
                 *   Above cond can be TRUE and still the sid may / not be
                 *   cacheable
                 *
                 * - Note: Have purposely excluded case of single entry cam.
                 *         Have purposely excluded case of single entry counter.
                 *         In reality there should not be such sid and if there
                 *         is one - we will treat it like multi-entry memory
                 *         even though it is inefficient.
                 */

                /* space to store data directly in vinfo array */
                pt_word_count = bcmdrd_pt_entry_wsize(unit, sid);

                
                ltid_seg_en = FALSE;

                
                SHR_IF_ERR_EXIT(ltid_seg_en?  SHR_E_UNAVAIL : SHR_E_NONE);
#if 0 /* ltid NOT SUPPORTED for regs */
                if (ltid_seg_en) {
                    pt_word_count++; /* to store LTID - even for 16b ltid */
                    sinfo_word.f.ptcache_type = PTCACHE_TYPE_LTID_SE_NOT_VALID;
    #if defined(PTCACHE_SENTINEL_SE_ENABLE)
                    pt_word_count++; /* sentinel for direct data */
                    sinfo_word.f.vinfo_cw_index = vinfo_index + 2;
    #else
                    sinfo_word.f.vinfo_cw_index = vinfo_index + 1;
    #endif /* PTCACHE_SENTINEL_SE_ENABLE */
                } else { /* no ltid */
                    /* see code below */
                }
#endif /* ltid NOT SUPPORTED for regs */

#if defined(PTCACHE_SENTINEL_SE_ENABLE)
                    pt_word_count++; /* sentinel for direct data */
                    sinfo_word.f.vinfo_cw_index = vinfo_index + 1;
#else
                    sinfo_word.f.vinfo_cw_index = vinfo_index;
#endif /* PTCACHE_SENTINEL_SE_ENABLE */

                if (!warm) {
                    /* Prevent marking valid register invalid in cache */
                    SINFO_DATA(csid) = sinfo_word.entry;
                }

                /* allocate space for data, valid segs in vinfo_array */
                /* cache_word_count += pt_word_count;
                 * as we are not allocating data, valid segs in vinfo_array
                 * for single entry sids */

                /* update vinfo_index - will be needed for next sid */
                vinfo_index += pt_word_count;
                /* alloc space in ctl section of vinfo_array to store data
                 * directly */

            } else { /* ME type */
                if (pt_is_ctr_with_cfg_fields) {
                    sinfo_word.f.ptcache_type = PTCACHE_TYPE_ME_CCI;
                } else {
                    sinfo_word.f.ptcache_type = PTCACHE_TYPE_ME;
                }

                /* space for valid_seg in vinfo array */
                pt_word_count = BCMPTM_BITS2WORDS(pt_entry_count);
                cw_count++; /* to store vs_index */
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                pt_word_count++; /* sentinel for valid_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */


                /* space for overlay_cws, data_seg in vinfo array */
                if (overlay_info->mode != BCMPTM_OINFO_MODE_NONE) {
                    sinfo_word.f.xinfo_en = 1;
                    /* no data_seg, sentinel_word for ds
                     * pt_word_count += 0;
                     */
                     if (overlay_info->num_we_field) {
                        cw_count += 1 + overlay_info->num_we_field;
                     } else { /* num_we_field = 0 */
                        cw_count += 2;
                            /* to store 1st two oinfo_words - min */
                     }
                } else { /* BCMPTM_OINFO_MODE_NONE */
                    pt_word_count += pt_entry_count *
                                     bcmdrd_pt_entry_wsize(unit, sid);
                    cw_count++; /* to store ds_index */
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    pt_word_count++; /* sentinel for data_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
                }


                /* space for ltid_seg in vinfo array */
                if (ltid_seg_en) {
                    sinfo_word.f.xinfo_en = 1;
                    if (ltid_size16b) { /* 16b ltid */
                        pt_word_count += BCMPTM_DBYTES2WORDS(pt_entry_count);
                    } else { /* 32b ltid */
                        pt_word_count += pt_entry_count;
                    }
                    cw_count++; /* to store ltid_seg_index */
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    pt_word_count++; /* sentinels for ltid_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
                }


                /* space for dfid_seg in vinfo array */
                if (dfid_seg_en) {
                    sinfo_word.f.xinfo_en = 1;
                    if (dfid_seg_en == 2) { /* 16b dfid */
                        pt_word_count += BCMPTM_DBYTES2WORDS(pt_entry_count);
                    } else { /* 8b dfid */
                        pt_word_count += BCMPTM_BYTES2WORDS(pt_entry_count);
                    }
                    cw_count++; /* to store dfid_seg_index */
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    pt_word_count++; /* sentinels for dfid_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
                }


                /* space for info_word_seg in vinfo array */
                if (pt_iw_count) {
                    sinfo_word.f.xinfo_en = 1;
                    pt_word_count += pt_iw_count;
                    cw_count++; /* to store is_index */
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    pt_word_count++; /* sentinel for info_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
                }


                /* space for tcam_corrupt_seg in vinfo array */
                if (bcmdrd_pt_attr_is_cam(unit, sid) && tc_seg_en) {
                    sinfo_word.f.xinfo_en = 1;
                    pt_word_count += BCMPTM_BITS2WORDS(pt_entry_count);
                    cw_count++; /* to store tcs_index */
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    pt_word_count++; /* sentinels for corrupt_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                    
                }

                if (sinfo_word.f.xinfo_en) {
                    cw_count++; /* to store xinfo_word */
                }

                /* Update sinfo word */
                sinfo_word.f.vinfo_cw_index = vinfo_index;
                SINFO_DATA(csid) = sinfo_word.entry;

                /* Update cache_word_count by num of words we want in
                 * vinfo.dw_section */
                cache_word_count += pt_word_count;

                /* Update vinfo_index by num of ctl words we used in
                 * vinfo.cw_section - will be needed for next sid */
                vinfo_index += cw_count;
            } /* ME type */

            pt_cache_count++;
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "pt=%s, sid=%0d, csid=%0d, entry_count=%0d, "
                            "entry_size=%0u, word_count=%0d, index_count=%0d, "
                            "tbl_inst_count=%0d, "
                            "is_cam=%0d, tc_en=%0d, overlay_mode=%0d, iw_en=%0d, "
                            "ltid_seg_en=%0d, dfid_seg_en=%0d, cw_count=%0d, "
                            "sinfo_ptr[%p] = %8x, vinfo_index=%0d, "
                            "cache_word_count=%0d, MBytes=%0d\n"),
                 pt_name, sid, csid, pt_entry_count,
                 bcmdrd_pt_entry_wsize(unit, sid), pt_word_count,
                 bcmptm_pt_index_count(unit, sid), tbl_inst_count,
                 bcmdrd_pt_attr_is_cam(unit, sid),
                 tc_seg_en, overlay_info->mode, pt_iw_count, ltid_seg_en,
                 dfid_seg_en, cw_count,
                 (void *)(SINFO_PTR + csid),
                 SINFO_DATA(csid), vinfo_index, cache_word_count,
                 (cache_word_count*4)/1000000));
        } else {
            pt_skip_count++;
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "pt=%s, sid=%0d, csid=%0d, entry_count=%0d, "
                            "entry_size=%0u, word_count=%0d, index_count=%0d, "
                            "tbl_inst_count=%0d, "
                            "is_cam=%0d, tc_en=%0d, overlay_mode=%0d, iw_en=%0d, "
                            "ltid_seg_en=%0d, dfid_seg_en=%0d, cw_count=%0d, "
                            "sinfo_ptr[%p] = %8x, vinfo_index=%0d, SKIPPED\n"),
                 pt_name, sid, csid, pt_entry_count,
                 bcmdrd_pt_entry_wsize(unit, sid), pt_word_count,
                 bcmptm_pt_index_count(unit, sid), tbl_inst_count,
                 bcmdrd_pt_attr_is_cam(unit, sid),
                 tc_seg_en, overlay_info->mode, pt_iw_count, ltid_seg_en,
                 dfid_seg_en, cw_count,
                 (void *)(SINFO_PTR + csid),
                 SINFO_DATA(csid), vinfo_index));
        }
    } /* foreach sid */

    /* Now: vinfo_index has total num of:
     *      a. data_words, sentinel_words for SE type mems
     *      b. control_words for ME type mems
     *      AND it also points to 1st dataword for cacheable mem in vinfo_array
     *      cache_word_count has number of (data, valid, corrupt, info) words
     */
    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "\n\nSummary for Pass_1: cache_count = %0d, "
                    "skip_count = %0d, error_count = %0d, \n"
                    "vinfo.num_CW = %0d, vinfo.num_DW = %0d, "
                    "sinfo.num_W = %0u\n\n"),
         pt_cache_count, pt_skip_count, pt_error_count, vinfo_index,
         cache_word_count, (uint32_t)sid_count));
    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "\n\nPTcache: #SID cached = %0d, Total space alloc "
                    "= %0d Bytes\n"),
         pt_cache_count,
         ((uint32_t)sid_count + vinfo_index + cache_word_count)*4));

    /* Make sure vinfo_index can fit in sinfo_word */
    SHR_IF_ERR_EXIT((vinfo_index >= VINFO_INDEX_CTL_WORD_MAX)?
                    SHR_E_INTERNAL : SHR_E_NONE);

    cache_word_count += vinfo_index; /* total words reqd in vinfo_array */

    if (!warm) {
        /* Allocate space for vinfo_array */
        vinfo_req_size = cache_word_count * sizeof(uint32_t);
        vinfo_alloc_size = vinfo_req_size;
        VINFO_PTR = shr_ha_mem_alloc(unit,
                                     BCMMGMT_PTM_COMP_ID,
                                     BCMPTM_HA_SUBID_PTCACHE_VINFO,
                                     &vinfo_alloc_size);
        SHR_NULL_CHECK(VINFO_PTR, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(vinfo_alloc_size < vinfo_req_size ?
                        SHR_E_MEMORY : SHR_E_NONE);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "VINFO_ARRAY: requested bytes = %0u, "
                              "allocated bytes = %0u, \n"),
             vinfo_req_size, vinfo_alloc_size));

        /* Zero out data, valid, corrupt bits, info_words, etc */
        sal_memset(VINFO_PTR, 0, vinfo_alloc_size);
    }

    /* Pass_2:
     * Populate vinfo_array:
     * 1. ctl_words with offsets to data, valid, corrupt, info segs
     * 2. fill data_seg with pre-defined pattern
     * 3. clear valid bits, tcam_corrupt bits
     * 4. fill info_seg with correct data
     * 5. also fill sentinel words
     */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_comp_config(unit, warm, BCMPTM_SUB_PHASE_2));

    /* Write hash words */
    VINFO_DATA(PTCACHE_VINFO_INDEX_HASH_WORD) = drd_hash_word;

    /* Write other important words */
    VINFO_DATA(PTCACHE_VINFO_INDEX_FLAGS_WORD) = ptcache_flags_word;
        /* store flags_word */
    VINFO_DATA(PTCACHE_VINFO_INDEX_SID_COUNT_WORD) = sid_count;
        /* num of sids - num_words of sinfo_array */
    VINFO_DATA(PTCACHE_VINFO_INDEX_CW_COUNT_WORD) = vinfo_index;
        /* num_words in cw_section of vinfo_array */
    VINFO_DATA(PTCACHE_VINFO_INDEX_TW_COUNT_WORD) = cache_word_count;
        /* total num_words in vinfo_array */

    vinfo_dw_index = vinfo_index; /* where 1st data_seg will begin */
    for (sid = 0; sid < sid_count; sid++) {
        uint32_t pt_entry_count = 0, tbl_inst_count = 0;
        uint32_t pt_word_count = 0;
        uint32_t pt_iw_count = 0;
        const bcmptm_overlay_info_t *overlay_info;
        uint32_t vs_word_count = 0;
        uint32_t tcs_word_count = 0;
        uint8_t dfid_seg_en = 0;
        xinfo_t xinfo_word;
        bool ltid_seg_en = FALSE;
        bool tc_seg_en = FALSE;
        uint32_t cci_map_id = 0;

        pt_name = bcmdrd_pt_sid_to_name(unit, sid);
        csid = sid;

        pt_entry_count = bcmptm_pt_index_count(unit, sid);
        TBL_INST_COUNT_SET(unit, sid);
        pt_entry_count *= tbl_inst_count;
        pt_word_count = pt_entry_count * bcmdrd_pt_entry_wsize(unit, sid);
        vs_word_count = BCMPTM_BITS2WORDS(pt_entry_count);
        tcs_word_count = vs_word_count;
        SHR_IF_ERR_EXIT(bcmptm_pt_overlay_info_get(unit, sid, &overlay_info));
        SHR_IF_ERR_EXIT(bcmptm_pt_needs_dfid(unit, sid, &dfid_seg_en));
        SHR_IF_ERR_EXIT(bcmptm_pt_needs_ltid(unit, sid, &ltid_seg_en));
        SHR_IF_ERR_EXIT(bcmptm_tcam_corrupt_bits_enable(unit, &tc_seg_en));
        SHR_IF_ERR_EXIT(bcmptm_pt_iw_count(unit, sid, &pt_iw_count));

        /* get sinfo for this sid */
        sinfo_word.entry = SINFO_DATA(csid);
        vinfo_cw_index = sinfo_word.f.vinfo_cw_index;

        /* Try to re-use as much code for PTCACHE_TYPE_ME */
        if (sinfo_word.f.ptcache_type == PTCACHE_TYPE_ME_CCI) {
            /* overlay_info->mode will be BCMPTM_OINFO_MODE_NONE */
            dfid_seg_en = FALSE;
            ltid_seg_en = FALSE;
            tc_seg_en = FALSE;
            pt_iw_count = 1; /* to store cci_map_id */
        }

        switch (sinfo_word.f.ptcache_type) {
            case PTCACHE_TYPE_CCI_ONLY:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_reg(unit, sid, &cci_map_id));

                /* Make sure cci_map_id (vinfo_index) can fit in sinfo_word */
                SHR_IF_ERR_EXIT((cci_map_id >= VINFO_INDEX_CTL_WORD_MAX)?
                                SHR_E_INTERNAL : SHR_E_NONE);

                /* Store cci_map_id in Pass_2 */
                sinfo_word.f.vinfo_cw_index = cci_map_id;
                SINFO_DATA(csid) = sinfo_word.entry;

                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, CCI_only"
                                "cci_map_id=%0d\n"),
                     pt_name, sid, cci_map_id));
                break; /* PTCACHE_TYPE_CCI_ONLY */

            case PTCACHE_TYPE_SE_NOT_VALID:
            case PTCACHE_TYPE_SE_VALID: /* can happen during WB */
                if (vinfo_cw_index !=
#if defined(PTCACHE_SENTINEL_SE_ENABLE)
                    (exp_vinfo_cw_index + 1)
                    /* sinfo_word will point to first data_word and not to
                     * sentinel_word, hence must do + 1 */
#else
                    exp_vinfo_cw_index
#endif /* PTCACHE_SENTINEL_SE_ENABLE */
                   ) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                    "vinfo_cw_index=%0d, "
                                    "exp_vinfo_cw_index=%0d\n"),
                         pt_name, sid, vinfo_cw_index, exp_vinfo_cw_index));
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
                if (pt_word_count <= 0) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                    "pt_word_count=%0d\n"),
                         pt_name, sid, pt_word_count));
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
#if defined(PTCACHE_SENTINEL_SE_ENABLE)
                VINFO_DATA(vinfo_cw_index - 1) =
                    ptcache_sentinel_calc(unit, sid, vinfo_cw_index - 1);
                exp_vinfo_cw_index++;
#endif /* PTCACHE_SENTINEL_SE_ENABLE */
                exp_vinfo_cw_index += pt_word_count; /* direct data */

                /* program entries in cache with null_entry */
                if (!warm &&
                    bcmdrd_pt_default_value_is_nonzero(unit, sid)) {
                    SHR_IF_ERR_EXIT(
                        fill_null_entry(unit, sid, vinfo_cw_index));
                }
                break;

            case PTCACHE_TYPE_ME:
            case PTCACHE_TYPE_ME_CCI:
                if (vinfo_cw_index != exp_vinfo_cw_index) {
                    /* sentinels for ME are stored in dw_section,
                     * so sinfo_word will point to 1st cw and no adjustment is
                     * needed to RHS
                     */
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                    "vinfo_cw_index=%0d, "
                                    "exp_vinfo_cw_index=%0d\n"),
                         pt_name, sid, vinfo_cw_index, exp_vinfo_cw_index));
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
                if ((pt_word_count <= 0) || (vs_word_count <= 0)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                    "pt_word_count=%0d, "
                                    "vs_word_count=%0d\n"),
                         pt_name, sid, pt_word_count, vs_word_count));
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }

                if (!sinfo_word.f.xinfo_en) { /* Normal case */
                     /* cw0, cw1 hold vs_index, ds_index respectively */

#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    /* program sentinel for valid_seg */
                    VINFO_DATA(vinfo_dw_index) =
                        ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                    vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                    /* program offset for valid_seg cw0 */
                    VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                    vinfo_cw_index++;
                    exp_vinfo_cw_index++;

                    /* advance dw_index to point data_seg */
                    vinfo_dw_index += vs_word_count;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                    "vs_word_count=%0d, "
                                    "next vinfo_dw_index=%0d\n"),
                         pt_name, sid, vs_word_count, vinfo_dw_index));


#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    /* program sentinel for data_seg */
                    VINFO_DATA(vinfo_dw_index) =
                    ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                    vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                    /* program offset for data_seg in cw1 */
                    VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                    vinfo_cw_index++;
                    exp_vinfo_cw_index++;

                    /* program entries in cache with null_entry */
                    if (!warm && bcmdrd_pt_default_value_is_nonzero(unit, sid)) {
                        SHR_IF_ERR_EXIT(
                            fill_null_entry(unit, sid, vinfo_dw_index));
                    }

                    /* offset to valid_seg for next sid */
                    vinfo_dw_index += pt_word_count;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                    "pt_word_count=%0d, "
                                    "next vinfo_dw_index=%0d\n"),
                         pt_name, sid, pt_word_count, vinfo_dw_index));

                } else { /* xinfo case */
                    /* We may have
                     * overlay_info_words (no data_seg)
                     * valid_seg
                     * data_seg
                     * ltid_seg
                     * dfid_seg
                     * info_seg
                     * tc_seg
                     *
                     * cw0 always stores the xinfo_word
                     * For overlay case:
                     *     cw1,2, ... ,N store overlay_info_words
                     *     cwN+1 stores the vs_index
                     * For non-overlay case
                     *     cw1 stores the vs_index
                     *
                     * Subsequent cw (the one after vs_index) may store
                     *     ds_index,
                     *     ltid_seg_index,
                     *     dfid_seg_index,
                     *     is_index,
                     *     tcs_index,
                     * in this order
                     * depending on whether these segs are enabled
                     */

                    /* compute xinfo_word */
                    xinfo_word.entry = 0;
                    /* xinfo_word is always in cw0, so by default (non_overlay
                     * case), vs_index cw is stored in cw1 */
                    xinfo_word.fme.cw_offset_vs = 1;
                    xinfo_word.fme.data_seg_en = 1;

                    if (overlay_info->mode != BCMPTM_OINFO_MODE_NONE) {
                        xinfo_word.fme.overlay_mode = overlay_info->mode;
                        xinfo_word.fme.data_seg_en = 0;

                        /* Make sure that we are not violating limits of
                         * cw_offset_vs */
                        if (overlay_info->num_we_field) {
                            SHR_IF_ERR_EXIT(
                                (overlay_info->num_we_field >= XINFO_CW_OFFSET_VS_MAX) ?
                                SHR_E_INTERNAL : SHR_E_NONE);

                           xinfo_word.fme.cw_offset_vs +=
                               (1 + overlay_info->num_we_field);
                        } else { /* num_we_field = 0 */
                           xinfo_word.fme.cw_offset_vs += 2;
                               /* To store 1st two oinfo_words - min
                                * overlay_words are stored in cw1,cw2
                                * and vs_index is stored in cw3 */
                        }
                    }

                    if (ltid_seg_en) {
                        xinfo_word.fme.ltid_seg_en = 1;
                    }
                    if (dfid_seg_en) {
                        xinfo_word.fme.dfid_seg_en = dfid_seg_en;
                    }
                    if (pt_iw_count) {
                        xinfo_word.fme.info_seg_en = 1;
                    }
                    if (bcmdrd_pt_attr_is_cam(unit, sid) && tc_seg_en) {
                        xinfo_word.fme.tc_seg_en = 1;
                    }

                    /* cw_section: program xinfo word in cw0 */
                    VINFO_DATA(vinfo_cw_index) = xinfo_word.entry;
                    vinfo_cw_index++;
                    exp_vinfo_cw_index++;

                    if (overlay_info->mode != BCMPTM_OINFO_MODE_NONE) {
                        uint32_t overlay_num_cw = 0;
                        /* cw_section: program overlay_info in cw1,2,..N */
                        SHR_IF_ERR_EXIT(
                            ptcache_overlay_info_set(unit, sid, overlay_info,
                                                     vinfo_cw_index,
                                                     &overlay_num_cw));

                        vinfo_cw_index += overlay_num_cw;
                        exp_vinfo_cw_index += overlay_num_cw;

                        /* No data_seg for this sid, so
                         * cw3 is offset to valid_seg for this sid */
                    }

#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                    /* program sentinel for valid_seg */
                    VINFO_DATA(vinfo_dw_index) =
                        ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                    vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                    /* program offset for valid_seg in cw2/cw1
                     * depending on whether overlay=1,0 respectively */
                    VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                    vinfo_cw_index++;
                    exp_vinfo_cw_index++;

                    /* advance dw_index to point data_seg */
                    vinfo_dw_index += vs_word_count;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                    "vs_word_count=%0d, "
                                    "next vinfo_dw_index=%0d\n"),
                         pt_name, sid, vs_word_count, vinfo_dw_index));

                    if (xinfo_word.fme.data_seg_en) {
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                        /* program sentinel for data_seg */
                        VINFO_DATA(vinfo_dw_index) =
                            ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                        vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                        /* program offset for data_seg in cw3/cw2
                         * depending on whether overlay=1,0 respectively */
                        VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                        vinfo_cw_index++;
                        exp_vinfo_cw_index++;

                        /* program entries in cache with null_entry */
                        if (!warm &&
                            bcmdrd_pt_default_value_is_nonzero(unit, sid)) {
                            SHR_IF_ERR_EXIT(
                                fill_null_entry(unit, sid, vinfo_dw_index));
                        }

                        /* offset to dfid_seg/info_seg/tc_seg for this sid or
                         * valid_seg for next sid */
                        vinfo_dw_index += pt_word_count;
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                        "pt_word_count=%0d, "
                                        "next vinfo_dw_index=%0d\n"),
                             pt_name, sid, pt_word_count, vinfo_dw_index));
                    }

                    if (xinfo_word.fme.ltid_seg_en) {
                        uint32_t ltid_seg_wsize = 0;
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                        /* program sentinel for ltid_seg */
                        VINFO_DATA(vinfo_dw_index) =
                            ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                        vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                        /* program offset for ltid_seg */
                        VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                        vinfo_cw_index++;
                        exp_vinfo_cw_index++;

                        /* offset to dfid_seg/info_seg/tc_seg for this sid or
                         * valid_seg for next sid */
                        if (ltid_size16b) { /* 16b ltid */
                            ltid_seg_wsize = BCMPTM_DBYTES2WORDS(pt_entry_count);
                        } else { /* 32b ltid */
                            ltid_seg_wsize = pt_entry_count;
                        }
                        vinfo_dw_index += ltid_seg_wsize;
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                        "ltid_seg_wsize=%0d, "
                                        "next vinfo_dw_index=%0d\n"),
                             pt_name, sid, ltid_seg_wsize, vinfo_dw_index));
                    }

                    if (xinfo_word.fme.dfid_seg_en) {
                        uint32_t dfid_seg_wsize = 0;
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                        /* program sentinel for dfid_seg */
                        VINFO_DATA(vinfo_dw_index) =
                            ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                        vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                        /* program offset for dfid_seg */
                        VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                        vinfo_cw_index++;
                        exp_vinfo_cw_index++;

                        /* offset to info_seg/tc_seg for this sid or
                         * valid_seg for next sid */
                        if (xinfo_word.fme.dfid_seg_en == 2) { /* 16b dfid */
                            dfid_seg_wsize = BCMPTM_DBYTES2WORDS(pt_entry_count);
                        } else { /* 8b dfid */
                            dfid_seg_wsize = BCMPTM_BYTES2WORDS(pt_entry_count);
                        }
                        vinfo_dw_index += dfid_seg_wsize;
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                        "dfid_seg_wsize=%0d, "
                                        "next vinfo_dw_index=%0d\n"),
                             pt_name, sid, dfid_seg_wsize, vinfo_dw_index));
                    }

                    if (pt_iw_count) {
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                        /* program sentinel for info_seg */
                        VINFO_DATA(vinfo_dw_index) =
                            ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                        vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                        /* program offset for info_seg */
                        VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                        vinfo_cw_index++;
                        exp_vinfo_cw_index++;

                        /* Now fill correct info */
                        if (sinfo_word.f.ptcache_type == PTCACHE_TYPE_ME_CCI) {

                            SHR_IF_ERR_EXIT(
                                bcmptm_cci_ctr_reg(unit, sid, &cci_map_id));

                            /* Make sure cci_map_id (vinfo_index) can fit in
                             * sinfo_word - even though we are not storing it
                             * in sinfo_word.f.vinfo_cw_index - to keep
                             * consistency with CCI_ONLY case */
                            SHR_IF_ERR_EXIT((cci_map_id >= VINFO_INDEX_CTL_WORD_MAX)?
                                            SHR_E_INTERNAL : SHR_E_NONE);

                            /* Store cci_map_id in Pass_2
                             * - but this time in info_seg */
                            VINFO_DATA(vinfo_dw_index) = cci_map_id;
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Pass_2: pt=%s, sid=%0d, "
                                            "ME_CCI cci_map_id=%0d\n"),
                                 pt_name, sid, cci_map_id));
                        } else {
                            SHR_IF_ERR_EXIT(
                                bcmptm_pt_iw_fill(unit, sid, vinfo_dw_index));
                        }

                        /* offset to tcam_corrupt_seg for this sid,
                         * OR offset to valid_seg for next sid */
                        vinfo_dw_index += pt_iw_count;
                    }

                    if (xinfo_word.fme.tc_seg_en) {
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                        /* program sentinel for tc_seg */
                        VINFO_DATA(vinfo_dw_index) =
                            ptcache_sentinel_calc(unit, sid, vinfo_dw_index);
                        vinfo_dw_index++;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                        /* program offset for tcam_corrupt_seg */
                        VINFO_DATA(vinfo_cw_index) = vinfo_dw_index;
                        vinfo_cw_index++;
                        exp_vinfo_cw_index++;

                        /* offset to valid_seg for next sid */
                        vinfo_dw_index += tcs_word_count;
                    }
                } /* xinfo_en */

                break; /* PTCACHE_TYPE_ME, PTCACHE_TYPE_ME_CCI */

/* cannot happen during ptcache_init
            case PTCACHE_TYPE_SE_VALID:
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                break;
*/

            default: /* PTCACHE_TYPE_NO_CACHE */
                break;
        }
    } /* foreach sid */

    /* End checks:
     * Now vinfo_dw_index represents total alloc words in vinfo_array */
    if ((vinfo_dw_index != cache_word_count) ||
        (exp_vinfo_cw_index != vinfo_index)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Pass_2: End check failed: "
                        "exp vinfo_dw_index = %0d, obs = %0d, "
                        "exp vinfo_cw_index = %0d, obs = %0d\n"),
             cache_word_count, vinfo_dw_index,
             vinfo_index, exp_vinfo_cw_index));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } else {
        BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Pass_2: completed without errors\n")));

    }
    SHR_IF_ERR_EXIT(bcmptm_ptcache_verify(unit));

    /* Write the signature words last */
    VINFO_DATA(PTCACHE_VINFO_INDEX_SIGN_WORD) = ref_vinfo_sign;
    *(sinfo_base_ptr + 3) = 0; /* reserved */
    *(sinfo_base_ptr + 2) = 0; /* reserved */
    *(sinfo_base_ptr + 1) = cache_word_count; /* vinfo word count */
    *sinfo_base_ptr = ref_sinfo_sign; /* very last on purpose */

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmptm_ptcache_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* No need to release HA mem */

    /* Reset static vars */
    SINFO_PTR = NULL;
    VINFO_PTR = NULL;

/* exit: */
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_verify(int unit)
{
    bcmdrd_sid_t sid;
    size_t sid_count = 0;
    uint32_t csid; /* compact, contiguous sid space */
    uint32_t vinfo_sw_index;
    sinfo_t sinfo_word;
    xinfo_t xinfo_word;
    SHR_FUNC_ENTER(unit);

    if (SINFO_PTR == NULL) {
       SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL); /* without display */
    }
    SHR_NULL_CHECK(VINFO_PTR, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_count));
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Number of sid = %0u\n"), (uint32_t)sid_count));

    /* return SHR_E_NONE; */
    for (sid = 0; sid < sid_count; sid++) {
        uint32_t vinfo_cw_index = 0, vinfo_cw_index_vs;

        /* pt_name = bcmdrd_pt_sid_to_name(unit, sid); */
        csid = sid;

        /* get sinfo for this sid */
        sinfo_word.entry = SINFO_DATA(csid);
        vinfo_cw_index = sinfo_word.f.vinfo_cw_index;

        switch (sinfo_word.f.ptcache_type) {
            case PTCACHE_TYPE_SE_NOT_VALID:
            case PTCACHE_TYPE_SE_VALID:
                SHR_IF_ERR_EXIT(
                    (vinfo_cw_index < PTCACHE_INIT_VINFO_ARRAY_INDEX)?
                    SHR_E_INTERNAL : SHR_E_NONE);
#if defined(PTCACHE_SENTINEL_SE_ENABLE)
                vinfo_sw_index = vinfo_cw_index - 1;
                SHR_IF_ERR_EXIT(
                    ptcache_sentinel_check(unit, sid, vinfo_sw_index, "REG"));
#endif /* PTCACHE_SENTINEL_SE_ENABLE */
                break;

            case PTCACHE_TYPE_ME:
            case PTCACHE_TYPE_ME_CCI:
                SHR_IF_ERR_EXIT(
                    (vinfo_cw_index < PTCACHE_INIT_VINFO_ARRAY_INDEX)?
                    SHR_E_INTERNAL : SHR_E_NONE);
#if defined(PTCACHE_SENTINEL_ME_ENABLE)

                /* read xinfo_word */
                if (sinfo_word.f.xinfo_en) {
                    xinfo_word.entry = VINFO_DATA(vinfo_cw_index);
                    vinfo_cw_index_vs = vinfo_cw_index +
                                        xinfo_word.fme.cw_offset_vs;

                    /* valid_seg */
                    vinfo_sw_index = VINFO_DATA(vinfo_cw_index_vs) - 1;
                    SHR_IF_ERR_EXIT(
                        ptcache_sentinel_check(unit, sid, vinfo_sw_index,
                                               "VS"));

                    /* data_seg */
                    if (xinfo_word.fme.data_seg_en) {
                        vinfo_cw_index_vs++; /* cw for ds */
                        vinfo_sw_index = VINFO_DATA(vinfo_cw_index_vs) - 1;
                        SHR_IF_ERR_EXIT(
                            ptcache_sentinel_check(unit, sid, vinfo_sw_index,
                                                   "DS"));
                    }

                    /* info_seg */
                    if (xinfo_word.fme.info_seg_en) {
                        vinfo_cw_index_vs++; /* cw for is */
                        vinfo_sw_index = VINFO_DATA(vinfo_cw_index_vs) - 1;
                        SHR_IF_ERR_EXIT(
                            ptcache_sentinel_check(unit, sid, vinfo_sw_index,
                                                   "IS"));
                    }

                    /* tc_seg */
                    if (xinfo_word.fme.tc_seg_en) {
                        vinfo_cw_index_vs++; /* cw for tcs */
                        vinfo_sw_index = VINFO_DATA(vinfo_cw_index_vs) - 1;
                        SHR_IF_ERR_EXIT(
                            ptcache_sentinel_check(unit, sid, vinfo_sw_index,
                                                   "TCS"));
                    }
                } else { /* !xinfo_en */
                    vinfo_cw_index_vs = vinfo_cw_index;

                    /* valid_seg */
                    vinfo_sw_index = VINFO_DATA(vinfo_cw_index_vs) - 1;
                    SHR_IF_ERR_EXIT(
                        ptcache_sentinel_check(unit, sid, vinfo_sw_index,
                                               "VS"));

                    /* data_seg */
                    vinfo_cw_index_vs++; /* cw for ds */
                    vinfo_sw_index = VINFO_DATA(vinfo_cw_index_vs) - 1;
                    SHR_IF_ERR_EXIT(
                        ptcache_sentinel_check(unit, sid, vinfo_sw_index,
                                               "DS"));
                } /* !xinfo_en */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
                break; /* PTCACHE_TYPE_ME, ME_CCI */

            default: /* PTCACHE_TYPE_NO_CACHE */
                /* ok */
                break;
        } /* ptcache_type */
    } /* BCMPTM_FOR_ITER */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_ctr_info_get(int unit, bcmdrd_sid_t sid,
                            bcmptm_ptcache_ctr_info_t *ctr_info)
{
    uint32_t csid; /* compact, contiguous sid space */
    sinfo_t sinfo_word;
    xinfo_t xinfo_word;
    uint32_t vinfo_cw_index, vinfo_cw_index_vs, vinfo_cw_index_is, iw_base_index;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(ptcache_sid_chk(unit, sid));
    SHR_NULL_CHECK(ctr_info, SHR_E_PARAM);
    ctr_info->pt_is_ctr = FALSE;

    /* Read sinfo_word */
    csid = sid;
    sinfo_word.entry = SINFO_DATA(csid);
    switch (sinfo_word.f.ptcache_type) {
        case PTCACHE_TYPE_CCI_ONLY:
            ctr_info->pt_is_ctr = TRUE;
            ctr_info->ctr_is_cci_only = TRUE;
            ctr_info->cci_map_id = sinfo_word.f.vinfo_cw_index;
            break;
        case PTCACHE_TYPE_ME_CCI:
            if (!sinfo_word.f.xinfo_en) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            vinfo_cw_index = sinfo_word.f.vinfo_cw_index; /* cw0 */

            /* Read xinfo_word */
            xinfo_word.entry = VINFO_DATA(vinfo_cw_index);

            if (!xinfo_word.fme.info_seg_en) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            vinfo_cw_index_vs = vinfo_cw_index + xinfo_word.fme.cw_offset_vs;
                /* cw pointed by LHS contains offset for valid_seg */
            vinfo_cw_index_is = (vinfo_cw_index_vs + 1) + /* cw after cw_index_vs */
                                xinfo_word.fme.data_seg_en +
                                xinfo_word.fme.ltid_seg_en +
                                (xinfo_word.fme.dfid_seg_en? 1 : 0);

            iw_base_index = VINFO_DATA(vinfo_cw_index_is);

            ctr_info->pt_is_ctr = TRUE;
            ctr_info->ctr_is_cci_only = FALSE;
            ctr_info->cci_map_id = VINFO_DATA(iw_base_index);
            break;
        default:
            ctr_info->pt_is_ctr = FALSE;
            ctr_info->ctr_is_cci_only = FALSE;
            ctr_info->cci_map_id = 0;
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_read(int unit,
                    bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    void *ovrr_info,
                    uint32_t entry_count,
                    size_t rsp_entry_wsize,
                    uint32_t *rsp_entry_words,
                    bool *rsp_entry_cache_vbit,
                    uint16_t *rsp_entry_dfid,
                    bcmltd_sid_t *rsp_entry_ltid)
{
    uint32_t csid, pt_index_count, w_csid = 0;
    uint32_t vinfo_cw_index, vinfo_cw_index_vs, w_vinfo_cw_index_vs = 0;
    uint32_t vinfo_ds_index, vinfo_vs_index, w_vinfo_vs_index, entry_num;
    uint32_t index = 0, entry_valid = 0;
    int tbl_inst = 0;
    uint32_t entry_wsize, w_entry_wsize;
    uint32_t vinfo_ltid_seg_index, vinfo_dfid_seg_index;
    sinfo_t sinfo_word, w_sinfo_word;
    xinfo_t xinfo_word, w_xinfo_word;
    bool ltid_size16b = FALSE;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(ovrr_info, SHR_E_PARAM); ok to be NULL */
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_cache_vbit, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_dfid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_ltid, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(entry_count == 0 ? SHR_E_PARAM : SHR_E_NONE);

    /* defaults */
    *rsp_entry_cache_vbit = FALSE;
    *rsp_entry_ltid = BCMPTM_LTID_RSP_NOCACHE;
    *rsp_entry_dfid = BCMPTM_DFID_RSP_NOCACHE;
    SHR_IF_ERR_EXIT(ptcache_sid_chk(unit, sid));

    csid = sid;
    sinfo_word.entry = SINFO_DATA(csid);
    vinfo_cw_index = sinfo_word.f.vinfo_cw_index;

    if (VINFO_DATA(PTCACHE_VINFO_INDEX_FLAGS_WORD) & PTCACHE_FLAGS_LTID_SIZE16) {
        ltid_size16b = TRUE;
    }

    switch (sinfo_word.f.ptcache_type) {
        case PTCACHE_TYPE_ME:
        case PTCACHE_TYPE_ME_CCI:
            
            SHR_IF_ERR_EXIT((ovrr_info != NULL)? SHR_E_INTERNAL : SHR_E_NONE);

            if (sinfo_word.f.tbl_inst_en) {
                if (dyn_info->tbl_inst >= 0) {
                    tbl_inst = dyn_info->tbl_inst;
                    SHR_IF_ERR_EXIT(
                        (tbl_inst >= bcmdrd_pt_num_tbl_inst(unit, sid))?
                        SHR_E_INTERNAL : SHR_E_NONE);
                } /* else tbl_inst = 0; --ANY_COPY for reads */
            }
            if (sinfo_word.f.index_en && (dyn_info->index > 0)) {
                index = dyn_info->index;
            }

            pt_index_count = bcmptm_pt_index_count(unit, sid);
            SHR_IF_ERR_EXIT((index >= pt_index_count)? SHR_E_PARAM
                                                     : SHR_E_NONE);
            SHR_IF_ERR_EXIT(
                ((index + entry_count - 1) >
                 (uint32_t)bcmdrd_pt_index_max(unit, sid)) ? SHR_E_PARAM
                                                           : SHR_E_NONE);
            entry_num = (tbl_inst * pt_index_count) + index;

            if (sinfo_word.f.xinfo_en) {
                /* Read xinfo_word */
                xinfo_word.entry = VINFO_DATA(vinfo_cw_index);
                vinfo_cw_index_vs = vinfo_cw_index +
                                    xinfo_word.fme.cw_offset_vs;

                
                if (xinfo_word.fme.overlay_mode || xinfo_word.fme.ltid_seg_en ||
                    xinfo_word.fme.dfid_seg_en) {
                    SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_UNAVAIL
                                                     : SHR_E_NONE);
                }
            } else {
                xinfo_word.entry = 0;
                vinfo_cw_index_vs = vinfo_cw_index;
            }
            vinfo_vs_index = VINFO_DATA(vinfo_cw_index_vs);
                /* LHS points to 1st word in valid_seg */

            vinfo_vs_index += entry_num / 32;
            if (BCMPTM_WORD_BMAP_TST(VINFO_DATA(vinfo_vs_index), entry_num)) {
                entry_valid = 1;
            }

            if (sinfo_word.f.xinfo_en && xinfo_word.fme.overlay_mode) {
                ptcache_oinfo_w0_t oinfo_w0;
                ptcache_oinfo_wn_t oinfo_w1;
                uint32_t w_vinfo_cw_index, w_vinfo_ds_index, w_sbit, w_ebit,
                         ne_entry_wsize;

                /* Always return the data from widest_entry even if:
                 * a. narrow_entry_valid = 0
                 * b. widest_entry_valid = 0
                 */

                /* req entry size */
                entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

                /* Read oinfo_w0,1 for narrow_sid */
                oinfo_w0.entry = VINFO_DATA(vinfo_cw_index + 1);
                oinfo_w1.entry = VINFO_DATA(vinfo_cw_index + 2);

                /* Narrow entry_size */
                ne_entry_wsize = BCMPTM_BITS2WORDS(oinfo_w1.f.width);

                /* Read sinfo for widest_sid */
                w_csid = oinfo_w0.f.sid;
                w_sinfo_word.entry = SINFO_DATA(w_csid);
                w_vinfo_cw_index = w_sinfo_word.f.vinfo_cw_index;
                w_entry_wsize = bcmdrd_pt_entry_wsize(unit, w_csid);

                SHR_IF_ERR_EXIT((entry_wsize > rsp_entry_wsize)?
                                SHR_E_INTERNAL : SHR_E_NONE);

                /* ptr to 1st word in data_seg of widest_view */
                if (w_sinfo_word.f.xinfo_en) {
                    w_xinfo_word.entry = VINFO_DATA(w_vinfo_cw_index);
                    w_vinfo_cw_index_vs = w_vinfo_cw_index +
                                          w_xinfo_word.fme.cw_offset_vs;
                    SHR_IF_ERR_EXIT(!w_xinfo_word.fme.data_seg_en?
                                    SHR_E_INTERNAL : SHR_E_NONE);
                } else {
                    w_xinfo_word.entry = 0;
                    w_vinfo_cw_index_vs = w_vinfo_cw_index;
                }
                w_vinfo_ds_index = VINFO_DATA(w_vinfo_cw_index_vs + 1);

                /* Few sanity checks for widest_view */
                if (w_sinfo_word.f.ptcache_type != PTCACHE_TYPE_ME ||
                    (ne_entry_wsize > entry_wsize) ||
                    (w_sinfo_word.f.tbl_inst_en != sinfo_word.f.tbl_inst_en) ||
                    w_xinfo_word.fme.overlay_mode
                   ) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }

                /* Extract narrow_entry data from wide_entry */
                if (xinfo_word.fme.overlay_mode == BCMPTM_OINFO_MODE_HASH) {
                    uint32_t w_index;
                    /* Note: we are not checking validity of w_entry on purpose
                     *       For eg: even if wide_entry invalid, any/all narrow
                     *               entry/entries on that wide_row could be valid.
                     */

                    /* Retrieve entry info for widest view */
                    SHR_IF_ERR_EXIT(
                        ptcache_w_entry_calc(unit,
                                             &oinfo_w0, &oinfo_w1,
                                             /* sinfo_word.f.tbl_inst_en, */
                                                /* same for narrow,wide views */
                                             tbl_inst,
                                                /* same for narrow,wide views */
                                             index,
                                                /* in tbl_inst of narrow view */
                                             w_entry_wsize,

                                             &w_index, &w_sbit, &w_ebit,
                                             &w_vinfo_ds_index));

                    /* Extract the narrow entry data (without hit bits)
                     * from wide entry into rsp buffer */
                    bcmdrd_field_get(VINFO_PTR + w_vinfo_ds_index,
                                     w_sbit, w_ebit, rsp_entry_words);

                    /* For debug display below */
                    vinfo_ds_index = w_vinfo_ds_index;
                } else { /* overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA */
                    /* Retrieve narrow_entry from wide_entry */
                    SHR_IF_ERR_EXIT(
                        ptcache_overlay_tcam_data_read(
                            unit,
                            &oinfo_w0,
                            (vinfo_cw_index + 2),
                               /* overlay cw (for narrow entry view) that holds
                                * 1st (width, start_bit) pair for narrow entry */
                            tbl_inst,
                               /* same for narrow,wide views */
                            index,
                               /* in tbl_inst of narrow view */
                            w_entry_wsize,

                            rsp_entry_words,
                            &w_vinfo_ds_index));

                    /* For debug display below */
                    vinfo_ds_index = w_vinfo_ds_index;

                    /* For Tcam_data type overlay, valid bit of narrow entry is
                     * not meaningful. Only valid bit of parent wide entry is
                     * used.
                     * Also, for Tcam_data type overlays, entry_num of wide
                     * entry is same as that for narrow entry */
                    w_vinfo_vs_index = VINFO_DATA(w_vinfo_cw_index_vs);
                        /* LHS points to 1st word in valid_seg */

                    w_vinfo_vs_index += entry_num / 32;
                    if (BCMPTM_WORD_BMAP_TST(VINFO_DATA(w_vinfo_vs_index), entry_num)) {
                        entry_valid = 1;
                    } else {
                        entry_valid = 0;
                    }

                    /* For debug display below */
                    vinfo_vs_index = w_vinfo_vs_index;

                } /* BCMPTM_OINFO_MODE_TCAM_DATA */

            } else { /* non_xinfo or non_overlay mem */
                if (sinfo_word.f.xinfo_en) { /* xinfo_en but !overlay */
                    SHR_IF_ERR_EXIT(!xinfo_word.fme.data_seg_en?
                                    SHR_E_INTERNAL : SHR_E_NONE);

                }
                /* else: non_xinfo => normal case (only vs, ds)
                 */

                entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
                SHR_IF_ERR_EXIT(
                    ((entry_count * entry_wsize) > rsp_entry_wsize) ?
                    SHR_E_INTERNAL : SHR_E_NONE);

                vinfo_ds_index = VINFO_DATA(vinfo_cw_index_vs + 1);
                    /* LHS points to 1st word of 1st_entry in data_seg */
                vinfo_ds_index += entry_num * entry_wsize;
                sal_memcpy(rsp_entry_words,
                           VINFO_PTR + vinfo_ds_index,
                           4 * entry_wsize * entry_count);
            } /* non_xinfo or non_overlay mem */

            if (entry_valid == 1) { /* strict checking */
                *rsp_entry_cache_vbit = TRUE;

                BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "MemData read from cache... for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "overlay_mode=%0d, vinfo_vs_index=%0d, "
                                "vinfo_ds_index=%0d\n"),
                     sid, tbl_inst, index, xinfo_word.fme.overlay_mode,
                     vinfo_vs_index, vinfo_ds_index));

                /* rsp LTID */
                if (sinfo_word.f.xinfo_en &&
                    (xinfo_word.fme.overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    (w_sinfo_word.f.xinfo_en && w_xinfo_word.fme.ltid_seg_en)) {
                    vinfo_ltid_seg_index = VINFO_DATA(w_vinfo_cw_index_vs + 1 +
                                                      w_xinfo_word.fme.data_seg_en);
                    /* LHS points to 1st word of 1st entry in ltid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_get(unit,
                                         w_csid, /* for display only */
                                         ltid_size16b,
                                         entry_num,
                                         vinfo_ltid_seg_index,
                                         rsp_entry_ltid));
                } else if (sinfo_word.f.xinfo_en && xinfo_word.fme.ltid_seg_en) {
                    /* For non_overlay case, we must check ltid_seg_en - Normal
                     * For overlay_hash case, ltid is stored separately for
                     * For overlay_tcam case, we are guaranteed that ltid_seg_en
                     * for narrow view will be 0 - so we cannot be here */
                    vinfo_ltid_seg_index = VINFO_DATA(vinfo_cw_index_vs + 1 +
                                                      xinfo_word.fme.data_seg_en);
                    /* LHS points to 1st word of 1st entry in ltid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_get(unit,
                                         csid, /* for display only */
                                         ltid_size16b,
                                         entry_num,
                                         vinfo_ltid_seg_index,
                                         rsp_entry_ltid));
                } else {
                    *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
                } /* ltid_not_supported */

                /* rsp DFID */
                if (sinfo_word.f.xinfo_en &&
                    (xinfo_word.fme.overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    (w_sinfo_word.f.xinfo_en && w_xinfo_word.fme.dfid_seg_en)) {
                    vinfo_dfid_seg_index = VINFO_DATA(w_vinfo_cw_index_vs + 1 +
                                                      w_xinfo_word.fme.data_seg_en +
                                                      w_xinfo_word.fme.ltid_seg_en);
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_get(unit,
                                         w_csid, /* for display only */
                                         w_xinfo_word.fme.dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_dfid_seg_index,
                                         rsp_entry_dfid));
                } else if (sinfo_word.f.xinfo_en && xinfo_word.fme.dfid_seg_en) {
                    vinfo_dfid_seg_index =
                        VINFO_DATA(vinfo_cw_index_vs + 1 +
                                   xinfo_word.fme.data_seg_en +
                                   xinfo_word.fme.ltid_seg_en);
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_get(unit,
                                         csid, /* for display only */
                                         xinfo_word.fme.dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_dfid_seg_index,
                                         rsp_entry_dfid));
                } else {
                    *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
                } /* dfid_not_supported */

            } else { /* entry is not valid in cache */
                BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "MemData not valid in cache... for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "overlay_mode=%0d, vinfo_vs_index=%0d, "
                                "vinfo_ds_index=%0d, "
                                "vinfo_ltid_seg_index=%0d\n, "
                                "vinfo_dfid_seg_index=%0d\n"),
                     sid, tbl_inst, index, xinfo_word.fme.overlay_mode,
                     vinfo_vs_index, vinfo_ds_index, -1, -1));
                if (sinfo_word.f.xinfo_en && xinfo_word.fme.ltid_seg_en) {
                    *rsp_entry_ltid = BCMPTM_LTID_RSP_INVALID;
                } else {
                    *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
                } /* ltid_not_supported */

                if (sinfo_word.f.xinfo_en && xinfo_word.fme.dfid_seg_en) {
                    *rsp_entry_dfid = BCMPTM_DFID_RSP_INVALID;
                } else {
                    *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
                } /* dfid_not_supported */
            }
            break; /* PTCACHE_TYPE_ME, ME_CCI */

        case PTCACHE_TYPE_SE_VALID:
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            /* vinfo_cw_index points to 1st data word */
            entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
            SHR_IF_ERR_EXIT((entry_wsize > rsp_entry_wsize)?
                            SHR_E_INTERNAL : SHR_E_NONE);
            sal_memcpy(rsp_entry_words,
                       VINFO_PTR + vinfo_cw_index,
                       4 * entry_wsize);
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "RegData read from cache... for sid=%0d, "
                            "tbl_inst=%0d, index=%0d, vinfo_index=%0d\n"),
                 sid, tbl_inst, index, vinfo_cw_index));
            *rsp_entry_cache_vbit = TRUE;
            
            *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
            *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
            break;

        case PTCACHE_TYPE_SE_NOT_VALID:
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            /* Because we have initialized cache with HW default value,
             * data in cache is still correct */
            entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
            SHR_IF_ERR_EXIT((entry_wsize > rsp_entry_wsize)?
                            SHR_E_INTERNAL : SHR_E_NONE);
            sal_memcpy(rsp_entry_words,
                       VINFO_PTR + vinfo_cw_index,
                       4 * entry_wsize);
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "RegData not valid in cache... for sid=%0d, "
                            "tbl_inst=%0d, index=%0d, vinfo_index=%0d\n"),
                 sid, tbl_inst, index, vinfo_cw_index));
            *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
            *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
            break;

        default: /* PTCACHE_TYPE_NO_CACHE, PTCACHE_TYPE_CCI_ONLY */
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid not cached ... for sid=%0d, "
                            "tbl_inst=%0d, index=%0d\n"),
                 sid, tbl_inst, index));
            *rsp_entry_ltid = BCMPTM_LTID_RSP_NOCACHE;
            *rsp_entry_dfid = BCMPTM_DFID_RSP_NOCACHE;
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/* Return values:
 *     SHR_E_INTERNAL - nonovrr_info != NULL,
 *                      tbl_inst_max < tbl_inst_min when dyn_info.tbl_inst=-1
 *     SHR_E_PARAM
 */
int
bcmptm_ptcache_update(int unit,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      void *ovrr_info,
                      uint32_t entry_count,
                      uint32_t *entry_words,
                      bool set_cache_vbit,
                      bcmltd_sid_t req_ltid,
                      uint16_t req_dfid)
{
    uint32_t csid, pt_index_count, w_csid = 0;
    uint32_t vinfo_cw_index, vinfo_cw_index_vs, w_vinfo_cw_index_vs = 0;
    uint32_t vinfo_ds_index, vinfo_vs_index, w_vinfo_vs_index, entry_num;
    uint32_t index = 0;
    int tbl_inst = 0, tbl_inst_min = 0, tbl_inst_max = 0;
    uint32_t entry_wsize;
    uint32_t vinfo_ltid_seg_index, vinfo_dfid_seg_index;
    sinfo_t sinfo_word, w_sinfo_word;
    xinfo_t xinfo_word, w_xinfo_word;
    bool ltid_size16b = FALSE;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(ovrr_info, SHR_E_PARAM); ok to be NULL */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(ptcache_sid_chk(unit, sid));
    SHR_IF_ERR_EXIT(entry_count == 0 ? SHR_E_PARAM : SHR_E_NONE);
    csid = sid;
    sinfo_word.entry = SINFO_DATA(csid);
    vinfo_cw_index = sinfo_word.f.vinfo_cw_index;

    if (VINFO_DATA(PTCACHE_VINFO_INDEX_FLAGS_WORD) & PTCACHE_FLAGS_LTID_SIZE16) {
        ltid_size16b = TRUE;
    }

    switch (sinfo_word.f.ptcache_type) {
        case PTCACHE_TYPE_ME:
        case PTCACHE_TYPE_ME_CCI:
            
            SHR_IF_ERR_EXIT((ovrr_info != NULL)? SHR_E_INTERNAL : SHR_E_NONE);

            if (sinfo_word.f.tbl_inst_en) {
                TBL_INST_MAX_SET(unit, sid);
                if (dyn_info->tbl_inst >= 0) { /* SINGLE INST WRITE */
                    SHR_IF_ERR_EXIT(
                        (dyn_info->tbl_inst > tbl_inst_max) ?
                        SHR_E_PARAM : SHR_E_NONE);
                    tbl_inst_min = tbl_inst_max = dyn_info->tbl_inst;
                } else { /* WRITE_ALL_COPIES */
                    tbl_inst_min = 0;
                }
            }
            if (sinfo_word.f.index_en && (dyn_info->index > 0)) {
                index = dyn_info->index;
            }

            
            for (tbl_inst = tbl_inst_min; tbl_inst <= tbl_inst_max; tbl_inst++) {
                pt_index_count = bcmptm_pt_index_count(unit, sid);
                SHR_IF_ERR_EXIT((index >= pt_index_count) ? SHR_E_PARAM
                                                          : SHR_E_NONE);
                SHR_IF_ERR_EXIT(
                    ((index + entry_count - 1) >
                     (uint32_t)bcmdrd_pt_index_max(unit, sid)) ?
                    SHR_E_PARAM : SHR_E_NONE);
                entry_num = (tbl_inst * pt_index_count) + index;

                entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

                if (sinfo_word.f.xinfo_en) {
                    /* Read xinfo_word */
                    xinfo_word.entry = VINFO_DATA(vinfo_cw_index);
                    vinfo_cw_index_vs = vinfo_cw_index +
                                        xinfo_word.fme.cw_offset_vs;
                    
                    if (xinfo_word.fme.overlay_mode ||
                        xinfo_word.fme.ltid_seg_en ||
                        xinfo_word.fme.dfid_seg_en) {
                        SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_UNAVAIL
                                                         : SHR_E_NONE);
                    }
                } else {
                    xinfo_word.entry = 0;
                    vinfo_cw_index_vs = vinfo_cw_index;
                }
                vinfo_vs_index = VINFO_DATA(vinfo_cw_index_vs);
                    /* LHS points to 1st word in valid_seg */
                vinfo_vs_index += entry_num / 32;

                /* Need to update the entry_words in cache */
                if (sinfo_word.f.xinfo_en && xinfo_word.fme.overlay_mode) {
                    ptcache_oinfo_w0_t oinfo_w0;
                    ptcache_oinfo_wn_t oinfo_w1;
                    uint32_t w_vinfo_cw_index, w_vinfo_ds_index, w_sbit, w_ebit,
                             ne_entry_wsize, w_entry_wsize;

                    /* Read oinfo_w0,1 for narrow_sid */
                    oinfo_w0.entry = VINFO_DATA(vinfo_cw_index + 1);
                    oinfo_w1.entry = VINFO_DATA(vinfo_cw_index + 2);

                    /* Narrow entry_size */
                    ne_entry_wsize = BCMPTM_BITS2WORDS(oinfo_w1.f.width);

                    /* Read sinfo for widest_sid */
                    w_csid = oinfo_w0.f.sid;
                    w_sinfo_word.entry = SINFO_DATA(w_csid);
                    w_vinfo_cw_index = w_sinfo_word.f.vinfo_cw_index;

                    /* ptr to 1st word in data_seg of widest_view */
                    if (w_sinfo_word.f.xinfo_en) {
                        w_xinfo_word.entry = VINFO_DATA(w_vinfo_cw_index);
                        w_vinfo_cw_index_vs = w_vinfo_cw_index +
                                              w_xinfo_word.fme.cw_offset_vs;
                        SHR_IF_ERR_EXIT(!w_xinfo_word.fme.data_seg_en?
                                        SHR_E_INTERNAL : SHR_E_NONE);
                    } else {
                        w_xinfo_word.entry = 0;
                        w_vinfo_cw_index_vs = w_vinfo_cw_index;
                    }
                    w_vinfo_ds_index = VINFO_DATA(w_vinfo_cw_index_vs + 1);

                    /* Few sanity checks for widest_view */
                    if (w_sinfo_word.f.ptcache_type != PTCACHE_TYPE_ME ||
                        (ne_entry_wsize > entry_wsize) ||
                        (w_sinfo_word.f.tbl_inst_en !=
                         sinfo_word.f.tbl_inst_en) ||
                        w_xinfo_word.fme.overlay_mode
                       ) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }

                    w_entry_wsize = bcmdrd_pt_entry_wsize(unit, w_csid);

                    if (xinfo_word.fme.overlay_mode == BCMPTM_OINFO_MODE_HASH) {
                        uint32_t w_index;
                        /* Retrieve entry info for widest view */
                        SHR_IF_ERR_EXIT(
                            ptcache_w_entry_calc(unit,
                                                 &oinfo_w0, &oinfo_w1,
                                                 /* sinfo_word.f.tbl_inst_en, */
                                                /* same for narrow,wide views */
                                                 tbl_inst,
                                                /* same for narrow,wide views */
                                                 index,
                                                /* in tbl_inst of narrow view */
                                                 w_entry_wsize,

                                                 &w_index, &w_sbit, &w_ebit,
                                                 &w_vinfo_ds_index));

                        /* Stuff narrow entry data into wide entry.
                         * Note: following will not copy Hit bits, etc from
                         *       'entry_words' into wide_cache_entry */
                        bcmdrd_field_set(VINFO_PTR + w_vinfo_ds_index,
                                         w_sbit, w_ebit, entry_words);

                        /* For debug display below */
                        vinfo_ds_index = w_vinfo_ds_index;
                    } else { /* overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA */
                        SHR_IF_ERR_EXIT(
                            ptcache_overlay_tcam_data_write(
                                unit,
                                &oinfo_w0,
                                (vinfo_cw_index + 2),
                                   /* overlay cw (for narrow entry view) that holds
                                    * 1st (width, start_bit) pair for narrow entry */
                                tbl_inst,
                                   /* same for narrow,wide views */
                                index,
                                   /* in tbl_inst of narrow view */
                                w_entry_wsize,

                                entry_words,
                                &w_vinfo_ds_index));

                        /* For debug display below */
                        vinfo_ds_index = w_vinfo_ds_index;

                        /* For Tcam_data type overlay, valid bit of narrow entry is
                         * not meaningful. Only valid bit of parent wide entry is
                         * used.
                         * Also, for Tcam_data type overlays, entry_num of wide
                         * entry is same as that for narrow entry */
                        w_vinfo_vs_index = VINFO_DATA(vinfo_cw_index_vs);
                            /* LHS points to 1st word in valid_seg */
                        w_vinfo_vs_index += entry_num / 32;

                        /* Setting of cache_vbit will be done only for parent
                         * view (see below).
                         * Also helps debug display */
                        vinfo_vs_index = w_vinfo_vs_index;

                    } /* BCMPTM_OINFO_MODE_TCAM_DATA */
                } else { /* non_xinfo or non_overlay */
                    if (sinfo_word.f.xinfo_en) { /* xinfo_en but !overlay */
                        SHR_IF_ERR_EXIT(!xinfo_word.fme.data_seg_en?
                                        SHR_E_INTERNAL : SHR_E_NONE);
                    }
                    /* else: non_xinfo => normal case (only vs, ds)
                     */
                    vinfo_ds_index = VINFO_DATA(vinfo_cw_index_vs + 1);
                    /* LHS points to 1st word of 1st entry in data_seg */
                    vinfo_ds_index += entry_num * entry_wsize;

                    sal_memcpy(VINFO_PTR + vinfo_ds_index,
                               entry_words, 4 * entry_wsize * entry_count);
                } /* non_xinfo or non_overlay mem */

                /* Update valid bit for cache_entry */
                cache_vbit_update(unit, set_cache_vbit,
                                  vinfo_cw_index_vs, /* base for valid seg */
                                  vinfo_vs_index, /* vs_index for first entry */
                                  entry_num, /* first_entry_num */
                                  entry_count);
                BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "MemData written to cache... for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "overlay=%0d, vinfo_vs_index=%0d, "
                                "vinfo_ds_index=%0d\n"),
                     sid, tbl_inst, index, xinfo_word.fme.overlay_mode,
                     vinfo_vs_index, vinfo_ds_index));

                if (set_cache_vbit &&
                    sinfo_word.f.xinfo_en &&
                    (xinfo_word.fme.overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    (w_sinfo_word.f.xinfo_en && w_xinfo_word.fme.ltid_seg_en)) {
                    vinfo_ltid_seg_index = VINFO_DATA(w_vinfo_cw_index_vs + 1 +
                                                      w_xinfo_word.fme.data_seg_en);
                    /* LHS points to 1st word of 1st entry in ltid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_set(unit,
                                         w_csid, /* for display only */
                                         ltid_size16b,
                                         entry_num,
                                         vinfo_ltid_seg_index,
                                         req_ltid));
                } else if (set_cache_vbit &&
                           sinfo_word.f.xinfo_en && xinfo_word.fme.ltid_seg_en) {
                    /* Presence of ltid_seg has no relation to overlay_en.
                     * Even if this SID is an overlay (thin view) of wider mem,
                     * we maintain separate cache_vbit, ltid for this view.
                     *
                     * Also, it is guaranteed that xinfo_en will be 1 if we have
                     * ltid_seg for a SID - so need to check for other
                     * combinations of xinfo_en and ltid_seg_en
                     */
                    /* For non_overlay case, we must check ltid_seg_en - Normal
                     * For overlay_hash case, ltid is stored separately for
                     * For overlay_tcam case, we are guaranteed that ltid_seg_en
                     * for narrow view will be 0 - so we cannot be here */
                    vinfo_ltid_seg_index =
                        VINFO_DATA(vinfo_cw_index_vs + 1 +
                                   xinfo_word.fme.data_seg_en);
                    /* LHS points to 1st word of 1st entry in ltid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_set(unit,
                                         csid, /* for display only */
                                         ltid_size16b,
                                         entry_num,
                                         vinfo_ltid_seg_index,
                                         req_ltid));
                } /* ltid_seg_en */

                if (set_cache_vbit &&
                    sinfo_word.f.xinfo_en &&
                    (xinfo_word.fme.overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    (w_sinfo_word.f.xinfo_en && w_xinfo_word.fme.dfid_seg_en)) {
                    vinfo_dfid_seg_index = VINFO_DATA(w_vinfo_cw_index_vs + 1 +
                                                      w_xinfo_word.fme.data_seg_en +
                                                      w_xinfo_word.fme.ltid_seg_en);
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_set(unit,
                                         w_csid, /* for display only */
                                         w_xinfo_word.fme.dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_dfid_seg_index,
                                         req_dfid));
                } else if (set_cache_vbit &&
                    /* Presence of dfid_seg has no relation to overlay_en.
                     * Even if this SID is an overlay (thin view) of wider mem,
                     * we maintain separate cache_vbit, dfid for this view.
                     *
                     * Also, it is guaranteed that xinfo_en will be 1 if we have
                     * dfid_seg for a SID - so need to check for other
                     * combinations of xinfo_en and dfid_seg_en
                     */
                    sinfo_word.f.xinfo_en && xinfo_word.fme.dfid_seg_en) {
                    vinfo_dfid_seg_index =
                        VINFO_DATA(vinfo_cw_index_vs + 1 +
                                   xinfo_word.fme.data_seg_en +
                                   xinfo_word.fme.ltid_seg_en);
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_set(unit,
                                         csid, /* for display only */
                                         xinfo_word.fme.dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_dfid_seg_index,
                                         req_dfid));
                } /* dfid_seg_en */

            } /* for each tbl_inst */
            break; /* PTCACHE_TYPE_ME, ME_CCI */

        case PTCACHE_TYPE_SE_NOT_VALID:
        case PTCACHE_TYPE_SE_VALID:
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);

            /* vinfo_cw_index points to 1st data word */
            entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
            sal_memcpy(VINFO_PTR + vinfo_cw_index,
                       entry_words,
                       4 * entry_wsize);

            BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "RegData written to cache... for "
                            "sid=%0d, tbl_inst=%0d, index=%0d, "
                            "vinfo_cw_index=%0d\n"),
                 sid, tbl_inst, index, vinfo_cw_index));

            /* Valid bit for cache_entry */
            sinfo_word.f.ptcache_type = set_cache_vbit?
                PTCACHE_TYPE_SE_VALID : PTCACHE_TYPE_SE_NOT_VALID;
            SINFO_DATA(csid) = sinfo_word.entry;

            break; /* PTCACHE_TYPE_SE */

        default: /* PTCACHE_TYPE_NO_CACHE, PTCACHE_TYPE_CCI_ONLY */
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid not cached ... for sid=%0d, "
                            "tbl_inst=%0d, index=%0d\n"),
                 sid, tbl_inst, index));
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_update_ireq(int unit,
                           bcmdrd_sid_t sid,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           void *ovrr_info,
                           uint32_t *entry_words,
                           bool set_cache_vbit,
                           bcmltd_sid_t req_ltid,
                           uint16_t req_dfid)
{
    int tmp_rv;
    uint32_t tmp_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    bcmltd_sid_t tmp_rsp_ltid, update_ltid;
    uint16_t tmp_rsp_dfid, update_dfid;
    bool tmp_rsp_cache_vbit = FALSE;
    uint32_t entry_count = 1;
    SHR_FUNC_ENTER(unit);

    

    /* First we must read previous state of entry in cache */
    tmp_rv = bcmptm_ptcache_read(unit,
                                 sid,
                                 dyn_info,
                                 ovrr_info,
                                 entry_count,
                                 COUNTOF(tmp_entry_words),
                                 tmp_entry_words, /* dont_care */
                                 &tmp_rsp_cache_vbit,
                                 &tmp_rsp_dfid,
                                 &tmp_rsp_ltid);
    SHR_IF_ERR_EXIT_EXCEPT_IF(tmp_rv, SHR_E_UNAVAIL);
    if (tmp_rv == SHR_E_UNAVAIL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        /* silently because it is ok if cache is not alloc for this sid */
    }

    if (tmp_rsp_cache_vbit) { /* entry is valid in cache */
        if (tmp_rsp_ltid >= BCMPTM_LTID_RSVD) {
           update_ltid = req_ltid; /* allow over-write */
        } else {
           update_ltid = tmp_rsp_ltid; /* preserve (managed by RM) */
        }

        if (tmp_rsp_dfid >= BCMPTM_DFID_IREQ) {
           update_dfid = req_dfid; /* over-write */
        } else {
           update_dfid = tmp_rsp_dfid; /* preserve (managed by RM) */
        }
    } else { /* entry is not valid in cache, allow over-write */
        update_ltid = req_ltid;
        update_dfid = req_dfid;
    }
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_update(unit,
                              sid,
                              dyn_info,
                              ovrr_info,
                              entry_count,
                              entry_words, /* km_entry */
                              TRUE,        /* Cache_valid_set */
                              update_ltid,
                              update_dfid));
exit:
    SHR_FUNC_EXIT();
}

/* Return values:
 *     SHR_E_PARAM
 *     SHR_E_UNAVAIL  - If cache is not alloc for this pt
 *     SHR_E_INTERNAL - nonovrr_info != NULL,
 *                      tbl_inst_max < tbl_inst_min when dyn_info.tbl_inst=-1
 *     SHR_E_NONE - cache is allocated for this pt, wr cache and set valid
 */
int
bcmptm_ptcache_tc_cmd(int unit,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      void *ovrr_info,
                      uint32_t tc_cmd,
                      uint32_t *prev_tc_rdata)
{
    uint32_t csid, pt_index_count;
    uint32_t vinfo_cw_index, vinfo_cw_index_vs, vinfo_cw_index_tcs;
    uint32_t vinfo_tc_index, entry_num;
    int tbl_inst = 0, tbl_inst_min = 0, tbl_inst_max = 0;
    uint32_t index = 0;
    sinfo_t sinfo_word;
    xinfo_t xinfo_word;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(ovrr_info, SHR_E_PARAM); ok to be NULL */
    SHR_NULL_CHECK(prev_tc_rdata, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(ptcache_sid_chk(unit, sid));

    /* Read sinfo_word */
    csid = sid;
    sinfo_word.entry = SINFO_DATA(csid);
    vinfo_cw_index = sinfo_word.f.vinfo_cw_index;
    if (!sinfo_word.f.xinfo_en) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL); /* tc_seg is not avail */
    }

    /* Read xinfo_word */
    xinfo_word.entry = VINFO_DATA(vinfo_cw_index);
    if (sinfo_word.f.ptcache_type != PTCACHE_TYPE_ME ||
        !xinfo_word.fme.tc_seg_en) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL); /* tc_seg is not avail */
    }
    vinfo_cw_index_vs = vinfo_cw_index + xinfo_word.fme.cw_offset_vs;
        /* cw pointed by LHS contains offset for valid_seg */

    /* Compute vinfo_cw_index_tcs */
    vinfo_cw_index_tcs = (vinfo_cw_index_vs + 1) + /* cw after cw_index_vs */
                         xinfo_word.fme.data_seg_en +
                         xinfo_word.fme.ltid_seg_en +
                         (xinfo_word.fme.dfid_seg_en? 1 : 0) +
                         xinfo_word.fme.info_seg_en;

    
    SHR_IF_ERR_EXIT((ovrr_info != NULL)? SHR_E_INTERNAL : SHR_E_NONE);

    if (sinfo_word.f.tbl_inst_en) {
        TBL_INST_MAX_SET(unit, sid);
        if (dyn_info->tbl_inst >= 0) { /* SINGLE INST WRITE */
            SHR_IF_ERR_EXIT(
                (dyn_info->tbl_inst > tbl_inst_max) ?
                SHR_E_PARAM : SHR_E_NONE);
            tbl_inst_min = tbl_inst_max = dyn_info->tbl_inst;
        } else { /* WRITE_ALL_COPIES */
            tbl_inst_min = 0;
        }
    }
    if (sinfo_word.f.index_en && (dyn_info->index > 0)) {
        index = dyn_info->index;
    }

    for (tbl_inst = tbl_inst_min; tbl_inst <= tbl_inst_max; tbl_inst++) {
        pt_index_count = bcmptm_pt_index_count(unit, sid);
        SHR_IF_ERR_EXIT((index >= pt_index_count)? SHR_E_INTERNAL
                                                 : SHR_E_NONE);
        entry_num = (tbl_inst * pt_index_count) + index;

        vinfo_tc_index = VINFO_DATA(vinfo_cw_index_tcs);
                /* above points to 1st word in tc_seg */
        vinfo_tc_index += entry_num / 32;

        *prev_tc_rdata = BCMPTM_WORD_BMAP_TST(VINFO_DATA(vinfo_tc_index),
                                              entry_num);
        switch (tc_cmd) {
            case BCMPTM_TCAM_CORRUPT_CMD_SET:
                BCMPTM_WORD_BMAP_SET(VINFO_DATA(vinfo_tc_index), entry_num);
                BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TCAM_Corrupt bit SET...for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "vinfo_tc_index=%0d, previous value = "
                                "%0d\n"),
                     sid, tbl_inst, index, vinfo_tc_index,
                     (*prev_tc_rdata? 1 : 0)));
                break;
            case BCMPTM_TCAM_CORRUPT_CMD_CLR:
                BCMPTM_WORD_BMAP_CLR(VINFO_DATA(vinfo_tc_index), entry_num);
                BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TCAM_Corrupt bit CLR...for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "vinfo_tc_index=%0d, previous value = "
                                "%0d\n"),
                     sid, tbl_inst, index, vinfo_tc_index,
                     (*prev_tc_rdata? 1 : 0)));
                break;
            default: /* GET */
                break;
        }
    } /* for each tbl_inst */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_iw_base(int unit,
                       bcmdrd_sid_t sid,
                       uint32_t *iw_base_index)
{
    uint32_t csid, vinfo_cw_index, vinfo_cw_index_vs, vinfo_cw_index_is;
    sinfo_t sinfo_word;
    xinfo_t xinfo_word;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(ptcache_sid_chk(unit, sid));
    SHR_NULL_CHECK(iw_base_index, SHR_E_PARAM);

    /* Read sinfo_word */
    csid = sid;
    sinfo_word.entry = SINFO_DATA(csid);
    vinfo_cw_index = sinfo_word.f.vinfo_cw_index; /* cw0 */
    if (!sinfo_word.f.xinfo_en) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL); /* info_seg is not avail */
    }

    /* Read xinfo_word */
    xinfo_word.entry = VINFO_DATA(vinfo_cw_index);
    if (sinfo_word.f.ptcache_type != PTCACHE_TYPE_ME ||
        !xinfo_word.fme.info_seg_en) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL); /* info_seg is not avail */
    }
    vinfo_cw_index_vs = vinfo_cw_index + xinfo_word.fme.cw_offset_vs;
        /* cw pointed by LHS contains offset for valid_seg */
    vinfo_cw_index_is = (vinfo_cw_index_vs + 1) + /* cw after cw_index_vs */
                        xinfo_word.fme.data_seg_en +
                        xinfo_word.fme.ltid_seg_en +
                        (xinfo_word.fme.dfid_seg_en? 1 : 0);

    *iw_base_index = VINFO_DATA(vinfo_cw_index_is);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "iw_base: sid=%0d, pt=%s, "
                    "vinfo_cw_index=%0d, iw_base_index=%0d\n"),
         sid, bcmdrd_pt_sid_to_name(unit, sid),
         vinfo_cw_index, *iw_base_index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_iw_read(int unit,
                       uint32_t iw_index,
                       uint32_t *iw_rdata)
{
    if (VINFO_PTR != NULL && iw_rdata != NULL) {
        *iw_rdata = VINFO_DATA(iw_index);
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "iw_read: iw_index=%0d, iw_rdata=0x%8x\n"),
             iw_index, *iw_rdata));
        return SHR_E_NONE;
    }
    return SHR_E_PARAM;
}

int
bcmptm_ptcache_iw_write(int unit,
                        uint32_t iw_index,
                        uint32_t iw_wdata)
{
    if (VINFO_PTR != NULL) {
        VINFO_DATA(iw_index) = iw_wdata;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "iw_write: iw_index=%0d, iw_wdata=0x%8x\n"),
             iw_index, iw_wdata));
        return SHR_E_NONE;
    }
    return SHR_E_UNAVAIL;
}

#if 0
   Implementation notes:
   - DFID not supported for SE type entries
   - LTID not supported for SE type entries

   - If we support LTID for registers:
     sinfo_word.f.vinfo_cw_index - 2; /* Sentinel word, if LTID is enabled */
     sinfo_word.f.vinfo_cw_index - 1; /* Sentinel word, if LTID is disabled */
     sinfo_word.f.vinfo_cw_index - 1; /* LTID word, if enabled */
     sinfo_word.f.vinfo_cw_index;     /* First 32 bits of data */

   - ptcache_read() behavior
cache_vbit, rsp_ltid, Means
   0,         rsvd,   entry is not valid in cache, so we dont know ltid for this entry
   1,         rsvd,   entry is valid in cache, rsvd_ltid means this table does not support ltid
   1,         else,   entry is valid in cache, this is ltid

cache_vbit, rsp_dfid, Means
   0,         rsvd,   entry is not valid in cache, so we dont know dfid for this entry
   1,         rsvd,   entry is valid in cache, rsvd_dfid means this table does not support dfid
   1,         else,   entry is valid in cache, this is dfid


    if (SINFO_PTR == NULL) {
       SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL); /* without display */
    }
    SHR_NULL_CHECK(VINFO_PTR, SHR_E_UNAVAIL);
#endif
