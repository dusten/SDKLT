/*
 *         
 * $Id: phymod.c,v 1.2.2.13 2013/08/12 11:43:46 dayad Exp $
 * 
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

#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/phymod_acc.h>

int phymod_core_probe(const phymod_access_t* access, phymod_dispatch_type_t* type, int* is_probed)
{
    uint32_t is_identified;
    phymod_dispatch_type_t idx;
    phymod_core_access_t core;
    uint32_t core_id;
    int rv=PHYMOD_E_NONE;

    *is_probed = 0;

    /*parameters validation*/
    if(PHYMOD_E_OK != phymod_access_t_validate(access)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("access validation failed")));
    }

    if(type == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("type NULL parameter")));
    }

    core_id = 0;
    
    /*Identify phy type & driver*/
    PHYMOD_MEMCPY(&core.access, access, sizeof(core.access));
    core.type = phymodDispatchTypeCount;
    is_identified = 0;
    for(idx = 0 ; idx < phymodDispatchTypeCount ; idx++) {
        if(NULL != __phymod__dispatch__[idx]->f_phymod_core_identify) {
            rv = __phymod__dispatch__[idx]->f_phymod_core_identify(&core, core_id, &is_identified);
            if (rv == PHYMOD_E_NONE && is_identified) {
                (*type) = idx;
                *is_probed = 1;
                break;
            }
        }
    }
         
    return PHYMOD_E_NONE;
}

int phymod_osr_mode_to_actual_os(phymod_osr_mode_t osr_mode, uint32_t* os_int, uint32_t* os_remainder)
{
    if(PHYMOD_E_OK != phymod_osr_mode_t_validate(osr_mode)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("osr_mode validation failed")));
    }

    if(os_int == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("os_int NULL parameter")));
    }
    if(os_remainder == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("os_remainder NULL parameter")));
    }

    *os_int = 0;
    *os_remainder = 0;

    switch (osr_mode) {
        case phymodOversampleMode1: 
            *os_int = 1;
            break;
        case phymodOversampleMode2: 
            *os_int = 2; 
            break;
        case phymodOversampleMode3: 
            *os_int = 3; 
            break;
        case phymodOversampleMode3P3: 
            *os_int = 3;
            *os_remainder = 300;
            break;
        case phymodOversampleMode4: 
            *os_int = 4; 
            break;
        case phymodOversampleMode5: 
            *os_int = 5; 
            break;
        case phymodOversampleMode7P5: 
            *os_int = 7;
            *os_remainder = 500;
            break;
        case phymodOversampleMode8: 
            *os_int = 8; 
            break;
        case phymodOversampleMode8P25:
            *os_int = 8;
            *os_remainder = 250;
            break;
        case phymodOversampleMode10: 
            *os_int = 10; 
            break;
        case phymodOversampleMode16P5:
            *os_int = 16;
            *os_remainder = 500;
            break;
        case phymodOversampleMode20P625:
            *os_int = 20;
            *os_remainder = 625;
            break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("OS mode not supported")));
    }
         
    return PHYMOD_E_NONE;
}




