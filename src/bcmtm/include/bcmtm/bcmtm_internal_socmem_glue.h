/*! \file bcmtm_internal_socmem_glue.h
 *
 * File containing glue functions for Flexport and TDM library.
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

#ifndef BCMTM_INTERNAL_SOCMEM_GLUE_H
#define BCMTM_INTERNAL_SOCMEM_GLUE_H

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>

#define BCM56960_DEVICE_ID 0xb960


#define SOC_E_PARAM   SHR_E_PARAM
#define SOC_E_NONE    SHR_E_NONE
#define SOC_E_FAIL    SHR_E_FAIL
#define SOC_E_MEMORY  SHR_E_MEMORY


#define MEM_BLOCK_ALL       -1
#define MEM_BLOCK_ANY       -1
#define REG_PORT_ANY        -1
#define SOC_MAX_MEM_WORDS   BCMDRD_MAX_PT_WSIZE


#define COMPILER_64_SET(dst, src_hi, src_lo)                \
    ((dst) = (((uint64) ((uint32)(src_hi))) << 32) | ((uint64) ((uint32)(src_lo))))

#define COMPILER_64_COPY(dst, src)                \
    (dst = src)


#define COMPILER_64_TO_32_LO(dst, src)  ((dst) = (uint32) (src))
#define COMPILER_64_TO_32_HI(dst, src)  ((dst) = (uint32) ((src) >> 32))
#define COMPILER_64_HI(src)     ((uint32) ((src) >> 32))
#define COMPILER_64_LO(src)     ((uint32) (src))
#define COMPILER_64_ZERO(dst)       ((dst) = 0)
#define COMPILER_64_IS_ZERO(src)    ((src) == 0)

#define COMPILER_64_EQ(src1, src2)    ((src1) == (src2))
#define COMPILER_64_NE(src1, src2)    ((src1) != (src2))
#define COMPILER_64_LT(src1, src2)    ((src1) <  (src2))
#define COMPILER_64_LE(src1, src2)    ((src1) <= (src2))
#define COMPILER_64_GT(src1, src2)    ((src1) >  (src2))
#define COMPILER_64_GE(src1, src2)    ((src1) >= (src2))

#define COMPILER_64_ADD_64(dst, src)    ((dst) += (src))
#define COMPILER_64_ADD_32(dst, src)    ((dst) += (src))
#define COMPILER_64_SUB_64(dst, src)    ((dst) -= (src))
#define COMPILER_64_SUB_32(dst, src)    ((dst) -= (src))

#define COMPILER_64_SHL(dst, bits)    ((dst) <<= (bits))
#define COMPILER_64_SHR(dst, bits)    ((dst) >>= (bits))

#define COMPILER_64_AND(dst, src)    ((dst) &= (src))
#define COMPILER_64_OR(dst, src)    ((dst) |= (src))
#define COMPILER_64_XOR(dst, src)    ((dst) ^= (src))
#define COMPILER_64_NOT(dst)        ((dst) = ~(dst))

/*  Set up a mask of width bits offset lft_shft.  No error checking */
#define COMPILER_64_MASK_CREATE(dst, width, lft_shift)                  \
    do {                                                                \
        COMPILER_64_ALLONES(dst);                                       \
        COMPILER_64_SHR((dst), (64 - (width)));                         \
        COMPILER_64_SHL((dst), (lft_shift));                            \
    } while (0)

#define COMPILER_64_DELTA(src, last, new)\
        do { \
            COMPILER_64_ZERO(src);\
            COMPILER_64_ADD_64(src, new);\
            COMPILER_64_SUB_64(src, last);\
        } while(0)

#define COMPILER_64_BITSET(dst, n)              \
        do {                                    \
            uint64_t temp64;                      \
            COMPILER_64_SET(temp64, 0, 1);      \
            COMPILER_64_SHL(temp64, n);         \
            COMPILER_64_OR(dst, temp64);        \
        } while(0)

#define COMPILER_64_BITCLR(dst, n)              \
        do {                                    \
            uint64_t temp64;                      \
            COMPILER_64_SET(temp64, 0, 1);      \
            COMPILER_64_SHL(temp64, n);         \
            COMPILER_64_NOT(temp64);            \
            COMPILER_64_AND(dst, temp64);       \
        } while(0)


#define _SHR_PORT_ENCAP_IEEE              BCMTM_PORTENCAP_IEEE
#define _SHR_PORT_ENCAP_HIGIG             BCMTM_PORTENCAP_HIGIG
#define _SHR_PORT_ENCAP_HIGIG2            BCMTM_PORTENCAP_HIGIG2

#define _SHR_PORT_ENCAP_HIGIG2_L2         BCMTM_PORTENCAP_COUNT
#define _SHR_PORT_ENCAP_HIGIG2_LITE       BCMTM_PORTENCAP_COUNT
#define _SHR_PORT_ENCAP_HIGIG2_IP_GRE     BCMTM_PORTENCAP_COUNT


#define SOC_PBMP_ITER BCMDRD_PBMP_ITER
#define SOC_PBMP_MEMBER BCMDRD_PBMP_MEMBER
#define SOC_PBMP_ASSIGN BCMDRD_PBMP_ASSIGN
#define SOC_PBMP_NOT_NULL BCMDRD_PBMP_NOT_NULL
#define SOC_PBMP_IS_NULL BCMDRD_PBMP_IS_NULL
#define SOC_PBMP_PORT_ADD BCMDRD_PBMP_PORT_ADD
#define SOC_PBMP_PORT_REMOVE BCMDRD_PBMP_PORT_REMOVE
/* definition from SDK6/include/shared/pbmp.h  _SHR_PBMP_PORT_SET */
#define SOC_PBMP_PORT_SET(pbm, port)    ((pbm) = (1U << (port)))

#define NUM_PIPE(unit) bcmtm_num_pipe(unit)

#define soc_mem_entry_words(unit, mem) \
    bcmdrd_pt_entry_wsize(unit, (bcmdrd_sid_t)mem)

/*  use bit 31 to indicate the port argument is an instance id */
#define SOC_REG_ADDR_INSTANCE_BP         (31)
#define SOC_REG_ADDR_INSTANCE_MASK       (1<<SOC_REG_ADDR_INSTANCE_BP)

#define SOC_IF_ERROR_RETURN(_expr) \
    do {                                                \
        shr_error_t _rv = _expr;                        \
        if (SHR_FAILURE(_rv)) {                         \
            LOG_ERROR(BSL_LOG_MODULE,                   \
                      (BSL_META_U(0, "%s\n"),  \
                       shr_errmsg(_rv)));               \
            return _rv;                                 \
        }                                               \
    } while (0)

#define SAL_BOOT_BCMSIM (0)
#define SAL_BOOT_XGSSIM (0)
#define SAL_BOOT_QUICKTURN (0)

#define SOC_SWITCH_BYPASS_MODE_NONE             0
#define SOC_SWITCH_BYPASS_MODE_BALANCED         1
#define SOC_SWITCH_BYPASS_MODE_LOW              2


#define SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, mem, xpe, pipe_num) \
    bcmtm_pt_sid_unique_xpe_pipe(unit, mem, xpe, pipe_num)

#define SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, pipe_num) \
    bcmtm_pt_sid_unique_xpe_pipe(unit, mem, -1, pipe_num)

/*
 * Typedef aliases
 */
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;
typedef uint64_t uint64;

typedef int soc_mem_t;
typedef int soc_reg_t;
typedef int soc_field_t;


static inline bcmdrd_sid_t
bcmtm_sid_unique_xpe_pipe(int unit, bcmdrd_sid_t mem, int xpe, int pipe_num)
{
    return mem;
}


static inline bcmdrd_sid_t
bcmtm_sid_unique_pipe(int unit, bcmdrd_sid_t mem, int pipe_num)
{
    return mem;
}

static inline int
soc_cm_get_id(int dev,
              uint16_t *dev_id,
              uint8_t *rev_id)
{
    bcmdrd_dev_id_t *devid;
    bcmdrd_dev_t *drd_dev;

    drd_dev = bcmdrd_dev_get(dev);

    devid = &(drd_dev->id);

    *dev_id = (devid->device_id);
    *rev_id = (uint8_t)(devid->revision);

    return 0;
}



static inline int
soc_cm_get_id_driver(uint16_t dev_id,
                     uint8_t rev_id,
                     uint16_t *dev_id_driver,
                     uint8_t *rev_id_driver)
{
    return 0;
}

/*
 * Write internal memories
 */
extern int soc_mem_write(int unit,
                         soc_mem_t mem,
                         int copyno,
                         int index,
                         void *entry_data);

/*
 * Read internal memories
 */
extern int soc_mem_read(int unit,
                        soc_mem_t mem,
                        int copyno,
                        int index,
                        void *entry_data);



/*
 * Set a field in a SOC memory
 */
void soc_mem_field_set(int unit,
                       soc_mem_t mem,
                       uint32_t *entbuf,
                       soc_field_t field,
                       uint32_t *fldbuf);

/*
 * Get field value for a SOC memory buffer
 */
uint32_t * soc_mem_field_get(int unit,
                           soc_mem_t mem,
                           const uint32_t *entbuf,
                           soc_field_t field,
                           uint32_t *fldbuf);

/*
 * Set a <=32 bit field out of a memory entry
 */
void
soc_mem_field32_set(int unit, soc_mem_t mem, void *entbuf,
                    soc_field_t field, uint32 value);

/*
 * Read a per-port register
 */
int soc_reg_rawport_get(int unit,
                        soc_reg_t reg,
                        int port,
                        int index,
                        uint64_t *data);

/*
 * Write a per-port register
 */
int soc_reg_rawport_set(int unit,
                        soc_reg_t reg,
                        int port,
                        int index,
                        uint64_t data);


/*
 * Set a field in a SOC register buffer
 */
void soc_reg_field_set(int unit,
                       soc_reg_t reg,
                       uint32_t *regval,
                       soc_field_t field,
                       uint32_t value);

/*
 * Get a field in a SOC register buffer
 */
uint32_t soc_reg_field_get(int unit,
                           soc_reg_t reg,
                           uint32_t regval,
                           soc_field_t field);


/*
 * Set a field in a SOC register buffer
 */
void soc_reg64_field_set(int unit,
                         soc_reg_t reg,
                         uint64_t *regval,
                         soc_field_t field,
                         uint64_t value);

/*
 * Get a field in a SOC register buffer
 */
uint64_t soc_reg64_field_get(int unit,
                             soc_reg_t reg,
                             uint64_t regval,
                             soc_field_t field);


int soc_reg32_set(int unit,
                  soc_reg_t reg,
                  int port,
                  int index,
                  uint32_t data);

int soc_reg32_get(int unit,
                  soc_reg_t reg,
                  int port,
                  int index,
                  uint32_t *data);

int soc_reg64_get(int unit,
                  soc_reg_t reg,
                  int port,
                  int index,
                  uint64_t *data);

int soc_reg64_set(int unit,
                  soc_reg_t reg,
                  int port,
                  int index,
                  uint64_t data);

int soc_reg32_rawport_get(int unit, soc_reg_t reg,
                          int port, int index, uint32_t *data);

int soc_reg32_rawport_set(int unit, soc_reg_t reg,
                          int port, int index, uint32_t data);


#endif /* BCMTM_INTERNAL_SOCMEM_GLUE_H */
