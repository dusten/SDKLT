/*! \file bcmtm_tdm_cfg_pipe.c
 *
 * TDM core functions for pipe configuration access.
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

#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_top.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_top.h>
#endif


/***********************************************************************
 * Internal functions
 */
/*!
 * \brief Get pipe config data on slot granularity.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Slot unit in Mbps.
 *
 * Slot_unit represents the bandwidth per slot of linerate main calendar.
 * Note that the slot unit has the same precision of port speed.
 */
int
bcmtm_tdm_pipe_slot_unit_get(tdm_mod_t *tdm)
{
    int slot_unit;

    slot_unit = tdm->core_data.cfg.slot_unit;
    /* NOTE: add new slot_unit below for backward consistency */
    switch (slot_unit) {
        case TDM_SLOT_UNIT_1P25G:
        case TDM_SLOT_UNIT_2P5G:
        case TDM_SLOT_UNIT_5G:
        case TDM_SLOT_UNIT_10G:
        case TDM_SLOT_UNIT_25G:
        case TDM_SLOT_UNIT_50G:
            break;
        default:
            slot_unit = TDM_SLOT_UNIT_2P5G;
            break;
    }

    return slot_unit;
}

/*!
 * \brief Get pipe config data on pipe index.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe ID.
 */
int
bcmtm_tdm_pipe_id_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.pipe_id;
}

/*!
 * \brief Get pipe config data on the lowest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest port number.
 */
int
bcmtm_tdm_pipe_port_lo_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.pipe_port_lo;
}

/*!
 * \brief Get pipe config data on the highest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest port number.
 */
int
bcmtm_tdm_pipe_port_hi_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.pipe_port_hi;
}

/*!
 * \brief Get pipe config data on limitiation of linerate slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max linerate slots.
 */
int
bcmtm_tdm_pipe_lr_limit_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.num_lr_limit;
}

/*!
 * \brief Get pipe config data on limitiation of ancilary slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max ancilary slots.
 */
int
bcmtm_tdm_pipe_ancl_limit_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.num_ancl_limit;
}

/*!
 * \brief Get pipe variable on linerate enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe linerate enable indicator.
 */
char
bcmtm_tdm_pipe_lr_en_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_en;
}

/*!
 * \brief Get pipe variable on oversub enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe oversub enable indicator.
 */
char
bcmtm_tdm_pipe_os_en_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_en;
}

/*!
 * \brief Get pipe variable on linerate port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of linerate ports in pipe.
 */
int
bcmtm_tdm_pipe_lr_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_cnt;
}

/*!
 * \brief Get pipe variable on oversub port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of oversub ports in pipe.
 */
int
bcmtm_tdm_pipe_os_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_cnt;
}

/*!
 * \brief Get number of speed types for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe linerate ports.
 */
int
bcmtm_tdm_pipe_lr_spd_types_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_spd_types;
}

/*!
 * \brief Get number of speed types for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe oversub ports.
 */
int
bcmtm_tdm_pipe_os_spd_types_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_spd_types;
}

/*!
 * \brief Get pointer of port buffer for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe linerate port buffer.
 */
int *
bcmtm_tdm_pipe_lr_buff_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_buff;
}

/*!
 * \brief Get pointer of port count buffer for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe oversub port buffer.
 */
int *
bcmtm_tdm_pipe_os_buff_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_buff;
}

/*!
 * \brief Get pointer of port count per speed for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based oversub port count buffer.
 */
int *
bcmtm_tdm_pipe_lr_spd_prt_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_spd_prt_cnt;
}

/*!
 * \brief Get pointer of port count per speed for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based linerate port count buffer.
 */
int *
bcmtm_tdm_pipe_os_spd_prt_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_spd_prt_cnt;
}
