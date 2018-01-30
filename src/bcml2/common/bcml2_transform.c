/*! \file bcml2_transform.c
 *
 * L2 Logic table fields Transform Handlers.
 * This file contains field transform information for
 * L2 Logic table fields.
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

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcml2/bcml2_l2_fdb_vlan_dest_resolve.h>
#include <bcml2/chip/bcml2_transform_chip.h>
#include <bcml2/common/bcml2_transform.h>
#include <bcml2/bcml2_l2_fdb_vlan_mac_addr.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCML2_TRANSFORM

/* L2 Learn driver database. */
static
l2_transform_t *transform[BCML2_DEV_NUM_MAX] = {NULL};

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_l2_transform_attach},
static struct {
    l2_transform_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};
/******************************************************************************
* Private functions
 */

/******************************************************************************
* Public functions
 */
/*!
 * \brief Attach L2 transform drivers.
 *
 * Attach L2 transform drivers.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to attach L2 transform drivers.
 */
int
bcml2_transform_attach(int unit)
{
    size_t size;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
        if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(
        (transform[unit] != NULL) ? SHR_E_INTERNAL : SHR_E_NONE);

    size = sizeof(l2_transform_t);
    transform[unit] = sal_alloc(size, "l2_fdb_vlan_gport_resolve_transform");
    SHR_NULL_CHECK(transform[unit], SHR_E_MEMORY);
    sal_memset(transform[unit], 0, size);

    SHR_IF_ERR_EXIT(
    list_dev_attach[dev_type].attach(unit, transform[unit]));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach L2 transform drivers.
 *
 * Detach L2 transform drivers.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to detach L2 transform drivers.
 */
int
bcml2_transform_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (transform[unit] != NULL) {
        sal_free(transform[unit]);
        transform[unit] = NULL;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN Logic table destination fields forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              L2_FDB_VLAN Logic table destination fields.
 * \param [out] out             Physical table L2X gport fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcml2_l2_fdb_vlan_dest_resolve_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (transform[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (transform[unit]->l2_fdb_vlan_dest_fwd == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(
            transform[unit]->l2_fdb_vlan_dest_fwd(unit, in, out, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN Logic table destination fields reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              L2_FDB_VLAN Logic table destination fields.
 * \param [out] out             Physical table L2X gport fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcml2_l2_fdb_vlan_dest_resolve_rev_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (transform[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (transform[unit]->l2_fdb_vlan_dest_rev == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(
        transform[unit]->l2_fdb_vlan_dest_rev(unit, in, out, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN.MAC_ADDR foward transform
 *        The transform records the key MAC_ADDR and pass it to LTM so that
 *        LTM can pass the key MAC_ADDR to the transform for gport fields.
 * \param [in]  unit            Unit number.
 * \param [in]  in              L2_FDB_VLAN.MAC_ADDR.
 * \param [out] out             Physical table L2X gport fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcml2_l2_fdb_vlan_mac_addr_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (transform[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (transform[unit]->l2_fdb_vlan_mac_addr_fwd == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(
        transform[unit]->l2_fdb_vlan_mac_addr_fwd(unit, in, out, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN.MAC_ADDR reverse transform
 *        The transform records the key MAC_ADDR and pass it to LTM so that
 *        LTM can pass the key MAC_ADDR to the transform for gport fields.
 * \param [in]  unit            Unit number.
 * \param [in]  in              Physical table L2X gport fields.
 * \param [out] out            L2_FDB_VLAN.MAC_ADDR.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcml2_l2_fdb_vlan_mac_addr_rev_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (transform[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (transform[unit]->l2_fdb_vlan_mac_addr_rev == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(
        transform[unit]->l2_fdb_vlan_mac_addr_rev(unit, in, out, arg));
    }

exit:
    SHR_FUNC_EXIT();
}
