/*! \file bcmptm_ptcache_tcam.c
 *
 * Tcam related utilities for PTM, ptcache
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Find minbit, width for user-specified (key, mask) fields
 * \n Note: Uses string based search - so not recommended for run-time.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] keyf Name of key field
 * \param [in] maskf Name of corresponding mask field
 *
 * \param [out] info_word_key field info word for key
 * \param [out] info_word_mask field info word for mask
 *
 * \retval SHR_E_NONE Success. User-specified keyf, maskf exist for this sid
 * \retval !SHR_E_NONE Not Found. User-specified keyf, maskf do not exist for
 * this sid.
 */
static int
tcam_km_fi_get(int unit,
               bcmdrd_sid_t sid,
               const char *keyf,
               const char *maskf,
               uint32_t *info_word_key,
               uint32_t *info_word_mask)
{
    int minbit, maxbit, tmp_rv;
    uint32_t kwidth, mwidth;
    bcmdrd_fid_t kfid, mfid;
    bcmptm_tcam_iw_fi_t tcam_info_word;
    SHR_FUNC_ENTER(unit);

    /* Searching of field by names MUST NOT BE DONE DURING RUNTIME
     * Must execute this once during init.
     */
    tmp_rv = bcmdrd_pt_field_name_to_fid(unit, sid, keyf, &kfid);
    if (SHR_FAILURE(tmp_rv)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    tmp_rv = bcmdrd_pt_field_name_to_fid(unit, sid, maskf, &mfid);
    if (SHR_FAILURE(tmp_rv)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    /* Must not use SHR_IF_ERR_EXIT() above - because that will also display
     * ERRor message - and this is not really error because if not all tcams
     * have all keyf, maskf named fields
     */

    /* info_word_key */
    SHR_NULL_CHECK(info_word_key, SHR_E_PARAM);
    tcam_info_word.entry = 0;
    minbit = bcmdrd_pt_field_minbit(unit, sid, kfid);
    maxbit = bcmdrd_pt_field_maxbit(unit, sid, kfid);
    SHR_IF_ERR_EXIT((minbit < 0)? SHR_E_FAIL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((minbit > BCMPTM_TCAM_IW_LIMIT_MINBIT)? SHR_E_FAIL
                                                          : SHR_E_NONE);
    SHR_IF_ERR_EXIT((maxbit < minbit)? SHR_E_FAIL : SHR_E_NONE);
    kwidth = maxbit - minbit + 1;
    SHR_IF_ERR_EXIT((kwidth > BCMPTM_TCAM_IW_LIMIT_WIDTH)? SHR_E_FAIL
                                                         : SHR_E_NONE);
    tcam_info_word.fi.type = BCMPTM_TCAM_IW_TYPE_FI_KEY;
    tcam_info_word.fi.minbit = minbit;
    tcam_info_word.fi.width = kwidth;
    *info_word_key = tcam_info_word.entry;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "For sid=%0d, found %s, kminbit=%0d, kwidth=%0d\n"),
         sid, keyf, minbit, kwidth));


    /* info_word_mask */
    SHR_NULL_CHECK(info_word_mask, SHR_E_PARAM);
    tcam_info_word.entry = 0;
    minbit = bcmdrd_pt_field_minbit(unit, sid, mfid);
    maxbit = bcmdrd_pt_field_maxbit(unit, sid, mfid);
    SHR_IF_ERR_EXIT((minbit < 0)? SHR_E_FAIL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((minbit > BCMPTM_TCAM_IW_LIMIT_MINBIT)? SHR_E_FAIL
                                                          : SHR_E_NONE);
    SHR_IF_ERR_EXIT((maxbit < minbit)? SHR_E_FAIL : SHR_E_NONE);
    mwidth = maxbit - minbit + 1;
    SHR_IF_ERR_EXIT((mwidth > BCMPTM_TCAM_IW_LIMIT_WIDTH)? SHR_E_FAIL
                                                         : SHR_E_NONE);
    tcam_info_word.fi.type = BCMPTM_TCAM_IW_TYPE_FI_MASK;
    tcam_info_word.fi.minbit = minbit;
    tcam_info_word.fi.width = mwidth;
    *info_word_mask = tcam_info_word.entry;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "For sid=%0d, found %s, kminbit=%0d, kwidth=%0d\n"),
         sid, maskf, minbit, mwidth));

    SHR_IF_ERR_EXIT((kwidth != mwidth)? SHR_E_FAIL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find and fill minbit, width for ALL key, mask fields for TCAM type PT
 * \n Note: Goes through a pre-defined list of popular KEY, MASK field names.
 * For every new device, we must ensure that this list gets updated.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] fill_mode 0 => only count, 1 => fill and count
 * \param [in] iw_index Start index into info_seg where fi words will be written
 *
 * \param [out] word_count number of words (one word for every field)
 *
 * \retval SHR_E_NONE Success. (word_count is guaranteed to be > 0)
 * \retval !SHR_E_NONE word_count may not be written
 */
static int
tcam_km_fields_set(int unit,
                   bcmdrd_sid_t sid,
                   bool fill_mode,
                   uint32_t iw_index,
                   uint32_t *word_count)
{
    int tmp_rv;
    uint32_t i, j, info_word_key, info_word_mask;
    static const struct {
        char *keyf;
        char *maskf;
    } kmf_list[] = {
        {"KEY0", "MASK0"},
        {"KEY1", "MASK1"},
        /* {"KEY2", "MASK2"}, */
        /* {"KEY3", "MASK3"}, */

        {"KEY0_UPR", "MASK0_UPR"},
        {"KEY1_UPR", "MASK1_UPR"},
        /* {"KEY2_UPR", "MASK2_UPR"}, */
        /* {"KEY3_UPR", "MASK3_UPR"}, */

        {"KEY0_LWR", "MASK0_LWR"},
        {"KEY1_LWR", "MASK1_LWR"},
        /* {"KEY2_LWR", "MASK2_LWR"}, */
        /* {"KEY3_LWR", "MASK3_LWR"}, */

        {"UPR_KEY0", "UPR_MASK0"},
        {"UPR_KEY1", "UPR_MASK1"},

        {"LWR_KEY0", "LWR_MASK0"},
        {"LWR_KEY1", "LWR_MASK1"},

        {"FULL_KEY", "FULL_MASK"}
        /* {"KEY", "MASK"} */
    };
    SHR_FUNC_ENTER(unit);

    assert(bcmdrd_pt_is_valid(unit, sid));
    SHR_NULL_CHECK(word_count, SHR_E_PARAM);

    j = 0; /* count (key, mask) pairs found */
    for (i = 0; i < COUNTOF(kmf_list); i++) {
        tmp_rv = tcam_km_fi_get(unit, sid,
                                kmf_list[i].keyf, kmf_list[i].maskf,
                                &info_word_key, &info_word_mask);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_EXIT(tmp_rv); /* any other failure - problem */

        if (fill_mode) {
            SHR_IF_ERR_EXIT( /* add key word */
                bcmptm_ptcache_iw_write(unit, iw_index++, info_word_key));
            SHR_IF_ERR_EXIT( /* add mask word */
                bcmptm_ptcache_iw_write(unit, iw_index++, info_word_mask));
        }
        j++;
    }
    if (j == 0) { /* if non-std fields not found, check for std fields */
        SHR_IF_ERR_EXIT(
            tcam_km_fi_get(unit, sid, "KEY", "MASK",
                           &info_word_key, &info_word_mask));
            /* SHR_E_NOT_FOUND here means no key, mask fields found
             * for this tcam - error condition !! */

        if (fill_mode) {
            SHR_IF_ERR_EXIT( /* add key word */
                bcmptm_ptcache_iw_write(unit, iw_index++, info_word_key));
            SHR_IF_ERR_EXIT( /* add mask word */
                bcmptm_ptcache_iw_write(unit, iw_index++, info_word_mask));
        }
        j++;
    }

    /* j is > 0 for sure */
    *word_count = j*2; /* j is counting pairs of (key, mask) */
    SHR_IF_ERR_EXIT((*word_count > BCMPTM_TCAM_IW_LIMIT_COUNT)? SHR_E_INTERNAL
                                                              : SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert tcam (key, mask) word to (x,y) format word as needed by tcam
 * \n Note: Meaning of km format:
 * \n       mask bit value of 1 means compare against the key
 * \n       mask bit value of 0 means dont_compare against the key
 *
 * \param [in] unit Logical device id
 * \param [in] tcam_km_format Format of key, mask required by TCAM
 * \param [in] keyp Ptr to key word (will be replaced by x)
 * \param [in] maskp Ptr to mask word (will be replaced by y)
 */
static int
tcam_word_km_to_xy(int unit,
                   bcmptm_pt_tcam_km_format_t tcam_km_format,
                   uint32_t *keyp, /* in, out */
                   uint32_t *maskp) /* in, out */
{
    uint32_t key, mask;
    uint32_t k0, k1, x, y;
    /* SHR_FUNC_ENTER(unit); */

    assert(keyp != NULL);
    assert(maskp != NULL);

    key = *keyp;
    mask = *maskp;

    /*
     * Notes:
     *     - In KM format, mask bit value of 1 means to compare against key
     *     - For writes (K0, K1) need to be written to TCAM
     */
    switch(tcam_km_format) {
        case BCMPTM_TCAM_KM_FORMAT_ONLY_CLEAR_DCD:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = MASK
             *
             *    Decode: KEY = K0 (with info loss)
             *            MASK = K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   0     0   0
             *     1   0     0   0     0   0  =====> info loss
             *     0   1     0   1     0   1
             *     1   1     1   1     1   1
             */
            k0 = mask & key;
            k1 = mask;
            break;

        case BCMPTM_TCAM_KM_FORMAT_40NM_XY:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = ~MASK | KEY
             *
             *    Decode: KEY = K0
             *            MASK = K0 | ~K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   1     0   0
             *     1   0     0   1     0   0  =====> info loss
             *     0   1     0   0     0   1
             *     1   1     1   1     1   1
             */
            k0 = mask & key;
            k1 = ~mask | key;
            break;

        case BCMPTM_TCAM_KM_FORMAT_X_YBAR:
            /*
             *     Encode: K0 = MASK & KEY
             *             K1 = MASK & ~KEY
             *
             *     Decode: KEY = K0
             *             MASK = K0 | K1
             *
             *     KEY MASK   K0  K1   KEY MASK
             *     --------   ------   --------
             *      0   0     0   0     0   0
             *      1   0     0   0     0   0  =====> info loss
             *      0   1     0   1     0   1
             *      1   1     1   0     1   1
             */
            k0 = mask & key;
            k1 = mask & ~key;
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT:
            /* Step1: Same as X_YBAR
             *     Encode: K0 = MASK & KEY
             *             K1 = MASK & ~KEY
             *
             *     Decode: KEY = K0
             *             MASK = K0 | K1
             *
             *     KEY MASK   K0  K1   KEY MASK
             *     --------   ------   --------
             *      0   0     0   0     0   0
             *      1   0     0   0     0   0  =====> info loss
             *      0   1     0   1     0   1
             *      1   1     1   0     1   1
             */
            x = mask & key;  /* k0 */
            y = mask & ~key; /* k1 */

            /* Step2: Convert XY -> LPT
             *
             * lpt_x[1] = !D[1] & M[1] | !D[0] & M[0] = y[1] | y[0];
             * lpt_x[0] =  D[1] & M[1] | !D[0] & M[0] = x[1] | y[0];
             *
             * lpt_y[1] = !D[1] & M[1] |  D[0] & M[0] = y[1] | x[0];
             * lpt_y[0] =  D[1] & M[1] |  D[0] & M[0] = x[1] | x[0];
             */
            k0 = ((y | (y << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((x >> 1) | y) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */

            k1 = ((y | (x << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((x >> 1) | x) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */

            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Write: k=0x%8x, m=0x%8x, lptx=0x%8x, lpty=0x%8x\n"),
                 key, mask, k0, k1));
             */
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT_7NM:
            /* Step 1 */
            x = mask & key;
            y = mask & ~key;

            /* Step 2: Convert XY -> LPT
             *
             * lpt_x[1] =  D[1] & M[1] | !D[0] & M[0] = x[1] | y[0];
             * lpt_x[0] =  D[1] & M[1] |  D[0] & M[0] = x[1] | x[0];
             *
             * lpt_y[1] = !D[1] & M[1] |  D[0] & M[0] = y[1] | x[0];
             * lpt_y[0] = !D[1] & M[1] | !D[0] & M[0] = y[1] | y[0];
             */
            k0 = ((x | (y << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((x >> 1) | x) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */

            k1 = ((y | (x << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((y >> 1) | y) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */
            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Write: k=0x%8x, m=0x%8x, x=0x%8x, y=0x%8x\n"),
                 key, mask, k0, k1));
             */
            break;

        default: /*  BCMPTM_TCAM_KM_FORMAT_IS_KM - no conversion */
            k0 = key;
            k1 = mask;
            break;
    } /* tcam_km_format */

    *keyp = k0;
    *maskp = k1;

/* exit: */
    /* SHR_FUNC_EXIT(); */
    return SHR_E_NONE;
}

/*!
 * \brief Convert tcam (k0, k1) word to (key, mask) format word
 * \n Note: Meaning of km format:
 * \n       mask bit value of 1 means compare against the key
 * \n       mask bit value of 0 means dont_compare against the key
 *
 * \param [in] unit Logical device id
 * \param [in] tcam_km_format Format of key, mask required by TCAM
 * \param [in] k0p Ptr to k0 word (will be replaced by key)
 * \param [in] k1p Ptr to k1 word (will be replaced by mask)
 */
static int
tcam_word_xy_to_km(int unit,
                   bcmptm_pt_tcam_km_format_t tcam_km_format,
                   uint32_t *k0p, /* in, out */
                   uint32_t *k1p) /* in, out */
{
    uint32_t k0, k1, x, y, xbar, ybar;
    /* uint32_t old_k0, old_k1; */
    uint32_t key, mask;
    /* SHR_FUNC_ENTER(unit); */

    assert(k0p != NULL);
    assert(k1p != NULL);
    k0 = *k0p;
    k1 = *k1p;

    /*
     * Notes:
     *     - In KM format, mask bit value of 1 means to compare against key
     *     - For reads (K0, K1) will be returned from TCAM
     */
    switch(tcam_km_format) {
        case BCMPTM_TCAM_KM_FORMAT_ONLY_CLEAR_DCD:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = MASK
             *
             *    Decode: KEY = K0 (with info loss)
             *            MASK = K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   0     0   0
             *     1   0     0   0     0   0  =====> info loss
             *     0   1     0   1     0   1
             *     1   1     1   1     1   1
             */
            key = k0; /* with info loss */
            mask = k1;
            break;

        case BCMPTM_TCAM_KM_FORMAT_40NM_XY:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = ~MASK | KEY
             *
             *    Decode: KEY = K0
             *            MASK = K0 | ~K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   1     0   0
             *     1   0     0   1     0   0  =====> info loss
             *     0   1     0   0     0   1
             *     1   1     1   1     1   1
             */
            key = k0; /* with info loss */
            mask = k0 | ~k1;
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT_7NM:
            /*
             * D[1] = (!LPT_Y[1] | !LPT_Y[0]) &
             *        LPT_X[0] &
             *        LPT_X[1]; -- OVERLAPPING STATES
             *
             * D[0] = LPT_Y[1] &
             *        (!LPT_X[1] | !LPT_Y[0]) &
             *        LPT_X[0]; -- OVERLAPPING STATES
             *
             * M[1] = (!LPT_Y[1] & LPT_X[1] & LPT_X[0]) |
             *        (LPT_X[1] & !LPT_Y[0] & LPT_X[0]) |
             *        (LPT_Y[1] & LPT_Y[0] & !LPT_X[0]) |
             *        (LPT_Y[1] & !LPT_X[1] & LPT_X[1]);
             *
             * M[0] = (!LPT_Y[1] & LPT_X[1] & LPT_Y[0]) |
             *        (LPT_X[1] & LPT_Y[0] & !LPT_X[0]) |
             *        (LPT_Y[1] & !LPT_Y[0] & LPT_X[0]) |
             *        (LPT_Y[1] & !LPT_X[1] & LPT_X[0]);
             */
            x = k0;
            y = k1;
            xbar = ~x;
            ybar = ~y;

            /* Following can be made more efficient - but not needed because
             * xy -> km conversion is done only for debug read from HW.
             */
            key = (0xaaaaaaaa & ((ybar | (ybar << 1)) &
                                 (x << 1)             &
                                 x
                                )
                  ) |
                  (0x55555555 & ((y >> 1)             &
                                 ((xbar >> 1) | ybar) &
                                 x
                                )
                  );
            mask = (0xaaaaaaaa & ((ybar & x           & (x << 1))    |
                                  (x    & (ybar << 1) & (x << 1))    |
                                  (y    & (y << 1)    & (xbar << 1)) |
                                  (y    & xbar        & x) 
                                 )
                   ) |
                   (0x55555555 & (((ybar >> 1) & (x >> 1)    & y)    |
                                  ((x >> 1)    & y           & xbar) |
                                  ((y >> 1)    & ybar        & x)    |
                                  ((y >> 1)    & (xbar >> 1) & x)
                                 )
                   );
            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Read: x=0x%8x, y=0x%8x, k=0x%8x, m=0x%8x\n"),
                 x, y, key, mask));
            */
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT:
            /*
             * D[1] = LPT_Y[0] & LPT_X[0]
             * D[0] = LPT_Y[1] & LPT_Y[0]
             *
             * M[1] = LPT_Y[1] & LPT_X[1] | LPT_Y[0] & LPT_X[0];
             * M[0] = LPT_X[1] & LPT_X[0] | LPT_Y[1] & LPT_Y[0];
             *
             * M[1]= LPT_Y[1] & LPT_X[1] | D[1];
             * M[0]= LPT_X[1] & LPT_X[0] | D[0];
             */

            /* Step1: Convert LPT -> X_YBAR
             *
             * xybar_x[1:0] = {(lpt_y[0] & lpt_x[0]), (lpt_y[1] & lpt_y[0])}
             * xybar_y[1:0] = {(lpt_y[1] & lpt_x[1]), (lpt_x[1] & lpt_x[0])}
             */
            x = (((k1 & k0) << 1) & 0xaaaaaaaa) | /* D[1] */
                (((k1 >> 1) & k1) & 0x55555555);  /* D[0] */

            y = ((k1 & k0) & 0xaaaaaaaa) |       /* 1st term of M[1] */
                (((k0 >> 1) & k0) & 0x55555555); /* 1st term of M[0] */

            /* Step2: X_YBAR -> key, mask*/
            /* old_k0 = k0; */
            /* old_k1 = k1; */
            k0 = x;
            k1 = y;
            /* break; goto next case item */

        case BCMPTM_TCAM_KM_FORMAT_X_YBAR:
            /*
             *     Encode: K0 = MASK & KEY
             *             K1 = MASK & ~KEY
             *
             *     Decode: KEY = K0
             *             MASK = K0 | K1
             *
             *     KEY MASK   K0  K1   KEY MASK
             *     --------   ------   --------
             *      0   0     0   0     0   0
             *      1   0     0   0     0   0  =====> info loss
             *      0   1     0   1     0   1
             *      1   1     1   0     1   1
             */
            key = k0; /* with info loss */
            mask = k0 | k1;
            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Read: lptx=0x%8x, lpty=0x%8x, key=0x%8x, mask=0x%8x\n"),
                 old_k0, old_k1, key, mask));
             */
            break;

        default: /*  BCMPTM_TCAM_KM_FORMAT_IS_KM - no conversion */
            key = k0;
            mask = k1;
            break;
    } /* tcam_km_format */

    *k0p = key;
    *k1p = mask;

/* exit: */
    /* SHR_FUNC_EXIT(); */
    return SHR_E_NONE;
}


/*******************************************************************************
 * Public Functions
 */
/* Change format for (key, mask) fields for one or multiple entries
 * User can specify to replace dm format with xy format by specifying
 * xy_entry = km_entry
 *
 * This func must take into account tcam type, chip type,
 * figure out where tcam_data, tcam_mask are in entry_words
 * and ensure that it does not modify assoc_data portion for case
 * when sid is aggr of tcam+assoc_dat
 *
 * For newer chips, SW does not need to do format change during
 * writes - it will by done inside chip by HW logic.
 */
int
bcmptm_pt_tcam_km_to_xy(int unit,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *km_entry,
                        uint32_t *xy_entry)
{
    uint32_t key[BCMPTM_MAX_PT_FIELD_WORDS], mask[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t field_count, entry_index, i, word, entry_word_count,
             entry_byte_count;
    bcmptm_pt_tcam_km_format_t tcam_km_format;
    bcmptm_tcam_iw_fi_t info_word;
    uint32_t iw_base_index;
    bool hw_does_km_to_xy = FALSE;
    SHR_FUNC_ENTER(unit);

    assert(bcmdrd_pt_is_valid(unit, sid));
    SHR_NULL_CHECK(km_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(xy_entry, SHR_E_PARAM);
    SHR_IF_ERR_EXIT((entry_count <= 0)? SHR_E_PARAM : SHR_E_NONE);

    SHR_IF_ERR_EXIT(bcmptm_ptcache_iw_base(unit, sid, &iw_base_index));
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_iw_read(unit, iw_base_index++, &info_word.entry));
    SHR_IF_ERR_EXIT(!info_word.cw.count? SHR_E_INTERNAL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((info_word.cw.type != BCMPTM_TCAM_IW_TYPE_CW)?
                    SHR_E_INTERNAL : SHR_E_NONE);
    field_count = info_word.cw.count / 2;

    SHR_IF_ERR_EXIT(bcmptm_hw_does_km_to_xy(unit, &hw_does_km_to_xy));
    SHR_IF_ERR_EXIT(bcmptm_pt_tcam_km_format(unit, sid, &tcam_km_format));
    entry_word_count = bcmdrd_pt_entry_wsize(unit, sid);
    entry_byte_count = entry_word_count * sizeof(uint32_t);

    for (entry_index = 0; entry_index < entry_count; entry_index++) {
        if (xy_entry != km_entry) {
            /* User can ask us to do replace by specifying same ptr for
             * km_entry, xy_entry */
            sal_memcpy(xy_entry, km_entry, entry_byte_count);
        }
        if (tcam_km_format == BCMPTM_TCAM_KM_FORMAT_IS_KM ||
            hw_does_km_to_xy) {
            km_entry += entry_word_count;
            xy_entry += entry_word_count;
            continue; /* to next entry */
        }
        for (i = 0; i < field_count; i++) {
            uint32_t k_sbit, k_ebit, m_sbit, m_ebit;
            uint32_t field_bit_count, field_word_count;

            /* info for key field */
            SHR_IF_ERR_EXIT(
                bcmptm_ptcache_iw_read(unit, iw_base_index++,
                                       &info_word.entry));
            k_sbit = info_word.fi.minbit;
            k_ebit = k_sbit + info_word.fi.width - 1;
            SHR_IF_ERR_EXIT((info_word.fi.type !=
                             BCMPTM_TCAM_IW_TYPE_FI_KEY)? SHR_E_INTERNAL
                                                        : SHR_E_NONE);
            /* info for mask field */
            SHR_IF_ERR_EXIT(
                bcmptm_ptcache_iw_read(unit, iw_base_index++,
                                       &info_word.entry));
            m_sbit = info_word.fi.minbit;
            m_ebit = m_sbit + info_word.fi.width - 1;
            SHR_IF_ERR_EXIT((info_word.fi.type !=
                             BCMPTM_TCAM_IW_TYPE_FI_MASK)? SHR_E_INTERNAL
                                                         : SHR_E_NONE);

            /* extract words of key, mask fields */
            bcmdrd_field_get(km_entry, k_sbit, k_ebit, key);
            bcmdrd_field_get(km_entry, m_sbit, m_ebit, mask);

            field_bit_count = info_word.fi.width;
            field_word_count = BCMPTM_BITS2WORDS(field_bit_count);

            for (word = 0; word < field_word_count; word++) {
                SHR_IF_ERR_EXIT(
                    tcam_word_km_to_xy(unit,
                                       tcam_km_format,
                                       &key[word],
                                       &mask[word]));
            }

            /* Zero out unused bits in last word of key, mask fields */
            if ((field_bit_count & 0x1f) != 0) {
                /* means valid bits in last word < 32 */
                key[word - 1]  &= (1 << (field_bit_count & 0x1f)) - 1;
                mask[word - 1] &= (1 << (field_bit_count & 0x1f)) - 1;
            }
                /* Example: if field_bit_count = 5
                 * then (1 << 5) = 32 and so RHS will be 31 = 0x1f
                 * and thus we will be zeroing out all mask bits except least
                 * significant 5 bits */

                /* Basically, extra bits in last key, mask word should be 0
                 * and for 28nm they translate into (K0=0, k1=0), but for
                 * 40nm, they translate to (K0=0, K1=1) - so cleanup is needed
                 * in last mask word - because when generating K0, K1 we did not
                 * bother to check num of bits in last key, mask words
                 */

            /* Stuff converted_key, _mask fields into xy_entry */
            bcmdrd_field_set(xy_entry, k_sbit, k_ebit, key);
            bcmdrd_field_set(xy_entry, m_sbit, m_ebit, mask);
        } /* for each {key, mask} field pair in this entry */

        /* Point to next entry */
        km_entry += entry_word_count;
        xy_entry += entry_word_count;
    } /* for each entry */

exit:
    SHR_FUNC_EXIT();
}

/* Change format for (key, mask) fields for one or multiple entries
 * User can specify to replace dm format with xy format by specifying
 * km_entry = xy_entry
 *
 * This func must take into account tcam type, chip type,
 * figure out where tcam_data, tcam_mask are in entry_words
 * and ensure that it does not modify assoc_data portion for case
 * when sid is aggr of tcam+assoc_dat.
 */
int
bcmptm_pt_tcam_xy_to_km(int unit,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *xy_entry,
                        uint32_t *km_entry)
{
    uint32_t k0[BCMPTM_MAX_PT_FIELD_WORDS], k1[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t field_count, entry_index, i, word, entry_word_count,
             entry_byte_count;
    bcmptm_pt_tcam_km_format_t tcam_km_format;
    bcmptm_tcam_iw_fi_t info_word;
    uint32_t iw_base_index;
    SHR_FUNC_ENTER(unit);

    assert(bcmdrd_pt_is_valid(unit, sid));
    SHR_NULL_CHECK(xy_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(km_entry, SHR_E_PARAM);
    SHR_IF_ERR_EXIT((entry_count <= 0)? SHR_E_PARAM : SHR_E_NONE);

    SHR_IF_ERR_EXIT(bcmptm_ptcache_iw_base(unit, sid, &iw_base_index));
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_iw_read(unit, iw_base_index++, &info_word.entry));
    SHR_IF_ERR_EXIT(!info_word.cw.count? SHR_E_INTERNAL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((info_word.cw.type != BCMPTM_TCAM_IW_TYPE_CW)?
                    SHR_E_INTERNAL : SHR_E_NONE);
    field_count = info_word.cw.count / 2;

    SHR_IF_ERR_EXIT(bcmptm_pt_tcam_km_format(unit, sid, &tcam_km_format));
    entry_word_count = bcmdrd_pt_entry_wsize(unit, sid);
    entry_byte_count = entry_word_count * sizeof(uint32_t);

    

    for (entry_index = 0; entry_index < entry_count; entry_index++) {
        if (km_entry != xy_entry) {
            /* User can ask us to do replace by specifying same ptr for
             * km_entry, xy_entry */
            sal_memcpy(km_entry, xy_entry, entry_byte_count);
        }
        if (tcam_km_format == BCMPTM_TCAM_KM_FORMAT_IS_KM) {
            xy_entry += entry_word_count;
            km_entry += entry_word_count;
            continue; /* to next entry */
        }
        for (i = 0; i < field_count; i++) {
            uint32_t k_sbit, k_ebit, m_sbit, m_ebit;
            uint32_t field_bit_count, field_word_count;

            /* info for key field */
            SHR_IF_ERR_EXIT(
                bcmptm_ptcache_iw_read(unit, iw_base_index++,
                                       &info_word.entry));
            k_sbit = info_word.fi.minbit;
            k_ebit = k_sbit + info_word.fi.width - 1;
            SHR_IF_ERR_EXIT((info_word.fi.type !=
                             BCMPTM_TCAM_IW_TYPE_FI_KEY)? SHR_E_INTERNAL
                                                        : SHR_E_NONE);
            /* info for mask field */
            SHR_IF_ERR_EXIT(
                bcmptm_ptcache_iw_read(unit, iw_base_index++,
                                       &info_word.entry));
            m_sbit = info_word.fi.minbit;
            m_ebit = m_sbit + info_word.fi.width - 1;
            SHR_IF_ERR_EXIT((info_word.fi.type !=
                             BCMPTM_TCAM_IW_TYPE_FI_MASK)? SHR_E_INTERNAL
                                                         : SHR_E_NONE);

            /* extract words of k0, k1 fields */
            bcmdrd_field_get(xy_entry, k_sbit, k_ebit, k0);
            bcmdrd_field_get(xy_entry, m_sbit, m_ebit, k1);

            field_bit_count = info_word.fi.width;
            field_word_count = BCMPTM_BITS2WORDS(field_bit_count);

            for (word = 0; word < field_word_count; word++) {
                SHR_IF_ERR_EXIT(
                    tcam_word_xy_to_km(unit,
                                       tcam_km_format,
                                       &k0[word],
                                       &k1[word]));
            }

            /* Zero out extra bits in last word of key, mask fields */
            if ((field_bit_count & 0x1f) != 0) {
                k0[word - 1] &= (1 << (field_bit_count & 0x1f)) - 1;
                k1[word - 1] &= (1 << (field_bit_count & 0x1f)) - 1;
            }

            /* stuff converted_k0, _k1 fields into km_entry */
            bcmdrd_field_set(km_entry, k_sbit, k_ebit, k0);
            bcmdrd_field_set(km_entry, m_sbit, m_ebit, k1);
        } /* for each {k0, k1} field pair in this entry */

        /* Point to next entry */
        xy_entry += entry_word_count;
        km_entry += entry_word_count;
    } /* For each entry */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_iw_count(int unit, bcmdrd_sid_t sid, uint32_t *iw_count)
{
    uint32_t word_count = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iw_count, SHR_E_PARAM);
    *iw_count = 0;

    if (bcmdrd_pt_attr_is_cam(unit, sid)) {
        /* Dont optimize based on whether TCAM needs xy conversion or not */
        SHR_IF_ERR_EXIT(tcam_km_fields_set(unit, sid,
                                           FALSE, /* only count */
                                           0,     /* fill index is dont_care */
                                           &word_count));
        *iw_count = word_count + 1;
            /* +1 word in info_seg to hold the word_count */
    }
    /* else {
        info_words not needed - can be expanded for hash, etc
    } */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_iw_fill(int unit,
                  bcmdrd_sid_t sid,
                  uint32_t iw_base_index)
{
    bcmptm_tcam_iw_fi_t tcam_info_word;
    uint32_t word_count = 0;
    SHR_FUNC_ENTER(unit);

    if (bcmdrd_pt_attr_is_cam(unit, sid)) {
        SHR_IF_ERR_EXIT(
            tcam_km_fields_set(unit, sid,
                               TRUE, /* fill_mode */
                               iw_base_index + 1,
                                   /* skip info_seg.word0 (used to store
                                    * ctl_word) */
                               &word_count));

        /* Add info_word of type CW to store word count */
        tcam_info_word.entry = 0;
        tcam_info_word.cw.type = BCMPTM_TCAM_IW_TYPE_CW;
        tcam_info_word.cw.count = word_count;
            /* Note: Only includes words needed to store fields.
             *       Does not include ctl_word itself */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "entry=0x%8x: type=%0d, count=%0d\n"),
             tcam_info_word.entry, tcam_info_word.cw.type,
             tcam_info_word.cw.count));

        SHR_IF_ERR_EXIT(bcmptm_ptcache_iw_write(unit, iw_base_index,
                                                tcam_info_word.entry));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            /* Currently, we only support storing field info for TCAMs.
             * ptcache should not be asking us to fill words for any other
             * memory */
    }

exit:
    SHR_FUNC_EXIT();
}
