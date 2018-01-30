#
# \file ha_yaml_parser.py
#
# Parsing YAML file describing HA data structure and generating
# 1. Header file to be included by the HA application
# 2. Data structure migration code from one version to higher one
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
import sys
import os
import getopt
import yaml        # YAML parser
from collections import OrderedDict   # keep YAML parser output in order
import hashlib    # support md5 hashing
import issu_db_gen
import tempfile
import os.path
import shutil
import filecmp

# This is a list of known types. Newly defined types are added to this list
# during parsing. The list is used to validate the type used.
variable_type_list = {'void', 'int', 'char', 'short', 'long', 'bool',
                      'unsigned', 'unsigned char', 'unsigned short',
                      'unsigned long', 'long long', 'unsigned long long',
                      'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t',
                      'int8_t', 'int16_t', 'int32_t', 'int64_t',
                      'float', 'double'}
const_dict = {}

output_file = ''

comp_cls = None

# Validate type. Type might be one or two segments. The first must be
# one of the types defined in variable_type_list and the second (if exist)
# must be * (for pointer)
def validate_type (var_type):
    global variable_type_list

    # remove pointer if exist
    segments = var_type.split()
    n_of_elem = len(segments)
    if n_of_elem > 2:
        print "too many elements in type %s" % var_type
        return False
    elif (n_of_elem == 2) and (segments[1] == '*'):
        return segments[0] in variable_type_list
    return var_type in variable_type_list

# Add new data type into the list of 'known' types (variable_type_list)
def update_valid_types (data_type):
    global variable_type_list
    variable_type_list.add(data_type)

# Parse structure attribute block.
# Params:
# elem is the attribute object
# struct_name is the structure typedef name
# out_fh is the output file handler
def struct_attrib_parse(elem, struct_name, out_fh):
    for val in elem:
        if val == 'version':
            if type(elem[val]) != int:
                print "Syntax error version %s must be integer" % elem[val]
                return False
    return True

# Parse enum structure.
# Params:
# elem is the enum object
# name is the enum typedef name
# out_fh is the output file handler
def enum_parse(elem, name, items, out_fh):
    # sanity check - verify that the type is not already defined
    if validate_type(name):
        print 'error: type %s already defined\n' % name
        return False

    out_fh.write('typedef enum {\n')
    idx = 1
    for val in elem:
        words = val.split('=')
        if words[0] != words[0].upper():
            print 'enum values must be in upper case'
            return false
        line = ''
        if len(words) == 1:
            line = '   ' + words[0]
        elif len(words) == 2:
            line = '   ' + words[0] + ' = ' + words[1]

        if idx < items:   # last element doesn't followed with comma
            line = line + ','

        out_fh.write('%s\n' % line)
        idx = idx + 1
        if len(words) == 1:
            out_fh.write('\n')
    out_fh.write('} %s;\n' % name)

    # update the valid types with the new structure
    update_valid_types (name)
    return True

# Process field attributes object
# This function process the field attributes object. As a result of processing
# the function sets the following global parameters:
# is_table - Indicating if the field is a table.
# is_array - Indicating if the field is an array.
# array_size - If the field is an array specifies the array size.
# table_size_var - The variable name that its value indicates the number of
# elements in the table. This variable only set if is_table == True
# Params:
# attr is the field attribute object
# comment is a list where the comment string can be added if available
def process_field_attrib(attr, comment):
    global is_table
    global is_array
    global array_size
    global table_size_var
    global is_volatile
    global const_dict

    field_type = None
    table_size_var = None
    for field in attr:
        if field == 'type':
            field_type = attr[field]
        elif field == 'length':
            is_array = True
            if type(attr[field]) == str and attr[field] not in const_dict:
                print 'undefined const %s' % attr[field]
                return None
            array_size = attr[field]
        elif field == 'comment':
            comment.append('/* ' + attr[field] + ' */')
        elif field == 'volatile':
            is_volatile = True
        elif field == 'table':
            is_table = True
        elif field == 'size':
            table_size_var = attr[field]
        else:
            print 'unknown field attribute ', field
            return None
    return field_type

# Process field.
# Params:
# field - Is the field type + comment (if present).
# comment is a list where the comment string can be added if available.
def process_field(field, comment):
    idx = field.find('/*')
    if idx != -1:
        comment.append(field[idx:])

    words = field.split()
    if len(words) > 1 and words[1] == '*':
        return words[0] + ' ' + words[1]
    else:
        return words[0]

# Process the object 'fields'.
# Params:
# fl - Is the field object.
# out_fh - Is the output file handler.
def struct_fields_parse(fl, out_fh, sdb):
    global is_table
    global is_array
    global array_size
    global table_size_var
    global is_volatile

    for field in fl:
        comment = ['']
        is_table = False
        is_array = False
        array_size = None
        is_volatile = False

        if type(fl[field]) == OrderedDict: # Var has attached attributes
            data_type = process_field_attrib(fl[field], comment)
            if not data_type:
                print 'Missing type for ', field
                return False
        else:
            data_type = process_field(fl[field], comment)

        if (not validate_type(data_type)):  # validate data type
            print "invalid type %s" % data_type
            return False

        if is_array:
            if not array_size:
                print 'Invalid array size for field ', field
                return False
            else:
                field = field + '[' + str(array_size) + ']'
        elif is_table:
            if not table_size_var:
                print 'Missing variable to indicate table length for ', field
                return False
            else:
                field = field + '[]'

        if data_type.find('*') != -1:
            is_volatile = True
            out_fh.write('   %s%s;' % (data_type, field))
        else:
            out_fh.write('   %s %s;' % (data_type, field))
        if len(comment) > 1:
            out_fh.write(' %s\n' % comment[1])
        else:
            out_fh.write('\n')

        # Update the ISSU class with the field information
        sdb.add_field(field, data_type, is_array, array_size,
                      is_table, table_size_var)

    return True

# Process structure object.
# Params:
# fl - Is the structure object.
# name - Is the name of the data structure.
# file_name - Is the name of the input file. It is used as part of the structure
# ID.
# out_fh - Is the output file handler.
def struct_parse(fl, name, file_name, out_fh, comp_cls):

    # sanity check - verify that the type is not already defined
    if validate_type(name):
        print 'error: type %s already defined\n' % name
        return False

    m = hashlib.md5()
    m.update(file_name + name)

    sdb = issu_db_gen.structure_db(file_name, name, m.hexdigest()[:16], comp_cls)

    out_fh.write('typedef struct {\n')
    for field in fl:
        if field == 'attrib':
            attrib_field = fl[field]
        elif field == 'fields':
            if not struct_fields_parse(fl[field], out_fh, sdb):
                return False
        elif field == 'type':
            continue    # type was processed before
        else:
            print 'unknown key ', field
            return False

    out_fh.write('} %s;\n\n' % name)

 #   struct_attrib_parse(attrib_field, name, out_fh)
    out_fh.write('#define %s_ID 0x%s\n' %
                 (name.upper(), m.hexdigest()[:16]))

    # update the valid types with the new structure
    update_valid_types (name)

    return sdb.output_to_file()

# Find data type of a data type decleration
# Params:
# fl - Is the structure object.
def find_data_type(fl):
    rv = None
    for item in fl:
        if item == 'type':
            rv = fl[item]
            break
    return rv

# Process constant definition and add the constant and its value into
# a global dictionary.
# Params:
# fl - Is the structure object.
def process_constants(fl):
    global const_dict

    constants = ''
    for item in fl:
        if item != item.upper():
            print 'Constants musts be upper case %s' % item
            return ''
        constants = constants + '#define ' + item + ' ' + str(fl[item]) + '\n'
        const_dict[item] = fl[item]
    return constants

# Write the fixed header to the output file.
# Params:
# out_fh - Is the output file handler
def write_output_banner(out_fh):
    out_fh.write('/*************************************************************************\n')
    out_fh.write(' *\n')
    out_fh.write(' * DO NOT EDIT THIS FILE!\n')
    out_fh.write(' * This file is auto-generated by HA parser from YAML formated file.\n')
    out_fh.write(' * Edits to this file will be lost when it is regenerated.\n')
    out_fh.write(' *\n')
    out_fh.write(' * $Copyright:.$\n')
    out_fh.write(' *\n')
    out_fh.write(' *************************************************************************/\n')


# Standard yaml.load mixes the order of the dictionary so it doesn't
# match the order in the YAML file. This loader makes the trick.
def ordered_load(stream, Loader=yaml.Loader, object_pairs_hook=OrderedDict):
    class OrderedLoader(Loader):
        pass
    def construct_mapping(loader, node):
        loader.flatten_mapping(node)
        return object_pairs_hook(loader.construct_pairs(node))
    OrderedLoader.add_constructor(
        yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG, construct_mapping)
    return yaml.load(stream, OrderedLoader)

# Top level function to parse the entire xml file. It start processing the
# tree where every child tag being processed separately.
def parse_file(file_name, out_fh, nest_level, ver):
    global output_file
    global comp_cls

    try:
        file = open(file_name, 'r')
    except:
        print 'invalid source file ', file_name
        return False

    root = ordered_load(file)

    constants = ''

    for declr in root:
        if declr == 'consts':
            constants = process_constants(root[declr])
        elif declr == 'include':
            parse_file(root[declr], out_fh, nest_level+1, ver)
        else:
            if nest_level == 0:
                if declr != 'component':
                    print "%s Expected component <name> for component decleration\n" % declr
                    return False
                if len(root[declr]) != 1:
                    print "expecting single component decleration found %d\n" % len(root[declr])
                    return False
                component = root[declr]
                for elem in component:
                    break

                comp_cls = issu_db_gen.component_module(os.environ['SDK'], ver,
                                                        elem, output_file)
                tree = component[elem]
            else:
                tree = root[declr]

            if len(constants) > 1:
                out_fh.write ('%s\n' % constants)

            for items in tree:
                data_type = find_data_type(tree[items])
                if data_type == None:
                    print "missing type for data decleration"
                    return False

                if data_type == 'enum':
                    rv = enum_parse(tree[items], items,
                                    len(tree[items]), out_fh)
                elif data_type == 'struct':
                    rv = struct_parse(tree[items], items, file_name,
                                      out_fh, comp_cls)
                else:
                    print "unknown decleration word %s. Expected struct \
                        or enum\n" % data_type
                    rv = False
                if not rv:
                    return False

                out_fh.write('\n')

    return True

def usage():
    print "python ha_yml_parser.py -g <yaml-file> -o <output-header-file>\
          -v <version>"


# The main function.
# Parse input parameters, open output file and call the file
# parser.
def main(argv):
    global comp_cls
    yaml_file = ''
    ver = ''
    global output_file
    tmp_file = ''

    try:
        opts, args = getopt.getopt(argv, "hg:o:v:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-g':
            yaml_file = arg
        elif opt == '-o':
            output_file = arg
        elif opt == '-v':
            ver = arg
        else:
            usage()
            sys.exit(0);

    if yaml_file == '' or output_file == '':
        usage()
        sys.exit(0);

    if ver == '':
        try:
            ver = os.environ['SDK_VERSION']
        except:
            ver = ''

    # The output file is generated as temp file first.
    tmp_file = tempfile.mktemp('.gen')
    try:
        out_fh = open (tmp_file, 'w')
    except:
        print 'Error: Failed to open temp file\n'
        return

    # Write the banner of the output file first
    write_output_banner(out_fh)

    index = output_file.rfind('/')
    if index == -1:
        header_protect = output_file.upper()
    else:
        header_protect = output_file[index+1:].upper()
    header_protect = header_protect.replace('.', '_')

    # Protect the header file from being included multiple times
    out_fh.write ("#ifndef %s\n" % header_protect)
    out_fh.write ("#define %s\n\n" % header_protect)

    rv = parse_file(yaml_file, out_fh, 0, ver)

    out_fh.write ("#endif /* %s */\n" % header_protect)

    out_fh.close()
    if not rv:
        os.remove(tmp_file)
        sys.exit(-1)
    else:
        # If the destination file does not exist or different from the temporary
        # file then move the temporary file over. Otherwise the temporary file
        # is not required (since identical file already present) so delete it.
        if (not os.path.isfile(output_file)) or \
           (not filecmp.cmp(tmp_file, output_file)):
            shutil.move(tmp_file, output_file)
        else:
            os.remove(tmp_file)

        comp_cls.save_generated_header(output_file)

if __name__ == "__main__":
    main(sys.argv[1:])

