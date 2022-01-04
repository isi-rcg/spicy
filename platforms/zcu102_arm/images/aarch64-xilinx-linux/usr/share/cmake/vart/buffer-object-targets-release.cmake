#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vart::buffer-object" for configuration "Release"
set_property(TARGET vart::buffer-object APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vart::buffer-object PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "glog::glog;vart::xrt-device-handle"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libvart-buffer-object.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libvart-buffer-object.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS vart::buffer-object )
list(APPEND _IMPORT_CHECK_FILES_FOR_vart::buffer-object "${_IMPORT_PREFIX}/lib/libvart-buffer-object.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
