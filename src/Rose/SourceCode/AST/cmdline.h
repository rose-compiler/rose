#ifndef ROSE_AST_CMDLINE_H
#define ROSE_AST_CMDLINE_H
#include <RoseFirst.h>

#include <string>
#include <vector>

class SgProject;

namespace Rose { namespace SourceCode { namespace AST {

//! Controls for command-line options that uses function the Rose::AST namespace
namespace cmdline {

  //! A simple template to facilitate configuration of modules that can be run after the frontend or before the backend.
  template <typename T>
  struct __when_T {
    T frontend; //!< Configuration to run after the frontend
    T backend;  //!< Configuration to run before the backend
  };

  //! Configure the AST visualization module
  struct graphviz_t {
    //! Two visualization modes
    enum class Mode {
      tree,   //!< the Abstract Syntax *Tree*, only contains nodes that directly correspond to code
      graph,  //!< the whole graph contains the AST and all other nodes, such as types and symbols
      unknown
    };

    bool on{false};
    Mode mode{Mode::tree}; //!< only a single mode, defaults to tree mode
    std::string out; //!< name of the output file

    void exec(SgProject * project) const;
    static Mode __mode(std::string const &);
  };
  extern __when_T<graphviz_t> graphviz; //!< Used by the -rose:ast:graphviz:XXX options

  //! Configure the AST checker module
  struct checker_t {
    //! Many different modes are available
    enum class Mode {
      all,                        //!< run all checkers
        integrity,                //!< run all integrity checkers
          integrity_edges,        //!< checks the edges of the AST for invalid pointers/nodes
          integrity_declarations, //!< checks each of the decalration subgraphs (1st-nondef / defn)
          integrity_symbols,      //!< checks the symbols subgraph
          integrity_types,        //!< checks the types subgraph
        consistency,              //!< run all consistency checkers
      unknown
    };
    //! One can select the effect if any defect is detected
    enum class Effect {
      none,    //!< NOP
      summary, //!< Shows a summary on the standard output
      report,  //!< Shows a full report of all defects on the standard output
      fail,    //!< Calls ROSE_ABORT (after save/log if any)
      unknown
    };

    bool on{false};
    std::vector<Mode> modes{Mode::integrity}; // !< User can run multiple checkers of their choice
    Effect effect{Effect::none}; 
    std::string log;  //!< Filepath to write out the log (equivalent to report)
    std::string save; //!< Filepath (without extension) where to save both the AST and a JSON dump of the defects

    void exec(SgProject * project) const;
    static Mode __mode(std::string const &);
    static Effect __effect(std::string const &);
  };
  extern __when_T<checker_t> checker; //!< Used by the -rose:ast:checker:XXX options

} } } }
#endif /* ROSE_AST_CMDLINE_H */
