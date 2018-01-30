/*! \file bcmfp_keygen_md.h
 *
 * All data structures, macros and enumerations internal
 * to keygen alogorithm.
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

#ifndef BCMFP_KEYGEN_MD_H
#define BCMFP_KEYGEN_MD_H

#include <bcmfp/bcmfp_keygen_api.h>

#define BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE (-1)

/*
 * Typedef:
 *     bcmfp_keygen_ext_sel_t
 * Purpose:
 *     Logical table key generation program control selectors.
 */
typedef struct bcmfp_keygen_ext_sel_s {
    int8_t ipbm_source;         /* Ingress Port Bitmap Source.          */
    int8_t ipbm_present;          /* Post mux IPBM in final key.          */
    int8_t normalize_l3_l4_sel;   /* Normalize L3 and L4 address.         */
    int8_t normalize_mac_sel;     /* Normalize L2 SA & DA.                */
    int8_t aux_tag_a_sel;         /* Auxiliary Tag A Selector.            */
    int8_t aux_tag_b_sel;         /* Auxiliary Tag B Selector.            */
    int8_t aux_tag_c_sel;         /* Auxiliary Tag C Selector.            */
    int8_t aux_tag_d_sel;         /* Auxiliary Tag d Selector.            */
    int8_t tcp_fn_sel;            /* TCP function selector.               */
    int8_t tos_fn_sel;            /* TOS function selector.               */
    int8_t ttl_fn_sel;            /* TTL function selector.               */
    int8_t class_id_cont_a_sel;   /* Class ID container A selector.       */
    int8_t class_id_cont_b_sel;   /* Class ID container B selector.       */
    int8_t class_id_cont_c_sel;   /* Class ID container C selector.       */
    int8_t class_id_cont_d_sel;   /* Class ID container D selector.       */
    int8_t src_cont_a_sel;        /* SRC container A selector.            */
    int8_t src_cont_b_sel;        /* SRC container B selector.            */
    int8_t src_dest_cont_0_sel;   /* SRC or DST container 0 selector.     */
    int8_t src_dest_cont_1_sel;   /* SRC or DST container 1 selector.     */
    int8_t dst_cont_a_sel;        /* DST container A selector.            */
    int8_t dst_cont_b_sel;        /* DST container B selector.            */
    int8_t udf_sel;               /* UDF0 or UDF1 selector.               */
    int8_t pmux_sel[15];          /* Post muxing extractors.              */
} bcmfp_keygen_ext_sel_t;

#define BCMFP_KEYGEN_EXT_GRANULAR_ARR_SIZE 5

#define BCMFP_KEYGEN_EXT_IN_SECTION_COUNT  5

#define BCMFP_KEYGEN_EXT_SECTION_PASS_THRU (1 << 0)

typedef struct bcmfp_keygen_ext_section_gran_s {

    uint8_t gran;

    uint8_t num_extractors;

} bcmfp_keygen_ext_section_gran_t;

typedef struct bcmfp_keygen_ext_section_gran_info_s {
    /* granularity information for out section. */
    bcmfp_keygen_ext_section_gran_t  out_gran_info[BCMFP_KEYGEN_EXT_GRANULAR_ARR_SIZE];
    /* number of valid indices in out_gran_info. */
    uint8_t out_gran_info_count;
    /* granularity information for out multi-gran section. */
    bcmfp_keygen_ext_section_gran_t  out_multigran_info[BCMFP_KEYGEN_EXT_GRANULAR_ARR_SIZE];
    /* number of valid indices in out_multugran_info. */
    uint8_t out_multigran_info_count;
    /* granularity information for in section.*/
    bcmfp_keygen_ext_section_gran_t  in_gran_info[BCMFP_KEYGEN_EXT_GRANULAR_ARR_SIZE];
    /* number of valid indices in in_gran_info. */
    uint8_t in_gran_info_count;

} bcmfp_keygen_ext_section_gran_info_t;

typedef enum bcmfp_keygen_acl_attr_s {


    BCMFP_KEYGEN_ACL_ATTR_MODE_HALF = 0,

    BCMFP_KEYGEN_ACL_ATTR_MODE_SINGLE_ASET_WIDE,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_IPBM,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_DROP,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_A,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_B,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_PKT_RESOLUTION,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_C,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_D,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_INT_PRI,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_IFP_DROP_VECTOR,

    BCMFP_KEYGEN_ACL_ATTR_PMUX_MH_OPCODE,

    BCMFP_KEYGEN_ACL_ATTR_COUNT

} bcmfp_keygen_acl_attr_t;

typedef struct bcmfp_keygen_acl_attrs_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_KEYGEN_ACL_ATTR_COUNT)];
} bcmfp_keygen_acl_attrs_t;

#define BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT 1

#define BCMFP_KEYGEN_EXT_FIELD_CHUNKS_MAX 128

#define BCMFP_KEYGEN_EXT_FIELDS_MAX 50

#define BCMFP_KEYGEN_EXT_GRAN_COUNT 5

#define BCMFP_KEYGEN_EXT_SECTION_GRAN_COUNT 3

#define BCMFP_KEYGEN_EXT_SECTIONS_COUNT 5

#define BCMFP_KEYGEN_EXT_FIELD_OFFSET_MAX 128

#define BCMFP_KEYGEN_EXT_FIELD_OFFSET_COUNT 16

typedef struct bcmfp_keygen_ext_field_offset_s {

    uint16_t offset;

    uint16_t width;

    uint8_t chunk_id;

} bcmfp_keygen_ext_field_offset_t;

typedef struct  bcmfp_keygen_ext_field_offset_info_s {

    uint16_t offset_count;

    bcmfp_keygen_ext_field_offset_t offsets[BCMFP_KEYGEN_EXT_FIELD_OFFSET_MAX];

}  bcmfp_keygen_ext_field_offset_info_t;

typedef struct bcmfp_keygen_ext_field_s {

     bcmfp_keygen_ext_section_t section;

     uint8_t sec_val;

     uint16_t offset;

     uint8_t width;

     uint32_t req_bmp[BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT];

     uint32_t rsp_bmp[BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT];

} bcmfp_keygen_ext_field_t;

/* When this flag is set, 16-bit granularity extractors are used at
 * level 2. This flag is set only in case of flowtracker style
 * extractors.
 */
#define BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN16  (1 << 0)

/* When this flag is set, 8-bit granularity extractors are used at
 * level 2. This flag is set only in case of flowtracker style
 * extractors.
 */
#define BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN8   (1 << 1)

typedef struct bcmfp_keygen_ext_field_info_s {

    uint32_t flags;

    uint8_t extracted;

    int part;

    bcmfp_keygen_ext_section_t section;

    uint8_t sec_val;

    uint8_t size;

    uint8_t req_bit_count;

    uint32_t req_bmp[BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT];

    uint32_t ext_bmp[BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT];

    bcmfp_keygen_ext_section_t l0_section;

    uint8_t l0_sec_val;

    uint16_t l0_field_offset;

    bcmfp_keygen_ext_ctrl_sel_t pri_ctrl_sel;

    uint16_t pri_ctrl_sel_val;

    bcmfp_keygen_ext_cfg_t *ext_cfg;

    uint8_t prev_offset;

    struct bcmfp_keygen_ext_field_info_s *prev;

    struct bcmfp_keygen_ext_field_info_s *next;

} bcmfp_keygen_ext_field_info_t;

typedef struct bcmfp_keygen_ext_field_info_db_s {

     bcmfp_keygen_ext_field_info_t *finfo[BCMFP_KEYGEN_EXT_SECTION_COUNT];

} bcmfp_keygen_ext_field_info_db_t;

typedef bcmfp_keygen_ext_section_t bcmfp_keygen_ext_section_list_t[BCMFP_KEYGEN_EXT_SECTION_COUNT];

typedef enum bcmfp_keygen_pmux_type_e {

    BCMFP_KEYGEN_PMUX_TYPE_IPBM  = 0,

    BCMFP_KEYGEN_PMUX_TYPE_DROP,

    BCMFP_KEYGEN_PMUX_TYPE_NAT_NEEDED,

    BCMFP_KEYGEN_PMUX_TYPE_NAT_DST_REALM_ID,

    BCMFP_KEYGEN_PMUX_TYPE_SRC_DST_CONT_0,

    BCMFP_KEYGEN_PMUX_TYPE_SRC_DST_CONT_1,

    BCMFP_KEYGEN_PMUX_TYPE_SRC_DST_CONT_0_SLICE_B,

    BCMFP_KEYGEN_PMUX_TYPE_SRC_DST_CONT_1_SLICE_B,

    BCMFP_KEYGEN_PMUX_TYPE_SRC_DST_CONT_0_SLICE_C,

    BCMFP_KEYGEN_PMUX_TYPE_SRC_DST_CONT_1_SLICE_C,

    BCMFP_KEYGEN_PMUX_TYPE_EM_FIRST_LOOKUP_HIT,

    BCMFP_KEYGEN_PMUX_TYPE_EM_SECOND_LOOKUP_HIT,

    BCMFP_KEYGEN_PMUX_TYPE_EM_FIRST_LOOKUP_CLASSID,

    BCMFP_KEYGEN_PMUX_TYPE_EM_SECOND_LOOKUP_CLASSID,

    BCMFP_KEYGEN_PMUX_TYPE_COUNT

} bcmfp_keygen_pmux_type_t;

typedef struct bcmfp_keygen_pmux_type_bmp_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_KEYGEN_PMUX_TYPE_COUNT)];
} bcmfp_keygen_pmux_type_bmp_t;

typedef struct bcmfp_keygen_pmux_info_s {

    bcmfp_qual_t pmux_qual;

    uint8_t pmux_part;

    bcmfp_keygen_qual_cfg_t *pmux_qual_cfg;

} bcmfp_keygen_pmux_info_t;

/* This metatdata structure has all the information
 * required during keygen creation.
 */
typedef struct bcmfp_keygen_md_s {

    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info[BCMFP_QUAL_COUNT];

    bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db[BCMFP_KEYGEN_EXT_LEVEL_COUNT];

    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db;

    uint16_t rkey_size;

    uint8_t *qual_cfg_id_arr;

    uint16_t qual_cfg_combinations;

    bcmfp_keygen_acl_attrs_t acl_attrs[BCMFP_KEYGEN_ENTRY_PARTS_MAX];

    bcmfp_keygen_ext_field_info_t finfo[BCMFP_KEYGEN_EXT_FIELDS_MAX];

    bcmfp_keygen_ext_field_info_db_t ext_finfo_db[BCMFP_KEYGEN_ENTRY_PARTS_MAX][BCMFP_KEYGEN_EXT_LEVEL_COUNT];

    uint32_t bits_extracted[BCMFP_KEYGEN_ENTRY_PARTS_MAX][BCMFP_KEYGEN_EXT_GRAN_COUNT];

    uint8_t next_part_id[BCMFP_KEYGEN_EXT_GRAN_COUNT];

    uint8_t tried_part_bmp[BCMFP_KEYGEN_EXT_GRAN_COUNT];

    bcmfp_keygen_ext_section_list_t sections[BCMFP_KEYGEN_ENTRY_PARTS_MAX][BCMFP_KEYGEN_EXT_LEVEL_COUNT];

    bcmfp_keygen_ext_section_gran_info_t section_gran_info[BCMFP_KEYGEN_EXT_SECTION_COUNT];

    uint8_t num_ext_levels;

    bcmfp_keygen_ext_field_offset_info_t offset_info;

    bcmfp_keygen_ext_sel_t ext_codes[BCMFP_KEYGEN_ENTRY_PARTS_MAX];

    bcmfp_keygen_pmux_info_t *pmux_info;

    uint8_t num_pmux_qual;

} bcmfp_keygen_md_t;

#define BCMFP_KEYGEN_EXT_SECTION_SIZE_GET(section, size)          \
    {                                                            \
        if (BCMFP_KEYGEN_EXT_SECTION_L1E32 == section) {          \
            f_size = 32;                                         \
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E16 == section) {   \
            f_size = 16;                                         \
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E8 == section)  {   \
            f_size = 8;                                          \
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E4 == section)  {   \
            f_size = 4;                                          \
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E2 == section)  {   \
            f_size = 2;                                          \
        } else {                                                 \
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);         \
        }                                                        \
    }

#define BCMFP_KEYGEN_EXT_SECTION_BASE_IDX(section, base_idx)         \
        {                                                           \
           if (BCMFP_KEYGEN_EXT_SECTION_L1E32 == section) {          \
               base_idx = 0;                                        \
           } else if (BCMFP_KEYGEN_EXT_SECTION_L1E16 == section) {   \
               base_idx = 1;                                        \
           } else if (BCMFP_KEYGEN_EXT_SECTION_L1E8 == section)  {   \
               base_idx = 2;                                        \
           } else if (BCMFP_KEYGEN_EXT_SECTION_L1E4 == section)  {   \
               base_idx = 3;                                        \
           } else if (BCMFP_KEYGEN_EXT_SECTION_L1E2 == section)  {   \
               base_idx = 4;                                        \
           } else {                                                 \
               SHR_IF_ERR_EXIT(SHR_E_INTERNAL);         \
           }                                                        \
        }

extern int
bcmfp_keygen_ext_cfg_reset(int unit,
                          bcmfp_keygen_cfg_t *keygen_cfg,
                          bcmfp_keygen_md_t *keygen_md);
extern int
bcmfp_keygen_ext_cfg_alloc(int unit,
                          bcmfp_keygen_cfg_t *keygen_cfg,
                          bcmfp_keygen_md_t *keygen_md);
extern int
bcmfp_keygen_ext_cfg_reset_part(int unit,
                               uint8_t part,
                               bcmfp_keygen_cfg_t *keygen_cfg,
                               bcmfp_keygen_md_t *keygen_md);
extern int
bcmfp_keygen_ext_sections_get(int unit,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_md_t *keygen_md);

extern int
bcmfp_keygen_ext_alloc(int unit,
                      bcmfp_keygen_cfg_t *keygen_cfg,
                      bcmfp_keygen_oper_t *keygen_oper,
                      bcmfp_keygen_md_t *keygen_md);

extern int
bcmfp_keygen_ext_codes_init(int unit, bcmfp_keygen_ext_sel_t *ext_codes);

extern int
bcmfp_keygen_ext_part_get(int unit,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md,
                         bcmfp_keygen_ext_field_info_t *finfo);

extern int
bcmfp_keygen_ext_codes_validate(int unit,
                               uint8_t part,
                               bcmfp_keygen_cfg_t *keygen_cfg,
                               bcmfp_keygen_md_t *keygen_md);

extern int
bcmfp_keygen_ext_part_incr(int unit,
                          bcmfp_keygen_cfg_t *keygen_cfg,
                          bcmfp_keygen_md_t *keygen_md,
                          bcmfp_keygen_ext_section_t section);

extern int
bcmfp_keygen_ext_codes_create(int unit,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_md_t *keygen_md,
                             bcmfp_keygen_oper_t *keygen_oper);

extern int
bcmfp_keygen_pmux_process(int unit,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md);
extern int
bcmfp_keygen_pmux_info_get(int unit,
                    bcmfp_qual_t qual,
                    uint8_t num_pmux_qual,
                    bcmfp_keygen_pmux_info_t *pmux_info_arr,
                    bcmfp_keygen_pmux_info_t **pmux_info);

extern int
bcmfp_keygen_pmux_qual_offset_get(int unit,
                                 bcmfp_qual_t qual,
                                 bcmfp_keygen_md_t *keygen_md,
                                 bcmfp_keygen_cfg_t *keygen_cfg,
                                 bcmfp_keygen_qual_offset_t *qual_offset);
extern int
bcmfp_keygen_ext_validate(int unit,
                       uint8_t part,
                       bcmfp_keygen_md_t *keygen_md,
                       bcmfp_keygen_ext_cfg_t *ext_cfg);
extern int
bcmfp_keygen_ext_section_gran_info_init(int unit,
                              bcmfp_keygen_md_t *keygen_md);

extern int
bcmfp_keygen_qual_cfg_max_size_get(int unit,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            bcmfp_qual_t qual_id,
                            uint16_t *qual_cfg_max_size);
extern int
bcmfp_keygen_ext_cfg_update(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        bcmfp_keygen_md_t *keygen_md);

#endif /* BCMFP_KEYGEN_MD_H */
