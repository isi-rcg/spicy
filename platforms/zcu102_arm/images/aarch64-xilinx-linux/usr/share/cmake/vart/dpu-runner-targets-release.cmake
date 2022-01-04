#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vart::dpu-runner" for configuration "Release"
set_property(TARGET vart::dpu-runner APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vart::dpu-runner PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vart::elf-util;XRT::XRT;vart::dpu-controller"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libvart-dpu-runner.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libvart-dpu-runner.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS vart::dpu-runner )
list(APPEND _IMPORT_CHECK_FILES_FOR_vart::dpu-runner "${_IMPORT_PREFIX}/lib/libvart-dpu-runner.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
