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
#include <cstdlib>
#include <string>
namespace vitis {
namespace ai {
template <typename T>
struct env_config_helper {
  static inline T from_string(const char* s);
};

template <typename T, typename env_name>
struct env_config {
  static T init() {
    const char* name = env_name::get_name();
    const char* defvalue = env_name::get_default_value();
    const char* p = getenv(name);
    const char* pstr = p != nullptr ? p : defvalue;
    const T value = env_config_helper<T>::from_string(pstr);
    return value;
  }
  static T value;
};
template <typename T, typename env_name>
T env_config<T, env_name>::value = env_config<T, env_name>::init();

template <>
inline int env_config_helper<int>::from_string(const char* s) {
  return std::stoi(std::string(s));
}
template <>
inline std::string env_config_helper<std::string>::from_string(const char* s) {
  return std::string(s);
}
}  // namespace ai
}  // namespace vitis

#define DEF_ENV_PARAM_2(param_name, defvalue1, type)                           \
  struct ENV_PARAM_##param_name                                                \
      : public ::vitis::ai::env_config<type, ENV_PARAM_##param_name> {         \
    static const char* get_name() { return #param_name; }                      \
    static const char* get_default_value() { return defvalue1; }               \
  };

#define ENV_PARAM(param_name) (ENV_PARAM_##param_name::value)

#define DEF_ENV_PARAM(param_name, defvalue1)                                   \
  DEF_ENV_PARAM_2(param_name, defvalue1, int)
