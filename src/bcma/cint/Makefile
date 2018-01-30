#
# Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
# Broadcom Limited and/or its subsidiaries.
# 
# Broadcom Switch Software License
# 
# This license governs the use of the accompanying Broadcom software. Your 
# use of the software indicates your acceptance of the terms and conditions 
# of this license. If you do not agree to the terms and conditions of this 
# license, do not use the software.
# 1. Definitions
#    "Licensor" means any person or entity that distributes its Work.
#    "Software" means the original work of authorship made available under 
#    this license.
#    "Work" means the Software and any additions to or derivative works of 
#    the Software that are made available under this license.
#    The terms "reproduce," "reproduction," "derivative works," and 
#    "distribution" have the meaning as provided under U.S. copyright law.
#    Works, including the Software, are "made available" under this license 
#    by including in or with the Work either (a) a copyright notice 
#    referencing the applicability of this license to the Work, or (b) a copy 
#    of this license.
# 2. Grant of Copyright License
#    Subject to the terms and conditions of this license, each Licensor 
#    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
#    copyright license to reproduce, prepare derivative works of, publicly 
#    display, publicly perform, sublicense and distribute its Work and any 
#    resulting derivative works in any form.
# 3. Grant of Patent License
#    Subject to the terms and conditions of this license, each Licensor 
#    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
#    patent license to make, have made, use, offer to sell, sell, import, and 
#    otherwise transfer its Work, in whole or in part. This patent license 
#    applies only to the patent claims licensable by Licensor that would be 
#    infringed by Licensor's Work (or portion thereof) individually and 
#    excluding any combinations with any other materials or technology.
#    If you institute patent litigation against any Licensor (including a 
#    cross-claim or counterclaim in a lawsuit) to enforce any patents that 
#    you allege are infringed by any Work, then your patent license from such 
#    Licensor to the Work shall terminate as of the date such litigation is 
#    filed.
# 4. Redistribution
#    You may reproduce or distribute the Work only if (a) you do so under 
#    this License, (b) you include a complete copy of this License with your 
#    distribution, and (c) you retain without modification any copyright, 
#    patent, trademark, or attribution notices that are present in the Work.
# 5. Derivative Works
#    You may specify that additional or different terms apply to the use, 
#    reproduction, and distribution of your derivative works of the Work 
#    ("Your Terms") only if (a) Your Terms provide that the limitations of 
#    Section 7 apply to your derivative works, and (b) you identify the 
#    specific derivative works that are subject to Your Terms. 
#    Notwithstanding Your Terms, this license (including the redistribution 
#    requirements in Section 4) will continue to apply to the Work itself.
# 6. Trademarks
#    This license does not grant any rights to use any Licensor's or its 
#    affiliates' names, logos, or trademarks, except as necessary to 
#    reproduce the notices described in this license.
# 7. Limitations
#    Platform. The Work and any derivative works thereof may only be used, or 
#    intended for use, with a Broadcom switch integrated circuit.
#    No Reverse Engineering. You will not use the Work to disassemble, 
#    reverse engineer, decompile, or attempt to ascertain the underlying 
#    technology of a Broadcom switch integrated circuit.
# 8. Termination
#    If you violate any term of this license, then your rights under this 
#    license (including the license grants of Sections 2 and 3) will 
#    terminate immediately.
# 9. Disclaimer of Warranty
#    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
#    KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
#    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
#    NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
#    THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
#    IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
# 10. Limitation of Liability
#    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
#    THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
#    SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
#    INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
#    OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
#    (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
#    LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
#    COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
#    THE POSSIBILITY OF SUCH DAMAGES.
# 
# 
#

ifdef BCMA_CINT_ENABLE
# Build CINT as SDKLT LIB

CINT_CPPFLAGS += -DCINT_CONFIG_INCLUDE_SDKLT=1
CINT_CPPFLAGS += -DCINT_CONFIG_INCLUDE_PARSER=1
CINT_CPPFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
TMP_SDK_CPPFLAGS := $(filter-out $(CINT_CPPFLAGS),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(CINT_CPPFLAGS)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)

include $(SDK)/make/sublib.mk

else
# Build standalone interpreter application

CC := /tools/bin/gcc
PERL := perl
FLEX := /projects/ntsw-tools/bin/flex-2.5.31
YACC := /tools/bin/yacc
PATCH := patch
REPLACESTRING := /projects/ntsw-tools/bin/replacestring
YEAR := $(shell date +%Y)
TMP := tmp.h
CORE_DIR := core/

SRCS := cint_ast.c
SRCS += cint_ast_debug.c
SRCS += cint_builtins.c
SRCS += cint_datatypes.c
SRCS += cint_debug.c
SRCS += cint_error.c
SRCS += cint_eval_ast_cint.c
SRCS += cint_eval_ast_print.c
SRCS += cint_eval_asts.c
SRCS += cint_internal.c
SRCS += cint_interpreter.c
SRCS += cint_operators.c
SRCS += cint_parser.c
SRCS += cint_test_data.c
SRCS += cint_variables.c
SRCS += main.c
SRCS += cint_stubs.c
# generated
SRCS += cint_c.lex.c
SRCS += cint_c.tab.c

CFLAGS := -g -O0

# portability interfaces - pick one
CFLAGS += -DCINT_CONFIG_INCLUDE_STDLIB=1
CFLAGS += -DCINT_CONFIG_INCLUDE_SDK_SAL=0
CFLAGS += -DCINT_CONFIG_INCLUDE_SDKLT=0
CFLAGS += -DCINT_CONFIG_INCLUDE_STUBS=0

# test features
CFLAGS += -DCINT_CONFIG_INCLUDE_LOAD=1
CFLAGS += -DCINT_CONFIG_INCLUDE_MAIN=1
CFLAGS += -DCINT_CONFIG_INCLUDE_TEST_DATA=1

# FILE IO and READLINE
#CFLAGS += -DCINT_CONFIG_FILE_IO=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_XINCLUDE=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_READLINE=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY=0

# other configs
#CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DOUBLES=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_LONGLONGS=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DTRACE=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DEBUG=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_POSIX_TIMER=0
#CFLAGS += -DCINT_CONFIG_YAPI=1

# unused configs
#CFLAGS += -DCINT_CONFIG_PARSER_STDIN=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_SOURCE=1

CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -DYYDEBUG

OBJS := $(SRCS:%.c=$(CORE_DIR)%.o)

DEPS := $(SRCS:%.c=$(CORE_DIR)%.d)

all: gen cint

gen: cint_c.lex.c cint_c.tab.c cint_wrappers.h

cint: $(OBJS)
	$(CC) -o $@ $+ -ldl -lrt -lreadline -lcurses

cint_c.tab.c: $(CORE_DIR)cint_grammar.y
	$(YACC) -dv -o $(CORE_DIR)$@ $<
# Avoid compiler warnings
	$(REPLACESTRING) -a "YYSTYPE yylval;" "YYSTYPE yylval = 0;" $(CORE_DIR)$@
	$(REPLACESTRING) -a "YYLTYPE yylloc;" "YYLTYPE yylloc = {0,0,0,0};" $(CORE_DIR)$@
# use CINT bindings via BISON interfaces
	$(REPLACESTRING) -a "yyps = (yypstate *) malloc (sizeof *yyps)" \
		"yyps = (yypstate *) YYMALLOC (sizeof *yyps)" $(CORE_DIR)$@
	$(REPLACESTRING) -a "free (yyps)" "YYFREE (yyps)" $(CORE_DIR)$@
# use standard BRCM id/copyright
	@echo '/* ' > $(TMP)
	@echo -n '   $$' >> $(TMP)
	@echo -n 'Id: ' >> $(TMP)
	@echo '$$' >> $(TMP)
	@echo '   $$Copyright: $(YEAR) Broadcom Corp.' >> $(TMP)
	@echo '   All Rights Reserved.$$' >> $(TMP)
	@echo '*/' >> $(TMP)
	@cat $(CORE_DIR)cint_c.tab.h >> $(TMP)
	@rm $(CORE_DIR)cint_c.tab.h
	@mv tmp.h $(CORE_DIR)cint_c.tab.h

cint_c.lex.c: $(CORE_DIR)cint_scanner.l $(CORE_DIR)lex.patch
	$(FLEX) --nounistd -o $(CORE_DIR)$@ $<
	$(PATCH) -p1 $(CORE_DIR)$@ < $(CORE_DIR)lex.patch

cint_wrappers.h: $(CORE_DIR)wrappergen.pl
	$(PERL) $< > $(CORE_DIR)$@

utest: all utest-a.h utest-b.h
	rm -f *.code *.actual *.expected
	$(PERL) $(CORE_DIR)utest.pl $(CORE_DIR)utests

utest-a.h:
	echo "print 1;" > $@

utest-b.h:
	echo "#include <utest-a.h>" > $@
	echo "print 121;" >> $@
	echo "#include <utest-a.h>" >> $@

clean:
	-rm -f cint $(OBJS) $(DEPS) $(CORE_DIR)cint_wrappers.h $(CORE_DIR)cint_c.*.[ch] $(CORE_DIR)cint_c.output utest-*.h

ifneq ($(MAKECMDGOALS),clean)
    include $(DEPS)
endif

# calculate C include dependencies
%.d: %.c
	@$(SHELL) -ec "$(CC) -M -MG $(CFLAGS) $< > $@; [ -s $@ ] || rm -f $@"

endif
