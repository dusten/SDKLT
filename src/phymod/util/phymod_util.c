/* 
 * $Id: phymod_diag.c $ 
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
#include <phymod/phymod_util.h>

int phymod_util_lane_config_get(const phymod_access_t *phys, int *start_lane, int *num_of_lane)
{
    int i ;
    switch (phys->lane_mask) {
        case 0x1:
            *start_lane = 0;
            *num_of_lane = 1;
            break;
        case 0x2:
            *start_lane = 1;
            *num_of_lane = 1;
            break;
        case 0x4:
            *start_lane = 2;
            *num_of_lane = 1;
            break;
        case 0x8:
            *start_lane = 3;
            *num_of_lane = 1;
            break;
        case 0x3:
            *start_lane = 0;
            *num_of_lane = 2;
            break;
        case 0xc:
            *start_lane = 2;
            *num_of_lane = 2;
            break;
        case 0x7:
            *start_lane = 0;
            *num_of_lane = 3;
            break;
        case 0xf:
            *start_lane = 0;
            *num_of_lane = 4;
            break;
        case 0x10:
            *start_lane = 4;
            *num_of_lane = 1;
            break;
        case 0x20:
            *start_lane = 5;
            *num_of_lane = 1;
            break;
        case 0x40:
            *start_lane = 6;
            *num_of_lane = 1;
            break;
        case 0x80:
            *start_lane = 7;
            *num_of_lane = 1;
            break;
        case 0x30:
            *start_lane = 4;
            *num_of_lane = 2;
            break;
        case 0xc0:
            *start_lane = 6;
            *num_of_lane = 2;
            break;
        case 0xf0:
            *start_lane = 4;
            *num_of_lane = 4;
            break;
        case 0xff:
            *start_lane = 0;
            *num_of_lane = 8;
            break;
        case 0x100:
            *start_lane = 8;
            *num_of_lane = 1;
            break;
        case 0x200:
            *start_lane = 9;
            *num_of_lane = 1;
            break;
        case 0x400:
            *start_lane = 10;
            *num_of_lane = 1;
            break;
        case 0x800:
            *start_lane = 11;
            *num_of_lane = 1;
            break;
        case 0x1000:
            *start_lane = 12;
            *num_of_lane = 1;
            break;
        case 0x2000:
            *start_lane = 13;
            *num_of_lane = 1;
            break;
        case 0x4000:
            *start_lane = 14;
            *num_of_lane = 1;
            break;
        case 0x8000:
            *start_lane = 15;
            *num_of_lane = 1;
            break;
        default:
            /*Support non-consecutive lanes*/
            for(i = 0; i < 16; i++)
            {
                if(phys->lane_mask & (1 << i))
                {
                    *start_lane = i;
                    break;
                }
            }
            *num_of_lane = 4;
    }
    return PHYMOD_E_NONE;
}

int phymod_core_name_get(const phymod_core_access_t *core,  uint32_t serdes_id, char *core_name, phymod_core_info_t *info)
{
    char* rev_char[4]={"A", "B", "C", "D"};
    char rev_num[]="7";
    uint8_t rev_letter, rev_number;

    info->serdes_id = serdes_id;
    rev_letter = (serdes_id & 0xc000) >> 14;
    rev_number = (serdes_id & 0x3800) >> 11;
    PHYMOD_STRCAT(core_name, rev_char[rev_letter]);
    PHYMOD_SNPRINTF(rev_num, 2, "%d", rev_number);
    PHYMOD_STRCAT(core_name, rev_num);
    PHYMOD_STRNCPY(info->name, core_name, PHYMOD_STRLEN(core_name)+1);

    return PHYMOD_E_NONE;
}

int phymod_swap_bit(uint16_t original_value, uint16_t *swapped_val)
{
    uint16_t new_val, temp_val;
    uint16_t i;

    new_val = 0;
    for (i = 0; i < 16; i++) {
        temp_val = 0x0;
        temp_val = (original_value & (0x1 << (15 - i))) >> (15 -i);
        temp_val = (temp_val << i) & (0x1 << i);
        new_val |= temp_val;
   }
   *swapped_val = new_val;
    return PHYMOD_E_NONE;
}
