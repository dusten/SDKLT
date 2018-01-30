/*
 * $Id$
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
 * Chip symbol table definitions.
 */

#ifndef __PHYMOD_SYMBOLS_H__
#define __PHYMOD_SYMBOLS_H__

/*******************************************************************************
 *
 * PHYMOD SYMBOL FORMATS
 */

#include <phymod/phymod_system.h>


/*******************************************************************************
 *
 * Resolve compile depedencies for optimized code size reduction.
 */

#if PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
#undef PHYMOD_CONFIG_INCLUDE_FIELD_INFO
#define PHYMOD_CONFIG_INCLUDE_FIELD_INFO 0
#endif

#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 0
#undef PHYMOD_CONFIG_INCLUDE_FIELD_NAMES
#define PHYMOD_CONFIG_INCLUDE_FIELD_NAMES 0
#endif

#define PHYMOD_MAX_REG_WSIZE            1

/*
 * Symbol Flags
 *
 * The first 24 bits are reserved for architecture-specific purposes
 * such as block types and address extensions.
 *
 * The final 8 bits are defined as follows:
 */
#define PHYMOD_SYMBOL_FLAG_START        0x01000000
#define PHYMOD_SYMBOL_FLAG_REGISTER     0x01000000
#define PHYMOD_SYMBOL_FLAG_PORT         0x02000000
#define PHYMOD_SYMBOL_FLAG_COUNTER      0x04000000
#define PHYMOD_SYMBOL_FLAG_MEMORY       0x08000000
#define PHYMOD_SYMBOL_FLAG_R64          0x10000000
#define PHYMOD_SYMBOL_FLAG_BIG_ENDIAN   0x20000000
#define PHYMOD_SYMBOL_FLAG_MEMMAPPED    0x40000000
#define PHYMOD_SYMBOL_FLAG_SOFT_PORT    0x80000000
#define PHYMOD_SYMBOL_FLAG_LAST         0x80000000
#define PHYMOD_SYMBOL_FLAGS_INVALID     0xffffffff


/*
 * Field Information Structure
 *
 * This structure defines a single field within a symbol
 */
typedef struct phymod_field_info_s {
    const char *name; 
    int fid; 
    uint16_t minbit;
    uint16_t maxbit; 
} phymod_field_info_t; 


/*
 * Index Information
 *
 * Some entries, such as memories and register arrays, have a size and minimum and 
 * maximum index value. 
 * 
 * These are combined in one 32 bit word to save space as follows:
 *
 *      size:   8 bits
 *      min:    5 bits
 *      max:   16 bits
 *      flags:  3 bits
 *
 * The ENCODE macros are used to create the fields in the symbol table entry. 
 * Use the GET macro to retrieve it when parsing the symbol. 
 *
 * The following flags affect the interpretation of min and max:
 *
 * EXP
 * Used if real max cannot be contained within max field.
 * Real max is encoded as ((1 << max) * min) - 1)
 * Real min is always zero
 * Ex.: (max,min)=(12,17) => real max = ((1 << 12) * 17) - 1) = 0x10fff
 *
 * VAR
 * Used e.g. if register array size is port-dependent.
 * Real max and min depend on configuration
 * min is encoding key which may be passed to an arch-specific function
 * 
 * STEP
 * Used e.g. if address step for a register array is non-standard.
 * Real min is always zero
 * min is the address step - 1
 * Ex.: (max,min)=(11,3) => 12 elements with addr(i+1) = addr(i)+4
 */

#define PHYMOD_SYMBOL_INDEX_F_EXP  0x1
#define PHYMOD_SYMBOL_INDEX_F_VAR  0x2
#define PHYMOD_SYMBOL_INDEX_F_STEP 0x4

#define PHYMOD_SYMBOL_INDEX_SIZE_ENCODE(s) __F_ENCODE((uint32_t)s, 0, 8)
#define PHYMOD_SYMBOL_INDEX_SIZE_GET(w) __F_GET(w, 0, 8)
#define PHYMOD_SYMBOL_INDEX_MIN_ENCODE(s) __F_ENCODE((uint32_t)s, 8, 5)
#define PHYMOD_SYMBOL_INDEX_MIN_GETRAW(w) __F_GET(w, 8, 5)
#define PHYMOD_SYMBOL_INDEX_MAX_ENCODE(s) __F_ENCODE((uint32_t)s, 13, 16)
#define PHYMOD_SYMBOL_INDEX_MAX_GETRAW(w) __F_GET(w, 13, 16)
#define PHYMOD_SYMBOL_INDEX_FLAGS_ENCODE(s) __F_ENCODE((uint32_t)s, 29, 3)
#define PHYMOD_SYMBOL_INDEX_FLAGS_GET(w) __F_GET(w, 29, 3)

#define PHYMOD_SYMBOL_INDEX_MIN_GET(w) \
    (PHYMOD_SYMBOL_INDEX_FLAGS_GET(w) ? \
       0 : \
       PHYMOD_SYMBOL_INDEX_MIN_GETRAW(w))

#define PHYMOD_SYMBOL_INDEX_ENC_GET(w) PHYMOD_SYMBOL_INDEX_MIN_GETRAW(w)

#define PHYMOD_SYMBOL_INDEX_MAX_GET(w) \
    ((PHYMOD_SYMBOL_INDEX_FLAGS_GET(w) & PHYMOD_SYMBOL_INDEX_F_EXP) ? \
       (PHYMOD_SYMBOL_INDEX_MIN_GETRAW(w) * (1U << PHYMOD_SYMBOL_INDEX_MAX_GETRAW(w))) - 1 : \
       PHYMOD_SYMBOL_INDEX_MAX_GETRAW(w))

#define PHYMOD_SYMBOL_INDEX_STEP_GET(w) \
    ((PHYMOD_SYMBOL_INDEX_FLAGS_GET(w) & PHYMOD_SYMBOL_INDEX_F_STEP) ? \
       (PHYMOD_SYMBOL_INDEX_MIN_GETRAW(w) + 1) : \
       1)


/*
 * Single Word Field Format
 *
 * Format:
 *
 * {LastField:31:31}{Ext:30:30}{FieldId:29:16}{Maxbit:15:8}{Minbit:7:0}
 *
 * Fields:
 *    LastField:1 Indicates this is the last field descriptor word in a field array
 *    Ext:1       Indicates that this is the first word in a Double Word Field, not
 *                a Single Word Field. This word and the next form a Double Word Field. 
 *    FieldId:14  This is the unique field id for this field. 
 *    Maxbit:8    This is the field's maxbit. 
 *    Minbit:8    This is the field's minbit. 
 *
 *
 *
 * Double Word Field Format
 *
 * Format:
 *
 *  {LastField:31:31}{Ext:30:30}{FieldId:29:0}
 *  {Maxbit:31:16}{Minbit:15:0}
 *
 * Fields:
 *     LastField:1 Indicates this is the last field descriptor in a field array. 
 *     Ext:1       Indicates this is the start of a Double Word field. 
 *     FieldId:30  This is the unique field id for this field. 
 *     Maxbit:16   This is the field's maxbit. 
 *     Minbit:16   This is the field's minbit. 
 */

#define PHYMOD_SYMBOL_FIELD_FLAG_LAST   (1U << 31)
#define PHYMOD_SYMBOL_FIELD_FLAG_EXT    (1U << 30)

#define PHYMOD_SYMBOL_FIELD_LAST(w) ((w) & PHYMOD_SYMBOL_FIELD_FLAG_LAST)
#define PHYMOD_SYMBOL_FIELD_EXT(w) ((w) & PHYMOD_SYMBOL_FIELD_FLAG_EXT)

/* Single Word Fields */
#define PHYMOD_SYMBOL_FIELD_ID_ENCODE(id) __F_ENCODE(id, 16, 14)
#define PHYMOD_SYMBOL_FIELD_MAX_ENCODE(b) __F_ENCODE(b, 8, 8)
#define PHYMOD_SYMBOL_FIELD_MIN_ENCODE(b) __F_ENCODE(b, 0, 8)

#define PHYMOD_SYMBOL_FIELD_ID_GET(w) __F_GET(w, 16, 14)
#define PHYMOD_SYMBOL_FIELD_MAX_GET(w) __F_GET(w, 8, 8)
#define PHYMOD_SYMBOL_FIELD_MIN_GET(w) __F_GET(w, 0, 8)

#define PHYMOD_SYMBOL_FIELD_ENCODE(id, max, min) \
PHYMOD_SYMBOL_FIELD_ID_ENCODE(id) | \
PHYMOD_SYMBOL_FIELD_MAX_ENCODE(max) | \
PHYMOD_SYMBOL_FIELD_MIN_ENCODE(min)

/* Double Word Fields */
#define PHYMOD_SYMBOL_FIELD_EXT_ID_ENCODE(id) __F_ENCODE(id, 0, 30)
#define PHYMOD_SYMBOL_FIELD_EXT_MAX_ENCODE(b) __F_ENCODE(b, 16, 16)
#define PHYMOD_SYMBOL_FIELD_EXT_MIN_ENCODE(b) __F_ENCODE(b, 0, 16)

#define PHYMOD_SYMBOL_FIELD_EXT_ENCODE(id, max, min) \
PHYMOD_SYMBOL_FIELD_FLAG_EXT | PHYMOD_SYMBOL_FIELD_EXT_ID_ENCODE(id), \
PHYMOD_SYMBOL_FIELD_EXT_MAX_ENCODE(max) | \
PHYMOD_SYMBOL_FIELD_EXT_MIN_ENCODE(min)


#define PHYMOD_SYMBOL_FIELD_EXT_ID_GET(w) __F_GET(w, 0, 30)
#define PHYMOD_SYMBOL_FIELD_EXT_MAX_GET(w) __F_GET(w, 16, 16)
#define PHYMOD_SYMBOL_FIELD_EXT_MIN_GET(w) __F_GET(w, 0, 16)

/* Operate on Field Descriptors */
extern uint32_t* phymod_field_info_decode(uint32_t* fp, 
                                       phymod_field_info_t* finfo, 
                                       const char** fnames);
extern uint32_t
phymod_field_info_count(uint32_t* fp);  

#define PHYMOD_SYMBOL_FIELDS_ITER_BEGIN(start, finfo, fnames)     \
{                                                              \
    uint32_t* __fp = start;                                    \
    for(;;) {                                                  \
      if(!__fp) {                                              \
        break;                                                 \
      }                                                        \
      else {                                                   \
        __fp = phymod_field_info_decode(__fp, &(finfo), (fnames));    

#define PHYMOD_SYMBOL_FIELDS_ITER_END() }}}
    

/*
 * Symbol Information Structure (single symbol)
 */
typedef struct phymod_symbol_s {

    
    uint32_t addr; 
#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1
    uint32_t* fields; 
#endif
    uint32_t index;     
    uint32_t flags; 
    const char *name; 
#if PHYMOD_CONFIG_INCLUDE_ALIAS_NAMES == 1
    const char *alias; 
    const char *ufname; 
#endif
#if PHYMOD_CONFIG_INCLUDE_RESET_VALUES == 1
    uint32_t resetval;
    uint32_t resetval_hi; /* Upper 32 bits of 64-bit register */
#endif

} phymod_symbol_t; 


/*
 * Symbols Information Structure (plural)
 */
typedef struct phymod_symbols_s {    
    
    const phymod_symbol_t *symbols; 
    const uint32_t size; 
    const char** field_names; 

} phymod_symbols_t; 


/*
 * Search a symbol array. Utility function. 
 */
extern const phymod_symbol_t *phymod_symbol_find(const char *name, 
                                           const phymod_symbol_t *table, 
                                           int size); 

/*
 * Primary symbol search function. Takes a pointer to the symbols structure, 
 * finds the name in either one, and returns a full symbol structure. 
 */

extern int phymod_symbols_find(const char *name, const phymod_symbols_t *symbols, 
                            phymod_symbol_t *rsym); 

/*
 * Get index of a specific symbol
 */
extern int phymod_symbols_index(const phymod_symbols_t *symbols,
                             const phymod_symbol_t *symbol);


/*
 * Get a specific symbol (by index)
 */
extern int phymod_symbols_get(const phymod_symbols_t *symbols, uint32_t sindex, 
                           phymod_symbol_t *rsym); 


/*
 * Search symbol table and call a user-defined function for each
 * matched symbol. Useful if multiple partial matches may occur.
 */

/* Matching modes */
#define PHYMOD_SYMBOLS_ITER_MODE_EXACT     0
#define PHYMOD_SYMBOLS_ITER_MODE_START     1
#define PHYMOD_SYMBOLS_ITER_MODE_STRSTR    2

typedef struct phymod_symbols_iter_s {

    const char* name;   /* String to match */

    int matching_mode;  /* Defines valid matches */

    uint32_t pflags;    /* Flags that must be present */
    uint32_t aflags;    /* Flags that must be absent */

    const phymod_symbols_t* symbols; 

    int (*function)(const phymod_symbol_t *sym, void *vptr);
    void* vptr; 

} phymod_symbols_iter_t; 

typedef int (*phymod_symbol_filter_cb_t)(const phymod_symbol_t *symbol,
                                      const char **field_names,
                                      const char *encoding,
                                      void *cookie);


extern int phymod_symbols_iter(phymod_symbols_iter_t *iter);

extern int phymod_symbol_field_filter(const phymod_symbol_t *symbol,
                                   const char **fnames,
                                   const char *encoding,
                                   void *cookie);

extern int phymod_symbol_show_fields(const phymod_symbol_t *symbol,
                                     const char **fnames, uint32_t *data, int nz,
                                     int (*print_str)(const char *str),
                                     phymod_symbol_filter_cb_t fcb, void *cookie);

extern int phymod_symbol_dump(const char *name, const phymod_symbols_t *symbols,
                              uint32_t *data,
                              int (*print_str)(const char *str));

extern uint32_t *phymod_field_get(const uint32_t *entbuf,
                                  int sbit, int ebit,
                                  uint32_t *fbuf);

extern int phymod_field_set(uint32_t *entbuf, int sbit, int ebit,
                            uint32_t *fbuf);

#endif /* __PHYMOD_SYMBOLS_H__ */
