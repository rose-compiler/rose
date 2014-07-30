
#ifndef __DLX_TOOLING_COMPILER_HPP__
#define __DLX_TOOLING_COMPILER_HPP__

class SgProject;

namespace DLX {

namespace Tooling {

struct compiler_modules_t {
  SgProject * project;
  compiler_modules_t(SgProject * project_);
};

}

}

#endif /* __DLX_TOOLING_COMPILER_HPP__ */

