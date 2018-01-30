/*! \file chelem.c
 *
 * Emulation chip elements
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

#include <sal/sal_assert.h>

#include <sim/chelem/chelem.h>

#ifndef CHELEM_MAX
#define CHELEM_MAX              (500 * 1024)
#endif

#ifndef CHELEM_ENTRY_NAX
#define CHELEM_ENTRY_NAX        (4 * 32)
#endif

typedef struct chelem_s {
    int unit;
    uint32_t addrx;
    uint32_t addr;
    uint8_t data[CHELEM_ENTRY_NAX];
} chelem_t;


static chelem_t chelem[CHELEM_MAX];

static uint32_t chelem_free_idx = 0;

static chelem_t *
find_chelem(int unit, uint32_t addrx, uint32_t addr, chelem_t *chelems)
{
    uint32_t i;
    for (i = 0; i < chelem_free_idx; i++) {
        if (chelems[i].addrx == addrx &&
            chelems[i].addr == addr &&
            chelems[i].unit == unit) {
            return chelems + i;
        }
    }
    return NULL;
}

static chelem_t *
find_or_create_chelem(int unit, uint32_t addrx, uint32_t addr, chelem_t *chelems)
{
    chelem_t *ch;

    if ((ch = find_chelem(unit, addrx, addr, chelems)) == NULL) {
        if (chelem_free_idx < CHELEM_MAX) {
            ch = &chelems[chelem_free_idx++];
        } else {
            assert(0);
            return NULL;
        }
    }

    ch->unit = unit;
    ch->addrx = addrx;
    ch->addr = addr;

    return ch;
}

void
chelem_clear_all(void)
{
    sal_memset(chelem, 0, sizeof(chelem));
}

int
chelem_read(int unit, uint32_t addrx, uint32_t addr,
            void *data, size_t size)
{
    chelem_t *ch;

    ch = find_chelem(unit, addrx, addr, chelem);
    if (ch) {
        assert(size < sizeof(ch->data));
        sal_memcpy(data, ch->data, size);
    } else {
        sal_memset(data, 0, size);
    }
    return 0;
}

int
chelem_write(int unit, uint32_t addrx, uint32_t addr,
             void *data, size_t size)
{
    chelem_t *ch;

    ch = find_or_create_chelem(unit, addrx, addr, chelem);
    if (!ch) {
        return -1;
    }
    assert(ch);
    assert(size <= sizeof(ch->data));
    sal_memcpy(ch->data, data, size);
    return 0;
}
