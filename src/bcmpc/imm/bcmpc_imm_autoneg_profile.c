/*! \file bcmpc_imm_autoneg_profile.c
 *
 * PC_AUTONEG_PROFILEt IMM handler.
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


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_AUTONEG_PROFILEt

/*! The data structure for PC_AUTONEG_PROFILEt entry. */
typedef struct pc_autoneg_prof_entry_s {
    int key;
    bcmpc_autoneg_prof_t data;
} pc_autoneg_prof_entry_t;

/*!
 * The data structure is used for mapping PC_AUTONEG_PROFILEt field IDs to
 * PC-specific enum values, e.g. \ref bcmpc_an_protocol_t and
 * \ref bcmpc_an_adv_ability_t.
 */
typedef struct pc_autoneg_ability_map_s {

    /*! LT field ID. */
    uint32_t fid;

    /*! PC enum value. */
    int pc_ability;

} pc_autoneg_ability_map_t;

/*! Autoneg protocol fields to \ref bcmpc_an_protocol_t value. */
static pc_autoneg_ability_map_t an_prot_map[] = {
    { PC_AUTONEG_PROFILEt_AN_PARALLEL_DETECTf, BCMPC_AN_PROT_PARALLEL_DETECT },
    { PC_AUTONEG_PROFILEt_AN_CL28f, BCMPC_AN_PROT_CL28 },
    { PC_AUTONEG_PROFILEt_AN_CL37f, BCMPC_AN_PROT_CL37 },
    { PC_AUTONEG_PROFILEt_AN_CL73f, BCMPC_AN_PROT_CL73 },
    { PC_AUTONEG_PROFILEt_AN_SGMIIf, BCMPC_AN_PROT_SGMII },
    { PC_AUTONEG_PROFILEt_AN_BAM37f, BCMPC_AN_PROT_BAM37 },
    { PC_AUTONEG_PROFILEt_AN_BAM73f, BCMPC_AN_PROT_BAM73 },
};

/*! Advertised ability fields to \ref bcmpc_an_adv_ability_t value. */
static pc_autoneg_ability_map_t an_advert_map[] = {
    { PC_AUTONEG_PROFILEt_ADV_10M_Tf, BCMPC_AN_ADV_10M_T },
    { PC_AUTONEG_PROFILEt_ADV_10M_T_HDXf, BCMPC_AN_ADV_10M_T_HDX },
    { PC_AUTONEG_PROFILEt_ADV_100M_Tf, BCMPC_AN_ADV_100M_T },
    { PC_AUTONEG_PROFILEt_ADV_100M_T_HDXf, BCMPC_AN_ADV_100M_T_HDX },
    { PC_AUTONEG_PROFILEt_ADV_100M_FXf, BCMPC_AN_ADV_100M_FX },
    { PC_AUTONEG_PROFILEt_ADV_1000M_Tf, BCMPC_AN_ADV_1000M_T },
    { PC_AUTONEG_PROFILEt_ADV_1000M_Xf, BCMPC_AN_ADV_1000M_X },
    { PC_AUTONEG_PROFILEt_ADV_2P5Gf, BCMPC_AN_ADV_2P5G },
    { PC_AUTONEG_PROFILEt_ADV_10Gf, BCMPC_AN_ADV_10G },
    { PC_AUTONEG_PROFILEt_ADV_10G_CXf, BCMPC_AN_ADV_10G_CX },
    { PC_AUTONEG_PROFILEt_ADV_10G_CRf, BCMPC_AN_ADV_10G_CR },
    { PC_AUTONEG_PROFILEt_ADV_10G_KRf, BCMPC_AN_ADV_10G_KR },
    { PC_AUTONEG_PROFILEt_ADV_10G_KXf, BCMPC_AN_ADV_10G_KX },
    { PC_AUTONEG_PROFILEt_ADV_12Gf, BCMPC_AN_ADV_12G },
    { PC_AUTONEG_PROFILEt_ADV_13Gf, BCMPC_AN_ADV_13G },
    { PC_AUTONEG_PROFILEt_ADV_20Gf, BCMPC_AN_ADV_20G },
    { PC_AUTONEG_PROFILEt_ADV_20G_CXf, BCMPC_AN_ADV_20G_CX },
    { PC_AUTONEG_PROFILEt_ADV_20G_CRf, BCMPC_AN_ADV_20G_CR },
    { PC_AUTONEG_PROFILEt_ADV_20G_KRf, BCMPC_AN_ADV_20G_KR },
    { PC_AUTONEG_PROFILEt_ADV_21Gf, BCMPC_AN_ADV_21G },
    { PC_AUTONEG_PROFILEt_ADV_40Gf, BCMPC_AN_ADV_40G },
    { PC_AUTONEG_PROFILEt_ADV_40G_CRf, BCMPC_AN_ADV_40G_CR },
    { PC_AUTONEG_PROFILEt_ADV_40G_KRf, BCMPC_AN_ADV_40G_KR },
    { PC_AUTONEG_PROFILEt_ADV_42Gf, BCMPC_AN_ADV_42G },
    { PC_AUTONEG_PROFILEt_ADV_100Gf, BCMPC_AN_ADV_100G },
    { PC_AUTONEG_PROFILEt_ADV_100G_CRf, BCMPC_AN_ADV_100G_CR },
    { PC_AUTONEG_PROFILEt_ADV_120Gf, BCMPC_AN_ADV_120G },
    { PC_AUTONEG_PROFILEt_ADV_127Gf, BCMPC_AN_ADV_127G },
    { PC_AUTONEG_PROFILEt_ADV_PAUSE_TXf, BCMPC_AN_ADV_PAUSE_TX },
    { PC_AUTONEG_PROFILEt_ADV_PAUSE_RXf, BCMPC_AN_ADV_PAUSE_RX },
    { PC_AUTONEG_PROFILEt_ADV_EEE_100Mf, BCMPC_AN_ADV_EEE_100M },
    { PC_AUTONEG_PROFILEt_ADV_EEE_1Gf, BCMPC_AN_ADV_EEE_1G },
};


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_autoneg_prof_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_autoneg_prof_entry_t_init(pc_autoneg_prof_entry_t *entry)
{
    entry->key = BCMPC_INVALID_PROF_ID;
    bcmpc_autoneg_prof_t_init(&entry->data);
}

 /*!
  * \brief Set the protocol field value to bcmpc_autoneg_prof_t.
  *
  * \param [in] prof Autoneg profile.
  * \param [in] fid Field ID.
  * \param [in] fval Field value.
  *
  * \retval SHR_E_NONE No errors.
  * \retval SHR_E_FAIL Failed to set fval to bcmpc_autoneg_prof_t.
  */
static int
pc_autoneg_prot_set(bcmpc_autoneg_prof_t *prof, uint32_t fid, uint64_t fval)
{
    int idx;

    for (idx = 0; idx < COUNTOF(an_prot_map); idx++) {
        if (fid != an_prot_map[idx].fid) {
            continue;
        }

        if (fval) {
            BCMPC_AN_PROT_SET(prof, an_prot_map[idx].pc_ability);
        } else {
            BCMPC_AN_PROT_CLR(prof, an_prot_map[idx].pc_ability);
        }
        break;
    }

    return (idx == COUNTOF(an_prot_map)) ? SHR_E_FAIL : SHR_E_NONE;
}

 /*!
  * \brief Set the advertised ability field value to bcmpc_autoneg_prof_t.
  *
  * \param [in] prof Autoneg profile.
  * \param [in] fid Field ID.
  * \param [in] fval Field value.
  *
  * \retval SHR_E_NONE No errors.
  * \retval SHR_E_FAIL Failed to set fval to bcmpc_autoneg_prof_t.
  */
static int
pc_autoneg_adv_set(bcmpc_autoneg_prof_t *prof, uint32_t fid, uint64_t fval)
{
    int idx;

    for (idx = 0; idx < COUNTOF(an_advert_map); idx++) {
        if (fid != an_advert_map[idx].fid) {
            continue;
        }

        if (fval) {
            BCMPC_AN_ADV_SET(prof, an_advert_map[idx].pc_ability);
        } else {
            BCMPC_AN_ADV_CLR(prof, an_advert_map[idx].pc_ability);
        }
        break;
    }

    return (idx == COUNTOF(an_advert_map)) ? SHR_E_FAIL : SHR_E_NONE;
}

/*!
 * \brief Fill PC structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_autoneg_prof_entry_set(int unit, uint32_t fid, uint64_t fval,
                          pc_autoneg_prof_entry_t *entry)
{
    bcmpc_autoneg_prof_t *prof = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_AUTONEG_PROFILEt_PC_AUTONEG_PROFILE_IDf:
            entry->key = fval;
            break;
        case PC_AUTONEG_PROFILEt_AN_PARALLEL_DETECTf:
        case PC_AUTONEG_PROFILEt_AN_CL28f:
        case PC_AUTONEG_PROFILEt_AN_CL37f:
        case PC_AUTONEG_PROFILEt_AN_CL73f:
        case PC_AUTONEG_PROFILEt_AN_SGMIIf:
        case PC_AUTONEG_PROFILEt_AN_BAM37f:
        case PC_AUTONEG_PROFILEt_AN_BAM73f:
            SHR_IF_ERR_EXIT
                (pc_autoneg_prot_set(prof, fid, fval));
            break;
        case PC_AUTONEG_PROFILEt_ADV_10M_Tf:
        case PC_AUTONEG_PROFILEt_ADV_10M_T_HDXf:
        case PC_AUTONEG_PROFILEt_ADV_100M_Tf:
        case PC_AUTONEG_PROFILEt_ADV_100M_T_HDXf:
        case PC_AUTONEG_PROFILEt_ADV_100M_FXf:
        case PC_AUTONEG_PROFILEt_ADV_1000M_Tf:
        case PC_AUTONEG_PROFILEt_ADV_1000M_Xf:
        case PC_AUTONEG_PROFILEt_ADV_2P5Gf:
        case PC_AUTONEG_PROFILEt_ADV_10Gf:
        case PC_AUTONEG_PROFILEt_ADV_10G_CXf:
        case PC_AUTONEG_PROFILEt_ADV_10G_CRf:
        case PC_AUTONEG_PROFILEt_ADV_10G_KRf:
        case PC_AUTONEG_PROFILEt_ADV_10G_KXf:
        case PC_AUTONEG_PROFILEt_ADV_12Gf:
        case PC_AUTONEG_PROFILEt_ADV_13Gf:
        case PC_AUTONEG_PROFILEt_ADV_20Gf:
        case PC_AUTONEG_PROFILEt_ADV_20G_CXf:
        case PC_AUTONEG_PROFILEt_ADV_20G_CRf:
        case PC_AUTONEG_PROFILEt_ADV_20G_KRf:
        case PC_AUTONEG_PROFILEt_ADV_21Gf:
        case PC_AUTONEG_PROFILEt_ADV_40Gf:
        case PC_AUTONEG_PROFILEt_ADV_40G_CRf:
        case PC_AUTONEG_PROFILEt_ADV_40G_KRf:
        case PC_AUTONEG_PROFILEt_ADV_42Gf:
        case PC_AUTONEG_PROFILEt_ADV_100Gf:
        case PC_AUTONEG_PROFILEt_ADV_100G_CRf:
        case PC_AUTONEG_PROFILEt_ADV_120Gf:
        case PC_AUTONEG_PROFILEt_ADV_127Gf:
        case PC_AUTONEG_PROFILEt_ADV_PAUSE_TXf:
        case PC_AUTONEG_PROFILEt_ADV_PAUSE_RXf:
        case PC_AUTONEG_PROFILEt_ADV_EEE_100Mf:
        case PC_AUTONEG_PROFILEt_ADV_EEE_1Gf:
            SHR_IF_ERR_EXIT
                (pc_autoneg_adv_set(prof, fid, fval));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to pc_autoneg_prof_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_autoneg_prof_flds_parse(int unit,
                              const bcmltd_field_t *key_flds,
                              const bcmltd_field_t *data_flds,
                              pc_autoneg_prof_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_autoneg_prof_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_autoneg_prof_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_autoneg_profile_stage(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key_flds,
                         const bcmltd_field_t *data_flds,
                         void *context,
                         bcmltd_fields_t *output_fields)
{
    pc_autoneg_prof_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_autoneg_prof_entry_t_init(&entry);

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_autoneg_prof_flds_parse(unit, key_flds, data_flds,
                                               &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_autoneg_prof_internal_update(unit, entry.key,
                                                    &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_autoneg_prof_flds_parse(unit, key_flds, NULL, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_db_imm_entry_lookup(unit,
                                           IMM_SID,
                                           P(&entry.key), P(&entry.data)));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_autoneg_prof_flds_parse(unit, NULL, data_flds, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_autoneg_prof_internal_update(unit, entry.key,
                                                    &entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to an integer profile id \c key and a
 * bcmpc_autoneg_prof_t \c data.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_autoneg_profile_entry_parse(int unit, const bcmltd_fields_t *flds,
                               size_t key_size, void *key,
                               size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    pc_autoneg_prof_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_autoneg_prof_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_autoneg_prof_entry_set(unit, fid, fval, &entry));
    }

    if (key) {
        sal_memcpy(key, &entry.key, key_size);
    }

    if (data) {
        sal_memcpy(data, &entry.data, data_size);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert an integer profile id \c key and a bcmpc_autoneg_prof_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_autoneg_profile_entry_fill(int unit,
                              size_t key_size, void *key,
                              size_t data_size, void *data,
                              bcmltd_fields_t *flds)
{
    size_t idx, fcnt = 0;
    uint32_t *prof_id = key;
    bcmpc_autoneg_prof_t *prof = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*prof_id)) ||
        (data && data_size != sizeof(*prof))) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_PC_AUTONEG_PROFILE_IDf, 0,
             *prof_id, fcnt);
    }

    if (data) {
        for (idx = 0; idx < COUNTOF(an_prot_map); idx++) {
            BCMPC_LT_FIELD_VAL_ADD
                (flds, an_prot_map[idx].fid, 0,
                 BCMPC_AN_PROT_GET(prof, an_prot_map[idx].pc_ability), fcnt);
        }
        for (idx = 0; idx < COUNTOF(an_advert_map); idx++) {
            BCMPC_LT_FIELD_VAL_ADD
                (flds, an_advert_map[idx].fid, 0,
                 BCMPC_AN_ADV_GET(prof, an_advert_map[idx].pc_ability), fcnt);
        }
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_autoneg_profile_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_autoneg_profile_entry_parse;
    db_hdl.entry_fill = pc_autoneg_profile_entry_fill;

    event_hdl.validate = NULL;
    event_hdl.stage = pc_autoneg_profile_stage;
    event_hdl.commit = NULL;
    event_hdl.abort = NULL;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}
