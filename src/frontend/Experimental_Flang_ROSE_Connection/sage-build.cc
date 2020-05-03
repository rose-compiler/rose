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

   auto & variable = std::get<0>(x.t);
   Build(variable, scope);

   auto & expr = std::get<1>(x.t);
   Build(expr, scope);
}

template<typename T>
void Build(const parser::Variable &x, T* scope)
{
   std::cout << "Rose::builder::Build(Variable)\n";

   // Designator, FunctionReference
   auto VariableVisitor = [&](const auto& y) { Build(y.value(), scope); };
   std::visit(VariableVisitor, x.u);
}

template<typename T>
void Build(const parser::Designator &x, T* scope)
{
   std::cout << "Rose::builder::Build(Designator)\n";

   // DataRef, Substring
   auto DesignatorVisitor = [&](const auto& y) { Build(y, scope); };
   std::visit(DesignatorVisitor, x.u);
}

template<typename T>
void Build(const parser::DataRef &x, T* scope)
{
   std::cout << "Rose::builder::Build(DataRef)\n";

   std::visit(
      common::visitors{
         [&] (const Fortran::parser::Name &y) {
            std::string name = std::get<0>(x.u).ToString();
            std::cout << "The name of the DataRef is " << name << std::endl;

            semantics::Symbol *symbol = y.symbol;

            if (symbol) {
               const parser::CharBlock &srcName = symbol->name();
               std::cout << "The symbol name is " << srcName.ToString() << "\n";

               semantics::Attrs &attrs = symbol->attrs();
               std::cout << "The attrs is " << attrs << "\n";

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
          },
         // StructureComponent, ArrayElement, or CoindexedNamedObject
         [&] (const auto &y) { Build(y.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::Substring &x, T* scope)
{
   std::cout << "Rose::builder::Build(Substring)\n";
}

template<typename T>
void Build(const parser::FunctionReference &x, T* scope)
{
   //Call v;
   //  Designator ConvertToArrayElementRef();
   //  StructureConstructor ConvertToStructureConstructor(
   //      const semantics::DerivedTypeSpec &);
   std::cout << "Rose::builder::Build(FunctionReference)\n";

   Build(x.v, scope); // Call

   //   const auto & arrRef = x.ConvertToArrayElementRef();
}

template<typename T>
void Build(const parser::Call &x, T* scope)
{
   std::cout << "Rose::builder::Build(Call)\n";

   const parser::CharBlock source = x.source;

   Build(std::get<0>(x.t), scope);   // ProcedureDesignator
   Build(std::get<1>(x.t), scope);   // std::list<ActualArgSpec>
}

template<typename T>
void Build(const parser::ProcedureDesignator &x, T* scope)
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
            std::cout << "The attrs is " << attrs << "\n";

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
            std::cout << "details is " << details << "\n";
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
         [&] (const auto &y) { Build(y, scope); }   // ProcComponentRef
      },
      x.u);
}

template<typename T>
void Build(const parser::ProcComponentRef &x, T* scope)
{
   std::cout << "Rose::builder::Build(ProcComponentRef)\n";
}

template<typename T>
void Build(const parser::ActualArgSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ActualArgSpec)\n";

   if (auto & opt = std::get<0>(x.t)) {    // std::optional<Keyword>
      Build(opt.value(), scope);
   }

   Build(std::get<1>(x.t), scope);         // ActualArg
}

template<typename T>
void Build(const parser::ActualArg &x, T* scope)
{
   std::cout << "Rose::builder::Build(ActualArg)\n";
   //  std::variant<common::Indirection<Expr>, AltReturnSpec, PercentRef, PercentVal>

   std::visit(
      common::visitors{
         [&] (const common::Indirection<parser::Expr> &y) { Build(y.value(), scope); },
         [&] (const auto &y) { }   // AltReturnSpec, PercentRef, PercentVal
      },
      x.u);
}

template<typename T>
void Build(const parser::Keyword &x, T* scope)
{
   std::cout << "Rose::builder::Build(Keyword)\n";
}

template<typename T>
void Build(const parser::Name &x, T* scope)
{
   std::cout << "Rose::builder::Build(Name)\n";
}

template<typename T>
void Build(const parser::Expr &x, T* scope)
{
   std::cout << "Rose::builder::Build(Expr)\n";

   std::visit(
      common::visitors{
         [&](const Fortran::common::Indirection<parser::CharLiteralConstantSubstring> &y)
               { Build(y.value(), scope); },
         [&](const Fortran::common::Indirection<parser::Designator> &y)
               { Build(y.value(), scope); },
         [&](const Fortran::common::Indirection<parser::FunctionReference> &y)
               { Build(y.value(), scope); },
         // LiteralConstant, ArrayConstructor, StructureConstructor, Parentheses, UnaryPlus,
         // Negate, NOT, PercentLoc, DefinedUnary, Power, Multiply, Divide, Add, Subtract, Concat
         // LT, LE, EQ, NE, GE, GT, AND, OR, EQV, NEQV, XOR, DefinedBinary, ComplexConstructor
         [&](const auto &y) { Build(y, scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::Expr::IntrinsicBinary &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntrinsicBinary)\n";
}

template<typename T>
void Build(const parser::LiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(LiteralConstant)\n";

   //   Build(std::get<1>(x.u), scope);

   //  HollerithLiteralConstant, IntLiteralConstant, RealLiteralConstant, ComplexLiteralConstant,
   //  BOZLiteralConstant, CharLiteralConstant, LogicalLiteralConstant
   auto LiteralConstVisitor = [&] (const auto &y) { Build(y, scope); };
   std::visit(LiteralConstVisitor, x.u);
}

   // LiteralConstant
template<typename T>
void Build(const parser::HollerithLiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(HollerithLiteralConstant)\n";
}

template<typename T>
void Build(const parser::IntLiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntLiteralConstant)\n";
   int literal = stoi(std::get<0>(x.t).ToString());
   std::cout << " The INTEGER LITERAL CONSTANT is " << literal << std::endl;
}

template<typename T>
void Build(const parser::RealLiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(RealLiteralConstant)\n";
}

template<typename T>
void Build(const parser::ComplexLiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(ComplexLiteralConstant)\n";
}

template<typename T>
void Build(const parser::BOZLiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(BOZLiteralConstant)\n";
}

template<typename T>
void Build(const parser::CharLiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(CharLiteralConstant)\n";
   std::string literal = x.GetString();
   std::cout << " The CHAR LITERAL CONSTANT is \"" << literal << "\"" << std::endl;
}

template<typename T>
void Build(const parser::LogicalLiteralConstant &x, T* scope)
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

   Build(std::get<0>(x.t), scope);    // DeclarationTypeSpec
   Build(std::get<1>(x.t), scope);    // std::list<AttrSpec>
   Build(std::get<2>(x.t), scope);    // std::list<EntityDecl>
}

template<typename T>
void Build(const parser::DeclarationTypeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(DeclarationTypeSpec)\n";

   // IntrinsicTypeSpec, Type, TypeStar, Class, ClassStar, Record
   auto DeclTypeSpecVisitor = [&] (const auto &y) { Build(y, scope); };
   std::visit(DeclTypeSpecVisitor, x.u);
}

   // DeclarationTypeSpec
template<typename T>
void Build(const parser::DeclarationTypeSpec::Type&x, T* scope)
{
   std::cout << "Rose::builder::Build(Type)\n";
   Build(x.derived, scope);   // DerivedTypeSpec
}

template<typename T>
void Build(const parser::DeclarationTypeSpec::TypeStar&x, T* scope)
{
   std::cout << "Rose::builder::Build(TypeStar)\n";
}

template<typename T>
void Build(const parser::DeclarationTypeSpec::Class&x, T* scope)
{
   std::cout << "Rose::builder::Build(Class)\n";
   Build(x.derived, scope);   // DerivedTypeSpec
}

template<typename T>
void Build(const parser::DeclarationTypeSpec::ClassStar&x, T* scope)
{
   std::cout << "Rose::builder::Build(ClassStar)\n";
}

template<typename T>
void Build(const parser::DeclarationTypeSpec::Record&x, T* scope)
{
   std::cout << "Rose::builder::Build(Record)\n";
}

template<typename T>
void Build(const parser::DerivedTypeSpec&x, T* scope)
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

template<typename T>
void Build(const parser::IntrinsicTypeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntrinsicTypeSpec)\n";

   // IntegerTypeSpec, Real, DoublePrecision, Complex, Character, Logical, DoubleComplex
   auto TypeVisitor = [&](const auto& y) { Build(y, scope); };
   std::visit(TypeVisitor, x.u);
}

template<typename T>
void Build(const parser::IntegerTypeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntegerTypeSpec)\n";
   std::cout << "TYPE IS : Integer\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Real &x, T* scope)
{
   std::cout << "Rose::builder::Build(Real)\n";
   std::cout << "TYPE IS : Real\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::DoublePrecision &x, T* scope)
{
   std::cout << "Rose::builder::Build(DoublePrecision)\n";
   std::cout << "TYPE IS : DoublePrecision\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Complex &x, T* scope)
{
   std::cout << "Rose::builder::Build(Complex)\n";
   std::cout << "TYPE IS : Complex\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Character &x, T* scope)
{
   std::cout << "Rose::builder::Build(Character)\n";
   std::cout << "TYPE IS : Character\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Logical &x, T* scope)
{
   std::cout << "Rose::builder::Build(Logical)\n";
   std::cout << "TYPE IS : Logical\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::DoubleComplex &x, T* scope)
{
   std::cout << "Rose::builder::Build(DoubleComplex)\n";
   std::cout << "TYPE IS : DoubleComplex\n";
}

template<typename T>
void Build(const parser::EntityDecl &x, T* scope)
{
   //  std::tuple<ObjectName, std::optional<ArraySpec>, std::optional<CoarraySpec>,
   //      std::optional<CharLength>, std::optional<Initialization>>

   std::cout << "Rose::builder::Build(EntityDecl)\n";
   std::string name = std::get<0>(x.t).ToString();
   std::cout << "The object name is: " << name << std::endl;

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

   // std::list<ExplicitShapeSpec>, std::list<AssumedShapeSpec>, DeferredShapeSpecList,
   // AssumedSizeSpec, ImpliedShapeSpec, AssumedRankSpec
   auto ArraySpecVisitor = [&](const auto& y) { Build(y, scope); };
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

template<typename T>
void Build(const parser::ExplicitShapeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExplicitShapeSpec)\n";

   if (auto & specExpr1 = std::get<0>(x.t)) {
      Build(specExpr1.value(), scope);      // 1st SpecificationExpr (optional)
   }

   Build(std::get<1>(x.t), scope);          // 2nd SpecificationExpr
}

template<typename T>
void Build(const parser::AssumedShapeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(AssumedShapeSpec)\n";
}

template<typename T>
void Build(const parser::DeferredShapeSpecList &x, T* scope)
{
   std::cout << "Rose::builder::Build(DeferredShapeSpecList)\n";
}

template<typename T>
void Build(const parser::AssumedSizeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(AssumedSizeSpec)\n";
}

template<typename T>
void Build(const parser::ImpliedShapeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImpliedShapeSpec)\n";
}

template<typename T>
void Build(const parser::AssumedRankSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(AssumedRankSpec)\n";
}

template<typename T>
void Build(const parser::SpecificationExpr &x, T* scope)
{
   std::cout << "Rose::builder::Build(SpecificationExpr)\n";

   Build(x.v, scope);  // Scalar<IntExpr>
}

template<typename T>
void Build(const parser::Scalar<parser::IntExpr> &x, T* scope)
{
   std::cout << "Rose::builder::Build(Scalar<IntExpr>)\n";

   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), scope);  // Expr
}


template<typename T>
void Build(const parser::Scalar<parser::LogicalExpr>&x, T* scope)
{
   std::cout << "Rose::builder::Build(Scalar<LogicalExpr>)\n";

   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), scope);  // Expr
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

   Build(std::get<0>(x.t), scope);
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

   auto FormatVisitor = [&] (const auto &y) { Build(y, scope); };
   std::visit(FormatVisitor, x.u);   // DefaultCharExpr, Label, Star
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

   std::visit(
      common::visitors{
         [&] (const common::Indirection<parser::OutputImpliedDo> &y) { ; },
         [&] (const auto &y) { Build(y, scope); }   // Expr
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
void Build(const parser::CharLiteralConstantSubstring&x, T* scope)
{
   std::cout << "Rose::builder::Build(CharLiteralConstantSubstring)\n";
}

template<typename T>
void Build(const parser::ArrayConstructor&x, T* scope)
{
   std::cout << "Rose::builder::Build(ArrayConstructor)\n";
}

template<typename T>
void Build(const parser::StructureConstructor&x, T* scope)
{
   std::cout << "Rose::builder::Build(StructureConstructor)\n";
}

template<typename T>
void Build(const parser::Expr::Parentheses&x, T* scope)
{
   std::cout << "Rose::builder::Build(Parentheses)\n";
}

template<typename T>
void Build(const parser::Expr::UnaryPlus&x, T* scope)
{
   std::cout << "Rose::builder::Build(UnaryPlus)\n";
}

template<typename T>
void Build(const parser::Expr::Negate&x, T* scope)
{
   std::cout << "Rose::builder::Build(Negate)\n";
}

template<typename T>
void Build(const parser::Expr::NOT&x, T* scope)
{
   std::cout << "Rose::builder::Build(NOT)\n";
}

template<typename T>
void Build(const parser::Expr::PercentLoc&x, T* scope)
{
   std::cout << "Rose::builder::Build(PercentLoc)\n";
}

template<typename T>
void Build(const parser::Expr::DefinedUnary&x, T* scope)
{
   std::cout << "Rose::builder::Build(DefinedUnary)\n";
}

template<typename T>
void Build(const parser::Expr::Power&x, T* scope)
{
   std::cout << "Rose::builder::Build(Power)\n";
}

template<typename T>
void Build(const parser::Expr::Multiply&x, T* scope)
{
   std::cout << "Rose::builder::Build(Multiply)\n";
   traverseBinaryExprs(x, scope);
}

template<typename T>
void Build(const parser::Expr::Divide&x, T* scope)
{
   std::cout << "Rose::builder::Build(Divide)\n";
   traverseBinaryExprs(x, scope);
}

template<typename T>
void traverseBinaryExprs(const T &x, SgScopeStatement* scope)
{
   std::cout << "Rose::builder::traverseBinaryExprs\n";
   Build(std::get<0>(x.t).value(), scope); // lhs Expr
   Build(std::get<1>(x.t).value(), scope); // rhs Expr
}

template<typename T>
void Build(const parser::Expr::Add&x, T* scope)
{
   std::cout << "Rose::builder::Build(Add)\n";

   traverseBinaryExprs(x, scope);

   //   Build(std::get<0>(x.t).value(), scope); // lhs
   //   Build(std::get<1>(x.t).value(), scope); // rhs
}

template<typename T>
void Build(const parser::Expr::Subtract&x, T* scope)
{
   std::cout << "Rose::builder::Build(Subtract)\n";
   traverseBinaryExprs(x, scope);
}

template<typename T>
void Build(const parser::Expr::Concat&x, T* scope)
{
   std::cout << "Rose::builder::Build(Concat)\n";
}

template<typename T>
void Build(const parser::Expr::LT&x, T* scope)
{
   std::cout << "Rose::builder::Build(LT)\n";
}

template<typename T>
void Build(const parser::Expr::LE&x, T* scope)
{
   std::cout << "Rose::builder::Build(LE)\n";
}

template<typename T>
void Build(const parser::Expr::EQ&x, T* scope)
{
   std::cout << "Rose::builder::Build(EQ)\n";
}

template<typename T>
void Build(const parser::Expr::NE&x, T* scope)
{
   std::cout << "Rose::builder::Build(NE)\n";
}

template<typename T>
void Build(const parser::Expr::GE&x, T* scope)
{
   std::cout << "Rose::builder::Build(GE)\n";
}

template<typename T>
void Build(const parser::Expr::GT&x, T* scope)
{
   std::cout << "Rose::builder::Build(GT)\n";
}

template<typename T>
void Build(const parser::Expr::AND&x, T* scope)
{
   std::cout << "Rose::builder::Build(AND)\n";
}

template<typename T>
void Build(const parser::Expr::OR&x, T* scope)
{
   std::cout << "Rose::builder::Build(OR)\n";
}

template<typename T>
void Build(const parser::Expr::EQV&x, T* scope)
{
   std::cout << "Rose::builder::Build(EQV)\n";
}

template<typename T>
void Build(const parser::Expr::NEQV&x, T* scope)
{
   std::cout << "Rose::builder::Build(NEQV)\n";
}

template<typename T>
void Build(const parser::Expr::DefinedBinary&x, T* scope)
{
   std::cout << "Rose::builder::Build(DefinedBinary)\n";
}

template<typename T>
void Build(const parser::Expr::ComplexConstructor&x, T* scope)
{
   std::cout << "Rose::builder::Build(ComplexConstructor)\n";
}

template<typename T>
void Build(const parser::StructureComponent&x, T* scope)
{
   std::cout << "Rose::builder::Build(StructureComponent)\n";
}

template<typename T>
void Build(const parser::ArrayElement&x, T* scope)
{
   std::cout << "Rose::builder::Build(ArrayElement)\n";
   Build(x.base, scope);        // DataRef
   Build(x.subscripts, scope);  // std::list<SectionSubscript>
}

template<typename T>
void Build(const parser::CoindexedNamedObject&x, T* scope)
{
   std::cout << "Rose::builder::Build(CoindexedNamedObject)\n";
   Build(x.base, scope);          // DataRef
   Build(x.imageSelector, scope); // ImageSelector
}

template<typename T>
void Build(const parser::ImageSelector&x, T* scope)
{
   std::cout << "Rose::builder::Build(ImageSelector)\n";

   Build(std::get<0>(x.t), scope);  // std::list<Cosubscript> - Cosubscript = Scalar<IntExpr>
   Build(std::get<1>(x.t), scope);  // std::list<ImageSelectorSpec>
}

template<typename T>
void Build(const parser::ImageSelectorSpec&x, T* scope)
{
   std::cout << "Rose::builder::Build(ImageSelectorSpec)\n";

   // Stat, TeamValue, Team_Number
   //   auto ImageSelectorSpecVisitor = [&] (const auto &y) { Build(y, scope); };
   //   std::visit(ImageSelectorSpecVisitor, x.u);
}

template<typename T>
void Build(const parser::SectionSubscript&x, T* scope)
{
   std::cout << "Rose::builder::Build(SectionSubscript)\n";

   std::visit(
      common::visitors{
         [&](const parser::Integer<common::Indirection<parser::Expr>>  &y) { Build(y.thing.value(), scope); },
         [&](const parser::SubscriptTriplet &y) { Build(y, scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::SubscriptTriplet&x, T* scope)
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
      std::cout << "The attrs is " << attrs << "\n";

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
   builder.Enter(derived_type_stmt, type_name);

   builder.Leave(derived_type_stmt);
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
