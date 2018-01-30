/*! \file bcmbd_pt.c
 *
 * <description>
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

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

int
bcmbd_pt_read(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data, size_t wsize)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_read == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_read(unit, sid, dyn_info, ovrr_info, data, wsize);
}

int
bcmbd_pt_write(int unit, bcmdrd_sid_t sid,
               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
               uint32_t *data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_write == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_write(unit, sid, dyn_info, ovrr_info, data);
}

int
bcmbd_pt_insert(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_insert == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_insert(unit, sid, dyn_hash_info, ovrr_hash_info, data, wsize,
                         resp_info, resp_data);
}

int
bcmbd_pt_delete(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_delete == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_delete(unit, sid, dyn_hash_info, ovrr_hash_info, data, wsize,
                         resp_info, resp_data);
}

int
bcmbd_pt_lookup(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_lookup == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_lookup(unit, sid, dyn_hash_info, ovrr_hash_info, data, wsize,
                         resp_info, resp_data);
}

int
bcmbd_pt_pop(int unit, bcmdrd_sid_t sid,
             bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
             uint32_t *data, size_t wsize)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_pop == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_pop(unit, sid, dyn_info, ovrr_info, data, wsize);
}

int
bcmbd_pt_push(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_push == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_push(unit, sid, dyn_info, ovrr_info, data);
}

int
bcmbd_pt_addr_decode(int unit,
                     uint32_t adext, uint32_t addr, uint32_t drd_flags,
                     bcmdrd_sid_t *sid, bcmbd_pt_dyn_info_t *dyn_info)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_addr_decode == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_addr_decode(unit, adext, addr, drd_flags, sid, dyn_info);
}

int
bcmbd_pt_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info,
                     void *ovrr_info, bcmbd_pt_cmd_t cmd,
                     uint32_t wsize, uint32_t *data,
                     uint32_t *hdr_wsize)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_cmd_hdr_get == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_cmd_hdr_get(unit, sid, dyn_info, ovrr_info, cmd, wsize,
                              data, hdr_wsize);
}
