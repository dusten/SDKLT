/*! \file bcmecn_ip_protocol_responsive_status.c
 *
 *  IP protocol responsive status handler.
 *
 *  This file contains function definition for IP protocol
 *  responsive status.
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
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmecn/bcmecn_ecn_protocol_utils.h>
#include <bcmdrd_config.h>
#include <shr/shr_ha.h>

#include <bcmecn/bcmecn_ip_protocol_responsive_status.h>

#define BSL_LOG_MODULE BSL_LS_BCMECN_ECN_PROTOCOL

/*******************************************************************************
 * Local definitions
 */
bcmecn_sw_state_t bcmecn_sw_state[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief bcmecn_ip_protocol_responsive_status_validate
 *
 * The method validates the incoming params for setting
 * responsive nature of a particular IP protocol.
 *
 * \param [in] unit    Unit number.
 * \param [in] opcode  Instrcution to be carried.
 * \param [in] in      Pointer to data containing configuration
 * \param [in] arg     Argumnets passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */

int
bcmecn_ip_protocol_responsive_status_validate(int unit,
                       bcmlt_opcode_t opcode,
                       const bcmltd_fields_t *in,
                       const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);


    /* There should be at least one input param. */
    if (in->count < 1) {
        if (opcode != BCMLT_OPCODE_TRAVERSE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "The minimum argumnet count should be 1.\n")));
        }

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* The input param should have some data. */
    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The input buffers can not be NULL.\n")));
        return SHR_E_PARAM;
    }

    /* if count is 1 and input is not key then return error. */
    if ((in->count == 1) && (in->field[0]->id != ECN_PROTOCOLt_IP_PROTOf)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Table key is not passed.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief bcmecn_ip_protocol_responsive_status_insert
 *
 * The method inserts the configuration for setting
 * responsive nature of a particular IP protocol.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [in] in        Pointer to data containing configuration
 * \param [in] arg       Argumnets passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_insert(int unit,
                     uint32_t trans_id,
                     const bcmltd_fields_t *in,
                     const bcmltd_generic_arg_t *arg)
{
    uint32_t ip_proto = 0;
    uint32_t responsive = 0;
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    /* Input Data validation. */
    if (in->count < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The minimum argumnet count should be 1.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Input Data validation. */
    if (in->count > 2) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The max arg count cannot be more than 2.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Check for first input param */
    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The input buffers can not be NULL.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* if count is 1 and input is only data then return error. */
    if ((in->count == 1) && (in->field[i]->id != ECN_PROTOCOLt_IP_PROTOf)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "No Key is present. Only Value is present.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Input fields validation */
    if ((in->count == 2) && (in->field[1] != NULL)) {
        /* the value of field can not be more than 1. */
        if ((in->field[1]->id == ECN_PROTOCOLt_RESPONSIVEf) &&
             (in->field[1]->data > 1)) {

            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "The Max value for responsive field is 1.\n")));

            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    for (i=0; i < in->count; i++) {

        switch(in->field[i]->id) {

            case ECN_PROTOCOLt_IP_PROTOf:
                /* Get the protocol field */
                ip_proto = (uint32_t) in->field[0]->data;
                break;

            case ECN_PROTOCOLt_RESPONSIVEf:
                /* Get the responsive status */
                responsive = (uint32_t) in->field[1]->data;
                break;

            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                break;
        }
    }

    if (BCMECN_PROTOCOL_IN_USE_GET(unit, ip_proto)) {
       SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Setting responsive state %d for protocol %d\n"),
        ip_proto, responsive));

    SHR_IF_ERR_EXIT(
        bcmecn_ecn_protocol_pt_write(unit, trans_id, ip_proto, responsive));

    BCMECN_PROTOCOL_IN_USE_SET(unit, ip_proto);

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief bcmecn_ip_protocol_responsive_status_lookup
 *
 * The method looks the incoming data and finds
 * the matching entry and puts the associated data
 * with that entry into the outgoing buffer.
 * The user sends the ip protocol and this methos
 * gets the responsive status of that protocol and
 * puts into outgoing buffer.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [in] in        Pointer to data containing configuration
 * \param [out] out      Pointer to outgoing buffer.
 * \param [in] arg       Argumnets passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_lookup(int unit,
                     uint32_t trans_id,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    uint32_t ip_proto = 0;
    uint32_t responsive;
    uint32_t i = 0;
    uint32_t ip_proto_preset = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);


   if (in->count < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The incoming object count is not correct. \n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The input buffers can not be NULL.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    for (i=0; i < in->count; i++) {

        switch(in->field[i]->id) {

            case ECN_PROTOCOLt_IP_PROTOf:
                /* Get the protocol field */
                ip_proto = (uint32_t) in->field[0]->data;
                ip_proto_preset = 1;
                break;

            default:
                /* Do not do anything if any other data is sent. */
                break;
        }
    }

    if (!BCMECN_PROTOCOL_IN_USE_GET(unit, ip_proto)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check if key is missing in inout */
    if (!ip_proto_preset) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " Key is missing in lookup.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Read hardware table entry to get lookup result. */
    SHR_IF_ERR_EXIT(
        bcmecn_ecn_protocol_pt_read(unit, trans_id, ip_proto, &responsive));

    /* Feed the inforamtion into output fields. */
    out->field[0]->id = ECN_PROTOCOLt_RESPONSIVEf;
    out->field[0]->data = (uint64_t) responsive;
    out->count = 1;


    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief bcmecn_ip_protocol_responsive_status_delete
 *
 * The method looks the incoming data and finds
 * the index. it deletes the entry for that index.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [in] in        Pointer to data containing configuration
 * \param [in] arg       Argumnets passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_delete(int unit,
                     uint32_t trans_id,
                     const bcmltd_fields_t *in,
                     const bcmltd_generic_arg_t *arg)
{
    uint32_t ip_proto = 0;
    uint32_t i = 0;
    uint32_t ip_proto_preset = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);


   if (in->count < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The incoming object count is not correct. \n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The input buffers can not be NULL.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    for (i=0; i < in->count; i++) {

        switch(in->field[i]->id) {

            case ECN_PROTOCOLt_IP_PROTOf:
                /* Get the protocol field */
                ip_proto = (uint32_t) in->field[0]->data;
                ip_proto_preset = 1;
                break;

            default:
                /* Do not do anything if any other data is sent. */
                break;
        }
    }

    /* Check if key is missing in inout */
    if (!ip_proto_preset) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " Key is missing in lookup.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (!BCMECN_PROTOCOL_IN_USE_GET(unit, ip_proto)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(
        bcmecn_ecn_protocol_pt_write(unit, trans_id, ip_proto, 0));

    BCMECN_PROTOCOL_IN_USE_CLR(unit, ip_proto);

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief bcmecn_ip_protocol_responsive_status_update
 *
 * The method updates the entry which was inserted
 * by user before.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [in] in        Pointer to data containing configuration
 * \param [in] arg       Argumnets passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_update(int unit,
                     uint32_t trans_id,
                     const bcmltd_fields_t *in,
                     const bcmltd_generic_arg_t *arg)
{
    uint32_t ip_proto = 0;
    uint32_t responsive = 0;
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    /* Input Data validation. */
    if (in->count < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The minimum argumnet count should be 1.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Input Data validation. */
    if (in->count > 2) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The max arg count cannot be more than 2.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Check for first input param */
    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The input buffers can not be NULL.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* First check if entry is created. */
    if (in->field[0]->id == ECN_PROTOCOLt_IP_PROTOf) {
        if (!BCMECN_PROTOCOL_IN_USE_GET(unit, in->field[0]->data)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* if count is 1 and input is only key then nothing to do */
    if ((in->count == 1) && (in->field[0]->id == ECN_PROTOCOLt_IP_PROTOf)) {

        LOG_DEBUG(BSL_LOG_MODULE,
            (BSL_META_U(unit, " Only key is sent so nothing much to do.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

   /* if count is 1 and input is only data then return error. */
    if ((in->count == 1) && (in->field[i]->id != ECN_PROTOCOLt_IP_PROTOf)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "No Key is present. Only Value is present.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Input fields validation */
    if ((in->count == 2) && (in->field[1] != NULL)) {
        /* the value of field can not be more than 1. */
        if ((in->field[1]->id == ECN_PROTOCOLt_RESPONSIVEf) &&
             (in->field[1]->data > 1)) {

            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "The Max value for responsive field is 1.\n")));

            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    for (i=0; i < in->count; i++) {

        switch(in->field[i]->id) {

            case ECN_PROTOCOLt_IP_PROTOf:
                /* Get the protocol field */
                ip_proto = (uint32_t) in->field[0]->data;
                break;

            case ECN_PROTOCOLt_RESPONSIVEf:
                /* Get the responsive status */
                responsive = (uint32_t) in->field[1]->data;
                break;

            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                break;
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Setting responsive state %d for protocol %d\n"),
        ip_proto, responsive));

    SHR_IF_ERR_EXIT(
        bcmecn_ecn_protocol_pt_write(unit, trans_id, ip_proto, responsive));

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmecn_ip_protocol_responsive_status_first
 *
 * This method finds the first entry in the LT and puts
 * the associated data with that entry into the outgoing buffer.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [out] out      Pointer to outgoing buffer.
 * \param [in] arg       Argumnets passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_first (int unit,
                                            uint32_t trans_id,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg)
{
    uint32_t    min = 0, max = 0;
    uint32_t    idx = 0, ip_proto = 0;
    uint32_t    responsive = 0, field_count = 0;
    bool        found = false;
    bcmlrd_field_def_t     field_def;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    ECN_PROTOCOLt,
                                    ECN_PROTOCOLt_IP_PROTOf,
                                    &field_def));
    min = BCMECN_FIELD_MIN(field_def.dtag);
    max = BCMECN_FIELD_MAX(field_def.dtag);

    for (idx = min; idx <= max; idx++) {
        if (BCMECN_PROTOCOL_IN_USE_GET(unit, idx)) {
            found = true;
            ip_proto = idx;
            break;
        }
    }

    if (!found) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Read hardware table entry to get lookup result. */
    SHR_IF_ERR_EXIT(
        bcmecn_ecn_protocol_pt_read(unit, trans_id, ip_proto, &responsive));

    field_count = out->count;
    /* Feed the information into output fields. */
    for (idx = 0; idx < field_count; idx++) {
        switch (idx) {
        case ECN_PROTOCOLt_IP_PROTOf:
            out->field[0]->id = ECN_PROTOCOLt_IP_PROTOf;
            out->field[0]->data = ip_proto;
            break;

        case ECN_PROTOCOLt_RESPONSIVEf:
            out->field[idx]->id = ECN_PROTOCOLt_RESPONSIVEf;
            out->field[idx]->data = (uint64_t) responsive;
            break;

        default:
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmecn_ip_protocol_responsive_status_next
 *
 * This method checks the incoming data and finds
 * the next entry and puts the associated data
 * with that entry into the outgoing buffer.
 * The user sends the ip protocol and this methos
 * gets the responsive status of the next protocol and
 * puts into outgoing buffer.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [in] in        Pointer to data containing configuration
 * \param [out] out      Pointer to outgoing buffer.
 * \param [in] arg       Argumnets passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_next (int unit,
                                           uint32_t trans_id,
                                           const bcmltd_fields_t *in,
                                           bcmltd_fields_t *out,
                                           const bcmltd_generic_arg_t *arg)
{
    bool        found = false;
    uint32_t    ip_proto = 0, responsive = 0;
    uint32_t    field_count = 0;
    uint32_t    idx = 0, min = 0, max = 0;
    uint32_t    ip_proto_present = 0;
    bcmlrd_field_def_t     field_def;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    if (in->count < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "The incoming object count is not correct. \n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "The input buffers can not be NULL.\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < in->count; idx++) {
        switch(in->field[idx]->id) {
        case ECN_PROTOCOLt_IP_PROTOf:
            /* Get the protocol field */
            ip_proto = (uint32_t) in->field[idx]->data;
            ip_proto_present = 1;
            break;
        default:
            /* Do not do anything if any other data is sent. */
            break;
        }
    }

    if (!ip_proto_present) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " Key is missing in lookup.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    ECN_PROTOCOLt,
                                    ECN_PROTOCOLt_IP_PROTOf,
                                    &field_def));
    max = BCMECN_FIELD_MAX(field_def.dtag);
    min = (ip_proto + 1);

    for (idx = min; idx <= max; idx++) {
        if (BCMECN_PROTOCOL_IN_USE_GET(unit, idx)) {
            found = true;
            ip_proto = idx;
            break;
        }
    }

    if (!found) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Read hardware table entry to get lookup result. */
    SHR_IF_ERR_EXIT(
        bcmecn_ecn_protocol_pt_read(unit, trans_id, ip_proto, &responsive));

    field_count = out->count;
    /* Feed the information into output fields. */
    for (idx = 0; idx < field_count; idx++) {
        switch (idx) {
        case ECN_PROTOCOLt_IP_PROTOf:
            out->field[0]->id = ECN_PROTOCOLt_IP_PROTOf;
            out->field[0]->data = ip_proto;
            break;

        case ECN_PROTOCOLt_RESPONSIVEf:
            out->field[idx]->id = ECN_PROTOCOLt_RESPONSIVEf;
            out->field[idx]->data = (uint64_t) responsive;
            break;

        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmecn_ip_protocol_responsive_status_commit
 *
 * This method copies the active state to backup.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [in] arg       Arguments passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_commit (int unit,
                                             uint32_t trans_id,
                                             const bcmltd_generic_arg_t *arg)
{
    uint32_t    alloc_sz = 0;
    uint8_t     *act_ptr = NULL;
    uint8_t     *bkp_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    act_ptr = BCMECN_SW_STATE_ACTIVE(unit);
    bkp_ptr = BCMECN_SW_STATE_BKP(unit);
    if (act_ptr == NULL || bkp_ptr == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    alloc_sz = (BCMECN_MAX_PROCOTOLS(unit) * sizeof(uint8_t));

    /* Copy active state to backup state. */
    sal_memcpy(bkp_ptr, act_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmecn_ip_protocol_responsive_status_abort
 *
 * This method restores the active state from the backup state.
 *
 * \param [in] unit      Unit number.
 * \param [in] trans_id  LT Transaction identifier.
 * \param [in] arg       Arguments passed for this method.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX Error that is generated by various checks
 */
int
bcmecn_ip_protocol_responsive_status_abort (int unit,
                                            uint32_t trans_id,
                                            const bcmltd_generic_arg_t *arg)
{
    uint32_t    alloc_sz = 0;
    uint8_t     *act_ptr = NULL;
    uint8_t     *bkp_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    act_ptr = BCMECN_SW_STATE_ACTIVE(unit);
    bkp_ptr = BCMECN_SW_STATE_BKP(unit);
    if (act_ptr == NULL || bkp_ptr == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    alloc_sz = (BCMECN_MAX_PROCOTOLS(unit) * sizeof(uint8_t));

    /* Copy active state to backup state. */
    sal_memcpy(act_ptr, bkp_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Initialize ECN s/w data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecn_sw_resources_alloc (int unit, bool warm)
{
    uint32_t    ha_alloc_size = 0;
    uint32_t    ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    ha_req_size = (BCMECN_MAX_PROCOTOLS(unit) * sizeof(uint8_t));
    ha_alloc_size = ha_req_size;

    BCMECN_SW_STATE_ACTIVE(unit) =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_ECN_COMP_ID,
                         BCMECN_PROTO_SUB_COMP_ID,
                         &ha_alloc_size);

    SHR_NULL_CHECK(BCMECN_SW_STATE_ACTIVE(unit), SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(BCMECN_SW_STATE_ACTIVE(unit), 0, ha_alloc_size);
    }

    ha_alloc_size = ha_req_size;
    BCMECN_SW_STATE_BKP(unit) =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_ECN_COMP_ID,
                         BCMECN_PROTO_BKP_SUB_COMP_ID,
                         &ha_alloc_size);

    SHR_NULL_CHECK(BCMECN_SW_STATE_BKP(unit), SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(BCMECN_SW_STATE_BKP(unit), 0, ha_alloc_size);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (BCMECN_SW_STATE_ACTIVE(unit) != NULL) {
            shr_ha_mem_free(unit, BCMECN_SW_STATE_ACTIVE(unit));
        }
        if (BCMECN_SW_STATE_BKP(unit) != NULL) {
            shr_ha_mem_free(unit, BCMECN_SW_STATE_BKP(unit));
        }
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup ECN s/w data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecn_sw_resources_free (int unit)
{
    SHR_FUNC_ENTER(unit);

    if (BCMECN_SW_STATE_ACTIVE(unit) != NULL) {
        shr_ha_mem_free(unit, BCMECN_SW_STATE_ACTIVE(unit));
    }
    if (BCMECN_SW_STATE_BKP(unit) != NULL) {
        shr_ha_mem_free(unit, BCMECN_SW_STATE_BKP(unit));
    }

    SHR_FUNC_EXIT();
}
