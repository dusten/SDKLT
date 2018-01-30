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
# SDK make rules for libraries
#

include $(SDK)/make/config.mk

include $(SDK)/make/rules.mk

BLIBNAME = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)

.SECONDARY:: $(BOBJS)

all:: $(BLDDIR)/.tree $(BLIBNAME)

ifneq ($(SDK_MAKE_LIBRARY_MODE),chunk)

# Normal cleaning

clean::
	@$(ECHO) Cleaning objects for $(notdir $(BLIBNAME))
	$(Q)$(RM) $(BLDDIR)/.tree $(BOBJS) $(BLIBNAME)

else

# Chunk cleaning (required if large number of objects)

clean::
	@$(ECHO) Chunk cleaning objects for $(notdir $(BLIBNAME))
ifneq ($(BOBJ_00),)
	$(Q)$(RM) $(BOBJ_00)
endif
ifneq ($(BOBJ_01),)
	$(Q)$(RM) $(BOBJ_01)
endif
ifneq ($(BOBJ_02),)
	$(Q)$(RM) $(BOBJ_02)
endif
ifneq ($(BOBJ_03),)
	$(Q)$(RM) $(BOBJ_03)
endif
ifneq ($(BOBJ_04),)
	$(Q)$(RM) $(BOBJ_04)
endif
ifneq ($(BOBJ_05),)
	$(Q)$(RM) $(BOBJ_05)
endif
ifneq ($(BOBJ_06),)
	$(Q)$(RM) $(BOBJ_06)
endif
ifneq ($(BOBJ_07),)
	$(Q)$(RM) $(BOBJ_07)
endif
ifneq ($(BOBJ_08),)
	$(Q)$(RM) $(BOBJ_08)
endif
ifneq ($(BOBJ_09),)
	$(Q)$(RM) $(BOBJ_09)
endif
ifneq ($(BOBJ_10),)
	$(Q)$(RM) $(BOBJ_10)
endif
ifneq ($(BOBJ_11),)
	$(Q)$(RM) $(BOBJ_11)
endif
ifneq ($(BOBJ_12),)
	$(Q)$(RM) $(BOBJ_12)
endif
ifneq ($(BOBJ_13),)
	$(Q)$(RM) $(BOBJ_13)
endif
ifneq ($(BOBJ_14),)
	$(Q)$(RM) $(BOBJ_14)
endif
ifneq ($(BOBJ_15),)
	$(Q)$(RM) $(BOBJ_15)
endif
ifneq ($(BOBJ_16),)
	$(Q)$(RM) $(BOBJ_16)
endif
ifneq ($(BOBJ_17),)
	$(Q)$(RM) $(BOBJ_17)
endif
ifneq ($(BOBJ_18),)
	$(Q)$(RM) $(BOBJ_18)
endif
ifneq ($(BOBJ_19),)
	$(Q)$(RM) $(BOBJ_19)
endif
ifneq ($(BOBJ_20),)
	$(Q)$(RM) $(BOBJ_20)
endif
ifneq ($(BOBJ_21),)
	$(Q)$(RM) $(BOBJ_21)
endif
ifneq ($(BOBJ_22),)
	$(Q)$(RM) $(BOBJ_22)
endif
ifneq ($(BOBJ_23),)
	$(Q)$(RM) $(BOBJ_23)
endif
ifneq ($(BOBJ_24),)
	$(Q)$(RM) $(BOBJ_24)
endif
ifneq ($(BOBJ_25),)
	$(Q)$(RM) $(BOBJ_25)
endif
ifneq ($(BOBJ_26),)
	$(Q)$(RM) $(BOBJ_26)
endif
ifneq ($(BOBJ_27),)
	$(Q)$(RM) $(BOBJ_27)
endif
ifneq ($(BOBJ_28),)
	$(Q)$(RM) $(BOBJ_28)
endif
ifneq ($(BOBJ_29),)
	$(Q)$(RM) $(BOBJ_29)
endif
ifneq ($(BOBJ_30),)
	$(Q)$(RM) $(BOBJ_30)
endif
ifneq ($(BOBJ_31),)
	$(Q)$(RM) $(BOBJ_31)
endif
ifneq ($(BOBJ_32),)
	$(Q)$(RM) $(BOBJ_32)
endif
ifneq ($(BOBJ_33),)
	$(Q)$(RM) $(BOBJ_33)
endif
ifneq ($(BOBJ_34),)
	$(Q)$(RM) $(BOBJ_34)
endif
ifneq ($(BOBJ_35),)
	$(Q)$(RM) $(BOBJ_35)
endif
ifneq ($(BOBJ_36),)
	$(Q)$(RM) $(BOBJ_36)
endif
ifneq ($(BOBJ_37),)
	$(Q)$(RM) $(BOBJ_37)
endif
ifneq ($(BOBJ_38),)
	$(Q)$(RM) $(BOBJ_38)
endif
ifneq ($(BOBJ_39),)
	$(Q)$(RM) $(BOBJ_39)
endif
ifneq ($(BOBJ_40),)
	$(Q)$(RM) $(BOBJ_40)
endif
ifneq ($(BOBJ_41),)
	$(Q)$(RM) $(BOBJ_41)
endif
ifneq ($(BOBJ_42),)
	$(Q)$(RM) $(BOBJ_42)
endif
ifneq ($(BOBJ_43),)
	$(Q)$(RM) $(BOBJ_43)
endif
ifneq ($(BOBJ_44),)
	$(Q)$(RM) $(BOBJ_44)
endif
ifneq ($(BOBJ_45),)
	$(Q)$(RM) $(BOBJ_45)
endif
ifneq ($(BOBJ_46),)
	$(Q)$(RM) $(BOBJ_46)
endif
ifneq ($(BOBJ_47),)
	$(Q)$(RM) $(BOBJ_47)
endif
ifneq ($(BOBJ_48),)
	$(Q)$(RM) $(BOBJ_48)
endif
ifneq ($(BOBJ_49),)
	$(Q)$(RM) $(BOBJ_49)
endif
ifneq ($(BOBJ_50),)
	$(Q)$(RM) $(BOBJ_50)
endif
ifneq ($(BOBJ_51),)
	$(Q)$(RM) $(BOBJ_51)
endif
ifneq ($(BOBJ_52),)
	$(Q)$(RM) $(BOBJ_52)
endif
ifneq ($(BOBJ_53),)
	$(Q)$(RM) $(BOBJ_53)
endif
ifneq ($(BOBJ_54),)
	$(Q)$(RM) $(BOBJ_54)
endif
ifneq ($(BOBJ_55),)
	$(Q)$(RM) $(BOBJ_55)
endif
ifneq ($(BOBJ_56),)
	$(Q)$(RM) $(BOBJ_56)
endif
ifneq ($(BOBJ_57),)
	$(Q)$(RM) $(BOBJ_57)
endif
ifneq ($(BOBJ_58),)
	$(Q)$(RM) $(BOBJ_58)
endif
ifneq ($(BOBJ_59),)
	$(Q)$(RM) $(BOBJ_59)
endif
ifneq ($(BOBJ_60),)
	$(Q)$(RM) $(BOBJ_60)
endif
ifneq ($(BOBJ_61),)
	$(Q)$(RM) $(BOBJ_61)
endif
ifneq ($(BOBJ_62),)
	$(Q)$(RM) $(BOBJ_62)
endif
ifneq ($(BOBJ_63),)
	$(Q)$(RM) $(BOBJ_63)
endif
ifneq ($(BOBJ_64),)
	$(Q)$(RM) $(BOBJ_64)
endif
ifneq ($(BOBJ_65),)
	$(Q)$(RM) $(BOBJ_65)
endif
ifneq ($(BOBJ_66),)
	$(Q)$(RM) $(BOBJ_66)
endif
ifneq ($(BOBJ_67),)
	$(Q)$(RM) $(BOBJ_67)
endif
ifneq ($(BOBJ_68),)
	$(Q)$(RM) $(BOBJ_68)
endif
ifneq ($(BOBJ_69),)
	$(Q)$(RM) $(BOBJ_69)
endif
ifneq ($(BOBJ_70),)
	$(Q)$(RM) $(BOBJ_70)
endif
ifneq ($(BOBJ_71),)
	$(Q)$(RM) $(BOBJ_71)
endif
ifneq ($(BOBJ_72),)
	$(Q)$(RM) $(BOBJ_72)
endif
ifneq ($(BOBJ_73),)
	$(Q)$(RM) $(BOBJ_73)
endif
ifneq ($(BOBJ_74),)
	$(Q)$(RM) $(BOBJ_74)
endif
ifneq ($(BOBJ_75),)
	$(Q)$(RM) $(BOBJ_75)
endif
ifneq ($(BOBJ_76),)
	$(Q)$(RM) $(BOBJ_76)
endif
ifneq ($(BOBJ_77),)
	$(Q)$(RM) $(BOBJ_77)
endif
ifneq ($(BOBJ_78),)
	$(Q)$(RM) $(BOBJ_78)
endif
ifneq ($(BOBJ_79),)
	$(Q)$(RM) $(BOBJ_79)
endif
ifneq ($(BOBJ_80),)
	$(Q)$(RM) $(BOBJ_80)
endif

# End of chunk library mode
endif

include $(SDK)/make/depend.mk
