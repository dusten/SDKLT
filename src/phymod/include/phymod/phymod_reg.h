/*
 * $Id: phymod_reg.h,v 1.15 2013/05/04 05:50:22 zhengwan Exp $
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

#ifndef __PHYMOD_REG_H__
#define __PHYMOD_REG_H__

#include <phymod/phymod_system.h>

/*
 * Generic 32-bit PHY register layout:
 *
 * [31:28] - Access method
 * [27:24] - Flags for access method
 * [23:0]  - Register address
 *
 *
 * Access method 0 (Raw)
 *
 * For clause 22 access, register address is in bits [4:0]. For
 * clause 45 access, register address is in bits [15:0] and
 * DEVAD is in bits [20:16].
 *
 * 
 * Access method 1 (Broadcom Gigabit/Fast Ethernet shadow)
 *
 * If register is shadowed (register 18h, 1Ch, etc.) then shadow
 * number is in bits [15:8] and base register is in bits [4:0].
 * If register is an expansion register (registers 15h/17h), then
 * register address is in bits [23:8] and bits [4:0] is 15h.
 * If register is a clause 45 register (registers 0Dh/0Eh), then
 * clause 45 device type is in  bits [27:24], clause 45 register
 * address is in bits [23:8] and bits [4:0] is 0Eh.
 *
 *
 * Access method 2 (Broadcom Gigabit/1000BASE-X registers)
 *
 * Base address is in bits [4:0]. The 1000BASE-X registers will
 * be mapped into the IEEE space before register is accessed.
 *
 *
 * Access method 3 (Internal SerDes PHY block-based)
 *
 * Block address is in bits [23:8] (will be written to register.
 * 1Fh). Base address is in bits [4:0]. For clause 45 devices
 * the DEVAD is stored in the flags field (bits [26:24]).
 *
 *
 * Access method 4 (Internal XAUI/SerDes PHY with IEEE-map)
 *
 * Identical to access method 3, but supports the following
 * access flag instead of clause 45 DEVAD:
 *
 * Bit 3 - If set, map SerDes registers before reading IEEE 
 *         register. otherwise map XAUI registers before 
 *         reading IEEE register.
 *         
 *
 * Access method 5 (Internal SerDes PHY clause 45 with AER)
 *
 * Clause 45 address is in bits [15:0] and DEVAD in bits [19:16].
 * If a register is not duplicated for each XAUI lane, the actual
 * number of copies (1 or 2) will be stored in bits [22:20].
 * If clause 45 access is not supported the access will be done
 * using clause 22 block-mapped access.
 *
 * This method supports the following flags:
 *
 * Bit 3 - If set, force write to lane specified in flag
 *         bits [2:0].
 *
 * Bit 2 - If set, broadcast writes to all lanes if not in
 *         independent lane mode (aka. serdes mode).
 *         Ignored if flag bit 3 is set (force lane).
 *
 * In addition to the standard flags, bit 23 is used as a
 * write-only flag.
 *
 *
 * Access method 6 (Broadcom 10-Gigabit registers)
 *
 * Standard clause 45 access with additional support for Gigabit
 * shadow and extension register. Also supports multiple PHYs
 * (each with multiple clause 45 device types) for a single PHY
 * address.
 * If register is a standard clause 45 register, then clause 45
 * device type is in bits [20:16] and register address is in
 * bits [15:0].
 * If register is shadowed (register 18h, 1Ch, etc.) then shadow
 * number is in bits [15:8] and base register is in bits [4:0].
 * If register is an expansion register (registers 15h/17h), then
 * register address is in bits [23:8] and bits [4:0] is 15h.
 *
 * This method supports the following flags:
 *
 * Bit 3 - Use shadow/extension register access.
 *         
 * Flag bits [2:0] may be used to select an alternative PHY device.
 */

/* Register layout */
#define PHYMOD_REG_ACCESS_METHOD_SHIFT          28
#define PHYMOD_REG_ACCESS_FLAGS_SHIFT           24
#define PHYMOD_INTERFACE_SIDE_SHIFT             31
/* Access methods */
#define PHYMOD_REG_ACC_RAW                      LSHIFT32(0, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_BRCM_SHADOW              LSHIFT32(1, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_BRCM_1000X               LSHIFT32(2, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_XGS_IBLK                 LSHIFT32(3, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_XAUI_IBLK                LSHIFT32(4, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_AER_IBLK                 LSHIFT32(5, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_BRCM_XE                  LSHIFT32(6, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_TSC_IBLK                 LSHIFT32(7, PHYMOD_REG_ACCESS_METHOD_SHIFT)
#define PHYMOD_REG_ACC_BRCM_RDB                 LSHIFT32(8, PHYMOD_REG_ACCESS_METHOD_SHIFT)

#define PHYMOD_REG_ACCESS_METHOD(_r)            ((_r) & LSHIFT32(0xf, PHYMOD_REG_ACCESS_METHOD_SHIFT))

#define PHYMOD_REG_IBLK_DEVAD(_r)               (((_r) >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7)

/* Spacing between elements in register arrays */
#define PHYMOD_REG_ACC_XGS_IBLK_STEP            0x1000
#define PHYMOD_REG_ACC_XAUI_IBLK_STEP           0x1000
#define PHYMOD_REG_ACC_AER_IBLK_STEP            0x10

/* Flags for PHYMOD_REG_ACC_XAUI_IBLK method */
#define PHYMOD_REG_ACC_XAUI_IBLK_CL22           LSHIFT32(0x8, PHYMOD_REG_ACCESS_FLAGS_SHIFT)

/* Flags for PHYMOD_REG_ACC_AER_IBLK method */
#define PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE      LSHIFT32(0x8, PHYMOD_REG_ACCESS_FLAGS_SHIFT)
#define PHYMOD_REG_ACC_AER_IBLK_FORCE_MASK      LSHIFT32(0x7, PHYMOD_REG_ACCESS_FLAGS_SHIFT)
#define PHYMOD_REG_ACC_AER_IBLK_WR_ONLY         LSHIFT32(1, 23)

/* Flags for PHYMOD_REG_ACC_BRCM_XE method */
#define PHYMOD_REG_ACC_BRCM_XE_SHADOW           LSHIFT32(0x8, PHYMOD_REG_ACCESS_FLAGS_SHIFT)

/* Flags for PHYMOD_REG_ACC_TSC_IBLK method */
#define PHYMOD_REG_ACC_TSC_IBLK_FORCE_LANE      LSHIFT32(0x8, PHYMOD_REG_ACCESS_FLAGS_SHIFT)
#define PHYMOD_REG_ACC_TSC_IBLK_BCAST           LSHIFT32(0x4, PHYMOD_REG_ACCESS_FLAGS_SHIFT)
#define PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY         LSHIFT32(1, 23)

extern int
phymod_reg_read(phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_reg_write(phymod_access_t *pa, uint32_t addr, uint32_t data);

#endif /* __PHYMOD_REG_H__ */
