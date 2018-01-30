/*! \file bcmmgmt_sysm_default.c
 *
 * Contains the default list of system manager components.
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

#include <bcmbd/bcmbd_sysm.h>
#include <bcmlt/bcmlt_sysm.h>
#include <bcmtrm/trm_sysm.h>
#include <bcmptm/bcmptm_cth_uft_sysm.h>
#include <bcmptm/bcmptm_cth_alpm_sysm.h>
#include <bcmptm/bcmptm_cth_alpm_be_sysm.h>
#include <bcmptm/bcmptm_rm_hash_sysm.h>
#include <bcmptm/bcmptm_rm_tcam_sysm.h>
#include <bcmptm/bcmptm_rm_alpm_sysm.h>
#include <bcmptm/bcmptm_sysm.h>
#include <bcmltm/bcmltm_sysm.h>
#include <bcmfp/bcmfp_sysm.h>
#include <bcmlm/bcmlm_sysm.h>
#include <bcmcfg/bcmcfg_sysm.h>
#include <bcmimm/bcmimm_sysm.h>
#include <bcmecmp/bcmecmp_sysm.h>
#include <bcmtrunk/bcmtrunk_sysm.h>
#include <bcmltp/bcmltp_sysm.h>
#include <bcmpc/bcmpc_sysm.h>
#include <bcmtm/bcmtm_sysm.h>
#include <bcmmeter/bcmmeter_sysm.h>
#include <bcmtnl/bcmtnl_sysm.h>
#include <bcmvlan/bcmvlan_sysm.h>
#include <bcml2/bcml2_sysm.h>
#include <bcmecn/bcmecn_sysm.h>
#include <bcmlrd/bcmlrd_sysm.h>
#include <bcmevm/bcmevm_sysm.h>
#include <bcmport/bcmport_sysm.h>
#include <bcmmirror/bcmmirror_sysm.h>

#include <bcmmgmt/bcmmgmt_sysm_default.h>

static bcmmgmt_comp_start_f def_comp_start[] = {
    bcmlrd_start,
    bcmcfg_start,
    bcmbd_start,
    bcmlt_start,
    bcmptm_cth_uft_start,
    bcmptm_cth_alpm_be_start,
    bcmptm_cth_alpm_start,
    bcmptm_rm_hash_start,
    bcmptm_rm_tcam_start,
    bcmptm_rm_alpm_start,
    bcmptm_start,
    bcmltm_start,
    bcmtrm_start,
    bcmltp_start,
    bcmfp_start,
    bcmlm_start,
    bcmimm_start,
    bcmecmp_start,
    bcmtrunk_start,
    bcmpc_start,
    bcmtm_start,
    bcmmeter_start,
    bcmtnl_start,
    bcmvlan_start,
    bcml2_start,
    bcmecn_start,
    bcmevm_start,
    bcmport_start,
    bcmmirror_start
};

static const bcmmgmt_comp_list_t comp_list = {
    def_comp_start,
    COUNTOF(def_comp_start)
};

const bcmmgmt_comp_list_t *
bcmmgmt_sysm_default_comp_list(void)
{
    return &comp_list;
}
