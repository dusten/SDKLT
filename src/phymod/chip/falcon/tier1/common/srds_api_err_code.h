/**********************************************************************************
**********************************************************************************
*  File Name     :  srds_api_err_code.h                                          *
*  Created On    :  18/09/2013                                                   *
*  Created By    :  Kiran Divakar                                                *
*  Description   :  Header file with Error Code enums                            *
*  Revision      :  $Id: srds_api_err_code.h 1261 2015-11-19 15:51:37Z eroes $ *
*                                                                                *
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
*                                                           *
*  No portions of this material may be reproduced in any form without            *
*  the written permission of:                                                    *
*      Broadcom Corporation                                                      *
*      5300 California Avenue                                                    *
*      Irvine, CA  92617                                                         *
*                                                                                *
*  All information contained in this document is Broadcom Corporation            *
*  company private proprietary, and trade secret.                                *
 */

/** @file srds_api_err_code.h
 * Error Code enumerations
 */

#ifndef SRDS_API_ERR_CODE_H
#define SRDS_API_ERR_CODE_H

#define err_code_t uint16_t

/** ERROR CODE Enum */
enum srds_err_code_enum {
	ERR_CODE_NONE = 0,
	ERR_CODE_INVALID_RAM_ADDR,
	ERR_CODE_SERDES_DELAY,
	ERR_CODE_POLLING_TIMEOUT,
	ERR_CODE_CFG_PATT_INVALID_PATTERN,
	ERR_CODE_CFG_PATT_INVALID_PATT_LENGTH,
	ERR_CODE_CFG_PATT_LEN_MISMATCH,
	ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN,
	ERR_CODE_CFG_PATT_INVALID_HEX,
	ERR_CODE_CFG_PATT_INVALID_BIN2HEX,
	ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE,
	ERR_CODE_PATT_GEN_INVALID_MODE_SEL,
	ERR_CODE_INVALID_UCODE_LEN,
	ERR_CODE_MICRO_INIT_NOT_DONE,
	ERR_CODE_UCODE_LOAD_FAIL,
	ERR_CODE_UCODE_VERIFY_FAIL,
	ERR_CODE_INVALID_TEMP_IDX,
	ERR_CODE_INVALID_PLL_CFG,
	ERR_CODE_TX_HPF_INVALID,
	ERR_CODE_VGA_INVALID,
	ERR_CODE_PF_INVALID,
	ERR_CODE_TX_AMP_CTRL_INVALID,
	ERR_CODE_INVALID_EVENT_LOG_WRITE,
	ERR_CODE_INVALID_EVENT_LOG_READ,
	ERR_CODE_UC_CMD_RETURN_ERROR,
	ERR_CODE_DATA_NOTAVAIL,
	ERR_CODE_BAD_PTR_OR_INVALID_INPUT,
	ERR_CODE_UC_NOT_STOPPED,
	ERR_CODE_UC_CRC_NOT_MATCH,
	ERR_CODE_CORE_DP_NOT_RESET,
	ERR_CODE_LANE_DP_NOT_RESET,
	ERR_CODE_INVALID_CLK90_ADJUST,
	ERR_CODE_INVALID_CLK90_OVERRIDE,
	ERR_CODE_INVALID_TTS_PRBS_TRAFFIC_SEL,
	ERR_CODE_CONFLICTING_PARAMETERS,
	ERR_CODE_BAD_LANE_COUNT,
	ERR_CODE_BAD_LANE,
	ERR_CODE_UC_ACTIVE,
	ERR_CODE_TXFIR   = 1 << 8,
	ERR_CODE_DFE_TAP = 2 << 8,
	ERR_CODE_DIAG    = 3 << 8
};

/** TXFIR Error Codes Enum */
enum srds_txfir_failcodes {
	ERR_CODE_TXFIR_PRE_INVALID         = ERR_CODE_TXFIR +   1,
	ERR_CODE_TXFIR_MAIN_INVALID        = ERR_CODE_TXFIR +   2,
	ERR_CODE_TXFIR_POST1_INVALID       = ERR_CODE_TXFIR +   4,
	ERR_CODE_TXFIR_POST2_INVALID       = ERR_CODE_TXFIR +   8,
	ERR_CODE_TXFIR_POST3_INVALID       = ERR_CODE_TXFIR +  16,
	ERR_CODE_TXFIR_V2_LIMIT            = ERR_CODE_TXFIR +  32,
	ERR_CODE_TXFIR_SUM_LIMIT           = ERR_CODE_TXFIR +  64,
	ERR_CODE_TXFIR_PRE_POST1_SUM_LIMIT = ERR_CODE_TXFIR + 128
};

/** DFE Tap Error Codes Enum */
enum srds_dfe_tap_failcodes {
	ERR_CODE_DFE1_INVALID        = ERR_CODE_DFE_TAP +  1,
	ERR_CODE_DFE2_INVALID        = ERR_CODE_DFE_TAP +  2,
	ERR_CODE_DFE3_INVALID        = ERR_CODE_DFE_TAP +  4,
	ERR_CODE_DFE4_INVALID        = ERR_CODE_DFE_TAP +  8,
	ERR_CODE_DFE5_INVALID        = ERR_CODE_DFE_TAP + 16,
	ERR_CODE_DFE_TAP_IDX_INVALID = ERR_CODE_DFE_TAP + 32
};

/** DIAG Error Codes Enum */
enum srds_diag_failcodes {
	ERR_CODE_DIAG_TIMEOUT               = ERR_CODE_DIAG + 1,
	ERR_CODE_DIAG_INVALID_STATUS_RETURN = ERR_CODE_DIAG + 2,
	ERR_CODE_DIAG_SCAN_NOT_COMPLETE     = ERR_CODE_DIAG + 3
};
#endif
