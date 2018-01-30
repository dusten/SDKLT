/*! \file bcmtnl_internal.h
 *
 * Internal Tunnel handler function header file
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

#ifndef BCMTNL_INTERNAL_H
#define BCMTNL_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>

/*! Structure to maintain SW state for MPLS tunnel entries. */
typedef struct bcmtnl_mpls_encap_entry_s {
    /*! Maximum number of labels that may be added in this entry. */
    int max_labels;
    /*! Number of labels currently added in this entry. */
    int num_labels;
} bcmtnl_mpls_encap_entry_t;

/*! IPV4 tunnel LT entry populate */
typedef int (*bcmtnl_ipv4_encap_hw_to_lt_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint32_t tnl_id,
                                                      uint8_t *buf,
                                                      bcmltd_fields_t *out);

/*! IPV6 tunnel LT entry populate */
typedef int (*bcmtnl_ipv6_encap_hw_to_lt_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint32_t tnl_id,
                                                      uint8_t *buf,
                                                      bcmltd_fields_t *out);
/*! MPLS tunnel LT entry populate */
typedef int (*bcmtnl_mpls_encap_hw_to_lt_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint32_t tnl_id,
                                                      bcmtnl_mpls_encap_entry_t *entry,
                                                      uint8_t *buf,
                                                      bcmltd_fields_t *out);

/*! IPV4 tunnel HW entry populate */
typedef int (*bcmtnl_ipv4_encap_lt_to_hw_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      const bcmltd_fields_t *in,
                                                      uint8_t *entry_buf);

/*! IPV6 tunnel HW entry populate */
typedef int (*bcmtnl_ipv6_encap_lt_to_hw_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      const bcmltd_fields_t *in,
                                                      uint8_t *entry_buf);

/*! MPLS tunnel HW entry populate */
typedef int (*bcmtnl_mpls_encap_lt_to_hw_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint32_t sub_id,
                                                      bcmtnl_mpls_encap_entry_t *entry,
                                                      const bcmltd_fields_t *in,
                                                      uint8_t *entry_buf);
/*! MPLS tunnel HW entry clear */
typedef int (*bcmtnl_mpls_encap_hw_entry_clear_f)(int unit,
                                                  uint32_t hw_sid,
                                                  uint32_t sub_id,
                                                  bcmtnl_mpls_encap_entry_t *entry,
                                                  bool remove,
                                                  uint8_t *entry_buf);
/*!
 * Tunnel encap driver structure.
 */
typedef struct bcmtnl_encap_drv_s {
    /*! Number of ipv4 tunnels supported. */
    uint32_t    num_ipv4_tnl;
    /*! Number of ipv6 tunnels supported. */
    uint32_t    num_ipv6_tnl;
    /*! Number of mpls tunnels supported. */
    uint32_t    num_mpls_tnl;
    /*! Maximum number of mpls labels per tunnel entry. */
    uint32_t    max_labels;
    /*! Width of one IPV4 tunnel entry relative to the base tunnel(IPV4). */
    uint8_t     ipv4_tnl_width;
    /*! Width of one IPV6 tunnel entry relative to the base tunnel(IPV4). */
    uint8_t     ipv6_tnl_width;
    /*! Width of one MPLS tunnel entry relative to the base tunnel(IPV4). */
    uint8_t     mpls_tnl_width;
    /*! First hardware index valid for IPv4. */
    uint8_t     ipv4_start_idx;
    /*! First hardware index valid for IPv6. */
    uint8_t     ipv6_start_idx;
    /*! First hardware index valid for MPLS. */
    uint8_t     mpls_start_idx;
    /*! Hardware table that is mapped to TNL_IPV4_ENCAP LT. */
    uint32_t    ipv4_hw_sid;
    /*! Hardware table that is mapped to TNL_IPV6_ENCAP LT. */
    uint32_t    ipv6_hw_sid;
    /*! Hardware table that is mapped to TNL_MPLS_ENCAP LT. */
    uint32_t    mpls_hw_sid;
    /*! Function to convert HW entry to LT entry. */
    bcmtnl_ipv4_encap_hw_to_lt_entry_map_f ipv4_hw_to_lt_map;
    /*! Function to convert HW entry to LT entry. */
    bcmtnl_ipv6_encap_hw_to_lt_entry_map_f ipv6_hw_to_lt_map;
    /*! Function to convert HW entry to LT entry. */
    bcmtnl_mpls_encap_hw_to_lt_entry_map_f mpls_hw_to_lt_map;
    /*! Function to convert LT entry to HW entry. */
    bcmtnl_ipv4_encap_lt_to_hw_entry_map_f ipv4_lt_to_hw_map;
    /*! Function to convert LT entry to HW entry. */
    bcmtnl_ipv6_encap_lt_to_hw_entry_map_f ipv6_lt_to_hw_map;
    /*! Function to convert LT entry to HW entry. */
    bcmtnl_mpls_encap_lt_to_hw_entry_map_f mpls_lt_to_hw_map;
    /*! Function to clear MPLS HW entry. */
    bcmtnl_mpls_encap_hw_entry_clear_f     mpls_hw_entry_clear;
} bcmtnl_encap_drv_t;

/*! No tunnel present. */
#define TNL_TYPE_NONE                       (0)
/*! IPV4 TUNNEL. */
#define TNL_TYPE_IPV4                       (1)
/*! IPV6 TUNNEL. */
#define TNL_TYPE_IPV6                       (2)
/*! MPLS TUNNEL. */
#define TNL_TYPE_MPLS                       (3)

/*! Dont push the label. */
#define BCMTNL_MPLS_PUSH_NONE               (0)
/*! Push this label. */
#define BCMTNL_MPLS_PUSH_ONE                (1)
/*! Push this label and the next label. */
#define BCMTNL_MPLS_PUSH_TWO                (2)

/*! Tunnel encap driver for this unit. */
#define BCMTNL_ENCAP_DRV(unit)              bcmtnl_encap_drv[unit]

/*! Number of ipv4 tunnels supported for this unit. */
#define BCMTNL_NUM_IPV4(unit)               BCMTNL_ENCAP_DRV(unit)->num_ipv4_tnl
/*! Number of ipv6 tunnels supported for this unit. */
#define BCMTNL_NUM_IPV6(unit)               BCMTNL_ENCAP_DRV(unit)->num_ipv6_tnl
/*! Number of mpls tunnels supported for this unit. */
#define BCMTNL_NUM_MPLS(unit)               BCMTNL_ENCAP_DRV(unit)->num_mpls_tnl
/*! Maximum number of mpls labels per tunnel entry for this unit. */
#define BCMTNL_MAX_MPLS_LABELS(unit)        BCMTNL_ENCAP_DRV(unit)->max_labels
/*! Width of one IPV4 tunnel entry relative to the base tunnel(IPV4) for this unit. */
#define BCMTNL_IPV4_WIDTH(unit)             BCMTNL_ENCAP_DRV(unit)->ipv4_tnl_width
/*! Width of one IPV6 tunnel entry relative to the base tunnel(IPV4) for this unit. */
#define BCMTNL_IPV6_WIDTH(unit)             BCMTNL_ENCAP_DRV(unit)->ipv6_tnl_width
/*! Width of one MPLS tunnel entry relative to the base tunnel(IPV4) for this unit. */
#define BCMTNL_MPLS_WIDTH(unit)             BCMTNL_ENCAP_DRV(unit)->mpls_tnl_width
/*! First hardware index valid for IPv4 for this unit. */
#define BCMTNL_IPV4_START_IDX(unit)         BCMTNL_ENCAP_DRV(unit)->ipv4_start_idx
/*! First hardware index valid for IPv6 for this unit. */
#define BCMTNL_IPV6_START_IDX(unit)         BCMTNL_ENCAP_DRV(unit)->ipv6_start_idx
/*! First hardware index valid for MPLS for this unit. */
#define BCMTNL_MPLS_START_IDX(unit)         BCMTNL_ENCAP_DRV(unit)->mpls_start_idx
/*! Hardware table that is mapped to TNL_IPV4_ENCAP LT for this unit. */
#define BCMTNL_IPV4_HW_SID(unit)            BCMTNL_ENCAP_DRV(unit)->ipv4_hw_sid
/*! Hardware table that is mapped to TNL_IPV6_ENCAP LT for this unit. */
#define BCMTNL_IPV6_HW_SID(unit)            BCMTNL_ENCAP_DRV(unit)->ipv6_hw_sid
/*! Hardware table that is mapped to TNL_MPLS_ENCAP LT for this unit. */
#define BCMTNL_MPLS_HW_SID(unit)            BCMTNL_ENCAP_DRV(unit)->mpls_hw_sid
/*! Convert IPV4 HW entry to LT entry. */
#define BCMTNL_IPV4_HW_TO_LT_ENTRY(unit)    (BCMTNL_ENCAP_DRV(unit)->ipv4_hw_to_lt_map)
/*! Convert IPV6 HW entry to LT entry. */
#define BCMTNL_IPV6_HW_TO_LT_ENTRY(unit)    (BCMTNL_ENCAP_DRV(unit)->ipv6_hw_to_lt_map)
/*! Convert MPLS HW entry to LT entry. */
#define BCMTNL_MPLS_HW_TO_LT_ENTRY(unit)    (BCMTNL_ENCAP_DRV(unit)->mpls_hw_to_lt_map)
/*! Convert IPV4 LT entry to HW entry. */
#define BCMTNL_IPV4_LT_TO_HW_ENTRY(unit)    (BCMTNL_ENCAP_DRV(unit)->ipv4_lt_to_hw_map)
/*! Convert IPV6 LT entry to HW entry. */
#define BCMTNL_IPV6_LT_TO_HW_ENTRY(unit)    (BCMTNL_ENCAP_DRV(unit)->ipv6_lt_to_hw_map)
/*! Convert MPLS LT entry to HW entry. */
#define BCMTNL_MPLS_LT_TO_HW_ENTRY(unit)    (BCMTNL_ENCAP_DRV(unit)->mpls_lt_to_hw_map)
/*! Clear MPLS HW entry. */
#define BCMTNL_MPLS_HW_ENTRY_CLEAR(unit)    (BCMTNL_ENCAP_DRV(unit)->mpls_hw_entry_clear)

/*! Get the minimum value of an LT field. */
#define BCMTNL_FIELD_MIN(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                  (fdef.min.u8) :                               \
                                  ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                   (fdef.min.u16) :                             \
                                   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                    (fdef.min.u32) :                            \
                                    ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                     (fdef.min.u64) :                           \
                                     (fdef.min.is_true)))))


/*! Get the default value of an LT field. */
#define BCMTNL_FIELD_DEF(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                  (fdef.def.u8) :                               \
                                  ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                   (fdef.def.u16) :                             \
                                   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                    (fdef.def.u32) :                            \
                                    ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                     (fdef.def.u64) :                           \
                                     (fdef.def.is_true)))))

/*! Get the maximum value of an LT field. */
#define BCMTNL_FIELD_MAX(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                  (fdef.max.u8) :                               \
                                  ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                   (fdef.max.u16) :                             \
                                   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                    (fdef.max.u32) :                            \
                                    ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                     (fdef.max.u64) :                           \
                                     (fdef.max.is_true)))))

/*! Convert uint32 array into one uint64 variable. */
#define BCMTNL_ENCAP_UINT32_TO_UINT64(val32, val64)     \
    do {                                                \
        val64 = val32[1];                               \
        val64 = ((val64 << 32) | val32[0]);             \
    } while(0)

/*! Convert uint64 variable into uint32 array. */
#define BCMTNL_ENCAP_UINT64_TO_UINT32(val64, val32)     \
    do {                                                \
        val32[0] = (uint32_t)val64;                     \
        val32[1] = (uint32_t)(val64 >> 32);             \
    } while(0)

/*! SW state for each index entry in tunnel table */
typedef struct bcmtnl_encap_sw_state_s {
    /*! IPv4/v6 tunnel bitmap*/
    SHR_BITDCL *bcmtnl_base_tnl_bitmap;
    /*! MPLS tunnel bitmap */
    SHR_BITDCL *bcmtnl_mpls_tnl_bitmap;
    /*! Tunnel type. */
    uint8_t    *bcmtnl_base_tnl_type;
    /*! Mpls tunnel entry software state */
    bcmtnl_mpls_encap_entry_t *bcmtnl_mpls_entry;
} bcmtnl_encap_sw_state_t;

/*! Tunnel encap software state. */
#define BCMTNL_ENCAP_SW_STATE(unit)     bcmtnl_encap_sw_state[unit]
/*! Tunnel encap committed software state. */
#define BCMTNL_ENCAP_SW_STATE_HA(unit)  bcmtnl_encap_sw_state_ha[unit]

/*! Tunnel sub component ID for base tunnel bitmap s/w state. */
#define BCMTNL_BASE_TNL_BMAP_SUB_COMP_ID    1
/*! Tunnel sub component ID for mpls tunnel bitmaps/w state. */
#define BCMTNL_MPLS_TNL_BMAP_SUB_COMP_ID    2
/*! Tunnel sub component ID for base tunnel type s/w state. */
#define BCMTNL_BASE_TNL_TYPE_SUB_COMP_ID    3
/*! Tunnel sub component ID for mpls tunnel entry s/w state. */
#define BCMTNL_MPLS_ENTRY_STATE_SUB_COMP_ID 4
/*! Tunnel sub component ID for L3 interface s/w state. */
#define BCMTNL_L3_INTF_SUB_COMP_ID          5

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_tnl_encap_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_tnl_encap_drv_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Internal validation of all field values entered by user.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          LT operation code.
 * \param [in]  sid             Logical Table ID.
 * \param [in]  in              Input parameters entered by user.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmtnl_entry_validate(int unit,
                      bcmlt_opcode_t opcode,
                      bcmltd_sid_t sid,
                      const bcmltd_fields_t *in);

/*!
 * \brief Insert a new entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  opcode          Transaction opcode.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmtnl_entry_insert(int unit,
                    uint32_t trans_id,
                    bcmlt_opcode_t opcode,
                    uint32_t lt_sid,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out);

/*!
 * \brief Lookup an entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmtnl_entry_lookup(int unit,
                    uint32_t trans_id,
                    uint32_t lt_sid,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out);

/*!
 * \brief Delete an entry from the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmtnl_entry_delete(int unit,
                    uint32_t trans_id,
                    uint32_t lt_sid,
                    const bcmltd_fields_t *in);


/*!
 * \brief Lookup an entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          LT ID.
 * \param [in]  in              Input parameters entered by user.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmtnl_entry_traverse(int unit,
                      uint32_t trans_id,
                      uint32_t lt_sid,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out);

/*!
 * \brief Initialise Tunnel SW structures.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  cold            Cold/Warm boot.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

extern int
bcmtnl_sw_state_init(int unit, bool cold);

/*!
 * \brief Free allocated Tunnel SW structures on exit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

extern int
bcmtnl_sw_state_cleanup(int unit);

/*!
 * \brief Commit SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          Logical table ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmtnl_transaction_commit(int unit,
                          uint32_t trans_id,
                          uint32_t lt_sid);

/*!
 * \brief Abort SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  lt_sid          Logical table ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmtnl_transaction_abort(int unit,
                         uint32_t trans_id,
                         uint32_t lt_sid);
/*!
 * \brief Set pointer to tunnel encap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Chip driver structure.
 *
 * \return SHR_E_NONE
 */
int
bcmtnl_encap_drv_set(int unit, bcmtnl_encap_drv_t *drv);

/*!
 * \brief Get pointer to tunnel encap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
int
bcmtnl_encap_drv_get(int unit, bcmtnl_encap_drv_t **drv);

#endif /* BCMTNL_INTERNAL_H */
