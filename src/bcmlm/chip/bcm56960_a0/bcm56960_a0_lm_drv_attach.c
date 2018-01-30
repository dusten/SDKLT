/*! \file bcm56960_a0_lm_drv_attach.c
 *
 * Linkscan chip-specific driver for BCM56960.
 *
 * 1. Hardware linkscan driver is used to configure CMICd to monitor port
 *    link status by reading PHY registers.
 *
 *    Internal phy address:
 *      Bus 0: PHYID 1-24 are mapped to physical ports 1-24
 *      Bus 1: PHYID 1-16 are mapped to physical ports 25-40
 *      Bus 2: PHYID 1-24 are mapped to physical ports 41-64
 *      Bus 3: PHYID 1-24 are mapped to physical ports 65-88
 *      Bus 4: PHYID 1-16 are mapped to physical ports 89-104
 *      Bus 5: PHYID 1-24 are mapped to physical ports 105-128
 *      Bus 6: PHYID 1 is mapped to physical port 129 and
 *             PHYID 3 is mapped to physical port 131
 *
 *    Note: CMICd omits CPU port from the PHY counting
 *
 * 2. Software linkscan driver in this file is used to provide BCM56960
 *    specific information related to linkscan.
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
#include <shr/shr_timeout.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/chip/bcm56960_a0_cmic_acc.h>
#include <bcmdrd/chip/bcm56960_a0_cmic_defs.h>

#include "../bcmlm_chip_internal.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE  BSL_LS_BCMLM_INIT

/*! Logical port number of CPU and loopback ports. */
#define LPORT_CMIC   0
#define LPORT_LPBK_0 33
#define LPORT_LPBK_1 67
#define LPORT_LPBK_2 101
#define LPORT_LPBK_3 135
#define LPORT_RSVD   134

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Select the appropriate MDIO bus.
 *
 * \param [in] unit Unit number.
 *
 * \return Nothing.
 */
static void
lm_cmic_miim_bus_sel(int unit)
{
    CMIC_MIIM_BUS_SEL_MAP_9_0r_t bsm0;
    CMIC_MIIM_BUS_SEL_MAP_19_10r_t bsm1;
    CMIC_MIIM_BUS_SEL_MAP_29_20r_t bsm2;
    CMIC_MIIM_BUS_SEL_MAP_39_30r_t bsm3;
    CMIC_MIIM_BUS_SEL_MAP_49_40r_t bsm4;
    CMIC_MIIM_BUS_SEL_MAP_59_50r_t bsm5;
    CMIC_MIIM_BUS_SEL_MAP_69_60r_t bsm6;
    CMIC_MIIM_BUS_SEL_MAP_79_70r_t bsm7;
    CMIC_MIIM_BUS_SEL_MAP_89_80r_t bsm8;
    CMIC_MIIM_BUS_SEL_MAP_99_90r_t bsm9;
    CMIC_MIIM_BUS_SEL_MAP_109_100r_t bsm10;
    CMIC_MIIM_BUS_SEL_MAP_119_110r_t bsm11;
    CMIC_MIIM_BUS_SEL_MAP_129_120r_t bsm12;
    CMIC_MIIM_BUS_SEL_MAP_139_130r_t bsm13;
    CMIC_MIIM_BUS_SEL_MAP_149_140r_t bsm14;
    CMIC_MIIM_BUS_SEL_MAP_159_150r_t bsm15;
    CMIC_MIIM_BUS_SEL_MAP_169_160r_t bsm16;
    CMIC_MIIM_BUS_SEL_MAP_179_170r_t bsm17;
    CMIC_MIIM_BUS_SEL_MAP_189_180r_t bsm18;
    CMIC_MIIM_BUS_SEL_MAP_191_190r_t bsm19;
    uint32_t rval[20];
    int phy;

    sal_memset(rval, 0, sizeof(rval));

    for (phy = 0; phy <= 131; phy++) {
        if (phy < 40) {
            rval[phy/10] |= 1 << ((phy % 10) * 3);
        } else if (phy < 64) {
            rval[phy/10] |= 2 << ((phy % 10) * 3);
        } else if (phy < 88) {
            rval[phy/10] |= 3 << ((phy % 10) * 3);
        } else if (phy < 104) {
            rval[phy/10] |= 4 << ((phy % 10) * 3);
        } else if (phy < 128) {
            rval[phy/10] |= 5 << ((phy % 10) * 3);
        } else if (phy == 128 || phy == 130) {
            rval[phy/10] |= 6 << ((phy % 10) * 3);
        }
    }

    CMIC_MIIM_BUS_SEL_MAP_9_0r_SET(bsm0, rval[0]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_9_0r(unit, bsm0);
    CMIC_MIIM_BUS_SEL_MAP_19_10r_SET(bsm1, rval[1]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_19_10r(unit, bsm1);
    CMIC_MIIM_BUS_SEL_MAP_29_20r_SET(bsm2, rval[2]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_29_20r(unit, bsm2);
    CMIC_MIIM_BUS_SEL_MAP_39_30r_SET(bsm3, rval[3]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_39_30r(unit, bsm3);
    CMIC_MIIM_BUS_SEL_MAP_49_40r_SET(bsm4, rval[4]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_49_40r(unit, bsm4);
    CMIC_MIIM_BUS_SEL_MAP_59_50r_SET(bsm5, rval[5]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_59_50r(unit, bsm5);
    CMIC_MIIM_BUS_SEL_MAP_69_60r_SET(bsm6, rval[6]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_69_60r(unit, bsm6);
    CMIC_MIIM_BUS_SEL_MAP_79_70r_SET(bsm7, rval[7]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_79_70r(unit, bsm7);
    CMIC_MIIM_BUS_SEL_MAP_89_80r_SET(bsm8, rval[8]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_89_80r(unit, bsm8);
    CMIC_MIIM_BUS_SEL_MAP_99_90r_SET(bsm9, rval[9]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_99_90r(unit, bsm9);
    CMIC_MIIM_BUS_SEL_MAP_109_100r_SET(bsm10, rval[10]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_109_100r(unit, bsm10);
    CMIC_MIIM_BUS_SEL_MAP_119_110r_SET(bsm11, rval[11]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_119_110r(unit, bsm11);
    CMIC_MIIM_BUS_SEL_MAP_129_120r_SET(bsm12, rval[12]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_129_120r(unit, bsm12);
    CMIC_MIIM_BUS_SEL_MAP_139_130r_SET(bsm13, rval[13]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_139_130r(unit, bsm13);
    CMIC_MIIM_BUS_SEL_MAP_149_140r_SET(bsm14, rval[14]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_149_140r(unit, bsm14);
    CMIC_MIIM_BUS_SEL_MAP_159_150r_SET(bsm15, rval[15]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_159_150r(unit, bsm15);
    CMIC_MIIM_BUS_SEL_MAP_169_160r_SET(bsm16, rval[16]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_169_160r(unit, bsm16);
    CMIC_MIIM_BUS_SEL_MAP_179_170r_SET(bsm17, rval[17]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_179_170r(unit, bsm17);
    CMIC_MIIM_BUS_SEL_MAP_189_180r_SET(bsm18, rval[18]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_189_180r(unit, bsm18);
    CMIC_MIIM_BUS_SEL_MAP_191_190r_SET(bsm19, rval[19]);
    WRITE_CMIC_MIIM_BUS_SEL_MAP_191_190r(unit, bsm19);
}

/*!
 * \brief Select internal phy.
 *
 * \param [in] unit Unit number.
 *
 * \return Nothing.
 */
static void
lm_cmic_miim_int_sel(int unit)
{
    CMIC_MIIM_INT_SEL_MAP_0r_t map0;
    CMIC_MIIM_INT_SEL_MAP_1r_t map1;
    CMIC_MIIM_INT_SEL_MAP_2r_t map2;
    CMIC_MIIM_INT_SEL_MAP_3r_t map3;
    CMIC_MIIM_INT_SEL_MAP_4r_t map4;

    CMIC_MIIM_INT_SEL_MAP_0r_SET(map0, 0xFFFFFFFF);
    WRITE_CMIC_MIIM_INT_SEL_MAP_0r(unit, map0);

    CMIC_MIIM_INT_SEL_MAP_1r_SET(map1, 0xFFFFFFFF);
    WRITE_CMIC_MIIM_INT_SEL_MAP_1r(unit, map1);

    CMIC_MIIM_INT_SEL_MAP_2r_SET(map2, 0xFFFFFFFF);
    WRITE_CMIC_MIIM_INT_SEL_MAP_2r(unit, map2);

    CMIC_MIIM_INT_SEL_MAP_3r_SET(map3, 0xFFFFFFFF);
    WRITE_CMIC_MIIM_INT_SEL_MAP_3r(unit, map3);

    CMIC_MIIM_INT_SEL_MAP_4r_SET(map4, 0x00000005);
    WRITE_CMIC_MIIM_INT_SEL_MAP_4r(unit, map4);
}

/*!
 *  \brief Set MIIM phy address map.
 *
 *  \param [in] unit Unit number.
 *
 *  \return Nothing.
 */
static void
lm_cmic_miim_phy_addr_map(int unit)
{
    CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r_t map0;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r_t map1;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r_t map2;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r_t map3;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r_t map4;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r_t map5;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r_t map6;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r_t map7;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r_t map8;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r_t map9;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r_t map10;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r_t map11;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r_t map12;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r_t map13;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_59_56r_t map14;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_63_60r_t map15;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_67_64r_t map16;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_71_68r_t map17;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_75_72r_t map18;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_79_76r_t map19;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_83_80r_t map20;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_87_84r_t map21;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_91_88r_t map22;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_95_92r_t map23;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_99_96r_t map24;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_103_100r_t map25;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_107_104r_t map26;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_111_108r_t map27;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_115_112r_t map28;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_119_116r_t map29;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_123_120r_t map30;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_127_124r_t map31;
    CMIC_MIIM_EXT_PHY_ADDR_MAP_131_128r_t map32;
    CMIC_MIIM_SCAN_CTRLr_t ctrl;
    int pport, phyid;
    uint32_t rval[33];

    sal_memset(rval, 0, sizeof(rval));

    phyid = 1;
    for (pport = 0; pport <= 131; pport++) {
        if (pport == 0 || pport == 24 || pport == 40 || pport == 64 ||
            pport == 88 || pport == 104 || pport == 128) {
            phyid = 1;
        }
        rval[pport/4] |= phyid << ((pport % 4) * 8);
        phyid++;
    }

    CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r_SET(map0, rval[0]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r(unit, map0);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r_SET(map1, rval[1]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r(unit, map1);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r_SET(map2, rval[2]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r(unit, map2);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r_SET(map3, rval[3]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r(unit, map3);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r_SET(map4, rval[4]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r(unit, map4);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r_SET(map5, rval[5]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r(unit, map5);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r_SET(map6, rval[6]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r(unit, map6);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r_SET(map7, rval[7]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r(unit, map7);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r_SET(map8, rval[8]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r(unit, map8);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r_SET(map9, rval[9]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r(unit, map9);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r_SET(map10, rval[10]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r(unit, map10);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r_SET(map11, rval[11]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r(unit, map11);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r_SET(map12, rval[12]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r(unit, map12);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r_SET(map13, rval[13]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r(unit, map13);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_59_56r_SET(map14, rval[14]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_59_56r(unit, map14);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_63_60r_SET(map15, rval[15]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_63_60r(unit, map15);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_67_64r_SET(map16, rval[16]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_67_64r(unit, map16);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_71_68r_SET(map17, rval[17]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_71_68r(unit, map17);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_75_72r_SET(map18, rval[18]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_75_72r(unit, map18);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_79_76r_SET(map19, rval[19]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_79_76r(unit, map19);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_83_80r_SET(map20, rval[20]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_83_80r(unit, map20);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_87_84r_SET(map21, rval[21]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_87_84r(unit, map21);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_91_88r_SET(map22, rval[22]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_91_88r(unit, map22);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_95_92r_SET(map23, rval[23]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_95_92r(unit, map23);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_99_96r_SET(map24, rval[24]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_99_96r(unit, map24);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_103_100r_SET(map25, rval[25]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_103_100r(unit, map25);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_107_104r_SET(map26, rval[26]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_107_104r(unit, map26);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_111_108r_SET(map27, rval[27]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_111_108r(unit, map27);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_115_112r_SET(map28, rval[28]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_115_112r(unit, map28);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_119_116r_SET(map29, rval[29]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_119_116r(unit, map29);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_123_120r_SET(map30, rval[30]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_123_120r(unit, map30);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_127_124r_SET(map31, rval[31]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_127_124r(unit, map31);
    CMIC_MIIM_EXT_PHY_ADDR_MAP_131_128r_SET(map32, rval[32]);
    WRITE_CMIC_MIIM_EXT_PHY_ADDR_MAP_131_128r(unit, map32);

    READ_CMIC_MIIM_SCAN_CTRLr(unit, &ctrl);
    CMIC_MIIM_SCAN_CTRLr_MIIM_ADDR_MAP_ENABLEf_SET(ctrl, 1);
    WRITE_CMIC_MIIM_SCAN_CTRLr(unit, ctrl);
}

/*!
 *  \brief Set the Auto Scan Address.
 *
 *  \param [in] unit Unit number.
 *
 *  \return Nothing.
 */
static void
lm_cmic_miim_auto_scan_addr_set(int unit)
{
    CMIC_MIIM_AUTO_SCAN_ADDRESSr_t rval;

    READ_CMIC_MIIM_AUTO_SCAN_ADDRESSr(unit, &rval);
    CMIC_MIIM_AUTO_SCAN_ADDRESSr_MIIM_LINK_STATUS_BIT_POSITIONf_SET(rval, 2);
    CMIC_MIIM_AUTO_SCAN_ADDRESSr_CLAUSE_22_REGADRf_SET(rval, 1);
    CMIC_MIIM_AUTO_SCAN_ADDRESSr_CLAUSE_45_REGADRf_SET(rval, 1);
    CMIC_MIIM_AUTO_SCAN_ADDRESSr_CLAUSE_45_DTYPEf_SET(rval, 1);
    WRITE_CMIC_MIIM_AUTO_SCAN_ADDRESSr(unit, rval);
}

/*!
 *  \brief Hardware linkscan initialization.
 *
 *  \param [in] unit Unit number.
 *
 *  \retval SHR_E_NONE No error.
 */
static int
bcm56960_a0_lm_hw_init(int unit)
{
    /*
     * Select the appropriate MDIO bus
     */
    lm_cmic_miim_bus_sel(unit);

    /*
     * Select internal phy
     */
    lm_cmic_miim_int_sel(unit);

    /*
     * Set MIIM phy port map
     */
    lm_cmic_miim_phy_addr_map(unit);

    /*
     * Set the Auto Scan Address
     */
    lm_cmic_miim_auto_scan_addr_set(unit);

    return SHR_E_NONE;
}

/*!
 *  \brief Hardware linkscan port config.
 *
 *  \param [in] unit Unit number.
 *  \param [in] pbm Physical port bit map.
 *
 *  \retval SHR_E_NONE No error.
 */
static int
bcm56960_a0_lm_hw_config(int unit, bcmdrd_pbmp_t pbm)
{
    shr_port_t pport;
    bcmdrd_pbmp_t phy_pbm;
    CMIC_MIIM_SCAN_PORTS_0r_t p0;
    CMIC_MIIM_SCAN_PORTS_1r_t p1;
    CMIC_MIIM_SCAN_PORTS_2r_t p2;
    CMIC_MIIM_SCAN_PORTS_3r_t p3;
    CMIC_MIIM_SCAN_PORTS_4r_t p4;
    CMIC_MIIM_SCAN_PORTS_5r_t p5;

    if (BCMDRD_PBMP_NOT_NULL(pbm)) {
        bcmbd_cmicd_intr_enable(unit,
                                0, /* CMC 0 */
                                CMICD_IRQ_LINK_STAT_MOD);
    } else {
        bcmbd_cmicd_intr_disable(unit,
                                 0, /* CMC 0 */
                                 CMICD_IRQ_LINK_STAT_MOD);
    }

    BCMDRD_PBMP_CLEAR(phy_pbm);
    BCMDRD_PBMP_ITER(pbm, pport) {
        BCMDRD_PBMP_PORT_ADD(phy_pbm, (pport - 1));
    }

    CMIC_MIIM_SCAN_PORTS_0r_SET(p0, BCMDRD_PBMP_WORD_GET(phy_pbm, 0));
    WRITE_CMIC_MIIM_SCAN_PORTS_0r(unit, p0);

    CMIC_MIIM_SCAN_PORTS_1r_SET(p1, BCMDRD_PBMP_WORD_GET(phy_pbm, 1));
    WRITE_CMIC_MIIM_SCAN_PORTS_1r(unit, p1);

    CMIC_MIIM_SCAN_PORTS_2r_SET(p2, BCMDRD_PBMP_WORD_GET(phy_pbm, 2));
    WRITE_CMIC_MIIM_SCAN_PORTS_2r(unit, p2);

    CMIC_MIIM_SCAN_PORTS_3r_SET(p3, BCMDRD_PBMP_WORD_GET(phy_pbm, 3));
    WRITE_CMIC_MIIM_SCAN_PORTS_3r(unit, p3);

    CMIC_MIIM_SCAN_PORTS_4r_SET(p4, BCMDRD_PBMP_WORD_GET(phy_pbm, 4));
    WRITE_CMIC_MIIM_SCAN_PORTS_4r(unit, p4);

    CMIC_MIIM_SCAN_PORTS_5r_SET(p5, BCMDRD_PBMP_WORD_GET(phy_pbm, 5));
    WRITE_CMIC_MIIM_SCAN_PORTS_5r(unit, p5);

    return SHR_E_NONE;
}

/*!
 *  \brief Get link status.
 *
 *  \param [in] unit Unit number.
 *  \param [out] pbm Bit map of physical port with link up.
 *
 *  \retval SHR_E_NONE No error.
 */
static int
bcm56960_a0_lm_hw_link_get(int unit, bcmdrd_pbmp_t *pbm)
{
    CMIC_MIIM_LINK_STATUS_0r_t st0;
    CMIC_MIIM_LINK_STATUS_1r_t st1;
    CMIC_MIIM_LINK_STATUS_2r_t st2;
    CMIC_MIIM_LINK_STATUS_3r_t st3;
    CMIC_MIIM_LINK_STATUS_4r_t st4;
    CMIC_MIIM_LINK_STATUS_5r_t st5;
    bcmdrd_pbmp_t phy_pbm;
    uint32_t pb;
    int phy;

    BCMDRD_PBMP_CLEAR(phy_pbm);

    READ_CMIC_MIIM_LINK_STATUS_0r(unit, &st0);
    pb = CMIC_MIIM_LINK_STATUS_0r_PORT_BITMAPf_GET(st0);
    BCMDRD_PBMP_WORD_SET(phy_pbm, 0, pb);

    READ_CMIC_MIIM_LINK_STATUS_1r(unit, &st1);
    pb = CMIC_MIIM_LINK_STATUS_1r_PORT_BITMAPf_GET(st1);
    BCMDRD_PBMP_WORD_SET(phy_pbm, 1, pb);

    READ_CMIC_MIIM_LINK_STATUS_2r(unit, &st2);
    pb = CMIC_MIIM_LINK_STATUS_2r_PORT_BITMAPf_GET(st2);
    BCMDRD_PBMP_WORD_SET(phy_pbm, 2, pb);

    READ_CMIC_MIIM_LINK_STATUS_3r(unit, &st3);
    pb = CMIC_MIIM_LINK_STATUS_3r_PORT_BITMAPf_GET(st3);
    BCMDRD_PBMP_WORD_SET(phy_pbm, 3, pb);

    READ_CMIC_MIIM_LINK_STATUS_4r(unit, &st4);
    pb = CMIC_MIIM_LINK_STATUS_4r_PORT_BITMAPf_GET(st4);
    BCMDRD_PBMP_WORD_SET(phy_pbm, 4, pb);

    READ_CMIC_MIIM_LINK_STATUS_5r(unit, &st5);
    pb = CMIC_MIIM_LINK_STATUS_5r_PORT_BITMAPf_GET(st5);
    BCMDRD_PBMP_WORD_SET(phy_pbm, 5, pb);

    BCMDRD_PBMP_ITER(phy_pbm, phy) {
        BCMDRD_PBMP_PORT_ADD(*pbm, (phy + 1));
    }

    return SHR_E_NONE;
}

/*!
 *  \brief Clear hardware linkscan interrupt.
 *
 *  \param [in] unit Unit number.
 *
 *  \retval SHR_E_NONE No error.
 */
static int
bcm56960_a0_lm_hw_intr_clear(int unit)
{
    CMIC_MIIM_SCAN_STATUSr_t st;
    CMIC_MIIM_CLR_SCAN_STATUSr_t clr_st;
    uint32_t rval;

    READ_CMIC_MIIM_SCAN_STATUSr(unit, &st);
    rval = CMIC_MIIM_SCAN_STATUSr_GET(st);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "SCAN STATUS: 0x%08x.\n"), rval));

    CMIC_MIIM_CLR_SCAN_STATUSr_SET(clr_st, rval);
    WRITE_CMIC_MIIM_CLR_SCAN_STATUSr(unit, clr_st);
    return SHR_E_NONE;
}

/*!
 *  \brief Register callback for hardware linkscan interrupt.
 *
 *  \param [in] unit Unit number.
 *  \param [in] intr_func Callback function.
 *
 *  \retval SHR_E_NONE No error.
 *  \retval SHR_E_PARAM Invalid CMC or interrupt source.
 */
static int
bcm56960_a0_lm_hw_intr_func_set(int unit, bcmbd_intr_f intr_func)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_cmicd_intr_func_set(unit,
                                   0, /* CMC 0 */
                                   CMICD_IRQ_LINK_STAT_MOD,
                                   intr_func,
                                   0));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stop hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT Failed to disable hw linkscan.
 */
static int
bcm56960_a0_lm_hw_scan_stop(int unit)
{
    CMIC_MIIM_SCAN_CTRLr_t ctrl;
    CMIC_MIIM_SCAN_STATUSr_t st;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = 1000000; /* 1 second */

    SHR_FUNC_ENTER(unit);

    READ_CMIC_MIIM_SCAN_CTRLr(unit, &ctrl);
    CMIC_MIIM_SCAN_CTRLr_MIIM_LINK_SCAN_ENf_SET(ctrl, 0);
    WRITE_CMIC_MIIM_SCAN_CTRLr(unit, ctrl);

    shr_timeout_init(&to, timeout_usec, 0);
    while (1) {
        READ_CMIC_MIIM_SCAN_STATUSr(unit, &st);
        if (CMIC_MIIM_SCAN_STATUSr_MIIM_SCAN_BUSYf_GET(st)) {
            if (shr_timeout_check(&to)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Failed to disable hw linkscan.\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_TIMEOUT);
            }
        } else {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Start hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_lm_hw_scan_start(int unit)
{
    CMIC_CMC_MIIM_ADDRESSr_t addr;
    CMIC_MIIM_SCAN_CTRLr_t ctrl;
    int cmc = BCMBD_CMICD_CMC_GET(unit);

    CMIC_CMC_MIIM_ADDRESSr_CLR(addr);
    CMIC_CMC_MIIM_ADDRESSr_CLAUSE_45_REGADRf_SET(addr, 1);
    CMIC_CMC_MIIM_ADDRESSr_CLAUSE_45_DTYPEf_SET(addr, 1);
    WRITE_CMIC_CMC_MIIM_ADDRESSr(unit, cmc, addr);

    READ_CMIC_MIIM_SCAN_CTRLr(unit, &ctrl);
    CMIC_MIIM_SCAN_CTRLr_MIIM_LINK_SCAN_ENf_SET(ctrl, 1);
    WRITE_CMIC_MIIM_SCAN_CTRLr(unit, ctrl);

    return SHR_E_NONE;
}

/*!
 * \brief Check if link fault checking enabled.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Enabled or disabled.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_fault_check_enabled(int unit, int *enabled)
{
    *enabled = TRUE;

    return SHR_E_NONE;
}

/*!
 * \brief Check if port is eligible for linkscan.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Port is not eligible for linkscan.
 */
static int
bcm56960_a0_lm_port_validate(int unit, shr_port_t port)
{
    SHR_FUNC_ENTER(unit);

    /* No link scan on CPU/Loopback ports. */
    if (port == LPORT_CMIC ||
        port == LPORT_LPBK_0 ||
        port == LPORT_LPBK_1 ||
        port == LPORT_LPBK_2 ||
        port == LPORT_LPBK_3 ||
        port == LPORT_RSVD) {
        SHR_RETURN_VAL_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcm56960_a0_lm_drv_attach(bcmlm_drv_t *drv)
{
    /* Attach the hw linkscan driver */
    drv->hw_init = &bcm56960_a0_lm_hw_init;
    drv->hw_config = &bcm56960_a0_lm_hw_config;
    drv->hw_link_get = &bcm56960_a0_lm_hw_link_get;
    drv->hw_intr_clear = &bcm56960_a0_lm_hw_intr_clear;
    drv->hw_intr_cb_set = &bcm56960_a0_lm_hw_intr_func_set;
    drv->hw_scan_stop = &bcm56960_a0_lm_hw_scan_stop;
    drv->hw_scan_start = &bcm56960_a0_lm_hw_scan_start;
    drv->fault_check_enabled = &bcm56960_a0_fault_check_enabled;
    drv->port_validate = &bcm56960_a0_lm_port_validate;

    return SHR_E_NONE;
}
