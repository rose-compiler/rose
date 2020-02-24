#include "sage3basic.h"
#include "rose_config.h"

#include "sage-tree-builder.h"
#include <boost/optional/optional_io.hpp>
#include <iostream>

using std::cout;

namespace Rose {
namespace builder {

using namespace General_Language_Translation;

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
    globalScope->setCaseInsensitive(true);

 // DQ (8/21/2008): endOfConstruct is not set to be consistent with startOfConstruct.
    ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
    ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

 // DQ (10/10/2010): Set the start position of global scope to "1".
    globalScope->get_startOfConstruct()->set_line(1);

 // DQ (10/10/2010): Set this position to the same value so that if we increment
 // by "1" the start and end will not be the same value.
    globalScope->get_endOfConstruct()->set_line(1);

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

#if 1
   cout << "SageTreeBuilder::setSourcePosition: for node: "
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
   cout << "SageTreeBuilder::Enter(SgScopeStatement* &) \n";

   scope = SageBuilder::getGlobalScopeFromScopeStack();
   ROSE_ASSERT(scope);
}

void SageTreeBuilder::
Enter(SgProgramHeaderStatement* &program_decl,
      const boost::optional<std::string> &name, const std::list<std::string> &labels, const SourcePositions &sources)
{
   cout << "SageTreeBuilder::Enter(SgProgramHeaderStatement* &, ...) \n";

   SgScopeStatement* scope = SageBuilder::topScopeStack();

   ROSE_ASSERT(scope);
   ROSE_ASSERT(scope->variantT() == V_SgGlobal);

   SgName program_name(name.get_value_or(ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME));

   SgFunctionParameterList* param_list = SageBuilder::buildFunctionParameterList_nfi();
   SgFunctionType* function_type = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), param_list);

   program_decl = new SgProgramHeaderStatement(program_name, function_type, /*function_def*/nullptr);

// A Fortran program has no non-defining declaration (assume same for other languages)
   program_decl->set_definingDeclaration(program_decl);

   program_decl->set_scope(scope);
   program_decl->set_parent(scope);

   SgBasicBlock* program_body = new SgBasicBlock();
   SgFunctionDefinition* program_def = new SgFunctionDefinition(program_decl, program_body);

   if (SageInterface::is_Fortran_language() || SageInterface::is_Jovial_language())
      {
         program_body->setCaseInsensitive(true);
         program_def ->setCaseInsensitive(true);
      }

   SageBuilder::pushScopeStack(program_def);
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
         SageInterface::setFortranNumericLabel(program_decl, std::stoi(labels.front()),
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

   cout << "SageTreeBuilder::Leave(SgProgramHeaderStatement*) \n";

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
         SageInterface::setFortranNumericLabel(program_decl, std::stoi(*label),
                                               SgLabelSymbol::e_end_label_type, /*label_scope=*/ program_def);
      }

   if (name)
      {
         program_decl->set_named_in_end_statement(true);
      }
}

void SageTreeBuilder::
Enter(SgFunctionParameterScope* &function_def)
{
   cout << "SageTreeBuilder::Enter(SgFunctionParameterScope*) \n";

   function_def = new SgFunctionParameterScope();
   ROSE_ASSERT(function_def != nullptr);
   SageInterface::setSourcePosition(function_def);

   SageBuilder::pushScopeStack(function_def);
}

void SageTreeBuilder::
Leave(SgFunctionParameterScope* function_def)
{
   cout << "SageTreeBuilder::Leave(SgFunctionParameterScope*) \n";
// don't pop the scope stack here as the function declaration will need it on enter
}

void SageTreeBuilder::
Enter(SgFunctionDefinition* &function_def)
{
   cout << "SageTreeBuilder::Enter(SgFunctionDefinition*) \n";

   SgBasicBlock* block = SageBuilder::buildBasicBlock_nfi();

   function_def = new SgFunctionDefinition(block);
   ROSE_ASSERT(function_def != nullptr);
   SageInterface::setSourcePosition(function_def);

   SageBuilder::pushScopeStack(function_def);
}

void SageTreeBuilder::
Leave(SgFunctionDefinition* function_def)
{
   cout << "SageTreeBuilder::Leave(SgFunctionDefinition*) \n";
// don't pop the scope stack here as the function declaration will need it on enter
}

void SageTreeBuilder::
Enter(SgFunctionDeclaration* &function_decl, const std::string &name,
                                             const std::list<General_Language_Translation::FormalParameter> &param_list,
                                             const General_Language_Translation::SubroutineAttribute &attr)
{
   cout << "SageTreeBuilder::Enter(SgFunctionDeclaration* &, ...) \n";

// Lots to do!
//   1. Get formal parameter types (based on input FormalParameter list) from the declarations
//   2. Build initialized name list for the formal parameters
//   3. Build the function declaration (and probably the definition is required)
//   4. Transfer statements to the function definition body (actually perhaps could push a FunctionDefinition on the stack, then create declaration stuff)?

   SgScopeStatement* scope = SageBuilder::topScopeStack();
   SgFunctionDefinition* function_def = isSgFunctionDefinition(scope);
   SgFunctionParameterScope* param_scope = isSgFunctionParameterScope(scope);

// TODO: for now assume it is a function parameter scope (a nondefining declaraion only)
   ROSE_ASSERT(param_scope != nullptr);

   SageBuilder::popScopeStack();

   SgScopeStatement* global_scope = SageBuilder::topScopeStack();
   ROSE_ASSERT(global_scope);
   ROSE_ASSERT(global_scope->variantT() == V_SgGlobal);

// NEXT: create a parameter_list
//       -----------------------
//  1. iterate through FormalParameter list
//  2. search for param name in param_scope
//  3. create an initialized name for the formal parameter
//  4. insert it in the SgFunctionParameterList

   // Try buildNondefiningDeclaration
   //function_decl = SageBuilder::buildNondefiningFunctionDeclaration (name, SgType *return_type, SgFunctionParameterList *parameter_list, global_scope, SgExprListExp *decoratorList=NULL)

   SgType* return_type = SageBuilder::buildVoidType();
   SgFunctionParameterList* sg_param_list = SageBuilder::buildFunctionParameterList_nfi();

   function_decl = nullptr;

   std::cout << "\nHACK ATTACK............ building function declaration for " << name << std::endl;
   std::cout << ".x. FormalParameter list size is " << param_list.size() << std::endl;
   std::cout << ".x. SubroutineAttribute is " << attr << std::endl;

   BOOST_FOREACH(const General_Language_Translation::FormalParameter &param, param_list)
      {
         std::cout << ".x. FormalParameter " << param.name << std::endl;
      }

   function_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, return_type, sg_param_list, SageBuilder::topScopeStack());
   SageInterface::setSourcePosition(function_decl);

   return;
//------------------------------------------------------------

#if 0
   SgType* return_type = SageBuilder::buildVoidType();
   SgFunctionParameterList* param_list = SageBuilder::buildFunctionParameterList_nfi();


// TODO - for now
   bool isDefiningDeclaration = false;

   if (isDefiningDeclaration == false)
      {
         function_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, return_type, param_list, scope);
         cout << "\nHACK ATTACK................\n\n";
      // setSourcePosition(function_decl, source);
      }
   else
      {
         function_decl = SageBuilder::buildDefiningFunctionDeclaration(name, return_type, param_list, scope);
      // setSourcePosition(function_decl, source);

         function_body = SageBuilder::buildBasicBlock_nfi();
      // setSourcePosition(function_body, source);

         function_def = new SgFunctionDefinition(function_decl, function_body);
         ROSE_ASSERT(function_def);
      // setSourcePosition(function_def, source);

         function_def->set_parent(function_decl);
         function_def->set_body(function_body);
         function_body->set_parent(function_def);

         ROSE_ASSERT(function_decl->get_definition() != NULL);

         if (SageInterface::is_Fortran_language() || SageInterface::is_Jovial_language())
            {
               function_body->setCaseInsensitive(true);
               function_def ->setCaseInsensitive(true);
            }
      }
   ROSE_ASSERT(function_decl);

#if 1
         std::cout << "---          : function_decl "  << function_decl << ": " << function_decl->class_name() << std::endl;
      // std::cout << "---          : function_def  "  << function_decl->get_definition() << std::endl;
         std::cout << "---          : function_def  "  << function_def << std::endl;
         std::cout << "---          : function_body " << function_body << std::endl;
#endif

   if (isDefiningDeclaration == true)
      {
         ROSE_ASSERT(function_def);
         ROSE_ASSERT(function_body);
         SageBuilder::pushScopeStack(function_def);
         SageBuilder::pushScopeStack(function_body);
      }
   else
      {
         SgFunctionParameterScope* param_scope = function_decl->get_functionParameterScope();
         if (param_scope)
            std::cout << "---          : function param scope "  << param_scope << ": " << param_scope->class_name() << std::endl;
         else
            std::cout << "---          : function param scope "  << param_scope << std::endl;
         //SageBuilder::pushScopeStack(param_scope);

         if (param_scope == nullptr)
            {
               std::cout << "---          : DON'T KNOW WHY -> function param scope "  << param_scope << std::endl;
               param_scope = new SgFunctionParameterScope();
               ROSE_ASSERT(param_scope);
               SageInterface::setSourcePosition(param_scope);
               SageBuilder::pushScopeStack(param_scope);
            }

      }

#if 0
   setSourcePosition(function_decl, sources.get<0>(), sources.get<2>());
   setSourcePosition(function_def,  sources.get<1>(), sources.get<2>());
   setSourcePosition(function_body, sources.get<1>(), sources.get<2>());
   SageInterface::setSourcePosition(function_decl->get_parameterList());
#endif

#endif

   ROSE_ASSERT(function_decl);
}

void SageTreeBuilder::
Leave(SgFunctionDeclaration* function_decl)
{
   cout << "SageTreeBuilder::Leave(SgFunctionDeclaration*) \n";

   ROSE_ASSERT(false);

   std::cout << "---  : Will pop scope(s) (maybe)\n ";
   SageBuilder::popScopeStack();
   SageBuilder::popScopeStack();

   return;
//-------------------------------------------------------

// TODO - only if a defining declaration!
//   SageBuilder::popScopeStack();

   SageInterface::appendStatement(function_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgDerivedTypeStatement* & derived_type_stmt, const std::string & name)
{
   cout << "SageTreeBuilder::Enter(SgDerivedTypeStatement* &, ...) \n";

   derived_type_stmt = SageBuilder::buildDerivedTypeStatement(name, SageBuilder::topScopeStack());

   SgClassDefinition* class_defn = derived_type_stmt->get_definition();
   ROSE_ASSERT(class_defn);
   SageBuilder::pushScopeStack(class_defn);
}

void SageTreeBuilder::
Leave(SgDerivedTypeStatement* derived_type_stmt)
{
   cout << "SageTreeBuilder::Leave(SgDerivedTypeStatement*) \n";

   SageBuilder::popScopeStack();  // class definition
   SageInterface::appendStatement(derived_type_stmt, SageBuilder::topScopeStack());
}

// Jovial specific nodes
//

void SageTreeBuilder::
Enter(SgJovialCompoolStatement* &compool_decl, const std::string &name, const SourcePositionPair &positions)
{
   cout << "SageTreeBuilder::Enter(SgJovialCompoolStatement* &, ...) \n";

   compool_decl = new SgJovialCompoolStatement(name);
   SageInterface::setSourcePosition(compool_decl);

   compool_decl->set_definingDeclaration(compool_decl);
   compool_decl->set_firstNondefiningDeclaration(compool_decl);

   SageInterface::appendStatement(compool_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialTableStatement* &table_decl,
      const std::string &name, const SourcePositionPair &positions, bool is_block)
{
   cout << "SageTreeBuilder::Enter(SgJovialTableStatement* &, ...) \n";

   SgName type_name = name;
   SgClassDeclaration::class_types struct_kind = SgClassDeclaration::e_jovial_table;
   if (is_block) struct_kind = SgClassDeclaration::e_jovial_block;

   // This function builds a class declaration and definition with both the defining and nondefining declarations as required
   table_decl = SageBuilder::buildJovialTableStatement(type_name, struct_kind, SageBuilder::topScopeStack());
   ROSE_ASSERT(table_decl);
   SageInterface::setSourcePosition(table_decl);

   SgClassDefinition* table_def = table_decl->get_definition();
   ROSE_ASSERT(table_def);

   if (SageInterface::is_Fortran_language() || SageInterface::is_Jovial_language())
      {
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

   SageBuilder::pushScopeStack(table_def);
}

void SageTreeBuilder::
Leave(SgJovialTableStatement* table_type_stmt)
{
   cout << "SageTreeBuilder::Leave(SgJovialTableStatement*) \n";

   SageBuilder::popScopeStack();  // class definition
   SageInterface::appendStatement(table_type_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgVariableDeclaration* &var_decl, const std::string &name, SgType* type, SgExpression* init_expr)
{
   cout << "SageTreeBuilder::Enter(SgVariableDeclaration* &, ...) \n";

   ROSE_ASSERT(type != nullptr);

   SgName var_name = name;
   SgInitializer* var_init = nullptr;

   if (init_expr)
      {
         var_init = SageBuilder::buildAssignInitializer_nfi(init_expr, type);
         ROSE_ASSERT(var_init);
         SageInterface::setSourcePosition(var_init);
      }

   var_decl = SageBuilder::buildVariableDeclaration_nfi(var_name, type, var_init, SageBuilder::topScopeStack());
   ROSE_ASSERT(var_decl != nullptr);

   SageInterface::appendStatement(var_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgVariableDeclaration* var_decl)
{
   cout << "SageTreeBuilder::Leave(SgVariableDeclaration*) \n";

#if 0
// There is a problem when the variable declaration has an initializer.  The typeptr for the initialized name
// is SgModifierType with a base_type of the correct type.  I wonder if this is a problem with SageBuilder?
// The problem is seen in the unparser and in a dot file but not below when searched.
   SgInitializedName* init_name = SageInterface::getFirstInitializedName(var_decl);
   SgInitializer* var_init = init_name->get_initializer();
   SgName var_name = init_name->get_name();
   SgType* type = init_name->get_type();
   SgType* typeptr = init_name->get_typeptr();

   std::cout << "--> seeking SgModifierType: type     is " << type << ": " << type->class_name() << std::endl;
   std::cout << "--> seeking SgModifierType: typeptr  is " << typeptr << ": " << typeptr->class_name() << ": " << var_name << std::endl;
   if (var_init)
   std::cout << "--> seeking SgModifierType: var_init is " << var_init << ": " << var_init->class_name() << std::endl;

   if (var_decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst())
      {
         std::cout << "--> CONSTANT \n";
      }
   else   std::cout << "--> NOT CONSTANT \n";
#endif
}

void SageTreeBuilder::
Enter(SgEnumDeclaration* &enum_decl, const std::string &name, std::list<SgInitializedName*> &status_list)
{
   cout << "SageTreeBuilder::Enter(SgEnumDeclaration* &, ...) \n";

   enum_decl = SageBuilder::buildEnumDeclaration_nfi(name, SageBuilder::topScopeStack());
   ROSE_ASSERT(enum_decl);

   BOOST_FOREACH(SgInitializedName *status_constant, status_list) {
      enum_decl->append_enumerator(status_constant);
      status_constant->set_scope(enum_decl->get_scope());
   }
}

void SageTreeBuilder::
Leave(SgEnumDeclaration* enum_decl)
{
   cout << "SageTreeBuilder::Leave(SgEnumDeclaration*) \n";

   SageInterface::appendStatement(enum_decl, SageBuilder::topScopeStack());
}

} // namespace builder
} // namespace Rose
