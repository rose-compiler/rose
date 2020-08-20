#ifndef ROSE_BUILDER_BUILD_H_
#define ROSE_BUILDER_BUILD_H_

// These includes are from the F18/flang source tree (./lib/parser)
//
#include "flang/Parser/parse-tree.h"
#include "flang/Semantics/symbol.h"
#include "flang/Semantics/scope.h"
#include <iostream>
#include <typeinfo>

// WARNING: This file has been designed to compile with -std=c++17
// This limits the use of ROSE header files at the moment.
//
class SgExpression;
class SgExprListExp;
class SgScopeStatement;
class SgStatement;
class SgType;


namespace Rose::builder {

// Converts parsed program to ROSE Sage nodes
void Build(const Fortran::parser::Program &x, Fortran::parser::CookedSource &cooked);

template<typename T> void Build(const Fortran::parser::ProgramUnit &x, T* scope);
template<typename T> void Build(const Fortran::parser::MainProgram &x, T* scope);

template<typename T> void Build(const Fortran::parser::     SpecificationPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::         ExecutionPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::ExecutionPartConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::   ExecutableConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::            ActionStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::        AssignmentStmt &x, T* scope);

void Build(const Fortran::parser::              Variable &x, SgExpression* &expr);
void Build(const Fortran::parser::            Designator &x, SgExpression* &expr);
void Build(const Fortran::parser::               DataRef &x, SgExpression* &expr);
void Build(const Fortran::parser::             Substring &x, SgExpression* &expr);
void Build(const Fortran::parser::     FunctionReference &x, SgExpression* &expr);
void Build(const Fortran::parser::                  Call &x, SgExpression* &expr);
void Build(const Fortran::parser::   ProcedureDesignator &x, SgExpression* &expr);
void Build(const Fortran::parser::      ProcComponentRef &x, SgExpression* &expr);
void Build(const Fortran::parser::         ActualArgSpec &x, SgExpression* &expr);
void Build(const Fortran::parser::             ActualArg &x, SgExpression* &expr);
void Build(const Fortran::parser::               Keyword &x, SgExpression* &expr);
void Build(const Fortran::parser::                  Name &x, SgExpression* &expr);
void Build(const Fortran::parser::         NamedConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::                  Expr &x, SgExpression* &expr);
void Build(const Fortran::parser:: Expr::IntrinsicBinary &x, SgExpression* &expr);
void Build(const Fortran::parser::         ConstantValue &x, SgExpression* &expr);
void Build(const Fortran::parser::       LiteralConstant &x, SgExpression* &expr);

// LiteralConstant
void Build(const Fortran::parser:: HollerithLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::       IntLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser:: SignedIntLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::      RealLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::SignedRealLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::   ComplexLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::       BOZLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::      CharLiteralConstant &x, SgExpression* &expr);
void Build(const Fortran::parser::   LogicalLiteralConstant &x, SgExpression* &expr);

void Build(const Fortran::parser::ComplexPart &x, SgExpression* &expr);

template<typename T> void Build(const Fortran::parser::InternalSubprogramPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::          ImplicitPart &x, T* scope);
template<typename T> void Build(const Fortran::parser::      ImplicitPartStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          ImplicitStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::          ImplicitSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::ImplicitStmt::ImplicitNoneNameSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::  DeclarationConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::SpecificationConstruct &x, T* scope);
template<typename T> void Build(const Fortran::parser::   TypeDeclarationStmt &x, T* scope);

void Build(const Fortran::parser::   DeclarationTypeSpec &x, SgType* &);

// DeclarationTypeSpec
void Build(const Fortran::parser::DeclarationTypeSpec::     Type&x, SgType* &);
void Build(const Fortran::parser::DeclarationTypeSpec:: TypeStar&x, SgType* &);
void Build(const Fortran::parser::DeclarationTypeSpec::    Class&x, SgType* &);
void Build(const Fortran::parser::DeclarationTypeSpec::ClassStar&x, SgType* &);
void Build(const Fortran::parser::DeclarationTypeSpec::   Record&x, SgType* &);

void Build(const Fortran::parser::       DerivedTypeSpec &x,                      SgType* &);
void Build(const Fortran::parser::            EntityDecl &x, std::string &, SgExpression* &, SgType* &, SgType *);
void Build(const std::list<Fortran::parser:: EntityDecl> &x, std::string &, SgExpression* &, SgType* &, SgType *);
template<typename T> void Build(const Fortran::parser::              AttrSpec &x, T* scope);
void Build(const Fortran::parser::             ArraySpec &x, SgType* &type, SgType* base_type);
template<typename T> void Build(const Fortran::parser::           CoarraySpec &x, T* scope);
void Build(const Fortran::parser::            CharLength &x, SgExpression* &);
void Build(const Fortran::parser::        Initialization &x, SgExpression* &);
void Build(const Fortran::parser::     IntrinsicTypeSpec &x,       SgType* &);
void Build(const Fortran::parser::       IntegerTypeSpec &x,       SgType* &);

// ArraySpec
void Build(const Fortran::parser::                   ExplicitShapeSpec &x, SgExpression* &expr);
void Build(const Fortran::parser::                    AssumedShapeSpec &x, SgExpression* &expr);
void Build(const Fortran::parser::               DeferredShapeSpecList &x, SgExpression* &expr);
void Build(const Fortran::parser::                     AssumedSizeSpec &x, SgExpression* &expr);
void Build(const Fortran::parser::                    ImpliedShapeSpec &x, SgExpression* &expr);
void Build(const Fortran::parser::                     AssumedRankSpec &x, SgExpression* &expr);
void Build(const Fortran::parser::                   SpecificationExpr &x, SgExpression* &expr);
void Build(const Fortran::parser::Scalar<Fortran::parser::IntExpr>     &x, SgExpression* &expr);
void Build(const Fortran::parser::Scalar<Fortran::parser::LogicalExpr> &x, SgExpression* &expr);
void Build(const Fortran::parser::                        ConstantExpr &x, SgExpression* &expr);

void Build(const Fortran::parser::IntrinsicTypeSpec::           Real &x, SgType* &);
void Build(const Fortran::parser::IntrinsicTypeSpec::DoublePrecision &x, SgType* &);
void Build(const Fortran::parser::IntrinsicTypeSpec::        Complex &x, SgType* &);
void Build(const Fortran::parser::IntrinsicTypeSpec::      Character &x, SgType* &);
void Build(const Fortran::parser::IntrinsicTypeSpec::        Logical &x, SgType* &);
void Build(const Fortran::parser::IntrinsicTypeSpec::  DoubleComplex &x, SgType* &);

void Build(const Fortran::parser::               CharSelector &x, SgExpression* &expr);
void Build(const Fortran::parser::             LengthSelector &x, SgExpression* &expr);
void Build(const Fortran::parser::CharSelector::LengthAndKind &x, SgExpression* &expr);
void Build(const Fortran::parser::             TypeParamValue &x, SgExpression* &expr);

// DeclarationConstruct
template<typename T> void Build(const Fortran::parser::        DataStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::      FormatStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::       EntryStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::StmtFunctionStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::   ErrorRecovery &x, T* scope);

// DataStmt
void Build(const Fortran::parser::   DataStmtValue &x, SgExpression* &expr);
void Build(const Fortran::parser::DataStmtConstant &x, SgExpression* &expr);

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

template<typename T> void Build(const Fortran::parser::               Format &x, T* scope);
template<typename T> void Build(const Fortran::parser::      DefaultCharExpr &x, T* scope);
template<typename T> void Build(const Fortran::parser::                Label &x, T* scope);
template<typename T> void Build(const Fortran::parser::                 Star &x, T* scope);
template<typename T> void Build(const Fortran::parser::           OutputItem &x, T* scope);
template<typename T> void Build(const Fortran::parser::      OutputImpliedDo &x, T* scope);

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
template<typename T> void Build(const Fortran::parser::         NamelistStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::        ParameterStmt &x, T* scope);
template<typename T> void Build(const Fortran::parser::     OldParameterStmt &x, T* scope);

// Expr
template<typename T> void traverseBinaryExprs(const T &x, SgExpression* &lhs, SgExpression* &rhs);

template<typename T> void Build(const Fortran::parser::CharLiteralConstantSubstring &x, T* &expr);
template<typename T> void Build(const Fortran::parser::            ArrayConstructor &x, T* &expr);
template<typename T> void Build(const Fortran::parser::        StructureConstructor &x, T* &expr);
template<typename T> void Build(const Fortran::parser::         Expr::DefinedBinary &x, T* &expr);
template<typename T> void Build(const Fortran::parser::    Expr::ComplexConstructor &x, T* &expr);
template<typename T> void Build(const Fortran::parser::           Expr::Parentheses &x, T* &expr);
template<typename T> void Build(const Fortran::parser::             Expr::UnaryPlus &x, T* &expr);
template<typename T> void Build(const Fortran::parser::                Expr::Negate &x, T* &expr);
template<typename T> void Build(const Fortran::parser::                   Expr::NOT &x, T* &expr);
template<typename T> void Build(const Fortran::parser::            Expr::PercentLoc &x, T* &expr);
template<typename T> void Build(const Fortran::parser::          Expr::DefinedUnary &x, T* &expr);

void Build(const Fortran::parser::   Expr::Power &x, SgExpression* &expr);
void Build(const Fortran::parser::Expr::Multiply &x, SgExpression* &expr);
void Build(const Fortran::parser::  Expr::Divide &x, SgExpression* &expr);
void Build(const Fortran::parser::     Expr::Add &x, SgExpression* &expr);
void Build(const Fortran::parser::Expr::Subtract &x, SgExpression* &expr);
void Build(const Fortran::parser::  Expr::Concat &x, SgExpression* &expr);
void Build(const Fortran::parser::      Expr::LT &x, SgExpression* &expr);
void Build(const Fortran::parser::      Expr::LE &x, SgExpression* &expr);
void Build(const Fortran::parser::      Expr::EQ &x, SgExpression* &expr);
void Build(const Fortran::parser::      Expr::NE &x, SgExpression* &expr);
void Build(const Fortran::parser::      Expr::GE &x, SgExpression* &expr);
void Build(const Fortran::parser::      Expr::GT &x, SgExpression* &expr);
void Build(const Fortran::parser::     Expr::AND &x, SgExpression* &expr);
void Build(const Fortran::parser::      Expr::OR &x, SgExpression* &expr);
void Build(const Fortran::parser::     Expr::EQV &x, SgExpression* &expr);
void Build(const Fortran::parser::    Expr::NEQV &x, SgExpression* &expr);

void Build(const Fortran::parser::  StructureComponent &x, SgExpression* &expr);
void Build(const Fortran::parser::        ArrayElement &x, SgExpression* &expr);
void Build(const Fortran::parser::CoindexedNamedObject &x, SgExpression* &expr);
void Build(const Fortran::parser::       ImageSelector &x, SgExpression* &expr);
void Build(const Fortran::parser::   ImageSelectorSpec &x, SgExpression* &expr);
void Build(const Fortran::parser::    SectionSubscript &x, SgExpression* &expr);
void Build(const Fortran::parser::    SubscriptTriplet &x, SgExpression* &expr);

// ExecutableConstruct
template<typename T> void Build(const Fortran::parser::   AssociateConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::       BlockConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::        CaseConstruct&x, T* scope);
void Build(const Fortran::parser::  CaseConstruct::Case&x, SgStatement* &stmt);
void Build(const Fortran::parser::             CaseStmt&x, std::list<SgExpression*> &case_list);
void Build(const Fortran::parser::         CaseSelector&x, std::list<SgExpression*> &case_list);
void Build(const Fortran::parser::       CaseValueRange&x, SgExpression* &expr);
void Build(const Fortran::parser::CaseValueRange::Range&x, SgExpression* &range);
template<typename T> void Build(const Fortran::parser::  ChangeTeamConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::    CriticalConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::          LabelDoStmt&x, T* scope);
template<typename T> void Build(const Fortran::parser::            EndDoStmt&x, T* scope);
template<typename T> void Build(const Fortran::parser::          DoConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::          IfConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::  SelectRankConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::  SelectTypeConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::       WhereConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::      ForallConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::    CompilerDirective&x, T* scope);
template<typename T> void Build(const Fortran::parser::      OpenMPConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::     OpenACCConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::  OmpEndLoopDirective&x, T* scope);

// DoConstruct
void Build(const Fortran::parser::NonLabelDoStmt&x, SgExpression* &expr);
void Build(const Fortran::parser::   LoopControl&x, SgExpression* &expr);

// SpecificationConstruct
template<typename T> void Build(const Fortran::parser::             DerivedTypeDef&x, T* scope);
template<typename T> void Build(const Fortran::parser::                    EnumDef&x, T* scope);
template<typename T> void Build(const Fortran::parser::             InterfaceBlock&x, T* scope);
template<typename T> void Build(const Fortran::parser::               StructureDef&x, T* scope);
template<typename T> void Build(const Fortran::parser::     OtherSpecificationStmt&x, T* scope);
template<typename T> void Build(const Fortran::parser::                GenericStmt&x, T* scope);
template<typename T> void Build(const Fortran::parser::   ProcedureDeclarationStmt&x, T* scope);
template<typename T> void Build(const Fortran::parser:: OpenMPDeclarativeConstruct&x, T* scope);
template<typename T> void Build(const Fortran::parser::OpenACCDeclarativeConstruct&x, T* scope);

// AttrSpec
template<typename T> void Build(const Fortran::parser::               AccessSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::              Allocatable &x, T* scope);
template<typename T> void Build(const Fortran::parser::             Asynchronous &x, T* scope);
template<typename T> void Build(const Fortran::parser::               Contiguous &x, T* scope);
template<typename T> void Build(const Fortran::parser::                 External &x, T* scope);
template<typename T> void Build(const Fortran::parser::               IntentSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::                Intrinsic &x, T* scope);
template<typename T> void Build(const Fortran::parser::      LanguageBindingSpec &x, T* scope);
template<typename T> void Build(const Fortran::parser::                 Optional &x, T* scope);
template<typename T> void Build(const Fortran::parser::                Parameter &x, T* scope);
template<typename T> void Build(const Fortran::parser::                  Pointer &x, T* scope);
template<typename T> void Build(const Fortran::parser::                Protected &x, T* scope);
template<typename T> void Build(const Fortran::parser::                     Save &x, T* scope);
template<typename T> void Build(const Fortran::parser::                   Target &x, T* scope);
template<typename T> void Build(const Fortran::parser::                    Value &x, T* scope);
template<typename T> void Build(const Fortran::parser::                 Volatile &x, T* scope);


// Traversal of needed STL template classes (optional, list, tuple, variant)                                                                
template<typename LT> void Build(const std::list<LT> &x, SgScopeStatement* scope)
{
   std::cout << "Rose::builder::Build(std::list) for T* node \n";

   for (const auto &elem : x) {
      Build(elem, scope);
   }
}

template<typename T> void Build(const std::list<T> &x, std::list<SgExpression*> &expr_list)
{
   std::cout << "Rose::builder::Build(std::list) for T* node building a list of SgExpression*\n";

   for (const auto &elem : x) {
      SgExpression* expr = nullptr;
      Build(elem, expr);
      expr_list.push_back(expr);
   }
}

template<typename LT, typename T> void Build(const std::list<LT> &x, T* &node)
{
   std::cout << "Rose::builder::Build(std::list) for T* &node \n";

   for (const auto &elem : x) {
      Build(elem, node);
   }
}

template<typename... A>
void Build(const std::variant<A...> &x, SgScopeStatement* scope) {
   try {
      auto & indirection = std::get<Fortran::common::Indirection<Fortran::parser::MainProgram, false>>(x);
      std::cout << "Rose::builder::Build(const std::variant<A...>): MainProgram \n";
      Build(indirection.value(), scope);
   }
   catch (const std::bad_variant_access&)
      {
         std::cout << "Rose::builder::Build(const std::variant<A...>) WARNING ProgramUnit variant not found type: "
                   << typeid(x).name() << "\n";
      }
}

}

#endif // ROSE_BUILDER_BUILD_H_
