/*
 *         
 * $Id: bcm_common_defines.h $
 * 
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
#ifndef BCM_COMMON_DEFINES_H
#define BCM_COMMON_DEFINES_H
#define BCM_LINE_SIDE 0
#define BCM_SYSTEM_SIDE 1

#define BCM_NUM_DFE_TAPS 14
#define MAX_LANES_PER_CORE  16
#define _PLP_API_UNAVAILABLE 0xdead
#define BCM_PLP_WARM_BOOT (1 << 0)


typedef int (*bcm_plp_mutex_take_f)(unsigned int phy_id, void* platform_ctxt);

typedef int (*bcm_plp_mutex_give_f)(unsigned int phy_id, void* platform_ctxt);

typedef struct bcm_plp_mutex_s {
    bcm_plp_mutex_take_f mutex_take; /**< bcm mutex take*/
    bcm_plp_mutex_give_f mutex_give; /**< bcm mutex give*/
} bcm_plp_mutex_info_t;

typedef enum bcm_pm_interface_e {
    bcm_pm_InterfaceBypass = 0,
    bcm_pm_InterfaceSR,
    bcm_pm_InterfaceSR4,
    bcm_pm_InterfaceKX,
    bcm_pm_InterfaceKX4,
    bcm_pm_InterfaceKR,
    bcm_pm_InterfaceKR2,
    bcm_pm_InterfaceKR4,
    bcm_pm_InterfaceCX,
    bcm_pm_InterfaceCX2,
    bcm_pm_InterfaceCX4,
    bcm_pm_InterfaceCR,
    bcm_pm_InterfaceCR2,
    bcm_pm_InterfaceCR4,
    bcm_pm_InterfaceCR10,
    bcm_pm_InterfaceXFI,
    bcm_pm_InterfaceSFI,
    bcm_pm_InterfaceSFPDAC,
    bcm_pm_InterfaceXGMII,
    bcm_pm_Interface1000X,
    bcm_pm_InterfaceSGMII,
    bcm_pm_InterfaceXAUI,
    bcm_pm_InterfaceRXAUI,
    bcm_pm_InterfaceX2,
    bcm_pm_InterfaceXLAUI,
    bcm_pm_InterfaceXLAUI2,
    bcm_pm_InterfaceCAUI,
    bcm_pm_InterfaceQSGMII,
    bcm_pm_InterfaceLR4,
    bcm_pm_InterfaceLR,
    bcm_pm_InterfaceLR2,
    bcm_pm_InterfaceER,
    bcm_pm_InterfaceER2,
    bcm_pm_InterfaceER4,
    bcm_pm_InterfaceSR2,
    bcm_pm_InterfaceSR10,
    bcm_pm_InterfaceCAUI4,
    bcm_pm_InterfaceVSR,
    bcm_pm_InterfaceLR10,
    bcm_pm_InterfaceKR10,
    bcm_pm_InterfaceCAUI4_C2C,
    bcm_pm_InterfaceCAUI4_C2M,
    bcm_pm_InterfaceZR,
    bcm_pm_InterfaceLRM,
    bcm_pm_InterfaceXLPPI,
    bcm_pm_InterfaceCount
} bcm_pm_interface_t;
typedef bcm_pm_interface_t bcm_plp_pm_interface_t;

typedef enum bcm_pm_ref_clk_e {
    bcm_pm_RefClk156Mhz = 0, /* 156.25MHz */
    bcm_pm_RefClk125Mhz, /* 125Mhz */
    bcm_pm_RefClk106Mhz, /* 106.25Mhz */
    bcm_pm_RefClk161Mhz, /* 161.1328125Mhz */
    bcm_pm_RefClk174Mhz, /* 174.703125Mhz */
    bcm_pm_RefClk312Mhz, /* 312Mhz */
    bcm_pm_RefClk322Mhz, /* 322Mhz */
    bcm_pm_RefClk349Mhz, /* 349Mhz */
    bcm_pm_RefClk644Mhz, /* 644Mhz */
    bcm_pm_RefClk698Mhz, /* 698Mhz */
    bcm_pm_RefClk155Mhz, /* 155Mhz */
    bcm_pm_RefClk156P6Mhz, /* 156P6Mhz */
    bcm_pm_RefClk157Mhz, /* 157Mhz */
    bcm_pm_RefClk158Mhz, /* 158Mhz */
    bcm_pm_RefClk159Mhz, /* 159Mhz */
    bcm_pm_RefClk168Mhz, /* 168Mhz */
    bcm_pm_RefClk172Mhz, /* 172Mhz */
    bcm_pm_RefClk173Mhz, /* 173Mhz */
    bcm_pm_RefClkCount
} bcm_pm_ref_clk_t;
typedef bcm_pm_ref_clk_t bcm_plp_pm_ref_clk_t;

typedef enum bcm_pm_interface_mode_e {
    bcm_pm_Interface_mode_IEEE = 0,
    bcm_pm_Interface_mode_HIGIG,
    bcm_pm_Interface_mode_OTN,
    bcm_pm_Interface_mode_FIBER,
    bcm_pm_Interface_mode_proprietary=11,
    bcm_pm_Interface_mode_count
} bcm_pm_interface_mode_t;
typedef bcm_pm_interface_mode_t bcm_plp_pm_interface_mode_t;

typedef enum bcm_pm_prbs_poly_e {
    bcm_pm_PrbsPoly7 = 0,
    bcm_pm_PrbsPoly9,
    bcm_pm_PrbsPoly11,
    bcm_pm_PrbsPoly15,
    bcm_pm_PrbsPoly23,
    bcm_pm_PrbsPoly31,
    bcm_pm_PrbsPoly58,
    bcm_pm_PrbsPolyCount
} bcm_pm_prbs_poly_t;
typedef bcm_pm_prbs_poly_t bcm_plp_pm_prbs_poly_t;

typedef enum bcm_pm_osr_mode_e {
    bcmpmOversampleMode1 = 0,
    bcmpmOversampleMode2,
    bcmpmOversampleMode3,
    bcmpmOversampleMode3P3,
    bcmpmOversampleMode4,
    bcmpmOversampleMode5,
    bcmpmOversampleMode8,
    bcmpmOversampleMode8P25,
    bcmpmOversampleMode10,
    bcmpmOversampleModeCount
} bcm_pm_osr_mode_t;
typedef bcm_pm_osr_mode_t bcm_plp_pm_osr_mode_t;

typedef enum bcm_pm_pmd_mode_e {
    bcmpmPmdModeOs = 0,
    bcmpmPmdModeOsDfe,
    bcmpmPmdModeBrDfe,
    bcmpmPmdModeCount
} bcm_pm_pmd_mode_t;
typedef bcm_pm_pmd_mode_t bcm_plp_pm_pmd_mode_t;

typedef enum bcm_pm_sequencer_operation_e {
    bcmpmSeqOpStop = 0, /* Stop Sequencer */
    bcmpmSeqOpStart, /* Start Sequencer */
    bcmpmSeqOpRestart, /* Toggle Sequencer */
    bcmpmSeqOpCount
} bcm_pm_sequencer_operation_t;
typedef bcm_pm_sequencer_operation_t bcm_plp_pm_sequencer_operation_t;

typedef enum bcm_pm_phy_tx_lane_control_e {
    bcmpmTxTrafficDisable = 0, /* disable tx traffic */
    bcmpmTxTrafficEnable, /* enable tx traffic */
    bcmpmTxReset, /* reset tx data path */
    bcmpmTxSquelchOn, /* squelch tx */
    bcmpmTxSquelchOff, /* squelch tx off */
    bcmpmTxCount
} bcm_pm_phy_tx_lane_control_t;
typedef bcm_pm_phy_tx_lane_control_t bcm_plp_pm_phy_tx_lane_control_t;

typedef enum bcm_pm_phy_rx_lane_control_e {
    bcmpmRxReset, /* reset rx data path */
    bcmpmRxSquelchOn, /* squelch rx */
    bcmpmRxSquelchOff, /* squelch rx off */
    bcmpmRxCount
} bcm_pm_phy_rx_lane_control_t;
typedef bcm_pm_phy_rx_lane_control_t bcm_plp_pm_phy_rx_lane_control_t;

typedef enum bcm_pm_firmware_broadcast_method_e {
	bcmpmFirmwareBroadcastNone=0,                 /*    Firmware downloaded as unicast for each phy device in mdio bus\n*/
    bcmpmFirmwareBroadcastCoreReset,             /* Reset the core for all phy id in mdio bus */
    bcmpmFirmwareBroadcastEnable,                    /* Enable the broadcast for all phy id in mdio bus */
    bcmpmFirmwareBroadcastFirmwareExecute,     /* Load the FW for only one phy_id of similar type of phys in mdio bus */
    bcmpmFirmwareBroadcastFirmwareVerify,   /*  FW load verify for all phy id in mdio bus */
    bcmpmFirmwareBroadcastEnd,                            /* Disable the broadcast for all phy id in mdio bus */
    bcmpmFirmwareBroadcastCount
} bcm_pm_firmware_broadcast_method_t;
typedef bcm_pm_firmware_broadcast_method_t bcm_plp_firmware_broadcast_method_t;

typedef enum bcm_pm_firmware_load_method_e {
    bcmpmFirmwareLoadMethodNone = 0, /*Don't load FW*/
    bcmpmFirmwareLoadMethodInternal, /*Load FW internaly*/
    bcmpmFirmwareLoadMethodExternal, /*Load FW by a given function*/
    bcmpmFirmwareLoadMethodProgEEPROM, /*Load FW and flash it on to EEPROM */
    bcmpmFirmwareLoadMethodAuto,     /* Auto download in case of firmware change */
    bcmpmFirmwareLoadMethodCount
} bcm_pm_firmware_load_method_t;
typedef bcm_pm_firmware_load_method_t bcm_plp_pm_firmware_load_method_t;

/*!
 * @enum bcm_pm_firmware_load_force_e
 * @brief Firmware load force 
 */ 
typedef enum bcm_pm_firmware_load_force_e {
    bcmpmFirmwareLoadSkip = 0, /* Skip load FW */
    bcmpmFirmwareLoadForce, /* Force load FW */
    bcmpmFirmwareLoadAuto, /* Auto load FW in case of firmware change */
    bcmpmFirmwareLoadCount
} bcm_pm_firmware_load_force_t;
typedef bcm_pm_firmware_load_force_t bcm_plp_firmware_load_force_t;

typedef enum bcm_pm_reset_direction_e {
    bcmpmResetDirectionIn = 0, /* In Reset */
    bcmpmResetDirectionOut, /* Out of Reset */
    bcmpmResetDirectionInOut, /* Toggle Reset */
    bcmpmResetDirectionCount
} bcm_pm_reset_direction_t;
typedef bcm_pm_reset_direction_t bcm_plp_pm_reset_direction_t;

typedef enum bcm_pm_firmware_mode_e {
    bcm_pm_fw_default = 0,
    bcm_pm_fw_dfe,
    bcm_pm_fw_osdfe,
    bcm_pm_fw_br_dfe,
    bcm_pm_fw_lp_dfe,
    bcm_pm_fw_sfp_dac,
    bcm_pm_fw_xlaui,
    bcm_pm_fw_sfp_opt_sr4,
    bcm_pm_firmware_mode_Count
} bcm_pm_firmware_mode_t;
typedef bcm_pm_firmware_mode_t bcm_plp_pm_firmware_mode_t;

typedef enum bcm_pm_firmware_media_type_e {
    bcmplpFirmwareMediaTypePcbTraceBackPlane = 0, /**< back plane */
    bcmplpFirmwareMediaTypeCopperCable, /**< copper cable */
    bcmplpFirmwareMediaTypeOptics, /**< optical */
    bcmplpFirmwareMediaTypeCount
} bcm_pm_firmware_media_type_t;
typedef bcm_pm_firmware_media_type_t bcm_plp_pm_firmware_media_type_t;
/*!
 * @enum bcm_plp_failover_mode_e
 * @brief Failover configuration 
 */ 
 typedef enum bcm_plp_failover_mode_e {
    bcmplpFailovermodeNone,
    bcmplpFailovermodeEnable, /* enable Failover mode */
    bcmplpFailovermodeCount
} bcm_plp_failover_mode_t;

/*!
 * @enum bcm_plp_edc_config_method_e
 * @brief Configuration method for Electronic Dispersion Compensation (EDC) 
 */ 
typedef enum bcm_plp_edc_config_method_e {
    bcmplpEdcConfigMethodNone,
    bcmplpEdcConfigMethodHardware, /* EDC mode is set automatically by hardware */
    bcmplpEdcConfigMethodSoftware, /* EDC mode is selected by driver software */
    bcmplpEdcConfigMethodCount
} bcm_plp_edc_config_method_t;



/*! PHY Access Information
 *
 * \arg void *platform_ctxt \n
 *             Represents user data. This member is passed to\n
 *             register read/write APIs. It can be NULL if not used.
 *
 * \arg unsigned int phy_addr \n
 *             Represents PHY-ID\n
 *
 * \arg unsigned int if_side \n
 *             Represents the interface side \n
 *                    0 - line side of the PHY device\n
 *                    1 - system side of the PHY device\n
 *
 * \arg unsigned int lane_map \n
 *             Represents the Lane mapping of a port\n
 *             LSB Bit 0 represents lane 0 of the specified PHY-ID.\n
 *             LSB Bit 1 represents lane 1 of the specified PHY-ID\n
 *             and similarly for lane 2 to lane N,\n
 *             where N is the maximum number of lanes on a PHY.
 *             It also supports multicast\n
 *             Eg:
 *                   0x3 represents lane 0 and 1 \n
 *                   0xF represents lane 0 to lane 3\n
 *
 * \arg unsigned int flags \n
 *             init flags which is used in init API.
 *             Eg: BCM_PLP_WARM_BOOT (1 << 0)
 *             Flags usage will be extended in future as and when
 *             required.
 */
typedef struct bcm_plp_access_s {
    void *platform_ctxt;
    unsigned int phy_addr;
    unsigned int if_side;
    unsigned int lane_map;
    unsigned int flags;
}bcm_plp_access_t;

typedef struct bcm_plp_value_override_s {
    unsigned int enable;
    int value;
} bcm_plp_value_override_t;

typedef struct bcm_plp_rx_s {
    bcm_plp_value_override_t vga;
    unsigned int num_of_dfe_taps; /*number of elements in DFE array*/
    bcm_plp_value_override_t dfe[BCM_NUM_DFE_TAPS];
    bcm_plp_value_override_t peaking_filter;
    bcm_plp_value_override_t low_freq_peaking_filter;
} bcm_plp_rx_t;

typedef struct bcm_plp_device_aux_modes_s {
    unsigned int pass_thru;
} bcm_plp_device_aux_modes_t;

typedef struct bcm_plp_tx_s {
    char pre;
    char main;
    char post;
    char post2;
    char post3;
    char amp;
}bcm_plp_tx_t;

typedef struct bcm_plp_pm_diag_slicer_offset_s {
    unsigned int offset_pe;
    unsigned int offset_ze;
    unsigned int offset_me;
    unsigned int offset_po;
    unsigned int offset_zo;
    unsigned int offset_mo;
} bcm_plp_pm_diag_slicer_offset_t;

typedef struct bcm_plp_pm_diag_eyescan_s {
    unsigned int heye_left;
    unsigned int heye_right;
    unsigned int veye_upper;
    unsigned int veye_lower;
} bcm_plp_pm_diag_eyescan_t;

typedef struct bcm_plp_pm_phy_diagnostics_s {
    unsigned int signal_detect;
    unsigned int vga_bias_reduced;
    unsigned int postc_metric;
    bcm_plp_pm_osr_mode_t osr_mode;
    bcm_plp_pm_pmd_mode_t pmd_mode;
    unsigned int rx_lock;
    unsigned int rx_ppm;
    unsigned int tx_ppm;
    unsigned int clk90_offset;
    unsigned int clkp1_offset;
    unsigned int p1_lvl;
    unsigned int m1_lvl;
    unsigned int dfe1_dcd;
    unsigned int dfe2_dcd;
    unsigned int slicer_target;
    bcm_plp_pm_diag_slicer_offset_t slicer_offset;
    bcm_plp_pm_diag_eyescan_t eyescan;
    unsigned int state_machine_status;
    unsigned int link_time; /* Added as required by Falcon core */
    int pf_main;
    int pf_hiz;
    int pf_bst;
    int pf_low;
    int pf2_ctrl;
    int vga;
    int dc_offset;
    int p1_lvl_ctrl;
    int dfe1;
    int dfe2;
    int dfe3;
    int dfe4;
    int dfe5;
    int dfe6;
    int txfir_pre;
    int txfir_main;
    int txfir_post1;
    int txfir_post2;
    int txfir_post3;
    int tx_amp_ctrl;
    unsigned char br_pd_en; 
} bcm_plp_pm_phy_diagnostics_t;

/*! Lane swap configuration using TX/RX map
 *
 * \arg num_of_lanes \n
 *     Number of elements in lane_map_rx/tx arrays \n
 *
 * \arg lane_map_rx \n
 *     lane_map_rx[x]=y means that rx lane x is mapped to rx lane y \n
 *
 * \arg lane_map_tx \n
 *     lane_map_tx[x]=y means that tx lane x is mapped to tx lane y \n
 */
typedef struct bcm_plp_laneswap_map_s {
    /* Number of elements in lane_map_rx/tx arrays */
    unsigned int num_of_lanes;
    union {
        /* MDI pair swap doesn't differentiate Tx/Rx directions,  *\
        |* lanswap_map[] shares the same space with lane_map_rx[] *|
        |* don't care lane_map_tx[] when setting MDI Pair Swap.   *|
        \* lanswap_map[x]=y means that pair x is mapped to pair y */
        unsigned int lanswap_map[MAX_LANES_PER_CORE];
        /* lane_map_rx[x]=y means that rx lane x is mapped to rx lane y */
        unsigned int lane_map_rx[MAX_LANES_PER_CORE];
    };
    /* lane_map_tx[x]=y means that tx lane x is mapped to tx lane y */
    unsigned int lane_map_tx[MAX_LANES_PER_CORE];
} bcm_plp_laneswap_map_t;
typedef bcm_plp_laneswap_map_t bcm_laneswap_map_t;

typedef struct bcm_plp_pm_phy_reset_s {
    bcm_plp_pm_reset_direction_t rx;
    bcm_plp_pm_reset_direction_t tx;
} bcm_plp_pm_phy_reset_t;

typedef struct plp_static_config_s {
    unsigned int ull_dp;
} plp_static_config_t;

typedef struct bcm_plp_phy_static_config_s {
    unsigned int phy_id;
	plp_static_config_t* bcm_static_config;
} bcm_plp_phy_static_config_t;

typedef struct bcm_plp_pm_firmware_lane_config_s {
    bcm_plp_pm_firmware_mode_t firmware_mode;
    unsigned int ena_dis;
    unsigned int LaneConfigFromPCS;
    unsigned int AnEnabled; /**< Autoneg */
    bcm_plp_pm_firmware_media_type_t MediaType; /**< Media Type */
    unsigned int UnreliableLos; /**< For optical use */
    unsigned int ScramblingDisable; /**< disable scrambling */
    unsigned int Cl72AutoPolEn; /**< Forced CL72 */
    unsigned int Cl72RestTO; /**< Forced CL72 */
} bcm_plp_pm_firmware_lane_config_t;

typedef struct bcm_plp_firmware_load_type_s{
    bcm_pm_firmware_load_method_t firmware_load_method;
    bcm_pm_firmware_load_force_t  force_load_method;
}bcm_plp_firmware_load_type_t;

typedef struct bcm_plp_an_config_s{
    unsigned int master_lane;
    unsigned int cl72_en;
}bcm_plp_an_config_t;

typedef enum bcm_plp_interface_mode_e {
    bcmplpInterfaceModeIEEE=0x100000,
    bcmplpInterfaceModeHIGIG=0x10,
    bcmplpInterfaceModeOS2=0x20,
    bcmplpInterfaceModeSCR=0x40,
    bcmplpInterfaceModeHALFDUPLEX=0x80,
    bcmplpInterfaceModeFIBER=0x100,
    bcmplpInterfaceModeTRIPLECORE=0x200,
    bcmplpInterfaceModeTC343=0x400,
    bcmplpInterfaceModeTC442=0x800,
    bcmplpInterfaceModeTC244=0x1000,
    bcmplpInterfaceModeBACKPLANE=0x2000,
    bcmplpInterfaceModeCOPPER=0x4000,
    bcmplpInterfaceModeOTN=0x8000,
    bcmplpInterfaceModeSIMPLEX=0x10000,
    bcmplpInterfaceModeUNRELIABLELOS=0x20000,
}bcm_plp_interface_mode_t;
/* MAC */
typedef enum bcm_plp_mac_lb_type_e {
    bcmplpMacOuterloopback = 0,
    bcmplpMaxloopbackCnt
}bcm_plp_mac_lb_type_t;

typedef enum bcm_plp_mac_flow_control_e {
    bcmplpFlowcontrolTerminateGenerate = 0,
    bcmplpFlowcontrolPassthrough
} bcm_plp_mac_flow_control_t;

typedef enum bcm_mac_fault_option_e {
    bcmplpFaultoptionTerminateGenerate = 0,
    bcmplpFaultoptionPassthrough
} bcm_plp_mac_fault_option_t;

typedef struct bcm_plp_mac_pause_control_s {
    unsigned char rx_enable;
    unsigned char tx_enable;
    int refresh_timer; /* use -1 for disable this feature; Threshold for pause timer to cause XOFF to be resent */
    int xoff_timer; /* Time value sent in the Timer Field for classes in XOFF state */
} bcm_plp_mac_pause_control_t;

typedef struct bcm_plp_mac_pfc_control_s {
    unsigned char rx_enable;
    unsigned char tx_enable;
    unsigned char stats_en; /* enable PFC counters */
    unsigned char force_xon; /* Instructs MAC to send Xon message to all classes of service */
    int refresh_timer; /* use -1 for disable this feature; Threshold for pause timer to cause XOFF to be resent */
    int xoff_timer; /* Time value sent in the Timer Field for classes in XOFF state */
} bcm_plp_mac_pfc_control_t;

typedef struct bcm_plp_mac_access_s {
    bcm_plp_access_t phy_info;
}bcm_plp_mac_access_t;

typedef enum bcm_plp_fec_type_e {
    bcmplpFecType91 = 0,
    bcmplpFecType74
} bcm_plp_fec_type_t;

typedef struct bcm_plp_l1_intr_status_s {
    unsigned int l1_intr_sts;
    unsigned int reserved;
}bcm_plp_l1_intr_status_t;

/*************************************\
** BASE_T Specific Defines
 */

typedef enum  bcm_plp_base_t_interrupt_e {
    bcmplpBaseTIntrPtpPic      = (1 << 0),    /* PTP Interrupt INT_PIC_EN  */
    bcmplpBaseTIntrPtpFsync    = (1 << 1),    /* PTP Interrupt INT_FYNC_EN */
    bcmplpBaseTIntrCount
} bcm_plp_base_t_interrupt_e;
typedef bcm_plp_base_t_interrupt_e  bcm_plp_base_t_interrupt_t;

typedef enum  bcm_plp_base_t_speed_e {
    bcmplpBaseTSpeed0     = 0x0,         /* Not used */
    bcmplpBaseTSpeed10    = (1 << 0),    /*  10M     */
    bcmplpBaseTSpeed100   = (1 << 5),    /* 100M     */
    bcmplpBaseTSpeed1000  = (1 << 6),    /*   1G     */
    bcmplpBaseTSpeed2500  = (1 << 7),    /* 2.5G     */
    bcmplpBaseTSpeed5000  = (1 << 9),    /*   5G     */
    bcmplpBaseTSpeed10000 = (1 << 11),   /*  10G     */
    bcmplpBaseTSpeedCount
} bcm_plp_base_t_speed_t;   /* This structure needs to be in sync *\
                            \*  with its SDK counterpart          */

typedef enum  bcm_plp_base_t_pause_e {
    bcmplpBaseTPauseTx     = (1 << 0),		/* Tx pause */
    bcmplpBaseTPauseRx     = (1 << 1), 		/* Rx pause */
    bcmplpBaseTPauseAsymm  = (1 << 2), 		/* asymmetric pause */
    bcmplpBaseTPauseCount
} bcm_plp_base_t_pause_t;   /* This structure needs to be in sync *\
                            \*  with its SDK counterpart          */

typedef enum  bcm_plp_base_t_eee_ability_e {
    bcmplpBaseTEee100mBaseTX = (1 << 0),
    bcmplpBaseTEee1gBaseT    = (1 << 1),
    bcmplpBaseTEee10gBaseT   = (1 << 2),
    bcmplpBaseTEeeCount
    
} bcm_plp_base_t_eee_ability_t;

typedef enum bcm_plp_base_t_mdix_mode_e {
    bcmplpBaseTMdixAuto = 0,
    bcmplpBaseTMdixForceAuto,
    bcmplpBaseTMdixStraight,
    bcmplpBaseTMdixCrossover,
    bcmplpBaseTMdixCount
} bcm_plp_base_t_mdix_mode_t;   /* This structure needs to be in sync *\
                                \*  with its SDK counterpart          */

typedef enum bcm_plp_base_t_ms_mode_e {
    bcmplpBaseTMsModeSlave = 0,
    bcmplpBaseTMsModeMaster,
    bcmplpBaseTMsModeAuto,
    bcmplpBaseTMsModeNone,
    bcmplpBaseTMsModeCount
} bcm_plp_base_t_ms_mode_e;     /* This structure needs to be in sync *\
                                \*  with its SDK counterpart          */
typedef bcm_plp_base_t_ms_mode_e  bcm_plp_base_t_ms_mode_t;

typedef enum bcm_plp_base_t_power_mode_e { /* AutoNeg master/slave mode */
    bcmplpBaseTPowerModeNoChange = -1,   /* stay where you are */
    bcmplpBaseTPowerModeFull = 0,        /* full power */
    bcmplpBaseTPowerModeLow,             /* low power */
    bcmplpBaseTPowerModeAuto,            /* auto power */
    bcmplpBaseTPowerModeCount
} bcm_plp_base_t_power_mode_t;  /* This structure needs to be in sync *\
                                \*  with its SDK counterpart          */

/*!
 * @enum bcm_plp_base_t_ctrl_e
 * @brief Aux PHY Controls from setting BASE-T features thru bcm_mode_config_set()/_get() 
 */ 
typedef enum  bcm_plp_base_t_ctrl_e {
    bcmplpBaseTCtrlDuplex = 0,
    bcmplpBaseTCtrlMdix,
    bcmplpBaseTCtrlMdixStatus,
    bcmplpBaseTCtrlMasterSlave,
    bcmplpBaseTCtrlCount
} bcm_plp_base_t_ctrl_t;

typedef enum bcm_plp_base_t_eee_mode_e {
    bcmplpBaseTEeeModeNoChange = -1, /* stay where you are */
    bcmplpBaseTEeeModeNone = 0,      /* EEE off */
    bcmplpBaseTEeeModeAuto,          /* AutogrEEEn mode */
    bcmplpBaseTEeeModeNative,        /* native IEEE EEE mode */
    bcmplpBaseTEeeModeGetStats,      /* for getting stats info, no change on EEE mode */
    bcmplpBaseTEeeModeClearStats,    /* for clearing stats info, no change on EEE mode */
    bcmplpBaseTEeeModeCount
} bcm_plp_base_t_eee_mode_t;

typedef enum bcm_plp_base_t_eee_latency_e {
    bcmplpBaseTEeeLatencyNoChange = -1,  /* stay where you are */
    bcmplpBaseTEeeLatencyVariable = 0,   /* variable latency mode */
    bcmplpBaseTEeeLatencyFixed,          /* fixed latency mode */
    bcmplpBaseTEeeLatencyCount
} bcm_plp_base_t_eee_latency_t;

typedef struct bcm_plp_base_t_eee_stats_s {
    unsigned int  tx_events;     /* Tx event counts */
    unsigned int  tx_duration;   /* Tx LPI duration */
    unsigned int  rx_events;     /* Rx event counts */
    unsigned int  rx_duration;   /* Rx LPI duration */
} bcm_plp_base_t_eee_stats_t;

typedef struct bcm_plp_base_t_eee_s {
    bcm_plp_base_t_eee_mode_t mode;         /* EEE mode */
    bcm_plp_base_t_eee_latency_t latency;   /* EEE latency */
    bcm_plp_base_t_eee_stats_t stats;       /* EEE statistic info */
} bcm_plp_base_t_eee_t;

typedef struct bcm_plp_base_t_device_aux_modes_s {
    unsigned int  ctrl_select;
    unsigned int  ctrl_value;
    void         *ctrl_data_ptr;
} bcm_plp_base_t_device_aux_modes_t;

typedef struct bcm_plp_base_t_ability_s {
    bcm_plp_base_t_speed_t  speed_half_duplex;   /* speed abilities for HD */
    bcm_plp_base_t_speed_t  speed_full_duplex;   /* speed abilities for FD */
    bcm_plp_base_t_pause_t  pause;               /* flow control ability */
    bcm_plp_base_t_eee_ability_t  eee;           /* EEE abilities */
} bcm_plp_base_t_ability_t;

/* Cable Diagnostics */

typedef enum bcm_plp_base_t_cable_state_e {
    bcmplpBaseTCableStateOk,
    bcmplpBaseTCableStateOpen,
    bcmplpBaseTCableStateShort,
    bcmplpBaseTCableStateOpenShort,
    bcmplpBaseTCableStateCrosstalk,
    bcmplpBaseTCableStateUnknown,
    bcmplpBaseTCableStateCount      /* last, as always */
} bcm_plp_base_t_cable_state_t;

typedef struct bcm_plp_base_t_cable_diag_s {
    bcm_plp_base_t_cable_state_t    state;          /* state of all pairs */
    int                             npairs;         /* number of pairs */
    bcm_plp_base_t_cable_state_t    pair_state[4];  /* pair state */
    int                             pair_len[4];    /* pair length in metres */
    int                             fuzz_len;       /* len values +/- this */
} bcm_plp_base_t_cable_diag_t;

/* State/Manufacturing Diag Control  op_type  */
typedef enum bcm_plp_base_t_diag_ctrl_type_e {
    bcmplpBaseTDiagCtrlTypeGet  =  0,    /* a get action */
    bcmplpBaseTDiagCtrlTypeSet  =  1,    /* a set action  */
    bcmplpBaseTDiagCtrltypeCmd  =  2,    /* a seqeunce of command actions, */
    bcmplpBaseTDiagCtrlTypeCount
} bcm_plp_base_t_diag_ctrl_type_t;

/* State/Manufacturing Diag Control  op_cmd  */
typedef enum bcm_plp_base_t_diag_ctrl_cmd_e {
    bcmplpBaseTDiagCtrlCmdCableDiag  = 0,
    bcmplpBaseTDiagCtrlCmdMfgHybCanc = 0x08000001B,
    bcmplpBaseTDiagCtrlCmdMfgDenc,
    bcmplpBaseTDiagCtrlCmdMfgTxOn,
    bcmplpBaseTDiagCtrlCmdMfgExit,
    bcmplpBaseTDiagCtrlCmdStateTrace1,
    bcmplpBaseTDiagCtrlCmdStateTrace2,
    bcmplpBaseTDiagCtrlCmdStateWhereAmI,
    bcmplpBaseTDiagCtrlCmdStateTemp,
    bcmplpBaseTDiagCtrlCmdStateGeneric,
    bcmplpBaseTDiagCtrlCmdCount
} bcm_plp_base_t_diag_ctrl_cmd_t;


/*************************************\
** End of BASE_T Specific Defines
\*************************************/


/**************************************\
|**  1588 TimeSync Specific Defines  **|
\**************************************/

typedef struct bcm_plp_timesync_mpls_label_s {
    unsigned int value;     /* MPLS label bits [19:0] */
    unsigned int mask;      /* MPLS label mask bits [19:0] */
    unsigned int flags;     /* MPLS label flags */
} bcm_plp_timesync_mpls_label_t;

typedef struct bcm_plp_timesync_mpls_ctrl_s {
    unsigned int flags;                             /* Flags */
    unsigned int special_label;                     /* bits [19:0] */
    bcm_plp_timesync_mpls_label_t labels[10];   /* Timesync MPLS labels */
    int size;                           /* Number of elements in label array */
} bcm_plp_timesync_mpls_ctrl_t;


typedef enum bcm_plp_timesync_valid_e {
    bcmplpTimesyncValidFlags                 = (1U <<  0),
    bcmplpTimesyncValidItpid                 = (1U <<  1),
    bcmplpTimesyncValidOtpid                 = (1U <<  2),
    bcmplpTimesyncValidOtpid2                = (1U <<  3),
    bcmplpTimesyncValidGmode                 = (1U <<  4),
    bcmplpTimesyncValidFramesyncMode         = (1U <<  5),
    bcmplpTimesyncValidSyncOutMode           = (1U <<  6),
    bcmplpTimesyncValidTsDivider             = (1U <<  7),
    bcmplpTimesyncValidOriginalTimecode      = (1U <<  8),
    bcmplpTimesyncValidTxTimestampOffset     = (1U <<  9),
    bcmplpTimesyncValidRxTimestampOffset     = (1U << 10),
    bcmplpTimesyncValidTxSyncMode            = (1U << 11),
    bcmplpTimesyncValidTxDelayRequestMode    = (1U << 12),
    bcmplpTimesyncValidTxPdelayRequestMode   = (1U << 13),
    bcmplpTimesyncValidTxPdelayResponseMode  = (1U << 14),
    bcmplpTimesyncValidRxSyncMode            = (1U << 15),
    bcmplpTimesyncValidRxDelayRequestMode    = (1U << 16),
    bcmplpTimesyncValidRxPdelayRequestMode   = (1U << 17),
    bcmplpTimesyncValidRxPdelayResponseMode  = (1U << 18),
    bcmplpTimesyncValidMplsControl           = (1U << 19),
    bcmplpTimesyncValidRxLinkDelay           = (1U << 20),
    bcmplpTimesyncValid1588SyncFreq          = (1U << 21),
    bcmplpTimesyncValid1588DpllK1            = (1U << 22),
    bcmplpTimesyncValid1588DpllK2            = (1U << 23),
    bcmplpTimesyncValid1588DpllK3            = (1U << 24),
    bcmplpTimesyncValid1588DpllLoopFilter    = (1U << 25),
    bcmplpTimesyncValid1588DpllRefPhase      = (1U << 26),
    bcmplpTimesyncValid1588DpllRefPhaseDelta = (1U << 27),
    bcmplpTimesyncValid1588InbandTsControl   = (1U << 28),
    bcmplpTimesyncValidCount
} bcm_plp_timesync_valid_t;

typedef enum bcm_plp_timesync_flag_e {
    bcmplpTimesyncFlagCaptureStampEnable            = (1U<<0),
    bcmplpTimesyncFlagHeartbeatStampEnable          = (1U<<1),
    bcmplpTimesyncFlagRXCrcEnable                   = (1U<<2),
    bcmplpTimesyncFlag8021asEnable                  = (1U<<3),
    bcmplpTimesyncFlagL2Enable                      = (1U<<4),
    bcmplpTimesyncFlagIp4Enable                     = (1U<<5),
    bcmplpTimesyncFlagIp6Enable                     = (1U<<6),
    bcmplpTimesyncFlagClockSrcExt                   = (1U<<7),
    bcmplpTimesyncFlagClockSrcExtMode               = (1U<<8),
    bcmplpTimesyncFlag1588EncryptedMode             = (1U<<9),
    bcmplpTimesyncFlagFollowUpAssistEnable          = (1U<<10),
    bcmplpTimesyncFlagDelayRespAssistEnable         = (1U<<11),
    bcmplpTimesyncFlag64BitTimestampEnable          = (1U<<12),
    bcmplpTimesyncFlag1588OverHsrEnable             = (1U<<13),
    bcmplpTimesyncFlagCaptureTimestampTxSync        = (1U<<14),
    bcmplpTimesyncFlagCaptureTimestampTxDelayReq    = (1U<<15),
    bcmplpTimesyncFlagCaptureTimestampTxPdelayReq   = (1U<<16),
    bcmplpTimesyncFlagCaptureTimestampTxPdelayResp  = (1U<<17),
    bcmplpTimesyncFlagCaptureTimestampRxSync        = (1U<<18),
    bcmplpTimesyncFlagCaptureTimestampRxDelayReq    = (1U<<19),
    bcmplpTimesyncFlagCaptureTimestampRxPdelayReq   = (1U<<20),
    bcmplpTimesyncFlagCaptureTimestampRxPdelayResp  = (1U<<21),
    bcmplpTimesyncFlagCount
} bcm_plp_timesync_flag_t;

typedef enum bcm_plp_timesync_timer_mode_e {
    bcmplpTimesyncTimerModeNone    = 0x0,
    bcmplpTimesyncTimerModeDefault = 0x1,
    bcmplpTimesyncTimerMode32Bit   = 0x2,
    bcmplpTimesyncTimerMode48Bit   = 0x4,
    bcmplpTimesyncTimerMode64Bit   = 0x8,
    bcmplpTimesyncTimerMode80Bit   = 0x10,
    bcmplpTimesyncTimerModeCount
} bcm_plp_timesync_timer_mode_t;

typedef enum bcm_plp_timesync_global_mode_e {
    bcmplpTimesyncGLobalModeFree   = 0x0,
    bcmplpTimesyncGLobalModeSyncIn = 0x1,
    bcmplpTimesyncGLobalModeCpu    = 0x2,
    bcmplpTimesyncGLobalModeCount
} bcm_plp_timesync_global_mode_t;

typedef enum bcm_plp_timesync_framesync_mode_e {
    bcmplpTimesyncFramsyncModeNone    = 0x0,
    bcmplpTimesyncFramsyncModeSyncIn0 = 0x1,
    bcmplpTimesyncFramsyncModeSyncIn1 = 0x2,
    bcmplpTimesyncFramsyncModeSyncOut = 0x3,
    bcmplpTimesyncFramsyncModeCpu     = 0x4,
    bcmplpTimesyncFramsyncModeCount
} bcm_plp_timesync_framesync_mode_t;

typedef enum bcm_plp_timesync_event_msg_action_e {
    bcmplpTimesyncEventMsgActionNone,
    bcmplpTimesyncEventMsgActionEgrModeUpdateCorrectionField,
    bcmplpTimesyncEventMsgActionEgrModeReplaceCorrectionFieldOrigin,
    bcmplpTimesyncEventMsgActionEgrModeCaptureTimestamp,
    bcmplpTimesyncEventMsgActionIngModeUpdateCorrectionField,
    bcmplpTimesyncEventMsgActionIngModeInsertTimestamp,
    bcmplpTimesyncEventMsgActionIngModeInsertDelaytime,
    bcmplpTimesyncEventMsgActionCount
} bcm_plp_timesync_event_msg_action_t;

typedef enum bcm_plp_timesync_inband_option_e {
    bcmplpTimesyncInbandResv0IdCheck     = (1U << 0),
    bcmplpTimesyncInbandSyncEnable       = (1U << 1),
    bcmplpTimesyncInbandDelayReqEnable   = (1U << 2),
    bcmplpTimesyncInbandPdelayReqEnable  = (1U << 3),
    bcmplpTimesyncInbandPdelaYRespEnable = (1U << 4),
    bcmplpTimesyncInbandResv0IdUpdate    = (1U << 5),
    bcmplpTimesyncInbandCapSrcPortClkId  = (1U << 6),
    bcmplpTimesyncInbandMatchVlanId      = (1U << 7),
    bcmplpTimesyncInbandMatchSrcPortNum  = (1U << 8),
    bcmplpTimesyncInbandMatchMacAddr     = (1U << 9),
    bcmplpTimesyncInbandMatchIpAddr      = (1U << 10),
    bcmplpTimesyncInbandFollowUpAssit    = (1U << 11),
    bcmplpTimesyncInbandDelayRespAssit   = (1U << 12),
    bcmplpTimesyncInbandTimerModeSelect  = (1U << 13),
    bcmplpTimesyncInbandOptCount
} bcm_plp_timesync_inband_option_t;

/* bcm_plp_timesync_event_msg_action_t validation */
typedef struct bcm_plp_timesync_timer_adjust_s {
    bcm_plp_timesync_timer_mode_t mode;         /* Timer mode */
    int delta;                                  /* Delta */
} bcm_plp_timesync_timer_adjust_t;

typedef struct bcm_plp_timesync_inband_ctrl_s {
    unsigned int flags;                         /* Flags */
    int resv0_id;                               /* Reserved ID */
    bcm_plp_timesync_timer_mode_t timer_mode;   /* Timer mode */
} bcm_plp_timesync_inband_ctrl_t;

typedef struct bcm_plp_timesync_framesync_s {
    bcm_plp_timesync_framesync_mode_t mode;     /* Mode */
    unsigned int length_threshold;              /* Threshold */
    unsigned int event_offset;                  /* Even off set */
} bcm_plp_timesync_framesync_t;

typedef enum bcm_plp_timesync_syncout_mode_s {
  bcmplpTimesyncSyncoutModeDisable        = 0,
  bcmplpTimesyncSyncoutModeOneTime        = 1,
  bcmplpTimesyncSyncoutModePulseTrain     = 2,
  bcmplpTimesyncSyncoutModePulseTrainSync = 3,
  bcmplpTimesyncSyncoutModeCount
} bcm_plp_timesync_syncout_mode_t;

typedef struct bcm_plp_timesync_syncout_s {
    bcm_plp_timesync_syncout_mode_t mode;
    unsigned short pulse_1_length;    /* Pulse 1 length in nanoseconds */
    unsigned short pulse_2_length;    /* Pulse 2 length in nanoseconds */
    unsigned int interval;            /* Interval in nanoseconds */
    unsigned long long timestamp;     /* Syncout timestamp */
} bcm_plp_timesync_syncout_t;

typedef struct bcm_plp_timesync_timespec_s {
    unsigned char isnegative;         /* Sign identifier. */
    unsigned long long seconds;       /* Seconds absolute value. */
    unsigned int nanoseconds;         /* Nanoseconds absolute value. */
} bcm_plp_timesync_timespec_t;

/* PTP 1588 TimeSync configuration type. */
typedef struct bcm_plp_timesync_config_s {
    unsigned int validity_mask;       /* Validity mask */
    unsigned int flags;               /* Flags PHYMOD_TS_F_* */
    unsigned short itpid;             /* 1588 inner tag */
    unsigned short otpid;             /* 1588 outer tag */
    unsigned short otpid2;            /* 1588 outer tag2 */
    bcm_plp_timesync_timer_adjust_t timer_adjust;   /* Timer adjustment */
    bcm_plp_timesync_inband_ctrl_t inband_ctrl;     /* Inband TS control */
    bcm_plp_timesync_global_mode_t gmode;           /* Global mode */
    bcm_plp_timesync_syncout_t syncout;             /* Syncout */
    unsigned short ts_divider;                      /* TS divider */
    bcm_plp_timesync_timespec_t original_timecode;  /* Original timecode to be inserted */
    unsigned int rx_link_delay;         /* RX link delay */
    bcm_plp_timesync_event_msg_action_t tx_sync_mode;        /* sync */
    bcm_plp_timesync_event_msg_action_t tx_delay_req_mode;   /* delay request */
    bcm_plp_timesync_event_msg_action_t tx_pdelay_req_mode;  /* pdelay request */
    bcm_plp_timesync_event_msg_action_t tx_pdelay_resp_mode; /* pdelay response */
    bcm_plp_timesync_event_msg_action_t rx_sync_mode;        /* sync */
    bcm_plp_timesync_event_msg_action_t rx_delay_req_mode;   /* delay request */
    bcm_plp_timesync_event_msg_action_t rx_pdelay_req_mode;  /* pdelay request */
    bcm_plp_timesync_event_msg_action_t rx_pdelay_resp_mode; /* pdelay response */
    bcm_plp_timesync_mpls_ctrl_t mpls_ctrl;                  /* MPLS control */
    unsigned int sync_freq;                       /* sync frequency */
    unsigned short phy_1588_dpll_k1;              /* DPLL K1 */
    unsigned short phy_1588_dpll_k2;              /* DPLL K2 */
    unsigned short phy_1588_dpll_k3;              /* DPLL K3 */
    unsigned long long phy_1588_dpll_loop_filter;     /* DPLL loop filter */
    unsigned long long phy_1588_dpll_ref_phase;       /* DPLL ref phase */
    unsigned int phy_1588_dpll_ref_phase_delta; /* DPLL ref phase delta */
    unsigned int tx_inband_spare;               /* Tx inband spare*/
    unsigned int tx_inband_clear;               /* Tx inband clear*/
    unsigned int tx_inband32_format;            /* tx inband using 32-bit format*/
    unsigned int tx_inband_strict;              /* Packet action based in inband only*/
    unsigned int tx_mdio_sop_mem;               /* SOP MEM based on GEN1*/
    unsigned int tx_partial_tc;                 /* Compute CF with TXSOP from RSRD field*/
    unsigned int tx_mode_tc;                    /* Set node as TC node*/
    unsigned int rx_inband_spare;               /* Rx inband spare*/
    unsigned int rx_inband_insert_4byt;         /* Insert 32-bit after 1588 header*/
    unsigned int rx_inband32_format;            /* Rx inband using 32-bit format*/
    unsigned int rx_inband_strict;              /* Packet action based in inband only*/
    unsigned int rx_mdio_sop_mem;               /* SOP MEM based on GEN1*/
    unsigned int rx_partial_tc;                 /* Compute CF with RXSOP from RSRD field*/
    unsigned int rx_mode_tc;                    /* Set node as TC node*/
    bcm_plp_timesync_mpls_ctrl_t rx_mpls_ctrl;               /* RX MPLS control */
} bcm_plp_timesync_config_t;

/*******************************************\
|** End of 1588 TimeSync Specific Defines **|
\*******************************************/

#endif /* BCM_COMMON_DEFINES_H */

