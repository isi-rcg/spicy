# -*- python; coding: utf-8 -*-
#
# gtk-doc - GTK DocBook documentation generator.
# Copyright (C) 1998 Owen Taylor
#               2001-2005 Damon Chaplin
#               2009-2017  Stefan Sauer
#               2017  Jussi Pakkanen
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

import logging
import subprocess

from . import config


def run(options):
    logging.info('options: %s', str(options.__dict__))

    module = options.args[0]
    document = options.args[1]
    if options.verbose:
        quiet = '0'
    else:
        quiet = '1'

    # we could do "--path $PWD " to avoid needing rewriting entities that
    # are copied from the header into docs under xml
    path_arg = []
    for path in options.path:
        path_arg += ['--path', path]

    # would it make sense to create man pages only for certain refentries
    # e.g. for tools
    # see http://bugzilla.gnome.org/show_bug.cgi?id=467488
    return subprocess.call([config.xsltproc] + path_arg + [
        '--nonet',
        '--xinclude',
        '--stringparam',
        'gtkdoc.bookname',
        module,
        '--stringparam',
        'gtkdoc.version',
        config.version,
        '--stringparam',
        'chunk.quietly ',
        quiet,
        '--stringparam',
        'chunker.output.quiet',
        quiet,
        'http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl',
        document])
