/*! \file bcmdrd_types.h
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

#ifndef BCMDRD_TYPES_H
#define BCMDRD_TYPES_H

#include <bcmdrd_config.h>

#include <sal/sal_types.h>

/*! 16-bit-safe left shift */
#define LSHIFT32(_val, _cnt) ((uint32_t)(_val) << (_cnt))



#ifndef F32_MASK
/*! Create a bit mask of w bits as a 32-bit word. */
#define F32_MASK(w) \
        (((uint32_t)1 << w) - 1)
#endif

#ifndef F32_GET
/*! Extract a field of w bits at offset o from a 32-bit word d. */
#define F32_GET(d,o,w) \
        (((d) >> o) & F32_MASK(w))
#endif

#ifndef F32_SET
/*! Set a field of w bits at offset o in a 32-bit word d. */
#define F32_SET(d,o,w,v) \
        (d = ((d & ~(F32_MASK(w) << o)) | (((v) & F32_MASK(w)) << o)))
#endif

#ifndef F32_ENCODE
/*!
 * Encode a value of a given width at a given offset. Performs
 * compile-time error checking on the value to ensure it fits within
 * the given width.
 */
#define F32_ENCODE(v,o,w) \
        ( ((v & F32_MASK(w)) == v) ? \
          /* Value fits in width */ ( (uint32_t)(v) << o ) : \
          /* Value does not fit -- compile time error */ 1 << 99)

#endif

/*! Max size of register/memory in words */
#define BCMDRD_MAX_PT_WSIZE       32

/*! Words in port bit maps */
#define BCMDRD_PBMP_WORD_MAX       (((BCMDRD_CONFIG_MAX_PORTS - 1) >> 5) + 1)

/*!
 * Bitmap of ports of a particular type or properties.
 */
typedef struct bcmdrd_pbmp_s {
    /*! Word array. */
    uint32_t w[BCMDRD_PBMP_WORD_MAX];
} bcmdrd_pbmp_t;

/* Port bitmap helper functions */

/*!
 * \brief Check if port bitmap is empty.
 *
 * Check that no bits are set in a port bitmap of type \ref
 * bcmdrd_pbmp_t.
 *
 * \param [in] pbmp Port bitmap.
 *
 * \retval true Port bitmap is empty.
 * \retval false Port bitmap is not empty.
 */
extern int
bcmdrd_pbmp_is_null(const bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Check if a port is member of a port bitmap.
 *
 * Check if a port is member of a port bitmap of type \ref
 * bcmdrd_pbmp_t.
 *
 * No range check is performed on the port number.
 *
 * \param [in] _pbmp Port bitmap.
 * \param [in] _port Port number to check.
 */
#define BCMDRD_PBMP_MEMBER(_pbmp, _port) \
     ((&(_pbmp))->w[(_port) >> 5] & LSHIFT32(1, (_port) & 0x1f))

/*!
 * \brief Iterate over a port bitmap.
 *
 * Iterate over a port bitmap of type \ref bcmdrd_pbmp_t and execute
 * the subsequent statement for all bits set in the port bitmap.
 *
 * \param [in] _pbmp Port bitmap.
 * \param [in] _port Port iterator variable.
 */
#define BCMDRD_PBMP_ITER(_pbmp, _port) \
    for (_port = 0; _port < BCMDRD_CONFIG_MAX_PORTS; _port++) \
        if (BCMDRD_PBMP_MEMBER(_pbmp, _port))

/*!
 * \brief Add a port to a port bitmap.
 *
 * Add a port to a port bitmap of type \ref bcmdrd_pbmp_t.
 *
 * No range check is performed on the port number.
 *
 * \param [in] _pbmp Port bitmap.
 * \param [in] _port Port number to add.
 */
#define BCMDRD_PBMP_PORT_ADD(_pbmp, _port) \
     ((&(_pbmp))->w[(_port) >> 5] |= LSHIFT32(1, (_port) & 0x1f))

/*!
 * \brief Remove a port from a port bitmap.
 *
 * Remove a port from a port bitmap of type \ref bcmdrd_pbmp_t.
 *
 * No range check is performed on the port number.
 *
 * \param [in] _pbmp Port bitmap.
 * \param [in] _port Port number to remove.
 */
#define BCMDRD_PBMP_PORT_REMOVE(_pbmp, _port) \
     ((&(_pbmp))->w[(_port) >> 5] &= ~(LSHIFT32(1, (_port) & 0x1f)))

/*!
 * \brief Clear a port bitmap.
 *
 * Clear a port bitmap of type \ref bcmdrd_pbmp_t.
 *
 * After clearing the port bitmap, it will have no members..
 *
 * \param [in] _pbmp Port bitmap.
 */
#define BCMDRD_PBMP_CLEAR(_pbmp) sal_memset(&_pbmp, 0, sizeof(bcmdrd_pbmp_t))

/*!
 * \brief Get a word from a port bitmap.
 *
 * Get a 32-bit word from a port bitmap of type \ref bcmdrd_pbmp_t.
 *
 * \param [in] _pbmp Port bitmap.
 * \param [in] _w Word number to get (first word is word 0).
 */
#define BCMDRD_PBMP_WORD_GET(_pbmp, _w)            ((&(_pbmp))->w[_w])

/*!
 * \brief Set a word in a port bitmap.
 *
 * Set a 32-bit word in a port bitmap of type \ref bcmdrd_pbmp_t.
 *
 * \param [in] _pbmp Port bitmap.
 * \param [in] _w Word number to set (first word is word 0).
 * \param [in] _val Value of word to set.
 */
#define BCMDRD_PBMP_WORD_SET(_pbmp, _w, _val)      ((&(_pbmp))->w[_w]) = (_val)

/*!
 * \brief Helper macro for port bimap operations.
 *
 * \param [in] _pbmp0 First port bitmap.
 * \param [in] _pbmp1 Second port bitmap.
 * \param [in] _op Port bitmap operator.
 */
#define BCMDRD_PBMP_BMOP(_pbmp0, _pbmp1, _op) \
    do { \
        int _w; \
        for (_w = 0; _w < BCMDRD_PBMP_WORD_MAX; _w++) { \
            BCMDRD_PBMP_WORD_GET(_pbmp0, _w) _op BCMDRD_PBMP_WORD_GET(_pbmp1, _w); \
        } \
    } while (0)

/*! Return true if port bitmap _pbmp is empty. */
#define BCMDRD_PBMP_IS_NULL(_pbmp)         (bcmdrd_pbmp_is_null(&(_pbmp)))

/*! Return true if port bitmap _pbmp is not empty. */
#define BCMDRD_PBMP_NOT_NULL(_pbmp)        (!(bcmdrd_pbmp_is_null(&(_pbmp))))

/*! Assign port bitmap src to port bitmap dst. */
#define BCMDRD_PBMP_ASSIGN(dst, src)       sal_memcpy(&(dst), &(src), sizeof(bcmdrd_pbmp_t))

/*!
 * Perform a logical AND operation between all bits of port bitmaps _pbmp0 and
 * pbmp1.
 */
#define BCMDRD_PBMP_AND(_pbmp0, _pbmp1)    BCMDRD_PBMP_BMOP(_pbmp0, _pbmp1, &=)

/*!
 * Perform a logical OR operation between all bits of port bitmaps _pbmp0 and
 * pbmp1.
 */
#define BCMDRD_PBMP_OR(_pbmp0, _pbmp1)     BCMDRD_PBMP_BMOP(_pbmp0, _pbmp1, |=)

/*!
 * Perform a logical XOR operation between all bits of port bitmaps _pbmp0 and
 * pbmp1.
 */
#define BCMDRD_PBMP_XOR(_pbmp0, _pbmp1)    BCMDRD_PBMP_BMOP(_pbmp0, _pbmp1, ^=)

/*!
 * Remove all bits in port bitmap _pbmp1 from port bitmap _pbmp0.
 */
#define BCMDRD_PBMP_REMOVE(_pbmp0, _pbmp1) BCMDRD_PBMP_BMOP(_pbmp0, _pbmp1, &= ~)

/*!
 * Assign an inversed port bitmap _pbmp1 to port bitmap _pbmp0,
 * i.e. any port which is a member of _pbmp1 will not be a member of
 * _pbmp0 and vice versa.
 */
#define BCMDRD_PBMP_NEGATE(_pbmp0, _pbmp1) BCMDRD_PBMP_BMOP(_pbmp0, _pbmp1, = ~)

/*! Initialize a port bitmap from a single 32-bit word. */
#define BCMDRD_PBMP_1(_w0) \
    { { _w0 } }

/*! Initialize a port bitmap from two 32-bit words. */
#define BCMDRD_PBMP_2(_w0, _w1) \
    { { _w0, _w1 } }

/*! Initialize a port bitmap from three 32-bit words. */
#define BCMDRD_PBMP_3(_w0, _w1, _w2) \
    { { _w0, _w1, _w2 } }

/*! Initialize a port bitmap from four 32-bit words. */
#define BCMDRD_PBMP_4(_w0, _w1, _w2, _w3) \
    { { _w0, _w1, _w2, _w3 } }

/*! Initialize a port bitmap from five 32-bit words. */
#define BCMDRD_PBMP_5(_w0, _w1, _w2, _w3, _w4) \
    { { _w0, _w1, _w2, _w3, _w4 } }

/*! Initialize a port bitmap from six 32-bit words. */
#define BCMDRD_PBMP_6(_w0, _w1, _w2, _w3, _w4, _w5) \
    { { _w0, _w1, _w2, _w3, _w4, _w5 } }

/*! Initialize a port bitmap from seven 32-bit words. */
#define BCMDRD_PBMP_7(_w0, _w1, _w2, _w3, _w4, _w5, _w6) \
    { { _w0, _w1, _w2, _w3, _w4, _w5, _w6 } }

/*! Initialize a port bitmap from eight 32-bit words. */
#define BCMDRD_PBMP_8(_w0, _w1, _w2, _w3, _w4, _w5, _w6, _w7) \
    { { _w0, _w1, _w2, _w3, _w4, _w5, _w6, _w7 } }

/*! Initialize a port bitmap from nine 32-bit words. */
#define BCMDRD_PBMP_9(_w0, _w1, _w2, _w3, _w4, _w5, _w6, _w7, _w8)  \
    { { _w0, _w1, _w2, _w3, _w4, _w5, _w6, _w7, _w8 } }

/*! Convert a number of (8-bit) bytes to a number of bits. */
#define BCMDRD_BYTES2BITS(x)    ((x) * 8)

/*! Convert a number of (8-bit) bytes to a number of 32-bit words. */
#define BCMDRD_BYTES2WORDS(x)   (((x) + 3) / 4)

/*! Convert a number of 32-bit words to a number of bits. */
#define BCMDRD_WORDS2BITS(x)    ((x) * 32)

/*! Convert a number of 32-bit words to a number of (8-bit) bytes. */
#define BCMDRD_WORDS2BYTES(x)   ((x) * 4)


/*! Create enumeration values from list of supported devices. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    BCMDRD_DEV_T_##_bd,
/*! Enumeration for all base device types. */
typedef enum {
    BCMDRD_DEV_T_NONE = 0,
#include <bcmdrd/bcmdrd_devlist.h>
    BCMDRD_DEV_T_COUNT
} bcmdrd_dev_type_t;

/*! Generic ID (enum). */
typedef uint32_t bcmdrd_id_t;

/*! Generic invalid ID value. */
#define BCMDRD_INVALID_ID       ((bcmdrd_id_t)-1)

/*! Invalid register value. */
#define INVALIDr        BCMDRD_INVALID_ID

/*! Invalid memory value. */
#define INVALIDm        BCMDRD_INVALID_ID

/*! Invalid field value. */
#define INVALIDf        BCMDRD_INVALID_ID

/*! Check if an ID is valid, i.e. different from BCMDRD_INVALID_ID. */
#define BCMDRD_ID_VALID(_id) \
    ((_id) != BCMDRD_INVALID_ID)

/*! Device-specific symbol ID (enum). */
typedef bcmdrd_id_t bcmdrd_sid_t;

/*! Device-specific field ID (enum). */
typedef bcmdrd_id_t bcmdrd_fid_t;

/*!
 * \brief Port number domain.
 *
 * Port-based registers and memories use different port number domains
 * in their physical address. For example, some registers use the
 * physical port number, some registers use the logical port number
 * and some use a MMU port number.
 *
 * For most devices, each block type use the same port number domain,
 * but there are a few exceptions, so this needs to be a per-reg/mem
 * property.
 */
typedef enum bcmdrd_port_num_domain_e {
    BCMDRD_PND_PHYS = 0,
    BCMDRD_PND_LOGIC = 1,
    BCMDRD_PND_MMU = 2,
    BCMDRD_PND_COUNT
} bcmdrd_port_num_domain_t;

#endif /* BCMDRD_TYPES_H */
