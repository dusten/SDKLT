/*! \file bcmpmac_cdport.h
 *
 * CDPORT driver.
 *
 * A CDPORT contains 1 CDPORT and 2 CDMACs, and supports 8 ports at most.
 *
 * In this driver, we always use the port number 0~7 to access the CDPORT
 * and CDMAC per-port registers without considering which CDMAC should be
 * configured.
 *
 * The lower level register access driver would know how to determine which
 * CDMAC should be accessed, i.e. port 0~3 belongs the first CDMAC and port 4~7
 * belongs to the second CDMAC.
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

#ifndef BCMPMAC_CDPORT_H
#define BCMPMAC_CDPORT_H

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpmac/bcmpmac_util_internal.h>


/*******************************************************************************
 * Local definitions
 */

/*! Drain cell waiting time. */
#define DRAIN_WAIT_MSEC 500

/*! Number of ports per CDMAC. */
#define PORTS_PER_CDMAC 4

/*! CDMAC header mode value per encap mode. */
typedef enum cdmac_hdr_mode_e {

    /*! IEEE Ethernet format. */
    CDMAC_HDR_MODE_IEEE = 0,

} cdmac_hdr_mode_t;

/*! CDPORT port mode value. */
typedef enum cdport_port_mode_e {

    /*! Quad Port Mode. All four ports are enabled. */
    CDPORT_PORT_MODE_QUAD = 0,

    /*! Tri Port Mode. Lanes 0, 1, and 2 active. lane 2 is dual. */
    CDPORT_PORT_MODE_TRI_012 = 1,

    /*! Tri Port Mode. Lanes 0, 2, and 3 active. lane 0 is dual. */
    CDPORT_PORT_MODE_TRI_023 = 2,

    /*! Dual Port Mode. Each of lanes 0 and 2 are dual. */
    CDPORT_PORT_MODE_DUAL = 3,

    /*! Single Port Mode. Lanes 0 through 3 are single XLGMII. */
    CDPORT_PORT_MODE_SINGLE = 4

} cdport_port_mode_t;


/******************************************************************************
 * Private functions
 */

static inline int
cdport_port_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t reset)
{
    /* There is no per-port RESET control in CDPORT. */
    return SHR_E_NONE;
}

static inline int
cdport_port_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t *reset)
{
    /* There is no per-port RESET control in CDPORT. */
    *reset = 0;
    return SHR_E_NONE;
}

static inline int
cdport_port_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t enable)
{
    /* There is no per-port ENABLE control in CDPORT. */
    return SHR_E_NONE;
}

static inline int
cdport_port_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t *enable)
{
    /* There is no per-port ENABLE control in CDPORT. */
    *enable = 1;
    return SHR_E_NONE;
}

static inline int
cdport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t init)
{
    int ioerr = 0;
    CDPORT_FLOW_CONTROL_CONFIGr_t flowctrl;

    if (!init) {
        return SHR_E_NONE;
    }

    /* Enable flow control. */
    ioerr += READ_CDPORT_FLOW_CONTROL_CONFIGr(pa, port, &flowctrl);
    CDPORT_FLOW_CONTROL_CONFIGr_MERGE_MODE_ENf_SET(flowctrl, 1);
    ioerr += WRITE_CDPORT_FLOW_CONTROL_CONFIGr(pa, port, flowctrl);

    /* Reset the MIB. */
    

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_mac_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t reset)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_SOFT_RESETf_SET(mac_ctrl, reset);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_mac_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *reset)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *reset = CDMAC_CTRLr_SOFT_RESETf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_rx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_RX_ENf_SET(mac_ctrl, enable);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_rx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *enable = CDMAC_CTRLr_RX_ENf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_tx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_TX_ENf_SET(mac_ctrl, enable);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_tx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *enable = CDMAC_CTRLr_TX_ENf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_speed_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t speed)
{
    /* There is no port SPEED control in CDMAC. */
    return SHR_E_NONE;
}

static inline int
cdport_speed_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t *speed)
{
    /* There is no port SPEED control in CDMAC. */
    *speed = 0;
    return SHR_E_NONE;
}

static inline int
cdport_encap_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t encap)
{
    int ioerr = 0;
    CDMAC_MODEr_t cdmac_mode;
    uint32_t hdr_mode;

    CDMAC_MODEr_CLR(cdmac_mode);

    switch (encap) {
        case BCMPMAC_ENCAP_IEEE:
            hdr_mode = CDMAC_HDR_MODE_IEEE;
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += READ_CDMAC_MODEr(pa, port, &cdmac_mode);
    CDMAC_MODEr_HDR_MODEf_SET(cdmac_mode, hdr_mode);
    ioerr += WRITE_CDMAC_MODEr(pa, port, cdmac_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_encap_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t *encap)
{
    int ioerr = 0;
    CDMAC_MODEr_t cdmac_mode;
    uint32_t hdr_mode;

    CDMAC_MODEr_CLR(cdmac_mode);

    ioerr += READ_CDMAC_MODEr(pa, port, &cdmac_mode);
    hdr_mode = CDMAC_MODEr_HDR_MODEf_GET(cdmac_mode);

    switch (hdr_mode) {
        case CDMAC_HDR_MODE_IEEE:
            *encap = BCMPMAC_ENCAP_IEEE;
            break;
        default:
            *encap = BCMPMAC_ENCAP_COUNT;
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_lpbk_set(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_LOCAL_LPBKf_SET(mac_ctrl, en ? 1 : 0);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t *en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *en = CDMAC_CTRLr_LOCAL_LPBKf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;

    CDMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    CDMAC_PAUSE_CTRLr_TX_PAUSE_ENf_SET(pause_ctrl, ctrl->tx_enable);
    CDMAC_PAUSE_CTRLr_RX_PAUSE_ENf_SET(pause_ctrl, ctrl->rx_enable);
    ioerr += WRITE_CDMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;

    CDMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    ctrl->tx_enable = CDMAC_PAUSE_CTRLr_TX_PAUSE_ENf_GET(pause_ctrl);
    ctrl->rx_enable = CDMAC_PAUSE_CTRLr_RX_PAUSE_ENf_GET(pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_addr_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    CDMAC_TX_MAC_SAr_t tx_mac_sa;
    CDMAC_RX_MAC_SAr_t rx_mac_sa;
    uint32_t sa[2];

    CDMAC_TX_MAC_SAr_CLR(tx_mac_sa);
    CDMAC_RX_MAC_SAr_CLR(rx_mac_sa);

    sa[1] = (mac[0] << 8) | mac[1];
    sa[0] = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5];

    ioerr += READ_CDMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    CDMAC_TX_MAC_SAr_CTRL_SAf_SET(tx_mac_sa, sa);
    ioerr += WRITE_CDMAC_TX_MAC_SAr(pa, port, tx_mac_sa);

    ioerr += READ_CDMAC_RX_MAC_SAr(pa, port, &rx_mac_sa);
    CDMAC_RX_MAC_SAr_RX_SAf_SET(rx_mac_sa, sa);
    ioerr += WRITE_CDMAC_RX_MAC_SAr(pa, port, rx_mac_sa);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_addr_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    CDMAC_TX_MAC_SAr_t tx_mac_sa;
    uint32_t sa[2];

    CDMAC_TX_MAC_SAr_CLR(tx_mac_sa);

    ioerr += READ_CDMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    CDMAC_TX_MAC_SAr_CTRL_SAf_GET(tx_mac_sa, sa);

    mac[0] = (uint8_t)(sa[1] >> 8);
    mac[1] = (uint8_t)(sa[1]);
    mac[2] = (uint8_t)(sa[0] >> 24);
    mac[3] = (uint8_t)(sa[0] >> 16);
    mac[4] = (uint8_t)(sa[0] >> 8);
    mac[5] = (uint8_t)(sa[0]);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_frame_max_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t size)
{
    int ioerr = 0;
    CDMAC_RX_MAX_SIZEr_t rx_max_size;

    CDMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    ioerr += READ_CDMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    CDMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_SET(rx_max_size, size);
    ioerr += WRITE_CDMAC_RX_MAX_SIZEr(pa, port, rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_frame_max_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *size)
{
    int ioerr = 0;
    CDMAC_RX_MAX_SIZEr_t rx_max_size;

    CDMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    ioerr += READ_CDMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    *size = CDMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_GET(rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_fault_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        bcmpmac_fault_status_t *st)
{
    int ioerr = 0;
    CDMAC_RX_LSS_CTRLr_t lss_ctrl;
    CDMAC_RX_LSS_STATUSr_t lss_st;

    sal_memset(st, 0, sizeof(*st));
    CDMAC_RX_LSS_CTRLr_CLR(lss_ctrl);
    CDMAC_RX_LSS_STATUSr_CLR(lss_st);

    /* The fault statu bit in CDMAC_RX_LSS_STATUSr is cleared-on-read. */
    ioerr += READ_CDMAC_RX_LSS_STATUSr(pa, port, &lss_st);
    ioerr += READ_CDMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    /* The fault status is vaild when the fault control is enabled. */
    if (CDMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->local_fault =
            CDMAC_RX_LSS_STATUSr_LOCAL_FAULT_STATUSf_GET(lss_st);
    }

    if (CDMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->remote_fault =
            CDMAC_RX_LSS_STATUSr_REMOTE_FAULT_STATUSf_GET(lss_st);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_core_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     bcmpmac_core_port_mode_t *mode)
{
    int rv, i, ioerr = 0;
    CDPORT_MODE_REGr_t cdport_mode;
    uint32_t is_400g, mode_mac0, mode_mac1;
    uint32_t lmap_400g[BCMPMAC_NUM_PORTS_PER_PM_MAX] = { 0xff, 0 };
    uint32_t lmap[BCMPMAC_NUM_PORTS_PER_PM_MAX] = { 0 };
    const bcmpmac_port_mode_map_t port_mode_map[] = {
        { { 0x1, 0x2, 0x4, 0x8, 0 }, CDPORT_PORT_MODE_QUAD },
        { { 0x3, 0, 0xc, 0 }, CDPORT_PORT_MODE_DUAL },
        { { 0x1, 0x2, 0xc, 0 }, CDPORT_PORT_MODE_TRI_012 },
        { { 0x3, 0x0, 0x4, 0x8, 0 }, CDPORT_PORT_MODE_TRI_023 },
        { { 0xf, 0 }, CDPORT_PORT_MODE_SINGLE }
    };

    CDPORT_MODE_REGr_CLR(cdport_mode);

    if (sal_memcmp(lmap_400g, mode->lane_map, sizeof(lmap_400g)) == 0) {
        /*
         * SINGLE_PORT_MODE_SPEED_400G.
         */
        is_400g = 1;
        mode_mac0 = CDPORT_PORT_MODE_SINGLE;
        mode_mac1 = CDPORT_PORT_MODE_SINGLE;
    } else {
        /*
         * More than one port.
         */
        is_400g = 0;

        /* Get the port mode of the first half part. */
        sal_memset(lmap, 0, sizeof(lmap));
        sal_memcpy(lmap, mode->lane_map, PORTS_PER_CDMAC * sizeof(uint32_t));
        rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                                   lmap, &mode_mac0);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        /* Get the port mode of the second half part. */
        sal_memset(lmap, 0, sizeof(lmap));
        for (i = 0; i < PORTS_PER_CDMAC; i++) {
            /*
             * The second part controls the upper 4 lanes, so the lane map
             * will be something like {0x10, 0x20, 0x40, 0x80}.
             *
             * Shift 4 bit on the lane map value and check if the combination is
             * valid.
             */
            lmap[i] = mode->lane_map[PORTS_PER_CDMAC + i] >> 4;
        }
        rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                                   lmap, &mode_mac1);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    ioerr += READ_CDPORT_MODE_REGr(pa, port, &cdport_mode);
    CDPORT_MODE_REGr_SINGLE_PORT_MODE_SPEED_400Gf_SET(cdport_mode, is_400g);
    CDPORT_MODE_REGr_MAC0_PORT_MODEf_SET(cdport_mode, mode_mac0);
    CDPORT_MODE_REGr_MAC1_PORT_MODEf_SET(cdport_mode, mode_mac1);
    ioerr += WRITE_CDPORT_MODE_REGr(pa, port, cdport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_tx_flush(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                uint32_t flush)
{
    int cnt, ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;
    CDMAC_TXFIFO_CELL_CNTr_t cell_cnt;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);
    CDMAC_TXFIFO_CELL_CNTr_CLR(cell_cnt);

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_DISCARDf_SET(tx_ctrl, flush);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    if (flush == 0) {
        return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
    }

    cnt = DRAIN_WAIT_MSEC / 10;
    while (--cnt >= 0) {
        ioerr += READ_CDMAC_TXFIFO_CELL_CNTr(pa, port, &cell_cnt);
        if (CDMAC_TXFIFO_CELL_CNTr_CELL_CNTf_GET(cell_cnt) == 0) {
            break;
        }
    }

    if (cnt < 0) {
        return SHR_E_BUSY;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

/*!
 * \brief Toggle LAG failover status.
 *
 * Toggle link_status_up bit to notify IPIPE on link up.
 *
 * When a link comes up, hardware will not update the LINK_STATUS register
 * until software has toggled the the LAG_FAILOVER_CONFIG.LINK_STATUS_UP field.
 * When a link goes down, hardware will update the LINK_STATUS register without
 * software intervention.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] arg Not used.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
static inline int
cdport_failover_status_toggle(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t arg)
{
    int ioerr = 0;
    CDPORT_LAG_FAILOVER_CONFIGr_t cfg;

    CDPORT_LAG_FAILOVER_CONFIGr_CLR(cfg);
    ioerr += READ_CDPORT_LAG_FAILOVER_CONFIGr(pa, port, &cfg);
    CDPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 1);
    ioerr += WRITE_CDPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);
    CDPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 0);
    ioerr += WRITE_CDPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *failover_lpbk)
{
    int ioerr = 0;
    CDMAC_LAG_FAILOVER_STATUSr_t st;

    CDMAC_LAG_FAILOVER_STATUSr_CLR(st);

    ioerr += READ_CDMAC_LAG_FAILOVER_STATUSr(pa, port, &st);
    *failover_lpbk = CDMAC_LAG_FAILOVER_STATUSr_LAG_FAILOVER_LOOPBACKf_GET(st);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;
    CDMAC_RX_LSS_CTRLr_t rx_lss;

    CDMAC_CTRLr_CLR(mac_ctrl);
    CDMAC_RX_LSS_CTRLr_CLR(rx_lss);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_LINK_STATUS_SELECTf_SET(mac_ctrl, en);
    CDMAC_CTRLr_LAG_FAILOVER_ENf_SET(mac_ctrl, en);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_CDMAC_RX_LSS_CTRLr(pa, port, &rx_lss);
    CDMAC_RX_LSS_CTRLr_DROP_TX_DATA_ON_LINK_INTERRUPTf_SET(rx_lss, en);
    CDMAC_RX_LSS_CTRLr_RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf_SET(rx_lss, en);
    ioerr += WRITE_CDMAC_RX_LSS_CTRLr(pa, port, rx_lss);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t *en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *en = CDMAC_CTRLr_LINK_STATUS_SELECTf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_lpbk_remove(bcmpmac_access_t *pa, bcmpmac_pport_t port)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 1);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 0);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pfc_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;
    CDMAC_PFC_CTRLr_t pfc_ctrl;
    CDMAC_PFC_TYPEr_t pfc_type;
    CDMAC_PFC_OPCODEr_t pfc_opcode;
    CDMAC_PFC_DAr_t pfc_da;
    uint32_t da[2];

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    CDMAC_PAUSE_CTRLr_PFC_REFRESH_ENf_SET(pause_ctrl, (cfg->refresh_timer > 0));
    CDMAC_PAUSE_CTRLr_PFC_REFRESH_TIMERf_SET(pause_ctrl, cfg->refresh_timer);
    CDMAC_PAUSE_CTRLr_PFC_XOFF_TIMERf_SET(pause_ctrl, cfg->xoff_timer);
    ioerr += WRITE_CDMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    CDMAC_PFC_CTRLr_PFC_STATS_ENf_SET(pfc_ctrl, cfg->stats_enable);
    CDMAC_PFC_CTRLr_TX_PFC_ENf_SET(pfc_ctrl, cfg->tx_enable);
    CDMAC_PFC_CTRLr_RX_PFC_ENf_SET(pfc_ctrl, cfg->rx_enable);
    ioerr += WRITE_CDMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    ioerr += READ_CDMAC_PFC_TYPEr(pa, port, &pfc_type);
    CDMAC_PFC_TYPEr_PFC_ETH_TYPEf_SET(pfc_type, cfg->eth_type);
    ioerr += WRITE_CDMAC_PFC_TYPEr(pa, port, pfc_type);

    ioerr += READ_CDMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    CDMAC_PFC_OPCODEr_PFC_OPCODEf_SET(pfc_opcode, cfg->opcode);
    ioerr += WRITE_CDMAC_PFC_OPCODEr(pa, port, pfc_opcode);

    da[1] = (cfg->da[0] << 8) | cfg->da[1];
    da[0] = (cfg->da[2] << 24) | (cfg->da[3] << 16) |
            (cfg->da[4] << 8) | cfg->da[5];
    ioerr += READ_CDMAC_PFC_DAr(pa, port, &pfc_da);
    CDMAC_PFC_DAr_PFC_MACDAf_SET(pfc_da, da);
    ioerr += WRITE_CDMAC_PFC_DAr(pa, port, pfc_da);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pfc_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;
    CDMAC_PFC_CTRLr_t pfc_ctrl;
    CDMAC_PFC_TYPEr_t pfc_type;
    CDMAC_PFC_OPCODEr_t pfc_opcode;
    CDMAC_PFC_DAr_t pfc_da;
    uint32_t da[2];

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    cfg->refresh_timer = CDMAC_PAUSE_CTRLr_PFC_REFRESH_TIMERf_GET(pause_ctrl);
    cfg->xoff_timer = CDMAC_PAUSE_CTRLr_PFC_XOFF_TIMERf_GET(pause_ctrl);

    ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    cfg->stats_enable = CDMAC_PFC_CTRLr_PFC_STATS_ENf_GET(pfc_ctrl);
    cfg->tx_enable = CDMAC_PFC_CTRLr_TX_PFC_ENf_GET(pfc_ctrl);
    cfg->rx_enable = CDMAC_PFC_CTRLr_RX_PFC_ENf_GET(pfc_ctrl);

    ioerr += READ_CDMAC_PFC_TYPEr(pa, port, &pfc_type);
    cfg->eth_type = CDMAC_PFC_TYPEr_PFC_ETH_TYPEf_GET(pfc_type);

    ioerr += READ_CDMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    cfg->opcode = CDMAC_PFC_OPCODEr_PFC_OPCODEf_GET(pfc_opcode);

    ioerr += READ_CDMAC_PFC_DAr(pa, port, &pfc_da);
    CDMAC_PFC_DAr_PFC_MACDAf_GET(pfc_da, da);
    cfg->da[0] = (uint8_t)(da[1] >> 8);
    cfg->da[1] = (uint8_t)(da[1]);
    cfg->da[2] = (uint8_t)(da[0] >> 24);
    cfg->da[3] = (uint8_t)(da[0] >> 16);
    cfg->da[4] = (uint8_t)(da[0] >> 8);
    cfg->da[5] = (uint8_t)(da[0]);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_op_exec(bcmpmac_access_t *pa, bcmpmac_pport_t port, const char *op,
               uint32_t op_param)
{
    bcmpmac_drv_op_f op_func;
    const bcmpmac_drv_op_hdl_t ops[] = {
        { "port_reset", cdport_port_reset_set },
        { "mac_reset", cdport_mac_reset_set },
        { "rx_enable", cdport_rx_enable_set },
        { "tx_enable", cdport_tx_enable_set },
        { "tx_flush", cdport_tx_flush },
        { "speed_set", cdport_speed_set },
        { "failover_toggle", cdport_failover_status_toggle}
    };

    op_func = bcmpmac_drv_op_func_get(ops, COUNTOF(ops), op);
    if (!op_func) {
        return SHR_E_UNAVAIL;
    }

    return op_func(pa, port, op_param);
}

#endif /* BCMPMAC_CDPORT_H */

