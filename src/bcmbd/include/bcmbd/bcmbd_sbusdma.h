/*! \file bcmbd_sbusdma.h
 *
 * SBUSDMA data structures and APIs
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

#ifndef BCMBD_SBUSDMA_H
#define BCMBD_SBUSDMA_H

/*! Maximum number of SBUSDMA devices supported. */
#define SBUSDMA_DEV_NUM_MAX     BCMDRD_CONFIG_MAX_UNITS

/*!
 * \name Data attributes.
 * \anchor SBUSDMA_DA_xxx
 */

/*! \{ */
/*! Append data buffer to the previous work item's, only used for batch work. */
#define SBUSDMA_DA_APND_BUF     (1 << 0)

/*! Jump to another work item indicated by "jump_item", only used for batch work. */
#define SBUSDMA_DA_JUMP_TO      (1 << 1)

/*! Skip over the current work item, only used for batch work. */
#define SBUSDMA_DA_SKIP_OVER    (1 << 2)
/*! \} */

/*!
 * \name Work flags.
 * \anchor SBUSDMA_WF_xxx
 */

/*! \{ */
/*! Read operation. */
#define SBUSDMA_WF_READ_CMD     (0 << 0)

/*! Write operation. */
#define SBUSDMA_WF_WRITE_CMD    (1 << 0)

/*! Use interrupt mode. */
#define SBUSDMA_WF_INT_MODE     (1 << 1)

/*! Inverse operation, access from the highest entry to the lowest entry. */
#define SBUSDMA_WF_DEC_ADDR     (1 << 2)

/*! Single data operation, access all the entries only using the same data of a single entry. */
#define SBUSDMA_WF_SGL_DATA     (1 << 3)

/*! Single address operation, only access a single entry using the data of all entries one by one. */
#define SBUSDMA_WF_SGL_ADDR     (1 << 4)
/*! \} */

/*!
 * \brief SBUSDMA work data.
 */
typedef struct bcmbd_sbusdma_data_s {
    /*! Start address of entries. */
    uint32_t start_addr;

    /*! Entry width in words. */
    uint32_t data_width;

    /*! Address bits of gap between entries. */
    uint32_t addr_gap;

    /*! Operation code. */
    uint32_t op_code;

    /*! Number of operations. */
    uint32_t op_count;

    /*! DMA buffer physical address. */
    uint64_t buf_paddr;

    /*! Jump to item. */
    int jump_item;

    /*! Data attributes (\ref SBUSDMA_DA_xxx). */
    uint32_t attrs;

    /*! Core clock delay between multiple pending SBUS commands (MOR). */
    uint32_t pend_clocks;
} bcmbd_sbusdma_data_t;

/*! Function type for SBUSDMA operation callbacks. */
typedef void (*sbusdma_cb_f)(int unit, void *data);

/*!
 * \brief SBUSDMA work state.
 */
typedef enum bcmbd_sbusdma_state_e {
    /*! Idle work. */
    SBUSDMA_WORK_NULL = 0,

    /*! Initialized light work. */
    SBUSDMA_WORK_INITED = 1,

    /*! Created batch work. */
    SBUSDMA_WORK_CREATED = 2,

    /*! Destroyed batch work. */
    SBUSDMA_WORK_DESTROYED = 3,

    /*! Queued light or batch work. */
    SBUSDMA_WORK_QUEUED = 4,

    /*! Scheduled light or batch work. */
    SBUSDMA_WORK_SCHED = 5,

    /*! Started light or batch work. */
    SBUSDMA_WORK_STARTED = 6,

    /*! Completed work. */
    SBUSDMA_WORK_DONE = 7,

    /*! Timeout work. */
    SBUSDMA_WORK_TIMEOUT = 8,

    /*! Failed work. */
    SBUSDMA_WORK_ERROR = 9,
} bcmbd_sbusdma_state_t;

/*!
 * \brief SBUSDMA work.
 */
typedef struct bcmbd_sbusdma_work_s {
    /*! Work data. */
    bcmbd_sbusdma_data_t *data;

    /*! Work items. */
    int items;

    /*! Pre-config callback. */
    sbusdma_cb_f pc;

    /*! Pre-config callback parameter. */
    void *pc_data;

    /*! Post callback. */
    sbusdma_cb_f cb;

    /*! Post callback parameter. */
    void *cb_data;

    /*! Work link used internally. */
    void *link;

    /*! Work descriptor chain used internally. */
    void *desc;

    /*! Work flags (\ref SBUSDMA_WF_xxx). */
    uint32_t flags;

    /*! Work state. */
    bcmbd_sbusdma_state_t state;
} bcmbd_sbusdma_work_t;

/*!
 * \brief Attach SBUSDMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_attach(int unit);

/*!
 * \brief Detach SBUSDMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_detach(int unit);

/*!
 * \brief Initialize light work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_light_work_init(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Create batch work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_batch_work_create(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Delete batch work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_batch_work_delete(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Execute light or batch work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_work_execute(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Add light or batch work to work queue.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_work_add(int unit, bcmbd_sbusdma_work_t *work);

#endif /* BCMBD_SBUSDMA_H */

