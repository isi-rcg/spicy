#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "target-factory::target-factory" for configuration "Release"
set_property(TARGET target-factory::target-factory APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(target-factory::target-factory PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtarget-factory.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libtarget-factory.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS target-factory::target-factory )
list(APPEND _IMPORT_CHECK_FILES_FOR_target-factory::target-factory "${_IMPORT_PREFIX}/lib/libtarget-factory.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
