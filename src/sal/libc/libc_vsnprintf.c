/*! \file libc_vsnprintf.c
 *
 * SDK implementation of vsnprintf.
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

#include <sal/sal_libc.h>

#ifndef sal_vsnprintf

/*
 * Reasonably complete subset of ANSI-style printf routines.
 * Needs only strlen and stdarg.
 * Behavior was regressed against Solaris printf(3s) routines (below).
 *
 * Supported format controls:
 *
 *      %%      percent sign
 *      %c      character
 *      %d      integer
 *      %hd     short integer
 *      %ld     long integer
 *      %u      unsigned integer
 *      %o      unsigned octal integer
 *      %x      unsigned hexadecimal integer (lowercase)
 *      %X      unsigned hexadecimal integer (uppercase)
 *      %s      string
 *      %p      pointer
 *      %n      store number of characters output so far
 *
 * Flag modifiers supported:
 *      Field width, argument field width (*), left justify (-),
 *      zero-fill (0), alternate form (#), always include sign (+),
 *      space before positive numbers (space).
 *
 * Functions implemented:
 *
 * int vsnprintf(char *buf, size_t bufsz, const char *fmt, va_list ap);
 * int vsprintf(char *buf, const char *fmt, va_list ap);
 * int snprintf(char *buf, size_t bufsz, const char *fmt, ...);
 * int sprintf(char *buf, const char *fmt, ...);
 *
 * Note that some functions are implemented in separate source files.
 */
static void
_itoa(char *buf,        /* Large enough result buffer   */
      uint32_t num,     /* Number to convert            */
      int base,         /* Conversion base (2 to 16)    */
      int caps,         /* Capitalize letter digits     */
      int prec)         /* Precision (minimum digits)   */
{
    char tmp[36], *s, *digits;

    digits = (caps ? "0123456789ABCDEF" : "0123456789abcdef");

    s = &tmp[sizeof(tmp) - 1];

    for (*s = 0; num || s == &tmp[sizeof(tmp) - 1]; num /= base, prec--) {
        *--s = digits[num % base];
    }

    while (prec-- > 0) {
        *--s = '0';
    }

    sal_strcpy(buf, s);
}

#define X_STORE(c) {    \
    if (bp < be) {      \
        *bp = (c);      \
    }                   \
    bp++;               \
}

#define X_INF           SAL_VSNPRINTF_X_INF

int
sal_vsnprintf(char *buf, size_t bufsz, const char *fmt, va_list ap)
{
    char c, *bp, *be;
    char *p_null = NULL;
    char *b_inf = p_null - 1;

    bp = buf;
    be = (bufsz == X_INF) ? b_inf : &buf[bufsz - 1];

    while ((c = *fmt++) != 0) {
        int width = 0, ljust = 0, plus = 0, space = 0;
        int altform = 0, prec = 0, half = 0, base = 0;
        int tlong = 0, fillz = 0, plen, pad;
        long num = 0;
        char tmp[36], *p = tmp;

        if (c != '%') {
            X_STORE(c);
            continue;
        }

        for (c = *fmt++; ; c = *fmt++)
            switch (c) {
            case 'h':
                half = 1;
                break;
            case 'l':
                tlong = 1;
                break;
            case '-':
                ljust = 1;
                break;
            case '+':
                plus = 1;
                break;
            case ' ':
                space = 1;
                break;
            case '0':
                fillz = 1;
                break;
            case '#':
                altform = 1;
                break;
            case '*':
                /* Mark as need-to-fetch */
                width = -1;
                break;
            case '.':
                if ((c = *fmt++) == '*') {
                    /* Mark as need-to-fetch */
                    prec = -1;
                } else {
                    for (prec = 0; c >= '0' && c <= '9'; c = *fmt++) {
                        prec = prec * 10 + (c - '0');
                    }
                    fmt--;
                }
                break;
            default:
                if (c >= '1' && c <= '9') {
                    for (width = 0; c >= '0' && c <= '9'; c = *fmt++) {
                        width = width * 10 + (c - '0');
                    }
                    fmt--;
                } else {
                    goto break_for;
                }
                break;
            }

      break_for:
        if (width == -1) {
            width = va_arg(ap, int);
        }
        if (prec == -1) {
            prec = va_arg(ap, int);
        }
        if (c == 0) {
            break;
        }

        switch (c) {
        case 'd':
        case 'i':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(short)num;
            }
            /* For zero-fill, the sign must be to the left of the zeroes */
            if (fillz && (num < 0 || plus || space)) {
                X_STORE(num < 0 ? '-' : space ? ' ' : '+');
                if (width > 0) {
                    width--;
                }
                if (num < 0) {
                    num = -num;
                }
            }
            if (!fillz) {
                if (num < 0) {
                    *p++ = '-';
                    num = -num;
                } else if (plus) {
                    *p++ = '+';
                } else if (space) {
                    *p++ = ' ';
                }
            }
            base = 10;
            break;
        case 'u':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(short)num;
            }
            base = 10;
            break;
        case 'p':
            altform = 0;
            /* Fall through */
        case 'x':
        case 'X':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(unsigned short)num;
            }
            if (altform) {
                prec += 2;
                *p++ = '0';
                *p++ = c;
            }
            base = 16;
            break;
        case 'o':
        case 'O':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(unsigned short)num;
            }
            if (altform) {
                prec++;
                *p++ = '0';
            }
            base = 8;
            break;
        case 's':
            p = va_arg(ap, char *);
            if (prec == 0) {
                prec = X_INF;
            }
            break;
        case 'c':
            p[0] = va_arg(ap, int);
            p[1] = 0;
            prec = 1;
            break;
        case 'n':
            *va_arg(ap, int *) = bp - buf;
            p[0] = 0;
            break;
        case '%':
            p[0] = '%';
            p[1] = 0;
            prec = 1;
            break;
        default:
            X_STORE(c);
            continue;
        }

        if (base != 0) {
            _itoa(p, (unsigned int)num, base, (c == 'X'), prec);
            if (prec) {
                fillz = 0;
            }
            p = tmp;
            prec = X_INF;
        }

        if ((plen = sal_strlen(p)) > prec) {
            plen = prec;
        }

        if (width < plen) {
            width = plen;
        }

        pad = width - plen;

        while (!ljust && pad-- > 0) {
            X_STORE(fillz ? '0' : ' ');
        }
        for (; plen-- > 0 && width-- > 0; p++) {
            X_STORE(*p);
        }
        while (pad-- > 0) {
            X_STORE(' ');
        }
    }

    if ((be == b_inf) || (bp < be)) {
        *bp = 0;
    } else {
        /* coverity[var_deref_op] */
        *be = 0;
    }

    return (bp - buf);
}

#endif
