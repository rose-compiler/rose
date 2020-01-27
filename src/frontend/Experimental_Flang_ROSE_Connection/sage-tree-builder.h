#ifndef ROSE_SAGE_TREE_BUILDER_H_
#define ROSE_SAGE_TREE_BUILDER_H_

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>

// WARNING: This file has been designed to compile with -std=c++17
// This limits the use of ROSE header files at the moment.
//
class SgDerivedTypeStatement;
class SgGlobal;
class SgLocatedNode;
class SgScopeStatement;
class SgSourceFile;
class SgProgramHeaderStatement;

// Jovial specific classes
class SgJovialCompoolStatement;
class SgJovialTableStatement;

namespace Rose {
namespace builder {

// This is similar to F18 Fortran::parser::SourcePosition
struct SourcePosition {
   std::string path;  // replaces Fortran::parser::SourceFile
   int line, column;
};

using SourcePositionPair = boost::tuple<SourcePosition, SourcePosition>;
using SourcePositions    = boost::tuple<SourcePosition, SourcePosition, SourcePosition>;

// The global must be initialized before using the SageTreeBuilder class
// Consider creating a constructor (may not work well with interface for F18/Flang)
SgGlobal* initialize_global_scope(SgSourceFile* file);

// Create a builder class that does nothing
class SageTreeBuilderNull {
public:
   // Default action for a sage tree node is to do nothing.
   template<typename T, typename... Options> void Enter(T* &, Options...) {}
   template<typename T>                      void Leave(T*) {}
};


class SageTreeBuilder {
public:

   // Default action for a sage tree node is to do nothing.
   template<typename T> void Enter(T* &) {}
   template<typename T> void Leave(T*)   {}

   void Leave(SgScopeStatement* &);

   void Enter(SgProgramHeaderStatement* &,
              const boost::optional<std::string> &, const std::list<std::string> &, const SourcePositions &);
   void Leave(SgProgramHeaderStatement*);

   void setFortranEndProgramStmt(SgProgramHeaderStatement*,
                                 const boost::optional<std::string> &,
                                 const boost::optional<std::string> &);

   void Enter(SgDerivedTypeStatement* &, const std::string &);
   void Leave(SgDerivedTypeStatement*);

// Jovial specific nodes
//
   void Enter(SgJovialCompoolStatement* &,
              const std::string &, const SourcePositionPair &);
   void Enter(SgJovialTableStatement* &,
              const std::string &, const SourcePositionPair &, bool is_block=false);
   void Leave(SgJovialTableStatement*);

private:

   void setSourcePosition(SgLocatedNode* node, const SourcePosition &start, const SourcePosition &end);
};

} // namespace builder
} // namespace Rose

#endif  // ROSE_SAGE_TREE_BUILDER_H_
