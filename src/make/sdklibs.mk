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
# Default SDK build targets.
#
# This file should be included in the main application makefile to
# allow building the sdklibs target.
#
# By default all SDK libraries are built, but this can be overridden
# using the SDK_TARGETS variable, for example to only build the SAL
# and BSL libraries:
#
# all: sdklibs
# SDK_TARGETS = sal bsl
# include $(SDK)/make/sdklibs.mk
#

# Basic make tools
include $(SDK)/make/maketools.mk

# Targets within this file should never become automatic targets
sdklibs_help:
	@$(ECHO) "Your makefile targets must precede this file"

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

#
# To avoid confusion, the list below should be in the same order as
# the libraries are listed in the following sections (i.e. "Configure
# XXX library").
#
SDK_DEFAULT_TARGETS = \
	bcma \
	bcmmgmt \
	bcmlu \
	bcmpkt \
	bcmcnet \
	bcmlt \
	bcmltp \
	bcmtrm \
	bcmltm \
	bcmlta \
	bcmltx \
	bcmfp \
	bcmport \
	bcmmeter \
	bcmmirror \
	bcmvlan \
	bcml2 \
	bcmecmp \
	bcmecn \
	bcmtrunk \
	bcmptm \
	bcmlm \
	bcmpc \
	bcmbd \
	bcmtm \
	bcmevm \
	bcmimm \
	bcmlrd \
	bcmcfg \
	bcmtnl \
	bcmpmac \
	phymod \
	bcmltd \
	bcmdrd \
	sim \
	shr \
	sal \
	bsl

ifeq (,$(SDK_TARGETS))
SDK_TARGETS = $(SDK_DEFAULT_TARGETS)
endif

ifneq (,$(SDK_EXCLUDE_TARGETS))
SDK_TARGETS := $(filter-out $(SDK_EXCLUDE_TARGETS),$(SDK_TARGETS))
endif

sdklibs: $(SDK_TARGETS)

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
SDK_CLEAN=clean
endif

clean:: sdklibs

.PHONY: sdklibs clean $(SDK_TARGETS)

#
# IMPORTANT:
# The following sections must be kept in (link) dependency order.
#

# Configure BCMA library
ifneq (,$(filter bcma,$(SDK_TARGETS)))
# BCMA_LIBS is defined in $(BCMA)/targets.mk
include $(BCMA)/targets.mk
SDK_LIBS += $(BCMA_LIBS)
SDK_INCLUDES += -I$(BCMA)/include
bcma:
	$(MAKE) -C $(BCMA) $(SDK_CLEAN)
endif

# Configure BCMMGMT library
ifneq (,$(filter bcmmgmt,$(SDK_TARGETS)))
# BCMMGMT_LIBS is defined in $(BCMMGMT)/targets.mk
include $(BCMMGMT)/targets.mk
SDK_LIBS += $(BCMMGMT_LIBS)
SDK_INCLUDES += -I$(BCMMGMT)/include
bcmmgmt:
	$(MAKE) -C $(BCMMGMT) $(SDK_CLEAN)
endif

# Configure BCMLU library
ifneq (,$(filter bcmlu,$(SDK_TARGETS)))
# BCMLU_LIBS is defined in $(BCMLU)/targets.mk
include $(BCMLU)/targets.mk
SDK_LIBS += $(BCMLU_LIBS)
SDK_INCLUDES += -I$(BCMLU)/include
bcmlu:
	$(MAKE) -C $(BCMLU) $(SDK_CLEAN)
endif

# Configure BCMPKT library
ifneq (,$(filter bcmpkt,$(SDK_TARGETS)))
# BCMPKT_LIBS is defined in $(BCMPKT)/targets.mk
include $(BCMPKT)/targets.mk
SDK_LIBS += $(BCMPKT_LIBS)
SDK_INCLUDES += -I$(BCMPKT)/include
bcmpkt:
	$(MAKE) -C $(BCMPKT) $(SDK_CLEAN)
endif

# Configure BCMCNET library
ifneq (,$(filter bcmcnet,$(SDK_TARGETS)))
# BCMCNET_LIBS is defined in $(BCMCNET)/targets.mk
include $(BCMCNET)/targets.mk
SDK_LIBS += $(BCMCNET_LIBS)
SDK_INCLUDES += -I$(BCMCNET)/include
bcmcnet:
	$(MAKE) -C $(BCMCNET) $(SDK_CLEAN)
endif

# Configure BCMLT library
ifneq (,$(filter bcmlt,$(SDK_TARGETS)))
# BCMLT_LIBS is defined in $(BCMLT)/targets.mk
include $(BCMLT)/targets.mk

SDK_LIBS += $(BCMLT_LIBS)
SDK_INCLUDES += -I$(BCMLT)/include
bcmlt:
	$(MAKE) -C $(BCMLT) $(SDK_CLEAN)
endif

# Configure BCMLTP library
ifneq (,$(filter bcmltp,$(SDK_TARGETS)))
# BCMLTP_LIBS is defined in $(BCMLTP)/targets.mk
include $(BCMLTP)/targets.mk
SDK_LIBS += $(BCMLTP_LIBS)
SDK_INCLUDES += -I$(BCMLTP)/include
bcmltp:
	$(MAKE) -C $(BCMLTP) $(SDK_CLEAN)
endif

# Configure BCMTRM library
ifneq (,$(filter bcmtrm,$(SDK_TARGETS)))
# BCMTRM_LIBS is defined in $(BCMTRM)/targets.mk
include $(BCMTRM)/targets.mk
SDK_LIBS += $(BCMTRM_LIBS)
SDK_INCLUDES += -I$(BCMTRM)/include
bcmtrm:
	$(MAKE) -C $(BCMTRM) $(SDK_CLEAN)
endif

# Configure BCMLTM library
ifneq (,$(filter bcmltm,$(SDK_TARGETS)))
# BCMLTM_LIBS is defined in $(BCMLTM)/targets.mk
include $(BCMLTM)/targets.mk
SDK_LIBS += $(BCMLTM_LIBS)
SDK_INCLUDES += -I$(BCMLTM)/include
bcmltm:
	$(MAKE) -C $(BCMLTM) $(SDK_CLEAN)
endif

# Configure BCMLTA library
ifneq (,$(filter bcmlta,$(SDK_TARGETS)))
# BCMLTA_LIBS is defined in $(BCMLTA)/targets.mk
include $(BCMLTA)/targets.mk
SDK_LIBS += $(BCMLTA_LIBS)
SDK_INCLUDES += -I$(BCMLTA)/include
bcmlta:
	$(MAKE) -C $(BCMLTA) $(SDK_CLEAN)
endif

# Configure BCMLTX library
ifneq (,$(filter bcmltx,$(SDK_TARGETS)))
# BCMLTX_LIBS is defined in $(BCMLTX)/targets.mk
include $(BCMLTX)/targets.mk
SDK_LIBS += $(BCMLTX_LIBS)
SDK_INCLUDES += -I$(BCMLTX)/include
bcmltx:
	$(MAKE) -C $(BCMLTX) $(SDK_CLEAN)
endif

# Configure BCMFP library
ifneq (,$(filter bcmfp,$(SDK_TARGETS)))
# BCMFP_LIBS is defined in $(BCMFP)/targets.mk
include $(BCMFP)/targets.mk
SDK_LIBS += $(BCMFP_LIBS)
SDK_INCLUDES += -I$(BCMFP)/include
bcmfp:
	$(MAKE) -C $(BCMFP) $(SDK_CLEAN)
endif

# Configure BCMPORT library
ifneq (,$(filter bcmport,$(SDK_TARGETS)))
# BCMPORT_LIBS is defined in $(BCMPORT)/targets.mk
include $(BCMPORT)/targets.mk
SDK_LIBS += $(BCMPORT_LIBS)
SDK_INCLUDES += -I$(BCMPORT)/include
bcmport:
	$(MAKE) -C $(BCMPORT) $(SDK_CLEAN)
endif

# Configure BCMMETER library
ifneq (,$(filter bcmmeter,$(SDK_TARGETS)))
# BCMMETER_LIBS is defined in $(BCMMETER)/targets.mk
include $(BCMMETER)/targets.mk
SDK_LIBS += $(BCMMETER_LIBS)
SDK_INCLUDES += -I$(BCMMETER)/include
bcmmeter:
	$(MAKE) -C $(BCMMETER) $(SDK_CLEAN)
endif

# Configure BCMMIRROR library
ifneq (,$(filter bcmmirror,$(SDK_TARGETS)))
# BCMMIRROR_LIBS is defined in $(BCMMIRROR)/targets.mk
include $(BCMMIRROR)/targets.mk
SDK_LIBS += $(BCMMIRROR_LIBS)
SDK_INCLUDES += -I$(BCMMIRROR)/include
bcmmirror:
	$(MAKE) -C $(BCMMIRROR) $(SDK_CLEAN)
endif

# Configure BCMVLAN library
ifneq (,$(filter bcmvlan,$(SDK_TARGETS)))
# BCMVLAN_LIBS is defined in $(BCMVLAN)/targets.mk
include $(BCMVLAN)/targets.mk
SDK_LIBS += $(BCMVLAN_LIBS)
SDK_INCLUDES += -I$(BCMVLAN)/include
bcmvlan:
	$(MAKE) -C $(BCMVLAN) $(SDK_CLEAN)
endif

# Configure BCML2 library
ifneq (,$(filter bcml2,$(SDK_TARGETS)))
# BCML2_LIBS is defined in $(BCML2)/targets.mk
include $(BCML2)/targets.mk
SDK_LIBS += $(BCML2_LIBS)
SDK_INCLUDES += -I$(BCML2)/include
bcml2:
	$(MAKE) -C $(BCML2) $(SDK_CLEAN)
endif

# Configure BCMECMP library
ifneq (,$(filter bcmecmp,$(SDK_TARGETS)))
# BCMECMP_LIBS is defined in $(BCMECMP)/targets.mk
include $(BCMECMP)/targets.mk
SDK_LIBS += $(BCMECMP_LIBS)
SDK_INCLUDES += -I$(BCMECMP)/include
bcmecmp:
	$(MAKE) -C $(BCMECMP) $(SDK_CLEAN)
endif

# Configure BCMECN library
ifneq (,$(filter bcmecn,$(SDK_TARGETS)))
# BCMECN_LIBS is defined in $(BCMECN)/targets.mk
include $(BCMECN)/targets.mk
SDK_LIBS += $(BCMECN_LIBS)
SDK_INCLUDES += -I$(BCMECN)/include
bcmecn:
	$(MAKE) -C $(BCMECN) $(SDK_CLEAN)
endif

# Configure BCMTRUNK library
ifneq (,$(filter bcmtrunk,$(SDK_TARGETS)))
# BCMTRUNK_LIBS is defined in $(BCMTRUNK)/targets.mk
include $(BCMTRUNK)/targets.mk
SDK_LIBS += $(BCMTRUNK_LIBS)
SDK_INCLUDES += -I$(BCMTRUNK)/include
bcmtrunk:
	$(MAKE) -C $(BCMTRUNK) $(SDK_CLEAN)
endif

# Configure BCMTM library
ifneq (,$(filter bcmtm,$(SDK_TARGETS)))
# BCMTM_LIBS is defined in $(BCMTM)/targets.mk
include $(BCMTM)/targets.mk
SDK_LIBS += $(BCMTM_LIBS)
SDK_INCLUDES += -I$(BCMTM)/include
bcmtm:
	$(MAKE) -C $(BCMTM) $(SDK_CLEAN)
endif

# Configure BCMPTM library
ifneq (,$(filter bcmptm,$(SDK_TARGETS)))
# BCMPTM_LIBS is defined in $(BCMPTM)/targets.mk
include $(BCMPTM)/targets.mk
SDK_LIBS += $(BCMPTM_LIBS)
SDK_INCLUDES += -I$(BCMPTM)/include
bcmptm:
	$(MAKE) -C $(BCMPTM) $(SDK_CLEAN)
endif

# Configure BCMLM library
ifneq (,$(filter bcmlm,$(SDK_TARGETS)))
# BCMLM_LIBS is defined in $(BCMLM)/targets.mk
include $(BCMLM)/targets.mk
SDK_LIBS += $(BCMLM_LIBS)
SDK_INCLUDES += -I$(BCMLM)/include
bcmlm:
	$(MAKE) -C $(BCMLM) $(SDK_CLEAN)
endif

# Configure BCMPC library
ifneq (,$(filter bcmpc,$(SDK_TARGETS)))
# BCMPC_LIBS is defined in $(BCMPC)/targets.mk
include $(BCMPC)/targets.mk
SDK_LIBS += $(BCMPC_LIBS)
SDK_INCLUDES += -I$(BCMPC)/include
bcmpc:
	$(MAKE) -C $(BCMPC) $(SDK_CLEAN)
endif

# Configure BCMBD library
ifneq (,$(filter bcmbd,$(SDK_TARGETS)))
# BCMBD_LIBS is defined in $(BCMBD)/targets.mk
include $(BCMBD)/targets.mk
SDK_LIBS += $(BCMBD_LIBS)
SDK_INCLUDES += -I$(BCMBD)/include
bcmbd:
	$(MAKE) -C $(BCMBD) $(SDK_CLEAN)
endif

# Configure BCMIMM library
ifneq (,$(filter bcmimm,$(SDK_TARGETS)))
include $(BCMIMM)/targets.mk
SDK_LIBS += $(BCMIMM_LIBS)
SDK_INCLUDES += -I$(BCMIMM)/include
bcmimm:
	$(MAKE) -C $(BCMIMM) $(SDK_CLEAN)
endif

# Configure BCMLRD library
ifneq (,$(filter bcmlrd,$(SDK_TARGETS)))
# BCMLRD_LIBS is defined in $(BCMLRD)/targets.mk
include $(BCMLRD)/targets.mk
SDK_LIBS += $(BCMLRD_LIBS)
SDK_INCLUDES += -I$(BCMLRD)/include
bcmlrd:
	$(MAKE) -C $(BCMLRD) $(SDK_CLEAN)
endif

# Configure BCMTNL library
ifneq (,$(filter bcmtnl,$(SDK_TARGETS)))
# BCMTNL_LIBS is defined in $(BCMTNL)/targets.mk
include $(BCMTNL)/targets.mk
SDK_LIBS += $(BCMTNL_LIBS)
SDK_INCLUDES += -I$(BCMTNL)/include
bcmtnl:
	$(MAKE) -C $(BCMTNL) $(SDK_CLEAN)
endif

# Configure BCMCFG library
ifneq (,$(filter bcmcfg,$(SDK_TARGETS)))
# BCMCFG_LIBS is defined in $(BCMCFG)/targets.mk
include $(BCMCFG)/targets.mk
SDK_LIBS += $(BCMCFG_LIBS)
SDK_INCLUDES += -I$(BCMCFG)/include
bcmcfg:
	$(MAKE) -C $(BCMCFG) $(SDK_CLEAN)
endif

# Configure BCMPMAC library
ifneq (,$(filter bcmpmac,$(SDK_TARGETS)))
# BCMPMAC_LIBS is defined in $(BCMPMAC)/targets.mk
include $(BCMPMAC)/targets.mk
SDK_LIBS += $(BCMPMAC_LIBS)
SDK_INCLUDES += -I$(BCMPMAC)/include
bcmpmac:
	$(MAKE) -C $(BCMPMAC) $(SDK_CLEAN)
endif

# Configure PHYMOD library
ifneq (,$(filter phymod,$(SDK_TARGETS)))
include $(SDK)/make/phymod.mk
PHYMOD_CPPFLAGS += $(SDK_CPPFLAGS) -I$(BSL)/include -I$(SAL)/include -I$(SHR)/include
export PHYMOD_CPPFLAGS
export PHYMOD_CFLAGS = $(SDK_CFLAGS)
export PHYMOD_BLDDIR = $(SDK_BLDDIR)
include $(SDK)/make/suffix.mk
export PHYMOD_LIBSUFFIX = $(SDK_LIBSUFFIX)
SDK_LIBS += $(addsuffix .$(PHYMOD_LIBSUFFIX),libphymod)
SDK_INCLUDES += -I$(PHYMOD)/include
phymod:
	$(MAKE) -C $(PHYMOD) PHYMOD_CHIPS=$(PHYMOD_CHIPS) $(SDK_CLEAN)
endif

# Configure BCMEVM library
ifneq (,$(filter bcmevm,$(SDK_TARGETS)))
include $(BCMEVM)/targets.mk
SDK_LIBS += $(BCMEVM_LIBS)
SDK_INCLUDES += -I$(BCMEVM)/include
bcmevm:
	$(MAKE) -C $(BCMEVM) $(SDK_CLEAN)
endif

# Configure BCMLTD library
ifneq (,$(filter bcmltd,$(SDK_TARGETS)))
# BCMLTD_LIBS is defined in $(BCMLTD)/targets.mk
include $(BCMLTD)/targets.mk
SDK_LIBS += $(BCMLTD_LIBS)
SDK_INCLUDES += -I$(BCMLTD)/include
bcmltd:
	$(MAKE) -C $(BCMLTD) $(SDK_CLEAN)
endif

# Configure BCMDRD library
ifneq (,$(filter bcmdrd,$(SDK_TARGETS)))
# BCMDRD_LIBS is defined in $(BCMDRD)/targets.mk
include $(BCMDRD)/targets.mk
SDK_LIBS += $(BCMDRD_LIBS)
SDK_INCLUDES += -I$(BCMDRD)/include
bcmdrd:
	$(MAKE) -C $(BCMDRD) $(SDK_CLEAN)
endif

# Configure SIM library
ifneq (,$(filter sim,$(SDK_TARGETS)))
# SIM_LIBS is defined in $(SIM)/targets.mk
include $(SIM)/targets.mk
SDK_LIBS += $(SIM_LIBS)
SDK_INCLUDES += -I$(SIM)/include
sim:
	$(MAKE) -C $(SIM) $(SDK_CLEAN)
endif

# Configure SHR library
ifneq (,$(filter shr,$(SDK_TARGETS)))
# SHR_LIBS is defined in $(SHR)/targets.mk
include $(SHR)/targets.mk
SDK_LIBS += $(SHR_LIBS)
SDK_INCLUDES += -I$(SHR)/include
shr:
	$(MAKE) -C $(SHR) $(SDK_CLEAN)
endif

# Configure SAL library
ifneq (,$(filter sal,$(SDK_TARGETS)))
# SAL_LIBS is defined in $(SAL)/targets.mk
include $(SAL)/targets.mk
SDK_LIBS += $(SAL_LIBS)
SDK_INCLUDES += -I$(SAL)/include
sal:
	$(MAKE) -C $(SAL) $(SDK_CLEAN)
endif

# Configure BSL library
ifneq (,$(filter bsl,$(SDK_TARGETS)))
# BSL_LIBS is defined in $(BSL)/targets.mk
include $(BSL)/targets.mk
SDK_LIBS += $(BSL_LIBS)
SDK_INCLUDES += -I$(BSL)/include
bsl:
	$(MAKE) -C $(BSL) $(SDK_CLEAN)
endif

# Targets with full build path
ifdef SDK_BLDDIR
SDK_BLIBS = $(addprefix $(SDK_BLDDIR)/,$(SDK_LIBS))
endif

# Add target to ensure that both lists are kept in the same order
LLST = $(subst linux,,$(subst /,,$(basename $(dir $(subst -I$(SDK),,$(SDK_INCLUDES))))))
TLST = $(SDK_DEFAULT_TARGETS)

sdklibs_check:
ifneq ("$(TLST)","$(LLST)")
	@$(ECHO) "Error: SDK component lists do not match"
	@exit -1;
endif
