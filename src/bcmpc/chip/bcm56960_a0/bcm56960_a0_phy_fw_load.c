/*! \file bcm56960_a0_phy_fw_load.c
 *
 * BCM56960 PHY firmware loader.
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

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56960_a0_acc.h>
#include <phymod/phymod.h>

#include <bcmpc/bcmpc_drv.h>
#include <bcmpc/bcmpc_util_internal.h>

#include "bcm56960_a0_pc_internal.h"

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

#define FW_ALIGN_BYTES                  16
#define FW_ALIGN_MASK                   (FW_ALIGN_BYTES - 1)

typedef struct fw_helper_info_s {
    int unit;
    int port;
} fw_helper_info_t;

static int
tsce_firmware_helper(const phymod_core_access_t *ca,
                     uint32_t fw_size, const uint8_t *fw_data)
{
    fw_helper_info_t *fi = (fw_helper_info_t *)PHYMOD_ACC_USER_ACC(&ca->access);
    int unit = fi->unit;
    int port = fi->port;
    int ioerr = 0;
    int rv = SHR_E_NONE;
    XLPORT_WC_UCMEM_CTRLr_t xl_ucmem_ctrl;
    XLPORT_WC_UCMEM_DATAm_t xl_ucmem_data;
    uint8_t buf[FW_ALIGN_BYTES];
    const uint8_t *data;
    uint32_t val;
    uint32_t size, idx, wdx, bdx;

    if (fw_size == 0) {
        return SHR_E_INTERNAL;
    }

    /* Aligned firmware size */
    size = (fw_size + FW_ALIGN_MASK) & ~FW_ALIGN_MASK;

    /* Enable parallel bus access */
    ioerr += READ_XLPORT_WC_UCMEM_CTRLr(unit, &xl_ucmem_ctrl, port);
    XLPORT_WC_UCMEM_CTRLr_ACCESS_MODEf_SET(xl_ucmem_ctrl, 1);
    ioerr += WRITE_XLPORT_WC_UCMEM_CTRLr(unit, xl_ucmem_ctrl, port);

    for (idx = 0; idx < size; idx += 16) {
        if (idx + 15 < fw_size) {
            data = fw_data + idx;
        } else {
            /* Use staging buffer for modulo bytes */
            sal_memset(buf, 0, sizeof(buf));
            sal_memcpy(buf, fw_data + idx, FW_ALIGN_BYTES - (size - fw_size));
            data = buf;
        }
        for (wdx = 0; wdx < 4; wdx++) {
            val = 0;
            for (bdx = 0; bdx < 4; bdx++) {
                val |= data[bdx + wdx * 4] << (bdx * 8);
            }
            XLPORT_WC_UCMEM_DATAm_SET(xl_ucmem_data, wdx, val);
        }
        WRITE_XLPORT_WC_UCMEM_DATAm(unit, idx >> 4, xl_ucmem_data, port);
    }

    /* Disable parallel bus access */
    XLPORT_WC_UCMEM_CTRLr_ACCESS_MODEf_SET(xl_ucmem_ctrl, 0);
    ioerr += WRITE_XLPORT_WC_UCMEM_CTRLr(unit, xl_ucmem_ctrl, port);

    return ioerr ? SHR_E_ACCESS : rv;
}

static int
tscf_firmware_helper(const phymod_core_access_t *ca,
                     uint32_t fw_size, const uint8_t *fw_data)
{
    fw_helper_info_t *fi = (fw_helper_info_t *)PHYMOD_ACC_USER_ACC(&ca->access);
    int unit = fi->unit;
    int port = fi->port;
    int ioerr = 0;
    int rv = SHR_E_NONE;
    CLPORT_WC_UCMEM_CTRLr_t cl_ucmem_ctrl;
    CLPORT_WC_UCMEM_DATAm_t cl_ucmem_data;
    uint8_t buf[FW_ALIGN_BYTES];
    const uint8_t *data;
    uint32_t val;
    uint32_t size, idx, wdx, bdx;

    if (fw_size == 0) {
        return SHR_E_INTERNAL;
    }

    /* Aligned firmware size */
    size = (fw_size + FW_ALIGN_MASK) & ~FW_ALIGN_MASK;

    /* Enable parallel bus access */
    ioerr += READ_CLPORT_WC_UCMEM_CTRLr(unit, &cl_ucmem_ctrl, port);
    CLPORT_WC_UCMEM_CTRLr_ACCESS_MODEf_SET(cl_ucmem_ctrl, 1);
    ioerr += WRITE_CLPORT_WC_UCMEM_CTRLr(unit, cl_ucmem_ctrl, port);

    for (idx = 0; idx < size; idx += 16) {
        if (idx + 15 < fw_size) {
            data = fw_data + idx;
        } else {
            /* Use staging buffer for modulo bytes */
            sal_memset(buf, 0, sizeof(buf));
            sal_memcpy(buf, fw_data + idx, FW_ALIGN_BYTES - (size - fw_size));
            data = buf;
        }
        for (wdx = 0; wdx < 4; wdx++) {
            val = 0;
            for (bdx = 0; bdx < 4; bdx++) {
                val |= data[bdx + wdx * 4] << (bdx * 8);
            }
            CLPORT_WC_UCMEM_DATAm_SET(cl_ucmem_data, wdx, val);
        }
        WRITE_CLPORT_WC_UCMEM_DATAm(unit, idx >> 4, cl_ucmem_data, port);
    }

    /* Disable parallel bus access */
    CLPORT_WC_UCMEM_CTRLr_ACCESS_MODEf_SET(cl_ucmem_ctrl, 0);
    ioerr += WRITE_CLPORT_WC_UCMEM_CTRLr(unit, cl_ucmem_ctrl, port);

    return ioerr ? SHR_E_ACCESS : rv;
}

static int
pm_tsce_firmware_load(int unit, bcmpc_topo_t *pm_info, bcmpc_drv_t *pc)
{
    int port = pm_info->base_pport;
    phymod_core_access_t core_access, *ca = &core_access;
    phymod_core_init_config_t core_init_config, *cic = &core_init_config;
    phymod_core_status_t core_status, *cs = &core_status;
    fw_helper_info_t fw_helper_info, *fi = &fw_helper_info;
    bcmpc_topo_type_t topo_type, *tt = &topo_type;
    uint32_t found = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(fi, 0, sizeof (*fi));
    fi->unit = unit;
    fi->port = port;

    sal_memset(ca, 0, sizeof(*ca));
    ca->type = phymodDispatchTypeTsce;
    PHYMOD_ACC_USER_ACC(&ca->access) = fi;
    PHYMOD_ACC_BUS(&ca->access) = pc->phy_bus_get(unit, port);
    PHYMOD_ACC_ADDR(&ca->access) = pc->phy_addr_get(unit, port);

    /* PHY probe */
    SHR_IF_ERR_EXIT
        (phymod_core_identify(ca, 0, &found));
    if (!found) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to identify PHY core for port %d\n"),
                   port));
    }

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, pm_info->tid, tt));

    sal_memset(cic, 0, sizeof(*cic));
    cic->firmware_loader = tsce_firmware_helper;
    cic->firmware_load_method = phymodFirmwareLoadMethodExternal;
    cic->lane_map.num_of_lanes = tt->num_lanes;
    PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(cic);

    cs->pmd_active = 0;
    SHR_IF_ERR_EXIT
        (phymod_core_init(ca, cic, cs));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_tscf_firmware_load(int unit, bcmpc_topo_t *pm_info, bcmpc_drv_t *pc)
{
    int port = pm_info->base_pport;
    phymod_core_access_t core_access, *ca = &core_access;
    phymod_core_init_config_t core_init_config, *cic = &core_init_config;
    phymod_core_status_t core_status, *cs = &core_status;
    fw_helper_info_t fw_helper_info, *fi = &fw_helper_info;
    bcmpc_topo_type_t topo_type, *tt = &topo_type;
    uint32_t found = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(fi, 0, sizeof (*fi));
    fi->unit = unit;
    fi->port = port;

    sal_memset(ca, 0, sizeof(*ca));
    ca->type = phymodDispatchTypeTscf;
    PHYMOD_ACC_USER_ACC(&ca->access) = fi;
    PHYMOD_ACC_BUS(&ca->access) = pc->phy_bus_get(unit, port);
    PHYMOD_ACC_ADDR(&ca->access) = pc->phy_addr_get(unit, port);

    /* PHY probe */
    SHR_IF_ERR_EXIT
        (phymod_core_identify(ca, 0, &found));
    if (!found) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to identify PHY core for port %d\n"),
                   port));
    }

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, pm_info->tid, tt));

    sal_memset(cic, 0, sizeof(*cic));
    cic->firmware_loader = tscf_firmware_helper;
    cic->firmware_load_method = phymodFirmwareLoadMethodExternal;
    cic->lane_map.num_of_lanes = tt->num_lanes;
    PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(cic);

    cs->pmd_active = 0;
    SHR_IF_ERR_EXIT
        (phymod_core_init(ca, cic, cs));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_firmware_load(int unit, int pm_id, bcmpc_topo_t *pm_info, void *cookie)
{
    bcmpc_drv_t *pc = (bcmpc_drv_t *)cookie;
    int port = pm_info->base_pport;
    phymod_dispatch_type_t phy_type;

    phy_type = pc->phy_drv_get(unit, port);
    if (phy_type == phymodDispatchTypeTscf) {
        return pm_tscf_firmware_load(unit, pm_info, pc);
    } else if (phy_type == phymodDispatchTypeTsce) {
        return pm_tsce_firmware_load(unit, pm_info, pc);
    }

    return 0;
}

int
bcm56960_a0_phy_firmware_load(int unit)
{
    bool real_hw = bcmdrd_feature_is_real_hw(unit);
    bcmpc_drv_t *pc;

    SHR_FUNC_ENTER(unit);

    if (!real_hw) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc));

    
    SHR_IF_ERR_EXIT
        (bcmpc_topo_traverse(unit, pm_firmware_load, pc));

exit:
    SHR_FUNC_EXIT();
}
