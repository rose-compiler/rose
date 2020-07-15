/*
<sage-build.cc> is distributed under the OSI-approved BSD 3-clause License:
Copyright (c) 2020, Sourcery, Inc.
Copyright (c) 2020, Sourcery Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the names of the copyright holders nor the names of their contributors may be used to endorse or
promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include "sage-build.h"
#include "sage-tree-builder.h"
#include <boost/optional.hpp>
#include <iostream>

// Helps with find source position information
enum class Order { begin, end };

namespace Rose::builder {

using namespace Fortran;

// The Build functions need to be turned into a class (global variable used for now)
//
   SageTreeBuilder builder{};
// TODO: change this to a reference
   parser::CookedSource* cooked_{nullptr};

template<typename T> SourcePosition BuildSourcePosition(const Fortran::parser::Statement<T> &x, Order from)
{
   std::optional<SourcePosition> pos{std::nullopt};

   if (auto sourceInfo{cooked_->GetSourcePositionRange(x.source)}) {
      if (from == Order::begin)
         pos.emplace(SourcePosition{sourceInfo->first.file.path(), sourceInfo->first.line, sourceInfo->first.column});
      else
         pos.emplace(SourcePosition{sourceInfo->second.file.path(), sourceInfo->second.line, sourceInfo->second.column});
   }
   else {
      pos.emplace(SourcePosition{});
   }

   return pos.value();
}

template<typename T>
std::optional<SourcePosition> BuildSourcePosition(const std::optional<Fortran::parser::Statement<T>> &opt, Order from)
{
   std::optional<SourcePosition> pos{std::nullopt};

   if (opt) pos.emplace(BuildSourcePosition(*opt, from));

   return pos;
}

template<typename T>
std::optional<SourcePosition> BuildSourcePosition(const std::variant<T> &u, Order from)
{
   // TODO
   return std::nullopt;
}

std::optional<SourcePosition> FirstSourcePosition(const parser::SpecificationPart &x)
{
   const auto & omp_stmts{std::get<0>(x.t)};
   if (omp_stmts.size() > 0) {
#if 0
      return std::optional<SourcePosition>{BuildSourcePosition(omp_stmts.front(), Order::begin)};
#endif
   }

   const auto & use_stmts{std::get<1>(x.t)};
   if (use_stmts.size() > 0) {
      return std::optional<SourcePosition>{BuildSourcePosition(use_stmts.front(), Order::begin)};
   }

   const auto & import_stmts{std::get<2>(x.t)};
   if (import_stmts.size() > 0) {
      return std::optional<SourcePosition>{BuildSourcePosition(import_stmts.front(), Order::begin)};
   }

   const auto & implicit_part_stmts{std::get<3>(x.t).v};
   if (implicit_part_stmts.size() > 0) {
      std::cout << "... implicit_part_stmts list count is " << implicit_part_stmts.size() << "\n";
      //      const auto & implicit_part_stmt
#if 0 // TODO
      return std::optional<SourcePosition>{BuildSourcePosition(implicit_part_stmts.front(), Order::begin)};
#endif
   }

   const auto & decl_stmts{std::get<4>(x.t)};
   if (decl_stmts.size() > 0) {
#if 0 // TODO
      return std::optional<SourcePosition>{BuildSourcePosition(decl_stmts.front(), Order::begin)};
#endif
   }

   return std::optional<SourcePosition>{std::nullopt};
}

// Converts parsed program to ROSE Sage nodes
void Build(const parser::Program &x, parser::CookedSource &cooked)
{
   std::cout << "\n";
   std::cout << "Rose::builder::Build(Program) \n";

   SgScopeStatement* scope{nullptr};

   cooked_ = &cooked;

   builder.Enter(scope);
   Build(x.v, scope);
   builder.Leave(scope);
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

   const auto & program_stmt{std::get<0>(x.t)};

   const auto & spec_part{std::get<1>(x.t)};
   const auto & exec_part{std::get<2>(x.t)};
   const auto & prog_part{std::get<3>(x.t)};

   const auto & end_program_stmt{std::get<4>(x.t)};

   std::list<std::string> labels{};
   std::optional<SourcePosition> srcPosBody{std::nullopt};
   std::optional<SourcePosition> srcPosBegin{BuildSourcePosition(program_stmt, Order::begin)};
   SourcePosition srcPosEnd{BuildSourcePosition(end_program_stmt, Order::end)};

   std::optional<std::string> program_name{std::nullopt};

// ProgramStmt is optional
   if (program_stmt) {
      program_name.emplace(program_stmt.value().statement.v.ToString());
   }
   if (program_stmt && program_stmt->label) {
      labels.push_back(std::to_string(program_stmt->label.value()));
   }

   if (auto pos{FirstSourcePosition(spec_part)}) {
      srcPosBody.emplace(*pos);
   }

// Fortran only needs an end statement so check for no beginning source position
   if (!srcPosBody) {
      srcPosBody.emplace(srcPosEnd);
   }

   // If there is no ProgramStmt the source begins at the body of the program
   if (!srcPosBegin) {
      srcPosBegin.emplace(*srcPosBody);
   }

// Build the SgProgramHeaderStatement node
//
   SgProgramHeaderStatement* program_decl{nullptr};
   boost::optional<std::string> boost_name{*program_name};

   builder.Enter(program_decl, boost_name, labels, SourcePositions{*srcPosBegin,*srcPosBody,srcPosEnd});

   SgScopeStatement* function_scope{nullptr};

// SpecificationPart
   Build(spec_part, function_scope);

// ExecutionPart
   Build(exec_part, function_scope);

// InternalSubprogramPart is optional
   if (prog_part) {
      Build(prog_part.value(), function_scope);
   }

// EndProgramStmt
   boost::optional<std::string> end_name {boost::none};
   boost::optional<std::string> end_label{boost::none};
   if (end_program_stmt.statement.v) {
      end_name = end_program_stmt.statement.v.value().ToString();
   }
   if (end_program_stmt.label) {
      end_label = std::to_string(end_program_stmt.label.value());
   }

// Fortran specific functionality
   builder.setFortranEndProgramStmt(program_decl, end_name, end_label);

   builder.Leave(program_decl);
}

template<typename T>
void Build(const parser::SpecificationPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(SpecificationPart)\n";

   const auto & implicit_part = std::get<3>(x.t);
   Build(implicit_part, scope);

   const auto & decl_construct = std::get<4>(x.t);
   Build(decl_construct, scope);

}

template<typename T>
void Build(const parser::ExecutionPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExecutionPart)\n";
   Build(x.v, scope);   // std::list<ExecutionPartConstruct>
}

template<typename T>
void Build(const parser::ExecutionPartConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExecutionPartConstruct)\n";

   std::visit(
      common::visitors{
         [&] (const parser::ExecutableConstruct &y) { Build(y, scope); },
         [&] (const parser::ErrorRecovery &y)       { Build(y, scope); },
         //  Statement<common::Indirection> - FormatStmt, EntryStmt, DataStmt, or NamelistStmt
         [&] (const auto &y) { Build(y.statement.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::ExecutableConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExecutableConstruct)\n";

   std::visit(
      common::visitors{
         [&] (const parser::Statement<parser::ActionStmt> &y) { Build(y.statement, scope); },
         [&] (const parser::Statement<common::Indirection<parser::LabelDoStmt>> &y)
                { Build(y.statement.value(), scope); },
         [&] (const parser::Statement<common::Indirection<parser::EndDoStmt>> &y)
                { Build(y.statement.value(), scope); },
         // common:: Indirection - AssociateConstruct, BlockConstruct, CaseConstruct, ChangeTeamConstruct,
         // CriticalConstruct, DoConstruct, IfConstruct, SelectRankConstruct, SelectTypeConstruct,
         // WhereConstruct, ForallConstruct, CompilerDirective, OpenMPConstruct, OmpEndLoopDirective
         [&] (const auto &y) { Build(y.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::ActionStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(ActionStmt)\n";

   std::visit(
      common::visitors{
         [&](const parser::ContinueStmt  &y) { Build(y, scope); },
         [&](const parser::FailImageStmt &y) { Build(y, scope); },
         // common::Indirection - AllocateStmt, AssignmentStmt, BackspaceStmt, CallStmt, CloseStmt,
         // CycleStmt, DeallocateStmt, EndfileStmt, EventPostStmt, EventWaitStmt, ExitStmt, FailImageStmt,
         // FlushStmt, FormTeamStmt, GotoStmt, IfStmt, InquireStmt, LockStmt, NullifyStmt, OpenStmt,
         // PointerAssignmentStmt, PrintStmt, ReadStmt, ReturnStmt, RewindStmt, StopStmt, SyncAllStmt,
         // SyncImagesStmt, SyncMemoryStmt, SyncTeamStmt, UnlockStmt, WaitStmt, WhereStmt, WriteStmt,
         // ComputedGotoStmt, ForallStmt, ArithmeticIfStmt, AssignStmt, AssignedGotoStmt, PauseStmt
         [&](const auto &y) { Build(y.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::AssignmentStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(AssignmentStmt)\n";

   SgExpression* lhs{nullptr};
   SgExpression* rhs{nullptr};

   auto & variable = std::get<0>(x.t);
   Build(variable, lhs);

   auto & expr = std::get<1>(x.t);
   Build(expr, rhs);

   SgExprStatement* assign_stmt = nullptr;
   std::vector<SgExpression*> vars;
   vars.push_back(lhs);

   // Begin SageTreeBuilder
   builder.Enter(assign_stmt, rhs, vars, std::string()/* no label*/);
   builder.Leave(assign_stmt);
}

void Build(const parser::Variable &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Variable)\n";

   // Designator, FunctionReference
   auto VariableVisitor = [&](const auto& y) { Build(y.value(), expr); };
   std::visit(VariableVisitor, x.u);
}

void Build(const parser::Designator &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Designator)\n";

   // DataRef, Substring
   auto DesignatorVisitor = [&](const auto& y) { Build(y, expr); };
   std::visit(DesignatorVisitor, x.u);
}

void Build(const parser::DataRef &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(DataRef)\n";

   std::visit(
      common::visitors{
         [&] (const Fortran::parser::Name &y) {
            std::string name = std::get<0>(x.u).ToString();
            std::cout << "The name of the DataRef is " << name << std::endl;

            semantics::Symbol *symbol = y.symbol;

            // create a varref
            expr = SageBuilderCpp17::buildVarRefExp_nfi(name);

#if 0
            if (symbol) {
               const parser::CharBlock &srcName = symbol->name();
               std::cout << "The symbol name is " << srcName.ToString() << "\n";

               semantics::Attrs &attrs = symbol->attrs();
               std::cout << "The attrs is " << &attrs << "\n";

               const semantics::Scope &owner_scope = symbol->owner();
               semantics::Scope *introduced_scope = symbol->scope();

               const semantics::Symbol *owner_scope_symbol = owner_scope.symbol();

               if (owner_scope_symbol) {
                  std::string owner_scope_name = owner_scope_symbol->name().ToString();
                  std::cout << "owner scope name is " << owner_scope_name << "\n";
               }
               else {
                  std::cout << "no symbol for owner scope \n";
               }

               semantics::Scope::Kind owner_scope_kind = owner_scope.kind();
               std::cout << "owner scope kind is " << semantics::Scope::EnumToString(owner_scope_kind) << "\n";

               const semantics::Scope &owner_scope_parent = owner_scope.parent();
               semantics::Scope::Kind owner_scope_parent_kind = owner_scope_parent.kind();
               std::cout << "owner scope parent kind is " << semantics::Scope::EnumToString(owner_scope_parent_kind) << "\n";

               if (introduced_scope) {
                  semantics::Scope::Kind introduced_scope_kind = introduced_scope->kind();
                  std::cout << "introduced scope kind is " << semantics::Scope::EnumToString(introduced_scope_kind) << "\n";
               }
               else {
                  std::cout << "No scope was introduced by " << name << "()\n";
               }
            }
#endif
          },
         // StructureComponent, ArrayElement, or CoindexedNamedObject
         [&] (const auto &y) { Build(y.value(), expr); },
      },
      x.u);
}

void Build(const parser::Substring &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Substring)\n";
}

void Build(const parser::FunctionReference &x, SgExpression* &expr)
{
   //Call v;
   //  Designator ConvertToArrayElementRef();
   //  StructureConstructor ConvertToStructureConstructor(
   //      const semantics::DerivedTypeSpec &);
   std::cout << "Rose::builder::Build(FunctionReference)\n";

   Build(x.v, expr); // Call

   //   const auto & arrRef = x.ConvertToArrayElementRef();
}

void Build(const parser::Call &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Call)\n";

   const parser::CharBlock source = x.source;

   SgExpression* proc_name{nullptr};
   SgExpression* arg_list{nullptr};

   Build(std::get<0>(x.t), proc_name);   // ProcedureDesignator
   Build(std::get<1>(x.t), arg_list);    // std::list<ActualArgSpec>
}

void Build(const parser::ProcedureDesignator &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ProcedureDesignator)\n";

   std::visit(
      common::visitors{
         [&] (const parser::Name &y)
         {
            std::string name = y.ToString();
            std::cout << "The ProcedureDesignator name is " << name << "\n";

            semantics::Symbol *symbol = y.symbol;

            const parser::CharBlock &srcName = symbol->name();
            std::cout << "The symbol name is " << srcName.ToString() << "\n";

            semantics::Attrs &attrs = symbol->attrs();
            std::cout << "The attrs is " << &attrs << "\n";

            const semantics::Scope &owner_scope = symbol->owner();
            semantics::Scope *introduced_scope = symbol->scope();

            const semantics::Symbol *owner_scope_symbol = owner_scope.symbol();

            if (owner_scope_symbol) {
               std::string owner_scope_name = owner_scope_symbol->name().ToString();
               std::cout << "owner scope name is " << owner_scope_name << "\n";
            }

            semantics::Scope::Kind owner_scope_kind = owner_scope.kind();
            std::cout << "owner scope kind is " << semantics::Scope::EnumToString(owner_scope_kind) << "\n";

            const semantics::Scope &owner_scope_parent = owner_scope.parent();
            semantics::Scope::Kind owner_scope_parent_kind = owner_scope_parent.kind();
            std::cout << "owner scope parent kind is " << semantics::Scope::EnumToString(owner_scope_parent_kind) << "\n";

            if (introduced_scope) {
               semantics::Scope::Kind introduced_scope_kind = introduced_scope->kind();
               std::cout << "introduced scope kind is " << semantics::Scope::EnumToString(introduced_scope_kind) << "\n";
            }
            else {
               std::cout << "No scope was introduced by call: " << name << "()\n";
            }

            const semantics::Details &details = symbol->details();
            std::cout << "details is " << &details << "\n";
            const auto proc_details = std::get<semantics::ProcEntityDetails>(details);
            const semantics::ProcInterface &proc_interface = proc_details.interface();
            const semantics::Symbol *proc_interface_symbol = proc_interface.symbol();
            const semantics::DeclTypeSpec *proc_interface_type = proc_interface.type();

            if (proc_interface_symbol) {
               std::cout << "details symbol name is " << proc_interface_symbol->name().ToString() << "\n";
            }
            else {
               std::cout << "no symbol for proc interface\n";
            }

            if (proc_interface_type) {
               std::cout << "proc interface type is " << proc_interface_type->category() << "\n";
            }
            else {
               std::cout << "no type for proc interface\n";
            }
         },
         [&] (const auto &y) { Build(y, expr); }   // ProcComponentRef
      },
      x.u);
}

void Build(const parser::ProcComponentRef &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ProcComponentRef)\n";
}

void Build(const parser::ActualArgSpec &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ActualArgSpec)\n";

   if (auto & opt = std::get<0>(x.t)) {    // std::optional<Keyword>
      Build(opt.value(), expr);
   }

   Build(std::get<1>(x.t), expr);         // ActualArg
}

void Build(const parser::ActualArg &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ActualArg)\n";
   //  std::variant<common::Indirection<Expr>, AltReturnSpec, PercentRef, PercentVal>

   std::visit(
      common::visitors{
         [&] (const common::Indirection<parser::Expr> &y) { Build(y.value(), expr); },
         [&] (const auto &y) { }   // AltReturnSpec, PercentRef, PercentVal
      },
      x.u);
}

void Build(const parser::Keyword &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Keyword)\n";
}

void Build(const parser::Name &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Name)\n";
}

void Build(const parser::Expr &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Expr)\n";

   std::visit(
      common::visitors{
         [&](const Fortran::common::Indirection<parser::CharLiteralConstantSubstring> &y)
               { Build(y.value(), expr); },
         [&](const Fortran::common::Indirection<parser::Designator> &y)
               { Build(y.value(), expr); },
         [&](const Fortran::common::Indirection<parser::FunctionReference> &y)
               { Build(y.value(), expr); },
         // LiteralConstant, ArrayConstructor, StructureConstructor, Parentheses, UnaryPlus,
         // Negate, NOT, PercentLoc, DefinedUnary, Power, Multiply, Divide, Add, Subtract, Concat
         // LT, LE, EQ, NE, GE, GT, AND, OR, EQV, NEQV, XOR, DefinedBinary, ComplexConstructor
         [&](const auto &y) { Build(y, expr); },
      },
      x.u);
}

void Build(const parser::Expr::IntrinsicBinary &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(IntrinsicBinary)\n";
}

void Build(const parser::LiteralConstant &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(LiteralConstant)\n";

   //   Build(std::get<1>(x.u), scope);

   //  HollerithLiteralConstant, IntLiteralConstant, RealLiteralConstant, ComplexLiteralConstant,
   //  BOZLiteralConstant, CharLiteralConstant, LogicalLiteralConstant
   auto LiteralConstVisitor = [&] (const auto &y) { Build(y, expr); };
   std::visit(LiteralConstVisitor, x.u);
}

   // LiteralConstant
template<typename T>
void Build(const parser::HollerithLiteralConstant &x, T* &expr)
{
   std::cout << "Rose::builder::Build(HollerithLiteralConstant)\n";
}

template<typename T>
void Build(const parser::IntLiteralConstant &x, T* &expr)
{
   std::cout << "Rose::builder::Build(IntLiteralConstant)\n";

   expr = SageBuilderCpp17::buildIntVal_nfi(stoi(std::get<0>(x.t).ToString()));
}

template<typename T>
void Build(const parser::RealLiteralConstant &x, T* &expr)
{
   std::cout << "Rose::builder::Build(RealLiteralConstant)\n";
}

template<typename T>
void Build(const parser::ComplexLiteralConstant &x, T* &expr)
{
   std::cout << "Rose::builder::Build(ComplexLiteralConstant)\n";
}

template<typename T>
void Build(const parser::BOZLiteralConstant &x, T* &expr)
{
   std::cout << "Rose::builder::Build(BOZLiteralConstant)\n";
}

template<typename T>
void Build(const parser::CharLiteralConstant &x, T* &expr)
{
   std::cout << "Rose::builder::Build(CharLiteralConstant)\n";
   std::string literal = x.GetString();
   std::cout << " The CHAR LITERAL CONSTANT is \"" << literal << "\"" << std::endl;
}

template<typename T>
void Build(const parser::LogicalLiteralConstant &x, T* &expr)
{
   std::cout << "Rose::builder::Build(LogicalLiteralConstant)\n";
   bool literal = std::get<0>(x.t);
   std::cout << " The LOGICAL LITERAL CONSTANT is " << literal << std::endl;
}

template<typename T>
void Build(const parser::InternalSubprogramPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(InternalSubprogramPart)\n";
}

template<typename T>
void Build(const parser::ImplicitPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitPart)\n";
   Build(x.v, scope);   // std::list<ImplicitPartStmt>
}

template<typename T>
void Build(const parser::ImplicitPartStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitPartStmt)\n";

   // Statement<common::Indirection<> - ImplicitStmt, ParameterStmt, OldParameterStmt, FormatStmt, EntryStmt
   auto ImplicitPartStmtVisitor = [&] (const auto &y) { Build(y.statement.value(), scope); };
   std::visit(ImplicitPartStmtVisitor, x.u);
}

template<typename T>
void Build(const parser::ImplicitStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitStmt)\n";

   // std::list<ImplicitSpec>, std::list<ImplicitNoneNameSpec>
   auto SpecVisitor = [&](const auto& y) { Build(y, scope); };
   std::visit(SpecVisitor, x.u);
}

template<typename T>
void Build(const parser::ImplicitSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitSpec)\n";
}

template<typename T>
void Build(const parser::ImplicitStmt::ImplicitNoneNameSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitNoneNameSpec)\n";
}

template<typename T>
void Build(const parser::DeclarationConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(DeclarationConstruct)\n";

   std::visit(
      common::visitors{
         [&](const parser::SpecificationConstruct &y) { Build(y, scope); },
         [&](const parser::ErrorRecovery &y)          { Build(y, scope); },
         //  Statement<common::Indirection<>> - DataStmt, FormatStmt, EntryStmt, StmtFunctionStmt
         [&](const auto &y) { Build(y.statement.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::SpecificationConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(SpecificationConstruct)\n";

   std::visit(
      common::visitors{
         [&](const common::Indirection<parser::DerivedTypeDef> &y)             { Build(y.value(), scope); },
         [&](const common::Indirection<parser::EnumDef> &y)                    { Build(y.value(), scope); },
         [&](const common::Indirection<parser::InterfaceBlock> &y)             { Build(y.value(), scope); },
         [&](const common::Indirection<parser::StructureDef> &y)               { Build(y.value(), scope); },
         [&](const common::Indirection<parser::CompilerDirective> &y)          { Build(y.value(), scope); },
         [&](const common::Indirection<parser::OpenMPDeclarativeConstruct> &y) { Build(y.value(), scope); },
         [&](const parser::Statement<parser::OtherSpecificationStmt> &y)     { Build(y.statement, scope); },
         // Statement<common::Indirection<>> - GenericStmt, ParameterStmt,
         // OldParameterStmt, ProcedureDeclarationStmt, TypeDeclarationStmt
         [&](const auto &y ) { Build(y.statement.value(), scope); }
      },
      x.u);
}

template<typename T>
void Build(const parser::TypeDeclarationStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(TypeDeclarationStmt)\n";

   SgVariableDeclaration* var_decl = nullptr;
   SgType* type = nullptr;
   SgExpression* init_expr = nullptr;

   // need name and type
   std::string name{};

   Build(std::get<0>(x.t), type);        // DeclarationTypeSpec
   Build(std::get<1>(x.t), scope);       // std::list<AttrSpec>
   Build(std::get<2>(x.t), name);        // std::list<EntityDecl>

   builder.Enter(var_decl, name, type, init_expr);
   builder.Leave(var_decl);
}

void Build(const parser::DeclarationTypeSpec &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(DeclarationTypeSpec)\n";

   // IntrinsicTypeSpec, Type, TypeStar, Class, ClassStar, Record
   auto DeclTypeSpecVisitor = [&] (const auto &y) { Build(y, type); };
   std::visit(DeclTypeSpecVisitor, x.u);
}

void Build(const parser::DeclarationTypeSpec::Type&x, SgType* &type)
{
   std::cout << "Rose::builder::Build(Type)\n";
   Build(x.derived, type);   // DerivedTypeSpec
}

void Build(const parser::DeclarationTypeSpec::TypeStar&x, SgType* &type)
{
   std::cout << "Rose::builder::Build(TypeStar)\n";
}

void Build(const parser::DeclarationTypeSpec::Class&x, SgType* &type)
{
   std::cout << "Rose::builder::Build(Class)\n";
   Build(x.derived, type);   // DerivedTypeSpec
}

void Build(const parser::DeclarationTypeSpec::ClassStar&x, SgType* &type)
{
   std::cout << "Rose::builder::Build(ClassStar)\n";
}

void Build(const parser::DeclarationTypeSpec::Record&x, SgType* &type)
{
   std::cout << "Rose::builder::Build(Record)\n";
}

void Build(const parser::DerivedTypeSpec&x, SgType* &type)
{
   std::cout << "Rose::builder::Build(DerivedTypeSpec)\n";

   //   std::tuple<Name, std::list<TypeParamSpec>> t;
   std::string name = std::get<parser::Name>(x.t).ToString();
   std::cout << "DerivedTypeSpec name is " << name << "\n";
}

template<typename T>
void Build(const parser::AttrSpec &x, T* scope)
{
   //  std::variant<AccessSpec, Allocatable, Asynchronous, CoarraySpec, Contiguous,
   //      ArraySpec, External, IntentSpec, Intrinsic, LanguageBindingSpec, Optional,
   //      Parameter, Pointer, Protected, Save, Target, Value, Volatile>
   std::cout << "Rose::builder::Build(AttrSpec)\n";

   auto AttrSpecVisitor = [&] (const auto &y) { Build(y, scope); };
   std::visit(AttrSpecVisitor, x.u);
}

void Build(const parser::IntrinsicTypeSpec &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(IntrinsicTypeSpec)\n";

   // IntegerTypeSpec, Real, DoublePrecision, Complex, Character, Logical, DoubleComplex
   auto TypeVisitor = [&](const auto& y) { Build(y, type); };
   std::visit(TypeVisitor, x.u);
}

void Build(const parser::IntegerTypeSpec &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(IntegerTypeSpec)\n";

   type = SageBuilderCpp17::buildIntType();
}

void Build(const parser::IntrinsicTypeSpec::Real &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(Real)\n";
   std::cout << "TYPE IS : Real\n";
}

void Build(const parser::IntrinsicTypeSpec::DoublePrecision &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(DoublePrecision)\n";
   std::cout << "TYPE IS : DoublePrecision\n";
}

void Build(const parser::IntrinsicTypeSpec::Complex &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(Complex)\n";
   std::cout << "TYPE IS : Complex\n";
}

void Build(const parser::IntrinsicTypeSpec::Character &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(Character)\n";
   std::cout << "TYPE IS : Character\n";
}

void Build(const parser::IntrinsicTypeSpec::Logical &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(Logical)\n";
   std::cout << "TYPE IS : Logical\n";
}

void Build(const parser::IntrinsicTypeSpec::DoubleComplex &x, SgType* &type)
{
   std::cout << "Rose::builder::Build(DoubleComplex)\n";
   std::cout << "TYPE IS : DoubleComplex\n";
}

void Build(const std::list<Fortran::parser::EntityDecl> &x, std::string &name)
{
   std::cout << "Rose::builder::Build(std::list) for EntityDecl\n";

   for (const auto &elem : x) {
      Build(elem, name);
   }
}

void Build(const parser::EntityDecl &x, std::string &name)
{
   //  std::tuple<ObjectName, std::optional<ArraySpec>, std::optional<CoarraySpec>,
   //      std::optional<CharLength>, std::optional<Initialization>>

   std::cout << "Rose::builder::Build(EntityDecl)\n";
   name = std::get<0>(x.t).ToString();
   std::cout << "The object name is: " << name << std::endl;

   SgScopeStatement *scope = nullptr;
   if (auto & opt = std::get<1>(x.t)) {    // ArraySpec
      Build(opt.value(), scope);
   }

   if (auto & opt = std::get<2>(x.t)) {    // CoarraySpec
      Build(opt.value(), scope);
   }

   if (auto & opt = std::get<3>(x.t)) {    // CharLength
      Build(opt.value(), scope);
   }

   if (auto & opt = std::get<4>(x.t)) {    // Initialization
      Build(opt.value(), scope);
   }
}

   // EntityDecl
template<typename T>
void Build(const parser::ArraySpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ArraySpec)\n";

   SgExpression* expr = nullptr;

   // std::list<ExplicitShapeSpec>, std::list<AssumedShapeSpec>, DeferredShapeSpecList,
   // AssumedSizeSpec, ImpliedShapeSpec, AssumedRankSpec
   auto ArraySpecVisitor = [&](const auto& y) { Build(y, expr); };
   std::visit(ArraySpecVisitor, x.u);
}

template<typename T>
void Build(const parser::CoarraySpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(CoarraySpec)\n";
}

template<typename T>
void Build(const parser::CharLength &x, T* scope)
{
   std::cout << "Rose::builder::Build(CharLength)\n";
}

template<typename T>
void Build(const parser::Initialization &x, T* scope)
{
   std::cout << "Rose::builder::Build(Initialization)\n";
}

   // ArraySpec

void Build(const parser::ExplicitShapeSpec &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ExplicitShapeSpec)\n";

   if (auto & specExpr1 = std::get<0>(x.t)) {
      Build(specExpr1.value(), expr);      // 1st SpecificationExpr (optional)
   }

   Build(std::get<1>(x.t), expr);          // 2nd SpecificationExpr
}

void Build(const parser::AssumedShapeSpec &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(AssumedShapeSpec)\n";
}

void Build(const parser::DeferredShapeSpecList &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(DeferredShapeSpecList)\n";
}

void Build(const parser::AssumedSizeSpec &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(AssumedSizeSpec)\n";
}

void Build(const parser::ImpliedShapeSpec &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ImpliedShapeSpec)\n";
}

void Build(const parser::AssumedRankSpec &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(AssumedRankSpec)\n";
}

void Build(const parser::SpecificationExpr &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(SpecificationExpr)\n";

   Build(x.v, expr);  // Scalar<IntExpr>
}

void Build(const parser::Scalar<parser::IntExpr> &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Scalar<IntExpr>)\n";

   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), expr);  // Expr
}

void Build(const parser::Scalar<parser::LogicalExpr>&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Scalar<LogicalExpr>)\n";

   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), expr);  // Expr
}

void Build(const parser::ConstantExpr &x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ConstantExpr)\n";

   // Constant<common::Indirection<Expr>>
   Build(x.thing.value(), expr);  // Expr
}

   // DeclarationConstruct

template<typename T>
void Build(const parser::DataStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(DataStmt)\n";
}

template<typename T>
void Build(const parser::FormatStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(FormatStmt)\n";
}

template<typename T>
void Build(const parser::EntryStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(EntryStmt)\n";
}

template<typename T>
void Build(const parser::StmtFunctionStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(StmtFunctionStmt)\n";
}

template<typename T>
void Build(const parser::ErrorRecovery &x, T* scope)
{
   std::cout << "Rose::builder::Build(ErrorRecovery)\n";
}

   // ActionStmt
template<typename T>
void Build(const parser::ContinueStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ContinueStmt)\n";
}

template<typename T>
void Build(const parser::FailImageStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(FailImageStmt)\n";
}

template<typename T>
void Build(const parser::AllocateStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(AllocateStmt)\n";
}

template<typename T>
void Build(const parser::BackspaceStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(BackspaceStmt)\n";
}

template<typename T>
void Build(const parser::CallStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(CallStmt)\n";
}

template<typename T>
void Build(const parser::CloseStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(CloseStmt)\n";
}

template<typename T>
void Build(const parser::CycleStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(CycleStmt)\n";
}

template<typename T>
void Build(const parser::DeallocateStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(DeallocateStmt)\n";
}

template<typename T>
void Build(const parser::EndfileStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(EndfileStmt)\n";
}

template<typename T>
void Build(const parser::EventPostStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(EventPostStmt)\n";
}

template<typename T>
void Build(const parser::EventWaitStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(EventWaitStmt)\n";
}

template<typename T>
void Build(const parser::ExitStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ExitStmt)\n";
}

template<typename T>
void Build(const parser::FlushStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(FlustStmt)\n";
}

template<typename T>
void Build(const parser::FormTeamStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(FormTeamStmt)\n";
}

template<typename T>
void Build(const parser::GotoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(GotoStmt)\n";
}

template<typename T>
void Build(const parser::IfStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(IfStmt)\n";
   //  std::tuple<ScalarLogicalExpr, UnlabeledStatement<ActionStmt>> t;

   SgExpression* expr = nullptr;

   Build(std::get<0>(x.t), expr);
   Build(std::get<1>(x.t).statement, scope);

}

template<typename T>
void Build(const parser::InquireStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(InquireStmt)\n";
}

template<typename T>
void Build(const parser::LockStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(LockStmt)\n";
}

template<typename T>
void Build(const parser::NullifyStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(NullifyStmt)\n";
}

template<typename T>
void Build(const parser::OpenStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(OpenStmt)\n";
}

template<typename T>
void Build(const parser::PointerAssignmentStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(PointerAssignmentStmt)\n";
}

template<typename T>
void Build(const parser::PrintStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(PrintStmt)\n";

   Build(std::get<0>(x.t), scope);   // Format
   Build(std::get<1>(x.t), scope);   // OutputItem
}

template<typename T>
void Build(const parser::Format&x, T* scope)
{
   std::cout << "Rose::builder::Build(Format)\n";

   //   auto FormatVisitor = [&] (const auto &y) { Build(y, scope); };
   //   std::visit(FormatVisitor, x.u);   // DefaultCharExpr, Label, Star
}

template<typename T>
void Build(const parser::DefaultCharExpr&x, T* scope)
{
   std::cout << "Rose::builder::Build(DefaultCharExpr)\n";
}

template<typename T>
void Build(const parser::Label&x, T* scope)
{
   std::cout << "Rose::builder::Build(Label)\n";
}

template<typename T>
void Build(const parser::Star&x, T* scope)
{
   std::cout << "Rose::builder::Build(Star)\n";
}

template<typename T>
void Build(const parser::OutputItem&x, T* scope)
{
   std::cout << "Rose::builder::Build(OutputItem)\n";

   SgExpression* expr = nullptr;

   std::visit(
      common::visitors{
         [&] (const common::Indirection<parser::OutputImpliedDo> &y) { ; },
         [&] (const auto &y) { Build(y, expr); }   // Expr
      },
      x.u);
}

template<typename T>
void Build(const parser::OutputImpliedDo&x, T* scope)
{
   std::cout << "Rose::builder::Build(OutputImpliedDo)\n";
}

template<typename T>
void Build(const parser::ReadStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ReadStmt)\n";
}

template<typename T>
void Build(const parser::ReturnStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ReturnStmt)\n";
}

template<typename T>
void Build(const parser::RewindStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(RewindStmt)\n";
}

template<typename T>
void Build(const parser::StopStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(StopStmt)\n";
}

template<typename T>
void Build(const parser::SyncAllStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncAllStmt)\n";
}

template<typename T>
void Build(const parser::SyncImagesStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncImagesStmt)\n";
}

template<typename T>
void Build(const parser::SyncMemoryStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncMemoryStmt)\n";
}

template<typename T>
void Build(const parser::SyncTeamStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncTeamStmt)\n";
}

template<typename T>
void Build(const parser::UnlockStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(UnlockStmt)\n";
}

template<typename T>
void Build(const parser::WaitStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(WaitStmt)\n";
}

template<typename T>
void Build(const parser::WhereStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(WhereStmt)\n";
}

template<typename T>
void Build(const parser::WriteStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(WriteStmt)\n";
}

template<typename T>
void Build(const parser::ComputedGotoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ComputedGotoStmt)\n";
}

template<typename T>
void Build(const parser::ForallStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ForallStmt)\n";
}

template<typename T>
void Build(const parser::ArithmeticIfStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ArithmeticIfStmt)\n";
}

template<typename T>
void Build(const parser::AssignStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(AssignStmt)\n";
}

template<typename T>
void Build(const parser::AssignedGotoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(AssignedGotoStmt)\n";
}

template<typename T>
void Build(const parser::PauseStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(PauseStmt)\n";
}

template<typename T>
void Build(const parser::NamelistStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(NamelistStmt)\n";
}

template<typename T>
void Build(const parser::ParameterStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ParameterStmt)\n";
}

template<typename T>
void Build(const parser::OldParameterStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(OldParameterStmt)\n";
}

   // Expr
template<typename T>
void Build(const parser::CharLiteralConstantSubstring&x, T* &expr)
{
   std::cout << "Rose::builder::Build(CharLiteralConstantSubstring)\n";
}

template<typename T>
void Build(const parser::ArrayConstructor&x, T* &expr)
{
   std::cout << "Rose::builder::Build(ArrayConstructor)\n";
}

template<typename T>
void Build(const parser::StructureConstructor&x, T* &expr)
{
   std::cout << "Rose::builder::Build(StructureConstructor)\n";
}

template<typename T>
void Build(const parser::Expr::Parentheses&x, T* &expr)
{
   std::cout << "Rose::builder::Build(Parentheses)\n";
}

template<typename T>
void Build(const parser::Expr::UnaryPlus&x, T* &expr)
{
   std::cout << "Rose::builder::Build(UnaryPlus)\n";
}

template<typename T>
void Build(const parser::Expr::Negate&x, T* &expr)
{
   std::cout << "Rose::builder::Build(Negate)\n";
}

template<typename T>
void Build(const parser::Expr::NOT&x, T* &expr)
{
   std::cout << "Rose::builder::Build(NOT)\n";
}

template<typename T>
void Build(const parser::Expr::PercentLoc&x, T* &expr)
{
   std::cout << "Rose::builder::Build(PercentLoc)\n";
}

template<typename T>
void Build(const parser::Expr::DefinedUnary&x, T* &expr)
{
   std::cout << "Rose::builder::Build(DefinedUnary)\n";
}

void Build(const parser::Expr::Power&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Power)\n";
}

void Build(const parser::Expr::Multiply&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Multiply)\n";

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildMultiplyOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::Divide&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Divide)\n";

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildDivideOp_nfi(lhs, rhs);
}

template<typename T>
void traverseBinaryExprs(const T &x, SgExpression* &lhs, SgExpression* &rhs)
{
   std::cout << "Rose::builder::traverseBinaryExprs\n";

   Build(std::get<0>(x.t).value(), lhs); // lhs Expr
   Build(std::get<1>(x.t).value(), rhs); // rhs Expr
}

void Build(const parser::Expr::Add&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Add)\n";

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildAddOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::Subtract&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(Subtract)\n";

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildSubtractOp_nfi(lhs, rhs);
}

template<typename T>
void Build(const parser::Expr::Concat&x, T* &expr)
{
   std::cout << "Rose::builder::Build(Concat)\n";
}

template<typename T>
void Build(const parser::Expr::LT&x, T* &expr)
{
   std::cout << "Rose::builder::Build(LT)\n";
}

template<typename T>
void Build(const parser::Expr::LE&x, T* &expr)
{
   std::cout << "Rose::builder::Build(LE)\n";
}

template<typename T>
void Build(const parser::Expr::EQ&x, T* &expr)
{
   std::cout << "Rose::builder::Build(EQ)\n";
}

template<typename T>
void Build(const parser::Expr::NE&x, T* &expr)
{
   std::cout << "Rose::builder::Build(NE)\n";
}

template<typename T>
void Build(const parser::Expr::GE&x, T* &expr)
{
   std::cout << "Rose::builder::Build(GE)\n";
}

template<typename T>
void Build(const parser::Expr::GT&x, T* &expr)
{
   std::cout << "Rose::builder::Build(GT)\n";
}

template<typename T>
void Build(const parser::Expr::AND&x, T* &expr)
{
   std::cout << "Rose::builder::Build(AND)\n";
}

template<typename T>
void Build(const parser::Expr::OR&x, T* &expr)
{
   std::cout << "Rose::builder::Build(OR)\n";
}

template<typename T>
void Build(const parser::Expr::EQV&x, T* &expr)
{
   std::cout << "Rose::builder::Build(EQV)\n";
}

template<typename T>
void Build(const parser::Expr::NEQV&x, T* &expr)
{
   std::cout << "Rose::builder::Build(NEQV)\n";
}

template<typename T>
void Build(const parser::Expr::DefinedBinary&x, T* &expr)
{
   std::cout << "Rose::builder::Build(DefinedBinary)\n";
}

template<typename T>
void Build(const parser::Expr::ComplexConstructor&x, T* &expr)
{
   std::cout << "Rose::builder::Build(ComplexConstructor)\n";
}

void Build(const parser::StructureComponent&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(StructureComponent)\n";
}

void Build(const parser::ArrayElement&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ArrayElement)\n";
   Build(x.base, expr);        // DataRef
   Build(x.subscripts, expr);  // std::list<SectionSubscript>
}

void Build(const parser::CoindexedNamedObject&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(CoindexedNamedObject)\n";
   Build(x.base, expr);          // DataRef
   Build(x.imageSelector, expr); // ImageSelector
}

void Build(const parser::ImageSelector&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ImageSelector)\n";

   Build(std::get<0>(x.t), expr);  // std::list<Cosubscript> - Cosubscript = Scalar<IntExpr>
   Build(std::get<1>(x.t), expr);  // std::list<ImageSelectorSpec>
}

void Build(const parser::ImageSelectorSpec&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(ImageSelectorSpec)\n";

   // Stat, TeamValue, Team_Number
   //   auto ImageSelectorSpecVisitor = [&] (const auto &y) { Build(y, expr); };
   //   std::visit(ImageSelectorSpecVisitor, x.u);
}

void Build(const parser::SectionSubscript&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(SectionSubscript)\n";

   std::visit(
      common::visitors{
         [&](const parser::Integer<common::Indirection<parser::Expr>>  &y) { Build(y.thing.value(), expr); },
         [&](const parser::SubscriptTriplet &y) { Build(y, expr); },
      },
      x.u);
}

void Build(const parser::SubscriptTriplet&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(SubscriptTriplet)\n";
}

   // ExecutableConstruct
template<typename T>
void Build(const parser::AssociateConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(AssociateConstruct)\n";
}

template<typename T>
void Build(const parser::BlockConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(BlockConstruct)\n";
   //   std::tuple<Statement<BlockStmt>, BlockSpecificationPart, Block,
   //      Statement<EndBlockStmt>>

   const parser::BlockStmt &block_stmt = std::get<0>(x.t).statement;
   //   std::optional<Name> v
   const parser::Name block_name = block_stmt.v.value();

   std::cout << "block name is " << block_name.ToString() << "\n";

   semantics::Symbol *symbol = block_name.symbol;

   if (symbol) {
      const parser::CharBlock &srcName = symbol->name();
      std::cout << "The symbol name is " << srcName.ToString() << "\n";

      semantics::Attrs &attrs = symbol->attrs();
      std::cout << "The attrs is " << &attrs << "\n";

      const semantics::Scope &owner_scope = symbol->owner();
      semantics::Scope *introduced_scope = symbol->scope();

      const semantics::Symbol *owner_scope_symbol = owner_scope.symbol();

      if (owner_scope_symbol) {
         std::string owner_scope_name = owner_scope_symbol->name().ToString();
         std::cout << "owner scope name is " << owner_scope_name << "\n";
      }

      semantics::Scope::Kind owner_scope_kind = owner_scope.kind();
      std::cout << "owner scope kind is " << semantics::Scope::EnumToString(owner_scope_kind) << "\n";

      const semantics::Scope &owner_scope_parent = owner_scope.parent();
      semantics::Scope::Kind owner_scope_parent_kind = owner_scope_parent.kind();
      std::cout << "owner scope parent kind is " << semantics::Scope::EnumToString(owner_scope_parent_kind) << "\n";

      if (introduced_scope) {
         semantics::Scope::Kind introduced_scope_kind = introduced_scope->kind();
         std::cout << "introduced scope kind is " << semantics::Scope::EnumToString(introduced_scope_kind) << "\n";
      }
      else {
         std::cout << "No scope was introduced by " << block_name.ToString() << "()\n";
      }
   }

   const auto &block = std::get<2>(x.t);
   std::cout << "The block is of type: " << typeid(block).name() << "\n";

   Build(block, scope);
}

template<typename T>
void Build(const parser::CaseConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(CaseConstruct)\n";

// Statements in the CaseConstruct
   const parser::SelectCaseStmt &select_case_stmt = std::get<0>(x.t).statement;
   const parser::EndSelectStmt   &end_select_stmt = std::get<2>(x.t).statement;
   // will want to deal with end select stmt if it has a name

   const std::optional<parser::Name> &name = std::get<0>(select_case_stmt.t);

   SgExpression* expr{nullptr};
   Build(std::get<1>(select_case_stmt.t).thing, expr);

   SgSwitchStatement* switch_stmt{nullptr};
   Rose::builder::SourcePositionPair sources;

   // Begin SageTreeBuilder
   builder.Enter(switch_stmt, expr, sources);

   // Traverse body of the CaseConstruct
   SgStatement* case_construct{nullptr};
   Build(std::get<1>(x.t), case_construct);

   // Finish SageTreeBuilder
   builder.Leave(switch_stmt);
}

void Build(const parser::CaseConstruct::Case&x, SgStatement* &stmt)
{
   std::cout << "Rose::builder::Build(Case)\n";
   // std::tuple<Statement<CaseStmt>, Block> t;

   SgStatement*                  block_stmt{nullptr};
   SgCaseOptionStmt*       case_option_stmt{nullptr};
   SgDefaultOptionStmt* default_option_stmt{nullptr};
   SgExprListExp*              sg_case_list{nullptr};
   std::list<SgExpression*> case_list;

   // Traverse CaseStmt
   Build(std::get<0>(x.t).statement, case_list);
   bool is_default = case_list.empty();

   // Begin SageTreeBuilder
   if (is_default) {
      builder.Enter(default_option_stmt);
   } else {
      sg_case_list = SageBuilderCpp17::buildExprListExp_nfi(case_list);
      builder.Enter(case_option_stmt, sg_case_list);
   }

   // Traverse Block
   Build(std::get<1>(x.t), block_stmt);

   // End SageTreeBuilder
   if (is_default) {
      builder.Leave(default_option_stmt);
   } else {
      builder.Leave(case_option_stmt);
   }
}

void Build(const parser::CaseStmt&x, std::list<SgExpression*> &case_list)
{
   std::cout << "Rose::builder::Build(CaseStmt)\n";
   //  std::tuple<CaseSelector, std::optional<Name>> t;

   Build(std::get<0>(x.t), case_list);
}

void Build(const parser::CaseSelector&x, std::list<SgExpression*> &case_list)
{
   std::cout << "Rose::builder::Build(CaseSelector)\n";
   //  std::variant<std::list<CaseValueRange>, Default> u;

   std::visit(
      common::visitors{
         [&] (const parser::Default &y) { ; },
         [&] (const auto &y) { Build(y, case_list); }, // CaseValueRange
      },
      x.u);
}

void Build(const std::list<parser::CaseValueRange> &x, std::list<SgExpression*> &case_list)
{
   std::cout << "Rose::builder::Build(std::list) for CaseValueRange \n";

   for (const auto &elem : x) {
      SgExpression* case_expr = nullptr;
      Build(elem, case_expr);
      case_list.push_back(case_expr);
   }
}

void Build(const parser::CaseValueRange&x, SgExpression* &expr)
{
   std::cout << "Rose::builder::Build(CaseValueRange)\n";
   //  std::variant<CaseValue, Range> u;

   std::visit(
      common::visitors{
         [&] (const parser::CaseValue &y) { Build(y.thing, expr); },  // using CaseValue = Scalar<ConstantExpr>;
         [&] (const parser::CaseValueRange::Range &y) { Build(y, expr); },
      },
      x.u);
}

void Build(const parser::CaseValueRange::Range&x, SgExpression* &range)
{
   std::cout << "Rose::builder::Build(Range)\n";
   //    std::optional<CaseValue> lower, upper;

   auto & lower_expr = x.lower;
   auto & upper_expr = x.upper;

   SgExpression * lower = nullptr, * upper = nullptr;

   if (lower_expr) {
      Build(lower_expr->thing, lower);
   } else {
      lower = SageBuilderCpp17::buildNullExpression_nfi();
   }

   if (upper_expr) {
      Build(upper_expr->thing, upper);
   } else {
      upper = SageBuilderCpp17::buildNullExpression_nfi();
   }

   SgExpression* stride = SageBuilderCpp17::buildIntVal_nfi(1);
   range = SageBuilderCpp17::buildSubscriptExpression_nfi(lower, upper, stride);
}

template<typename T>
void Build(const parser::ChangeTeamConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(ChangeTeamConstruct)\n";
}

template<typename T>
void Build(const parser::CriticalConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(CriticalConstruct)\n";
}

template<typename T>
void Build(const parser::LabelDoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(LabelDoStmt)\n";
}

template<typename T>
void Build(const parser::EndDoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(EndDoStmt)\n";
}

template<typename T>
void Build(const parser::DoConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(DoConstruct)\n";
}

template<typename T>
void Build(const parser::IfConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(IfConstruct)\n";
}

template<typename T>
void Build(const parser::SelectRankConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(SelectRankConstruct)\n";
}

template<typename T>
void Build(const parser::SelectTypeConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(SelectTypeConstruct)\n";
}

template<typename T>
void Build(const parser::WhereConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(WhereConstruct)\n";
}

template<typename T>
void Build(const parser::ForallConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(ForallConstruct)\n";
}

template<typename T>
void Build(const parser::CompilerDirective&x, T* scope)
{
   std::cout << "Rose::builder::Build(CompilerDirective)\n";
}

template<typename T>
void Build(const parser::OpenMPConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(OpenMPConstruct)\n";
}

template<typename T>
void Build(const parser::OmpEndLoopDirective&x, T* scope)
{
   std::cout << "Rose::builder::Build(OmpEndLoopDirective)\n";
}

   // SpecificationConstruct
template<typename T>
void Build(const parser::DerivedTypeDef&x, T* scope)
{
   std::cout << "Rose::builder::Build(DerivedTypeDef)\n";

#if 0
   std::tuple<Statement<DerivedTypeStmt>,
              std::list<Statement<TypeParamDefStmt>>,
              std::list<Statement<PrivateOrSequence>>,
              std::list<Statement<ComponentDefStmt>>,
              std::optional<TypeBoundProcedurePart>,
              Statement<EndTypeStmt>>
      t;
#endif

   const auto & stmt    {std::get<0>(x.t)};
   const auto & end_stmt{std::get<5>(x.t)};

   std::string type_name{std::get<1>(stmt.statement.t).ToString()};

   SgDerivedTypeStatement* derived_type_stmt{nullptr};
// TODO
#if FORTRAN
   builder.Enter(derived_type_stmt, type_name);

   builder.Leave(derived_type_stmt);
#endif
}

template<typename T>
void Build(const parser::EnumDef&x, T* scope)
{
   std::cout << "Rose::builder::Build(EnumDef)\n";
}

template<typename T>
void Build(const parser::InterfaceBlock&x, T* scope)
{
   std::cout << "Rose::builder::Build(InterfaceBlock)\n";
}

template<typename T>
void Build(const parser::StructureDef&x, T* scope)
{
   std::cout << "Rose::builder::Build(StructureDef)\n";
}

template<typename T>
void Build(const parser::OtherSpecificationStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(OtherSpecificationStmt)\n";
}

template<typename T>
void Build(const parser::GenericStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(GenericStmt)\n";
}

template<typename T>
void Build(const parser::ProcedureDeclarationStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ProcedureDeclarationStmt)\n";
}

template<typename T>
void Build(const parser::OpenMPDeclarativeConstruct&x, T* scope)
{
   std::cout << "Rose::builder::Build(OpenMPDeclarativeConstruct)\n";
}

   // AttrSpec
template<typename T>
void Build(const parser::AccessSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(AccessSpec)\n";
}

template<typename T>
void Build(const parser::Allocatable &x, T* scope)
{
   std::cout << "Rose::builder::Build(Allocatable)\n";
}

template<typename T>
void Build(const parser::Asynchronous &x, T* scope)
{
   std::cout << "Rose::builder::Build(Asynchronous)\n";
}

template<typename T>
void Build(const parser::Contiguous &x, T* scope)
{
   std::cout << "Rose::builder::Build(Contiguous)\n";
}

template<typename T>
void Build(const parser::External &x, T* scope)
{
   std::cout << "Rose::builder::Build(External)\n";
}

template<typename T>
void Build(const parser::IntentSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntentSpec)\n";
}

template<typename T>
void Build(const parser::Intrinsic &x, T* scope)
{
   std::cout << "Rose::builder::Build(Intrinsic)\n";
}

template<typename T>
void Build(const parser::LanguageBindingSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(LanguageBindingSpec)\n";
}

template<typename T>
void Build(const parser::Optional &x, T* scope)
{
   std::cout << "Rose::builder::Build(Optional)\n";
}

template<typename T>
void Build(const parser::Parameter &x, T* scope)
{
   std::cout << "Rose::builder::Build(Parameter)\n";
}

template<typename T>
void Build(const parser::Pointer &x, T* scope)
{
   std::cout << "Rose::builder::Build(Pointer)\n";
}

template<typename T>
void Build(const parser::Protected &x, T* scope)
{
   std::cout << "Rose::builder::Build(Protected)\n";
}

template<typename T>
void Build(const parser::Save &x, T* scope)
{
   std::cout << "Rose::builder::Build(Save)\n";
}

template<typename T>
void Build(const parser::Target &x, T* scope)
{
   std::cout << "Rose::builder::Build(Target)\n";
}

template<typename T>
void Build(const parser::Value &x, T* scope)
{
   std::cout << "Rose::builder::Build(Value)\n";
}

template<typename T>
void Build(const parser::Volatile &x, T* scope)
{
   std::cout << "Rose::builder::Build(Volatile)\n";
}

} // namespace Rose::builder
