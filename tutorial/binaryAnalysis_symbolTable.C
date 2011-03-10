// This simple example code shows how to retrieve symbol information 
// on Linux (ELF) or Windows (PE) binary executables.

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

class Visitor: public AstSimpleProcessing
   {
     public:
          void visit(SgNode* n);
   };

void
Visitor::visit(SgNode* n)
   {
     SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(n);
     if (symbol != NULL)
        {
          printf("Found a SgAsmGenericSymbol, Here: offset = %zu, %p = %s = %s \n",
                 (size_t)0, symbol, escapeString(symbol->class_name()).c_str(),
                 symbol->get_name()->get_string(true).c_str());
          printf ("symbol->get_name()      = %s \n",symbol->get_name()->get_string(true).c_str());
          printf ("symbol->get_bound()     = %p \n",symbol->get_bound());
          printf ("symbol->get_size()      = %"PRIu64" \n",symbol->get_size());
          printf ("symbol->get_value()     = %"PRIu64" \n",symbol->get_value());

          printf ("symbol->get_type()      = %u = %s \n",symbol->get_type(),symbol->stringifyType().c_str());
          printf ("symbol->get_binding()   = %u = %s \n",symbol->get_binding(),symbol->stringifyBinding().c_str());
          printf ("symbol->get_def_state() = %u = %s \n",symbol->get_def_state(),symbol->stringifyDefState().c_str());

          /* All the ELF-specific stuff and most of the COFF-specific stuff is also represented in the base class, displayed
           * above.  The general rule is that notions that are present in at least two file formats are represented in a
           * common base class and there's no need to access the copies that might be stored in the derived classes. However,
           * they are duplicated in the derived classes because those classes tend to represent data structures described
           * directly in the format specifications. The only reason they're publicly visible in the derived classes is because
           * ROSETTA doesn't support marking certain things as being for internal use only. I'm working on documenting these
           * things better.
           * 
           * For instance, the p_st_name member of an SgAsmElfSymbol is a byte offset into an ELF String Table. The actual
           * string is stored in the p_name member of SgAsmGenericSymbol. Modifying the string will cause all the correct
           * memory management actions to occur (ultimately adjusting p_st_name), while modifying p_st_name bypasses string
           * table management (and your new value would be overwritten if/when memory management does occur).
           * 
           * [RPM 2008-12-15] */

          /* The Coff-specific stuff may eventually disappear as the base class evolves to handle more of this. In fact, some
           * of these are already duplicates of the same info above. */
          SgAsmCoffSymbol* coffSymbol = isSgAsmCoffSymbol(symbol);
          if (coffSymbol != NULL)
             {
               printf ("   coffSymbol->get_st_name()            = %s \n",coffSymbol->get_st_name().c_str());
               printf ("   coffSymbol->get_st_name_offset()     = %"PRIu64" \n",coffSymbol->get_st_name_offset());
               printf ("   coffSymbol->get_st_section_num()     = %d  \n",coffSymbol->get_st_section_num());
               printf ("   coffSymbol->get_st_type()            = %u  \n",coffSymbol->get_st_type());
               printf ("   coffSymbol->get_st_storage_class()   = %u  \n",coffSymbol->get_st_storage_class());
               printf ("   coffSymbol->get_st_num_aux_entries() = %u \n",coffSymbol->get_st_num_aux_entries());
               printf ("   coffSymbol->get_aux_data():  size    = %zu \n",coffSymbol->get_aux_data().size());
               for (SgUnsignedCharList::iterator i = coffSymbol->get_aux_data().begin(); i != coffSymbol->get_aux_data().end(); i++)
                  {
                    printf ("      coffSymbol->get_aux_data() = %u \n",*i);
                  }
             }
        }
   }

int
main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);

  // Run available internal tests (not required)
     AstTests::runAllTests(project);

#if 1
  // Output a graph of the AST for the binary executable (file format and instructions)
     generateDOT ( *project );

  // Output a graph of the whole graph (all connections in the AST) for the binary executable (file format and instructions)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 2000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // Build the traversal object
     Visitor v;

  // Run the traversal to to analysis
     v.traverseInputFiles(project, postorder);

  // Unparse the output and return.
     return backend(project);
   }
