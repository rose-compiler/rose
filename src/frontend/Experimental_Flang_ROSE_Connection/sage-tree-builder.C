#include "sage3basic.h"
#include "rose_config.h"

#include "sage-tree-builder.h"
#include <boost/optional/optional_io.hpp>
#include <iostream>

using std::cout;

namespace Rose {
namespace builder {

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

   node->set_endOfConstruct(new Sg_File_Info(end.path, end.line, end.column-1)); // ROSE endis inclusive
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


} // namespace builder
} // namespace Rose
