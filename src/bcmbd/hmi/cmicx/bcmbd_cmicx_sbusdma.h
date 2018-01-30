/*! \file bcmbd_cmicx_sbusdma.h
 *
 * CMICx SBUSDMA data structures
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

#ifndef BCMBD_CMICX_SBUSDMA_H
#define BCMBD_CMICX_SBUSDMA_H

#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_sbusdma_internal.h>

/*! Maximum number of CMCs in CMICx. */
#define CMICX_SBUSDMA_CMC_MAX   2
/*! Mask of CMCs. */
#define CMICX_SBUSDMA_CMC_MASK  0x3
/*! Maximum number of channels per CMC. */
#define CMICX_SBUSDMA_CMC_CHAN  4
/*! Maximum number of channels in device. */
#define CMICX_SBUSDMA_CHAN_MAX  (CMICX_SBUSDMA_CMC_MAX * CMICX_SBUSDMA_CMC_CHAN)
/*! Mask of channels per CMC. */
#define CMICX_SBUSDMA_CHAN_MASK 0xF

/*! Wait microseconds. */
#define CMICX_SBUSDMA_WAIT      200
/*! Retry times. */
#define CMICX_SBUSDMA_RETRY     10000

/*!
 * \brief CMICx SBUSDMA descriptor (8 Dwords).
 */
typedef struct cmicx_sbusdma_desc_s {
    /*! Control word. */
    uint32_t control;

    /*! Request word. */
    uint32_t request;

    /*! Count word. */
    uint32_t count;

    /*! Opcode word. */
    uint32_t opcode;

    /*! Address word. */
    uint32_t address;

    /*! Hostaddr LO word. */
    uint32_t hostaddr_lo;

    /*! Hostaddr HI word. */
    uint32_t hostaddr_hi;

    /*! Reserved. */
    uint32_t reserved;
} cmicx_sbusdma_desc_t;

/*!
 * \name Control word definitions.
 */

/*! \{ */
#define CMICX_SBUSDMA_DESC_CTRL_LAST    (1 << 31)
#define CMICX_SBUSDMA_DESC_CTRL_SKIP    (1 << 30)
#define CMICX_SBUSDMA_DESC_CTRL_JUMP    (1 << 29)
#define CMICX_SBUSDMA_DESC_CTRL_APND    (1 << 28)
#define CMICX_SBUSDMA_DESC_CTRL_REMN(x) (((x) & 0xf) << 24)
#define CMICX_SBUSDMA_DESC_BURST_MAX    (128 / sizeof(cmicx_sbusdma_desc_t))
/*! \} */

/*!
 * \brief Descriptors address container.
 */
typedef struct cmicx_sbusdma_desc_addr_s {
    /*! DMA memory physical address. */
    uint64_t paddr;

    /*! DMA memory address. */
    void *addr;

    /*! DMA memory size. */
    uint32_t size;
} cmicx_sbusdma_desc_addr_t;

/*!
 * \name Control flags.
 * \anchor SBUSDMA_CF_xxx
 */

/*! \{ */
/*! Descriptor in big-endianess mode. */
#define SBUSDMA_CF_DESC_BIGENDIAN       (1 << 0)

/*! Memory read in big-endianess mode. */
#define SBUSDMA_CF_WR_BIGENDIAN         (1 << 1)

/*! Memory write in big-endianess mode. */
#define SBUSDMA_CF_RD_BIGENDIAN         (1 << 2)
/*! \} */

/*!
 * \name Device status.
 * \anchor SBUSDMA_DS_xxx
 */

/*! \{ */
/*! Channels usage status. */
#define SBUSDMA_DS_CHAN_BUSY            (1 << 0)
/*! \} */

/*!
 * \brief CMICx SBUSDMA device structure.
 */
typedef struct cmicx_sbusdma_dev_s {
    /*! Device number. */
    int unit;

    /*! SBUSDMA control. */
    sbusdma_ctrl_t *ctrl;

    /*! Bitmap of CMCs enabled. */
    uint32_t bm_cmc;

    /*! Bitmap of channels enabled. */
    uint32_t bm_chan;

    /*! Semaphore for channel management. */
    sal_sem_t sem;

    /*! Spinlock for channel management. */
    sal_spinlock_t lock;

    /*! Control flags (\ref SBUSDMA_CF_xxx). */
    uint32_t flags;

    /*! Device status (\ref SBUSDMA_DS_xxx). */
    uint32_t status;

    /*! Device initialization indicator. */
    int inited;
} cmicx_sbusdma_dev_t;

/*!
 * \brief Execute light work in register mode.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_sbusdma_reg_op(int unit, bcmbd_sbusdma_work_t *work);

#endif /* BCMBD_CMICX_SBUSDMA_H */

