/*! \file bcmbd_cmicx_schan.h
 *
 * S-Channel Message: data structure used by firmware to transport
 * S-Channel Data into SOC via CMIC.
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

#ifndef BCMBD_CMICX_SCHAN_H
#define BCMBD_CMICX_SCHAN_H

#include <bcmdrd/bcmdrd_types.h>

/*! \cond */

#define CMIC_SCHAN_WORDS_ALLOC  22

typedef uint32_t schan_header_t;

#define SCMH_NACK_GET(d)        F32_GET(d,0,1)
#define SCMH_NACK_SET(d,v)      F32_SET(d,0,1,v)
#define SCMH_BANK_GET(d)        F32_GET(d,1,2)
#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define SCMH_DMA_GET(d)         F32_GET(d,3,1)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_ECODE_GET(d)       F32_GET(d,4,2)
#define SCMH_ECODE_SET(d,v)     F32_SET(d,4,2,v)
#define SCMH_EBIT_GET(d)        F32_GET(d,6,1)
#define SCMH_EBIT_SET(d,v)      F32_SET(d,6,1,v)
#define SCMH_DATALEN_GET(d)     F32_GET(d,7,7)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_ACCTYPE_GET(d)     F32_GET(d,14,5)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DSTBLK_GET(d)      F32_GET(d,19,7)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_OPCODE_GET(d)      F32_GET(d,26,6)
#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)

/*
 * Individual S-Channel message formats.
 * Different ways of peeking and poking at an S-Channel message
 * packet.  Applicable message types are listed inside each structure.
 */

typedef struct schan_msg_plain_s {
    /* GBP Full Notification */
    /* GBP Available Notification */
    /* Write Memory Ack */
    /* Write Register Ack */
    /* ARL Insert Complete */
    /* ARL Delete Complete */
    /* Memory Failed Notification */
    /* Initialize CFAP (Cell FAP) */
    /* Initialize SFAP (Slot FAP) */
    /* Enter Debug Mode */
    /* Exit Debug Mode */
    schan_header_t header;
} schan_msg_plain_t;

typedef struct schan_msg_bitmap_s {
    /* Back Pressure Warning Status */
    /* Back Pressure Discard Status */
    /* Link Status Notification (except 5695) */
    /* COS Queue Status Notification */
    /* HOL Status Notification */
    schan_header_t header;
    uint32_t bitmap;
    uint32_t bitmap_word1;  /* 5665 only, so far */
} schan_msg_bitmap_t;

typedef struct schan_msg_readcmd_s {
    /* Read Memory Command */
    /* Read Register Command */
    schan_header_t header;
    uint32_t address;
} schan_msg_readcmd_t;

typedef struct schan_msg_readresp_s {
    /* Read Memory Ack */
    /* Read Register Ack */
    schan_header_t header;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_readresp_t;

typedef struct schan_msg_writecmd_s {
    /* Write Memory Command */
    /* Write Register Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_writecmd_t;

typedef struct schan_msg_arlins_s {
    /* ARL Insert Command */
    /* (Also: ARL Message Buffer Format) */
    /* (Also: ARL DMA Message Format) */
    schan_header_t header;
    uint32_t data[3];
} schan_msg_arlins_t;

typedef struct schan_msg_arldel_s {
    /* ARL Delete Command */
    schan_header_t header;
    uint32_t data[2];
} schan_msg_arldel_t;

typedef struct schan_msg_arllkup_s {
    /* ARL Lookup Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[2];
} schan_msg_arllkup_t;

typedef struct schan_msg_l3ins_s {
    /* L3 Insert Command */
    schan_header_t header;
    uint32_t data[4];
} schan_msg_l3ins_t;

typedef struct schan_msg_l3del_s {
    /* L3 Delete Command */
    schan_header_t header;
    uint32_t data[4];
} schan_msg_l3del_t;

typedef struct schan_msg_l3lkup_s {
    /* L3 Lookup Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[4];
} schan_msg_l3lkup_t;

typedef struct schan_msg_l2x2_s {
    /* L2 Insert/Delete/Lookup Command 56120 */
    schan_header_t header;
    uint32_t data[3];
} schan_msg_l2x2_t;

typedef struct schan_msg_l3x2_s {
    /* L3 Insert/Delete/Lookup Command 56120 */
    schan_header_t header;
    uint32_t data[13];
} schan_msg_l3x2_t;

typedef struct schan_msg_gencmd_s {
    /* Generic table Insert/Delete/Lookup Command 5661x */
    schan_header_t header;
    uint32_t address;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_gencmd_t;

typedef struct schan_msg_genresp_s {
    /* Generic table Insert/Delete/Lookup Command 5661x */
    schan_header_t header;
    uint32_t response;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_genresp_t;

typedef struct schan_msg_popcmd_s {
    /* Pop Memory Command */
    schan_header_t header;
    uint32_t address;
} schan_msg_popcmd_t;

typedef struct schan_msg_popresp_s {
    /* Pop Memory Response */
    schan_header_t header;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_popresp_t;

typedef struct schan_msg_pushcmd_s {
    /* Push Memory Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_pushcmd_t;

typedef struct schan_msg_pushresp_s {
    /* Push Memory Response */
    schan_header_t header;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_pushresp_t;

/*
 * Union of all S-Channel message types (use to declare all message buffers)
 *
 * When building messages, address the union according to packet type.
 * When writing to PCI, address data.dwords.
 * When writing to I2C, address data.bytes.
 */

#define SCHAN_MSG_CLEAR(m)      ((m)->header = 0)

typedef union schan_msg_u {
    schan_header_t header;
    schan_msg_plain_t plain;
    schan_msg_bitmap_t bitmap;
    schan_msg_readcmd_t readcmd;
    schan_msg_readresp_t readresp;
    schan_msg_writecmd_t writecmd;
    schan_msg_arlins_t arlins;
    schan_msg_arldel_t arldel;
    schan_msg_arllkup_t arllkup;
    schan_msg_l3ins_t l3ins;
    schan_msg_l3del_t l3del;
    schan_msg_l3lkup_t l3lkup;
    schan_msg_l2x2_t l2x2;
    schan_msg_l3x2_t l3x2;
    schan_msg_gencmd_t gencmd;
    schan_msg_genresp_t genresp;
    schan_msg_popcmd_t  popcmd;
    schan_msg_popresp_t popresp;
    schan_msg_pushcmd_t  pushcmd;
    schan_msg_pushresp_t pushresp;
    uint32_t dwords[CMIC_SCHAN_WORDS_ALLOC];
    uint8_t bytes[sizeof(uint32_t) * CMIC_SCHAN_WORDS_ALLOC];
} schan_msg_t;

/*
 * S-Channel Message Types
 */

#define BP_WARN_STATUS_MSG            0x01
#define BP_DISCARD_STATUS_MSG         0x02
#define COS_QSTAT_NOTIFY_MSG          0x03      /* Not on XGS */
#define IPIC_HOL_STAT_MSG             0x03      /* 5665 (alias) */
#define HOL_STAT_NOTIFY_MSG           0x04
#define GBP_FULL_NOTIFY_MSG           0x05      /* 5605/5615/5625/xgsd only */
#define GBP_AVAIL_NOTIFY_MSG          0x06      /* 5605/5615/5625/xgsd only */
#define READ_MEMORY_CMD_MSG           0x07
#define READ_MEMORY_ACK_MSG           0x08
#define WRITE_MEMORY_CMD_MSG          0x09
#define WRITE_MEMORY_ACK_MSG          0x0a
#define READ_REGISTER_CMD_MSG         0x0b
#define READ_REGISTER_ACK_MSG         0x0c
#define WRITE_REGISTER_CMD_MSG        0x0d
#define WRITE_REGISTER_ACK_MSG        0x0e
#define ARL_INSERT_CMD_MSG            0x0f
#define ARL_INSERT_DONE_MSG           0x10
#define ARL_DELETE_CMD_MSG            0x11
#define ARL_DELETE_DONE_MSG           0x12
#define LINKSTAT_NOTIFY_MSG           0x13      /* Strata I/II only */
#define MEMORY_FAIL_NOTIFY            0x14
#define INIT_CFAP_MSG                 0x15      /* 5690 only */
#define IPIC_GBP_FULL_MSG             0x15      /* 5665 (alias) */
#define INIT_SFAP_MSG                 0x16      /* 5605/5615/5625 only */
#define IPIC_GBP_AVAIL_MSG            0x16      /* 5665 (alias) */
#define ENTER_DEBUG_MODE_MSG          0x17
#define EXIT_DEBUG_MODE_MSG           0x18
#define ARL_LOOKUP_CMD_MSG            0x19
#define L3_INSERT_CMD_MSG             0x1a
#define L3_INSERT_DONE_MSG            0x1b
#define L3_DELETE_CMD_MSG             0x1c
#define L3_DELETE_DONE_MSG            0x1d
#define L3_LOOKUP_CMD_MSG             0x1e      /* 5695 */
#define L2_LOOKUP_CMD_MSG             0x20      /* 56120 */
#define L2_LOOKUP_ACK_MSG             0x21      /* 56120 */
#define L3X2_LOOKUP_CMD_MSG           0x22      /* 56120 */
#define L3X2_LOOKUP_ACK_MSG           0x23      /* 56120 */
#define TABLE_INSERT_CMD_MSG          0x24      /* 5662x/5668x */
#define TABLE_INSERT_DONE_MSG         0x25      /* 5662x/5668x */
#define TABLE_DELETE_CMD_MSG          0x26      /* 5662x/5668x */
#define TABLE_DELETE_DONE_MSG         0x27      /* 5662x/5668x */
#define TABLE_LOOKUP_CMD_MSG          0x28      /* 5662x/5668x */
#define TABLE_LOOKUP_DONE_MSG         0x29      /* 5662x/5668x */
#define FIFO_POP_CMD_MSG              0x2a      /* 5661x */
#define FIFO_POP_DONE_MSG             0x2b      /* 5661x */
#define FIFO_PUSH_CMD_MSG             0x2c      /* 5661x */
#define FIFO_PUSH_DONE_MSG            0x2d      /* 5661x */

/*
 * Schan error types
 */
typedef enum soc_schan_err_e {
    SOC_SCERR_CFAP_OVER_UNDER,
    SOC_SCERR_SDRAM_CHKSUM,
    SOC_SCERR_UNEXP_FIRST_CELL,
    SOC_SCERR_MMU_SOFT_RST,
    SOC_SCERR_CBP_CELL_CRC,
    SOC_SCERR_CBP_HEADER_PARITY,
    SOC_SCERR_MMU_NPKT_CELLS,
    SOC_SCERR_MEMORY_PARITY,
    SOC_SCERR_PLL_DLL_LOCK_LOSS,
    SOC_SCERR_CELL_PTR_CRC,
    SOC_SCERR_CELL_DATA_CRC,
    SOC_SCERR_FRAME_DATA_CRC,
    SOC_SCERR_CELL_PTR_BLOCK_CRC,
    SOC_SCERR_MULTIPLE_ERR,
    SOC_SCERR_INVALID
} soc_schan_err_t;

#define CMICX_SCHAN_NUM_MAX      3

/* Transact an S-Channel message to CMIC */
extern int bcmbd_cmicx_schan_op(int unit, schan_msg_t *msg,
                                int dwc_write, int dwc_read);

/*! \endcond */

#endif  /* BCMBD_CMICX_SCHAN_H */
