#ifndef ROSE_BUILDER_BUILD_H_
#define ROSE_BUILDER_BUILD_H_

// These includes are from the F18/flang source tree (./lib/parser)
//
#include "parse-tree.h"
#include <iostream>
#include <typeinfo>

namespace Rose {
   struct SgScopeStatement;
}

namespace Rose::builder {

// Converts parsed program to ROSE Sage nodes
void Build(const Fortran::parser::Program &x, Rose::SgScopeStatement* scope);

template<typename T> void Build(const Fortran::parser::ProgramUnit &x, T* scope);
template<typename T> void Build(const Fortran::parser::MainProgram &x, T* scope);

template<typename T> void Build(const Fortran::parser::     SpecificationPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::         ExecutionPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::InternalSubprogramPart &x, T* scope);

// Traversal of needed STL template classes (optional, list, tuple, variant)                                                                
//

template<typename T> void Build(const std::list<T> &x, Rose::SgScopeStatement* scope)
{
   std::cout << "Rose::builder::Build(std::list) \n";

   for (const auto &elem : x) {
      Build(elem, scope);
   }
}

template<typename... A>
void Build(const std::variant<A...> &x, Rose::SgScopeStatement* scope) {
   try {
      auto & indirection = std::get<Fortran::common::Indirection<Fortran::parser::MainProgram, false>>(x);
      std::cout << "Rose::builder::Build(const std::variant<A...>): MainProgram \n";
      Build(indirection.value(), scope);
   }
   catch (const std::bad_variant_access&)
      {
         std::cout << "Rose::builder::Build(const std::variant<A...>) NOT FOUND a MainProgram \n";
      }
}

}

#endif // ROSE_BUILDER_BUILD_H_
