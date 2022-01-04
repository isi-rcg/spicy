# Internal -- for use with UseGtkDoc.cmake
#
#=============================================================================
# Copyright 2009 Rich Wareham
# Copyright 2015 Lautsprecher Teufel GmbH
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#=============================================================================

# This is needed for find_package(PkgConfig) to work correctly --
# CMAKE_MINIMUM_REQUIRED_VERSION needs to be defined.
cmake_minimum_required(VERSION 3.2)

if(NOT APPLE)
    # We use pkg-config to find glib et al
    find_package(PkgConfig)
    # Find glib et al
    pkg_check_modules(GLIB REQUIRED glib-2.0 gobject-2.0)

foreach(_flag ${EXTRA_CFLAGS} ${GLIB_CFLAGS})
    set(ENV{CFLAGS} "$ENV{CFLAGS} '${_flag}'")
endforeach(_flag)

foreach(_flag ${EXTRA_LDFLAGS} ${GLIB_LDFLAGS})
    set(ENV{LDFLAGS} "$ENV{LDFLAGS} '${_flag}'")
endforeach(_flag)

foreach(_flag ${EXTRA_LDPATH})
    if(DEFINED ENV{LD_LIBRARY_PATH})
        set(ENV{LD_LIBRARY_PATH} "$ENV{LD_LIBRARY_PATH}:${_flag}")
    else(DEFINED ENV{LD_LIBRARY_PATH})
        set(ENV{LD_LIBRARY_PATH} "${_flag}")
    endif(DEFINED ENV{LD_LIBRARY_PATH})
endforeach(_flag)

message(STATUS "Executing gtkdoc-scangobj with:")
message(STATUS "   CFLAGS: $ENV{CFLAGS}")
message(STATUS "  LDFLAGS: $ENV{LDFLAGS}")
message(STATUS "   LDPATH: $ENV{LD_LIBRARY_PATH}")

execute_process(COMMAND ${GTKDOC_SCANGOBJ_EXE}
    "--module=${doc_prefix}"
    "--types=${output_types}"
    "--output-dir=${output_dir}"
    ${_scanobjopts}
    WORKING_DIRECTORY "${output_dir}"
    RESULT_VARIABLE _scan_result)

if(_scan_result EQUAL 0)
    message(STATUS "Scan succeeded.")
else(_scan_result EQUAL 0)
    message(SEND_ERROR "Scan failed.")
endif(_scan_result EQUAL 0)

endif(NOT APPLE)

# vim:sw=4:ts=4:et:autoindent
