#ifndef VECTOR_COMMAND_OPTIONS_H
#define VECTOR_COMMAND_OPTIONS_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

#include "rosedll.h"

class VectorCmdOptions
{
  std::vector<std::string>* cmd;
  static VectorCmdOptions *inst;
 public:
  VectorCmdOptions() : cmd(NULL) {}
  void SetOptions  (std::vector<std::string>& options) {
    cmd = &options;
  }
  const std::vector<std::string>& GetOptions() const {
    assert (cmd);
    return *cmd;
  }
  size_t HasOption( const std::string& opt) const;  // Returns one-based index or zero for not found
  ROSE_UTIL_API static VectorCmdOptions* GetInstance();
};

#endif
