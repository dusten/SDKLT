/*! \file bcma_cli.h
 *
 * CLI core functions.
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

#ifndef BCMA_CLI_H
#define BCMA_CLI_H

#include <sal/sal_types.h>

#include <bcma/cli/bcma_cli_config.h>
#include <bcma/cli/bcma_cli_arg.h>

struct bcma_cli_s;

/*!
 * \brief Call-back to read from command input device.
 *
 * When the CLI is started, it must be provided a function for reading
 * user input. The function should display the supplied 'prompt' and
 * return the null-terminated user input in 'str', which is a buffer
 * of size 'max'.
 *
 * Multiple CLI instances might be created but will still share the same
 * input device. The \c cli is used to differentiate the CLI instance
 * which is currently running on the input device.
 *
 * \param [in] cli CLI object
 * \param [in] prompt Command prompt to display
 * \param [out] str Raw command line
 * \param [in] max Size of raw command line buffer
 *
 * \return Pointer to input string
 */
typedef char *(*bcma_cli_gets_t)(struct bcma_cli_s *cli,
                                 const char *prompt, char *str, int max);

/*!
 * \brief Call-back to add command to history.
 *
 * \param [in] str String to be added to history
 */
typedef void (*bcma_cli_add_history_t)(char *str);

/*!
 * \brief Handler for unrecognized CLI commands.
 *
 * If a CLI command is not recognized, the command can optionally
 * be passed to an application-provided exception handler for special
 * processing.
 *
 * The return value should be BCMA_CLI_CMD_NOT_FOUND if the command
 * is not processed in the exception handler.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
typedef int (*bcma_cli_cmd_except)(struct bcma_cli_s *cli,
                                   struct bcma_cli_args_s *args);

/*!
 * \brief Call-back for reading environment variable.
 *
 * This function must return the value of the specified environment
 * variable or NULL if the variable does not exist.
 *
 * \param [in] cookie Pass-through environment data
 * \param [in] name Environment variable to retrieve
 *
 * \return Value of environment variable or NULL if not found.
 */
typedef const char *(*bcma_cli_var_get_t)(void *cookie, const char *name);

/*!
 * \brief Call-back for setting an environment variable.
 *
 * This function will set the value of the specified environment variable.
 * If 'value' is NULL, environment variable 'name' will be cleared.
 * If the environment variable does not already exist, it will be created.
 *
 * \param [in] cookie Pass-through environment data
 * \param [in] name Environment variable to set
 * \param [in] value Value to set to environment variable
 * \param [in] local Set variable in local scope (TRUE/FALSE)
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong
 */
typedef int(*bcma_cli_var_set_t)(void *cookie, const char *name, char *value,
                                 int local);

/*!
 * \brief Call-back to push scope of local environment variables.
 *
 * The environment variable scope is pushed whenever the command input
 * source changes. This typically happens when a new command script is
 * loaded. The new scope inherits the global variables, but not the
 * local ones.
 *
 * \param [in] cookie Pass-through environment data
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong
 */
typedef int (*bcma_cli_var_scope_push_t)(void *cookie);

/*!
 * \brief Call-back to pop scope of local environment variables.
 *
 * The environment variable scope is popped whenever the current
 * command input source is exhausted. This typically happens when a
 * command script is complete.
 *
 * \param [in] cookie Pass-through environment data
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong
 */
typedef int (*bcma_cli_var_scope_pop_t)(void *cookie);

/*!
 * \brief CLI environment variable call-back vectors.
 *
 * This structure can be used to add environment variable support to
 * the basic CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_var_cb_set().
 */
typedef struct bcma_cli_var_cb_s {

    /*! Function for shell variable expansion */
    bcma_cli_var_get_t var_get;

    /*! Function for shell variable expansion */
    bcma_cli_var_set_t var_set;

    /*! Function to push local environment variable scope. */
    bcma_cli_var_scope_push_t var_scope_push;

    /*! Function to pop local environment variable scope. */
    bcma_cli_var_scope_pop_t var_scope_pop;

} bcma_cli_var_cb_t;

/*!
 * \brief Call-back to enable/disable Ctrl-C traps.
 *
 * When enabled, a Ctrl-C trap handler is installed, and when
 * disabled, the original Ctrl-C behavior is restored.
 *
 * \param [in] cookie Pass-through Ctrl-C data
 * \param [in] enable Enable (1) or disable (0) Ctrl-C traps
 *
 * \retval 0 No errors
 */
typedef int (*bcma_cli_ctrlc_enable_set_t)(void *cookie, int enable);

/*!
 * \brief Call-back to execute a function with Ctrl-C hook
 *
 * This function is used to allow a Ctrl-C keypress to interrupt an
 * active CLI command, e.g. if it was invoked in error or the user
 * does not want to wait for normal command completion.
 *
 * \param [in] cookie Pass-through Ctrl-C data
 * \param [in] func Function to call
 * \param [in] data Function context
 * \param [in] rv_intr Return value when interrupted by Ctrl-C
 *
 * \retval 0 No errors
 */
typedef int (*bcma_cli_ctrlc_exec_t)(void *cookie, int (*func)(void *),
                                     void *data, int rv_intr);

/*!
 * \brief CLI environment variable call-back vectors.
 *
 * This structure can be used to add Ctrl-C trap support to the basic
 * CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_ctrlc_cb_set().
 */
typedef struct bcma_cli_ctrlc_cb_s {

    /*! Function for setting Ctrl-C response mode. */
    bcma_cli_ctrlc_enable_set_t ctrlc_enable_set;

    /*! Function for executing a function with Ctrl-C trap. */
    bcma_cli_ctrlc_exec_t ctrlc_exec;

} bcma_cli_ctrlc_cb_t;

/*!
 * \brief Call-back to check if a device feature is present.
 *
 * This function is used to support different sets of CLI commands on
 * different devices or to have different implementations of the same
 * command on different devices.
 *
 * \param [in] cookie Pass-through unit data
 * \param [in] unit Unit number
 * \param [in] feature Feature to check for
 *
 * \retval TRUE Unit is valid
 * \retval FALSE Unit is not valid
 */
typedef int (*bcma_cli_unit_valid_t)(void *cookie, int unit);

/*!
 * \brief Call-back to check if a device feature is present.
 *
 * This function is used to support different sets of CLI commands on
 * different devices or to have different implementations of the same
 * command on different devices.
 *
 * \param [in] cookie Pass-through unit data
 * \param [in] unit Unit number
 * \param [in] feature Feature to check for
 *
 * \retval TRUE Feature is enabled for this unit
 * \retval FALSE Feature is not enabled for this unit
 */
typedef int (*bcma_cli_unit_feature_enabled_t)(void *cookie,
                                               int unit, int feature);

/*!
 * \brief CLI environment unit call-back vectors.
 *
 * This structure can be used for specialized unit (device) management
 * in the CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_unit_cb_set().
 */
typedef struct bcma_cli_unit_cb_s {

    /*! Function for checking if a device is present. */
    bcma_cli_unit_valid_t unit_valid;

    /*! Function for checking if a device feature is present. */
    bcma_cli_unit_feature_enabled_t unit_feature_enabled;

} bcma_cli_unit_cb_t;

/*!
 * \brief Call-back to control CLI I/O redirection.
 *
 * If a CLI command is followed by '|', the I/O redirection will be
 * enabled, otherwise it will be disabled.
 *
 * \param [in] cookie Pass-through redirection data
 * \param [in] enable Enable (1) or disable (0) CLI I/O redirection
 *
 * \retval 0 No errors
 */
typedef int (*bcma_redir_enable_set_t)(void *cookie, int enable);

/*!
 * \brief Call-back for indicating the completion of a CLI command.
 *
 * \param [in] cookie Pass-through redirection data
 *
 * \retval 0 No errors
 */
typedef int (*bcma_redir_cmd_done_t)(void *cookie);

/*!
 * \brief Call-back to get input source of CLI I/O redirection.
 *
 * The input source is normally a file or a named pipe.
 *
 * \param [in] cookie Pass-through redirection data
 *
 * \return Pointer to name of input source
 */
typedef const char *(*bcma_redir_input_name_get_t)(void *cookie);

/*!
 * \brief Call-back to get output source of CLI I/O redirection.
 *
 * The output source is normally a file or a named pipe.
 *
 * \param [in] cookie Pass-through redirection data
 *
 * \return Pointer to name of output source
 */
typedef const char *(*bcma_redir_output_name_get_t)(void *cookie);

/*!
 * \brief CLI I/O redirection call-back vectors.
 *
 * This structure can be used to add I/O redirection support to the
 * CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_redir_cb_set().
 */
typedef struct bcma_cli_redir_cb_s {

    /*! Function for setting CLI I/O redirection. */
    bcma_redir_enable_set_t redir_enable_set;

    /*! Function for indicating completion of a CLI command. */
    bcma_redir_cmd_done_t redir_cmd_done;

    /*! Function for getting CLI input source. */
    bcma_redir_input_name_get_t redir_input_name_get;

    /*! Function for getting CLI output source. */
    bcma_redir_output_name_get_t redir_output_name_get;

} bcma_cli_redir_cb_t;

struct bcma_cli_command_s;

/*!
 * \brief CLI command list object.
 *
 * All installed CLI commands are stored in a list of this type.
 *
 * Each CLI object maintains its own list of commands.
 */
typedef struct bcma_cli_cmd_list_s {

    /*! Next CLI command in list. */
    struct bcma_cli_cmd_list_s *next;

    /*! CLI command object. */
    struct bcma_cli_command_s *cmd;

    /*!
     * Command feature condition.
     *
     * If non-zero, this value is checked against a list of device
     * features, and only if the feature is enabled for the current
     * device, the command will be active/visible. This field can also
     * be used to implement multiple versions of a particular command,
     * and verwsion executed will depend on the devices it is being
     * executed on. For example, a register "get" command may need
     * different implementations for different device architectures.
     */
    int feature;

} bcma_cli_cmd_list_t;

/*!
 * \brief CLI object.
 *
 * The object is designed to provide minimal functionality by default.
 *
 * Support for environment variables can bee added through a set of
 * call-back functions. Environment variables can be useful for
 * scripting and other types of automation.
 *
 * Support for Ctrl-C can also be added through a set of call-back
 * functions. This functionality is intended to allow a Ctrl-C
 * keypress to terminate the execution of a CLI command
 * immediately.
 */
typedef struct bcma_cli_s {

    /*! Signature which indicates that object is initialized. */
    unsigned int cli_sig;

    /*! Signature for dynamically allocated objects. */
    unsigned int dyn_sig;

    /*! List of installed commands. */
    struct bcma_cli_cmd_list_s *command_root;

    /*! Input buffer from console device. */
    char ibuf[BCMA_CLI_CONFIG_INPUT_BUFFER_MAX];

    /*! Command propmpt. */
    const char *prompt;

    /*! Function to read console input. */
    bcma_cli_gets_t gets;

    /*! Function to add string to input history. */
    bcma_cli_add_history_t add_history;

    /*! Optional exception handler for unrecognized CLI commands. */
    bcma_cli_cmd_except cmd_except;

    /*! Current default device number */
    int cur_unit;

    /*! Current command device number */
    int cmd_unit;

    /*! Call-backs for environment variable management */
    const bcma_cli_var_cb_t *var_cb;

    /*! Cookie for environment variable operations. */
    void *var_cb_data;

    /*! Call-backs for Ctrl-C management */
    const bcma_cli_ctrlc_cb_t *ctrlc_cb;

    /*! Cookie for Ctrl-C operations. */
    void *ctrlc_cb_data;

    /*! Call-backs for unit management */
    const bcma_cli_unit_cb_t *unit_cb;

    /*! Cookie for unit operations. */
    void *unit_cb_data;

    /*! Call-backs for I/O redirection management */
    const bcma_cli_redir_cb_t *redir_cb;

    /*! Cookie for I/O redirection. */
    void *redir_cb_data;

} bcma_cli_t;

/*!
 * \brief CLI command structure.
 *
 * This structure is used to define commands for the CLI.
 */
typedef struct bcma_cli_command_s {

    /*! Command name (mandatory). */
    char *name;

    /*! Command function (mandatory). */
    int (*func)(bcma_cli_t *cli, bcma_cli_args_t *arg);

    /*! Brief command description (mandatory). */
    char *desc;

    /*! Command syntax (recommended). */
    char *synop;

    /*! Detailed command help (recommended). */
    char *help[BCMA_CLI_CONFIG_MAX_HELP_LINES];

} bcma_cli_command_t;

/*! Possible results for CLI commands */
typedef enum bcma_cmd_result_e {
    BCMA_CLI_CMD_OK          =  0,
    BCMA_CLI_CMD_FAIL        = -1,
    BCMA_CLI_CMD_USAGE       = -2,
    BCMA_CLI_CMD_NOT_FOUND   = -3,
    BCMA_CLI_CMD_EXIT        = -4,
    BCMA_CLI_CMD_INTR        = -5,
    BCMA_CLI_CMD_NO_SYM      = -6,
    BCMA_CLI_CMD_BAD_ARG     = -7,
    BCMA_CLI_CMD_AMBIGUOUS   = -8
} bcma_cmd_result_t;

/*!
 * \brief Allocate CLI object.
 *
 * \return Newly created CLI object or NULL if error.
 */
extern bcma_cli_t *
bcma_cli_create(void);

/*!
 * \brief Free previously allocated CLI object.
 *
 * This function will also free all resources associated with the CLI
 * object.
 *
 * \param [in] cli CLI object
 *
 * \retval 0 No errors
 * \retval -1 Not dynamically allocated.
 */
extern int
bcma_cli_destroy(bcma_cli_t *cli);

/*!
 * \brief Initialize CLI object.
 *
 * \param [in] cli CLI object
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_init(bcma_cli_t *cli);

/*!
 * \brief Deinitialize CLI object.
 *
 * \param [in] cli CLI object
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_cleanup(bcma_cli_t *cli);

/*!
 * \brief Clear command list.
 *
 * This function will remove all installed CLI commands and free the
 * associated resources.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 */
extern int
bcma_cli_cmd_list_clear(bcma_cli_t *cli);

/*!
 * \brief Create CLI command list entry.
 *
 * \return Newly created command list entry or NULL if error.
 */
extern bcma_cli_cmd_list_t *
bcma_cli_cmd_entry_create(void);

/*!
 * \brief Free previously allocated CLI command list entry.
 *
 * \param [in] cl CLI command list entry.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid entry.
 */
extern int
bcma_cli_cmd_entry_destroy(bcma_cli_cmd_list_t *cl);

/*!
 * \brief Set prompt and input call-back.
 *
 * Install function to read from input device.
 *
 * \param [in] cli CLI object
 * \param [in] prompt String to show when prompting for commands
 * \param [in] cb_gets Function to read commands from input device
 * \param [in] add_history Optional function to add string to input history
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_input_cb_set(bcma_cli_t *cli, const char *prompt,
                      bcma_cli_gets_t cb_gets,
                      bcma_cli_add_history_t add_history);

/*!
 * \brief Execute function with Ctrl-C hook
 *
 * This function is used to allow a Ctrl-C keypress to interrupt an
 * active CLI command, e.g. if it was invoked in error or the user
 * does not want to wait for normal command completion.
 *
 * \param [in] cli CLI object
 * \param [in] func Function to call
 * \param [in] data Function context
 *
 * \retval 0 No errors
 */
extern int
bcma_cli_ctrlc_exec(bcma_cli_t *cli, int (*func)(void *), void *data);

/*!
 * \brief Show list of available CLI commands.
 *
 * The list will be filtered based on the current device and an
 * optional prefix string. The prefix string is typically used to show
 * a list of possible commands, when an ambigious command string has
 * been entered. Only commands starting with the supplied prefix
 * string will be listed.
 *
 * \param [in] cli CLI object
 * \param [in] prefix Optional prefix string for comparison
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_show_cmds_avail(bcma_cli_t *cli, char *prefix);

/*!
 * \brief Parse and execute command line.
 *
 * The optional c_next parameter can be used to parse separate command
 * statements within a longer command line. Each statement is
 * typically terminated by a semicolon (or a NUL character for the
 * last statement of the input string.) If the c_next pointer is
 * provided, then it will be updated to point to the next statement or
 * set to NULL if no more statements are available.
 *
 * \param [in] cli CLI object
 * \param [in] str Command line from input device
 * \param [out] c_next Pointer to the next statement (may be NULL)
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully
 * \retval BCMA_CLI_CMD_NOT_FOUND Command was not recognized
 * \retval BCMA_CLI_CMD_AMBIGUOUS Command is not unique
 * \retval BCMA_CLI_CMD_EXIT Command requested shell to terminate
 * \retval BCMA_CLI_CMD_* Command returned an error
 */
extern int
bcma_cli_cmd_dispatch(bcma_cli_t *cli, const char *str, const char **c_next);

/*!
 * \brief Lookup command in list of installed commands.
 *
 * BCMA_CLI_CMD_OK will be returned on an exact match even if one or more
 * partial matches exist. In other words, an exact match overrides
 * command ambiguity.
 *
 * \param [in] cli CLI object
 * \param [in] name Command string token from input device
 * \param [out] clicmd CLI command structure for matching command
 *
 * \retval BCMA_CLI_CMD_OK Unique match was found
 * \retval BCMA_CLI_CMD_AMBIGUOUS Two or more partial matches
 * \retval BCMA_CLI_CMD_NOT_FOUND No matches
 */
extern int
bcma_cli_cmd_lookup(bcma_cli_t *cli, const char *name,
                    bcma_cli_command_t **clicmd);

/*!
 * \brief Process and execute command line.
 *
 * <long-description>
 *
 * \param [in] cli CLI object
 * \param [in] str Raw command string
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_NOT_FOUND Command was not recognized.
 * \retval BCMA_CLI_CMD_AMBIGUOUS Command was not unique.
 * \retval BCMA_CLI_CMD_EXIT Command requested shell to terminate.
 * \retval BCMA_CLI_CMD_* Command returned an error.
 */
extern int
bcma_cli_cmd_process(bcma_cli_t *cli, const char *str);

/*!
 * \brief Main CLI command loop
 *
 * Returns only after a 'quit" command has been entered.
 *
 * \param [in] cli CLI object
 */
extern int
bcma_cli_cmd_loop(bcma_cli_t *cli);

/*!
 * \brief Add command to shell command table.
 *
 * If NULL is passed as clicmd, the command table is cleared.
 *
 * \param [in] cli CLI object
 * \param [in] clicmd CLI command structure
 * \param [in] feature Optional feature check to be preformed on this command
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_add_command(bcma_cli_t *cli, bcma_cli_command_t *clicmd, int feature);

/*!
 * \brief Check if a CLI command is enabled.
 *
 * This function will check if an installed CLI command is valid for
 * the current device and configuration. The check is performed
 * through an application-provided call-back.
 *
 * \param [in] cli CLI object
 * \param [in] feature Optional command feature
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_cmd_enabled(bcma_cli_t *cli, int feature);

/*!
 * \brief Wrapper for the call-back to read from command input device.
 *
 * When the CLI is started is must be provided a function for reading
 * user input. The function should display the supplied 'prompt' and
 * return the null-terminated user input in 'str', which is a buffer
 * of size 'max'.
 *
 * \param [in] cli CLI object
 * \param [in] prompt Command prompt to display
 * \param [out] str Raw command line
 * \param [in] max Size of raw command line buffer
 *
 * \return Pointer to input string
 */
extern char *
bcma_cli_gets(bcma_cli_t *cli, const char *prompt, char *str, int max);

/*!
 * \brief Wrapper for the call-back to add string to input history.
 *
 * \param [in] cli CLI object
 * \param [in] str String to be added to history
 */
extern void
bcma_cli_add_history(bcma_cli_t *cli, char *str);

/*!
 * \brief Set exception handler for unrecognized CLI command.
 *
 * Install function to execute while CLI command is invalid.
 *
 * \param [in] cli CLI object
 * \param[in] cmd_except Function to be called for unrecognized CLI commands
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_cmd_exception_set(bcma_cli_t *cli, bcma_cli_cmd_except cmd_except);

/*!
 * \brief Display usage message of a CLI command.
 *
 * \param [in] cli CLI object
 * \param[in] cmd Target CLI command
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object or bad CLI command
 */
extern int
bcma_cli_cmd_usage_show(bcma_cli_t *cli, bcma_cli_command_t *cmd);

#endif /* BCMA_CLI_H */
