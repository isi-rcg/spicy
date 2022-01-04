# -*- python; coding: utf-8 -*-
#
# gtk-doc - GTK DocBook documentation generator.
# Copyright (C) 2009-2017  Stefan Sauer
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
import os
import sys
import subprocess

from . import config


def run_xsltproc(options, args):
    command = [config.xsltproc]
    # we could do "--path $PWD " to avoid needing rewriting entities that are
    # copied from the header into docs under xml
    if len(options.path):
        command += ['--path', ':'.join(options.path)]
    logging.info('running "%s"', ' '.join(command + args))
    pc = subprocess.Popen(command + args, stderr=subprocess.PIPE)
    (o, stde) = pc.communicate()
    with open('profile.txt', 'wb') as h:
        h.write(stde)
    return pc.returncode


def run(options):
    logging.info('options: %s', str(options.__dict__))

    module = options.args[0]
    document = options.args[1]

    if options.uninstalled:
        # this does not work from buiddir!=srcdir
        # we could try this
        # MAKE_SCRDIR=$(abs_srcdir) MAKE_BUILDDIR=$(abs_builddir) gtkdoc-mkpdf ...
        gtkdocdir = os.path.split(sys.argv[0])[0]
    else:
        gtkdocdir = os.path.join(config.datadir, 'gtk-doc/data')

    if config.dblatex != '':
        # extra options to consider
        # -I FIG_PATH
        # -V is useful for debugging
        # -T db2latex : different style
        # -d : keep transient files (for debugging)
        # -P abc.def=$quiet : once the stylesheets have a quiet mode
        # -x "--path /path/to/more/files"
        # xsltproc is already called with --xinclude
        # does not work: --xslt-opts "--path $searchpath --nonet $@"
        dblatex_options = ['-o', module + '.pdf']
        for i in options.imgdir:
            dblatex_options += ['-I', i]
        if len(options.path):
            dblatex_options += ['-x', '--path ' + ':'.join(options.path)]
        dblatex_options.append(document)
        if not options.verbose:
            pc = subprocess.Popen([config.dblatex, '--help'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            (stdo, stde) = pc.communicate()
            if b'--quiet' in stdo or b'--quiet' in stde:
                dblatex_options = ['--quiet'] + dblatex_options
        dbcmd = [config.dblatex] + dblatex_options
        pc = subprocess.Popen(dbcmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (stde, _) = pc.communicate()
        for line in stde.decode('utf-8').split('\n'):
            if not line.strip():
                continue
            if 'programlisting or screen' in line:
                continue
            # This happens when dblatex has no support for some special chars
            if 'Missing character' in line:
                continue
            print(line)
        res = pc.returncode
    elif config.fop != '':
        if options.verbose:
            quiet = '0'
        else:
            quiet = '1'
        res = run_xsltproc(options, ['--nonet',
                                     '--xinclude',
                                     '--stringparam',
                                     'gtkdoc.bookname',
                                     module,
                                     '--stringparam',
                                     'gtkdoc.version',
                                     config.version,
                                     '--stringparam',
                                     'chunk.quietly',
                                     quiet,
                                     '--stringparam',
                                     'chunker.output.quiet',
                                     quiet,
                                     module,
                                     document,
                                     '-o',
                                     module + '.fo',
                                     gtkdocdir + '/gtk-doc-fo.xsl',
                                     document])
        # TODO: fop dies too easily :(
        # res = subprocess.call([config.fop, module + '.fo', module + '.pdf'))
        fname = module + '.fo'
        if os.path.exists(fname):
            os.unlink(fname)
    else:
        print("dblatex or fop must be installed to use gtkdoc-mkpdf.")
        res = 1

    with open('pdf.stamp', 'w') as h:
        h.write('timestamp')
    return res
