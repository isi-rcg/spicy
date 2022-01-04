#
# Copyright 2019 Xilinx Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
set(_supported_components util;runner;mem-manager;xrt-device-handle;buffer-object;dpu-controller;elf-util;dpu-runner)
set(vart_FOUND True)

foreach(_comp ${vart_FIND_COMPONENTS})
  if (NOT ";${_supported_components};" MATCHES ${_comp})
    set(vart_FOUND False)
    set(vart_NOT_FOUND_MESSAGE "Unsupported component: ${_comp}")
  endif()
  include("${CMAKE_CURRENT_LIST_DIR}/${_comp}-targets.cmake")
  get_target_property(TARGET_LOCATION vart::${_comp} LOCATION)
  message(STATUS "Found importable target vart::${_comp}: ${TARGET_LOCATION}")
endforeach()

get_filename_component(vart_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(vart_INCLUDE_DIRS "${vart_CMAKE_DIR}/../../../include")
