/*! \file bcmpkt_buf.c
 *
 * Interfaces for packet buffer management.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bsl/bsl.h>
#include <bcmpkt/bcmpkt_buf.h>
#include "bcmpkt_buf_internal.h"

#define BSL_LOG_MODULE          BSL_LS_BCMPKT_PACKET

/* Minimum allocation size. */
#define BCMPKT_ALLOC_LEN_MIN    64

int
bcmpkt_alloc(int unit, uint32_t len, uint32_t flags, bcmpkt_packet_t **packet)
{
    bcmpkt_packet_t *pkt = NULL;
    uint32_t extr_size;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    /* Overwrite pointer is not allowed. */
    if (*packet) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit,
                              "Allocate buffer for not NULL pointer\n")));
        return SHR_E_EXISTS;
    }

    if (len < BCMPKT_ALLOC_LEN_MIN) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit, "Len (%d) is too small\n"), len));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    pkt = bcmpkt_ppool_alloc();
    SHR_NULL_CHECK(pkt, SHR_E_MEMORY);

    sal_memset(pkt, 0, sizeof(*pkt));

    extr_size = (flags & BCMPKT_BUF_F_TX) ? BCMPKT_TX_HDR_RSV : 0;
    SHR_IF_ERR_EXIT
        (bcmpkt_data_buf_alloc(unit, len + extr_size, &pkt->data_buf));

    bcmpkt_reserve(pkt->data_buf, extr_size);
    bcmpkt_pmd_format(pkt);
    pkt->unit = unit;
    *packet = pkt;

exit:
    if (SHR_FUNC_ERR() && pkt != NULL) {
        sal_free(pkt);
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_free(int unit, bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (packet->data_buf != NULL) {
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_free(packet->unit, packet->data_buf));
    }
    SHR_IF_ERR_EXIT
        (bcmpkt_ppool_free(packet));

exit:
    SHR_FUNC_EXIT();
}

uint8_t *
bcmpkt_reserve(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    int tailroom;

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }
    assert(dbuf->data >= dbuf->head && dbuf->len >= dbuf->data_len);

    tailroom = dbuf->len - dbuf->data_len - (dbuf->data - dbuf->head);
    assert(tailroom >= 0);

    /* Reserve function works for blank packet (len=0) only */
    if (dbuf->data_len == 0 && (uint32_t)tailroom >= len) {
        dbuf->data += len;
        return dbuf->data;
    } else {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("Reserve space failed\n")));
        return NULL;
    }
}

uint8_t *
bcmpkt_push(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    int headroom;

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }

    headroom = dbuf->data - dbuf->head;
    assert(headroom >= 0);
    if ((uint32_t)headroom >= len) {
        dbuf->data -= len;
        dbuf->data_len  += len;
        return dbuf->data;
    }
    LOG_ERROR(BSL_LS_BCMPKT_PACKET,
              (BSL_META("Not enough space: Request - %dB, headroom - %dB\n"),
               len, headroom));
    return NULL;
}

uint8_t *
bcmpkt_put(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    int tailroom;

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }

    assert(dbuf->data >= dbuf->head &&
           dbuf->len >= dbuf->data_len);

    tailroom = dbuf->len - dbuf->data_len - (dbuf->data - dbuf->head);
    assert(tailroom >= 0);
    if ((uint32_t)tailroom >= len) {
        uint8_t *addr = dbuf->data + dbuf->data_len;
        dbuf->data_len  += len;
        return addr;
    }

    LOG_ERROR(BSL_LS_BCMPKT_PACKET,
              (BSL_META("Not enough space: Request - %dB, tailroom - %dB\n"),
               len, tailroom));
    return NULL;
}

uint8_t *
bcmpkt_pull(bcmpkt_data_buf_t *dbuf, uint32_t len)
{

    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return NULL;
    }

    if (dbuf->data_len >= len) {
        dbuf->data += len;
        dbuf->data_len  -= len;
        return dbuf->data;
    }

    LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("Invalid length\n")));

    return NULL;
}

int
bcmpkt_trim(bcmpkt_data_buf_t *dbuf, uint32_t len)
{
    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return SHR_E_PARAM;
    }

    if (dbuf->data_len > len) {
        dbuf->data_len = len;
    }

    return SHR_E_NONE;
}

uint32_t
bcmpkt_headroom(bcmpkt_data_buf_t *dbuf)
{
    int headroom;
    if (!dbuf) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return 0;
    }

    headroom = dbuf->data - dbuf->head;
    assert(headroom >= 0);

    return (uint32_t)headroom;
}

uint32_t
bcmpkt_tailroom(bcmpkt_data_buf_t *dbuf)
{
    int tailroom;

    if (dbuf == NULL) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET, (BSL_META("NULL pointer\n")));
        return 0;
    }

    assert(dbuf->data >= dbuf->head &&
           dbuf->len >= dbuf->data_len);

    tailroom = dbuf->len - dbuf->data_len - (dbuf->data - dbuf->head);
    assert(tailroom >= 0);

    if (tailroom < 0) {
        tailroom = 0;
    }
    return (uint32_t)tailroom;
}

int
bcmpkt_data_buf_alloc(int unit, uint32_t size, bcmpkt_data_buf_t **dbuf)
{
    bcmpkt_data_buf_t *buf;
    uint32_t req_size;
    uint32_t buf_size = 0;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    if (*dbuf) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **data_buf content should be NULL\n")));
    }

    if (size < BCMPKT_ALLOC_LEN_MIN) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit, "Size (%d) is too small\n"), size));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Add buffer descriptor space.
     * A descriptor is embedded in the front of each buffer to provide
     * convenience for buffer operation. (e.g. add/remove header)
     */
    req_size = size + sizeof(bcmpkt_data_buf_t);
    /* Overflow check. */
    if (size > req_size) {
        LOG_ERROR(BSL_LS_BCMPKT_PACKET,
                  (BSL_META_U(unit, "Size (%d) is too large\n"), size));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    buf = (bcmpkt_data_buf_t *)bcmpkt_bpool_alloc(unit, req_size, &buf_size);
    SHR_NULL_CHECK(buf, SHR_E_MEMORY);

    /* Format buffer descriptor. */
    buf->head = (uint8_t *)buf + sizeof(bcmpkt_data_buf_t);
    buf->len = buf_size - sizeof(bcmpkt_data_buf_t);
    buf->data = buf->head;
    buf->data_len = 0;
    buf->ref_count = 1;
    *dbuf = buf;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_data_buf_free(int unit, bcmpkt_data_buf_t *dbuf)
{
    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);

    if ((--dbuf->ref_count) > 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Avoid double free crashes buffer pool link list. */
    if (dbuf->ref_count < 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Unexpected free\n")));
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_free(unit, (void *)dbuf));

exit:
    /* Restore ref_count if the buffer does not belong to the pool or
     * could not get lock.
     */
    if (SHR_FUNC_VAL_IS(SHR_E_MEMORY) || SHR_FUNC_VAL_IS(SHR_E_FAIL)) {
        dbuf->ref_count++;
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_packet_claim(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt)
{
    bcmpkt_packet_t *npkt = NULL;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(new_pkt, SHR_E_PARAM);
    if (*new_pkt) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **new_pkt content should be NULL\n")));
    }

    npkt = bcmpkt_ppool_alloc();
    SHR_NULL_CHECK(npkt, SHR_E_MEMORY);

    sal_memset(npkt, 0, sizeof(bcmpkt_packet_t));

    if (unit == pkt->unit) {
        npkt->data_buf = pkt->data_buf;
        /* Detach data buffer from pkt. */
        pkt->data_buf = NULL;
    } else {
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_copy(unit, pkt->data_buf, &npkt->data_buf));
    }

    /* Copy packet structure information and metadata. */
    bcmpkt_pmd_format(npkt);
    sal_memcpy(npkt->pmd.data, pkt->pmd.data, sizeof(npkt->pmd.data));
    npkt->unit = unit;
    npkt->flags = pkt->flags;
    npkt->type = pkt->type;
    *new_pkt = npkt;

exit:
    if (SHR_FUNC_ERR() && npkt != NULL) {
        sal_free(npkt);
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_packet_clone(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt)
{
    bcmpkt_packet_t *npkt = NULL;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(new_pkt, SHR_E_PARAM);
    if (*new_pkt) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **new_pkt content should be NULL\n")));
    }

    npkt = bcmpkt_ppool_alloc();
    SHR_NULL_CHECK(npkt, SHR_E_MEMORY);

    sal_memset(npkt, 0, sizeof(bcmpkt_packet_t));

    /* Reference the buffer. */
    npkt->data_buf = pkt->data_buf;
    SHR_NULL_CHECK(npkt->data_buf, SHR_E_FAIL);
    /* If ref_count has been clear, return failure. */
    if ((++(npkt->data_buf->ref_count)) <= 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Copy packet structure information and metadata. */
    bcmpkt_pmd_format(npkt);
    sal_memcpy(npkt->pmd.data, pkt->pmd.data, sizeof(npkt->pmd.data));
    npkt->unit = pkt->unit;
    npkt->flags = pkt->flags;
    npkt->type = pkt->type;
    *new_pkt = npkt;

exit:
    if (SHR_FUNC_ERR() && npkt != NULL) {
        sal_free(npkt);
    }
    SHR_FUNC_EXIT();
}

int
bcmpkt_data_buf_copy(int unit, bcmpkt_data_buf_t *dbuf,
                     bcmpkt_data_buf_t **new_dbuf)
{
    bcmpkt_data_buf_t *buf = NULL;
    uint32_t headroom;

    SHR_FUNC_ENTER(unit);

    if (!(unit == BCMPKT_BPOOL_SHARED_ID || bcmdrd_dev_exists(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(new_dbuf, SHR_E_PARAM);

    if (*new_dbuf) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "The **new_buf content should be NULL\n")));
    }

    headroom = bcmpkt_headroom(dbuf);
    SHR_IF_ERR_EXIT
        (bcmpkt_data_buf_alloc(unit, dbuf->data_len + headroom, &buf));
    assert(bcmpkt_reserve(buf, headroom));
    assert(bcmpkt_put(buf, dbuf->data_len));
    sal_memcpy(buf->data, dbuf->data, dbuf->data_len);
    buf->ref_count = 1;
    *new_dbuf = buf;

exit:
    SHR_FUNC_EXIT();
}

