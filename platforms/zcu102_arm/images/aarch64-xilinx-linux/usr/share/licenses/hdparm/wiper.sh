# Copyright (C) 2009-2010 Mark Lord.  All rights reserved.
#
# Contains hfsplus and ntfs code contributed by Heiko Wegeler <heiko.wegeler@googlemail.com>.
# Package sleuthkit version >=3.1.1 is required for HFS+. Package ntfs-3g and ntfsprogs is required for NTFS.
#
# Requires gawk, a really-recent hdparm, and various other programs.
# This needs to be redone entirely in C, for 64-bit math, someday.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License Version 2,
# as published by the Free Software Foundation.
# 
# This program is distributed in the hope that it would be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Note for OCZ Vertex-LE users:  the drive firmware will error when
# attempting to trim the final sector of the drive.  To avoid this,
# partition the drive such that the final sector is not used.

