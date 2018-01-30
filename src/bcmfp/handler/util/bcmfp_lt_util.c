/*! \file bcmfp_lt_util.c
 *
 * APIs for common utlity functions
 *
 * This file contains function definitions for common funtions
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
  Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmmeter/bcmmeter_fp_ing.h>
#include <bcmmeter/bcmmeter_fp_egr.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*******************************************************************************
  Static Functions
 */
int
bcmfp_lt_lrd_info_init(int unit,
                      bcmfp_lt_info_t *lt_info)
{
    uint32_t i, elements = 0;
    size_t num_fids;
    const bcmlrd_table_rep_t *tbl_rep = bcmlrd_table_get(lt_info->ltid);
    bcmlrd_client_field_info_t *f_info = NULL;
    bcmfp_lt_field_info_t *field_info = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, lt_info->ltid, &num_fids));
    BCMFP_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fids, "");
    BCMFP_ALLOC(field_info, sizeof(bcmfp_lt_field_info_t) * num_fids, "");

    SHR_IF_ERR_EXIT(bcmlrd_table_fields_def_get(unit,
                        tbl_rep->name, num_fids, f_info, &num_fids));
    for(i = 0; i < num_fids; i++) {
        sal_memcpy(&field_info[i].min, &f_info[i].min,
                    sizeof(bcmltd_field_data_t));
        sal_memcpy(&field_info[i].max, &f_info[i].max,
                    sizeof(bcmltd_field_data_t));
        sal_memcpy(&field_info[i].def, &f_info[i].def,
                    sizeof(bcmltd_field_data_t));
        field_info[i].dtag = f_info[i].dtag;
        field_info[i].fid = f_info[i].id;
        field_info[i].access = f_info[i].access;
        field_info[i].width = f_info[i].width;
        field_info[i].elements = f_info[i].elements;
        field_info[i].is_symbol = f_info[i].symbol;
        if (f_info[i].depth != 0) {
            field_info[i].is_array = TRUE;
            field_info[i].depth = f_info[i].depth;
        }
        field_info[i].field_coord = elements;
        elements += field_info[i].elements;
    }
    lt_info->fields_info = field_info;
    lt_info->fields = num_fids;
    lt_info->out_fields = elements;
    field_info = NULL;

exit:

    SHR_FREE(f_info);
    SHR_FREE(field_info);
    SHR_FUNC_EXIT();
}


int
bcmfp_lt_lrd_info_cleanup(int unit,
                          bcmfp_lt_info_t *lt_info)
{
    SHR_FREE(lt_info->fields_info);
    return SHR_E_NONE;
}


int
bcmfp_meter_entry_exists(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint64_t meter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_valid(unit, stage_id, meter_id));

exit:

    SHR_FUNC_EXIT();
}


int
bcmfp_counter_entry_exists(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint64_t counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_info_get(int unit,
                        uint32_t fid,
                        bcmfp_lt_info_t *lt_info,
                        bcmfp_lt_field_info_t **field_info)
{
    int32_t i;
    bcmfp_lt_field_info_t *field_info_list = NULL;
    SHR_FUNC_ENTER(unit);

    field_info_list = lt_info->fields_info;
    /* Fid is always between index=0 to index=fid */
    for (i = (fid < lt_info->fields) ? fid : (lt_info->fields - 1);
              i >= 0; i--) {
        if (field_info_list[i].fid == fid) {
            *field_info = &field_info_list[i];
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_duplicate_check(int unit,
                               const bcmltd_fields_t *multi_fields,
                               const bcmltd_fields_t *inmem_multi_fields)
{
    uint32_t i,j;
    uint32_t elements = 0;
    uint32_t data = 0;
    uint32_t idx = 0;
    bcmltd_fields_t m_fields = {0};
    bcmltd_field_t **field = NULL;

    SHR_FUNC_ENTER(unit);

    elements = multi_fields->count + inmem_multi_fields->count;

    BCMFP_ALLOC(field, sizeof(bcmltd_field_t*) * elements, "Field ptr array");

    m_fields.field = field;

    /* Consolidate inmem and update multifields to single m fields*/
    for (i = 0 ; i < multi_fields->count; i++) {
        m_fields.field[i] = multi_fields->field[i];
    }

    for (j = 0 ; j < inmem_multi_fields->count; j++) {
        m_fields.field[i + j] = inmem_multi_fields->field[j];
    }

    for (i = 0; i < elements; i++) {
        data = m_fields.field[i]->data;
        idx = m_fields.field[i]->idx;
        if (data == 0)  {
            continue;
        }
        for (j = (i + 1); j < elements; j++) {
            /* Allow config, If the idx is same*/
            if ((m_fields.field[j]->data == data) &&
                ( m_fields.field[j]->idx != idx)) {
                /* Duplicate data */
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FREE(field);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_data_get(int unit,
                        uint32_t fid,
                        const bcmltd_fields_t *in,
                        uint64_t *data)
{
    uint32_t i = 0;
    SHR_FUNC_ENTER(unit);

    for(i = 0; i < in->count; i++) {
        if (in->field[i]->id == fid) {
            *data = in->field[i]->data;
            break;
        }
    }
    if(i == in->count) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_multi_data_get(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t fid,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *multi_fields)
{
    bool found = FALSE;
    bool more_fields = FALSE;
    uint32_t i = 0;
    uint32_t j = 0;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == fid) {
            if(j >= multi_fields->count) {
                more_fields = TRUE;
                break;
            }
            multi_fields->field[j]->id = fid;
            multi_fields->field[j]->idx = in->field[i]->idx;
            multi_fields->field[j]->data = in->field[i]->data;
            found = TRUE;
            j++;
        }
    }

    if(more_fields) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    if(!found) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    multi_fields->count = j;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_bcmltd_fields_buff_alloc(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmltd_fields_t *in)
{
    uint32_t i = 0;
    uint32_t out_fields = 0;
    bcmfp_lt_info_t *lt_info = NULL;
    SHR_FUNC_ENTER(unit);

    in->count = 0;
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, ltid, &lt_info));

    out_fields = lt_info->out_fields;
    BCMFP_ALLOC(in->field, out_fields * sizeof(bcmltd_field_t *),
                "bcmltd field array");
    for (i = 0; i < out_fields; i++) {
        BCMFP_ALLOC(in->field[i], sizeof (bcmltd_field_t), "field");
    }
    in->count = out_fields;

exit:

    if(SHR_FUNC_ERR()) {
        bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, in);
    }
    SHR_FUNC_EXIT();

}

int
bcmfp_lt_bcmltd_fields_buff_free(int unit,
                                 uint32_t fields,
                                 bcmltd_fields_t *in)
{
    uint32_t i = 0;
    SHR_FUNC_ENTER(unit);

    if((fields == 0) || (in->field == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (i = 0; i < fields; i++) {
        SHR_FREE(in->field[i]);
    }
    SHR_FREE(in->field);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_data_set(int unit,
                        bcmltd_sid_t ltid,
                        uint32_t fid,
                        uint64_t *data,
                        bcmltd_fields_t *out)
{
    uint32_t i = 0;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < out->count; i++) {
        if (fid == out->field[i]->id) {
            out->field[i]->data = *data;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_data_to_uint32_array(int unit,
                                    bcmltd_field_t *field,
                                    bcmfp_lt_field_info_t *field_info,
                                    uint32_t *field_val)
{
    uint32_t idx = 0;
    SHR_FUNC_ENTER(unit);

    idx = field->idx;
    /* Max. 64-bit array element is supported. */
    if (field_info->is_array) {
        idx = 0;
    }
    field_val[2 * idx]     = field->data & 0xFFFFFFFF;
    field_val[2 * idx + 1] = (field->data >> 32) & 0xFFFFFFFF;

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_add(int unit,
                   bcmltd_sid_t ltid,
                   uint32_t fid,
                   uint32_t out_fields,
                   bcmltd_fields_t *imm_fields)
{
    uint32_t i = 0;
    uint32_t last_index = 0;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_field_info_t *field_info = NULL;
    uint32_t ele_count = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, ltid, &lt_info));

    last_index = imm_fields->count;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_field_info_get(unit, fid, lt_info, &field_info));

    ele_count = (field_info->is_array) ? 1 : field_info->elements;

    if ((last_index + ele_count) >= out_fields) {
         SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < ele_count; i++) {
        imm_fields->field[last_index + i]->id = fid;
        imm_fields->field[last_index + i]->idx = i;
        imm_fields->field[last_index + i]->data = 0;
    }

    imm_fields->count = last_index + ele_count;

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_data_get_from_data_array(int unit,
                                        uint32_t fid,
                                        const bcmltd_field_t *data_start,
                                        uint64_t *data_val)
{
  bcmltd_field_t *data = (bcmltd_field_t *)data_start;

  SHR_FUNC_ENTER(unit);

  SHR_NULL_CHECK(data, SHR_E_PARAM);

  do {
    if (data->id == fid)  {
      *data_val = data->data;
      break;
    }
    data = data->next;
  } while (data != NULL);

  if(data == NULL) {
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
  }

exit:
  SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_key_get_from_key_array(int unit,
                                      uint32_t fid,
                                      const bcmltd_field_t *key_start,
                                      uint64_t *key_val)
{
  bcmltd_field_t *key =  (bcmltd_field_t *)key_start;

  SHR_FUNC_ENTER(unit);

  if(key == NULL) {
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
  }

  do {
    if (key->id == fid)  {
      *key_val = key->data;
      break;
    }
    key = key->next;
  } while (key != NULL);

  if(key == NULL) {
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
  }

exit:
  SHR_FUNC_EXIT();
}

int
bcmfp_lt_field_multi_data_get_from_data_array(int unit,
                                              uint32_t fid,
                                              const bcmltd_field_t *data_start,
                                              bcmltd_fields_t *multi_fields)
{
    bool found = FALSE;
    bool more_fields = FALSE;
    bcmltd_field_t *data = (bcmltd_field_t *)data_start;
    uint32_t j = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);

    do {
        if (data->id == fid) {
            if(j >= multi_fields->count) {
                more_fields = TRUE;
                break;
            }
            multi_fields->field[j]->id = fid;
            multi_fields->field[j]->idx = data->idx;
            multi_fields->field[j]->data = data->data;
            found = TRUE;
            j++;
        }
        data = data->next;
    } while (data != NULL);

    if(more_fields) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    if(!found) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    multi_fields->count = j;
exit:
    SHR_FUNC_EXIT();
}
