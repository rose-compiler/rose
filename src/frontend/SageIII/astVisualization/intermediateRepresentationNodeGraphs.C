
#include "sage3basic.h"
#include "intermediateRepresentationNodeGraphs.h"

using namespace std;

void
intermediateRepresentationNodeGraph( const std::vector<VariantT> & nodeKindList )
   {
  // Build filename...
     string filename = "nodeGraph";
     for (size_t i = 0; i < nodeKindList.size(); i++)
        {
          filename += string("_") + string(Cxx_GrammarTerminalNames[nodeKindList[i]].name.c_str());
        }
     string dot_header = filename;
     filename += ".dot";

     printf ("In intermediateRepresentationNodeGraph(): filename = %s \n",filename.c_str());

  // Open file...
     ofstream file;
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
     file << "digraph \"" << dot_header << "\" {" << endl;

     IntermediateRepresentationNodeGraph t(file,nodeKindList);
     t.traverseMemoryPool();

  // Close off the DOT file.
     file << endl;
     file << "} " << endl;
     file.close();
   }

IntermediateRepresentationNodeGraph::IntermediateRepresentationNodeGraph(ofstream & inputFile, const std::vector<VariantT> & inputNodeKindList)
   : file(inputFile), 
     nodeKindList(inputNodeKindList)
   {
     for (size_t i = 0; i < nodeKindList.size(); i++)
        {
          printf ("Adding nodeKindList[%zu] = %d = %s to nodeKindSet \n",i,Cxx_GrammarTerminalNames[nodeKindList[i]].variant,Cxx_GrammarTerminalNames[nodeKindList[i]].name.c_str());
          nodeKindSet.insert(nodeKindList[i]);
        }
   }

void
IntermediateRepresentationNodeGraph::visit(SgNode* node)
   {
  // digraph "Gtest2013_322_WholeAST.dot" {
  // "0x2b6c15bf1010"[label="\n13SgSymbolTable\n  Unnamed Symbol Table\n  0x2b6c15bf1010  " shape=house,regular=0,URL="\N",tooltip="more info at \N",sides=5,peripheries=1,color="blue",fillcolor=orange,fontname="7x13bold",fontcolor=black,style=filled];
  // "0x2b6c15bf1010" -> "0x1535c1c0"[label="foo" dir=forward ];

  // "0x2b6c15cb3138" -> "0x152fc188"[label="storageModifier" dir=forward ];
  // "0x2b6c15cb3138" -> "0x2b6c15bb0010"[label="scope" color="green" decorate dir=forward ];
  // "0x2b6c15cb3138" -> "0x2b6c15c1a280"[label="parent" color="blue" decorate dir=forward ];
  // }

     if (nodeKindSet.find(node->variantT()) != nodeKindSet.end())
        {
#if 0
          printf ("##### IntermediateRepresentationNodeGraph::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif
       // file << node->class_name() << endl;
          file << "\"" << StringUtility::numberToString(node) << "\"[" << "label=\"" << node->class_name() << "\\n" << StringUtility::numberToString(node) << "\"];" << endl;
        }

#if 0
     printf ("##### IntermediateRepresentationNodeGraph::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif
     
   }
