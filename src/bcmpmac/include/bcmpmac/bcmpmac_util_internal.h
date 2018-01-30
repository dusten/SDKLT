/*! \file bcmpmac_util_internal.h
 *
 * Utilities of BCM PortMAC.
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

#ifndef BCMPMAC_UTIL_INTERNAL_H
#define BCMPMAC_UTIL_INTERNAL_H

#include <bcmpmac/bcmpmac_acc.h>


/*******************************************************************************
 * Utilities for the PMAC driver.
 */

/*!
 * \brief BCMPMAC port mode map.
 *
 * This structure is used to define the supported port lane map and specify the
 * corresponding PM port mode.
 */
typedef struct bcmpmac_port_mode_map_s {

    /*! Port lane map. */
    uint32_t lane_map[BCMPMAC_NUM_PORTS_PER_PM_MAX];

    /*! Port mode value. */
    uint32_t port_mode;

} bcmpmac_port_mode_map_t;

/*!
 * \brief BCMPMAC function type for the string-based API.
 */
typedef int
(*bcmpmac_drv_op_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t param);

/*!
 * \brief BCMPMAC port mode map.
 *
 * This structure is used to define the operation for the string-based APIs.
 */
typedef struct bcmpmac_drv_op_hdl_s {

    /*! Operation name. */
    char *name;

    /*! Operation handler. */
    bcmpmac_drv_op_f func;

} bcmpmac_drv_op_hdl_t;

/*!
 * \brief Retrieve the PM port mode value for the given port lane maps.
 *
 * Get the PM port mode value \c mval by searching the PM mode mapping \c map
 * for the given port lane maps \c lane_map.
 *
 * \param [in] modes Port mode map.
 * \param [in] mode_num The entry number of \c modes.
 * \param [in] lane_map Port lane maps.
 * \param [out] mval PM port mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM No PM port mode is defined for the given lane maps.
 */
int
bcmpmac_port_mode_get(const bcmpmac_port_mode_map_t *modes, int mode_num,
                      uint32_t *lane_map, uint32_t *mval);

/*!
 * \brief Retrieve the function pointer of the given operation name.
 *
 * Get the function pointer by searching the operation list \c ops for the
 * given operation string \c op_name.
 *
 * \param [in] ops Supported operations.
 * \param [in] op_num The number of the supported operations.
 * \param [in] op_name The operation name to search.
 *
 * \return The function pointer of the given operation name or NULL when not
 *         found.
 */
bcmpmac_drv_op_f
bcmpmac_drv_op_func_get(const bcmpmac_drv_op_hdl_t *ops, int op_num,
                        const char *op_name);

/*******************************************************************************
 * Register access functions.
 */

/*!
 * \brief BCMPMAC read function
 *
 * Read data from the given register
 *
 * \param [in] pm_acc PortMAC access info
 * \param [in] pm_pport The port index in the block
 * \param [in] address Reg/Mem offset
 * \param [in] idx Entry index
 * \param [in] size Data size in byte
 * \param [out] data Data buffer
 *
 * \retval 0 No errors
 */
extern int
bcmpmac_read(bcmpmac_access_t *pm_acc, bcmpmac_pport_t pm_pport,
             uint32_t address, uint32_t idx, size_t size, uint32_t *data);

/*!
 * \brief Write register data
 *
 * Write data to the given register
 *
 * \param [in] pm_acc PortMAC access info
 * \param [in] pm_pport The port index in the block
 * \param [in] address Reg/Mem offset
 * \param [in] idx Entry index
 * \param [in] size Data size in byte
 * \param [in] data Data buffer
 *
 * \retval 0 No errors
 */
extern int
bcmpmac_write(bcmpmac_access_t *pm_acc, bcmpmac_pport_t pm_pport,
              uint32_t address, uint32_t idx, size_t size, uint32_t *data);


/*******************************************************************************
 * Filed operations for little endian word order (normal)
 */

/*!
 * \brief Extract multi-word field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 * \param [out] fbuf buffer where to store extracted field value
 *
 * \return Pointer to extracted field value.
 */
extern uint32_t *
bcmpmac_field_get(const uint32_t *entbuf, int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Assign multi-word field value in multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fbuf buffer with new field value
 *
 * \return Nothing.
 */
extern void
bcmpmac_field_set(uint32_t *entbuf, int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Extract field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 *
 * \return Extracted field value.
 */
extern uint32_t
bcmpmac_field32_get(const uint32_t *entbuf, int sbit, int ebit);

/*!
 * \brief Assign field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fval the value of the field to assign
 *
 * \return Nothing.
 */
extern void
bcmpmac_field32_set(uint32_t *entbuf, int sbit, int ebit, uint32_t fval);


/*******************************************************************************
 * Filed operations for big endian word order (some exceptions)
 */

/*!
 * \brief Extract multi-word field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 * \param [out] fbuf buffer where to store extracted field value
 *
 * \return Pointer to extracted field value.
 */
extern uint32_t *
bcmpmac_field_be_get(const uint32_t *entbuf, int wsize,
                     int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Assign multi-word field value in multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fbuf buffer with new field value
 *
 * \return Nothing.
 */
extern void
bcmpmac_field_be_set(uint32_t *entbuf, int wsize,
                     int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Extract field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 *
 * \return Extracted field value.
 */
extern uint32_t
bcmpmac_field32_be_get(const uint32_t *entbuf, int wsize,
                       int sbit, int ebit);

/*!
 * \brief Assign field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fval the value of the field to assign
 *
 * \return Nothing.
 */
extern void
bcmpmac_field32_be_set(uint32_t *entbuf, int wsize,
                       int sbit, int ebit, uint32_t fval);

#endif /* BCMPMAC_UTIL_INTERNAL_H */
