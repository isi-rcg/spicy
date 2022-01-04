#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vart::util" for configuration "Release"
set_property(TARGET vart::util APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vart::util PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libvart-util.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libvart-util.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS vart::util )
list(APPEND _IMPORT_CHECK_FILES_FOR_vart::util "${_IMPORT_PREFIX}/lib/libvart-util.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
