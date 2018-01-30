/*
   $Id: $
   Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
   Broadcom Limited and/or its subsidiaries.
   
   Broadcom Switch Software License
   
   This license governs the use of the accompanying Broadcom software. Your 
   use of the software indicates your acceptance of the terms and conditions 
   of this license. If you do not agree to the terms and conditions of this 
   license, do not use the software.
   1. Definitions
      "Licensor" means any person or entity that distributes its Work.
      "Software" means the original work of authorship made available under 
      this license.
      "Work" means the Software and any additions to or derivative works of 
      the Software that are made available under this license.
      The terms "reproduce," "reproduction," "derivative works," and 
      "distribution" have the meaning as provided under U.S. copyright law.
      Works, including the Software, are "made available" under this license 
      by including in or with the Work either (a) a copyright notice 
      referencing the applicability of this license to the Work, or (b) a copy 
      of this license.
   2. Grant of Copyright License
      Subject to the terms and conditions of this license, each Licensor 
      grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
      copyright license to reproduce, prepare derivative works of, publicly 
      display, publicly perform, sublicense and distribute its Work and any 
      resulting derivative works in any form.
   3. Grant of Patent License
      Subject to the terms and conditions of this license, each Licensor 
      grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
      patent license to make, have made, use, offer to sell, sell, import, and 
      otherwise transfer its Work, in whole or in part. This patent license 
      applies only to the patent claims licensable by Licensor that would be 
      infringed by Licensor's Work (or portion thereof) individually and 
      excluding any combinations with any other materials or technology.
      If you institute patent litigation against any Licensor (including a 
      cross-claim or counterclaim in a lawsuit) to enforce any patents that 
      you allege are infringed by any Work, then your patent license from such 
      Licensor to the Work shall terminate as of the date such litigation is 
      filed.
   4. Redistribution
      You may reproduce or distribute the Work only if (a) you do so under 
      this License, (b) you include a complete copy of this License with your 
      distribution, and (c) you retain without modification any copyright, 
      patent, trademark, or attribution notices that are present in the Work.
   5. Derivative Works
      You may specify that additional or different terms apply to the use, 
      reproduction, and distribution of your derivative works of the Work 
      ("Your Terms") only if (a) Your Terms provide that the limitations of 
      Section 7 apply to your derivative works, and (b) you identify the 
      specific derivative works that are subject to Your Terms. 
      Notwithstanding Your Terms, this license (including the redistribution 
      requirements in Section 4) will continue to apply to the Work itself.
   6. Trademarks
      This license does not grant any rights to use any Licensor's or its 
      affiliates' names, logos, or trademarks, except as necessary to 
      reproduce the notices described in this license.
   7. Limitations
      Platform. The Work and any derivative works thereof may only be used, or 
      intended for use, with a Broadcom switch integrated circuit.
      No Reverse Engineering. You will not use the Work to disassemble, 
      reverse engineer, decompile, or attempt to ascertain the underlying 
      technology of a Broadcom switch integrated circuit.
   8. Termination
      If you violate any term of this license, then your rights under this 
      license (including the license grants of Sections 2 and 3) will 
      terminate immediately.
   9. Disclaimer of Warranty
      THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
      KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
      NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
      THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
      IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
   10. Limitation of Liability
      EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
      THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
      SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
      INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
      OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
      (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
      LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
      COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
      THE POSSIBILITY OF SUCH DAMAGES.
   
   
*/
/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     XEOF = 0,
     IDENTIFIER = 258,
     CONSTANT = 259,
     STRING_LITERAL = 260,
     SIZEOF = 261,
     PTR_OP = 262,
     INC_OP = 263,
     DEC_OP = 264,
     LEFT_OP = 265,
     RIGHT_OP = 266,
     LE_OP = 267,
     GE_OP = 268,
     EQ_OP = 269,
     NE_OP = 270,
     AND_OP = 271,
     OR_OP = 272,
     MUL_ASSIGN = 273,
     DIV_ASSIGN = 274,
     MOD_ASSIGN = 275,
     ADD_ASSIGN = 276,
     SUB_ASSIGN = 277,
     LEFT_ASSIGN = 278,
     RIGHT_ASSIGN = 279,
     AND_ASSIGN = 280,
     XOR_ASSIGN = 281,
     OR_ASSIGN = 282,
     TYPE_NAME = 283,
     TYPEDEF = 284,
     EXTERN = 285,
     STATIC = 286,
     AUTO = 287,
     REGISTER = 288,
     CHAR = 289,
     SHORT = 290,
     INT = 291,
     LONG = 292,
     SIGNED = 293,
     UNSIGNED = 294,
     FLOAT = 295,
     DOUBLE = 296,
     CONST = 297,
     VOLATILE = 298,
     T_VOID = 299,
     STRUCT = 300,
     UNION = 301,
     ENUM = 302,
     ELLIPSIS = 303,
     CASE = 304,
     DEFAULT = 305,
     IF = 306,
     ELSE = 307,
     SWITCH = 308,
     WHILE = 309,
     DO = 310,
     FOR = 311,
     GOTO = 312,
     CONTINUE = 313,
     BREAK = 314,
     RETURN = 315,
     PRINT = 316,
     CINT = 317,
     ITERATOR = 318,
     MACRO = 319
   };
#endif
/* Tokens.  */
#define XEOF 0
#define IDENTIFIER 258
#define CONSTANT 259
#define STRING_LITERAL 260
#define SIZEOF 261
#define PTR_OP 262
#define INC_OP 263
#define DEC_OP 264
#define LEFT_OP 265
#define RIGHT_OP 266
#define LE_OP 267
#define GE_OP 268
#define EQ_OP 269
#define NE_OP 270
#define AND_OP 271
#define OR_OP 272
#define MUL_ASSIGN 273
#define DIV_ASSIGN 274
#define MOD_ASSIGN 275
#define ADD_ASSIGN 276
#define SUB_ASSIGN 277
#define LEFT_ASSIGN 278
#define RIGHT_ASSIGN 279
#define AND_ASSIGN 280
#define XOR_ASSIGN 281
#define OR_ASSIGN 282
#define TYPE_NAME 283
#define TYPEDEF 284
#define EXTERN 285
#define STATIC 286
#define AUTO 287
#define REGISTER 288
#define CHAR 289
#define SHORT 290
#define INT 291
#define LONG 292
#define SIGNED 293
#define UNSIGNED 294
#define FLOAT 295
#define DOUBLE 296
#define CONST 297
#define VOLATILE 298
#define T_VOID 299
#define STRUCT 300
#define UNION 301
#define ENUM 302
#define ELLIPSIS 303
#define CASE 304
#define DEFAULT 305
#define IF 306
#define ELSE 307
#define SWITCH 308
#define WHILE 309
#define DO 310
#define FOR 311
#define GOTO 312
#define CONTINUE 313
#define BREAK 314
#define RETURN 315
#define PRINT 316
#define CINT 317
#define ITERATOR 318
#define MACRO 319




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



#ifndef YYPUSH_DECLS
#  define YYPUSH_DECLS
struct cint_c_pstate;
typedef struct cint_c_pstate cint_c_pstate;
enum { YYPUSH_MORE = 4 };
#if defined __STDC__ || defined __cplusplus
int cint_c_parse (yyscan_t yyscanner, cint_cparser_t* cparser);
#else
int cint_c_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
int cint_c_push_parse (cint_c_pstate *yyps, int yypushed_char, YYSTYPE const *yypushed_val, YYLTYPE const *yypushed_loc, yyscan_t yyscanner, cint_cparser_t* cparser);
#else
int cint_c_push_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
int cint_c_pull_parse (cint_c_pstate *yyps, yyscan_t yyscanner, cint_cparser_t* cparser);
#else
int cint_c_pull_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
cint_c_pstate * cint_c_pstate_new (void);
#else
cint_c_pstate * cint_c_pstate_new ();
#endif
#if defined __STDC__ || defined __cplusplus
void cint_c_pstate_delete (cint_c_pstate *yyps);
#else
void cint_c_pstate_delete ();
#endif
#endif

