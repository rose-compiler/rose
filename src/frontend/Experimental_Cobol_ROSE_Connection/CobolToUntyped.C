#include "sage3basic.h"
#include "untypedBuilder.h"

#include <assert.h>
#include <iostream>
#include "CobolToUntyped.h"

using namespace CobolSupport;

FILE* rose_fd;

const struct cb_program* cobpt_program;
struct cobpt_base_list* cobpt_local_cache;

int cobpt_convert_cb_program(const struct cb_program* prog, struct cobpt_base_list* local_cache)
{
   int status = 0;

   cobpt_program = prog;
   cobpt_local_cache = local_cache;

   ROSE_ASSERT(cobpt_program != NULL);
   ROSE_ASSERT(cobpt_local_cache != NULL);

#if PRINT_PARSE_TREE
   status = rose_print_cb_program(cobpt_program, cobpt_local_cache);
#endif

   return status;
}

CobolToUntyped::CobolToUntyped(SgSourceFile* source)
{
   UntypedBuilder::set_language(SgFile::e_Cobol_language);

   ROSE_ASSERT(cobpt_program != NULL);
   ROSE_ASSERT(cobpt_local_cache != NULL);

   cb_program = cobpt_program;
   cb_local_cache = cobpt_local_cache;
}

CobolToUntyped::~CobolToUntyped()
{
}

void
CobolToUntyped::convert_CobolSourceProgram(const struct cb_program* cb_prog, struct cobpt_base_list* cb_local_cache)
{
   ROSE_ASSERT(cb_prog != NULL);
   ROSE_ASSERT(cb_local_cache != NULL);

   struct cobpt_base_list *field;
   cb_tree tree;

   SgUntypedGlobalScope* global_scope = get_scope();

   std::string name = cb_prog->orig_program_id;

   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedProgramHeaderDeclaration* main_program = NULL;
   SgUntypedDeclarationStatementList* global_decls = global_scope->get_declaration_list();

// program
   std::cout << "\"" << prog << "\"[label=\"cb_program\\n\\\"" << name << "\\\"\" ";
   std::cout << "];" << std::endl;

#if 0
// variable declarations
   for (field = local_cache; field; field = field->next)
      {
         convert_cb_field(field->f, prog);
      }

// statements
   for (tree = prog->exec_list; tree; tree = CB_CHAIN (tree))
      {
      // struct cb_statement *p;
         cb_tree tt = CB_VALUE (tree);

         switch (tt->tag) {
         case CB_TAG_LABEL: /* 18 */
         // rose_convert_cb_label(fd, CB_LABEL (tt));
            break;
         case CB_TAG_PERFORM: /* 25 */
         // rose_convert_cb_perform(fd, CB_PERFORM (tt));
            break;
         case CB_TAG_STATEMENT: /* 26 */
            graph(CB_STATEMENT (tt), prog);
            break;
         default:
            break;
         }
      }
#endif

#if 0
   if (traverse_DeclarationList(t_decls, global_decls)) {
      // MATCHED DeclarationList
   } else return ATfalse;

   if (traverse_ProgramBody(t_body, &function_scope)) {
      // MATCHED ProgramBody
      assert(function_scope != NULL);
   } else return ATfalse;
#endif

   std::cout << "CobolSourceProgram COBOL_SOURCE_PROGRAM:\n";

   std::string label = "";

   SgUntypedInitializedNameList* param_list  = new SgUntypedInitializedNameList();
   SgUntypedTokenList*           prefix_list = new SgUntypedTokenList();

   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);
   SgUntypedNamedStatement* end_program_stmt = new SgUntypedNamedStatement("",0,"");

// create the program
   main_program = new SgUntypedProgramHeaderDeclaration(label, name, param_list, type,
                                                        function_scope, prefix_list, end_program_stmt);

// This could probably be improved to as it includes decls and funcs in global scope
// setSourcePosition(main_program, term);
// No end statement so this will mimic Fortran usage
// setSourcePositionFromEndOnly(end_program_stmt, main_program);

// add program to the global scope
   global_scope->get_function_list()->get_func_list().push_back(main_program);

#if 0
   if (traverse_NonNestedSubroutineList(t_funcs, global_scope)) {
      // MATCHED NonNestedSubroutineList
   } else return ATfalse;
#endif
}

void
CobolToUntyped::convert_cb_field(struct cb_field* field)
{
// variable
   std::cout << "\"" << field << "\"[label=\"cb_field\\n\\\"" << field->name << "\\\"\" ";
   std::cout << "];" << std::endl;
}

void
CobolToUntyped::convert_cb_statement(struct cb_statement* stmt)
{
// statement
   std::cout << "\"" << stmt << "\"[label=\"cb_statement\\n\\\"" << stmt->name << "\\\"\" ";
   std::cout << "];" << std::endl;
}
