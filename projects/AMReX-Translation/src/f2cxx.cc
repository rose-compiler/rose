#include <iostream>
#include <algorithm>
//~ #include <iterator>
//~ #include <string>
//~ #include <boost/filesystem.hpp>

#include <boost/algorithm/string.hpp>

#include "rose.h"
#include "sageBuilder.h"
#include "sageGeneric.h"

#include "f2dot.hpp"
#include "f2cxx_analyzer.hpp"
#include "f2cxx_translator.hpp"
#include "f2cxx_convenience.hpp"

namespace sb = SageBuilder;
namespace si = SageInterface;

static
bool hasFortranExtension(std::string filename)
{
  return boost::ends_with(filename, std::string(".f03"))
      || boost::ends_with(filename, std::string(".f95"))
      || boost::ends_with(filename, std::string(".f90"));
}

static
bool hasCppExtension(std::string filename)
{
  return boost::ends_with(filename, std::string(".C"))
      || boost::ends_with(filename, std::string(".cpp"))
      || boost::ends_with(filename, std::string(".cxx"))
      || boost::ends_with(filename, std::string(".cc"));
}



namespace f2cxx
{
  static const bool first_of = false;

  template <class P>
  static
  bool set_file_if(P property_tester, SgSourceFile*& tgt, SgSourceFile& val)
  {
    if (!property_tester(val.getFileName()))
      return false;

    ROSE_ASSERT(tgt == NULL);
    tgt = &val;
    return true;
  }

  template <class Derived>
  struct TraversalBase
  {
    static
    void _descend(SgNode& n, Derived& self)
    {
      self = sg::traverseChildren(self, n);
    }

    void descend(SgNode& n) { _descend(n, static_cast<Derived&>(*this)); }
  };

  /// \brief Gathers files in a project.
  /// \details
  ///   stops AST traversal at the level of SgSourceFile.
  /// \todo
  ///   consider making the tests generic
  struct FileGatherer : TraversalBase<FileGatherer>
  {
    typedef TraversalBase<FileGatherer> base;

    FileGatherer()
    : base(), fxx_file(NULL), cpp_file(NULL)
    {}

    void handle(SgNode& n)     { sg::unexpected_node(n); }

    void handle(SgFileList& n) { descend(n); }

    void handle(SgSourceFile& n)
    {
      first_of
      || set_file_if(hasFortranExtension, fxx_file, n)
      || set_file_if(hasCppExtension,     cpp_file, n);
    }

    SgSourceFile& fortran() const { return sg::deref(fxx_file); }
    SgSourceFile& cpp() const     { return sg::deref(cpp_file); }

    SgSourceFile* fxx_file;
    SgSourceFile* cpp_file;
  };


  /// gathers all top level procedures
  struct ProcGatherer : TraversalBase<ProcGatherer>
  {
    void handle(SgNode& n)                 { sg::unexpected_node(n); }

    void handle(SgSourceFile& n)           { descend(n); }
    void handle(SgGlobal& n)               { descend(n); }
    void handle(SgModuleStatement& n)      { descend(n); }
    void handle(SgClassDefinition& n)      { descend(n); }
    void handle(SgDeclarationStatement& n) { }

    void handle(SgProcedureHeaderStatement& n)
    {
      procedures.push_back(&n);
    }

    std::vector<SgProcedureHeaderStatement*>&
    procs()
    {
      return procedures;
    }

    std::vector<SgProcedureHeaderStatement*> procedures;
  };
}

// can be in principle be executed in parallel
template <class Seq, class Analyzer>
Analyzer par_for_each(Analyzer prototype, Seq& s)
{
  // default is sequential :)
  return std::for_each(s.begin(), s.end(), prototype);
}



int main(int argc, char* argv[])
{
  typedef Rose_STL_Container<std::string> argument_container;

  static constexpr const bool generate_dot_file = true;

  ROSE_ASSERT(argc > 0);

  argument_container  arguments(argv, argv + argc);
  SgProject*          p     = frontend(arguments);
  f2cxx::FileGatherer files = sg::traverseChildren(f2cxx::FileGatherer(), p);
  SgSourceFile&       inp   = files.fortran();
  SgSourceFile&       out   = files.cpp();

  // query Fortran procedures
  f2cxx::ProcGatherer procs    = sg::traverseChildren(f2cxx::ProcGatherer(), inp);

  // analyze procs and their parameters
  par_for_each(f2cxx::Analyzer(), procs.procs());

  // print out analysis for debugging purposes
  std::for_each(procs.procs().begin(), procs.procs().end(), f2cxx::print_param_tags);

  // create C++ top-level declarations; to preserve order, we do this sequentially
  f2cxx::DeclMaker    declmkr(out);

  declmkr = std::for_each(procs.procs().begin(), procs.procs().end(), declmkr);

  // Transform all procedures
  par_for_each(f2cxx::Translator(out), declmkr.decls());

  if (generate_dot_file)
  {
    std::cerr << "dot begin" << std::endl;
    f2cxx::save_dot("fxx.dot", inp, f2cxx::simple);
    f2cxx::save_dot("cxx.dot", out, f2cxx::balanced);
    std::cerr << "dot end." << std::endl;
    //~ exit(0);
  }

  backend(p);
  return 0;
}


  /*
  struct FunctionTranslator
  {

    explicit
    FunctionTranslator(SgSourceFile* cpp_file)
    : cxxsrc(cpp_file), ctx()
    {
      ROSE_ASSERT(cxxsrc);
    }

    mapping::iterator begin() { return ctx.begin(); }
    mapping::iterator end()   { return ctx.end();   }

    SgSourceFile* cxxsrc;
    mapping       ctx;
  };
  */
