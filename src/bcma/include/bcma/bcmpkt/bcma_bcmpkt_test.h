/*! \file bcma_bcmpkt_test.h
 *
 * General functions for Packet I/O test.
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

#ifndef BCMA_BCMPKT_TEST_H
#define BCMA_BCMPKT_TEST_H

#include <sal/sal_time.h>

#include <bcmpc/bcmpc_lport.h>
#include <bcmpkt/bcmpkt_buf.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

/*!
 * \brief IFP info for packet I/O test.
 */
typedef struct bcma_bcmpkt_test_fp_cfg_s {
    /*! FP entry ID. */
    uint64_t entry_id;

    /*! FP group ID. */
    uint64_t grp_id;

    /*! FP rule ID. */
    uint64_t rule_id;

    /*! FP drop policy ID. */
    uint64_t drop_policy_id;

    /*! FP copy to CPU policy ID. */
    uint64_t copy_policy_id;
} bcma_bcmpkt_test_fp_cfg_t;

/*!
 * \brief Parameters for packet I/O rx test.
 */
typedef struct bcma_bcmpkt_test_rx_param_s {
    /*! TRUE: initialized. */
    int init;

    /*! Enable rx packets counting. */
    uint32_t count_enable;

    /*! Counted rx packets. */
    uint64_t pkt_received;

    /*! Timestamp for the first packet. */
    sal_usecs_t time_start;

    /*! Timestamp for the last packet. */
    sal_usecs_t time_end;

    /*! Port number. */
    int port;

    /*! Packet I/O rx test duration. */
    uint32_t time;

    /*! Starting packet length. */
    uint32_t len_start;

    /*! Ending packet length. */
    uint32_t len_end;

    /*! Increasing step of packet length. */
    uint32_t len_inc;

    /*! Packet I/O packet structure. */
    bcmpkt_packet_t *packet;

    /*! FP config info. */
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg;
}bcma_bcmpkt_test_rx_param_t;

/*!
 * \brief Parameters for packet I/O tx test.
 */
typedef struct bcma_bcmpkt_test_tx_param_s {
    /*! TRUE: initialized. */
    int init;

    /*! Port number. */
    int port;

    /*! The number of packets to be sent. */
    int send_count;

    /*! Starting packet length. */
    uint32_t len_start;

    /*! Ending packet length. */
    uint32_t len_end;

    /*! Increasing step of packet length. */
    uint32_t len_inc;

    /*! Packet I/O packet structure. */
    bcmpkt_packet_t *packet;

    /*! FP config info. */
    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg;
} bcma_bcmpkt_test_tx_param_t;

/*!
 * \brief Generate packet for test.
 *
 * \param [in] unit Unit number.
 * \param [in] packet The packet structure.
 * \param [in] packet_len Packet length.
 * \param [in] buf_size Buf size.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_packet_generate(int unit, bcmpkt_packet_t **packet,
                                 uint32_t packet_len, uint32_t buf_size,
                                 int port);

/*!
 * \brief Set port loopback mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] lb_mode Loopback mode.
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_port_lb_set(int unit, int port, bcmpc_port_lpbk_t lb_mode);

/*!
 * \brief Create traffic data path.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] redirect_port Redirect port number.
 * \param [in] ifp_cfg IFP cfg info.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dpath_create(int unit, int port, int redirect_port,
                              bcma_bcmpkt_test_fp_cfg_t *ifp_cfg);

/*!
 * \brief Destroy traffic data path.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] ifp_cfg IFP cfg info.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dpath_destroy(int unit, int port,
                               bcma_bcmpkt_test_fp_cfg_t *ifp_cfg);

/*!
 * \brief Parse parameter from CLI.
 *
 * \param [in] unit Unit number.
 * \param [in] cli CLI object.
 * \param [in] a Argument list.
 * \param [out] rx_param Packet I/O rx test parameter.
 * \param [in] show Print CLI parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_rx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_rx_param_t *rx_param, bool show);

/*!
 * \brief Parse parameter from CLI.
 *
 * \param [in] unit Unit number.
 * \param [in] cli CLI object.
 * \param [in] a Argument list.
 * \param [out] tx_param Packet I/O tx test parameter.
 * \param [in] show Print CLI parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_tx_parse_args(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                               bcma_bcmpkt_test_tx_param_t *tx_param, bool show);
/*!
 * \brief Packet I/O rx test procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] rx_param Packet I/O rx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_rx_run(int unit, bcma_bcmpkt_test_rx_param_t *rx_param);

/*!
 * \brief Packet I/O tx test procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] tx_param Packet I/O tx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_tx_run(int unit, bcma_bcmpkt_test_tx_param_t *tx_param);

/*!
 * \brief Packet I/O rx test cleanup procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] rx_param Packet I/O rx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern void
bcma_bcmpkt_test_rx_recycle(int unit, bcma_bcmpkt_test_rx_param_t *rx_param);

/*!
 * \brief Packet I/O tx test cleanup procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] tx_param Packet I/O tx test parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern void
bcma_bcmpkt_test_tx_recycle(int unit, bcma_bcmpkt_test_tx_param_t *tx_param);

/*!
 * \brief Cleanup packet I/O device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dev_cleanup(int unit);

/*!
 * \brief Init packet I/O device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_dev_init(int unit);

/*!
 * \brief Map queues to a RX channel.
 *
 * \param [in] unit Unit number.
 * \param [in] chan_id DMA channel ID.
 * \param [in] bmp_high Bitmap for CPU high 16 queues.
 * \param [in] bmp_low Bitmap for CPU low 32 queues.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_bcmpkt_test_chan_qmap_set(int unit, int chan_id,
                               uint32_t bmp_high, uint32_t bmp_low);

#endif /* BCMA_BCMPKT_TEST_H */

