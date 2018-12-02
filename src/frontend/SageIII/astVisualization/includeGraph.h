#ifndef INCLUDE_FILE_GRAPH_H
#define INCLUDE_FILE_GRAPH_H

// #include "StringUtility.h"
// Support for building include graphs (graphs of include files)

namespace IncludeFileGraph
   {
  // File for output for generated graph.
     extern std::ofstream file;
     extern std::set<void*> graphNodeSet;

     void insertGraphNode (void* node);

     void graph_include_files( SgProject* project );

  // void graph_include_files( SgFileList* fileList );

     void graph_include_files( SgSourceFile* sourceFile );

     void graph_include_files( SgIncludeFile* includeFile );


   }

// INCLUDE_FILE_GRAPH_H
#endif
