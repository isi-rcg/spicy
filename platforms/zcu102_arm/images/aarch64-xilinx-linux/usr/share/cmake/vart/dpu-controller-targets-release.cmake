#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vart::dpu-controller" for configuration "Release"
set_property(TARGET vart::dpu-controller APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vart::dpu-controller PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "glog::glog;vart::buffer-object;vart::xrt-device-handle;XRT::XRT"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libvart-dpu-controller.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libvart-dpu-controller.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS vart::dpu-controller )
list(APPEND _IMPORT_CHECK_FILES_FOR_vart::dpu-controller "${_IMPORT_PREFIX}/lib/libvart-dpu-controller.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
