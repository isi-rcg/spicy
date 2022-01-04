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

#include "./tensor.hpp"
#include "vitis/ai/weak.hpp"
#ifdef __QNX__
#include <sys/elf.h>
#else
#include <elf.h>
#endif
#include <glog/logging.h>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
namespace vitis {
namespace dpurt {

class buffer_object_fd {
 public:
  static std::shared_ptr<buffer_object_fd> create(const std::string& name,
                                                  int flags);

 public:
  explicit buffer_object_fd(const std::string& name, int flags);
  buffer_object_fd(const buffer_object_fd&) = delete;
  buffer_object_fd& operator=(const buffer_object_fd& other) = delete;
  virtual ~buffer_object_fd();

  int fd() { return fd_; }

 private:
  int fd_;
};

#define ELF_PHDR(name)                                                         \
  (is_class32() ? get<Elf32_Ehdr>(0)->name : get<Elf64_Ehdr>(0)->name)

#define ELF_SHDR(i, name)                                                      \
  (is_class32() ? get<Elf32_Shdr>(shoff())[i].name                             \
                : get<Elf64_Shdr>(shoff())[i].name)

class Elf {
 public:
  static std::unique_ptr<Elf> create(const std::string& filename,
                                     const std::string& kernel_name);

  static std::map<std::string, std::unique_ptr<Elf>> create(
      const std::string& filename);

 public:
  Elf(const std::string& filename, const std::string& kernel_name);
  ~Elf();

 public:
  std::string Show();
  size_t CodeSize();
  int8_t* Code();
  size_t InitialCodeSize();
  int8_t* InitialCode();
  size_t ParameterSize();
  int8_t* Parameter();
  int8_t* Symtab();
  struct node;
  const std::vector<node*>& Nodes() const { return nodes_; }
  node* FindNodeByName(const char* name);
  tensor* FindInputTensorByName(const char* name, int idx);
  tensor* FindOutputTensorByName(const char* name, int idx);
  const std::vector<std::string>& kernels() const { return kernels_; }

 public:
  struct metadata {
    uint32_t dpu_arch;           //
    uint32_t ver_dnnc;           // =  1;
    uint32_t mode;               // =  2;
    uint32_t node_cnt;           // =  3;
    uint32_t tensor_size;        // =  4;
    uint32_t kernel_io_size;     // =  5;
    uint32_t kernel_mean_c1;     // =  6;
    uint32_t kernel_mean_c2;     // =  7;
    uint32_t kernel_mean_c3;     // =  8;
    uint16_t abi_ver_minor;      // = // LSB(0~15): minor verion
    uint16_t abi_ver_major;      // = // MSB(16~31): major version
    uint16_t dpu_ver_target;     // = 10;  // LSB(0~15): dpu target
    uint16_t dpu_ver_arch_type;  // = 10;  // MSB(16~31): dpu arch type
    uint32_t tensor_cnt;         // =  11;
  };
  template <typename T>
  struct list {
    uint32_t cnt;
    T data[];
    template <typename NextType>
    NextType* next() {
      return reinterpret_cast<NextType*>(reinterpret_cast<char*>(this) +
                                         cnt * sizeof(T) + sizeof(cnt));
    }
  };
  struct node {
    uint32_t type;
    uint32_t name;
    uint64_t workload;
    struct reg {
      uint16_t reg_id;
      uint16_t data_type;
    };
    uint32_t* marker[];
    list<reg>* regs() { return reinterpret_cast<list<reg>*>(&marker[0]); };
    list<uint32_t>* inputs() { return regs()->next<list<uint32_t>>(); }
    list<uint32_t>* outputs() { return inputs()->next<list<uint32_t>>(); };
    struct code {
      uint32_t offset;
      uint32_t size;
      uint32_t name_idx;
      uint32_t align;
    };
    list<code>* codes() { return outputs()->next<list<code>>(); };
    struct param {
      uint32_t offset;
      uint32_t size;
      uint32_t fix_w;
      uint32_t fix_p;
      uint32_t name_idx;
      uint32_t align;
      uint32_t height;
      uint32_t width;
      uint32_t channel;
      uint32_t out_channel;
    };
    list<param>* param() {  //
      return codes()->next<list<struct param>>();
    }
    list<uint32_t>* pre_nodes() { return param()->next<list<uint32_t>>(); }
    list<uint32_t>* suc_nodes() { return pre_nodes()->next<list<uint32_t>>(); };
  };
  // for internal use, but still public
  template <typename T>
  T* get(size_t offset) {
    return reinterpret_cast<T*>(static_cast<char*>(data_) + offset);
  }
  bool is_class32() {
    return get<Elf32_Ehdr>(0)->e_ident[EI_CLASS] == ELFCLASS32;
  }
  bool is_class64() {
    return get<Elf32_Ehdr>(0)->e_ident[EI_CLASS] == ELFCLASS64;
  }

  size_t shstrndx() { return ELF_PHDR(e_shstrndx); }
  size_t shoff() { return ELF_PHDR(e_shoff); }

  size_t shnum() { return ELF_PHDR(e_shnum); }

  std::string deephi_string(size_t offset) {
    return std::string(
        get<char>(section_offset(section_deephi_strtab_) + offset));
  }

  std::string strtab_string(size_t offset) {
    return std::string(get<char>(section_offset(section_strtab_) + offset));
  }

  std::string section_name(int section_idx) {
    return std::string(get<char>(ELF_SHDR(shstrndx(), sh_offset) +
                                 ELF_SHDR(section_idx, sh_name)));
  }

  size_t section_offset(int section_idx) {
    return ELF_SHDR(section_idx, sh_offset);
  }

  size_t section_size(int section_idx) {
    return ELF_SHDR(section_idx, sh_size);
  }

  size_t section_link(int section_idx) {
    return ELF_SHDR(section_idx, sh_link);
  }

  size_t section_entry_size(int section_idx) {
    return ELF_SHDR(section_idx, sh_entsize);
  }

  metadata* get_metadata() {
    return get<metadata>(section_offset(section_metadata_));
  }

  node* get_node() {  //
    return get<node>(section_offset(section_node_));
  }
  std::string get_file_name() { return filename_; }
  std::string get_kernel_name() { return kernel_; }
  std::string dump_meta_data();
  std::string dump_tensor(tensor* t);
  void parse_sections();
  void parse_section(size_t i);
  std::unique_ptr<std::pair<uint32_t, uint32_t>> get_preload_offset_and_size(
      const std::string& layer);
  size_t get_num_of_tensors() {  //
    return section_size(section_tensor_) / sizeof(tensor);
  }
  tensor* get_tensor(int i) {
    return get<tensor>(section_offset(section_tensor_) + i * sizeof(tensor));
  }
  void parse_nodes();
  void parse_symtab();
  std::string dump_nodes();
  std::string dump_node(node* n);

 private:
  std::string filename_;
  std::shared_ptr<buffer_object_fd> fd_;
  size_t size_;
  void* data_;
  std::string kernel_;
  uint32_t section_metadata_;
  uint32_t section_strtab_;
  uint32_t section_deephi_strtab_;
  uint32_t section_node_;
  uint32_t section_parameter_;
  uint32_t section_code_;
  uint32_t section_initial_code_;
  uint32_t section_tensor_;
  uint32_t section_symtab_;
  std::vector<node*> nodes_;
  std::vector<std::string> kernels_;
};

class ElfSectionBuilder;
struct ElfBuilder {
  ElfBuilder(std::ostream& out) : out_{out} {};
  void build();
  ElfSectionBuilder* new_section(const std::string& name);

 private:
  ElfSectionBuilder* get_section(const std::string& name);
  ElfSectionBuilder* get_or_new_section(const std::string& name);
  void prepare_build();
  void build_header();
  void build_section_headers();
  void build_section_header(ElfSectionBuilder* s);
  void build_sections();
  void build_section(ElfSectionBuilder* s);
  uint32_t total_section_size();

 private:
  std::ostream& out_;
  std::vector<std::unique_ptr<ElfSectionBuilder>> sections_;
};

struct ElfSectionBuilder {
 public:
  ElfSectionBuilder(const std::string& name, uint32_t id);
  uint32_t allocate_string(const std::string& str);
  void allocate_section_name(ElfSectionBuilder* str);
  uint32_t allocate_section_space(uint32_t pos);
  const std::string& get_name() const { return name_; }
  uint32_t get_id() const { return id_; }
  std::string data() const { return out_.str(); }
  size_t size() { return out_.tellp(); }
  uint32_t get_section_name();
  uint32_t offset();
  void align();
  void set_type(uint32_t type) { type_ = type; }
  uint32_t get_type() const { return type_; }
  template <typename X>
  void write(const X& x) {
    CHECK(out_.write(reinterpret_cast<const char*>(&x), sizeof(x)).good());
  }
  void write(const std::vector<char>& x) {
    CHECK(out_.write(&x[0], x.size()).good());
  }
  void write(const std::string& x) {
    CHECK(out_.write(&x[0], x.size()).good());
  }

 private:
  std::string name_;
  uint32_t id_;
  std::ostringstream out_;
  uint32_t section_name_;
  uint32_t offset_;
  uint32_t type_;
};

struct DnncKernel {
 public:
  DnncKernel(const std::string& name, ElfBuilder* elf);

 public:
  void set_kernel_iosize(size_t size) { kernel_io_size_ = size; }
  void set_num_of_nodes(size_t size) { num_of_nodes_ = size; }
  void build_meta_section();
  void build_code_section(const std::vector<char>& code);
  void build_parameter_section(const std::vector<char>& parameter);
  void build_node_section();
  void build_tensor_section();
  void add_tensor(const tensor& tensor) { tensors_.emplace_back(tensor); };
  void add_node(const std::function<void(std::ostream&)>& builder);
  uint32_t deephi_allocate_string(const std::string& str) {
    return strtab_->allocate_string(str);
  }
  uint32_t find_tensor_by_ddr_addr(uint32_t ddr_addr);

 private:
  ElfSectionBuilder* strtab_;
  ElfSectionBuilder* metadata_;
  ElfSectionBuilder* configurable_;
  ElfSectionBuilder* tensor_;
  ElfSectionBuilder* node_;
  ElfSectionBuilder* parameter_;
  ElfSectionBuilder* code_;
  std::string name_;
  size_t kernel_io_size_ = 0u;
  std::vector<tensor> tensors_ = {};
  size_t num_of_nodes_ = 0u;
};
}  // namespace dpurt
}  // namespace vitis

// Local Variables:
// mode:c++ddd
// c-basic-offset: 2
// coding: utf-8-unix
// End:
