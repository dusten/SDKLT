/*! \file bcmbd_cmicm.h
 *
 * Definitions for CMICm.
 * Also applicable for CMICd v1.
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

#ifndef BCMBD_CMICM_H
#define BCMBD_CMICM_H

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>

/*! Size of memory-mapped I/O window for CMICm. */
#define BCMBD_CMICM_IOMEM_SIZE          0x40000

/*!
 * \name S-bus protocol v4 address encoding.
 * \anchor bcmbd_cmicm_sbus
 *
 * The SOC register/memory address information for S-bus protocol v4
 * is carried in two 32-bit words, addr (address) and adext (address
 * extension).
 *
 * \{
 */

/*! Extract block number from address extension. */
#define BCMBD_CMICM_ADEXT2BLOCK(_adext) \
        ((_adext) & 0x3f)

/*! Extract access type from address extension. */
#define BCMBD_CMICM_ADEXT2ACCTYPE(_adext) \
        (((_adext) >> 8) & 0x7)

/*! Modify block in existing address extension. */
#define BCMBD_CMICM_ADEXT_BLOCK_SET(_adext, _block) \
        (_adext = ((_adext) & ~0xff) | (_block))

/*! Modify access type in existing address extension */
#define BCMBD_CMICM_ADEXT_ACCTYPE_SET(_adext, _acctype) \
        (_adext = ((_adext) & ~0x700) | (((_acctype) & 0x7) << 8))

/* Extract access type from block/access info */
#define BCMBD_CMICM_BLKACC2ACCTYPE(_blkacc) \
        (((_blkacc) >> 16) & 0x7)

/* Convert block/access info to address extension */
#define BCMBD_CMICM_BLKACC2ADEXT(_blkacc) \
        (((_blkacc) >> 8) & 0x700)


#define BCMBD_CMICM_CMC 0

/*
 * CMIC CMC selection for PCI interface
 */
#if defined(BCMBD_CMICM_CMC)
/* Allow override by hard-coded CMC selection */
#define BCMBD_CMICM_CMC_GET(_u)    BCMBD_CMICM_CMC
#define BCMBD_CMICM_CMC_SET(_u,_c)
#else
/* Use dynamic CMC selection by default */
#define BCMBD_CMICM_CMC_GET(_u)    BCMBD_CMICM_CMIC_CMC(_u)
#define BCMBD_CMICM_CMC_SET(_u,_c) BCMBD_CMICM_CMIC_CMC(_u) = _c;
#endif

/* CMC base address offset */
#define BCMBD_CMICM_CMC_OFFSET(_u) (BCMBD_CMICM_CMC_GET(_u) * 0x1000)

#define BCMBD_CMICM_CMC_READ(_u, _addr, _val) \
    BCMDRD_DEV_READ32(_u, (_addr) + BCMBD_CMICM_CMC_OFFSET(_u), _val)
#define BCMBD_CMICM_CMC_WRITE(_u, _addr, _val) \
    BCMDRD_DEV_WRITE32(_u, (_addr) + BCMBD_CMICM_CMC_OFFSET(_u), _val)

/*!
 * \brief Enable symbolic address decoding in debug messages.
 *
 * If this function is not called, register and memory addresses in
 * verbose/debug messages will be shown in raw hex format.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicm_sym_addr_enable(int unit);

/*!
 * \brief Initialize DRD features for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicm_feature_init(int unit);

/*!
 * \brief Initialize CMICm base driver for a device.
 *
 * This function will initialize default function vectors for
 * register/memory access, etc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicm_drv_init(int unit);

/*!
 * \brief Read physical table entry.
 *
 * Read a register/memory entry on a CMICm-based device.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicm_read(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data, size_t wsize);

/*!
 * \brief Write physical table entry.
 *
 * Write a register/memory entry on a CMICm-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [in] data Data entry to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
int
bcmbd_cmicm_write(int unit, bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                  uint32_t *data);

/*!
 * \brief Insert physical table entry.
 *
 * Insert a memory entry on a CMICm-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
 * \param [in] data Buffer to insert to table.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Old entry data on table insert if not NULL.
 *
 * \retval SHR_E_NONE New table entry inserted or replaced successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_cmicm_insert(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Delete physical table entry.
 *
 * Delete a memory entry on a CMICm-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
 * \param [in] data Buffer contains key to delete from table.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Deleted entry data if not NULL.
 *
 * \retval SHR_E_NONE table entry deleted successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_cmicm_delete(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Lookup physical table entry.
 *
 * Lookup a memory entry on a CMICm-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
 * \param [in] data Buffer contains key to lookup in table.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Matched data entry if not NULL.
 *
 * \retval SHR_E_NONE table entry found successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_cmicm_lookup(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

#endif /* BCMBD_CMICM_H */
