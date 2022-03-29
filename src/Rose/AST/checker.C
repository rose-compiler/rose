
#include "sage3basic.h"

#include "Rose/AST/checker.h"
#include "Rose/AST/cmdline.h"

namespace Rose {
namespace AST {

__when_T<checker_t> checker;

void checker_t::exec(SgProject * project) const {
  if (!on) return;

  unsigned issues = 0;
  for (auto mode: modes) {
    switch (mode) {
      case Mode::ll: {
        // TODO
        break;
      }
      case Mode::ast: {
        // TODO
        break;
      }
      default:
        ROSE_ABORT();
    }
  }
  if (log.size() > 0) {
    // TODO
  }
  if (save.size() > 0) {
    // TODO
  }
  if (issues > 0) {
    switch (effect) {
      case Effect::none: break;
      case Effect::summary: {
        // TODO
        break;
      }
      case Effect::report: {
        // TODO
        break;
      }
      case Effect::fail: {
        // TODO
        break;
      }
      default:
        ROSE_ABORT();
    }
  }
}
checker_t::Mode checker_t::__mode(std::string const & str) {
  if (str == "ll") {
    return Mode::ll;
  } else if (str == "ast") {
    return Mode::ast;
  } else {
    return Mode::unknown;
  }
}

checker_t::Effect checker_t::__effect(std::string const & str) {
  if (str == "none") {
    return Effect::none;
  } else if (str == "summary") {
    return Effect::summary;
  } else if (str == "report") {
    return Effect::report;
  } else if (str == "fail") {
    return Effect::fail;
  } else {
    return Effect::unknown;
  }
}

}
}

