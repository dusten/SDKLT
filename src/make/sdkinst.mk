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
# Build consolidated static and shared libraries from individual
# component libraries.
#
# Output will be placed in the directory $(SDK_DSTDIR).
#
# Note that this make file will work only on Linux systems.
#

# Library names
LIBNAME = libsdklt
SDK_STATIC_LIB = $(LIBNAME).a
SDK_SHARED_LIB = $(LIBNAME).so

# Default build directory
ifndef SDK_BLDDIR
SDK_BLDDIR = $(SDK)/build
endif

# Temporary MRI file for building single static library
MRI = $(SDK_BLDDIR)/sdklt.mri

# Temporary directory for building shared library
TMPDIR = $(SDK_BLDDIR)/$(LIBNAME).tmp

# The Linux-specific libraries are organized differently than the SDK
# core libraries, so we handle them separately.
SDK_LINUX_TARGETS = bcmlu
SDK_INCS = $(filter-out $(SDK_LINUX_TARGETS),$(SDK_TARGETS))
LINUX_INCS = $(SDK_LINUX_TARGETS)

# Default installation base directory
ifndef SDK_DSTDIR
SDK_DSTDIR = $(SDK_BLDDIR)
endif

# Installation directories for libraries and header files.
SDK_LIBDIR = $(SDK_DSTDIR)/lib
SDK_INCDIR = $(SDK_DSTDIR)/include/sdklt

LIBSTATIC = $(SDK_LIBDIR)/$(SDK_STATIC_LIB)
LIBSHARED = $(SDK_LIBDIR)/$(SDK_SHARED_LIB)
LIBHEADER = $(SDK_INCDIR)/.sdklt

# Need to build sdklibs first for proper parallel build support
all:
	$(MAKE) sdklibs
	$(MAKE) install

install: $(LIBSTATIC) $(LIBSHARED) $(LIBHEADER)

# Make rules to build library collection (defines SDK_BLIBS)
include $(SDK)/make/sdklibs.mk

# Build static library from individual component libraries
$(LIBSTATIC): $(SDK_BLIBS)
	$(Q)echo "Building static library $(SDK_STATIC_LIB) ..."
	$(Q)(echo create $(SDK_STATIC_LIB); \
	     for l in $(SDK_LIBS); do \
	         echo addlib $$l; \
	     done; \
	     echo save; \
	     echo end) > $(MRI)
	$(Q)(cd $(SDK_BLDDIR); \
	     rm -f $(SDK_STATIC_LIB); \
	     $(AR) -M < $(MRI); \
	     rm -f $(MRI))
	$(Q)mkdir -p $(SDK_LIBDIR)
	$(Q)mv $(SDK_BLDDIR)/$(SDK_STATIC_LIB) $@

$(LIBSHARED): $(LIBSTATIC)
ifeq (,$(findstring fPIC,$(SDK_CFLAGS)))
	$(Q)echo "Skip building shared library (no -fPIC option in CFLAGS)"
else
	$(Q)echo "Building shared library $(SDK_SHARED_LIB) ..."
	$(Q)mkdir -p $(TMPDIR)
	$(Q)(cd $(TMPDIR); \
	     ar -x $(SDK_LIBDIR)/$(SDK_STATIC_LIB); \
	     $(CC) -shared -o $(SDK_SHARED_LIB) *.o; \
	     mv $(TMPDIR)/$(SDK_SHARED_LIB) $@; \
	     rm -rf $(TMPDIR))
endif

$(LIBHEADER): $(SDK_BLIBS)
	$(Q)echo "Building include directory ..."
	$(Q)rm -rf $(SDK_INCDIR)
	$(Q)mkdir -p $(SDK_INCDIR)
	$(Q)for l in $(SDK_INCS); do \
	        cp -dR $(SDK)/$$l/include/* $(SDK_INCDIR); \
	    done;
	$(Q)for l in $(LINUX_INCS); do \
	        cp -dR $(SDK)/linux/$$l/include/* $(SDK_INCDIR); \
	    done;
	$(Q)touch $@

clean::
	$(Q)rm -rf $(SDK_INCDIR)
	$(Q)rm -rf $(SDK_LIBDIR)

.PHONY: all install clean
