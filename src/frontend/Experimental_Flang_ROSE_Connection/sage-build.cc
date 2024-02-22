#include "sage3basic.h"

#include "sage-build.h"
#include "unparse-sage.h"
#include "flang-sage.h"
#include "SageTreeBuilder.h"

#include "BuildExprVisitor.h"
#include "BuildVisitor.h"

#include <boost/optional.hpp>
#include <iostream>

#include "type-parsers.h"

// Controls debugging information output
#define PRINT_FLANG_TRAVERSAL 0

#define ABORT_NO_IMPL ROSE_ABORT()
#define ABORT_NO_TEST ROSE_ABORT()

#define REPLACE 0

#define DO_TODO 0
#define DEPRECATED 0

namespace Fortran::parser {

void UnparseSage(llvm::raw_ostream &out, const Fortran::parser::Program &program,
    Fortran::parser::Encoding encoding, bool capitalizeKeywords, bool backslashEscapes,
    Fortran::parser::preStatementType* preStatement /*, AnalyzedObjectsAsFortran* */) {

  std::cerr << "UnparseSage:: found it \n";
  ABORT_NO_IMPL;
}

void Replace(Fortran::parser::IntLiteralConstant &x, const SgExpression* sg) {
  auto sage{isSgLongLongIntVal(sg)};
  ASSERT_not_null(isSgLongLongIntVal(sg));

  // Testing for now
#if REPLACE
  std::string value{sage->get_valueString()};

  std::cerr << "Replace(Fortran::parser::IntLiteralConstant: need memory for string " << value << "\n";
  static const char* tt = "666xxx";

  // Need to also try replacing with a KindParam
  parser::CharBlock cb{tt,3};
  std::tuple<parser::CharBlock, std::optional<parser::KindParam>> tup{std::move(cb),std::nullopt};

  x.t = std::move(tup);
#endif
}

}// namespace Fortran::parser

// Dump debug information
template <class T> static void info(const T &x) {
  std::cerr << "Rose::builder::Build() for flang type " << typeid(x).name() << "\n";
}
template <class T> static void info(const T &x, const std::string &pre) {
  std::cerr << pre;
  std::cerr << "Rose::builder::Build() for flang type " << typeid(x).name() << "\n";
}

namespace Rose::builder {

using namespace Fortran;

// Helps with finding source position information
enum class Order {begin, end};

// Why is this needed?
template <typename T> void WalkExpr(T &root, SgExpression* &expr);

template <typename T>
void BuildExprVisitor::BuildExpressions(T &x, SgExpression* &lhs, SgExpression* &rhs) {
  WalkExpr(std::get<0>(x.t).value(), lhs); // lhs Expr
  WalkExpr(std::get<1>(x.t).value(), rhs); // rhs Expr
}

// Name
void BuildExprVisitor::Build(Fortran::parser::Name &x) {
  std::string name{x.ToString()};
  this->set(SageBuilderCpp17::buildVarRefExp_nfi(name));
}

// IntLiteralConstant
void BuildExprVisitor::Build(Fortran::parser::IntLiteralConstant &x) {
  SgExpression* sg{nullptr};
  BuildImpl(x, sg);
  Replace(x, sg);
  this->set(sg);
}

// RealLiteralConstant
void BuildExprVisitor::Build(Fortran::parser::RealLiteralConstant &x) {
  SgExpression* sg{nullptr};
  BuildImpl(x, sg);
#if REPLACE
  Replace(x, sg);
#endif
  this->set(sg);
}

// AssumedImpliedSpec
void BuildExprVisitor::Build(Fortran::parser::AssumedImpliedSpec &x) {
  SgExpression* sg{nullptr};
  BuildImpl(x, sg);
#if REPLACE
  Replace(x, sg);
#endif
  this->set(sg);
}

// AssumedShapeSpec
void BuildExprVisitor::Build(Fortran::parser::AssumedShapeSpec &x) {
  SgExpression* sg{nullptr};
  BuildImpl(x, sg);
#if REPLACE
  Replace(x, sg);
#endif
  this->set(sg);
}

// ExplicitShapeSpec
void BuildExprVisitor::Build(Fortran::parser::ExplicitShapeSpec &x) {
  SgExpression* sg{nullptr};
  BuildImpl(x, sg);
#if REPLACE
  Replace(x, sg);
#endif
  this->set(sg);
}

//-------------------------------------------

  // The Build functions need to be turned into a class (global variable used for now)

  // This constructor is temporary until ROSE supports C++17
  // WARNING, requires that setSgSourceFile be called (see below) before fully constructed
  SageTreeBuilder builder{SageTreeBuilder::LanguageEnum::Fortran};
  // TEMPORARY until C++17
  void setSgSourceFile(SgSourceFile* sg_file) {
    builder.setSourceFile(sg_file);
  }

  // TODO: change this to a reference
  Fortran::parser::AllCookedSources* cooked_{nullptr};

  template<typename T> SourcePosition BuildSourcePosition(const Fortran::parser::Statement<T> &x, Order from)
  {
    std::optional<SourcePosition> pos{std::nullopt};

    if (auto sourceInfo{cooked_->GetSourcePositionRange(x.source)}) {
      if (from == Order::begin)
        pos.emplace(SourcePosition{sourceInfo->first.path, sourceInfo->first.line, sourceInfo->first.column});
      else
        pos.emplace(SourcePosition{sourceInfo->second.path, sourceInfo->second.line, sourceInfo->second.column});
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
    const auto &omp_stmts{std::get<0>(x.t)};
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
      std::cout << "... FirstSourcePosition: implicit_part_stmts list count is " << implicit_part_stmts.size() << "\n";
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

// Callback for the Flang parser. Converts a parsed
// Fortran::Parser::Program to ROSE Sage nodes.
void Build(parser::Program &x, parser::AllCookedSources &cooked) {
  BuildVisitor visitor{cooked};

  // TODO: make go away
  cooked_ = &cooked;

  // Testing...
  parser::Encoding encoding{Fortran::parser::Encoding::LATIN_1};
  parser::Unparse(llvm::outs(), x, encoding, true /*capitalize*/, false, cooked_, nullptr);

  // Initialize SageBuilder global scope
  SgScopeStatement* scope{nullptr};
  builder.Enter(scope);

  // Start by building ProgramUnit(s)
  Walk(x.v, visitor);

  // Root of tree, finished
  visitor.Done();

  builder.Leave(scope);
}

// MainProgram
void BuildVisitor::Build(parser::MainProgram &x) {
  // std::tuple<>
  //   std::optional<Statement<ProgramStmt>>, SpecificationPart, ExecutionPart, std::optional<InternalSubprogramPart>,
  //   Statement<EndProgramStmt>>
  using namespace Fortran::parser;

  auto &stmt{std::get<std::optional<Statement<ProgramStmt>>>(x.t)};
  auto &spec{std::get<SpecificationPart>(x.t)};
  auto &end{std::get<Statement<EndProgramStmt>>(x.t)};

  std::vector<std::string> labels{};
  std::optional<SourcePosition> srcPosBody{std::nullopt};
  std::optional<SourcePosition> srcPosBegin{BuildSourcePosition(stmt, Order::begin)};
  SourcePosition srcPosEnd{BuildSourcePosition(end, Order::end)};
  std::vector<Rose::builder::Token> comments{};

  std::optional<std::string> name{std::nullopt};

  // ProgramStmt is optional
  if (stmt) {
    name.emplace(stmt.value().statement.v.ToString());
  }
  if (stmt && stmt->label) {
    labels.push_back(std::to_string(stmt->label.value()));
  }

  if (auto pos{FirstSourcePosition(spec)}) {
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
  SgProgramHeaderStatement* programDecl{nullptr};
  boost::optional<std::string> boostName{*name};

  builder.Enter(programDecl, boostName, labels, SourcePositions{*srcPosBegin,*srcPosBody,srcPosEnd}, comments);

  // SpecificationPart, ExecutionPart, and optional InternalSubprogramPart
  Walk(std::get<SpecificationPart>(x.t));
  Walk(std::get<ExecutionPart>(x.t));
  Walk(std::get<std::optional<InternalSubprogramPart>>(x.t));

  // EndProgramStmt
  boost::optional<std::string> endName {boost::none};
  boost::optional<std::string> endLabel{boost::none};
  if (end.statement.v) {
    endName = end.statement.v.value().ToString();
  }
  if (end.label) {
    endLabel = std::to_string(end.label.value());
  }

  // Fortran specific functionality
  builder.setFortranEndProgramStmt(programDecl, endName, endLabel);
  builder.Leave(programDecl);
}

void DummyArg(std::list<parser::DummyArg> &x, std::list<std::string> &args) {
  // std::variant<> Name, Star
  using namespace Fortran::parser;

  for (const auto &arg : x) {
    common::visit(common::visitors{
        [&](const Name &y) {args.push_back(y.ToString());},
        [&](const Star &y) {args.push_back(std::string("*"));}
      },
      arg.u);
  }
}

void getSubroutineStmt(parser::SubroutineStmt &x, std::list<std::string> &args,
                       LanguageTranslation::FunctionModifierList &modifiers)
{
  // std::tuple<> std::list<PrefixSpec>, Name, std::list<DummyArg>, std::optional<LanguageBindingSpec>
  using namespace Fortran::parser;

  // DummyArg list
  DummyArg(std::get<std::list<parser::DummyArg>>(x.t), args);

  if (auto &opt = std::get<std::optional<LanguageBindingSpec>>(x.t)) {
    // WARNING, likely need optional expression (or NullExpressions?)
    //BuildExpr(opt.value(), expr);
    //WalkExpr(opt.value(), expr);
    LanguageTranslation::ExpressionKind m;
    getModifiers(opt.value(), m);
#if 0
    functionModifiers.push_back(m);
#endif
    ABORT_NO_IMPL;
  }
}

void BuildVisitor::Build(parser::InternalSubprogramPart &x) {
  // std::tuple<> Statement<ContainsStmt>, std::list<InternalSubprogram>
  using namespace Fortran::parser;
  Walk(std::get<Statement<ContainsStmt>>(x.t));
  Walk(std::get<std::list<InternalSubprogram>>(x.t));
}

void BuildVisitor::Build(parser::ContainsStmt &x) {
  // ContainsStmt is an empty class
  SgContainsStatement* stmt{nullptr};
  builder.Enter(stmt);
  builder.Leave(stmt);
}

// SubroutineSubprogram
void BuildVisitor::Build(parser::SubroutineSubprogram &x) {
  // std::tuple<> - Statement<SubroutineStmt>, SpecificationPart, ExecutionPart, std::optional<InternalSubprogramPart>,
  //                Statement<EndSubroutineStmt>
  using namespace Fortran::parser;

  auto &stmt{std::get<Statement<SubroutineStmt>>(x.t)};
  auto &end{std::get<Statement<EndSubroutineStmt>>(x.t)};

  std::vector<std::string> labels{};
  std::optional<SourcePosition> srcPosBody{FirstSourcePosition(std::get<SpecificationPart>(x.t))};
  std::optional<SourcePosition> srcPosBegin{BuildSourcePosition(stmt, Order::begin)};
  SourcePosition srcPosEnd{BuildSourcePosition(end, Order::end)};
  std::vector<Rose::builder::Token> comments{};

  // There need not be any statements
  if (!srcPosBody) {
    srcPosBody.emplace(srcPosEnd);
  }
  Rose::builder::SourcePositions sources(*srcPosBegin, *srcPosBody, srcPosEnd);

  SgFunctionParameterList* paramList{nullptr};
  SgScopeStatement* paramScope{nullptr};
  SgFunctionDeclaration* funcDecl{nullptr};

  std::list<std::string> dummyArgs;
  LanguageTranslation::FunctionModifierList modifiers;
  getSubroutineStmt(stmt.statement, dummyArgs, modifiers);

  // Enter SageTreeBuilder for SgFunctionParameterList
  bool isDefDecl{true};
  std::string name{std::get<Name>(stmt.statement.t).ToString()};
  builder.Enter(paramList, paramScope, name, /*function_type*/nullptr, isDefDecl);

  // SpecificationPart and ExecutionPart
  Walk(std::get<SpecificationPart>(x.t));
  Walk(std::get<ExecutionPart>(x.t));

  // Leave SageTreeBuilder for SgFunctionParameterList
  builder.Leave(paramList, paramScope, dummyArgs);

  // Begin SageTreeBuilder for SgFunctionDeclaration
  builder.Enter(funcDecl, name, /*return_type*/nullptr, paramList,
                modifiers, isDefDecl, sources, comments);

  // EndSubroutineStmt - std::optional<Name> v;
  bool haveEndStmt{false};
  if (end.statement.v) {
    haveEndStmt = true;
  }

  // InternalSubprogramPart is optional
  Walk(std::get<std::optional<InternalSubprogramPart>>(x.t));

  // Leave SageTreeBuilder for SgFunctionDeclaration
  builder.Leave(funcDecl, paramScope, haveEndStmt);
}

// FunctionSubprogram
void BuildVisitor::Build(parser::FunctionSubprogram &x)
{
  // std::tuple<> Statement<FunctionStmt>, SpecificationPart, ExecutionPart,
  //              std::optional<InternalSubprogramPart>, Statement<EndFunctionStmt>
  std::cout << "BuildVisitor::Build(FunctionSubprogram)\n";
  using namespace Fortran::parser;

  // FunctionStmt - std::tuple<> std::list<PrefixSpec>, Name, std::list<Name>, std::optional<Suffix>
  auto &stmt{std::get<Statement<FunctionStmt>>(x.t)};
  auto &end{std::get<Statement<EndFunctionStmt>>(x.t)};

  std::optional<SourcePosition> srcPosBody{FirstSourcePosition(std::get<SpecificationPart>(x.t))};
  std::optional<SourcePosition> srcPosBegin{BuildSourcePosition(stmt, Order::begin)};
  SourcePosition srcPosEnd{BuildSourcePosition(end, Order::end)};

  // There need not be any statements
  if (!srcPosBody) {
    srcPosBody.emplace(srcPosEnd);
  }
  Rose::builder::SourcePositions sources(*srcPosBegin, *srcPosBody, srcPosEnd);

  SgFunctionParameterList* paramList{nullptr};
  SgScopeStatement* paramScope{nullptr};
  SgFunctionDeclaration* functionDecl{nullptr};
  SgType* returnType{nullptr};
  LanguageTranslation::FunctionModifierList modifiers;
  std::vector<Rose::builder::Token> comments{};
  std::string resultName;
  bool isDefDecl{true};

  std::string name{std::get<Name>(stmt.statement.t).ToString()};

  std::list<std::string> dummyArgs;
  for (const auto &arg : std::get<std::list<Name>>(stmt.statement.t)) {
    dummyArgs.push_back(arg.ToString());
  }

  // PrefixSpec
  BuildPrefix(std::get<std::list<parser::PrefixSpec>>(stmt.statement.t), modifiers, returnType);

  // Suffix
  bool undeclaredResultName{false};
  auto &suffix{std::get<std::optional<Suffix>>(stmt.statement.t)};
  if (suffix && suffix->resultName) {
    //TODO: LanguageBinding also in suffix
    resultName = suffix->resultName.value().ToString();
  }
  if (!resultName.empty() && returnType) {
    undeclaredResultName = true;
  }

  // Peek into the SpecificationPart to get the return type if don't already know it
  if (!returnType) {
    BuildFunctionReturnType(std::get<parser::SpecificationPart>(x.t), resultName, returnType);
  }

  // Enter SageTreeBuilder for SgFunctionParameterList
  builder.Enter(paramList, paramScope, name, returnType, isDefDecl);

  // SpecificationPart
  Walk(std::get<SpecificationPart>(x.t));

  // Need to create initialized name here for result, if result is not declared in SpecificationPart
  if (undeclaredResultName) {
    SageBuilderCpp17::fixUndeclaredResultName(resultName, paramScope, returnType);
  }

  // ExecutionPart
  Walk(std::get<ExecutionPart>(x.t));

  // Leave SageTreeBuilder for SgFunctionParameterList
  builder.Leave(paramList, paramScope, dummyArgs);

#if 0
  Rose::builder::SourcePosition headingStart; // start of ProcedureHeading
  Rose::builder::SourcePosition declStart, declEnd; // start and end of Declaration
  Rose::builder::SourcePositions sources(headingStart, declStart, declEnd);
#endif

  // Begin SageTreeBuilder for SgFunctionDeclaration
  builder.Enter(functionDecl, name, returnType, paramList, modifiers, isDefDecl, sources, comments);

  // EndFunctionStmt - std::optional<Name>
  bool haveEndStmt{false};
  if (end.statement.v) {
    haveEndStmt = true;
  }

  // InternalSubprogramPart is optional
  Walk(std::get<std::optional<InternalSubprogramPart>>(x.t));

  // Leave SageTreeBuilder for SgFunctionDeclaration
  builder.Leave(functionDecl, paramScope, haveEndStmt, resultName);
}

// Module
void BuildVisitor::Build(parser::Module &x)
{
  // std::tuple<> Statement<ModuleStmt>, SpecificationPart, std::optional<ModuleSubprogramPart>,
  //              Statement<EndModuleStmt>
  using namespace Fortran::parser;

  auto &stmt{std::get<Statement<ModuleStmt>>(x.t)};
  auto &end{std::get<Statement<EndModuleStmt>>(x.t)};

  SgModuleStatement* module{nullptr};
  builder.Enter(module, stmt.statement.v.ToString());

  Walk(std::get<parser::SpecificationPart>(x.t));
  Walk(std::get<std::optional<ModuleSubprogramPart>>(x.t));

  // EndModuleStmt - std::optional<Name> v;
  std::string endName;
  if (end.statement.v) {
    endName = end.statement.v->ToString();
  }

  // Leave SageTreeBuilder for SgModuleStatement
  builder.Leave(module);
}

void BuildVisitor::Build(parser::ModuleSubprogramPart &x)
{
  // std::tuple<> Statement<ContainsStmt>, std::list<ModuleSubprogram>

  // ContainsStmt
  SgContainsStatement* contains{nullptr};
  builder.Enter(contains);
  builder.Leave(contains);

  Walk(std::get<std::list<parser::ModuleSubprogram>>(x.t));
}

// Submodule
void Build(parser::Submodule &x) {
  // std::tuple<> Statement<SubmoduleStmt>, SpecificationPart,
  //              std::optional<ModuleSubprogramPart>, Statement<EndSubmoduleStmt>

  std::cout << "Rose::builder::Build(Submodule)\n";
  ABORT_NO_IMPL;
}

// BlockData
void Build(parser::BlockData &x) {
  // std::tuple<> Statement<BlockDataStmt>, SpecificationPart, Statement<EndBlockDataStmt>
  std::cout << "Rose::builder::Build(BlockData)\n";

  // BlockDataStmt std::optional<Name> v;
  // EndBlockDataStmt std::optional<Name> v;

  ABORT_NO_IMPL;
}

void BuildFunctionReturnType(const parser::SpecificationPart &x, std::string &result_name, SgType* &return_type)
{
  std::cout << "Rose::builder::Build(SpecificationPart)\n";

#if 0
    // Look for the variable declaration of the result to get the function return type

    const auto & decl_construct = std::get<std::list<parser::DeclarationConstruct>>(x.t);

    for (const auto &elem : decl_construct) {
      const auto & spec_construct = std::get<parser::SpecificationConstruct>(elem.u);
      const auto & type_decl_stmt = std::get<parser::Statement<common::Indirection<parser::TypeDeclarationStmt>>>(spec_construct.u).statement.value();
      const auto & entity_decl = std::get<std::list<parser::EntityDecl>>(type_decl_stmt.t);

      for (const auto &name : entity_decl) {
        // Look for the result name
        if (std::get<0>(name.t).ToString() == result_name) {
          // When result name is found, get the return type
          const auto & decl_type_spec = std::get<parser::DeclarationTypeSpec>(type_decl_stmt.t);
          Build(decl_type_spec, return_type);
        }
      }
    }
#endif
}


void BuildVisitor::Build(parser::AssignmentStmt &x)
{
  // std::tuple<> Variable, Expr
  using namespace Fortran::parser;

  std::vector<std::string> labels{};
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  SgExprStatement* stmt{nullptr};

  WalkExpr(std::get<Variable>(x.t), lhs);
  WalkExpr(std::get<Expr>(x.t), rhs);

  std::vector<SgExpression*> vars;
  vars.push_back(lhs);

  // Begin SageTreeBuilder
  builder.Enter(stmt, rhs, vars);

  // Leave SageTreeBuilder
  builder.Leave(stmt, labels);
}

void Build(parser::FunctionStmt &x, std::list<std::string> &dummy_arg_name_list, std::string &name, std::string &result_name, LanguageTranslation::FunctionModifierList &function_modifiers, SgType* &type)
{
  using namespace Fortran::parser;
  info(x, "Rose::builder::Build(FunctionStmt)");
  ABORT_NO_IMPL;

#if 0
  Build(std::get<std::list<PrefixSpec>>(x.t), function_modifiers, type);
  Build(std::get<Name>(x.t), name);
  Build(std::get<std::list<Name>>(x.t), dummy_arg_name_list);

  if (auto & opt = std::get<std::optional<Suffix>>(x.t)) {
    Build(opt.value(), result_name);
  }
#endif
}

void BuildVisitor::
BuildPrefix(std::list<parser::PrefixSpec> &x, LanguageTranslation::FunctionModifierList &modifiers, SgType* &type)
{
  // std::variant<> - DeclarationTypeSpec, Elemental, Impure, Module, Non_Recursive,
  //                  Pure, Recursive, Attributes, Launch_Bounds, Cluster_Dims
  std::cout << "[WARN] BuildVisitor::Build(PrefixSpec): NEEDS further IMPLEMENTATION\n";

  for (auto &prefix : x) {
    common::visit(
        common::visitors{
            [&] (parser::DeclarationTypeSpec &y) {
                   BuildType(y, type);
                },
            [&] (auto &y) {
                   std::cout << "   [WARN] IMPL_ME_ SOMETHING something else\n";
                }
        },
        prefix.u);
  }
}

void BuildSuffix(parser::Suffix &x, std::string &resultName)
{
  std::cout << "BuildVisitor::BuildSuffix(Suffix)\n";

  // std::optional<Name>
  if (x.resultName) {
    resultName = x.resultName.value().ToString();
  }

  // TODO:
  //  std::optional<LanguageBindingSpec> binding;
  ABORT_NO_IMPL;
}

void Build(parser::Substring &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(Substring)\n";
  ABORT_NO_IMPL;
}

void Build(parser::FunctionReference &x, SgExpression* &expr)
{
#if PRINT_FLANG_TRAVERSAL
   std::cout << "Rose::builder::Build(FunctionReference)\n";
#endif

   std::list<SgExpression*> arg_list;
   std::string func_name;

   Build(x.v, arg_list, func_name); // Call

   SgExprListExp* param_list = SageBuilderCpp17::buildExprListExp_nfi(arg_list);

   // Begin SageTreeBuilder
   SgFunctionCallExp* func_call;
   builder.Enter(func_call, func_name, param_list);

   // Use wrapper function because can't use inheritance of pointers until Rose accepts Cpp17
   expr = SageBuilder::buildFunctionCallExp(func_call);

}

void Build(parser::Call &x, std::list<SgExpression*> &arg_list, std::string &name)
{
  std::cout << "Rose::builder::Build(Call)\n";
  ABORT_NO_IMPL;

#if 0
   SgExpression* expr{nullptr};

   Build(std::get<0>(x.t), expr, name);   // ProcedureDesignator
   Build(std::get<1>(x.t), arg_list);     // std::list<ActualArgSpec>
#endif
}

void Build(parser::ProcComponentRef &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(ProcComponentRef)\n";
  ABORT_NO_IMPL;
}

void Build(parser::ActualArgSpec &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(ActualArgSpec)\n";
  ABORT_NO_IMPL;

#if 0
   if (auto & opt = std::get<0>(x.t)) {    // std::optional<Keyword>
      Build(opt.value(), expr);
   }

   Build(std::get<1>(x.t), expr);          // ActualArg
#endif
}

void Build(parser::Keyword &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(Keyword)");
  ABORT_NO_IMPL;
}

void Build(parser::NamedConstant &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(NamedConstant)\n";
  ABORT_NO_IMPL;
}

void Build(parser::Expr::IntrinsicBinary &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(IntrinsicBinary)\n";
  ABORT_NO_IMPL;
}

// LiteralConstant(s)
void BuildImpl(parser::HollerithLiteralConstant &x, SgExpression* &expr)
{
  std::cout << "BuildImpl(HollerithLiteralConstant)\n";
  ABORT_NO_IMPL;
}

// KindParam - for now create a string (seems that a Sage value expression should have Fortran kind
void BuildImpl(std::optional<Fortran::parser::KindParam> &x, std::uint64_t &ikind, std::string &strVal)
{
  // std::variant<> = std::uint64_t, Scalar<Integer<Constant<Name>>>
  using namespace Fortran::parser;

  if (x) {
    common::visit(
        common::visitors{
            [&] (const std::int64_t &y) {
                   ikind = y;
                   strVal = std::to_string(ikind);
                },
            [&] (const Scalar<Integer<Constant<Name>>> &y) {
                   strVal = y.thing.thing.thing.ToString();
                }
        },
        x.value().u);
  }
}

void BuildImpl(parser::IntLiteralConstant &x, SgExpression* &expr)
{
  // std::tuple<> - CharBlock, std::optional<KindParam>
  using namespace Fortran::parser;

  // Use long long as integer representation because we don't really know
  // except for default integer and kind param (compiler dependent).
  long long llVal{0};
  std::string strVal{std::get<CharBlock>(x.t).ToString()};

  try {
    llVal = std::stoll(strVal);
  } catch (const std::out_of_range& e) {
    std::cerr << "[WARN] IntLiteralConstant out of range: " << e.what() << std::endl;
  } catch (const std::invalid_argument& e) {
    std::cerr << "[WARN] IntLiteralConstant invalid argument: " << e.what() << std::endl;
  }

  if (std::get<1>(x.t)) {
    std::string kind{};
    uint64_t ikind{0};
    BuildImpl(std::get<1>(x.t), ikind, kind);
    strVal += "_" + kind;
  }

  expr = SageBuilder::buildLongLongIntVal_nfi(llVal, strVal);
}

void BuildImpl(parser::SignedIntLiteralConstant &x, SgExpression* &expr)
{
  // std::tuple<> - CharBlock, std::optional<KindParam>
  std::cout << "BuildImpl(SignedIntLiteralConstant)\n";
  ABORT_NO_TEST;

  expr = SageBuilder::buildIntVal_nfi(stoi(std::get<0>(x.t).ToString()));
}

void BuildImpl(parser::RealLiteralConstant &x, SgExpression* &expr)
{
  // has std::optional<KindParam> kind
  expr = SageBuilder::buildFloatVal_nfi(x.real.source.ToString());
}

void BuildImpl(parser::SignedRealLiteralConstant &x, SgExpression* &expr)
{
  // std::tuple<std::optional<Sign>, RealLiteralConstant> t;
  ABORT_NO_IMPL;

#if 0
   Build(std::get<1>(x.t), expr);
#endif
}

void BuildImpl(parser::ComplexLiteralConstant &x, SgExpression* &expr)
{
  // std::tuple<ComplexPart, ComplexPart> t;
  std::cout << "BuildImpl(ComplexLiteralConstant)\n";
  ABORT_NO_IMPL;

#if 0
  SgExpression * real_value = nullptr, * imaginary_value = nullptr;

  BuildImpl(std::get<0>(x.t), real_value);
  BuildImpl(std::get<1>(x.t), imaginary_value);

  expr = SageBuilder::buildComplexVal_nfi(real_value, imaginary_value, "");
#endif
}

void BuildImpl(parser::BOZLiteralConstant &x, SgExpression* &expr)
{
  std::cout << "BuildImpl(BOZLiteralConstant)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::CharLiteralConstant &x, SgExpression* &expr)
{
  std::cout << "BuildImpl(CharLiteralConstant)\n";
  ABORT_NO_IMPL;

  expr = SageBuilder::buildStringVal_nfi(x.GetString());
}

void BuildImpl(parser::LogicalLiteralConstant &x, SgExpression* &expr)
{
  std::cout << "BuildImpl(LogicalLiteralConstant)\n";
  ABORT_NO_TEST;

  expr = SageBuilder::buildBoolValExp_nfi(std::get<0>(x.t));
}

void BuildImpl(parser::AssumedImpliedSpec &x, SgExpression* &expr)
{
  // is [ lower-bound : ] *
  // AssumedImpliedSpec std::optional<SpecificationExpr> v;
  SgExpression* ub{SageBuilderCpp17::buildAsteriskShapeExp_nfi()};
  expr = ub;

  // There may be a lower-bound
  if (x.v) {
    SgExpression* lb{nullptr};
    SgExpression* stride{SageBuilder::buildIntVal_nfi(std::string("1"))}; // default stride of 1
    WalkExpr(x.v, lb);
    expr = SageBuilder::buildSubscriptExpression_nfi(lb, ub, stride);
  }
}

void BuildImpl(parser::AssumedShapeSpec &x, SgExpression* &expr)
{
  // is [ lower-bound ] :
  // AssumedShapeSpec std::optional<SpecificationExpr> v;
  SgExpression* lb{nullptr}; // maybe lower-bound
  SgExpression* ub{SageBuilder::buildNullExpression_nfi()}; // no upper-bound
  SgExpression* stride{SageBuilder::buildIntVal_nfi(std::string("1"))}; // default stride of 1

  if (x.v) {
    WalkExpr(x.v, lb);
  }
  expr = SageBuilder::buildSubscriptExpression_nfi(lb, ub, stride);
}

void BuildImpl(parser::ExplicitShapeSpec &x, SgExpression* &expr)
{
  // is [ lower-bound : ] upper-bound
  // ExplicitShapeSpec std::tuple<std::optional<SpecificationExpr>, SpecificationExpr> t;

  // There shall be an upper-bound
  SgExpression* ub{nullptr};
  WalkExpr(std::get<1>(x.t), ub);
  expr = ub;

  // There may be a lower-bound
  if (std::get<0>(x.t)) {
    SgExpression* lb{nullptr};
    SgExpression* stride{SageBuilder::buildIntVal_nfi(std::string("1"))}; // default stride of 1
    WalkExpr(std::get<0>(x.t).value(), lb);
    expr = SageBuilder::buildSubscriptExpression_nfi(lb, ub, stride);
  }
}

void BuildImpl(parser::UseStmt &x)
{
  std::cout << "BuildImpl(UseStmt)\n";
  ABORT_NO_TEST;

#if 0
   std::string module_nature= "";
   if (auto & opt = x.nature) {
      module_nature = parser::UseStmt::EnumToString(opt.value());
   }

   std::string module_name;
   Build(x.moduleName, module_name);

   // TODO
   // std::variant<std::list<Rename>, std::list<Only>> u;

   SgUseStatement* use_stmt{nullptr};
   builder.Enter(use_stmt, module_name, module_nature);
   builder.Leave(use_stmt);
#endif
}

void BuildImpl(const parser::InternalSubprogramPart &x)
{
   // std::tuple<> - Statement<ContainsStmt>, std::list<InternalSubprogram>
  std::cout << "Rose::builder::Build(InternalSubprogramPart)\n";
  ABORT_NO_TEST;

#if 0
   // build ContainsStmt
   //TODO: source position (looks like actual statement not needed, add?)
   SgContainsStatement* containsStmt{nullptr};
   builder.Enter(containsStmt);
   builder.Leave(containsStmt);

   Build(std::get<1>(x.t));
#endif
}

void Build(std::list<parser::ImplicitSpec> &x, std::list<std::tuple<SgType*, std::list<std::tuple<char, boost::optional<char>>>>> &implicit_spec_list)
{
  std::cout << "Rose::builder::Build(std::list<ImplicitSpec>)\n";
  ABORT_NO_IMPL;

#if 0
   for (const auto &elem : x) {
      std::tuple<SgType*, std::list<std::tuple<char, boost::optional<char>>>> implicit_spec;
      SgType* type;
      std::list<std::tuple<char, boost::optional<char>>> letter_spec_list;

      // Get type and list of LetterSpec
      Build(elem, type, letter_spec_list);

      // Make tuple from type and list of LetterSpec, add tuple to ImplicitSpecList
      implicit_spec = std::make_tuple(type, letter_spec_list);
      implicit_spec_list.push_back(implicit_spec);
   }
#endif
}

void Build(parser::ImplicitSpec &x, SgType* &type, std::list<std::tuple<char, boost::optional<char>>> &letter_spec_list)
{
  // std::tuple<DeclarationTypeSpec, std::list<LetterSpec>> t;
  std::cout << "Rose::builder::Build(ImplicitSpec)\n";
  ABORT_NO_IMPL;

#if 0
   Build(std::get<0>(x.t), type);             // DeclarationTypeSpec
   Build(std::get<1>(x.t), letter_spec_list); // LetterSpecList
#endif
}

void Build(std::list<parser::LetterSpec> &x, std::list<std::tuple<char, boost::optional<char>>> &letter_spec_list)
{
  std::cout << "Rose::builder::Build(std::list<LetterSpec>)\n";
  ABORT_NO_IMPL;

#if 0
   for (const auto &elem : x) {
      std::tuple<char, boost::optional<char>> letter_spec;

      // Get LetterSpec tuple
      Build(elem, letter_spec);

      // Add LetterSpec tuple to list of LetterSpec
      letter_spec_list.push_back(letter_spec);
   }
#endif
}

void Build(parser::LetterSpec &x, std::tuple<char, boost::optional<char>> &letter_spec)
{
  // std::tuple<Location, std::optional<Location>> t;
  // using Location = const char *;
  std::cout << "Rose::builder::Build(LetterSpec)\n";
  ABORT_NO_IMPL;

#if 0
   char first;
   boost::optional<char> second;

   first = std::get<0>(x.t)[0];

   if (auto & opt = std::get<1>(x.t)) {
      second = opt.value()[0];
   } else {
      second = boost::none;
   }

   letter_spec = std::make_tuple(first, second);
#endif
}

#define TEMPORARY_COOL_FIXME 0
#if TEMPORARY_COOL_FIXME
const Fortran::parser::ImplicitStmt::ImplicitNoneNameSpec & makeImplicitNone() {
  //  return std::move(Fortran::parser::ImplicitStmt::ImplicitNoneNameSpec::External);
  return Fortran::parser::ImplicitStmt::ImplicitNoneNameSpec::External;
}
#endif

void BuildVisitor::Build(parser::ImplicitStmt &x) {
  // std::variant<> std::list<ImplicitSpec>, std::list<ImplicitNoneNameSpec>
  using namespace Fortran::parser;
  bool implicitNone{false}, implicitExternal{false}, implicitType{false};

  common::visit(common::visitors{
      [&](const std::list<ImplicitSpec> &y) {
            ABORT_NO_IMPL;
          },
      [&](const std::list<ImplicitStmt::ImplicitNoneNameSpec> &y) {
            // ENUM_CLASS(ImplicitNoneNameSpec, External, Type) // R866
            implicitNone = true;
            for (const auto &spec : y) {
              if (spec == ImplicitStmt::ImplicitNoneNameSpec::External) implicitExternal = true;
              if (spec == ImplicitStmt::ImplicitNoneNameSpec::Type) implicitType = true;
            }
          }
    },
    x.u);

  if (implicitNone) {
    SgImplicitStatement* stmt{nullptr};
    builder.Enter(stmt, implicitExternal, implicitType);
    builder.Leave(stmt);
  }

#if TEMPORARY_COOL_FIXME
  // Need variant I think (no, like an enum, just assign to ONE of the variants!)
  //  x.u = std::move(makeImplicitNone());
  //  const std::list<ImplicitStmt::ImplicitNoneNameSpec> &implicitList{makeImplicitNone()};
#else
  const std::list<ImplicitStmt::ImplicitNoneNameSpec>
    &implicitList{ImplicitStmt::ImplicitNoneNameSpec::External, ImplicitStmt::ImplicitNoneNameSpec::Type};
  const ImplicitStmt & xx{std::move(implicitList)};
  x.u = std::move(implicitList);
#endif
}

void BuildVisitor::Build(parser::TypeDeclarationStmt &x) {
  // std::tuple<> DeclarationTypeSpec, std::list<AttrSpec>, std::list<EntityDecl>
  using namespace Fortran::parser;

  SgType* type{nullptr};
  BuildType(std::get<parser::DeclarationTypeSpec>(x.t), type);

  std::list<LanguageTranslation::ExpressionKind> modifiers{};
  for (auto &attr : std::get<std::list<AttrSpec>>(x.t)) {
    getAttrSpec(attr, modifiers, type);
  }

  std::list<EntityDeclTuple> initInfo{};
  EntityDecls(std::get<std::list<EntityDecl>>(x.t), initInfo, type); // std::list<EntityDecl>

  SgVariableDeclaration* varDecl{nullptr};
  builder.Enter(varDecl, type, initInfo);
  builder.Leave(varDecl, modifiers);
}

// SpecificationConstruct
void BuildVisitor::Build(parser::DerivedTypeDef &x) {
  // std::tuple<> Statement<DerivedTypeStmt>, std::list<Statement<TypeParamDefStmt>>,
  //              std::list<Statement<PrivateOrSequence>>,
  //              std::list<Statement<ComponentDefStmt>>,
  //              std::optional<TypeBoundProcedurePart>, Statement<EndTypeStmt>
  using namespace Fortran::parser;

  std::cerr << "BuildVisitor::Build(DerivedTypeDef)\n";

  auto &stmt{std::get<Statement<DerivedTypeStmt>>(x.t)};
  auto &end{std::get<Statement<EndTypeStmt>>(x.t)};

  // DerivedTypeStmt std::tuple<std::list<TypeAttrSpec>, Name, std::list<Name>> t;
  std::string name{std::get<Name>(stmt.statement.t).ToString()};

  SgDerivedTypeStatement* derived{nullptr};
  builder.Enter(derived, name);

  std::list<LanguageTranslation::ExpressionKind> modifiers;
  for (auto &attr : std::get<0>(stmt.statement.t)) {
    LanguageTranslation::ExpressionKind m;
    getModifiers(attr, m);
    modifiers.push_back(m);
  }

  // Leave SageTreeBuilder for SgDerivedTypeStmt
  builder.Leave(derived, modifiers);
}

void Build(parser::DeclarationTypeSpec::Type &x, SgType* &type)
{
   Build(x.derived, type);   // DerivedTypeSpec
}

void Build(parser::DeclarationTypeSpec::TypeStar &x, SgType* &type)
{
  std::cout << "Rose::builder::Build(TypeStar)\n";
  ABORT_NO_IMPL;
}

void Build(parser::DeclarationTypeSpec::Class &x, SgType* &type)
{
  std::cout << "Rose::builder::Build(Class)\n";
  ABORT_NO_IMPL;

#if 0
  Build(x.derived, type);   // DerivedTypeSpec
#endif
}

void Build(parser::DeclarationTypeSpec::ClassStar &x, SgType* &type)
{
  std::cout << "Rose::builder::Build(ClassStar)\n";
  ABORT_NO_IMPL;
}

void Build(parser::DeclarationTypeSpec::Record &x, SgType* &type)
{
  std::cout << "Rose::builder::Build(Record)\n";
  ABORT_NO_IMPL;
}

void Build(parser::DerivedTypeSpec &x, SgType* &type)
{
  std::cout << "Rose::builder::Build(DerivedTypeSpec)\n";
  ABORT_NO_IMPL;

#if 0
   //   std::tuple<Name, std::list<TypeParamSpec>> t;
   std::string name = std::get<parser::Name>(x.t).ToString();
   std::cout << "DerivedTypeSpec name is " << name << "\n";
#endif
}

void Build(parser::AttrSpec &x, LanguageTranslation::ExpressionKind &modifier)
{
  std::cout << "Rose::builder::Build(AttrSpec)\n";
  ABORT_NO_IMPL;
}

void Build(parser::KindSelector &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(KindSelector)\n";
  ABORT_NO_IMPL;
}

void BuildVisitor::Build(parser::IntegerTypeSpec &x)
{
  SgType* type{nullptr};
  if (auto &kind = x.v) {   // std::optional<KindSelector>
    ABORT_NO_IMPL;
#if 0
    SgExpression* kind_expr = nullptr;
    Build(kind.value(), kind_expr);
    type = SageBuilder::buildIntType(kind_expr);
#endif
  } else {
    type = SageBuilder::buildIntType();
  }
  this->set(type); // synthesized attribute
}

void BuildVisitor::Build(parser::IntrinsicTypeSpec::Real &x)
{
  SgType* type{nullptr};
  if (auto &kind = x.kind) {   // std::optional<KindSelector>
#if 0
    SgExpression* kind_expr{nullptr};
    Build(kind.value(), kind_expr);
    type = SageBuilder::buildFloatType(kind_expr);
#endif
  } else {
    type = SageBuilder::buildFloatType();
  }
  this->set(type); // synthesized attribute
}

void BuildVisitor::Build(parser::IntrinsicTypeSpec::DoublePrecision &x)
{
  SgType* type{nullptr};
  std::cout << "Rose::builder::Build(DoublePrecision)\n";
  ABORT_NO_TEST;

  type = SageBuilder::buildDoubleType();
  this->set(type); // sythesized attribute
}

void BuildVisitor::Build(parser::IntrinsicTypeSpec::Complex &x)
{
  SgType* type{nullptr};
  std::cout << "Rose::builder::Build(Complex)\n";
  ABORT_NO_TEST;

  SgType* base_type = SageBuilder::buildIntType();
  type = SageBuilder::buildComplexType(base_type);
  this->set(type); // sythesized attribute
}

void BuildVisitor::Build(parser::IntrinsicTypeSpec::Character &x)
{
  SgType* type{nullptr};
  std::cout << "Rose::builder::Build(Character)\n";
  ABORT_NO_TEST;

  if (auto & opt = x.selector) {    // std::optional<CharSelector> selector
#if 0
    SgExpression* expr{nullptr};
    Build(opt.value(), expr);
    type = SageBuilder::buildStringType(expr);
#endif
  } else {
    type = SageBuilder::buildCharType();
  }
  this->set(type); // synthesized attribute
}

void BuildVisitor::Build(parser::IntrinsicTypeSpec::Logical &x) {
  SgType* type{nullptr};
  if (auto & kind = x.kind) {   // std::optional<KindSelector>
    SgExpression* kindExpr = nullptr;
    ABORT_NO_IMPL;
#if 0
    Build(kind.value(), kindExpr);
    type = SageBuilder::buildBoolType(kindExpr);
#endif
  } else {
    type = SageBuilder::buildBoolType();
  }
  this->set(type); // synthesized attribute
}

void BuildVisitor::Build(parser::IntrinsicTypeSpec::DoubleComplex &x)
{
  std::cout << "Rose::builder::Build(DoubleComplex)\n";
  ABORT_NO_IMPL;

#if 0
  SgType* base_type = SageBuilder::buildDoubleType();
  type = SageBuilder::buildComplexType(base_type);
  this->set(type); // synthesized attribute
#endif
}

void Build(parser::CharSelector &x, SgExpression* &expr)
{
  // std::variant<LengthSelector, LengthAndKind> u;
  std::cout << "Rose::builder::Build(CharSelector)\n";
  ABORT_NO_IMPL;

#if 0
  auto CharSelectorVisitor = [&](const auto& y) { Build(y, expr); };
  std::visit(CharSelectorVisitor, x.u);
#endif
}

void Build(parser::VectorTypeSpec &x, SgType* &type)
{
  std::cout << "Rose::builder::Build(VectorTypeSpec)\n";
  ABORT_NO_IMPL;
}

void Build(parser::LengthSelector &x, SgExpression* &expr)
{
  // std::variant<TypeParamValue, CharLength> u;
  std::cout << "Rose::builder::Build(LengthSelector)\n";
  ABORT_NO_IMPL;

#if 0
   auto LengthSelectorVisitor = [&](const auto& y) { Build(y, expr); };
   std::visit(LengthSelectorVisitor, x.u);
#endif
}

void Build(parser::CharSelector::LengthAndKind &x, SgExpression* &expr)
{
  //    std::optional<TypeParamValue> length;
  //    ScalarIntConstantExpr kind;
  std::cout << "Rose::builder::Build(LengthAndKind)\n";
  ABORT_NO_IMPL;
}

void Build(parser::TypeParamValue &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(TypeParamVale)\n";
  ABORT_NO_IMPL;
}

void EntityDecls(std::list<Fortran::parser::EntityDecl> &x, std::list<EntityDeclTuple> &entityDecls, SgType* baseType)
{
  for (auto &entity : x) {
    SgType* type{nullptr};
    SgExpression* init{nullptr};
    std::string name{std::get<0>(entity.t).ToString()};

    if (auto &opt = std::get<1>(entity.t)) {  // ArraySpec
      Build(opt.value(), type, baseType);
    }
    if (auto &opt = std::get<2>(entity.t)) {  // CoarraySpec
      ABORT_NO_TEST;
      Build(opt.value(), type, baseType);
    }
    if (auto &opt = std::get<3>(entity.t)) {  // CharLength
      WalkExpr(opt.value(), init);
    }
    if (auto &opt = std::get<4>(entity.t)) {  // Initialization
      WalkExpr(opt.value(), init);
    }
    entityDecls.push_back(std::make_tuple(name, type, init));
  }
}

// ArraySpec
void Build(parser::ArraySpec &x, SgType* &type, SgType* baseType)
{
  // std::variant<> - std::list<ExplicitShapeSpec>, std::list<AssumedShapeSpec>,
  //                  DeferredShapeSpecList, AssumedSizeSpec, ImpliedShapeSpec, AssumedRankSpec
  //
  // ExplicitShapeSpec - std::tuple<std::optional<SpecificationExpr>, SpecificationExpr> t;
  // AssumedShapeSpec - std::optional<SpecificationExpr> v;
  // DeferredShapeSpecList - int v;
  // AssumedSizeSpec - std::tuple<std::list<ExplicitShapeSpec>, AssumedImpliedSpec> t;
  // ImpliedShapeSpec - std::list<AssumedImpliedSpec> v;
  // AssumedRankSpec - using EmptyTrait = std::true_type;
  //
  using namespace Fortran::parser;

  SgExpression* expr{nullptr};
  SgExprListExp* dimInfo{SageBuilder::buildExprListExp_nfi()};

  common::visit(common::visitors{
             [&] (std::list<ExplicitShapeSpec> &y) {
                   for (ExplicitShapeSpec &spec : y) { // is [ lower-bound : ] upper-bound
                     WalkExpr(spec, expr=nullptr);
                     dimInfo->get_expressions().push_back(expr);
                   }
               },
             [&] (std::list<AssumedShapeSpec> &y) {
                   for (AssumedShapeSpec &spec : y) { // is [ lower-bound ]:
                     WalkExpr(spec, expr=nullptr);
                     dimInfo->get_expressions().push_back(expr);
                   }
               },
             [&] (DeferredShapeSpecList &y) {
                   for (int ii{0}; ii < y.v; ii++) { // is :
                     dimInfo->get_expressions().push_back(SageBuilder::buildColonShapeExp_nfi());
                   }
               },
             [&] (AssumedSizeSpec &y) {
                   // std::tuple<std::list<ExplicitShapeSpec>, AssumedImpliedSpec> t;
                   for (ExplicitShapeSpec &spec : std::get<0>(y.t)) { // is [ lower-bound : ] upper-bound
                     WalkExpr(spec, expr=nullptr);
                     dimInfo->get_expressions().push_back(expr);
                   }
                   AssumedImpliedSpec &spec{std::get<1>(y.t)}; // is [ lower-bound : ] *
                   WalkExpr(spec, expr=nullptr);
                   dimInfo->get_expressions().push_back(expr);
               },
             [&] (ImpliedShapeSpec &y) {
                   for (AssumedImpliedSpec &spec : y.v) { // is [ lower-bound : ] *
                     WalkExpr(spec, expr=nullptr);
                     dimInfo->get_expressions().push_back(expr);
                   }
                },
             [&] (AssumedRankSpec &y) {
                   // is ..
                   // TODO: Need new espression type in ROSE
                   // dimInfo->get_expressions().push_back(SageBuilder::buildRankShapeExp_nfi());
                   ABORT_NO_IMPL;
                }
           },
     x.u);

  // build the final array type as return value
  type = SageBuilder::buildArrayType(baseType, dimInfo);
}

// CoarraySpec
void Build(parser::CoarraySpec &x, SgType* &type, SgType* baseType)
{
  std::cout << "Rose::builder::Build(CoarraySpec)\n";
  ABORT_NO_IMPL;
}

void Build(parser::CharLength &x, SgExpression* &)
{
   std::cout << "Rose::builder::Build(CharLength)\n";
   ABORT_NO_IMPL;
}

void Build(parser::Initialization &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(Initialization)\n";
   ABORT_NO_IMPL;
}

void Build(parser::SpecificationExpr &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(SpecificationExpr)\n";
  ABORT_NO_IMPL;

  Build(x.v, expr);  // Scalar<IntExpr>
}

void Build(parser::Scalar<parser::IntExpr> &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(Scalar<IntExpr>)");
  ABORT_NO_IMPL;

#if 0
   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), expr);  // Expr
#endif
}

void Build(parser::Scalar<parser::LogicalExpr> &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(Scalar<LogicalExpr>)");
  ABORT_NO_IMPL;

#if 0
   // Scalar<Integer<std::optional::Expr>>
   Build(x.thing.thing.value(), expr);  // Expr
#endif
}

void Build(parser::ConstantExpr &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(Scalar<ConstantExpr>)");
  ABORT_NO_IMPL;

#if 0
   // Constant<common::Indirection<Expr>>
   Build(x.thing.value(), expr);  // Expr
#endif
}

// DeclarationConstruct

void BuildImpl(parser::DataStmt &x)
{
  std::cout << "BuildImpl(DataStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::FormatStmt &x)
{
  std::cout << "BuildImpl(FormatStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::EntryStmt &x)
{
  std::cout << "BuildImpl(EntryStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::StmtFunctionStmt &x)
{
  std::cout << "BuildImpl(StmtFunctionStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::ErrorRecovery &x)
{
  std::cout << "BuildImpl(ErrorRecovery)\n";
  ABORT_NO_IMPL;
}

// DataStmt
void Build(parser::DataStmtValue &x, SgExpression* &expr)
{
  // std::tuple<std::optional<DataStmtRepeat>, DataStmtConstant> t;
  std::cout << "Rose::builder::Build(DataStmtValue)\n";
  ABORT_NO_IMPL;

#if 0
  Build(std::get<parser::DataStmtConstant>(x.t), expr);
#endif
}

void Build(parser::DataStmtConstant &x, SgExpression* &expr)
{
  //     std::variant<LiteralConstant, SignedIntLiteralConstant,
  //      SignedRealLiteralConstant, SignedComplexLiteralConstant, NullInit,
  //      common::Indirection<Designator>, StructureConstructor>  u;
  std::cout << "Rose::builder::Build(DataStmtConstant)\n";
  ABORT_NO_IMPL;
}

// ActionStmt

void Build(parser::ContinueStmt &x, const OptLabel &label)
{
  std::cout << "Rose::builder::Build(ContinueStmt)\n";
  ABORT_NO_IMPL;

#if 0
   std::vector<std::string> labels{};
   if (label) {
      labels.push_back(std::to_string(label.value()));
   }

   // Begin SageTreeBuilder
   SgFortranContinueStmt* continueStmt{nullptr};
   builder.Enter(continueStmt);

   // Finish SageTreeBuilder
   builder.Leave(continueStmt, labels);
#endif
}

void Build(parser::FailImageStmt &x, const OptLabel &label)
{
  std::cout << "Rose::builder::Build(FailImageStmt)\n";
  ABORT_NO_IMPL;

#if 0
   std::vector<std::string> labels{};
   if (label) {
      labels.push_back(std::to_string(label.value()));
   }

   // Begin SageTreeBuilder
   SgProcessControlStatement* failStmt{nullptr};
   builder.Enter(failStmt, "fail_image", boost::none, boost::none);

   // Finish SageTreeBuilder
   builder.Leave(failStmt, labels);
#endif
}

void BuildImpl(parser::AllocateStmt &x)
{
  std::cout << "BuildImpl(AllocateStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::BackspaceStmt &x)
{
  std::cout << "BuildImpl(BackspaceStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::CallStmt &x)
{
  std::cout << "BuildImpl(CallStmt)\n";
  ABORT_NO_IMPL;

#if 0
   std::list<SgExpression*> arg_list;
   std::string name;

   if (x.chevrons) {
     std::cerr << "[WARN] Rose::builder::Build(CallStmt): chevrons found but not accounted for\n";
   }

   // Get argument list and build rose node from it
   Build(x.call, arg_list, name);  // Call
   SgExprListExp* param_list = SageBuilder::buildExprListExp_nfi(arg_list);

   // Begin SageTreeBuilder
   SgExprStatement* call_stmt{nullptr};
   builder.Enter(call_stmt, name, param_list, "" /* abort_phrase */);

   // Finish SageTreeBuilder
   builder.Leave(call_stmt);
#endif
}

void BuildImpl(parser::CloseStmt &x)
{
  std::cout << "BuildImpl(CloseStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::CycleStmt &x, const OptLabel &label)
{
  std::cout << "Rose::builder::Build(CycleStmt)\n";
  ABORT_NO_IMPL;

  // A Fortran CycleStmt is semantically similar to a C/C++ continue statement

#if 0
   std::vector<std::string> labels{};
   if (label) {
      labels.push_back(std::to_string(label.value()));
   }

   // Begin SageTreeBuilder
   SgContinueStmt* continueStmt{nullptr};
   builder.Enter(continueStmt);

   // Finish SageTreeBuilder
   builder.Leave(continueStmt, labels);
#endif
}

void Build(parser::DeallocateStmt &x)
{
  std::cout << "Rose::builder::Build(DeallocateStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::EndfileStmt &x)
{
  std::cout << "Rose::builder::Build(EndfileStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::EventPostStmt &x)
{
  std::cout << "Rose::builder::Build(EventPostStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::EventWaitStmt &x)
{
  std::cout << "Rose::builder::Build(EventWaitStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::ExitStmt &x)
{
  // std::optional<Name> v;
  std::cout << "Rose::builder::Build(ExitStmt)\n";
  ABORT_NO_IMPL;

#if 0
   // TODO: exit with a name
   SgProcessControlStatement* exit_stmt{nullptr};
   builder.Enter(exit_stmt, "exit", boost::none, boost::none);
   builder.Leave(exit_stmt);
#endif
}

void Build(parser::FlushStmt &x)
{
  std::cout << "Rose::builder::Build(FlustStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::FormTeamStmt &x)
{
  std::cout << "Rose::builder::Build(FormTeamStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::GotoStmt &x)
{
  std::cout << "Rose::builder::Build(GotoStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::IfStmt &x)
{
  //  std::tuple<ScalarLogicalExpr, UnlabeledStatement<ActionStmt>> t;
  std::cout << "Rose::builder::Build(IfStmt)\n";
  ABORT_NO_IMPL;

#if 0
   SgIfStmt* if_stmt{nullptr};
   SgExpression* conditional{nullptr};
   std::vector<Rose::builder::Token> comments{};
   const OptLabel label{};

   // Traverse conditional expr
   Build(std::get<0>(x.t), conditional);

   // Build true body and push scope
   SgBasicBlock* true_body = SageBuilder::buildBasicBlock_nfi();
   SageBuilder::pushScopeStack(true_body);

   // Traverse true body statement
   Build(std::get<1>(x.t).statement);

   // Pop true body scope
   SageBuilder::popScopeStack();

   // Enter SageTreeBuilder
   builder.Enter(if_stmt, conditional, true_body, nullptr/*false_body*/, comments);

   // Leave SageTreeBuilder
   builder.Leave(if_stmt);
#endif
}

void Build(parser::InquireStmt &x)
{
  std::cout << "Rose::builder::Build(InquireStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::LockStmt &x)
{
  std::cout << "Rose::builder::Build(LockStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(const parser::NullifyStmt &x)
{
  std::cout << "BuildImpl(NullifyStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(const parser::OpenStmt &x)
{
  std::cout << "BuildImpl(OpenStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::PointerAssignmentStmt &x)
{
  std::cout << "BuildImpl(PointerAssignmentStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::PrintStmt &x)
{
  std::cout << "BuildImpl(PrintStmt)\n";
  ABORT_NO_IMPL;

#if 0
   std::list<SgExpression*> output_item_list;

   SgExpression* format = nullptr;

   Build(std::get<0>(x.t), format);           // Format
   Build(std::get<1>(x.t), output_item_list); // std::list<OutputItem>

   SgPrintStatement* print_stmt = nullptr;

   builder.Enter(print_stmt, format, output_item_list);
   builder.Leave(print_stmt);
#endif
}

void Build(parser::DefaultCharExpr&x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(DefaultCharExpr)");
  ABORT_NO_IMPL;
}

void Build(parser::Label &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(Label)");
  ABORT_NO_IMPL;
}

void Build(parser::Star &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(Star)");
  ABORT_NO_TEST;

  expr = SageBuilderCpp17::buildAsteriskShapeExp_nfi();
}

void Build(parser::OutputItem &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(OutputItem)");
  ABORT_NO_IMPL;

   expr = nullptr;
}

void Build(parser::OutputImpliedDo &x)
{
  std::cout << "Rose::builder::Build(OutputImpliedDo)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::ReadStmt &x)
{
  std::cout << "BuildImpl(ReadStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::ReturnStmt &x)
{
  std::cout << "BuildImpl(ReturnStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::RewindStmt &x)
{
  std::cout << "BuildImpl(RewindStmt)\n";
  ABORT_NO_IMPL;
}

void BuildVisitor::Build(parser::StopStmt &x)
{
  // std::tuple<Kind, std::optional<StopCode>, std::optional<ScalarLogicalExpr>> t;
  std::cout << "Rose::builder::Build(StopStmt)\n";
  ABORT_NO_IMPL;
  
#if 0
  SgProcessControlStatement* stmt{nullptr};
  boost::optional<SgExpression*> code{boost::none}, quiet{boost::none};
  std::string_view kind{parser::StopStmt::EnumToString(std::get<0>(x.t))};

  std::vector<std::string> labels;
#if DO_TODO
//TODO: labels
  if (label) {
    labels.push_back(std::to_string(*label));
  }
#endif

  // change strings to match builder function
  if (kind == "Stop") {
    kind = "stop";
  } else if (kind == "ErrorStop") {
    kind = "error_stop";
  }

  // stop code
  if (auto & opt = std::get<1>(x.t)) {
    SgExpression* expr{nullptr};
    WalkExpr(opt.value().v.thing, expr);
    code = expr;
  }

  // quiet
#if 1
  if (auto & opt = std::get<2>(x.t)) {
    ABORT_NO_IMPL;
    SgExpression* expr{nullptr};
    WalkExpr(opt.value(), expr);
    quiet = expr;
  }
#endif

  builder.Enter(stmt, std::string{kind}, code, quiet);
  builder.Leave(stmt, labels);
#endif
}

void BuildImpl(parser::SyncAllStmt &x)
{
  std::cout << "BuildImpl(SyncAllStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::SyncImagesStmt &x)
{
  std::cout << "BuildImpl(SyncImagesStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::SyncMemoryStmt &x)
{
  std::cout << "BuildImpl(SyncMemoryStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::SyncTeamStmt &x)
{
  std::cout << "BuildImpl(SyncTeamStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::UnlockStmt &x)
{
  std::cout << "BuildImpl(UnlockStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::WaitStmt &x)
{
  std::cout << "BuildImpl(WaitStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::WhereStmt &x)
{
  std::cout << "BuildImpl(WhereStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::WriteStmt &x)
{
  std::cout << "BuildImpl(WriteStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::ComputedGotoStmt &x)
{
  std::cout << "BuildImpl(ComputedGotoStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::ForallStmt &x)
{
  std::cout << "BuildImpl(ForallStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::ArithmeticIfStmt &x)
{
  std::cout << "BuildImpl(ArithmeticIfStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::AssignStmt &x)
{
  std::cout << "BuildImpl(AssignStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::AssignedGotoStmt &x)
{
  std::cout << "BuildImpl(AssignedGotoStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::PauseStmt &x)
{
  std::cout << "BuildImpl(PauseStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::NamelistStmt &x)
{
  std::cout << "BuildImpl(NamelistStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::ParameterStmt &x)
{
  std::cout << "BuildImpl(ParameterStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::OldParameterStmt &x)
{
  std::cout << "BuildImpl(OldParameterStmt)\n";
  ABORT_NO_IMPL;
}

void BuildImpl(parser::CommonStmt &x)
{
  // Flang::parser::CommonStmt -> SgCommonBlock in ROSE
  std::cout << "BuildImpl(CommonStmt)\n";
  ABORT_NO_IMPL;

#if 0
   std::list<SgCommonBlockObject*> common_block_object_list;
   Build(x.blocks, common_block_object_list);   // std::list<Block> blocks;

   SgCommonBlock* common_block = nullptr;
   builder.Enter(common_block, common_block_object_list);
   builder.Leave(common_block);
#endif
}

void Build(parser::CommonStmt::Block&x, SgCommonBlockObject* &common_block_object)
{
  // Flang::parser::CommonStmt::Block -> SgCommonBlockObject in ROSE
  std::cout << "Rose::builder::Build(CommonStmt::Block)\n";
  ABORT_NO_IMPL;

#if 0
   // Get name of CommonStmt::Block
   std::string name;
   if (auto & opt = std::get<0>(x.t)) {   // std::optional<Name>
      Build(opt.value(), name);
   }

   // Build std::list of variable references built from names in Flang::Parser::CommonBlockObject
   std::list<SgExpression*> var_ref_list;
   Build(std::get<1>(x.t), var_ref_list);

   // Build SgExprListExp from std::list of variable references
   SgExprListExp* sg_list = SageBuilder::buildExprListExp_nfi(var_ref_list);

   // Build ROSE SgCommonBlockObject from name of CommonStmt::Block and sg_list of variable references
   common_block_object = SageBuilder::buildCommonBlockObject(name, sg_list);
#endif
}

void Build(parser::CommonBlockObject&x, SgExpression* &var_ref)
{
  // Flang::parser::CommonBlockObject -> an SgExpression in the SgExprListExp member of SgCommonBlockObject in ROSE
  std::cout << "Rose::builder::Build(CommonBlockObject)\n";
  ABORT_NO_IMPL;

#if 0
   parser::Name name = std::get<parser::Name>(x.t);

   Build(name, var_ref);

   if (auto & opt = std::get<1>(x.t)) {   // std::optional<ArraySpec>
   }
#endif
}

// Expr
//
#if 0
template<typename T>
void Build(parser::CharLiteralConstantSubstring&x, T* &expr)
#else
void Build(parser::CharLiteralConstantSubstring&x, SgExpression* &expr)
#endif
{
  std::cout << "Rose::builder::Build(CharLiteralConstantSubstring)\n";
  ABORT_NO_IMPL;
}

void Build(parser::SubstringInquiry &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(SubstringInquiry)\n";
  ABORT_NO_IMPL;
}

void Build(parser::ArrayConstructor&x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(ArrayConstructor)\n";
  ABORT_NO_IMPL;

#if 0
   Build(x.v, expr);
#endif
}

void Build(parser::AcSpec&x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(AcSpec)\n";
  ABORT_NO_IMPL;

#if 0
   std::list<SgExpression*> acvalue_list;

   if (auto & opt = x.type)) {   // std::optional<TypeSpec>
   }

   Build(x.values, acvalue_list);   // std::list<AcValue> values;
   SgExprListExp* initializers = SageBuilder::buildExprListExp_nfi(acvalue_list);
   expr = SageBuilder::buildAggregateInitializer_nfi(initializers);
#endif
}

template<typename T>
void Build(parser::StructureConstructor&x, T* &expr)
{
  info(x, "Rose::builder::Build(StructureConstructor)");
  ABORT_NO_IMPL;
}

template<typename T>
void Build(parser::Expr::Parentheses &x, T* &expr)
{
  info(x, "Rose::builder::Build(Parentheses)");
  ABORT_NO_IMPL;

#if 0
   Build(x.v.value(), expr); // common::Indirection<Expr>
   SageBuilder::set_need_paren(expr);
#endif
}

template<typename T>
void Build(parser::Expr::UnaryPlus &x, T* &expr)
{
  info(x, "Rose::builder::Build(UnaryPlus)");
  ABORT_NO_IMPL;
}

template<typename T>
void Build(parser::Expr::Negate &x, T* &expr)
{
  info(x, "Rose::builder::Build(Negate)");
  ABORT_NO_IMPL;

#if 0
   SgExpression* val;
   Build(x.v.value(), val); // common::Indirection<Expr>
   expr = SageBuilder::buildMinusOp_nfi(val);
#endif
}

void BuildExprVisitor::Build(parser::Expr::NOT &x/*, SgExpression* &expr*/)
{
  info(x, "BuildExprVisitor::::Build(NOT)");
  ABORT_NO_IMPL;
}

void BuildExprVisitor::Build(parser::Expr::Power &x/*, SgExpression* &expr*/)
{
  std::cout << "Rose::builder::Build(Power)\n";
  ABORT_NO_IMPL;
}

void BuildExprVisitor::Build(parser::Expr::Multiply &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildMultiplyOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::Divide &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildDivideOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::Add &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildAddOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::Subtract &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildSubtractOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::Concat &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildConcatenationOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::LT &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildLessThanOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::LE &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildLessOrEqualOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::EQ &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildEqualityOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::NE &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildNotEqualOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::GE &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildGreaterOrEqualOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::GT &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildGreaterThanOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::AND &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildAndOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::OR &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildOrOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::EQV &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildEqualityOp_nfi(lhs, rhs));
}

void BuildExprVisitor::Build(parser::Expr::NEQV &x) {
  SgExpression *lhs{nullptr}, *rhs{nullptr};
  BuildExpressions(x, lhs, rhs);
  this->set(SageBuilder::buildNotEqualOp_nfi(lhs, rhs));
}

void Build(parser::Expr::DefinedBinary &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(DefinedBinary)\n";
  ABORT_NO_IMPL;
}

void Build(parser::Expr::ComplexConstructor &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(ComplexConstructor)\n";
  ABORT_NO_IMPL;
}

void Build(parser::StructureComponent &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(StructureComponent)\n";
  ABORT_NO_IMPL;
}

void Build(parser::ArrayElement &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(ArrayElement)\n";
  ABORT_NO_IMPL;

#if 0
  SgExpression* lhs{nullptr};
  SgExprListExp* rhs{nullptr};

  Build(x.base, lhs);        // DataRef
  //erasmus
  // Build(x.subscripts, rhs);  // std::list<SectionSubscript>
  ABORT_NO_IMPL;

  expr = SageBuilder::buildPntrArrRefExp_nfi(lhs, rhs);
#endif
}

void Build(parser::CoindexedNamedObject &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(CoindexedNamedObject)\n";
  ABORT_NO_IMPL;

#if 0
  Build(x.base, expr);          // DataRef
  Build(x.imageSelector, expr); // ImageSelector
#endif
}

void Build(parser::ImageSelector &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(ImageSelector)\n";

  SgExprListExp* cosubscripts{nullptr}, selectors{nullptr};
  ABORT_NO_IMPL;

#if 0
  Build(std::get<0>(x.t), cosubscripts);  // std::list<Cosubscript> - Cosubscript = Scalar<IntExpr>
  Build(std::get<1>(x.t), selectors);     // std::list<ImageSelectorSpec>
#endif
}

void Build(parser::ImageSelectorSpec&x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(ImageSelectorSpec)");
  ABORT_NO_IMPL;
}

void Build(parser::SectionSubscript &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(SectionSubscript)");
  ABORT_NO_IMPL;
}

void Build(parser::SubscriptTriplet &x, SgExpression* &expr)
{
  info(x, "Rose::builder::Build(SubscriptTriplet)");
  ABORT_NO_IMPL;
}

// ExecutableConstruct
//
void Build(parser::AssociateConstruct &x)
{
  info(x, "Rose::builder::Build(AssiciateConstruct)");
  ABORT_NO_IMPL;
}

void Build(parser::BlockConstruct &x)
{
  // std::tuple<Statement<BlockStmt>, BlockSpecificationPart, Block, Statement<EndBlockStmt>>
  info(x, "Rose::builder::Build(BlockConstruct)");
  ABORT_NO_IMPL;

#if 0
   Build(std::get<2>(x.t));   // Block
#endif
}

void Build(parser::CaseConstruct &x)
{
  info(x, "Rose::builder::Build(CaseConstruct)");
  ABORT_NO_IMPL;

#if 0
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
#endif
}

void Build(parser::CaseConstruct::Case &x, SgStatement* &stmt)
{
  // std::tuple<Statement<CaseStmt>, Block> t;
  info(x, "Rose::builder::Build(CaseConstruct)");
  ABORT_NO_IMPL;

#if 0
  SgStatement* block_stmt{nullptr};
  SgCaseOptionStmt* case_option_stmt{nullptr};
  SgDefaultOptionStmt* default_option_stmt{nullptr};
  SgExprListExp* sg_case_list{nullptr};
  std::list<SgExpression*> case_list;

  // Traverse CaseStmt
  Build(std::get<0>(x.t).statement, case_list);
  bool is_default = case_list.empty();

  // Begin SageTreeBuilder
  if (is_default) {
    builder.Enter(default_option_stmt);
  } else {
    sg_case_list = SageBuilder::buildExprListExp_nfi(case_list);
    builder.Enter(case_option_stmt, sg_case_list);
  }

  // Traverse Block
  //erasmus
  //Build(std::get<1>(x.t), block_stmt);
  ABORT_NO_IMPL;

  // End SageTreeBuilder
  if (is_default) {
    builder.Leave(default_option_stmt);
  } else {
    builder.Leave(case_option_stmt);
  }
#endif
}

void Build(parser::CaseStmt &x, std::list<SgExpression*> &case_list)
{
  //  std::tuple<CaseSelector, std::optional<Name>> t;
  info(x, "Rose::builder::Build(CaseStmt)");
  ABORT_NO_IMPL;

#if 0
   Build(std::get<0>(x.t), case_list);
#endif
}

void Build(parser::CaseValueRange::Range&x, SgExpression* &range)
{
  std::cout << "Rose::builder::Build(Range)\n";
  ABORT_NO_IMPL;

#if 0
   SgExpression * lower = nullptr, * upper = nullptr;
   auto & lower_expr = x.lower;
   auto & upper_expr = x.upper;

   if (lower_expr) {
      Build(lower_expr->thing, lower);   // CaseValue = Scalar<ConstantExpr>

   } else {
      lower = SageBuilder::buildNullExpression_nfi();
   }

   if (upper_expr) {
      Build(upper_expr->thing, upper);   // CaseValue = Scalar<ConstantExpr>
   } else {
      upper = SageBuilder::buildNullExpression_nfi();
   }

   SgExpression* stride = SageBuilder::buildIntVal_nfi(1);
   range = SageBuilder::buildSubscriptExpression_nfi(lower, upper, stride);
#endif
}

void Build(parser::ChangeTeamConstruct &x)
{
  info(x, "Rose::builder::Build(ChangeTeamConstruct)");
  ABORT_NO_IMPL;
}

void Build(parser::CriticalConstruct &x)
{
  info(x, "Rose::builder::Build(CriticalConstruct)");
  ABORT_NO_IMPL;
}

void Build(parser::LabelDoStmt &x)
{
  info(x, "Rose::builder::Build(CriticalConstruct)");
  ABORT_NO_IMPL;
}

void Build(parser::EndDoStmt &x)
{
  std::cout << "Rose::builder::Build(EndDoStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::DoConstruct &x)
{
  //  std::tuple<Statement<NonLabelDoStmt>, Block, Statement<EndDoStmt>> t;
  //  bool IsDoNormal() const;  bool IsDoWhile() const; bool IsDoConcurrent() const;
  std::cout << "Rose::builder::Build(DoConstruct)\n";
  ABORT_NO_IMPL;

#if 0
  SgStatement* blockStmt{nullptr};
  SgWhileStmt* whileStmt{nullptr};
  SgFortranDo* doStmt{nullptr};
  SgExpression* condition{nullptr}; // loop-control
  SgExpression* name{nullptr}; // do-construct-name

  // Traverse NonLabelDoStmt to get the loop condition
  Build(std::get<0>(x.t).statement, name, condition);

  // Enter SageTreeBuilder
  if (x.IsDoWhile()) {
    builder.Enter(whileStmt, condition);
  } else {
    // Simple form for now
    builder.Enter(doStmt);
  }

  // Traverse the body
  //erasmus
  //Build(std::get<1>(x.t), blockStmt);
  ABORT_NO_IMPL;

  // Leave SageTreeBuilder
  if (x.IsDoWhile()) {
    builder.Leave(whileStmt, true /* has_end_do_stmt */);
  } else {
    builder.Leave(doStmt);
  }
#endif
}

void Build(parser::IfConstruct &x)
{
  // std::tuple<>
  //   Statement<IfThenStmt>, Block, std::list<ElseIfBlock>, std::optional<ElseBlock>, Statement<EndIfStmt>
  std::cout << "Rose::builder::Build(IfConstruct)\n";
  ABORT_NO_IMPL;

#if 0
  std::vector<Rose::builder::Token> comments{};

  // Traverse IfThenStmt
  SgExpression* ifthen_expr{nullptr};
  Build(std::get<0>(x.t).statement, ifthen_expr);

  // True body
  SgBasicBlock* true_body = SageBuilder::buildBasicBlock_nfi();
  SageBuilder::pushScopeStack(true_body); // Push true body scope
  Build(std::get<1>(x.t));                     // Build Block
  SageBuilder::popScopeStack();           // Pop true body scope

  // Else ifs
  SgIfStmt* else_if_stmts{nullptr};
  SgBasicBlock* else_if_block{nullptr};
  // If there is a list of else if clauses, after the following call, else_if_block will be the block with an SgIfStmt
  // that represents the first else clause, this SgBasicBlock will be used as the false body for the main SgIfStmt
  // and else_if_stmts will be an SgIfStmt that represents the last else if clause. If there is an else clause,
  // this SgIfStmt will get its false_body set to be the else clause
  Build(std::get<2>(x.t), else_if_block, else_if_stmts);   // Traverse list of else if clauses

  // Optional Else
  SgBasicBlock* false_body{nullptr};
  if (auto & else_block = std::get<3>(x.t)) {
    Build(else_block.value(), false_body); // Traverse optional ElseBlock

    if (else_if_stmts) {
      // add false_body to the else if stmt
      SageBuilder::set_false_body(else_if_stmts, false_body);
    }
  }

  // EndIfStmt - std::optional<Name> v;
  bool have_end_stmt_name = false;
  if (auto & opt = std::get<parser::Statement<parser::EndIfStmt>>(x.t).statement.v) {
    have_end_stmt_name = true;
  }

  if (else_if_stmts) {
    // block with first else_if_stmt becomes false body for main SgIfStmt
    false_body = else_if_block;
  }

  // Enter SageTreeBuilder
  SgIfStmt* if_stmt{nullptr};
  builder.Enter(if_stmt, ifthen_expr, true_body, false_body, comments, true/* is_ifthen */, true/* has_end_stmt */);

  // Leave SageTreeBuilder
  builder.Leave(if_stmt);
#endif
}

void Build(parser::IfThenStmt&x, SgExpression* &expr)
{
 // std::tuple<std::optional<Name>, ScalarLogicalExpr> t;
  std::cout << "Rose::builder::Build(IfThenStmt)\n";
  ABORT_NO_IMPL;
  
#if 0
   Build(std::get<1>(x.t), expr); // ScalarLogicalExpr
#endif
}

void Build(parser::IfConstruct::ElseBlock&x, SgBasicBlock* &false_body)
{
  // std::tuple<Statement<ElseStmt>, Block> t;
  std::cout << "Rose::builder::Build(ElseBlock)\n";
  ABORT_NO_IMPL;
  
#if 0
   // Build false body and push scope
   false_body = SageBuilder::buildBasicBlock_nfi();
   SageBuilder::pushScopeStack(false_body);

   // Traverse Block
   Build(std::get<1>(x.t));

   // Pop false body scope
   SageBuilder::popScopeStack();
#endif
}

void Build(std::list<parser::IfConstruct::ElseIfBlock> &x, SgBasicBlock* &else_if_block, SgIfStmt* &else_if_stmt)
{
  std::cout << "Rose::builder::Build(std::list<ElseIfBlock>)\n";
  ABORT_NO_IMPL;
  
#if 0
   bool first_pass{true};
   std::vector<Rose::builder::Token> comments{};

   for (auto & else_if_clause : x) {
      SgBasicBlock* new_block = SageBuilder::buildBasicBlock_nfi();
      SageBuilder::pushScopeStack(new_block); // Push else if block

      SgExpression* conditional{nullptr};
      Build(std::get<0>(std::get<0>(else_if_clause.t).statement.t), conditional);

      // Build true body and push scope
      SgBasicBlock* true_body = SageBuilder::buildBasicBlock_nfi();
      SageBuilder::pushScopeStack(true_body);
      // Traverse true body statement
      SgScopeStatement* scope{nullptr};
      //erasmus
      //Build(std::get<1>(else_if_clause.t), scope);
      ABORT_NO_IMPL;
      // Pop true body scope
      SageBuilder::popScopeStack();

      // Enter SageTreeBuilder
      SgIfStmt* new_if_stmt{nullptr};
      builder.Enter(new_if_stmt, conditional, true_body, nullptr/*false_body*/,
                    comments, true/*is_ifthen*/, false/*has_end_stmt*/, true/*is_else_if*/);
      // Leave SageTreeBuilder
      builder.Leave(new_if_stmt);

      SageBuilder::popScopeStack();               // Pop else if block

      if (first_pass) {
         else_if_stmt = new_if_stmt;
         else_if_block = new_block;
         first_pass = false;
      } else {
         SageBuilder::set_false_body(else_if_stmt, new_block);
         else_if_stmt = new_if_stmt;
      }
   }
#endif
}

void Build(parser::SelectRankConstruct &x)
{
  std::cout << "Rose::builder::Build(SelectRankConstruct)\n";
  ABORT_NO_IMPL;
}

void Build(parser::SelectTypeConstruct &x)
{
  std::cout << "Rose::builder::Build(SelectTypeConstruct)\n";
  ABORT_NO_IMPL;
}

void Build(parser::WhereConstruct &x)
{
  std::cout << "Rose::builder::Build(WhereConstruct)\n";
  ABORT_NO_IMPL;
}

void Build(parser::ForallConstruct &x)
{
  std::cout << "Rose::builder::Build(ForallConstruct)\n";
  ABORT_NO_IMPL;
}

void Build(parser::OpenMPConstruct &x)
{
  std::cerr << "[WARN] Rose::builder::Build(OpenMPConstruct) unimplemented\n";
  ABORT_NO_IMPL;
}

void Build(parser::OpenACCConstruct &x)
{
  std::cerr << "[WARN] Rose::builder::Build(OpenACCConstruct) unimplemented\n";
  ABORT_NO_IMPL;
}

void Build(parser::AccEndCombinedDirective &x)
{
  std::cerr << "[WARN] Rose::builder::Build(AccEndCombinedDirective) unimplemented\n";
  ABORT_NO_IMPL;
}

// CUFKernelDoConstruct
void Build(parser::CUFKernelDoConstruct &x)
{
  std::cerr << "[WARN] Rose::builder::Build(CUFKernelDoConstruct) unimplemented\n";
  ABORT_NO_IMPL;
}

void Build(parser::OmpEndLoopDirective &x)
{
  std::cerr << "[WARN] Rose::builder::Build(Build(OmpEndLoopDirective) unimplemented\n";
  ABORT_NO_IMPL;
}

// DoConstructf3037
void Build(parser::NonLabelDoStmt&x, SgExpression* &name, SgExpression* &control)
{
  std::cout << "Rose::builder::Build(NonLabelDoStmt)\n";
  ABORT_NO_IMPL;

#if 0
  if (auto & opt = std::get<0>(x.t)) {   // std::optional<Name>
    Build(opt.value(), name);
  }

  if (auto & opt = std::get<1>(x.t)) {   // std::optional<LoopControlf3048
    Build(opt.value(), control);
  }
#endif
}

void Build(parser::LoopControl::Concurrent &x, SgExpression* &expr)
{
  std::cout << "Rose::builder::Build(LoopControl::Concurrent)\n";
  ABORT_NO_IMPL;

  // x.t (tuple)
  // [0] parser::ConcurrentHeader
  // [1] [LocalitySpec (a list)]
}

// SpecificationConstruct
void Build(parser::DerivedTypeDef &x)
{
  // std::tuple<Statement<DerivedTypeStmt>, std::list<Statement<TypeParamDefStmt>>,
  //     std::list<Statement<PrivateOrSequence>>, std::list<Statement<ComponentDefStmt>>,
  //     std::optional<TypeBoundProcedurePart>, Statement<EndTypeStmt>> t;
  std::cout << "Rose::builder::Build(DerivedTypeDef)\n";
  ABORT_NO_IMPL;

#if 0
   std::string name{};
   std::list<LanguageTranslation::ExpressionKind> modifiers;
   Build(std::get<parser::Statement<parser::DerivedTypeStmt>>(x.t).statement, name, modifiers);

   // Begin SageTreeBuilder for SgDerivedTypeStatement
   SgDerivedTypeStatement* derivedTypeStmt{nullptr};
   builder.Enter(derivedTypeStmt, name);

   // Traverse body of type-def
   std::list<SgStatement*> stmts{};
   Build(std::get<3>(x.t), stmts);

   // EndTypeStmt - std::optional<Name> v;
   bool haveEndStmt{false};
   if (auto &opt = std::get<parser::Statement<parser::EndTypeStmt>>(x.t).statement.v) {
      haveEndStmt = true;
   }

   // Leave SageTreeBuilder for SgDerivedTypeStatement
   builder.Leave(derivedTypeStmt, modifiers);
#endif
}

void Build(parser::DerivedTypeStmt&x, std::string &name, std::list<LanguageTranslation::ExpressionKind> &modifiers)
{
  // std::tuple<std::list<TypeAttrSpec>, Name, std::list<Name>> t;
  std::cout << "Rose::builder::Build(DerivedTypeStmt)\n";
  ABORT_NO_IMPL;

#if 0
   Build(std::get<0>(x.t), modifiers);  // std::list<TypeAttrSpec>
   name = std::get<1>(x.t).ToString();  // Name
#endif
}

void Build(parser::DataComponentDefStmt&x, SgStatement* &stmt)
{
  // std::tuple<> DeclarationTypeSpec, std::list<ComponentAttrSpec>, std::list<ComponentOrFill>
  std::cout << "Rose::builder::Build(DataComponentDefStmt)\n";
  ABORT_NO_IMPL;

#if 0
  using namespace LanguageTranslation;

  SgVariableDeclaration* varDecl{nullptr};
  SgType* type{nullptr};
  SgType* baseType{nullptr};
  SgExpression* init{nullptr};
  std::list<ExpressionKind> modifiers{};
  std::list<EntityDeclTuple> initInfo{};

  Build(std::get<0>(x.t), baseType);              // DeclarationTypeSpec
  Build(std::get<1>(x.t), modifiers);             // std::list<ComponentAttrSpec>
#if 1
  Build(std::get<2>(x.t), initInfo, baseType);    // std::list<ComponentOrFill>
#endif

  builder.Enter(varDecl, baseType, initInfo);
  builder.Leave(varDecl, modifiers);
#endif
}

void Build(Fortran::parser::ComponentDecl &x, std::list<EntityDeclTuple> &componentDecls, SgType* baseType)
{
  std::cout << "Rose::builder::Build(ComponentDecl&)\n";
  ABORT_NO_IMPL;

#if 0
   std::string name;
   SgType* type{nullptr};
   SgExpression* init{nullptr};

   Build(x, name, init, type, baseType);
   componentDecls.push_back(std::make_tuple(name, type, init));
#endif
}

void Build(parser::ComponentDecl &x, std::string &name, SgExpression* &init, SgType* &type, SgType* base_type)
{
  //  std::tuple<> Name, std::optional<ComponentArraySpec>, std::optional<CoarraySpec>, std::optional<CharLength>,
  //               std::optional<Initialization>
  std::cout << "Rose::builder::Build(ComponentDecl)\n";
  ABORT_NO_IMPL;

#if 0

  name = std::get<parser::Name>(x.t).ToString();

  if (auto & opt = std::get<1>(x.t)) {    // ComponentArraySpec
    Build(opt.value(), type, base_type);
  }

  if (auto & opt = std::get<2>(x.t)) {    // CoarraySpec
    Build(opt.value());
  }

  if (auto & opt = std::get<3>(x.t)) {    // CharLength
    //erasmus
    SgExpression* expr{nullptr};
    Build(opt.value(), expr);
    ABORT_NO_IMPL;
  }

  if (auto & opt = std::get<4>(x.t)) {    // Initialization
    Build(opt.value(), init);
  }
#endif
}

void Build(parser::EnumDef &x)
{
  std::cout << "Rose::builder::Build(EnumDef)\n";
  ABORT_NO_IMPL;
}

void Build(parser::InterfaceBlock &x)
{
  std::cout << "Rose::builder::Build(InterfaceBlock)\n";
  ABORT_NO_IMPL;
}

void Build(parser::StructureDef &x)
{
  std::cout << "Rose::builder::Build(StructureDef)\n";
  ABORT_NO_IMPL;
}

void Build(parser::GenericStmt &x)
{
  std::cout << "Rose::builder::Build(GenericStmt)\n";
  ABORT_NO_IMPL;
}

void Build(parser::ProcedureDeclarationStmt &x)
{
  std::cout << "Rose::builder::Build(ProcedureDeclarationStmt)\n";
  ABORT_NO_IMPL;
}

// OpenACCDeclarativeConstruct
void Build(parser::OpenMPDeclarativeConstruct &x)
{
  std::cout << "Rose::builder::Build(OpenMPDeclarativeConstruct)\n";
  ABORT_NO_IMPL;
}

// OpenACCDeclarativeConstruct
void Build(parser::OpenACCDeclarativeConstruct &x)
{
  std::cout << "Rose::builder::Build(OpenACCDeclarativeConstruct)\n";
  ABORT_NO_IMPL;
}

// AccessSpec
void getModifiers(parser::AccessSpec &x, LanguageTranslation::ExpressionKind &m) {
  using namespace LanguageTranslation;
  switch(x.v) {
    case parser::AccessSpec::Kind::Public:
      m = ExpressionKind::e_access_modifier_public;
      break;
    case parser::AccessSpec::Kind::Private:
      m = ExpressionKind::e_access_modifier_private;
      break;
  }
}

// IntentSpec
void getModifiers(parser::IntentSpec &x, LanguageTranslation::ExpressionKind &m) {
  using namespace LanguageTranslation;
  switch(x.v) {
    case parser::IntentSpec::Intent::In:
      m = ExpressionKind::e_type_modifier_intent_in;
      break;
    case parser::IntentSpec::Intent::Out:
      m = ExpressionKind::e_type_modifier_intent_out;
      break;
    case parser::IntentSpec::Intent::InOut:
      m = ExpressionKind::e_type_modifier_intent_inout;
      break;
  }
}

// LanguageBindingSpec
void getModifiers(parser::LanguageBindingSpec &x, LanguageTranslation::ExpressionKind &m) {
  std::cout << "[WARN] getModifiers(LanguageBindingSpec): MAYBE need build of ScalarDefaultCharConstantExpr\n";
  ABORT_NO_IMPL;
}

// TypeAttrSpec
void getModifiers(parser::TypeAttrSpec &x, LanguageTranslation::ExpressionKind &m) {
  // std::variant<> Abstract, AccessSpec, BindC, Extends
  using namespace Fortran::parser;
  using namespace LanguageTranslation;

  std::cout << "[WARN] getModifiers(TypeAttrSpec):\n";

  common::visit(common::visitors{
             [&] (Abstract &y) {
                   m = ExpressionKind::e_type_modifier_abstract;
                },
             [&] (AccessSpec &y) {
                   getModifiers(y, m);
                },
             [&] (TypeAttrSpec::BindC &y) {
                   m = ExpressionKind::e_type_modifier_bind_c;
                },
             [&] (TypeAttrSpec::Extends &y) {
                   ABORT_NO_IMPL;
                }
           },
     x.u);
}

void getAttrSpec(parser::AttrSpec &x, std::list<LanguageTranslation::ExpressionKind> &modifiers, SgType* &baseType) {
  // std::variant<> AccessSpec, Allocatable, Asynchronous, CoarraySpec, Contiguous,
  //                ArraySpec, External, IntentSpec, Intrinsic, LanguageBindingSpec, Optional,
  //                Parameter, Pointer, Protected, Save, Target, Value, Volatile,
  //                common::CUDADataAttr
  using namespace Fortran::parser;
  using namespace LanguageTranslation;

  common::visit(common::visitors{
                  [&](ArraySpec &y) {
                         /*DIMENSION*/
                         SgType* type{nullptr};
                         Build(y, type, baseType);
                         baseType = type;
                     },
                  [&](CoarraySpec &) { ABORT_NO_IMPL; /*CODIMENSION*/ },
                  [&](ComponentArraySpec &) { ABORT_NO_TEST; /*DIMENSION*/ },
                  [&](const IntentSpec &) { ABORT_NO_IMPL; /*INTENT*/ },
                  [&](const LanguageBindingSpec &) { ABORT_NO_IMPL; /*BINDING*/ },
                  [&](const common::CUDADataAttr &) { ABORT_NO_IMPL; /*CUDADataAttr*/ },
                  [&](const Allocatable &) {modifiers.push_back(ExpressionKind::e_type_modifier_allocatable);},
                  [&](const Asynchronous &) {modifiers.push_back(ExpressionKind::e_type_modifier_asynchronous);},
                  [&](const Contiguous &) {modifiers.push_back(ExpressionKind::e_storage_modifier_contiguous);},
                  [&](const External &) {modifiers.push_back(ExpressionKind::e_storage_modifier_external);},
                  [&](const Intrinsic &) {modifiers.push_back(ExpressionKind::e_type_modifier_intrinsic);},
                  [&](const Optional &) {modifiers.push_back(ExpressionKind::e_type_modifier_optional);},
                  [&](const Parameter &) {modifiers.push_back(ExpressionKind::e_type_modifier_parameter);},
                  [&](const Pointer &) {modifiers.push_back(ExpressionKind::e_type_modifier_pointer);},
                  [&](const Protected &) {modifiers.push_back(ExpressionKind::e_type_modifier_protected);},
                  [&](const Save &) {modifiers.push_back(ExpressionKind::e_type_modifier_save);},
                  [&](const Target &) {modifiers.push_back(ExpressionKind::e_type_modifier_target);},
                  [&](const Value &) {modifiers.push_back(ExpressionKind::e_param_binding_value);},
                  [&](const Volatile &) {modifiers.push_back(ExpressionKind::e_type_modifier_volatile);},
                  [&](const auto &y) {ABORT_NO_IMPL;}
                },
        x.u);
}

} // namespace Rose::builder
