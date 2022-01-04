#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "xir::xir" for configuration "Release"
set_property(TARGET xir::xir APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xir::xir PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libxir.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libxir.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS xir::xir )
list(APPEND _IMPORT_CHECK_FILES_FOR_xir::xir "${_IMPORT_PREFIX}/lib/libxir.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
