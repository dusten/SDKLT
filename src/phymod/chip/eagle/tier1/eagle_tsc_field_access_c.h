/**************************************************************************************
**************************************************************************************
*  File Name     :  eagle_tsc_field_access.c                                        *
*  Created On    :  29/04/2013                                                       *
*  Created By    :  Kiran Divakar                                                    *
*  Description   :  APIs to access Serdes IP Registers and Reg fields                *
*  Revision      :  $Id: eagle_tsc_field_access.c 1123 2015-09-03 22:36:34Z eroes $ *
*                                                                                    *
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
* 
*                                                               *
*  No portions of this material may be reproduced in any form without                *
*  the written permission of:                                                        *
*      Broadcom Corporation                                                          *
*      5300 California Avenue                                                        *
*      Irvine, CA  92617                                                             *
*                                                                                    *
*  All information contained in this document is Broadcom Corporation                *
*  company private proprietary, and trade secret.                                    *
 */

/** @file eagle_tsc_field_access.c
 * Registers and field access
 */

#include "eagle_tsc_field_access.h"
#include "eagle_tsc_dependencies.h"
#include "eagle_tsc_ipconfig.h"
#include "eagle_tsc_internal.h"

err_code_t _eagle_tsc_pmd_rdt_field( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint16_t *val_p){
#ifdef ATE_LOG
	EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n", addr, (((uint16_t)(0xFFFF << shift_left)) >> shift_right) << (shift_right - shift_left), shift_right - shift_left));
#endif

	EFUN(eagle_tsc_pmd_rdt_reg(pa,addr, val_p));
	*val_p <<= shift_left;                  /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
	*val_p >>= shift_right;                 /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

	return ERR_CODE_NONE;
}

err_code_t _eagle_tsc_pmd_rdt_field_signed( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int16_t *val_p){
#ifdef ATE_LOG
	EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n", addr, (((uint16_t)(0xFFFF << shift_left)) >> shift_right) << (shift_right - shift_left), shift_right - shift_left));
#endif

	EFUN(eagle_tsc_pmd_rdt_reg(pa,addr, (uint16_t*)val_p));
	*val_p <<= shift_left;                  /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
	*val_p >>= shift_right;                 /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

	return ERR_CODE_NONE;
}

/*-------------------------------*/
/* Byte Write and Read Functions */
/*-------------------------------*/

err_code_t _eagle_tsc_pmd_mwr_reg_byte( const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val){

	EFUN(eagle_tsc_pmd_mwr_reg(pa, addr, mask, lsb, (uint16_t)val));

	return ERR_CODE_NONE;
}

err_code_t _eagle_tsc_pmd_rdt_field_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, uint8_t *val8_p){

	uint16_t val;

#ifdef ATE_LOG
	EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n", addr, (((uint16_t)(0xFFFF << shift_left)) >> shift_right) << (shift_right - shift_left), shift_right - shift_left));
#endif

	EFUN(eagle_tsc_pmd_rdt_reg(pa,addr, &val));
	val <<= shift_left;             /* Move the MSB to the left most      [shift_left  = (15-msb)]     */
	val >>= shift_right;            /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

	*val8_p = (uint8_t)val;

	return ERR_CODE_NONE;
}

err_code_t _eagle_tsc_pmd_rdt_field_signed_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, int8_t *val8_p){

	int16_t val;

#ifdef ATE_LOG
	EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n", addr, (((uint16_t)(0xFFFF << shift_left)) >> shift_right) << (shift_right - shift_left), shift_right - shift_left));
#endif

	EFUN(eagle_tsc_pmd_rdt_reg(pa,addr, (uint16_t*)&val));
	val <<= shift_left;             /* Move the sign bit to the left most [shift_left  = (15-msb)]     */
	val >>= shift_right;            /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

	*val8_p = (int8_t)val;

	return ERR_CODE_NONE;
}

/*-------------------------------*/
/* MDIO specific Misc functions  */
/*-------------------------------*/

err_code_t eagle_tsc_pmd_mdio_mwr_reg( const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val){

	/* Can't use "wrc_mdio_maskdata(~mask)" to avoid circular function calls */
#if              defined(MDIO_MMDSEL_AER_COM_MDIO_MASKDATA)
	EFUN(eagle_tsc_pmd_wr_reg(pa, MDIO_MMDSEL_AER_COM_MDIO_MASKDATA, ~mask));
#elif            defined(DIG_COM_MASKDATA_REG)
	EFUN(eagle_tsc_pmd_wr_reg(pa, DIG_COM_MASKDATA_REG, ~mask));
#else
#error "SERDES_MDIO_MASKED_WRITE enabled without mask register specified."
#endif
	EFUN(eagle_tsc_pmd_wr_reg(pa, addr, val << lsb));


	return ERR_CODE_NONE;
}


/*-------------------------------*/
/* EVAL specific functions  */
/*-------------------------------*/

uint16_t _eagle_tsc_pmd_rde_reg( const phymod_access_t *pa, uint16_t addr, err_code_t *err_code_p){
	uint16_t data;

	EPFUN(eagle_tsc_pmd_rdt_reg(pa,addr, &data));
	return data;
}

uint16_t _eagle_tsc_pmd_rde_field( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p){
	uint16_t data;

#ifdef ATE_LOG
	EFUN_PRINTF(("// ATE_LOG Addr x%04x : MASK x%04x : Right Shift %d\n", addr, (((uint16_t)(0xFFFF << shift_left)) >> shift_right) << (shift_right - shift_left), shift_right - shift_left));
#endif
	EPSTM(data = _eagle_tsc_pmd_rde_reg( pa, addr, err_code_p));

	data <<= shift_left;            /* Move the sign bit to the left most [shift_left  = (15-msb)] */
	data >>= shift_right;           /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

	return data;
}

int16_t _eagle_tsc_pmd_rde_field_signed( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p){
	int16_t data;

#ifdef ATE_LOG
	EFUN_PRINTF(("// ATE_LOG Addr x%04x : Signed MASK x%04x : Right Shift %d\n", addr, (((uint16_t)(0xFFFF << shift_left)) >> shift_right) << (shift_right - shift_left), shift_right - shift_left));
#endif
	EPSTM(data = (int16_t)_eagle_tsc_pmd_rde_reg( pa, addr, err_code_p));        /* convert it to signed */

	data <<= shift_left;                                                    /* Move the sign bit to the left most    [shift_left  = (15-msb)] */
	data >>= shift_right;                                                   /* Move to the right with sign extension [shift_right = (15-msb+lsb)] */

	return data;
}

uint8_t _eagle_tsc_pmd_rde_field_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p){
	return (uint8_t)_eagle_tsc_pmd_rde_field( pa, addr, shift_left, shift_right, err_code_p);
}

int8_t _eagle_tsc_pmd_rde_field_signed_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p){
	return (int8_t)_eagle_tsc_pmd_rde_field_signed( pa, addr, shift_left, shift_right, err_code_p);
}
