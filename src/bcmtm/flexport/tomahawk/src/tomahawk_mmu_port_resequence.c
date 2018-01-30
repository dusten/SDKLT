/*! \file tomahawk_mmu_port_resequence.c
 *
 * MMU port sequence with flex port operations.
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

#include "../../main/flexport_top.h"




#if defined(BCM_TOMAHAWK_SUPPORT)
/*! @file tomahawk_mmu_port_resequence.c
 *  @brief
 */


/*! @fn int soc_tomahawk_mmu_set_mmu_to_phy_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the mmu port to physical port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk_mmu_set_mmu_to_phy_port_mapping(
    int unit,soc_port_resource_t *port_resource_t)
{

    int mmu_port;
    uint64 phy_port_new, dev_port_new;
    soc_reg_t reg;
    uint64 rval;
    int inst;

    COMPILER_64_SET(phy_port_new, 0, 0);
    COMPILER_64_SET(dev_port_new, 0, 0);

    mmu_port = port_resource_t->mmu_port;
    inst = mmu_port;


    /* When a physical port goes down, the port resource struct phy port number
     * becomes -1. That time, we need to invalidate the phy port to 'hff  When
     * a port is coming back up, the struct is populated correctly.
     */
    if (port_resource_t->physical_port == -1) {
        COMPILER_64_SET(phy_port_new, 0, TH_INVALID_PHY_PORT_NUMBER); /* All 1's */
    }
    else {
        COMPILER_64_SET(phy_port_new, 0, port_resource_t->physical_port);
        COMPILER_64_SET(dev_port_new, 0, port_resource_t->logical_port);
    }


    reg= MMU_PORT_TO_PHY_PORT_MAPPINGr;
    COMPILER_64_ZERO(rval);
    soc_reg64_field_set(unit, reg, &rval, PHY_PORTf, phy_port_new);
    /* BT:IPORT, AT: SINGLE, Block ID: MMU_GLOBAL */
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    if (port_resource_t->physical_port != -1) {
        reg= MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
        COMPILER_64_ZERO(rval);
        soc_reg64_field_set(unit, reg, &rval, DEVICE_PORTf, dev_port_new);
        /* BT:IPORT, AT: SINGLE, Block ID: MMU_GLOBAL */
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;

}


/*! @fn int soc_tomahawk_mmu_vbs_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int set_val)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param set_val Value to be set to the port flush register
 *  @brief API to Set the Port Flush register
 */
int
soc_tomahawk_mmu_vbs_port_flush(int unit,soc_port_resource_t *port_resource_t,
                                 uint64 set_val)
{


    soc_reg_t reg;
    uint64 rval;
    uint64 enable_val;
    int mmu_port, lcl_mmu_port;
    int pipe_number, inst;

    reg = Q_SCHED_PORT_FLUSHr;
    pipe_number = port_resource_t->pipe;
    mmu_port = port_resource_t->mmu_port;
    lcl_mmu_port = mmu_port % TH_MMU_PORT_PIPE_OFFSET;

    /* READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    COMPILER_64_ZERO(rval);

    inst = pipe_number;
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                          inst, 0, &rval));
    enable_val = soc_reg64_field_get(unit, reg,
                                  rval, ENABLEf);
    if (COMPILER_64_IS_ZERO(set_val) == 1) {
        COMPILER_64_BITCLR(enable_val, lcl_mmu_port);
    }
    else {
        COMPILER_64_BITSET(enable_val, lcl_mmu_port);
    }
    soc_reg64_field_set(unit, reg, &rval, ENABLEf, enable_val);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));


    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_mmu_rqe_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int set_val)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param set_val Value to be set to the snapshot register
 *  @brief API to Set the RQE Snapshot register to Flush out packets in the
 *         RQE Replication FIFO
 */
int
soc_tomahawk_mmu_rqe_port_flush(int unit,soc_port_resource_t *port_resource_t,
                                 uint64 set_val)
{


    soc_reg_t reg;
    uint64 rval;
    uint64 enable_val;
    int count=0;
    uint64 rd_val;
    int pipe_number, inst;

    reg = Q_SCHED_RQE_SNAPSHOTr;
    pipe_number = port_resource_t->pipe;


    inst = pipe_number;

    enable_val = set_val;
    COMPILER_64_ZERO(rval);
    soc_reg64_field_set(unit, reg, &rval, INITIATEf, enable_val);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    while (1) {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                              inst, 0, &rval));
        rd_val = soc_reg64_field_get(unit, reg,
                                     rval, INITIATEf);
        if (COMPILER_64_IS_ZERO(rd_val)) {
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 10000) {
            return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_mmu_mtro_port_flush(int unit,
 *              soc_port_resource_t *port_resource_t, int set_val)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param set_val Value to be set to the snapshot register
 *  @brief This function is used to set the MTRO Port Flush register to
 *         ignore shaper information on a port during flexport operation
 */
int
soc_tomahawk_mmu_mtro_port_flush(int unit,soc_port_resource_t *port_resource_t,
                                  uint64 set_val)
{


    soc_reg_t reg;
    uint64 rval;
    int mmu_port, lcl_mmu_port;
    int pipe_number, inst;
    uint64 enable_val;

    reg = MTRO_PORT_ENTITY_DISABLEr;
    pipe_number = port_resource_t->pipe;
    mmu_port = port_resource_t->mmu_port;
    lcl_mmu_port = mmu_port % TH_MMU_PORT_PIPE_OFFSET;

    /* READ MODIFY WRITE IN SW ... Hence get
       Register Value and Then Write ..  */
    inst = pipe_number;
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                          inst, 0, &rval));
    enable_val = soc_reg64_field_get(unit, reg,
                                  rval, METERING_DISABLEf);
    if (COMPILER_64_IS_ZERO(set_val) == 1) {
        COMPILER_64_BITCLR(enable_val, lcl_mmu_port);
    }
    else {
        COMPILER_64_BITSET(enable_val, lcl_mmu_port);
    }
    COMPILER_64_ZERO(rval);
    soc_reg64_field_set(unit, reg, &rval, METERING_DISABLEf, enable_val);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK_SUPPORT */
