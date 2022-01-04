#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unilog::unilog" for configuration "Release"
set_property(TARGET unilog::unilog APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(unilog::unilog PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libunilog.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libunilog.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS unilog::unilog )
list(APPEND _IMPORT_CHECK_FILES_FOR_unilog::unilog "${_IMPORT_PREFIX}/lib/libunilog.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
