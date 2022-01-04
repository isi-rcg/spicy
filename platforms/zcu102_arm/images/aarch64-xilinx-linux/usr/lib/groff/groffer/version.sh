#! /bin/sh

# groffer - display groff files

# Source file position: <groff-source>/contrib/groffer/version.sh
# Installed position: <prefix>/lib/groff/groffer/version.sh

# Copyright (C) 2001-2018 Free Software Foundation, Inc.
# Written by Bernd Warken <groff-bernd.warken-72@web.de>.

# This file is part of 'groffer', which is part of 'groff'.

# 'groff' is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.

# 'groff' is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

########################################################################

export _PROGRAM_VERSION;
_PROGRAM_VERSION='2.3.0';

# this setting of the groff version is only used before make is run,
# otherwise @VERSION@ will set it, see groffer.sh.
export _GROFF_VERSION_PRESET;
_GROFF_VERSION_PRESET='1.22.4';
