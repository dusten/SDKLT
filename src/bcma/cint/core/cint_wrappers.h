/*
 * $Id: cint_wrappers.h,v 1.8 2012/03/02 16:21:40 yaronm Exp $
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
 * File:        cint_wrappers.h
 * Purpose:     CINT wrapper interfaces
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated by wrappergen.pl.
 * Edits to this file will be lost when it is regenerated.
 */

#ifndef __CINT_WRAPPERS_H__
#define __CINT_WRAPPERS_H__

#define CINT_FARGS0() \


#define CINT_FARGS1(pt0,p0) \
    pt0 p0; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \


#define CINT_FARGS2(pt0,p0,pt1,p1) \
    pt0 p0; \
    pt1 p1; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \


#define CINT_FARGS3(pt0,p0,pt1,p1,pt2,p2) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \


#define CINT_FARGS4(pt0,p0,pt1,p1,pt2,p2,pt3,p3) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \


#define CINT_FARGS5(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    pt4 p4; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \
    CINT_MEMCPY(&p4, fparams->args[4], sizeof(p4)); \


#define CINT_FARGS6(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    pt4 p4; \
    pt5 p5; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \
    CINT_MEMCPY(&p4, fparams->args[4], sizeof(p4)); \
    CINT_MEMCPY(&p5, fparams->args[5], sizeof(p5)); \


#define CINT_FARGS7(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    pt4 p4; \
    pt5 p5; \
    pt6 p6; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \
    CINT_MEMCPY(&p4, fparams->args[4], sizeof(p4)); \
    CINT_MEMCPY(&p5, fparams->args[5], sizeof(p5)); \
    CINT_MEMCPY(&p6, fparams->args[6], sizeof(p6)); \


#define CINT_FARGS8(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    pt4 p4; \
    pt5 p5; \
    pt6 p6; \
    pt7 p7; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \
    CINT_MEMCPY(&p4, fparams->args[4], sizeof(p4)); \
    CINT_MEMCPY(&p5, fparams->args[5], sizeof(p5)); \
    CINT_MEMCPY(&p6, fparams->args[6], sizeof(p6)); \
    CINT_MEMCPY(&p7, fparams->args[7], sizeof(p7)); \


#define CINT_FARGS9(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    pt4 p4; \
    pt5 p5; \
    pt6 p6; \
    pt7 p7; \
    pt8 p8; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \
    CINT_MEMCPY(&p4, fparams->args[4], sizeof(p4)); \
    CINT_MEMCPY(&p5, fparams->args[5], sizeof(p5)); \
    CINT_MEMCPY(&p6, fparams->args[6], sizeof(p6)); \
    CINT_MEMCPY(&p7, fparams->args[7], sizeof(p7)); \
    CINT_MEMCPY(&p8, fparams->args[8], sizeof(p8)); \


#define CINT_FARGS10(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    pt4 p4; \
    pt5 p5; \
    pt6 p6; \
    pt7 p7; \
    pt8 p8; \
    pt9 p9; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \
    CINT_MEMCPY(&p4, fparams->args[4], sizeof(p4)); \
    CINT_MEMCPY(&p5, fparams->args[5], sizeof(p5)); \
    CINT_MEMCPY(&p6, fparams->args[6], sizeof(p6)); \
    CINT_MEMCPY(&p7, fparams->args[7], sizeof(p7)); \
    CINT_MEMCPY(&p8, fparams->args[8], sizeof(p8)); \
    CINT_MEMCPY(&p9, fparams->args[9], sizeof(p9)); \


#define CINT_FARGS11(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10) \
    pt0 p0; \
    pt1 p1; \
    pt2 p2; \
    pt3 p3; \
    pt4 p4; \
    pt5 p5; \
    pt6 p6; \
    pt7 p7; \
    pt8 p8; \
    pt9 p9; \
    pt10 p10; \
    CINT_MEMCPY(&p0, fparams->args[0], sizeof(p0)); \
    CINT_MEMCPY(&p1, fparams->args[1], sizeof(p1)); \
    CINT_MEMCPY(&p2, fparams->args[2], sizeof(p2)); \
    CINT_MEMCPY(&p3, fparams->args[3], sizeof(p3)); \
    CINT_MEMCPY(&p4, fparams->args[4], sizeof(p4)); \
    CINT_MEMCPY(&p5, fparams->args[5], sizeof(p5)); \
    CINT_MEMCPY(&p6, fparams->args[6], sizeof(p6)); \
    CINT_MEMCPY(&p7, fparams->args[7], sizeof(p7)); \
    CINT_MEMCPY(&p8, fparams->args[8], sizeof(p8)); \
    CINT_MEMCPY(&p9, fparams->args[9], sizeof(p9)); \
    CINT_MEMCPY(&p10, fparams->args[10], sizeof(p10)); \


#define CINT_FWRAPPER_FUNCTION_VP0(fname)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
\
  fname (); \
\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP0(fname)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP0(fname)  \
CINT_FWRAPPER_FUNCTION_VP0(fname)   \
CINT_FWRAPPER_PARAMS_VP0(fname)

#define CINT_FWRAPPER_PARAMS2_VP0(fname)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP0(fname)  \
CINT_FWRAPPER_FUNCTION_VP0(fname)   \
CINT_FWRAPPER_PARAMS2_VP0(fname)

#define CINT_FWRAPPER_FUNCTION_VP1(fname,pt0,p0)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS1(pt0,p0);  \
\
  fname (p0); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP1(fname,pb0,p0,pp0,pa0)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP1(fname,pt0,pb0,p0,pp0,pa0)  \
CINT_FWRAPPER_FUNCTION_VP1(fname,pt0,p0)   \
CINT_FWRAPPER_PARAMS_VP1(fname,pb0,p0,pp0,pa0)

#define CINT_FWRAPPER_PARAMS2_VP1(fname,pb0,p0,pp0,pa0,pf0)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP1(fname,pt0,pb0,p0,pp0,pa0,pf0)  \
CINT_FWRAPPER_FUNCTION_VP1(fname,pt0,p0)   \
CINT_FWRAPPER_PARAMS2_VP1(fname,pb0,p0,pp0,pa0,pf0)

#define CINT_FWRAPPER_FUNCTION_VP2(fname,pt0,p0,pt1,p1)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS2(pt0,p0,pt1,p1);  \
\
  fname (p0,p1); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP2(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP2(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1)  \
CINT_FWRAPPER_FUNCTION_VP2(fname,pt0,p0,pt1,p1)   \
CINT_FWRAPPER_PARAMS_VP2(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1)

#define CINT_FWRAPPER_PARAMS2_VP2(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP2(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1)  \
CINT_FWRAPPER_FUNCTION_VP2(fname,pt0,p0,pt1,p1)   \
CINT_FWRAPPER_PARAMS2_VP2(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1)

#define CINT_FWRAPPER_FUNCTION_VP3(fname,pt0,p0,pt1,p1,pt2,p2)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS3(pt0,p0,pt1,p1,pt2,p2);  \
\
  fname (p0,p1,p2); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP3(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP3(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2)  \
CINT_FWRAPPER_FUNCTION_VP3(fname,pt0,p0,pt1,p1,pt2,p2)   \
CINT_FWRAPPER_PARAMS_VP3(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2)

#define CINT_FWRAPPER_PARAMS2_VP3(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP3(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2)  \
CINT_FWRAPPER_FUNCTION_VP3(fname,pt0,p0,pt1,p1,pt2,p2)   \
CINT_FWRAPPER_PARAMS2_VP3(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2)

#define CINT_FWRAPPER_FUNCTION_VP4(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS4(pt0,p0,pt1,p1,pt2,p2,pt3,p3);  \
\
  fname (p0,p1,p2,p3); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP4(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP4(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3)  \
CINT_FWRAPPER_FUNCTION_VP4(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3)   \
CINT_FWRAPPER_PARAMS_VP4(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3)

#define CINT_FWRAPPER_PARAMS2_VP4(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP4(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3)  \
CINT_FWRAPPER_FUNCTION_VP4(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3)   \
CINT_FWRAPPER_PARAMS2_VP4(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3)

#define CINT_FWRAPPER_FUNCTION_VP5(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS5(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4);  \
\
  fname (p0,p1,p2,p3,p4); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP5(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP5(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4)  \
CINT_FWRAPPER_FUNCTION_VP5(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4)   \
CINT_FWRAPPER_PARAMS_VP5(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4)

#define CINT_FWRAPPER_PARAMS2_VP5(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP5(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4)  \
CINT_FWRAPPER_FUNCTION_VP5(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4)   \
CINT_FWRAPPER_PARAMS2_VP5(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4)

#define CINT_FWRAPPER_FUNCTION_VP6(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS6(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5);  \
\
  fname (p0,p1,p2,p3,p4,p5); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP6(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP6(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5)  \
CINT_FWRAPPER_FUNCTION_VP6(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5)   \
CINT_FWRAPPER_PARAMS_VP6(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5)

#define CINT_FWRAPPER_PARAMS2_VP6(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP6(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5)  \
CINT_FWRAPPER_FUNCTION_VP6(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5)   \
CINT_FWRAPPER_PARAMS2_VP6(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5)

#define CINT_FWRAPPER_FUNCTION_VP7(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS7(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6);  \
\
  fname (p0,p1,p2,p3,p4,p5,p6); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP7(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP7(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6)  \
CINT_FWRAPPER_FUNCTION_VP7(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6)   \
CINT_FWRAPPER_PARAMS_VP7(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6)

#define CINT_FWRAPPER_PARAMS2_VP7(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP7(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6)  \
CINT_FWRAPPER_FUNCTION_VP7(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6)   \
CINT_FWRAPPER_PARAMS2_VP7(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6)

#define CINT_FWRAPPER_FUNCTION_VP8(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS8(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7);  \
\
  fname (p0,p1,p2,p3,p4,p5,p6,p7); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP8(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP8(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7)  \
CINT_FWRAPPER_FUNCTION_VP8(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7)   \
CINT_FWRAPPER_PARAMS_VP8(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7)

#define CINT_FWRAPPER_PARAMS2_VP8(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP8(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7)  \
CINT_FWRAPPER_FUNCTION_VP8(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7)   \
CINT_FWRAPPER_PARAMS2_VP8(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7)

#define CINT_FWRAPPER_FUNCTION_VP9(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS9(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8);  \
\
  fname (p0,p1,p2,p3,p4,p5,p6,p7,p8); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP9(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  { #pb8,#p8,pp8,pa8 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP9(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7,pt8,pb8,p8,pp8,pa8)  \
CINT_FWRAPPER_FUNCTION_VP9(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8)   \
CINT_FWRAPPER_PARAMS_VP9(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8)

#define CINT_FWRAPPER_PARAMS2_VP9(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  { #pb8,#p8,pp8,pa8,pf8 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP9(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7,pt8,pb8,p8,pp8,pa8,pf8)  \
CINT_FWRAPPER_FUNCTION_VP9(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8)   \
CINT_FWRAPPER_PARAMS2_VP9(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8)

#define CINT_FWRAPPER_FUNCTION_VP10(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS10(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9);  \
\
  fname (p0,p1,p2,p3,p4,p5,p6,p7,p8,p9); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP10(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  { #pb8,#p8,pp8,pa8 }, \
  { #pb9,#p9,pp9,pa9 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP10(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7,pt8,pb8,p8,pp8,pa8,pt9,pb9,p9,pp9,pa9)  \
CINT_FWRAPPER_FUNCTION_VP10(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9)   \
CINT_FWRAPPER_PARAMS_VP10(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9)

#define CINT_FWRAPPER_PARAMS2_VP10(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  { #pb8,#p8,pp8,pa8,pf8 }, \
  { #pb9,#p9,pp9,pa9,pf9 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP10(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7,pt8,pb8,p8,pp8,pa8,pf8,pt9,pb9,p9,pp9,pa9,pf9)  \
CINT_FWRAPPER_FUNCTION_VP10(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9)   \
CINT_FWRAPPER_PARAMS2_VP10(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9)

#define CINT_FWRAPPER_FUNCTION_VP11(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS11(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10);  \
\
  fname (p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_VP11(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9,pb10,p10,pp10,pa10)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  { #pb8,#p8,pp8,pa8 }, \
  { #pb9,#p9,pp9,pa9 }, \
  { #pb10,#p10,pp10,pa10 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_VP11(fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7,pt8,pb8,p8,pp8,pa8,pt9,pb9,p9,pp9,pa9,pt10,pb10,p10,pp10,pa10)  \
CINT_FWRAPPER_FUNCTION_VP11(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10)   \
CINT_FWRAPPER_PARAMS_VP11(fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9,pb10,p10,pp10,pa10)

#define CINT_FWRAPPER_PARAMS2_VP11(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9,pb10,p10,pp10,pa10,pf10)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { "void", "r", 0, 0 }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  { #pb8,#p8,pp8,pa8,pf8 }, \
  { #pb9,#p9,pp9,pa9,pf9 }, \
  { #pb10,#p10,pp10,pa10,pf10 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_VP11(fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7,pt8,pb8,p8,pp8,pa8,pf8,pt9,pb9,p9,pp9,pa9,pf9,pt10,pb10,p10,pp10,pa10,pf10)  \
CINT_FWRAPPER_FUNCTION_VP11(fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10)   \
CINT_FWRAPPER_PARAMS2_VP11(fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9,pb10,p10,pp10,pa10,pf10)

#define CINT_FWRAPPER_FUNCTION_RP0(rt,fname)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
\
  CINT_FRET(rt) = fname (); \
\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP0(rb,rp,ra,fname)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP0(rt,rb,rp,ra,fname)  \
CINT_FWRAPPER_FUNCTION_RP0(rt,fname)   \
CINT_FWRAPPER_PARAMS_RP0(rb,rp,ra,fname)

#define CINT_FWRAPPER_PARAMS2_RP0(rb,rp,ra,fname)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP0(rt,rb,rp,ra,fname)  \
CINT_FWRAPPER_FUNCTION_RP0(rt,fname)   \
CINT_FWRAPPER_PARAMS2_RP0(rb,rp,ra,fname)

#define CINT_FWRAPPER_FUNCTION_RP1(rt,fname,pt0,p0)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS1(pt0,p0);  \
\
  CINT_FRET(rt) = fname (p0); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP1(rb,rp,ra,fname,pb0,p0,pp0,pa0)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP1(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0)  \
CINT_FWRAPPER_FUNCTION_RP1(rt,fname,pt0,p0)   \
CINT_FWRAPPER_PARAMS_RP1(rb,rp,ra,fname,pb0,p0,pp0,pa0)

#define CINT_FWRAPPER_PARAMS2_RP1(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP1(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0)  \
CINT_FWRAPPER_FUNCTION_RP1(rt,fname,pt0,p0)   \
CINT_FWRAPPER_PARAMS2_RP1(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0)

#define CINT_FWRAPPER_FUNCTION_RP2(rt,fname,pt0,p0,pt1,p1)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS2(pt0,p0,pt1,p1);  \
\
  CINT_FRET(rt) = fname (p0,p1); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP2(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP2(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1)  \
CINT_FWRAPPER_FUNCTION_RP2(rt,fname,pt0,p0,pt1,p1)   \
CINT_FWRAPPER_PARAMS_RP2(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1)

#define CINT_FWRAPPER_PARAMS2_RP2(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP2(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1)  \
CINT_FWRAPPER_FUNCTION_RP2(rt,fname,pt0,p0,pt1,p1)   \
CINT_FWRAPPER_PARAMS2_RP2(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1)

#define CINT_FWRAPPER_FUNCTION_RP3(rt,fname,pt0,p0,pt1,p1,pt2,p2)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS3(pt0,p0,pt1,p1,pt2,p2);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP3(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP3(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2)  \
CINT_FWRAPPER_FUNCTION_RP3(rt,fname,pt0,p0,pt1,p1,pt2,p2)   \
CINT_FWRAPPER_PARAMS_RP3(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2)

#define CINT_FWRAPPER_PARAMS2_RP3(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP3(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2)  \
CINT_FWRAPPER_FUNCTION_RP3(rt,fname,pt0,p0,pt1,p1,pt2,p2)   \
CINT_FWRAPPER_PARAMS2_RP3(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2)

#define CINT_FWRAPPER_FUNCTION_RP4(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS4(pt0,p0,pt1,p1,pt2,p2,pt3,p3);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP4(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP4(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3)  \
CINT_FWRAPPER_FUNCTION_RP4(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3)   \
CINT_FWRAPPER_PARAMS_RP4(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3)

#define CINT_FWRAPPER_PARAMS2_RP4(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP4(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3)  \
CINT_FWRAPPER_FUNCTION_RP4(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3)   \
CINT_FWRAPPER_PARAMS2_RP4(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3)

#define CINT_FWRAPPER_FUNCTION_RP5(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS5(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3,p4); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP5(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP5(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4)  \
CINT_FWRAPPER_FUNCTION_RP5(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4)   \
CINT_FWRAPPER_PARAMS_RP5(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4)

#define CINT_FWRAPPER_PARAMS2_RP5(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP5(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4)  \
CINT_FWRAPPER_FUNCTION_RP5(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4)   \
CINT_FWRAPPER_PARAMS2_RP5(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4)

#define CINT_FWRAPPER_FUNCTION_RP6(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS6(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3,p4,p5); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP6(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP6(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5)  \
CINT_FWRAPPER_FUNCTION_RP6(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5)   \
CINT_FWRAPPER_PARAMS_RP6(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5)

#define CINT_FWRAPPER_PARAMS2_RP6(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP6(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5)  \
CINT_FWRAPPER_FUNCTION_RP6(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5)   \
CINT_FWRAPPER_PARAMS2_RP6(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5)

#define CINT_FWRAPPER_FUNCTION_RP7(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS7(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3,p4,p5,p6); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP7(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP7(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6)  \
CINT_FWRAPPER_FUNCTION_RP7(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6)   \
CINT_FWRAPPER_PARAMS_RP7(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6)

#define CINT_FWRAPPER_PARAMS2_RP7(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP7(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6)  \
CINT_FWRAPPER_FUNCTION_RP7(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6)   \
CINT_FWRAPPER_PARAMS2_RP7(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6)

#define CINT_FWRAPPER_FUNCTION_RP8(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS8(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3,p4,p5,p6,p7); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP8(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP8(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7)  \
CINT_FWRAPPER_FUNCTION_RP8(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7)   \
CINT_FWRAPPER_PARAMS_RP8(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7)

#define CINT_FWRAPPER_PARAMS2_RP8(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP8(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7)  \
CINT_FWRAPPER_FUNCTION_RP8(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7)   \
CINT_FWRAPPER_PARAMS2_RP8(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7)

#define CINT_FWRAPPER_FUNCTION_RP9(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS9(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3,p4,p5,p6,p7,p8); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP9(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  { #pb8,#p8,pp8,pa8 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP9(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7,pt8,pb8,p8,pp8,pa8)  \
CINT_FWRAPPER_FUNCTION_RP9(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8)   \
CINT_FWRAPPER_PARAMS_RP9(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8)

#define CINT_FWRAPPER_PARAMS2_RP9(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  { #pb8,#p8,pp8,pa8,pf8 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP9(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7,pt8,pb8,p8,pp8,pa8,pf8)  \
CINT_FWRAPPER_FUNCTION_RP9(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8)   \
CINT_FWRAPPER_PARAMS2_RP9(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8)

#define CINT_FWRAPPER_FUNCTION_RP10(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS10(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3,p4,p5,p6,p7,p8,p9); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP10(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  { #pb8,#p8,pp8,pa8 }, \
  { #pb9,#p9,pp9,pa9 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP10(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7,pt8,pb8,p8,pp8,pa8,pt9,pb9,p9,pp9,pa9)  \
CINT_FWRAPPER_FUNCTION_RP10(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9)   \
CINT_FWRAPPER_PARAMS_RP10(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9)

#define CINT_FWRAPPER_PARAMS2_RP10(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  { #pb8,#p8,pp8,pa8,pf8 }, \
  { #pb9,#p9,pp9,pa9,pf9 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP10(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7,pt8,pb8,p8,pp8,pa8,pf8,pt9,pb9,p9,pp9,pa9,pf9)  \
CINT_FWRAPPER_FUNCTION_RP10(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9)   \
CINT_FWRAPPER_PARAMS2_RP10(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9)

#define CINT_FWRAPPER_FUNCTION_RP11(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10)  \
static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \
{ \
  CINT_FARGS11(pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10);  \
\
  CINT_FRET(rt) = fname (p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10); \
\
  CINT_FWRAPPER_END;\
  return 0; \
}

#define CINT_FWRAPPER_PARAMS_RP11(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9,pb10,p10,pp10,pa10)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0 }, \
  { #pb1,#p1,pp1,pa1 }, \
  { #pb2,#p2,pp2,pa2 }, \
  { #pb3,#p3,pp3,pa3 }, \
  { #pb4,#p4,pp4,pa4 }, \
  { #pb5,#p5,pp5,pa5 }, \
  { #pb6,#p6,pp6,pa6 }, \
  { #pb7,#p7,pp7,pa7 }, \
  { #pb8,#p8,pp8,pa8 }, \
  { #pb9,#p9,pp9,pa9 }, \
  { #pb10,#p10,pp10,pa10 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE_RP11(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pt1,pb1,p1,pp1,pa1,pt2,pb2,p2,pp2,pa2,pt3,pb3,p3,pp3,pa3,pt4,pb4,p4,pp4,pa4,pt5,pb5,p5,pp5,pa5,pt6,pb6,p6,pp6,pa6,pt7,pb7,p7,pp7,pa7,pt8,pb8,p8,pp8,pa8,pt9,pb9,p9,pp9,pa9,pt10,pb10,p10,pp10,pa10)  \
CINT_FWRAPPER_FUNCTION_RP11(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10)   \
CINT_FWRAPPER_PARAMS_RP11(rb,rp,ra,fname,pb0,p0,pp0,pa0,pb1,p1,pp1,pa1,pb2,p2,pp2,pa2,pb3,p3,pp3,pa3,pb4,p4,pp4,pa4,pb5,p5,pp5,pa5,pb6,p6,pp6,pa6,pb7,p7,pp7,pa7,pb8,p8,pp8,pa8,pb9,p9,pp9,pa9,pb10,p10,pp10,pa10)

#define CINT_FWRAPPER_PARAMS2_RP11(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9,pb10,p10,pp10,pa10,pf10)  \
static cint_parameter_desc_t __cint_parameters__##fname [] = \
{ \
  { #rb, "r", rp, ra }, \
  { #pb0,#p0,pp0,pa0,pf0 }, \
  { #pb1,#p1,pp1,pa1,pf1 }, \
  { #pb2,#p2,pp2,pa2,pf2 }, \
  { #pb3,#p3,pp3,pa3,pf3 }, \
  { #pb4,#p4,pp4,pa4,pf4 }, \
  { #pb5,#p5,pp5,pa5,pf5 }, \
  { #pb6,#p6,pp6,pa6,pf6 }, \
  { #pb7,#p7,pp7,pa7,pf7 }, \
  { #pb8,#p8,pp8,pa8,pf8 }, \
  { #pb9,#p9,pp9,pa9,pf9 }, \
  { #pb10,#p10,pp10,pa10,pf10 }, \
  CINT_ENTRY_LAST \
}

#define CINT_FWRAPPER_CREATE2_RP11(rt,rb,rp,ra,fname,pt0,pb0,p0,pp0,pa0,pf0,pt1,pb1,p1,pp1,pa1,pf1,pt2,pb2,p2,pp2,pa2,pf2,pt3,pb3,p3,pp3,pa3,pf3,pt4,pb4,p4,pp4,pa4,pf4,pt5,pb5,p5,pp5,pa5,pf5,pt6,pb6,p6,pp6,pa6,pf6,pt7,pb7,p7,pp7,pa7,pf7,pt8,pb8,p8,pp8,pa8,pf8,pt9,pb9,p9,pp9,pa9,pf9,pt10,pb10,p10,pp10,pa10,pf10)  \
CINT_FWRAPPER_FUNCTION_RP11(rt,fname,pt0,p0,pt1,p1,pt2,p2,pt3,p3,pt4,p4,pt5,p5,pt6,p6,pt7,p7,pt8,p8,pt9,p9,pt10,p10)   \
CINT_FWRAPPER_PARAMS2_RP11(rb,rp,ra,fname,pb0,p0,pp0,pa0,pf0,pb1,p1,pp1,pa1,pf1,pb2,p2,pp2,pa2,pf2,pb3,p3,pp3,pa3,pf3,pb4,p4,pp4,pa4,pf4,pb5,p5,pp5,pa5,pf5,pb6,p6,pp6,pa6,pf6,pb7,p7,pp7,pa7,pf7,pb8,p8,pp8,pa8,pf8,pb9,p9,pp9,pa9,pf9,pb10,p10,pp10,pa10,pf10)

#endif /* __CINT_WRAPPERS_H__ */

