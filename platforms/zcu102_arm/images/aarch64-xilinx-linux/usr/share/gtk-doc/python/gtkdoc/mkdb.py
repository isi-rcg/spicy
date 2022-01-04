# -*- python; coding: utf-8 -*-
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
Creates the DocBook files from the source comments.
"""

from collections import OrderedDict
import logging
import os
import re
import string

from . import common, md_to_db

# Options
MODULE = None
DB_OUTPUT_DIR = None
INLINE_MARKUP_MODE = None
NAME_SPACE = ''
ROOT_DIR = '.'

# These global arrays store information on signals. Each signal has an entry
# in each of these arrays at the same index, like a multi-dimensional array.
SignalObjects = []        # The GtkObject which emits the signal.
SignalNames = []        # The signal name.
SignalReturns = []        # The return type.
SignalFlags = []        # Flags for the signal
SignalPrototypes = []        # The rest of the prototype of the signal handler.

# These global arrays store information on Args. Each Arg has an entry
# in each of these arrays at the same index, like a multi-dimensional array.
ArgObjects = []                # The GtkObject which has the Arg.
ArgNames = []                # The Arg name.
ArgTypes = []                # The Arg type - gint, GtkArrowType etc.
ArgFlags = []                # How the Arg can be used - readable/writable etc.
ArgNicks = []                # The nickname of the Arg.
ArgBlurbs = []          # Docstring of the Arg.
ArgDefaults = []        # Default value of the Arg.
ArgRanges = []                # The range of the Arg type

# These global hashes store declaration info keyed on a symbol name.
Declarations = {}
DeclarationTypes = {}
DeclarationConditional = {}
DeclarationOutput = {}
Deprecated = {}
Since = {}
StabilityLevel = {}
StructHasTypedef = {}

# These global hashes store the existing documentation.
SymbolDocs = {}
SymbolParams = {}
SymbolAnnotations = {}

# These global hashes store documentation scanned from the source files.
SourceSymbolDocs = {}
SourceSymbolParams = {}
SymbolSourceLocation = {}

# all documentation goes in here, so we can do coverage analysis
AllSymbols = {}
AllIncompleteSymbols = {}
AllUnusedSymbols = {}
AllDocumentedSymbols = {}

# Undeclared yet documented symbols
UndeclaredSymbols = {}

# These global arrays store GObject, subclasses and the hierarchy (also of
# non-object derived types).
Objects = []
ObjectLevels = []
ObjectRoots = {}

Interfaces = {}
Prerequisites = {}

# holds the symbols which are mentioned in <MODULE>-sections.txt and in which
# section they are defined
KnownSymbols = {}  # values are 1 for public symbols and 0 otherwise
SymbolSection = {}
SymbolSectionId = {}

# collects index entries
IndexEntriesFull = {}
IndexEntriesSince = {}
IndexEntriesDeprecated = {}

# Standard C preprocessor directives, which we ignore for '#' abbreviations.
PreProcessorDirectives = {
    'assert', 'define', 'elif', 'else', 'endif', 'error', 'if', 'ifdef', 'ifndef',
    'include', 'line', 'pragma', 'unassert', 'undef', 'warning'
}

# remember used annotation (to write minimal glossary)
AnnotationsUsed = {}

# the regexp that parses the annotation is in ScanSourceFile()
AnnotationDefinition = {
    # the GObjectIntrospection annotations are defined at:
    # https://live.gnome.org/GObjectIntrospection/Annotations
    'allow-none': "NULL is OK, both for passing and for returning.",
    'nullable': "NULL may be passed as the value in, out, in-out; or as a return value.",
    'not nullable': "NULL must not be passed as the value in, out, in-out; or as a return value.",
    'optional': "NULL may be passed instead of a pointer to a location.",
    'not optional': "NULL must not be passed as the pointer to a location.",
    'array': "Parameter points to an array of items.",
    'attribute': "Deprecated free-form custom annotation, replaced by (attributes) annotation.",
    'attributes': "Free-form key-value pairs.",
    'closure': "This parameter is a 'user_data', for callbacks; many bindings can pass NULL here.",
    'constructor': "This symbol is a constructor, not a static method.",
    'destroy': "This parameter is a 'destroy_data', for callbacks.",
    'default': "Default parameter value (in case a function which shadows this one via <acronym>rename-to</acronym> has fewer parameters).",
    'element-type': "Generics and defining elements of containers and arrays.",
    'error-domains': "Typed errors. Similar to throws in Java.",
    'foreign': "This is a foreign struct.",
    'get-value-func': "The specified function is used to convert a struct from a GValue, must be a GTypeInstance.",
    'in': "Parameter for input. Default is <acronym>transfer none</acronym>.",
    'inout': "Parameter for input and for returning results. Default is <acronym>transfer full</acronym>.",
    'in-out': "Parameter for input and for returning results. Default is <acronym>transfer full</acronym>.",
    'method': "This is a method",
    'not-error': "A GError parameter is not to be handled like a normal GError.",
    'out': "Parameter for returning results. Default is <acronym>transfer full</acronym>.",
    'out caller-allocates': "Out parameter, where caller must allocate storage.",
    'out callee-allocates': "Out parameter, where callee must allocate storage.",
    'ref-func': "The specified function is used to ref a struct, must be a GTypeInstance.",
    'rename-to': "Rename the original symbol's name to SYMBOL.",
    'scope call': "The callback is valid only during the call to the method.",
    'scope async': "The callback is valid until first called.",
    'scope notified': "The callback is valid until the GDestroyNotify argument is called.",
    'set-value-func': "The specified function is used to convert from a struct to a GValue, must be a GTypeInstance.",
    'skip': "Exposed in C code, not necessarily available in other languages.",
    'transfer container': "Free data container after the code is done.",
    'transfer floating': "Alias for <acronym>transfer none</acronym>, used for objects with floating refs.",
    'transfer full': "Free data after the code is done.",
    'transfer none': "Don't free data after the code is done.",
    'type': "Override the parsed C type with given type.",
    'unref-func': "The specified function is used to unref a struct, must be a GTypeInstance.",
    'virtual': "This is the invoker for a virtual method.",
    'value': "The specified value overrides the evaluated value of the constant.",
    # Stability Level definition
    # https://bugzilla.gnome.org/show_bug.cgi?id=170860
    'Stable': '''The intention of a Stable interface is to enable arbitrary third parties to
develop applications to these interfaces, release them, and have confidence that
they will run on all minor releases of the product (after the one in which the
interface was introduced, and within the same major release). Even at a major
release, incompatible changes are expected to be rare, and to have strong
justifications.
''',
    'Unstable': '''Unstable interfaces are experimental or transitional. They are typically used to
give outside developers early access to new or rapidly changing technology, or
to provide an interim solution to a problem where a more general solution is
anticipated. No claims are made about either source or binary compatibility from
one minor release to the next.

The Unstable interface level is a warning that these interfaces are  subject to
change without warning and should not be used in unbundled products.

Given such caveats, customer impact need not be a factor when considering
incompatible changes to an Unstable interface in a major or minor release.
Nonetheless, when such changes are introduced, the changes should still be
mentioned in the release notes for the affected release.
''',
    'Private': '''An interface that can be used within the GNOME stack itself, but that is not
documented for end-users.  Such functions should only be used in specified and
documented ways.
''',
}

# Function and other declaration output settings.
RETURN_TYPE_FIELD_WIDTH = 20
MAX_SYMBOL_FIELD_WIDTH = 40

# XML header
doctype_header = None

# docbook templates
DB_REFENTRY = string.Template('''${header}
<refentry id="${section_id}">
<refmeta>
<refentrytitle role="top_of_page" id="${section_id}.top_of_page">${title}</refentrytitle>
<manvolnum>3</manvolnum>
<refmiscinfo>${MODULE} Library${image}</refmiscinfo>
</refmeta>
<refnamediv>
<refname>${title}</refname>
<refpurpose>${short_desc}</refpurpose>
</refnamediv>
${stability}
${functions_synop}${args_synop}${signals_synop}${object_anchors}${other_synop}${hierarchy}${prerequisites}${derived}${interfaces}${implementations}
${include_output}
<refsect1 id="${section_id}.description" role="desc">
<title role="desc.title">Description</title>
${extralinks}${long_desc}
</refsect1>
<refsect1 id="${section_id}.functions_details" role="details">
<title role="details.title">Functions</title>
${functions_details}
</refsect1>
${other_desc}${args_desc}${signals_desc}${see_also}
</refentry>
''')

DB_REFSECT1_SYNOPSIS = string.Template('''<refsect1 id="${section_id}.${type}" role="${role}">
<title role="${role}.title">${title}</title>
<informaltable frame="none">
<tgroup cols="3">
<colspec colname="${role}_type" colwidth="150px"/>
<colspec colname="${role}_name" colwidth="300px"/>
<colspec colname="${role}_flags" colwidth="200px"/>
<tbody>
${content}
</tbody>
</tgroup>
</informaltable>
</refsect1>
''')

DB_REFSECT1_DESC = string.Template('''<refsect1 id="${section_id}.${type}" role="${role}">
<title role="${role}.title">${title}</title>
${content}
</refsect1>
''')


def Run(options):
    global MODULE, INLINE_MARKUP_MODE, NAME_SPACE, DB_OUTPUT_DIR, doctype_header

    logging.info('options: %s', str(options.__dict__))

    # We should pass the options variable around instead of this global variable horror
    # but too much of the code expects these to be around. Fix this once the transition is done.
    MODULE = options.module
    INLINE_MARKUP_MODE = options.xml_mode or options.sgml_mode
    NAME_SPACE = options.name_space

    main_sgml_file = options.main_sgml_file
    if not main_sgml_file:
        # backwards compatibility
        if os.path.exists(MODULE + "-docs.sgml"):
            main_sgml_file = MODULE + "-docs.sgml"
        else:
            main_sgml_file = MODULE + "-docs.xml"

    # -- phase 1: read files produced by previous tools and scane sources

    # extract docbook header or define default
    doctype_header = GetDocbookHeader(main_sgml_file)

    ReadKnownSymbols(os.path.join(ROOT_DIR, MODULE + "-sections.txt"))
    ReadSignalsFile(os.path.join(ROOT_DIR, MODULE + ".signals"))
    ReadArgsFile(os.path.join(ROOT_DIR, MODULE + ".args"))
    obj_tree = ReadObjectHierarchy(os.path.join(ROOT_DIR, MODULE + ".hierarchy"))
    ReadInterfaces(os.path.join(ROOT_DIR, MODULE + ".interfaces"))
    ReadPrerequisites(os.path.join(ROOT_DIR, MODULE + ".prerequisites"))

    ReadDeclarationsFile(os.path.join(ROOT_DIR, MODULE + "-decl.txt"), 0)
    if os.path.isfile(os.path.join(ROOT_DIR, MODULE + "-overrides.txt")):
        ReadDeclarationsFile(os.path.join(ROOT_DIR, MODULE + "-overrides.txt"), 1)

    logging.info("Data files read")

    # -- phase 2: scan sources

    # TODO: move this to phase 3 once we fixed the call to OutputProgramDBFile()
    DB_OUTPUT_DIR = DB_OUTPUT_DIR if DB_OUTPUT_DIR else os.path.join(ROOT_DIR, "xml")
    if not os.path.isdir(DB_OUTPUT_DIR):
        os.mkdir(DB_OUTPUT_DIR)

    # Scan sources
    if options.source_suffixes:
        suffix_list = ['.' + ext for ext in options.source_suffixes.split(',')]
    else:
        suffix_list = ['.c', '.h']

    source_dirs = options.source_dir
    ignore_files = options.ignore_files
    logging.info(" ignore files: " + ignore_files)
    for sdir in source_dirs:
        ReadSourceDocumentation(sdir, suffix_list, source_dirs, ignore_files)

    logging.info("Sources scanned")

    # -- phase 3: write docbook files

    changed, book_top, book_bottom = OutputDB(os.path.join(ROOT_DIR, MODULE + "-sections.txt"), options)
    OutputBook(main_sgml_file, book_top, book_bottom, obj_tree)

    # If any of the DocBook files have changed, update the timestamp file (so
    # it can be used for Makefile dependencies).
    if changed or not os.path.exists(os.path.join(ROOT_DIR, "sgml.stamp")):
        # TODO: MakeIndexterms() uses NAME_SPACE, but also fills IndexEntriesFull
        # which DetermineNamespace is using
        # Can we use something else?
        # no: AllSymbols, KnownSymbols
        # IndexEntriesFull: consist of all symbols from sections file + signals and properties
        #
        # logging.info('# index_entries_full=%d, # declarations=%d',
        #     len(IndexEntriesFull), len(Declarations))
        # logging.info('known_symbols - index_entries_full: ' + str(Declarations.keys() - IndexEntriesFull.keys()))

        # try to detect the common prefix
        # GtkWidget, GTK_WIDGET, gtk_widget -> gtk
        if NAME_SPACE == '':
            NAME_SPACE = DetermineNamespace(IndexEntriesFull.keys())

        logging.info('namespace prefix ="%s"', NAME_SPACE)

        OutputObjectTree(obj_tree)
        OutputObjectList()

        OutputIndex("api-index-full", IndexEntriesFull)
        OutputIndex("api-index-deprecated", IndexEntriesDeprecated)
        OutputSinceIndexes()
        OutputAnnotationGlossary()

        with open(os.path.join(ROOT_DIR, 'sgml.stamp'), 'w') as h:
            h.write('timestamp')

    logging.info("All files created: %d", changed)


def OutputObjectList():
    """This outputs the alphabetical list of objects, in a columned table."""
    # FIXME: Currently this also outputs ancestor objects which may not actually
    # be in this module.
    cols = 3

    # FIXME: use .xml
    old_object_index = os.path.join(DB_OUTPUT_DIR, "object_index.sgml")
    new_object_index = os.path.join(DB_OUTPUT_DIR, "object_index.new")

    OUTPUT = open(new_object_index, 'w', encoding='utf-8')

    OUTPUT.write('''%s
<informaltable pgwide="1" frame="none">
<tgroup cols="%s">
<colspec colwidth="1*"/>
<colspec colwidth="1*"/>
<colspec colwidth="1*"/>
<tbody>
''' % (MakeDocHeader("informaltable"), cols))

    count = 0
    object = None
    for object in sorted(Objects):
        xref = MakeXRef(object)
        if count % cols == 0:
            OUTPUT.write("<row>\n")
        OUTPUT.write("<entry>%s</entry>\n" % xref)
        if count % cols == cols - 1:
            OUTPUT.write("</row>\n")
        count += 1

    if count == 0:
        # emit an empty row, since empty tables are invalid
        OUTPUT.write("<row><entry> </entry></row>\n")

    else:
        if count % cols > 0:
            OUTPUT.write("</row>\n")

    OUTPUT.write('''</tbody></tgroup></informaltable>\n''')
    OUTPUT.close()

    common.UpdateFileIfChanged(old_object_index, new_object_index, 0)


def trim_leading_and_trailing_nl(text):
    """Trims extra newlines.

    Leaves a single trailing newline.

    Args:
        text (str): the text block to trim. May contain newlines.

    Returns:
       (str): trimmed text
    """
    text = re.sub(r'^\n*', '', text)
    return re.sub(r'\n+$', '\n', text)


def trim_white_spaces(text):
    """Trims extra whitespace.

    Empty lines inside a block are preserved. All whitespace and the end is
    replaced with a single newline.

    Args:
        text (str): the text block to trim. May contain newlines.

    Returns:
       (str): trimmed text
    """

    # strip trailing spaces on every line
    return re.sub(r'\s+$', '\n', text.lstrip(), flags=re.MULTILINE)


def make_refsect1_synopsis(content, title, section_id, section_type, role=None):
    # TODO(ensonic): canonicalize xml to use the same string for section_type
    # and role. Needs fixes on gtk-doc.xsl
    if role is None:
        role = section_type.replace('-', '_')

    return DB_REFSECT1_SYNOPSIS.substitute({
        'content': content,
        'role': role,
        'section_id': section_id,
        'title': title,
        'type': section_type,
    })


def make_refsect1_desc(content, title, section_id, section_type, role=None):
    content = trim_white_spaces(content)
    if content == '':
        return ''

    # TODO(ensonic): canonicalize xml to use the same string for section_type
    # and role. Needs fixes on gtk-doc.xsl
    if role is None:
        role = section_type.replace('-', '_')

    return DB_REFSECT1_DESC.substitute({
        'content': content,
        'role': role,
        'section_id': section_id,
        'title': title,
        'type': section_type,
    })


def OutputDB(file, options):
    """Generate docbook files.

    This collects the output for each section of the docs, and outputs each file
    when the end of the section is found.

    Args:
        file (str): the $MODULE-sections.txt file which contains all of the
                    functions/macros/structs etc. being documented, organised
                    into sections and subsections.
        options:    commandline options
    """

    logging.info("Reading: %s", file)
    INPUT = open(file, 'r', encoding='utf-8')
    filename = ''
    book_top = ''
    book_bottom = ''
    includes = options.default_includes or ''
    section_includes = ''
    in_section = 0
    title = ''
    section_id = ''
    subsection = ''
    num_symbols = 0
    changed = 0
    functions_synop = ''
    other_synop = ''
    functions_details = ''
    other_details = ''
    other_desc = ''
    signals_synop = ''
    signals_desc = ''
    args_synop = ''
    child_args_synop = ''
    style_args_synop = ''
    args_desc = ''
    child_args_desc = ''
    style_args_desc = ''
    hierarchy_str = ''
    hierarchy = []
    interfaces = ''
    implementations = ''
    prerequisites = ''
    derived = ''
    file_objects = []
    file_def_line = {}
    symbol_def_line = {}

    MergeSourceDocumentation()

    line_number = 0
    for line in INPUT:
        line_number += 1

        if line.startswith('#'):
            continue

        logging.info("section file data: %d: %s", line_number, line)

        m1 = re.search(r'^<SUBSECTION\s*(.*)>', line, re.I)
        m2 = re.search(r'^<TITLE>(.*)<\/TITLE', line)
        m3 = re.search(r'^<FILE>(.*)<\/FILE>', line)
        m4 = re.search(r'^<INCLUDE>(.*)<\/INCLUDE>', line)
        m5 = re.search(r'^(\S+)', line)

        if line.startswith('<SECTION>'):
            num_symbols = 0
            in_section = False
            file_objects = []
            symbol_def_line = {}

        elif m1:
            other_synop += "\n"
            functions_synop += "\n"
            subsection = m1.group(1)

        elif line.startswith('<SUBSECTION>'):
            continue
        elif m2:
            title = m2.group(1)
            logging.info("Section: %s", title)

            # We don't want warnings if object & class structs aren't used.
            DeclarationOutput[title] = 1
            DeclarationOutput["%sClass" % title] = 1
            DeclarationOutput["%sIface" % title] = 1
            DeclarationOutput["%sInterface" % title] = 1

        elif m3:
            filename = m3.group(1)
            if filename not in file_def_line:
                file_def_line[filename] = line_number
            else:
                common.LogWarning(file, line_number, "Double <FILE>%s</FILE> entry. Previous occurrence on line %s." %
                                  (filename, file_def_line[filename]))
            if title == '':
                key = filename + ":title"
                if key in SourceSymbolDocs:
                    title = SourceSymbolDocs[key].rstrip()

        elif m4:
            if in_section:
                section_includes = m4.group(1)
            else:
                if options.default_includes:
                    common.LogWarning(file, line_number, "Default <INCLUDE> being overridden by command line option.")
                else:
                    includes = m4.group(1)

        elif re.search(r'^<\/SECTION>', line):
            logging.info("End of section: %s", title)
            # TODO: also output if we have sections docs?
            # long_desc = SymbolDocs.get(filename + ":long_description")
            if num_symbols > 0:
                # collect documents
                book_bottom += "    <xi:include href=\"xml/%s.xml\"/>\n" % filename

                key = filename + ":include"
                if key in SourceSymbolDocs:
                    if section_includes:
                        common.LogWarning(file, line_number, "Section <INCLUDE> being overridden by inline comments.")
                    section_includes = SourceSymbolDocs[key]

                if section_includes == '':
                    section_includes = includes

                signals_synop = trim_leading_and_trailing_nl(signals_synop)
                if signals_synop != '':
                    signals_synop = make_refsect1_synopsis(
                        signals_synop, 'Signals', section_id, 'signals', 'signal_proto')
                    signals_desc = make_refsect1_desc(signals_desc, 'Signal Details',
                                                      section_id, 'signal-details', 'signals')

                args_synop = trim_leading_and_trailing_nl(args_synop)
                if args_synop != '':
                    args_synop = make_refsect1_synopsis(args_synop, 'Properties', section_id, 'properties')
                    args_desc = make_refsect1_desc(args_desc, 'Property Details', section_id, 'property-details')

                child_args_synop = trim_leading_and_trailing_nl(child_args_synop)
                if child_args_synop != '':
                    args_synop += make_refsect1_synopsis(child_args_synop,
                                                         'Child Properties', section_id, 'child-properties')
                    args_desc += make_refsect1_desc(child_args_desc, 'Child Property Details',
                                                    section_id, 'child-property-details')

                style_args_synop = trim_leading_and_trailing_nl(style_args_synop)
                if style_args_synop != '':
                    args_synop += make_refsect1_synopsis(style_args_synop,
                                                         'Style Properties', section_id, 'style-properties')
                    args_desc += make_refsect1_desc(style_args_desc, 'Style Property Details',
                                                    section_id, 'style-property-details')

                hierarchy_str = AddTreeLineArt(hierarchy)
                if hierarchy_str != '':
                    hierarchy_str = make_refsect1_desc('<screen>' + hierarchy_str + '\n</screen>',
                                                       'Object Hierarchy', section_id, 'object-hierarchy')

                interfaces = make_refsect1_desc(interfaces, 'Implemented Interfaces', section_id,
                                                'implemented-interfaces', 'impl_interfaces')
                implementations = make_refsect1_desc(
                    implementations, 'Known Implementations', section_id, 'implementations')
                prerequisites = make_refsect1_desc(prerequisites, 'Prerequisites', section_id, 'prerequisites')
                derived = make_refsect1_desc(derived, 'Known Derived Interfaces', section_id, 'derived-interfaces')

                functions_synop = trim_leading_and_trailing_nl(functions_synop)
                if functions_synop != '':
                    functions_synop = '''<refsect1 id="%s.functions" role="functions_proto">
<title role="functions_proto.title">Functions</title>
<informaltable pgwide="1" frame="none">
<tgroup cols="2">
<colspec colname="functions_return" colwidth="150px"/>
<colspec colname="functions_name"/>
<tbody>
%s
</tbody>
</tgroup>
</informaltable>
</refsect1>
''' % (section_id, functions_synop)

                other_synop = trim_leading_and_trailing_nl(other_synop)
                if other_synop != '':
                    other_synop = '''<refsect1 id="%s.other" role="other_proto">
<title role="other_proto.title">Types and Values</title>
<informaltable pgwide="1" frame="none">
<tgroup cols="2">
<colspec colname="name" colwidth="150px"/>
<colspec colname="description"/>
<tbody>
%s
</tbody>
</tgroup>
</informaltable>
</refsect1>
''' % (section_id, other_synop)
                    other_desc += make_refsect1_desc(other_details, 'Types and Values',
                                                     section_id, 'other_details', 'details')

                file_changed = OutputDBFile(filename, title, section_id,
                                            section_includes,
                                            functions_synop, other_synop,
                                            functions_details, other_desc,
                                            signals_synop, signals_desc,
                                            args_synop, args_desc,
                                            hierarchy_str, interfaces,
                                            implementations,
                                            prerequisites, derived,
                                            file_objects,
                                            options.default_stability)
                if file_changed:
                    changed = True

            title = ''
            section_id = ''
            subsection = ''
            in_section = 0
            section_includes = ''
            functions_synop = ''
            other_synop = ''
            functions_details = ''
            other_details = ''
            other_desc = ''
            signals_synop = ''
            signals_desc = ''
            args_synop = ''
            child_args_synop = ''
            style_args_synop = ''
            args_desc = ''
            child_args_desc = ''
            style_args_desc = ''
            hierarchy_str = ''
            hierarchy = []
            interfaces = ''
            implementations = ''
            prerequisites = ''
            derived = ''

        elif m5:
            symbol = m5.group(1)
            logging.info('  Symbol: "%s" in subsection: "%s"', symbol, subsection)

            # check for duplicate entries
            if symbol not in symbol_def_line:
                declaration = Declarations.get(symbol)
                # FIXME: with this we'll output empty declaration
                if declaration is not None:
                    if CheckIsObject(symbol):
                        file_objects.append(symbol)

                    # We don't want standard macros/functions of GObjects,
                    # or private declarations.
                    if subsection != "Standard" and subsection != "Private":
                        synop, desc = OutputDeclaration(symbol, declaration)
                        type = DeclarationTypes[symbol]

                        if type == 'FUNCTION' or type == 'USER_FUNCTION':
                            functions_synop += synop
                            functions_details += desc
                        elif type == 'MACRO' and re.search(symbol + r'\(', declaration):
                            functions_synop += synop
                            functions_details += desc
                        else:
                            other_synop += synop
                            other_details += desc

                    sig_synop, sig_desc = GetSignals(symbol)
                    arg_synop, child_arg_synop, style_arg_synop, arg_desc, child_arg_desc, style_arg_desc = GetArgs(
                        symbol)
                    ifaces = GetInterfaces(symbol)
                    impls = GetImplementations(symbol)
                    prereqs = GetPrerequisites(symbol)
                    der = GetDerived(symbol)
                    hierarchy = GetHierarchy(symbol, hierarchy)

                    signals_synop += sig_synop
                    signals_desc += sig_desc
                    args_synop += arg_synop
                    child_args_synop += child_arg_synop
                    style_args_synop += style_arg_synop
                    args_desc += arg_desc
                    child_args_desc += child_arg_desc
                    style_args_desc += style_arg_desc
                    interfaces += ifaces
                    implementations += impls
                    prerequisites += prereqs
                    derived += der

                    # Note that the declaration has been output.
                    DeclarationOutput[symbol] = True
                elif subsection != "Standard" and subsection != "Private":
                    UndeclaredSymbols[symbol] = True
                    common.LogWarning(file, line_number, "No declaration found for %s." % symbol)

                num_symbols += 1
                symbol_def_line[symbol] = line_number

                if section_id == '':
                    if title == '' and filename == '':
                        common.LogWarning(file, line_number, "Section has no title and no file.")

                    # FIXME: one of those would be enough
                    # filename should be an internal detail for gtk-doc
                    if title == '':
                        title = filename
                    elif filename == '':
                        filename = title

                    filename = filename.replace(' ', '_')

                    section_id = SourceSymbolDocs.get(filename + ":section_id")
                    if section_id and section_id.strip() != '':
                        # Remove trailing blanks and use as is
                        section_id = section_id.rstrip()
                    elif CheckIsObject(title):
                        # GObjects use their class name as the ID.
                        section_id = common.CreateValidSGMLID(title)
                    else:
                        section_id = common.CreateValidSGMLID(MODULE + '-' + title)

                SymbolSection[symbol] = title
                SymbolSectionId[symbol] = section_id

            else:
                common.LogWarning(file, line_number, "Double symbol entry for %s. "
                                  "Previous occurrence on line %d." % (symbol, symbol_def_line[symbol]))
    INPUT.close()

    OutputMissingDocumentation()
    OutputUndeclaredSymbols()
    OutputUnusedSymbols()

    if options.outputallsymbols:
        OutputAllSymbols()

    if options.outputsymbolswithoutsince:
        OutputSymbolsWithoutSince()

    for filename in options.expand_content_files.split():
        file_changed = OutputExtraFile(filename)
        if file_changed:
            changed = True

    return (changed, book_top, book_bottom)


def DetermineNamespace(symbols):
    """Find common set of characters.

    Args:
         symbols (list): a list of symbols to scan for a common prefix

    Returns:
        str: a common namespace prefix (might be empty)
    """
    name_space = ''
    pos = 0
    ratio = 0.0
    while True:
        prefix = {}
        letter = ''
        for symbol in symbols:
            if name_space == '' or name_space.lower() in symbol.lower():
                if len(symbol) > pos:
                    letter = symbol[pos:pos + 1]
                    # stop prefix scanning
                    if letter == "_":
                        # stop on "_"
                        break
                    # Should we also stop on a uppercase char, if last was lowercase
                    #   GtkWidget, if we have the 'W' and had the 't' before
                    # or should we count upper and lowercase, and stop one 2nd uppercase, if we already had a lowercase
                    #   GtkWidget, the 'W' would be the 2nd uppercase and with 't','k' we had lowercase chars before
                    # need to recound each time as this is per symbol
                    ul = letter.upper()
                    if ul in prefix:
                        prefix[ul] += 1
                    else:
                        prefix[ul] = 1

        if letter != '' and letter != "_":
            maxletter = ''
            maxsymbols = 0
            for letter in prefix.keys():
                logging.debug("ns prefix: %s: %s", letter, prefix[letter])
                if prefix[letter] > maxsymbols:
                    maxletter = letter
                    maxsymbols = prefix[letter]

            ratio = float(len(symbols)) / prefix[maxletter]
            logging.debug('most symbols start with %s, that is %f', maxletter, (100 * ratio))
            if ratio > 0.9:
                # do another round
                name_space += maxletter

            pos += 1

        else:
            ratio = 0.0

        if ratio < 0.9:
            break
    return name_space


def OutputIndex(basename, apiindex):
    """Writes an index that can be included into the main-document into an <index> tag.

    Args:
        basename (str): name of the index file without extension
        apiindex (dict): the index data
    """
    old_index = os.path.join(DB_OUTPUT_DIR, basename + '.xml')
    new_index = os.path.join(DB_OUTPUT_DIR, basename + '.new')
    lastletter = " "
    divopen = 0
    symbol = None
    short_symbol = None

    OUTPUT = open(new_index, 'w')

    OUTPUT.write(MakeDocHeader("indexdiv") + "\n<indexdiv id=\"%s\">\n" % basename)

    logging.info("generate %s index (%d entries) with namespace %s", basename, len(apiindex), NAME_SPACE)

    # do a case insensitive sort while chopping off the prefix
    mapped_keys = [
        {
            'original': x,
            'short': re.sub(r'^' + NAME_SPACE + r'\_?(.*)', r'\1', x.upper(), flags=re.I),
        } for x in apiindex.keys()]
    sorted_keys = sorted(mapped_keys, key=lambda d: (d['short'], d['original']))

    for key in sorted_keys:
        symbol = key['original']
        short = key['short']
        if short != '':
            short_symbol = short
        else:
            short_symbol = symbol

        # generate a short symbol description
        symbol_desc = ''
        symbol_section = ''
        symbol_section_id = ''
        symbol_type = ''
        if symbol in DeclarationTypes:
            symbol_type = DeclarationTypes[symbol].lower()

        if symbol_type == '':
            logging.info("trying symbol %s", symbol)
            m1 = re.search(r'(.*)::(.*)', symbol)
            m2 = re.search(r'(.*):(.*)', symbol)
            if m1:
                oname = m1.group(1)
                osym = m1.group(2)
                logging.info("  trying object signal %s:%s in %d signals", oname, osym, len(SignalNames))
                for name in SignalNames:
                    logging.info("    " + name)
                    if name == osym:
                        symbol_type = "object signal"
                        if oname in SymbolSection:
                            symbol_section = SymbolSection[oname]
                            symbol_section_id = SymbolSectionId[oname]
                        break
            elif m2:
                oname = m2.group(1)
                osym = m2.group(2)
                logging.info("  trying object property %s::%s in %d properties", oname, osym, len(ArgNames))
                for name in ArgNames:
                    logging.info("    " + name)
                    if name == osym:
                        symbol_type = "object property"
                        if oname in SymbolSection:
                            symbol_section = SymbolSection[oname]
                            symbol_section_id = SymbolSectionId[oname]
                        break
        else:
            if symbol in SymbolSection:
                symbol_section = SymbolSection[symbol]
                symbol_section_id = SymbolSectionId[symbol]

        if symbol_type != '':
            symbol_desc = ", " + symbol_type
            if symbol_section != '':
                symbol_desc += " in <link linkend=\"%s\">%s</link>" % (symbol_section_id, symbol_section)
                # symbol_desc +=" in " + ExpandAbbreviations(symbol, "#symbol_section")

        curletter = short_symbol[0].upper()
        ixid = apiindex[symbol]

        logging.info("  add symbol %s with %s to index in section '%s' (derived from %s)",
                     symbol, ixid, curletter, short_symbol)

        if curletter != lastletter:
            lastletter = curletter

            if divopen:
                OUTPUT.write("</indexdiv>\n")

            OUTPUT.write("<indexdiv><title>%s</title>\n" % curletter)
            divopen = True

        OUTPUT.write('<indexentry><primaryie linkends="%s"><link linkend="%s">%s</link>%s</primaryie></indexentry>\n' %
                     (ixid, ixid, symbol, symbol_desc))

    if divopen:
        OUTPUT.write("</indexdiv>\n")

    OUTPUT.write("</indexdiv>\n")
    OUTPUT.close()

    common.UpdateFileIfChanged(old_index, new_index, 0)


def OutputSinceIndexes():
    """Generate the 'since' api index files."""
    for version in set(Since.values()):
        logging.info("Since : [%s]", version)
        index = {x: IndexEntriesSince[x] for x in IndexEntriesSince.keys() if Since[x] == version}
        OutputIndex("api-index-" + version, index)


def OutputAnnotationGlossary():
    """Writes a glossary of the used annotation terms.

    The glossary file can be included into the main document.
    """
    # if there are no annotations used return
    if not AnnotationsUsed:
        return

    old_glossary = os.path.join(DB_OUTPUT_DIR, "annotation-glossary.xml")
    new_glossary = os.path.join(DB_OUTPUT_DIR, "annotation-glossary.new")
    lastletter = " "
    divopen = False

    # add acronyms that are referenced from acronym text
    rerun = True
    while rerun:
        rerun = False
        for annotation in AnnotationsUsed:
            if annotation not in AnnotationDefinition:
                continue
            m = re.search(r'<acronym>([\w ]+)<\/acronym>', AnnotationDefinition[annotation])
            if m and m.group(1) not in AnnotationsUsed:
                AnnotationsUsed[m.group(1)] = 1
                rerun = True
                break

    OUTPUT = open(new_glossary, 'w', encoding='utf-8')

    OUTPUT.write('''%s
<glossary id="annotation-glossary">
  <title>Annotation Glossary</title>
''' % MakeDocHeader("glossary"))

    for annotation in sorted(AnnotationsUsed.keys(), key=str.lower):
        if annotation in AnnotationDefinition:
            definition = AnnotationDefinition[annotation]
            curletter = annotation[0].upper()

            if curletter != lastletter:
                lastletter = curletter

                if divopen:
                    OUTPUT.write("</glossdiv>\n")

                OUTPUT.write("<glossdiv><title>%s</title>\n" % curletter)
                divopen = True

            OUTPUT.write('''    <glossentry>
      <glossterm><anchor id="annotation-glossterm-%s"/>%s</glossterm>
      <glossdef>
        <para>%s</para>
      </glossdef>
    </glossentry>
''' % (annotation, annotation, definition))

    if divopen:
        OUTPUT.write("</glossdiv>\n")

    OUTPUT.write("</glossary>\n")
    OUTPUT.close()

    common.UpdateFileIfChanged(old_glossary, new_glossary, 0)


def OutputObjectTree(tree):
    if not tree:
        return

    # FIXME: use xml
    old_tree_index = os.path.join(DB_OUTPUT_DIR, "tree_index.sgml")
    new_tree_index = os.path.join(DB_OUTPUT_DIR, "tree_index.new")

    with open(new_tree_index, 'w', encoding='utf-8') as out:
        out.write(MakeDocHeader("screen"))
        out.write("\n<screen>\n")
        out.write(AddTreeLineArt(tree))
        out.write("\n</screen>\n")

    common.UpdateFileIfChanged(old_tree_index, new_tree_index, 0)


def ReadKnownSymbols(file):
    """Collect the names of non-private symbols from the $MODULE-sections.txt file.

    Args:
        file: the $MODULE-sections.txt file
    """

    subsection = ''

    logging.info("Reading: %s", file)
    INPUT = open(file, 'r', encoding='utf-8')
    for line in INPUT:
        if line.startswith('#'):
            continue

        if line.startswith('<SECTION>'):
            subsection = ''
            continue

        m = re.search(r'^<SUBSECTION\s*(.*)>', line, flags=re.I)
        if m:
            subsection = m.group(1)
            continue

        if line.startswith('<SUBSECTION>'):
            continue

        if re.search(r'^<TITLE>(.*)<\/TITLE>', line):
            continue

        m = re.search(r'^<FILE>(.*)<\/FILE>', line)
        if m:
            KnownSymbols[m.group(1) + ":long_description"] = 1
            KnownSymbols[m.group(1) + ":short_description"] = 1
            continue

        m = re.search(r'^<INCLUDE>(.*)<\/INCLUDE>', line)
        if m:
            continue

        m = re.search(r'^<\/SECTION>', line)
        if m:
            continue

        m = re.search(r'^(\S+)', line)
        if m:
            symbol = m.group(1)
            if subsection != "Standard" and subsection != "Private":
                KnownSymbols[symbol] = 1
            else:
                KnownSymbols[symbol] = 0
    INPUT.close()


def OutputDeclaration(symbol, declaration):
    """Returns the formatted documentation block for a symbol.

    Args:
        symbol (str): the name of the function/macro/...
        declaration (str): the declaration of the function/macro.

    Returns:
        str: the formatted documentation
    """

    dtype = DeclarationTypes[symbol]
    if dtype == 'MACRO':
        return OutputMacro(symbol, declaration)
    elif dtype == 'TYPEDEF':
        return OutputTypedef(symbol, declaration)
    elif dtype == 'STRUCT':
        return OutputStruct(symbol, declaration)
    elif dtype == 'ENUM':
        return OutputEnum(symbol, declaration)
    elif dtype == 'UNION':
        return OutputUnion(symbol, declaration)
    elif dtype == 'VARIABLE':
        return OutputVariable(symbol, declaration)
    elif dtype == 'FUNCTION':
        return OutputFunction(symbol, declaration, dtype)
    elif dtype == 'USER_FUNCTION':
        return OutputFunction(symbol, declaration, dtype)
    else:
        logging.warning("Unknown symbol type %s for symbol %s", dtype, symbol)
        return ('', '')


def OutputSymbolTraits(symbol):
    """Returns the Since and StabilityLevel paragraphs for a symbol.

    Args:
        symbol (str): the name to describe

    Returns:
        str: paragraph or empty string
    """

    desc = ''

    if symbol in Since:
        link_id = "api-index-" + Since[symbol]
        desc += "<para role=\"since\">Since: <link linkend=\"%s\">%s</link></para>" % (link_id, Since[symbol])

    if symbol in StabilityLevel:
        stability = StabilityLevel[symbol]
        if stability in AnnotationDefinition:
            AnnotationsUsed[stability] = True
            stability = "<acronym>%s</acronym>" % stability
        desc += "<para role=\"stability\">Stability Level: %s</para>" % stability
    return desc


def uri_escape(text):
    if text is None:
        return None

    # Build a char to hex map
    escapes = {chr(i): ("%%%02X" % i) for i in range(256)}

    # Default unsafe characters.  RFC 2732 ^(uric - reserved)
    def do_escape(char):
        return escapes[char]
    return re.sub(r"([^A-Za-z0-9\-_.!~*'()]", do_escape, text)


def extract_struct_body(symbol, decl, has_typedef, public):
    """Returns a normalized struct body.

    Normalizes whitespace and newlines and supresses non public members.

    Returns:
      str: the nomalized struct declaration
    """
    decl_out = ''

    m = re.search(
        r'^\s*(typedef\s+)?struct\s*\w*\s*(?:\/\*.*\*\/)?\s*{(.*)}\s*\w*\s*;\s*$', decl, flags=re.DOTALL)
    if m:
        new_boby = ''
        for line in m.group(2).splitlines():
            logging.info("Struct line: %s", line)
            m2 = re.search(r'/\*\s*<\s*public\s*>\s*\*/', line)
            m3 = re.search(r'/\*\s*<\s*(private|protected)\s*>\s*\*/', line)
            if m2:
                public = True
            elif m3:
                public = False
            elif public:
                new_boby += line + "\n"

        if new_boby:
            # Strip any blank lines off the ends.
            new_boby = re.sub(r'^\s*\n', '', new_boby)
            new_boby = re.sub(r'\n\s*$', r'\n', new_boby)

            if has_typedef:
                decl_out = "typedef struct {\n%s} %s;\n" % (new_boby, symbol)
            else:
                decl_out = "struct %s {\n%s};\n" % (symbol, new_boby)
    else:
        common.LogWarning(*GetSymbolSourceLocation(symbol),
                          "Couldn't parse struct:\n%s" % decl)

    if decl_out == '':
        # If we couldn't parse the struct or it was all private, output an
        # empty struct declaration.
        if has_typedef:
            decl_out = "typedef struct _%s %s;" % (symbol, symbol)
        else:
            decl_out = "struct %s;" % symbol

    return decl_out


def OutputSymbolExtraLinks(symbol):
    """Returns extralinks for the symbol (if enabled).

    Args:
        symbol (str): the name to describe

    Returns:
        str: paragraph or empty string
    """
    desc = ''

    if False:   # NEW FEATURE: needs configurability
        sstr = uri_escape(symbol)
        mstr = uri_escape(MODULE)
        desc += '''<ulink role="extralinks" url="http://www.google.com/codesearch?q=%s">code search</ulink>
<ulink role="extralinks" url="http://library.gnome.org/edit?module=%s&amp;symbol=%s">edit documentation</ulink>
''' % (sstr, mstr, sstr)

    return desc


def OutputSectionExtraLinks(symbol, docsymbol):
    desc = ''

    if False:   # NEW FEATURE: needs configurability
        sstr = uri_escape(symbol)
        mstr = uri_escape(MODULE)
        dsstr = uri_escape(docsymbol)
        desc += '''<ulink role="extralinks" url="http://www.google.com/codesearch?q=%s">code search</ulink>
<ulink role="extralinks" url="http://library.gnome.org/edit?module=%s&amp;symbol=%s">edit documentation</ulink>
''' % (sstr, mstr, dsstr)
    return desc


def OutputMacro(symbol, declaration):
    """Returns the synopsis and detailed description of a macro.

    Args:
        symbol (str): the macro name.
        declaration (str): the declaration of the macro.

    Returns:
        str: the formatted docs
    """
    sid = common.CreateValidSGMLID(symbol)
    condition = MakeConditionDescription(symbol)
    synop = "<row><entry role=\"define_keyword\">#define</entry><entry role=\"function_name\"><link linkend=\"%s\">%s</link>" % (
        sid, symbol)

    fields = common.ParseMacroDeclaration(declaration, CreateValidSGML)
    title = symbol
    if len(fields) > 0:
        title += '()'

    desc = '<refsect2 id="%s" role="macro"%s>\n<title>%s</title>\n' % (sid, condition, title)
    desc += MakeIndexterms(symbol, sid)
    desc += "\n"
    desc += OutputSymbolExtraLinks(symbol)

    if len(fields) > 0:
        synop += "<phrase role=\"c_punctuation\">()</phrase>"

    synop += "</entry></row>\n"

    # Don't output the macro definition if is is a conditional macro or it
    # looks like a function, i.e. starts with "g_" or "_?gnome_", or it is
    # longer than 2 lines, otherwise we get lots of complicated macros like
    # g_assert.
    if symbol not in DeclarationConditional and not symbol.startswith('g_') \
            and not re.search(r'^_?gnome_', symbol) and declaration.count('\n') < 2:
        decl_out = CreateValidSGML(declaration)
        desc += "<programlisting language=\"C\">%s</programlisting>\n" % decl_out
    else:
        desc += "<programlisting language=\"C\">" + "#define".ljust(RETURN_TYPE_FIELD_WIDTH) + symbol
        m = re.search(r'^\s*#\s*define\s+\w+(\([^\)]*\))', declaration)
        if m:
            args = m.group(1)
            pad = ' ' * (RETURN_TYPE_FIELD_WIDTH - len("#define "))
            # Align each line so that if should all line up OK.
            args = args.replace('\n', '\n' + pad)
            desc += CreateValidSGML(args)

        desc += "</programlisting>\n"

    desc += MakeDeprecationNote(symbol)

    parameters = OutputParamDescriptions("MACRO", symbol, fields)

    if symbol in SymbolDocs:
        symbol_docs = ConvertMarkDown(symbol, SymbolDocs[symbol])
        desc += symbol_docs

    desc += parameters
    desc += OutputSymbolTraits(symbol)
    desc += "</refsect2>\n"
    return (synop, desc)


def OutputTypedef(symbol, declaration):
    """Returns the synopsis and detailed description of a typedef.

    Args:
        symbol (str): the typedef.
        declaration (str): the declaration of the typedef,
                           e.g. 'typedef unsigned int guint;'

    Returns:
        str: the formatted docs
    """
    sid = common.CreateValidSGMLID(symbol)
    condition = MakeConditionDescription(symbol)
    desc = "<refsect2 id=\"%s\" role=\"typedef\"%s>\n<title>%s</title>\n" % (sid, condition, symbol)
    synop = "<row><entry role=\"typedef_keyword\">typedef</entry><entry role=\"function_name\"><link linkend=\"%s\">%s</link></entry></row>\n" % (
        sid, symbol)

    desc += MakeIndexterms(symbol, sid)
    desc += "\n"
    desc += OutputSymbolExtraLinks(symbol)

    if symbol not in DeclarationConditional:
        decl_out = CreateValidSGML(declaration)
        desc += "<programlisting language=\"C\">%s</programlisting>\n" % decl_out

    desc += MakeDeprecationNote(symbol)

    if symbol in SymbolDocs:
        desc += ConvertMarkDown(symbol, SymbolDocs[symbol])

    desc += OutputSymbolTraits(symbol)
    desc += "</refsect2>\n"
    return (synop, desc)


def OutputStruct(symbol, declaration):
    """Returns the synopsis and detailed description of a struct.

    We check if it is a object struct, and if so we only output parts of it that
    are noted as public fields. We also use a different IDs for object structs,
    since the original ID is used for the entire RefEntry.

    Args:
        symbol (str): the struct.
        declaration (str): the declaration of the struct.

    Returns:
        str: the formatted docs
    """
    is_gtype = False
    default_to_public = True
    if CheckIsObject(symbol):
        logging.info("Found struct gtype: %s", symbol)
        is_gtype = True
        default_to_public = ObjectRoots[symbol] == 'GBoxed'

    sid = None
    condition = None
    if is_gtype:
        sid = common.CreateValidSGMLID(symbol + "_struct")
        condition = MakeConditionDescription(symbol + "_struct")
    else:
        sid = common.CreateValidSGMLID(symbol)
        condition = MakeConditionDescription(symbol)

    # Determine if it is a simple struct or it also has a typedef.
    has_typedef = False
    if symbol in StructHasTypedef or re.search(r'^\s*typedef\s+', declaration):
        has_typedef = True

    type_output = None
    desc = None
    if has_typedef:
        # For structs with typedefs we just output the struct name.
        type_output = ''
        desc = "<refsect2 id=\"%s\" role=\"struct\"%s>\n<title>%s</title>\n" % (sid, condition, symbol)
    else:
        type_output = "struct"
        desc = "<refsect2 id=\"%s\" role=\"struct\"%s>\n<title>struct %s</title>\n" % (sid, condition, symbol)

    synop = "<row><entry role=\"datatype_keyword\">%s</entry><entry role=\"function_name\"><link linkend=\"%s\">%s</link></entry></row>\n" % (
        type_output, sid, symbol)

    desc += MakeIndexterms(symbol, sid)
    desc += "\n"
    desc += OutputSymbolExtraLinks(symbol)

    # Form a pretty-printed, private-data-removed form of the declaration

    decl_out = ''
    if re.search(r'^\s*$', declaration):
        logging.info("Found opaque struct: %s", symbol)
        decl_out = "typedef struct _%s %s;" % (symbol, symbol)
    elif re.search(r'^\s*struct\s+\w+\s*;\s*$', declaration):
        logging.info("Found opaque struct: %s", symbol)
        decl_out = "struct %s;" % symbol
    else:
        decl_out = extract_struct_body(symbol, declaration, has_typedef, default_to_public)

    decl_out = CreateValidSGML(decl_out)
    desc += "<programlisting language=\"C\">%s</programlisting>\n" % decl_out

    desc += MakeDeprecationNote(symbol)

    if symbol in SymbolDocs:
        desc += ConvertMarkDown(symbol, SymbolDocs[symbol])

    # Create a table of fields and descriptions

    # FIXME: Inserting &#160's into the produced type declarations here would
    #        improve the output in most situations ... except for function
    #        members of structs!
    def pfunc(*args):
        return '<structfield id="%s">%s</structfield>' % (common.CreateValidSGMLID(sid + '.' + args[0]), args[0])
    fields = common.ParseStructDeclaration(declaration, not default_to_public, 0, MakeXRef, pfunc)
    field_descrs, found = GetSymbolParams(symbol)

    if found:
        missing_parameters = ''
        unused_parameters = ''
        sid = common.CreateValidSGMLID(symbol + ".members")

        desc += '''<refsect3 id="%s" role="struct_members">\n<title>Members</title>
<informaltable role="struct_members_table" pgwide="1" frame="none">
<tgroup cols="3">
<colspec colname="struct_members_name" colwidth="300px"/>
<colspec colname="struct_members_description"/>
<colspec colname="struct_members_annotations" colwidth="200px"/>
<tbody>
''' % sid

        for field_name, text in fields.items():
            param_annotations = ''

            desc += "<row role=\"member\"><entry role=\"struct_member_name\"><para>%s</para></entry>\n" % text
            if field_name in field_descrs:
                field_descr, param_annotations = ExpandAnnotation(symbol, field_descrs[field_name])
                field_descr = ConvertMarkDown(symbol, field_descr)
                # trim
                field_descr = re.sub(r'^(\s|\n)+', '', field_descr, flags=re.M | re.S)
                field_descr = re.sub(r'(\s|\n)+$', '', field_descr, flags=re.M | re.S)
                desc += "<entry role=\"struct_member_description\">%s</entry>\n<entry role=\"struct_member_annotations\">%s</entry>\n" % (
                    field_descr, param_annotations)
                del field_descrs[field_name]
            else:
                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "Field description for %s::%s is missing in source code comment block." % (symbol, field_name))
                if missing_parameters != '':
                    missing_parameters += ", " + field_name
                else:
                    missing_parameters = field_name

                desc += "<entry /><entry />\n"

            desc += "</row>\n"

        desc += "</tbody></tgroup></informaltable>\n</refsect3>\n"
        for field_name in field_descrs:
            # Documenting those standard fields is not required anymore, but
            # we don't want to warn if they are documented anyway.
            m = re.search(r'(g_iface|parent_instance|parent_class)', field_name)
            if m:
                continue

            common.LogWarning(*GetSymbolSourceLocation(symbol),
                              "Field description for %s::%s is not used from source code comment block." % (symbol, field_name))
            if unused_parameters != '':
                unused_parameters += ", " + field_name
            else:
                unused_parameters = field_name

        # remember missing/unused parameters (needed in tmpl-free build)
        if missing_parameters != '' and (symbol not in AllIncompleteSymbols):
            AllIncompleteSymbols[symbol] = missing_parameters

        if unused_parameters != '' and (symbol not in AllUnusedSymbols):
            AllUnusedSymbols[symbol] = unused_parameters
    else:
        if fields:
            if symbol not in AllIncompleteSymbols:
                AllIncompleteSymbols[symbol] = "<items>"
                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "Field descriptions for struct %s are missing in source code comment block." % symbol)
                logging.info("Remaining structs fields: " + ':'.join(fields) + "\n")

    desc += OutputSymbolTraits(symbol)
    desc += "</refsect2>\n"
    return (synop, desc)


def OutputUnion(symbol, declaration):
    """Returns the synopsis and detailed description of a union.

    Args:
        symbol (str): the union.
        declaration (str): the declaration of the union.

    Returns:
        str: the formatted docs
    """
    is_gtype = False
    if CheckIsObject(symbol):
        logging.info("Found union gtype: %s", symbol)
        is_gtype = True

    sid = None
    condition = None
    if is_gtype:
        sid = common.CreateValidSGMLID(symbol + "_union")
        condition = MakeConditionDescription(symbol + "_union")
    else:
        sid = common.CreateValidSGMLID(symbol)
        condition = MakeConditionDescription(symbol)

    # Determine if it is a simple struct or it also has a typedef.
    has_typedef = False
    if symbol in StructHasTypedef or re.search(r'^\s*typedef\s+', declaration):
        has_typedef = True

    type_output = None
    desc = None
    if has_typedef:
        # For unions with typedefs we just output the union name.
        type_output = ''
        desc = "<refsect2 id=\"%s\" role=\"union\"%s>\n<title>%s</title>\n" % (sid, condition, symbol)
    else:
        type_output = "union"
        desc = "<refsect2 id=\"%s\" role=\"union\"%s>\n<title>union %s</title>\n" % (sid, condition, symbol)

    synop = "<row><entry role=\"datatype_keyword\">%s</entry><entry role=\"function_name\"><link linkend=\"%s\">%s</link></entry></row>\n" % (
        type_output, sid, symbol)

    desc += MakeIndexterms(symbol, sid)
    desc += "\n"
    desc += OutputSymbolExtraLinks(symbol)
    desc += MakeDeprecationNote(symbol)

    if symbol in SymbolDocs:
        desc += ConvertMarkDown(symbol, SymbolDocs[symbol])

    # Create a table of fields and descriptions

    # FIXME: Inserting &#160's into the produced type declarations here would
    #        improve the output in most situations ... except for function
    #        members of structs!
    def pfunc(*args):
        return '<structfield id="%s">%s</structfield>' % (common.CreateValidSGMLID(sid + '.' + args[0]), args[0])
    fields = common.ParseStructDeclaration(declaration, 0, 0, MakeXRef, pfunc)
    field_descrs, found = GetSymbolParams(symbol)

    logging.debug('Union %s has %d entries, found=%d, has_typedef=%d', symbol, len(fields), found, has_typedef)

    if found:
        missing_parameters = ''
        unused_parameters = ''
        sid = common.CreateValidSGMLID('%s.members' % symbol)

        desc += '''<refsect3 id="%s" role="union_members">\n<title>Members</title>
<informaltable role="union_members_table" pgwide="1" frame="none">
<tgroup cols="3">
<colspec colname="union_members_name" colwidth="300px"/>
<colspec colname="union_members_description"/>
<colspec colname="union_members_annotations" colwidth="200px"/>
<tbody>
''' % sid

        for field_name, text in fields.items():
            param_annotations = ''

            desc += "<row><entry role=\"union_member_name\"><para>%s</para></entry>\n" % text
            if field_name in field_descrs:
                field_descr, param_annotations = ExpandAnnotation(symbol, field_descrs[field_name])
                field_descr = ConvertMarkDown(symbol, field_descr)

                # trim
                field_descr = re.sub(r'^(\s|\n)+', '', field_descr, flags=re.M | re.S)
                field_descr = re.sub(r'(\s|\n)+$', '', field_descr, flags=re.M | re.S)
                desc += "<entry role=\"union_member_description\">%s</entry>\n<entry role=\"union_member_annotations\">%s</entry>\n" % (
                    field_descr, param_annotations)
                del field_descrs[field_name]
            else:
                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "Field description for %s::%s is missing in source code comment block." % (symbol, field_name))
                if missing_parameters != '':
                    missing_parameters += ", " + field_name
                else:
                    missing_parameters = field_name

                desc += "<entry /><entry />\n"

            desc += "</row>\n"

        desc += "</tbody></tgroup></informaltable>\n</refsect3>"
        for field_name in field_descrs:
            common.LogWarning(*GetSymbolSourceLocation(symbol),
                              "Field description for %s::%s is not used from source code comment block." % (symbol, field_name))
            if unused_parameters != '':
                unused_parameters += ", " + field_name
            else:
                unused_parameters = field_name

        # remember missing/unused parameters (needed in tmpl-free build)
        if missing_parameters != '' and (symbol not in AllIncompleteSymbols):
            AllIncompleteSymbols[symbol] = missing_parameters

        if unused_parameters != '' and (symbol not in AllUnusedSymbols):
            AllUnusedSymbols[symbol] = unused_parameters
    else:
        if len(fields) > 0:
            if symbol not in AllIncompleteSymbols:
                AllIncompleteSymbols[symbol] = "<items>"
                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "Field descriptions for union %s are missing in source code comment block." % symbol)
                logging.info("Remaining union fields: " + ':'.join(fields) + "\n")

    desc += OutputSymbolTraits(symbol)
    desc += "</refsect2>\n"
    return (synop, desc)


def OutputEnum(symbol, declaration):
    """Returns the synopsis and detailed description of a enum.

    Args:
        symbol (str): the enum.
        declaration (str): the declaration of the enum.

    Returns:
        str: the formatted docs
    """
    is_gtype = False
    if CheckIsObject(symbol):
        logging.info("Found enum gtype: %s", symbol)
        is_gtype = True

    sid = None
    condition = None
    if is_gtype:
        sid = common.CreateValidSGMLID(symbol + "_enum")
        condition = MakeConditionDescription(symbol + "_enum")
    else:
        sid = common.CreateValidSGMLID(symbol)
        condition = MakeConditionDescription(symbol)

    synop = "<row><entry role=\"datatype_keyword\">enum</entry><entry role=\"function_name\"><link linkend=\"%s\">%s</link></entry></row>\n" % (
        sid, symbol)
    desc = "<refsect2 id=\"%s\" role=\"enum\"%s>\n<title>enum %s</title>\n" % (sid, condition, symbol)

    desc += MakeIndexterms(symbol, sid)
    desc += "\n"
    desc += OutputSymbolExtraLinks(symbol)
    desc += MakeDeprecationNote(symbol)

    if symbol in SymbolDocs:
        desc += ConvertMarkDown(symbol, SymbolDocs[symbol])

    # Create a table of fields and descriptions

    fields = common.ParseEnumDeclaration(declaration)
    field_descrs, found = GetSymbolParams(symbol)

    missing_parameters = ''
    unused_parameters = ''

    sid = common.CreateValidSGMLID("%s.members" % symbol)
    desc += '''<refsect3 id="%s" role="enum_members">\n<title>Members</title>
<informaltable role="enum_members_table" pgwide="1" frame="none">
<tgroup cols="3">
<colspec colname="enum_members_name" colwidth="300px"/>
<colspec colname="enum_members_description"/>
<colspec colname="enum_members_annotations" colwidth="200px"/>
<tbody>
''' % sid

    for field_name in fields:
        field_descr = field_descrs.get(field_name)
        param_annotations = ''

        sid = common.CreateValidSGMLID(field_name)
        condition = MakeConditionDescription(field_name)
        desc += "<row role=\"constant\"><entry role=\"enum_member_name\"><para id=\"%s\">%s</para></entry>\n" % (
            sid, field_name)
        if field_descr:
            field_descr, param_annotations = ExpandAnnotation(symbol, field_descr)
            field_descr = ConvertMarkDown(symbol, field_descr)
            desc += "<entry role=\"enum_member_description\">%s</entry>\n<entry role=\"enum_member_annotations\">%s</entry>\n" % (
                field_descr, param_annotations)
            del field_descrs[field_name]
        else:
            if found:
                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "Value description for %s::%s is missing in source code comment block." % (symbol, field_name))
                if missing_parameters != '':
                    missing_parameters += ", " + field_name
                else:
                    missing_parameters = field_name
            desc += "<entry /><entry />\n"
        desc += "</row>\n"

    desc += "</tbody></tgroup></informaltable>\n</refsect3>"
    for field_name in field_descrs:
        common.LogWarning(*GetSymbolSourceLocation(symbol),
                          "Value description for %s::%s is not used from source code comment block." % (symbol, field_name))
        if unused_parameters != '':
            unused_parameters += ", " + field_name
        else:
            unused_parameters = field_name

    # remember missing/unused parameters (needed in tmpl-free build)
    if missing_parameters != '' and (symbol not in AllIncompleteSymbols):
        AllIncompleteSymbols[symbol] = missing_parameters

    if unused_parameters != '' and (symbol not in AllUnusedSymbols):
        AllUnusedSymbols[symbol] = unused_parameters

    if not found:
        if len(fields) > 0:
            if symbol not in AllIncompleteSymbols:
                AllIncompleteSymbols[symbol] = "<items>"
                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "Value descriptions for %s are missing in source code comment block." % symbol)

    desc += OutputSymbolTraits(symbol)
    desc += "</refsect2>\n"
    return (synop, desc)


def OutputVariable(symbol, declaration):
    """Returns the synopsis and detailed description of a variable.

    Args:
        symbol (str): the extern'ed variable.
        declaration (str): the declaration of the variable.

    Returns:
        str: the formatted docs
    """
    sid = common.CreateValidSGMLID(symbol)
    condition = MakeConditionDescription(symbol)

    logging.info("ouputing variable: '%s' '%s'", symbol, declaration)

    type_output = None
    m1 = re.search(
        r'^\s*extern\s+((const\s+|signed\s+|unsigned\s+|long\s+|short\s+)*\w+)(\s+\*+|\*+|\s)(\s*)(const\s+)*([A-Za-z]\w*)\s*;', declaration)
    m2 = re.search(
        r'\s*((const\s+|signed\s+|unsigned\s+|long\s+|short\s+)*\w+)(\s+\*+|\*+|\s)(\s*)(const\s+)*([A-Za-z]\w*)\s*=', declaration)
    if m1:
        mod1 = m1.group(1) or ''
        ptr = m1.group(3) or ''
        space = m1.group(4) or ''
        mod2 = m1.group(5) or ''
        type_output = "extern %s%s%s%s" % (mod1, ptr, space, mod2)
    elif m2:
        mod1 = m2.group(1) or ''
        ptr = m2.group(3) or ''
        space = m2.group(4) or ''
        mod2 = m2.group(5) or ''
        type_output = '%s%s%s%s' % (mod1, ptr, space, mod2)
    else:
        type_output = "extern"

    synop = "<row><entry role=\"variable_type\">%s</entry><entry role=\"function_name\"><link linkend=\"%s\">%s</link></entry></row>\n" % (
        type_output, sid, symbol)

    desc = "<refsect2 id=\"%s\" role=\"variable\"%s>\n<title>%s</title>\n" % (sid, condition, symbol)

    desc += MakeIndexterms(symbol, sid)
    desc += "\n"
    desc += OutputSymbolExtraLinks(symbol)

    decl_out = CreateValidSGML(declaration)
    desc += "<programlisting language=\"C\">%s</programlisting>\n" % decl_out

    desc += MakeDeprecationNote(symbol)

    if symbol in SymbolDocs:
        desc += ConvertMarkDown(symbol, SymbolDocs[symbol])

    if symbol in SymbolAnnotations:
        param_desc = SymbolAnnotations[symbol]
        param_desc, param_annotations = ExpandAnnotation(symbol, param_desc)
        if param_annotations != '':
            desc += "\n<para>%s</para>" % param_annotations

    desc += OutputSymbolTraits(symbol)
    desc += "</refsect2>\n"
    return (synop, desc)


def OutputFunction(symbol, declaration, symbol_type):
    """Returns the synopsis and detailed description of a function.

    Args:
        symbol (str): the function.
        declaration (str): the declaration of the function.

    Returns:
        str: the formatted docs
    """
    sid = common.CreateValidSGMLID(symbol)
    condition = MakeConditionDescription(symbol)

    # Take out the return type
    #                      $1                                                                                       $2   $3
    regex = r'<RETURNS>\s*((?:const\s+|G_CONST_RETURN\s+|signed\s+|unsigned\s+|long\s+|short\s+|struct\s+|enum\s+)*)(\w+)(\s*\**\s*(?:const|G_CONST_RETURN)?\s*\**\s*(?:restrict)?\s*)<\/RETURNS>\n'
    m = re.search(regex, declaration)
    declaration = re.sub(regex, '', declaration)
    type_modifier = m.group(1) or ''
    type = m.group(2)
    pointer = m.group(3)
    pointer = pointer.rstrip()
    xref = MakeXRef(type, tagify(type, "returnvalue"))
    start = ''
    # if (symbol_type == 'USER_FUNCTION')
    #    start = "typedef "
    #

    # We output const rather than G_CONST_RETURN.
    type_modifier = re.sub(r'G_CONST_RETURN', 'const', type_modifier)
    pointer = re.sub(r'G_CONST_RETURN', 'const', pointer)
    pointer = re.sub(r'^\s+', '&#160;', pointer)

    ret_type_output = "%s%s%s%s\n" % (start, type_modifier, xref, pointer)

    indent_len = len(symbol) + 2
    char1 = char2 = char3 = ''
    if symbol_type == 'USER_FUNCTION':
        indent_len += 3
        char1 = "<phrase role=\"c_punctuation\">(</phrase>"
        char2 = "*"
        char3 = "<phrase role=\"c_punctuation\">)</phrase>"

    symbol_output = "%s<link linkend=\"%s\">%s%s</link>%s" % (char1, sid, char2, symbol, char3)
    if indent_len < MAX_SYMBOL_FIELD_WIDTH:
        symbol_desc_output = "%s%s%s%s " % (char1, char2, symbol, char3)
    else:
        indent_len = MAX_SYMBOL_FIELD_WIDTH - 8
        symbol_desc_output = ('%s%s%s%s\n' % (char1, char2, symbol, char3)) + (' ' * (indent_len - 1))

    synop = "<row><entry role=\"function_type\">%s</entry><entry role=\"function_name\">%s&#160;<phrase role=\"c_punctuation\">()</phrase></entry></row>\n" % (
        ret_type_output, symbol_output)

    desc = "<refsect2 id=\"%s\" role=\"function\"%s>\n<title>%s&#160;()</title>\n" % (sid, condition, symbol)

    desc += MakeIndexterms(symbol, sid)
    desc += "\n"
    desc += OutputSymbolExtraLinks(symbol)

    desc += "<programlisting language=\"C\">%s%s(" % (ret_type_output, symbol_desc_output)

    def tagfun(*args):
        return tagify(args[0], "parameter")

    fields = common.ParseFunctionDeclaration(declaration, MakeXRef, tagfun)

    first = True
    for field_name in fields.values():
        if first:
            desc += field_name
            first = False
        else:
            desc += ",\n" + (' ' * indent_len) + field_name

    desc += ");</programlisting>\n"

    desc += MakeDeprecationNote(symbol)

    if symbol in SymbolDocs:
        desc += ConvertMarkDown(symbol, SymbolDocs[symbol])

    if symbol in SymbolAnnotations:
        param_desc = SymbolAnnotations[symbol]
        param_desc, param_annotations = ExpandAnnotation(symbol, param_desc)
        if param_annotations != '':
            desc += "\n<para>%s</para>" % param_annotations

    desc += OutputParamDescriptions("FUNCTION", symbol, fields.keys())
    desc += OutputSymbolTraits(symbol)
    desc += "</refsect2>\n"
    return (synop, desc)


def OutputParamDescriptions(symbol_type, symbol, fields):
    """Returns the DocBook output describing the parameters of a symbol.

    This can be used for functions, macros or signal handlers.

    Args:
        symbol_type (str): 'FUNCTION', 'MACRO' or 'SIGNAL'. Signal
                           handlers have an implicit user_data parameter last.
        symbol (str): the name of the symbol being described.
        fields (list): parsed fields from the declaration, used to determine
                       undocumented/unused entries

    Returns:
        str: the formatted parameter docs
    """
    output = ''
    num_params = 0
    field_descrs = None

    if fields:
        field_descrs = [f for f in fields if f not in ['void', 'Returns']]
    else:
        field_descrs = []

    params = SymbolParams.get(symbol)
    logging.info("param_desc(%s, %s) = %s", symbol_type, symbol, str(params))
    # This might be an empty dict, but for SIGNALS we append the user_data docs.
    # TODO(ensonic): maybe create that docstring in GetSignals()
    if params is not None:
        returns = ''
        params_desc = ''
        missing_parameters = ''
        unused_parameters = ''

        for param_name, param_desc in params.items():
            param_desc, param_annotations = ExpandAnnotation(symbol, param_desc)
            param_desc = ConvertMarkDown(symbol, param_desc)
            # trim
            param_desc = re.sub(r'^(\s|\n)+', '', param_desc, flags=re.M | re.S)
            param_desc = re.sub(r'(\s|\n)+$', '', param_desc, flags=re.M | re.S)
            if param_name == "Returns":
                returns = param_desc
                if param_annotations != '':
                    returns += "\n<para>%s</para>" % param_annotations

                elif param_name == "void":
                    # FIXME: &common.LogWarning()?
                    logging.info("!!!! void in params for %s?\n", symbol)
            else:
                if fields:
                    if param_name not in field_descrs:
                        common.LogWarning(*GetSymbolSourceLocation(symbol),
                                          "Parameter description for %s::%s is not used from source code comment block." % (symbol, param_name))
                        if unused_parameters != '':
                            unused_parameters += ", " + param_name
                        else:
                            unused_parameters = param_name
                    else:
                        field_descrs.remove(param_name)

                if param_desc != '':
                    params_desc += "<row><entry role=\"parameter_name\"><para>%s</para></entry>\n<entry role=\"parameter_description\">%s</entry>\n<entry role=\"parameter_annotations\">%s</entry></row>\n" % (
                        param_name, param_desc, param_annotations)
                    num_params += 1

        for param_name in field_descrs:
            common.LogWarning(*GetSymbolSourceLocation(symbol),
                              "Parameter description for %s::%s is missing in source code comment block." % (symbol, param_name))
            if missing_parameters != '':
                missing_parameters += ", " + param_name
            else:
                missing_parameters = param_name

        # Signals have an implicit user_data parameter which we describe.
        if symbol_type == "SIGNAL":
            params_desc += "<row><entry role=\"parameter_name\"><simpara>user_data</simpara></entry>\n<entry role=\"parameter_description\"><simpara>user data set when the signal handler was connected.</simpara></entry>\n<entry role=\"parameter_annotations\"></entry></row>\n"

        # Start a table if we need one.
        if params_desc != '':
            sid = common.CreateValidSGMLID("%s.parameters" % symbol)

            output += '''<refsect3 id="%s" role="parameters">\n<title>Parameters</title>
<informaltable role="parameters_table" pgwide="1" frame="none">
<tgroup cols="3">
<colspec colname="parameters_name" colwidth="150px"/>
<colspec colname="parameters_description"/>
<colspec colname="parameters_annotations" colwidth="200px"/>
<tbody>
''' % sid
            output += params_desc
            output += "</tbody></tgroup></informaltable>\n</refsect3>"

        # Output the returns info last
        if returns != '':
            sid = common.CreateValidSGMLID("%s.returns" % symbol)

            output += '''<refsect3 id="%s" role=\"returns\">\n<title>Returns</title>
''' % sid
            output += returns
            output += "\n</refsect3>"

        # remember missing/unused parameters (needed in tmpl-free build)
        if missing_parameters != '' and (symbol not in AllIncompleteSymbols):
            AllIncompleteSymbols[symbol] = missing_parameters

        if unused_parameters != '' and (symbol not in AllUnusedSymbols):
            AllUnusedSymbols[symbol] = unused_parameters

    if num_params == 0 and fields and field_descrs:
        if symbol not in AllIncompleteSymbols:
            AllIncompleteSymbols[symbol] = "<parameters>"
    return output


def ParseStabilityLevel(stability, file, line, message):
    """Parses a stability level and outputs a warning if it isn't valid.
    Args:
        stability (str): the stability text.
        file, line: context for error message
        message: description of where the level is from, to use in any error message.
    Returns:
        str: the parsed stability level string.
    """
    stability = stability.strip()
    sl = stability.strip().lower()
    if sl == 'stable':
        stability = "Stable"
    elif sl == 'unstable':
        stability = "Unstable"
    elif sl == 'private':
        stability = "Private"
    else:
        common.LogWarning(file, line,
                          "%s is %s. It should be one of these: Stable, "
                          "Unstable, or Private." % (
                              message, stability))
    return str(stability)


def OutputDBFile(file, title, section_id, includes, functions_synop, other_synop, functions_details, other_desc, signals_synop, signals_desc, args_synop, args_desc, hierarchy, interfaces, implementations, prerequisites, derived, file_objects, default_stability):
    """Outputs the final DocBook file for one section.

    Args:
        file (str): the name of the file.
        title (str): the title from the $MODULE-sections.txt file
        section_id (str): the id to use for the toplevel tag.
        includes (str): comma-separates list of include files added at top of
                   synopsis, with '<' '>' around them (if not already enclosed in '').
        functions_synop (str): the DocBook for the Functions Synopsis part.
        other_synop (str): the DocBook for the Types and Values Synopsis part.
        functions_details (str): the DocBook for the Functions Details part.
        other_desc (str): the DocBook for the Types and Values Details part.
        signal_synop (str): the DocBook for the Signal Synopsis part
        signal_desc (str): the DocBook for the Signal Description part
        args_synop (str): the DocBook for the Arg Synopsis part
        args_desc (str): the DocBook for the Arg Description part
        hierarchy (str): the DocBook for the Object Hierarchy part
        interfaces (str): the DocBook for the Interfaces part
        implementations (str): the DocBook for the Known Implementations part
        prerequisites (str): the DocBook for the Prerequisites part
        derived (str): the DocBook for the Derived Interfaces part
        file_objects (list): objects in this file
        default_stability (str): fallback if no api-stability is set (optional)

    Returns:
        bool: True if the docs where updated
    """

    logging.info("Output docbook for file %s with title '%s'", file, title)

    # The edited title overrides the one from the sections file.
    new_title = SymbolDocs.get(file + ":title")
    if new_title and not new_title.strip() == '':
        title = new_title
        logging.info("Found title: %s", title)

    short_desc = SymbolDocs.get(file + ":short_description")
    if not short_desc or short_desc.strip() == '':
        short_desc = ''
    else:
        # Don't use ConvertMarkDown here for now since we don't want blocks
        short_desc = ExpandAbbreviations(title + ":short_description", short_desc)
        logging.info("Found short_desc: %s", short_desc)

    long_desc = SymbolDocs.get(file + ":long_description")
    if not long_desc or long_desc.strip() == '':
        long_desc = ''
    else:
        long_desc = ConvertMarkDown(title + ":long_description", long_desc)
        logging.info("Found long_desc: %s", long_desc)

    see_also = SymbolDocs.get(file + ":see_also")
    if not see_also or re.search(r'^\s*(<para>)?\s*(</para>)?\s*$', see_also):
        see_also = ''
    else:
        see_also = ConvertMarkDown(title + ":see_also", see_also)
        logging.info("Found see_also: %s", see_also)

    if see_also:
        see_also = "<refsect1 id=\"%s.see-also\">\n<title>See Also</title>\n%s\n</refsect1>\n" % (section_id, see_also)

    stability = SymbolDocs.get(file + ":stability")
    if not stability or re.search(r'^\s*$', stability):
        stability = ''
    else:
        line_number = GetSymbolSourceLocation(file + ":stability")[1]
        stability = ParseStabilityLevel(stability, file, line_number, "Section stability level")
        logging.info("Found stability: %s", stability)

    if not stability:
        stability = default_stability or ''

    if stability:
        if stability in AnnotationDefinition:
            AnnotationsUsed[stability] = True
            stability = "<acronym>%s</acronym>" % stability
        stability = "<refsect1 id=\"%s.stability-level\">\n<title>Stability Level</title>\n%s, unless otherwise indicated\n</refsect1>\n" % (
            section_id, stability)

    image = SymbolDocs.get(file + ":image")
    if not image or re.search(r'^\s*$', image):
        image = ''
    else:
        image = image.strip()

        format = None

        il = image.lower()
        if re.search(r'jpe?g$', il):
            format = "format='JPEG'"
        elif il.endswith('png'):
            format = "format='PNG'"
        elif il.endswith('svg'):
            format = "format='SVG'"
        else:
            format = ''

        image = "  <inlinegraphic fileref='%s' %s/>\n" % (image, format)

    include_output = ''
    if includes:
        include_output += "<refsect1 id=\"%s.includes\"><title>Includes</title><synopsis>" % section_id
        for include in includes.split(','):
            if re.search(r'^\".+\"$', include):
                include_output += "#include %s\n" % include
            else:
                include = re.sub(r'^\s+|\s+$', '', include, flags=re.S)
                include_output += "#include &lt;%s&gt;\n" % include

        include_output += "</synopsis></refsect1>\n"

    extralinks = OutputSectionExtraLinks(title, "Section:%s" % file)

    old_db_file = os.path.join(DB_OUTPUT_DIR, file + '.xml')
    new_db_file = os.path.join(DB_OUTPUT_DIR, file + '.xml.new')

    OUTPUT = open(new_db_file, 'w', encoding='utf-8')

    object_anchors = ''
    for fobject in file_objects:
        if fobject == section_id:
            continue
        sid = common.CreateValidSGMLID(fobject)
        logging.info("Adding anchor for %s\n", fobject)
        object_anchors += "<anchor id=\"%s\"/>" % sid

    # Make sure we produce valid docbook
    if not functions_details:
        functions_details = "<para />"

    # We used to output this, but is messes up our common.UpdateFileIfChanged code
    # since it changes every day (and it is only used in the man pages):
    # "<refentry id="$section_id" revision="$mday $month $year">"

    OUTPUT.write(DB_REFENTRY.substitute({
        'args_desc': args_desc,
        'args_synop': args_synop,
        'derived': derived,
        'extralinks': extralinks,
        'functions_details': functions_details,
        'functions_synop': functions_synop,
        'header': MakeDocHeader('refentry'),
        'hierarchy': hierarchy,
        'image': image,
        'include_output': include_output,
        'interfaces': interfaces,
        'implementations': implementations,
        'long_desc': long_desc,
        'object_anchors': object_anchors,
        'other_desc': other_desc,
        'other_synop': other_synop,
        'prerequisites': prerequisites,
        'section_id': section_id,
        'see_also': see_also,
        'signals_desc': signals_desc,
        'signals_synop': signals_synop,
        'short_desc': short_desc,
        'stability': stability,
        'title': title,
        'MODULE': MODULE.upper(),
    }))
    OUTPUT.close()

    return common.UpdateFileIfChanged(old_db_file, new_db_file, 0)


def OutputProgramDBFile(program, section_id):
    """Outputs the final DocBook file for one program.

    Args:
        file (str): the name of the file.
        section_id (str): the id to use for the toplevel tag.

    Returns:
        bool: True if the docs where updated
    """
    logging.info("Output program docbook for %s", program)

    short_desc = SourceSymbolDocs.get(program + ":short_description")
    if not short_desc or short_desc.strip() == '':
        short_desc = ''
    else:
        # Don't use ConvertMarkDown here for now since we don't want blocks
        short_desc = ExpandAbbreviations(program, short_desc)
        logging.info("Found short_desc: %s", short_desc)

    synopsis = SourceSymbolDocs.get(program + ":synopsis")
    if synopsis and synopsis.strip() != '':
        items = synopsis.split(' ')
        for i in range(0, len(items)):
            parameter = items[i]
            choice = "plain"
            rep = ''

            # first parameter is the command name
            if i == 0:
                synopsis = "<command>%s</command>\n" % parameter
                continue

            # square brackets indicate optional parameters, curly brackets
            # indicate required parameters ("plain" parameters are also
            # mandatory, but do not get extra decoration)
            m1 = re.search(r'^\[(.+?)\]$', parameter)
            m2 = re.search(r'^\{(.+?)\}$', parameter)
            if m1:
                choice = "opt"
                parameter = m1.group(1)
            elif m2:
                choice = "req"
                parameter = m2.group(1)

            # parameters ending in "..." are repeatable
            if parameter.endswith('...'):
                rep = ' rep=\"repeat\"'
                parameter = parameter[:-3]

            # italic parameters are replaceable parameters
            parameter = re.sub(r'\*(.+?)\*', r'<replaceable>\1</replaceable>', parameter)

            synopsis += "<arg choice=\"%s\"%s>" % (choice, rep)
            synopsis += parameter
            synopsis += "</arg>\n"

        logging.info("Found synopsis: %s", synopsis)
    else:
        synopsis = "<command>%s</command>" % program

    long_desc = SourceSymbolDocs.get(program + ":long_description")
    if not long_desc or long_desc.strip() == '':
        long_desc = ''
    else:
        long_desc = ConvertMarkDown("%s:long_description" % program, long_desc)
        logging.info("Found long_desc: %s", long_desc)

    options = ''
    o = program + ":options"
    if o in SourceSymbolDocs:
        opts = SourceSymbolDocs[o].split('\t')

        logging.info('options: %d, %s', len(opts), str(opts))

        options = "<refsect1>\n<title>Options</title>\n<variablelist>\n"
        for k in range(0, len(opts), 2):
            opt_desc = opts[k + 1]

            opt_desc = re.sub(r'\*(.+?)\*', r'<replaceable>\1</replaceable>', opt_desc)

            options += "<varlistentry>\n<term>"
            opt_names = opts[k].split(',')
            for i in range(len(opt_names)):
                prefix = ', ' if i > 0 else ''
                # italic parameters are replaceable parameters
                opt_name = re.sub(r'\*(.+?)\*', r'<replaceable>\1</replaceable>', opt_names[i])

                options += "%s<option>%s</option>\n" % (prefix, opt_name)

            options += "</term>\n"
            options += "<listitem><para>%s</para></listitem>\n" % opt_desc
            options += "</varlistentry>\n"

        options += "</variablelist></refsect1>\n"

    exit_status = SourceSymbolDocs.get(program + ":returns")
    if exit_status and exit_status != '':
        exit_status = ConvertMarkDown("%s:returns" % program, exit_status)
        exit_status = "<refsect1 id=\"%s.exit-status\">\n<title>Exit Status</title>\n%s\n</refsect1>\n" % (
            section_id, exit_status)
    else:
        exit_status = ''

    see_also = SourceSymbolDocs.get(program + ":see_also")
    if not see_also or re.search(r'^\s*(<para>)?\s*(</para>)?\s*$', see_also):
        see_also = ''
    else:
        see_also = ConvertMarkDown("%s:see_also" % program, see_also)
        logging.info("Found see_also: %s", see_also)

    if see_also:
        see_also = "<refsect1 id=\"%s.see-also\">\n<title>See Also</title>\n%s\n</refsect1>\n" % (section_id, see_also)

    old_db_file = os.path.join(DB_OUTPUT_DIR, program + ".xml")
    new_db_file = os.path.join(DB_OUTPUT_DIR, program + ".xml.new")

    OUTPUT = open(new_db_file, 'w', encoding='utf-8')

    OUTPUT.write('''%s
<refentry id="%s">
<refmeta>
<refentrytitle role="top_of_page" id="%s.top_of_page">%s</refentrytitle>
<manvolnum>1</manvolnum>
<refmiscinfo>User Commands</refmiscinfo>
</refmeta>
<refnamediv>
<refname>%s</refname>
<refpurpose>%s</refpurpose>
</refnamediv>
<refsynopsisdiv>
<cmdsynopsis>%s</cmdsynopsis>
</refsynopsisdiv>
<refsect1 id="%s.description" role="desc">
<title role="desc.title">Description</title>
%s
</refsect1>
%s%s%s
</refentry>
''' % (MakeDocHeader("refentry"), section_id, section_id, program, program, short_desc, synopsis, section_id, long_desc, options, exit_status, see_also))
    OUTPUT.close()

    return common.UpdateFileIfChanged(old_db_file, new_db_file, 0)


def OutputExtraFile(file):
    """Copies an "extra" DocBook file into the output directory, expanding abbreviations.

    Args:
        file (str): the source file.

    Returns:
        bool: True if the docs where updated
    """

    basename = os.path.basename(file)

    old_db_file = os.path.join(DB_OUTPUT_DIR, basename)
    new_db_file = os.path.join(DB_OUTPUT_DIR, basename + ".new")

    contents = open(file, 'r', encoding='utf-8').read()

    with open(new_db_file, 'w', encoding='utf-8') as out:
        out.write(ExpandAbbreviations(basename + " file", contents))

    return common.UpdateFileIfChanged(old_db_file, new_db_file, 0)


def GetDocbookHeader(main_file):
    if os.path.exists(main_file):
        INPUT = open(main_file, 'r', encoding='utf-8')
        header = ''
        for line in INPUT:
            if re.search(r'^\s*<(book|chapter|article)', line):
                # check that the top-level tagSYSTEM or the doctype decl contain the xinclude namespace decl
                if not re.search(r'http:\/\/www.w3.org\/200[13]\/XInclude', line) and \
                        not re.search(r'http:\/\/www.w3.org\/200[13]\/XInclude', header, flags=re.MULTILINE):
                    header = ''
                break

            # if there are SYSTEM ENTITIES here, we should prepend "../" to the path
            # FIXME: not sure if we can do this now, as people already work-around the problem
            # r'#<!ENTITY % ([a-zA-Z-]+) SYSTEM \"([^/][a-zA-Z./]+)\">', r'<!ENTITY % \1 SYSTEM \"../\2\">';
            line = re.sub(
                r'<!ENTITY % gtkdocentities SYSTEM "([^"]*)">', r'<!ENTITY % gtkdocentities SYSTEM "../\1">', line)
            header += line
        INPUT.close()
        header = header.strip()
    else:
        header = '''<?xml version="1.0"?>
<!DOCTYPE refentry PUBLIC "-//OASIS//DTD DocBook XML V4.3//EN"
               "http://www.oasis-open.org/docbook/xml/4.3/docbookx.dtd"
[
  <!ENTITY % local.common.attrib "xmlns:xi  CDATA  #FIXED 'http://www.w3.org/2003/XInclude'">
  <!ENTITY % gtkdocentities SYSTEM "../xml/gtkdocentities.ent">
  %gtkdocentities;
]>'''
    return header


def OutputBook(main_file, book_top, book_bottom, obj_tree):
    """Outputs the entities that need to be included into the main docbook file for the module.

    Args:
        book_top (str): the declarations of the entities, which are added
                        at the top of the main docbook file.
        book_bottom (str): the entities, which are added in the main docbook
                           file at the desired position.
        obj_tree (list): object tree list
    """

    old_file = os.path.join(DB_OUTPUT_DIR, MODULE + "-doc.top")
    new_file = os.path.join(DB_OUTPUT_DIR, MODULE + "-doc.top.new")

    with open(new_file, 'w', encoding='utf-8') as out:
        out.write(book_top)

    common.UpdateFileIfChanged(old_file, new_file, 0)

    old_file = os.path.join(DB_OUTPUT_DIR, MODULE + "-doc.bottom")
    new_file = os.path.join(DB_OUTPUT_DIR, MODULE + "-doc.bottom.new")

    with open(new_file, 'w', encoding='utf-8') as out:
        out.write(book_bottom)

    common.UpdateFileIfChanged(old_file, new_file, 0)

    # If the main docbook file hasn't been created yet, we create it here.
    # The user can tweak it later.
    if main_file and not os.path.exists(main_file):
        OUTPUT = open(main_file, 'w', encoding='utf-8')

        logging.info("no master doc, create default one at: " + main_file)

        OUTPUT.write('''%s
<book id="index" xmlns:xi="http://www.w3.org/2003/XInclude">
  <bookinfo>
    <title>&package_name; Reference Manual</title>
    <releaseinfo>
      for &package_string;.
      The latest version of this documentation can be found on-line at
      <ulink role="online-location" url="http://[SERVER]/&package_name;/index.html">http://[SERVER]/&package_name;/</ulink>.
    </releaseinfo>
  </bookinfo>

  <chapter>
    <title>[Insert title here]</title>
%s  </chapter>
''' % (MakeDocHeader("book"), book_bottom))
        if obj_tree:
            OUTPUT.write('''  <chapter id="object-tree">
    <title>Object Hierarchy</title>
    <xi:include href="xml/tree_index.sgml"/>
  </chapter>
''')
        else:
            OUTPUT.write('''  <!-- enable this when you use gobject types
  <chapter id="object-tree">
    <title>Object Hierarchy</title>
    <xi:include href="xml/tree_index.sgml"/>
  </chapter>
  -->
''')

        OUTPUT.write('''  <index id="api-index-full">
    <title>API Index</title>
    <xi:include href="xml/api-index-full.xml"><xi:fallback /></xi:include>
  </index>
  <index id="deprecated-api-index" role="deprecated">
    <title>Index of deprecated API</title>
    <xi:include href="xml/api-index-deprecated.xml"><xi:fallback /></xi:include>
  </index>
''')
        for version in set(Since.values()):
            dash_version = version.replace('.', '-')
            OUTPUT.write('''  <index id="api-index-%s" role="%s">
    <title>Index of new API in %s</title>
    <xi:include href="xml/api-index-%s.xml"><xi:fallback /></xi:include>
  </index>
''' % (dash_version, version, version, version))

        if AnnotationsUsed:
            OUTPUT.write('''  <xi:include href="xml/annotation-glossary.xml"><xi:fallback /></xi:include>
''')
        else:
            OUTPUT.write('''  <!-- enable this when you use gobject introspection annotations
  <xi:include href="xml/annotation-glossary.xml"><xi:fallback /></xi:include>
  -->
''')

        OUTPUT.write('''</book>
''')

        OUTPUT.close()


def CreateValidSGML(text):
    """Turn any chars which are used in XML into entities.

    e.g. '<' into '&lt;'

    Args:
        text (str): the text to turn into proper XML.

    Returns:
        str: escaped input
    """

    text = text.replace('&', '&amp;')        # Do this first, or the others get messed up.
    text = text.replace('<', '&lt;')
    text = text.replace('>', '&gt;')
    # browsers render single tabs inconsistently
    text = re.sub(r'([^\s])\t([^\s])', r'\1&#160;\2', text)
    return text


def ConvertXMLChars(symbol, text):
    """Escape XML chars.

    This is used for text in source code comment blocks, to turn
    chars which are used in XML into entities, e.g. '<' into
    &lt;'. Depending on INLINE_MARKUP_MODE, this is done
    unconditionally or only if the character doesn't seem to be
    part of an XML construct (tag or entity reference).
    Args:
        text (str): the text to turn into proper XML.

    Returns:
        str: escaped input
    """

    if INLINE_MARKUP_MODE:
        # For the XML mode only convert to entities outside CDATA sections.
        return ModifyXMLElements(text, symbol,
                                 "<!\\[CDATA\\[|<programlisting[^>]*>",
                                 ConvertXMLCharsEndTag,
                                 ConvertXMLCharsCallback)
    # For the simple non-sgml mode, convert to entities everywhere.

    text = re.sub(r'&(?![a-zA-Z#]+;)', r'&amp;', text)        # Do this first, or the others get messed up.
    # Allow '<' in verbatim markdown
    # TODO: we don't want to convert any of them between ``
    text = re.sub(r'(?<!`)<', r'&lt;', text)
    # Allow '>' at beginning of string for blockquote markdown
    text = re.sub(r'''(?<=[^\w\n"'\/-])>''', r'&gt;', text)

    return text


def ConvertXMLCharsEndTag(start_tag):
    if start_tag == '<![CDATA[':
        return "]]>"
    return "</programlisting>"


def ConvertXMLCharsCallback(text, symbol, tag):
    if re.search(r'^<programlisting', tag):
        logging.debug('call modifyXML')
        # We can handle <programlisting> specially here.
        return ModifyXMLElements(text, symbol,
                                 "<!\\[CDATA\\[",
                                 ConvertXMLCharsEndTag,
                                 ConvertXMLCharsCallback2)
    elif tag == '':
        logging.debug('replace entities')
        # If we're not in CDATA convert to entities.
        text = re.sub(r'&(?![a-zA-Z#]+;)', r'&amp;', text)        # Do this first, or the others get messed up.
        text = re.sub(r'<(?![a-zA-Z\/!])', r'&lt;', text)
        # Allow '>' at beginning of string for blockquote markdown
        text = re.sub(r'''(?<=[^\w\n"'\/-])>''', r'&gt;', text)

        # Handle "#include <xxxxx>"
        text = re.sub(r'#include(\s+)<([^>]+)>', r'#include\1&lt;\2&gt;', text)

    return text


def ConvertXMLCharsCallback2(text, symbol, tag):
    # If we're not in CDATA convert to entities.
    # We could handle <programlisting> differently, though I'm not sure it helps.
    if tag == '':
        # replace only if its not a tag
        text = re.sub(r'&(?![a-zA-Z#]+;)', r'&amp;', text)        # Do this first, or the others get messed up.
        text = re.sub(r'<(?![a-zA-Z\/!])', r'&lt;', text)
        text = re.sub(r'''(?<![a-zA-Z0-9"'\/-])>''', r'&gt;', text)
        # Handle "#include <xxxxx>"
        text = re.sub(r'/#include(\s+)<([^>]+)>', r'#include\1&lt;\2&gt;', text)

    return text


def ExpandAnnotation(symbol, param_desc):
    """This turns annotations into acronym tags.
    Args:
        symbol (str): the symbol being documented, for error messages.
        param_desc (str): the text to expand.

    Returns:
        str: the remaining param_desc
        str: the formatted annotations
    """
    param_annotations = ''

    # look for annotations at the start of the comment part
    # function level annotations don't end with a colon ':'
    m = re.search(r'^\s*\((.*?)\)(:|$)', param_desc)
    if m:
        param_desc = param_desc[m.end():]

        annotations = re.split(r'\)\s*\(', m.group(1))
        logging.info("annotations for %s: '%s'\n", symbol, m.group(1))
        for annotation in annotations:
            # need to search for the longest key-match in %AnnotationDefinition
            match_length = 0
            match_annotation = ''

            for annotationdef in AnnotationDefinition:
                if annotation.startswith(annotationdef):
                    if len(annotationdef) > match_length:
                        match_length = len(annotationdef)
                        match_annotation = annotationdef

            annotation_extra = ''
            if match_annotation != '':
                m = re.search(match_annotation + r'\s+(.*)', annotation)
                if m:
                    annotation_extra = " " + m.group(1)

                AnnotationsUsed[match_annotation] = 1
                param_annotations += "[<acronym>%s</acronym>%s]" % (match_annotation, annotation_extra)
            else:
                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "unknown annotation \"%s\" in documentation for %s." % (annotation, symbol))
                param_annotations += "[%s]" % annotation

        param_desc = param_desc.strip()
        m = re.search(r'^(.*?)\.*\s*$', param_desc, flags=re.S)
        param_desc = m.group(1) + '. '

    if param_annotations != '':
        param_annotations = "<emphasis role=\"annotation\">%s</emphasis>" % param_annotations

    return (param_desc, param_annotations)


def ExpandAbbreviations(symbol, text):
    """Expand the shortcut notation for symbol references.

    This turns the abbreviations function(), macro(), @param, %constant, and #symbol
    into appropriate DocBook markup. CDATA sections and <programlisting> parts
    are skipped.

    Args:
        symbol (str): the symbol being documented, for error messages.
        text (str): the text to expand.

    Returns:
        str: the expanded text
    """
    # Note: This is a fallback and normally done in the markdown parser

    logging.debug('expand abbreviations for "%s", text: [%s]', symbol, text)
    m = re.search(r'\|\[[^\n]*\n(.*)\]\|', text, flags=re.M | re.S)
    if m:
        logging.debug('replaced entities in code block')
        text = text[:m.start(1)] + md_to_db.ReplaceEntities(m.group(1)) + text[m.end(1):]

    # Convert "|[" and "]|" into the start and end of program listing examples.
    # Support \[<!-- language="C" --> modifiers
    text = re.sub(r'\|\[<!-- language="([^"]+)" -->',
                  r'<informalexample><programlisting role="example" language="\1"><![CDATA[', text)
    text = re.sub(r'\|\[', r'<informalexample><programlisting role="example"><![CDATA[', text)
    text = re.sub(r'\]\|', r']]></programlisting></informalexample>', text)

    # keep CDATA unmodified, preserve ulink tags (ideally we preseve all tags
    # as such)
    return ModifyXMLElements(text, symbol,
                             "<!\\[CDATA\\[|<ulink[^>]*>|<programlisting[^>]*>|<!DOCTYPE",
                             ExpandAbbreviationsEndTag,
                             ExpandAbbreviationsCallback)


def ExpandAbbreviationsEndTag(start_tag):
    # Returns the end tag (as a regexp) corresponding to the given start tag.
    if start_tag == r'<!\[CDATA\[':
        return "]]>"
    if start_tag == "<!DOCTYPE":
        return '>'
    m = re.search(r'<(\w+)', start_tag)
    if m:
        return "</%s>" % m.group(1)

    logging.warning('no end tag for "%s"', start_tag)
    return ''


def ExpandAbbreviationsCallback(text, symbol, tag):
    # Called inside or outside each CDATA or <programlisting> section.
    if tag.startswith(r'^<programlisting'):
        # Handle any embedded CDATA sections.
        return ModifyXMLElements(text, symbol,
                                 "<!\\[CDATA\\[",
                                 ExpandAbbreviationsEndTag,
                                 ExpandAbbreviationsCallback2)
    elif tag == '':
        # NOTE: this is a fallback. It is normally done by the Markdown parser.
        # but is also used for OutputExtraFile

        # We are outside any CDATA or <programlisting> sections, so we expand
        # any gtk-doc abbreviations.

        # Convert '@param()'
        # FIXME: we could make those also links ($symbol.$2), but that would be less
        # useful as the link target is a few lines up or down
        text = re.sub(r'(\A|[^\\])\@(\w+((\.|->)\w+)*)\s*\(\)', r'\1<parameter>\2()</parameter>', text)

        # Convert 'function()' or 'macro()'.
        # if there is abc_*_def() we don't want to make a link to _def()
        # FIXME: also handle abc(def(....)) : but that would need to be done recursively :/
        def f1(m):
            return m.group(1) + MakeXRef(m.group(2), tagify(m.group(2) + "()", "function"))
        text = re.sub(r'([^\*.\w])(\w+)\s*\(\)', f1, text)
        # handle #Object.func()
        text = re.sub(r'(\A|[^\\])#([\w\-:\.]+[\w]+)\s*\(\)', f1, text)

        # Convert '@param', but not '\@param'.
        text = re.sub(r'(\A|[^\\])\@(\w+((\.|->)\w+)*)', r'\1<parameter>\2</parameter>', text)
        text = re.sub(r'/\\\@', r'\@', text)

        # Convert '%constant', but not '\%constant'.
        # Also allow negative numbers, e.g. %-1.
        def f2(m):
            return m.group(1) + MakeXRef(m.group(2), tagify(m.group(2), "literal"))
        text = re.sub(r'(\A|[^\\])\%(-?\w+)', f2, text)
        text = re.sub(r'\\\%', r'\%', text)

        # Convert '#symbol', but not '\#symbol'.
        def f3(m):
            return m.group(1) + MakeHashXRef(m.group(2), "type")
        text = re.sub(r'(\A|[^\\])#([\w\-:\.]+[\w]+)', f3, text)
        text = re.sub(r'\\#', '#', text)

    return text


def ExpandAbbreviationsCallback2(text, symbol, tag):
    # This is called inside a <programlisting>
    if tag == '':
        # We are inside a <programlisting> but outside any CDATA sections,
        # so we expand any gtk-doc abbreviations.
        # FIXME: why is this different from &ExpandAbbreviationsCallback(),
        #        why not just call it
        text = re.sub(r'#(\w+)', lambda m: '%s;' % MakeHashXRef(m.group(1), ''), text)
    elif tag == "<![CDATA[":
        # NOTE: this is a fallback. It is normally done by the Markdown parser.
        text = md_to_db.ReplaceEntities(text, symbol)

    return text


def MakeHashXRef(symbol, tag):
    text = symbol

    # Check for things like '#include', '#define', and skip them.
    if symbol in PreProcessorDirectives:
        return "#%s" % symbol

    # Get rid of special suffixes ('-struct','-enum').
    text = re.sub(r'-struct$', '', text)
    text = re.sub(r'-enum$', '', text)

    # If the symbol is in the form "Object::signal", then change the symbol to
    # "Object-signal" and use "signal" as the text.
    if '::' in symbol:
        o, s = symbol.split('::', 1)
        symbol = '%s-%s' % (o, s)
        text = u'“' + s + u'”'

    # If the symbol is in the form "Object:property", then change the symbol to
    # "Object--property" and use "property" as the text.
    if ':' in symbol:
        o, p = symbol.split(':', 1)
        symbol = '%s--%s' % (o, p)
        text = u'“' + p + u'”'

    if tag != '':
        text = tagify(text, tag)

    return MakeXRef(symbol, text)


def ModifyXMLElements(text, symbol, start_tag_regexp, end_tag_func, callback):
    """Rewrite XML blocks.

    Looks for given XML element tags within the text, and calls
    the callback on pieces of text inside & outside those elements.
    Used for special handling of text inside things like CDATA
    and <programlisting>.

    Args:
        text (str): the text.
        symbol (str): the symbol currently being documented (only used for
                      error messages).
        start_tag_regexp (str): the regular expression to match start tags.
                                e.g. "<!\\[CDATA\\[|<programlisting[^>]*>" to
                                match CDATA sections or programlisting elements.
        end_tag_func (func): function which is passed the matched start tag
                             and should return the appropriate end tag string
                             regexp.
        callback - callback called with each part of the text. It is
                   called with a piece of text, the symbol being
                   documented, and the matched start tag or '' if the text
                   is outside the XML elements being matched.

    Returns:
        str: modified text
    """
    before_tag = start_tag = end_tag_regexp = end_tag = None
    result = ''

    logging.debug('modify xml for symbol: %s, regex: %s, text: [%s]', symbol, start_tag_regexp, text)

    m = re.search(start_tag_regexp, text, flags=re.S)
    while m:
        before_tag = text[:m.start()]  # Prematch for last successful match string
        start_tag = m.group(0)         # Last successful match
        text = text[m.end():]          # Postmatch for last successful match string
        # get the matching end-tag for current tag
        end_tag_regexp = end_tag_func(start_tag)

        logging.debug('symbol: %s matched start: %s, end_tag: %s, text: [%s]', symbol, start_tag, end_tag_regexp, text)

        logging.debug('converting before tag: [%s]', before_tag)
        result += callback(before_tag, symbol, '')
        result += start_tag

        m2 = re.search(end_tag_regexp, text, flags=re.S)
        if m2:
            before_tag = text[:m2.start()]
            end_tag = m2.group(0)
            text = text[m2.end():]

            logging.debug('symbol: %s matched end %s: text: [%s]', symbol, end_tag, text)

            result += callback(before_tag, symbol, start_tag)
            result += end_tag
        else:
            common.LogWarning(*GetSymbolSourceLocation(symbol),
                              "Can't find tag end: %s in docs for: %s." % (end_tag_regexp, symbol))
            # Just assume it is all inside the tag.
            result += callback(text, symbol, start_tag)
            text = ''
        m = re.search(start_tag_regexp, text, flags=re.S)

    # Handle any remaining text outside the tags.
    logging.debug('converting after tag: [%s]', text)
    result += callback(text, symbol, '')
    logging.debug('results for symbol: %s, text: [%s]', symbol, result)

    return result


def tagify(text, elem):
    # Adds a tag around some text.
    # e.g tagify("Text", "literal") => "<literal>Text</literal>".
    return '<' + elem + '>' + text + '</' + elem + '>'


def MakeDocHeader(tag):
    """Builds a docbook header for the given tag.

    Args:
        tag (str): doctype tag

    Returns:
        str: the docbook header
    """
    header = re.sub(r'<!DOCTYPE \w+', r'<!DOCTYPE ' + tag, doctype_header)
    # fix the path for book since this is one level up
    if tag == 'book':
        header = re.sub(
            r'<!ENTITY % gtkdocentities SYSTEM "../([a-zA-Z./]+)">', r'<!ENTITY % gtkdocentities SYSTEM "\1">', header)
    return header


def MakeXRef(symbol, text=None):
    """This returns a cross-reference link to the given symbol.

    Though it doesn't try to do this for a few standard C types that it knows
    won't be in the documentation.

    Args:
        symbol (str): the symbol to try to create a XRef to.
        text (str): text to put inside the XRef, defaults to symbol

    Returns:
        str: a docbook link
    """
    symbol = symbol.strip()
    if not text:
        text = symbol

        # Get rid of special suffixes ('-struct','-enum').
        text = re.sub(r'-struct$', '', text)
        text = re.sub(r'-enum$', '', text)

    if ' ' in symbol:
        return text

    logging.info("Getting type link for %s -> %s", symbol, text)

    symbol_id = common.CreateValidSGMLID(symbol)
    return "<link linkend=\"%s\">%s</link>" % (symbol_id, text)


def MakeIndexterms(symbol, sid):
    """This returns a indexterm elements for the given symbol

    Args:
        symbol (str): the symbol to create indexterms for

    Returns:
        str: doxbook index terms
    """
    terms = ''
    sortas = ''

    # make the index useful, by omitting the namespace when sorting
    if NAME_SPACE != '':
        m = re.search(r'^%s\_?(.*)' % NAME_SPACE, symbol, flags=re.I)
        if m:
            sortas = ' sortas="%s"' % m.group(1)

    if symbol in Deprecated:
        terms += "<indexterm zone=\"%s\" role=\"deprecated\"><primary%s>%s</primary></indexterm>" % (
            sid, sortas, symbol)
        IndexEntriesDeprecated[symbol] = sid
        IndexEntriesFull[symbol] = sid
    if symbol in Since:
        since = Since[symbol].strip()
        if since != '':
            terms += "<indexterm zone=\"%s\" role=\"%s\"><primary%s>%s</primary></indexterm>" % (
                sid, since, sortas, symbol)
        IndexEntriesSince[symbol] = sid
        IndexEntriesFull[symbol] = sid
    if terms == '':
        terms += "<indexterm zone=\"%s\"><primary%s>%s</primary></indexterm>" % (sid, sortas, symbol)
        IndexEntriesFull[symbol] = sid
    return terms


def MakeDeprecationNote(symbol):
    """This returns a deprecation warning for the given symbol.

    Args:
        symbol (str): the symbol to try to create a warning for.

    Returns:
        str: formatted warning or empty string if symbol is not deprecated
    """
    desc = ''
    if symbol in Deprecated:
        desc += "<warning><para><literal>%s</literal> " % symbol
        note = Deprecated[symbol]

        m = re.search(r'^\s*([0-9\.]+)\s*:?', note)
        if m:
            desc += "has been deprecated since version %s and should not be used in newly-written code.</para>" % m.group(
                1)
        else:
            desc += "is deprecated and should not be used in newly-written code.</para>"

        note = re.sub(r'^\s*([0-9\.]+)\s*:?\s*', '', note)
        note = note.strip()

        if note != '':
            note = ConvertMarkDown(symbol, note)
            desc += " " + note

        desc += "</warning>\n"

    return desc


def MakeConditionDescription(symbol):
    """This returns a summary of conditions for the given symbol.

    Args:
        symbol (str): the symbol to create the summary for.

    Returns:
        str: formatted text or empty string if no special conditions apply.
    """
    desc = ''
    if symbol in Deprecated:
        if desc != '':
            desc += "|"
        m = re.search(r'^\s*(.*?)\s*$', Deprecated[symbol])
        if m:
            desc += "deprecated:%s" % m.group(1)
        else:
            desc += "deprecated"

    if symbol in Since:
        if desc != '':
            desc += "|"
        m = re.search(r'^\s*(.*?)\s*$', Since[symbol])
        if m:
            desc += "since:%s" % m.group(1)
        else:
            desc += "since"

    if symbol in StabilityLevel:
        if desc != '':
            desc += "|"

        desc += "stability:" + StabilityLevel[symbol]

    if desc != '':
        cond = re.sub(r'"', r'&quot;', desc)
        desc = ' condition=\"%s\"' % cond
        logging.info("condition for '%s' = '%s'", symbol, desc)

    return desc


def GetHierarchy(gobject, hierarchy):
    """Generate the object inheritance graph.

    Returns the DocBook output describing the ancestors and
    immediate children of a GObject subclass. It uses the
    global Objects and ObjectLevels arrays to walk the tree.

    Args:
        object (str): the GtkObject subclass.
        hierarchy (list) - previous hierarchy

    Returns:
        list: lines of docbook describing the hierarchy
    """
    # Find object in the objects array.
    found = False
    children = []
    level = 0
    j = 0
    for i in range(len(Objects)):
        if found:
            if ObjectLevels[i] <= level:
                break

            elif ObjectLevels[i] == level + 1:
                children.append(Objects[i])

        elif Objects[i] == gobject:
            found = True
            j = i
            level = ObjectLevels[i]

    if not found:
        return hierarchy

    logging.info("=== Hierarchy for: %s (%d existing entries) ===", gobject, len(hierarchy))

    # Walk up the hierarchy, pushing ancestors onto the ancestors array.
    ancestors = [gobject]
    logging.info("Level: %s", level)
    while level > 1:
        j -= 1
        if ObjectLevels[j] < level:
            ancestors.append(Objects[j])
            level = ObjectLevels[j]
            logging.info("Level: %s", level)

    # Output the ancestors, indented and with links.
    logging.info('%d ancestors', len(ancestors))
    last_index = 0
    level = 1
    for i in range(len(ancestors) - 1, -1, -1):
        ancestor = ancestors[i]
        ancestor_id = common.CreateValidSGMLID(ancestor)
        indent = ' ' * (level * 4)
        # Don't add a link to the current object, i.e. when i == 0.
        if i > 0:
            entry_text = indent + "<link linkend=\"%s\">%s</link>" % (ancestor_id, ancestor)
            alt_text = indent + ancestor
        else:
            entry_text = indent + ancestor
            alt_text = indent + "<link linkend=\"%s\">%s</link>" % (ancestor_id, ancestor)

        logging.info("Checking for '%s' or '%s'", entry_text, alt_text)
        # Check if we already have this object
        index = -1
        for j in range(len(hierarchy)):
            if hierarchy[j] == entry_text or (hierarchy[j] == alt_text):
                index = j
                break
        if index == -1:
            # We have a new entry, find insert position in alphabetical order
            found = False
            for j in range(last_index, len(hierarchy)):
                if not re.search(r'^' + indent, hierarchy[j]):
                    last_index = j
                    found = True
                    break
                elif re.search(r'^%s[^ ]' % indent, hierarchy[j]):
                    stripped_text = hierarchy[j]
                    if r'<link linkend' not in entry_text:
                        stripped_text = re.sub(r'<link linkend="[A-Za-z]*">', '', stripped_text)
                        stripped_text = re.sub(r'</link>', '', stripped_text)

                    if entry_text < stripped_text:
                        last_index = j
                        found = True
                        break

            # Append to bottom
            if not found:
                last_index = len(hierarchy)

            logging.debug('insert at %d: %s', last_index, entry_text)
            hierarchy.insert(last_index, entry_text)
            last_index += 1
        else:
            # Already have this one, make sure we use the not linked version
            if r'<link linkend' not in entry_text:
                hierarchy[j] = entry_text

            # Remember index as base insert point
            last_index = index + 1

        level += 1

    # Output the children, indented and with links.
    logging.info('%d children', len(children))
    for i in range(len(children)):
        sid = common.CreateValidSGMLID(children[i])
        indented_text = ' ' * (level * 4) + "<link linkend=\"%s\">%s</link>" % (sid, children[i])
        logging.debug('insert at %d: %s', last_index, indented_text)
        hierarchy.insert(last_index, indented_text)
        last_index += 1
    return hierarchy


def GetInterfaces(gobject):
    """Generate interface implementation graph.

    Returns the DocBook output describing the interfaces
    implemented by a class. It uses the global Interfaces hash.

    Args:
        object (str): the GObject subclass.

    Returns:
        str: implemented interfaces
    """
    text = ''
    # Find object in the objects array.
    if gobject in Interfaces:
        ifaces = Interfaces[gobject].split()
        text = '''<para>
%s implements
''' % gobject
        count = len(ifaces)
        for i in range(count):
            sid = common.CreateValidSGMLID(ifaces[i])
            text += " <link linkend=\"%s\">%s</link>" % (sid, ifaces[i])
            if i < count - 2:
                text += ', '
            elif i < count - 1:
                text += ' and '
            else:
                text += '.'
        text += '</para>\n'
    return text


def GetImplementations(gobject):
    """Generate interface usage graph.

    Returns the DocBook output describing the implementations
    of an interface. It uses the global Interfaces hash.

    Args:
        object (str): the GObject subclass.

    Returns:
        str: interface implementations
    """
    text = ''
    impls = []
    for key in Interfaces:
        if re.search(r'\b%s\b' % gobject, Interfaces[key]):
            impls.append(key)

    count = len(impls)
    if count > 0:
        impls.sort()
        text = '''<para>
%s is implemented by
''' % gobject
        for i in range(count):
            sid = common.CreateValidSGMLID(impls[i])
            text += " <link linkend=\"%s\">%s</link>" % (sid, impls[i])
            if i < count - 2:
                text += ', '
            elif i < count - 1:
                text += ' and '
            else:
                text += '.'
        text += '</para>\n'
    return text


def GetPrerequisites(iface):
    """Generates interface requirements.

    Returns the DocBook output describing the prerequisites
    of an interface. It uses the global Prerequisites hash.
    Args:
        iface (str): the interface.

    Returns:
        str: required interfaces
    """

    text = ''
    if iface in Prerequisites:
        text = '''<para>
%s requires
''' % iface
        prereqs = Prerequisites[iface].split()
        count = len(prereqs)
        for i in range(count):
            sid = common.CreateValidSGMLID(prereqs[i])
            text += " <link linkend=\"%s\">%s</link>" % (sid, prereqs[i])
            if i < count - 2:
                text += ', '
            elif i < count - 1:
                text += ' and '
            else:
                text += '.'
        text += '</para>\n'
    return text


def GetDerived(iface):
    """
    Returns the DocBook output describing the derived interfaces
    of an interface. It uses the global %Prerequisites hash.

    Args:
        iface (str): the interface.

    Returns:
        str: derived interfaces
    """
    text = ''
    derived = []
    for key in Prerequisites:
        if re.search(r'\b%s\b' % iface, Prerequisites[key]):
            derived.append(key)

    count = len(derived)
    if count > 0:
        derived.sort()
        text = '''<para>
%s is required by
''' % iface
        for i in range(count):
            sid = common.CreateValidSGMLID(derived[i])
            text += " <link linkend=\"%s\">%s</link>" % (sid, derived[i])
            if i < count - 2:
                text += ', '
            elif i < count - 1:
                text += ' and '
            else:
                text += '.'
        text += '</para>\n'
    return text


def GetSignals(gobject):
    """Generate signal docs.

    Returns the synopsis and detailed description DocBook output
    for the signal handlers of a given GObject subclass.

    Args:
        object (str): the GObject subclass, e.g. 'GtkButton'.

    Returns:
        str: signal docs
    """
    synop = ''
    desc = ''

    for i in range(len(SignalObjects)):
        if SignalObjects[i] == gobject:
            logging.info("Found signal: %s", SignalNames[i])
            name = SignalNames[i]
            symbol = '%s::%s' % (gobject, name)
            sid = common.CreateValidSGMLID('%s-%s' % (gobject, name))

            desc += u"<refsect2 id=\"%s\" role=\"signal\"><title>The <literal>“%s”</literal> signal</title>\n" % (
                sid, name)
            desc += MakeIndexterms(symbol, sid)
            desc += "\n"
            desc += OutputSymbolExtraLinks(symbol)

            desc += "<programlisting language=\"C\">"

            m = re.search(r'\s*(const\s+)?(\w+)\s*(\**)', SignalReturns[i])
            type_modifier = m.group(1) or ''
            gtype = m.group(2)
            pointer = m.group(3)
            xref = MakeXRef(gtype, tagify(gtype, "returnvalue"))

            ret_type_output = '%s%s%s' % (type_modifier, xref, pointer)
            callback_name = "user_function"
            desc += '%s\n%s (' % (ret_type_output, callback_name)

            indentation = ' ' * (len(callback_name) + 2)

            sourceparams = SourceSymbolParams.get(symbol)
            sourceparam_names = None
            if sourceparams:
                sourceparam_names = list(sourceparams)  # keys as list
            params = SignalPrototypes[i].splitlines()
            type_len = len("gpointer")
            name_len = len("user_data")
            # do two passes, the first one is to calculate padding
            for l in range(2):
                for j in range(len(params)):
                    param_name = None
                    # allow alphanumerics, '_', '[' & ']' in param names
                    m = re.search(r'^\s*(\w+)\s*(\**)\s*([\w\[\]]+)\s*$', params[j])
                    if m:
                        gtype = m.group(1)
                        pointer = m.group(2)
                        if sourceparam_names:
                            if j < len(sourceparam_names):
                                param_name = sourceparam_names[j]
                                logging.info('from sourceparams: "%s" (%d: %s)', param_name, j, params[j])
                            # we're missing the docs for this param, don't warn here though
                        else:
                            param_name = m.group(3)
                            logging.info('from params: "%s" (%d: %s)', param_name, j, params[j])

                        if not param_name:
                            param_name = "arg%d" % j

                        if l == 0:
                            if len(gtype) + len(pointer) > type_len:
                                type_len = len(gtype) + len(pointer)
                            if len(param_name) > name_len:
                                name_len = len(param_name)
                        else:
                            logging.info("signal arg[%d]: '%s'", j, param_name)
                            xref = MakeXRef(gtype, tagify(gtype, "type"))
                            pad = ' ' * (type_len - len(gtype) - len(pointer))
                            desc += '%s%s %s%s,\n' % (xref, pad, pointer, param_name)
                            desc += indentation

                    else:
                        common.LogWarning(*GetSymbolSourceLocation(symbol),
                                          "Can't parse arg: %s\nArgs:%s" % (params[j], SignalPrototypes[i]))

            xref = MakeXRef("gpointer", tagify("gpointer", "type"))
            pad = ' ' * (type_len - len("gpointer"))
            desc += '%s%s user_data)' % (xref, pad)
            desc += "</programlisting>\n"

            flags = SignalFlags[i]
            flags_string = ''
            if flags:
                if 'f' in flags:
                    flags_string = "<link linkend=\"G-SIGNAL-RUN-FIRST:CAPS\">Run First</link>"

                elif 'l' in flags:
                    flags_string = "<link linkend=\"G-SIGNAL-RUN-LAST:CAPS\">Run Last</link>"

                elif 'c' in flags:
                    flags_string = "<link linkend=\"G-SIGNAL-RUN-CLEANUP:CAPS\">Cleanup</link>"
                    flags_string = "Cleanup"

                if 'r' in flags:
                    if flags_string:
                        flags_string += " / "
                    flags_string = "<link linkend=\"G-SIGNAL-NO-RECURSE:CAPS\">No Recursion</link>"

                if 'd' in flags:
                    if flags_string:
                        flags_string += " / "
                    flags_string = "<link linkend=\"G-SIGNAL-DETAILED:CAPS\">Has Details</link>"

                if 'a' in flags:
                    if flags_string:
                        flags_string += " / "
                    flags_string = "<link linkend=\"G-SIGNAL-ACTION:CAPS\">Action</link>"

                if 'h' in flags:
                    if flags_string:
                        flags_string += " / "
                    flags_string = "<link linkend=\"G-SIGNAL-NO-HOOKS:CAPS\">No Hooks</link>"

            synop += "<row><entry role=\"signal_type\">%s</entry><entry role=\"signal_name\"><link linkend=\"%s\">%s</link></entry><entry role=\"signal_flags\">%s</entry></row>\n" % (
                ret_type_output, sid, name, flags_string)

            parameters = OutputParamDescriptions("SIGNAL", symbol, None)
            logging.info("formatted signal params: '%s' -> '%s'", symbol, parameters)

            AllSymbols[symbol] = 1
            if symbol in SymbolDocs:
                symbol_docs = ConvertMarkDown(symbol, SymbolDocs[symbol])

                desc += symbol_docs

                if not IsEmptyDoc(SymbolDocs[symbol]):
                    AllDocumentedSymbols[symbol] = 1

            if symbol in SymbolAnnotations:
                param_desc = SymbolAnnotations[symbol]
                param_desc, param_annotations = ExpandAnnotation(symbol, param_desc)
                if param_annotations != '':
                    desc += "\n<para>%s</para>" % param_annotations

            desc += MakeDeprecationNote(symbol)

            desc += parameters
            if flags_string:
                desc += "<para>Flags: %s</para>\n" % flags_string

            desc += OutputSymbolTraits(symbol)
            desc += "</refsect2>"

    return (synop, desc)


def GetArgs(gobject):
    """Generate property docs.

    Returns the synopsis and detailed description DocBook output
    for the Args of a given GtkObject subclass.

    Args:
        object (str): the GObject subclass, e.g. 'GtkButton'.

    Returns:
        str: property docs
    """
    synop = ''
    desc = ''
    child_synop = ''
    child_desc = ''
    style_synop = ''
    style_desc = ''

    for i in range(len(ArgObjects)):
        if ArgObjects[i] == gobject:
            logging.info("Found arg: %s", ArgNames[i])
            name = ArgNames[i]
            flags = ArgFlags[i]
            flags_string = ''
            kind = ''
            id_sep = ''

            if 'c' in flags:
                kind = "child property"
                id_sep = "c-"
            elif 's' in flags:
                kind = "style property"
                id_sep = "s-"
            else:
                kind = "property"

            # Remember only one colon so we don't clash with signals.
            symbol = '%s:%s' % (gobject, name)
            # use two dashes and ev. an extra separator here for the same reason.
            sid = common.CreateValidSGMLID('%s--%s%s' % (gobject, id_sep, name))

            atype = ArgTypes[i]
            type_output = None
            arange = ArgRanges[i]
            range_output = CreateValidSGML(arange)
            default = ArgDefaults[i]
            default_output = CreateValidSGML(default)

            if atype == "GtkString":
                atype = "char&#160;*"

            if atype == "GtkSignal":
                atype = "GtkSignalFunc, gpointer"
                type_output = MakeXRef("GtkSignalFunc") + ", " + MakeXRef("gpointer")
            elif re.search(r'^(\w+)\*$', atype):
                m = re.search(r'^(\w+)\*$', atype)
                type_output = MakeXRef(m.group(1), tagify(m.group(1), "type")) + "&#160;*"
            else:
                type_output = MakeXRef(atype, tagify(atype, "type"))

            if 'r' in flags:
                flags_string = "Read"

            if 'w' in flags:
                if flags_string:
                    flags_string += " / "
                flags_string += "Write"

            if 'x' in flags:
                if flags_string:
                    flags_string += " / "
                flags_string += "Construct"

            if 'X' in flags:
                if flags_string:
                    flags_string += " / "
                flags_string += "Construct&#160;Only"

            AllSymbols[symbol] = 1
            blurb = ''
            if symbol in SymbolDocs and not IsEmptyDoc(SymbolDocs[symbol]):
                blurb = ConvertMarkDown(symbol, SymbolDocs[symbol])
                logging.info(".. [%s][%s]", SymbolDocs[symbol], blurb)
                AllDocumentedSymbols[symbol] = 1

            else:
                if ArgBlurbs[i] != '':
                    blurb = "<para>" + CreateValidSGML(ArgBlurbs[i]) + "</para>"
                    AllDocumentedSymbols[symbol] = 1
                else:
                    # FIXME: print a warning?
                    logging.info(".. no description")

            pad1 = ''
            if len(name) < 24:
                pad1 = " " * (24 - len(name))

            arg_synop = "<row><entry role=\"property_type\">%s</entry><entry role=\"property_name\"><link linkend=\"%s\">%s</link></entry><entry role=\"property_flags\">%s</entry></row>\n" % (
                type_output, sid, name, flags_string)
            arg_desc = u"<refsect2 id=\"%s\" role=\"property\"><title>The <literal>“%s”</literal> %s</title>\n" % (
                sid, name, kind)
            arg_desc += MakeIndexterms(symbol, sid)
            arg_desc += "\n"
            arg_desc += OutputSymbolExtraLinks(symbol)

            arg_desc += u"<programlisting>  “%s”%s %s</programlisting>\n" % (name, pad1, type_output)
            arg_desc += blurb
            if symbol in SymbolAnnotations:
                param_desc = SymbolAnnotations[symbol]
                param_desc, param_annotations = ExpandAnnotation(symbol, param_desc)
                if param_annotations != '':
                    arg_desc += "\n<para>%s</para>" % param_annotations

            arg_desc += MakeDeprecationNote(symbol)

            arg_desc += "<para>Owner: %s</para>\n" % gobject

            if flags_string:
                arg_desc += "<para>Flags: %s</para>\n" % flags_string

            if arange != '':
                arg_desc += "<para>Allowed values: %s</para>\n" % range_output

            if default != '':
                arg_desc += "<para>Default value: %s</para>\n" % default_output

            arg_desc += OutputSymbolTraits(symbol)
            arg_desc += "</refsect2>\n"

            if 'c' in flags:
                child_synop += arg_synop
                child_desc += arg_desc

            elif 's' in flags:
                style_synop += arg_synop
                style_desc += arg_desc

            else:
                synop += arg_synop
                desc += arg_desc

    return (synop, child_synop, style_synop, desc, child_desc, style_desc)


def IgnorePath(path, source_dirs, ignore_files):
    for sdir in source_dirs:
        # Cut off base directory
        m1 = re.search(r'^%s/(.*)$' % re.escape(sdir), path)
        if m1:
            # Check if the filename is in the ignore list.
            m2 = re.search(r'(\s|^)%s(\s|$)' % re.escape(m1.group(1)), ignore_files)
            if m2:
                logging.info("Skipping path: %s", path)
                return True
            else:
                logging.info("No match for: %s", m1.group(1))
        else:
            logging.info("No match for: %s", path)
    return False


def ReadSourceDocumentation(source_dir, suffix_list, source_dirs, ignore_files):
    """Read the documentation embedded in comment blocks in the source code.

    It recursively descends the source directory looking for source files and
    scans them looking for specially-formatted comment blocks.

    Args:
        source_dir (str): the directory to scan.
        suffix_list (list): extensions to check
    """
    if IgnorePath(source_dir, source_dirs, ignore_files):
        return

    logging.info("Scanning source directory: %s", source_dir)

    # This array holds any subdirectories found.
    subdirs = []

    for ifile in sorted(os.listdir(source_dir)):
        logging.debug("... : %s", ifile)
        if ifile.startswith('.'):
            continue
        fname = os.path.join(source_dir, ifile)
        if os.path.isdir(fname):
            subdirs.append(fname)
        else:
            for suffix in suffix_list:
                if ifile.endswith(suffix):
                    if not IgnorePath(fname, source_dirs, ignore_files):
                        ScanSourceFile(fname, ignore_files)
                        break

    # Now recursively scan the subdirectories.
    for sdir in subdirs:
        ReadSourceDocumentation(sdir, suffix_list, source_dirs, ignore_files)


def ScanSourceFile(ifile, ignore_files):
    """Scans one source file looking for specially-formatted comment blocks.

    Later MergeSourceDocumentation() is copying over the doc blobs that are not
    suppressed/ignored.

    Args:
        ifile (str): the file to scan.
    """
    m = re.search(r'^.*[\/\\]([^\/\\]*)$', ifile)
    if m:
        basename = m.group(1)
    else:
        common.LogWarning(ifile, 1, "Can't find basename for this filename.")
        basename = ifile

    # Check if the basename is in the list of files to ignore.
    if re.search(r'(\s|^)%s(\s|$)' % re.escape(basename), ignore_files):
        logging.info("Skipping source file: %s", ifile)
        return

    logging.info("Scanning %s", ifile)

    with open(ifile, 'r', encoding='utf-8') as src:
        input_lines = src.readlines()

    for c in ScanSourceContent(input_lines, ifile):
        ParseCommentBlock(c[0], c[1], ifile)

    logging.info("Scanning %s done", ifile)


def ScanSourceContent(input_lines, ifile=''):
    """Scans the source code lines for specially-formatted comment blocks.

    Updates global state in SourceSymbolDocs, SymbolSourceLocation,
    Since, StabilityLevel, Deprecated, SymbolAnnotations.

    Might read from global state in DeclarationTypes

    Args:
        input_lines (list): list of source code lines
        ifile (str): file name of the source file (for reporting)

    Returns:
        list: tuples with comment block and its starting line
    """
    comments = []
    in_comment_block = False
    line_number = 0
    comment = []
    starting_line = 0
    for line in input_lines:
        line_number += 1

        if not in_comment_block:
            # Look for the start of a comment block.
            if re.search(r'^\s*/\*.*\*/', line):
                # one-line comment - not gtkdoc
                pass
            elif re.search(r'^\s*/\*\*\s', line):
                logging.info("Found comment block start")

                in_comment_block = True
                comment = []
                starting_line = line_number + 1
        else:
            # Look for end of comment
            if re.search(r'^\s*\*+/', line):
                comments.append((comment, starting_line))
                in_comment_block = False
                continue

            # Get rid of ' * ' at start of every line in the comment block.
            line = re.sub(r'^\s*\*\s?', '', line)
            # But make sure we don't get rid of the newline at the end.
            if not line.endswith('\n'):
                line += "\n"

            logging.info("scanning :%s", line.strip())
            comment.append(line)

    return comments


def SegmentCommentBlock(lines, line_number=0, ifile=''):
    """Cut a single comment block into segments.

    Args:
        lines (list): the comment block
        line_number (int): the first line of the block (for reporting)
        ifile (str): file name of the source file (for reporting)

    Returns:
        (str, dict): symbol name and dict of comment segments
    """
    symbol = None
    in_part = ''
    segments = {'body': ''}
    params = OrderedDict()
    param_name = None
    param_indent = None
    line_number -= 1
    for line in lines:
        line_number += 1
        logging.info("scanning[%s] :%s", in_part, line.strip())

        # If we haven't found the symbol name yet, look for it.
        if not symbol:
            m1 = re.search(r'^\s*((SECTION|PROGRAM):\s*\S+)', line)
            m2 = re.search(r'^\s*([\w:-]*\w)\s*:?\s*(\(.+?\)\s*)*$', line)
            if m1:
                symbol = m1.group(1)
                logging.info("docs found in source for : '%s'", symbol)
            elif m2:
                symbol = m2.group(1)
                logging.info("docs found in source for : '%s'", symbol)
                if m2.group(2):
                    annotation = m2.group(2).strip()
                    if annotation != '':
                        SymbolAnnotations[symbol] = annotation
                        logging.info("remaining text for %s: '%s'", symbol, annotation)

            continue

        if in_part == "body":
            # Get rid of 'Description:'
            line = re.sub(r'^\s*Description:', '', line)

        m1 = re.search(r'^\s*(returns|return\s+value):', line, flags=re.I)
        m2 = re.search(r'^\s*since:', line, flags=re.I)
        m3 = re.search(r'^\s*deprecated:', line, flags=re.I)
        m4 = re.search(r'^\s*stability:', line, flags=re.I)

        if m1:
            if in_part != '':
                in_part = "return"
                segments[in_part] = line[m1.end():]
                continue
        if m2:
            if in_part != "param":
                in_part = "since"
                segments[in_part] = line[m2.end():]
                continue
        elif m3:
            if in_part != "param":
                in_part = "deprecated"
                segments[in_part] = line[m3.end():]
                continue
        elif m4:
            in_part = "stability"
            segments[in_part] = line[m4.end():]
            continue

        if in_part in ["body", "return", "since", "stability", "deprecated"]:
            segments[in_part] += line
            continue

        # We must be in the parameters. Check for the empty line below them.
        if re.search(r'^\s*$', line):
            # TODO: only switch if next line starts without indent?
            in_part = "body"
            continue

        # Look for a parameter name.
        m = re.search(r'^\s*@(.+?)\s*:\s*', line)
        if m:
            param_name = m.group(1)
            param_desc = line[m.end():]
            param_indent = None

            # Allow varargs variations
            if re.search(r'^\.\.\.$', param_name):
                param_name = "..."

            logging.info("Found param for symbol %s : '%s'= '%s'", symbol, param_name, line)
            params[param_name] = param_desc
            in_part = "param"
            continue
        elif in_part == '':
            logging.info("continuation for %s annotation '%s'", symbol, line)
            annotation = re.sub(r'^\s+|\s+$', '', line)
            if symbol in SymbolAnnotations:
                SymbolAnnotations[symbol] += annotation
            else:
                SymbolAnnotations[symbol] = annotation
            continue

        # We must be in the middle of a parameter description, so add it on
        # to the last element in @params.
        if not param_name:
            common.LogWarning(ifile, line_number,
                              "Parsing comment block file : parameter expected, but got '%s'" % line)
        else:
            if not param_indent:
                # determine indentation of first continuation line
                spc = len(line) - len(line.lstrip(' '))
                if spc > 0:
                    param_indent = spc
                    logging.debug("Found param-indentation of %d", param_indent)
            if param_indent:
                # cut common indentation (after double checking that it is all spaces)
                if line[:param_indent].strip() == '':
                    line = line[param_indent:]
                else:
                    logging.warning("Not cutting param-indentation for %s: '%s'",
                                    param_name, line[:param_indent])

            params[param_name] += line

    return (symbol, segments, params)


def ParseCommentBlockSegments(symbol, segments, params, line_number=0, ifile=''):
    """Parse the comemnts block segments.

    Args:
        symbol (str): the symbol name
        segments(dict): the comment block segments (except params)
        parans (dict): the comment block params
        line_number (int): the first line of the block (for reporting)
        ifile (str): file name of the source file (for reporting)

    """
    # Add the return value description to the end of the params.
    if "return" in segments:
        # TODO(ensonic): check for duplicated Return docs
        # common.LogWarning(file, line_number, "Multiple Returns for %s." % symbol)
        params['Returns'] = segments["return"]

    # Convert special characters
    segments["body"] = ConvertXMLChars(symbol, segments["body"])
    for (param_name, param_desc) in params.items():
        params[param_name] = ConvertXMLChars(symbol, param_desc)

    # Handle Section docs
    m = re.search(r'SECTION:\s*(.*)', symbol)
    m2 = re.search(r'PROGRAM:\s*(.*)', symbol)
    if m:
        real_symbol = m.group(1)

        logging.info("SECTION DOCS found in source for : '%s'", real_symbol)
        for param_name, param_desc in params.items():
            logging.info("   '" + param_name + "'")
            param_name = param_name.lower()
            if param_name in ['image', 'include', 'section_id', 'see_also', 'short_description', 'stability', 'title']:
                key = real_symbol + ':' + param_name
                SourceSymbolDocs[key] = param_desc
                SymbolSourceLocation[key] = (ifile, line_number)

        key = real_symbol + ":long_description"
        if key not in KnownSymbols or KnownSymbols[key] != 1:
            common.LogWarning(
                ifile, line_number, "Section %s is not defined in the %s-sections.txt file." % (real_symbol, MODULE))
        SourceSymbolDocs[key] = segments["body"]
        SymbolSourceLocation[key] = (ifile, line_number)
    elif m2:
        real_symbol = m2.group(1)
        section_id = None

        logging.info("PROGRAM DOCS found in source for '%s'", real_symbol)
        for param_name, param_desc in params.items():
            logging.info("PROGRAM key %s: '%s'", real_symbol, param_name)
            param_name = param_name.lower()
            if param_name in ['returns', 'section_id', 'see_also', 'short_description', 'synopsis']:
                key = real_symbol + ':' + param_name
                logging.info("PROGRAM value %s: '%s'", real_symbol, param_desc.rstrip())
                SourceSymbolDocs[key] = param_desc.rstrip()
                SymbolSourceLocation[key] = (ifile, line_number)
            elif re.search(r'^(-.*)', param_name):
                logging.info("PROGRAM opts: '%s': '%s'", param_name, param_desc)
                key = real_symbol + ":options"
                opts = []
                opts_str = SourceSymbolDocs.get(key)
                if opts_str:
                    opts = opts_str.split('\t')
                opts.append(param_name)
                opts.append(param_desc)

                logging.info("Setting options for symbol: %s: '%s'", real_symbol, '\t'.join(opts))
                SourceSymbolDocs[key] = '\t'.join(opts)

        key = real_symbol + ":long_description"
        SourceSymbolDocs[key] = segments["body"]
        SymbolSourceLocation[key] = (ifile, line_number)

        # TODO(ensonic): we need to track these somehow and output the files
        # later, see comment in Run()
        section_id = SourceSymbolDocs.get(real_symbol + ":section_id")
        if section_id and section_id.strip() != '':
            # Remove trailing blanks and use as is
            section_id = section_id.rstrip()
        else:
            section_id = common.CreateValidSGMLID('%s-%s' % (MODULE, real_symbol))
        OutputProgramDBFile(real_symbol, section_id)

    else:
        logging.info("SYMBOL DOCS found in source for : '%s'", symbol)
        SourceSymbolDocs[symbol] = segments["body"]
        SourceSymbolParams[symbol] = params
        SymbolSourceLocation[symbol] = (ifile, line_number)

    if "since" in segments:
        arr = segments["since"].splitlines()
        desc = arr[0].strip()
        extra_lines = arr[1:]
        logging.info("Since(%s) : [%s]", symbol, desc)
        Since[symbol] = ConvertXMLChars(symbol, desc)
        if len(extra_lines) > 1:
            common.LogWarning(ifile, line_number, "multi-line since docs found")

    if "stability" in segments:
        desc = ParseStabilityLevel(
            segments["stability"], ifile, line_number, "Stability level for %s" % symbol)
        StabilityLevel[symbol] = ConvertXMLChars(symbol, desc)

    if "deprecated" in segments:
        if symbol not in Deprecated:
            # don't warn for signals and properties
            # if ($symbol !~ m/::?(.*)/)
            if symbol in DeclarationTypes:
                common.LogWarning(ifile, line_number,
                                  "%s is deprecated in the inline comments, but no deprecation guards were found around the declaration. (See the --deprecated-guards option for gtkdoc-scan.)" % symbol)

        Deprecated[symbol] = ConvertXMLChars(symbol, segments["deprecated"])


def ParseCommentBlock(lines, line_number=0, ifile=''):
    """Parse a single comment block.

    Args:
        lines (list): the comment block
        line_number (int): the first line of the block (for reporting)
        ifile (str): file name of the source file (for reporting)
    """
    (symbol, segments, params) = SegmentCommentBlock(lines, line_number, ifile)
    if not symbol:
        # maybe its not even meant to be a gtk-doc comment?
        common.LogWarning(ifile, line_number, "Symbol name not found at the start of the comment block.")
        return

    ParseCommentBlockSegments(symbol, segments, params, line_number, ifile)


def OutputMissingDocumentation():
    """Outputs report of documentation coverage to a file.

    Returns:
        bool: True if the report was updated
    """
    old_undocumented_file = os.path.join(ROOT_DIR, MODULE + "-undocumented.txt")
    new_undocumented_file = os.path.join(ROOT_DIR, MODULE + "-undocumented.new")

    n_documented = 0
    n_incomplete = 0
    total = 0
    symbol = None
    percent = None
    buffer = ''
    buffer_deprecated = ''
    buffer_descriptions = ''

    UNDOCUMENTED = open(new_undocumented_file, 'w', encoding='utf-8')

    for symbol in sorted(AllSymbols.keys()):
        # FIXME: should we print common.LogWarnings for undocumented stuff?
        m = re.search(
            r':(title|long_description|short_description|see_also|stability|include|section_id|image)', symbol)
        m2 = re.search(r':(long_description|short_description)', symbol)
        if not m:
            total += 1
            if symbol in AllDocumentedSymbols:
                n_documented += 1
                if symbol in AllIncompleteSymbols:
                    n_incomplete += 1
                    buffer += symbol + " (" + AllIncompleteSymbols[symbol] + ")\n"

            elif symbol in Deprecated:
                if symbol in AllIncompleteSymbols:
                    n_incomplete += 1
                    buffer_deprecated += symbol + " (" + AllIncompleteSymbols[symbol] + ")\n"
                else:
                    buffer_deprecated += symbol + "\n"

            else:
                if symbol in AllIncompleteSymbols:
                    n_incomplete += 1
                    buffer += symbol + " (" + AllIncompleteSymbols[symbol] + ")\n"
                else:
                    buffer += symbol + "\n"

        elif m2:
            total += 1
            if (symbol in SymbolDocs and len(SymbolDocs[symbol]) > 0)\
               or (symbol in AllDocumentedSymbols and AllDocumentedSymbols[symbol] > 0):
                n_documented += 1
            else:
                buffer_descriptions += symbol + "\n"

    if total == 0:
        percent = 100
    else:
        percent = (n_documented / total) * 100.0

    UNDOCUMENTED.write("%.0f%% symbol docs coverage.\n" % percent)
    UNDOCUMENTED.write("%s symbols documented.\n" % n_documented)
    UNDOCUMENTED.write("%s symbols incomplete.\n" % n_incomplete)
    UNDOCUMENTED.write("%d not documented.\n" % (total - n_documented))

    if buffer_deprecated != '':
        buffer += "\n" + buffer_deprecated

    if buffer_descriptions != '':
        buffer += "\n" + buffer_descriptions

    if buffer != '':
        UNDOCUMENTED.write("\n\n" + buffer)

    UNDOCUMENTED.close()

    return common.UpdateFileIfChanged(old_undocumented_file, new_undocumented_file, 0)


def OutputUndeclaredSymbols():
    """Reports undeclared symbols.

    Outputs symbols that are listed in the section file, but have no declaration
    in the sources.

    Returns:
        bool: True if the report was updated
    """
    old_undeclared_file = os.path.join(ROOT_DIR, MODULE + "-undeclared.txt")
    new_undeclared_file = os.path.join(ROOT_DIR, MODULE + "-undeclared.new")

    with open(new_undeclared_file, 'w', encoding='utf-8') as out:
        if UndeclaredSymbols:
            out.write("\n".join(sorted(UndeclaredSymbols.keys())))
            out.write("\n")
            print("See %s-undeclared.txt for the list of undeclared symbols." % MODULE)

    return common.UpdateFileIfChanged(old_undeclared_file, new_undeclared_file, 0)


def OutputUnusedSymbols():
    """Reports unused documentation.

    Outputs symbols that are documented in comments, but not declared in the
    sources.

    Returns:
        bool: True if the report was updated
    """
    num_unused = 0
    old_unused_file = os.path.join(ROOT_DIR, MODULE + "-unused.txt")
    new_unused_file = os.path.join(ROOT_DIR, MODULE + "-unused.new")

    with open(new_unused_file, 'w', encoding='utf-8') as out:

        for symbol in sorted(Declarations.keys()):
            if symbol not in DeclarationOutput:
                out.write("%s\n" % symbol)
                num_unused += 1

        for symbol in sorted(AllUnusedSymbols.keys()):
            out.write(symbol + "(" + AllUnusedSymbols[symbol] + ")\n")
            num_unused += 1

    if num_unused != 0:
        common.LogWarning(
            old_unused_file, 1, "%d unused declarations. They should be added to %s-sections.txt in the appropriate place." % (num_unused, MODULE))

    return common.UpdateFileIfChanged(old_unused_file, new_unused_file, 0)


def OutputAllSymbols():
    """Outputs list of all symbols to a file."""
    new_symbols_file = os.path.join(ROOT_DIR, MODULE + "-symbols.txt")
    with open(new_symbols_file, 'w', encoding='utf-8') as out:
        for symbol in sorted(AllSymbols.keys()):
            out.write(symbol + "\n")


def OutputSymbolsWithoutSince():
    """Outputs list of all symbols without a since tag to a file."""
    new_nosince_file = os.path.join(ROOT_DIR, MODULE + "-nosince.txt")
    with open(new_nosince_file, 'w', encoding='utf-8') as out:
        for symbol in sorted(SourceSymbolDocs.keys()):
            if symbol in Since:
                out.write(symbol + "\n")


def CheckParamsDocumented(symbol, params):
    stype = DeclarationTypes.get(symbol)

    item = "Parameter"
    if stype:
        if stype == 'STRUCT':
            item = "Field"
        elif stype == 'ENUM':
            item = "Value"
        elif stype == 'UNION':
            item = "Field"
    else:
        stype = "SIGNAL"
    logging.info("Check param docs for %s, params: %s entries, type=%s", symbol, len(params), stype)

    if len(params) > 0:
        logging.info("params: %s", str(params))
        for (param_name, param_desc) in params.items():
            # Output a warning if the parameter is empty and remember for stats.
            if param_name != "void" and not re.search(r'\S', param_desc):
                if symbol in AllIncompleteSymbols:
                    AllIncompleteSymbols[symbol] += ", " + param_name
                else:
                    AllIncompleteSymbols[symbol] = param_name

                common.LogWarning(*GetSymbolSourceLocation(symbol),
                                  "%s description for %s::%s is missing in source code comment block." % (item, symbol, param_name))

    elif len(params) == 0:
        AllIncompleteSymbols[symbol] = "<items>"
        common.LogWarning(*GetSymbolSourceLocation(symbol),
                          "%s descriptions for %s are missing in source code comment block." % (item, symbol))


def MergeSourceDocumentation():
    """Merges documentation read from a source file.

    Parameter descriptions override any in the template files.
    Function descriptions are placed before any description from
    the template files.
    """

    # add what's found in the source
    symbols = set(SourceSymbolDocs.keys())

    # and add known symbols from -sections.txt
    for symbol in KnownSymbols.keys():
        if KnownSymbols[symbol] == 1:
            symbols.add(symbol)

    logging.info("num source entries: %d", len(symbols))

    for symbol in symbols:
        AllSymbols[symbol] = 1

        if symbol in SourceSymbolDocs:
            logging.info("merging [%s] from source", symbol)

            # remove leading and training whitespaces
            src_docs = SourceSymbolDocs[symbol].strip()
            if src_docs != '':
                AllDocumentedSymbols[symbol] = 1

            SymbolDocs[symbol] = src_docs

            # merge parameters
            if symbol in SourceSymbolParams:
                param_docs = SourceSymbolParams[symbol]
                SymbolParams[symbol] = param_docs
                # if this symbol is documented, check if docs are complete
                # remove all xml-tags and whitespaces
                check_docs = re.sub(r'\s', '', re.sub(r'<.*?>', '', src_docs))
                if check_docs != '' and param_docs:
                    CheckParamsDocumented(symbol, param_docs)
        else:
            logging.info("[%s] undocumented", symbol)

    logging.info("num doc entries: %d / %d", len(SymbolDocs), len(SourceSymbolDocs))


def IsEmptyDoc(doc):
    """Check if a doc-string is empty.

    It is also regarded as empty if it only consist of whitespace or e.g. FIXME.

    Args:
        doc (str): the doc-string

    Returns:
        bool: True if empty
    """
    if re.search(r'^\s*$', doc):
        return True
    if re.search(r'^\s*<para>\s*(FIXME)?\s*<\/para>\s*$', doc):
        return True
    return False


def ConvertMarkDown(symbol, text):
    md_to_db.Init()
    return md_to_db.MarkDownParse(text, symbol)


def ReadDeclarationsFile(ifile, override):
    """Reads in a file containing the function/macro/enum etc. declarations.

    Note that in some cases there are several declarations with
    the same name, e.g. for conditional macros. In this case we
    set a flag in the DeclarationConditional hash so the
    declaration is not shown in the docs.

    If a macro and a function have the same name, e.g. for
    g_object_ref, the function declaration takes precedence.

    Some opaque structs are just declared with 'typedef struct
    _name name;' in which case the declaration may be empty.
    The structure may have been found later in the header, so
    that overrides the empty declaration.

    Args:
        file (str): the declarations file to read
        override (bool): if declarations in this file should override
                         any current declaration.
    """
    if override == 0:
        Declarations.clear()
        DeclarationTypes.clear()
        DeclarationConditional.clear()
        DeclarationOutput.clear()

    INPUT = open(ifile, 'r', encoding='utf-8')
    declaration_type = ''
    declaration_name = None
    declaration = None
    is_deprecated = 0
    line_number = 0
    for line in INPUT:
        line_number += 1
        # logging.debug("%s:%d: %s", ifile, line_number, line)
        if not declaration_type:
            m1 = re.search(r'^<([^>]+)>', line)
            if m1:
                declaration_type = m1.group(1)
                declaration_name = ''
                logging.info("Found declaration: %s", declaration_type)
                declaration = ''
        else:
            m2 = re.search(r'^<NAME>(.*)</NAME>', line)
            m3 = re.search(r'^<DEPRECATED/>', line)
            m4 = re.search(r'^</%s>' % declaration_type, line)
            if m2:
                declaration_name = m2.group(1)
            elif m3:
                is_deprecated = True
            elif m4:
                logging.info("Found end of declaration: %s, %s", declaration_type, declaration_name)
                # Check that the declaration has a name
                if declaration_name == '':
                    common.LogWarning(ifile, line_number, declaration_type + " has no name.\n")

                # If the declaration is an empty typedef struct _XXX XXX
                # set the flag to indicate the struct has a typedef.
                if (declaration_type == 'STRUCT' or declaration_type == 'UNION') \
                        and re.search(r'^\s*$', declaration):
                    logging.info("Struct has typedef: %s", declaration_name)
                    StructHasTypedef[declaration_name] = 1

                # Check if the symbol is already defined.
                if declaration_name in Declarations and override == 0:
                    # Function declarations take precedence.
                    if DeclarationTypes[declaration_name] == 'FUNCTION':
                        # Ignore it.
                        pass
                    elif declaration_type == 'FUNCTION':
                        if is_deprecated:
                            Deprecated[declaration_name] = ''

                        Declarations[declaration_name] = declaration
                        DeclarationTypes[declaration_name] = declaration_type
                    elif DeclarationTypes[declaration_name] == declaration_type:
                        # If the existing declaration is empty, or is just a
                        # forward declaration of a struct, override it.
                        if declaration_type == 'STRUCT' or declaration_type == 'UNION':
                            if re.search(r'^\s*((struct|union)\s+\w+\s*;)?\s*$', Declarations[declaration_name]):
                                if is_deprecated:
                                    Deprecated[declaration_name] = ''
                                Declarations[declaration_name] = declaration
                            elif re.search(r'^\s*((struct|union)\s+\w+\s*;)?\s*$', declaration):
                                # Ignore an empty or forward declaration.
                                pass
                            else:
                                common.LogWarning(
                                    ifile, line_number, "Structure %s has multiple definitions." % declaration_name)

                        else:
                            # set flag in %DeclarationConditional hash for
                            # multiply defined macros/typedefs.
                            DeclarationConditional[declaration_name] = 1

                    else:
                        common.LogWarning(ifile, line_number, declaration_name + " has multiple definitions.")

                else:
                    if is_deprecated:
                        Deprecated[declaration_name] = ''

                    Declarations[declaration_name] = declaration
                    DeclarationTypes[declaration_name] = declaration_type
                    logging.debug("added declaration: %s, %s, [%s]", declaration_type, declaration_name, declaration)

                declaration_type = ''
                is_deprecated = False
            else:
                declaration += line
    INPUT.close()


def ReadSignalsFile(ifile):
    """Reads information about object signals.

    It creates the arrays @SignalNames and @SignalPrototypes containing details
    about the signals. The first line of the SignalPrototype is the return type
    of the signal handler. The remaining lines are the parameters passed to it.
    The last parameter, "gpointer user_data" is always the same so is not included.

    Args:
       ifile (str): the file containing the signal handler prototype information.
    """
    in_signal = 0
    signal_object = None
    signal_name = None
    signal_returns = None
    signal_flags = None
    signal_prototype = None

    # Reset the signal info.
    SignalObjects[:] = []
    SignalNames[:] = []
    SignalReturns[:] = []
    SignalFlags[:] = []
    SignalPrototypes[:] = []

    if not os.path.isfile(ifile):
        return

    INPUT = open(ifile, 'r', encoding='utf-8')
    line_number = 0
    for line in INPUT:
        line_number += 1
        if not in_signal:
            if re.search(r'^<SIGNAL>', line):
                in_signal = 1
                signal_object = ''
                signal_name = ''
                signal_returns = ''
                signal_prototype = ''

        else:
            m = re.search(r'^<NAME>(.*)<\/NAME>', line)
            m2 = re.search(r'^<RETURNS>(.*)<\/RETURNS>', line)
            m3 = re.search(r'^<FLAGS>(.*)<\/FLAGS>', line)
            if m:
                signal_name = m.group(1)
                m1_2 = re.search(r'^(.*)::(.*)$', signal_name)
                if m1_2:
                    signal_object = m1_2.group(1)
                    signal_name = m1_2.group(2).replace('_', '-')
                    logging.info("Found signal: %s", signal_name)
                else:
                    common.LogWarning(ifile, line_number, "Invalid signal name: %s." % signal_name)

            elif m2:
                signal_returns = m2.group(1)
            elif m3:
                signal_flags = m3.group(1)
            elif re.search(r'^</SIGNAL>', line):
                logging.info("Found end of signal: %s::%s\nReturns: %s\n%s",
                             signal_object, signal_name, signal_returns, signal_prototype)
                SignalObjects.append(signal_object)
                SignalNames.append(signal_name)
                SignalReturns.append(signal_returns)
                SignalFlags.append(signal_flags)
                SignalPrototypes.append(signal_prototype)
                in_signal = False
            else:
                signal_prototype += line
    INPUT.close()


def ReadObjectHierarchy(ifile):
    """Reads the $MODULE-hierarchy file.

    This contains all the GObject subclasses described in this module (and their
    ancestors).
    It places them in the Objects array, and places their level
    in the object hierarchy in the ObjectLevels array, at the
    same index. GObject, the root object, has a level of 1.

    Args:
        ifile (str): the input filename.
    """

    if not os.path.isfile(ifile):
        logging.debug('no %s file found', ifile)
        return

    Objects[:] = []
    ObjectLevels[:] = []

    INPUT = open(ifile, 'r', encoding='utf-8')

    # Only emit objects if they are supposed to be documented, or if
    # they have documented children. To implement this, we maintain a
    # stack of pending objects which will be emitted if a documented
    # child turns up.
    pending_objects = []
    pending_levels = []
    root = None
    tree = []
    for line in INPUT:
        m1 = re.search(r'\S+', line)
        if not m1:
            continue

        gobject = m1.group(0)
        level = len(line[:m1.start()]) // 2 + 1

        if level == 1:
            root = gobject

        while pending_levels and pending_levels[-1] >= level:
            pending_objects.pop()
            pending_levels.pop()

        pending_objects.append(gobject)
        pending_levels.append(level)

        if gobject in KnownSymbols:
            while len(pending_levels) > 0:
                gobject = pending_objects.pop(0)
                level = pending_levels.pop(0)
                xref = MakeXRef(gobject)

                tree.append(' ' * (level * 4) + xref)
                Objects.append(gobject)
                ObjectLevels.append(level)
                ObjectRoots[gobject] = root
        # else
        #    common.LogWarning(ifile, line_number, "unknown type %s" % object)
        #

    INPUT.close()
    logging.debug('got %d entries for hierarchy', len(tree))
    return tree


def ReadInterfaces(ifile):
    """Reads the $MODULE.interfaces file.

    Args:
        ifile (str): the input filename.
    """

    Interfaces.clear()

    if not os.path.isfile(ifile):
        return

    INPUT = open(ifile, 'r', encoding='utf-8')

    for line in INPUT:
        line = line.strip()
        ifaces = line.split()
        gobject = ifaces.pop(0)
        if gobject in KnownSymbols and KnownSymbols[gobject] == 1:
            knownIfaces = []

            # filter out private interfaces, but leave foreign interfaces
            for iface in ifaces:
                if iface not in KnownSymbols or KnownSymbols[iface] == 1:
                    knownIfaces.append(iface)

            Interfaces[gobject] = ' '.join(knownIfaces)
            logging.info("Interfaces for %s: %s", gobject, Interfaces[gobject])
        else:
            logging.info("skipping interfaces for unknown symbol: %s", gobject)

    INPUT.close()


def ReadPrerequisites(ifile):
    """This reads in the $MODULE.prerequisites file.

    Args:
        ifile (str): the input filename.
    """
    Prerequisites.clear()

    if not os.path.isfile(ifile):
        return

    INPUT = open(ifile, 'r', encoding='utf-8')

    for line in INPUT:
        line = line.strip()
        prereqs = line.split()
        iface = prereqs.pop(0)
        if iface in KnownSymbols and KnownSymbols[iface] == 1:
            knownPrereqs = []

            # filter out private prerequisites, but leave foreign prerequisites
            for prereq in prereqs:
                if prereq not in KnownSymbols or KnownSymbols[prereq] == 1:
                    knownPrereqs.append(prereq)

            Prerequisites[iface] = ' '.join(knownPrereqs)

    INPUT.close()


def ReadArgsFile(ifile):
    """Reads information about object properties

    It creates the arrays ArgObjects, ArgNames, ArgTypes, ArgFlags, ArgNicks and
    ArgBlurbs containing info on the args.

    Args:
        ifile (str): the input filename.
    """
    in_arg = False
    arg_object = None
    arg_name = None
    arg_type = None
    arg_flags = None
    arg_nick = None
    arg_blurb = None
    arg_default = None
    arg_range = None

    # Reset the args info.
    ArgObjects[:] = []
    ArgNames[:] = []
    ArgTypes[:] = []
    ArgFlags[:] = []
    ArgNicks[:] = []
    ArgBlurbs[:] = []
    ArgDefaults[:] = []
    ArgRanges[:] = []

    if not os.path.isfile(ifile):
        return

    INPUT = open(ifile, 'r', encoding='utf-8')
    line_number = 0
    for line in INPUT:
        line_number += 1
        if not in_arg:
            if line.startswith('<ARG>'):
                in_arg = True
                arg_object = ''
                arg_name = ''
                arg_type = ''
                arg_flags = ''
                arg_nick = ''
                arg_blurb = ''
                arg_default = ''
                arg_range = ''

        else:
            m1 = re.search(r'^<NAME>(.*)</NAME>', line)
            m2 = re.search(r'^<TYPE>(.*)</TYPE>', line)
            m3 = re.search(r'^<RANGE>(.*)</RANGE>', line)
            m4 = re.search(r'^<FLAGS>(.*)</FLAGS>', line)
            m5 = re.search(r'^<NICK>(.*)</NICK>', line)
            m6 = re.search(r'^<BLURB>(.*)</BLURB>', line)
            m7 = re.search(r'^<DEFAULT>(.*)</DEFAULT>', line)
            if m1:
                arg_name = m1.group(1)
                m1_1 = re.search(r'^(.*)::(.*)$', arg_name)
                if m1_1:
                    arg_object = m1_1.group(1)
                    arg_name = m1_1.group(2).replace('_', '-')
                    logging.info("Found arg: %s", arg_name)
                else:
                    common.LogWarning(ifile, line_number, "Invalid argument name: " + arg_name)

            elif m2:
                arg_type = m2.group(1)
            elif m3:
                arg_range = m3.group(1)
            elif m4:
                arg_flags = m4.group(1)
            elif m5:
                arg_nick = m5.group(1)
            elif m6:
                arg_blurb = m6.group(1)
                if arg_blurb == "(null)":
                    arg_blurb = ''
                    common.LogWarning(
                        ifile, line_number, "Property %s:%s has no documentation." % (arg_object, arg_name))

            elif m7:
                arg_default = m7.group(1)
            elif re.search(r'^</ARG>', line):
                logging.info("Found end of arg: %s::%s\n%s : %s", arg_object, arg_name, arg_type, arg_flags)
                ArgObjects.append(arg_object)
                ArgNames.append(arg_name)
                ArgTypes.append(arg_type)
                ArgRanges.append(arg_range)
                ArgFlags.append(arg_flags)
                ArgNicks.append(arg_nick)
                ArgBlurbs.append(arg_blurb)
                ArgDefaults.append(arg_default)
                in_arg = False

    INPUT.close()


def AddTreeLineArt(tree):
    """Generate a line art tree.

    Add unicode lineart to a pre-indented string array and returns
    it as as multiline string.

    Args:
        tree (list): of indented strings.

    Returns:
        str: multiline string with tree line art
    """
    # iterate bottom up over the tree
    for i in range(len(tree) - 1, -1, -1):
        # count leading spaces
        m = re.search(r'^([^<A-Za-z]*)', tree[i])
        indent = len(m.group(1))
        # replace with ╰───, if place of ╰ is not space insert ├
        if indent > 4:
            if tree[i][indent - 4] == " ":
                tree[i] = tree[i][:indent - 4] + "--- " + tree[i][indent:]
            else:
                tree[i] = tree[i][:indent - 4] + "+-- " + tree[i][indent:]

            # go lines up while space and insert |
            j = i - 1
            while j >= 0 and tree[j][indent - 4] == ' ':
                tree[j] = tree[j][:indent - 4] + '|' + tree[j][indent - 3:]
                j -= 1

    res = "\n".join(tree)
    # unicode chars for: ╰──
    res = re.sub(r'---', '<phrase role=\"lineart\">&#9584;&#9472;&#9472;</phrase>', res)
    # unicde chars for: ├──
    res = re.sub(r'\+--', '<phrase role=\"lineart\">&#9500;&#9472;&#9472;</phrase>', res)
    # unicode char for: │
    res = re.sub(r'\|', '<phrase role=\"lineart\">&#9474;</phrase>', res)

    return res


def CheckIsObject(name):
    """Check if symbols is an object.

    It uses the global Objects array. Note that the Objects array only contains
    classes in the current module and their ancestors - not all GObject classes.

    Args:
        name (str): the object name to check.

    Returns:
        bool: True if the given name is a GObject or a subclass.
    """
    root = ObjectRoots.get(name)
    # Let GBoxed pass as an object here to get -struct appended to the id
    # and prevent conflicts with sections.
    return root and root != 'GEnum' and root != 'GFlags'


def GetSymbolParams(symbol):
    """Get the symbol params and check that they are not empty.

    If no parameters are filled in, we don't generate the description table,
    for backwards compatibility.

    Args:
        symbol: the symbol to check the parameters for

    Returns:
        dict: The parameters
        bool: True if empty
    """
    params = SymbolParams.get(symbol, {})
    # TODO: strip at parsing stage?
    found = next((True for p in params.values() if p.strip() != ''), False)
    return (params, found)


def GetSymbolSourceLocation(symbol):
    """Get the filename and line where the symbol docs where taken from."""
    return SymbolSourceLocation.get(symbol, ('', 0))
