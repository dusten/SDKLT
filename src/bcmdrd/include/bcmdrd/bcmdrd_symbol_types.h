/*! \file bcmdrd_symbol_types.h
 *
 * <description>
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

#ifndef BCMDRD_SYMBOL_TYPES_H
#define BCMDRD_SYMBOL_TYPES_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \name Symbol flags.
 * \anchor BCMDRD_SYMBOL_FLAG_xxx
 */

/*! \{ */

/*! Symbol is not suitable for read/write tests. */
#define BCMDRD_SYMBOL_FLAG_NOTEST                           (1U << 16)

/*! Symbol is a non-CMIC iProc register. */
#define BCMDRD_SYMBOL_FLAG_IPROC                            (1U << 17)

/*! Symbol is an overlay of other symbols. */
#define BCMDRD_SYMBOL_FLAG_OVERLAY                          (1U << 18)

/*! Symbol is read-only or any field within the symbol is read-only. */
#define BCMDRD_SYMBOL_FLAG_READONLY                         (1U << 19)

/*! Symbol with FIFO operations. */
#define BCMDRD_SYMBOL_FLAG_FIFO                             (1U << 20)

/*! Symbol is reasonable to cache in S/W. */
#define BCMDRD_SYMBOL_FLAG_CACHEABLE                        (1U << 21)

/*! Symbol is a hashed table. */
#define BCMDRD_SYMBOL_FLAG_HASHED                           (1U << 22)

/*! Symbol is an external CAM. */
#define BCMDRD_SYMBOL_FLAG_EXT_CAM                          (1U << 23)

/*! Symbol is a CAM. */
#define BCMDRD_SYMBOL_FLAG_CAM                              (1U << 24)

/*! Symbol is a register. */
#define BCMDRD_SYMBOL_FLAG_REGISTER                         (1U << 25)

/*! Symbol is a port-based register, i.e. one ionstance per port. */
#define BCMDRD_SYMBOL_FLAG_PORT                             (1U << 26)

/*! Symbol is a counter register. */
#define BCMDRD_SYMBOL_FLAG_COUNTER                          (1U << 27)

/*! Symbol is a memory. */
#define BCMDRD_SYMBOL_FLAG_MEMORY                           (1U << 28)

/*! Symbol uses big endian word ordering. */
#define BCMDRD_SYMBOL_FLAG_BIG_ENDIAN                       (1U << 29)

/*! Symbol is a memory-mapped register. */
#define BCMDRD_SYMBOL_FLAG_MEMMAPPED                        (1U << 30)

/*! Symbol is a port-block register, i.e. one instance per port-block. */
#define BCMDRD_SYMBOL_FLAG_SOFT_PORT                        (1U << 31)

/*! \} */

/*!
 * \name Symbol attributes.
 * \anchor BCMDRD_SYM_ATTR_xxx
 */

/*! \{ */

/*! Symbol is a CAM. */
#define BCMDRD_SYM_ATTR_CAM                                 (1 << 0)

/*! Symbol is a hashed memory. */
#define BCMDRD_SYM_ATTR_HASHED                              (1 << 1)

/*! Symbol is not visible in this configuration. */
#define BCMDRD_SYM_ATTR_HIDDEN                              (1 << 2)

/*! \} */

/*!
 * \brief Symbol (register/memory) information
 *
 * The symbol information is dynamic information about a symbol in the
 * current device configuraton. In many situations, this information
 * will be identical to the static symbol information of the base
 * device.
 */
typedef struct {

    /*! Symbol ID (unique per device). */
    bcmdrd_sid_t sid;

    /*! Symbol name, e.g. "VLANm" or "MISCCONFIGr". */
    const char *name;

    /*! Mask of block types this symbol is valid for. */
    uint32_t blktypes;

    /*! Special attributes of this symbol (\ref BCMDRD_SYMBOL_FLAG_xxx). */
    uint32_t flags;

    /*! Fixed part of symbol address (composition is device-dependent). */
    uint32_t offset;

    /*! Minimum valid index for array-type symbols. */
    uint32_t index_min;

    /*! Maximum valid index for array-type symbols. */
    uint32_t index_max;

    /*! Size of symbol data (or entry if array-type) in 32-bit words. */
    uint32_t entry_wsize;

    /*! Index address step value for array-type symbols. */
    uint32_t step_size;

} bcmdrd_sym_info_t;

/*!
 * \name Symbol field flags.
 * \anchor BCMDRD_SYMBOL_FIELD_FLAG_xxx
 */

/*! \{ */

/*! Symbol field is a counter field. */
#define BCMDRD_SYMBOL_FIELD_FLAG_COUNTER                    (1 << 0)

/*! Symbol field is a key field. */
#define BCMDRD_SYMBOL_FIELD_FLAG_KEY                        (1 << 1)

/*! Symbol field is a mask field. */
#define BCMDRD_SYMBOL_FIELD_FLAG_MASK                       (1 << 2)

/*! \} */

/*!
 * \brief Field information structure.
 *
 * This structure defines a single field within a symbol.
 */
typedef struct bcmdrd_sym_field_info_s {

    /*! Field name, e.g. "VLANf" or "VALIDf". */
    const char *name;

    /*! Field ID (unique per device). */
    int fid;

    /*! Special field ID, which defines how multi-view memories are decoded. */
    int view;

    /*! Special attributes of this field (\ref BCMDRD_SYMBOL_FIELD_FLAG_xxx). */
    uint32_t flags;

    /*! First bit of this field. */
    uint16_t minbit;

    /*! Last bit of this field. */
    uint16_t maxbit;

} bcmdrd_sym_field_info_t;

#endif /* BCMDRD_SYMBOL_TYPES_H */
