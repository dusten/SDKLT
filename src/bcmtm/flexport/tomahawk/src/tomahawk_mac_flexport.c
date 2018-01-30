/*! \file tomahawk_mac_flexport.c
 *
 * MAC operation apis for flex port operations.
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

#include "../../main/flexport_top.h"



#if defined(BCM_TOMAHAWK_SUPPORT)

/* Temp holder */
#define CLMAC_AVERAGE_IPG_DEFAULT 12
#define CLMAC_AVERAGE_IPG_HIGIG   8


/*** START SDK API COMMON CODE ***/


/*! @file tomahawk_mac_flexport.c
 *  @brief MAC flex.
 *  Details are shown below.
 */

int
soc_tomahawk_flex_mac_rx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64, fldval64;
    int  lport;

    /*Disable TSC lanes: */

    /* Disable MAC RX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == BCMPC_INVALID_PPORT) {
            lport = port_schedule_state->resource[i].logical_port;

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, lport,
                                                    0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, RX_ENf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, lport,
                                                    0, rval64));

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_TX_CTRLr,
                                                    lport, 0, &rval64));
            COMPILER_64_SET(fldval64, 0, 1);
            soc_reg64_field_set(
                      unit, CLMAC_TX_CTRLr, &rval64, DISCARDf, fldval64);
            soc_reg64_field_set(
                      unit, CLMAC_TX_CTRLr, &rval64, EP_DISCARDf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_TX_CTRLr,
                                                    lport, 0, rval64));
        }
    }
    return SOC_E_NONE;
}

int
soc_tomahawk_flex_mac_tx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64, fldval64;
    int lport;

    /* Disable MAC TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == BCMPC_INVALID_PPORT) {
            lport = port_schedule_state->resource[i].logical_port;

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, lport,
                                                    0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, TX_ENf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, lport,
                                                    0, rval64));
        }
    }

    /* Apply soft reset */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == BCMPC_INVALID_PPORT) {
            lport = port_schedule_state->resource[i].logical_port;

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, lport,
                                                    0, &rval64));
            COMPILER_64_SET(fldval64, 0, 1);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, lport,
                                                    0, rval64));
        }
    }
    return SOC_E_NONE;
}

int
soc_tomahawk_flex_mac_port_up(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64, fldval64;
    uint32 rval32;
    int port;
    int phy_port;
    int subp;
    int mode;
    int speed_100g;
    int clport;
    int higig2_mode;
    int strict_preamble;
    int higig_mode;
    uint64 average_ipg;
    static const int clport_mode_values[SOC_TH_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };


    /*Disable TSC lanes: */

    /*CLMAC_RX_CTRL */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;
            higig2_mode =
                ((port_schedule_state->resource[i].encap ==
                  _SHR_PORT_ENCAP_HIGIG2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_LITE)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_L2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_IP_GRE));
            higig_mode =
                (port_schedule_state->resource[i].encap ==
                 _SHR_PORT_ENCAP_HIGIG);
            strict_preamble = (higig2_mode||higig_mode)?0:1;
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_RX_CTRLr,
                                                    port, 0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(
                      unit, CLMAC_RX_CTRLr, &rval64, RX_ANY_STARTf, fldval64);
            soc_reg64_field_set(
                      unit, CLMAC_RX_CTRLr, &rval64, STRIP_CRCf, fldval64);
            COMPILER_64_SET(fldval64, 0, strict_preamble);
            soc_reg64_field_set(unit, CLMAC_RX_CTRLr, &rval64, STRICT_PREAMBLEf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, 64);
            soc_reg64_field_set(unit, CLMAC_RX_CTRLr, &rval64, RUNT_THRESHOLDf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_RX_CTRLr,
                                                    port, 0, rval64));
        }
    }

    /*CLPORT_CONFIG */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;
            higig2_mode =
                ((port_schedule_state->resource[i].encap ==
                  _SHR_PORT_ENCAP_HIGIG2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_LITE)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_L2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_IP_GRE));
            higig_mode =
                (port_schedule_state->resource[i].encap ==
                 _SHR_PORT_ENCAP_HIGIG);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CLPORT_CONFIGr,
                                                      port, 0, &rval32));
            soc_reg_field_set(unit, CLPORT_CONFIGr, &rval32, HIGIG2_MODEf,
                              higig2_mode);
            soc_reg_field_set(unit, CLPORT_CONFIGr, &rval32, HIGIG_MODEf,
                              higig_mode);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CLPORT_CONFIGr,
                                                      port, 0, rval32));
        }
    }

    /*CLPORT Enable: */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CLPORT_ENABLE_REGr,
                                                      subp, 0, &rval32));
            if (subp == 0) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT0f, 1);
            } else if (subp==1) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT1f, 1);
            } else if (subp==2) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT2f, 1);
            } else {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT3f, 1);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CLPORT_ENABLE_REGr,
                                                      subp, 0, rval32));
        }
    }
    sal_usleep(100 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    /*CLPORT mode: */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            clport = ((phy_port-1)>>2);
            soc_tomahawk_port_ratio_get(unit, port_schedule_state, clport,
                                         &mode,
                                         0);
            if (port_schedule_state->out_port_map.log_port_speed[port]>=
                100000) {
                speed_100g = 1;
            } else {
                speed_100g = 0;
            }

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CLPORT_MODE_REGr,
                                                      subp, 0, &rval32));
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval32,
                              SINGLE_PORT_MODE_SPEED_100Gf,
                              speed_100g);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval32,
                              XPORT0_CORE_PORT_MODEf,
                              clport_mode_values[mode]);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval32,
                              XPORT0_PHY_PORT_MODEf,
                              clport_mode_values[mode]);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CLPORT_MODE_REGr,
                                                      subp, 0, rval32));
        }
    }
    sal_usleep(200 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    /* Release soft reset */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, port,
                                                    0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, port,
                                                    0, rval64));
        }
    }

    sal_usleep(100 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
    /* Enable MAC RX_EN & TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;
            higig2_mode =
                ((port_schedule_state->resource[i].encap ==
                  _SHR_PORT_ENCAP_HIGIG2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_LITE)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_L2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_IP_GRE));
            if (higig2_mode) {
               COMPILER_64_SET(average_ipg, 0, CLMAC_AVERAGE_IPG_HIGIG);
            } else {
               COMPILER_64_SET(average_ipg, 0, CLMAC_AVERAGE_IPG_DEFAULT);
            }

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_TX_CTRLr,
                                                    port, 0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_TX_CTRLr, &rval64, DISCARDf, fldval64);
            soc_reg64_field_set(unit, CLMAC_TX_CTRLr, &rval64, EP_DISCARDf, fldval64);
            soc_reg64_field_set(unit, CLMAC_TX_CTRLr, &rval64, AVERAGE_IPGf, average_ipg);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_TX_CTRLr,
                                                    port, 0, rval64));

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, port,
                                                    0, &rval64));
            COMPILER_64_SET(fldval64, 0, 1);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, RX_ENf, fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, TX_ENf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, port,
                                                    0, rval64));

        }
    }

    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK_SUPPORT */
