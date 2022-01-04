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
import os
import sys
import subprocess
import shutil
from glob import glob

from . import config


def run_xsltproc(options, args):
    command = [config.xsltproc]
    # we could do "$path_option $PWD " to avoid needing rewriting entities that
    # are copied from the header into docs under xml
    if os.environ.get("GTKDOC_PROFILE", '') == '':
        if len(options.path):
            command += ['--path', ':'.join(options.path)]
        logging.info('running "%s"', ' '.join(command + args))
        return subprocess.call(command + args)
    else:
        command += ['--profile']
        if len(options.path):
            command += ['--path', ':'.join(options.path)]
        logging.info('running "%s"', ' '.join(command + args))
        return subprocess.call(command + args, stderr=open('profile.txt', 'w'))


def get_dirs(uninstalled):
    if uninstalled:
        # this does not work from buiddir!=srcdir
        gtkdocdir = os.path.split(sys.argv[0])[0]
        if not os.path.exists(gtkdocdir + '/gtk-doc.xsl'):
            # try 'srcdir' (set from makefiles) too
            if os.path.exists(os.environ.get("ABS_TOP_SRCDIR", '') + '/gtk-doc.xsl'):
                gtkdocdir = os.environ['ABS_TOP_SRCDIR']
        styledir = gtkdocdir + '/style'
    else:
        gtkdocdir = os.path.join(config.datadir, 'gtk-doc/data')
        styledir = gtkdocdir
    return (gtkdocdir, styledir)


def run(options):
    logging.info('options: %s', str(options.__dict__))

    module = options.args[0]
    document = options.args[1]
    if options.verbose:
        quiet = '0'
    else:
        quiet = '1'
    remaining_args = options.args[2:]

    (gtkdocdir, styledir) = get_dirs(options.uninstalled)

    res = run_xsltproc(options, [
        '--nonet',
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
        quiet] + remaining_args + [gtkdocdir + '/gtk-doc.xsl', document])

    # profiling
    if os.environ.get("GTKDOC_PROFILE", '') != '':
        subprocess.check_call('cat profile.txt | gprof2dot.py -e 0.01 -n 0.01 | dot -Tpng -o profile.png', shell=True)

    # copy navigation images and stylesheets to html directory ...
    for f in glob(styledir + '/*.png') + glob(styledir + '/*.css'):
        shutil.copy(f, '.')

    with open('../html.stamp', 'w') as h:
        h.write('timestamp')
    return res
