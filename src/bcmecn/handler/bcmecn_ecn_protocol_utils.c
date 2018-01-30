/*! \file bcmecn_ecn_protocol_utils.c
 *
 * This file defines HW interface handler functions for
 * ecn protocol status custom handler.
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

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_bitop.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmecn/bcmecn_ecn_protocol_utils.h>

#define BSL_LOG_MODULE          BSL_LS_BCMECN_ECN_PROTOCOL

/*******************************************************************************
 * Local definitions
 */
bcmecn_drv_t      *bcmecn_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief bcmecn_ecn_protocol_pt_write
 *
 * Write ecn protocol entry into physical table.
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id LT  Transaction identifier.
 * \param [in] ip_protocol  Internet protocol.
 * \param [in] responsive   Responsiveness of the protocol.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
int
bcmecn_ecn_protocol_pt_write(int unit, uint32_t trans_id,
                             uint32_t ip_protocol,
                             uint32_t responsive)
{
    uint32_t responsive_bitmap[2];
    uint32_t rsp_flags = 0, table_index = 0;
    uint32_t entry_sz = 0;
    bcmbd_pt_dyn_info_t     pt_dyn_info;
    bcmltd_sid_t            rsp_ltid, req_ltid;
    bcmdrd_sid_t            sid;
    bcmdrd_fid_t            fid;
    uint8_t                 *entry_buf = NULL;
    int                     pos = 0;

    SHR_FUNC_ENTER(unit);

    req_ltid = ECN_PROTOCOLt;
    sid = BCMECN_PROTO_MATCH_HW_SID(unit);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "ecn protocol entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    /* derive hardware table index */
    table_index = (ip_protocol / BCMECN_PROTOCOLS_PER_ENTRY(unit));

    /* Initialize the memory to 0. */
    sal_memset(entry_buf, 0, entry_sz);
    sal_memset(responsive_bitmap, 0, 2 * sizeof(uint32_t));

    /* Field id. */
    fid = BCMECN_PROTO_RESPONSIVE_FID(unit);

    /* Set pt table entry */
    pt_dyn_info.index = table_index;
    pt_dyn_info.tbl_inst = -1;

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    entry_sz/4,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));


    /* Read hardware information. */
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)entry_buf, fid,
                        responsive_bitmap);

    /* Get position information to set corresponding bit */
    pos = (ip_protocol % BCMECN_PROTOCOLS_PER_ENTRY(unit));

    /* Use 64-bit macros to set bitmap. */
    if (!responsive) {
        SHR_BITCLR(responsive_bitmap, pos);
    } else {
        SHR_BITSET(responsive_bitmap, pos);
    }

    /* Set the updated value in pt table entry */
    bcmdrd_pt_field_set(unit, sid, (uint32_t *)entry_buf, fid,
                        responsive_bitmap);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)entry_buf,
                                    0,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}


/*!
 * \brief bcmecn_ecn_protocol_pt_read
 *
 * Read ecn protocol entry into physical table.
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id LT  Transaction identifier.
 * \param [in] ip_protocol  Internet protocol.
 * \param [in] responsive   Responsiveness of the protocol.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
int
bcmecn_ecn_protocol_pt_read(int unit, uint32_t trans_id,
                            uint32_t ip_protocol,
                            uint32_t *responsive)
{
    uint32_t responsive_bitmap[2];
    uint32_t rsp_flags = 0, table_index = 0;
    uint32_t entry_sz = 0;
    bcmbd_pt_dyn_info_t     pt_dyn_info;
    bcmltd_sid_t            rsp_ltid, req_ltid;
    bcmdrd_sid_t            sid;
    bcmdrd_fid_t            fid;
    uint8_t                 *entry_buf = NULL;
    int                     pos = 0;

    SHR_FUNC_ENTER(unit);

    req_ltid = ECN_PROTOCOLt;
    sid = BCMECN_PROTO_MATCH_HW_SID(unit);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "ecn protocol entry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    /* Initialize the memory to zero. */
    sal_memset(entry_buf, 0, entry_sz);
    sal_memset(responsive_bitmap, 0, 2 * sizeof(uint32_t));


    /* Set hardware table index */
    table_index = (ip_protocol / BCMECN_PROTOCOLS_PER_ENTRY(unit));

    /* Field id of responsiveness. */
    fid = BCMECN_PROTO_RESPONSIVE_FID(unit);

    pt_dyn_info.index = table_index;
    pt_dyn_info.tbl_inst = -1;


    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    entry_sz/4,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    bcmdrd_pt_field_get(unit, sid, (uint32_t *)entry_buf, fid,
                        responsive_bitmap);

    /* Now set the corresponding bit */
    pos = (ip_protocol % BCMECN_PROTOCOLS_PER_ENTRY(unit));


    /* Get responsive status from bitmap. */
    *responsive = ((SHR_BITGET(responsive_bitmap, pos)) ? 1 : 0);

    LOG_DEBUG(BSL_LOG_MODULE,
        (BSL_META_U(unit," ip_protocol = %d, pos = %d :: *responsive = %d\n"),
                    ip_protocol, pos, *responsive));


exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();

}

/*!
 * \brief Set pointer to ecn driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Chip driver structure.
 *
 * \return SHR_E_NONE
 */
int
bcmecn_drv_set(int unit,
               bcmecn_drv_t *drv)
{
    bcmecn_drv[unit] = drv;

    return SHR_E_NONE;
}

/*!
 * \brief Get pointer to ecn driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
int
bcmecn_drv_get(int unit,
               bcmecn_drv_t **drv)
{
    *drv = bcmecn_drv[unit];

    return SHR_E_NONE;
}
