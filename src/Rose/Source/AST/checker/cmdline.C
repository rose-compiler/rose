
#include "sage3basic.h"

#include "Rose/Source/AST/Checker.h"
#include "Rose/Source/AST/cmdline.h"

namespace Rose { namespace Source { namespace AST { namespace cmdline {

__when_T<checker_t> checker;

void checker_t::exec(SgProject*) const {
  if (!on) return;

  unsigned issues = 0;
  for (auto mode: modes) {
    switch (mode) { // TODO
      case Mode::all:                      break;
      case Mode::integrity:                break;
      case Mode::integrity_edges:          break;
      case Mode::integrity_declarations:   break;
      case Mode::integrity_symbols:        break;
      case Mode::integrity_types:          break;
      case Mode::consistency:              break;
      default:                ROSE_ABORT();
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
  if (str == "all") {
    return Mode::all;
  } else if (str == "integrity") {
    return Mode::integrity;
  } else if (str == "integrity_edges") {
    return Mode::integrity_edges;
  } else if (str == "integrity_declarations") {
    return Mode::integrity_declarations;
  } else if (str == "integrity_symbols") {
    return Mode::integrity_symbols;
  } else if (str == "integrity_types") {
    return Mode::integrity_types;
  } else if (str == "consistency") {
    return Mode::consistency;
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

} } } }

