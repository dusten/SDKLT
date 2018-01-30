/*! \file bcmtm_pt_internal.h
 *
 * TM moduler interaction with physical table functions.
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

#ifndef BCMTM_PT_INTERNAL_H
#define BCMTM_PT_INTERNAL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>

#ifndef BSL_LOG_MODULE
#define BSL_LOG_MODULE 1
#endif


#define BCMTM_PT_DYN_INFO(pt_dyn_info, id, inst)\
    do { \
        pt_dyn_info.index = id; \
        pt_dyn_info.tbl_inst = inst; \
    } while(0); \


#define REG_PORT_ANY        -1
/*  use bit 31 to indicate the port argument is an instance id */
#define SOC_REG_ADDR_INSTANCE_BP         (31)
#define SOC_REG_ADDR_INSTANCE_MASK       (1<<SOC_REG_ADDR_INSTANCE_BP)

#define BCMTM_MAX_ENTRY_WSIZE 3
#define BCMTM_MAX_THD_ENTRY_WSIZE 12
/*!
 * \brief writes to the corresponding physical SID for indexed table.
 *
 * based on the index and instance passed in py_dyn_info the physical table is
 * populated with the content of ltmbuf
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id     Transaction ID
 * \param [in] sid          Physical table sid.
 * \param [in] ltid         Logical table sid.
 * \param [in] pt_dyn_info  Physical Table index and instance information.
 * \param [in] ltmbuf       data to be written to the physical table.
 *
 * \retval SHR_E_PARAM      Hardware table not found.
 * \retval SHR_E_NONE       In case of no failure.
 */
static inline
int bcmtm_pt_indexed_write(int unit,
                           bcmdrd_sid_t sid,
                           bcmltd_sid_t lt_id,
                           void *pt_dyn_info,
                           uint32_t *ltmbuf)
{
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit, 0, sid, pt_dyn_info,
                           NULL, ltmbuf, lt_id, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief reads from the corresponding physical SID for indexed table.
 *
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id     Transaction ID
 * \param [in] sid          Physical table sid.
 * \param [in] ltid         Logical table sid.
 * \param [in] pt_dyn_info  Physical Table index and instance information.
 * \param [in] ltmbuf       data to be written to the physical table.
 *
 * \retval SHR_E_PARAM      Hardware table not found.
 * \retval SHR_E_NONE       In case of no failure.
 */
static inline
int bcmtm_pt_indexed_read(int unit,
                          bcmdrd_sid_t sid,
                          bcmltd_sid_t lt_id,
                          void *pt_dyn_info,
                          uint32_t *ltmbuf)
{
    bcmdrd_sym_info_t sinfo;
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;

    SHR_FUNC_ENTER(unit);
    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmptm_ireq_read(unit, 0, sid, pt_dyn_info,
                                     NULL, sinfo.entry_wsize,
                                     lt_id, ltmbuf,
                                     &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief get the physical table field size
 *
 * return the size of a field in the physical table.
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] fid          Physical table field id.
 * \param [in] size         Field width.
 *
 * \retval   returns the size of the field.
 */
static inline uint32_t
bcmtm_pt_fid_size_get(int unit,
                      bcmdrd_sid_t sid,
                      bcmdrd_fid_t fid,
                      uint32_t *size)
{
    bcmdrd_sym_field_info_t finfo;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    *size = (finfo.maxbit - finfo.minbit + 1);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief prepares buffer for the physical table
 *
 * set the field value to the buffer for physical table write
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] fid          Physical table field id.
 * \param [in] entbuf       buffer to be updated.
 * \param [in] fbuf         field value.
 *
 * \retval SHR_E_NOT_FOUND  finfo not found for sid and fid.
 */
static inline int
bcmtm_field_set(int unit,
                   bcmdrd_sid_t sid,
                   bcmdrd_fid_t fid,
                   uint32_t *entbuf,
                   uint32_t *fbuf)
{
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    bcmdrd_field_set(entbuf, finfo.minbit, finfo.maxbit, fbuf);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief get field values for the fields physical table
 *
 * get the field value to the buffer for physical table
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          Physical table sid.
 * \param [in] fid          Physical table field id.
 * \param [in] entbuf       buffer to be updated.
 * \param [in] fbuf         field value.
 *
 * \retval SHR_E_NOT_FOUND  finfo not found for sid and fid.
 */
static inline int
bcmtm_field_get(int unit,
                bcmdrd_sid_t sid,
                bcmdrd_fid_t fid,
                uint32_t *entbuf,
                uint32_t *fbuf)
{
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);
    *fbuf = 0;
    SHR_IF_ERR_EXIT(bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    bcmdrd_field_get(entbuf, finfo.minbit, finfo.maxbit, fbuf);
exit:
    SHR_FUNC_EXIT();
}

static inline int
bcmtm_pt_sid_xform(int unit,
                   bcmdrd_sid_t in_sid,
                   int port,
                   int in_inst,
                   bcmdrd_sid_t *out_sid,
                   int *out_inst)
{
    bcmtm_drv_t *bcmtm_drv;
    bcmdrd_sym_info_t sinfo;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &bcmtm_drv));
    (void)bcmtm_drv->pt_sid_rename(unit, in_sid, in_inst, out_sid, out_inst);

    SHR_IF_ERR_EXIT(bcmdrd_pt_info_get(unit, *out_sid, &sinfo));

exit:
    SHR_FUNC_EXIT();
}

/* Function to get fields in PT buffer */
static inline int
bcmtm_pt_field_get(int unit,
                   bcmdrd_sid_t sid,
                   bcmdrd_fid_t fid,
                   uint32_t *entbuf,
                   uint32_t *fbuf)
{
    int inst = -1;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    *fbuf = 0;

    (void)(bcmtm_pt_sid_xform(unit, sid, -1, inst, &sid, &inst));

    SHR_IF_ERR_EXIT(bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    bcmdrd_field_get(entbuf, finfo.minbit, finfo.maxbit, fbuf);

exit:
    SHR_FUNC_EXIT();
}

/* Function to configure fields in PT buffer */
static inline int
bcmtm_pt_field_set(int unit,
                   bcmdrd_sid_t sid,
                   bcmdrd_fid_t fid,
                   uint32_t *entbuf,
                   uint32_t *fbuf)
{
    int inst = -1;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_pt_sid_xform(unit, sid, -1, 0, &sid, &inst));

    SHR_IF_ERR_EXIT(bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    bcmdrd_field_set(entbuf, finfo.minbit, finfo.maxbit, fbuf);

exit:
    SHR_FUNC_EXIT();
}


/* Write indexed physical table interactively */
static inline int
bcmtm_pt_iwrite(int unit,
                bcmdrd_sid_t sid,
                int port,
                int inst,
                int idx,
                uint32_t *data)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;
    bcmltd_sid_t lt_id = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_pt_sid_xform(unit, sid, port, inst, &sid, &inst));

    /* For per-port data structures, port num is the inst_id */
    if (port != REG_PORT_ANY) {
        inst = port;
    }

    BCMTM_PT_DYN_INFO(pt_dyn_info, idx, inst);

    /* Use PTM function instead of BD direct access */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit, BCMPTM_REQ_FLAGS_PASSTHRU, sid, &pt_dyn_info,
                           NULL, data, lt_id, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/* Read indexed physical table interactively */
static inline int
bcmtm_pt_iread(int unit,
               bcmdrd_sid_t sid,
               int port,
               int inst,
               int idx,
               uint32_t *data)
{
    uint32_t wsize;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t lt_id = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT(bcmtm_pt_sid_xform(unit, sid, port, inst, &sid, &inst));

    /* For per-port data structures, port num is the inst_id */
    if (port != REG_PORT_ANY) {
        inst = port;
    }


    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    BCMTM_PT_DYN_INFO(pt_dyn_info, idx, inst);

    /* Use PTM function instead of BD direct access */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit, BCMPTM_REQ_FLAGS_PASSTHRU, sid, &pt_dyn_info, NULL, wsize,
                          lt_id, data, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}



static inline int
bcmtm_num_pipe(int unit)
{
    return 4;
}


#endif /* BCMTM_PT_INTERNAL_H */
