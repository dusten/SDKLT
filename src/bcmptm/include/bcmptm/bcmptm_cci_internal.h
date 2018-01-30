/*! \file bcmptm_cci_internal.h
 *
 * Counter Collection Infrastructure(CCI)  interfaces
 *
 * This file contains APIs, defines for CCI interfaces Used by PTM
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

#ifndef BCMPTM_CCI_INTERNAL_H
#define BCMPTM_CCI_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmdrd/bcmdrd_pt.h>

/*!
 * \brief Description Counter Type
 */

typedef enum {
    /*! Port counters */
    CCI_CTR_TYPE_PORT = 0,

    /*! Traffice management counters */
    CCI_CTR_TYPE_TM,

    /*! Egress pipeline counters */
    CCI_CTR_TYPE_EPIPE,

    /*! Ingress pipeline counters */
    CCI_CTR_TYPE_IPIPE,

    /*! Eviction counters */
    CCI_CTR_TYPE_EVICT,

    CCI_CTR_TYPE_NUM
} cci_ctr_type_id;

/*!
 * \brief Description of information associated to counter SID
 */
typedef struct bcmptm_cci_ctr_info_s {
    uint32_t flags;               /*!< flags to alter default behavior */
    uint32_t  map_id;             /*!< map_id counter map id */
    bcmdrd_sid_t sid;             /*!< sid Enum to specify reg, mem */
    int  txfm_fld;                /*! Transformed field Num */
    bcmbd_pt_dyn_info_t dyn_info; /*! dynamic info */
    uint32_t lt_map_id;           /*!< transformed counter map id */
    void *in_pt_dyn_info;         /*!< pt_dyn_info Ptr dynamic info */
    void *in_pt_ovrr_info;        /*!< pt_ovrr_info Ptr, leave unchanged */
    bcmltd_sid_t req_ltid;        /*!< req_ltid Logical Table enum */
    bcmltd_sid_t *rsp_ltid;       /* rsp_ltid,set req_ltid=rsp_ltid */
    uint32_t *rsp_flags;          /*rsp_flags, response flags */
} bcmptm_cci_ctr_info_t;

/*!
 * \brief SER request information to do counters correction
 */
typedef struct bcmptm_cci_ser_req_info_s {
    bcmdrd_sid_t        sid;        /* Counter SID */
    bcmbd_pt_dyn_info_t dyn_info;   /* Dynamic info associated with SID */
} bcmptm_cci_ser_req_info_t;

/******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Initialize CCI (initialize and allocate resources  etc)
 * \n Called during System Manager INIT state for each unit
 *
 * \param [in] unit Logical device id
 * \param [in] warm  warm boot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_init(int unit,
                    bool warm);

/*!
 * \brief Register counter with CCI
 * \n Called during System Manager CONFIGURE state for each unit
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] map_id, id in counter map table
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_reg(int unit,
                    bcmdrd_sid_t sid,
                    uint32_t *map_id);


/*!
 * \brief  Configure CCI
 * \n Called during System Manager CONFIG state for each unit
 *
 * \param [in] unit Logical device id
 * \param [in] warm  warm boot
 * \param [in] phase configuration phase (1,2,3)
 *
 * Phase 1: CCI will determine the size of Counter
 * Map Table and Counter Table in HA sace.
 * Phase 2: CCI will initialize  Counter Map Table
 * Phase 3: CCI set up  the CCI Configuration information
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_comp_config(int unit,
                       bool warm,
                       bcmptm_sub_phase_t phase);


/*!
 * \brief Allocate DMA resources, configure Hardware, Start threads
 * \n Called during System Manager RUN state for each unit
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_run(int unit);

/*!
 * \brief Read Dynamic fileds of Counter SID
 *
 * \param [in] unit Logical device id
 * \param [in] info counter Info
 * \param [out] entry_words, counter value
 * \param [in] entry_size, buffer size
 * \pRead from Counter Cache
 *
 * \LT:           Counter only
 * \PYS SID:    Counter (Counter Fileds)
 * \Buffer:       64 bit counts
 * \CCI Cache: DONT_USE_CACHE=0
 *                  No Change
 *                  DONT_USE_CACHE=1
 *                  Increment with value read from HW
 * \PT Cache:  No Change
 * \HW:          DONT_USE_CACHE=0
 *                  No op
 *                  DONT_USE_CACHE=1
 *                  Issue Read to HW.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_read(int unit,
                    bcmptm_cci_ctr_info_t *info,
                    uint32_t *rsp_entry_words,
                    size_t   rsp_entry_size);

/*!
 * \brief Read Configuration fields Counter HW Entry
 *
 * \param [in] unit Logical device id
 * \param [in] info counter Info
 * \param [out] entry_words, counter value
 * \param [in] entry_size, buffer size
 *
 * \LT:            Config fields
 * \PYS SID:    Counter (Config Fileds)
 * \Buffer:       HW_entry format
 * \CCI Cache: No Change
 * \PT Cache:  Issue Read
 * \HW:          Issue read from HW if PTcache read was not successful.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_config_read(int unit,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *rsp_entry_words,
                           size_t   rsp_entry_size);

/*!
 * \brief Read Counter HW Entry in pass through mode
 *
 * \param [in] unit Logical device id
 * \param [in] info counter Info
 * \param [out] entry_words, counter value
 * \param [in] entry_size, buffer size
 *
 * \LT:            Physial COunter Table
 * \PYS SID:    Physial COunter Table
 * \Buffer:       HW_entry format
 * \CCI Cache: No Change
 * \PT Cache:  Issue Read
 * \HW:          Issue read from HW if PTcache read was not successful.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_passthru_read(int unit,
                             bcmptm_cci_ctr_info_t *info,
                             uint32_t *rsp_entry_words,
                             size_t   rsp_entry_size);

/*!
 * \brief Write Counter value
 *
 * \param [in] unit Logical device id
 * \param [in] counter Info
 * \param [in] entry_words, counter value
 *
 * \LT:           Counter only
 * \PYS SID:    Counter (Counter Fileds)
 * \Buffer:       64 bit counts
 * \CCI Cache: Will be updated to user-provided value
 * \PT Cache:  No Change
 * \HW:          Zero out counter-only fields by doing Read-modify-Write
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_write(int unit,
                     bcmptm_cci_ctr_info_t *info,
                     uint32_t *entry_words);

/*!
 * \brief Write Configuration fields Counter HW Entry
 *
 * \param [in] unit Logical device id
 * \param [in] counter Info
 * \param [in] entry_words, counter value
 *
 * \LT:            Config fields
 * \PYS SID:    Counter (Config Fileds)
 * \Buffer:       HW_entry format
 * \CCI Cache: Clear CCI cache
 * \PT Cache:  Issue update
 * \HW:          Directly write entry (no read-modify-write).
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_config_write(int unit,
                            bcmptm_cci_ctr_info_t *info,
                            uint32_t *entry_words);

/*!
 * \brief Write Counter HW Entry in pass through mode
 *
 * \param [in] unit Logical device id
 * \param [in] counter Info
 * \param [in] entry_words, counter value
 *
 * \LT:            Physial Counter Table
 * \PYS SID:    Physial COunter Table
 * \Buffer:       HW_entry format
 * \CCI Cache: No Change
 * \PT Cache:  Issue update
 * \HW:          Issue Write
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_passthru_write(int unit,
                              bcmptm_cci_ctr_info_t *info,
                              uint32_t *entry_words);




/*!
 * \brief Request to handle SER event on Counter SID
 *
 * \param [in] unit Logical device id
 * \param [in] sid , symbol ID of the counter
 * \param [in] pt_dyn_info, Ptr to dynamic info
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_ser_req(int unit,
                       bcmdrd_sid_t sid,
                       void *pt_dyn_info);

/*!
 * \brief  Request to handle SER event on Counter SIDs to clear counters.
 *
 * \param [in] unit             Logical device id
 * \param [in] ser_req_info     Request SID and dynamic address
 * \param [in] array_count      Request array count of the ser_req_info
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_pt_cci_ser_array_req(int unit,
                            bcmptm_cci_ser_req_info_t *ser_req_info,
                            int array_count);

/*!
 * \brief Stop (terminate thread , free resources)
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_stop(int unit);


/*!
 * \brief De-Initialize CCI software resources
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cleanup(int unit);

/*!
 * \brief Get Counter Type
 *
 * \param [in] unit Logical device id
 * \param [in] unit Symbol ID
 *
 * \retval Counter Type on Success
 */
extern int
bcmptm_pt_cci_ctrtype_get(int unit,
                          bcmdrd_sid_t sid);

/*!
 * \brief Map the counter to appropriate symbol.
 *
 * \param [in] unit Logical device id
 * \param [in] ctr_info Counter information
 *
 * \retval Counter Type on Success
 */
int
bcmptm_pt_cci_ctr_sym_map(int unit,
                          bcmptm_cci_ctr_info_t *ctr_info);

/*!
 * \brief Get format symbol ID if exist.
 *
 * \param [in] unit Unit number
 * \param [in, out] sid Symbol ID, out format Symbol ID, if exist
 * \retval SHR_E_NONE No errors
 */

extern int
bcmptm_pt_cci_frmt_sym_get(int unit, bcmdrd_sid_t *sid);

/*!
 * \brief Get index range in formatted table for given port.
 *
 * \param [in] unit Unit number
 * \param [in, out] sid Symbol ID, out format Symbol ID, if exist
 * \retval SHR_E_NONE No errors
 */

extern int
bcmptm_pt_cci_frmt_index_from_port(int unit, bcmdrd_sid_t sid, int port,
                                   int *index_min, int *index_max);


/*!
 * \brief Check if physical table index is valid.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] tbl_inst Table instance
 * \param [in] index Table index to check
 *
 * \retval true Index is valid
 * \retval false Index is not valid
 */
extern bool
bcmptm_pt_cci_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index);

/*!
 * \brief Enable eviction counter
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] enable, TRUE(Enable), FALSE(DISABLE)
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmptm_pt_cci_ctr_evict_enable(int unit, bcmdrd_sid_t sid, bool enable);

/*!
 * \brief Eviction counter is enabled
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] enable, TRUE(Enable), FALSE(DISABLE)
 *
* \retval true Enabled
 * \retval false Disabled
 */
extern int
bcmptm_pt_cci_ctr_evict_is_enable(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Eviction counter is clear on read
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] enable, TRUE(Enable), FALSE(DISABLE)
 *
* \retval true Enabled
 * \retval false Disabled
 */
extern int
bcmptm_pt_cci_ctr_evict_is_clr_on_read(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Check if Symbol is in bypass mode.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 *
* \retval true Bypass
 */

extern bool
bcmptm_pt_cci_ctr_is_bypass(int unit, bcmdrd_sid_t sid);

#endif /* BCMPTM_CCI_INTERNAL_H */
