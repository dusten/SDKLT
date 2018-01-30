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
# Build shared ISSU library.
#
# Objects will be placed in the directory $(SDK_DSTDIR).
# Library will be placed in the directory $(ISSU_LIBDIR).
#
# Note that this make file will work only on Linux systems.
#

# Library names
ISSU_LIBNAME = libsdkissu
ISSU_STATIC_LIB = $(ISSU_LIBNAME).a
ISSU_SHARED_LIB = $(ISSU_LIBNAME).so

# Default build directory
ifndef SDK_BLDDIR
SDK_BLDDIR = $(SDK)/build
endif

# Temporary MRI file for building single static library
ISSU_MRI = $(SDK_BLDDIR)/sdkissu.mri

# Temporary directory for building shared library
ISSU_TMPDIR = $(SDK_BLDDIR)/$(ISSU_LIBNAME).tmp

# Default installation base directory
ifndef SDK_DSTDIR
SDK_DSTDIR = $(SDK_BLDDIR)
endif

# Installation directory for library.
ISSU_LIBDIR = $(SDK_DSTDIR)/lib

ISSU_LIBSTATIC = $(ISSU_LIBDIR)/$(ISSU_STATIC_LIB)
ISSU_LIBSHARED = $(ISSU_LIBDIR)/$(ISSU_SHARED_LIB)

ifndef BCMISSU
include $(SDK)/make/libpath.mk
endif

# Need to build issulibs first for proper parallel build support
issu:
	@echo Building ISSU - $(BCMISSU)
	$(MAKE) bcmissu
	$(MAKE) issuinst

# BCMISSU_LIBS is defined in $(BCMISSU)/targets.mk
include $(BCMISSU)/targets.mk

bcmissu:
	$(MAKE) -C $(BCMISSU) $(SDK_CLEAN)

# Targets with full build path
ifdef SDK_BLDDIR
ISSU_BLIBS = $(addprefix $(SDK_BLDDIR)/,$(BCMISSU_LIBS))
endif

issuinst: $(ISSU_LIBSTATIC) $(ISSU_LIBSHARED)

# Build static library from individual component libraries
$(ISSU_LIBSTATIC): $(ISSU_BLIBS)
	$(Q)echo "Building static library $(ISSU_STATIC_LIB) ..."
	$(Q)(echo create $(ISSU_STATIC_LIB); \
	     for l in $(BCMISSU_LIBS); do \
	         echo addlib $$l; \
	     done; \
	     echo save; \
	     echo end) > $(ISSU_MRI)
	$(Q)(cd $(SDK_BLDDIR); \
	     rm -f $(ISSU_STATIC_LIB); \
	     $(AR) -M < $(ISSU_MRI); \
	     rm -f $(ISSU_MRI))
	$(Q)mkdir -p $(ISSU_LIBDIR)
	$(Q)mv $(SDK_BLDDIR)/$(ISSU_STATIC_LIB) $@

$(ISSU_LIBSHARED): $(ISSU_LIBSTATIC)
ifeq (,$(findstring fPIC,$(SDK_CFLAGS)))
	$(Q)echo "Skip building shared ISSU library (no -fPIC option in CFLAGS)"
else
	$(Q)echo "Building shared ISSU library $(ISSU_SHARED_LIB) ..."
	$(Q)mkdir -p $(ISSU_TMPDIR)
	$(Q)(cd $(ISSU_TMPDIR); \
	     ar -x $(ISSU_LIBDIR)/$(ISSU_STATIC_LIB); \
	     $(CC) -shared -o $(ISSU_SHARED_LIB) *.o; \
	     mv $(ISSU_TMPDIR)/$(ISSU_SHARED_LIB) $@; \
	     rm -rf $(ISSU_TMPDIR))
endif

.PHONY: issu bcmissu issuinst
