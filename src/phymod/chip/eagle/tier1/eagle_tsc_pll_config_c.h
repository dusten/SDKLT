/*******************************************************************************
*******************************************************************************
*  File Name     :  eagle_pll_config.c                                        *
*  Created On    :  14/07/2013                                                *
*  Created By    :  Kiran Divakar                                             *
*  Description   :  Eagle PLL Configuration API                               *
*  Revision      :  $Id: eagle_pll_config.c 1103 2015-08-14 16:57:21Z kirand $ *
*                                                                             *
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
*                                                        *
*  No portions of this material may be reproduced in any form without         *
*  the written permission of:                                                 *
*      Broadcom Corporation                                                   *
*      5300 California Avenue                                                 *
*      Irvine, CA  92617                                                      *
*                                                                             *
*  All information contained in this document is Broadcom Corporation         *
*  company private proprietary, and trade secret.                             *
 */

/** @file
 * Eagle PLL Configuration
 */

#include "eagle_tsc_enum.h"

err_code_t eagle_tsc_configure_pll( const phymod_access_t *pa, enum eagle_tsc_pll_enum pll_cfg){

	/* KVH VCO Range          */
	/* 00  11.5    -  13.125  */
	/* 01  10.3125 - <11.5    */
	/* 10   9.375  - <10.3125 */
	/* 11   8.125  - < 9.375  */

	/* CKBWVCO Range          */
	/* 00  12.5               */
	/* 10  10.625  - 11.5G    */
	/* 01   9.375  - 10.3125G */
	/* 11  <9.375G            */

#ifndef ATE_LOG
	uint8_t reset_state;
#endif
	/*  Use this to restore defaults if reprogramming the PLL under dp-reset (typically Auto-Neg FW) */
	  EFUN(wrc_pll_mode              (   0xA)); 
	  EFUN(wrc_ams_pll_fracn_ndiv_int(   0x0)); 
	  EFUN(wrc_ams_pll_fracn_div_h   (   0x0)); 
	  EFUN(wrc_ams_pll_fracn_div_l   (   0x0)); 
	  EFUN(wrc_ams_pll_fracn_bypass  (   0x0)); 
	  EFUN(wrc_ams_pll_fracn_divrange(   0x0)); 
	  EFUN(wrc_ams_pll_fracn_sel     (   0x0)); 
	  EFUN(wrc_ams_pll_ditheren      (   0x0)); 
	  EFUN(wrc_ams_pll_force_kvh_bw  (   0x0)); 
	  EFUN(wrc_ams_pll_kvh_force     (   0x0)); 
	  EFUN(wrc_ams_pll_2rx_clkbw     (   0x0)); 
	  EFUN(wrc_ams_pll_vco_div2      (   0x0)); 
	  EFUN(wrc_ams_pll_vco_div4      (   0x0)); 
	  EFUN(wrc_ams_pll_refclk_doubler(   0x0)); 
	  EFUN(wrc_ams_pll_fp3_ctrl      (   0x0)); 
	  EFUN(wrc_ams_pll_fp3_rh        (   0x0)); 

	/* Use core_s_rstb to re-initialize all registers to default before calling this function. */
#ifndef ATE_LOG

	ESTM(reset_state = rdc_core_dp_reset_state());
	if (reset_state < 7) {
		EFUN_PRINTF(("ERROR: eagle_tsc_configure_pll( pa, ..) called without core_dp_s_rstb=0\n"));
		return _error(ERR_CODE_CORE_DP_NOT_RESET);
	}
#endif
	switch (pll_cfg) {
	/******************/
	/*  Integer Mode  */
	/******************/
	case EAGLE_TSC_pll_div_80x_refc125: /* pll_mode<3:0> 1101 VCO 10G      REF 125   MHz */
		EFUN(wrc_pll_mode( 0xD));
		EFUN(wrc_ams_pll_force_kvh_bw( 0x1));
		EFUN(wrc_ams_pll_2rx_clkbw( 0x1));
		EFUN(wrc_ams_pll_kvh_force( 0x2));
		EFUN(wrc_ams_pll_vco_div2( 0x0));
		break;
	case EAGLE_TSC_pll_div_80x_refc106: /* pll_mode<3:0> 1101 VCO  8.5G    REF 106.25MHz */
		EFUN(wrc_pll_mode( 0xD));
		EFUN(wrc_ams_pll_force_kvh_bw( 0x1));
		EFUN(wrc_ams_pll_2rx_clkbw( 0x3));
		EFUN(wrc_ams_pll_kvh_force( 0x3));
		EFUN(wrc_ams_pll_vco_div2( 0x0));
		break;
	case EAGLE_TSC_pll_div_80x:
	case EAGLE_TSC_pll_div_80x_refc156: /* pll_mode<3:0> 1101 VCO 12.5G    REF 156.25MHz */
		EFUN(wrc_pll_mode( 0xD));
		EFUN(wrc_ams_pll_force_kvh_bw( 0x1));
		EFUN(wrc_ams_pll_2rx_clkbw( 0x0));
		EFUN(wrc_ams_pll_kvh_force( 0x2));
		EFUN(wrc_ams_pll_vco_div2( 0x0));
		break;
	case EAGLE_TSC_pll_div_46x:     /* pll_mode<3:0> 0000 VCO  5.75G   REF 125   MHz */
		EFUN(wrc_pll_mode( 0x0));
		break;
	case EAGLE_TSC_pll_div_50x:     /* pll_mode<3:0> 0101 VCO  6.25G   REF 125   MHz */
		EFUN(wrc_pll_mode( 0x5));
		break;
	case EAGLE_TSC_pll_div_68x:     /* pll_mode<3:0> 1011 VCO  8.5G    REF 125   MHz */
		EFUN(wrc_pll_mode( 0xB));
		break;
	case EAGLE_TSC_pll_div_92x:     /* pll_mode<3:0> 1110 VCO 11.5G    REF 125   MHz */
		EFUN(wrc_pll_mode( 0xE));
		break;
	case EAGLE_TSC_pll_div_100x:    /* pll_mode<3:0> 1111 VCO 12.5G    REF 125   MHz */
		EFUN(wrc_pll_mode( 0xF));
		break;
	case EAGLE_TSC_pll_div_40x:     /* pll_mode<3:0> 0010 VCO  6.25G   REF 156.25MHz  */
		EFUN(wrc_pll_mode( 0x2));
		EFUN(wrc_ams_pll_force_kvh_bw( 0x1));
		EFUN(wrc_ams_pll_2rx_clkbw( 0x3));
		EFUN(wrc_ams_pll_kvh_force( 0x2));
		EFUN(wrc_ams_pll_vco_div2( 0x1));
		break;
	case EAGLE_TSC_pll_div_42x:     /* pll_mode<3:0> 0011 VCO  6.5625G REF 156.25MHz */
		EFUN(wrc_pll_mode( 0x3));
		EFUN(wrc_ams_pll_force_kvh_bw( 0x1));
		EFUN(wrc_ams_pll_2rx_clkbw( 0x3));
		EFUN(wrc_ams_pll_kvh_force( 0x2));
		EFUN(wrc_ams_pll_vco_div2( 0x1));
		break;
	case EAGLE_TSC_pll_div_52x:     /* pll_mode<3:0> 0110 VCO  8.125G  REF 156.25MHz */
		EFUN(wrc_pll_mode( 0x6));
		break;
	/* 5.0G   mode NOT verified by digital team or DVT */
	/* case EAGLE_TSC_pll_div_32x:       *//* pll_mode<3:0> 0111 VCO  5.0G    REF 156.25MHz  */
	/*     EFUN(wrc_pll_mode              (   0x7));   */
	/*     break;                                      */
	/* 3.125G mode NOT verified by digital team or DVT */
	/* case EAGLE_TSC_pll_div_20x:       *//* pll_mode<3:0> 0100 VCO  3.125G  REF 156.25MHz  */
	/*     EFUN(wrc_pll_mode              (   0x4));   */
	/*     break;                                      */
	case EAGLE_TSC_pll_div_60x:
		/* pll_mode<3:0> 1000 VCO  9.375 G REF 156.25MHz */
		EFUN(wrc_pll_mode(   0x8));
		break;
	case EAGLE_TSC_pll_div_64x_refc161:
		/* pll_mode<3:0> 1001 VCO 10.3125G REF 161.13MHz */
		EFUN(wrc_pll_mode(   0x9));
		EFUN(wrc_ams_pll_force_kvh_bw(   0x1));
		EFUN(wrc_ams_pll_2rx_clkbw(   0x1));
		EFUN(wrc_ams_pll_kvh_force(   0x2));
		EFUN(wrc_ams_pll_vco_div2( 0x0));
		break;
	case EAGLE_TSC_pll_div_64x:
	case EAGLE_TSC_pll_div_64x_refc156:
		/* pll_mode<3:0> 1001 VCO 10G      REF 156.25MHzMHz */
		EFUN(wrc_pll_mode(   0x9));
		EFUN(wrc_ams_pll_force_kvh_bw(   0x1));
		EFUN(wrc_ams_pll_2rx_clkbw(   0x1));
		EFUN(wrc_ams_pll_kvh_force(   0x1));
		EFUN(wrc_ams_pll_vco_div2( 0x0));
		break;
	case EAGLE_TSC_pll_div_66x:
		/* pll_mode<3:0> 1010 VCO 10.3125G REF 156.25MHz */
		EFUN(wrc_pll_mode(   0xA));
		break;
	case EAGLE_TSC_pll_div_70x:
		/* pll_mode<3:0> 1100 VCO 10.9375G REF 156.25MHz  */
		EFUN(wrc_pll_mode(   0xC));
		break;
	case EAGLE_TSC_pll_div_72x:
		/* pll_mode<3:0> 0001 VCO 11.25  G REF 156.25MHz */
		EFUN(wrc_pll_mode(   0x1));
		break;
	/*****************/
	/*  Frac-N Mode  */
	/*****************/
	case EAGLE_TSC_pll_div_82p5x: /* DIV  82.5  VCO 10.3125G REF 125   MHz */
		EFUN(wrc_ams_pll_fracn_ndiv_int(  0x52));
		EFUN(wrc_ams_pll_fracn_div_l(0x0000));
		EFUN(wrc_ams_pll_fracn_div_h(   0x2));
		EFUN(wrc_ams_pll_fracn_bypass(   0x0));
		EFUN(wrc_ams_pll_fracn_divrange(   0x0));
		EFUN(wrc_ams_pll_fracn_sel(   0x1));
		EFUN(wrc_ams_pll_ditheren(   0x1));
		EFUN(wrc_ams_pll_force_kvh_bw(   0x1));
		EFUN(wrc_ams_pll_2rx_clkbw(   0x1));
		EFUN(wrc_ams_pll_kvh_force(   0x1));
		EFUN(wrc_ams_pll_vco_div2(   0x0));
		EFUN(wrc_ams_pll_vco_div4(   0x0));
		EFUN(wrc_ams_pll_refclk_doubler(   0x0));
		EFUN(wrc_ams_pll_fp3_ctrl(   0x3));
		EFUN(wrc_ams_pll_fp3_rh(   0x1));
		break;
	case EAGLE_TSC_pll_div_87p5x: /* DIV  87.5  VCO 10.9375G REF 125   MHz */
		EFUN(wrc_ams_pll_fracn_ndiv_int(  0x57));
		EFUN(wrc_ams_pll_fracn_div_l(0x0000));
		EFUN(wrc_ams_pll_fracn_div_h(   0x2));
		EFUN(wrc_ams_pll_fracn_bypass(   0x0));
		EFUN(wrc_ams_pll_fracn_divrange(   0x0));
		EFUN(wrc_ams_pll_fracn_sel(   0x1));
		EFUN(wrc_ams_pll_ditheren(   0x1));
		EFUN(wrc_ams_pll_force_kvh_bw(   0x1));
		EFUN(wrc_ams_pll_2rx_clkbw(   0x2));
		EFUN(wrc_ams_pll_kvh_force(   0x1));
		EFUN(wrc_ams_pll_vco_div2(   0x0));
		EFUN(wrc_ams_pll_vco_div4(   0x0));
		EFUN(wrc_ams_pll_refclk_doubler(   0x0));
		EFUN(wrc_ams_pll_fp3_ctrl(   0x3));
		EFUN(wrc_ams_pll_fp3_rh(   0x1));
		break;
	case EAGLE_TSC_pll_div_73p6x: /* DIV  73.6  VCO 11.5G    REF 156.25MHz */
		EFUN(wrc_ams_pll_fracn_ndiv_int(  0x49));
		EFUN(wrc_ams_pll_fracn_div_l(0x6666));
		EFUN(wrc_ams_pll_fracn_div_h(   0x2));
		EFUN(wrc_ams_pll_fracn_bypass(   0x0));
		EFUN(wrc_ams_pll_fracn_divrange(   0x0));
		EFUN(wrc_ams_pll_fracn_sel(   0x1));
		EFUN(wrc_ams_pll_ditheren(   0x1));
		EFUN(wrc_ams_pll_force_kvh_bw(   0x1));
		EFUN(wrc_ams_pll_2rx_clkbw(   0x2));
		EFUN(wrc_ams_pll_kvh_force(   0x0));
		EFUN(wrc_ams_pll_vco_div2(   0x0));
		EFUN(wrc_ams_pll_vco_div4(   0x0));
		EFUN(wrc_ams_pll_refclk_doubler(   0x0));
		EFUN(wrc_ams_pll_fp3_ctrl(   0x3));
		EFUN(wrc_ams_pll_fp3_rh(   0x1));
		break;
	case EAGLE_TSC_pll_div_36p8x: /* DIV  73.6  VCO 11.5G/2  REF 156.25MHz */
		EFUN(wrc_ams_pll_fracn_ndiv_int(  0x49));
		EFUN(wrc_ams_pll_fracn_div_l(0x6666));
		EFUN(wrc_ams_pll_fracn_div_h(   0x2));
		EFUN(wrc_ams_pll_fracn_bypass(   0x0));
		EFUN(wrc_ams_pll_fracn_divrange(   0x0));
		EFUN(wrc_ams_pll_fracn_sel(   0x1));
		EFUN(wrc_ams_pll_ditheren(   0x1));
		EFUN(wrc_ams_pll_force_kvh_bw(   0x1));
		EFUN(wrc_ams_pll_kvh_force(   0x0));
		EFUN(wrc_ams_pll_2rx_clkbw(   0x3));
		EFUN(wrc_ams_pll_vco_div2(   0x1));
		EFUN(wrc_ams_pll_vco_div4(   0x0));
		EFUN(wrc_ams_pll_refclk_doubler(   0x0));
		EFUN(wrc_ams_pll_fp3_ctrl(   0x3));
		EFUN(wrc_ams_pll_fp3_rh(   0x1));
		break;
	case EAGLE_TSC_pll_div_199p04x: /* DIV 199.04 VCO  9.952G  REF  25   MHz */
		EFUN(wrc_ams_pll_fracn_ndiv_int(  0xc7));
		EFUN(wrc_ams_pll_fracn_div_l(0x28f5));
		EFUN(wrc_ams_pll_fracn_div_h(   0x0));
		EFUN(wrc_ams_pll_fracn_bypass(   0x0));
		EFUN(wrc_ams_pll_fracn_divrange(   0x0));
		EFUN(wrc_ams_pll_fracn_sel(   0x1));
		EFUN(wrc_ams_pll_ditheren(   0x1));
		EFUN(wrc_ams_pll_force_kvh_bw(   0x1));
		EFUN(wrc_ams_pll_2rx_clkbw(   0x1));
		EFUN(wrc_ams_pll_kvh_force(   0x2));
		EFUN(wrc_ams_pll_vco_div2(   0x0));
		EFUN(wrc_ams_pll_vco_div4(   0x0));
		EFUN(wrc_ams_pll_refclk_doubler(   0x1));
		EFUN(wrc_ams_pll_fp3_ctrl(   0x3));
		EFUN(wrc_ams_pll_fp3_rh(   0x1));
		break;
	/*******************************/
	/*  Invalid 'pll_cfg' Selector */
	/*******************************/
	default:                 /* Invalid pll_cfg value  */
		return _error(ERR_CODE_INVALID_PLL_CFG);
		break;
	} /* switch (pll_cfg) */
	return ERR_CODE_NONE;
}   /* eagle_tsc_configure_pll */

err_code_t eagle_tsc_get_pll_div_from_enum (enum eagle_tsc_pll_enum pll_cfg, uint32_t *new_pll_div){

    switch (pll_cfg) {
	/******************/
	/*  Integer Mode  */
	/******************/
	case EAGLE_TSC_pll_div_80x_refc125: /* pll_mode<3:0> 1101 VCO 10G      REF 125   MHz */
		*new_pll_div  = 0xD;
		break;
	case EAGLE_TSC_pll_div_80x_refc106: /* pll_mode<3:0> 1101 VCO  8.5G    REF 106.25MHz */
		*new_pll_div  = 0xD;
		break;
	case EAGLE_TSC_pll_div_80x:
	case EAGLE_TSC_pll_div_80x_refc156: /* pll_mode<3:0> 1101 VCO 12.5G    REF 156.25MHz */
		*new_pll_div  = 0xD;
		break;
	case EAGLE_TSC_pll_div_46x:     /* pll_mode<3:0> 0000 VCO  5.75G   REF 125   MHz */
		*new_pll_div  = 0x0;
		break;
	case EAGLE_TSC_pll_div_50x:     /* pll_mode<3:0> 0101 VCO  6.25G   REF 125   MHz */
		*new_pll_div  = 0x5;
		break;
	case EAGLE_TSC_pll_div_68x:     /* pll_mode<3:0> 1011 VCO  8.5G    REF 125   MHz */
		*new_pll_div  = 0xB;
		break;
	case EAGLE_TSC_pll_div_92x:     /* pll_mode<3:0> 1110 VCO 11.5G    REF 125   MHz */
		*new_pll_div  = 0xE;
		break;
	case EAGLE_TSC_pll_div_100x:    /* pll_mode<3:0> 1111 VCO 12.5G    REF 125   MHz */
		*new_pll_div  = 0xF;
		break;
	case EAGLE_TSC_pll_div_40x:     /* pll_mode<3:0> 0010 VCO  6.25G   REF 156.25MHz  */
		*new_pll_div  = 0x2;
		break;
	case EAGLE_TSC_pll_div_42x:     /* pll_mode<3:0> 0011 VCO  6.5625G REF 156.25MHz */
		*new_pll_div  = 0x3;
		break;
	case EAGLE_TSC_pll_div_52x:     /* pll_mode<3:0> 0110 VCO  8.125G  REF 156.25MHz */
		*new_pll_div  = 0x6;
		break;
	/* 5.0G   mode NOT verified by digital team or DVT */
	/* case EAGLE_TSC_pll_div_32x:       *//* pll_mode<3:0> 0111 VCO  5.0G    REF 156.25MHz  */
	/*     EFUN(wrc_pll_mode              (   0x7));   */
	/*     break;                                      */
	/* 3.125G mode NOT verified by digital team or DVT */
	/* case EAGLE_TSC_pll_div_20x:       *//* pll_mode<3:0> 0100 VCO  3.125G  REF 156.25MHz  */
	/*     EFUN(wrc_pll_mode              (   0x4));   */
	/*     break;                                      */
	case EAGLE_TSC_pll_div_60x:
		/* pll_mode<3:0> 1000 VCO  9.375 G REF 156.25MHz */
		*new_pll_div  = 0x8;
		break;
	case EAGLE_TSC_pll_div_64x_refc161:
		/* pll_mode<3:0> 1001 VCO 10.3125G REF 161.13MHz */
		*new_pll_div  = 0x9;
		break;
	case EAGLE_TSC_pll_div_64x:
	case EAGLE_TSC_pll_div_64x_refc156:
		/* pll_mode<3:0> 1001 VCO 10G      REF 156.25MHzMHz */
		*new_pll_div  = 0x9;
		break;
	case EAGLE_TSC_pll_div_66x:
		/* pll_mode<3:0> 1010 VCO 10.3125G REF 156.25MHz */
		*new_pll_div  = 0xA;
		break;
	case EAGLE_TSC_pll_div_70x:
		/* pll_mode<3:0> 1100 VCO 10.9375G REF 156.25MHz  */
		*new_pll_div  = 0xC;
		break;
	case EAGLE_TSC_pll_div_72x:
		/* pll_mode<3:0> 0001 VCO 11.25  G REF 156.25MHz */
		*new_pll_div  = 0x1;
		break;
	default:                 /* Invalid pll_cfg value  */
		return _error(ERR_CODE_INVALID_PLL_CFG);
		break;
	}
    return ERR_CODE_NONE;
}
