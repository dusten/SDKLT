/*! \file bcmdrd_dev.c
 *
 * <description>
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

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>

/* Declare all chip_info structures */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmdrd_chip_info_t _bc##_chip_info;
#define BCMDRD_DEVLIST_INCLUDE_ALL
#include <bcmdrd/bcmdrd_devlist.h>

/* Create device table */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _vn, _dv, _rv, _md, #_nm, #_fn, #_bd, BCMDRD_DEV_T_##_bd, &_bc##_chip_info },
#define BCMDRD_DEVLIST_INCLUDE_ALL
static struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t revision;
    uint16_t model;
    const char *name;
    const char *full_name;
    const char *type_str;
    bcmdrd_dev_type_t type;
    const bcmdrd_chip_info_t *chip_info;
} dev_table[] = {
#include <bcmdrd/bcmdrd_devlist.h>
    { 0, 0 } /* end-of-list */
};

static bcmdrd_dev_t bcmdrd_dev[BCMDRD_CONFIG_MAX_UNITS];

static int
dev_lookup(bcmdrd_dev_id_t *id)
{
    int match_idx = -1;
    int idx = 0;
    int rev = -1;
    while (dev_table[idx].vendor_id) {
        if (id->vendor_id == dev_table[idx].vendor_id &&
            id->device_id == dev_table[idx].device_id &&
            id->model == dev_table[idx].model) {
            /*
             * Vendor/device ID may match multiple entries,
             * so we look for highest matching revision.
             */
            if (id->revision >= dev_table[idx].revision &&
                rev < dev_table[idx].revision) {
                /* Found a (better) match */
                match_idx = idx;
                rev = dev_table[idx].revision;
            }
        }
        idx++;
    }
    return match_idx;
}

static bool
adj_byte_swap(int unit, bool byte_swap)
{
    bool byte_swap_pio;

    byte_swap_pio = bcmdrd_dev[unit].io.byte_swap_pio;
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_IPROC)) {
        if (bcmdrd_dev_bus_type_get(unit) == BCMDRD_BT_PCI) {
            if (byte_swap_pio) {
                /*
                 * If BE_PIO is set, then we assume that the iProc
                 * PCI bridge will byte-swap all access types,
                 * i.e. both PIO and DMA access.
                 *
                 * In order to avoid double-swapping, we then need to
                 * invert all byte-swap settings provided by the system.
                 */
                byte_swap = !byte_swap;
            }
        }
    }

    return byte_swap;
}

static int
dev_valid_pipes_set(bcmdrd_dev_t *dev)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    int port, pipe;

    dev->valid_pipes = 0;
    sal_memset(pi, 0, sizeof(*pi));
    pi->pnd = BCMDRD_PND_PHYS;
    BCMDRD_PBMP_ITER(dev->chip_info->valid_pbmps[BCMDRD_PND_PHYS], port) {
        pi->port = port;
        pipe = bcmdrd_chip_pipe_info(dev->chip_info, pi,
                                     BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT);
        dev->valid_pipes |= (1 << pipe);
    }
    return 0;
}

static uint32_t
dev_pipe_num(bcmdrd_dev_t *dev)
{
    return bcmdrd_chip_pipe_info(dev->chip_info, NULL,
                                 BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);
}

bool
bcmdrd_dev_supported(bcmdrd_dev_id_t *id)
{
    return (dev_lookup(id) >= 0);
}

int
bcmdrd_dev_destroy(int unit)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        /* Device has not been created */
        return SHR_E_INIT;
    }

    dev = &bcmdrd_dev[unit];
    sal_memset(dev, 0, sizeof(*dev));

    return SHR_E_NONE;
}

int
bcmdrd_dev_create(int unit, bcmdrd_dev_id_t *id)
{
    bcmdrd_dev_t *dev;
    int match_idx;
    int rv = SHR_E_NOT_FOUND;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type != BCMDRD_DEV_T_NONE) {
        return SHR_E_EXISTS;
    }

    match_idx = dev_lookup(id);

    if (match_idx >= 0) {
        /* Warn about possible incompatibility */
        if (id->revision > dev_table[match_idx].revision) {
            LOG_WARN(BSL_LS_BCMDRD_DEV,
                     (BSL_META_U(unit,
                                 "Device revision (0x%02x) is newer than "
                                 "supported driver revision (0x%02x)\n"),
                         id->revision, dev_table[match_idx].revision));
        }
        dev = &bcmdrd_dev[unit];

        /* Fill out device structure */
        sal_memcpy(&dev->id, id, sizeof(*id));
        dev->name = dev_table[match_idx].name;
        dev->type_str = dev_table[match_idx].type_str;
        dev->type = dev_table[match_idx].type;

        bcmdrd_dev_chip_info_set(unit, dev_table[match_idx].chip_info);

        /* Initialize default valid port bitmap */
        bcmdrd_dev_valid_ports_set(unit,
                                   &dev->chip_info->valid_pbmps[BCMDRD_PND_PHYS]);

        dev_valid_pipes_set(dev);

        /* Reset feature list */
        bcmdrd_feature_disable(unit, BCMDRD_FT_ALL);

        LOG_INFO(BSL_LS_BCMDRD_DEV,
                 (BSL_META_U(unit,
                             "Created %s device "
                             "(%04x:%04x:%02x)\n"),
                  dev->name, id->vendor_id, id->device_id, id->revision));

        rv = SHR_E_NONE;
    }

    /* Return an error on failure or the new unit number when successful */
    return (rv < 0) ? rv : unit;
}

bcmdrd_dev_type_t
bcmdrd_dev_type(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return BCMDRD_DEV_T_NONE;
    }

    return bcmdrd_dev[unit].type;
}

const char *
bcmdrd_dev_type_str(int unit)
{
    const char *type_str = NULL;

    if (BCMDRD_UNIT_VALID(unit)) {
        type_str = bcmdrd_dev[unit].type_str;
    }

    return type_str ? type_str : "";
}

int
bcmdrd_dev_id_get(int unit, bcmdrd_dev_id_t *id)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(id, &bcmdrd_dev[unit].id, sizeof(*id));

    return SHR_E_NONE;
}

int
bcmdrd_dev_id_from_name(const char *dev_name, bcmdrd_dev_id_t *id)
{
    int idx = 0;

    assert(dev_name);
    assert(id);

    while (dev_table[idx].vendor_id) {
        if (sal_strcasecmp(dev_name, dev_table[idx].full_name) == 0) {
            id->vendor_id = dev_table[idx].vendor_id;
            id->device_id = dev_table[idx].device_id;
            id->revision = dev_table[idx].revision;
            id->model = dev_table[idx].model;
            return SHR_E_NONE;
        }
        idx++;
    }
    return SHR_E_NOT_FOUND;
}

int
bcmdrd_dev_hal_io_init(int unit, bcmdrd_hal_io_t *io)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(&bcmdrd_dev[unit].io, io, sizeof(*io));

    return SHR_E_NONE;
}

int
bcmdrd_dev_hal_dma_init(int unit, bcmdrd_hal_dma_t *dma)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(&bcmdrd_dev[unit].dma, dma, sizeof(*dma));

    return SHR_E_NONE;
}

int
bcmdrd_dev_hal_intr_init(int unit, bcmdrd_hal_intr_t *intr)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(&bcmdrd_dev[unit].intr, intr, sizeof(*intr));

    return SHR_E_NONE;
}

bcmdrd_dev_t *
bcmdrd_dev_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return NULL;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return NULL;
    }

    return &bcmdrd_dev[unit];
}

bool
bcmdrd_dev_byte_swap_pio_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return adj_byte_swap(unit, bcmdrd_dev[unit].io.byte_swap_pio);
}

bool
bcmdrd_dev_byte_swap_packet_dma_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return adj_byte_swap(unit, bcmdrd_dev[unit].io.byte_swap_packet_dma);
}

bool
bcmdrd_dev_byte_swap_non_packet_dma_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return adj_byte_swap(unit, bcmdrd_dev[unit].io.byte_swap_non_packet_dma);
}

bcmdrd_hal_bus_type_t
bcmdrd_dev_bus_type_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return BCMDRD_BT_UNKNOWN;
    }

    return bcmdrd_dev[unit].io.bus_type;
}

uint32_t
bcmdrd_dev_io_flags_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return 0;
    }

    return bcmdrd_dev[unit].io.flags;
}

bool
bcmdrd_dev_exists(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return (bcmdrd_dev[unit].chip_info != NULL);
}

int
bcmdrd_dev_chip_info_set(int unit, const bcmdrd_chip_info_t *chip_info)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];

    dev->chip_info = chip_info;

    return 0;
}

const bcmdrd_chip_info_t *
bcmdrd_dev_chip_info_get(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info;
}

int
bcmdrd_dev_valid_ports_set(int unit, const bcmdrd_pbmp_t *pbmp)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];

    assert(pbmp);
    sal_memcpy(&dev->valid_ports, pbmp, sizeof(dev->valid_ports));
    BCMDRD_PBMP_AND(dev->valid_ports,
                    dev->chip_info->valid_pbmps[BCMDRD_PND_PHYS]);

    return 0;
}

int
bcmdrd_dev_valid_ports_get(int unit, bcmdrd_pbmp_t *pbmp)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];

    assert(pbmp);
    sal_memcpy(pbmp, &dev->valid_ports, sizeof(*pbmp));

    return 0;
}

int
bcmdrd_dev_valid_pipes_get(int unit, uint32_t *pipe_map)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];

    assert(pipe_map);
    *pipe_map = dev->valid_pipes;

    return 0;
}

const bcmdrd_symbols_t *
bcmdrd_dev_symbols_get(int unit, int idx)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    /* Currently we only support a single symbol table */
    if (idx != 0) {
        return NULL;
    }

    return dev->chip_info->symbols;
}

void
bcmdrd_dev_bypass_mode_set(int unit, uint32_t bypass_mode)
{
    if (BCMDRD_UNIT_VALID(unit)) {
        LOG_VERBOSE(BSL_LS_BCMDRD_BYPASS,
                    (BSL_META_U(unit,
                                "Bypass mode: 0x%x"PRIu32"\n"),
                     bypass_mode));
        bcmdrd_dev[unit].bypass_mode = bypass_mode;
    }
}

uint32_t
bcmdrd_dev_bypass_mode_get(int unit)
{
    if (BCMDRD_UNIT_VALID(unit)) {
        return bcmdrd_dev[unit].bypass_mode;
    }
    return 0;
}

uint32_t
bcmdrd_dev_pipe_info(int unit, bcmdrd_pipe_info_t *pi,
                     bcmdrd_pipe_info_type_t pi_type)
{
    bcmdrd_dev_t *dev;
    uint32_t pipe_info, pipe_num;

    if (!bcmdrd_dev_exists(unit)) {
        return 0;
    }

    dev = &bcmdrd_dev[unit];

    pipe_info = bcmdrd_chip_pipe_info(dev->chip_info, pi, pi_type);
    if (pipe_info == 0) {
        return 0;
    }

    /*
     * Check the device valid pipes if the queried information is related to
     * pipe mask.
     */
    switch (pi_type) {
    case BCMDRD_PIPE_INFO_TYPE_PMASK:
        pipe_num = bcmdrd_chip_pipe_info(dev->chip_info, pi,
                                         BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);
        if (pipe_num == dev_pipe_num(dev)) {
            pipe_info &= dev->valid_pipes;
        }
        break;
    case BCMDRD_PIPE_INFO_TYPE_BASETYPE_PMASK:
        pipe_num = bcmdrd_chip_pipe_info(dev->chip_info, pi,
                                         BCMDRD_PIPE_INFO_TYPE_LINST);
        if (pipe_num == dev_pipe_num(dev)) {
            pipe_info &= dev->valid_pipes;
        }
        break;
    default:
        break;
    }

    return pipe_info;
}

bcmdrd_block_port_addr_f
bcmdrd_dev_block_port_addr_func(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info->block_port_addr;
}

bcmdrd_addr_decode_f
bcmdrd_dev_addr_decode_func(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info->addr_decode;
}
