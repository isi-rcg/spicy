include("${CMAKE_CURRENT_LIST_DIR}/unilog-targets.cmake")
set(unilog_FOUND True)

get_target_property(TARGET_LOCATION unilog::unilog LOCATION)
message(STATUS "Found importable target unilog::unilog: ${TARGET_LOCATION}")

get_filename_component(unilog_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(unilog_INCLUDE_DIRS "${unilog_CMAKE_DIR}/../../../include")

find_package(glog REQUIRED)
