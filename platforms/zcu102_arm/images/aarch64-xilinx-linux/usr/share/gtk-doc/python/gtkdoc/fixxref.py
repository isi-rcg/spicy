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

''"Fix cross-references in the HTML documentation.''"

import logging
import os
import re

from . import common, highlight

# This contains all the entities and their relative URLs.
Links = {}

# failing link targets we don't warn about even once
NoLinks = {
    'char',
    'double',
    'float',
    'int',
    'long',
    'main',
    'signed',
    'unsigned',
    'va-list',
    'void',
}


def Run(options):
    logging.info('options: %s', str(options.__dict__))

    LoadIndicies(options.module_dir, options.html_dir, options.extra_dir)
    ReadSections(options.module)
    FixCrossReferences(options.module_dir, options.module, options.src_lang)
    highlight.append_style_defs(os.path.join(options.module_dir, 'style.css'))


# TODO(ensonic): try to refactor so that we get a list of path's and then just
# loop over them.
# - module_dir is by default 'html'
# - html_dir can be set by configure, defaults to $(docdir)
def LoadIndicies(module_dir, html_dir, extra_dirs):
    # Cache of dirs we already scanned for index files
    dir_cache = {}

    path_prefix = ''
    m = re.search(r'(.*?)/share/gtk-doc/html', html_dir)
    if m:
        path_prefix = m.group(1)
        logging.info('Path prefix: %s', path_prefix)
    prefix_match = r'^' + re.escape(path_prefix) + r'/'

    # We scan the directory containing GLib and any directories in GNOME2_PATH
    # first, but these will be overridden by any later scans.
    dir = common.GetModuleDocDir('glib-2.0')
    if dir and os.path.exists(dir):
        # Some predefined link targets to get links into type hierarchies as these
        # have no targets. These are always absolute for now.
        Links['GBoxed'] = dir + '/gobject/gobject-Boxed-Types.html'
        Links['GEnum'] = dir + '/gobject/gobject-Enumeration-and-Flag-Types.html'
        Links['GFlags'] = dir + '/gobject/gobject-Enumeration-and-Flag-Types.html'
        Links['GInterface'] = dir + '/gobject/GTypeModule.html'

        if dir != html_dir:
            logging.info('Scanning GLib directory: %s', dir)
            ScanIndices(dir, (re.search(prefix_match, dir) is None), dir_cache)
    else:
        NoLinks.add('GBoxed')
        NoLinks.add('GEnum')
        NoLinks.add('GFlags')
        NoLinks.add('GInterface')

    path = os.environ.get('GNOME2_PATH')
    if path:
        for dir in path.split(':'):
            dir += 'share/gtk-doc/html'
            if os.path.exists(dir) and dir != html_dir:
                logging.info('Scanning GNOME2_PATH directory: %s', dir)
                ScanIndices(dir, (re.search(prefix_match, dir) is None), dir_cache)

    logging.info('Scanning HTML_DIR directory: %s', html_dir)
    ScanIndices(html_dir, False, dir_cache)
    logging.info('Scanning MODULE_DIR directory: %s', module_dir)
    ScanIndices(module_dir, False, dir_cache)

    # check all extra dirs, but skip already scanned dirs or subdirs of those
    for dir in extra_dirs:
        dir = dir.rstrip('/')
        logging.info('Scanning EXTRA_DIR directory: %s', dir)

        # If the --extra-dir option is not relative and is not sharing the same
        # prefix as the target directory of the docs, we need to use absolute
        # directories for the links
        if not dir.startswith('..') and re.search(prefix_match, dir) is None:
            ScanIndices(dir, True, dir_cache)
        else:
            ScanIndices(dir, False, dir_cache)


def ScanIndices(scan_dir, use_absolute_links, dir_cache):
    if not scan_dir or scan_dir in dir_cache:
        return
    dir_cache[scan_dir] = 1

    logging.info('Scanning index directory: %s, absolute: %d', scan_dir, use_absolute_links)

    # TODO(ensonic): this code is the same as in rebase.py
    if not os.path.isdir(scan_dir):
        logging.info('Cannot open dir "%s"', scan_dir)
        return

    subdirs = []
    for entry in sorted(os.listdir(scan_dir)):
        full_entry = os.path.join(scan_dir, entry)
        if os.path.isdir(full_entry):
            subdirs.append(full_entry)
            continue

        if entry.endswith('.devhelp2'):
            # if devhelp-file is good don't read index.sgml
            ReadDevhelp(full_entry, use_absolute_links)
        elif entry == "index.sgml.gz" and not os.path.exists(os.path.join(scan_dir, 'index.sgml')):
            # debian/ubuntu started to compress this as index.sgml.gz :/
            print(''' Please fix https://bugs.launchpad.net/ubuntu/+source/gtk-doc/+bug/77138 . For now run:
gunzip %s
''' % full_entry)
        elif entry.endswith('.devhelp2.gz') and not os.path.exists(full_entry[:-3]):
            # debian/ubuntu started to compress this as *devhelp2.gz :/
            print('''Please fix https://bugs.launchpad.net/ubuntu/+source/gtk-doc/+bug/1466210 . For now run:
gunzip %s
''' % full_entry)
        # we could consider supporting: gzip module

    # Now recursively scan the subdirectories.
    for subdir in subdirs:
        ScanIndices(subdir, use_absolute_links, dir_cache)


def ReadDevhelp(file, use_absolute_links):
    # Determine the absolute directory, to be added to links in $file
    # if we need to use an absolute link.
    # $file will be something like /prefix/gnome/share/gtk-doc/html/gtk/$file
    # We want the part up to 'html/.*' since the links in $file include
    # the rest.
    dir = "../"
    if use_absolute_links:
        # For uninstalled index files we'd need to map the path to where it
        # will be installed to
        if not file.startswith('./'):
            m = re.search(r'(.*\/)(.*?)\/.*?\.devhelp2', file)
            dir = m.group(1) + m.group(2) + '/'
    else:
        m = re.search(r'(.*\/)(.*?)\/.*?\.devhelp2', file)
        if m:
            dir += m.group(2) + '/'
        else:
            dir = ''

    logging.info('Scanning index file=%s, absolute=%d, dir=%s', file, use_absolute_links, dir)

    for line in open(file, 'r', encoding='utf-8'):
        m = re.search(r' link="([^#]*)#([^"]*)"', line)
        if m:
            link = m.group(1) + '#' + m.group(2)
            logging.debug('Found id: %s href: %s', m.group(2), link)
            Links[m.group(2)] = dir + link


def ReadSections(module):
    """We don't warn on missing links to non-public sysmbols."""
    for line in open(module + '-sections.txt', 'r', encoding='utf-8'):
        m1 = re.search(r'^<SUBSECTION\s*(.*)>', line)
        if line.startswith('#') or line.strip() == '':
            continue
        elif line.startswith('<SECTION>'):
            subsection = ''
        elif m1:
            subsection = m1.group(1)
        elif line.startswith('<SUBSECTION>') or line.startswith('</SECTION>'):
            continue
        elif re.search(r'^<TITLE>(.*)<\/TITLE>', line):
            continue
        elif re.search(r'^<FILE>(.*)<\/FILE>', line):
            continue
        elif re.search(r'^<INCLUDE>(.*)<\/INCLUDE>', line):
            continue
        else:
            symbol = line.strip()
            if subsection == "Standard" or subsection == "Private":
                NoLinks.add(common.CreateValidSGMLID(symbol))


def FixCrossReferences(module_dir, module, src_lang):
    # TODO(ensonic): use glob.glob()?
    for entry in sorted(os.listdir(module_dir)):
        full_entry = os.path.join(module_dir, entry)
        if os.path.isdir(full_entry):
            continue
        elif entry.endswith('.html') or entry.endswith('.htm'):
            FixHTMLFile(src_lang, module, full_entry)


def FixHTMLFile(src_lang, module, file):
    logging.info('Fixing file: %s', file)

    content = open(file, 'r', encoding='utf-8').read()

    # FIXME: ideally we'd pass a clue about the example language to the highligher
    # unfortunately the "language" attribute is not appearing in the html output
    # we could patch the customization to have <code class="xxx"> inside of <pre>
    def repl_func(m):
        return HighlightSourcePygments(src_lang, m.group(1), m.group(2))
    content = re.sub(
        r'<div class=\"(example-contents|informalexample)\"><pre class=\"programlisting\">(.*?)</pre></div>',
        repl_func, content, flags=re.DOTALL)
    content = re.sub(r'\&lt;GTKDOCLINK\s+HREF=\&quot;(.*?)\&quot;\&gt;(.*?)\&lt;/GTKDOCLINK\&gt;',
                     r'\<GTKDOCLINK\ HREF=\"\1\"\>\2\</GTKDOCLINK\>', content, flags=re.DOTALL)

    # From the highlighter we get all the functions marked up. Now we can turn them into GTKDOCLINK items
    def repl_func(m):
        return MakeGtkDocLink(m.group(1), m.group(2), m.group(3))
    content = re.sub(r'(<span class=\"function\">)(.*?)(</span>)', repl_func, content, flags=re.DOTALL)
    # We can also try the first item in stuff marked up as 'normal'
    content = re.sub(
        r'(<span class=\"normal\">\s*)(.+?)((\s+.+?)?\s*</span>)', repl_func, content, flags=re.DOTALL)

    lines = content.rstrip().split('\n')

    def repl_func_with_ix(i):
        def repl_func(m):
            return MakeXRef(module, file, i + 1, m.group(1), m.group(2))
        return repl_func

    for i in range(len(lines)):
        lines[i] = re.sub(r'<GTKDOCLINK\s+HREF="([^"]*)"\s*>(.*?)</GTKDOCLINK\s*>', repl_func_with_ix(i), lines[i])
        if 'GTKDOCLINK' in lines[i]:
            logging.info('make xref failed for line %d: "%s"', i, lines[i])

    new_file = file + '.new'
    content = '\n'.join(lines)
    with open(new_file, 'w', encoding='utf-8') as h:
        h.write(content)

    os.unlink(file)
    os.rename(new_file, file)


def GetXRef(id):
    href = Links.get(id)
    if href:
        return (id, href)

    # This is a workaround for some inconsistency we have with CreateValidSGMLID
    if ':' in id:
        tid = id.replace(':', '--')
        href = Links.get(tid)
        if href:
            return (tid, href)

    # poor mans plural support
    if id.endswith('es'):
        tid = id[:-2]
        href = Links.get(tid)
        if href:
            return (tid, href)
        tid += '-struct'
        href = Links.get(tid)
        if href:
            return (tid, href)
    elif id.endswith('s'):
        tid = id[:-1]
        href = Links.get(tid)
        if href:
            return (tid, href)
        tid += '-struct'
        href = Links.get(tid)
        if href:
            return (tid, href)

    tid = id + '-struct'
    href = Links.get(tid)
    if href:
        return (tid, href)

    return (id, None)


def ReportBadXRef(file, line, id, text):
    logging.info('no link for: id=%s, linktext=%s', id, text)

    # don't warn multiple times and also skip blacklisted (ctypes)
    if id in NoLinks:
        return
    # if it's a function, don't warn if it does not contain a "_"
    # (transformed to "-")
    # - gnome coding style would use '_'
    # - will avoid wrong warnings for ansi c functions
    if re.search(r' class=\"function\"', text) and '-' not in id:
        return
    # if it's a 'return value', don't warn (implicitly created link)
    if re.search(r' class=\"returnvalue\"', text):
        return
    # if it's a 'type', don't warn if it starts with lowercase
    # - gnome coding style would use CamelCase
    if re.search(r' class=\"type\"', text) and id[0].islower():
        return
    # don't warn for self links
    if text == id:
        return

    common.LogWarning(file, line, 'no link for: "%s" -> (%s).' % (id, text))
    NoLinks.add(id)


def MakeRelativeXRef(module, href):
    # if it is a link to same module, remove path to make it work uninstalled
    m = re.search(r'^\.\./' + module + '/(.*)$', href)
    if m:
        href = m.group(1)
    return href


def MakeXRef(module, file, line, id, text):
    href = GetXRef(id)[1]

    if href:
        href = MakeRelativeXRef(module, href)
        logging.info('Fixing link: %s, %s, %s', id, href, text)
        return "<a href=\"%s\">%s</a>" % (href, text)
    else:
        ReportBadXRef(file, line, id, text)
        return text


def MakeGtkDocLink(pre, symbol, post):
    id = common.CreateValidSGMLID(symbol)

    # these are implicitly created links in highlighted sources
    # we don't want warnings for those if the links cannot be resolved.
    NoLinks.add(id)

    return pre + '<GTKDOCLINK HREF="' + id + '">' + symbol + '</GTKDOCLINK>' + post


def HighlightSourcePygments(src_lang, div_class, source):
    # chop of leading and trailing empty lines, leave leading space in first real line
    source = source.strip(' ')
    source = source.strip('\n')
    source = source.rstrip()

    # cut common indent
    m = re.search(r'^(\s+)', source)
    if m:
        source = re.sub(r'^' + m.group(1), '', source, flags=re.MULTILINE)
    # avoid double entity replacement
    source = source.replace('&lt;', '<')
    source = source.replace('&gt;', '>')
    source = source.replace('&amp;', '&')

    highlighted_source = highlight.highlight_code(source, src_lang)
    if not highlighted_source:
        highlighted_source = source

    # chop of leading and trailing empty lines
    highlighted_source = highlighted_source.strip()

    # turn common urls in comments into links
    highlighted_source = re.sub(r'<span class="url">(.*?)</span>',
                                r'<span class="url"><a href="\1">\1</a></span>',
                                highlighted_source, flags=re.DOTALL)

    # we do own line-numbering
    line_count = highlighted_source.count('\n')
    source_lines = '\n'.join([str(i) for i in range(1, line_count + 2)])

    return """<div class="%s">
  <table class="listing_frame" border="0" cellpadding="0" cellspacing="0">
    <tbody>
      <tr>
        <td class="listing_lines" align="right"><pre>%s</pre></td>
        <td class="listing_code"><pre class="programlisting">%s</pre></td>
      </tr>
    </tbody>
  </table>
</div>
""" % (div_class, source_lines, highlighted_source)
