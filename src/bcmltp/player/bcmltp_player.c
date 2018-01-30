/*! \file bcmltp_player.c
 *
 * BCMLTP Player
 *
 * The LTP player "plays back" logical and physical table
 * configurations recorded by BCMCFG. This is not intended as an SDK
 * application interface.
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
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_player.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtrm/trm_api.h>
#include <bcmdrd_config.h>
#include <bcmltp/bcmltp_player.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/*!
 * \brief Player function.
 *
 * Callback function to play back logical or physical table data.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Table ID.
 * \param [in,out] field        Field list element.
 * \param [in]     user_data    User data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmltp_play_f)(int unit,
                             uint32_t sid,
                             const char *name,
                             shr_fmm_t *field,
                             void *user_data);

/*!
 * \brief enum array.
 *
 * Playback enum list in array form. Field enum values are initially
 * an index into this list.
 */
typedef struct bcmltp_enum_str_s {
    /* Number of enum strings in array. */
    size_t count;

    /* Array of enum strings. */
    const char **str;
} bcmltp_enum_str_t;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocate a TRM entry for playback
 *
 * Allocate a TRM entry and copy input fields for playback via TRM.
 * Note that the TRM interface assumes that field data is allocated
 * via shr_fmm_alloc(), and will be freed by bcmtrm_entry_free().
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical/Physical Table ID.
 * \param [in]     name         Logical table name.
 * \param [in]     interactive  True if table is interactive.
 * \param [in]     field_in     Playback field list.
 * \param [out]    entry_p      Pointer to TRM entry.
 *
 * \retval 0      OK
 * \retval !0     ERROR
 */
static int
bcmltp_entry_alloc(int unit,
                   uint32_t sid,
                   const char *name,
                   bool interactive,
                   shr_fmm_t *field_in,
                   bcmtrm_entry_t **entry_p)
{
    bcmtrm_entry_t *entry;
    SHR_FUNC_ENTER(unit);

    do {
        shr_fmm_t *field;

        entry = bcmtrm_entry_alloc(unit, sid, interactive, false, NULL, name);
        if (entry == NULL) {
            break;
        }

        while (field_in) {
            /* Allocate field data. */
            field = shr_fmm_alloc();
            if (field == NULL) {
                int rv;

                /* Handle field allocation error - error in
                   bcmtrm_entry_free() */
                rv = bcmtrm_entry_free(entry);
                entry = NULL;
                if (SHR_FAILURE(rv)) {
                    SHR_IF_ERR_CONT(rv);
                } else {
                    SHR_IF_ERR_CONT(SHR_E_MEMORY);
                }
                break;
            }
            /* Set field data. */
            field->id = field_in->id;
            field->flags = field_in->flags;
            field->data = field_in->data;
            field->idx = field_in->idx;
            field->next = entry->l_field;

            /* Link into entry. */
            entry->l_field = field;

            /* Next input field. */
            field_in = field_in->next;
        }

    } while (0);

    *entry_p = entry;

    SHR_FUNC_EXIT();
}



/*!
 * \brief Format TRM entry.
 *
 * Return an allocated string with a formatted TRM entry.
 *
 * \param [in]     entry        TRM entry.
 *
 * \retval !NULL string
 * \retval NULL  ERROR
 */
static char *
bcmltp_entry_str(bcmtrm_entry_t *entry)
{
    size_t len = 32768;
    char *str = sal_alloc(len, "bcmltp_entry_str");
    const bcmlrd_table_rep_t *tbl = bcmlrd_table_get(entry->table_id);
    if (str != NULL && tbl != NULL) {
        shr_fmm_t *field = entry->l_field;
        char *p = str;
        int n;
        *p = 0;
        /* opcode entry->opcode.lt_opcode */
        n = sal_snprintf(p, len-1, "op:%d\n", entry->opcode.lt_opcode);
        p += n;
        len -= n;
        /* name */
        n = sal_snprintf(p, len-1, "tbl:%s\n", entry->info.table_name);
        p += n;
        len -= n;
        /* fields entry->l_field  */
        while (field != NULL) {
            n = sal_snprintf(p, len-1, "   %s[%u] %"PRIu64"\n",
                             tbl->field[field->id].name,
                             field->idx,
                             field->data);
            p += n;
            len -= n;
            field = field->next;
        }
    }
    return str;
}

/*!
 * \brief Resolve enumeration.
 *
 * Resolve an enumeration string. This is not known when the BCMCFG
 * file is parsed, so resolve during playback.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     field_data   Field data.
 * \param [in]     enum_str     Enum string to resolve.
 * \param [in,out] field        Field list element.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_resolve_enum(int unit,
                    const bcmlrd_map_field_data_t *field_data,
                    const bcmltp_enum_str_t *enum_str,
                    shr_fmm_t *field)
{
    size_t i;
    const char *str;
    const bcmlrd_field_data_t *lfield;

    SHR_FUNC_ENTER(unit);
    do {
        if (field->id >= field_data->fields) {
            /* ID out of range. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field ID out of range.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        /* Get LRD field data. */
        lfield = field_data->field + field->id;

        if ((lfield->flags & BCMLRD_FIELD_F_ENUM) == 0) {
            /* Not an enum, so value already resolved. */
            break;
        }

        if (enum_str->str == NULL) {
            /* We have an enum, but there is no resolution data. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("missing enum resolution data.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        if (field->data >= enum_str->count) {
            /* enum string index out of range of enum string table. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("enum string index out of range.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        if (lfield->sym == NULL) {
            /* Claims to be an enum, but no symbol data. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("missing enum symbol data.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        /* Search for string in enum. */
        str = enum_str->str[field->data];

        for (i = 0; i < lfield->num_sym; i++) {
            if (!sal_strcmp(str, lfield->sym[i].name)) {
                /* Resolved! */
                field->data = lfield->sym[i].val;
                break;
            }
        }

        if (i == lfield->num_sym) {
            /* Symbol not found. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Symbol not found.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }

    } while (0);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Resolve enumerations.
 *
 * Resolve enumerations for an LTM entry.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     enum_str     Enum string struct.
 * \param [in,out] entry        TRM entry.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_resolve_enums(int unit,
                     const bcmltp_enum_str_t *enum_str,
                     bcmtrm_entry_t *entry)
{
    const bcmlrd_map_t *map;
    shr_fmm_t *field;
    int rv;

    SHR_FUNC_ENTER(unit);
    do {
        rv = bcmlrd_map_get(unit, entry->table_id, &map);

        /* Sanity check map data. */
        if (SHR_FAILURE(rv)) {
            /* Not mapped.*/
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Table not mapped.\n")));
            SHR_IF_ERR_CONT(rv);
            break;
        }

        if (map == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Mapped table is NULL.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        if (map->field_data == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Mapped field data is NULL.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        if (map->field_data->field == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Mapped field instance is NULL.\n")));
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        /* Resolve any enumerations found. */
        field = entry->l_field;

        while (field) {
            rv = bcmltp_resolve_enum(unit, map->field_data, enum_str, field);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_CONT(rv);
                break;
            }
            field = field->next;
        }

    } while (0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Play back LT data
 *
 * Play back BCMCFG Logical table data. User data is the enum list used
 * for enum resolution. Data is written by first trying an insert, and if the
 * insert says the entry exists, then try an update. This avoid duplicating
 * reference data in playback.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Logical table ID.
 * \param [in]     name         Logical table name.
 * \param [in,out] field        Playback field list.
 * \param [in]     user_data    Player user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_lt_play(int unit,
               uint32_t sid,
               const char *name,
               shr_fmm_t *field,
               void *user_data)
{
    bcmtrm_entry_t *entry;
    const bcmltp_enum_str_t *enum_str = (const bcmltp_enum_str_t *)user_data;
    int rv;
    bool interactive = false;

    SHR_FUNC_ENTER(unit);

    do {
        rv = bcmlrd_table_interactive_get(unit, sid, &interactive);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        rv = bcmltp_entry_alloc(unit, sid, name, interactive, field, &entry);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        entry->opcode.lt_opcode = BCMLT_OPCODE_INSERT;

        rv = bcmltp_resolve_enums(unit, enum_str, entry);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        rv = bcmtrm_entry_req(entry);

        if (SHR_SUCCESS(rv) && entry->info.status == SHR_E_EXISTS) {
            /* Assume an entry has been inserted, so just update. */
            entry->opcode.lt_opcode = BCMLT_OPCODE_UPDATE;
            rv = bcmtrm_entry_req(entry);

            if (SHR_FAILURE(rv) || SHR_FAILURE(entry->info.status)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Cannot update entry.\n")));
            }
        } else if (SHR_FAILURE(rv) || SHR_FAILURE(entry->info.status)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Cannot insert entry.\n")));
        }
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            char *str = bcmltp_entry_str(entry);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), str));
            sal_free(str);
        }
        SHR_IF_ERR_CONT(rv);
        rv = bcmtrm_entry_free(entry);
        SHR_IF_ERR_CONT(rv);
    } while (0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Play back PT data
 *
 * Play back BCMCFG Physical table data. No user data needed.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     sid          Physical table ID.
 * \param [in]     name         Physical table name.
 * \param [in,out] field        Playback field list.
 * \param [in]     user_data    Player user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_pt_play(int unit,
               uint32_t sid,
               const char *name,
               shr_fmm_t *field,
               void *user_data)
{
    bcmtrm_entry_t *entry;
    int rv;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(user_data);

    do {
        rv = bcmltp_entry_alloc(unit, sid, name, true, field, &entry);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        entry->opcode.pt_opcode = BCMLT_PT_OPCODE_SET;
        entry->pt = true;

        rv = bcmtrm_entry_req(entry);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Cannot set entry.\n")));
        }
        SHR_IF_ERR_CONT(rv);

        rv = bcmtrm_entry_free(entry);
        SHR_IF_ERR_CONT(rv);
    } while (0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Play back BCMCFG table data
 *
 * Play back BCMCFG table data for the given player.
 *
 * \param [in]     locus        BCMCFG source locus.
 * \param [in]     tlist        List of table data.
 * \param [in]     player       Player function.
 * \param [in]     user_data    Player user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_tlist_play(const char *locus,
                  const bcmcfg_tbl_list_t *tlist,
                  bcmltp_play_f player,
                  void *user_data)
{
    int rv = SHR_E_NONE;
    int unit;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (tlist) {
        for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            if (!tlist->unit_set[unit]) {
                continue;
            }
            rv = player(unit, tlist->sid, tlist->name, tlist->field, user_data);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("%s:%d:%d: playback error\n"),
                           locus, tlist->line, tlist->column));
                SHR_IF_ERR_CONT(rv);
                break;
            }
        }
        if (SHR_FAILURE(rv)) {
            break;
        }
        tlist = tlist->next;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Play back BCMCFG data
 *
 * Play back BCMCFG data for the given player.
 *
 * \param [in]     list         Playback list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmltp_list_play(const bcmcfg_playback_list_t *list)
{
    int rv;
    bcmltp_enum_str_t enum_str;
    const char *locus;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (list) {

        if (list->enum_count > 0) {
            size_t i;
            bcmcfg_playback_enum_t *elist = list->enum_head;

            if (elist == NULL) {
                /* count > 0 but no list head? */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Non-zero enum count "
                                    "but no enum head pointer.\n")));
                SHR_IF_ERR_CONT(SHR_E_INTERNAL);
                break;
            }

            enum_str.str = sal_alloc(list->enum_count * sizeof(char *),
                                     "bcmltp_list_play");
            if (enum_str.str == NULL) {
                SHR_IF_ERR_CONT(SHR_E_MEMORY);
                break;
            }

            for (i = 0; i < list->enum_count; i++) {
                if (elist == NULL) {
                    /* list ended early */
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("enum list ended early.\n")));
                    SHR_IF_ERR_CONT(SHR_E_INTERNAL);
                    break;
                }
                enum_str.str[i] = elist->value;
                elist = elist->next;
            }
            enum_str.count = list->enum_count;
        } else {
            enum_str.str = NULL;
            enum_str.count = 0;
        }

        locus = list->locus ? list->locus : "<unknown>";

        rv = bcmltp_tlist_play(locus, list->lt, bcmltp_lt_play, &enum_str);

        if (enum_str.str) {
            sal_free(enum_str.str);
        }

        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        rv = bcmltp_tlist_play(list->locus, list->pt, bcmltp_pt_play, NULL);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
        list = list->next;
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

/* Play back BCMCFG data. */
int bcmltp_play(void)
{
    int rv;
    const bcmcfg_playback_list_t *list = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        rv = bcmcfg_playback_get(&list);
        if (rv == SHR_E_UNAVAIL) {
            /* Unavailability is a verbose message, not warning. */
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            /* No break here because static analysis says it is unreachable. */
        }
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
        rv = bcmltp_list_play(list);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
    } while(0);

 exit:
    SHR_FUNC_EXIT();
}
