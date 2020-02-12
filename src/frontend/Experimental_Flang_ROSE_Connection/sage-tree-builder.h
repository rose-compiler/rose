#ifndef ROSE_SAGE_TREE_BUILDER_H_
#define ROSE_SAGE_TREE_BUILDER_H_

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>

// WARNING: This file has been designed to compile with -std=c++17
// This limits the use of ROSE header files at the moment.
//
class SgLocatedNode;
class SgScopeStatement;
class SgProgramHeaderStatement;

namespace Rose {
namespace builder {

// This is similar to F18 Fortran::parser::SourcePosition
struct SourcePosition {
   std::string path;  // replaces Fortran::parser::SourceFile
   int line, column;
};

using SourcePositions = boost::tuple<SourcePosition, SourcePosition, SourcePosition>;

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


private:

   void setSourcePosition(SgLocatedNode* node, const SourcePosition &start, const SourcePosition &end);
};

} // namespace builder
} // namespace Rose

#endif  // ROSE_SAGE_TREE_BUILDER_H_
