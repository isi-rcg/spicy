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

#include "vart/tensor_buffer.hpp"

#include <xir/tensor/tensor.hpp>

namespace vart {
namespace mm {

class HostFlatTensorBuffer : public TensorBuffer {
 public:
  explicit HostFlatTensorBuffer(const xir::Tensor* tensor);
  explicit HostFlatTensorBuffer(const xir::Tensor* tensor,
                                std::vector<uint32_t> strides);
  virtual ~HostFlatTensorBuffer();

 public:
  virtual std::pair<uint64_t, size_t> data(
      const std::vector<int> idx = {}) override;

 public:
  const xir::DataType data_type_;
  const uint32_t data_size_;             // bit
  const std::vector<uint32_t> shape_;    // element
  const std::vector<uint32_t> strides_;  // bit
  const uint32_t last_continued_dim_;

 private:
  char* data_;
};

void init_from_file(HostFlatTensorBuffer* buffer, std::string file_name);
void dump_to_file(HostFlatTensorBuffer* buffer, std::string file_name);

std::pair<std::unique_ptr<HostFlatTensorBuffer>, std::unique_ptr<xir::Tensor>>
transform_to_fix_buffer(TensorBuffer* buffer, int32_t fix_point,
                        int32_t bit_width, bool if_signed,
                        std::string round_mode);

}  // namespace mm
}  // namespace vart
