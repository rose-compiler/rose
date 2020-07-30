#include "sage3basic.h"
#include "rose_config.h"

#include "sage-tree-builder.h"
#include "Jovial_to_ROSE_translation.h"
#include "ModuleBuilder.h"

#include <boost/optional/optional_io.hpp>
#include <iostream>

namespace Rose {
namespace builder {

using namespace Rose::Diagnostics;
using namespace LanguageTranslation;

SgGlobal* initialize_global_scope(SgSourceFile* file)
{
 // First we have to get the global scope initialized (and pushed onto the stack).

 // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
 // TODO      SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionCompilerGenerated);

    SgGlobal* globalScope = file->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);
    ROSE_ASSERT(globalScope->get_parent() != NULL);

 // Fortran is case insensitive
    std::cout << "--- is this needed global_scope is case sensitive " << globalScope->isCaseInsensitive() << std::endl;
    globalScope->setCaseInsensitive(true);
    std::cout << "--- is this needed global_scope is case sensitive " << globalScope->isCaseInsensitive() << std::endl;

 // DQ (8/21/2008): endOfConstruct is not set to be consistent with startOfConstruct.
    ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
    ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

 // DQ (10/10/2010): Set the start position of global scope to "1".
    globalScope->get_startOfConstruct()->set_line(1);

 // DQ (10/10/2010): Set this position to the same value so that if we increment
 // by "1" the start and end will not be the same value.
    globalScope->get_endOfConstruct()->set_line(1);

ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());//TEMPORARY
    ROSE_ASSERT(SageBuilder::emptyScopeStack() == true);
    SageBuilder::pushScopeStack(globalScope);

    return globalScope;
}

void
SageTreeBuilder::setSourcePosition(SgLocatedNode* node, const SourcePosition &start, const SourcePosition &end)
{
   ROSE_ASSERT(node != NULL);

// SageBuilder may have been used and it builds FileInfo
   if (node->get_startOfConstruct() != NULL) {
      delete node->get_startOfConstruct();
      node->set_startOfConstruct(NULL);
   }
   if (node->get_endOfConstruct() != NULL) {
      delete node->get_endOfConstruct();
      node->set_endOfConstruct(NULL);
   }

#if 0
   std::cout << "SageTreeBuilder::setSourcePosition: for node: "
        << node << " "
        << ":" << start.line << ":" << start.column
        << "-" <<   end.line << ":" <<   end.column << "\n";
#endif

   node->set_startOfConstruct(new Sg_File_Info(start.path, start.line, start.column));
   node->get_startOfConstruct()->set_parent(node);

   node->set_endOfConstruct(new Sg_File_Info(end.path, end.line, end.column-1)); // ROSE end is inclusive
   node->get_endOfConstruct()->set_parent(node);

   SageInterface::setSourcePosition(node);
}

void SageTreeBuilder::Leave(SgScopeStatement* & scope)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgScopeStatement* &) \n";

   scope = SageBuilder::getGlobalScopeFromScopeStack();
   ROSE_ASSERT(scope);
}

void SageTreeBuilder::Enter(SgBasicBlock* &block)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgBasicBlock* &) \n";

   // Set the parent (at least temporarily) so that symbols can be traced.
   block = SageBuilder::buildBasicBlock_nfi(SageBuilder::topScopeStack());

   SageBuilder::pushScopeStack(block);
}

void SageTreeBuilder::Leave(SgBasicBlock* block)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgBasicBlock*) \n";

   SageBuilder::popScopeStack();  // this basic block
   SageInterface::appendStatement(block, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgProgramHeaderStatement* &program_decl,
      const boost::optional<std::string> &name, const std::list<std::string> &labels, const SourcePositions &sources)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgProgramHeaderStatement* &, ...) \n";

   SgScopeStatement* scope = SageBuilder::topScopeStack();

   ROSE_ASSERT(scope);
   ROSE_ASSERT(scope->variantT() == V_SgGlobal);

   SgName program_name(name.get_value_or(ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME));

   SgFunctionParameterList* param_list = SageBuilder::buildFunctionParameterList_nfi();
   SgFunctionType* function_type = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), param_list);

   program_decl = new SgProgramHeaderStatement(program_name, function_type, /*function_def*/nullptr);
   ROSE_ASSERT(program_decl != nullptr);

// A Fortran program has no non-defining declaration (assume same for other languages)
   program_decl->set_definingDeclaration(program_decl);

   program_decl->set_scope(scope);
   program_decl->set_parent(scope);
   param_list  ->set_parent(program_decl);

   SgBasicBlock* program_body = new SgBasicBlock();
   SgFunctionDefinition* program_def = new SgFunctionDefinition(program_decl, program_body);

   if (SageInterface::is_language_case_insensitive())
      {
         program_body->setCaseInsensitive(true);
         program_def ->setCaseInsensitive(true);
      }

   ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(program_def);
   ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(program_body);

   program_body->set_parent(program_def);
   program_def ->set_parent(program_decl);

   setSourcePosition(program_decl, sources.get<0>(), sources.get<2>());
   setSourcePosition(program_def,  sources.get<1>(), sources.get<2>());
   setSourcePosition(program_body, sources.get<1>(), sources.get<2>());
   SageInterface::setSourcePosition(program_decl->get_parameterList());

// set labels
   if (SageInterface::is_Fortran_language() && labels.size() == 1)
      {
         SageInterface::setFortranNumericLabel(program_decl, atoi(labels.front().c_str()),
                                               SgLabelSymbol::e_start_label_type, /*label_scope=*/ program_def);
      }

// If there is no program name then there is no ProgramStmt (this probably needs to be marked somehow?)
   if (!name)
      {
         std::cerr << "WARNING: no ProgramStmt in the Fortran MainProgram \n";
      }

   ROSE_ASSERT(program_body == SageBuilder::topScopeStack());
   ROSE_ASSERT(program_decl->get_firstNondefiningDeclaration() == NULL);
}

void SageTreeBuilder::Leave(SgProgramHeaderStatement* program_decl)
{
// On exit, this function will have checked that the program declaration is
// properly connected, cleaned up the scope stack, resolved symbols, and
// inserted the declaration into its scope.

   mlog[TRACE] << "SageTreeBuilder::Leave(SgProgramHeaderStatement*) \n";

   SageBuilder::popScopeStack();  // program body
   SageBuilder::popScopeStack();  // program definition

   SgScopeStatement* scope = SageBuilder::topScopeStack();

 // The program declaration must go into the global scope
   SgGlobal* global_scope = isSgGlobal(scope);
   ROSE_ASSERT(global_scope != NULL);

// A symbol using this name should not already exist
   SgName program_name = program_decl->get_name();
   ROSE_ASSERT(!global_scope->symbol_exists(program_name));

// Add a symbol to the symbol table in the global scope
   SgFunctionSymbol* symbol = new SgFunctionSymbol(program_decl);
   global_scope->insert_symbol(program_name, symbol);

   SageInterface::appendStatement(program_decl, global_scope);
}

// Fortran has an end statement which may have an optional name and label
void SageTreeBuilder::
setFortranEndProgramStmt(SgProgramHeaderStatement* program_decl,
                         const boost::optional<std::string> &name,
                         const boost::optional<std::string> &label)
{
   ROSE_ASSERT(program_decl != NULL);

   SgFunctionDefinition* program_def = program_decl->get_definition();
   ROSE_ASSERT(program_def);

   if (label)
      {
         SageInterface::setFortranNumericLabel(program_decl, atoi(label->c_str()),
                                               SgLabelSymbol::e_end_label_type, /*label_scope=*/ program_def);
      }

   if (name)
      {
         program_decl->set_named_in_end_statement(true);
      }
}

void SageTreeBuilder::
Enter(SgFunctionParameterList* &param_list, SgBasicBlock* &param_scope)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionParameterList*) \n";

   param_list = SageBuilder::buildFunctionParameterList_nfi();

// The newly created SgBasicBloc (stored in param_scope) will be used to temporarily store declarations
// needed to build the types of the initialized names in the parameter list. These declarations are
// transferred to the function definition scope during later processing.
//
   param_scope = new SgBasicBlock();
   ROSE_ASSERT(param_scope != nullptr);
   ROSE_ASSERT(param_scope->get_parent() == nullptr); // make sure this node is unattached in the AST
   SageInterface::setSourcePosition(param_scope);

   // if unattached then symbol lookups may fail
   param_scope->set_parent(SageBuilder::topScopeStack());

   if (SageInterface::is_language_case_insensitive()) {
      param_scope->setCaseInsensitive(true);
   }

   ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());//TEMPORARY
   SageBuilder::pushScopeStack(param_scope);
}

void SageTreeBuilder::
Leave(SgFunctionParameterList* param_list, SgBasicBlock* param_scope, const std::list<FormalParameter> &param_name_list)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionParameterList*) \n";

   ROSE_ASSERT(param_list);
   ROSE_ASSERT(param_scope);

// Sanity check
   ROSE_ASSERT(param_scope == SageBuilder::topScopeStack());

#if 0
   std::cout << "--- param_name_list size is " << param_name_list.size() << std::endl;
   std::cout << "--- param      list size is " << param_scope->get_statements().size() << std::endl;
#endif

// Populate the function parameter list from declarations in the parameter block
   BOOST_FOREACH(const FormalParameter &param, param_name_list)
      {
         SgVariableSymbol* symbol = SageInterface::lookupVariableSymbolInParentScopes(param.name, param_scope);

         if (symbol == nullptr) {
            std::cerr << "WARNING UNIMPLEMENTED: SageTreeBuilder::Enter(SgFunctionDeclaration*) - symbol lookup failed for name "
                      << param.name << "\n";
            ROSE_ASSERT(symbol);
         }

      // Create a new initialized name for the parameter list
         SgInitializedName* init_name = symbol->get_declaration();
         SgType* type = init_name->get_type();
         SgInitializedName* new_init_name = SageBuilder::buildInitializedName (param.name, type, /*initializer*/nullptr);
         param_list->append_arg(new_init_name);

         if (param.output)
            {
               init_name    ->get_storageModifier().setMutable();
               new_init_name->get_storageModifier().setMutable();
            }
#if 0
         std::cout << "--> FormalParameter is " << param.name << std::endl;
         std::cout << "--> symbol is " << symbol << ": " << symbol->class_name() << std::endl;
         std::cout << "--> declared variable name is " << new_init_name->get_name() << std::endl;
         std::cout << "--> is output variable is " << param.output << std::endl;
#endif
      }

   SageBuilder::popScopeStack(); // remove parameter scope from the stack
}

void SageTreeBuilder::
Enter(SgFunctionDefinition* &function_def)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionDefinition*) \n";

   SgBasicBlock* block = SageBuilder::buildBasicBlock_nfi();

   function_def = new SgFunctionDefinition(block);
   ROSE_ASSERT(function_def != nullptr);
   SageInterface::setSourcePosition(function_def);

   ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(function_def);
}

void SageTreeBuilder::
Leave(SgFunctionDefinition* function_def)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionDefinition*) \n";
// don't pop the scope stack here as the function declaration will need it on enter
}

void SageTreeBuilder::
Enter(SgFunctionDeclaration* &function_decl, const std::string &name, SgType* return_type, SgFunctionParameterList* param_list,
                                             const LanguageTranslation::FunctionModifierList &modifiers)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionDeclaration* &, ...) \n";

   SgFunctionDefinition* function_def = nullptr;
   SgBasicBlock* function_body = nullptr;
   SgProcedureHeaderStatement::subprogram_kind_enum subprogram_kind;

   function_decl = nullptr;

   SgScopeStatement* scope = SageBuilder::topScopeStack();
   ROSE_ASSERT(scope);

   if (return_type == nullptr)
      {
         return_type = SageBuilder::buildVoidType();
         subprogram_kind = SgProcedureHeaderStatement::e_subroutine_subprogram_kind;
      }
   else
      {
         subprogram_kind = SgProcedureHeaderStatement::e_function_subprogram_kind;
      }

#if 0
   std::cout << "\nHACK ATTACK............ building function declaration for " << name << std::endl;
   std::cout << "--- parameter list size is " << param_list->get_args().size() << std::endl;
   std::cout << "--- SubroutineAttribute is " << attr << std::endl;
#endif

   SgName sg_name(name);

   SgProcedureHeaderStatement*
   proc_header = SageBuilder::buildProcedureHeaderStatement(sg_name, return_type, param_list, subprogram_kind, scope);

   function_decl = proc_header;
   ROSE_ASSERT(function_decl);

   function_def = function_decl->get_definition();
   function_body = function_def->get_body();

   ROSE_ASSERT(function_def);
   ROSE_ASSERT(function_body);

   if (list_contains(modifiers, e_function_modifier_definition))  function_decl->get_declarationModifier().setJovialDef();
   if (list_contains(modifiers, e_function_modifier_reference ))  function_decl->get_declarationModifier().setJovialRef();

   if (list_contains(modifiers, e_function_modifier_recursive))   function_decl->get_functionModifier().setRecursive();
   if (list_contains(modifiers, e_function_modifier_reentrant))   function_decl->get_functionModifier().setReentrant();

#if 0
   std::cout << "---   : function_decl "  << function_decl << ": " << function_decl->class_name() << std::endl;
   std::cout << "---   : function_def  "  << function_def << std::endl;
   std::cout << "---   : function_def  "  << function_decl->get_definition() << std::endl;
   std::cout << "---   : function_body " << function_body << std::endl;
   ROSE_ASSERT(function_body->isCaseInsensitive());
   ROSE_ASSERT(function_def ->isCaseInsensitive());
#endif
#if 0
   setSourcePosition(function_decl, sources.get<0>(), sources.get<2>());
   setSourcePosition(function_def,  sources.get<1>(), sources.get<2>());
   setSourcePosition(function_body, sources.get<1>(), sources.get<2>());
   SageInterface::setSourcePosition(function_decl->get_parameterList());
#endif

   SageBuilder::pushScopeStack(function_def);
   SageBuilder::pushScopeStack(function_body);
}

void SageTreeBuilder::
Leave(SgFunctionDeclaration* function_decl, SgBasicBlock* param_scope)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionDeclaration*) \n";

   SgBasicBlock* function_body = isSgBasicBlock(SageBuilder::topScopeStack());
   ROSE_ASSERT(function_body);

// Move all of the statements temporarily stored in the parameter scope
// into the scope for the function body.
//
   if (param_scope) {
      SageInterface::moveStatementsBetweenBlocks (param_scope, function_body);
   }
// The param_scope (SgBasicBlock) is still connected, so try to set its parent
// to nullptr and delete it.
   param_scope->set_parent(nullptr);
   delete param_scope;

   SageBuilder::popScopeStack();  // function body
   SageBuilder::popScopeStack();  // function definition

   SageInterface::appendStatement(function_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgDerivedTypeStatement* & derived_type_stmt, const std::string & name)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgDerivedTypeStatement* &, ...) \n";

   derived_type_stmt = SageBuilder::buildDerivedTypeStatement(name, SageBuilder::topScopeStack());

   SgClassDefinition* class_defn = derived_type_stmt->get_definition();
   ROSE_ASSERT(class_defn);
   ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(class_defn);
}

void SageTreeBuilder::
Leave(SgDerivedTypeStatement* derived_type_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgDerivedTypeStatement*) \n";
   ROSE_ASSERT(derived_type_stmt != nullptr);

   SageBuilder::popScopeStack();  // class definition
   SageInterface::appendStatement(derived_type_stmt, SageBuilder::topScopeStack());
}

// Statements
//

void SageTreeBuilder::
Enter(SgNamespaceDeclarationStatement* &namespace_decl, const std::string &name, const SourcePositionPair &positions)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgNamespaceDeclarationStatement* &, ...) \n";

   namespace_decl = SageBuilder::buildNamespaceDeclaration_nfi(name, true, SageBuilder::topScopeStack());
   SageInterface::setSourcePosition(namespace_decl);

   SgNamespaceDefinitionStatement* namespace_defn = namespace_decl->get_definition();
   ROSE_ASSERT(namespace_defn);
   ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());

// TEMPORARY: fix in SageBuilder
   namespace_defn->setCaseInsensitive(true);
   ROSE_ASSERT(namespace_defn->isCaseInsensitive());

   SageInterface::appendStatement(namespace_decl, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(namespace_defn);
}

void SageTreeBuilder::
Leave(SgNamespaceDeclarationStatement* namespace_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgNamespaceDeclarationStatement*, ...) \n";

   SageBuilder::popScopeStack();  // namespace definition
}

void SageTreeBuilder::
Enter(SgExprStatement* &proc_call_stmt, const std::string &proc_name,
      SgExprListExp* param_list, const std::string &abort_phrase)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgExprStatement* &, ...) \n";

   SgFunctionCallExp* proc_call_exp;

   Enter(proc_call_exp, proc_name, param_list);

   // TODO: AbortPhrase for Jovial
   proc_call_stmt = SageBuilder::buildExprStatement_nfi(proc_call_exp);
}

void SageTreeBuilder::
Enter(SgExprStatement* &assign_stmt, SgExpression* &rhs, const std::vector<SgExpression*> &vars, const std::string& label)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgExprStatement* &, ...) \n";

   SgEnumVal* old_val = isSgEnumVal(rhs);

   if (old_val && old_val->get_value() == -1) {
      // I don't think this is true anymore as the SgEnumVal is found by symbol lookup
      // Need to test for assignment to a status
      ROSE_ASSERT(false);

      SgEnumType* enum_type = isSgEnumType(vars[0]->get_type());
      ROSE_ASSERT(enum_type);
      SgEnumVal* enum_val = ReplaceEnumVal(enum_type, old_val->get_name());

      rhs = enum_val;
      delete old_val;
   }

   SgAssignOp* assign_op = SageBuilder::buildBinaryExpression_nfi<SgAssignOp>(vars[0], rhs);
   assign_stmt = SageBuilder::buildExprStatement_nfi(assign_op);
}

void SageTreeBuilder::
Leave(SgExprStatement* expr_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgExprStatement*) \n";
   ROSE_ASSERT(expr_stmt != nullptr);

   SageInterface::appendStatement(expr_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgFunctionCallExp* &func_call, const std::string &name, SgExprListExp* params)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionCallExp* &, ...) \n";

   SgFunctionSymbol* func_symbol = SageInterface::lookupFunctionSymbolInParentScopes(name, SageBuilder::topScopeStack());
   if (func_symbol == nullptr) {
    // Function calls are ambiguous with arrays in Fortran and type casts (at least) in Jovial
       func_call = nullptr;
       return;
   }

   func_call = SageBuilder::buildFunctionCallExp(func_symbol, params);
   SageInterface::setSourcePosition(func_call);
}

void SageTreeBuilder::
Enter(SgIfStmt* &if_stmt, SgExpression* conditional, SgBasicBlock* true_body, SgBasicBlock* false_body)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgIfStmt* &, ...) \n";

   ROSE_ASSERT(conditional);
   ROSE_ASSERT(true_body);

   SgStatement* conditional_stmt = SageBuilder::buildExprStatement_nfi(conditional);
   if_stmt = SageBuilder::buildIfStmt_nfi(conditional_stmt, true_body, false_body);
}

void SageTreeBuilder::
Leave(SgIfStmt* if_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgIfStmt*) \n";

   ROSE_ASSERT(if_stmt);
   SageInterface::appendStatement(if_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgProcessControlStatement* &control_stmt, const std::string &stmt_kind,
      const boost::optional<SgExpression*> &opt_code)
{
   return Enter(control_stmt, stmt_kind, opt_code, boost::none);
}

void SageTreeBuilder::
Enter(SgProcessControlStatement* &control_stmt, const std::string &stmt_kind,
      const boost::optional<SgExpression*> &opt_code, const boost::optional<SgExpression*> &opt_quiet)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgProcessControlStatement* &, ...) \n";

   SgExpression* code = nullptr;
   SgExpression* quiet = nullptr;

   if (opt_code) {
      code = *opt_code;
   }
   else {
      code = SageBuilder::buildNullExpression_nfi();
   }

   if (opt_quiet) {
      quiet = *opt_quiet;
   }
   else {
      quiet = SageBuilder::buildNullExpression_nfi();
   }

   ROSE_ASSERT(code);
   control_stmt = new SgProcessControlStatement(code);
   ROSE_ASSERT(control_stmt);
   SageInterface::setSourcePosition(control_stmt);

   ROSE_ASSERT(quiet);
   control_stmt->set_quiet(quiet);

   if (stmt_kind == "abort") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_abort);
   }
   else if (stmt_kind == "error_stop") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_error_stop);
   }
   else if (stmt_kind == "exit") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_exit);
   }
   else if (stmt_kind == "pause") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_pause);
   }
   else if (stmt_kind == "stop") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_stop);
   }
   ROSE_ASSERT(control_stmt->get_control_kind() != SgProcessControlStatement::e_unknown);

   code->set_parent(control_stmt);

   SageInterface::appendStatement(control_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgProcessControlStatement* control_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgProcessControlStatement*, ...) \n";

   ROSE_ASSERT(control_stmt);
}

void SageTreeBuilder::
Enter(SgSwitchStatement* &switch_stmt, SgExpression* selector, const SourcePositionPair &sources)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgSwitchStatement* &, ...) \n";

   ROSE_ASSERT(selector);
   SgExprStatement* selector_stmt = SageBuilder::buildExprStatement_nfi(selector);
   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();

   switch_stmt = SageBuilder::buildSwitchStatement_nfi(selector_stmt, body);

   SageInterface::appendStatement(switch_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgSwitchStatement* switch_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgSwitchStatement*, ...) \n";
   ROSE_ASSERT(switch_stmt);

   SageBuilder::popScopeStack();  // switch statement body
}

void SageTreeBuilder::
Enter(SgCaseOptionStmt* &case_option_stmt, SgExprListExp* key)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgCaseOptionStmt* &, ...) \n";
   ROSE_ASSERT(key);

   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();
   case_option_stmt = SageBuilder::buildCaseOptionStmt_nfi(key, body);

   SageInterface::appendStatement(case_option_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgCaseOptionStmt* case_option_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgCaseOptionStmt*, ...) \n";
   ROSE_ASSERT(case_option_stmt);

   SageBuilder::popScopeStack();  // case_option_stmt body
}

void SageTreeBuilder::
Enter(SgDefaultOptionStmt* &default_option_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgDefautlOptionStmt* &, ...) \n";

   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();
   default_option_stmt = SageBuilder::buildDefaultOptionStmt(body);

// Shouldn't we append later?  I'll try for while statement
   SageInterface::appendStatement(default_option_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgDefaultOptionStmt* default_option_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgDefautlOptionStmt*, ...) \n";
   ROSE_ASSERT(default_option_stmt);

   SageBuilder::popScopeStack();  // default_option_stmt body
}

void SageTreeBuilder::
Enter(SgWhileStmt* &while_stmt, SgExpression* condition)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgWhileStmt* &, ...) \n";
   ROSE_ASSERT(condition);

   SgExprStatement* condition_stmt = SageBuilder::buildExprStatement_nfi(condition);
   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();

   while_stmt = SageBuilder::buildWhileStmt_nfi(condition_stmt, body, /*else_body*/nullptr);

   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgWhileStmt* while_stmt, bool has_end_do_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgWhileStmt*, ...) \n";
   ROSE_ASSERT(while_stmt);

   // The default value of has_end_do_stmt is false so if true,
   // then the language supports it and it needs to be set.
   if (has_end_do_stmt) {
      while_stmt->set_has_end_statement(true);
   }

   SageBuilder::popScopeStack();  // while statement body
   SageInterface::appendStatement(while_stmt, SageBuilder::topScopeStack());
}

SgEnumVal* SageTreeBuilder::
ReplaceEnumVal(SgEnumType* enum_type, const std::string &name)
{
   SgEnumDeclaration* enum_decl = isSgEnumDeclaration(enum_type->get_declaration());
   ROSE_ASSERT(enum_decl);

   SgInitializedNamePtrList &enum_list = enum_decl->get_enumerators();
   SgInitializedName* init_name = nullptr;

   BOOST_FOREACH(SgInitializedName* status_constant, enum_list) {
      if (status_constant->get_name() == name) {
         init_name = status_constant;
      }
   }

   ROSE_ASSERT(init_name);
   SgEnumFieldSymbol* enum_symbol = isSgEnumFieldSymbol(init_name->get_symbol_from_symbol_table());
   ROSE_ASSERT(enum_symbol);

   SgEnumVal* enum_val = SageBuilder::buildEnumVal(enum_symbol);
   ROSE_ASSERT(enum_val);

   return enum_val;
}

// Jovial specific nodes
//

void SageTreeBuilder::
Enter(SgJovialDefineDeclaration* &define_decl, const std::string &define_string)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialDefineDeclaration* &, ...) \n";

   define_decl = new SgJovialDefineDeclaration(define_string);
   ROSE_ASSERT(define_decl != nullptr);
   SageInterface::setSourcePosition(define_decl);

// The first nondefining declaration must be set
   define_decl->set_firstNondefiningDeclaration(define_decl);
}

void SageTreeBuilder::
Leave(SgJovialDefineDeclaration* define_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialDirectiveStatement*) \n";

   ROSE_ASSERT(define_decl != nullptr);

   SageInterface::appendStatement(define_decl, SageBuilder::topScopeStack());
   ROSE_ASSERT(define_decl->get_parent() == SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialDirectiveStatement* &directive, const std::string &directive_string, bool is_compool)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialDirectiveStatement* &, ...) \n";

   directive = new SgJovialDirectiveStatement(directive_string, SgJovialDirectiveStatement::e_unknown);
   ROSE_ASSERT(directive);
   SageInterface::setSourcePosition(directive);

// The first nondefining declaration must be set
   directive->set_firstNondefiningDeclaration(directive);

   if (is_compool)
     {
        // Can't use SgJovialDirectiveStatement::e_compool enum as function parameter to SageTreeBuilder
        // because API can't see Sage nodes until C++17, so set it correctly as it is known here.
        directive->set_directive_type(SgJovialDirectiveStatement::e_compool);
        importModule(directive_string);
     }
}

void SageTreeBuilder::
Leave(SgJovialDirectiveStatement* directive)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialDirectiveStatement*) \n";

   ROSE_ASSERT(directive != nullptr);

   switch (directive->get_directive_type())
     {
       case SgJovialDirectiveStatement::e_compool:
       // A compool directive reads in the compool file and pushes its scope to the scope stack
          SageBuilder::popScopeStack();
          break;
       case SgJovialDirectiveStatement::e_unknown:
          mlog[ERROR] << "SageTreeBuilder::Leave(SgJovialDirectiveStatement*) directive_type is unknown \n";
          break;
       default:
          break;
     }

   SageInterface::appendStatement(directive, SageBuilder::topScopeStack());
   ROSE_ASSERT(directive->get_parent() == SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialForThenStatement* &for_stmt, SgExpression* init_expr, SgExpression* incr_expr,
         SgExpression* test_expr)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialForThenStatement* &, ...) \n";

   ROSE_ASSERT(init_expr);
   ROSE_ASSERT(incr_expr);
   ROSE_ASSERT(test_expr);

   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();

   for_stmt = new SgJovialForThenStatement(init_expr, incr_expr, test_expr, body);
   ROSE_ASSERT(for_stmt);
   SageInterface::setSourcePosition(for_stmt);

   if (SageInterface::is_language_case_insensitive()) {
      for_stmt->setCaseInsensitive(true);
   }

   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgJovialForThenStatement* for_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialForThenStatement*, ...) \n";

   SageBuilder::popScopeStack();  // for body
   SageInterface::appendStatement(for_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialCompoolStatement* &compool_decl, const std::string &name, const SourcePositionPair &positions)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialCompoolStatement* &, ...) \n";

   compool_decl = new SgJovialCompoolStatement(name);
   SageInterface::setSourcePosition(compool_decl);

   compool_decl->set_definingDeclaration(compool_decl);
   compool_decl->set_firstNondefiningDeclaration(compool_decl);

// TODO?
// SageBuilder::pushScopeStack(compool_defn);

   SageInterface::appendStatement(compool_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgJovialCompoolStatement* compool_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialCompoolStatement*, ...) \n";

// TODO?
// SageBuilder::popScopeStack();  // compool definition
}

void SageTreeBuilder::
Enter(SgJovialOverlayDeclaration* &overlay_decl, SgExpression* address, SgExprListExp* overlay)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialOverlayDeclaration* &, ...) \n";

   ROSE_ASSERT(address);
   ROSE_ASSERT(overlay);

   //TOOD: SageBuilder function
   overlay_decl = new SgJovialOverlayDeclaration(address, overlay);
   ROSE_ASSERT(overlay_decl);
   SageInterface::setSourcePosition(overlay_decl);

   address->set_parent(overlay_decl);
   overlay->set_parent(overlay_decl);
}

void SageTreeBuilder::
Leave(SgJovialOverlayDeclaration* overlay_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialOverlayDeclaration*) \n";

   SageInterface::appendStatement(overlay_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialTableStatement* &table_decl,
      const std::string &name, const SourcePositionPair &positions, bool is_block)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialTableStatement* &, ...) \n";

   SgName type_name = name;
   SgClassDeclaration::class_types struct_kind = SgClassDeclaration::e_jovial_table;
   if (is_block) struct_kind = SgClassDeclaration::e_jovial_block;

   // This function builds a class declaration and definition with both the defining and nondefining declarations as required
   table_decl = SageBuilder::buildJovialTableStatement(type_name, struct_kind, SageBuilder::topScopeStack());
   ROSE_ASSERT(table_decl);
   SageInterface::setSourcePosition(table_decl);

   SgClassDefinition* table_def = table_decl->get_definition();
   ROSE_ASSERT(table_def);

   if (SageInterface::is_language_case_insensitive()) {
      table_def->setCaseInsensitive(true);
   }

   SgType* sg_type = table_decl->get_type();
   SgJovialTableType* sg_table_type = isSgJovialTableType(sg_type);
   ROSE_ASSERT(sg_table_type);

#if 0
   cout << "--> TYPE " << name << " TABLE;\n";
   cout << "--> table_decl " << table_decl << "\n";
   cout << "--> table_def " << table_def << "\n";
   cout << "--> table type " << table_decl->get_type() << "\n";
   cout << "--> dim_info " << sg_table_type->get_dim_info() << "\n";
#endif

   ROSE_ASSERT(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(table_def);
}

void SageTreeBuilder::
Leave(SgJovialTableStatement* table_type_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialTableStatement*) \n";

   SageBuilder::popScopeStack();  // class definition
   SageInterface::appendStatement(table_type_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgVariableDeclaration* &var_decl, const std::string &name, SgType* type, SgExpression* init_expr)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgVariableDeclaration* &, ...) \n";

   ROSE_ASSERT(type != nullptr);

   SgName var_name = name;
   SgInitializer* var_init = nullptr;

   if (init_expr)
      {
         var_init = SageBuilder::buildAssignInitializer_nfi(init_expr, type);
      }

   var_decl = SageBuilder::buildVariableDeclaration_nfi(var_name, type, var_init, SageBuilder::topScopeStack());
   ROSE_ASSERT(var_decl != nullptr);

// Why isn't this done in SageBuilder?
   if (var_decl->get_definingDeclaration() == NULL)
      {
         var_decl->set_definingDeclaration(var_decl);
      }

   SgVariableDefinition* var_def = var_decl->get_definition();
   ROSE_ASSERT(var_def);

   SgInitializedName* init_name = var_decl->get_decl_item(var_name);
   ROSE_ASSERT(init_name);

   SgDeclarationStatement* decl_ptr = init_name->get_declptr();
   ROSE_ASSERT(decl_ptr);
   ROSE_ASSERT(decl_ptr == var_def);

   SgInitializedName* var_defn = var_def->get_vardefn();
   ROSE_ASSERT(var_defn);
   ROSE_ASSERT(var_defn == init_name);

   SageInterface::appendStatement(var_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgVariableDeclaration* var_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgVariableDeclaration*) \n";
}

void SageTreeBuilder::
Enter(SgEnumDeclaration* &enum_decl, const std::string &name)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgEnumDeclaration* &, ...) \n";

   enum_decl = SageBuilder::buildEnumDeclaration_nfi(name, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgEnumDeclaration* enum_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgEnumDeclaration*) \n";

   SageInterface::appendStatement(enum_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgEnumVal* &enum_val, const std::string &name, SgEnumDeclaration* enum_decl, int value)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgEnumVal*) \n";

   ROSE_ASSERT(enum_decl);
   SgEnumType* enum_type = enum_decl->get_type();

   enum_val = SageBuilder::buildEnumVal(value, enum_decl, name);
   SageInterface::setSourcePosition(enum_val);

   SgAssignInitializer* initializer = SageBuilder::buildAssignInitializer_nfi(enum_val, enum_type);
   SgInitializedName* init_name = SageBuilder::buildInitializedName_nfi(name, enum_type, initializer);
   SageInterface::setSourcePosition(init_name);

   enum_decl->append_enumerator(init_name);
   init_name->set_scope(enum_decl->get_scope());

   SgEnumFieldSymbol* enum_field_symbol = new SgEnumFieldSymbol(init_name);
   ROSE_ASSERT(enum_field_symbol);
   enum_decl->get_scope()->insert_symbol(name, enum_field_symbol);
}

void SageTreeBuilder::
Enter(SgTypedefDeclaration* &type_def, const std::string &name, SgType* type)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgTypedefDeclaration*) \n";

   type_def = SageBuilder::buildTypedefDeclaration(name, type, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgTypedefDeclaration* type_def)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgTypedefDeclaration*) \n";

   SageInterface::appendStatement(type_def, SageBuilder::topScopeStack());
}

// template <typename T>
void SageTreeBuilder::
importModule(const std::string &module_name)
{
   mlog[TRACE] << "SageTreeBuilder::importModule " << module_name << std::endl;

   ModuleBuilderFactory::get_compool_builder().getModule(module_name);
}

// Temporary wrappers for SageInterface functions (needed until ROSE builds with C++17)
//
namespace SageBuilderCpp17 {

SgType* buildBoolType()
{
   return SageBuilder::buildBoolType();
}

SgType* buildIntType()
{
   return SageBuilder::buildIntType();
}

// Operators
//
SgExpression* buildAddOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildAddOp_nfi(lhs, rhs);
}

SgExpression* buildAndOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildAndOp_nfi(lhs, rhs);
}

SgExpression* buildDivideOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildDivideOp_nfi(lhs, rhs);
}

SgExpression* buildEqualityOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildEqualityOp_nfi(lhs, rhs);
}

SgExpression* buildGreaterThanOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildGreaterThanOp_nfi(lhs, rhs);
}

SgExpression* buildGreaterOrEqualOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildGreaterOrEqualOp_nfi(lhs, rhs);
}

SgExpression* buildMultiplyOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildMultiplyOp_nfi(lhs, rhs);
}

SgExpression* buildLessThanOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildLessThanOp_nfi(lhs, rhs);
}

SgExpression* buildLessOrEqualOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildLessOrEqualOp_nfi(lhs, rhs);
}

SgExpression* buildNotEqualOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildNotEqualOp_nfi(lhs, rhs);
}

SgExpression* buildOrOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildOrOp_nfi(lhs, rhs);
}

SgExpression* buildSubtractOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildSubtractOp_nfi(lhs, rhs);
}

// Expressions
//
SgExpression* buildConcatenationOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildConcatenationOp_nfi(lhs, rhs);
}

SgExpression* buildExprListExp_nfi()
{
   return SageBuilder::buildExprListExp_nfi();
}

SgExpression* buildBoolValExp_nfi(bool value)
{
   return SageBuilder::buildBoolValExp_nfi(value);
}

SgExpression* buildIntVal_nfi(int value = 0)
{
   return SageBuilder::buildIntVal_nfi(value);
}

SgExpression* buildStringVal_nfi(std::string value)
{
   return SageBuilder::buildStringVal_nfi(value);
}

SgExpression* buildVarRefExp_nfi(std::string &name, SgScopeStatement* scope)
{
   SgVarRefExp* var_ref = SageBuilder::buildVarRefExp(name, scope);
   SageInterface::setSourcePosition(var_ref);

   return var_ref;
}

SgExpression* buildSubscriptExpression_nfi(SgExpression* lower_bound, SgExpression* upper_bound, SgExpression* stride)
{
   return SageBuilder::buildSubscriptExpression_nfi(lower_bound, upper_bound, stride);
}

SgExpression* buildNullExpression_nfi()
{
   return SageBuilder::buildNullExpression_nfi();
}

SgExprListExp* buildExprListExp_nfi(const std::list<SgExpression*> &list)
{
   SgExprListExp* expr_list = SageBuilder::buildExprListExp_nfi();

   BOOST_FOREACH(SgExpression* expr, list) {
      expr_list->get_expressions().push_back(expr);
   }
   return expr_list;
}

} // namespace SageBuilderCpp17

} // namespace builder
} // namespace Rose
