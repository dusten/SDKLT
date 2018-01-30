/*! \file bcmpc_pmgr_convert.c
 *
 * BCMPC Type Conversion
 *
 * Convert BCMPC data type to/from PMAC and PHYMOD.
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
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/*! Port operating mode map to the corresponding config. */
typedef struct pmgr_opmode_map_s {
    bcmpc_port_opmode_t opmode;
    bcmpc_port_opmode_config_t opmode_cfg;
} pmgr_opmode_map_t;

/*! Encap mode map. */
typedef struct pmgr_encap_map_s {
    bcmpc_port_encap_t pmgr_mode;
    bcmpmac_encap_mode_t pmac_mode;
} pmgr_encap_map_t;

static const pmgr_opmode_map_t pmgr_opmode_map[] = {
    /* bcmpc_port_opmode_t, bcmpc_port_opmode_config_t */

    /*! 10 Mbps copper mode (10BASE-T). */
    { BCMPC_PORT_OPMODE_10M_T, { BCMPC_PHY_INTF_MII, 10, 0 } },

    /*! 10 Mbps copper half-duplex mode (10BASE-T HDX). */
    { BCMPC_PORT_OPMODE_10M_T_HDX, { BCMPC_PHY_INTF_MII, 10, 1 } },

    /*! 100 Mbps copper mode (100BASE-T). */
    { BCMPC_PORT_OPMODE_100M_T, { BCMPC_PHY_INTF_MII, 100, 0 } },

    /*! 100 Mbps copper half-duplex mode (100BASE-T HDX). */
    { BCMPC_PORT_OPMODE_100M_T_HDX, { BCMPC_PHY_INTF_MII, 100, 1 } },

    /*! 100 Mbps fiber mode (100BASE-FX). */
    { BCMPC_PORT_OPMODE_100M_FX, { BCMPC_PHY_INTF_SERDES, 100, 0 } },

    /*! 1000 Mbps copper mode (1000BASE-T). */
    { BCMPC_PORT_OPMODE_1000M_T, { BCMPC_PHY_INTF_MII, 1000, 0 } },

    /*! 1000 Mbps fiber mode (1000BASE-X). */
    { BCMPC_PORT_OPMODE_1000M_X, { BCMPC_PHY_INTF_SERDES, 1000, 0 } },

    /*! 2.5 Gbps mode (HiGig only). */
    { BCMPC_PORT_OPMODE_2P5G, { BCMPC_PHY_INTF_SERDES, 2500, 0 } },

    /*! Default 10 Gbps mode for this port type (typically XAUI). */
    { BCMPC_PORT_OPMODE_10G, { BCMPC_PHY_INTF_XAUI, 10000, 0 } },

    /*! 10 Gbps SFI mode. */
    { BCMPC_PORT_OPMODE_10G_SFI, { BCMPC_PHY_INTF_SFI, 10000, 0 } },

    /*! 10 Gbps XFI mode. */
    { BCMPC_PORT_OPMODE_10G_XFI, { BCMPC_PHY_INTF_XFI, 10000, 0 } },

    /*! 10 Gbps CX mode. */
    { BCMPC_PORT_OPMODE_10G_CX, { BCMPC_PHY_INTF_CX, 10000, 0 } },

    /*! 10 Gbps CR mode. */
    { BCMPC_PORT_OPMODE_10G_CR, { BCMPC_PHY_INTF_CR, 10000, 0 } },

    /*! 10 Gbps KR mode. */
    { BCMPC_PORT_OPMODE_10G_KR, { BCMPC_PHY_INTF_KR, 10000, 0 } },

    /*! 10 Gbps KX mode. */
    { BCMPC_PORT_OPMODE_10G_KX, { BCMPC_PHY_INTF_KX, 10000, 0 } },

    /*! 12 Gbps mode (HiGig only). */
    { BCMPC_PORT_OPMODE_12G, { BCMPC_PHY_INTF_XAUI, 12000, 0 } },

    /*! 13 Gbps mode (HiGig only). */
    { BCMPC_PORT_OPMODE_13G, { BCMPC_PHY_INTF_XAUI, 13000, 0 } },

    /*! Default 20 Gbps mode for this port type (typically RXAUI). */
    { BCMPC_PORT_OPMODE_20G, { BCMPC_PHY_INTF_RXAUI, 20000, 0 } },

    /*! 20 Gbps KR mode (20GBASE-KR2). */
    { BCMPC_PORT_OPMODE_20G_KR, { BCMPC_PHY_INTF_KR, 20000, 0 } },

    /*! 21 Gbps mode (HiGig only). */
    { BCMPC_PORT_OPMODE_21G, { BCMPC_PHY_INTF_XAUI, 21000, 0 } },

    /*! 25 Gbps XFI mode. */
    { BCMPC_PORT_OPMODE_25G_XFI, { BCMPC_PHY_INTF_XFI, 25000, 0 } },

    /*! 27 Gbps XFI mode. */
    { BCMPC_PORT_OPMODE_27G_XFI, { BCMPC_PHY_INTF_XFI, 27000, 0 } },

    /*! Default 40 Gbps mode for this port type (typically (XLAUI). */
    { BCMPC_PORT_OPMODE_40G, { BCMPC_PHY_INTF_XLAUI, 40000, 0 } },

    /*! 40 Gbps CR mode (40GBASE-CR4). */
    { BCMPC_PORT_OPMODE_40G_CR, { BCMPC_PHY_INTF_CR, 40000, 0 } },

    /*! 40 Gbps KR mode (40GBASE-KR4). */
    { BCMPC_PORT_OPMODE_40G_KR, { BCMPC_PHY_INTF_KR, 40000, 0 } },

    /*! 42 Gbps mode (HiGig only). */
    { BCMPC_PORT_OPMODE_42G, { BCMPC_PHY_INTF_XLAUI, 42000, 0 } },

    /*! 50 Gbps KR mode. */
    { BCMPC_PORT_OPMODE_50G_KR, { BCMPC_PHY_INTF_KR, 50000, 0 } },

    /*! 53 Gbps KR mode. */
    { BCMPC_PORT_OPMODE_53G_KR, { BCMPC_PHY_INTF_KR, 53000, 0 } },

    /*! Default 100 Gbps mode for this port type (typically CAUI). */
    { BCMPC_PORT_OPMODE_100G, { BCMPC_PHY_INTF_CAUI, 100000, 0 } },

    /*! 100 Gbps CR mode. */
    { BCMPC_PORT_OPMODE_100G_CR, { BCMPC_PHY_INTF_CR, 100000, 0 } },

    /*! 106 Gbps KR mode. */
    { BCMPC_PORT_OPMODE_106G_KR, { BCMPC_PHY_INTF_KR, 106000, 0 } },

    /*! 120 Gbps mode (HiGig only). */
    { BCMPC_PORT_OPMODE_120G, { BCMPC_PHY_INTF_CAUI, 120000, 0 } },

    /*! 127 Gbps mode (HiGig only). */
    { BCMPC_PORT_OPMODE_127G, { BCMPC_PHY_INTF_CAUI, 127000, 0 } },

    
    /*! 200 Gbps mode. */
    { BCMPC_PORT_OPMODE_200G, { BCMPC_PHY_INTF_CAUI, 200000, 0 } },

    /*! 400 Gbps mode. */
    { BCMPC_PORT_OPMODE_400G, { BCMPC_PHY_INTF_CAUI, 400000, 0 } }
};

static const pmgr_encap_map_t pmgr_encap_map[] = {
    /* bcmpc_port_encap_t, bcmpmac_encap_mode_t */
    { BCMPC_PORT_ENCAP_IEEE, BCMPMAC_ENCAP_IEEE },
    { BCMPC_PORT_ENCAP_HIGIG_LITE, BCMPMAC_ENCAP_IEEE },
    { BCMPC_PORT_ENCAP_HIGIG, BCMPMAC_ENCAP_HIGIG },
    { BCMPC_PORT_ENCAP_HIGIG2, BCMPMAC_ENCAP_HIGIG2 }
};


/*******************************************************************************
 * Private functions
 */

/*
 * \brief Set media type in PHYMOD interface mode.
 *
 * The media mode is depended on interface type.
 *
 * \param [in] intf_type PHYMOD interface type.
 * \param [out] intf_cfg The media mode of \c intf_cfg will be set per
 *              \c intf_type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phymod_intf_media_mode_set(phymod_interface_t intf_type,
                                phymod_phy_inf_config_t *intf_cfg)
{
    PHYMOD_INTF_MODES_FIBER_CLR(intf_cfg);
    PHYMOD_INTF_MODES_BACKPLANE_CLR(intf_cfg);
    PHYMOD_INTF_MODES_COPPER_CLR(intf_cfg);

    switch(intf_type) {
        case phymodInterfaceSFI:
        case phymodInterfaceSR:
        case phymodInterfaceSR2:
        case phymodInterfaceSR4:
        case phymodInterfaceSR10:
        case phymodInterfaceLR:
        case phymodInterfaceLR2:
        case phymodInterfaceLR4:
        case phymodInterface1000X:
        case phymodInterfaceXGMII:
        case phymodInterfaceLRM:
        case phymodInterfaceER:
        case phymodInterfaceER2:
        case phymodInterfaceER4:
            PHYMOD_INTF_MODES_FIBER_SET(intf_cfg);
            break ;
        case phymodInterfaceKR:
        case phymodInterfaceKR2:
        case phymodInterfaceKR4:
        case phymodInterfaceKR10:
        case phymodInterfaceCAUI:
        case phymodInterfaceCAUI4:
        case phymodInterfaceXFI:
        case phymodInterfaceSGMII:
        case phymodInterfaceQSGMII:
        case phymodInterfaceXAUI:
        case phymodInterfaceRXAUI:
        case phymodInterfaceXLAUI:
        case phymodInterfaceXLAUI2:
            PHYMOD_INTF_MODES_BACKPLANE_SET(intf_cfg);
            break;
        case phymodInterfaceCR:
        case phymodInterfaceCR2:
        case phymodInterfaceCR4:
        case phymodInterfaceCR10:
            PHYMOD_INTF_MODES_COPPER_SET(intf_cfg);
             break;
        default:
            PHYMOD_INTF_MODES_BACKPLANE_SET(intf_cfg);
    }

    return SHR_E_NONE;
}

/*
 * \brief Get the PHYMOD interface type for 1-lane port.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phymod_intf_one_lane_get(int speed, int is_hg, int fiber_pref,
                              phymod_interface_t *intf)
{
    if (speed > 12000) {
        if (is_hg) {
            *intf = phymodInterfaceCR;
        } else {
            *intf = fiber_pref ? phymodInterfaceSR : phymodInterfaceCR;
        }
    } else if (speed >= 10000) {
        if (is_hg) {
            *intf = phymodInterfaceXFI;
        } else {
            *intf = fiber_pref ? phymodInterfaceSFI : phymodInterfaceXFI;
        }
    } else if (speed == 5000) {
        *intf = phymodInterface1000X;
    } else {
        *intf = fiber_pref ? phymodInterface1000X : phymodInterfaceSGMII;
    }

    return SHR_E_NONE;
}

/*
 * \brief Get the PHYMOD interface type for 2-lanes port.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phymod_intf_two_lane_get(int speed, int is_hg, int fiber_pref,
                              phymod_interface_t *intf)
{
    if (speed >= 40000) {
        *intf = phymodInterfaceKR2;
    } else if (speed >= 20000) {
        *intf = phymodInterfaceCR2;
    } else {
        *intf = phymodInterfaceRXAUI;
    }

    return SHR_E_NONE;
}

/*
 * \brief Get the PHYMOD interface type for 4-lanes port.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phymod_intf_four_lane_get(int speed, int is_hg, int fiber_pref,
                               phymod_interface_t *intf)
{
    if (speed >= 100000) {
        *intf = is_hg ? phymodInterfaceCR4 : phymodInterfaceCAUI4;
    } else if (speed >= 40000) {
        *intf = is_hg ? phymodInterfaceCR4 : phymodInterfaceXLAUI;
    } else if (speed >= 20000) {
        if (is_hg) {
            *intf = phymodInterfaceXGMII;
        } else {
            *intf = fiber_pref ? phymodInterfaceSR4 : phymodInterfaceCR4;
        }
    } else {
        *intf = is_hg ? phymodInterfaceXGMII : phymodInterfaceXAUI;
    }

    return SHR_E_NONE;
}

/*
 * \brief Convert PHY interface to PHYMOD interface type.
 *
 * \param [in] pmgr_intf Pmgr PHY interface type.
 * \param [in] num_lanes Nunber of lanes.
 * \param [out] phymod_intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phy_intf_to_phymod(bcmpc_phy_intf_t pmgr_intf, int num_lanes,
                        phymod_interface_t *phymod_intf)
{
    switch (pmgr_intf) {
        case BCMPC_PHY_INTF_GMII:
            *phymod_intf = phymodInterface1000X;
            break;
        case BCMPC_PHY_INTF_SGMII:
            *phymod_intf = phymodInterfaceSGMII;
            break;
        case BCMPC_PHY_INTF_XGMII:
            *phymod_intf = phymodInterfaceXGMII;
            break;
        case BCMPC_PHY_INTF_XAUI:
            *phymod_intf = phymodInterfaceXAUI;
            break;
        case BCMPC_PHY_INTF_RXAUI:
            *phymod_intf = phymodInterfaceRXAUI;
            break;
        case BCMPC_PHY_INTF_XLAUI:
            *phymod_intf = phymodInterfaceXLAUI;
            break;
        case BCMPC_PHY_INTF_CAUI:
            *phymod_intf = phymodInterfaceCAUI;
            break;
        case BCMPC_PHY_INTF_XFI:
            *phymod_intf = phymodInterfaceXFI;
            break;
        case BCMPC_PHY_INTF_SFI:
            *phymod_intf = phymodInterfaceSFI;
            break;
        case BCMPC_PHY_INTF_CX:
            if (num_lanes == 1) {
                *phymod_intf = phymodInterfaceCX;
            } else if (num_lanes == 2) {
                *phymod_intf = phymodInterfaceCX2;
            } else {
                *phymod_intf = phymodInterfaceCX4;
            }
            break;
        case BCMPC_PHY_INTF_CR:
            if (num_lanes == 1) {
                *phymod_intf = phymodInterfaceCR;
            } else if (num_lanes == 2) {
                *phymod_intf = phymodInterfaceCR2;
            } else if (num_lanes == 4) {
                *phymod_intf = phymodInterfaceCR4;
            } else {
                *phymod_intf = phymodInterfaceCR10;
            }
            break;
        case BCMPC_PHY_INTF_KR:
            if (num_lanes == 1) {
                *phymod_intf = phymodInterfaceKR;
            } else if (num_lanes == 2) {
                *phymod_intf = phymodInterfaceKR2;
            } else if (num_lanes == 4) {
                *phymod_intf = phymodInterfaceKR4;
            } else {
                *phymod_intf = phymodInterfaceKR10;
            }
            break;
        case BCMPC_PHY_INTF_KX:
            if (num_lanes == 1) {
                *phymod_intf = phymodInterfaceKX;
            } else {
                *phymod_intf = phymodInterfaceKX4;
            }
            break;
        case BCMPC_PHY_INTF_SR:
            if (num_lanes == 1) {
                *phymod_intf = phymodInterfaceSR;
            } else if (num_lanes == 2) {
                *phymod_intf = phymodInterfaceSR2;
            } else if (num_lanes == 4) {
                *phymod_intf = phymodInterfaceSR4;
            } else {
                *phymod_intf = phymodInterfaceSR10;
            }
            break;
        case BCMPC_PHY_INTF_LR:
            if (num_lanes == 1) {
                *phymod_intf = phymodInterfaceLR;
            } else if (num_lanes == 2) {
                *phymod_intf = phymodInterfaceLR2;
            } else if (num_lanes == 4) {
                *phymod_intf = phymodInterfaceLR4;
            } else {
                *phymod_intf = phymodInterfaceLR10;
            }
            break;
        case BCMPC_PHY_INTF_MII:
        case BCMPC_PHY_INTF_TBI:
        case BCMPC_PHY_INTF_RGMII:
        case BCMPC_PHY_INTF_RVMII:
        default:
            *phymod_intf = phymodInterfaceBypass;
            break;
    }

    return SHR_E_NONE;
}

/*
 * \brief Convert PHY interface from PHYMOD interface type.
 *
 * \param [in] phymod_intf PHYMOD interface type.
 * \param [out] pmgr_intf Pmgr PHY interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phy_intf_from_phymod(phymod_interface_t phymod_intf,
                          bcmpc_phy_intf_t *pmgr_intf)
{
    switch (phymod_intf) {
        case phymodInterface1000X:
            *pmgr_intf = BCMPC_PHY_INTF_GMII;
            break;
        case phymodInterfaceSGMII:
            *pmgr_intf = BCMPC_PHY_INTF_SGMII;
            break;
        case phymodInterfaceXGMII:
            *pmgr_intf = BCMPC_PHY_INTF_XGMII;
            break;
        case phymodInterfaceXAUI:
            *pmgr_intf = BCMPC_PHY_INTF_XAUI;
            break;
        case phymodInterfaceRXAUI:
            *pmgr_intf = BCMPC_PHY_INTF_RXAUI;
            break;
        case phymodInterfaceXLAUI:
            *pmgr_intf = BCMPC_PHY_INTF_XLAUI;
            break;
        case phymodInterfaceCAUI:
            *pmgr_intf = BCMPC_PHY_INTF_CAUI;
            break;
        case phymodInterfaceXFI:
            *pmgr_intf = BCMPC_PHY_INTF_XFI;
            break;
        case phymodInterfaceSFI:
            *pmgr_intf = BCMPC_PHY_INTF_SFI;
            break;
        case phymodInterfaceCX:
        case phymodInterfaceCX2:
        case phymodInterfaceCX4:
            *pmgr_intf = BCMPC_PHY_INTF_CX;
            break;
        case phymodInterfaceCR:
        case phymodInterfaceCR2:
        case phymodInterfaceCR4:
        case phymodInterfaceCR10:
            *pmgr_intf = BCMPC_PHY_INTF_CR;
            break;
        case phymodInterfaceKR:
        case phymodInterfaceKR2:
        case phymodInterfaceKR4:
        case phymodInterfaceKR10:
            *pmgr_intf = BCMPC_PHY_INTF_KR;
            break;
        case phymodInterfaceKX:
        case phymodInterfaceKX4:
            *pmgr_intf = BCMPC_PHY_INTF_KX;
            break;
        case phymodInterfaceSR:
        case phymodInterfaceSR2:
        case phymodInterfaceSR4:
        case phymodInterfaceSR10:
            *pmgr_intf = BCMPC_PHY_INTF_SR;
            break;
        case phymodInterfaceLR:
        case phymodInterfaceLR2:
        case phymodInterfaceLR4:
        case phymodInterfaceLR10:
            *pmgr_intf = BCMPC_PHY_INTF_LR;
            break;
        default:
            *pmgr_intf = BCMPC_PHY_INTF_NULL;
            break;
    }

    return SHR_E_NONE;
}

/*
 * \brief Get the constant operating mode configuration.
 *
 * \param [in] opmode A pre-defined operating mode value from
 *                    \ref bcmpc_port_opmode_t.
 * \param [out] opmode_cfg Operating mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_const_opmode_type_to_cfg(bcmpc_port_opmode_t opmode,
                              bcmpc_port_opmode_config_t *opmode_cfg)
{
    int idx;
    const pmgr_opmode_map_t *me;
    const bcmpc_port_opmode_config_t *me_cfg;

    for (idx = 0; idx < COUNTOF(pmgr_opmode_map); idx++) {
        me = &(pmgr_opmode_map[idx]);
        me_cfg = &(me->opmode_cfg);
        if (opmode == me->opmode) {
            sal_memcpy(opmode_cfg, me_cfg, sizeof(*opmode_cfg));
            return SHR_E_NONE;
        }
    }

    return SHR_E_PARAM;
}

/*
 * \brief Get the constant operating mode.
 *
 * \param [in] opmode_cfg Operating mode configuration.
 * \param [out] opmode A pre-defined operating mode value from
 *                     \ref bcmpc_port_opmode_t.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_const_opmode_type_from_cfg(bcmpc_port_opmode_config_t *opmode_cfg,
                                bcmpc_port_opmode_t *opmode)
{
    int idx;
    const pmgr_opmode_map_t *me;
    const bcmpc_port_opmode_config_t *me_cfg;
    bcmpc_port_opmode_t def_opmode = BCMPC_PORT_OPMODE_INVALID;

    /*
     * Return the operating mode with the given interface, speed and duplex
     * mode. Otherwise, return the first found opmode with the given speed
     * and duplex mode.
     */

    for (idx = 0; idx < COUNTOF(pmgr_opmode_map); idx++) {
        me = &(pmgr_opmode_map[idx]);
        me_cfg = &(me->opmode_cfg);
        if (BCMPC_FIELD_VAL_EQUAL(opmode_cfg, me_cfg, speed) &&
            BCMPC_FIELD_VAL_EQUAL(opmode_cfg, me_cfg, hd)) {
            if (BCMPC_FIELD_VAL_EQUAL(opmode_cfg, me_cfg, phy_intf)) {
                *opmode = me->opmode;
                return SHR_E_NONE;
            }

            if (def_opmode == BCMPC_PORT_OPMODE_INVALID) {
                def_opmode = me->opmode;;
            }
        }
    }

    if (def_opmode != BCMPC_PORT_OPMODE_INVALID) {
        *opmode = def_opmode;
        return SHR_E_NONE;
    }

    return SHR_E_PARAM;
}

/*
 * \brief Get the custom operating mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pid Profile ID.
 * \param [out] opmode_cfg Operating mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_custom_opmode_id_to_cfg(int unit, uint32_t pid,
                             bcmpc_port_opmode_config_t *opmode_cfg)
{
    
    sal_memset(opmode_cfg, 0, sizeof(*opmode_cfg));

    return SHR_E_NONE;
}

/*
 * \brief Get the operating mode setting from port configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pcfg Port configuration.
 * \param [out] opmode_cfg Operating mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_opmode_cfg_get(int unit, bcmpc_port_cfg_t *pcfg,
                    bcmpc_port_opmode_config_t *opmode_cfg)
{
    if (pcfg->opmode == BCMPC_PORT_OPMODE_CUSTOM) {
        return pmgr_custom_opmode_id_to_cfg(unit, pcfg->custom_opmode_id,
                                            opmode_cfg);
    }

    return pmgr_const_opmode_type_to_cfg(pcfg->opmode, opmode_cfg);
}


/*******************************************************************************
 * Internal public functions: PMAC utilities
 */

int
bcmpc_pmgr_encap_mode_to_pmac(bcmpc_port_encap_t pmgr_mode,
                              bcmpmac_encap_mode_t *pmac_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_encap_map); idx++) {
        if (pmgr_mode == pmgr_encap_map[idx].pmgr_mode) {
            *pmac_mode = pmgr_encap_map[idx].pmac_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_PARAM;
}

int
bcmpc_pmgr_encap_mode_from_pmac(bcmpmac_encap_mode_t pmac_mode,
                                bcmpc_port_encap_t *pmgr_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_encap_map); idx++) {
        if (pmac_mode == pmgr_encap_map[idx].pmac_mode) {
            *pmgr_mode = pmgr_encap_map[idx].pmgr_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_PARAM;
}

int
bcmpc_pmgr_pm_mode_to_pmac(bcmpc_pm_mode_t *pm_mode,
                           bcmpmac_core_port_mode_t *pmac_mode)
{
    sal_memcpy(pmac_mode->lane_map, pm_mode->lane_map,
               sizeof(pmac_mode->lane_map));

    return SHR_E_NONE;
}


/*******************************************************************************
 * Internal public functions: PHYMOD utilities
 */

/*
 * \brief Get the default PHYMOD interface.
 *
 * The default interface type is derived from port speed, number of lanes and
 * encap mode.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_default_phymod_intf_get(int speed, int num_lanes, int is_hg,
                                   int fiber_pref, phymod_interface_t *intf)
{
    switch (num_lanes) {
        case 1:
            pmgr_phymod_intf_one_lane_get(speed, is_hg, fiber_pref, intf);
            break;
        case 2:
            pmgr_phymod_intf_two_lane_get(speed, is_hg, fiber_pref, intf);
            break;
        case 4:
            pmgr_phymod_intf_four_lane_get(speed, is_hg, fiber_pref, intf);
            break;
        case 8:
        case 10:
        case 12:
            *intf = phymodInterfaceCAUI;
            break;
        default:
            return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

/*
 * \brief Convert core clock value to PHYMOD reference clock type.
 *
 * \param [in] ref_clk Core reference clock in KHz.
 * \param [out] phymod_clk PHYMOD reference clock value.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_ref_clk_to_phy(int ref_clk, phymod_ref_clk_t *phymod_clk)
{
    switch (ref_clk) {
        case 0:
        case 156000:
            *phymod_clk = phymodRefClk156Mhz;
            break;
        case 125000:
            *phymod_clk = phymodRefClk125Mhz;
            break;
        case 106000:
            *phymod_clk = phymodRefClk106Mhz;
            break;
        case 161000:
            *phymod_clk = phymodRefClk161Mhz;
            break;
        case 174000:
            *phymod_clk = phymodRefClk174Mhz;
            break;
        case 312000:
            *phymod_clk = phymodRefClk312Mhz;
            break;
        case 322000:
            *phymod_clk = phymodRefClk322Mhz;
            break;
        case 349000:
            *phymod_clk = phymodRefClk349Mhz;
            break;
        case 644000:
            *phymod_clk = phymodRefClk644Mhz;
            break;
        case 698000:
            *phymod_clk = phymodRefClk698Mhz;
            break;
        case 155000:
            *phymod_clk = phymodRefClk155Mhz;
            break;
        case 156600:
            *phymod_clk = phymodRefClk156P6Mhz;
            break;
        case 157000:
            *phymod_clk = phymodRefClk157Mhz;
            break;
        case 158000:
            *phymod_clk = phymodRefClk158Mhz;
            break;
        case 159000:
            *phymod_clk = phymodRefClk159Mhz;
            break;
        case 168000:
            *phymod_clk = phymodRefClk168Mhz;
            break;
        case 172000:
            *phymod_clk = phymodRefClk172Mhz;
            break;
        case 173000:
            *phymod_clk = phymodRefClk173Mhz;
            break;
        case 169409:
            *phymod_clk = phymodRefClk169P409Mhz;
            break;
        case 348125:
            *phymod_clk = phymodRefClk348P125Mhz;
            break;
        case 162948:
            *phymod_clk = phymodRefClk162P948Mhz;
            break;
        case 336094:
            *phymod_clk = phymodRefClk336P094Mhz;
            break;
        case 168120:
            *phymod_clk = phymodRefClk168P12Mhz;
            break;
        case 346740:
            *phymod_clk = phymodRefClk346P74Mhz;
            break;
        case 167410:
            *phymod_clk = phymodRefClk167P41Mhz;
            break;
        case 345280:
            *phymod_clk = phymodRefClk345P28Mhz;
            break;
        case 162260:
            *phymod_clk = phymodRefClk162P26Mhz;
            break;
        case 334660:
            *phymod_clk = phymodRefClk334P66Mhz;
            break;
        default:
            return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

int
bcmpc_pmgr_opmode_to_phy(int unit, bcmpc_port_cfg_t *pcfg,
                         int num_lanes, int is_hg, int ref_clk,
                         phymod_phy_inf_config_t *intf_cfg)
{
    bcmpc_port_opmode_config_t opmode_cfg;
    phymod_interface_t intf_type;
    int fiber_pref = 0;

    SHR_FUNC_ENTER(unit);

    /* Get operating mode configuration. */
    SHR_IF_ERR_EXIT
        (pmgr_opmode_cfg_get(unit, pcfg, &opmode_cfg));

    if (opmode_cfg.phy_intf == BCMPC_PHY_INTF_SERDES) {
        fiber_pref = 1;
    }

    /* Convert operating mode configuration to phymod_interface_t. */
    if (is_hg || opmode_cfg.phy_intf == BCMPC_PHY_INTF_SERDES) {
        /*
         * Using the default interface when the user does not specify the PHY
         * interface or the encap mode is higig.
         */
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_default_phymod_intf_get(opmode_cfg.speed, num_lanes,
                                                is_hg, fiber_pref, &intf_type));
    } else {
        /*
         * Convert the user specified PHY interface to the corresponding PHYMOD
         * interface type.
         */
        SHR_IF_ERR_EXIT
            (pmgr_phy_intf_to_phymod(opmode_cfg.phy_intf, num_lanes,
                                     &intf_type));
    }

    /* Fill up phymod_phy_inf_config_t. */
    intf_cfg->data_rate = opmode_cfg.speed;
    intf_cfg->interface_type = intf_type;
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_ref_clk_to_phy(ref_clk, &intf_cfg->ref_clock));

    if (opmode_cfg.hd) {
        PHYMOD_INTF_MODES_HALF_DUPLEX_SET(intf_cfg);
    } else {
        PHYMOD_INTF_MODES_HALF_DUPLEX_CLR(intf_cfg);
    }
    pmgr_phymod_intf_media_mode_set(intf_cfg->interface_type, intf_cfg);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_opmode_from_phy(int unit, phymod_phy_inf_config_t *intf_cfg,
                           bcmpc_port_opmode_t *opmode)
{
    bcmpc_port_opmode_config_t opmode_cfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (pmgr_phy_intf_from_phymod(intf_cfg->interface_type,
                                   &opmode_cfg.phy_intf));

    opmode_cfg.speed = intf_cfg->data_rate;
    opmode_cfg.hd = PHYMOD_INTF_MODES_HALF_DUPLEX_GET(intf_cfg);

    SHR_IF_ERR_EXIT
        (pmgr_const_opmode_type_from_cfg(&opmode_cfg, opmode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pm_core_cfg_to_phy(int unit, bcmpc_pm_core_cfg_t *ccfg,
                              int num_lanes,
                              phymod_core_init_config_t *phymod_ccfg)
{
    int i;
    phymod_lane_map_t *lane_map = &phymod_ccfg->lane_map;
    phymod_polarity_t *polarity_map = &phymod_ccfg->polarity_map;

    SHR_FUNC_ENTER(unit);

    phymod_core_init_config_t_init(phymod_ccfg);

    phymod_ccfg->firmware_load_method = phymodFirmwareLoadMethodNone;
    lane_map->num_of_lanes = num_lanes;
    for (i = 0; i < num_lanes; i++) {
        lane_map->lane_map_rx[i] = BCMPC_LANE_MAP_GET(ccfg->rx_lane_map, i);
        lane_map->lane_map_tx[i] = BCMPC_LANE_MAP_GET(ccfg->tx_lane_map, i);
    }
    polarity_map->rx_polarity = ccfg->rx_polarity_flip;
    polarity_map->tx_polarity = ccfg->tx_polarity_flip;

    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_tx_lane_prof_to_phy(int unit, bcmpc_pm_tx_lane_prof_t *prof,
                               phymod_tx_t *phy_tx)
{
    SHR_FUNC_ENTER(unit);

    phymod_tx_t_init(phy_tx);

    phy_tx->amp = prof->amp;
    phy_tx->main = prof->main;
    phy_tx->pre = prof->pre;
    phy_tx->post = prof->post;
    phy_tx->post2 = prof->post2;
    phy_tx->post3 = prof->post3;
    phy_tx->drivermode = prof->drv_mode;

    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_rx_lane_prof_to_phy(int unit, bcmpc_pm_rx_lane_prof_t *prof,
                               int dfe_taps, phymod_rx_t *phy_rx)
{
    int idx;

    SHR_FUNC_ENTER(unit);

    phymod_rx_t_init(phy_rx);

    phy_rx->vga.enable = prof->rx_vga_override;
    phy_rx->vga.value = prof->rx_vga;
    phy_rx->peaking_filter.enable = prof->rx_peak_filter_override;
    phy_rx->peaking_filter.value = prof->rx_peak_filter;
    phy_rx->low_freq_peaking_filter.enable = prof->rx_lf_peak_filter_override;
    phy_rx->low_freq_peaking_filter.value = prof->rx_lf_peak_filter;
    phy_rx->num_of_dfe_taps = dfe_taps;
    for (idx = 0; idx < dfe_taps; idx++) {
        phy_rx->dfe[idx].enable = prof->rx_def_override[idx];
        phy_rx->dfe[idx].value = prof->rx_def[idx];
    }

    if (prof->rx_adap) {
        PHYMOD_RX_ADAPTATION_ON_SET(phy_rx);
    } else {
        PHYMOD_RX_ADAPTATION_ON_CLR(phy_rx);
    }

    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_tx_lane_status_from_phy(int unit, phymod_tx_t *phymod_tx,
                                   bcmpc_pm_tx_lane_prof_t *st_tx)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(st_tx, 0, sizeof(*st_tx));

    st_tx->amp = phymod_tx->amp;
    st_tx->main = phymod_tx->main;
    st_tx->pre = phymod_tx->pre;
    st_tx->post = phymod_tx->post;
    st_tx->post2 = phymod_tx->post2;
    st_tx->post3 = phymod_tx->post3;
    st_tx->drv_mode = phymod_tx->drivermode;

    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_rx_lane_status_from_phy(int unit, phymod_rx_t *phy_rx,
                                   bcmpc_pm_rx_lane_prof_t *st_rx)
{
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(st_rx, 0, sizeof(*st_rx));

    st_rx->rx_vga_override = phy_rx->vga.enable;
    st_rx->rx_vga = phy_rx->vga.value;
    st_rx->rx_peak_filter_override = phy_rx->peaking_filter.enable;
    st_rx->rx_peak_filter = phy_rx->peaking_filter.value;
    st_rx->rx_lf_peak_filter_override = phy_rx->low_freq_peaking_filter.enable;
    st_rx->rx_lf_peak_filter = phy_rx->low_freq_peaking_filter.value;

    for (idx = 0; idx < phy_rx->num_of_dfe_taps; idx++) {
        st_rx->rx_def_override[idx] = phy_rx->dfe[idx].enable;
        st_rx->rx_def[idx] = phy_rx->dfe[idx].value;
    }

    st_rx->rx_adap = PHYMOD_RX_ADAPTATION_ON_GET(phy_rx);

    SHR_FUNC_EXIT();
}

int bcmpc_port_opmode_to_speed(int unit, bcmpc_port_opmode_t opmode,
                               uint32_t *speed)
{
    bcmpc_port_opmode_config_t opmode_cfg;

    SHR_FUNC_ENTER(unit);

    *speed = 0;

    if (opmode == BCMPC_PORT_OPMODE_AUTONEG ||
        opmode == BCMPC_PORT_OPMODE_INVALID) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (pmgr_const_opmode_type_to_cfg(opmode, &opmode_cfg));
    *speed = opmode_cfg.speed;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_autoneg_prof_to_phy_ctrl(int unit, bcmpc_autoneg_prof_t *prof,
                               phymod_autoneg_control_t *phy_an_ctrl)
{
    SHR_FUNC_ENTER(unit);

    /* Protocol to phymod_an_mode_type_t. */
    if (BCMPC_AN_PROT_GET(prof, BCMPC_AN_PROT_CL37) &&
        BCMPC_AN_PROT_GET(prof, BCMPC_AN_PROT_SGMII)) {
        phy_an_ctrl->an_mode = phymod_AN_MODE_CL37_SGMII;
    } else if (BCMPC_AN_PROT_GET(prof, BCMPC_AN_PROT_CL37)) {
        phy_an_ctrl->an_mode = phymod_AN_MODE_CL37;
    } else if (BCMPC_AN_PROT_GET(prof, BCMPC_AN_PROT_BAM37)) {
        phy_an_ctrl->an_mode = phymod_AN_MODE_CL37BAM;
    } else if (BCMPC_AN_PROT_GET(prof, BCMPC_AN_PROT_CL73)) {
        phy_an_ctrl->an_mode = phymod_AN_MODE_CL73;
    } else if (BCMPC_AN_PROT_GET(prof, BCMPC_AN_PROT_BAM73)) {
        phy_an_ctrl->an_mode = phymod_AN_MODE_CL73BAM;
    } else if (BCMPC_AN_PROT_GET(prof, BCMPC_AN_PROT_SGMII)) {
        phy_an_ctrl->an_mode = phymod_AN_MODE_SGMII;
    }

    

    SHR_FUNC_EXIT();
}

int
bcmpc_autoneg_prof_to_phy_adv(int unit, int num_lanes, int is_hg,
                              bcmpc_autoneg_prof_t *prof,
                              phymod_autoneg_ability_t *phy_an_ability)
{
    uint32_t an_tech_ability = 0;
    uint32_t an_bam37_ability = 0;
    uint32_t an_bam73_ability = 0;

    SHR_FUNC_ENTER(unit);

    phymod_autoneg_ability_t_init(phy_an_ability);

    /*
     * an_tech_ability
     */
    if (num_lanes == 10 || num_lanes == 12) {
        /* 100G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_100G) ||
            BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_100G_CR)) {
            PHYMOD_AN_CAP_100G_CR10_SET(an_tech_ability);
        }
    } else if (num_lanes == 4) {
        /* 100G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_100G_CR)) {
            PHYMOD_AN_CAP_100G_CR4_SET(an_tech_ability);
        } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_100G)) {
            PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
        }

        /* 40G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G_CR)) {
            PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
        } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G) ||
                   BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G_KR)) {
            PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
        }

        /* 10G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G) ||
            BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G_KX)) {
            PHYMOD_AN_CAP_10G_KX4_SET(an_tech_ability);
        }
    } else if (num_lanes == 1) {
        

        /* 10G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G) ||
            BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G_KR)) {
            PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
        }

        /* 2.5G */
        
/*
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_2P5G)) {
            PHYMOD_AN_CAP_2P5G_X_SET(an_tech_ability);
        }
*/

        /* 1G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_1000M_X)) {
            PHYMOD_AN_CAP_1G_KX_SET(an_tech_ability);
        }

        /* 100M */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_100M_T)) {
            PHYMOD_AN_CAP_100M_SET(an_tech_ability);
        }
    }

    /*
     * an_bam73_ability
     */
    if (num_lanes == 4) {
        
    } else if (num_lanes == 2) {
        /* 20G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G_CR)) {
            PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_bam73_ability);
        } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G) ||
                   BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G_KR)) {
            PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
        }

        

        /* 40G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G_CR)) {
            PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_bam73_ability);
        } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G) ||
                   BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G_KR)) {
            PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_bam73_ability);
        }
    } else {
        /* num_lanes == 1 */
        
    }

    /*
     * an_bam37_ability
     */
    if (num_lanes == 4) {
        
        /* 40G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G) ||
            BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G_CR) ||
            BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_40G_KR)) {
            PHYMOD_BAM_CL37_CAP_40G_SET(an_bam37_ability);
        }

        /* 21G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_21G)) {
            PHYMOD_BAM_CL37_CAP_21G_X4_SET(an_bam37_ability);
        }

        /* 20G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G)) {
            PHYMOD_BAM_CL37_CAP_20G_X4_SET(an_bam37_ability);
        }
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G_CX)) {
            PHYMOD_BAM_CL37_CAP_20G_X4_CX4_SET(an_bam37_ability);
        }

        /* 13G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_13G)) {
            PHYMOD_BAM_CL37_CAP_13G_X4_SET(an_bam37_ability);
        }

        /* 10G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G_CX)) {
            PHYMOD_BAM_CL37_CAP_10G_CX4_SET(an_bam37_ability);
        } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G)) {
            PHYMOD_BAM_CL37_CAP_10G_HIGIG_SET(an_bam37_ability);
        }
    } else if (num_lanes == 2) {     /* 2 lanes */
        
        /* 20G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G) ||
            BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G_CR) ||
            BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G_KR)) {
            PHYMOD_BAM_CL37_CAP_20G_X2_SET(an_bam37_ability);
        }
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_20G_CX)) {
            PHYMOD_BAM_CL37_CAP_20G_X2_CX4_SET(an_bam37_ability);
        }

        /* 13G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_13G)) {
            PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_bam37_ability);
        }

        /* 10G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G_CX)) {
            PHYMOD_BAM_CL37_CAP_10G_X2_CX4_SET(an_bam37_ability);
        } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10G)) {
            PHYMOD_BAM_CL37_CAP_10G_DXGXS_SET(an_bam37_ability);
        }
    } else if (num_lanes == 1) {
        /* 2.5G */
        if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_2P5G)) {
            PHYMOD_BAM_CL37_CAP_2P5G_SET(an_bam37_ability);
        }
    }

    phy_an_ability->an_cap = an_tech_ability;
    phy_an_ability->cl73bam_cap = an_bam73_ability;
    phy_an_ability->cl37bam_cap = an_bam37_ability;

    /*
     * SGMII ability.
     */
    if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_1000M_X)) {
        PHYMOD_AN_CAP_SGMII_SET(phy_an_ability);
        phy_an_ability->sgmii_speed = phymod_CL37_SGMII_1000M;
    } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_100M_T)) {
        PHYMOD_AN_CAP_SGMII_SET(phy_an_ability);
        phy_an_ability->sgmii_speed = phymod_CL37_SGMII_100M;
    } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_10M_T)) {
        PHYMOD_AN_CAP_SGMII_SET(phy_an_ability);
        phy_an_ability->sgmii_speed = phymod_CL37_SGMII_10M;
    }

    /*
     * Pause ability
     */
    if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_PAUSE_TX) &&
        BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_PAUSE_RX)) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(phy_an_ability);
    } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_PAUSE_RX)) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(phy_an_ability);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(phy_an_ability);
    } else if (BCMPC_AN_ADV_GET(prof, BCMPC_AN_ADV_PAUSE_TX)) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(phy_an_ability);
    }

    /*
     * HIGIG ability
     */
    phy_an_ability->an_hg2 = is_hg ? 1 : 0;

    /*
     * FIXME: SUPPORT
     *
     * - CL72 setting. phy_an_ability->an_cl72
     * - FEC settings. PHYMOD_AN_FEC_XXX_SET(phy_an_ability->an_fec)
 */

    SHR_FUNC_EXIT();
}
