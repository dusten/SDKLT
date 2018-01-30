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
 * 
 * 
 *
 * DO NOT EDIT THIS FILE!
 * This file will be auto-generated in the near future.
 *
 * This config file defines all compilation-time specifications for
 * the BCMDRD.
 *
 * Reasonable defaults are provided for all configuration options
 * where appropriate.
 *
 * You need not edit this file directly to change your configuration,
 * nor is modifying this file advised -- so doing will require
 * manually merging whenever the BCMDRD is upgraded.
 *
 * You should provide your own configuration options or overrides
 * through a combination of:
 *
 *      1. The compiler command line, such as -D{OPTION}={VALUE}
 *
 *      2. Create your own custom configuration file:
 *         a) Create a file called 'bcmdrd_custom_config.h'
 *         b) Define all custom settings, using this file as
 *            the reference
 *         c) Add -DBCMDRD_INCLUDE_CUSTOM_CONFIG to your
 *            compilation
 *         d) Make sure the compilation include path includes
 *            'bcmdrd_custom_config.h'
 */

#ifndef BCMDRD_CONFIG_H
#define BCMDRD_CONFIG_H


/*
 * Include system config file if specified:
 */
#ifdef BCMDRD_INCLUDE_CUSTOM_CONFIG
#include <bcmdrd_custom_config.h>
#endif


/*
 * OPTIONAL configuration and feature values.
 * Defaults are provided for all non-specified values.
 */

/* Maximum number of chips supported */
#ifndef BCMDRD_CONFIG_MAX_UNITS
#define BCMDRD_CONFIG_MAX_UNITS                    8
#endif

/* Maximum number of ports per chip supported */
#ifndef BCMDRD_CONFIG_MAX_PORTS
#define BCMDRD_CONFIG_MAX_PORTS                    288
#endif

/* Maximum number of SCHAN polls */
#ifndef BCMDRD_CONFIG_SCHAN_MAX_POLLS
#define BCMDRD_CONFIG_SCHAN_MAX_POLLS              100000
#endif

/* Maximum number of MIIM polls */
#ifndef BCMDRD_CONFIG_MIIM_MAX_POLLS
#define BCMDRD_CONFIG_MIIM_MAX_POLLS               100000
#endif

/* Direct access to memory-mapped registers */
#ifndef BCMDRD_CONFIG_MEMMAP_DIRECT
#define BCMDRD_CONFIG_MEMMAP_DIRECT                0
#endif

/*
 * Include chip symbol tables for the debug shell.
 *
 * No symbolic debugging (register/memory names) will be available
 * without this defined.
 *
 * You should enable at least these symbols if you can afford the
 * space.
 *
 * This define is required to get any symbols at all.
 *
 * If you only wish to include symbols for a subset of chips in the
 * system (probably for code space reasons), you can define the
 * following for each chip whose symbols you wish to EXCLUDE:
 *
 *       BCMDRD_CONFIG_EXCLUDE_CHIP_SYMBOLS_<CHIP>
 */
#ifndef BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS
#define BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS         1
#endif

/*
 * Include register and memory field information for the debug shell.
 *
 * This provides encoding, decoding, and displaying individual field
 * values for each register and memory.
 *
 * Requires more code space than just the chip symbols alone.
 *
 * The per-chip exclusion define
 * (BCMDRD_CONFIG_EXCLUDE_FIELD_INFO_<CHIP>) also applies.
 */
#ifndef BCMDRD_CONFIG_INCLUDE_FIELD_INFO
#define BCMDRD_CONFIG_INCLUDE_FIELD_INFO           1
#endif

/*
 * Include alternative symbol names for registers and memories.
 *
 * Mainly for internal Broadcom use, so you can safely leave this
 * option off.
 */
#ifndef BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES
#define BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES          1
#endif

#endif /* BCMDRD_CONFIG_H */

#ifdef CONFIG_OPTION
#ifdef BCMDRD_INCLUDE_CUSTOM_CONFIG
CONFIG_OPTION(BCMDRD_INCLUDE_CUSTOM_CONFIG)
#endif
#ifdef BCMDRD_CONFIG_MAX_UNITS
CONFIG_OPTION(BCMDRD_CONFIG_MAX_UNITS)
#endif
#ifdef BCMDRD_CONFIG_MAX_PORTS
CONFIG_OPTION(BCMDRD_CONFIG_MAX_PORTS)
#endif
#ifdef BCMDRD_CONFIG_SCHAN_MAX_POLLS
CONFIG_OPTION(BCMDRD_CONFIG_SCHAN_MAX_POLLS)
#endif
#ifdef BCMDRD_CONFIG_MIIM_MAX_POLLS
CONFIG_OPTION(BCMDRD_CONFIG_MIIM_MAX_POLLS)
#endif
#ifdef BCMDRD_CONFIG_MEMMAP_DIRECT
CONFIG_OPTION(BCMDRD_CONFIG_MEMMAP_DIRECT)
#endif
#ifdef BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS
CONFIG_OPTION(BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS)
#endif
#ifdef BCMDRD_CONFIG_INCLUDE_FIELD_INFO
CONFIG_OPTION(BCMDRD_CONFIG_INCLUDE_FIELD_INFO)
#endif
#ifdef BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES
CONFIG_OPTION(BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES)
#endif
#endif /* CONFIG_OPTION */
#include "bcmdrd_config_chips.h"
