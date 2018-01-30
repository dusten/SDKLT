/*
 * $Id: phymod_acc.h,v 1.1.2.3 2013/06/11 15:43:36 mlarsen Exp $
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

#ifndef __PHYMOD_ACC_H__
#define __PHYMOD_ACC_H__

#include <phymod/phymod_system.h>
#include <phymod/phymod.h>

/*
 * Occasionally the phymod_bus_t changes to support new features. This define
 * allows applications to write backward compatible PHY bus drivers.
 */
#define PHYMOD_BUS_VERSION              1

/*
 * Extended PHY bus operation.
 *
 * If a PHY bus driver supports extended capabilities this must be
 * indicated via the flags field.  The access driver will then provide
 * additional information embedded in the function parameters.
 *
 * Read parameter encodings:
 *
 *   addr:  bits [31:16] - address extension
 *          bits [15:0]  - register address
 *
 *   data:  bits [31:16] - unused
 *          bits [15:0]  - register value
 * 
 * Write parameter encodings:
 *
 *   addr:  bits [31:16] - address extension
 *          bits [15:0]  - register address
 *
 *   data:  bits [31:16] - write mask
 *          bits [15:0]  - register value
 *
 * The address extension is hardware-sepcific, but typically it
 * contains the clause 45 DEVAD and a XAUI lane encoding.
 *
 * A write mask of zero will write all bits, otherwise only the
 * register value bits for which the corresponsing mask bit it set
 * will be written.
 */
 
/* Raw (unadjusted) PHY address */
#define PHYMOD_ACC_CACHE_INVAL(pa_) \
    PHYMOD_ACC_CACHE(pa_) = ~0
#define PHYMOD_ACC_CACHE_SYNC(pa_,data_) \
    PHYMOD_ACC_CACHE(pa_) = (data_)
#define PHYMOD_ACC_CACHE_IS_DIRTY(pa_,data_) \
    ((PHYMOD_ACC_FLAGS(pa_) & PHYMOD_ACC_F_USE_CACHE) == 0 || \
      PHYMOD_ACC_CACHE(pa_) != (data_))
#define PHYMOD_ACC_CACHE_ENABLE(pa_) do { \
        PHYMOD_ACC_FLAGS(pa_) |= PHYMOD_ACC_F_USE_CACHE; \
        PHYMOD_ACC_CACHE_INVAL(pa_); \
    } while (0)

/* PHY bus address (MDIO PHYAD) */
#define PHYMOD_ACC_BUS_ADDR(pa_) PHYMOD_ACC_ADDR(pa_)

/* Ensures that phymod_access_t structure is properly initialized */
#define PHYMOD_ACC_CHECK(pa_) \
    do { \
        if (phymod_acc_check(pa_) < 0) return PHYMOD_E_INTERNAL; \
    } while (0)

/* Access to PHY bus with instance adjustment (addr_offset) */
#define PHYMOD_BUS_READ(pa_,r_,v_) phymod_bus_read(pa_,r_,v_)
#define PHYMOD_BUS_WRITE(pa_,r_,v_) phymod_bus_write(pa_,r_,v_)
#define PHYMOD_IS_WRITE_DISABLED(pa_,v_) phymod_is_write_disabled(pa_,v_)

extern int
phymod_acc_check(const phymod_access_t *pa);

extern int
phymod_bus_read(const phymod_access_t *pa, uint32_t reg, uint32_t *data);

extern int
phymod_bus_write(const phymod_access_t *pa, uint32_t reg, uint32_t data);

extern int
phymod_is_write_disabled(const phymod_access_t *pa, uint32_t *data);

extern int
phymod_mem_write(const phymod_access_t *pa, phymod_mem_type_t mem_type, uint32_t mem_index, uint32_t* data);

extern int
phymod_mem_read(const phymod_access_t *pa, phymod_mem_type_t mem_type, uint32_t mem_index, uint32_t* val);



#endif /* __PHYMOD_ACC_H__ */
