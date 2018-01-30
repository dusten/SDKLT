/*! \file rm_alpm_util.c
 *
 * Utility functions for alpm
 *
 * This file contains the implementation for utilities
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

#include "rm_alpm.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_leveln.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Translate mask into mask_len
 *
 * \param [in] u Device u.
 * \param [in] mask IP address mask
 * \param [out] mask_len IP mask len
 *
 * \return SHR_E_XXX.
 */
static inline int
ipmask2len(int u, uint32_t mask, int *mask_len)
{
    while (mask & (1 << 31)) {
        *mask_len += 1;
        mask <<= 1;
    }
    return ((mask) ? SHR_E_PARAM : SHR_E_NONE);
}

/*!
 * \brief Reverse ip and copy to trie_ip
 *
 * \param [in] ipv IP version
 * \param [in] IP Normal IP
 * \param [out] trie_ip Trie format ip
 *
 * \return nothing.
 */
static inline void
reverse_ip_to_trie_ip(int v6, alpm_ip_t ip, trie_ip_t *trie_ip)
{
    int i;
    if (v6) {
        for (i = 0; i < 4; i++) {
            trie_ip->v6_key[4 - i] = ip[i];
           /* assert(trie_ip->v6_key[0] == 0); */
        }
    } else {
        trie_ip->v4_key[1] = ip[0];
      /*  assert(trie_ip->v4_key[0] == 0); */
    }
}


/*!
 * \brief Reverse trie_ip and copy to ip
 *
 * \param [in] ipv IP version
 * \param [in] trie_ip Trie format ip
 * \param [out] IP Normal IP
 *
 * \return nothing.
 */
static void
reverse_trie_ip_to_ip(int v6, trie_ip_t *trie_ip, alpm_ip_t ip)
{
    int i;
    if (v6) {
        for (i = 0; i < 4; i++) {
            ip[i] = trie_ip->v6_key[4 - i];
            assert(trie_ip->v6_key[0] == 0);
        }
    } else {
        ip[0] = trie_ip->v4_key[1];
        assert(trie_ip->v4_key[0] == 0);
    }
}

/*!
 * \brief Shift ip_addr left or right to format_ip.
 *
 *  Src IP : hi -> lo : msb -> lsb
 *  Dst IP : hi -> lo : msb -> lsb  if reverse = false
 *  Dst IP: lo -> hi : msb -> lsb  if reverse = true
 *
 *  Dst ip assumed to have 4 elements.
 *
 * \param [in] src_ip Source IP address
 * \param [out] dst_ip Destination IP address
 * \param [in] shift Shift right if positive, else shift left.
 * \param [in] reverse If true reverse elements in dst_ip.
 *
 * \return SHR_E_NONE.
 */
static int
ip6_addr_shift(alpm_ip_t src_ip, alpm_ip_t dst_ip, int shift, bool reverse)
{
    int start, left_shift;
    int right_shift;
    int i, j, msb, lsb;
    int incr;
    if (shift >= 0)  { /* shift right */
        right_shift = shift;
        start = right_shift / 32;
        right_shift = right_shift % 32;

        j = reverse ? 3 : 0;
        incr = reverse ? -1 : 1;
        for (i = start; i < 4; i++) { /* lo --> hi */
            /* lsb part */
            lsb = ALPM_SHIFT_RIGHT(src_ip[i], right_shift);
            /* msb part */
            if (i == 3) {
                msb = 0;
            } else {
                msb = src_ip[i + 1] & ((1 << right_shift) - 1);
                msb = ALPM_SHIFT_LEFT(msb, 32 - right_shift);
            }
            /* concat & assign */
            dst_ip[j] = msb | lsb;
            j += incr;
        }
    } else { /* shift left */
        left_shift = -shift;
        if (left_shift == 128) {
            sal_memset(dst_ip, 0, sizeof(alpm_ip_t));
        } else {
            assert(left_shift < 128);
            if (reverse) {
                start = (left_shift / 32);
                left_shift = left_shift % 32;

                j = 3;
                for (i = start; i < 4; i++) {
                    /* lsb part */
                    msb = ALPM_SHIFT_LEFT(src_ip[i], left_shift);
                    /* msb part */
                    if (i == 3) {
                        lsb = 0;
                    } else {
                        lsb = ALPM_SHIFT_RIGHT(src_ip[i + 1], 32 - left_shift);
                    }
                    /* concat & assign */
                    dst_ip[j] = msb | lsb;
                    j--;
                }
            } else {
                start = 3 - (left_shift / 32);
                left_shift = left_shift % 32;

                j = 0;
                for (i = start; i >= 0; i--) {
                    /* lsb part */
                    msb = ALPM_SHIFT_LEFT(src_ip[i], left_shift);
                    /* msb part */
                    if (i == 0) {
                        lsb = 0;
                    } else {
                        lsb = ALPM_SHIFT_RIGHT(src_ip[i - 1], 32 - left_shift);
                    }
                    /* concat & assign */
                    dst_ip[j] = msb | lsb;
                    j++;
                }
            }
        }
    }
    return SHR_E_NONE;
}

static inline uint32_t
log2n(uint32_t n)
{
   return (n > 1)? 1 + log2n(n/2): 0;
}

/*!
 * \brief One bit prefix length in this segment (level)
 *
 * \param [in] fmt_len Format length
 * \param [in] format_pfx One bit prefix format
 *
 * \return nothing
 */
static int
one_bit_pfx_seg_len(int fmt_len, alpm_format_ip_t format_pfx)
{

    int i, bp = 0; /* Bit pos for least significant bit 1 */
    int count = sizeof(alpm_format_ip_t) / sizeof(*format_pfx);
    for (i = 0; i < count; i++) { /* Optimized for performance */
        if (format_pfx[i] == 0) {
            bp += 32;
        } else {
            bp += log2n(format_pfx[i] & (~format_pfx[i] + 1));
            break;
        }
    }
    if (i == count) {
        return -1; /* indicating invalid */
    } else { /* at least 1 valid bit */
        return (fmt_len - bp);
    }
}


/*!
 * \brief One bit format encoding
 *
 * \param [in] v6 Is IPv6
 * \param [in] ip_addr IP address
 * \param [in] len IP address full length
 * \param [in] fmt_len Format length
 * \param [in] kshift Key shift
 * \param [out] format_pfx One bit prefix format
 *
 * \return SHR_E_NONE
 */
static int
one_bit_pfx_encode(int v6,
                   uint32_t *ip_addr,
                   int len,
                   int fmt_len,
                   int kshift,
                   alpm_format_ip_t format_pfx)
{
    int src_sbit;
    int seg_len = len - kshift;
    sal_memset(format_pfx, 0, sizeof(alpm_format_ip_t));
    assert(seg_len <= fmt_len);
    src_sbit = seg_len - fmt_len + 128;
    SHR_BITCOPY_RANGE(format_pfx, 1, ip_addr, src_sbit, fmt_len);
    SHR_BITSET(format_pfx, fmt_len - seg_len);
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        int seg_len2;
        seg_len2 = one_bit_pfx_seg_len(fmt_len, format_pfx);
        assert(seg_len == seg_len2);
    }
    return SHR_E_NONE;
}


/*******************************************************************************
 * Public Functions
 */
const char *
bcmptm_rm_alpm_kt_name(int u, alpm_key_type_t kt)
{
    char *name;
    switch (kt) {
    case KEY_TYPE_INVALID:
        name = "Invalid";
        break;
    case KEY_IPV4:
        name = "IPv4";
        break;
    case KEY_IPV6_32:
        name = "IPv6-32";
        break;
    case KEY_IPV6_64:
        name = "IPv6-64";
        break;
    case KEY_IPV6_128:
        name = "IPv6-128";
        break;
    default:
        name = "?";
        break;
    }
    return name;
}

const char *
bcmptm_rm_alpm_vt_name(int u, alpm_vrf_type_t vt)
{
    char *name;
    switch (vt) {
    case VRF_TYPE_INVALID:
        name = "Invalid";
        break;
    case VRF_PRIVATE:
        name = "PRIVATE";
        break;
    case VRF_GLOBAL:
        name = "GLOBAL";
        break;
    case VRF_OVERRIDE:
        name = "OVERRIDE";
        break;
    default:
        name = "?";
        break;
    }
    return name;
}

int
bcmptm_rm_alpm_write(int u, bcmdrd_sid_t sid,
                     int index, uint32_t *entry_words)
{
    bcmbd_pt_dyn_info_t dyn_info;
    void                *pt_ovrr_info = NULL;
    alpm_info_t         *alpm_info;
    alpm_cmd_ctrl_t     *ctrl;
    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u);

    SHR_NULL_CHECK(alpm_info, SHR_E_PARAM);
    ctrl = &alpm_info->cmd_ctrl;

    dyn_info.index      = index;
    dyn_info.tbl_inst   = ctrl->inst;

    SHR_IF_ERR_EXIT(
        bcmptm_cmdproc_write(u,
                             ctrl->req_flags,
                             ctrl->cseq_id,
                             sid,
                             &dyn_info,
                             pt_ovrr_info,
                             NULL, /* misc_info */
                             entry_words,
                             ctrl->write_hw,
                             ctrl->write_cache,
                             ctrl->set_cache_vbit,
                             BCMPTM_RM_OP_NORMAL, NULL,
                             ctrl->req_ltid,
                             ctrl->req_format_id,
                             &ctrl->rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_ireq_write(int u, bcmdrd_sid_t sid,
                          int index, uint32_t *entry_words)
{
    bcmbd_pt_dyn_info_t dyn_info;
    void                *pt_ovrr_info = NULL;
    alpm_info_t         *alpm_info;
    alpm_cmd_ctrl_t     *ctrl;
    bcmltd_sid_t        rsp_ltid;
    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u);

    SHR_NULL_CHECK(alpm_info, SHR_E_PARAM);
    ctrl = &alpm_info->cmd_ctrl;

    dyn_info.index      = index;
    dyn_info.tbl_inst   = ctrl->inst;

    assert(ctrl->write_hw && ctrl->write_cache && ctrl->set_cache_vbit);
    assert(ctrl->req_format_id == 0);

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_write(u,
                          ctrl->req_flags,
                          sid,
                          &dyn_info,
                          pt_ovrr_info,
                          entry_words,
                          ctrl->req_ltid,

                          &rsp_ltid,
                          &ctrl->rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_read(int u, bcmdrd_sid_t sid,
                    int index, uint32_t *entry_words)
{
    bcmbd_pt_dyn_info_t dyn_info;
    void                *pt_ovrr_info = NULL;
    alpm_info_t         *alpm_info;
    alpm_cmd_ctrl_t     *ctrl;
    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u);

    SHR_NULL_CHECK(alpm_info, SHR_E_PARAM);
    ctrl = &alpm_info->cmd_ctrl;

    dyn_info.index    = index;
    dyn_info.tbl_inst = ctrl->inst;

    SHR_IF_ERR_EXIT(
        bcmptm_cmdproc_read(u,
                            ctrl->req_flags,
                            ctrl->cseq_id,
                            sid,
                            &dyn_info,
                            pt_ovrr_info,
                            NULL, /* misc_info */
                            ALPM_MEM_ENT_MAX,
                            entry_words,
                            &ctrl->rsp_entry_cache_vbit,
                            &ctrl->rsp_ltid,
                            &ctrl->rsp_dfield_format_id,
                            &ctrl->rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_ireq_read(int u, bcmdrd_sid_t sid,
                         int index, uint32_t *entry_words)
{
    bcmbd_pt_dyn_info_t dyn_info;
    void                *pt_ovrr_info = NULL;
    alpm_info_t         *alpm_info;
    alpm_cmd_ctrl_t     *ctrl;
    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u);

    SHR_NULL_CHECK(alpm_info, SHR_E_PARAM);
    ctrl = &alpm_info->cmd_ctrl;

    dyn_info.index    = index;
    dyn_info.tbl_inst = ctrl->inst;

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_read(u,
                         ctrl->req_flags,
                         sid,
                         &dyn_info,
                         pt_ovrr_info,
                         ALPM_MEM_ENT_MAX,
                         ctrl->req_ltid,

                         entry_words,
                         &ctrl->rsp_ltid,
                         &ctrl->rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_arg_init(int u, alpm_arg_t *arg)
{
    int i;

    if (arg) {
        sal_memset(arg, 0, sizeof(*arg));
        arg->key.kt = KEY_TYPE_INVALID;
        arg->key.vt = VRF_TYPE_INVALID;
        for (i = 0; i < LEVELS; i++) {
            arg->index[i] = INVALID_INDEX;
            arg->log_bkt[i] = INVALID_LOG_BKT;
        }
    }
}

alpm_vrf_type_t
bcmptm_rm_alpm_vt_from_wvrf(int u, int db, int w_vrf, int max_vrf)
{
    alpm_vrf_type_t vt;
    if (bcmptm_rm_alpm_database_count(u) == MAX_DBS_2) {
        if (db == DBH) {
            vt = VRF_OVERRIDE;
        } else if (w_vrf == COMBINED_WVRF_GLOBAL(max_vrf)) {
            vt = VRF_GLOBAL;
        } else {
            vt = VRF_PRIVATE;
        }
    } else {
        ALPM_ASSERT(bcmptm_rm_alpm_database_count(u) == MAX_DBS_3);
        if (db == DBH) {
            vt = VRF_OVERRIDE;
        } else if (db == DB0) {
            vt = VRF_PRIVATE;
        } else {
            vt = VRF_GLOBAL;
        }
    }
    return vt;
}

int
bcmptm_rm_alpm_len_to_mask(int u, int v6, int len, uint32_t *mask)
{
    int i = v6 ? 3 : 0;

    while (i >= 0 && len > 32) {
        mask[i--] = 0xffffffff;
        len -= 32;
    }

    mask[i--] = ~ALPM_SHIFT_RIGHT(0xffffffff, len);

    while (i >= 0) {
        mask[i--] = 0;
    }
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_mask_to_len(int u, int v6, uint32_t *mask, int *len)
{
    int i = 0;
    int max = v6 ? 4 : 1;
    int rv;

    *len = 0;
    while (i < max && *len == 0) {
        if (mask[i]) {
            rv = ipmask2len(u, mask[i], len);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
        i++;
    }

    while (i < max) {
        ALPM_ASSERT_ERRMSG(mask[i] == 0xFFFFFFFF);
        *len += 32;
        i++;
    }
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_trie_key_create(int u, int v6, alpm_ip_t ip_addr, uint32_t len,
                               trie_ip_t *key)
{

/*    sal_memset(key, 0, sizeof(trie_ip_t)); */

    if (!v6) {
        key->v4_key[1] = ALPM_SHIFT_RIGHT(*ip_addr, 32 - len);
    } else {
        ip6_addr_shift(&ip_addr[0], &key->v6_key[1], 128 - len, true);
    }
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_api_key_create(int u, int v6, trie_ip_t *key, uint32_t len,
                              alpm_ip_t ip_addr)
{
/*    sal_memset(ip_addr, 0, sizeof(alpm_ip_t)); */
    if (!v6) {
        *ip_addr = ALPM_SHIFT_LEFT(key->v4_key[1], 32 - len);
    } else {
        ip6_addr_shift(&key->v6_key[1], &ip_addr[0], len - 128, true);
    }
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_one_bit_encode(int v6,
                              trie_ip_t *trie_ip,
                              int len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx)
{
    alpm_ip_t ip_addr[2] = {{0}};
    assert(len - kshift <= fmt_len);
    reverse_trie_ip_to_ip(v6, trie_ip, ip_addr[1]);
    return one_bit_pfx_encode(v6, ip_addr[0], len, fmt_len, kshift, format_pfx);
}

int
bcmptm_rm_alpm_one_bit_decode(int v6,
                              trie_ip_t *trie_ip,
                              int *len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx)
{
    int seg_len, src_bit;
    alpm_ip_t src_ip = {0};
    alpm_ip_t dst_ip = {0};
    reverse_trie_ip_to_ip(v6, trie_ip, src_ip);

    seg_len = one_bit_pfx_seg_len(fmt_len, format_pfx);
    if (seg_len < 0) {
        return 0;
    }
    src_bit = fmt_len - seg_len;
    SHR_BITCOPY_RANGE(dst_ip, seg_len, src_ip, 0, kshift);
    SHR_BITCOPY_RANGE(dst_ip, 0, format_pfx, src_bit + 1, seg_len);

    *len = (seg_len + kshift);
    reverse_ip_to_trie_ip(v6, dst_ip, trie_ip);
    return 1;
}

