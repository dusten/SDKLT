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

#include <phymod/phymod_symbols.h>

static size_t
phymod_strlcpy(char *dest, const char *src, size_t cnt)
{
    char *ptr = dest;
    size_t copied = 0;

    while (*src && (cnt > 1)) {
	*ptr++ = *src++;
	cnt--;
	copied++;
    }
    *ptr = '\0';

    return copied;
}

/*
 * Function:
 *	phymod_symbol_field_filter
 * Purpose:
 *	Callback for Filtering fields based on current data view.
 * Parameters:
 *	symbol - symbol information
 *	fnames - list of all field names for this device
 *	encoding - key for decoding overlay
 *	cookie - context data
 * Returns:
 *      Non-zero if field should be filtered out (not displayed)
 * Notes:
 *      The filter key has the following syntax:
 *
 *        {[<keysrc>]:<keyfield>:<keyval>[|<keyval> ... ]}
 *
 *      Ideally the keysrc is the same data entry which is
 *      being decoded, and in this case it can left out, e.g.:
 *
 *        {:KEY_TYPEf:1}
 *
 *      This example encoding means that if KEY_TYPEf=1, then
 *      this field is valid for this view.
 *
 *      Note that a filed can be for multiple views, e.g.:
 *
 *        {:KEY_TYPEf:1|3}
 *
 *      This example encoding means that this field is valid
 *      if KEY_TYPEf=1 or KEY_TYPEf=3.
 *
 *      The special <keyval>=-1 means that this field is valid
 *      even if there is no context (cookie=NULL).
 *
 *      Note that this filter code does NOT support a <keysrc>
 *      which is different from the current data entry.
 */
int 
phymod_symbol_field_filter(const phymod_symbol_t *symbol, const char **fnames,
                              const char *encoding, void *cookie)
{
#if PHYMOD_CONFIG_INCLUDE_FIELD_NAMES == 1
    uint32_t *data = (uint32_t *)cookie;
    uint32_t val[PHYMOD_MAX_REG_WSIZE];
    phymod_field_info_t finfo; 
    char tstr[128];
    char *keyfield, *keyvals;
    char *ptr;
    uint32_t *kval = NULL;

    /* Do not filter if no (or unknown) encoding */
    if (encoding == NULL || *encoding != '{') {
        return 0;
    }

    /* Do not filter if encoding cannot be parsed */
    phymod_strlcpy(tstr, encoding, sizeof(tstr));
    ptr = tstr;
    if ((ptr = PHYMOD_STRCHR(ptr, ':')) == NULL) {
        return 0;
    }
    *ptr++ = 0;
    keyfield = ptr;
    if ((ptr = PHYMOD_STRCHR(ptr, ':')) == NULL) {
        return 0;
    }
    *ptr++ = 0;
    keyvals = ptr;

    /* Only show default view if no context */
    if (data == NULL) {
        return (PHYMOD_STRSTR(keyvals, "-1") == NULL) ? 1 : 0;
    }

    /* Look for <keyfield> in data entry */
    PHYMOD_SYMBOL_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        if (finfo.name && PHYMOD_STRCMP(finfo.name, keyfield) == 0) {
            /* Get normalized field value */
            PHYMOD_MEMSET(val, 0, sizeof(val));
            phymod_field_get(data, finfo.minbit, finfo.maxbit, val);
            kval = &val[0];
            break;
        }

    } PHYMOD_SYMBOL_FIELDS_ITER_END(); 

    /* Check if current key matches any <keyval> in encoding */
    ptr = keyvals;
    while (ptr) {
        if (kval && PHYMOD_STRTOUL(ptr, NULL, 0) == *kval) {
            return 0;
        }
        if ((ptr = PHYMOD_STRCHR(ptr, '|')) != NULL) {
            ptr++;
        }
    }
#endif

    /* No match - filter this field */
    return 1; 
}
