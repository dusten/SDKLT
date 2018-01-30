/*! \file bcma_bcmltcmd_lt.h
 *
 * CLI command related to logical table access.
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

#ifndef BCMA_BCMLTCMD_LT_H
#define BCMA_BCMLTCMD_LT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMLTCMD_LT_DESC \
    "Logical table access"

/*! Syntax for CLI command. */
#define BCMA_BCMLTCMD_LT_SYNOP \
    "list [<OPT>] [<lt_name> ...]\n" \
    "traverse [<OPT>] [<lt_name> ...]\n" \
    "<table_op> [<table_op> ...]"

/*! Help for CLI command. */
#define BCMA_BCMLTCMD_LT_HELP \
    "<table_op> := <name> [<ATTR>] <entry_op> [<entry_op> ...]\n" \
    "<entry_op> := <op> [<OPT>] [<field>=<val> ...]\n\n" \
    "The <table_op> performs logical table operation(s) through an entry\n" \
    "in the given logical table <name>.\n" \
    "\n" \
    "<ATTR>: attr=<attr>[,<attr>...]\n" \
    "  Currently supported <attr> is CACHE_BYPASS.\n" \
    "\n" \
    "The valid <op>s in table <entry_op> are:\n" \
    "  alloc, delete, insert, update, lookup and traverse.\n" \
    "\n" \
    "<OPT>: A dash followed by one or more letters.\n" \
    "lt <table_op>\n" \
    "  -a: Auto-fill default values for any unspecified key fields.\n" \
    "      Applicable to insert/update/lookup/delete operations.\n" \
    "  -i: Automatically attempt an insert operation if an update\n" \
    "      operation returns entry-not-found.\n" \
    "  -l: Applicable to traverse operation to perform lookup in traverse.\n" \
    "  -u: Applicable to traverse operation to perform update in traverse.\n" \
    "  -d: Applicable to traverse operation to perform delete in traverse.\n" \
    "lt list\n" \
    "  -l: Use long output format when listing table information.\n" \
    "  -b: Use brief output format when listing table information.\n" \
    "  -d: Display table documentation.\n" \
    "  -m: Display physical table mapping information.\n" \
    "lt traverse\n" \
    "  -l: List all entry fields instead of key fields only.\n" \
    "  -v: List the traverse result in verbose mode.\n" \
    "\n" \
    "Examples:\n" \
    "lt list -b\n" \
    "lt list -l @VLAN\n" \
    "lt list -m L2_MY_STATION\n" \
    "lt list -d TABLE_OP_PT_INFO\n" \
    "lt list -ld @L2_FDB_VLAN\n" \
    "lt VLAN insert VLAN_ID=0x11 EGR_MEMBER_PORTS=2 ING_MEMBER_PORTS=2 \n" \
    "   UNTAGGED_MEMBER_PORTS=2 VLAN_STG_ID=1 L3_IIF_ID=1\n" \
    "lt VLAN lookup VLAN_ID=0x11\n" \
    "lt VLAN update -i VLAN_ID=7 VLAN_STG_ID=1\n" \
    "lt VLAN traverse\n" \
    "lt VLAN traverse -u FID=3\n" \
    "lt VLAN traverse -l\n" \
    "lt traverse -v VLAN\n" \
    "lt VLAN_ING_TAG_ACTION_PROFILE insert \n" \
    "   VLAN_ING_TAG_ACTION_PROFILE_ID=1 UT_OTAG=ADD\n" \
    "lt VLAN_ING_TAG_ACTION_PROFILE attr=CACHE_BYPASS lookup \n" \
    "   VLAN_ING_TAG_ACTION_PROFILE_ID=1\n" \
    "lt VLAN_STG insert VLAN_STG_ID=11 STATE[0-135]=FORWARD\n" \
    "lt PC_PM insert PC_PM_ID=0 SPEED_MAX[0-15]=0,SPEED_MAX[0]=40000 \n" \
    "   PM_OPMODE[0]=PC_PM_OPMODE_DEFAULT LANE_MAP[0]=0xf,LANE_MAP[1-15]=0"

/*!
 * \brief Logical table access command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmltcmd_lt(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMLTCMD_LT_H */
