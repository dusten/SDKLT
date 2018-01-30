#
# $Id: wrappergen.pl,v 1.9 2012/03/02 16:21:40 yaronm Exp $
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
# File:        wrappergen.pl
# Purpose:     CINT C wrapper generator
#




sub plist
  {
    my $i = shift;
    my @pre = @_;
    my @params = map { "$_$i" } @pre;
    return join ",", @params;
  }

sub pplist
  {
    my $num = shift;
    my @pre = @_;

    my $string;

    for(my $x = 0; $x < $num; $x++) {
      $string = $string . "," . plist($x, @pre);
    }
    return $string;
  }

sub ctrim
  {
    my $string = shift;
    $string =~ s/^,//;
    return $string;
  }

sub macro
  {
    my $define = shift;
    my $name = shift;
    my $returns = shift;
    my $fname = shift;
    my $pcount = shift;
    my @pre = @_;

    printf "%s${name}_%sP%d(%s%s%s)  %s",
      $define ? "#define " : "",
      defined($returns) ? "R" : "V",
        $pcount,
          $returns, $fname, pplist($pcount, @pre), $define ? "\\\n" : "";
  }


sub fwrapper_function
  {
    my ($returns, $count) = @_;
    my $rs;

    if($returns) {
      $rs = "rt,";
    }
    macro(1, "CINT_FWRAPPER_FUNCTION", $rs, "fname", $count, qw(pt p));
    print "static int __cint_fwrapper__##fname (cint_fparams_t* fparams) \\\n";
    print "{ \\\n";
    if($count) {
      printf "  CINT_FARGS%d(%s);  \\\n",
        $count, ctrim(pplist($count, qw(pt p)));
    }
    print "\\\n";
    if($returns) {
      print "  CINT_FRET(rt) = ";
    }
    else {
      print "  ";
    }
    my $list = ctrim(pplist($count, qw(p)));
    printf "fname (%s); \\\n", $list;
    print "\\\n";
    if($count) {
      print "  CINT_FWRAPPER_END;\\\n";
    }
    print "  return 0; \\\n";
    print "}\n\n";
  }

sub f_params
  {
    my ($name, $returns, $count) = @_;
    my $rs;
    if($returns) {
      $rs = "rb,rp,ra,";
    }
    macro(1, "CINT_${name}_PARAMS", $rs, "fname", $count, qw(pb p pp pa));
    print "static cint_parameter_desc_t __cint_parameters__##fname [] = \\\n";
    print "{ \\\n";
    if($returns) {
      print "  { #rb, \"r\", rp, ra }, \\\n";
    }
    else {
      print "  { \"void\", \"r\", 0, 0 }, \\\n";
    }
    for(my $i = 0; $i < $count; $i++) {
      my $list = ctrim(plist($i, qw(#pb #p pp pa)));
      print "  { $list }, \\\n";
    }
    print "  CINT_ENTRY_LAST \\\n";
    print "}\n\n";
  }

sub f_params2
  {
    my ($name, $returns, $count) = @_;
    my $rs;
    if($returns) {
      $rs = "rb,rp,ra,";
    }
    macro(1, "CINT_${name}_PARAMS2", $rs, "fname", $count, qw(pb p pp pa pf));
    print "static cint_parameter_desc_t __cint_parameters__##fname [] = \\\n";
    print "{ \\\n";
    if($returns) {
      print "  { #rb, \"r\", rp, ra }, \\\n";
    }
    else {
      print "  { \"void\", \"r\", 0, 0 }, \\\n";
    }
    for(my $i = 0; $i < $count; $i++) {
      my $list = ctrim(plist($i, qw(#pb #p pp pa pf)));
      print "  { $list }, \\\n";
    }
    print "  CINT_ENTRY_LAST \\\n";
    print "}\n\n";
  }

sub fwrapper_create
{
  my ($returns, $count) = @_;
  my $rs;
  if($returns) {
    $rs = "rt,rb,rp,ra,";
  }
  macro(1, "CINT_FWRAPPER_CREATE", $rs, "fname", $count, qw(pt pb p pp pa));
  macro(0, "CINT_FWRAPPER_FUNCTION", $returns ? "rt," : undef, "fname", $count, qw(pt p));
  printf(" \\\n");
  macro(0, "CINT_FWRAPPER_PARAMS", $returns ? "rb,rp,ra," : undef, "fname", $count, qw(pb p pp pa));
  printf("\n\n");
}

sub fwrapper_create2
{
  my ($returns, $count) = @_;
  my $rs;
  if($returns) {
    $rs = "rt,rb,rp,ra,";
  }
  macro(1, "CINT_FWRAPPER_CREATE2", $rs, "fname", $count, qw(pt pb p pp pa pf));
  macro(0, "CINT_FWRAPPER_FUNCTION", $returns ? "rt," : undef, "fname", $count, qw(pt p));
  printf(" \\\n");
  macro(0, "CINT_FWRAPPER_PARAMS2", $returns ? "rb,rp,ra," : undef, "fname", $count, qw(pb p pp pa pf));
  printf("\n\n");
}

sub farg_macro
  {
    my $count = shift;

    printf("#define CINT_FARGS%d(%s) \\\n", $count, ctrim(pplist($count, qw(pt p))));

    my $i;

    for($i = 0; $i < $count; $i++) {
      printf "    pt$i p$i; \\\n";
    }
    for($i = 0; $i < $count; $i++) {
      printf("    CINT_MEMCPY(\&p$i, fparams->args[$i], sizeof(p$i)); \\\n");
    }
    printf "\n\n";
  }

# split up dollar-id-dollar to avoid keyword substitution
print "/*\n * \$";
print "Id: \$\n";
my $sec, $min, $hr, $md, $mo, $year, $wday, $yday, $dst;
($sec, $min, $hr, $md, $mo, $year, $wday, $yday, $dst) = localtime(time);
$year += 1900;

my $dollar = "\$";
print <<EOF
 * ${dollar}Copyright: (c) $year Broadcom Corp.
 * All Rights Reserved.${dollar}
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

EOF
;

my $pcount = 12;

for(my $p = 0; $p < $pcount; $p++) {
  farg_macro($p);
}

for(my $returns = 0; $returns < 2; $returns++) {
  for(my $p = 0; $p < $pcount; $p++) {
    fwrapper_function($returns, $p);
    f_params("FWRAPPER", $returns, $p);
    fwrapper_create($returns, $p);
    f_params2("FWRAPPER", $returns, $p);
    fwrapper_create2($returns, $p);
  }
}

print "#endif /* __CINT_WRAPPERS_H__ */\n\n";
