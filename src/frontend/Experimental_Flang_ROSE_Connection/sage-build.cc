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
   std::cout << "Rose::builder::Build(SpecificationPart) \n";

   const auto & implicit_part = std::get<3>(x.t);
   Build(implicit_part, scope);

   const auto & decl_construct = std::get<4>(x.t);
   Build(decl_construct, scope);

}

template<typename T>
void Build(const parser::ExecutionPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExecutionPart) \n";
   Build(x.v, scope);
}

template<typename T>
void Build(const parser::ExecutionPartConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExecutionPartConstruct) \n";
   //   auto & exect_construct = std::get<0>(x.u);
   Build(std::get<0>(x.u), scope);
}

template<typename T>
void Build(const parser::ExecutableConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(ExecutableConstruct) \n";
   Build(std::get<0>(x.u).statement, scope);
}

template<typename T>
void Build(const parser::ActionStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(ActionStmt) \n";


#if 0
   auto & assign_stmt = std::get<1>(x.u);

   try {
      auto & indirection = std::get<Fortran::common::Indirection<Fortran::parser::AssignmentStmt, false>>(x.u);
      std::cout << "Rose::builder::Build(const std::variant<A...>): FOUND a AssignmentStmt \n";
      Build(indirection.value(), scope);
   }
   catch (const std::bad_variant_access&)
      {
         std::cout << "Rose::builder::Build(const std::variant<A...>) NOT FOUND a AssignmentStmt \n";
      }
#endif

   std::visit(
      common::visitors{
         [&](const parser::ContinueStmt  &y) { Build(y, scope); },
         [&](const parser::FailImageStmt &y) { Build(y, scope); },
            //         [&](const auto &y) { },
         [&](const auto &y) { Build(y.value(), scope); },
      },
      x.u);

}

template<typename T>
void Build(const parser::AssignmentStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(AssignmentStmt) \n";

   auto & variable = std::get<0>(x.t);
   Build(variable, scope);

   auto & expr = std::get<1>(x.t);
   Build(expr, scope);
}

template<typename T>
void Build(const parser::Variable &x, T* scope)
{
   std::cout << "Rose::builder::Build(Variable) \n";

   auto VariableVisitor = [&](const auto& t) { Build(t.value(), scope); };
   std::visit(VariableVisitor, x.u);
}

template<typename T>
void Build(const parser::Designator &x, T* scope)
{
   std::cout << "Rose::builder::Build(Designator) \n";

   auto DesignatorVisitor = [&](const auto& t) { Build(t, scope); };
   std::visit(DesignatorVisitor, x.u);
}

template<typename T>
void Build(const parser::DataRef &x, T* scope)
{
   std::cout << "Rose::builder::Build(DataRef) \n";
   std::string name = std::get<0>(x.u).ToString();
   std::cout << "The name of the DataRef is " << name << std::endl;
}

template<typename T>
void Build(const parser::Substring &x, T* scope)
{
   std::cout << "Rose::builder::Build(Substring) \n";
}

template<typename T>
void Build(const parser::FunctionReference &x, T* scope)
{
   std::cout << "Rose::builder::Build(FunctionReference) \n";
}

template<typename T>
void Build(const parser::Expr &x, T* scope)
{
   std::cout << "Rose::builder::Build(Expr) \n";

   //   auto & lit_constant = std::get<1>(x.u);
   //   Build(lit_constant, scope);

   std::visit(
      common::visitors{
         [&](const Fortran::common::Indirection<parser::CharLiteralConstantSubstring> &y) {
            Build(y.value(), scope);
          },
         [&](const Fortran::common::Indirection<parser::Designator> &y) {
            Build(y.value(), scope);
          },
         [&](const Fortran::common::Indirection<parser::FunctionReference> &y) {
            Build(y.value(), scope);
          },
         [&](const auto &y) { Build(y, scope); },
            //         [&](const auto &y) { },
      },
      x.u);

}

template<typename T>
void Build(const parser::Expr::IntrinsicBinary &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntrinsicBinary) \n";
}

template<typename T>
void Build(const parser::LiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(LiteralConstant) \n";

   Build(std::get<1>(x.u), scope);
}

template<typename T>
void Build(const parser::IntLiteralConstant &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntLiteralConstant) \n";
   int literal = stoi(std::get<0>(x.t).ToString());
   std::cout << " The INTEGER LITERAL CONSTANT is " << literal << std::endl;
}

template<typename T>
void Build(const parser::InternalSubprogramPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(InternalSubprogramPart) \n";
}

template<typename T>
void Build(const parser::ImplicitPart &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitPart) \n";
   Build(x.v, scope);
}

template<typename T>
void Build(const parser::ImplicitPartStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitPartStmt) \n";

   try {
      auto & indirection = std::get<Fortran::parser::Statement<Fortran::common::Indirection<Fortran::parser::ImplicitStmt, false>>>(x.u);
      std::cout << "Rose::builder::Build(const std::variant<A...>): FOUND a ImplicitStmt \n";
      Build(indirection.statement.value(), scope);
   }
   catch (const std::bad_variant_access&)
      {
         std::cout << "Rose::builder::Build(const std::variant<A...>) NOT FOUND a ImplicitStmt \n";
      }
}

template<typename T>
void Build(const parser::ImplicitStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitStmt) \n";

   auto SpecVisitor = [&](const auto& t) { Build(t, scope); };
   std::visit(SpecVisitor, x.u);
}

template<typename T>
void Build(const parser::ImplicitSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitSpec) \n";
}

template<typename T>
void Build(const parser::ImplicitStmt::ImplicitNoneNameSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(ImplicitNoneNameSpec) \n";
}

template<typename T>
void Build(const parser::DeclarationConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(DeclarationConstruct) \n";

   std::visit(
      common::visitors{
         [&](const parser::SpecificationConstruct &y) { Build(y, scope); },
         [&](const parser::ErrorRecovery &y) { Build(y, scope); },
#if 0
         [&](const parser::Statement<Fortran::common::Indirection<parser::DataStmt>> &y) {
            Build(y.statement.value(), scope);
          },
         [&](const parser::Statement<Fortran::common::Indirection<parser::FormatStmt>> &y) {
            Build(y.statement.value(), scope);
          },
         [&](const parser::Statement<Fortran::common::Indirection<parser::EntryStmt>> &y) {
            Build(y.statement.value(), scope);
          },
         [&](const parser::Statement<Fortran::common::Indirection<parser::StmtFunctionStmt>> &y) {
            Build(y.statement.value(), scope);
          },
#endif
            [&](const auto &y) { Build(y.statement.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::SpecificationConstruct &x, T* scope)
{
   std::cout << "Rose::builder::Build(SpecificationConstruct) \n";

   std::visit(
      common::visitors{
         [&](const Fortran::common::Indirection<parser::DerivedTypeDef> &) {},
         [&](const parser::Statement<Fortran::common::Indirection<parser::TypeDeclarationStmt>> &y) {
            Build(y.statement.value(), scope);
          },
         [&](const Fortran::common::Indirection<parser::InterfaceBlock> &y) {
               std::cout << "Found an InterfaceBlock\n";
          },
          [&](const auto &) {},
      },
      x.u);
}

template<typename T>
void Build(const parser::TypeDeclarationStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(TypeDeclarationStmt) \n";

   auto & decl_spec = std::get<0>(x.t);
   Build(decl_spec, scope);

   auto & attr_spec = std::get<1>(x.t);
   Build(attr_spec, scope);

   auto & entity_decl = std::get<2>(x.t);
   Build(entity_decl, scope);
}

template<typename T>
void Build(const parser::DeclarationTypeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(DeclarationTypeSpec) \n";
   //   auto & type = std::get<Fortran::parser::IntrinsicTypeSpec>(x.u);
   Build(std::get<Fortran::parser::IntrinsicTypeSpec>(x.u), scope);
}

template<typename T>
void Build(const parser::AttrSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(AttrSpec) \n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntrinsicTypeSpec) \n";

   auto TypeVisitor = [&](const auto& t) { Build(t, scope); };
   std::visit(TypeVisitor, x.u);
}

template<typename T>
void Build(const parser::IntegerTypeSpec &x, T* scope)
{
   std::cout << "Rose::builder::Build(IntegerTypeSpec) \n";
   std::cout << "TYPE IS : Integer\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Real &x, T* scope)
{
   std::cout << "Rose::builder::Build(Real) \n";
   std::cout << "TYPE IS : Real\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::DoublePrecision &x, T* scope)
{
   std::cout << "Rose::builder::Build(DoublePrecision) \n";
   std::cout << "TYPE IS : DoublePrecision\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Complex &x, T* scope)
{
   std::cout << "Rose::builder::Build(Complex) \n";
   std::cout << "TYPE IS : Complex\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Character &x, T* scope)
{
   std::cout << "Rose::builder::Build(Character) \n";
   std::cout << "TYPE IS : Character\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::Logical &x, T* scope)
{
   std::cout << "Rose::builder::Build(Logical) \n";
   std::cout << "TYPE IS : Logical\n";
}

template<typename T>
void Build(const parser::IntrinsicTypeSpec::DoubleComplex &x, T* scope)
{
   std::cout << "Rose::builder::Build(DoubleComplex) \n";
   std::cout << "TYPE IS : DoubleComplex\n";
}

template<typename T>
void Build(const parser::EntityDecl &x, T* scope)
{
   std::cout << "Rose::builder::Build(EntityDecl) \n";
   std::string name = std::get<0>(x.t).ToString();
   std::cout << "The object name is: " << name << std::endl;
}



   // DeclarationConstruct

template<typename T>
void Build(const parser::DataStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(DataStmt) \n";
}

template<typename T>
void Build(const parser::FormatStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(FormatStmt) \n";
}

template<typename T>
void Build(const parser::EntryStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(EntryStmt) \n";
}

template<typename T>
void Build(const parser::StmtFunctionStmt &x, T* scope)
{
   std::cout << "Rose::builder::Build(StmtFunctionStmt) \n";
}

template<typename T>
void Build(const parser::ErrorRecovery &x, T* scope)
{
   std::cout << "Rose::builder::Build(ErrorRecovery) \n";
}



//template<typename T> void Build(const Fortran::parser::     ArithmeticIfStmt &x, T* scope);
//template<typename T> void Build(const Fortran::parser::           AssignStmt &x, T* scope);
//template<typename T> void Build(const Fortran::parser::     AssignedGotoStmt &x, T* scope);
//template<typename T> void Build(const Fortran::parser::            PauseStmt &x, T* scope);

   // ActionStmt
template<typename T>
void Build(const parser::ContinueStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ContinueStmt) \n";
}

template<typename T>
void Build(const parser::FailImageStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(FailImageStmt) \n";
}

template<typename T>
void Build(const parser::AllocateStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(AllocateStmt) \n";
}

template<typename T>
void Build(const parser::BackspaceStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(BackspaceStmt) \n";
}

template<typename T>
void Build(const parser::CallStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(CallStmt) \n";
}

template<typename T>
void Build(const parser::CloseStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(CloseStmt) \n";
}

template<typename T>
void Build(const parser::CycleStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(CycleStmt) \n";
}

template<typename T>
void Build(const parser::DeallocateStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(DeallocateStmt) \n";
}

template<typename T>
void Build(const parser::EndfileStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(EndfileStmt) \n";
}

template<typename T>
void Build(const parser::EventPostStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(EventPostStmt) \n";
}

template<typename T>
void Build(const parser::EventWaitStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(EventWaitStmt) \n";
}

template<typename T>
void Build(const parser::ExitStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ExitStmt) \n";
}

template<typename T>
void Build(const parser::FlushStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(FlustStmt) \n";
}

template<typename T>
void Build(const parser::FormTeamStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(FormTeamStmt) \n";
}

template<typename T>
void Build(const parser::GotoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(GotoStmt) \n";
}

template<typename T>
void Build(const parser::IfStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(IfStmt) \n";
}

template<typename T>
void Build(const parser::InquireStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(InquireStmt) \n";
}

template<typename T>
void Build(const parser::LockStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(LockStmt) \n";
}

template<typename T>
void Build(const parser::NullifyStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(NullifyStmt) \n";
}

template<typename T>
void Build(const parser::OpenStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(OpenStmt) \n";
}

template<typename T>
void Build(const parser::PointerAssignmentStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(PointerAssignmentStmt) \n";
}

template<typename T>
void Build(const parser::PrintStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(PrintStmt) \n";
}

template<typename T>
void Build(const parser::ReadStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ReadStmt) \n";
}

template<typename T>
void Build(const parser::ReturnStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ReturnStmt) \n";
}

template<typename T>
void Build(const parser::RewindStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(RewindStmt) \n";
}

template<typename T>
void Build(const parser::StopStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(StopStmt) \n";
}

template<typename T>
void Build(const parser::SyncAllStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncAllStmt) \n";
}

template<typename T>
void Build(const parser::SyncImagesStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncImagesStmt) \n";
}

template<typename T>
void Build(const parser::SyncMemoryStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncMemoryStmt) \n";
}

template<typename T>
void Build(const parser::SyncTeamStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(SyncTeamStmt) \n";
}

template<typename T>
void Build(const parser::UnlockStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(UnlockStmt) \n";
}

template<typename T>
void Build(const parser::WaitStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(WaitStmt) \n";
}

template<typename T>
void Build(const parser::WhereStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(WhereStmt) \n";
}

template<typename T>
void Build(const parser::WriteStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(WriteStmt) \n";
}

template<typename T>
void Build(const parser::ComputedGotoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ComputedGotoStmt) \n";
}

template<typename T>
void Build(const parser::ForallStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ForallStmt) \n";
}

template<typename T>
void Build(const parser::ArithmeticIfStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(ArithmeticIfStmt) \n";
}

template<typename T>
void Build(const parser::AssignStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(AssignStmt) \n";
}

template<typename T>
void Build(const parser::AssignedGotoStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(AssignedGotoStmt) \n";
}

template<typename T>
void Build(const parser::PauseStmt&x, T* scope)
{
   std::cout << "Rose::builder::Build(PauseStmt) \n";
}

   // Expr
template<typename T>
void Build(const parser::CharLiteralConstantSubstring&x, T* scope)
{
   std::cout << "Rose::builder::Build(CharLiteralConstantSubstring) \n";
}

template<typename T>
void Build(const parser::ArrayConstructor&x, T* scope)
{
   std::cout << "Rose::builder::Build(ArrayConstructor) \n";
}

template<typename T>
void Build(const parser::StructureConstructor&x, T* scope)
{
   std::cout << "Rose::builder::Build(StructureConstructor) \n";
}

template<typename T>
void Build(const parser::Expr::Parentheses&x, T* scope)
{
   std::cout << "Rose::builder::Build(Parentheses) \n";
}

template<typename T>
void Build(const parser::Expr::UnaryPlus&x, T* scope)
{
   std::cout << "Rose::builder::Build(UnaryPlus) \n";
}

template<typename T>
void Build(const parser::Expr::Negate&x, T* scope)
{
   std::cout << "Rose::builder::Build(Negate) \n";
}

template<typename T>
void Build(const parser::Expr::NOT&x, T* scope)
{
   std::cout << "Rose::builder::Build(NOT) \n";
}

template<typename T>
void Build(const parser::Expr::PercentLoc&x, T* scope)
{
   std::cout << "Rose::builder::Build(PercentLoc) \n";
}

template<typename T>
void Build(const parser::Expr::DefinedUnary&x, T* scope)
{
   std::cout << "Rose::builder::Build(DefinedUnary) \n";
}

template<typename T>
void Build(const parser::Expr::Power&x, T* scope)
{
   std::cout << "Rose::builder::Build(Power) \n";
}

template<typename T>
void Build(const parser::Expr::Multiply&x, T* scope)
{
   std::cout << "Rose::builder::Build(Multiply) \n";
}

template<typename T>
void Build(const parser::Expr::Divide&x, T* scope)
{
   std::cout << "Rose::builder::Build(Divide) \n";
}

template<typename T>
void Build(const parser::Expr::Add&x, T* scope)
{
   std::cout << "Rose::builder::Build(Add) \n";
}

template<typename T>
void Build(const parser::Expr::Subtract&x, T* scope)
{
   std::cout << "Rose::builder::Build(Subtract) \n";
}

template<typename T>
void Build(const parser::Expr::Concat&x, T* scope)
{
   std::cout << "Rose::builder::Build(Concat) \n";
}

template<typename T>
void Build(const parser::Expr::LT&x, T* scope)
{
   std::cout << "Rose::builder::Build(LT) \n";
}

template<typename T>
void Build(const parser::Expr::LE&x, T* scope)
{
   std::cout << "Rose::builder::Build(LE) \n";
}

template<typename T>
void Build(const parser::Expr::EQ&x, T* scope)
{
   std::cout << "Rose::builder::Build(EQ) \n";
}

template<typename T>
void Build(const parser::Expr::NE&x, T* scope)
{
   std::cout << "Rose::builder::Build(NE) \n";
}

template<typename T>
void Build(const parser::Expr::GE&x, T* scope)
{
   std::cout << "Rose::builder::Build(GE) \n";
}

template<typename T>
void Build(const parser::Expr::GT&x, T* scope)
{
   std::cout << "Rose::builder::Build(GT) \n";
}

template<typename T>
void Build(const parser::Expr::AND&x, T* scope)
{
   std::cout << "Rose::builder::Build(AND) \n";
}

template<typename T>
void Build(const parser::Expr::OR&x, T* scope)
{
   std::cout << "Rose::builder::Build(OR) \n";
}

template<typename T>
void Build(const parser::Expr::EQV&x, T* scope)
{
   std::cout << "Rose::builder::Build(EQV) \n";
}

template<typename T>
void Build(const parser::Expr::NEQV&x, T* scope)
{
   std::cout << "Rose::builder::Build(NEQV) \n";
}

template<typename T>
void Build(const parser::Expr::DefinedBinary&x, T* scope)
{
   std::cout << "Rose::builder::Build(DefinedBinary) \n";
}

template<typename T>
void Build(const parser::Expr::ComplexConstructor&x, T* scope)
{
   std::cout << "Rose::builder::Build(ComplexConstructor) \n";
}

} // namespace Rose::builder
