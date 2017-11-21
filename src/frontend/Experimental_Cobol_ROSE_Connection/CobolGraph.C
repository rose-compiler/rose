#include <assert.h>
#include <iostream>
#include "CobolGraph.h"

using namespace CobolSupport;

CobolGraph::CobolGraph(std::string filename)
{
   pGraphFilename = filename + ".dot";
   pGraphFile.open(pGraphFilename.c_str(), std::ofstream::out);

// label styles for nodes and edges
   node_style = "color=\"blue\",fillcolor=cyan4,fontname=\"7x13bold\",fontcolor=black,style=filled";
   edge_style = "color=\"blue\" labelfontcolor=\"blue4\" weight=1 len=2";

// opening digraph information
   pGraphFile << "digraph \"" << pGraphFilename << "\" {\n";
}

CobolGraph::~CobolGraph()
{
// closing digraph information
   pGraphFile << "}\n";

   pGraphFile.close();
}

void
CobolGraph::graph(const struct cb_program* prog, struct cobpt_base_list* local_cache, const void* parent)
{
   struct cobpt_base_list *field;
   cb_tree tree;

// program
   pGraphFile << "\"" << prog << "\"[label=\"cb_program\\n\\\"" << prog->orig_program_id << "\\\"\" ";
   pGraphFile << node_style << "];" << std::endl;
   if (parent)
      {
         pGraphFile << "\"" << parent << "\" -> \"" << prog << "\"";
         pGraphFile << "[" << edge_style << "];" << std::endl;
      }

// variable declarations
   for (field = local_cache; field; field = field->next)
      {
         graph(field->f, prog);
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
}

void
CobolGraph::graph(struct cb_field* field, const void* parent)
{
// variable
   pGraphFile << "\"" << field << "\"[label=\"cb_field\\n\\\"" << field->name << "\\\"\" ";
   pGraphFile << node_style << "];" << std::endl;
   if (parent)
      {
         pGraphFile << "\"" << parent << "\" -> \"" << field << "\"";
         pGraphFile << "[" << edge_style << "];" << std::endl;
      }
}

void
CobolGraph::graph(struct cb_statement* stmt, const void* parent)
{
// statement
   pGraphFile << "\"" << stmt << "\"[label=\"cb_statement\\n\\\"" << stmt->name << "\\\"\" ";
   pGraphFile << node_style << "];" << std::endl;
   if (parent)
      {
         pGraphFile << "\"" << parent << "\" -> \"" << stmt << "\"";
         pGraphFile << "[" << edge_style << "];" << std::endl;
      }
}
