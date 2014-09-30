
#include "sage3basic.h"
#include "intermediateRepresentationNodeGraphs.h"

using namespace std;

void
intermediateRepresentationNodeGraph( SgProject* project, const std::vector<VariantT> & nodeKindList )
   {
  // Build filename...
     string filename = "nodeGraph";
#if 0
     for (size_t i = 0; i < nodeKindList.size(); i++)
        {
          filename += string("_") + string(Cxx_GrammarTerminalNames[nodeKindList[i]].name.c_str());
        }
#endif
     string dot_header = filename;
     filename += ".dot";

     printf ("In intermediateRepresentationNodeGraph(): filename = %s \n",filename.c_str());

  // Open file...
     ofstream file;
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
     file << "digraph \"" << dot_header << "\" {" << endl;

     IntermediateRepresentationNodeGraph t(file,project,nodeKindList);
     t.traverseMemoryPool();

  // Close off the DOT file.
     file << endl;
     file << "} " << endl;
     file.close();
   }

IntermediateRepresentationNodeGraph::IntermediateRepresentationNodeGraph(ofstream & inputFile, SgProject* project, const std::vector<VariantT> & inputNodeKindList)
   : file(inputFile), 
     nodeKindList(inputNodeKindList)
   {
     for (size_t i = 0; i < nodeKindList.size(); i++)
        {
          printf ("Adding nodeKindList[%" PRIuPTR "] = %d = %s to nodeKindSet \n",i,Cxx_GrammarTerminalNames[nodeKindList[i]].variant,Cxx_GrammarTerminalNames[nodeKindList[i]].name.c_str());
          include_nodeKindSet.insert(nodeKindList[i]);
        }

  // Build a list of functions within the AST
  // Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);
  // Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgDeclarationStatement);
     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgStatement);

  // include_nodeSet.insert(functionDeclarationList.begin(),functionDeclarationList.end());

     int maxNumberOfNodes = 5000;
     int numberOfNodes = (int)functionDeclarationList.size();
     printf ("Number of IR nodes in IntermediateRepresentationNodeGraph = %d \n",numberOfNodes);

#if 1
     int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
          SgStatement*            statement           = isSgStatement(*i);
          SgDeclarationStatement* declaration         = isSgDeclarationStatement(*i);
          SgFunctionDeclaration*  functionDeclaration = isSgFunctionDeclaration(*i);

          SgTemplateInstantiationFunctionDecl*  templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(*i);

       // if (declaration != NULL && declaration->get_file_info()->isCompilerGenerated() == false)
          if (statement != NULL && statement->get_file_info()->isCompilerGenerated() == false)
             {
               include_nodeSet.insert(statement);
             }

          if (templateInstantiationFunctionDeclaration != NULL)
             {
               include_nodeSet.insert(statement);
             }
           
#if 0
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          if (functionDeclaration != NULL)
             {
            // DQ (3/5/2006): Only output the non-compiler generated IR nodes
               if ( (*i)->get_file_info()->isCompilerGenerated() == false)
                  {
                 // output the function number and the name of the function
                    printf ("Function #%2d name is %s at line %d \n",counter++,functionDeclaration->get_name().str(),functionDeclaration->get_file_info()->get_line());
                  }
                 else
                  {
                 // Output something about the compiler-generated builtin functions
                    printf ("Compiler-generated (builtin) function #%2d name is %s \n",counter++,functionDeclaration->get_name().str());
                  }
             }
            else
             {
               SgDeclarationStatement* declaration = isSgDeclarationStatement(*i);
               ROSE_ASSERT(declaration != NULL);
               printf ("--- declaration #%2d is: %p = %s \n",counter++,declaration,declaration->class_name().c_str());
             }
#endif
        }
#endif

     if (numberOfNodes <= maxNumberOfNodes)
        {
       // Ouput nodes.
          for (std::set<SgNode*>::iterator i = include_nodeSet.begin(); i != include_nodeSet.end(); i++)
             {
               SgNode* node = *i;
               file << "\"" << StringUtility::numberToString(node) << "\"[" << "label=\"" << node->class_name() << "\\n" << StringUtility::numberToString(node) << "\"];" << endl;
             }

       // Output edges
          for (std::set<SgNode*>::iterator i = include_nodeSet.begin(); i != include_nodeSet.end(); i++)
             {
               SgNode* node = *i;

               std::vector<std::pair<SgNode*,std::string> > listOfIRnodes = node->returnDataMemberPointers();
               std::vector<std::pair<SgNode*,std::string> >::iterator j = listOfIRnodes.begin();
               while (j != listOfIRnodes.end())
                  {
                    if (include_nodeSet.find(j->first) != include_nodeSet.end())
                       {
                         file << "\"" << StringUtility::numberToString(node) << "\" -> \"" << StringUtility::numberToString(j->first) << "\"[label=\"" << j->second << "\"];" << endl;
                       }

                    j++;
                  }
             }
        }
       else
        {
          printf ("WARNING: IntermediateRepresentationNodeGraph is too large to generate: numberOfNodes = %d (max size = %d) \n",numberOfNodes,maxNumberOfNodes);
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

#if 0
     if (nodeKindSet.find(node->variantT()) != nodeKindSet.end())
        {
#if 0
          printf ("##### IntermediateRepresentationNodeGraph::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif
       // file << node->class_name() << endl;
          file << "\"" << StringUtility::numberToString(node) << "\"[" << "label=\"" << node->class_name() << "\\n" << StringUtility::numberToString(node) << "\"];" << endl;
        }
#endif

#if 0
     printf ("##### IntermediateRepresentationNodeGraph::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif
     
   }
