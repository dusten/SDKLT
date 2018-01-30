/************************************************************************************
************************************************************************************
*  File Name     :  falcon_tsc_pll_config.c                                        *
*  Created On    :  23/12/2013                                                     *
*  Created By    :  Kiran Divakar                                                  *
*  Description   :  Falcon TSC PLL Configuration API                               *
*  Revision      :  $Id: falcon_tsc_pll_config.c 996 2015-03-30 22:39:03Z kirand $ *
*                                                                                  *
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
*                                                             *
*  No portions of this material may be reproduced in any form without              *
*  the written permission of:                                                      *
*      Broadcom Corporation                                                        *
*      5300 California Avenue                                                      *
*      Irvine, CA  92617                                                           *
*                                                                                  *
*  All information contained in this document is Broadcom Corporation              *
*  company private proprietary, and trade secret.                                  *
 */


/** @file falcon_tsc_pll_config.c
 * Falcon TSC PLL Configuration
 */

#include "falcon_tsc_enum.h"


err_code_t falcon_tsc_configure_pll (const phymod_access_t *pa, enum falcon_tsc_pll_enum pll_cfg){
#ifndef ATE_LOG
	uint8_t reset_state;

	/* Restore defaults, needed for non-register reset cases */
	/* EFUN(wrc_pll_mode (0x7)); */

	/* Use core_s_rstb to re-initialize all registers to default before calling this function. */
	ESTM(reset_state = rdc_core_dp_reset_state());
	if (reset_state < 7) {
		EFUN_PRINTF(("ERROR: falcon_tsc_configure_pll (pa, ..) called without core_dp_s_rstb=0\n"));
		return _error(ERR_CODE_CORE_DP_NOT_RESET);
	}
#endif

	switch (pll_cfg) {
	/******************/
	/*  Integer Mode  */
	/******************/
	case FALCON_TSC_pll_div_128x:
		/* pll_mode<3:0> = 0011, VCO = 20.625  G, Refclk = 106.133MHz */
		EFUN(wrc_pll_mode(0x3));
		break;
	case FALCON_TSC_pll_div_132x:
		/* pll_mode<3:0> = 0100, VCO = 20.625  G, Refclk = 156.25 MHz */
		/* pll_mode<3:0> = 0100, VCO = 28.055  G, Refclk = 212.5  MHz */
		EFUN(wrc_pll_mode(0x4));
		break;
	case FALCON_TSC_pll_div_140x:
		/* pll_mode<3:0> = 0101, VCO = 21.875  G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0x5));
		break;
	case FALCON_TSC_pll_div_160x:
		/* pll_mode<3:0> = 0110, VCO = 25.0    G, Refclk = 156.25 MHz */
		/* pll_mode<3:0> = 0110, VCO = 25.78125G, Refclk = 161.3  MHz */
		EFUN(wrc_pll_mode(0x6));
		break;
	case FALCON_TSC_pll_div_165x:
		/* pll_mode<3:0> = 0111, VCO = 20.625  G, Refclk = 125    MHz */
		/* pll_mode<3:0> = 0111, VCO = 25.78125G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0x7));
		break;
	case FALCON_TSC_pll_div_168x:
		/* pll_mode<3:0> = 1000, VCO = 26.25   G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0x8));
		break;
	case FALCON_TSC_pll_div_175x:
		/* pll_mode<3:0> = 1010, VCO = 27.34375G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0xA));
		break;
	case FALCON_TSC_pll_div_180x:
		/* pll_mode<3:0> = 1011, VCO = 22.5    G, Refclk = 125.0  MHz */
		/* pll_mode<3:0> = 1011, VCO = 28.125  G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0xB));
		break;
	case FALCON_TSC_pll_div_184x:
		/* pll_mode<3:0> = 1100, VCO = 23.0    G, Refclk = 125.0  MHz */
		EFUN(wrc_pll_mode(0xC));
		break;
	case FALCON_TSC_pll_div_200x:
		/* pll_mode<3:0> = 1101, VCO = 25.0    G, Refclk = 125.0  MHz */
		EFUN(wrc_pll_mode(0xD));
		break;
	case FALCON_TSC_pll_div_224x:
		/* pll_mode<3:0> = 1110, VCO = 28.0    G, Refclk = 125.0  MHz */
		EFUN(wrc_pll_mode(0xE));
		break;
	case FALCON_TSC_pll_div_264x:
		/* pll_mode<3:0> = 1111, VCO = 28.05   G, Refclk = 106.25  MHz */
		EFUN(wrc_pll_mode(0xF));
		break;
	/*******************************/
	/*  Invalid 'pll_cfg' Selector */
	/*******************************/
	default:
		return _error(ERR_CODE_INVALID_PLL_CFG);
		break;
	} /* switch (pll_cfg) */

	return ERR_CODE_NONE;
}   /* falcon_tsc_configure_pll */


