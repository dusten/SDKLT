#
# \file issu_db_gen.py
#
# This script creates a C file that can provide the offset of each field in
# the data structure. The file also contain the database description of the
# given data structure.
#
# Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to Broadcom Limited and/or its subsidiaries.
# 
# Broadcom Switch Software License
# 
# This license governs the use of the accompanying Broadcom software. Your use of 
# the software indicates your acceptance of the terms and conditions of this 
# license. If you do not agree to the terms and conditions of this license, do not 
# use the software.
# 1. Definitions
#    "Licensor" means any person or entity that distributes its Work.
#    "Software" means the original work of authorship made available under this 
#    license.
#    "Work" means the Software and any additions to or derivative works of the 
#    Software that are made available under this license.
#    The terms "reproduce," "reproduction," "derivative works," and "distribution" 
#    have the meaning as provided under U.S. copyright law.
#    Works, including the Software, are "made available" under this license by 
#    including in or with the Work either (a) a copyright notice referencing the 
#    applicability of this license to the Work, or (b) a copy of this license.
# 2. Grant of Copyright License
#    Subject to the terms and conditions of this license, each Licensor grants to 
#    you a perpetual, worldwide, non-exclusive, and royalty-free copyright license 
#    to reproduce, prepare derivative works of, publicly display, publicly 
#    perform, sublicense and distribute its Work and any resulting derivative 
#    works in any form.
# 3. Grant of Patent License
#    Subject to the terms and conditions of this license, each Licensor grants to 
#    you a perpetual, worldwide, non-exclusive, and royalty-free patent license to 
#    make, have made, use, offer to sell, sell, import, and otherwise transfer its 
#    Work, in whole or in part. This patent license applies only to the patent 
#    claims licensable by Licensor that would be infringed by Licensor's Work (or 
#    portion thereof) individually and excluding any combinations with any other 
#    materials or technology.
#    If you institute patent litigation against any Licensor (including a 
#    cross-claim or counterclaim in a lawsuit) to enforce any patents that you 
#    allege are infringed by any Work, then your patent license from such Licensor 
#    to the Work shall terminate as of the date such litigation is filed.
# 4. Redistribution
#    You may reproduce or distribute the Work only if (a) you do so under this 
#    License, (b) you include a complete copy of this License with your 
#    distribution, and (c) you retain without modification any copyright, patent, 
#    trademark, or attribution notices that are present in the Work.
# 5. Derivative Works
#    You may specify that additional or different terms apply to the use, 
#    reproduction, and distribution of your derivative works of the Work 
#    ("Your Terms") only if (a) Your Terms provide that the limitations of 
#    Section 7 apply to your derivative works, and (b) you identify the specific 
#    derivative works that are subject to Your Terms. Notwithstanding Your Terms, 
#    this license (including the redistribution requirements in Section 4) will 
#    continue to apply to the Work itself.
# 6. Trademarks
#    This license does not grant any rights to use any Licensor's or its 
#    affiliates' names, logos, or trademarks, except as necessary to reproduce 
#    the notices described in this license.
# 7. Limitations
#    Platform. The Work and any derivative works thereof may only be used, or 
#    intended for use, with a Broadcom switch integrated circuit.
#    No Reverse Engineering. You will not use the Work to disassemble, reverse 
#    engineer, decompile, or attempt to ascertain the underlying technology of a 
#    Broadcom switch integrated circuit.
# 8. Termination
#    If you violate any term of this license, then your rights under this license 
#    (including the license grants of Sections 2 and 3) will terminate immediately.
# 9. Disclaimer of Warranty
#    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
#    EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
#    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR NON-INFRINGEMENT. 
#    YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER THIS LICENSE. SOME 
#    STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN IMPLIED WARRANTY, SO THIS 
#    DISCLAIMER MAY NOT APPLY TO YOU.
# 10. Limitation of Liability
#    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL THEORY, 
#    WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE SHALL ANY 
#    LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, INDIRECT, 
#    SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR RELATED TO 
#    THIS LICENSE, THE USE OR INABILITY TO USE THE WORK (INCLUDING BUT NOT LIMITED 
#    TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, LOST PROFITS OR DATA, COMPUTER 
#    FAILURE OR MALFUNCTION, OR ANY OTHER COMMERCIAL DAMAGES OR LOSSES), EVEN IF 
#    THE LICENSOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
# 

import os
import csv
import hashlib    # Support md5 hashing
import filecmp
import tempfile
import shutil
from shutil import copyfile

# Defines structure DB entry.
class struct_db_entry:
    name = ''         # Structure name.
    field_info = ''   # Variable name of the field information.
    struct_sig = ''   # Structure signature, used to identify structure changes.

def prepare_generic_output_file_header(fh, header_file):
    fh.write('/*************************************************************************\n')
    fh.write(' *\n')
    fh.write(' * DO NOT EDIT THIS FILE!\n')
    fh.write(' * This file is auto-generated by HA parser from YAML formated file.\n')
    fh.write(' * Edits to this file will be lost when it is regenerated.\n')
    fh.write(' *\n')
    fh.write(' * $Copyright:.$\n')
    fh.write(' *\n')
    fh.write(' *************************************************************************/\n')
    fh.write('\n')
    fh.write('#include <bcmissu/issu_types.h>\n')
    if header_file != None:
        fh.write('#include \"%s\"\n' % header_file)


# This class maintains a database of all the data structures that were generated
# by the component module class. The database is being maintained as a text file
# named struct_def.db. The DB file is sorted based on the structure ID (digest
# of the structure name). It is assumed that the structure ID remains identical
# throughout the life cycle of the structure. The last element of the DB is
# the structure signature. This signature is being generated as a digest of all
# the fields of the structure (including their type). Therefore if any type
# or field will changed, the signature will change as well and the parser will
# know that this structure has changed.
class global_struct_db:
    """Maintains the global structure DB."""

    # The initializer reads the content of the structure DB into local
    # dictionary.
    def __init__(self, base_dir, header_file):
        self.base_dir = base_dir
        self.older_struct_dict = {}
        self.temp_struct_dict = {}
        self.version_list = []
        self.output_header_file = header_file
        self.file_name = base_dir + '/struct_def.db'
        self.struct_id_dict = {}
        self.out_fh = None
        try:
            fh = open(self.file_name, 'r')
        except:
            print 'file %s not found' % self.file_name
            return
        # Read the content of the file into csv dictionary
        csvdict = csv.DictReader(fh, fieldnames=['id','name',
                                                 'field_info','struct_sig'])
        for rec in csvdict:
            ent = struct_db_entry()
            ent.name = rec['name']
            ent.field_info = rec['field_info']
            ent.struct_sig = rec['struct_sig']
            self.struct_id_dict[rec['id']] = ent

        fh.close()

    def __del__(self):
        try:
            fh = open(self.file_name, 'w')
        except:
            return

        # Write the content of the dictionary (sorted based on struct id) back
        # into the DB file
        for id in sorted(self.struct_id_dict.iterkeys()):
            ent = self.struct_id_dict[id]
            fh.write('%s,%s,%s,%s\n' %
                          (id, ent.name, ent.field_info, ent.struct_sig))
        fh.close()

        if self.out_fh != None:
            # Declare the global version list
            self.out_fh.write('\nconst issu_version_list_t issu_struct_version[] = {\n')
            # If there are no versions simply place the current list there
            if len(self.version_list) == 0:
                self.out_fh.write('    {\n')
                self.out_fh.write('        .version = \"None\",\n')
                self.out_fh.write('        .ver_db = issu_struct_db_\n')
                self.out_fh.write('    }\n')
            else:
                for ver in self.version_list:
                    self.out_fh.write('    {\n')
                    self.out_fh.write('        .version = \"%s\",\n' % ver)
                    self.out_fh.write('        .ver_db = issu_struct_db_%s\n' % ver)
                    self.out_fh.write('    },\n')
            self.out_fh.write('};\n')

            self.out_fh.close()

    def add_struct_info(self, id, ent):
        if '0x' + id in self.struct_id_dict:
            return

        entry = struct_db_entry()
        entry.struct_sig = '0x' + ent.struct_sig
        entry.field_info = ent.field_info
        entry.name = ent.name
        self.struct_id_dict['0x' + id] = entry

    def generate_c_db(self, ver, version_list):
        try:
            fh = open(self.base_dir + '/struct_def.c', 'w')
        except:
            return False

        self.out_fh = fh
        self.version_list = version_list
        prepare_generic_output_file_header(fh, self.output_header_file)

        # Start by declaring all the external data strcutrures
        fh.write('\n')
        for id in self.struct_id_dict.iterkeys():
            ent = self.struct_id_dict[id]
            fh.write('extern issu_struct_t %s;\n' % ent.field_info)

        fh.write('\nstatic const issu_struct_db_t issu_struct_db_%s[] = {\n' % ver)
        for id in self.struct_id_dict.iterkeys():
            ent = self.struct_id_dict[id]
            fh.write('    {\n')
            fh.write('        .id = %s,\n' % id)
            fh.write('        .struct_info = &%s\n' % ent.field_info)
            fh.write('    },\n')
        fh.write('};\n')
        return True;

    def __add_struct_older_ver(self, rec):
        # First let check that the structure was not already included in the
        # changed structure list. If it is in the changed list verify that
        # the structure signature is different. Return if it is the same.
        if rec['id'] in self.older_struct_dict:
            entry = self.older_struct_dict[rec['id']]
            if entry.struct_sig == rec['struct_sig']:
                return

        # Add the structure into the older structure list
        ent = struct_db_entry()
        ent.name = rec['name']
        ent.field_info = rec['field_info']
        ent.struct_sig = rec['struct_sig']
        self.older_struct_dict[rec['id']] = ent

        # Add the structure to the temporary dictionary
        self.temp_struct_dict[rec['id']] = ent

    def __update_with_version(self, ver):
        fh = self.out_fh
        # Start by declaring all the external data strcutrures
        fh.write('\n')
        for id in self.temp_struct_dict.iterkeys():
            ent = self.temp_struct_dict[id]
            fh.write('extern issu_struct_t %s;\n' % ent.field_info)

        fh.write('\nstatic const issu_struct_db_t issu_stuct_db_%s[] = {\n' % ver)
        for id in self.temp_struct_dict.iterkeys():
            ent = self.temp_struct_dict[id]
            fh.write('    {\n')
            fh.write('        .id = %s,\n' % id)
            fh.write('        .struct_info = &%s\n' % ent.field_info)
            fh.write('    },\n')
        fh.write('};')


    # It is important that this function will be called with decending version
    # numbers (i.e. start from the latest version backward.
    def parse_previous_versions(self, base_dir, ver):
        file_name = base_dir + ver + '/struct_def.db'
        try:
            fh = open(file_name, 'r')
        except:
            print 'file %s not found' % file_name
            return False

        # Clear the temporary dictionary
        self.temp_struct_dict.clear()
        # Read the content of the file into csv dictionary
        csvdict = csv.DictReader(fh, fieldnames=['id','name',
                                                 'field_info','struct_sig'])
        for rec in csvdict:
            # A structure require migration if it still exist and its signature
            # had changed.
            if rec['id'] in self.struct_id_dict:
                ent = self.struct_id_dict[rec['id']]
                if ent.struct_sig != rec['struct_sig']:
                    self.__add_struct_older_ver(rec)

        # Last add the version changes to the C file
        self.__update_with_version(ver)
        return True

# Defines active structure class.
class struct_entry:
    id = ''           # Structure ID (64 bit unique).
    field_info = ''   # Variable name of the field information.
    struct_sig = ''   # Structure signature, used to identify structure changes.


# This class is a singleton class that handles the processing of a complete
# yaml file structure definition. The class is generating a C file that contains
# the necessary structure information for every structure defined in the yaml
# file.
class component_module:
    """Handles file creation and generic file content"""

    struct_dict = {}
    def __init__(self, sdk_dir, ver, component, out_header):
        if ver != '':
            self.base_dir = sdk_dir + '/bcmissu/db/ver_' + ver
        else:
            self.base_dir = sdk_dir + '/bcmissu/db/current'
        # Create the directory if not exist
        if not os.path.isdir(self.base_dir):
            try:
                os.makedirs(self.base_dir)
            except OSError:
                print 'Target directory exist\n'
        self.file_name = self.base_dir  + '/' + component + '.c'
        self.tmp_file = tempfile.mktemp('.gen')
        try:
            self.fh = open (self.tmp_file, 'w')
        except:
            print 'Error: Failed to open temp file\n'
            return
        self.version = ver
        idx = out_header.rfind('/')
        if idx == -1:
            self.output_header_file = out_header
        else:
            self.output_header_file = out_header[idx+1:]
        prepare_generic_output_file_header(self.fh, self.output_header_file)
        self.db_file_gen = global_struct_db(self.base_dir, self.output_header_file)

    def __del__(self):
        ent = struct_db_entry()
        # For every structure processed, inform the structure DB class about
        # the structure.
        for name in self.struct_dict:
            st = self.struct_dict[name]
            ent.name = name
            ent.field_info = st.field_info
            ent.struct_sig = st.struct_sig
            self.db_file_gen.add_struct_info(st.id, ent)
        # Need to close the C file
        self.fh.close()
        # If the destination file does not exist or different from the temporary
        # file then move the temporary file over. Otherwise the temporary file
        # is not required (since identical file already present) so delete it.
        if (not os.path.isfile(self.file_name)) or \
           (not filecmp.cmp(self.tmp_file, self.file_name)):
            print 'Moving %s to %s\n' % (self.tmp_file, self.file_name)
            shutil.move(self.tmp_file, self.file_name)
        else:
            print 'Nothing changed'
            os.remove(self.tmp_file)


    def save_generated_header(self, header_file):
        # Verify that the source header file exist
        if not os.path.isfile(header_file):
            print 'Error: Missing header file %s\n' % header_file
            return
        # Verify that the destination directory exist
        if os.path.exists(self.base_dir):
            dest_file = self.base_dir + '/' + self.output_header_file
            copyfile(header_file, dest_file)

class field_entry:
    name = ''
    id = ''
    ftype = ''
    is_array = False
    is_table = False
    table_size_var = None
    array_size = None
    struct_id = ''

# This class handles a single data structure.
class structure_db:
    """Class that handles data structures for ISSU purpose"""

    # Initializing the class. The comp_cls is a class that handles the entire
    # component and is of type component_module.
    def __init__(self, file_name, struct_name, struct_id, comp_cls):
        self.struct_name = struct_name
        self.struct_id = struct_id
        idx = file_name.rfind('/')
        clean_file_name = file_name[idx+1:file_name.find('.')-1]
        self.external_struct_name = 'bcmissu_struct_' + clean_file_name + \
                                    '_' + struct_name + '_v' + comp_cls.version
        self.fields = []
        self.comp_class = comp_cls
        st_ent = struct_entry()
        st_ent.id = struct_id
        st_ent.field_info = self.external_struct_name
        comp_cls.struct_dict[struct_name] = st_ent

    def __create_struct_definition(self, fh):
        fh.write('\nstatic const issu_field_t bcmissu_fields_%s[] = {\n' %
                 self.struct_name)
        for entry in self.fields:
            fh.write('    {\n')
            fh.write('        .fid = 0x%s,\n' % entry.id)
            fh.write('        .width = sizeof(%s),\n' % entry.ftype)
            if entry.is_array or entry.is_table:
                 fh.write('        .is_array = true,\n')
                 if entry.array_size:
                     fh.write('        .size = %s,\n' % str(entry.array_size))
                     fh.write('        .var_size_id = 0,\n')
                 else:
                     fh.write('        .size = 0,\n')
                     for e in self.fields:
                         if e.name == entry.table_size_var:
                             fh.write('        .var_size_id = 0x%s,\n' % e.id)
                             break
            else:
                 fh.write('        .is_array = false,\n')
                 fh.write('        .var_size_id = 0,\n')
                 fh.write('        .size = 0,\n')

            if entry.ftype in self.comp_class.struct_dict:
                fh.write('        .struct_id = 0x%s,\n' % entry.struct_id)
            else:
                fh.write('        .struct_id = %s,\n' % entry.struct_id)

            fh.write('    },\n')

        fh.write('};\n')

    def __generate_offset_calc_func(self, fh):
        fh.write('\nstatic uint32_t %s_field_offset_get (uint32_t field_id)\n'
                 % self.struct_name)
        fh.write('{\n')
        fh.write('    %s var;\n\n' % self.struct_name)
        fh.write('    switch (field_id) {\n')
        for entry in self.fields:
            fh.write('        case 0x%s:\n' % entry.id)
            fh.write('            return ((uint8_t *)&var.%s) - (uint8_t *)&var;\n'
                      % entry.name)
        fh.write('        default:\n')
        fh.write('            return 0xFFFFFFFF;\n')
        fh.write('    }\n')
        fh.write('}\n')

    def __finalize_struct_info(self, fh):
        fh.write('\nconst issu_struct_t %s = {\n' % self.external_struct_name)
        fh.write('    .offset_get_func = %s_field_offset_get,\n' %
                 self.struct_name)
        fh.write('    .generic_size = sizeof(%s),\n' % self.struct_name)
        fh.write('    .field_count = %s,\n' % len (self.fields))
        fh.write('    .fields = bcmissu_fields_%s,\n' % self.struct_name)
        fh.write('};\n')


    def __calc_struct_signature(self):
        m = hashlib.md5()
        for entry in self.fields:
            m.update(entry.name + entry.ftype)

        st = self.comp_class.struct_dict[self.struct_name]
        st.struct_sig = m.hexdigest()[:16]
#        del (self.comp_class.struct_dict[self.struct_name])
        self.comp_class.struct_dict[self.struct_name] = st

    def add_field(self, field, ftype, is_array, array_size, is_table, table_size_var):
        m = hashlib.md5()
        m.update(field)

        entry = field_entry()
        idx = field.find('[')
        if idx == -1:
            entry.name = field
        else:
            entry.name = field[:idx]
        entry.id = m.hexdigest()[:8]
        entry.ftype = ftype
        entry.is_array = is_array
        entry.is_table = is_table
        entry.array_size = array_size
        entry.table_size_var = table_size_var
        if ftype in self.comp_class.struct_dict:
            entry.struct_id = self.comp_class.struct_dict[ftype].id
        else:
            entry.struct_id = 'ISSU_INVALID_STRUCT_ID'
        self.fields.append(entry)

    def output_to_file(self):
        self.__calc_struct_signature()

        self.__create_struct_definition(self.comp_class.fh)

        self.__generate_offset_calc_func(self.comp_class.fh)

        self.__finalize_struct_info(self.comp_class.fh)

        return True
