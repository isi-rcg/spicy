# -*- python -*-
#
# gtk-doc - GTK DocBook documentation generator.
# Copyright (C) 1998  Damon Chaplin
#               2007-2016  Stefan Sauer
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

"""
Extracts declarations of functions, macros, enums, structs and unions from
header files.

It is called with a module name, an optional source directory, an optional
output directory, and the header files to scan.

It outputs all declarations found to a file named '$MODULE-decl.txt', and the
list of decarations to another file '$MODULE-decl-list.txt'.

This second list file is typically copied to '$MODULE-sections.txt' and
organized into sections ready to output the XML pages.
"""

import logging
import os
import re
import shutil

from . import common

TYPE_MODIFIERS = ['const', 'signed', 'unsigned', 'long', 'short', 'struct', 'union', 'enum']
VAR_TYPE_MODIFIER = '(?:' + '|'.join([t + '\s+' for t in TYPE_MODIFIERS]) + ')*'
RET_TYPE_MODIFIER = '(?:' + '|'.join([t + '\s+' for t in TYPE_MODIFIERS + ['G_CONST_RETURN']]) + ')*'

# Matchers for current line
CLINE_MATCHER = [
    # 0: MACROS
    re.compile(
        r"""^\s*\#\s*define\s+
        (\w+)               # 1: name
        """, re.VERBOSE),
    # 1-4: TYPEDEF'D FUNCTIONS
    re.compile(
        r"""^\s*typedef\s+
        (%s\w+)                              # 1: return type
        (\s+const)?\s*                       # 2: 2nd const
        (\**)\s*                             # 3: ptr
        \(\*\s*
          (\w+)                              # 4: name
        \)\s*\(""" % RET_TYPE_MODIFIER, re.VERBOSE),
    re.compile(
        r"""^\s*
        (%s?\w+)                             # 1: return type
        (\s+const)?\s*                       # 2: 2nd const
        (\**)\s*                             # 3: ptr
        \(\*\s*
          (\w+)                              # 4: name
        \)\s*\(""" % RET_TYPE_MODIFIER, re.VERBOSE),
    re.compile(
        r"""^\s*
        (\**)\s*                             # 1: ptr
        \(\*\s*
          (\w+)                              # 2: name
        \)\s*\(""", re.VERBOSE),
    # 4: FUNCTION POINTER VARIABLES
    None,  # in InitScanner()
    # 5-7: ENUMS
    re.compile(
        r"""^\s*enum\s+
        _?(\w+)                              # 1: name
        \s+\{""", re.VERBOSE),
    None,  # in InitScanner()
    re.compile(r'^\s*typedef\s+enum'),
    # 8-11: STRUCTS AND UNIONS
    None,  # in InitScanner()
    re.compile(r'^\s*(?:struct|union)\s+_(\w+)\s*;'),
    re.compile(
        r"""^\s*
        (struct|union)\s+                    # 1: struct/union
        (\w+)                                # 2: name
        \s*;""", re.VERBOSE),
    re.compile(
        r"""^\s*typedef\s+
        (struct|union)\s*
        \w*\s*{""", re.VERBOSE),
    # 12-14: OTHER TYPEDEFS
    re.compile(
        r"""^\s*typedef\s+
        (?:struct|union)\s+\w+[\s\*]+
        (\w+)                                # 1: name
        \s*;""", re.VERBOSE),
    re.compile(
        r"""^\s*
        (?:G_GNUC_EXTENSION\s+)?
        typedef\s+
        (.+[\s\*])                           # 1: e.g. 'unsigned int'
        (\w+)                                # 2: name
        (?:\s*\[[^\]]+\])*
        \s*;""", re.VERBOSE),
    re.compile(r'^\s*typedef\s+'),
    # 15: VARIABLES (extern'ed variables)
    None,  # in InitScanner()
    # 16: VARIABLES
    re.compile(
        r"""^\s*
        (?:%s\w+)
        (?:\s+\*+|\*+|\s)\s*
        (?:const\s+)*
        ([A-Za-z]\w*)                        # 1: name
        \s*\=""" % VAR_TYPE_MODIFIER, re.VERBOSE),
    # 17: G_DECLARE_*
    re.compile(
        r""".*G_DECLARE_
        (FINAL_TYPE|DERIVABLE_TYPE|INTERFACE) # 1: variant
        \s*\(""", re.VERBOSE),
    # 18-21: FUNCTIONS
    None,  # in InitScanner()
    None,  # in InitScanner()
    re.compile(r'^\s*\(?([A-Za-z]\w*)\)?\s*\('),
    re.compile(r'^\s*\('),
    # 22-23: STRUCTS
    re.compile(r'^\s*struct\s+_?(\w+)\s*\*'),
    re.compile(r'^\s*struct\s+_?(\w+)'),
    # 24-25: UNIONS
    re.compile(r'^\s*union\s+_(\w+)\s*\*'),
    re.compile(r'^\s*union\s+_?(\w+)'),
]

# Matchers for previous line
PLINE_MATCHER = [
    # 0-1: TYPEDEF'D FUNCTIONS
    re.compile(
        r"""^\s*typedef\s*
        (%s\w+)                              # 1: return type
        (\s+const)?\s*                       # 2: 2nd const
        (\**)\s*                             # 3: ptr
        """ % RET_TYPE_MODIFIER, re.VERBOSE),
    re.compile(r'^\s*typedef\s*'),
    # 2-4 :FUNCTIONS
    None,  # in InitScanner()
    None,  # in InitScanner()
    None,  # in InitScanner()
]

# Matchers for 2nd previous line
PPLINE_MATCHER = None

# Matchers for sub expressions
SUB_MATCHER = [
    # 0: STRUCTS AND UNIONS
    re.compile(r'^(\S+)(Class|Iface|Interface)\b'),
]


def Run(options):
    logging.info('options: %s', str(options.__dict__))

    InitScanner(options)

    if not os.path.isdir(options.output_dir):
        os.mkdir(options.output_dir)

    base_filename = os.path.join(options.output_dir, options.module)
    old_decl_list = base_filename + '-decl-list.txt'
    new_decl_list = base_filename + '-decl-list.new'
    old_decl = base_filename + '-decl.txt'
    new_decl = base_filename + '-decl.new'
    old_types = base_filename + '.types'
    new_types = base_filename + '.types.new'
    sections_file = base_filename + '-sections.txt'

    # If this is the very first run then we create the .types file automatically.
    if not os.path.exists(sections_file) and not os.path.exists(old_types):
        options.rebuild_types = True

    section_list = {}
    decl_list = []
    get_types = []

    # do not read files twice; checking it here permits to give both srcdir and
    # builddir as --source-dir without fear of duplicities
    seen_headers = {}

    for file in options.headers:
        ScanHeader(file, section_list, decl_list, get_types, seen_headers, options)

    for dir in options.source_dir:
        ScanHeaders(dir, section_list, decl_list, get_types, seen_headers, options)

    with open(new_decl_list, 'w', encoding='utf-8') as f:
        for section in sorted(section_list.keys()):
            f.write(section_list[section])
    common.UpdateFileIfChanged(old_decl_list, new_decl_list, True)

    with open(new_decl, 'w', encoding='utf-8') as f:
        for decl in decl_list:
            f.write(decl)
    common.UpdateFileIfChanged(old_decl, new_decl, True)

    if options.rebuild_types:
        with open(new_types, 'w', encoding='utf-8') as f:
            for func in sorted(get_types):
                f.write(func + '\n')

        # remove the file if empty
        if len(get_types) == 0:
            os.unlink(new_types)
            if os.path.exists(old_types):
                os.rename(old_types, old_types + '.bak')
        else:
            common.UpdateFileIfChanged(old_types, new_types, True)

    # If there is no MODULE-sections.txt file yet or we are asked to rebuild it,
    # we copy the MODULE-decl-list.txt file into its place. The user can tweak it
    # later if they want.
    if options.rebuild_sections or not os.path.exists(sections_file):
        new_sections_file = base_filename + '-sections.new'
        shutil.copyfile(old_decl_list, new_sections_file)
        common.UpdateFileIfChanged(sections_file, new_sections_file, False)

    # If there is no MODULE-overrides.txt file we create an empty one
    # because EXTRA_DIST in gtk-doc.make requires it.
    overrides_file = base_filename + '-overrides.txt'
    if not os.path.exists(overrides_file):
        open(overrides_file, 'w', encoding='utf-8').close()


def InitScanner(options):
    """Apply options to regexps.
    """

    # avoid generating regex with |'' (matching no string)
    # TODO(ensonic): keep in sync with ScanHeaderContent()
    ignore_decorators = ''
    optional_decorators_regex = ''
    if options.ignore_decorators:
        ignore_decorators = '|' + options.ignore_decorators.replace('()', '\(\w*\)')
        optional_decorators_regex = '(?:\s+(?:%s))?' % ignore_decorators[1:]

    # FUNCTION POINTER VARIABLES
    CLINE_MATCHER[4] = re.compile(
        r"""^\s*(?:\b(?:extern|static|inline|G_INLINE_FUNC%s)\s*)*
        ((?:const\s+|G_CONST_RETURN\s+)?\w+)           # 1: 1st const
        (\s+const)?\s*                                 # 2: 2nd const
        (\**)\s*                                       # 3: ptr
        \(\*\s*
          (\w+)                                        # 4: name
        \)\s*\(""" % ignore_decorators, re.VERBOSE)

    CLINE_MATCHER[6] = re.compile(r'^\s*typedef\s+enum\s+_?(\w+)\s+\1%s\s*;' % optional_decorators_regex)
    CLINE_MATCHER[8] = re.compile(
        r"""^\s*typedef\s+
        (struct|union)\s+                    # 1: struct/union
        _(\w+)\s+\2                          # 2: name
        %s                                   # 3: optional decorator
        \s*;""" % optional_decorators_regex, re.VERBOSE)
    # OTHER TYPEDEFS
    CLINE_MATCHER[15] = re.compile(
        r"""^\s*
        (?:extern|[A-Za-z_]+VAR%s)\s+
        (?:%s\w+)
        (?:\s+\*+|\*+|\s)\s*
        (?:const\s+)*
        ([A-Za-z]\w*)                                  # 1: name
        \s*;""" % (ignore_decorators, RET_TYPE_MODIFIER), re.VERBOSE)
    # FUNCTIONS
    CLINE_MATCHER[18] = re.compile(
        r"""^\s*
        (?:\b(?:extern|static|inline|G_INLINE_FUNC%s)\s*)*
        (%s\w+)                                                     # 1: return type
        ([\s*]+(?:\s*(?:\*+|\bconst\b|\bG_CONST_RETURN\b))*)\s*     # 2: .. cont'
        (_[A-Za-z]\w*)                                              # 3: name
        \s*\(""" % (ignore_decorators, RET_TYPE_MODIFIER), re.VERBOSE)
    CLINE_MATCHER[19] = re.compile(
        r"""^\s*
        (?:\b(?:extern|static|inline|G_INLINE_FUNC%s)\s*)*
        (%s\w+)                                                     # 1: return type
        ([\s*]+(?:\s*(?:\*+|\bconst\b|\bG_CONST_RETURN\b))*)\s*     # 2: .. cont'
        \(?([A-Za-z]\w*)\)?                                         # 3: name
        \s*\(""" % (ignore_decorators, RET_TYPE_MODIFIER), re.VERBOSE)

    PLINE_MATCHER[2] = re.compile(
        r"""^\s*
        (?:\b(?:extern%s)\s*)*
        (%s\w+)                                                     # 1: retun type
        ((?:\s*(?:\*+|\bconst\b|\bG_CONST_RETURN\b))*)              # 2: .. cont'
        \s*$""" % (ignore_decorators, RET_TYPE_MODIFIER), re.VERBOSE)

    PLINE_MATCHER[3] = re.compile(
        r"""^\s*(?:\b(?:extern|static|inline|G_INLINE_FUNC%s)\s*)*
        (%s\w+)                                                     # 1: return type
        ((?:\s*(?:\*+|\bconst\b|\bG_CONST_RETURN\b))*)              # 2: .. cont'
        \s*$""" % (ignore_decorators, RET_TYPE_MODIFIER), re.VERBOSE)

    PLINE_MATCHER[4] = re.compile(
        r"""^\s*(?:\b(?:extern|static|inline|G_INLINE_FUNC%s)\s*)*
        (%s\w+)                                                     # 1: return type
        (\s+\*+|\*+|\s)\s*                                          # 2: ptr?
        ([A-Za-z]\w*)                                               # 3: symbols
        \s*$""" % (ignore_decorators, RET_TYPE_MODIFIER), re.VERBOSE)

    # Matchers for 2nd previous line
    global PPLINE_MATCHER
    PPLINE_MATCHER = [
        # 0: FUNCTIONS
        re.compile(
            r"""^\s*(?:\b(?:extern|static|inline|G_INLINE_FUNC%s)\s*)*
            (
              (?:const\s+|G_CONST_RETURN\s+|signed\s+|unsigned\s+|struct\s+|union\s+|enum\s+)*
              \w+
              (?:\**\s+\**(?:const|G_CONST_RETURN))?
              (?:\s+|\s*\*+)
            )\s*$""" % ignore_decorators, re.VERBOSE)
        ]


def ScanHeaders(source_dir, section_list, decl_list, get_types, seen_headers, options):
    """Scans a directory tree looking for header files.

    Args:
      source_dir (str): the directory to scan.
      section_list (dict): map of section to filenames.
      seen_headers (set): set to avoid scanning headers twice
    """

    logging.info('Scanning source directory: %s', source_dir)

    # This array holds any subdirectories found.
    subdirs = []

    for file in sorted(os.listdir(source_dir)):
        if file.startswith('.'):
            continue
        fullname = os.path.join(source_dir, file)
        if os.path.isdir(fullname):
            subdirs.append(file)
        elif file.endswith('.h'):
            ScanHeader(fullname, section_list, decl_list, get_types,
                       seen_headers, options)

    # Now recursively scan the subdirectories.
    for dir in subdirs:
        matchstr = r'(\s|^)' + re.escape(dir) + r'(\s|$)'
        if re.search(matchstr, options.ignore_headers):
            continue
        ScanHeaders(os.path.join(source_dir, dir), section_list, decl_list,
                    get_types, seen_headers, options)


def ScanHeader(input_file, section_list, decl_list, get_types, seen_headers, options):
    """Scan a header file for doc commants.

    Look for doc comments and extract them. Parse each doc comments and the
    symbol declaration.

    Args:
      input_file (str): the header file to scan.
      section_list (dict): a map of section per filename
      decl_list (list): a list of declarations
      seen_headers (set): set to avoid scanning headers twice
    """

    # Don't scan headers twice
    canonical_input_file = os.path.realpath(input_file)
    if canonical_input_file in seen_headers:
        logging.info('File already scanned: %s', input_file)
        return

    seen_headers[canonical_input_file] = 1

    file_basename = os.path.split(input_file)[1][:-2]  # filename ends in .h

    # Check if the basename is in the list of headers to ignore.
    matchstr = r'(\s|^)' + re.escape(file_basename) + r'\.h(\s|$)'
    if re.search(matchstr, options.ignore_headers):
        logging.info('File ignored: %s', input_file)
        return

    # Check if the full name is in the list of headers to ignore.
    matchstr = r'(\s|^)' + re.escape(input_file) + r'(\s|$)'
    if re.search(matchstr, options.ignore_headers):
        logging.info('File ignored: %s', input_file)
        return

    if not os.path.exists(input_file):
        logging.warning('File does not exist: %s', input_file)
        return

    logging.info('Scanning %s', input_file)

    with open(input_file, 'r', encoding='utf-8') as hdr:
        input_lines = hdr.readlines()

    try:
        slist, doc_comments = ScanHeaderContent(input_lines, decl_list, get_types, options)
        logging.info("Scanning %s done", input_file)

        liststr = SeparateSubSections(slist, doc_comments)
        if liststr != '':
            if file_basename not in section_list:
                section_list[file_basename] = ''
            section_list[file_basename] += "<SECTION>\n<FILE>%s</FILE>\n%s</SECTION>\n\n" % (file_basename, liststr)

    except RuntimeError as e:
        common.LogWarning(input_file, 0, str(e))


def ScanHeaderContent(input_lines, decl_list, get_types, options):
    """Scan the the given content lines.

    Args:
      input_lines (list):
      decl_list (list): symbols declarations
      get_types (list): lst of symbols that have a get_type function
      options: commandline options

    Returns:
      list: a list of symbols found and a set of symbols for which we have a
            doc-comment
    """

    # Holds the resulting list of declarations.
    slist = []
    # Holds the title of the section
    title = None
    # True if we are in a comment.
    in_comment = 0
    # The type of declaration we are in, e.g. 'function' or 'macro'.
    in_declaration = ''
    # True if we should skip a block.
    skip_block = False
    # The current symbol being declared.
    symbol = None
    # Holds the declaration of the current symbol.
    decl = ''
    # For functions and function typedefs this holds the function's return type.
    ret_type = None
    # The pre-previous line read in - some Gnome functions have the return type
    # on one line, the function name on the next, and the rest of the
    # declaration after.
    pre_previous_line = ''
    # The previous line read in - some Gnome functions have the return type on
    # one line and the rest of the declaration after.
    previous_line = ''
    # Used to try to skip the standard #ifdef XXX #define XXX at the start of
    # headers.
    first_macro = 1
    # Used to handle structs/unions which contain nested structs or unions.
    level = None
    # Set to 1 for internal symbols, we need to fully parse, but don't add them
    # to docs
    internal = 0
    # Dict of forward declarations, we skip them if we find the real declaration
    # later.
    forward_decls = {}
    # Dict of doc-comments we found. The key is lowercase symbol name, val=1.
    doc_comments = {}

    deprecated_conditional_nest = 0
    ignore_conditional_nest = 0

    deprecated = ''
    doc_comment = ''

    # avoid generating regex with |'' (matching no string)
    # TODO(ensonic): keep in sync with InitScanner()
    # TODO(ensonic): extract the remaining regexps
    ignore_decorators = ''          # 1 uses
    optional_decorators_regex = ''  # 4 uses
    if options.ignore_decorators:
        ignore_decorators = '|' + options.ignore_decorators.replace('()', '\(\w*\)')
        optional_decorators_regex = '(?:\s+(?:%s))?' % ignore_decorators[1:]

    for line in input_lines:
        # If this is a private header, skip it.
        # TODO: consider scanning this first, so that we don't modify: decl_list
        # and get_types
        if re.search(r'^\s*/\*\s*<\s*private_header\s*>\s*\*/', line):
            return [], {}

        # Skip to the end of the current comment.
        if in_comment:
            logging.info('Comment: %s', line.strip())
            doc_comment += line
            if re.search(r'\*/', line):
                m = re.search(r'\* ([a-zA-Z][a-zA-Z0-9_]+):', doc_comment)
                if m:
                    doc_comments[m.group(1).lower()] = 1
                in_comment = 0
                doc_comment = ''
            continue

        # Keep a count of #if, #ifdef, #ifndef nesting,
        # and if we enter a deprecation-symbol-bracketed
        # zone, take note.
        m = re.search(r'^\s*#\s*if(?:n?def\b|\s+!?\s*defined\s*\()\s*(\w+)', line)
        if m:
            define_name = m.group(1)
            if deprecated_conditional_nest < 1 and re.search(options.deprecated_guards, define_name):
                deprecated_conditional_nest = 1
            elif deprecated_conditional_nest >= 1:
                deprecated_conditional_nest += 1
            if ignore_conditional_nest == 0 and '__GTK_DOC_IGNORE__' in define_name:
                ignore_conditional_nest = 1
            elif ignore_conditional_nest > 0:
                ignore_conditional_nest = 1

        elif re.search(r'^\s*#\sif', line):
            if deprecated_conditional_nest >= 1:
                deprecated_conditional_nest += 1
            if ignore_conditional_nest > 0:
                ignore_conditional_nest += 1
        elif re.search(r'^\s*#endif', line):
            if deprecated_conditional_nest >= 1:
                deprecated_conditional_nest -= 1
            if ignore_conditional_nest > 0:
                ignore_conditional_nest -= 1

        # If we find a line containing _DEPRECATED, we hope that this is
        # attribute based deprecation and also treat this as a deprecation
        # guard, unless it's a macro definition or the end of a deprecation
        # section (#endif /* XXX_DEPRECATED */
        if deprecated_conditional_nest == 0 and '_DEPRECATED' in line:
            m = re.search(r'^\s*#\s*(if*|define|endif)', line)
            if not (m or in_declaration == 'enum'):
                logging.info('Found deprecation annotation (decl: "%s"): "%s"',
                             in_declaration, line.strip())
                deprecated_conditional_nest += 0.1

        # set flag that is used later when we do AddSymbolToList
        if deprecated_conditional_nest > 0:
            deprecated = '<DEPRECATED/>\n'
        else:
            deprecated = ''

        if ignore_conditional_nest:
            continue

        if not in_declaration:
            # Skip top-level comments.
            m = re.search(r'^\s*/\*', line)
            if m:
                re.sub(r'^\s*/\*', '', line)
                if re.search(r'\*/', line):
                    logging.info('Found one-line comment: %s', line.strip())
                else:
                    in_comment = 1
                    doc_comment = line
                    logging.info('Found start of comment: %s', line.strip())
                continue

            logging.info('no decl: %s', line.strip())

            cm = [m.match(line) for m in CLINE_MATCHER]
            pm = [m.match(previous_line) for m in PLINE_MATCHER]
            ppm = [m.match(pre_previous_line) for m in PPLINE_MATCHER]

            # MACROS

            if cm[0]:
                symbol = cm[0].group(1)
                decl = line
                # We assume all macros which start with '_' are private, but
                # we accept '_' itself which is the standard gettext macro.
                # We also try to skip the first macro if it looks like the
                # standard #ifndef HEADER_FILE #define HEADER_FILE etc.
                # And we only want TRUE & FALSE defined in GLib.
                if not symbol.startswith('_') \
                        and (not re.search(r'#ifndef\s+' + symbol, previous_line)
                             or first_macro == 0) \
                        and ((symbol != 'TRUE' and symbol != 'FALSE')
                             or options.module == 'glib') \
                        or symbol == '_':
                    in_declaration = 'macro'
                    logging.info('Macro: "%s"', symbol)
                else:
                    logging.info('skipping Macro: "%s"', symbol)
                    in_declaration = 'macro'
                    internal = 1
                first_macro = 0

            # TYPEDEF'D FUNCTIONS (i.e. user functions)
            elif cm[1]:
                ret_type = format_ret_type(cm[1].group(1), cm[1].group(2), cm[1].group(3))
                symbol = cm[1].group(4)
                decl = line[cm[1].end():]
                in_declaration = 'user_function'
                logging.info('user function (1): "%s", Returns: "%s"', symbol, ret_type)

            elif pm[1] and cm[2]:
                ret_type = format_ret_type(cm[2].group(1), cm[2].group(2), cm[2].group(3))
                symbol = cm[2].group(4)
                decl = line[cm[2].end():]
                in_declaration = 'user_function'
                logging.info('user function (2): "%s", Returns: "%s"', symbol, ret_type)

            elif pm[1] and cm[3]:
                ret_type = cm[3].group(1)
                symbol = cm[3].group(2)
                decl = line[cm[3].end():]
                if pm[0]:
                    ret_type = format_ret_type(pm[0].group(1), pm[0].group(2), pm[0].group(3)) + ret_type
                    in_declaration = 'user_function'
                    logging.info('user function (3): "%s", Returns: "%s"', symbol, ret_type)

            # FUNCTION POINTER VARIABLES
            elif cm[4]:
                ret_type = format_ret_type(cm[4].group(1), cm[4].group(2), cm[4].group(3))
                symbol = cm[4].group(4)
                decl = line[cm[4].end():]
                in_declaration = 'user_function'
                logging.info('function pointer variable: "%s", Returns: "%s"', symbol, ret_type)

            # ENUMS

            elif cm[5]:
                re.sub(r'^\s*enum\s+_?(\w+)\s+\{', r'enum \1 {', line)
                # We assume that 'enum _<enum_name> {' is really the
                # declaration of enum <enum_name>.
                symbol = cm[5].group(1)
                decl = line
                in_declaration = 'enum'
                logging.info('plain enum: "%s"', symbol)

            elif cm[6]:
                # We skip 'typedef enum <enum_name> _<enum_name>;' as the enum will
                # be declared elsewhere.
                logging.info('skipping enum typedef: "%s"', line)

            elif cm[7]:
                symbol = ''
                decl = line
                in_declaration = 'enum'
                logging.info('typedef enum: -')

            # STRUCTS AND UNIONS

            elif cm[8]:
                # We've found a 'typedef struct _<name> <name>;'
                # This could be an opaque data structure, so we output an
                # empty declaration. If the structure is actually found that
                # will override this (technically if will just be another entry
                # in the output file and will be joined when reading the file).
                structsym = cm[8].group(1).upper()
                logging.info('%s typedef: "%s"', structsym, cm[8].group(2))
                forward_decls[cm[8].group(2)] = '<%s>\n<NAME>%s</NAME>\n%s</%s>\n' % (
                    structsym, cm[8].group(2), deprecated, structsym)

                m = SUB_MATCHER[0].match(cm[8].group(2))
                if m:
                    objectname = m.group(1)
                    logging.info('Found object: "%s"', objectname)
                    title = '<TITLE>%s</TITLE>' % objectname

            elif cm[9]:
                # Skip private structs/unions.
                logging.info('private struct/union')

            elif cm[10]:
                # Do a similar thing for normal structs as for typedefs above.
                # But we output the declaration as well in this case, so we
                # can differentiate it from a typedef.
                structsym = cm[10].group(1).upper()
                logging.info('%s:%s', structsym, cm[10].group(2))
                forward_decls[cm[10].group(2)] = '<%s>\n<NAME>%s</NAME>\n%s%s</%s>\n' % (
                    structsym, cm[10].group(2), line, deprecated, structsym)

            elif cm[11]:
                symbol = ''
                decl = line
                level = 0
                in_declaration = cm[11].group(1)
                logging.info('typedef struct/union "%s"', in_declaration)

            # OTHER TYPEDEFS

            elif cm[12]:
                logging.info('Found struct/union(*) typedef "%s": "%s"', cm[12].group(1), line)
                if AddSymbolToList(slist, cm[12].group(1)):
                    decl_list.append('<TYPEDEF>\n<NAME>%s</NAME>\n%s%s</TYPEDEF>\n' %
                                     (cm[12].group(1), deprecated, line))

            elif cm[13]:
                if cm[13].group(1).split()[0] not in ('struct', 'union'):
                    logging.info('Found typedef: "%s"', line)
                    if AddSymbolToList(slist, cm[13].group(2)):
                        decl_list.append(
                            '<TYPEDEF>\n<NAME>%s</NAME>\n%s%s</TYPEDEF>\n' % (cm[13].group(2), deprecated, line))
            elif cm[14]:
                logging.info('Skipping typedef: "%s"', line)

            # VARIABLES (extern'ed variables)

            elif cm[15]:
                symbol = cm[15].group(1)
                line = re.sub(r'^\s*([A-Za-z_]+VAR)\b', r'extern', line)
                decl = line
                logging.info('Possible extern var "%s": "%s"', symbol, decl)
                if AddSymbolToList(slist, symbol):
                    decl_list.append('<VARIABLE>\n<NAME>%s</NAME>\n%s%s</VARIABLE>\n' % (symbol, deprecated, decl))

            # VARIABLES

            elif cm[16]:
                symbol = cm[16].group(1)
                decl = line
                logging.info('Possible global var" %s": "%s"', symbol, decl)
                if AddSymbolToList(slist, symbol):
                    decl_list.append('<VARIABLE>\n<NAME>%s</NAME>\n%s%s</VARIABLE>\n' % (symbol, deprecated, decl))

            # G_DECLARE_*

            elif cm[17]:
                in_declaration = 'g-declare'
                symbol = 'G_DECLARE_' + cm[17].group(1)
                decl = line[cm[17].end():]

            # FUNCTIONS

            elif cm[18]:
                # We assume that functions starting with '_' are private and skip them.
                ret_type = format_ret_type(cm[18].group(1), None, cm[18].group(2))
                symbol = cm[18].group(3)
                decl = line[cm[18].end():]
                logging.info('internal Function: "%s", Returns: "%s""%s"', symbol, cm[18].group(1), cm[18].group(2))
                in_declaration = 'function'
                internal = 1
                skip_block |= is_inline_func(line)

            elif cm[19]:
                ret_type = format_ret_type(cm[19].group(1), None, cm[19].group(2))
                symbol = cm[19].group(3)
                decl = line[cm[19].end():]
                logging.info('Function (1): "%s", Returns: "%s""%s"', symbol, cm[19].group(1), cm[19].group(2))
                in_declaration = 'function'
                skip_block |= is_inline_func(line)

            # Try to catch function declarations which have the return type on
            # the previous line. But we don't want to catch complete functions
            # which have been declared G_INLINE_FUNC, e.g. g_bit_nth_lsf in
            # glib, or 'static inline' functions.
            elif cm[20]:
                symbol = cm[20].group(1)
                decl = line[cm[20].end():]

                if is_inline_func(previous_line):
                    skip_block = True
                    if pm[3]:
                        ret_type = format_ret_type(pm[3].group(1), None, pm[3].group(2))
                        logging.info('Function  (3): "%s", Returns: "%s"', symbol, ret_type)
                        in_declaration = 'function'
                else:
                    if pm[2]:
                        ret_type = format_ret_type(pm[2].group(1), None, pm[2].group(2))
                        logging.info('Function  (2): "%s", Returns: "%s"', symbol, ret_type)
                        in_declaration = 'function'

            # Try to catch function declarations with the return type and name
            # on the previous line(s), and the start of the parameters on this.
            elif cm[21]:
                decl = line[cm[21].end():]
                if pm[4]:
                    ret_type = pm[4].group(1) + ' ' + pm[4].group(2).strip()
                    symbol = pm[4].group(3)
                    in_declaration = 'function'
                    logging.info('Function (5): "%s", Returns: "%s"', symbol, ret_type)

                elif re.search(r'^\s*\w+\s*$', previous_line) and ppm[0]:
                    ret_type = ppm[0].group(1)
                    ret_type = re.sub(r'\s*\n', '', ret_type, flags=re.MULTILINE)
                    in_declaration = 'function'

                    symbol = previous_line
                    symbol = re.sub(r'^\s+', '', symbol)
                    symbol = re.sub(r'\s*\n', '', symbol, flags=re.MULTILINE)
                    logging.info('Function (6): "%s", Returns: "%s"', symbol, ret_type)

            # } elsif (m/^extern\s+/) {
                # print "DEBUG: Skipping extern: $_"

            # STRUCTS
            elif cm[22]:
                # Skip 'struct _<struct_name> *', since it could be a
                # return type on its own line.
                pass
            elif cm[23]:
                # We assume that 'struct _<struct_name>' is really the
                # declaration of struct <struct_name>.
                symbol = cm[23].group(1)
                decl = line
                # we will find the correct level as below we do $level += tr/{//
                level = 0
                in_declaration = 'struct'
                logging.info('Struct(_): "%s"', symbol)

            # UNIONS
            elif cm[24]:
                # Skip 'union _<union_name> *' (see above)
                pass
            elif cm[25]:
                symbol = cm[25].group(1)
                decl = line
                level = 0
                in_declaration = 'union'
                logging.info('Union(_): "%s"', symbol)
        else:
            logging.info('in decl %s: skip=%s %s', in_declaration, skip_block, line.strip())
            decl += line

            if skip_block and '{' in decl:
                (skip_block, decl) = remove_braced_content(decl)
                logging.info('in decl: skip=%s decl=[%s]', skip_block, decl)

        pre_previous_line = previous_line
        previous_line = line

        if skip_block:
            logging.info('skipping, in decl %s, decl=[%s]', in_declaration, decl)
            continue

        if in_declaration == "g-declare":
            dm = re.search(r'\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*\).*$', decl)
            # FIXME the original code does s// stuff here and we don't. Is it necessary?
            if dm:
                ModuleObjName = dm.group(1)
                module_obj_name = dm.group(2)
                if options.rebuild_types:
                    get_types.append(module_obj_name + '_get_type')
                forward_decls[ModuleObjName] = '<STRUCT>\n<NAME>%s</NAME>\n%s</STRUCT>\n' % (ModuleObjName, deprecated)
                if symbol.startswith('G_DECLARE_DERIVABLE'):
                    forward_decls[ModuleObjName + 'Class'] = '<STRUCT>\n<NAME>%sClass</NAME>\n%s</STRUCT>\n' % (
                        ModuleObjName, deprecated)
                if symbol.startswith('G_DECLARE_INTERFACE'):
                    forward_decls[ModuleObjName + 'Interface'] = '<STRUCT>\n<NAME>%sInterface</NAME>\n%s</STRUCT>\n' % (
                        ModuleObjName, deprecated)
                in_declaration = ''

        if in_declaration == 'function':
            # Note that sometimes functions end in ') G_GNUC_PRINTF (2, 3);' or
            # ') __attribute__ (...);'.
            regex = r'\)\s*(G_GNUC_.*|.*DEPRECATED.*%s\s*|__attribute__\s*\(.*\)\s*)*;.*$' % ignore_decorators
            pm = re.search(regex, decl, flags=re.MULTILINE)
            if pm:
                logging.info('scrubbing:[%s]', decl.strip())
                decl = re.sub(regex, '', decl, flags=re.MULTILINE)
                logging.info('scrubbed:[%s]', decl.strip())
                if internal == 0:
                    decl = re.sub(r'/\*.*?\*/', '', decl, flags=re.MULTILINE)   # remove comments.
                    decl = re.sub(r'\s*\n\s*(?!$)', ' ', decl, flags=re.MULTILINE)  # remove newlines
                    # consolidate whitespace at start/end of lines.
                    decl = decl.strip()
                    ret_type = re.sub(r'/\*.*?\*/', '', ret_type).strip()       # remove comments in ret type.
                    if AddSymbolToList(slist, symbol):
                        decl_list.append('<FUNCTION>\n<NAME>%s</NAME>\n%s<RETURNS>%s</RETURNS>\n%s\n</FUNCTION>\n' %
                                         (symbol, deprecated, ret_type, decl))
                        if options.rebuild_types:
                            # check if this looks like a get_type function and if so remember
                            if symbol.endswith('_get_type') and 'GType' in ret_type and re.search(r'^(void|)$', decl):
                                logging.info(
                                    "Adding get-type: [%s] [%s] [%s]", ret_type, symbol, decl)
                                get_types.append(symbol)
                else:
                    internal = 0
                deprecated_conditional_nest = int(deprecated_conditional_nest)
                in_declaration = ''
                skip_block = False

        if in_declaration == 'user_function':
            if re.search(r'\).*$', decl):
                decl = re.sub(r'\).*$', '', decl)
                # TODO: same as above
                decl = re.sub(r'/\*.*?\*/', '', decl, flags=re.MULTILINE)   # remove comments.
                decl = re.sub(r'\s*\n\s*(?!$)', ' ', decl, flags=re.MULTILINE)  # remove newlines
                # TODO: don't stip here (it works above, but fails some test
                # consolidate whitespace at start/end of lines.
                # decl = decl.strip()
                if AddSymbolToList(slist, symbol):
                    decl_list.append('<USER_FUNCTION>\n<NAME>%s</NAME>\n%s<RETURNS>%s</RETURNS>\n%s</USER_FUNCTION>\n' %
                                     (symbol, deprecated, ret_type, decl))
                deprecated_conditional_nest = int(deprecated_conditional_nest)
                in_declaration = ''

        if in_declaration == 'macro':
            if not re.search(r'\\\s*$', decl):
                if internal == 0:
                    if AddSymbolToList(slist, symbol):
                        decl_list.append('<MACRO>\n<NAME>%s</NAME>\n%s%s</MACRO>\n' % (symbol, deprecated, decl))
                else:
                    logging.info('skip internal macro: [%s]', symbol)
                    internal = 0
                deprecated_conditional_nest = int(deprecated_conditional_nest)
                in_declaration = ''
            else:
                logging.info('skip empty macro: [%s]', symbol)

        if in_declaration == 'enum':
            # Examples:
            # "};"
            # "} MyEnum;"
            # "} MyEnum DEPRECATED_FOR(NewEnum);"
            # "} DEPRECATED_FOR(NewEnum);"
            em = re.search(r'\n\s*\}\s*(?:(\w+)?%s)?;\s*$' % optional_decorators_regex, decl)
            if em:
                if symbol == '':
                    symbol = em.group(1)
                # Enums could contain deprecated values and that doesn't mean
                # the whole enum is deprecated, so they are ignored when setting
                # deprecated_conditional_nest above. Here we can check if the
                # _DEPRECATED is between '}' and ';' which would mean the enum
                # as a whole is deprecated.
                if re.search(r'\n\s*\}.*_DEPRECATED.*;\s*$', decl):
                    deprecated = '<DEPRECATED/>\n'
                if AddSymbolToList(slist, symbol):
                    stripped_decl = re.sub(optional_decorators_regex, '', decl)
                    decl_list.append('<ENUM>\n<NAME>%s</NAME>\n%s%s</ENUM>\n' % (symbol, deprecated, stripped_decl))
                deprecated_conditional_nest = int(deprecated_conditional_nest)
                in_declaration = ''

        # We try to handle nested structs/unions, but unmatched brackets in
        # comments will cause problems.
        if in_declaration == 'struct' or in_declaration == 'union':
            # Same regex as for enum
            sm = re.search(r'\n\}\s*(?:(\w+)?%s)?;\s*$' % optional_decorators_regex, decl)
            if level <= 1 and sm:
                if symbol == '':
                    symbol = sm.group(1)

                bm = re.search(r'^(\S+)(Class|Iface|Interface)\b', symbol)
                if bm:
                    objectname = bm.group(1)
                    logging.info('Found object: "%s"', objectname)
                    title = '<TITLE>%s</TITLE>' % objectname

                logging.info('Store struct: "%s"', symbol)
                if AddSymbolToList(slist, symbol):
                    structsym = in_declaration.upper()
                    stripped_decl = re.sub('(%s)' % optional_decorators_regex, '', decl)
                    decl_list.append('<%s>\n<NAME>%s</NAME>\n%s%s</%s>\n' %
                                     (structsym, symbol, deprecated, stripped_decl, structsym))
                    if symbol in forward_decls:
                        del forward_decls[symbol]
                deprecated_conditional_nest = int(deprecated_conditional_nest)
                in_declaration = ''
            else:
                # We use tr to count the brackets in the line, and adjust
                # $level accordingly.
                level += line.count('{')
                level -= line.count('}')
                logging.info('struct/union level : %d', level)

    # here we want in_declaration=='', otherwise we have a partial declaration
    if in_declaration != '':
        raise RuntimeError('partial declaration (%s) : %s ' % (in_declaration, decl))

    # print remaining forward declarations
    for symbol in sorted(forward_decls.keys()):
        if forward_decls[symbol]:
            AddSymbolToList(slist, symbol)
            decl_list.append(forward_decls[symbol])

    # add title
    if title:
        slist = [title] + slist
    return slist, doc_comments


def remove_braced_content(decl):
    """Remove all nested pairs of curly braces.

    Args:
      decl (str): the decl

    Returns:
      str: a declaration stripped of braced content
    """

    skip_block = True
    # Remove all nested pairs of curly braces.
    brace_remover = r'{[^{]*?}'
    bm = re.search(brace_remover, decl)
    while bm:
        decl = re.sub(brace_remover, '', decl)
        logging.info('decl=[%s]' % decl)
        bm = re.search(brace_remover, decl)

    # If all '{' have been matched and removed, we're done
    bm = re.search(r'(.*?){', decl)
    if not bm:
        # this is a hack to detect the end of declaration
        decl = decl.rstrip() + ';'
        skip_block = False
        logging.info('skip_block done')

    return skip_block, decl


def is_inline_func(line):
    line = line.strip()
    if line.startswith('G_INLINE_FUNC'):
        logging.info('skip block after G_INLINE_FUNC function')
        return True
    if re.search(r'static\s+inline', line):
        logging.info('skip block after static inline function')
        return True
    return False


def format_ret_type(base_type, const, ptr):
    ret_type = base_type
    if const:
        ret_type += const
    if ptr:
        ret_type += ' ' + ptr.strip()
    return ret_type


def SeparateSubSections(slist, doc_comments):
    """Separate the standard macros and functions.

    Place them at the end of the current section, in a subsection named
    'Standard'. Do this in a loop to catch objects, enums and flags.

    Args:
      slist (list): list of symbols
      doc_comments (dict): comments for each symbol

    Returns:
      str: the section doc xml fomatted as string
    """

    klass = lclass = prefix = lprefix = None
    standard_decl = []
    liststr = '\n'.join(s for s in slist if s) + '\n'
    while True:
        m = re.search(r'^(\S+)_IS_(\S*)_CLASS\n', liststr, flags=re.MULTILINE)
        m2 = re.search(r'^(\S+)_IS_(\S*)\n', liststr, flags=re.MULTILINE)
        m3 = re.search(r'^(\S+?)_(\S*)_get_type\n', liststr, flags=re.MULTILINE)
        if m:
            prefix = m.group(1)
            lprefix = prefix.lower()
            klass = m.group(2)
            lclass = klass.lower()
            logging.info("Found gobject type '%s_%s' from is_class macro", prefix, klass)
        elif m2:
            prefix = m2.group(1)
            lprefix = prefix.lower()
            klass = m2.group(2)
            lclass = klass.lower()
            logging.info("Found gobject type '%s_%s' from is_ macro", prefix, klass)
        elif m3:
            lprefix = m3.group(1)
            prefix = lprefix.upper()
            lclass = m3.group(2)
            klass = lclass.upper()
            logging.info("Found gobject type '%s_%s' from get_type function", prefix, klass)
        else:
            break

        cclass = lclass
        cclass = cclass.replace('_', '')
        mtype = lprefix + cclass

        liststr, standard_decl = replace_once(liststr, standard_decl, r'^%sPrivate\n' % mtype)

        # We only leave XxYy* in the normal section if they have docs
        if mtype not in doc_comments:
            logging.info("  Hide instance docs for %s", mtype)
            liststr, standard_decl = replace_once(liststr, standard_decl, r'^%s\n' % mtype)

        if mtype + 'class' not in doc_comments:
            logging.info("  Hide class docs for %s", mtype)
            liststr, standard_decl = replace_once(liststr, standard_decl, r'^%sClass\n' % mtype)

        if mtype + 'interface' not in doc_comments:
            logging.info("  Hide iface docs for %s", mtype)
            liststr, standard_decl = replace_once(liststr, standard_decl, r'%sInterface\n' % mtype)

        if mtype + 'iface' not in doc_comments:
            logging.info("  Hide iface docs for " + mtype)
            liststr, standard_decl = replace_once(liststr, standard_decl, r'%sIface\n' % mtype)

        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_IS_%s\n' % klass)
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_TYPE_%s\n' % klass)
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_%s_get_type\n' % lclass)
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_IS_%s_CLASS\n' % klass)
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_%s_CLASS\n' % klass)
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_%s_GET_CLASS\n' % klass)
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_%s_GET_IFACE\n' % klass)
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_%s_GET_INTERFACE\n' % klass)
        # We do this one last, otherwise it tends to be caught by the IS_$class macro
        liststr, standard_decl = replace_all(liststr, standard_decl, r'^\S+_%s\n' % klass)

    logging.info('Decl:%s---', liststr)
    logging.info('Std :%s---', ''.join(sorted(standard_decl)))
    if len(standard_decl):
        # sort the symbols
        liststr += '<SUBSECTION Standard>\n' + ''.join(sorted(standard_decl))
    return liststr


def replace_once(liststr, standard_decl, regex):
    mre = re.search(regex, liststr,  flags=re.IGNORECASE | re.MULTILINE)
    if mre:
        standard_decl.append(mre.group(0))
        liststr = re.sub(regex, '', liststr, flags=re.IGNORECASE | re.MULTILINE)
    return liststr, standard_decl


def replace_all(liststr, standard_decl, regex):
    mre = re.search(regex, liststr, flags=re.MULTILINE)
    while mre:
        standard_decl.append(mre.group(0))
        liststr = re.sub(regex, '', liststr, flags=re.MULTILINE)
        mre = re.search(regex, liststr, flags=re.MULTILINE)
    return liststr, standard_decl


def AddSymbolToList(slist, symbol):
    """ Adds symbol to list of declaration if not already present.

    Args:
        slist: The list of symbols.
        symbol: The symbol to add to the list.
    """
    if symbol in slist:
        # logging.info('Symbol %s already in list. skipping', symbol)
        # we return False to skip outputting another entry to -decl.txt
        # this is to avoid redeclarations (e.g. in conditional sections).
        return False
    slist.append(symbol)
    return True
