/*! \file bcma_sys_conf.h
 *
 * Shared system configuration API.
 *
 * Convenience functions for initilizing various SDK components.
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

#ifndef BCMA_SYS_CONF_H
#define BCMA_SYS_CONF_H

#include <bsl/bsl.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/env/bcma_env.h>
#include <bcma/cfgdb/bcma_cfgdb.h>

/*! Default YAML configuration file name. */
#define BCMA_SYS_CONF_CONFIG_YML "config.yml"

/*!
 * \brief System configuration options.
 *
 * Used to enable or disable optional features.
 */
typedef enum bcma_sys_conf_opt_e {

    /*!
     * Boolean option to turn off memory debugging, i.e. the sal_alloc
     * debug hooks will not be added.
     */
    BCMA_SYS_CONF_OPT_MEM_DEBUG = 0,

    /*!
     * Boolean option to turn on memory tracking, which means that all
     * allocation done through sal_alloc will be store in a dabatasbe,
     * which can be dumped at will.
     */
    BCMA_SYS_CONF_OPT_MEM_TRACK,

    /*!
     * Boolean option to run the SDK on a chip emulator instead of a
     * real device. Typically this will tweak the initialization
     * sequence and various timeout values.
     */
    BCMA_SYS_CONF_OPT_EMULATION,

    /*!
     * Boolean option to force a warm-boot initialization sequence.
     */
    BCMA_SYS_CONF_OPT_WARM_BOOT,

    /*!
     * Use this option to set the name of the SDK configuration
     * file(s). The file name must be passed as the string value and
     * the (zero-based) configuration file number must be passed as
     * the integer value.
     */
    BCMA_SYS_CONF_OPT_CONFIG_FILE,

    /*!
     * Bypass mode is a device-specific integer value that specified
     * the pipeline bypass mode. Various bypass modes typically lowers
     * packet latency at the expense of packet processing features.
     */
    BCMA_SYS_CONF_OPT_BYPASS_MODE,

    /*!
     * Boolean option to indicate whether the HA (High-Availability)
     * file should be deleted upon exit. This option enables the
     * testing of warm boot as the previous state (prior to warm boot)
     * will be kept in HA memory.
     */
    BCMA_SYS_CONF_OPT_KEEP_HA_FILE,

    /*! Should always be last. */
    BCMA_SYS_CONF_OPT_MAX

} bcma_sys_conf_opt_t;

/*!
 * \brief System configuration object.
 *
 * Used mainly to track resources allocated during system
 * initialization.
 */
typedef struct bcma_sys_conf_s {

    /*! Signature which indicates that object is initialized. */
    unsigned int sys_conf_sig;

    /*! Pointer to CLI object. */
    bcma_cli_t *cli;

    /*! Pointer to debug shell object. */
    bcma_cli_t *dsh;

    /*! CLI environment handle. */
    bcma_env_handle_t eh;

    /*! Configuration database (SDK6-style). */
    bcma_cfgdb_cfg_t cfgdb_cfg;

} bcma_sys_conf_t;

/*!
 * \brief Initialize system configuration data structure.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_init(bcma_sys_conf_t *sc);

/*!
 * \brief Release system configuration data structure resources.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize shared (SHR) libraries.
 *
 * Some shared libraries need to perform basic initialization
 * (resource allocation) before they can be used.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_shr_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up shared (SHR) libraries.
 *
 * This function will clean up all resources allocated by \ref
 * bcma_sys_conf_shr_init.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_shr_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize a basic CLI.
 *
 * Initialize basic CLI including terminal I/O, Ctrl-C handler, basic
 * CLI commands, shell variables, etc.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cli_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up basic CLI resources.
 *
 * This function will clean up all resources allocated by \ref
 * bcma_sys_conf_cli_init.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cli_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief CLI redirection hook for BSL output sink.
 *
 * This function should be installed into the BSL output hook to allow
 * command output to be fed back into the CLI input function.
 *
 * The call signature is identical to the BSL output hook itself.
 *
 * \param [in] meta Meta data for this log message.
 * \param [in] format A printf-style format string.
 * \param [in] args List of printf-style arguments.
 *
 * \return Negative value on error.
 */
extern int
bcma_sys_conf_cli_redir_bsl(bsl_meta_t *meta, const char *format, va_list args);

/*!
 * \brief Initialize and populate configuration database.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cfgdb_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up configuration database.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cfgdb_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize device resource database.
 *
 * This function will probe for supported devices and add them to the
 * device resource database.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_drd_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up device resources for installed devices.
 *
 * This function will essentially undo a device probe.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_drd_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize base driver for installed devices.
 *
 * This function will probe for supported devices and initialize the
 * base driver for each device.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_bd_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up device resources for base driver instance.
 *
 * This function will shut down and clean up the base driver for a
 * device.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_bd_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up pkt I/O driver for installed devices.
 *
 * This function will clean up packet I/O resources and datapath, and detach
 * the packet I/O driver.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_pkt_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize packet I/O driver for installed devices.
 *
 * This function will attach packet I/O driver for each device.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_pkt_init(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize test component for installed devices.
 *
 * This function will probe for supported devices and initialize the
 * test conponent.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_test_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up test component for installed devices.
 *
 * This function will detach the test component .
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_test_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize CLI readline completion.
 *
 * Initialize CLI readline completion and add the commands that support
 * completion feature to the command completion table.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_clirlc_init(void);

#endif /* BCMA_SYS_CONF_H */
