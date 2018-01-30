/*! \file bcmpkt_buf.h
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

#ifndef BCMPKT_BUF_H
#define BCMPKT_BUF_H

#include <sal/sal_types.h>
#include <bcmpkt/bcmpkt_net.h>

/*! Shared buff pool ID */
#define BCMPKT_BPOOL_SHARED_ID     (-1)

/*! Default packet count. */
#define BCMPKT_PPOOL_COUNT_DEF      128

/*! Default per device buffer count. */
#define BCMPKT_BPOOL_BCOUNT_DEF     256

/*! Minimum buffer size. */
#define BCMPKT_BPOOL_BSIZE_MIN      (1536)

/*! Minimum packet size. (not include CRC) */
#define BCMPKT_FRAME_SIZE_MIN       (60)

/*! Buffer maximum operation overhead.
 * sizeof(bcmpkt_data_buf_t) + maximum metadata size.
 */
#define BCMPKT_BUF_SIZE_OVERHEAD    (BCMPKT_RCPU_MAX_ENCAP_SIZE + \
                                     sizeof(bcmpkt_data_buf_t))

/**
 * \name BCMPKT packet copy flags.
 * \anchor BCMPKT_PKT_CP_F_XXX
 */
/*! \{ */
/* TX overhead will be counted into alloc size. */
#define BCMPKT_BUF_F_TX             (0x1 << 0)
/*! \} */

/*! Packet buffer pool structure. */
typedef struct bcmpkt_bpool_status_s {
    /*! Active status. */
    bool active;
    /*! Each buffer size (including operation overhead size). */
    uint32_t buf_size;
    /*! Total buffer count. */
    uint32_t buf_count;
    /*! Available buffer count. */
    uint32_t free_count;
} bcmpkt_bpool_status_t;

/*!
 * \brief Allocate packet object and its buffer.
 *
 * Create packet object, allocate data buffer and private header space (internal
 * use only), and format Packet MetaData structure (including Higig information).
 *
 * \param [in] unit Switch unit number.
 * \param [in] len The size packet data buffer to be allocated (unit is byte).
 * \param [in] flags Reserved.
 * \param [out] packet Packet handle.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input len is too small.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_alloc(int unit, uint32_t len, uint32_t flags, bcmpkt_packet_t **packet);

/*!
 * \brief Release packet data buffer and destroy packet object.
 *
 * \param [in] unit Switch unit number.
 * \param [in] packet Packet handle.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Parameter is NULL.
 */
extern int
bcmpkt_free(int unit, bcmpkt_packet_t *packet);

/*!
 * \brief Reserve buffer size in headroom.
 *
 * This function is to reserve buffer in headroom for possible header
 * encapsulation or add tag. This function works for empty packet data (len = 0)
 * only.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Bytes to reserve.
 *
 * \return A pointer to the updated packet->data_buf->data.
 */
extern uint8_t *
bcmpkt_reserve(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Add extra size in the front of packet data.
 *
 * This function extends the used data area in the beginning when the headroom
 * has enough space for the request.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Bytes to add.
 *
 * \return A pointer to the first byte of the extra data. NULL for failure.
 */
extern uint8_t *
bcmpkt_push(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Add bytes in the end of a packet data.
 *
 * This function extends the used data area of the buffer at the buffer end when
 * the tailroom has enough space for the request. The pointer of the first byte
 * of the extra data is returned.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Number of bytes to add.
 *
 * \return A pointer to the first byte of the extra data. NULL for failure.
 */
extern uint8_t *
bcmpkt_put(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Remove bytes from the start of packet data.
 *
 * This function removes some bytes from the start of a packet data, returning
 * the memory to the headroom. A pointer to the next data in the packet data
 * buffer is returned. Once the data has been pulled future pushes will
 * overwrite the old data.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Bytes to be removed.
 *
 * \return A pointer to the next data in the buffer.
 */
extern uint8_t *
bcmpkt_pull(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Remove end from packet buffer.
 *
 * Cut the length of a buffer down by removing data from the tail. If
 * the buffer is already under the length specified it is not modified.
 * The skb must be linear.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len New data length.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_PARAM Input len is too small.
 */
extern int
bcmpkt_trim(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Get headroom size.
 *
 * Get free space size before packet data in the packet buffer.
 *
 * \param [in] dbuf Packet data buffer handle.
 *
 * \return Available bytes for push.
 */
extern uint32_t
bcmpkt_headroom(bcmpkt_data_buf_t *dbuf);

/*!
 * \brief Get tailroom size.
 *
 *  Get free space after packet data in the packet buffer.
 *
 * \param [in] dbuf Packet data buffer handle.
 *
 * \return Available bytes for put.
 */
extern uint32_t
bcmpkt_tailroom(bcmpkt_data_buf_t *dbuf);

/*!
 * \brief Create a buffer pool.
 *
 * The \c buf_size must be large enough to hold the components show below,
 * where (m) denotes mandatory components (part of all packets) and (o)
 * denotes optional components.
 *
 *  (m) Maximum size ethernet packet
 *  (m) Buffer control (bcmpkt_data_buf_t)
 *  (o) RCPU header
 *  (o) Tx meta data (Tx PMD)
 *  (o) Loopback header
 *  (o) HiGig header
 *
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID means the pool shared among units.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] buf_size Number of bytes for each buffer.
 * \param [in] buf_count Number of buffers.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_bpool_create(int unit, int buf_size, int buf_count);

/*!
 * \brief Destroy a buffer pool.
 *
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID means destroy the shared pool.
 *
 * \param [in] unit Switch unit number.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_bpool_destroy(int unit);

/*!
 * \brief Destroy all buffer pools.
 *
 * \retval SHR_E_NONE Succeed.
 */
extern int
bcmpkt_bpool_cleanup(void);

/*!
 * \brief Fetch a buffer pool status.
 *
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID is for the shared pool.
 *
 * \param [in] unit Switch unit number.
 * \param [in] status Buffer pool status handle.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_bpool_status_get(int unit, bcmpkt_bpool_status_t *status);

/*!
 * \brief Dump buffer pool information.
 *
 * Dump a buffer pool's information.
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID is for shared pool.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_bpool_info_dump(int unit);

/*!
 * \brief Allocate a packet data buffer.
 *
 * \param [in] unit Switch unit number.
 * \param [in] size Number of bytes for each buffer.
 * \param [in] dbuf Data buffer handle.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Out of memory.
 */
extern int
bcmpkt_data_buf_alloc(int unit, uint32_t size, bcmpkt_data_buf_t **dbuf);

/*!
 * \brief Release a packet data buffer.
 *
 * \param [in] unit Switch unit number.
 * \param [in] dbuf Data buffer handle.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY The dbuf does not belong to the unit's buffer pool.
 * \retval SHR_E_FAIL Could not get lock.
 */
extern int
bcmpkt_data_buf_free(int unit, bcmpkt_data_buf_t *dbuf);

/*!
 * \brief Clone a packet.
 *
 * Create a new packet, copy meta data content and refer to same data_buf.
 *
 * \param [in] unit Switch number.
 * \param [in] pkt Packet handle.
 * \param [out] new_pkt New packet handle.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_packet_clone(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt);

/*!
 * \brief Claim the packet.
 *
 * Create a new packet. Copy metadata. Try to take over\c pkt->data_buf. If
 * can't take over it, because unit is different, will create a new data_buf
 * and copy content.
 *
 * \param [in] unit Switch number, from which buffer pool allocate new buffer.
 * \param [in] pkt Packet handle.
 * \param [out] new_pkt New packet handle.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_packet_claim(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt);

/*!
 * \brief Create a new similar data buffer with same content.
 *
 * Allocate a new data buffer, and copy \c dbuf content to it.
 *
 * \param [in] unit Switch number, from which buffer pool allocate new buffer.
 * \param [in] dbuf Packet data buffer for copy.
 * \param [out] new_dbuf New packet data buffer.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_data_buf_copy(int unit, bcmpkt_data_buf_t *dbuf,
                     bcmpkt_data_buf_t **new_dbuf);

/*!
 * \brief Create packet pool.
 *
 * This API is for creating bcmpkt_packet_t objects pool for allocation to
 * low done allocation overhead.
 *
 * This is not a must feature. If the pool was not created, bcmpkt_packet_t
 * object will be allocated from sal_alloc directly.
 *
 * When packet pool was created, the allocation will be done on it. If no
 * free buffer in it, a buffer will be allocated from sal_alloc(). Both pool
 * buffer and the buffer was allocated from sal_alloc will be released
 * into packet pool, only if packet pool free count is under \c pkt_count.
 * Otherwise, either one will be freed from sal_free().
 *
 * \param [in] pkt_count Number of buffers.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_ppool_create(int pkt_count);

/*!
 * \brief Destroy packet pool.
 *
 * \retval SHR_E_NONE Succeed.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_ppool_destroy(void);

/*!
 * \brief Dump packet pool information.
 *
 *
 * \retval SHR_E_NONE Succeed.
 */
extern int
bcmpkt_ppool_info_dump(void);

#endif /* BCMPKT_BUF_H */
