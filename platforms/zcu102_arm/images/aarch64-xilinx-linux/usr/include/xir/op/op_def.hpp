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

#include <algorithm>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <vector>

#include "xir/attrs/attr_def.hpp"
#include "xir/util/any.hpp"

#define XIR_OP_ARG(NAME, OCCUR, DATE_TYPE, ANNOTATION)               \
  xir::OpArgDef {                                                    \
    NAME, xir::OpArgDef::OCCUR, xir::DataType::DATE_TYPE, ANNOTATION \
  }

#define XIR_MAKE_VEC(...)                                                      \
  { __VA_ARGS__ }

#define XIR_OP(NAME, INPUT_ARG, ATTR, INFER_SHAPE, ANNOTATION)                 \
  xir::OpDef { NAME, INPUT_ARG, ATTR, INFER_SHAPE, ANNOTATION }

#define XIR_REGISTER_OPS(...)                                                  \
  extern "C" void register_ops(xir::OpDefFactory* self) {                      \
    auto ops = std::vector<xir::OpDef>{__VA_ARGS__};                           \
    std::for_each(ops.begin(), ops.end(),                                      \
                  [self](const xir::OpDef& def) { self->register_h(def); });   \
  }

namespace xir {

/*
 *@struct OpArgDef
 *@brief Op argument definition
 *This struct defines an input argument of an op.
 */
struct OpArgDef {
  /**
   * @brief Element Occurence Specifier
   */
  enum OccurenceType {
    /// Once and only once
    REQUIRED,
    /// Never or once
    OPTIONAL,
    /// No limitation
    REPEATED,
    /// At least once
    REQUIRED_AND_REPEATED,
    NUM
  };

  /// Name of the op argument
  const std::string name;
  /// Occurence type
  const OccurenceType occur_type;
  /// DataType
  const xir::DataType data_type;
  /// Some comments
  const std::string annotation;
};

/*
 *@struct OpDef
 *@brief Op definition
 *This struct defines an op, like conv2d
 */
struct OpDef {
  /// Name of the op
  const std::string name;
  /// Input arguments
  const std::vector<OpArgDef> input_args;
  /// Op attributes
  const std::vector<AttrDef> attrs;
  /// Shape-infer functions
  std::function<void(Op* op)> shape_infer;
  /// Some comments
  const std::string annotation;
};

class OpDefFactory {
 public:
  virtual void register_h(const OpDef& def) = 0;

 public:
  virtual ~OpDefFactory() = default;
};

}  // namespace xir
