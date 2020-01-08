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
template<typename T> void Build(const Fortran::parser::ExecutionPartConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::   ExecutableConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::            ActionStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::        AssignmentStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::              Variable &x, T* scope);
template<typename T> void Build(const Fortran::parser::            Designator &x, T* scope);
template<typename T> void Build(const Fortran::parser::               DataRef &x, T* scope);
template<typename T> void Build(const Fortran::parser::             Substring &x, T* scope);
template<typename T> void Build(const Fortran::parser::     FunctionReference &x, T* scope);
template<typename T> void Build(const Fortran::parser::                  Expr &x, T* scope);
template<typename T> void Build(const Fortran::parser:: Expr::IntrinsicBinary &x, T* scope);
template<typename T> void Build(const Fortran::parser::       LiteralConstant &x, T* scope);
template<typename T> void Build(const Fortran::parser::    IntLiteralConstant &x, T* scope);
template<typename T> void Build(const Fortran::parser::InternalSubprogramPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::          ImplicitPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::      ImplicitPartStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          ImplicitStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          ImplicitSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::ImplicitStmt::ImplicitNoneNameSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::  DeclarationConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::SpecificationConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::   TypeDeclarationStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::   DeclarationTypeSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::              AttrSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::            EntityDecl &x, T* scope);
template<typename T> void Build(const Fortran::parser::     IntrinsicTypeSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::       IntegerTypeSpec &x, T* scope);

template<typename T> void Build(const Fortran::parser::IntrinsicTypeSpec::           Real &x, T* scope);
template<typename T> void Build(const Fortran::parser::IntrinsicTypeSpec::DoublePrecision &x, T* scope);
template<typename T> void Build(const Fortran::parser::IntrinsicTypeSpec::        Complex &x, T* scope);
template<typename T> void Build(const Fortran::parser::IntrinsicTypeSpec::      Character &x, T* scope);
template<typename T> void Build(const Fortran::parser::IntrinsicTypeSpec::        Logical &x, T* scope);
template<typename T> void Build(const Fortran::parser::IntrinsicTypeSpec::  DoubleComplex &x, T* scope);

// DeclarationConstruct
template<typename T> void Build(const Fortran::parser::        DataStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::      FormatStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::       EntryStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::StmtFunctionStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::   ErrorRecovery &x, T* scope);

// ActionStmt
template<typename T> void Build(const Fortran::parser::         ContinueStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::        FailImageStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::         AllocateStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::        BackspaceStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             CallStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::            CloseStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::            CycleStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::       DeallocateStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          EndfileStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::        EventPostStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::        EventWaitStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             ExitStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::            FlushStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::         FormTeamStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             GotoStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::               IfStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          InquireStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             LockStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          NullifyStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             OpenStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::PointerAssignmentStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::            PrintStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             ReadStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::           ReturnStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::           RewindStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             StopStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          SyncAllStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::       SyncImagesStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::       SyncMemoryStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::         SyncTeamStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::           UnlockStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::             WaitStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::            WhereStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::            WriteStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::     ComputedGotoStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::           ForallStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::     ArithmeticIfStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::           AssignStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::     AssignedGotoStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::            PauseStmt &x, T* scope);

// Expr
template<typename T> void Build(const Fortran::parser::CharLiteralConstantSubstring &x, T* scope);
template<typename T> void Build(const Fortran::parser::            ArrayConstructor &x, T* scope);
template<typename T> void Build(const Fortran::parser::        StructureConstructor &x, T* scope);
template<typename T> void Build(const Fortran::parser::         Expr::DefinedBinary &x, T* scope);
template<typename T> void Build(const Fortran::parser::    Expr::ComplexConstructor &x, T* scope);
template<typename T> void Build(const Fortran::parser::           Expr::Parentheses &x, T* scope);
template<typename T> void Build(const Fortran::parser::             Expr::UnaryPlus &x, T* scope);
template<typename T> void Build(const Fortran::parser::                Expr::Negate &x, T* scope);
template<typename T> void Build(const Fortran::parser::                   Expr::NOT &x, T* scope);
template<typename T> void Build(const Fortran::parser::            Expr::PercentLoc &x, T* scope);
template<typename T> void Build(const Fortran::parser::          Expr::DefinedUnary &x, T* scope);

template<typename T> void Build(const Fortran::parser::   Expr::Power &x, T* scope);
template<typename T> void Build(const Fortran::parser::Expr::Multiply &x, T* scope);
template<typename T> void Build(const Fortran::parser::  Expr::Divide &x, T* scope);
template<typename T> void Build(const Fortran::parser::     Expr::Add &x, T* scope);
template<typename T> void Build(const Fortran::parser::Expr::Subtract &x, T* scope);
template<typename T> void Build(const Fortran::parser::  Expr::Concat &x, T* scope);
template<typename T> void Build(const Fortran::parser::      Expr::LT &x, T* scope);
template<typename T> void Build(const Fortran::parser::      Expr::LE &x, T* scope);
template<typename T> void Build(const Fortran::parser::      Expr::EQ &x, T* scope);
template<typename T> void Build(const Fortran::parser::      Expr::NE &x, T* scope);
template<typename T> void Build(const Fortran::parser::      Expr::GE &x, T* scope);
template<typename T> void Build(const Fortran::parser::      Expr::GT &x, T* scope);
template<typename T> void Build(const Fortran::parser::     Expr::AND &x, T* scope);
template<typename T> void Build(const Fortran::parser::      Expr::OR &x, T* scope);
template<typename T> void Build(const Fortran::parser::     Expr::EQV &x, T* scope);
template<typename T> void Build(const Fortran::parser::    Expr::NEQV &x, T* scope);


// Traversal of needed STL template classes (optional, list, tuple, variant)                                                                
//

template<typename T> void Build(const std::list<T> &x, Rose::SgScopeStatement* scope)
{
   std::cout << "Rose::builder::Build(std::list) \n";

   if (x.empty()) {
      std::cout << "The list is EMPTY and is of type: " << typeid(x).name() << "\n";
   }

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
