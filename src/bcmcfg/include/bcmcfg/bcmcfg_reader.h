/*! \file bcmcfg_reader.h
 *
 * BCMCFG outer reader.
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

#ifndef BCMCFG_READER_H
#define BCMCFG_READER_H

#include <sal/sal_types.h>

/*!
 * \brief YAML reader state.
 *
 * Reader state is tracked by the various data readers, and represents
 * the current YAML event context.
 */
typedef enum bcmcfg_read_state_e {
    /*! Undefined YAML reader state. */
    BCMCFG_READ_STATE_NONE,

    /*! Reading a YAML stream. */
    BCMCFG_READ_STATE_STREAM,

    /*! Reading a YAML document. */
    BCMCFG_READ_STATE_DOC,

    /*! Reading a YAML sequence. */
    BCMCFG_READ_STATE_SEQ,

    /*! Reading a YAML map key. */
    BCMCFG_READ_STATE_MAP,

    /*! Reading a YAML map value. */
    BCMCFG_READ_STATE_MAP_V,
} bcmcfg_read_state_t;

/* This could eventually be derived from analysis */
/*!
 * \brief Maximum YAML state depth.
 *
 * To ease implementation, there is a fixed upper limit on the depth
 * of the YAML reader. This roughly corresponds to the "indent level"
 * of the YAML file. The value given is expected to be much larger
 * then what would be considered practical.
 */
#define BCMCFG_MAX_LEVEL 20

/*!
 * \brief YAML per level reader information.
 *
 * This contains generic information that may be needed by all readers
 * this is derivable from LibYAML event processing.
 */
typedef struct bcmcfg_read_level_info_s {
    /*! Reader state. */
    bcmcfg_read_state_t                 state;

    /*! Level of the sequence container if present. */
    int                                 seq_container;

    /*! Current sequence index if in a container. */
    uint32_t                            seq_idx;

    /*! Reader locus (filename or string). */
    const char *locus;

    /*! Reader line number. */
    int line;

    /*! Reader column number. */
    int column;

    /*! Read handler for this level. */
    const struct bcmcfg_read_handler_s *handler;

    /*! Private data for the read handler. */
    void                               *user_data;
} bcmcfg_read_level_info_t;

/*!
 * \brief YAML read context.
 *
 * Current read nesting level, and info for each level all
 * all those below.
 */
typedef struct bcmcfg_read_context_s {
    /*! Current reader level. */
    int level;

    /*! Info for each level. */
    bcmcfg_read_level_info_t info[BCMCFG_MAX_LEVEL];
} bcmcfg_read_context_t;

/*!
 * \brief Reader setup function.
 *
 * Prototype for a YAML reader setup function.
 *
 * \param [in]  start           Start (true) or stop (false).
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_s_f)(bool start,
                               void *user_data);

/*!
 * \brief Reader element function.
 *
 * Prototype for a YAML reader non-scalar element function.
 *
 * \param [in]  start           Start (true) or stop (false).
 * \param [in]  context         Reader context.
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_e_f)(bool start,
                               bcmcfg_read_context_t *context,
                               void *user_data);

/*!
 * \brief Reader scalar function.
 *
 * Prototype for a YAML reader scalar element function.
 *
 * \param [in]  value           Scalar value.
 * \param [in]  context         Reader context.
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_v_f)(const char *value,
                               bcmcfg_read_context_t *context,
                               void *user_data);

/*!
 * \brief Abort function.
 *
 * Prototype for a YAML reader abort function.
 *
 * \param [in]  context         Reader context.
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_n_f)(bcmcfg_read_context_t *context,
                               void *user_data);

/*!
 * \brief Reader handler.
 *
 * YAML reader handler structure that contains the handler
 * functions and the user data. Functions not required may
 * be set to NULL.
 */
typedef struct bcmcfg_read_handler_s {
    /*! Key string for this handler. */
    const char *key;

    /*! Setup/Cleanup function. */
    const bcmcfg_read_s_f setup;

    /*! Document start/stop function. */
    const bcmcfg_read_e_f doc;

    /*! Scalar read function. */
    const bcmcfg_read_v_f scalar;

    /*! Sequence start/stop function. */
    const bcmcfg_read_e_f seq;

    /*! Map start/stop function. */
    const bcmcfg_read_e_f map;

    /*! Abort notification function. */
    const bcmcfg_read_n_f abort;

    /*! Handler user data. */
    void *user_data;
} bcmcfg_read_handler_t;

/*!
 * \brief Read component handler.
 *
 * Data structure for the software component configuration reader.
 */
extern const bcmcfg_read_handler_t bcmcfg_read_component;

/*!
 * \brief Read device handler.
 *
 * Data structure for the device configuration reader.
 */
extern const bcmcfg_read_handler_t bcmcfg_read_device;

/*!
 * \brief BCMCFG read initializer.
 *
 * Initialize the YAML reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_init(void);

/*!
 * \brief BCMCFG read cleanup.
 *
 * Cleanup the YAML reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_cleanup(void);

/*!
 * \brief BCMCFG outer read initializer.
 *
 * The outer reader is the first reader in the handler stack, so it
 * must be initialized before parsing. This function will be
 * responsible for initializing any other readers.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_init(void);

/*!
 * \brief BCMCFG read context initializer.
 *
 * Initialize the YAML reader context stack.
 *
 * \param [in,out]  context     Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_init_context(bcmcfg_read_context_t *context);

/*!
 * \brief BCMCFG outer read cleanup.
 *
 * Cleanup the YAML outer reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_cleanup(void);

#endif /* BCMCFG_READER_H */
