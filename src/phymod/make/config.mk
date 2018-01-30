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
# PHYMOD make rules and definitions
#

#
# Provide reasonable defaults for configuration variables
#

# Default build directory
ifndef PHYMOD_BLDDIR
PHYMOD_BLDDIR = $(PHYMOD)/build
endif

# Location to build objects in
ifndef PHYMOD_OBJDIR
PHYMOD_OBJDIR = $(PHYMOD_BLDDIR)/obj
endif
override BLDDIR := $(PHYMOD_OBJDIR)

# Location to place libraries
ifndef PHYMOD_LIBDIR
PHYMOD_LIBDIR = $(PHYMOD_BLDDIR)
endif
LIBDIR := $(PHYMOD_LIBDIR)

# Option to retrieve compiler version
ifndef PHYMOD_CC_VERFLAGS
PHYMOD_CC_VERFLAGS := -v
endif
CC_VERFLAGS = $(PHYMOD_CC_VERFLAGS); 

# Default suffix for object files
ifndef PHYMOD_OBJSUFFIX
PHYMOD_OBJSUFFIX = o
endif
OBJSUFFIX = $(PHYMOD_OBJSUFFIX)

# Default suffix for library files
ifndef PHYMOD_LIBSUFFIX
PHYMOD_LIBSUFFIX = a
endif
LIBSUFFIX = $(PHYMOD_LIBSUFFIX)

#
# Set up compiler options, etc.
#

# Default include path
PHYMOD_INCLUDE_PATH = -I$(PHYMOD)/include
PHYMOD_INCLUDE_PATH += -I$(PHYMOD)/chip/

# Import preprocessor flags avoiding include duplicates
TMP_PHYMOD_CPPFLAGS := $(filter-out $(PHYMOD_INCLUDE_PATH),$(PHYMOD_CPPFLAGS))

# Convenience Makefile flags for building specific chips
ifdef PHYMOD_CHIPS
PHYMOD_DSYM_CPPFLAGS := -DPHYMOD_CONFIG_INCLUDE_CHIP_DEFAULT=0 
PHYMOD_DSYM_CPPFLAGS += $(foreach chip,$(PHYMOD_CHIPS),-DPHYMOD_CONFIG_INCLUDE_${chip}=1) 
endif
ifdef PHYMOD_NCHIPS
PHYMOD_DSYM_CPPFLAGS += $(foreach chip,$(PHYMOD_NCHIPS),-DPHYMOD_CONFIG_INCLUDE_${chip}=0)
endif

TMP_PHYMOD_CPPFLAGS += $(PHYMOD_DSYM_CPPFLAGS)
export PHYMOD_DSYM_CPPFLAGS

ifdef DSYMS
TMP_PHYMOD_CPPFLAGS += -DPHYMOD_CONFIG_CHIP_SYMBOLS_USE_DSYMS=1
endif

override CPPFLAGS = $(TMP_PHYMOD_CPPFLAGS) $(PHYMOD_INCLUDE_PATH)


# Import compiler flags
override CFLAGS = $(PHYMOD_CFLAGS)




#
# Define standard targets, etc.
#

ifdef LOCALDIR
override BLDDIR := $(BLDDIR)/$(LOCALDIR)
endif

ifndef LSRCS
LSRCS = $(wildcard *.c)
endif
ifndef LOBJS
LOBJS = $(addsuffix .$(OBJSUFFIX), $(basename $(LSRCS)))
endif
ifndef BOBJS
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))
endif

# Use PHYMOD_QUIET=1 to control printing of compilation lines.
ifdef PHYMOD_QUIET
Q = @
endif

#
# Define rules for creating object directories
#

.PRECIOUS: $(BLDDIR)/.tree

%/.tree:
	@$(ECHO) 'Creating build directory $(dir $@)'
	$(Q)$(MKDIR) $(dir $@)
	@$(ECHO) "Build Directory for $(LOCALDIR) created" > $@

#
# Configure build tools
#
include $(PHYMOD)/make/maketools.mk
