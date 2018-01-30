/*! \file rm_alpm_util.h
 *
 * Utilities internal to RM ALPM
 *
 * This file contains utilities which are internal to
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

#ifndef RM_ALPM_UTIL_H
#define RM_ALPM_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */

#define ALPM_SHIFT_LEFT(val, count) (((count) == 32) ? 0 : (val) << (count))
#define ALPM_SHIFT_RIGHT(val, count) (((count) == 32) ? 0 : (val) >> (count))


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief String name for key type
 *
 * \param [in] u Device u.
 * \param [in] kt Key type
 *
 * \return String name.
 */
const char *
bcmptm_rm_alpm_kt_name(int u, alpm_key_type_t kt);

/*!
 * \brief String name for vrf type
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type
 *
 * \return String name.
 */
const char *
bcmptm_rm_alpm_vt_name(int u, alpm_vrf_type_t vt);

/*!
 * \brief Update HW table entry (through WAL) and PTcache
 *
 * \param [in] u Device u.
 * \param [in] sid ALPM symbols
 * \param [in] index Pt index
 * \param [in] entry_words Entry content
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_write(int u, bcmdrd_sid_t sid,
                     int index, uint32_t *entry_words);

/*!
 * \brief Update HW table entry (through Interactive path) and PTcache
 * \n Must be used only for INIT.
 *
 * \param [in] u Device u.
 * \param [in] sid ALPM symbols
 * \param [in] index Pt index
 * \param [in] entry_words Entry content
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ireq_write(int u, bcmdrd_sid_t sid,
                          int index, uint32_t *entry_words);

/*!
 * \brief Read latest table entry
 *
 * \param [in] u Device u.
 * \param [in] sid ALPM symbols
 * \param [in] index Pt index
 * \param [out] entry_words Entry content
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_read(int u, bcmdrd_sid_t sid,
                    int index, uint32_t *entry_words);


/*!
 * \brief Read latest HW table entry through Interactive path.
 *
 * \param [in] u Device u.
 * \param [in] sid ALPM symbols
 * \param [in] index Pt index
 * \param [out] entry_words Entry content
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ireq_read(int u, bcmdrd_sid_t sid,
                         int index, uint32_t *entry_words);

/*!
 * \brief Initialize alpm arg
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM arg
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_arg_init(int u, alpm_arg_t *arg);

/*!
 * \brief Convert weighted vrf to vrf type
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] w_vrf Weighted vrf
 * \param [in] max_vrf Max vrf for the unit
 *
 * \return VRF type
 */
alpm_vrf_type_t
bcmptm_rm_alpm_vt_from_wvrf(int u, int db, int w_vrf, int max_vrf);

/*!
 * \brief Get IP address mask from IP mask len
 *
 * \param [in] u Device u.
 * \param [in] v6 Is IPv6
 * \param [in] len IP mask len
 * \param [out] mask IP address mask
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_len_to_mask(int u, int v6, int len, uint32_t *mask);


/*!
 * \brief Get trie format key from normal IP address
 *
 * ip_addr[3] -> ip_addr[0] : msb -> lsb (containing zeros).
 * key[0]     -> key[4]     : msb -> lsb (no zeros)
 *
 * \param [in] u Device u.
 * \param [in] ipv IP version
 * \param [in] ip_addr IP address
 * \param [in] len IP mask length
 * \param [out] key Trie format key
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_trie_key_create(int u, int ipv, uint32_t *ip_addr, uint32_t len,
                               trie_ip_t *key);


/*!
 * \brief Get normal IP address from trie format key
 *
 * ip_addr[3] -> ip_addr[0] : msb -> lsb (containing zeros).
 * key[0]     -> key[4]     : msb -> lsb (no zeros)
 *
 * \param [in] u Device u.
 * \param [in] ipv IP version
 * \param [in] key Trie format key
 * \param [in] len IP mask length
 * \param [out] ip_addr IP address
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_api_key_create(int u, int ipv, trie_ip_t *key, uint32_t len,
                              alpm_ip_t ip_addr);

/*!
 * \brief Get IP mask length from IP address mask
 *
 * \param [in] u Device u.
 * \param [in] v6 Is IPv6
 * \param [in] mask IP address mask
 * \param [out] len IP mask length
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_mask_to_len(int u, int v6, uint32_t *mask, int *len);

/*!
 * \brief One bit format encoding
 *
 * \param [in] v6 Is IPv6
 * \param [in] trie_ip IP address
 * \param [in] len IP address full length
 * \param [in] fmt_len Format length
 * \param [in] kshift Key shift
 * \param [out] format_pfx One bit prefix format
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_one_bit_encode(int v6,
                              trie_ip_t *trie_ip,
                              int len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx);

/*!
 * \brief One bit format decoding
 *
 * \param [in] v6 Is IPv6
 * \param [in] trie_ip IP address, in as prev level trie_ip, out as trie_ip.s
 * \param [out] len IP address full length
 * \param [in] fmt_len Format length
 * \param [in] kshift Key shift
 * \param [in] format_pfx One bit prefix format
 *
 * \return valid
 */
extern int
bcmptm_rm_alpm_one_bit_decode(int v6,
                              trie_ip_t *trie_ip,
                              int *len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx);

#endif /* RM_ALPM_UTIL_H */
