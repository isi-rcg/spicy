include("${CMAKE_CURRENT_LIST_DIR}/xir-targets.cmake")
set(xir_FOUND True)

get_target_property(TARGET_LOCATION xir::xir LOCATION)
message(STATUS "Found importable target xir::xir: ${TARGET_LOCATION}")

get_filename_component(xir_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(xir_INCLUDE_DIRS "${xir_CMAKE_DIR}/../../../include")
