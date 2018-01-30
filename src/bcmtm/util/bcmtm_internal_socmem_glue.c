/*! \file bcmtm_internal_socmem_glue.c
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_internal_socmem_glue.h>

#include <bcmbd/chip/bcm56960_a0_acc.h>

/*******************************************************************************
 * Public functions
 */
/*
 * Read internal memories
 */
int
soc_mem_read(int unit, soc_mem_t mem, int copyno, int index, void *data)
{
    return bcmtm_pt_iread(unit, mem, -1, 0, index, (uint32_t *)data);
}


/*
 * Write internal memories
 */
int
soc_mem_write(int unit, soc_mem_t mem, int copyno, int index, void *data)
{
    return bcmtm_pt_iwrite(unit, mem, -1, 0, index, (uint32_t *)data);
}



/*
 * Set a field in a SOC memory
 */
void
soc_mem_field_set(int unit,
                  soc_mem_t mem,
                  uint32_t *entbuf,
                  soc_field_t field,
                  uint32_t *fldbuf)
{
    bcmtm_pt_field_set(unit, mem, field, entbuf, fldbuf);
    return;
}


/*
 * Get field value for a SOC memory buffer
 */
uint32_t *
soc_mem_field_get(int unit,
                  soc_mem_t mem,
                  const uint32_t *entbuf,
                  soc_field_t field,
                  uint32_t *fldbuf)
{
    size_t size;
    uint32_t entbuf_l[SOC_MAX_MEM_WORDS], fldbuf_l[SOC_MAX_MEM_WORDS];

    size = bcmdrd_pt_entry_size(unit, mem);
    sal_memcpy(entbuf_l, entbuf, size);

    (void)bcmtm_pt_field_get(unit, mem, field, entbuf_l, fldbuf_l);

    sal_memcpy(fldbuf, fldbuf_l, size);

    return fldbuf;
}


/*
 * Function:     soc_mem_field32_set
 * Purpose:      Set a <=32 bit field out of a memory entry
 * Returns:      void
 */
void
soc_mem_field32_set(int unit, soc_mem_t mem, void *entbuf,
                    soc_field_t field, uint32 value)
{
    soc_mem_field_set(unit, mem, entbuf, field, &value);
}


/*
 * Read a per-port register
 */
int
soc_reg_rawport_get(int unit,
                    soc_reg_t reg,
                    int port,
                    int index,
                    uint64_t *data)
{
    uint32_t reg_data[2] = {0};
    uint64_t data_l = 0;

    SHR_FUNC_ENTER(unit);

    if ((port != REG_PORT_ANY) && (port & SOC_REG_ADDR_INSTANCE_MASK)) {
        port &= ~SOC_REG_ADDR_INSTANCE_MASK;
    }

    SHR_IF_ERR_EXIT(
        bcmtm_pt_iread(unit, reg, port, -1, index, reg_data));

    COMPILER_64_SET(data_l, reg_data[1], reg_data[0]);

    (*data) = data_l;

exit:
    SHR_FUNC_EXIT();
}


/*
 * Write a per-port register
 */
int
soc_reg_rawport_set(int unit,
                    soc_reg_t reg,
                    int port,
                    int index,
                    uint64_t data)
{
    uint32_t reg_data[2] = {0};

    SHR_FUNC_ENTER(unit);

    if ((port != REG_PORT_ANY) && (port & SOC_REG_ADDR_INSTANCE_MASK)) {
        port &= ~SOC_REG_ADDR_INSTANCE_MASK;
    }

    reg_data[0] = COMPILER_64_LO(data);
    reg_data[1] = COMPILER_64_HI(data);

    
    switch (reg) {
    case ASF_IPORT_CFGr:
    case ASF_EPORT_CFGr:
    case ASF_CREDIT_THRESH_HIr:
        return SHR_E_NONE;
        break;
    default:
        break;
    }

    SHR_IF_ERR_EXIT(
        bcmtm_pt_iwrite(unit, reg, port, -1, index, reg_data));

exit:
    SHR_FUNC_EXIT();
}


int
soc_reg32_rawport_get(int unit, soc_reg_t reg,
                      int port, int index, uint32_t *data)
{
    int rv;
    uint64_t d64;

    rv = soc_reg_rawport_get(unit, reg, port, index, &d64);
    *data = COMPILER_64_LO(d64);

    return rv;
}


int
soc_reg32_rawport_set(int unit, soc_reg_t reg,
                      int port, int index, uint32_t data)
{
    uint64_t d64;

    COMPILER_64_SET(d64, 0, data);
    return soc_reg_rawport_set(unit, reg, port, index, d64);
}



int
soc_reg32_set(int unit,
              soc_reg_t reg,
              int port,
              int index,
              uint32_t data)
{
    int inst = -1;
    SHR_FUNC_ENTER(unit);

    if ((port != REG_PORT_ANY) && (port & SOC_REG_ADDR_INSTANCE_MASK)) {
        port &= ~SOC_REG_ADDR_INSTANCE_MASK;
    }

    SHR_IF_ERR_EXIT(
        bcmtm_pt_iwrite(unit, reg, port, inst, index, &data));

exit:
    SHR_FUNC_EXIT();
}


int
soc_reg32_get(int unit,
              soc_reg_t reg,
              int port,
              int index,
              uint32_t *data)
{
    int inst = -1;
    uint32_t buf[1] = {0};

    SHR_FUNC_ENTER(unit);

    if ((port != REG_PORT_ANY) && (port & SOC_REG_ADDR_INSTANCE_MASK)) {
        port &= ~SOC_REG_ADDR_INSTANCE_MASK;
    }

    SHR_IF_ERR_EXIT(
        bcmtm_pt_iread(unit, reg, port, inst, index, buf));

    *data = buf[0];

exit:
    SHR_FUNC_EXIT();
}



int
soc_reg64_set(int unit,
              soc_reg_t reg,
              int port,
              int index,
              uint64_t data)
{
    uint32_t reg_data[2] = {0};
    int inst = -1;

    SHR_FUNC_ENTER(unit);

    if ((port != REG_PORT_ANY) && (port & SOC_REG_ADDR_INSTANCE_MASK)) {
        inst = port & ~SOC_REG_ADDR_INSTANCE_MASK;
        port = REG_PORT_ANY;
    }

    reg_data[0] = COMPILER_64_LO(data);
    reg_data[1] = COMPILER_64_HI(data);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_iwrite(unit, reg, port, inst, index, reg_data));

exit:
    SHR_FUNC_EXIT();
}


int
soc_reg64_get(int unit,
              soc_reg_t reg,
              int port,
              int index,
              uint64_t *data)
{
    uint32_t reg_data[2] = {0};
    uint64_t data_l = 0;
    int inst = -1;

    SHR_FUNC_ENTER(unit);

    if ((port != REG_PORT_ANY) && (port & SOC_REG_ADDR_INSTANCE_MASK)) {
        inst = port & ~SOC_REG_ADDR_INSTANCE_MASK;
        port = REG_PORT_ANY;
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pt_iread(unit, reg, port, inst, index, reg_data));

    COMPILER_64_SET(data_l, reg_data[1], reg_data[0]);

    (*data) = data_l;

exit:
    SHR_FUNC_EXIT();
}


/*
 * Set a field in a SOC register buffer
 */
void
soc_reg_field_set(int unit,
                  soc_reg_t reg,
                  uint32_t *regval,
                  soc_field_t field,
                  uint32_t value)
{
    uint32_t vbuf[1];
    vbuf[0] = value;

    (void)bcmtm_pt_field_set(unit, reg, field, regval, vbuf);

    return;
}


/*
 * Get a field in a SOC register buffer
 */
uint32_t
soc_reg_field_get(int unit, soc_reg_t reg, uint32_t regval, soc_field_t field)
{
    uint32_t fldval[1] = {0};
    uint32_t regbuf[1] = {0};

    regbuf[0] = regval;

    (void)bcmtm_pt_field_get(unit, reg, field, regbuf, fldval);

    return fldval[0];
}


/*
 * Set a field in a SOC register buffer
 */
void
soc_reg64_field_set(int unit,
                    soc_reg_t reg,
                    uint64_t *regval,
                    soc_field_t field,
                    uint64_t value)
{
    uint32_t reg_data[2] = {0};
    uint32_t field_data[2] = {0};
    uint64_t data_l = 0;

    field_data[0] = (uint32_t)value;
    field_data[1] = (uint32_t)(value >> 32);

    reg_data[0] = (uint32_t)(*regval);
    reg_data[1] = (uint32_t)(*regval >> 32);

    (void)bcmtm_pt_field_set(unit, reg, field, reg_data, field_data);

    COMPILER_64_SET(data_l, reg_data[1], reg_data[0]);

    (*regval) = data_l;
    return;
}

/*
 * Get a field in a SOC register buffer
 */
uint64_t
soc_reg64_field_get(int unit,
                    soc_reg_t reg,
                    uint64_t regval,
                    soc_field_t field)
{
    uint32_t reg_data[2] = {0};
    uint32_t field_data[2] = {0};
    uint64_t fldval_64 = {0};

    reg_data[0] = (uint32_t)regval;
    reg_data[1] = (uint32_t)(regval >> 32);

    (void)bcmtm_pt_field_get(unit, reg, field, reg_data, field_data);

    COMPILER_64_SET(fldval_64, field_data[1], field_data[0]);

    return fldval_64;
}
