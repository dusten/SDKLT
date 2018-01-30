/*! \file bcmtm_tdm_grp.c
 *
 * TDM oversub group generation functions.
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
 * \brief Check speed class validity for pipe oversub speed groups.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Check passed.
 * \retval TDM_FAIL Check failed.
 */
static int
bcmtm_tdm_pipe_os_speed_chk(tdm_mod_t *tdm)
{
    int i, spd_idx, spd_type_cnt = 0, grp_num_req = 0;
    char os_en;
    int *spd_prt_cnt;
    int pipe_id;
    int grp_num, grp_len;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    grp_num = bcmtm_tdm_os_grp_num_get(tdm, pipe_id, 0);
    grp_len = bcmtm_tdm_os_grp_len_get(tdm, pipe_id, 0);
    os_en = bcmtm_tdm_pipe_os_en_get(tdm);
    spd_prt_cnt = bcmtm_tdm_pipe_os_spd_prt_cnt_get(tdm);

    if (os_en && grp_len > 0) {
        for (i = 0; i < TDM_MAX_SPEED_TYPES; i++) {
            if (spd_prt_cnt[i]) {
                spd_type_cnt++;
                grp_num_req += (spd_prt_cnt[i]-1)/grp_len + 1;
            }
        }
    }

    for (spd_idx = 0; spd_idx < TDM_MAX_SPEED_TYPES; spd_idx++) {
        if (spd_prt_cnt[spd_idx] > 0) {
            switch (spd_idx) {
                case TDM_SPEED_IDX_1G:
                    TDM_PRINT1("%6s", "1G");
                    break;
                case TDM_SPEED_IDX_1P25G:
                    TDM_PRINT1("%6s", "1.25G");
                    break;
                case TDM_SPEED_IDX_2P5G:
                    TDM_PRINT1("%6s", "2.5G");
                    break;
                case TDM_SPEED_IDX_10G:
                    TDM_PRINT1("%6s", "10G");
                    break;
                case TDM_SPEED_IDX_12P5G:
                    TDM_PRINT1("%6s", "12.5G");
                    break;
                case TDM_SPEED_IDX_20G:
                    TDM_PRINT1("%6s", "20G");
                    break;
                case TDM_SPEED_IDX_25G:
                    TDM_PRINT1("%6s", "25G");
                    break;
                case TDM_SPEED_IDX_40G:
                    TDM_PRINT1("%6s", "40G");
                    break;
                case TDM_SPEED_IDX_50G:
                    TDM_PRINT1("%6s", "50G");
                    break;
                case TDM_SPEED_IDX_100G:
                    TDM_PRINT1("%6s", "100G");
                    break;
                case TDM_SPEED_IDX_120G:
                    TDM_PRINT1("%6s", "120G");
                    break;
                case TDM_SPEED_IDX_200G:
                    TDM_PRINT1("%6s", "200G");
                    break;
                case TDM_SPEED_IDX_400G:
                    TDM_PRINT1("%6s", "400G");
                    break;
                default:
                    break;
            }
            TDM_PRINT1(" port count -- %0d\n", spd_prt_cnt[spd_idx]);
        }
    }
    TDM_PRINT1("\nNumber of OVSB speed types - %3d\n", spd_type_cnt);
    TDM_SML_BAR

    if (spd_type_cnt > grp_num) {
        TDM_ERROR2("OVSB speed types exceeded, spd_types %0d, limit %0d\n",
                    spd_type_cnt, grp_num);
        return TDM_FAIL;
    }
    if (grp_num_req > grp_num) {
        TDM_ERROR2("OVSB speed groups overflow, grp_req %0d, limit %0d\n",
                    grp_num_req, grp_num);
        return TDM_FAIL;
    }

    return TDM_PASS;
}

/*!
 * \brief Get number of ports in given oversub group.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] grp Oversub group index.
 *
 * \retval Number of ports in given oversub group.
 */
static int
bcmtm_tdm_os_grp_port_count(tdm_mod_t *tdm, int grp)
{
    int prt, pos, prt_cnt = 0;
    int token_empty;
    int pipe_id;
    int grp_num, grp_len, *cal_grp;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    grp_num = bcmtm_tdm_os_grp_num_get(tdm, pipe_id, 0);
    grp_len = bcmtm_tdm_os_grp_len_get(tdm, pipe_id, 0);
    if (grp_num == 0 || grp_len == 0) {
        return TDM_FAIL;
    }

    token_empty = bcmtm_tdm_token_empty_get(tdm);
    if (grp >= 0 && grp < grp_num) {
        cal_grp = bcmtm_tdm_os_grp_get(tdm, pipe_id, grp, 0);
        for (pos = 0; pos < grp_len; pos++) {
            prt = cal_grp[pos];
            if (prt != token_empty &&
                prt != 0) {
                prt_cnt++;
            }
        }
    }

    return prt_cnt;
}

/*!
 * \brief Check if given oversub group is empty.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] grp Oversub group index.
 *
 * \retval TDM_TRUE Oversub group is empty.
 * \retval TDM_FALSE Oversub group in not empty.
 */
static int
bcmtm_tdm_os_grp_empty_chk(tdm_mod_t *tdm, int grp)
{
    int pipe_id, grp_num, grp_len;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    grp_num = bcmtm_tdm_os_grp_num_get(tdm, pipe_id, 0);
    grp_len = bcmtm_tdm_os_grp_len_get(tdm, pipe_id, 0);
    if (grp_num == 0 || grp_len == 0) {
        return TDM_FALSE;
    }

    return (bcmtm_tdm_os_grp_port_count(tdm, grp) == 0 ?
            TDM_TRUE : TDM_FALSE);
}

/*!
 * \brief Fill pipe oversub ports into pipe oversub speed groups.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Oversub group generated successfully.
 * \retval TDM_FAIL Oversub group generated failed.
 */
static int
bcmtm_tdm_pipe_os_grp_fill(tdm_mod_t *tdm)
{
    int i, n, prt, spd, spd_idx, grp_idx, prt_idx, grp_req, prt_cnt, cnt,
        result = TDM_PASS;
    char os_en;
    int os_cnt, *os_buff, *spd_prt_cnt;
    int same_spd_ports[TDM_MAX_PORTS_PER_PIPE];
    int pipe_id, grp_num, grp_len, *cal_grp;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    grp_num = bcmtm_tdm_os_grp_num_get(tdm, pipe_id, 0);
    grp_len = bcmtm_tdm_os_grp_len_get(tdm, pipe_id, 0);
    os_en = bcmtm_tdm_pipe_os_en_get(tdm);
    os_cnt = bcmtm_tdm_pipe_os_cnt_get(tdm);
    os_buff = bcmtm_tdm_pipe_os_buff_get(tdm);
    spd_prt_cnt = bcmtm_tdm_pipe_os_spd_prt_cnt_get(tdm);

    if (os_en) {
        for (spd_idx = 0; spd_idx < TDM_MAX_SPEED_TYPES; spd_idx++) {
            prt_cnt = spd_prt_cnt[spd_idx];
            if (prt_cnt == 0) continue;

            /* Calc number of groups required by ports with the same speed. */
            grp_req = (grp_len > 0) ? ((prt_cnt - 1) / grp_len + 1) : 0;

            /* Allocate ports with the same speed into ovsb groups. */
            TDM_MEMSET(same_spd_ports, 0, sizeof(int) *
                                    TDM_MAX_PORTS_PER_PIPE);
            cnt = 0;
            for (i = 0; i < os_cnt && i < TDM_MAX_PORTS_PER_PIPE; i++) {
                prt = os_buff[i];
                if (bcmtm_tdm_port_speed_idx_get(tdm, prt, 0) == spd_idx) {
                    same_spd_ports[cnt++] = prt;
                    if (cnt == prt_cnt) break;
                }
            }
            cnt = 0;
            spd = bcmtm_tdm_cfg_port_speed_get(tdm, same_spd_ports[0], 0);
            for (n = 0; n < grp_req; n++) {
                /* Find the first empty group. */
                grp_idx = 0;
                while (grp_idx < grp_num) {
                    if (bcmtm_tdm_os_grp_empty_chk(tdm, grp_idx) == TDM_TRUE) {
                        break;
                    } else {
                        grp_idx++;
                    }
                }
                /* Allocate ports into group. */
                if (grp_idx < grp_num) {
                    cal_grp = bcmtm_tdm_os_grp_get(tdm, pipe_id, grp_idx, 0);
                    for (prt_idx = 0; prt_idx < grp_len &&
                         cnt < TDM_MAX_PORTS_PER_PIPE; prt_idx++) {
                        prt = same_spd_ports[cnt++];
                        if (prt == 0) break;
                        cal_grp[prt_idx] = prt;
                        TDM_PRINT4("Assign %3dG port %3d to grp[%0d][%0d]\n",
                                   (spd / 1000), prt, grp_idx, prt_idx);
                    }
                } else {
                    result = TDM_FAIL;
                    prt = same_spd_ports[cnt];
                    TDM_ERROR3("%s, port %3d, speed %0dG\n",
                               "OVSB speed groups overflow",
                               prt, (bcmtm_tdm_cfg_port_speed_get(tdm, prt, 0)
                               / 1000));
                }
            }
        }
    }
    TDM_SML_BAR

    return result;
}

/*!
 * \brief Generate pipe oversub ports into pipe oversub speed groups.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Oversub group generated successfully.
 * \retval TDM_FAIL Oversub group generated failed.
 */
int
bcmtm_tdm_pipe_os_grp_gen(tdm_mod_t *tdm)
{
    TDM_BIG_BAR
    TDM_PRINT1("TDM: Generating OVSB speed groups for pipe %0d\n\n",
               bcmtm_tdm_pipe_id_get(tdm));
    /* Check oversub speed types. */
    if (bcmtm_tdm_pipe_os_speed_chk(tdm) != TDM_PASS) {
        return TDM_FAIL;
    }

    /* Generate speed groups. */
    if (bcmtm_tdm_pipe_os_grp_fill(tdm) != TDM_PASS) {
        return TDM_FAIL;
    }

    return TDM_PASS;
}
