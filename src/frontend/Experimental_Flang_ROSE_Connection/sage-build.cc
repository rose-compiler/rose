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

   const auto & use_stmts{std::get<std::list<parser::Statement<common::Indirection<parser::UseStmt>>>>(x.t)};
   if (use_stmts.size() > 0) {
      return std::optional<SourcePosition>{BuildSourcePosition(use_stmts.front(), Order::begin)};
   }

   const auto & import_stmts{std::get<std::list<parser::Statement<common::Indirection<parser::ImportStmt>>>>(x.t)};
   if (import_stmts.size() > 0) {
      return std::optional<SourcePosition>{BuildSourcePosition(import_stmts.front(), Order::begin)};
   }

   const auto & implicit_part_stmts{std::get<parser::ImplicitPart>(x.t).v};
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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "\n";
   std::cout << "Rose::builder::Build(Program)\n";
#endif

   SgScopeStatement* scope{nullptr};

   cooked_ = &cooked;

   builder.Enter(scope);
   Build(x.v, scope);
   builder.Leave(scope);
}

template<typename T>
void Build(const parser::ProgramUnit &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ProgramUnit)\n";
#endif

   Build(x.u, scope);
}

template<typename T>
void Build(const parser::MainProgram &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(MainProgram)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SpecificationPart)\n";
#endif

   const auto & implicit_part = std::get<parser::ImplicitPart>(x.t);
   Build(implicit_part, scope);

   const auto & decl_construct = std::get<std::list<parser::DeclarationConstruct>>(x.t);
   Build(decl_construct, scope);

}

template<typename T>
void Build(const parser::ExecutionPart &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ExecutionPart)\n";
#endif

   Build(x.v, scope);   // std::list<ExecutionPartConstruct>
}

template<typename T>
void Build(const parser::ExecutionPartConstruct &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ExecutionPartConstruct)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ExecutableConstruct)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const parser::Statement<parser::ActionStmt> &y) { Build(y.statement, scope); },
         [&] (const parser::Statement<common::Indirection<parser::LabelDoStmt>> &y)
                { Build(y.statement.value(), scope); },
         [&] (const parser::Statement<common::Indirection<parser::EndDoStmt>> &y)
                { Build(y.statement.value(), scope); },
         // common:: Indirection - AssociateConstruct, BlockConstruct, CaseConstruct, ChangeTeamConstruct,
         // CriticalConstruct, DoConstruct, IfConstruct, SelectRankConstruct, SelectTypeConstruct,
         // WhereConstruct, ForallConstruct, CompilerDirective, OpenMPConstruct, OpenACCConstruct, OmpEndLoopDirective
         [&] (const auto &y) { Build(y.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::ActionStmt &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ActionStmt)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AssignmentStmt)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Variable)\n";
#endif

   // Designator, FunctionReference
   auto VariableVisitor = [&](const auto& y) { Build(y.value(), expr); };
   std::visit(VariableVisitor, x.u);
}

void Build(const parser::Designator &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Designator)\n";
#endif

   // DataRef, Substring
   auto DesignatorVisitor = [&](const auto& y) { Build(y, expr); };
   std::visit(DesignatorVisitor, x.u);
}

void Build(const parser::DataRef &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DataRef)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const Fortran::parser::Name &y) {
            std::string name = std::get<0>(x.u).ToString();

            expr = SageBuilderCpp17::buildVarRefExp_nfi(name);
          },
         // StructureComponent, ArrayElement, or CoindexedNamedObject
         [&] (const auto &y) { Build(y.value(), expr); },
      },
      x.u);
}

void Build(const parser::Substring &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Substring)\n";
#endif
}

void Build(const parser::FunctionReference &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(FunctionReference)\n";
#endif

   Build(x.v, expr); // Call
}

void Build(const parser::Call &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Call)\n";
#endif

   const parser::CharBlock source = x.source;

   SgExpression* proc_name{nullptr};
   SgExpression* arg_list{nullptr};

   Build(std::get<0>(x.t), proc_name);   // ProcedureDesignator
   Build(std::get<1>(x.t), arg_list);    // std::list<ActualArgSpec>
}

void Build(const parser::ProcedureDesignator &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ProcedureDesignator)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const parser::Name &y)
         {
            std::string name = y.ToString();
            std::cout << "The ProcedureDesignator name is " << name << "\n";
         },
         [&] (const auto &y) { Build(y, expr); }   // ProcComponentRef
      },
      x.u);
}

void Build(const parser::ProcComponentRef &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ProcComponentRef)\n";
#endif
}

void Build(const parser::ActualArgSpec &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ActualArgSpec)\n";
#endif

   if (auto & opt = std::get<0>(x.t)) {    // std::optional<Keyword>
      Build(opt.value(), expr);
   }

   Build(std::get<1>(x.t), expr);          // ActualArg
}

void Build(const parser::ActualArg &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ActualArg)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const common::Indirection<parser::Expr> &y) { Build(y.value(), expr); },
         [&] (const auto &y) { }   // AltReturnSpec, PercentRef, PercentVal
      },
      x.u);
}

void Build(const parser::Keyword &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Keyword)\n";
#endif
}

void Build(const parser::Name &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Name)\n";
#endif
}

void Build(const parser::Name &x, std::string &name)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Name) - build std::string\n";
#endif

   name = x.ToString();
}

void Build(const parser::NamedConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(NamedConstant)\n";
#endif
}

void Build(const parser::Expr &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Expr)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(IntrinsicBinary)\n";
#endif
}

void Build(const parser::ConstantValue &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ConstantValue)\n";
#endif

   // LiteralConstant, NamedConstant
   auto ConstantValueVisitor = [&] (const auto &y) { Build(y, expr); };
   std::visit(ConstantValueVisitor, x.u);
}

void Build(const parser::LiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LiteralConstant)\n";
#endif

   //  HollerithLiteralConstant, IntLiteralConstant, RealLiteralConstant, ComplexLiteralConstant,
   //  BOZLiteralConstant, CharLiteralConstant, LogicalLiteralConstant
   auto LiteralConstVisitor = [&] (const auto &y) { Build(y, expr); };
   std::visit(LiteralConstVisitor, x.u);
}

   // LiteralConstant
void Build(const parser::HollerithLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(HollerithLiteralConstant)\n";
#endif
}

void Build(const parser::IntLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(IntLiteralConstant)\n";
#endif

   expr = SageBuilderCpp17::buildIntVal_nfi(stoi(std::get<0>(x.t).ToString()));
}

void Build(const parser::SignedIntLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SignedIntLiteralConstant)\n";
#endif
   // std::tuple<CharBlock, std::optional<KindParam>> t;

   expr = SageBuilderCpp17::buildIntVal_nfi(stoi(std::get<0>(x.t).ToString()));
}

void Build(const parser::RealLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(RealLiteralConstant)\n";
#endif

   expr = SageBuilderCpp17::buildFloatVal_nfi(x.real.source.ToString());
}

void Build(const parser::SignedRealLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SignedRealLiteralConstant)\n";
#endif
   // std::tuple<std::optional<Sign>, RealLiteralConstant> t;

   Build(std::get<1>(x.t), expr);
}

void Build(const parser::ComplexLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ComplexLiteralConstant)\n";
#endif
   // std::tuple<ComplexPart, ComplexPart> t;

   SgExpression * real_value = nullptr, * imaginary_value = nullptr;

   Build(std::get<0>(x.t), real_value);
   Build(std::get<1>(x.t), imaginary_value);

   expr = SageBuilderCpp17::buildComplexVal_nfi(real_value, imaginary_value, "");
}

void Build(const parser::BOZLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(BOZLiteralConstant)\n";
#endif
}

void Build(const parser::CharLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CharLiteralConstant)\n";
#endif

   expr = SageBuilderCpp17::buildStringVal_nfi(x.GetString());
}

void Build(const parser::LogicalLiteralConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LogicalLiteralConstant)\n";
#endif

   expr = SageBuilderCpp17::buildBoolValExp_nfi(std::get<0>(x.t));
}

void Build(const parser::ComplexPart &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ComplexPart)\n";
#endif

   // std::variant<SignedIntLiteralConstant, SignedRealLiteralConstant, NamedConstant>
   auto ComplexPartVisitor = [&](const auto& y) { Build(y, expr); };
   std::visit(ComplexPartVisitor, x.u);
}

template<typename T>
void Build(const parser::InternalSubprogramPart &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(InternalSubprogramPart)\n";
#endif
}

template<typename T>
void Build(const parser::ImplicitPart &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImplicitPart)\n";
#endif

   Build(x.v, scope);   // std::list<ImplicitPartStmt>
}

template<typename T>
void Build(const parser::ImplicitPartStmt &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImplicitPartStmt)\n";
#endif

   std::visit(
      common::visitors{
         [&](const common::Indirection<parser::CompilerDirective> &y) { Build(y.value(), scope); },
         // Statement<common::Indirection<> - ImplicitStmt, ParameterStmt, OldParameterStmt, FormatStmt, EntryStmt
         [&](const auto &y) { Build(y.statement.value(), scope); },
      },
      x.u);
}

template<typename T>
void Build(const parser::ImplicitStmt &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImplicitStmt)\n";
#endif

   // std::list<ImplicitSpec>, std::list<ImplicitNoneNameSpec>
   auto SpecVisitor = [&](const auto& y) { Build(y, scope); };
   std::visit(SpecVisitor, x.u);
}

template<typename T>
void Build(const parser::ImplicitSpec &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImplicitSpec)\n";
#endif
}

template<typename T>
void Build(const parser::ImplicitStmt::ImplicitNoneNameSpec &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImplicitNoneNameSpec)\n";
#endif
}

template<typename T>
void Build(const parser::DeclarationConstruct &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DeclarationConstruct)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SpecificationConstruct)\n";
#endif

   std::visit(
      common::visitors{
         [&](const common::Indirection<parser::DerivedTypeDef> &y)              { Build(y.value(), scope); },
         [&](const common::Indirection<parser::EnumDef> &y)                     { Build(y.value(), scope); },
         [&](const common::Indirection<parser::InterfaceBlock> &y)              { Build(y.value(), scope); },
         [&](const common::Indirection<parser::StructureDef> &y)                { Build(y.value(), scope); },
         [&](const common::Indirection<parser::CompilerDirective> &y)           { Build(y.value(), scope); },
         [&](const common::Indirection<parser::OpenMPDeclarativeConstruct>  &y) { Build(y.value(), scope); },
         [&](const common::Indirection<parser::OpenACCDeclarativeConstruct> &y) { Build(y.value(), scope); },
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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(TypeDeclarationStmt)\n";
#endif

   SgVariableDeclaration* var_decl = nullptr;
   SgType * type = nullptr, * base_type = nullptr;
   SgExpression* init = nullptr;

   // need name and type
   std::string name{};

   Build(std::get<0>(x.t), base_type);                    // DeclarationTypeSpec
   Build(std::get<1>(x.t), scope);                        // std::list<AttrSpec>
   Build(std::get<2>(x.t), name, init, type, base_type);  // std::list<EntityDecl>

   if (!type) {
      type = base_type;
   }

   builder.Enter(var_decl, name, type, init);
   builder.Leave(var_decl);
}

void Build(const parser::DeclarationTypeSpec &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DeclarationTypeSpec)\n";
#endif

   // IntrinsicTypeSpec, Type, TypeStar, Class, ClassStar, Record
   auto DeclTypeSpecVisitor = [&] (const auto &y) { Build(y, type); };
   std::visit(DeclTypeSpecVisitor, x.u);
}

void Build(const parser::DeclarationTypeSpec::Type&x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Type)\n";
#endif

   Build(x.derived, type);   // DerivedTypeSpec
}

void Build(const parser::DeclarationTypeSpec::TypeStar&x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(TypeStar)\n";
#endif
}

void Build(const parser::DeclarationTypeSpec::Class&x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Class)\n";
#endif

   Build(x.derived, type);   // DerivedTypeSpec
}

void Build(const parser::DeclarationTypeSpec::ClassStar&x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ClassStar)\n";
#endif
}

void Build(const parser::DeclarationTypeSpec::Record&x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Record)\n";
#endif
}

void Build(const parser::DerivedTypeSpec&x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DerivedTypeSpec)\n";
#endif

   //   std::tuple<Name, std::list<TypeParamSpec>> t;
   std::string name = std::get<parser::Name>(x.t).ToString();
   std::cout << "DerivedTypeSpec name is " << name << "\n";
}

template<typename T>
void Build(const parser::AttrSpec &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AttrSpec)\n";
#endif

   //  std::variant<AccessSpec, Allocatable, Asynchronous, CoarraySpec, Contiguous,
   //      ArraySpec, External, IntentSpec, Intrinsic, LanguageBindingSpec, Optional,
   //      Parameter, Pointer, Protected, Save, Target, Value, Volatile>
   auto AttrSpecVisitor = [&] (const auto &y) { ; };
   std::visit(AttrSpecVisitor, x.u);
}

void Build(const parser::KindSelector &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(KindSelector)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const parser::ScalarIntConstantExpr  &y) { Build(y.thing.thing, expr); },
         [&] (const parser::KindSelector::StarSize &y) { ; },
      },
      x.u);
}

void Build(const parser::IntrinsicTypeSpec &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(IntrinsicTypeSpec)\n";
#endif

   // IntegerTypeSpec, Real, DoublePrecision, Complex, Character, Logical, DoubleComplex
   auto TypeVisitor = [&](const auto& y) { Build(y, type); };
   std::visit(TypeVisitor, x.u);
}

void Build(const parser::IntegerTypeSpec &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(IntegerTypeSpec)\n";
#endif

   if (auto & kind = x.v) {   // std::optional<KindSelector>
      SgExpression* kind_expr = nullptr;
      Build(kind.value(), kind_expr);
      type = SageBuilderCpp17::buildIntType(kind_expr);
   } else {
      type = SageBuilderCpp17::buildIntType();
   }
}

void Build(const parser::IntrinsicTypeSpec::Real &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Real)\n";
#endif

   if (auto & kind = x.kind) {   // std::optional<KindSelector>
      SgExpression* kind_expr = nullptr;
      Build(kind.value(), kind_expr);
      type = SageBuilderCpp17::buildFloatType(kind_expr);
   } else {
      type = SageBuilderCpp17::buildFloatType();
   }
}

void Build(const parser::IntrinsicTypeSpec::DoublePrecision &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DoublePrecision)\n";
#endif

   type = SageBuilderCpp17::buildDoubleType();
}

void Build(const parser::IntrinsicTypeSpec::Complex &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Complex)\n";
#endif

   SgType* base_type = SageBuilderCpp17::buildIntType();
   type = SageBuilderCpp17::buildComplexType(base_type);
}

void Build(const parser::IntrinsicTypeSpec::Character &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Character)\n";
#endif

   SgExpression* expr = nullptr;

   if (auto & opt = x.selector) {    // std::optional<CharSelector> selector
      Build(opt.value(), expr);
      type = SageBuilderCpp17::buildStringType(expr);
   } else {
      type = SageBuilderCpp17::buildCharType();
   }
}

void Build(const parser::IntrinsicTypeSpec::Logical &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Logical)\n";
#endif

   if (auto & kind = x.kind) {   // std::optional<KindSelector>
      SgExpression* kind_expr = nullptr;
      Build(kind.value(), kind_expr);
      type = SageBuilderCpp17::buildBoolType(kind_expr);
   } else {
      type = SageBuilderCpp17::buildBoolType();
   }
}

void Build(const parser::IntrinsicTypeSpec::DoubleComplex &x, SgType* &type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DoubleComplex)\n";
#endif

   SgType* base_type = SageBuilderCpp17::buildDoubleType();
   type = SageBuilderCpp17::buildComplexType(base_type);
}

void Build(const parser::CharSelector &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CharSelector)\n";
#endif

   // std::variant<LengthSelector, LengthAndKind> u;
   auto CharSelectorVisitor = [&](const auto& y) { Build(y, expr); };
   std::visit(CharSelectorVisitor, x.u);
}

void Build(const parser::LengthSelector &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LengthSelector)\n";
#endif

   //  std::variant<TypeParamValue, CharLength> u;
   auto LengthSelectorVisitor = [&](const auto& y) { Build(y, expr); };
   std::visit(LengthSelectorVisitor, x.u);
}

void Build(const parser::CharSelector::LengthAndKind &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LengthAndKind)\n";
#endif

   //    std::optional<TypeParamValue> length;
   //    ScalarIntConstantExpr kind;
}

void Build(const parser::TypeParamValue &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(TypeParamVale)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const parser::ScalarIntExpr &y)  { Build(y, expr); },
         [&] (const auto &y)                                 { ; },  // Star, Deferred
      },
      x.u);
}

void Build(const std::list<Fortran::parser::EntityDecl> &x, std::string &name, SgExpression* &init, SgType* &type, SgType* base_type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(std::list) for EntityDecl\n";
#endif

   for (const auto &elem : x) {
      Build(elem, name, init, type, base_type);
   }
}

void Build(const parser::EntityDecl &x, std::string &name, SgExpression* &init, SgType* &type, SgType* base_type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EntityDecl)\n";
#endif

   name = std::get<0>(x.t).ToString();

   SgScopeStatement *scope = nullptr;
   if (auto & opt = std::get<1>(x.t)) {    // ArraySpec
      Build(opt.value(), type, base_type);
   }

   if (auto & opt = std::get<2>(x.t)) {    // CoarraySpec
      Build(opt.value(), scope);
   }

   if (auto & opt = std::get<3>(x.t)) {    // CharLength
      //      Build(opt.value(), scope);
   }

   if (auto & opt = std::get<4>(x.t)) {    // Initialization
      Build(opt.value(), init);
   }
}

   // EntityDecl
void Build(const parser::ArraySpec &x, SgType* &type, SgType* base_type)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ArraySpec)\n";
#endif

   SgExpression* expr = nullptr;
   std::list<SgExpression*> expr_list;

   std::visit(
      common::visitors{
         [&] (const std::list<parser::ExplicitShapeSpec> &y)
            {
               Build(y, expr_list);
               type = SageBuilderCpp17::buildArrayType(base_type, expr_list);
            },
         // std::list<AssumedShapeSpec>, DeferredShapeSpecList,
         // AssumedSizeSpec, ImpliedShapeSpec, AssumedRankSpec
         [&] (const auto &y)
            {
               Build(y, expr);
            }
      },
      x.u);
}

template<typename T>
void Build(const parser::CoarraySpec &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CoarraySpec)\n";
#endif
}

void Build(const parser::CharLength &x, SgExpression* &)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CharLength)\n";
#endif
}

void Build(const parser::Initialization &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Initialization)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const std::list<common::Indirection<parser::DataStmtValue>> &y)
            {
               for (const auto &elem : y) {
                  Build(elem.value(), expr);
               }
            },
         [&] (const parser::ConstantExpr &y) { Build(y, expr); },
         [&] (const auto &y)                               { ; }    // NullInit, InitialDataTarget
      },
      x.u);
}

   // ArraySpec

void Build(const parser::ExplicitShapeSpec &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ExplicitShapeSpec)\n";
#endif

   if (auto & specExpr1 = std::get<0>(x.t)) {
      Build(specExpr1.value(), expr);      // 1st SpecificationExpr (optional)
   }

   Build(std::get<1>(x.t), expr);          // 2nd SpecificationExpr
}

void Build(const parser::AssumedShapeSpec &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AssumedShapeSpec)\n";
#endif
}

void Build(const parser::DeferredShapeSpecList &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DeferredShapeSpecList)\n";
#endif
}

void Build(const parser::AssumedSizeSpec &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AssumedSizeSpec)\n";
#endif
}

void Build(const parser::ImpliedShapeSpec &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImpliedShapeSpec)\n";
#endif
}

void Build(const parser::AssumedRankSpec &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AssumedRankSpec)\n";
#endif
}

void Build(const parser::SpecificationExpr &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SpecificationExpr)\n";
#endif

   Build(x.v, expr);  // Scalar<IntExpr>
}

void Build(const parser::Scalar<parser::IntExpr> &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Scalar<IntExpr>)\n";
#endif

   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), expr);  // Expr
}

void Build(const parser::Scalar<parser::LogicalExpr>&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Scalar<LogicalExpr>)\n";
#endif

   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), expr);  // Expr
}

void Build(const parser::ConstantExpr &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ConstantExpr)\n";
#endif

   // Constant<common::Indirection<Expr>>
   Build(x.thing.value(), expr);  // Expr
}

   // DeclarationConstruct

template<typename T>
void Build(const parser::DataStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DataStmt)\n";
#endif
}

template<typename T>
void Build(const parser::FormatStmt &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(FormatStmt)\n";
#endif
}

template<typename T>
void Build(const parser::EntryStmt &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EntryStmt)\n";
#endif
}

template<typename T>
void Build(const parser::StmtFunctionStmt &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(StmtFunctionStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ErrorRecovery &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ErrorRecovery)\n";
#endif
}

   // DataStmt
void Build(const parser::DataStmtValue &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DataStmtValue)\n";
#endif

   // std::tuple<std::optional<DataStmtRepeat>, DataStmtConstant> t;
   Build(std::get<parser::DataStmtConstant>(x.t), expr);
}

void Build(const parser::DataStmtConstant &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DataStmtConstant)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const parser::Scalar<parser::ConstantValue> &y) { Build(y.thing, expr); },
         // Scalar<ConstantSubobject>, SignedIntLiteralConstant, SignedRealLiteralConstant,
         // SignedComplexLiteralConstant, NullInit, InitialDataTarget, StructureConstructor
         [&] (const auto &y) { ; }
      },
      x.u);
}

   // ActionStmt
template<typename T>
void Build(const parser::ContinueStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ContinueStmt)\n";
#endif
}

template<typename T>
void Build(const parser::FailImageStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(FailImageStmt)\n";
#endif
}

template<typename T>
void Build(const parser::AllocateStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AllocateStmt)\n";
#endif
}

template<typename T>
void Build(const parser::BackspaceStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(BackspaceStmt)\n";
#endif
}

template<typename T>
void Build(const parser::CallStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CallStmt)\n";
#endif
}

template<typename T>
void Build(const parser::CloseStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CloseStmt)\n";
#endif
}

template<typename T>
void Build(const parser::CycleStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CycleStmt)\n";
#endif
}

template<typename T>
void Build(const parser::DeallocateStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DeallocateStmt)\n";
#endif
}

template<typename T>
void Build(const parser::EndfileStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EndfileStmt)\n";
#endif
}

template<typename T>
void Build(const parser::EventPostStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EventPostStmt)\n";
#endif
}

template<typename T>
void Build(const parser::EventWaitStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EventWaitStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ExitStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ExitStmt)\n";
#endif
}

template<typename T>
void Build(const parser::FlushStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(FlustStmt)\n";
#endif
}

template<typename T>
void Build(const parser::FormTeamStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(FormTeamStmt)\n";
#endif
}

template<typename T>
void Build(const parser::GotoStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(GotoStmt)\n";
#endif
}

template<typename T>
void Build(const parser::IfStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(IfStmt)\n";
#endif

   SgExpression* expr = nullptr;

   Build(std::get<0>(x.t), expr);            // ScalarLogicalExpr
   Build(std::get<1>(x.t).statement, scope); // UnlabeledStatement<ActionStmt>

}

template<typename T>
void Build(const parser::InquireStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(InquireStmt)\n";
#endif
}

template<typename T>
void Build(const parser::LockStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LockStmt)\n";
#endif
}

template<typename T>
void Build(const parser::NullifyStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(NullifyStmt)\n";
#endif
}

template<typename T>
void Build(const parser::OpenStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OpenStmt)\n";
#endif
}

template<typename T>
void Build(const parser::PointerAssignmentStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(PointerAssignmentStmt)\n";
#endif
}

template<typename T>
void Build(const parser::PrintStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(PrintStmt)\n";
#endif

   std::list<SgExpression*> output_item_list;

   SgExpression* format = nullptr;

   Build(std::get<0>(x.t), format);           // Format
   Build(std::get<1>(x.t), output_item_list); // std::list<OutputItem>

   SgPrintStatement* print_stmt = nullptr;

   builder.Enter(print_stmt, format, output_item_list);
   builder.Leave(print_stmt);
}

void Build(const parser::Format&x, SgExpression* &format)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Format)\n";
#endif

   auto FormatVisitor = [&] (const auto &y) { Build(y, format); };
   std::visit(FormatVisitor, x.u);   // DefaultCharExpr, Label, Star
}

void Build(const parser::DefaultCharExpr&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DefaultCharExpr)\n";
#endif
}

void Build(const parser::Label&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Label)\n";
#endif
}

void Build(const parser::Star&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Star)\n";
#endif

   expr = SageBuilderCpp17::buildAsteriskShapeExp_nfi();
}

void Build(const parser::OutputItem&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OutputItem)\n";
#endif

   expr = nullptr;

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OutputImpliedDo)\n";
#endif
}

template<typename T>
void Build(const parser::ReadStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ReadStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ReturnStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ReturnStmt)\n";
#endif
}

template<typename T>
void Build(const parser::RewindStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(RewindStmt)\n";
#endif
}

template<typename T>
void Build(const parser::StopStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(StopStmt)\n";
#endif
}

template<typename T>
void Build(const parser::SyncAllStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SyncAllStmt)\n";
#endif
}

template<typename T>
void Build(const parser::SyncImagesStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SyncImagesStmt)\n";
#endif
}

template<typename T>
void Build(const parser::SyncMemoryStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SyncMemoryStmt)\n";
#endif
}

template<typename T>
void Build(const parser::SyncTeamStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SyncTeamStmt)\n";
#endif
}

template<typename T>
void Build(const parser::UnlockStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(UnlockStmt)\n";
#endif
}

template<typename T>
void Build(const parser::WaitStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(WaitStmt)\n";
#endif
}

template<typename T>
void Build(const parser::WhereStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(WhereStmt)\n";
#endif
}

template<typename T>
void Build(const parser::WriteStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(WriteStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ComputedGotoStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ComputedGotoStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ForallStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ForallStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ArithmeticIfStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ArithmeticIfStmt)\n";
#endif
}

template<typename T>
void Build(const parser::AssignStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AssignStmt)\n";
#endif
}

template<typename T>
void Build(const parser::AssignedGotoStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AssignedGotoStmt)\n";
#endif
}

template<typename T>
void Build(const parser::PauseStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(PauseStmt)\n";
#endif
}

template<typename T>
void Build(const parser::NamelistStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(NamelistStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ParameterStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ParameterStmt)\n";
#endif
}

template<typename T>
void Build(const parser::OldParameterStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OldParameterStmt)\n";
#endif
}

template<typename T>
void Build(const parser::CommonStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CommonStmt)\n";
#endif

   Build(x.blocks, scope);   // std::list<Block> blocks;
}

template<typename T>
void Build(const parser::CommonStmt::Block&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CommonStmt::Block)\n";
#endif

   std::string name;
   if (auto & opt = std::get<0>(x.t)) {   // std::optional<Name>
      Build(opt.value(), name);
      std::cout << "The name of the CommonStmt::Block is " << name << "\n";
   }

   std::list<SgCommonBlockObject*> common_block_object_list;
   Build(std::get<1>(x.t), common_block_object_list);
}

void Build(const parser::CommonBlockObject&x, SgCommonBlockObject* &common_block_object)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CommonBlockObject)\n";
#endif

   std::string name;
   Build(std::get<parser::Name>(x.t), name);
   std::cout << "The name of the CommonBlockObject is " << name << "\n";

#if 0
   if (auto & opt = std::get<1>(x.t)) {   // std::optional<ArraySpec>
   }
#endif

}

   // Expr
template<typename T>
void Build(const parser::CharLiteralConstantSubstring&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CharLiteralConstantSubstring)\n";
#endif
}

void Build(const parser::ArrayConstructor&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ArrayConstructor)\n";
#endif

   Build(x.v, expr);
}

void Build(const parser::AcSpec&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AcSpec)\n";
#endif

   std::list<SgExpression*> acvalue_list;

#if 0
   if (auto & opt = x.type)) {   // std::optional<TypeSpec>
   }
#endif

   Build(x.values, acvalue_list);   // std::list<AcValue> values;
   SgExprListExp* initializers = SageBuilderCpp17::buildExprListExp_nfi(acvalue_list);
   expr = SageBuilderCpp17::buildAggregateInitializer_nfi(initializers);
}

void Build(const parser::AcValue&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AcValue)\n";
#endif

   std::visit(
      common::visitors{
         [&](const common::Indirection<parser::Expr>  &y) { Build(y.value(), expr); },
         [&](const auto &y) { ; },   // Triplet, common::Indirection<AcImpliedDo>

      },
      x.u);
}

template<typename T>
void Build(const parser::StructureConstructor&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(StructureConstructor)\n";
#endif
}

template<typename T>
void Build(const parser::Expr::Parentheses&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Parentheses)\n";
#endif
}

template<typename T>
void Build(const parser::Expr::UnaryPlus&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(UnaryPlus)\n";
#endif
}

template<typename T>
void Build(const parser::Expr::Negate&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Negate)\n";
#endif
}

template<typename T>
void Build(const parser::Expr::NOT&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(NOT)\n";
#endif
}

template<typename T>
void Build(const parser::Expr::PercentLoc&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(PercentLoc)\n";
#endif
}

template<typename T>
void Build(const parser::Expr::DefinedUnary&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DefinedUnary)\n";
#endif
}

void Build(const parser::Expr::Power&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Power)\n";
#endif
}

void Build(const parser::Expr::Multiply&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Multiply)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildMultiplyOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::Divide&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Divide)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildDivideOp_nfi(lhs, rhs);
}

template<typename T>
void traverseBinaryExprs(const T &x, SgExpression* &lhs, SgExpression* &rhs)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::traverseBinaryExprs\n";
#endif

   Build(std::get<0>(x.t).value(), lhs); // lhs Expr
   Build(std::get<1>(x.t).value(), rhs); // rhs Expr
}

void Build(const parser::Expr::Add&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Add)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildAddOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::Subtract&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Subtract)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildSubtractOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::Concat&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Concat)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildConcatenationOp_nfi(lhs, rhs);

}

void Build(const parser::Expr::LT&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LT)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildLessThanOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::LE&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LE)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildLessOrEqualOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::EQ&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EQ)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildEqualityOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::NE&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(NE)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildNotEqualOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::GE&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(GE)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildGreaterOrEqualOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::GT&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(GT)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildGreaterThanOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::AND&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AND)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildAndOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::OR&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OR)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildOrOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::EQV&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EQV)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildEqualityOp_nfi(lhs, rhs);
}

void Build(const parser::Expr::NEQV&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(NEQV)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;
   traverseBinaryExprs(x, lhs, rhs);

   expr = SageBuilderCpp17::buildNotEqualOp_nfi(lhs, rhs);
}

template<typename T>
void Build(const parser::Expr::DefinedBinary&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DefinedBinary)\n";
#endif
}

template<typename T>
void Build(const parser::Expr::ComplexConstructor&x, T* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ComplexConstructor)\n";
#endif
}

void Build(const parser::StructureComponent&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(StructureComponent)\n";
#endif
}

void Build(const parser::ArrayElement&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ArrayElement)\n";
#endif

   SgExpression * lhs = nullptr, * rhs = nullptr;

   Build(x.base, lhs);        // DataRef
   Build(x.subscripts, rhs);  // std::list<SectionSubscript>

   expr = SageBuilderCpp17::buildPntrArrRefExp_nfi(lhs, rhs);
}

void Build(const parser::CoindexedNamedObject&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CoindexedNamedObject)\n";
#endif

   Build(x.base, expr);          // DataRef
   Build(x.imageSelector, expr); // ImageSelector
}

void Build(const parser::ImageSelector&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImageSelector)\n";
#endif

   Build(std::get<0>(x.t), expr);  // std::list<Cosubscript> - Cosubscript = Scalar<IntExpr>
   Build(std::get<1>(x.t), expr);  // std::list<ImageSelectorSpec>
}

void Build(const parser::ImageSelectorSpec&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ImageSelectorSpec)\n";
#endif

   // Stat, TeamValue, Team_Number
   //   auto ImageSelectorSpecVisitor = [&] (const auto &y) { Build(y, expr); };
   //   std::visit(ImageSelectorSpecVisitor, x.u);
}

void Build(const parser::SectionSubscript&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SectionSubscript)\n";
#endif

   std::visit(
      common::visitors{
         [&](const parser::Integer<common::Indirection<parser::Expr>>  &y) { Build(y.thing.value(), expr); },
         [&](const parser::SubscriptTriplet &y) { Build(y, expr); },
      },
      x.u);
}

void Build(const parser::SubscriptTriplet&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SubscriptTriplet)\n";
#endif
}

   // ExecutableConstruct
template<typename T>
void Build(const parser::AssociateConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AssociateConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::BlockConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(BlockConstruct)\n";
#endif

   // std::tuple<Statement<BlockStmt>, BlockSpecificationPart, Block, Statement<EndBlockStmt>>

   Build(std::get<2>(x.t), scope);   // Block
}

template<typename T>
void Build(const parser::CaseConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CaseConstruct)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Case)\n";
#endif

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CaseStmt)\n";
#endif

   //  std::tuple<CaseSelector, std::optional<Name>> t;
   Build(std::get<0>(x.t), case_list);
}

void Build(const parser::CaseSelector&x, std::list<SgExpression*> &case_list)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CaseSelector)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const parser::Default &y) { ; },
         [&] (const auto &y) { Build(y, case_list); }, // std::list<CaseValueRange>
      },
      x.u);
}

void Build(const parser::CaseValueRange&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CaseValueRange)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const parser::CaseValue &y) { Build(y.thing, expr); },  // using CaseValue = Scalar<ConstantExpr>
         [&] (const parser::CaseValueRange::Range &y) { Build(y, expr); },
      },
      x.u);
}

void Build(const parser::CaseValueRange::Range&x, SgExpression* &range)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Range)\n";
#endif

   SgExpression * lower = nullptr, * upper = nullptr;
   auto & lower_expr = x.lower;
   auto & upper_expr = x.upper;

   if (lower_expr) {
      Build(lower_expr->thing, lower);   // CaseValue = Scalar<ConstantExpr>

   } else {
      lower = SageBuilderCpp17::buildNullExpression_nfi();
   }

   if (upper_expr) {
      Build(upper_expr->thing, upper);   // CaseValue = Scalar<ConstantExpr>
   } else {
      upper = SageBuilderCpp17::buildNullExpression_nfi();
   }

   SgExpression* stride = SageBuilderCpp17::buildIntVal_nfi(1);
   range = SageBuilderCpp17::buildSubscriptExpression_nfi(lower, upper, stride);
}

template<typename T>
void Build(const parser::ChangeTeamConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ChangeTeamConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::CriticalConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CriticalConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::LabelDoStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LabelDoStmt)\n";
#endif
}

template<typename T>
void Build(const parser::EndDoStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EndDoStmt)\n";
#endif
}

template<typename T>
void Build(const parser::DoConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DoConstruct)\n";
#endif

   //  std::tuple<Statement<NonLabelDoStmt>, Block, Statement<EndDoStmt>> t;
   //  bool IsDoNormal() const;  bool IsDoWhile() const; bool IsDoConcurrent() const;

   SgStatement* block_stmt{nullptr};
   SgWhileStmt* while_stmt{nullptr};
   SgExpression* condition{nullptr};

   // Traverse NonLabelDoStmt to get the loop condition
   Build(std::get<0>(x.t).statement, condition);

   // Enter SageTreeBuilder if is DoWhile
   if (x.IsDoWhile()) {
      builder.Enter(while_stmt, condition);
   }

   // Traverse the body
   Build(std::get<1>(x.t), block_stmt);

   // Leave SageTreeBuilder
   if (x.IsDoWhile()) {
      builder.Leave(while_stmt, true /* has_end_do_stmt */);
   }
}

template<typename T>
void Build(const parser::IfConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(IfConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::SelectRankConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SelectRankConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::SelectTypeConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(SelectTypeConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::WhereConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(WhereConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::ForallConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ForallConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::CompilerDirective&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(CompilerDirective)\n";
#endif
}

template<typename T>
void Build(const parser::OpenMPConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OpenMPConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::OpenACCConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OpenACConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::OmpEndLoopDirective&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OmpEndLoopDirective)\n";
#endif
}

// DoConstruct
void Build(const parser::NonLabelDoStmt&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(NonLabelDoStmt)\n";
#endif

#if 0
   if (auto & opt = std::get<0>(x.t)) {   // std::optional<Name>
      Build(opt.value(), expr);
   }
#endif

   if (auto & opt = std::get<1>(x.t)) {   // std::optional<LoopControl>
      Build(opt.value(), expr);
   }
}

void Build(const parser::LoopControl&x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LoopControl)\n";
#endif

   //  std::variant<Bounds, ScalarLogicalExpr, Concurrent> u;
   //  using Bounds = LoopBounds<ScalarName, ScalarExpr>;
   // struct LoopBounds { VAR name;  BOUND lower, upper;  std::optional<BOUND> step; }

   std::string name;
   SgExpression * lower_bound = nullptr, * upper_bound = nullptr;

   std::visit(
      common::visitors{
         [&] (const parser::LoopControl::Bounds &y)
            {
#if 0
               name = y.name.thing.ToString();
               std::cout << "the name in the loop control is " << name << "\n";

               Build(y.lower.thing.value(), lower_bound);
               Build(y.upper.thing.value(), upper_bound);
#endif
            },
         [&] (const parser::LoopControl::Concurrent &y) { ; },
         [&] (const auto &y) { Build(y, expr); }, // ScalarLogicalExpr
      },
      x.u);
}

   // SpecificationConstruct
template<typename T>
void Build(const parser::DerivedTypeDef&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(DerivedTypeDef)\n";
#endif

   // std::tuple<Statement<DerivedTypeStmt>, std::list<Statement<TypeParamDefStmt>>,
   //     std::list<Statement<PrivateOrSequence>>, std::list<Statement<ComponentDefStmt>>,
   //     std::optional<TypeBoundProcedurePart>, Statement<EndTypeStmt>> t;

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
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(EnumDef)\n";
#endif
}

template<typename T>
void Build(const parser::InterfaceBlock&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(InterfaceBlock)\n";
#endif
}

template<typename T>
void Build(const parser::StructureDef&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(StructureDef)\n";
#endif
}

template<typename T>
void Build(const parser::OtherSpecificationStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OtherSpecificationStmt)\n";
#endif

   std::visit(
      common::visitors{
         [&] (const common::Indirection<parser::  CommonStmt> &y) { Build(y.value(), scope); },
         [&] (const common::Indirection<parser::NamelistStmt> &y) { Build(y.value(), scope); },
         // common::Indirection -
         // AccessStmt, AllocatableStmt, AsynchronousStmt, BindStmt, CodimensionStmt, ContiguousStmt,
         // DimensionStmt, ExternalStmt, IntentStmt, IntrinsicStmt, OptionalStmt, PointerStmt, ProtectedStmt,
         // SaveStmt, TargetStmt, ValueStmt, VolatileStmt, CommonStmt, EquivalenceStmt, BasedPointerStmt
         [&] (const auto &y) { ; },
      },
      x.u);

}

template<typename T>
void Build(const parser::GenericStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(GenericStmt)\n";
#endif
}

template<typename T>
void Build(const parser::ProcedureDeclarationStmt&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(ProcedureDeclarationStmt)\n";
#endif
}

template<typename T>
void Build(const parser::OpenMPDeclarativeConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OpenMPDeclarativeConstruct)\n";
#endif
}

template<typename T>
void Build(const parser::OpenACCDeclarativeConstruct&x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(OpenACCDeclarativeConstruct)\n";
#endif
}

   // AttrSpec
template<typename T>
void Build(const parser::AccessSpec &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(AccessSpec)\n";
#endif
}

template<typename T>
void Build(const parser::Allocatable &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Allocatable)\n";
#endif
}

template<typename T>
void Build(const parser::Asynchronous &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Asynchronous)\n";
#endif
}

template<typename T>
void Build(const parser::Contiguous &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Contiguous)\n";
#endif
}

template<typename T>
void Build(const parser::External &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(External)\n";
#endif
}

template<typename T>
void Build(const parser::IntentSpec &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(IntentSpec)\n";
#endif
}

template<typename T>
void Build(const parser::Intrinsic &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Intrinsic)\n";
#endif
}

template<typename T>
void Build(const parser::LanguageBindingSpec &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(LanguageBindingSpec)\n";
#endif
}

template<typename T>
void Build(const parser::Optional &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Optional)\n";
#endif
}

template<typename T>
void Build(const parser::Parameter &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Parameter)\n";
#endif
}

template<typename T>
void Build(const parser::Pointer &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Pointer)\n";
#endif
}

template<typename T>
void Build(const parser::Protected &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Protected)\n";
#endif
}

template<typename T>
void Build(const parser::Save &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Save)\n";
#endif
}

template<typename T>
void Build(const parser::Target &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Target)\n";
#endif
}

template<typename T>
void Build(const parser::Value &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Value)\n";
#endif
}

template<typename T>
void Build(const parser::Volatile &x, T* scope)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(Volatile)\n";
#endif
}

} // namespace Rose::builder
