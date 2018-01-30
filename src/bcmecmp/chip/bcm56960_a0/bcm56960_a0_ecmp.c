/*! \file bcm56960_a0_ecmp.c
 *
 * This file implements BCM56960_A0 device specific ECMP functions.
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
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_pb.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include "bcm56960_a0_ecmp.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Local definitions
 */
static bool bcmecmp_use_dma = TRUE;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Initalize a bcmecmp_pt_op_info_t structure memory.
 *
 * \param [in] pt_op_info Pointer to bcmecmp_pt_op_info_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_pt_op_info_t_init(bcmecmp_pt_op_info_t *pt_op_info)
{
    /* Initialize structure member variables. */
    if (pt_op_info) {
        pt_op_info->drd_sid = INVALIDm;
        pt_op_info->req_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->rsp_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->trans_id = (uint32_t)BCMECMP_INVALID;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = BCMECMP_INVALID;
        pt_op_info->dyn_info.tbl_inst = BCMECMP_ALL_PIPES_INST;
        pt_op_info->wsize = 0;
        pt_op_info->dma = FALSE;
        pt_op_info->ecount = 0;
    }

    return;
}

/*!
 * \brief Initalize a bcmecmp_grp_drd_fields_t structure memory.
 *
 * \param [in] gdrd Pointer to bcmecmp_grp_drd_fields_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_grp_drd_fields_t_init(bcmecmp_grp_drd_fields_t *gdrd)
{
    if (gdrd) {
        sal_memset
            (gdrd, BCMECMP_INVALID, sizeof(*gdrd));
    }
    return;
}

/*!
 * \brief Initalize a bcmecmp_grp_ltd_fields_t structure memory.
 *
 * \param [in] gltd Pointer to bcmecmp_grp_ltd_fields_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_grp_ltd_fields_t_init(bcmecmp_grp_ltd_fields_t *gltd)
{
    if (gltd) {
        sal_memset(gltd, BCMECMP_INVALID, sizeof(*gltd));
    }
    return;
}

/*!
 * \brief Initalize a bcmecmp_memb_drd_fields_t structure memory.
 *
 * \param [in] mdrd Pointer to bcmecmp_memb_drd_fields_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_memb_drd_fields_t_init(bcmecmp_memb_drd_fields_t *mdrd)
{
    if (mdrd) {
        sal_memset
            (mdrd, BCMECMP_INVALID, sizeof(*mdrd));
    }
    return;
}

/*!
 * \brief Initalize a bcmecmp_lt_field_attrs_t structure memory.
 *
 * \param [in] fattrs Pointer to bcmecmp_lt_field_attrs_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_lt_field_attrs_t_init(bcmecmp_lt_field_attrs_t *fattrs)
{
    if (fattrs) {
        sal_memset(fattrs, BCMECMP_INVALID, sizeof(*fattrs));
    }
    return;
}

/*!
 * \brief Write an entry or entries to a table given.
 *
 * Write an entry or entries to a table given index or start index and number
 * of entries to write.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmecmp_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56960_a0_ecmp_pt_write(int unit,
                          bcmecmp_pt_op_info_t *op_info,
                          uint32_t *buf)
{
    uint32_t *rsp_ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        op_info->req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        op_info->req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_write[drd_sid=%d idx=%d]: trans_id=0x%x "
                          "req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->trans_id,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Read an entry or entries from a table given.
 *
 * Read an entry or entries from a table given index or start index and number
 * of entries to read.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmecmp_pt_op_info_t structure.
 * \param [out] buf Pointer to hardware table entry read data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware table read operation unsuccessful.
 */
static int
bcm56960_a0_ecmp_pt_read(int unit,
                         bcmecmp_pt_op_info_t *op_info,
                         uint32_t *buf)
{
    uint32_t *ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-in[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->trans_id,
              op_info->req_flags,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        op_info->req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));

    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        op_info->req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }

    /*
     * PTM Values:
     *  - BCMPTM_LTID_RSP_NOT_SUPPORTED 0xFFFFFFFD
     *  - BCMPTM_LTID_RSP_HW 0xFFFFFFFF
     */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-out[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->trans_id,
              op_info->req_flags,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an ECMP Group into group and member tables using DMA mechanism.
 *
 * Installs an ECMP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_grp_dma_ins(int unit,
                             void *info)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    bcmecmp_memb_drd_fields_t *memb_flds; /* Member table DRD fields. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    uint32_t *dma_buf = NULL;             /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;            /* Local DMA buffer alloc size. */
    uint32_t hw_npaths = 0;               /* HW (zero based) num_paths value. */
    int mstart_idx = BCMECMP_INVALID;     /* Grp start index in Member table. */
    uint32_t nhop_arr_idx;                /* NHOP array index variable. */
    uint32_t val = 0;                     /* Temporary local variable. */
    uint32_t rrlb_cnt = 0;                /* Reset RRLB counter value. */
    uint32_t ent_size = 0;                /* Table entry size. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_dma_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960GrpDmaInsPtEntBuf");


    /* Allocate buffer for INITIAL_L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, init_member, gtype) * sizeof(uint32_t) *
                        lt_ent->max_paths);
    BCMECMP_ALLOC
        (dma_buf, dma_alloc_sz, "bcmecmpBcm56960GrpDmaInsInitMembDmaBuf");

    /*
     * Program the Member table entries in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    memb_flds = (bcmecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* INITIAL_L3_ECMP member table entry size. */
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);

    for (nhop_arr_idx = 0; nhop_arr_idx < lt_ent->num_paths; nhop_arr_idx++) {
        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Pre-IFP ECMP member table.
         */
        if (lt_ent->ecmp_nhop) {
            val = (uint32_t)lt_ent->uecmp_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID
                                        (unit, init_member, gtype),
                                     &dma_buf[nhop_arr_idx * ent_size],
                                     memb_flds->group_id,
                                     &(val)));
        } else {
            val = (uint32_t)lt_ent->nhop_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID
                                        (unit, init_member, gtype),
                                     &dma_buf[nhop_arr_idx * ent_size],
                                     memb_flds->nhop_id,
                                     &(val)));
        }

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, init_member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->ecmp_flag,
                                 &(val)));
    }

    /* Update with Pre-IFP ECMP Member table DRD SID value. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
    op_info.dyn_info.index = mstart_idx;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dma = TRUE;
    op_info.ecount = lt_ent->max_paths;

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Allocate buffer for L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                        lt_ent->max_paths);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56960GrpDmaInsMembDmaBuf");

    /* L3_ECMP member table entry size. */
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    for (nhop_arr_idx = 0; nhop_arr_idx < lt_ent->num_paths; nhop_arr_idx++) {
        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Post-IFP ECMP member table.
         */
        if (lt_ent->ecmp_nhop) {
            val = (uint32_t)lt_ent->uecmp_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[nhop_arr_idx * ent_size],
                                     memb_flds->group_id,
                                     &(val)));
        } else {
            val = (uint32_t)lt_ent->nhop_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[nhop_arr_idx * ent_size],
                                     memb_flds->nhop_id,
                                     &(val)));
        }

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->ecmp_flag,
                                 &(val)));
    }

    /* Update with Post-IFP ECMP Member table DRD SID value. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.dyn_info.index = mstart_idx;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dma = TRUE;
    op_info.ecount = lt_ent->max_paths;

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /*
     * Program Group table entry in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    /* Program Pre-IFP ECMP resolution Group table. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = (lt_ent->ecmp_id);
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, init_group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    /*
     * If num_paths value is non-zero, decrement it by '1'
     * as hardware count field value is zero base.
     */
    if (lt_ent->num_paths) {
        hw_npaths = (uint32_t) lt_ent->num_paths - 1;
    }
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Program Post-IFP ECMP resolution Group table. */
    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /*
     * Reset the group's RRLB_CNT count field value for INSERT and UPDATE
     * operations.
     */
    if (BCMECMP_LB_MODE_ROUNDROBIN == lt_ent->lb_mode) {
        op_info.drd_sid = BCMECMP_TBL_DRD_SID
                                (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE
                                (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE);
        /* Clear the buffer. */
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(*pt_entry_buff));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 op_info.drd_sid,
                                 pt_entry_buff,
                                 grp_flds->rrlb_cnt,
                                 &rrlb_cnt));
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    exit:
        BCMECMP_FREE(pt_entry_buff);
        if (SHR_FUNC_ERR() && dma_buf) {
            BCMECMP_FREE(dma_buf);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an ECMP Group into group and member tables using PIO mechanism.
 *
 * Installs an ECMP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_grp_pio_ins(int unit,
                             void *info)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    bcmecmp_memb_drd_fields_t *memb_flds; /* Member table DRD fields. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    uint32_t hw_npaths = 0;               /* HW (zero based) num_paths value. */
    int mstart_idx = BCMECMP_INVALID;     /* Grp start index in Member table. */
    uint32_t nhop_arr_idx;                /* NHOP array index variable. */
    uint32_t val = 0;                     /* Temporary local variable. */
    uint32_t rrlb_cnt = 0;                /* Reset RRLB counter value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_pio_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960GrpPioInsPtBuf");

    /*
     * Program the Member table entries in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    memb_flds = (bcmecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    for (nhop_arr_idx = 0; nhop_arr_idx < lt_ent->num_paths; nhop_arr_idx++) {
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Pre-IFP ECMP member table.
         */
        if (lt_ent->ecmp_nhop) {
            val = (uint32_t)lt_ent->uecmp_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID
                                        (unit, init_member, gtype),
                                     pt_entry_buff,
                                     memb_flds->group_id,
                                    &(val)));
        } else {
            val = (uint32_t)lt_ent->nhop_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID
                                        (unit, init_member, gtype),
                                     pt_entry_buff,
                                     memb_flds->nhop_id,
                                     &(val)));
        }

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, init_member, gtype),
                                 pt_entry_buff,
                                 memb_flds->ecmp_flag,
                                 &(val)));

        /* Update with Post-IFP ECMP Member table DRD SID value. */
        op_info.trans_id = lt_ent->trans_id;
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
        op_info.dyn_info.index = (mstart_idx + (int)nhop_arr_idx);
        op_info.op = BCMPTM_OP_WRITE;

        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Post-IFP ECMP member table.
         */
        if (lt_ent->ecmp_nhop) {
            val = (uint32_t)lt_ent->uecmp_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     pt_entry_buff,
                                     memb_flds->group_id,
                                     &(val)));
        } else {
            val = (uint32_t)lt_ent->nhop_id[nhop_arr_idx];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     pt_entry_buff,
                                     memb_flds->nhop_id,
                                     &(val)));
        }

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 pt_entry_buff,
                                 memb_flds->ecmp_flag,
                                 &(val)));

        /* Update with Post-IFP ECMP Member table DRD SID value. */
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    /* Clear the hardware entries that are between num_paths and max_paths. */
    for (nhop_arr_idx = lt_ent->num_paths;
         nhop_arr_idx < lt_ent->max_paths;
         nhop_arr_idx++) {
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /* Update with Post-IFP ECMP Member table DRD SID value. */
        op_info.trans_id = lt_ent->trans_id;
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.op = BCMPTM_OP_WRITE;
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
        op_info.dyn_info.index = (mstart_idx + (int)nhop_arr_idx);

        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

        /* Update with Post-IFP ECMP Member table DRD SID value. */
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    /*
     * Program Group table entry in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    /* Program Pre-IFP ECMP resolution Group table. */
    op_info.dyn_info.index = (lt_ent->ecmp_id);
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, init_group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    /*
     * If num_paths value is non-zero, decrement it by '1'
     * as hardware count field value is zero base.
     */
    if (lt_ent->num_paths) {
        hw_npaths = (uint32_t) lt_ent->num_paths - 1;
    }
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Program Post-IFP ECMP resolution Group table. */
    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /*
     * Reset the group's RRLB_CNT count field value for INSERT and UPDATE
     * operations.
     */
    if (BCMECMP_LB_MODE_ROUNDROBIN == lt_ent->lb_mode) {
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, rrlb_cnt,
                                              BCMECMP_GRP_TYPE_SINGLE);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, rrlb_cnt,
                                             BCMECMP_GRP_TYPE_SINGLE);
        /* Clear the buffer. */
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(*pt_entry_buff));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 op_info.drd_sid,
                                 pt_entry_buff,
                                 grp_flds->rrlb_cnt,
                                 &rrlb_cnt));
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    exit:
        BCMECMP_FREE(pt_entry_buff);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an ECMP Group from group and member tables using DMA mechanism.
 *
 * Deletes an ECMP Group Logical Table entry from hardware Group and Member
 * tables. Entries are deleted from Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_grp_dma_del(int unit,
                             void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL; /* LT entry pointer. */
    uint32_t *pt_entry_buff = NULL;    /* Ptr to physical table entry buf. */
    uint32_t *dma_buf = NULL;          /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;         /* Local DMA buffer alloc size. */
    uint32_t max_paths;                /* Group MAX_PATHS value. */
    bcmecmp_pt_op_info_t op_info;      /* Physical table operation info. */
    int start_idx = BCMECMP_INVALID;   /* Member table start index. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_dma_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Get Group start index in ECMP Member table. */
    start_idx =  BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);

    /* Get MAX_PATHS value of the group. */
    max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960GrpDmaDelPtBuf");

    /* Allocate buffer for INITIAL_L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, init_member, gtype) * sizeof(uint32_t) *
                         max_paths);
    BCMECMP_ALLOC
        (dma_buf, dma_alloc_sz, "bcmecmpBcm56960GrpDmaDelInitMembDmaBuf");

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Update with Post-IFP ECMP Member table DRD SID value. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
    op_info.dyn_info.index = start_idx;
    op_info.dma = TRUE;
    op_info.ecount = max_paths;

    /* Write entry to Pre-IFP Member table. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Allocate buffer for L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) * max_paths);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56960GrpDmaDelMembDmaBuf");

    /* Update Post-IFP ECMP Member table DRD SID value and word size.*/
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.dyn_info.index = start_idx;
    op_info.dma = TRUE;
    op_info.ecount = max_paths;

    /* Write entry to Post-IFP Member table. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Delete Group entry in Pre-IFP ECMP Group Table. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = lt_ent->ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /* Delete Group entry in Post-IFP ECMP Group Table. */
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /* Clear group's RRLB_CNT value in the hardware table. */
    if (BCMECMP_LB_MODE_ROUNDROBIN == lt_ent->lb_mode) {
        op_info.drd_sid = BCMECMP_TBL_DRD_SID
                            (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE
                            (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE);
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    exit:
        BCMECMP_FREE(pt_entry_buff);
        if (SHR_FUNC_ERR() && dma_buf) {
            BCMECMP_FREE(dma_buf);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an ECMP Group from group and member tables using PIO mechanism.
 *
 * Deletes an ECMP Group Logical Table entry from hardware Group and Member
 * tables. Entries are deleted from Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_grp_pio_del(int unit,
                             void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL; /* LT entry pointer. */
    uint32_t *pt_entry_buff = NULL;    /* Ptr to physical table entry buf. */
    bcmecmp_pt_op_info_t op_info;      /* Physical table operation info. */
    uint32_t nhop_arr_idx;             /* NHOP array index iterator. */
    int start_idx = BCMECMP_INVALID;   /* Member table start index. */
    uint32_t max_paths;                /* Group MAX_PATHS value. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_pio_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960GrpPioDelPtBuf");

    /* Get Group start index in ECMP Member table. */
    start_idx =  BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);

    /* Get MAX_PATHS value of the group. */
    max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);
    for (nhop_arr_idx = 0; nhop_arr_idx < max_paths; nhop_arr_idx++) {
        /* Setup BCMPTM_OP_WRITE operation parameters. */
        op_info.trans_id = lt_ent->trans_id;
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
        op_info.dyn_info.index = (start_idx + (int)nhop_arr_idx);
        op_info.op = BCMPTM_OP_WRITE;

        /* Write entry to Pre-IFP Member table. */
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit,
                                       &op_info,
                                       pt_entry_buff));

        /* Update Post-IFP ECMP Member table DRD SID value and word size.*/
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

        /* Write entry to Post-IFP Member table. */
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    /* Delete Group entry in Pre-IFP ECMP Group Table. */
    op_info.dyn_info.index = lt_ent->ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /* Delete Group entry in Post-IFP ECMP Group Table. */
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /* Clear group's RRLB_CNT value in the hardware table. */
    if (BCMECMP_LB_MODE_ROUNDROBIN == lt_ent->lb_mode) {
        op_info.drd_sid = BCMECMP_TBL_DRD_SID
                            (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE
                            (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE);
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }
    exit:
        BCMECMP_FREE(pt_entry_buff);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Construct an ECMP group LT entry by reading table entries using DMA.
 *
 * Construct an ECMP Group Logical Table entry by getting field values from
 * entries in group and member tables.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_grp_dma_get(int unit,
                             void *info)
{
    uint32_t *pt_entry_buff = NULL;       /* Pointer to PT entry buffer. */
    bcmecmp_lt_entry_t *lt_ent = NULL;    /* Pointer to LT entry buffer. */
    bcmecmp_pt_op_info_t op_info;         /* PT operation info structure. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Pointer to Group table fields. */
    bcmecmp_memb_drd_fields_t *memb_flds; /* Pointer to Member table fields. */
    uint32_t *dma_buf = NULL;             /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;            /* Local DMA buffer alloc size. */
    uint32_t nhop_arr_idx;                /* Next-Hop array index. */
    uint32_t num_paths[1] = {0};          /* Variable to store HW num_paths. */
    uint32_t lb_mode[1];                  /* Variable to store HW lb_mode. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    uint32_t val = 0;                     /* Temporary local variable. */
    uint32_t ent_size = 0;                /* Table entry size. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_dma_get.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960GrpDmaGetPtEntBuf");

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Set BCMPTM_OP_READ operation parameters for group table. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.dyn_info.index = lt_ent->ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    op_info.op = BCMPTM_OP_READ;

    /* Read Post-IFP ECMP Group hardware table entry. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_read(unit, &op_info, pt_entry_buff));

    /* Initialize ECMP Group table hardware fields information. */
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Get LB_MODE field value from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             lb_mode));
    lt_ent->lb_mode = lb_mode[0];

    /* Get NUM_PATHS field value from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             num_paths));

    /*
     * Increment hardware NUM_PATHS value by "1" as zero-based numbering
     * is used for this "COUNT" field in the hardware entry.
     */
    lt_ent->num_paths = num_paths[0] + 1;

    /* Get ECMP Member table base pointer from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &val));
    lt_ent->mstart_idx = (int)val;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d LB_MODE=%u NUM_PATHS=%d BASE_IDX"
                            "=%d.\n"),
                lt_ent->ecmp_id,
                lt_ent->lb_mode,
                lt_ent->num_paths,
                lt_ent->mstart_idx));

    /* Allocate buffer for INITIAL_L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         lt_ent->num_paths);
    BCMECMP_ALLOC
        (dma_buf, dma_alloc_sz, "bcmecmpBcm56960GrpDmaGetMembDmaBuf");

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Set BCMPTM_OP_READ operation parameters for member table. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * lt_ent->num_paths);
    op_info.op = BCMPTM_OP_READ;
    op_info.dyn_info.index = lt_ent->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = lt_ent->num_paths;

    /* Read the entry from PTM layer. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_read(unit, &op_info, dma_buf));

    memb_flds = (bcmecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    /* L3_ECMP member table entry size. */
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    for (nhop_arr_idx = 0; nhop_arr_idx < lt_ent->num_paths; nhop_arr_idx++) {

        /* Retrieve ECMP_FLAG value from the PT entry buffer. */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_get(unit,
                                 op_info.drd_sid,
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->ecmp_flag,
                                 &val));
        lt_ent->ecmp_nhop = val ? TRUE : FALSE;

        /* Check ECMP_NHOP status. */
        if (lt_ent->ecmp_nhop) {
            /* Retrieve ECMP_UNDERLAY_ID value from the PT entry buffer. */
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit,
                                     op_info.drd_sid,
                                     &dma_buf[nhop_arr_idx * ent_size],
                                     memb_flds->group_id,
                                     &val));
            lt_ent->uecmp_id[nhop_arr_idx] = (bcmecmp_id_t)val;
        } else {
            /* Retrieve the NHOP_ID value from the PT entry buffer. */
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit,
                                     op_info.drd_sid,
                                     &dma_buf[nhop_arr_idx * ent_size],
                                     memb_flds->nhop_id,
                                     &val));
            lt_ent->nhop_id[nhop_arr_idx] = (bcmecmp_nhop_id_t)val;
        }
    }
    BCMECMP_FREE(dma_buf);

    exit:
        BCMECMP_FREE(pt_entry_buff);
        if (SHR_FUNC_ERR() && dma_buf) {
            BCMECMP_FREE(dma_buf);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Construct an ECMP group LT entry by reading table entries using PIO.
 *
 * Construct an ECMP Group Logical Table entry by getting field values from
 * entries in group and member tables.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_grp_pio_get(int unit,
                             void *info)
{
    uint32_t *pt_entry_buff = NULL;       /* Pointer to PT entry buffer. */
    bcmecmp_lt_entry_t *lt_ent = NULL;    /* Pointer to LT entry buffer. */
    bcmecmp_pt_op_info_t op_info;         /* PT operation info structure. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Pointer to Group table fields. */
    bcmecmp_memb_drd_fields_t *memb_flds; /* Pointer to Member table fields. */
    uint32_t nhop_arr_idx;                /* Next-Hop array index. */
    uint32_t num_paths[1] = {0};          /* Variable to store HW num_paths. */
    uint32_t lb_mode[1];                  /* Variable to store HW lb_mode. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    uint32_t val = 0;                     /* Temporary local variable. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_pio_get.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960GrpPioGetPtBuf");

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Set BCMPTM_OP_READ operation parameters for group table. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.dyn_info.index = lt_ent->ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    op_info.op = BCMPTM_OP_READ;

    /* Read Post-IFP ECMP Group hardware table entry. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_read(unit, &op_info, pt_entry_buff));

    /* Initialize ECMP Group table hardware fields information. */
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Get LB_MODE field value from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             lb_mode));
    lt_ent->lb_mode = lb_mode[0];

    /* Get NUM_PATHS field value from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             num_paths));

    /*
     * Increment hardware NUM_PATHS value by "1" as zero-based numbering
     * is used for this "COUNT" field in the hardware entry.
     */
    lt_ent->num_paths = num_paths[0] + 1;

    /* Get ECMP Member table base pointer from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &val));
    lt_ent->mstart_idx = (int) val;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d LB_MODE=%u NUM_PATHS=%d BASE_IDX"
                            "=%d.\n"),
                lt_ent->ecmp_id,
                lt_ent->lb_mode,
                lt_ent->num_paths,
                lt_ent->mstart_idx));

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Update with member table DRD_SID value for BCMPTM_OP_READ. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.op = BCMPTM_OP_READ;

    memb_flds = (bcmecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    for (nhop_arr_idx = 0; nhop_arr_idx < lt_ent->num_paths; nhop_arr_idx++) {
        /* Update hardware index to be read. */
        op_info.dyn_info.index = (lt_ent->mstart_idx + (int)nhop_arr_idx);

        /* Clear buffer to be used for hardware read operation. */
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /* Read the entry from PTM layer. */
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_read(unit, &op_info, pt_entry_buff));

        /* Retrieve ECMP_FLAG value from the PT entry buffer. */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_get(unit,
                                 op_info.drd_sid,
                                 pt_entry_buff,
                                 memb_flds->ecmp_flag,
                                 &val));
        lt_ent->ecmp_nhop = val ? TRUE : FALSE;

        /* Check ECMP_NHOP status. */
        if (lt_ent->ecmp_nhop) {
            /* Retrieve ECMP_UNDERLAY_ID value from the PT entry buffer. */
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit,
                                     op_info.drd_sid,
                                     pt_entry_buff,
                                     memb_flds->group_id,
                                     &val));
            lt_ent->uecmp_id[nhop_arr_idx] = (bcmecmp_id_t) val;
        } else {
            /* Retrieve the NHOP_ID value from the PT entry buffer. */
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit,
                                     op_info.drd_sid,
                                     pt_entry_buff,
                                     memb_flds->nhop_id,
                                     &val));
            lt_ent->nhop_id[nhop_arr_idx] = (bcmecmp_nhop_id_t) val;
        }
    }

    exit:
        BCMECMP_FREE(pt_entry_buff);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Insert a RH-ECMP group into device group and member tables using DMA.
 *
 * Installs a RH-ECMP group logical table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_rh_grp_dma_ins(int unit,
                                void *info)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    bcmecmp_memb_drd_fields_t *memb_flds; /* Member table DRD fields. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *dma_buf = NULL;             /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;            /* Local DMA buffer alloc size. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    int mstart_idx = BCMECMP_INVALID;     /* Grp start index in Member table. */
    uint32_t ent_arr_idx;                 /* NHOP array index variable. */
    uint32_t val = 0;                     /* Temporary local variable. */
    uint32_t ent_size = 0;                /* Table entry size. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_rh_grp_dma_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960RhGrpDmaInsPtEntBuf");

    /* Allocate buffer for INITIAL_L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, init_member, gtype) * sizeof(uint32_t) *
                        lt_ent->rh_entries_cnt);
    BCMECMP_ALLOC
        (dma_buf, dma_alloc_sz, "bcmecmpBcm56960RhGrpDmaInsInitMembDmaBuf");

    /*
     * Program the Member table entries in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    memb_flds = (bcmecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* INITIAL_L3_ECMP member table entry size. */
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);

    for (ent_arr_idx = 0; ent_arr_idx < lt_ent->rh_entries_cnt;
         ent_arr_idx++) {
        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Pre-IFP ECMP member table.
         */
        val = (uint32_t)lt_ent->rh_entries_arr[ent_arr_idx];
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, init_member, gtype),
                                 &dma_buf[ent_arr_idx * ent_size],
                                 (lt_ent->ecmp_nhop ? memb_flds->group_id :
                                                        memb_flds->nhop_id),
                                 &(val)));

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, init_member, gtype),
                                 &dma_buf[ent_arr_idx * ent_size],
                                 memb_flds->ecmp_flag,
                                 &(val)));
    }

    /* Setup BCMPTM_OP_WRITE operation parameters. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
    op_info.dyn_info.index = mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = lt_ent->rh_entries_cnt;

    /* Update with Post-IFP ECMP Member table DRD SID value. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Allocate buffer for L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                        lt_ent->rh_entries_cnt);
    BCMECMP_ALLOC
        (dma_buf, dma_alloc_sz, "bcmecmpBcm56960RhGrpDmaInsMembDmaBuf");

    /* INITIAL_L3_ECMP member table entry size. */
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    for (ent_arr_idx = 0; ent_arr_idx < lt_ent->rh_entries_cnt;
         ent_arr_idx++) {
        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Post-IFP ECMP member table.
         */
        val = (uint32_t)lt_ent->rh_entries_arr[ent_arr_idx];
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[ent_arr_idx * ent_size],
                                 (lt_ent->ecmp_nhop ? memb_flds->group_id :
                                                        memb_flds->nhop_id),
                                 &(val)));

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[ent_arr_idx * ent_size],
                                 memb_flds->ecmp_flag,
                                 &(val)));
    }

    /* Update with Post-IFP ECMP Member table DRD SID value. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.dyn_info.index = mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = lt_ent->rh_entries_cnt;

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);
    /*
     * Program Group table entry in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    /* Program Pre-IFP ECMP resolution Group table. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = (lt_ent->ecmp_id);
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);

    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, init_group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(lt_ent->rh_size_enc)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                            &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /* Update with group table DRD_SID value for BCMPTM_OP_WRITE. */
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Program Post-IFP ECMP resolution Group table. */
    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(lt_ent->rh_size_enc)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    exit:
        BCMECMP_FREE(pt_entry_buff);
        if (SHR_FUNC_ERR() && dma_buf) {
            BCMECMP_FREE(dma_buf);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Insert a RH-ECMP group into device group and member tables using PIO.
 *
 * Installs a RH-ECMP group logical table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_rh_grp_pio_ins(int unit,
                                void *info)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */
    bcmecmp_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmecmp_pt_op_info_t op_info;         /* Physical Table operation info. */
    bcmecmp_memb_drd_fields_t *memb_flds; /* Member table DRD fields. */
    bcmecmp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    int mstart_idx = BCMECMP_INVALID;     /* Grp start index in Member table. */
    uint32_t ent_arr_idx;                 /* NHOP array index variable. */
    uint32_t val = 0;                     /* Temporary local variable. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_rh_grp_pio_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960RhGrpPioInsPtEntBuf");

    /*
     * Program the Member table entries in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    memb_flds = (bcmecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    for (ent_arr_idx = 0; ent_arr_idx < lt_ent->rh_entries_cnt;
         ent_arr_idx++) {
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Pre-IFP ECMP member table.
         */
        val = (uint32_t)lt_ent->rh_entries_arr[ent_arr_idx];
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, init_member, gtype),
                                 pt_entry_buff,
                                 (lt_ent->ecmp_nhop ? memb_flds->group_id :
                                                        memb_flds->nhop_id),
                                 &(val)));

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, init_member, gtype),
                                 pt_entry_buff,
                                 memb_flds->ecmp_flag,
                                 &(val)));

        /* Update with Post-IFP ECMP Member table DRD SID value. */
        op_info.trans_id = lt_ent->trans_id;
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.op = BCMPTM_OP_WRITE;
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
        op_info.dyn_info.index = (mstart_idx + (int)ent_arr_idx);

        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /*
         * Set Underlay ECMP Group ID or NHOP_ID value based on ECMP_NHOP
         * status in Post-IFP ECMP member table.
         */
        val = (uint32_t)lt_ent->rh_entries_arr[ent_arr_idx];
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 pt_entry_buff,
                                 (lt_ent->ecmp_nhop ? memb_flds->group_id :
                                                        memb_flds->nhop_id),
                                 &(val)));

        val = lt_ent->ecmp_nhop;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 pt_entry_buff,
                                 memb_flds->ecmp_flag,
                                 &(val)));

        /* Update with Post-IFP ECMP Member table DRD SID value. */
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    /*
     * Program Group table entry in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */
    /* Program Pre-IFP ECMP resolution Group table. */
    op_info.dyn_info.index = (lt_ent->ecmp_id);
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, init_group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(lt_ent->rh_size_enc)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT(bcmdrd_pt_field_set(unit,
                        op_info.drd_sid,
                        pt_entry_buff,
                        grp_flds->base_ptr,
                        &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Program Post-IFP ECMP resolution Group table. */
    sal_memset(pt_entry_buff, 0,
               BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    val = lt_ent->lb_mode;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(lt_ent->rh_size_enc)));

    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    exit:
        BCMECMP_FREE(pt_entry_buff);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a RH-ECMP group LT entry from device group and member tables.
 *
 * Deletes a RH-ECMP group logical table entry from device Group and Member
 * tables by using DMA mechanism. Entries are deleted from Pre-IFP and Post-IFP
 * ECMP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_rh_grp_dma_del(int unit,
                                void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL; /* LT entry pointer. */
    uint32_t *pt_entry_buff = NULL;    /* Ptr to physical table entry buf. */
    bcmecmp_pt_op_info_t op_info;      /* Physical table operation info. */
    uint32_t *dma_buf = NULL;          /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;         /* Local DMA buffer alloc size. */
    int start_idx = BCMECMP_INVALID;   /* Member table start index. */
    uint32_t rh_entries_cnt;           /* RH group total entries count. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_rh_grp_dma_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960RhGrpDmaDelPtEntBuf");

    /* Get Group start index in ECMP Member table. */
    start_idx =  BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);

    /* Get MAX_PATHS value of the group. */
    rh_entries_cnt = BCMECMP_GRP_RH_ENTRIES_CNT(unit, lt_ent->ecmp_id);

    /* Allocate buffer for INITIAL_L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, init_member, gtype) * sizeof(uint32_t) *
                        rh_entries_cnt);
    BCMECMP_ALLOC
        (dma_buf, dma_alloc_sz, "bcmecmpBcm56960RhGrpDmaDelInitMembDmaBuf");

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Setup BCMPTM_OP_WRITE operation parameters. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
    op_info.dyn_info.index = start_idx;
    op_info.dma = TRUE;
    op_info.ecount = rh_entries_cnt;

    /* Write entry to Pre-IFP Member table. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Allocate buffer for INITIAL_L3_ECMP member table entries. */
    dma_alloc_sz = (BCMECMP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                        rh_entries_cnt);
    BCMECMP_ALLOC
        (dma_buf, dma_alloc_sz, "bcmecmpBcm56960RhGrpDmaDelMembDmaBuf");

    /* Update Post-IFP ECMP Member table DRD SID value and word size.*/
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    op_info.dyn_info.index = start_idx;
    op_info.dma = TRUE;
    op_info.ecount = rh_entries_cnt;


    /* Write entry to Post-IFP Member table. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, dma_buf));
    BCMECMP_FREE(dma_buf);

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);

    /* Delete Group entry in Pre-IFP ECMP Group Table. */
    op_info.trans_id = lt_ent->trans_id;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = lt_ent->ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /* Delete Group entry in Post-IFP ECMP Group Table. */
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    exit:
        BCMECMP_FREE(pt_entry_buff);
        if (SHR_FUNC_ERR() && dma_buf) {
            BCMECMP_FREE(dma_buf);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a RH-ECMP group LT entry from device group and member tables.
 *
 * Deletes a RH-ECMP group logical table entry from device Group and Member
 * tables by using PIO mechanism. Entries are deleted from Pre-IFP and Post-IFP
 * ECMP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56960_a0_ecmp_rh_grp_pio_del(int unit,
                                void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL; /* LT entry pointer. */
    uint32_t *pt_entry_buff = NULL;    /* Ptr to physical table entry buf. */
    bcmecmp_pt_op_info_t op_info;      /* Physical table operation info. */
    uint32_t ent_arr_idx;              /* NHOP array index iterator. */
    int start_idx = BCMECMP_INVALID;   /* Member table start index. */
    uint32_t rh_entries_cnt;           /* RH group total entries count. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* ECMP Group type. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_rh_grp_pio_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Get ECMP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56960RhGrpPioDelPtEntBuf");

    /* Get Group start index in ECMP Member table. */
    start_idx =  BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);

    /* Get MAX_PATHS value of the group. */
    rh_entries_cnt = BCMECMP_GRP_RH_ENTRIES_CNT(unit, lt_ent->ecmp_id);

    /* Initialize op_info structure member values. */
    bcmecmp_pt_op_info_t_init(&op_info);
    for (ent_arr_idx = 0; ent_arr_idx < rh_entries_cnt; ent_arr_idx++) {
        /* Update with Post-IFP ECMP Member table DRD SID value. */
        op_info.trans_id = lt_ent->trans_id;
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.op = BCMPTM_OP_WRITE;
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_member, gtype);
        op_info.dyn_info.index = (start_idx + (int)ent_arr_idx);

        /* Write entry to Pre-IFP Member table. */
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

        /* Update Post-IFP ECMP Member table DRD SID value and word size.*/
        op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, gtype);
        op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

        /* Write entry to Post-IFP Member table. */
        SHR_IF_ERR_EXIT
            (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));
    }

    /* Delete Group entry in Pre-IFP ECMP Group Table. */
    op_info.dyn_info.index = lt_ent->ecmp_id;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, init_group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, init_group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    /* Delete Group entry in Post-IFP ECMP Group Table. */
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    SHR_IF_ERR_EXIT
        (bcm56960_a0_ecmp_pt_write(unit, &op_info, pt_entry_buff));

    exit:
        BCMECMP_FREE(pt_entry_buff);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Tomahawk ECMP tables DRD-SID and LTD-SID information.
 *
 * Initialize DRD-SID and LTD-SID details for ECMP Group, Member and Round-Robin
 * load balancing count tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid ECMP mode value.
 */
static int
bcm56960_a0_ecmp_sids_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = ECMP_CONFIGr;
    BCMECMP_REG_LTD_SID(unit, config) = ECMP_CONFIGt;

    /* Initialize Pre-IFP ECMP resolution tables DRD SID values. */
    BCMECMP_TBL_DRD_SID
        (unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = INITIAL_L3_ECMP_GROUPm;
    BCMECMP_TBL_DRD_SID
        (unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = INITIAL_L3_ECMPm;

    /* Initialize Post-IFP ECMP resolution tables DRD SID values. */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = L3_ECMP_COUNTm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = L3_ECMPm;
    BCMECMP_TBL_DRD_SID
        (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = L3_ECMP_RRLB_CNTm;

    /* Initialize ECMP_MODE specific DRD + LTD SID values. */
    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FAST:
            /* Initialize Fast ECMP logical table SID value. */
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = ECMP_FASTt;
            break;
        case BCMECMP_MODE_FULL:
            /* Initialize Full feature ECMP logical table SID value. */
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = ECMPt;
            break;
        case BCMECMP_MODE_HIER:
            /*
             * For Hierarchical mode initialize Overlay and Underlay ECMP
             * group, Member tables DRD SID values.
             *
             * Note:
             *     On Tomahawk device, Group and Member hardware tables are
             *     split into two to support Overlay and Underlay lookups.
             *     So, the DRD-SID values for Overlay and Underlay tables are
             *     same as the full feature single lookup ECMP tables.
             */
            /* Initialize Pre-IFP ECMP resolution tables DRD SID values. */
            BCMECMP_TBL_DRD_SID
                (unit, init_group, BCMECMP_GRP_TYPE_OVERLAY)
                                                = INITIAL_L3_ECMP_GROUPm;
            BCMECMP_TBL_DRD_SID
                (unit, init_member, BCMECMP_GRP_TYPE_OVERLAY)
                                                = INITIAL_L3_ECMPm;

            /* Initialize Post-IFP ECMP resolution tables DRD-SID values. */
            BCMECMP_TBL_DRD_SID
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = L3_ECMP_COUNTm;
            BCMECMP_TBL_DRD_SID
                (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = L3_ECMPm;

            /* Initialize Pre-IFP ECMP resolution tables DRD SID values. */
            BCMECMP_TBL_DRD_SID
                (unit, init_group, BCMECMP_GRP_TYPE_UNDERLAY)
                                                = INITIAL_L3_ECMP_GROUPm;
            BCMECMP_TBL_DRD_SID
                (unit, init_member, BCMECMP_GRP_TYPE_UNDERLAY)
                                                = INITIAL_L3_ECMPm;

            /* Initialize Post-IFP ECMP resolution tables DRD-SID values. */
            BCMECMP_TBL_DRD_SID
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = L3_ECMP_COUNTm;
            BCMECMP_TBL_DRD_SID
                (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = L3_ECMPm;

            /*
             * Initialize ECMP Group LTD-SID values for full (single), Overlay
             * and Underlay logical tables.
             */
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = ECMP_HIERARCHICALt;
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = ECMP_OVERLAYt;
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = ECMP_UNDERLAYt;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Clear ECMP tables LRD-SID and DRD-SID information.
 *
 * Clear ECMP Group, Member and Round-Robin load balancing hardware tables
 * LRD-SID and DRD-SID information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_ecmp_sids_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = INVALIDm;
    BCMECMP_REG_LTD_SID(unit, config) = BCMLTD_SID_INVALID;

    /* Clear DRD-SID information for single level groups. */
    BCMECMP_TBL_DRD_SID
        (unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;

    /* Clear ECMP Group table LTD-SID information for single level groups. */
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMLTD_SID_INVALID;

    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)) {
        /* Clear Pre-IFP Overlay ECMP tables DRD-SID information. */
        BCMECMP_TBL_DRD_SID
            (unit, init_group, BCMECMP_GRP_TYPE_OVERLAY) = INVALIDm;
        BCMECMP_TBL_DRD_SID
            (unit, init_member, BCMECMP_GRP_TYPE_OVERLAY) = INVALIDm;
        /* Clear Post-IFP Overlay ECMP tables DRD-SID information. */
        BCMECMP_TBL_DRD_SID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = INVALIDm;
        BCMECMP_TBL_DRD_SID
            (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = INVALIDm;
        /* Clear Overlay ECMP Group table LTD-SID information. */
        BCMECMP_TBL_LTD_SID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = BCMLTD_SID_INVALID;

        /* Clear Pre-IFP Underlay ECMP tables DRD-SID information. */
        BCMECMP_TBL_DRD_SID
            (unit, init_group, BCMECMP_GRP_TYPE_UNDERLAY) = INVALIDm;
        BCMECMP_TBL_DRD_SID
            (unit, init_member, BCMECMP_GRP_TYPE_UNDERLAY) = INVALIDm;
        /* Clear Post-IFP Underlay ECMP tables DRD-SID information. */
        BCMECMP_TBL_DRD_SID
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = INVALIDm;
        BCMECMP_TBL_DRD_SID
            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = INVALIDm;
        /* Clear Underlay ECMP Group table LTD-SID information. */
        BCMECMP_TBL_LTD_SID
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = BCMLTD_SID_INVALID;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Debug ECMP information print function.
 *
 * Debug ECMP information print function to verify software ECMP database has
 * been setup correctly based on the device ECMP resolution mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_ecmp_info_print(int unit)
{
    shr_pb_t *pb; /* Print buffer. */

    SHR_FUNC_ENTER(unit);

    pb = shr_pb_create();
    shr_pb_printf(pb,
                  "\n\tECMP INFO - (Unit = %d, ECMP_MODE = %s):\n"
                  "\tECMP_CONFIG (Dev)            : wsize = %d\n"
                  "\t============== Single ==============\n"
                  "\tINITIAL_L3_ECMP_GROUPm (Grp) : wsize = %d\n"
                  "\tL3_ECMP_COUNTm (Grp)         : wsize = %d\n"
                  "\tINITIAL_L3_ECMPm (Memb)      : wsize = %d\n"
                  "\tL3_ECMPm (Memb)              : wsize = %d\n"
                  "\tL3_ECMP_RRLB_CNTm            : wsize = %d\n"
                  "\tECMP Tbls max entry sz       : wsize = %d\n"
                  "\tGrp  : idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                  "\tMemb : idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                  "\t\tECMP_ID   : min = %d, max = %d\n"
                  "\t\tLB_MODE   : min = %d, max = %d\n"
                  "\t\tMAX_PATHS : min = %d, max = %d\n"
                  "\t\tNUM_PATHS : min = %d, max = %d\n"
                  "\t\tNHOP_ID   : min = %d, max = %d\n"
                  "\t\tRH_SIZE   : min = %d, max = %d\n",
                 unit,
                 ((BCMECMP_MODE(unit) == BCMECMP_MODE_FAST) ? "FAST" :
                    (BCMECMP_MODE(unit) == BCMECMP_MODE_FULL) ? "FULL" :
                    (BCMECMP_MODE(unit) == BCMECMP_MODE_HIER) ? "HIER" :
                    "INVALID"),
                 BCMECMP_REG_ENT_SIZE(unit, config),
                 BCMECMP_TBL_ENT_SIZE
                    (unit, init_group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_ENT_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_ENT_SIZE
                    (unit, init_member, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_ENT_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_ENT_SIZE(unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_ENT_SIZE_MAX(unit),
                 BCMECMP_TBL_IDX_MIN(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_IDX_MIN(unit, member, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_IDX_MAX(unit, member, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MIN_ECMP_ID(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MAX_ECMP_ID(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MIN_LB_MODE(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MAX_LB_MODE(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MIN_PATHS(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MAX_PATHS(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MIN_NPATHS(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MAX_NPATHS(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MIN_NHOP_ID(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MAX_NHOP_ID(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MIN_RH_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE),
                 BCMECMP_LT_MAX_RH_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE));

    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)) {
        shr_pb_printf(pb,
                      "\n\t============== Overlay ==============\n"
                      "\tL3_ECMP_COUNTm (OGrp)\t: wsize = %d\n"
                      "\tL3_ECMPm (OMemb)\t: wsize = %d\n"
                      "\tOGrp :idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                      "\tOMemb:idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                      "\t\tECMP_ID   : min = %d, max = %d\n"
                      "\t\tLB_MODE   : min = %d, max = %d\n"
                      "\t\tMAX_PATHS : min = %d, max = %d\n"
                      "\t\tNUM_PATHS : min = %d, max = %d\n"
                      "\t\tUECMP_ID  : min = %d, max = %d\n"
                      "\t\tNHOP_ID   : min = %d, max = %d\n"
                      "\t\tRH_SIZE   : min = %d, max = %d\n"
                      "\n\t============== Underlay ==============\n"
                      "\tL3_ECMP_COUNTm (UGrp)\t: wsize = %d\n"
                      "\tL3_ECMPm (UMemb)\t: wsize = %d\n"
                      "\tUGrp : idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                      "\tUMemb: idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                      "\t\tECMP_ID   : min = %d, max = %d\n"
                      "\t\tLB_MODE   : min = %d, max = %d\n"
                      "\t\tMAX_PATHS : min = %d, max = %d\n"
                      "\t\tNUM_PATHS : min = %d, max = %d\n"
                      "\t\tNHOP_ID   : min = %d, max = %d\n"
                      "\t\tRH_SIZE   : min = %d, max = %d\n",
                     BCMECMP_TBL_ENT_SIZE
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_ENT_SIZE
                        (unit, member, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_IDX_MIN(unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_IDX_MIN
                        (unit, member, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_IDX_MAX
                        (unit, member, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MIN_LB_MODE
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MAX_LB_MODE
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MIN_PATHS
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MAX_PATHS
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MIN_NPATHS
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MAX_NPATHS
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MIN_UECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MAX_UECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MIN_NHOP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MAX_NHOP_ID
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MIN_RH_SIZE
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_LT_MAX_RH_SIZE
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                     BCMECMP_TBL_ENT_SIZE
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_TBL_ENT_SIZE
                        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_TBL_IDX_MIN
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_TBL_IDX_MAX
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_TBL_IDX_MIN
                        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_TBL_IDX_MAX
                        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MIN_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MAX_ECMP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MIN_LB_MODE
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MAX_LB_MODE
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MIN_PATHS
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MAX_PATHS
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MIN_NPATHS
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MAX_NPATHS
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MIN_NHOP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MAX_NHOP_ID
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MIN_RH_SIZE
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                     BCMECMP_LT_MAX_RH_SIZE
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Tomahawk device specific ECMP tables fields information.
 *
 * Initialize ECMP Group and Member tables fields information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid ECMP mode, invalid LB mode, null pointer.
 */
static int
bcm56960_a0_ecmp_fields_init(int unit)
{
    static bcmecmp_grp_drd_fields_t gdrd_fields;  /* Group tbl DRD info. */
    static bcmecmp_memb_drd_fields_t mdrd_fields; /* Member tbl DRD info. */
    static bcmecmp_grp_ltd_fields_t lt_fids;      /* Group tbl LTD info. */
    static bcmecmp_grp_ltd_fields_t olt_fids;     /* Overlay grp LTD fields. */
    static bcmecmp_grp_ltd_fields_t ult_fids;     /* Underlay grp LTD fields. */
    static bcmecmp_lt_field_attrs_t grp_attrs;    /* Single level grp attrs. */
    static bcmecmp_lt_field_attrs_t ogrp_attrs;   /* Overlay grp attrs. */
    static bcmecmp_lt_field_attrs_t ugrp_attrs;   /* Underlay grp attrs. */
    const bcmlrd_map_t *map = NULL;               /* LT map. */
    const bcmlrd_field_data_t *fdata = NULL;      /* LT field data map. */
    uint32_t idx;                                 /* Enum symbol array index. */
    uint32_t lb_val;                              /* LB mode value. */
    const char *const lb_name[BCMECMP_LB_MODE_COUNT] = BCMECMP_LB_MODE_NAME;

    SHR_FUNC_ENTER(unit);

    /* Initialize device ECMP resolution mode field identifier value. */
    BCMECMP_MODE_DRD_FID(unit, config) = ECMP_MODEf;

    /* Initialize ECMP Group hardware table DRD field names. */
    bcmecmp_grp_drd_fields_t_init(&gdrd_fields);
    gdrd_fields.lb_mode = LB_MODEf;
    gdrd_fields.base_ptr = BASE_PTRf;
    gdrd_fields.count = COUNTf;
    gdrd_fields.rrlb_cnt = RRLB_CNTf;

    /* Initialize ECMP Member hardware table DRD field names. */
    bcmecmp_memb_drd_fields_t_init(&mdrd_fields);
    mdrd_fields.ecmp_flag = ECMP_FLAGf;
    mdrd_fields.group_id = NHOP_ECMP_GROUPf;
    mdrd_fields.nhop_id = NEXT_HOP_INDEXf;

    /* Initialize single lookup ECMP Group LT field attributes pointer. */
    bcmecmp_lt_field_attrs_t_init(&grp_attrs);
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = (void *)&grp_attrs;

    /* Check and return error if Field attributes pointer is NULL. */
    SHR_NULL_CHECK
        (BCMECMP_LT_FIELD_ATTRS_VOID_PTR
            (unit, group, BCMECMP_GRP_TYPE_SINGLE), SHR_E_INTERNAL);

    /*
     * Retrieve Single lookup ECMP Group LT logical fields attribute values
     * from LRD and update in SW database.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_SINGLE),
                        &map));
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /*
     * Initialize Logical Table LTD field identifiers values for
     * single lookup ECMP Group LT logical fields.
     */
    bcmecmp_grp_ltd_fields_t_init(&lt_fids);
    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_HIER:
            lt_fids.ecmp_id_fid = ECMP_HIERARCHICALt_ECMP_IDf;
            lt_fids.lb_mode_fid = ECMP_HIERARCHICALt_LB_MODEf;
            lt_fids.nhop_sorted_fid = ECMP_HIERARCHICALt_NHOP_SORTEDf;
            lt_fids.max_paths_fid = ECMP_HIERARCHICALt_MAX_PATHSf;
            lt_fids.num_paths_fid = ECMP_HIERARCHICALt_NUM_PATHSf;
            lt_fids.nhop_id_fid = ECMP_HIERARCHICALt_NHOP_IDf;
            lt_fids.rh_size_fid = ECMP_HIERARCHICALt_RH_SIZEf;
            break;
        case BCMECMP_MODE_FAST:
            lt_fids.ecmp_id_fid = ECMP_FASTt_ECMP_IDf;
            lt_fids.lb_mode_fid = ECMP_FASTt_LB_MODEf;
            lt_fids.nhop_sorted_fid = ECMP_FASTt_NHOP_SORTEDf;
            lt_fids.max_paths_fid = ECMP_FASTt_MAX_PATHSf;
            lt_fids.num_paths_fid = ECMP_FASTt_NUM_PATHSf;
            lt_fids.nhop_id_fid = ECMP_FASTt_NHOP_IDf;
            lt_fids.rh_size_fid = ECMP_FASTt_RH_SIZEf;
            break;
        default:
            lt_fids.ecmp_id_fid = ECMPt_ECMP_IDf;
            lt_fids.lb_mode_fid = ECMPt_LB_MODEf;
            lt_fids.nhop_sorted_fid = ECMPt_NHOP_SORTEDf;
            lt_fids.max_paths_fid = ECMPt_MAX_PATHSf;
            lt_fids.num_paths_fid = ECMPt_NUM_PATHSf;
            lt_fids.nhop_id_fid = ECMPt_NHOP_IDf;
            lt_fids.rh_size_fid = ECMPt_RH_SIZEf;
    }

    /* Initialize Group tables fields LTD information. */
    BCMECMP_TBL_LTD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = (void *)&lt_fids;

    /* Get ECMP ID min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids.ecmp_id_fid]);
    BCMECMP_LT_MIN_ECMP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_ECMP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /* Get load balancing mode min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids.lb_mode_fid]);
    BCMECMP_LT_MIN_LB_MODE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u32;
    BCMECMP_LT_MAX_LB_MODE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u32;

    /*
     * Iterate over lb_mode enum symbols, get enum values and store in SW
     * database.
     */
    for (idx = 0; idx < fdata->num_sym; idx++) {

        /* Get LB_MODE enum value. */
        lb_val = (uint32_t)fdata->sym[idx].val;

        /*
         * Check and confirm LB_MODE enum name matches expected name for this
         * LB_MODE enum value. Return error and exit initialization if
         * invalid/unrecognized LB_MODE enum name is found.
         */
        if (!sal_strcmp(lb_name[lb_val], fdata->sym[idx].name)) {

            /* Initialize supported LB modes for single level ECMP LT. */
            BCMECMP_LT_LB_MODE
                (unit, group, BCMECMP_GRP_TYPE_SINGLE, lb_val) = lb_val;
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be in
     * powers-of-two for entries in ECMP, ECMP_FAST and ECMP_HIERARCHICAL LTs
     * for this device.
     */
    BCMECMP_LT_POW_TWO_PATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = FALSE;

    /*
     * Get MAX_PATHS logical table LRD field data pointer.
     * Initialize device max_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids.max_paths_fid]);
    BCMECMP_LT_MIN_PATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_PATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids.num_paths_fid]);
    BCMECMP_LT_MIN_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /*
     * Get NHOP_ID logical table LRD field data pointer.
     * Initialize device NHOP_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids.nhop_id_fid]);
    BCMECMP_LT_MIN_NHOP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_NHOP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /*
     * Get rh_size logical table LRD field data pointer.
     * Initialize device rh_size minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids.rh_size_fid]);
    BCMECMP_LT_MIN_RH_SIZE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u32;
    BCMECMP_LT_MAX_RH_SIZE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u32;

    /*
     * Initialize Group and Member tables fields DRD information.
     * Note:
     *     In Tomahawk device, Overlay and Underlay logical tables share
     *     the same physical table. Hence their DRD fields information are
     *     same.
     */
    /* Pre-IFP ECMP Tables. */
    BCMECMP_TBL_DRD_FIELDS
        (unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = (void *)&mdrd_fields;
    BCMECMP_TBL_DRD_FIELDS
        (unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = (void *)&gdrd_fields;
    /* Post-IFP ECMP Tables. */
    BCMECMP_TBL_DRD_FIELDS
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = (void *)&mdrd_fields;
    BCMECMP_TBL_DRD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = (void *)&gdrd_fields;


    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)) {
        /* Pre-IFP ECMP Tables for Overlay Groups. */
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_member, BCMECMP_GRP_TYPE_OVERLAY)
                                                    = (void *)&mdrd_fields;
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_group, BCMECMP_GRP_TYPE_OVERLAY)
                                                    = (void *)&gdrd_fields;
        /* Post-IFP ECMP Tables for Overlay Groups. */
        BCMECMP_TBL_DRD_FIELDS
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = (void *)&gdrd_fields;
        BCMECMP_TBL_DRD_FIELDS
            (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = (void *)&mdrd_fields;

        /* Pre-IFP ECMP Tables for Underlay Groups. */
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_member, BCMECMP_GRP_TYPE_UNDERLAY)
                                                    = (void *)&mdrd_fields;
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_group, BCMECMP_GRP_TYPE_UNDERLAY)
                                                    = (void *)&gdrd_fields;
        /* Post-IFP ECMP Tables for Underlay Groups. */
        BCMECMP_TBL_DRD_FIELDS
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = (void *)&gdrd_fields;
        BCMECMP_TBL_DRD_FIELDS
            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = (void *)&mdrd_fields;

        /*
         * Initialize Overlay ECMP logical table field attributes
         * structure.
         */
        bcmecmp_lt_field_attrs_t_init(&ogrp_attrs);
        BCMECMP_LT_FIELD_ATTRS_VOID_PTR
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = (void *)&ogrp_attrs;

        /* Check and return error if Field attributes pointer is NULL. */
        SHR_NULL_CHECK
            (BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
             SHR_E_INTERNAL);

        /*
         * Retrieve Overlay ECMP Group LT logical fields attribute
         * values from LRD and update in SW database.
         */
        SHR_IF_ERR_EXIT
            (bcmlrd_map_get(unit,
                            BCMECMP_TBL_LTD_SID
                                (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                            &map));
        SHR_NULL_CHECK(map, SHR_E_INTERNAL);

        /* Initialize Overlay ECMP Group LTD fields information. */
        olt_fids.ecmp_id_fid = ECMP_OVERLAYt_ECMP_IDf;
        olt_fids.lb_mode_fid = ECMP_OVERLAYt_LB_MODEf;
        olt_fids.nhop_sorted_fid = ECMP_OVERLAYt_NHOP_SORTEDf;
        olt_fids.max_paths_fid = ECMP_OVERLAYt_MAX_PATHSf;
        olt_fids.num_paths_fid = ECMP_OVERLAYt_NUM_PATHSf;
        olt_fids.ecmp_nhop_fid = ECMP_OVERLAYt_ECMP_NHOPf;
        olt_fids.uecmp_id_fid = ECMP_OVERLAYt_ECMP_UNDERLAY_IDf;
        olt_fids.nhop_id_fid = ECMP_OVERLAYt_NHOP_IDf;
        olt_fids.rh_size_fid = ECMP_OVERLAYt_RH_SIZEf;

        /* Initialize LTD fields pointer. */
        BCMECMP_TBL_LTD_FIELDS
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = (void *)&olt_fids;

        /* Get ECMP ID min and max values from LRD field data. */
        fdata = &(map->field_data->field[olt_fids.ecmp_id_fid]);
        BCMECMP_LT_MIN_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->max->u16;

        /*
        * Get load balancing mode min and max values from LRD field
        * data.
        */
        fdata = &(map->field_data->field[olt_fids.lb_mode_fid]);
        BCMECMP_LT_MIN_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->min->u32;
        BCMECMP_LT_MAX_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->max->u32;

        /*
        * Iterate over lb_mode enum symbols, get enum values and store in
        * SW database.
        */
        for (idx = 0; idx < fdata->num_sym; idx++) {
            /* Get LB_MODE enum value. */
            lb_val = (uint32_t)fdata->sym[idx].val;

            /*
             * Check and confirm LB_MODE enum name matches expected name
             * for this LB_MODE enum value. Return error and exit
             * initialization if invalid/unrecognized LB_MODE enum name is
             * found.
             */
            if (!sal_strcmp(lb_name[lb_val], fdata->sym[idx].name)) {
                /* Initialize supported LB modes for Overlay ECMP LT. */
                BCMECMP_LT_LB_MODE
                    (unit, group, BCMECMP_GRP_TYPE_OVERLAY, lb_val) = lb_val;
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }

        /*
         * It's not mandatory that MAX_PATHS and NUM_PATHS values must be in
         * powers-of-two for entries in ECMP_OVERLAY LT for this device.
         */
        BCMECMP_LT_POW_TWO_PATHS
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = FALSE;

        /*
         * Get MAX_PATHS logical table LRD field data pointer.
         * Initialize device max_paths minimum and maximum values.
         */
        fdata = &(map->field_data->field[olt_fids.max_paths_fid]);
        BCMECMP_LT_MIN_PATHS
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_PATHS
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->max->u16;

        /*
         * Get NUM_PATHS logical table LRD field data pointer.
         * Initialize device num_paths minimum and maximum values.
         */
        fdata = &(map->field_data->field[olt_fids.num_paths_fid]);
        BCMECMP_LT_MIN_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->max->u16;

        /*
         * Get ECMP_NHOP logical table LRD field data pointer.
         * Initialize device ECMP_NHOP default value.
         */
        fdata = &(map->field_data->field[olt_fids.ecmp_nhop_fid]);
        BCMECMP_LT_DEF_ECMP_NHOP
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->def->is_true;

        /*
         * Get ECMP_UNDERLAY_ID logical table LRD field data pointer.
         * Initialize device ECMP_UNDERLAY_ID minimum and maximum values.
         */
        fdata = &(map->field_data->field[olt_fids.uecmp_id_fid]);
        BCMECMP_LT_MIN_UECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_UECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->max->u16;

        /*
         * Get NHOP_ID logical table LRD field data pointer.
         * Initialize device NHOP_ID minimum and maximum values.
         */
        fdata = &(map->field_data->field[olt_fids.nhop_id_fid]);
        BCMECMP_LT_MIN_NHOP_ID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_NHOP_ID
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->max->u16;

        /*
         * Get rh_size logical table LRD field data pointer.
         * Initialize device rh_size minimum and maximum values.
         */
        fdata = &(map->field_data->field[olt_fids.rh_size_fid]);
        BCMECMP_LT_MIN_RH_SIZE
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = fdata->min->u32;
        BCMECMP_LT_MAX_RH_SIZE
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = (fdata->max->u32 - 1);

        /*
         * Initialize Underlay ECMP logical table field attributes
         * structure.
         */
        bcmecmp_lt_field_attrs_t_init(&ugrp_attrs);
        BCMECMP_LT_FIELD_ATTRS_VOID_PTR
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = (void *)&ugrp_attrs;

        /* Check and return error if Field attributes pointer is NULL. */
        SHR_NULL_CHECK
            (BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY), SHR_E_INTERNAL);

        /* Get LRD map information for the enabled ECMP logical table. */
        SHR_IF_ERR_EXIT
            (bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                            &map));
        SHR_NULL_CHECK(map, SHR_E_INTERNAL);

        /*
         * Initialize Underlay ECMP Group LTD fields info, the field ID
         * values are same as single level ECMP Group except that
         * RH_SIZE (last field in LT) is not supported for this LT.
         */
        ult_fids.ecmp_id_fid = ECMP_UNDERLAYt_ECMP_IDf;
        ult_fids.lb_mode_fid = ECMP_UNDERLAYt_LB_MODEf;
        ult_fids.nhop_sorted_fid = ECMP_UNDERLAYt_NHOP_SORTEDf;
        ult_fids.max_paths_fid = ECMP_UNDERLAYt_MAX_PATHSf;
        ult_fids.num_paths_fid = ECMP_UNDERLAYt_NUM_PATHSf;
        ult_fids.nhop_id_fid = ECMP_UNDERLAYt_NHOP_IDf;
        BCMECMP_TBL_LTD_FIELDS
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = (void *)&ult_fids;

        /* Get ECMP ID min and max values from LRD field data. */
        fdata = &(map->field_data->field[ult_fids.ecmp_id_fid]);
        BCMECMP_LT_MIN_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_ECMP_ID
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->max->u16;

        /*
         * Get load balancing mode min and max values from LRD field
         * data.
         */
        fdata = &(map->field_data->field[ult_fids.lb_mode_fid]);
        BCMECMP_LT_MIN_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->min->u32;
        BCMECMP_LT_MAX_LB_MODE
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->max->u32;

        /*
         * Iterate over lb_mode enum symbols, get enum values and store in
         * SW database.
         */
        for (idx = 0; idx < fdata->num_sym; idx++) {
            lb_val = (uint32_t)fdata->sym[idx].val;
            if (!sal_strcmp(lb_name[lb_val], fdata->sym[idx].name)) {
                /* Initialize supported LB modes for single level ECMP LT. */
                BCMECMP_LT_LB_MODE
                    (unit, group, BCMECMP_GRP_TYPE_UNDERLAY, lb_val) = lb_val;
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }

        /*
         * It's not mandatory that MAX_PATHS and NUM_PATHS values must be in
         * powers-of-two for entries in ECMP_OVERLAY LT for this device.
         */
        BCMECMP_LT_POW_TWO_PATHS
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = FALSE;

        /*
         * Get MAX_PATHS logical table LRD field data pointer.
         * Initialize device max_paths minimum and maximum values.
         */
        fdata = &(map->field_data->field[ult_fids.max_paths_fid]);
        BCMECMP_LT_MIN_PATHS
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_PATHS
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->max->u16;

        /*
         * Get NUM_PATHS logical table LRD field data pointer.
         * Initialize device num_paths minimum and maximum values.
         */
        fdata = &(map->field_data->field[ult_fids.num_paths_fid]);
        BCMECMP_LT_MIN_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_NPATHS
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->max->u16;

        /*
         * Get NHOP_ID logical table LRD field data pointer.
         * Initialize device NHOP_ID minimum and maximum values.
         */
        fdata = &(map->field_data->field[ult_fids.nhop_id_fid]);
        BCMECMP_LT_MIN_NHOP_ID
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->min->u16;
        BCMECMP_LT_MAX_NHOP_ID
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = fdata->max->u16;
    }

    exit:
        bcm56960_a0_ecmp_info_print(unit);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Tomahawk device specific ECMP Table fields information.
 *
 * Clear ECMP Group and Member tables fields information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_ecmp_fields_clear(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP resolution mode field identifier value. */
    BCMECMP_MODE_DRD_FID(unit, config) = BCM56960_A0_FIELD_COUNT;

    /* Clear single level ECMP Group and Member tables fields information. */
    BCMECMP_TBL_DRD_FIELDS(unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = NULL;
    BCMECMP_TBL_DRD_FIELDS(unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = NULL;

    BCMECMP_TBL_DRD_FIELDS(unit, group, BCMECMP_GRP_TYPE_SINGLE) = NULL;
    BCMECMP_TBL_DRD_FIELDS(unit, member, BCMECMP_GRP_TYPE_SINGLE) = NULL;

    BCMECMP_TBL_LTD_FIELDS(unit, group, BCMECMP_GRP_TYPE_SINGLE) = NULL;
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = NULL;

    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)) {
        /* Clear Overlay Group and Member tables Fields information. */
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_group, BCMECMP_GRP_TYPE_OVERLAY) = NULL;
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_member, BCMECMP_GRP_TYPE_OVERLAY) = NULL;
        BCMECMP_TBL_DRD_FIELDS(unit, group, BCMECMP_GRP_TYPE_OVERLAY) = NULL;
        BCMECMP_TBL_DRD_FIELDS(unit, member, BCMECMP_GRP_TYPE_OVERLAY) = NULL;
        BCMECMP_TBL_LTD_FIELDS(unit, group, BCMECMP_GRP_TYPE_OVERLAY) = NULL;
        BCMECMP_LT_FIELD_ATTRS_VOID_PTR
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = NULL;

        /* Clear Underlay Group and Member tables Fields information. */
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_group, BCMECMP_GRP_TYPE_UNDERLAY) = NULL;
        BCMECMP_TBL_DRD_FIELDS
            (unit, init_member, BCMECMP_GRP_TYPE_UNDERLAY) = NULL;
        BCMECMP_TBL_DRD_FIELDS(unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = NULL;
        BCMECMP_TBL_DRD_FIELDS(unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = NULL;
        BCMECMP_TBL_LTD_FIELDS(unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = NULL;
        BCMECMP_LT_FIELD_ATTRS_VOID_PTR
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Tomahawk device specfic ECMP tables range information.
 *
 * Initialize ECMP tables size, entry size and valid index range details.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid ECMP mode value.
 */
static int
bcm56960_a0_ecmp_hw_tbls_range_init(int unit)
{
    uint32_t wsize; /* Hardware table entry size in words. */
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */
    int grp_imin;   /* ECMP Group hardware table Min index value. */
    int grp_imax;   /* ECMP Group hardware table Max index value. */
    int tbl_sz;     /* Local variable to store calculated table size. */

    SHR_FUNC_ENTER(unit);

    /* Initialize device ECMP config register entry size in words from DRD. */
    BCMECMP_REG_ENT_SIZE(unit, config) = bcmdrd_pt_entry_wsize
                                            (unit,
                                             BCMECMP_REG_DRD_SID(unit, config));

    /* Get Pre-IFP Group table HW entry size (in words) from DRD. */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, init_group,
                                                      BCMECMP_GRP_TYPE_SINGLE));
    BCMECMP_TBL_ENT_SIZE(unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = wsize;
    BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;

    /* Get Pre-IFP Member table HW entry size (in words) from DRD. */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, init_member,
                                                      BCMECMP_GRP_TYPE_SINGLE));
    BCMECMP_TBL_ENT_SIZE(unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    /* Get Post-IFP Group table HW entry size (in words) from DRD. */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, group,
                                                      BCMECMP_GRP_TYPE_SINGLE));
    BCMECMP_TBL_ENT_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    /* Get Post-IFP Member table HW entry size (in words) from DRD. */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, member,
                                                      BCMECMP_GRP_TYPE_SINGLE));
    BCMECMP_TBL_ENT_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    /* Get Round-Robin Table HW entry size (in words) from DRD. */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, rrlb_cnt,
                                                      BCMECMP_GRP_TYPE_SINGLE));
    BCMECMP_TBL_ENT_SIZE(unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    memb_imin = bcmdrd_pt_index_min
                    (unit, BCMECMP_TBL_DRD_SID
                                (unit, member, BCMECMP_GRP_TYPE_SINGLE));
    memb_imax = bcmdrd_pt_index_max
                    (unit, BCMECMP_TBL_DRD_SID
                                (unit, member, BCMECMP_GRP_TYPE_SINGLE));

    /* Get ECMP Group HW table Min and Max index ranges from DRD. */
    grp_imin = bcmdrd_pt_index_min
                    (unit, BCMECMP_TBL_DRD_SID
                                (unit, group, BCMECMP_GRP_TYPE_SINGLE));
    grp_imax = bcmdrd_pt_index_max
                    (unit, BCMECMP_TBL_DRD_SID
                                (unit, group, BCMECMP_GRP_TYPE_SINGLE));

    /* Initialize table information based on the device ECMP mode. */
    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FAST:
            /*
             * Initialize Fast ECMP Member hardware table min and max
             * indices.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = (memb_imax / 2);

            /* Calcuate and initialize Fast ECMP Member table size. */
            tbl_sz = (BCMECMP_TBL_IDX_MAX(unit, member, BCMECMP_GRP_TYPE_SINGLE)
                        - BCMECMP_TBL_IDX_MIN
                                (unit, member, BCMECMP_GRP_TYPE_SINGLE)) + 1;
            BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE) = tbl_sz;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;

            /* Initialize Fast ECMP Group hardware table min and max indices. */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = (grp_imax / 2);

            /* Calculate and initialize Fast ECMP Group table size. */
            tbl_sz = (BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_SINGLE)
                        - BCMECMP_TBL_IDX_MIN
                                (unit, group, BCMECMP_GRP_TYPE_SINGLE)) + 1;
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE) = tbl_sz;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;
            break;

        case BCMECMP_MODE_FULL:
            /*
             * ECMP Member HW Table Min & Max indicies, Size and Min-index used
             * status initialization.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imax;
            BCMECMP_TBL_SIZE
                (unit, member, BCMECMP_GRP_TYPE_SINGLE)
                    = BCMECMP_TBL_IDX_MAX
                            (unit, member, BCMECMP_GRP_TYPE_SINGLE) + 1;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;

            /*
             * ECMP Group HW Table Min & Max indicies, Size and Min-index used
             * status initialization.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imax;
            BCMECMP_TBL_SIZE
                (unit, group, BCMECMP_GRP_TYPE_SINGLE)
                    = BCMECMP_TBL_IDX_MAX
                            (unit, group, BCMECMP_GRP_TYPE_SINGLE) + 1;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;
            break;

        case BCMECMP_MODE_HIER:
            /*
             * Initialize Overlay + Underlay Group and Member tables entry size
             * in words.
             * Note:
             *     On Tomamhawk device, Overlay and Underlay hardware table
             *     entry sizes are same as the single ECMP lookup Group
             *     and Member tables size as same physical tables are used for
             *     Overlay and Underlay tables.
             */
            BCMECMP_TBL_ENT_SIZE
                (unit, init_group, BCMECMP_GRP_TYPE_OVERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                        (unit, init_group, BCMECMP_GRP_TYPE_SINGLE);
            BCMECMP_TBL_ENT_SIZE
                (unit, init_member, BCMECMP_GRP_TYPE_OVERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                        (unit, init_group, BCMECMP_GRP_TYPE_SINGLE);
            BCMECMP_TBL_ENT_SIZE
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                            (unit, group, BCMECMP_GRP_TYPE_SINGLE);
            BCMECMP_TBL_ENT_SIZE
                (unit, member, BCMECMP_GRP_TYPE_OVERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                            (unit, member, BCMECMP_GRP_TYPE_SINGLE);

            BCMECMP_TBL_ENT_SIZE
                (unit, init_group, BCMECMP_GRP_TYPE_UNDERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                        (unit, init_group, BCMECMP_GRP_TYPE_SINGLE);
            BCMECMP_TBL_ENT_SIZE
                (unit, init_member, BCMECMP_GRP_TYPE_UNDERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                        (unit, init_group, BCMECMP_GRP_TYPE_SINGLE);

            BCMECMP_TBL_ENT_SIZE
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                            (unit, group, BCMECMP_GRP_TYPE_SINGLE);
            BCMECMP_TBL_ENT_SIZE
                (unit, member, BCMECMP_GRP_TYPE_UNDERLAY)
                    = BCMECMP_TBL_ENT_SIZE
                            (unit, member, BCMECMP_GRP_TYPE_SINGLE);

            /*
             * ECMP Member HW Table Min & Max indicies, Size and Min-index used
             * status initialization.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imax;
            BCMECMP_TBL_SIZE
                (unit, member, BCMECMP_GRP_TYPE_SINGLE)
                    = BCMECMP_TBL_IDX_MAX
                            (unit, member, BCMECMP_GRP_TYPE_SINGLE) + 1;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;

            /*
             * ECMP Group HW Table Min & Max indicies, Size and Min-index used
             * status initialization.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imax;
            BCMECMP_TBL_SIZE
                (unit, group, BCMECMP_GRP_TYPE_SINGLE)
                    = BCMECMP_TBL_IDX_MAX
                            (unit, group, BCMECMP_GRP_TYPE_SINGLE) + 1;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;

            /* Calculate Overlay ECMP Group table size. */
            tbl_sz = (BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_SINGLE)
                        - BCMECMP_TBL_IDX_MIN
                                (unit, group, BCMECMP_GRP_TYPE_SINGLE)) + 1;
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE) = tbl_sz;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;

            /*
             * Initialize Overlay ECMP Member table Min & Max indicies, table
             * size and used indices status.
             * Max index value is half of the hardware table size. Both Member
             * and Group tables are split into two to support Overlay and
             * Underlay ECMP lookups.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = memb_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = (memb_imax / 2);

            /* Calculate single level ECMP Member table size. */
            tbl_sz = (BCMECMP_TBL_IDX_MAX
                        (unit, member, BCMECMP_GRP_TYPE_OVERLAY)
                        - BCMECMP_TBL_IDX_MIN
                            (unit, member, BCMECMP_GRP_TYPE_OVERLAY)) + 1;
            BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_OVERLAY) = tbl_sz;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = memb_imin;

            /*
             * Overlay ECMP Group HW Table Min & Max indicies, Size and Used
             * indices status intialization.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = grp_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = (grp_imax / 2);

            /* Calculate Overlay ECMP Group table size. */
            tbl_sz = (BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_OVERLAY)
                        - BCMECMP_TBL_IDX_MIN
                            (unit, group, BCMECMP_GRP_TYPE_OVERLAY)) + 1;
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_OVERLAY) = tbl_sz;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = grp_imin;

            /*
             * Initialize Underlay ECMP Member table Min & Max indices, Size
             * and Used indices status.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_UNDERLAY)
                    = BCMECMP_TBL_IDX_MAX
                        (unit, member, BCMECMP_GRP_TYPE_OVERLAY) + 1;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = memb_imax;

            /* Calcuate Underlay ECMP Member table size. */
            tbl_sz = (BCMECMP_TBL_IDX_MAX
                        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY)
                        - BCMECMP_TBL_IDX_MIN
                            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY)) + 1;
            BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = tbl_sz;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = memb_imin;

            /*
             * Initialize Underlay ECMP Group HW Table Min & Max indicies, Size
             * and Used indices status intialization.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY)
                    = BCMECMP_TBL_IDX_MAX
                        (unit, group, BCMECMP_GRP_TYPE_OVERLAY) + 1;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = grp_imax;

            /* Calculate Underlay ECMP Group table size. */
            tbl_sz = (BCMECMP_TBL_IDX_MAX
                        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY)
                        - BCMECMP_TBL_IDX_MIN
                            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY)) + 1;
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = tbl_sz;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = grp_imin;
            break;

        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Tomahawk device specfic ECMP tables range information .
 *
 * Clear ECMP tables size, entry size and valid index range details.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_ecmp_hw_tbls_range_clear(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP config register entry size in words value. */
    BCMECMP_REG_ENT_SIZE(unit, config) = 0;

    /* Clear Max hardware tables entry size info. */
    BCMECMP_TBL_ENT_SIZE_MAX(unit) = 0;

    /* Clear Initial Group Table entry size in words from DRD. */
    BCMECMP_TBL_ENT_SIZE(unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_ENT_SIZE(unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_ENT_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_ENT_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE) = 0;
    BCMECMP_TBL_ENT_SIZE(unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = 0;

    /* Clear ECMP Member HW Table indicies information. */
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE) = 0;

    /* Clear ECMP Group HW Table indicies range information. */
    BCMECMP_TBL_IDX_MIN(unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
    BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;

    /* Clear Underlay ECMP tables DRD information. */
    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)) {
        /* Clear Overlay ECMP tables entry size value. */
        BCMECMP_TBL_ENT_SIZE(unit, init_group, BCMECMP_GRP_TYPE_OVERLAY) = 0;
        BCMECMP_TBL_ENT_SIZE(unit, init_member, BCMECMP_GRP_TYPE_OVERLAY) = 0;
        BCMECMP_TBL_ENT_SIZE(unit, group, BCMECMP_GRP_TYPE_OVERLAY) = 0;
        BCMECMP_TBL_ENT_SIZE(unit, member, BCMECMP_GRP_TYPE_OVERLAY) = 0;

        /* Clear Overlay ECMP Member table hardware data. */
        BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX_USED
            (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_OVERLAY) = 0;

        /* Clear Overlay ECMP Group table hardware data. */
        BCMECMP_TBL_IDX_MIN
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX_USED
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_OVERLAY) = 0;

        /* Clear Underlay ECMP tables entry size value. */
        BCMECMP_TBL_ENT_SIZE(unit, init_group, BCMECMP_GRP_TYPE_UNDERLAY) = 0;
        BCMECMP_TBL_ENT_SIZE(unit, init_member, BCMECMP_GRP_TYPE_UNDERLAY) = 0;
        BCMECMP_TBL_ENT_SIZE(unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = 0;
        BCMECMP_TBL_ENT_SIZE(unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = 0;

        /* Clear Underlay ECMP Member table hardware data. */
        BCMECMP_TBL_IDX_MIN
            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX
            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX_USED
            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = 0;

        /* Clear Underlay ECMP Group table hardware data. */
        BCMECMP_TBL_IDX_MIN
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_IDX_MAX_USED
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = BCMECMP_INVALID;
        BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = 0;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize LT ECMP mode to hardware ECMP mode encoding mapping array.
 *
 * Maps ECMP_CONFIG Logical Table ECMP MODE values to hardware ECMP resolution
 * mode encoding values.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_ecmp_mode_init(int unit)
{
    bcmecmp_mode_t mode; /* ECMP mode iterator variable. */
    uint32_t hw_ecmp_mode[BCMECMP_MODE_COUNT] = {1,  /* Full. */
                                                 3,  /* Hierarchical. */
                                                 0}; /* Fast. */

    SHR_FUNC_ENTER(unit);

    /*
     * Validate Admin configured device ECMP mode is valid for this chip
     * before proceeding with further initialization.
     */
    if (BCMECMP_MODE(unit) > BCMECMP_MODE_FAST) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Setup hardware ECMP mode values list for supported LT ECMP modes. */
    for (mode = 0; mode < BCMECMP_MODE_COUNT; mode++) {
        BCMECMP_MODE_HW_VAL(unit, config, mode) = hw_ecmp_mode[mode];
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Tomahawk device specific ECMP tables information.
 *
 * Initialize ECMP Group and Member tables information by retrieving details
 * from LRD and DRD.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid ECMP mode value, null pointer detected.
 */
static int
bcm56960_a0_ecmp_tables_init(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Initialize ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_sids_init(unit));

    /* Initialize ECMP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_hw_tbls_range_init(unit));

    /* Initialize ECMP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_fields_init(unit));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Clears Tomahawk device specific ECMP tables information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_ecmp_tables_clear(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Clear ECMP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_fields_clear(unit));

    /* Clear ECMP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_hw_tbls_range_clear(unit));

    /* Clear ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_sids_clear(unit));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP Group and Member table entries initialization function.
 *
 * Allocates memory to store ECMP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56960_a0_ecmp_entries_init(int unit, bool warm)
{
    bcmecmp_tbl_prop_t *tbl_ptr;
    size_t tbl_size;

    SHR_FUNC_ENTER(unit);

    /* Allocate memory for Member Table entries array. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_SINGLE);
    tbl_size = (size_t) BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE);
    BCMECMP_ALLOC
        (tbl_ptr->ent_arr,
         tbl_size * sizeof(bcmecmp_hw_entry_info_t),
         "bcmecmpBcm56960MembTbl");
    BCMECMP_ALLOC
        (tbl_ptr->ent_bk_arr,
         tbl_size * sizeof(bcmecmp_hw_entry_info_t),
         "bcmecmpBcm56960MembTblBk");

    /* Allocate memory for Group Table entries arrary. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_SINGLE);
    tbl_size = (size_t) BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE);
    BCMECMP_ALLOC
        (tbl_ptr->ent_arr,
         tbl_size * sizeof(bcmecmp_hw_entry_info_t),
         "bcmecmpBcm56960GrpTbl");
    BCMECMP_ALLOC
        (tbl_ptr->ent_bk_arr,
         tbl_size * sizeof(bcmecmp_hw_entry_info_t),
         "bcmecmpBcm56960GrpTblBk");

    /* Allocate memory to store ECMP Group information. */
    BCMECMP_ALLOC(BCMECMP_GRP_INFO_PTR(unit),
                  sizeof(bcmecmp_grp_info_t) * tbl_size,
                  "bcmecmpBcm56960GrpInfoArr");
    BCMECMP_ALLOC(BCMECMP_GRP_INFO_BK_PTR(unit),
                  sizeof(bcmecmp_grp_info_t) * tbl_size,
                  "bcmecmpBcm56960GrpInfoBkArr");

    /*
     * If device is operational in Hierarchical ECMP mode, allocate table
     * entries memory for Underlay tables.
     */
    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)) {
        /* Get single level ECMP Member table pointer. */
        tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_SINGLE);

        /*
         * Initialize Overlay Member table entries array pointer.
         * Note:
         *     As the physical table is shared for hierarchical mode, software
         *     Overlay Member table entry base pointer is same as single level
         *     ECMP Member table entry base pointer.
         */
        (BCMECMP_TBL_PTR
            (unit, member, BCMECMP_GRP_TYPE_OVERLAY))->ent_arr
                                                        = tbl_ptr->ent_arr;
        (BCMECMP_TBL_PTR
            (unit, member, BCMECMP_GRP_TYPE_OVERLAY))->ent_bk_arr
                                                        = tbl_ptr->ent_bk_arr;

        /*
         * Initialize Underlay Member table entries array pointer.
         * Note:
         *     As the physical table is shared for hierarchical mode, software
         *     Underlay Member table entry base pointer is same as single level
         *     ECMP Member table entry base pointer.
         */
        (BCMECMP_TBL_PTR
            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY))->ent_arr
                                                        = tbl_ptr->ent_arr;
        (BCMECMP_TBL_PTR
            (unit, member, BCMECMP_GRP_TYPE_UNDERLAY))->ent_bk_arr
                                                        = tbl_ptr->ent_bk_arr;

        /* Get single level ECMP Group table pointer. */
        tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_SINGLE);

        /*
         * Initialize Overlay Group table entries array base pointer.
         * Note:
         *     It is same as the single level ECMP Group table entry base
         *     pointer as hardware tables are shared.
         */
        (BCMECMP_TBL_PTR
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY))->ent_arr
                                                        = tbl_ptr->ent_arr;
        (BCMECMP_TBL_PTR
            (unit, group, BCMECMP_GRP_TYPE_OVERLAY))->ent_bk_arr
                                                        = tbl_ptr->ent_bk_arr;

        /*
         * Initialize Underlay Group table entries array pointer.
         * Note:
         *     It is same as the single level ECMP Group table entry base
         *     pointer as hardware tables are shared.
         */
        (BCMECMP_TBL_PTR
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY))->ent_arr
                                                        = tbl_ptr->ent_arr;
        (BCMECMP_TBL_PTR
            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY))->ent_bk_arr
                                                        = tbl_ptr->ent_bk_arr;
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP Group and Member table entries cleanup function.
 *
 * Cleanup memory allocated to store ECMP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_ecmp_entries_cleanup(int unit)
{
    bcmecmp_tbl_prop_t *tbl_ptr;

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for ECMP Group information. */
    if (BCMECMP_GRP_INFO_PTR(unit)) {
        BCMECMP_FREE(BCMECMP_GRP_INFO_PTR(unit));
    }
    if (BCMECMP_GRP_INFO_BK_PTR(unit)) {
        BCMECMP_FREE(BCMECMP_GRP_INFO_BK_PTR(unit));
    }

    /*
     * If device is operational in Hierarchical ECMP mode, cleanup memory
     * allocated for Underlay tables.
     */
    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)) {
        /* Clear Overlay Member table entries array pointer. */
        tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_OVERLAY);
        tbl_ptr->ent_arr = NULL;
        tbl_ptr->ent_bk_arr = NULL;

        /* Clear Overlay Group table entries array pointer. */
        tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
        tbl_ptr->ent_arr = NULL;
        tbl_ptr->ent_bk_arr = NULL;

        /* Clear Underlay Member table entries array pointer. */
        tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
        tbl_ptr->ent_arr = NULL;
        tbl_ptr->ent_bk_arr = NULL;

        /* Clear Underlay Group table entries array pointer. */
        tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
        tbl_ptr->ent_arr = NULL;
        tbl_ptr->ent_bk_arr = NULL;
    }

    /* Free Member table entries array memory. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_SINGLE);
    if (tbl_ptr->ent_arr) {
        BCMECMP_FREE(tbl_ptr->ent_arr);
    }
    if (tbl_ptr->ent_bk_arr) {
        BCMECMP_FREE(tbl_ptr->ent_bk_arr);
    }

    /* Free Group table entries array memory. */
    tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_SINGLE);
    if (tbl_ptr->ent_arr) {
        BCMECMP_FREE(tbl_ptr->ent_arr);
    }
    if (tbl_ptr->ent_bk_arr) {
        BCMECMP_FREE(tbl_ptr->ent_bk_arr);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Tomahawk device ECMP hardware table details initialization
 * function.
 *
 * Initializes ECMP hardware Tables, Fields and Entries information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_info_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize BCMCFG-ECMP mode to hardware ECMP mode mapping list. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_mode_init(unit));

    /* Initialize ECMP tables information for Tomahawk device. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_tables_init(unit));

    /*
     * Initialize ECMP Group and Member tables entries array for Single level,
     * Overlay and Underlay groups support.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_entries_init(unit, warm));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Tomahawk device ECMP hardware table details cleanup function.
 *
 * Cleanup Tomahawk ECMP hardware table information and free memory allocated
 * to store this information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_info_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Cleanup memory allocated for ECMP Group and Member table entries info. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_entries_cleanup(unit));

    /* Clear ECMP tables information stored in SW. */
    SHR_IF_ERR_VERBOSE_EXIT(bcm56960_a0_ecmp_tables_clear(unit));

    /* Initialize device ECMP mode to hardware reset default value. */
    BCMECMP_MODE(unit) = BCMECMP_MODE_FULL;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Tomahawk device ECMP resolution mode configuration function.
 *
 * Configures device ECMP resolution mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_mode_config(int unit)
{
    bcmltd_sid_t rsp_ltid = BCM56960_A0_ENUM_COUNT;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    uint32_t rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
    uint32_t pt_entry_buff = 0;
    uint32_t ecmp_mode = BCMECMP_MODE_COUNT;
    bcmecmp_mode_t sw_ecmp_mode = BCMECMP_MODE(unit);

    SHR_FUNC_ENTER(unit);

    /* Initialize physical table index and table instance information. */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = BCMECMP_ALL_PIPES_INST;

    /* Read ECMP_CONFIG register entry from hardware. */
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit,
                              BCMPTM_REQ_FLAGS_DONT_USE_CACHE,
                              BCMECMP_REG_DRD_SID(unit, config),
                              &pt_dyn_info,
                              ptr_pt_ovr_info,
                              BCMECMP_REG_ENT_SIZE(unit, config),
                              BCMECMP_REG_LTD_SID(unit, config),
                              &pt_entry_buff,
                              &rsp_ltid,
                              &rsp_flags));

    /* Get current device ECMP_MODE value. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             BCMECMP_REG_DRD_SID(unit, config),
                             &pt_entry_buff,
                             BCMECMP_MODE_DRD_FID(unit, config),
                             &ecmp_mode));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_MODE(CFG)=%d ECMP_MODE(Init-HW)=%d.\n"),
                            BCMECMP_MODE_HW_VAL(unit, config, sw_ecmp_mode),
                            ecmp_mode));

    if (ecmp_mode != BCMECMP_MODE_HW_VAL(unit, config, sw_ecmp_mode)) {
        /* Clear entry buffer. */
        pt_entry_buff = 0;

        /* Set ECMP_MODEf value in the PT entry buffer. */
        SHR_IF_ERR_EXIT(bcmdrd_pt_field_set(unit,
                            BCMECMP_REG_DRD_SID(unit, config),
                            &pt_entry_buff,
                            BCMECMP_MODE_DRD_FID(unit, config),
                            &BCMECMP_MODE_HW_VAL(unit, config, sw_ecmp_mode)));

        /* Write ECMP mode value to ECMP_CONFIG register. */
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_ireq_write(unit,
                                   BCMPTM_REQ_FLAGS_NO_FLAGS,
                                   BCMECMP_REG_DRD_SID(unit, config),
                                   (void *) &pt_dyn_info,
                                   ptr_pt_ovr_info,
                                   &pt_entry_buff,
                                   BCMECMP_REG_LTD_SID(unit, config),
                                   &rsp_ltid,
                                   &rsp_flags));

        /* Read ECMP_CONFIG register and confirm ECMP_MODE has been Set. */
        pt_entry_buff = 0;
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_ireq_read(unit,
                              BCMPTM_REQ_FLAGS_DONT_USE_CACHE,
                              BCMECMP_REG_DRD_SID(unit, config),
                              &pt_dyn_info,
                              ptr_pt_ovr_info,
                              BCMECMP_REG_ENT_SIZE(unit, config),
                              BCMECMP_REG_LTD_SID(unit, config),
                              &pt_entry_buff,
                              &rsp_ltid,
                              &rsp_flags));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_get(unit,
                                 BCMECMP_REG_DRD_SID(unit, config),
                                 &pt_entry_buff,
                                 BCMECMP_MODE_DRD_FID(unit, config),
                                 &ecmp_mode));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_MODE(Cur-HW)=0x%x.\n"), ecmp_mode));
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Insert ECMP Group logical table entry into hardware tables.
 *
 * Installs ECMP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_grp_ins(int unit,
                         void *info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_ins.\n")));

    if (bcmecmp_use_dma) {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_grp_dma_ins(unit, info));
    } else {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_grp_pio_ins(unit, info));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete ECMP Group logical table entry from hardware tables.
 *
 * Deletes ECMP Group Logical Table entry from hardware Group and Member
 * tables. Entries are deleted from Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_grp_del(int unit,
                         void *info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_del.\n")));

    if (bcmecmp_use_dma) {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_grp_dma_del(unit, info));
    } else {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_grp_pio_del(unit, info));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Get ECMP Group logical table entry from hardware tables.
 *
 * Construct ECMP Group Logical Table entry by retrieving hardware table entries
 * from ECMP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_grp_get(int unit,
                         void *info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_get.\n")));

    if (bcmecmp_use_dma) {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_grp_dma_get(unit, info));
    } else {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_grp_pio_get(unit, info));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Insert a RH-ECMP group logical table entry into hardware tables.
 *
 * Installs RH-ECMP group logical table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_rh_grp_ins(int unit,
                            void *info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_ins.\n")));

    if (bcmecmp_use_dma) {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_rh_grp_dma_ins(unit, info));
    } else {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_rh_grp_pio_ins(unit, info));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a RH-ECMP group logical table entry from hardware tables.
 *
 * Deletes a RH-ECMP group logical table entry from hardware Group and Member
 * tables. Entries are deleted from Pre-IFP and Post-IFP ECMP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to ECMP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56960_a0_ecmp_rh_grp_del(int unit,
                            void *info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_grp_del.\n")));
    if (bcmecmp_use_dma) {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_rh_grp_dma_del(unit, info));
    } else {
        SHR_RETURN_VAL_EXIT
            (bcm56960_a0_ecmp_rh_grp_pio_del(unit, info));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Attach device ECMP driver functions.
 *
 * Attach device ECMP driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56960_a0_ecmp_attach(int unit)
{
    bcmecmp_drv_t *ecmp_drv = bcmecmp_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_attach.\n")));

    SHR_NULL_CHECK(ecmp_drv, SHR_E_FAIL);

    /* Initialize device ECMP feature function pointers. */
    BCMECMP_FNCALL_PTR(unit) = ecmp_drv;

    /* Setup device ECMP component initialization function. */
    BCMECMP_FNCALL(unit, info_init) = bcm56960_a0_ecmp_info_init;

    /* Setup device ECMP component clean-up function. */
    BCMECMP_FNCALL
        (unit, info_cleanup) = bcm56960_a0_ecmp_info_cleanup;

    /* Setup device ECMP resoultion mode hardware config function. */
    BCMECMP_FNCALL(unit, mode_config) = bcm56960_a0_ecmp_mode_config;

    /* Initialize device ECMP group management function pointers. */
    BCMECMP_FNCALL(unit, grp_ins) = bcm56960_a0_ecmp_grp_ins;
    BCMECMP_FNCALL(unit, grp_del) = bcm56960_a0_ecmp_grp_del;
    BCMECMP_FNCALL(unit, grp_get) = bcm56960_a0_ecmp_grp_get;
    BCMECMP_FNCALL(unit, rh_grp_ins) = bcm56960_a0_ecmp_rh_grp_ins;
    BCMECMP_FNCALL(unit, rh_grp_del) = bcm56960_a0_ecmp_rh_grp_del;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Deatch device ECMP driver functions.
 *
 * Detach device ECMP driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56960_a0_ecmp_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56960_a0_ecmp_detach.\n")));

    if (BCMECMP_FNCALL_PTR(unit)) {
        /* Clear ECMP group management function pointers. */
        BCMECMP_FNCALL(unit, grp_get) = NULL;
        BCMECMP_FNCALL(unit, grp_del) = NULL;
        BCMECMP_FNCALL(unit, grp_ins) = NULL;

        /* Clear device ECMP feature function pointers. */
        BCMECMP_FNCALL(unit, mode_config) = NULL;
        BCMECMP_FNCALL(unit, info_cleanup) = NULL;
        BCMECMP_FNCALL(unit, info_init) = NULL;
        BCMECMP_FNCALL_PTR(unit) = NULL;
    }

    SHR_FUNC_EXIT();
}
