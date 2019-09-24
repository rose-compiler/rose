#include "sage-build.h"
#include <iostream>

namespace Rose::builder {

using namespace Fortran;

// Converts parsed program to ROSE Sage nodes
void Build(const parser::Program &x, SgScopeStatement* scope)
{
   std::cout << "\n";
   std::cout << "Rose::builder::Build(Program) \n";
   Build(x.v, scope);
}

template<typename T>
void Build(const parser::ProgramUnit &x, T* scope)
{
   std::cout << "Rose::builder::Build(ProgramUnit) \n";

   Build(x.u, scope);
}

template<typename T>
void Build(const parser::MainProgram &x, T* scope)
{
   std::cout << "Rose::builder::Build(MainProgram) \n";

   std::string program_name;

// ProgramStmt is optional
   if (std::get<0>(x.t)) {
      const auto & stmt = std::get<0>(x.t).value();
      program_name = stmt.statement.v.ToString();
      std::cout << "--> PROGRAM " << program_name << std::endl;
   }

// Build Sage nodes
//
   SgScopeStatement* function_scope{nullptr};

// SpecificationPart
   const auto & spec_part = std::get<1>(x.t);
   Build(spec_part, function_scope);

// ExecutionPart
   const auto & exec_part = std::get<2>(x.t);
   Build(exec_part, function_scope);

// InternalSubprogramPart is optional
   if (std::get<3>(x.t)) {
      const auto & prog_part = std::get<3>(x.t).value();
      Build(prog_part, function_scope);
   }

// EndProgramStmt
   const auto & end_program_stmt = std::get<4>(x.t).statement;
   std::cout << "--> END PROGRAM";
   if (end_program_stmt.v) {
      std::cout << " " << end_program_stmt.v.value().ToString();
   }
   std::cout << std::endl;
   
}

template<typename T>
void Build(const parser::SpecificationPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(SpecificationPart) \n";
}

template<typename T>
void Build(const parser::ExecutionPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExecutionPart) \n";
}

template<typename T>
void Build(const parser::InternalSubprogramPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(InternalSubprogramPart) \n";
}

}
