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
# SDK default make rules. These can optionally be overridden by
# letting SDK_MAKE_RULES point to a different rules file.
#

ifdef SDK_MAKE_RULES

include $(SDK_MAKE_RULES)

else

#
# Using abspath ensures that the __FILE__ macro supplies the full path
#
$(BLDDIR)/%.$(OBJSUFFIX): %.c $(BLDDIR)/.tree
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $(abspath $<) -o $@

$(BLDDIR)/%.$(OBJSUFFIX): %.cpp $(BLDDIR)/.tree
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $(abspath $<) -o $@

$(BLDDIR)/%.$(OBJSUFFIX): %.cc $(BLDDIR)/.tree
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $(abspath $<) -o $@


#
# Default to normal mode or chunk mode based on size of BOBJS
#
ifeq ($(SDK_MAKE_LIBRARY_MODE),)

ifneq ($(word 600,$(BOBJS)),)
SDK_MAKE_LIBRARY_MODE=chunk
else
SDK_MAKE_LIBRARY_MODE=normal
endif

endif


#
# Normal Library Builds. 
# All object files archived at once. 
#
ifeq ($(SDK_MAKE_LIBRARY_MODE),normal)

$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree 
	@$(ECHO) 'Building library $(LIBNAME)...'
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJS)

endif


# Serialized make. No limit on number of files. 

ifeq ($(SDK_MAKE_LIBRARY_MODE),serial)

#
# This mode can be selected when the OBJECT list is too large
# to archive as a single execution due to command-line constraints. 
#
# This occurs when the file paths are very long and the object
# count is very high. 
#
# Instead of archiving all files at once, we use the built in
# auto-archive target for each object file. 
#
# These rules cannot be executing in parallel because they would
# cause concurrent access to the target library file, which will
# corrupt it. As a result, this mode is split into two different
# invocations:
#	The first stage allows parallel builds of all object files
#	The second stage disables parallelism and archives the objects
#
#
# This mode is very SLOW due to serial archive and the obviously large
# number of files which need to be archived. 
# 

ifndef SDK_MAKE_LIBRARY_SERIAL_SECOND_STAGE

#
# This is the first stage -- build all object files needed by the library
# These are the prequisites. Then invoke the second stage to archive them. 
#
$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree 
	$(MAKE) $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX) SDK_MAKE_LIBRARY_SERIAL_SECOND_STAGE=1 SDK_MAKE_LIBRARY_MODE=serial

else

#
# This is the second stage -- disable parallelism and archive the resulting files. 
#

.NOTPARALLEL:

# Builtin archive target
LIBBUILD = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)($(BOBJS))

# Custom archive rule
.o.$(LIBSUFFIX):
	$(Q)$(AR) $(ARFLAGS) $@ $<

# Archive the library
$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(LIBBUILD)
	@$(ECHO) 'Serial built library $(notdir $@)'

endif
endif


#
# Chunking Library Mode
# 
# This is hideous but fast. It also works in parallel builds. 
# Just don't look at it. Ever. 
#
# You don't want to enable this for every library. 
#

ifeq ($(SDK_MAKE_LIBRARY_MODE),chunk)

BOBJ_00 = $(wordlist    1, 250, $(BOBJS))
BOBJ_01 = $(wordlist  251, 500, $(BOBJS))
BOBJ_02 = $(wordlist  501, 750, $(BOBJS))
BOBJ_03 = $(wordlist  751,1000, $(BOBJS))
BOBJ_04 = $(wordlist 1001,1250, $(BOBJS))
BOBJ_05 = $(wordlist 1251,1500, $(BOBJS))
BOBJ_06 = $(wordlist 1501,1750, $(BOBJS))
BOBJ_07 = $(wordlist 1751,2000, $(BOBJS))
BOBJ_08 = $(wordlist 2001,2250, $(BOBJS))
BOBJ_09 = $(wordlist 2251,2500, $(BOBJS))
BOBJ_10 = $(wordlist 2501,2750, $(BOBJS))
BOBJ_11 = $(wordlist 2751,3000, $(BOBJS))
BOBJ_12 = $(wordlist 3001,3250, $(BOBJS))
BOBJ_13 = $(wordlist 3251,3500, $(BOBJS))
BOBJ_14 = $(wordlist 3501,3750, $(BOBJS))
BOBJ_15 = $(wordlist 3751,4000, $(BOBJS))
BOBJ_16 = $(wordlist 4001,4250, $(BOBJS))
BOBJ_17 = $(wordlist 4251,4500, $(BOBJS))
BOBJ_18 = $(wordlist 4501,4750, $(BOBJS))
BOBJ_19 = $(wordlist 4751,5000, $(BOBJS))
BOBJ_20 = $(wordlist 5001,5250, $(BOBJS))
BOBJ_21 = $(wordlist 5251,5500, $(BOBJS))
BOBJ_22 = $(wordlist 5501,5750, $(BOBJS))
BOBJ_23 = $(wordlist 5751,6000, $(BOBJS))
BOBJ_24 = $(wordlist 6001,6250, $(BOBJS))
BOBJ_25 = $(wordlist 6251,6500, $(BOBJS))
BOBJ_26 = $(wordlist 6501,6750, $(BOBJS))
BOBJ_27 = $(wordlist 6751,7000, $(BOBJS))
BOBJ_28 = $(wordlist 7001,7250, $(BOBJS))
BOBJ_29 = $(wordlist 7251,7500, $(BOBJS))
BOBJ_30 = $(wordlist 7501,7750, $(BOBJS))
BOBJ_31 = $(wordlist 7751,8000, $(BOBJS))
BOBJ_32 = $(wordlist 8001,8250, $(BOBJS))
BOBJ_33 = $(wordlist 8251,8500, $(BOBJS))
BOBJ_34 = $(wordlist 8501,8750, $(BOBJS))
BOBJ_35 = $(wordlist 8751,9000, $(BOBJS))
BOBJ_36 = $(wordlist 9001,9250, $(BOBJS))
BOBJ_37 = $(wordlist 9251,9500, $(BOBJS))
BOBJ_38 = $(wordlist 9501,9750, $(BOBJS))
BOBJ_39 = $(wordlist 9751,10000, $(BOBJS))
BOBJ_40 = $(wordlist 10001,10250, $(BOBJS))
BOBJ_41 = $(wordlist 10251,10500, $(BOBJS))
BOBJ_42 = $(wordlist 10501,10750, $(BOBJS))
BOBJ_44 = $(wordlist 10751,11000, $(BOBJS))
BOBJ_45 = $(wordlist 11001,11250, $(BOBJS))
BOBJ_46 = $(wordlist 11251,11500, $(BOBJS))
BOBJ_47 = $(wordlist 11501,11750, $(BOBJS))
BOBJ_48 = $(wordlist 11751,12000, $(BOBJS))
BOBJ_49 = $(wordlist 12001,12250, $(BOBJS))
BOBJ_50 = $(wordlist 12251,12500, $(BOBJS))
BOBJ_51 = $(wordlist 12501,12750, $(BOBJS))
BOBJ_52 = $(wordlist 12751,13000, $(BOBJS))
BOBJ_53 = $(wordlist 13001,13250, $(BOBJS))
BOBJ_54 = $(wordlist 13251,13500, $(BOBJS))
BOBJ_55 = $(wordlist 13501,13750, $(BOBJS))
BOBJ_56 = $(wordlist 13751,14000, $(BOBJS))
BOBJ_57 = $(wordlist 14001,14250, $(BOBJS))
BOBJ_58 = $(wordlist 14251,14500, $(BOBJS))
BOBJ_59 = $(wordlist 14501,14750, $(BOBJS))
BOBJ_60 = $(wordlist 14751,15000, $(BOBJS))
BOBJ_61 = $(wordlist 15001,15250, $(BOBJS))
BOBJ_62 = $(wordlist 15251,15500, $(BOBJS))
BOBJ_63 = $(wordlist 15501,15750, $(BOBJS))
BOBJ_64 = $(wordlist 15751,16000, $(BOBJS))
BOBJ_65 = $(wordlist 16001,16250, $(BOBJS))
BOBJ_66 = $(wordlist 16251,16500, $(BOBJS))
BOBJ_67 = $(wordlist 16501,16750, $(BOBJS))
BOBJ_68 = $(wordlist 16751,17000, $(BOBJS))
BOBJ_69 = $(wordlist 17001,17250, $(BOBJS))
BOBJ_70 = $(wordlist 17251,17500, $(BOBJS))
BOBJ_71 = $(wordlist 17501,17750, $(BOBJS))
BOBJ_72 = $(wordlist 17751,18000, $(BOBJS))
BOBJ_73 = $(wordlist 18001,18250, $(BOBJS))
BOBJ_74 = $(wordlist 18251,18500, $(BOBJS))
BOBJ_75 = $(wordlist 18501,18750, $(BOBJS))
BOBJ_76 = $(wordlist 18751,19000, $(BOBJS))
BOBJ_77 = $(wordlist 19001,19250, $(BOBJS))
BOBJ_78 = $(wordlist 19251,19500, $(BOBJS))
BOBJ_79 = $(wordlist 19501,19750, $(BOBJS))
BOBJ_80 = $(wordlist 19751,20000, $(BOBJS))

ifneq ($(word 20001,$(BOBJS)),)
$(error Too many files in chunk library build $(LIBNAME). Add more chunks @ $$SDK/make/rules.mk)
endif

$(LIBDIR)/%.$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree
	@$(ECHO) 'Chunk Building library $(LIBNAME)...'
ifneq ($(BOBJ_00),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_00)
endif
ifneq ($(BOBJ_01),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_01)
endif
ifneq ($(BOBJ_02),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_02)
endif
ifneq ($(BOBJ_03),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_03)
endif
ifneq ($(BOBJ_04),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_04)
endif
ifneq ($(BOBJ_05),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_05)
endif
ifneq ($(BOBJ_06),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_06)
endif
ifneq ($(BOBJ_07),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_07)
endif
ifneq ($(BOBJ_08),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_08)
endif
ifneq ($(BOBJ_09),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_09)
endif
ifneq ($(BOBJ_10),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_10)
endif
ifneq ($(BOBJ_11),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_11)
endif
ifneq ($(BOBJ_12),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_12)
endif
ifneq ($(BOBJ_13),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_13)
endif
ifneq ($(BOBJ_14),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_14)
endif
ifneq ($(BOBJ_15),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_15)
endif
ifneq ($(BOBJ_16),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_16)
endif
ifneq ($(BOBJ_17),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_17)
endif
ifneq ($(BOBJ_18),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_18)
endif
ifneq ($(BOBJ_19),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_19)
endif
ifneq ($(BOBJ_20),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_20)
endif
ifneq ($(BOBJ_21),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_21)
endif
ifneq ($(BOBJ_22),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_22)
endif
ifneq ($(BOBJ_23),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_23)
endif
ifneq ($(BOBJ_24),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_24)
endif
ifneq ($(BOBJ_25),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_25)
endif
ifneq ($(BOBJ_26),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_26)
endif
ifneq ($(BOBJ_27),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_27)
endif
ifneq ($(BOBJ_28),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_28)
endif
ifneq ($(BOBJ_29),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_29)
endif
ifneq ($(BOBJ_30),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_30)
endif
ifneq ($(BOBJ_31),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_31)
endif
ifneq ($(BOBJ_32),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_32)
endif
ifneq ($(BOBJ_33),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_33)
endif
ifneq ($(BOBJ_34),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_34)
endif
ifneq ($(BOBJ_35),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_35)
endif
ifneq ($(BOBJ_36),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_36)
endif
ifneq ($(BOBJ_37),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_37)
endif
ifneq ($(BOBJ_38),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_38)
endif
ifneq ($(BOBJ_39),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_39)
endif
ifneq ($(BOBJ_40),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_40)
endif
ifneq ($(BOBJ_41),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_41)
endif
ifneq ($(BOBJ_42),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_42)
endif
ifneq ($(BOBJ_43),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_43)
endif
ifneq ($(BOBJ_44),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_44)
endif
ifneq ($(BOBJ_45),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_45)
endif
ifneq ($(BOBJ_46),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_46)
endif
ifneq ($(BOBJ_47),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_47)
endif
ifneq ($(BOBJ_48),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_48)
endif
ifneq ($(BOBJ_49),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_49)
endif
ifneq ($(BOBJ_50),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_50)
endif
ifneq ($(BOBJ_51),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_51)
endif
ifneq ($(BOBJ_52),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_52)
endif
ifneq ($(BOBJ_53),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_53)
endif
ifneq ($(BOBJ_54),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_54)
endif
ifneq ($(BOBJ_55),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_55)
endif
ifneq ($(BOBJ_56),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_56)
endif
ifneq ($(BOBJ_57),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_57)
endif
ifneq ($(BOBJ_58),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_58)
endif
ifneq ($(BOBJ_59),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_59)
endif
ifneq ($(BOBJ_60),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_60)
endif
ifneq ($(BOBJ_61),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_61)
endif
ifneq ($(BOBJ_62),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_62)
endif
ifneq ($(BOBJ_63),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_63)
endif
ifneq ($(BOBJ_64),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_64)
endif
ifneq ($(BOBJ_65),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_65)
endif
ifneq ($(BOBJ_66),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_66)
endif
ifneq ($(BOBJ_67),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_67)
endif
ifneq ($(BOBJ_68),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_68)
endif
ifneq ($(BOBJ_69),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_69)
endif
ifneq ($(BOBJ_70),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_70)
endif
ifneq ($(BOBJ_71),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_71)
endif
ifneq ($(BOBJ_72),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_72)
endif
ifneq ($(BOBJ_73),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_73)
endif
ifneq ($(BOBJ_74),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_74)
endif
ifneq ($(BOBJ_75),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_75)
endif
ifneq ($(BOBJ_76),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_76)
endif
ifneq ($(BOBJ_77),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_77)
endif
ifneq ($(BOBJ_78),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_78)
endif
ifneq ($(BOBJ_79),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_79)
endif
ifneq ($(BOBJ_80),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_80)
endif


# End of chunk library mode
endif
endif
