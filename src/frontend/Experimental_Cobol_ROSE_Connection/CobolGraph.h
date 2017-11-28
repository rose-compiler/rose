#ifndef COBOL_GRAPH_H
#define COBOL_GRAPH_H

#include <fstream>
#include <string>
#include "gnucobpt.h"

namespace CobolSupport {

class CobolGraph
{
 public:
   CobolGraph(std::string filename);
   virtual ~CobolGraph();

   void graph(const struct cb_program* prog, struct cobpt_base_list* local_cache, const void* parent);
   void graph(struct cb_field* field, const void* parent);
   void graph(struct cb_statement* stmt, const void* parent);

   std::ofstream & get_filestream()  { return pGraphFile; }

   std::string getGraphFilename()
      {
         return pGraphFilename;
      }

 public:
   std::string node_style;
   std::string edge_style;

 protected:
   std::ofstream pGraphFile;
   std::string pGraphFilename;

}; // class CobolGraph
}  // namespace CobolSupport

#endif
