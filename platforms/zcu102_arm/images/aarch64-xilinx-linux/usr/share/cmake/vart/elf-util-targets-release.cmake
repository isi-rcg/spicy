#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vart::elf-util" for configuration "Release"
set_property(TARGET vart::elf-util APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vart::elf-util PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "glog::glog"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libvart-elf-util.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libvart-elf-util.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS vart::elf-util )
list(APPEND _IMPORT_CHECK_FILES_FOR_vart::elf-util "${_IMPORT_PREFIX}/lib/libvart-elf-util.so.1.2.0" )

# Import target "vart::dpu_model_inspect" for configuration "Release"
set_property(TARGET vart::dpu_model_inspect APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vart::dpu_model_inspect PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/dpu_model_inspect"
  )

list(APPEND _IMPORT_CHECK_TARGETS vart::dpu_model_inspect )
list(APPEND _IMPORT_CHECK_FILES_FOR_vart::dpu_model_inspect "${_IMPORT_PREFIX}/bin/dpu_model_inspect" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
