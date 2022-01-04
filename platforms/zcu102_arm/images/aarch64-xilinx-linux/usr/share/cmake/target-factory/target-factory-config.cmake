include("${CMAKE_CURRENT_LIST_DIR}/target-factory-targets.cmake")
set(target-factory_FOUND True)

get_target_property(TARGET_LOCATION target-factory::target-factory LOCATION)
message(STATUS "Found importable target target-factory::target-factory: ${TARGET_LOCATION}")

get_filename_component(target-factory_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(target-factory_INCLUDE_DIRS "${target-factory_CMAKE_DIR}/../../../include")
