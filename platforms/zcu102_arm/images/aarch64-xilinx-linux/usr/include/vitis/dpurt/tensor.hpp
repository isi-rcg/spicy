/*
 * Copyright 2019 Xilinx Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <cstdint>
// this struct is defined by elf format, try to be compatible
namespace vitis {
namespace dpurt {
struct tensor {
  uint32_t attr;
  uint32_t height;
  uint32_t width;
  uint32_t channel;
  uint32_t addr_logical;
  uint32_t size;
  uint32_t fix_width;
  int32_t fix_pos;
  uint32_t channel_stride;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
  uint32_t reserved3;
};
}  // namespace dpurt
}  // namespace vitis
