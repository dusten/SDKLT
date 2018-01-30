/*! \file ser_bd.h
 *
 * APIs of SER sub-component can read or write H/W by BD routine.
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

#ifndef SER_BD_H
#define SER_BD_H

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmbd/bcmbd_sbusdma.h>


/*******************************************************************************
 * Defines
 */
/*!
  * \brief register is belonged to port or soft port register
  */
#define PT_IS_PORT_OR_SW_PORT_REG(unit, sid) \
    (bcmdrd_pt_is_port_reg(unit, sid) || bcmdrd_pt_is_soft_port_reg(unit, sid))

/*!
 * \brief Batch DMA read memory table.
 *
 * \param [in] unit                  Logical device id.
 * \param [in] sid                   SID.
 * \param [in] tbl_inst              table instance.
 * \param [in] start_ports           First ports scanned by DMA mode.
 * \param [in] port_ranges           Ports ranges need to be scanned.
 * \param [in] start_indexes         first indexes scanned by DMA mode.
 * \param [in] index_ranges          ranges need to be scanned.
 * \param [in] work_num              Work number.
 * \param [in] buf_paddr             DMA buffer phsical address.
 * \param [in] is_port_reg           SID is port register.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_batch_sbusdma_read(int unit, bcmdrd_sid_t sid, int tbl_inst,
                              int *start_ports, int *port_ranges,
                              int *start_indexes, int *index_ranges, int work_num,
                              uint64_t buf_paddr, bool is_port_reg);

/*!
 * \brief Batch DMA write memory table.
 *
 * \param [in] unit                   Logical device id.
 * \param [in] sid                    SID.
 * \param [in] tbl_inst               table instance.
 * \param [in] start_ports            First ports written by DMA mode.
 * \param [in] port_ranges            Ports ranges need to be written.
 * \param [in] start_indexes          first indexes written by DMA mode.
 * \param [in] index_ranges           ranges need to be written.
 * \param [in] work_num               Work number.
 * \param [in] buf_paddr              DMA buffer phsical address.
 * \param [in] is_port_reg            SID is port register.
 * \param [in] work_flags             Such as: SBUSDMA_WF_WRITE_CMD etc.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_batch_sbusdma_write(int unit, bcmdrd_sid_t sid, int tbl_inst,
                              int *start_ports, int *port_ranges,
                              int *start_indexes, int *index_ranges, int work_num,
                              uint64_t buf_paddr, bool is_port_reg, uint32_t work_flags);

/*!
 * \brief read or write controlling register or memory
 *
 *
 * \param [in] unit Logical device id
 * \param [in] bcmptm_ser_ctrl_reg_info_t information of controlling register or memory
 * \param [in] reg_num number of controlling register or memory
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_ctrl_reg_operate(int unit,
                            bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info,
                            int reg_num);
/*!
 * \brief Get block number of SID
 *
 * \param [in] unit             Logical device id
 * \param [in] sid              SID (can be INVALIDm)
 * \param [in] tbl_inst         Table instance
 * \param [in] blk_type         block type (can be -1)
 * \param [out] blk_num         block number
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_blknum_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                      int blk_type, int *blk_num);

/*!
 * \brief Get instance id of status_reg which is per port or per PM
 *
 * \param [in] unit            Logical device id
 * \param [in] top_rb_info     ser route information
 * \param [in] status_reg      status register SID
 * \param [out] begin_inst_id  begin instance id
 * \param [out] end_inst_id    end instance id
 *
 * \n For example, CLMAC_TX_CDC_ECC_STATUSr is per port register, if top_rb_info->blk_inst is 0,
 * \n begin_inst_id will be 0, and end_inst_id will be 3. if top_rb_info->blk_inst is 1,
 * \n begin_inst_id will be 4, and end_inst_id will be 7.
 *
 * \n CLPORT_ECC_CONTROLr is per PM register, if  top_rb_info->blk_inst is 0,
 * \n begin_inst_id will be 0, and end_inst_id will be 0.
 * \n if  top_rb_info->blk_inst is 1,
 * \n begin_inst_id will be 1, and end_inst_id will be 1.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_non_mmu_status_reg_inst_get(int unit,
                                       bcmptm_ser_top_level_route_info_t *top_rb_info,
                                       bcmdrd_sid_t status_reg,
                                       int *begin_inst_id, int *end_inst_id);
/*!
 * \brief Get address of appointed memory table
 *
 * \param [in] unit          Logical device id
 * \param [in] sid           SID of memory
 * \param [in] tbl_inst      instance of memory
 * \param [in] index         index of memory
 * \param [out] addr         address of SID
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_pt_addr_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                       int index, uint32_t *addr);


#endif /* SER_BD_H */
