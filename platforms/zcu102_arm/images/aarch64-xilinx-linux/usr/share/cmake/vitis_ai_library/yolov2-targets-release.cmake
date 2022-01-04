#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vitis_ai_library::yolov2" for configuration "Release"
set_property(TARGET vitis_ai_library::yolov2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vitis_ai_library::yolov2 PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libvitis_ai_library-yolov2.so.1.2.0"
  IMPORTED_SONAME_RELEASE "libvitis_ai_library-yolov2.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS vitis_ai_library::yolov2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_vitis_ai_library::yolov2 "${_IMPORT_PREFIX}/lib/libvitis_ai_library-yolov2.so.1.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
