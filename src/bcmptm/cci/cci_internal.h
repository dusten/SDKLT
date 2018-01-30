/*! \file cci_internal.h
 *
 * Defines shared across CCI sub-modules
 *
 * This file contains utils, defines shared across CCI sub-modules
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

#ifndef CCI_INTERNAL_H
#define CCI_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_thread.h>
#include <sal/sal_time.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm_cci_internal.h>

/*!
 * \brief CCI time out in microseconds.
 */
#define CFG_CCI_TIME_OUT      10000000

/*!
 * \brief Message Q time out
 */
#define MSGQ_TIME_OUT      1000000

/*!
 * \brief Retry Count
 */
#define CFG_SER_RETRY_COUNT      3

/*!
 * \brief MAx Physical port count (words * sizeof word * bits in byte)
 */
#define CCI_MAX_PPORT_COUNT      (BCMDRD_PBMP_WORD_MAX * 4 * 8)

/*!
 * \brief Time Delay in usec
 */
#define CCI_DELAY_TIME(tm)      (sal_time_usecs() - (tm))


/*! BSL_LOG_MODULE used for SHR_FUNC_ENTER, EXIT
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CCI

/*! Maximum number of threads that can be used for counter collection
 */
#define CCI_COL_MAX_THREADS     (1)


/*! If configuration is needed, in case of cold boot or
 * config state of previous boot is not completed
 */

#define DO_CONFIG(warm, con) \
             ((!warm) || (con->info->state < CCI_STATE_CONFIG))



/*!
 * \brief Description CCI  state information
 */

typedef enum {
    /*! System Manager Init  */
    CCI_STATE_INIT = 1,

    /*! System Manager Configure  */
    CCI_STATE_CONFIG,

    /*! System Manager Run  */
    CCI_STATE_RUN,

    /*! System Manager Stop  */
    CCI_STATE_STOP,

    /*! System Manager Shut down */
    CCI_STATE_SHUTDOWN
} cci_state_t;

/*!
* \brief Thread message Queue commands
*/

typedef enum {
    MSG_CMD_POL_CONFIG_COL_EN = 1,
    MSG_CMD_POL_CONFIG_DMA,
    MSG_CMD_POL_CONFIG_INTERVAL,
    MSG_CMD_POL_CONFIG_PORT,
    MSG_CMD_POL_CONFIG_MULTIPLIER_PORT,
    MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE,
    MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE,
    MSG_CMD_POL_CONFIG_MULTIPLIER_TM,
    MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT,
    MSG_CMD_FIFODMA_REQ,
    MSG_CMD_POL_STOP
} cci_pol_cmd_t;

/*!
* \brief Description of counter thread message
*/

typedef struct col_thread_msg_s {
    /*! Collection thread command types */
   cci_pol_cmd_t cmd;
    /*! message data */
   uint64_t      data[BCMDRD_PBMP_WORD_MAX/2];
}  col_thread_msg_t;

/*!
 * \brief Description CCI inforamton. This will be saved in HA space
 */

typedef struct cci_info_s {
    /*! Logical device id  */
    int unit;

    /*! cci state  */
    cci_state_t state;

    /*! sub sub state  */
    int sub;

    /*! Size of counter map table */
    size_t ctr_map_size;

    /*! Size of counter table */
    size_t ctr_table_size;
} cci_info_t;


/*!
 * \brief Description of the current configuration
 */
typedef struct cci_config_s {
    /*! Enable collection */
    bool col_enable;

    /*! Number of threads */
    uint8_t nthreads;

    /*! Interval time period in microseconds */
    uint32_t interval;

    /*! DMA based collection */
    bool dma;

    /*! Interval multipier */
    uint8_t multiplier[CCI_CTR_TYPE_NUM];

    /*! Ports active ports */
    bcmdrd_pbmp_t pbmp;
} cci_config_t;

/*!
 * \brief In memory table resources
 */
typedef struct config_inmem_s {
    /*! Config Logical Table ID */
    bcmltd_sid_t sid;

    /*! Number of Field ID's */
    size_t num_fid;

    /*! Number of Field count */
    size_t count_fld;

    /*! Field Information  */
    bcmlrd_client_field_info_t *f_info;

   /*! Configuration fields */
    bcmltd_fields_t fields[2];
} config_inmem_t;

/*!
 * \brief Handles of subcompont initialized per unit
 */

typedef void *cci_handle_t;

/*!
 * \brief Description context of CCI
 */

typedef struct cci_context_s {
    /*! Logical device id  */
    int unit;

    /*! Handle for collection */
    cci_handle_t hcol;

    /*! Handle for collection */
    cci_handle_t hcache;

    /*! HA pointer for CCI information */
    cci_info_t  *info;

    /*! In memory configuration */
    config_inmem_t config;

    /*! In memory configuration initialized */
    bool init_config;
} cci_context_t;


/*!
 * \brief Get CCI Configuration information
 *
 * \param [in] unit Logical device id
 * \param [out] config, pointer to structure cci_config_t
 *
 * \retval SHR_E_NONE Success
 */
int
bcmptm_cci_config_get(int unit,
                      cci_config_t *config);


/*!
 * \brief Get CCI cache Handle
 *
 * \param [in] unit Logical device id
 * \param [in]con CCI context
 * \param [out] handle of CCI Cache Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_handle_get(int unit,
                            const cci_context_t *con,
                            cci_handle_t *handle);

#endif /* CCI_INTERNAL_H */
