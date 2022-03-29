
#ifndef __ROSE_AST_CMDLINE_H__
#define __ROSE_AST_CMDLINE_H__

#include <string>
#include <vector>

class SgProject;

namespace Rose {
namespace AST {

  template <typename T>
  struct __when_T { T frontend; T backend; };

  struct graphviz_t {
    enum class Mode { tree,graph,unknown };

    bool on{false};
    Mode mode{Mode::tree};
    std::string out;

    void exec(SgProject * project) const;
    static Mode __mode(std::string const &);
  };
  extern __when_T<graphviz_t> graphviz;

  struct checker_t {
    enum class Mode { ll,ast,unknown };
    enum class Effect { none,summary,report,fail,unknown };

    bool on{false};
    std::vector<Mode> modes{Mode::ll, Mode::ast};
    Effect effect{Effect::none};
    std::string log;
    std::string save;

    void exec(SgProject * project) const;
    static Mode __mode(std::string const &);
    static Effect __effect(std::string const &);
  };
  extern __when_T<checker_t> checker;

}
}

#endif /* __ROSE_AST_CMDLINE_H__ */
