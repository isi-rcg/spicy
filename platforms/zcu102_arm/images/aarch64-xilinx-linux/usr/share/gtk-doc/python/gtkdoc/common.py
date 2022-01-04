# -*- python -*-
#
# gtk-doc - GTK DocBook documentation generator.
# Copyright (C) 2001  Damon Chaplin
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

from collections import OrderedDict
import logging
import os
import re
import subprocess
import sys

from . import config


def setup_logging():
    """Check GTKDOC_TRACE environment variable.

    Set python log level to the value of the environment variable (DEBUG, INFO,
    WARNING, ERROR and CRITICAL) or INFO if the environment variable is empty.
    """
    log_level = os.environ.get('GTKDOC_TRACE', 'WARNING')
    if log_level == '':
        log_level = 'WARNING'
    logging.basicConfig(stream=sys.stdout,
                        level=logging.getLevelName(log_level.upper()),
                        format='%(asctime)s:%(filename)s:%(funcName)s:%(lineno)d:%(levelname)s:%(message)s')
    # When redirecting the output and running with a non utf-8 locale
    # we get UnicodeEncodeError:
    encoding = sys.stdout.encoding
    if 'PYTHONIOENCODING' not in os.environ and (not encoding or encoding != 'UTF-8'):
        sys.stdout.flush()
        sys.stdout = open(sys.stdout.fileno(), mode='w', encoding='utf8', buffering=1)


def UpdateFileIfChanged(old_file, new_file, make_backup):
    """Compares the old version of the file with the new version and if the
    file has changed it moves the new version into the old versions place. This
    is used so we only change files if needed, so we can do proper dependency
    tracking.

    Args:
        old_file (str): The pathname of the old file.
        new_file (str): The pathname of the new version of the file.
        make_backup (bool): True if a backup of the old file should be kept.
                           It will have the .bak suffix added to the file name.

    Returns:
        bool: It returns False if the file hasn't changed, and True if it has.
    """

    logging.debug("Comparing %s with %s...", old_file, new_file)

    if os.path.exists(old_file):
        old_contents = new_contents = None
        with open(old_file, 'rb') as f:
            old_contents = f.read()
        with open(new_file, 'rb') as f:
            new_contents = f.read()
        if old_contents == new_contents:
            os.unlink(new_file)
            logging.debug("-> content is the same.")
            return False

        if make_backup:
            backupname = old_file + '.bak'
            if os.path.exists(backupname):
                os.unlink(backupname)
            os.rename(old_file, backupname)
        else:
            os.unlink(old_file)
        logging.debug("-> content differs.")
    else:
        logging.debug("-> %s created.", old_file)

    os.rename(new_file, old_file)
    return True


def GetModuleDocDir(module_name):
    """Get the docdir for the given module via pkg-config

    Args:
      module_name (string): The module, e.g. 'glib-2.0'

    Returns:
      str: the doc directory or None
    """
    path = None
    try:
        path = subprocess.check_output([config.pkg_config, '--variable=prefix', module_name], universal_newlines=True)
    except subprocess.CalledProcessError:
        return None
    return os.path.join(path.strip(), 'share/gtk-doc/html')


def LogWarning(filename, line, message):
    """Log a warning in gcc style format

    Args:
      file (str): The file the error comes from
      line (int): line number in the file
      message (str): the error message to print
    """
    filename = filename or "unknown"

    # TODO: write to stderr
    print("%s:%d: warning: %s" % (filename, line, message))


def CreateValidSGMLID(xml_id):
    """Creates a valid SGML 'id' from the given string.

    According to http://www.w3.org/TR/html4/types.html#type-id "ID and NAME
    tokens must begin with a letter ([A-Za-z]) and may be followed by any number
    of letters, digits ([0-9]), hyphens ("-"), underscores ("_"), colons (":"),
    and periods (".")."

    When creating SGML IDS, we append ":CAPS" to all all-caps identifiers to
    prevent name clashes (SGML ids are case-insensitive). (It basically never is
    the case that mixed-case identifiers would collide.)

    Args:
      id (str): The text to be converted into a valid SGML id.

    Returns:
      str: The converted id.
    """

    # Special case, '_' would end up as '' so we use 'gettext-macro' instead.
    if xml_id == '_':
        return "gettext-macro"

    xml_id = re.sub(r'[,;]', '', xml_id)
    xml_id = re.sub(r'[_ ]', '-', xml_id)
    xml_id = re.sub(r'^-+', '', xml_id)
    xml_id = xml_id.replace('::', '-')
    xml_id = xml_id.replace(':', '--')

    # Append ":CAPS" to all all-caps identifiers
    # FIXME: there are some inconsistencies here, we have index files containing e.g. TRUE--CAPS
    if xml_id.isupper() and not xml_id.endswith('-CAPS'):
        xml_id += ':CAPS'

    return xml_id


# Parsing helpers (move to mkdb ?)

class ParseError(Exception):
    pass


def PreprocessStructOrEnum(declaration):
    """Trim a type declaration for display.

    Removes private sections and comments from the declaration.

    Args:
      declaration (str): the type declaration (struct or enum)

    Returns:
      str: the trimmed declaration
    """
    # Remove private symbols
    # Assume end of declaration if line begins with '}'
    declaration = re.sub(r'\n?[ \t]*/\*\s*<\s*(private|protected)\s*>\s*\*/.*?(?:/\*\s*<\s*public\s*>\s*\*/|(?=^\}))',
                         '', declaration, flags=re.MULTILINE | re.DOTALL)

    # Remove all other comments
    declaration = re.sub(r'\n\s*/\*.*?\*/\s*\n', r'\n', declaration, flags=re.MULTILINE | re.DOTALL)
    declaration = re.sub(r'/\*([^*]+|\*(?!/))*\*/', r' ', declaration)
    declaration = re.sub(r'\n\s*//.*?\n', r'\n', declaration, flags=re.MULTILINE | re.DOTALL)
    declaration = re.sub(r'//.*', '', declaration)

    return declaration


# TODO: output_function_params is always passed as 0
# TODO: we always pass both functions
def ParseStructDeclaration(declaration, is_object, output_function_params, typefunc=None, namefunc=None):
    """ Parse a struct declaration.

    Takes a structure declaration and breaks it into individual type declarations.

    Args:
      declaration (str): the declaration to parse
      is_object (bool): true if this is an object structure
      output_function_params (bool): true if full type is wanted for function pointer members
      typefunc (func): function to apply to type
      namefunc (func): function to apply to name

    Returns:
      dict: map of (symbol, decl) pairs describing the public declaration
    """

    # For forward struct declarations just return an empty array.
    if re.search(r'(?:struct|union)\s+\S+\s*;', declaration, flags=re.MULTILINE | re.DOTALL):
        return {}

    # Remove all private parts of the declaration
    # For objects, assume private
    if is_object:
        declaration = re.sub(r'''((?:struct|union)\s+\w*\s*\{)
                                 .*?
                                 (?:/\*\s*<\s*public\s*>\s*\*/|(?=\}))''',
                             r'\1', declaration, flags=re.MULTILINE | re.DOTALL | re.VERBOSE)

    # Remove g_iface, parent_instance and parent_class if they are first member
    declaration = re.sub(r'(\{)\s*(\w)+\s+(g_iface|parent_instance|parent_class)\s*;', r'\1', declaration)

    declaration = PreprocessStructOrEnum(declaration)

    if declaration.strip() == '':
        return {}

    # Prime match after "struct/union {" declaration
    match = re.search(r'(?:struct|union)\s+\w*\s*\{', declaration, flags=re.MULTILINE | re.DOTALL)
    if not match:
        raise ParseError('Declaration "%s" does not begin with "struct/union [NAME] {"' % declaration)

    logging.debug('public fields in struct/union: %s', declaration)

    result = OrderedDict()

    # Treat lines in sequence, allowing singly nested anonymous structs and unions.
    for m in re.finditer(r'\s*([^{;]+(\{[^\}]*\}[^{;]+)?);', declaration[match.end():], flags=re.MULTILINE | re.DOTALL):
        line = m.group(1)

        logging.debug('checking "%s"', line)

        if re.search(r'^\s*\}\s*\w*\s*$', line):
            break

        # FIXME: Just ignore nested structs and unions for now
        if '{' in line:
            continue

        # ignore preprocessor directives
        line = re.sub(r'^#.*?\n\s*', '', line, flags=re.MULTILINE | re.DOTALL)

        if re.search(r'^\s*\}\s*\w*\s*$', line):
            break

        func_match = re.search(r'''^
                                   (const\s+|G_CONST_RETURN\s+|unsigned\s+|signed\s+|long\s+|short\s+)*(struct\s+|enum\s+)?  # mod1
                                   (\w+)\s*                             # type
                                   (\**(?:\s*restrict)?)\s*             # ptr1
                                   (const\s+)?                          # mod2
                                   (\**\s*)                             # ptr2
                                   (const\s+)?                          # mod3
                                   \(\s*\*\s*(\w+)\s*\)\s*              # name
                                   \(([^)]*)\)\s*                       # func_params
                                   $''', line, flags=re.VERBOSE)
        vars_match = re.search(r'''^
                                   ((?:const\s+|volatile\s+|unsigned\s+|signed\s+|short\s+|long\s+)?)(struct\s+|enum\s+)? # mod1
                                   (\w+)\s*                            # type
                                   (\** \s* const\s+)?                 # mod2
                                   (.*)                                # variables
                                   $''', line, flags=re.VERBOSE)

        # Try to match structure members which are functions
        if func_match:
            mod1 = func_match.group(1) or ''
            if func_match.group(2):
                mod1 += func_match.group(2)
            func_type = func_match.group(3)
            ptr1 = func_match.group(4)
            mod2 = func_match.group(5) or ''
            ptr2 = func_match.group(6)
            mod3 = func_match.group(7) or ''
            name = func_match.group(8)
            func_params = func_match.group(9)
            ptype = func_type
            if typefunc:
                ptype = typefunc(func_type, '<type>%s</type>' % func_type)
            pname = name
            if namefunc:
                pname = namefunc(name)

            if output_function_params:
                result[name] = '%s%s%s%s%s%s&#160;(*%s)&#160;(%s)' % (
                    mod1, ptype, ptr1, mod2, ptr2, mod3, pname, func_params)
            else:
                result[name] = '%s&#160;()' % pname

        # Try to match normal struct fields of comma-separated variables/
        elif vars_match:
            mod1 = vars_match.group(1) or ''
            if vars_match.group(2):
                mod1 += vars_match.group(2)
            vtype = vars_match.group(3)
            ptype = vtype
            if typefunc:
                ptype = typefunc(vtype, '<type>%s</type>' % vtype)
            mod2 = vars_match.group(4) or ''
            if mod2:
                mod2 = ' ' + mod2
            var_list = vars_match.group(5)

            logging.debug('"%s" "%s" "%s" "%s"', mod1, vtype, mod2, var_list)

            mod1 = mod1.replace(' ', '&#160;')
            mod2 = mod2.replace(' ', '&#160;')

            for n in var_list.split(','):
                # Each variable can have any number of '*' before the identifier,
                # and be followed by any number of pairs of brackets or a bit field specifier.
                # e.g. *foo, ***bar, *baz[12][23], foo : 25.
                m = re.search(
                    r'^\s* (\**(?:\s*restrict\b)?) \s* (\w+) \s* (?: ((?:\[[^\]]*\]\s*)+) | (:\s*\d+)?) \s* $',
                    n, flags=re.VERBOSE)
                if m:
                    ptrs = m.group(1)
                    name = m.group(2)
                    array = m.group(3) or ''
                    bits = m.group(4)
                    if bits:
                        bits = ' ' + bits
                    else:
                        bits = ''
                    if ptrs and not ptrs.endswith('*'):
                        ptrs += ' '

                    array = array.replace(' ', '&#160;')
                    bits = bits.replace(' ', '&#160;')

                    pname = name
                    if namefunc:
                        pname = namefunc(name)

                    result[name] = '%s%s%s&#160;%s%s%s%s;' % (mod1, ptype, mod2, ptrs, pname, array, bits)

                    logging.debug('Matched line: %s%s%s %s%s%s%s', mod1, ptype, mod2, ptrs, pname, array, bits)
                else:
                    logging.warning('Cannot parse struct field: "%s"', n)

        else:
            logging.warning('Cannot parse structure field: "%s"', line)

    return result


def ParseEnumDeclaration(declaration):
    """Parse an enum declaration.

    This function takes a enumeration declaration and breaks it into individual
    enum member declarations.

    Args:
      declaration (str): the declaration to parse

    Returns:
      str: list of strings describing the public declaration
    """

    # For forward struct declarations just return an empty array.
    if re.search(r'enum\s+\S+\s*;', declaration, flags=re.MULTILINE | re.DOTALL):
        return ()

    declaration = PreprocessStructOrEnum(declaration)

    if declaration.strip() == '':
        return ()

    result = []

    # Remove parenthesized expressions (in macros like GTK_BLAH = BLAH(1,3))
    # to avoid getting confused by commas they might contain. This doesn't
    # handle nested parentheses correctly.
    declaration = re.sub(r'\([^)\n]+\)', '', declaration)

    # Remove apostrophed characters (e.g. '}' or ',') values to avoid getting
    # confused with end of enumeration.
    # See https://bugzilla.gnome.org/show_bug.cgi?id=741305
    declaration = re.sub(r'\'.\'', '', declaration)

    # Remove comma from comma - possible whitespace - closing brace sequence
    # since it is legal in GNU C and C99 to have a trailing comma but doesn't
    # result in an actual enum member
    declaration = re.sub(r',(\s*})', r'\1', declaration)

    # Prime match after "typedef enum {" declaration
    match = re.search(r'(typedef\s+)?enum\s*(\S+\s*)?\{', declaration, flags=re.MULTILINE | re.DOTALL)
    if not match:
        raise ParseError('Enum declaration "%s" does not begin with "typedef enum {" or "enum [NAME] {"' % declaration)

    logging.debug("public fields in enum: %s'", declaration)

    # Treat lines in sequence.
    for m in re.finditer(r'\s*([^,\}]+)([,\}])', declaration[match.end():], flags=re.MULTILINE | re.DOTALL):
        line = m.group(1)
        terminator = m.group(2)

        # ignore preprocessor directives
        line = re.sub(r'^#.*?\n\s*', '', line, flags=re.MULTILINE | re.DOTALL)

        m1 = re.search(r'^(\w+)\s*(=.*)?$', line, flags=re.MULTILINE | re.DOTALL)
        # Special case for GIOCondition, where the values are specified by
        # macros which expand to include the equal sign like '=1'.
        m2 = re.search(r'^(\w+)\s*GLIB_SYSDEF_POLL', line, flags=re.MULTILINE | re.DOTALL)
        if m1:
            result.append(m1.group(1))
        elif m2:
            result.append(m2.group(1))
        elif line.strip().startswith('#'):
            # Special case include of <gdk/gdkcursors.h>, just ignore it
            # Special case for #ifdef/#else/#endif, just ignore it
            break
        else:
            logging.warning('Cannot parse enumeration member: %s', line)

        if terminator == '}':
            break

    return result


def ParseFunctionDeclaration(declaration, typefunc, namefunc):
    """Parse a function declaration.

    This function takes a function declaration and breaks it into individual
    parameter declarations.

    Args:
      declaration (str): the declaration to parse
      typefunc (func): function to apply to type
      namefunc (func): function to apply to name

    Returns:
      dict: map of (symbol, decl) pairs describing the prototype
    """

    result = OrderedDict()

    param_num = 0
    while declaration:
        logging.debug('decl=[%s]', declaration)

        # skip whitespace and commas
        declaration, n = re.subn(r'^[\s,]+', '', declaration)
        if n:
            continue

        declaration, n = re.subn(r'^void\s*[,\n]', '', declaration)
        if n:
            if param_num != 0:
                logging.warning('void used as parameter %d in function %s', param_num, declaration)
            result['void'] = namefunc('<type>void</type>')
            param_num += 1
            continue

        declaration, n = re.subn(r'^\s*[_a-zA-Z0-9]*\.\.\.\s*[,\n]', '', declaration)
        if n:
            result['...'] = namefunc('...')
            param_num += 1
            continue

        # allow alphanumerics, '_', '[' & ']' in param names, try to match a standard parameter
        #              $1                                                                                                                                            $2                             $3                                                                                                $4       $5
        regex = r'^\s*((?:(?:G_CONST_RETURN|G_GNUC_[A-Z_]+\s+|unsigned long|unsigned short|signed long|signed short|unsigned|signed|long|short|volatile|const)\s+)*)((?:struct\b|enum\b)?\s*\w+)\s*((?:(?:const\b|restrict\b|G_GNUC_[A-Z_]+\b)?\s*\*?\s*(?:const\b|restrict\b|G_GNUC_[A-Z_]+\b)?\s*)*)(\w+)?\s*((?:\[\S*\])*)\s*(?:G_GNUC_[A-Z_]+)?\s*[,\n]'
        m = re.match(regex, declaration)
        if m:
            declaration = re.sub(regex, '', declaration)

            pre = m.group(1) or ''
            type = m.group(2)
            ptr = m.group(3) or ''
            name = m.group(4) or ''
            array = m.group(5) or ''

            pre = re.sub(r'\s+', ' ', pre)
            type = re.sub(r'\s+', ' ', type)
            ptr = re.sub(r'\s+', ' ', ptr)
            ptr = re.sub(r'\s+$', '', ptr)
            if ptr and not ptr.endswith('*'):
                ptr += ' '

            logging.debug('"%s" "%s" "%s" "%s" "%s"', pre, type, ptr, name, array)

            m = re.search(r'^((un)?signed .*)\s?', pre)
            if name == '' and m:
                name = type
                type = m.group(1)
                pre = ''

            if name == '':
                name = 'Param' + str(param_num + 1)

            logging.debug('"%s" "%s" "%s" "%s" "%s"', pre, type, ptr, name, array)

            xref = typefunc(type, '<type>%s</type>' % type)
            result[name] = namefunc('%s%s %s%s%s' % (pre, xref, ptr, name, array))
            param_num += 1
            continue

        # Try to match parameters which are functions
        #           $1                                                                  $2          $3      $4                        $5              $6            $7             $8
        regex = r'^(const\s+|G_CONST_RETURN\s+|G_GNUC_[A-Z_]+\s+|signed\s+|unsigned\s+)*(struct\s+)?(\w+)\s*(\**)\s*(?:restrict\b)?\s*(const\s+)?\(\s*(\*[\s\*]*)\s*(\w+)\s*\)\s*\(([^)]*)\)\s*[,\n]'
        m = re.match(regex, declaration)
        if m:
            declaration = re.sub(regex, '', declaration)

            mod1 = m.group(1) or ''
            if m.group(2):
                mod1 += m.group(2)
            type = m.group(3)
            ptr1 = m.group(4)
            mod2 = m.group(5) or ''
            func_ptr = m.group(6)
            name = m.group(7)
            func_params = m.group(8) or ''

            if ptr1 and not ptr1.endswith('*'):
                ptr1 += ' '
            func_ptr = re.sub(r'\s+', ' ', func_ptr)

            logging.debug('"%s" "%s" "%s" "%s" "%s"', mod1, type, mod2, func_ptr, name)

            xref = typefunc(type, '<type>%s</type>' % type)
            result[name] = namefunc('%s%s%s%s (%s%s) (%s)' % (mod1, xref, ptr1, mod2, func_ptr, name, func_params))
            param_num += 1
            continue

        logging.warning('Cannot parse args for function in "%s"', declaration)
        break

    return result


def ParseMacroDeclaration(declaration, namefunc):
    """Parse a macro declaration.

    This function takes a macro declaration and breaks it into individual
    parameter declarations.

    Args:
      declaration (str): the declaration to parse
      namefunc (func): function to apply to name

    Returns:
      dict: map of (symbol, decl) pairs describing the macro
    """

    result = OrderedDict()

    logging.debug('decl=[%s]', declaration)

    m = re.search(r'^\s*#\s*define\s+\w+\(([^\)]*)\)', declaration)
    if m:
        params = m.group(1)
        params = re.sub(r'\n', '', params)

        logging.debug('params=[%s]', params)

        for param in params.split(','):
            param = param.strip()

            # Allow varargs variations
            if param.endswith('...'):
                param = '...'

            if param != '':
                result[param] = namefunc(param)

    return result
