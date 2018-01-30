/*
 * $Id$
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

#include <phymod/phymod_system.h>
#include <phymod/phymod_symbols.h>

#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1

static int
phymod_util_bit_range(char *buf, int size, int minbit, int maxbit)
{
    if (minbit == maxbit) {
        PHYMOD_SNPRINTF(buf, size, "<%d>", minbit);
    } else {
        PHYMOD_SNPRINTF(buf, size, "<%d:%d>", maxbit, minbit);
    }
    return 0;
}

/*
 * Function:
 *	phymod_symbol_show_fields
 * Purpose:
 *	Display all fields of a register/memory
 * Parameters:
 *	symbol - symbol information
 *	fnames - list of all field names
 *	data - symbol data (one or more 32-bit words)
 *	nz - skip fields with value of zero
 *	fcb - filter call-back function
 *	cookie - filter call-back context
 * Returns:
 *      Always 0
 */
int 
phymod_symbol_show_fields(const phymod_symbol_t *symbol,
                          const char **fnames, uint32_t *data, int nz,
                          int (*print_str)(const char *str),
                          phymod_symbol_filter_cb_t fcb, void *cookie)
{
    int mval; 
    phymod_field_info_t finfo; 
    char *ptr;
    char vstr[8 * PHYMOD_MAX_REG_WSIZE + 32];
    char fname_str[16];
    const char *fname;
    uint32_t val[PHYMOD_MAX_REG_WSIZE];
    int idx = 0;

    PHYMOD_SYMBOL_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        /* Create default field name */
        PHYMOD_SPRINTF(fname_str, "field%d", idx++);
        fname = fname_str;

#if PHYMOD_CONFIG_INCLUDE_FIELD_NAMES == 1
        /* Use real field name when available */
        if (finfo.name) {
            if (fcb && fcb(symbol, fnames, finfo.name, cookie) != 0) {
                continue;
            }
            /* Skip encoding part of field name if present */
            if ((fname = PHYMOD_STRCHR(finfo.name, '}')) != NULL) {
                fname++;
            } else {
                fname = finfo.name;
            }
        }
#endif

        /* Create bit range string */
        phymod_util_bit_range(vstr, sizeof(vstr),
                              finfo.minbit, 
                              finfo.maxbit);

        if (data) {
            /* Get normalized field value */
            PHYMOD_MEMSET(val, 0, sizeof(val));
            phymod_field_get(data, finfo.minbit, finfo.maxbit, val);

            /* Create field value string */
            mval = COUNTOF(val) - 1;
            while (mval && val[mval] == 0) {
                mval--;
            }

            /* Optionally skip fields with all zeros */
            if (nz && mval == 0 && val[mval] == 0) {
                continue;
            }

            ptr = vstr + PHYMOD_STRLEN(vstr);
            PHYMOD_SPRINTF(ptr, "=0x%"PRIx32"", val[mval]);
            while (mval--) {
                ptr += PHYMOD_STRLEN(ptr);
                PHYMOD_SPRINTF(ptr, "%08"PRIx32"", val[mval]);
            }
        }

        print_str("\t");
        print_str(fname);
        print_str(vstr);
        print_str("\n");

    } PHYMOD_SYMBOL_FIELDS_ITER_END(); 

    return 0; 
}

#endif /* PHYMOD_CONFIG_INCLUDE_FIELD_INFO */
