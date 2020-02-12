#include "sage3basic.h"
#include "rose_config.h"

#include "sage-tree-builder.h"
#include <boost/optional/optional_io.hpp>
#include <iostream>

using std::cout;

namespace Rose {
namespace builder {

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

// We should test if this is in the function type table, but do this later?
   SgFunctionType* type = new SgFunctionType(SgTypeVoid::createType(), false);

   program_decl = new SgProgramHeaderStatement(program_name, type, nullptr);

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

   cout << "--> TYPE " << name << " TABLE;\n";

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

   SgName var_name = name;
   SgInitializer* var_init = nullptr;

   if (init_expr)
      {
         var_init = new SgAssignInitializer(init_expr, init_expr->get_type());
         ROSE_ASSERT(var_init);
         SageInterface::setSourcePosition(var_init);
      }

   ROSE_ASSERT(type != nullptr);

   var_decl = SageBuilder::buildVariableDeclaration_nfi(var_name, type, var_init, SageBuilder::topScopeStack());
   ROSE_ASSERT(var_decl != nullptr);

   SageInterface::appendStatement(var_decl, SageBuilder::topScopeStack());
}

} // namespace builder
} // namespace Rose
