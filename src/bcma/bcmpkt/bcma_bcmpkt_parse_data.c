/*! \file bcma_bcmpkt_parse_data.c
 *
 * Parse data drivers for bcmpkt command.
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

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include "./cmd/bcma_bcmpktcmd_internal.h"

/* Module header Op-Codes */
#define HIGIG_OP_CPU        0x00 /* CPU Frame */
#define HIGIG_OP_UC         0x01 /* Unicast Frame */
#define HIGIG_OP_BC         0x02 /* Broadcast or DLF frame */
#define HIGIG_OP_MC         0x03 /* Multicast Frame */
#define HIGIG_OP_IPMC       0x04 /* IP Multicast Frame */

static int parse_bitmap_str(const char *str, SHR_BITDCL *bits, int size)
{
    int rv = 0;
    int bit = 0;
    int pstart = -1;
    char ch;
    const char *cptr = str;

    SHR_BITCLR_RANGE(bits, 0, size);

    if (!sal_strcasecmp(str, "none")) {
        return rv;
    }

    do {
        ch = *cptr++;
        if (ch >= '0' && ch <= '9') {
            bit = (bit * 10) + (ch - '0');
        } else {
            if (pstart >= 0) {
                while (pstart < bit) {
                    SHR_BITSET(bits, pstart);
                    pstart++;
                }
                pstart = -1;
            }
            if (ch == ',' || ch == 0) {
                SHR_BITSET(bits, bit);
                bit = 0;
            } else if (ch == '-') {
                pstart = bit;
                bit = 0;
            } else {
                rv = -1;
                break;
            }
        }
    } while (ch != 0);

    return rv;
}

static int
bmp_parse_arg(const char *arg, void *data, void **option)
{
    SHR_BITDCL *bits;
    int size;

    if (data == NULL || option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }
    bits = (SHR_BITDCL *)data;
    size = **(int **)option;
    if (parse_bitmap_str(arg, bits, size) != 0) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("Input error!\n")));
        return -1;
    }

    return 0;
}

static char *
bmp_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    SHR_BITDCL *bits;
    int size;
    int len = 0;
    int bit;

    if (data == NULL || option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return NULL;
    }
    bits = (SHR_BITDCL *)data;
    size = *(int *)option;

    sal_memset(buf, 0, bufsz);
    for (bit = 0; bit < size; bit++) {
        if (bits[(bit) / SHR_BITWID] == 0) {
            bit += (SHR_BITWID - 1);
        }
        else if (SHR_BITGET(bits, bit)) {
            len += sal_snprintf(buf + len, bufsz - len, "%d,", bit);
        }
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_bmp = {
    "bmp",
    bmp_parse_arg,
    bmp_format_arg,
    NULL
};

static int
txpmd_parse_arg(const char *arg, void *data, void **option)
{
    bcma_txpmd_t *txpmd = (bcma_txpmd_t *)data;
    uint32_t val;
    int fid;

    if (txpmd == NULL || txpmd->data == NULL ||
        option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }
    fid = **(int **) option;

    if (bcmpkt_txpmd_field_get(txpmd->dev_type, txpmd->data,
                               BCMPKT_TXPMD_START, &val)) {
        return -1;
    }
    if (val == 0) {
        bcma_bcmpkt_txpmd_init(txpmd->dev_type, txpmd->data);
    }

    val = sal_strtoul(arg, NULL, 0);

    return bcmpkt_txpmd_field_set(txpmd->dev_type, txpmd->data, fid, val);
}

static char *
txpmd_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    bcma_txpmd_t *txpmd = (bcma_txpmd_t *)data;
    int len = 0;
    uint32_t val;
    int fid;

    if (txpmd == NULL || txpmd->data == NULL || option == NULL || buf == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return buf;
    }
    sal_memset(buf, 0, bufsz);
    fid = *(int *)option;
    if (!bcmpkt_txpmd_field_get(txpmd->dev_type, txpmd->data, fid, &val)) {
        len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_txpmd = {
    "txpmd",
    txpmd_parse_arg,
    txpmd_format_arg,
    NULL
};

static int
lbhdr_parse_arg(const char *arg, void *data, void **option)
{
    bcma_lbhdr_t *lbhdr = (bcma_lbhdr_t *)data;
    uint32_t val;
    int fid;

    if (lbhdr == NULL || lbhdr->data == NULL ||
        option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }
    fid = **(int **) option;

    if (bcmpkt_lbhdr_field_get(lbhdr->dev_type, lbhdr->data,
                               BCMPKT_LBHDR_START, &val)) {
        return -1;
    }
    if (val == 0) {
        bcma_bcmpkt_lbhdr_init(lbhdr->dev_type, lbhdr->data);
    }

    val = sal_strtoul(arg, NULL, 0);

    return bcmpkt_lbhdr_field_set(lbhdr->dev_type, lbhdr->data, fid, val);
}

static char *
lbhdr_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    bcma_lbhdr_t *lbhdr = (bcma_lbhdr_t *)data;
    int len = 0;
    uint32_t val;
    int fid;

    if (lbhdr == NULL || lbhdr->data == NULL || option == NULL || buf == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return buf;
    }
    sal_memset(buf, 0, bufsz);
    fid = *(int *)option;
    if (!bcmpkt_lbhdr_field_get(lbhdr->dev_type, lbhdr->data, fid, &val)) {
        len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_lbhdr = {
    "lbhdr",
    lbhdr_parse_arg,
    lbhdr_format_arg,
    NULL
};

static void
higig_init(HIGIG_t *mh)
{
    sal_memset(mh, 0, sizeof(*mh));

    HIGIG_STARTf_SET(*mh, BCMPKT_HIGIG_SOF);
    HIGIG_OPCODEf_SET(*mh, HIGIG_OP_UC);

    /*!FIXME: my modid get and set. */

}

static void
higig2_init(HIGIG2_t *mh)
{
    sal_memset(mh, 0, sizeof(*mh));

    HIGIG2_STARTf_SET(*mh, BCMPKT_HIGIG2_SOF);
    HIGIG2_PPD_TYPEf_SET(*mh, 2);
    HIGIG2_PPD2_OPCODEf_SET(*mh, HIGIG_OP_UC);

    /*!FIXME: my modid get and set. */
}

static int
hg_sym_field_get(uint32_t sflags, uint32_t size, uint16_t minbit,
                 uint16_t maxbit, uint32_t *sbuf, uint32_t *fbuf)
{
    if (sflags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        bcmdrd_field_be_get(sbuf, size, minbit, maxbit, fbuf);
    } else {
        bcmdrd_field_get(sbuf, minbit, maxbit, fbuf);
    }
    return 0;
}

static int
hg_sym_field_set(uint32_t sflags, uint32_t size, uint16_t minbit,
                 uint16_t maxbit, uint32_t *sbuf, uint32_t *fbuf)
{
    if (sflags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        bcmdrd_field_be_set(sbuf, size, minbit, maxbit, fbuf);
    } else {
        bcmdrd_field_set(sbuf, minbit, maxbit, fbuf);
    }
    return 0;
}

/* Parse data driver for type "higig". */
static int
higig_parse_arg(const char *arg, void *data, void **option)
{
    HIGIG_t *mh = (HIGIG_t *)data;
    higig_symbl_info_t *info;
    uint32_t val = 0;

    if (mh == NULL || option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }

    info = *(higig_symbl_info_t **)option;

    val = HIGIG_STARTf_GET(*mh);

    if (val != BCMPKT_HIGIG_SOF ) {
        if (val != 0) {
            LOG_WARN(BSL_LS_APPL_PACKET,
                     (BSL_META("Switch module header (start=0x%x) to HIGIG\n"),
                       val));
        }
        higig_init(mh);
    }

    val = sal_strtoul(arg, NULL, 0);
    /*
     * No Higig field size bigger than 32 bits.
     */
    /* coverity[address_of] */
    hg_sym_field_set(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                     info->maxbit, (uint32_t *)mh, &val);

    return 0;
}

static char *
higig_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int len;
    HIGIG_t *mh = (HIGIG_t *)data;
    higig_symbl_info_t *info;
    uint32_t val = 0;

    if (mh == NULL || option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return "<null pointer>";
    }

    info = (higig_symbl_info_t *)option;
    hg_sym_field_get(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                     info->maxbit, (uint32_t *)mh, &val);

    sal_memset(buf, 0, bufsz);
    len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_higig = {
    "higig",
    higig_parse_arg,
    higig_format_arg,
    NULL
};

static int
higig2_parse_arg(const char *arg, void *data, void **option)
{
    HIGIG2_t *mh = (HIGIG2_t *)data;
    higig_symbl_info_t *info;
    uint32_t val[2]={0,};

    if (mh == NULL || option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }

    info = *(higig_symbl_info_t **)option;

    val[0] = HIGIG2_STARTf_GET(*mh);

    if (val[0] != BCMPKT_HIGIG2_SOF ) {
        if (val[0] != 0) {
            LOG_WARN(BSL_LS_APPL_PACKET,
                     (BSL_META("Switch module header (start=0x%x) to HIGIG2\n"),
                      val[0]));
        }
        higig2_init(mh);
    }

    if (!sal_strcasecmp(info->name, "ppd_data")) {
        int len=0;
        char str1[9];
        char str2[9];
        char *strtmp;
        sal_memset(str1, 0, 9);
        sal_memset(str2, 0, 9);
        if (arg[0] == 'x' || arg[0] == 'X') {
            arg++;
        }
        else if (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) {
            arg += 2;
        }
        len = sal_strlen(arg);
        if (len > 16) {
            return -1;
        }

        if (len <= 8) {
            val[0] = sal_strtoul(arg, &strtmp, 16);
            if (strtmp != NULL && sal_strlen(strtmp) > 0) {
                return -1;
            }
        }
        else {
            sal_memcpy(str1, arg + len - 8, 8);
            val[0] = sal_strtoul(str1, &strtmp, 16);
            if (strtmp != NULL && sal_strlen(strtmp) > 0) {
                return -1;
            }
            sal_memcpy(str2, arg, len - 8);
            val[1] = sal_strtoul(str2, &strtmp, 16);
            if (strtmp != NULL && sal_strlen(strtmp) > 0) {
                return -1;
            }
        }
        hg_sym_field_set(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                         info->maxbit, (uint32_t *)mh, val);
    }
    else {
        val[0] = sal_strtoul(arg, NULL, 0);
        hg_sym_field_set(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                         info->maxbit, (uint32_t *)mh, val);
    }

    return 0;
}

static char *
higig2_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int len = 0;
    HIGIG2_t *mh = (HIGIG2_t *)data;
    higig_symbl_info_t *info;
    uint32_t val[2]={0,};

    if (mh == NULL || option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return "<null pointer>";
    }

    info = (higig_symbl_info_t *)option;
    hg_sym_field_get(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                     info->maxbit, (uint32_t *)mh, val);
    sal_memset(buf, 0, bufsz);
    len = sal_snprintf(buf, bufsz, "0x");
    if (!sal_strcasecmp(info->name, "HG2ppd_data")) {
        len += sal_snprintf(buf + len, bufsz, "%08x ", val[1]);
        len += sal_snprintf(buf + len, bufsz, "%08x", val[0]);
    }
    else {
        len += sal_snprintf(buf + len, bufsz, "0x%x(%d)", val[0], val[0]);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_higig2 = {
    "higig2",
    higig2_parse_arg,
    higig2_format_arg,
    NULL
};

int
bcma_bcmpkt_parse_data_add(void)
{
    bcma_cli_parse_data_add(&parse_data_txpmd);
    bcma_cli_parse_data_add(&parse_data_lbhdr);
    bcma_cli_parse_data_add(&parse_data_higig);
    bcma_cli_parse_data_add(&parse_data_higig2);
    bcma_cli_parse_data_add(&parse_data_bmp);

    return BCMA_CLI_CMD_OK;
}

