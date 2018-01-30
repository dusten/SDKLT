/*! \file bcma_cfgdb.h
 *
 * Configuration data manager
 *
 * Interface for managing configuration data.
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

#ifndef BCMA_CFGDB_H
#define BCMA_CFGDB_H

#include <bcma/env/bcma_env.h>

/*! Flag to record if varaible is imported from file. */
#define BCMA_CFGDB_TAG_FLAG_IMPORTED     0x00000001

/*! Flag to record if variable is flushed to file. */
#define BCMA_CFGDB_TAG_FLAG_FLUSHED      0x00000002

/*! Flag to record if variabe has ever been read. */
#define BCMA_CFGDB_TAG_FLAG_HIT          0x00000004

/*! Flag mask in variable tag. */
#define BCMA_CFGDB_TAG_FLAG_MASK         0x0000ffff

/*! Line number mask in variable tag. */
#define BCMA_CFGDB_TAG_LINE_MASK         0x0fff0000

/*! Line number shift bit in variable tag. */
#define BCMA_CFGDB_TAG_LINE_SHIFT        16

/*! Get flags value from cfgdb variable tag \c _t. */
#define BCMA_CFGDB_TAG_FLAGS_GET(_t)     ((_t) & BCMA_CFGDB_TAG_FLAG_MASK)

/*! Get line number imported from file from cfgdb variable tag \c _t. */
#define BCMA_CFGDB_TAG_LINE_NUM_GET(_t) \
    (((_t) & BCMA_CFGDB_TAG_LINE_MASK) >> BCMA_CFGDB_TAG_LINE_SHIFT)

/*! Set line number \c _l imported from file to cfgdb variable tag \c _t. */
#define BCMA_CFGDB_TAG_LINE_NUM_SET(_t, _l) \
    (_t) = (((_t) & ~BCMA_CFGDB_TAG_LINE_MASK) | \
           (((_l) << BCMA_CFGDB_TAG_LINE_SHIFT) &  BCMA_CFGDB_TAG_LINE_MASK))

/*!
 * Return value from bcma_cfgdb_var_traverse call-back function to stop traversing.
 */
#define BCMA_CFGDB_VAR_TRAVERSE_STOP     BCMA_ENV_VAR_TRAVERSE_STOP

/*!
 * Return value from bcma_cfgdb_var_traverse call-back function to delete variable.
 */
#define BCMA_CFGDB_VAR_TRAVERSE_DELETE   BCMA_ENV_VAR_TRAVERSE_DELETE

/*!
 * \brief Configuration Information for Configuration data manager object.
 *
 * Used mainly to pass information for configuration data manager
 * initialization.
 */
typedef struct bcma_cfgdb_cfg_s {

    /*! File name for configuration data manager to load variables from. */
    const char *filename;

    /*! Temporary file name for configuration data manager to write data. */
    const char *tmp_filename;

} bcma_cfgdb_cfg_t;

/*!
 * \brief Initialize configuration data manager object.
 *
 * \param[in] cfg Information for configuration data manager initialization
 *
 * \retval 0 No errors
 * \retval -1 Bad configuration data manager or memory allocation error
 */
extern int
bcma_cfgdb_init(bcma_cfgdb_cfg_t *cfg);

/*!
 * \brief Free resources of configuration data manager object.
 *
 * \retval 0 No errors
 * \retval -1 Bad configuration data manager
 */
extern int
bcma_cfgdb_cleanup(void);

/*!
 * \brief Load variables of configuration data manager.
 *
 * Load variables of configuration manager from config file.
 *
 * \retval 0 No errors
 * \retval -1 Bad configuration data manager
 */
extern int
bcma_cfgdb_load(void);

/*!
 * \brief Save variables of configuration data manager to file.
 *
 * Save variables of configuration data manager to 'filename'.
 * If argument 'append' is TRUE, the file would be saved as
 * appended. Otherwise the file would be written from the beginning
 * and saved. The function would only save variable names match 'wildcard_str'
 * if 'wildcar_str' is not NULL or non-empty.
 *
 * Wildcard string 'wildcard_str' is string containing wildcard character '*'.
 * Example of 'wildcard_str' usage:
 * "*" will match all strings.
 * "*mem*" will match "memory" and "socmem", but "mem*" will only
 * match "memory".
 *
 * \param[in] filename File name that variables to be written to and saved
 * \param[in] wildcard_str String contains '*' for comparison
 * \param[in] append File to be written from the beginning or appended(TRUE/FALSE)
 *
 * \retval 0 No errors
 * \retval -1 Invalid file name
 */
extern int
bcma_cfgdb_save(const char *filename, const char *wildcard_str, int append);

/*!
 * \brief Save variables of configuration data manager to file that variables loaded from.
 *
 * \retval 0 No errors
 * \retval -1 Invalid file name
 */
extern int
bcma_cfgdb_flush(void);

/*!
 * \brief Set a configuration data manager variable.
 *
 * The function could be operated to set or clear variable
 * depending on the 'value' argument. If 'value' is not NULL,
 * the variable 'name' would be updated or added (if 'name'
 * is not an existing variable) by string 'value'. If 'value' is NULL,
 * the variable 'name' (if existing) would be deleted.
 *
 * For delete operation (value is NULL), wildcard string is supported
 * for variable name 'name'. If the given variable name 'name' is
 * not found for deletion and 'name' contains wildcard character '*',
 * variable names match the wildcard string 'name' would be deleted.
 * For example:
 * bcma_cfgdb_var_set("*", NULL) would delete all variables in cfgdb.
 * bcma_cfgdb_var_set("*mem*, NULL") would delete variables contain sub-string
 * "mem".
 *
 * \param[in] name Name of variable
 * \param[in] value Value of variable
 *
 * \retval 0 Variable was set successfully
 * \retval -1 Bad environment or variable not found for deletion
 */
extern int
bcma_cfgdb_var_set(const char *name, char *value);

/*!
 * \brief Get value of a configuration data manager variable.
 *
 * \param[in] name Name of variable
 *
 * \return Name of variable or NULL if the variable is not found
 */
extern const char *
bcma_cfgdb_var_get(const char *name);

/*!
 * \brief Traverse function for all variables of configuration data manager.
 *
 * The function traverses all variables in cfgdb, call-back function 'cb' would
 * be called for all variables in this traverse function. The traverse function
 * would pass variable name, value, tag pointer and hash index to call-back
 * function 'cb'. If no hashing is applied for variable list, the 'hash_idx'
 * would be passed by -1.
 *
 * The call-back function 'cb' can control the traverse function by return value.
 * If \ref BCMA_CFGDB_VAR_TRAVERSE_STOP is returned from 'cb',
 * bcma_cfgdb_var_traverse function will be stopped immediately.
 * If \ref BCMA_CFGDB_VAR_TRAVERSE_DELETE is returned
 * from 'cb', current traversing variable will be deleted then.
 * Negtive value should be returned from call-back functioin 'cb' on error
 * and this would also stop traversing immediately. Otherwise 0 should be
 * returned from call-back function 'cb'.
 *
 * \param[in] cb Call-back function for variables traversing
 * \param[in] cookie User data for call-back function
 *
 * \retval Last return value from call-back function 'cb'
 */
extern int
bcma_cfgdb_var_traverse(int (*cb)(void *cookie,
                                  const char *name, const char *value,
                                  uint32_t *tag, int hash_idx),
                        void *cookie);


/*!
 * \brief Function to get file name and temporary file name in cfgdb.
 *
 * If 'filename' is not NULL, file name passed from bcma_cfgdb_init would be
 * assigned to filename. If 'tmp_filename' is not NULL, temporary
 * file name passed from bcma_cfgdb_init would be assigned to tmp_filename.
 *
 * \param[in,out] filename cfgdb file name to load file from
 * \param[in,out] tmp_filename cfgdb temporary file name to write data to
 *
 * \retval 0 No errors
 * \retval -1 Bad environment
 */
extern int
bcma_cfgdb_file_name_get(const char **filename, const char **tmp_filename);

#endif /* BCMA_CFGDB_H */
