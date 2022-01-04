#!/usr/bin/env python3
# -*- python; coding: utf-8 -*-
#
# gtk-doc - GTK DocBook documentation generator.
# Copyright (C) 2018  Stefan Sauer
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
Highlight sourcecode snippets.
"""
import os

from pygments import highlight
from pygments.lexers import CLexer
from pygments.lexers import get_lexer_by_name
from pygments.formatters import HtmlFormatter

# lazily constructed lexer cache
LEXERS = {
    'c': CLexer()
}
HTML_FORMATTER = HtmlFormatter(nowrap=True)


def highlight_code(code, lang='c'):
    if lang not in LEXERS:
        LEXERS[lang] = get_lexer_by_name(lang)
    lexer = LEXERS.get(lang, None)
    if not lexer:
        return None
    return highlight(code, lexer, HTML_FORMATTER)


def append_style_defs(css_file_name):
    os.chmod(css_file_name, 0o644)
    with open(css_file_name, 'at', newline='\n', encoding='utf-8') as css:
        css.write(HTML_FORMATTER.get_style_defs())
