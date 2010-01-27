// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"
#include "requiredNodes.h"
using namespace std;

#if 0
void
RequiredIRNodes::display()
   {
     set<SgNode*>::iterator i = requiredNodesList.begin();
     while (i != requiredNodesList.end())
        {
       // printf ("requiredNodesList: i = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
          Sg_File_Info* fileInfo = (*i)->get_file_info();
          printf ("requiredNodesList: node = %p = %s = %s at file = %s \n",
               *i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str(),(fileInfo != NULL) ? fileInfo->get_raw_filename().c_str() : "NULL");
          i++;
        }
   }
#endif


// forward function declaration
// set<SgNode*> buildRequiredNodeList ( SgNode* node );

void
RequiredIRNodes::visit(SgNode* node)
   {
  // Skip deleting any IR nodes in the original AST (e.g. SgInitializedName under a SgVariableDeclaration after the merge!)
  // printf ("In RequiredIRNodes::visit(): deleting %p = %s \n",node,node->class_name().c_str());
#if 0
     string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=black,fontname=\"7x13bold\",fontcolor=white,style=filled";
  // Make this statement different in the generated dot graph

  // DQ (5/14/2006): this is an error when processing stdio.h
  // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
     string labelWithSourceCode = "\\n" +  StringUtility::numberToString(*i) + "  ";

     NodeType graphNode(*i,labelWithSourceCode,additionalNodeOptions);
     addNode(graphNode);
#endif

  // Save the current IR node
     requiredNodesList.insert(node);

  // Traverse the child nodes and add them to the list to save
     typedef vector<pair<SgNode*,string> > DataMemberMapType;
     DataMemberMapType dataMemberMap = node->returnDataMemberPointers();

     DataMemberMapType::iterator i = dataMemberMap.begin();
     while (i != dataMemberMap.end())
        {
       // Ignore the parent pointer since it will be reset differently if required
          SgNode* childPointer = i->first;
          string  debugString  = i->second;

          if (childPointer != NULL)
             {
            // printf ("At node = %p = %s on edge %s found child %p = %s \n",node,node->class_name().c_str(),debugString.c_str(),childPointer,childPointer->class_name().c_str());
               requiredNodesList.insert(childPointer);
               addAssociatedNodes(childPointer,requiredNodesList,false);
             }
          i++;
        }

     addAssociatedNodes(node,requiredNodesList,false);

  // DQ (2/7/2007): There are three situations where AST islands can exist, we will handle these explicitly 
  // since they are required to be visited to correctly record the set of required IR nodes in the AST.
  // The the ROSE/proposal notes on AST islands for more information (this will be fixed in the traversal at some point).
     switch(node->variantT())
        {
       // A typedef can hide another declaration (a named type used as the typedef base type)
          case V_SgTypedefDeclaration:
             {
            // printf ("Found a SgTypedefDeclaration = %p looking for islands of untraversed AST ... \n",node);
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
               ROSE_ASSERT(typedefDeclaration != NULL);

               bool islandFound = typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration();
            // printf ("islandFound = %s \n",(islandFound == true) ? "true" : "false");
               if (islandFound == true)
                  {
                 // We only want to traverse the base type (since all "*" and "&" are associated
                 // with the variables in this variable list, e.g. list of SgInitializedName objects)
                    SgType* baseType = typedefDeclaration->get_base_type();
                    ROSE_ASSERT(baseType != NULL);

                    requiredNodesList.insert(baseType);
                    addAssociatedNodes(baseType,requiredNodesList,false);

                 // printf ("baseType = %p = %s \n",baseType,baseType->class_name().c_str());

                    SgType* strippedType = baseType->stripType();
                    ROSE_ASSERT(strippedType != NULL);
                 // printf ("strippedType = %p = %s \n",strippedType,strippedType->class_name().c_str());

                 // This should not be required
                    requiredNodesList.insert(strippedType);
                    addAssociatedNodes(strippedType,requiredNodesList,false);

                 // This allows the three cases of enum, class, and typedef to be generalized (though a typedef can't be defined in a typedef)
                    SgNamedType* namedType = isSgNamedType(strippedType);
                    ROSE_ASSERT(namedType != NULL);
                    SgDeclarationStatement* declaration = namedType->get_declaration();

                 // printf ("hidden declaration = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());

                    requiredNodesList.insert(declaration);
                    addAssociatedNodes(declaration,requiredNodesList,false);

                    ROSE_ASSERT(declaration->get_definingDeclaration() != NULL);
                    set<SgNode*> islandNodes = buildRequiredNodeList(declaration->get_definingDeclaration());
#if 0
                    printf ("Island nodes found: \n");
                    displaySet(islandNodes,"case V_SgTypedefDeclaration: island nodes");
#endif
                 // Insert the islandNode list into the requiredNodesList
                    requiredNodesList.insert(islandNodes.begin(),islandNodes.end());
                 }
              break;
            }

       // A variable declaration can hide another declaration (a named type used as the type within a veriable declaration)
          case V_SgVariableDeclaration:
             {
            // printf ("Found a SgVariableDeclaration = %p looking for islands of untraversed AST ... \n",node);
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
               ROSE_ASSERT(variableDeclaration != NULL);

               bool islandFound = variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration();
            // printf ("islandFound = %s \n",(islandFound == true) ? "true" : "false");
               if (islandFound == true)
                  {
                 // We only want to traverse the base type (since all "*" and "&" are associated
                 // with the variables in this variable list, e.g. list of SgInitializedName objects)

                 // Iterate through the varaible list
                    SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
                    SgInitializedNamePtrList::iterator i = variableList.begin();
                    while (i != variableList.end())
                       {
                         SgType* baseType = (*i)->get_type();
                         ROSE_ASSERT(baseType != NULL);

                         requiredNodesList.insert(baseType);
                         addAssociatedNodes(baseType,requiredNodesList,false);

                      // printf ("baseType = %p = %s \n",baseType,baseType->class_name().c_str());

                         SgType* strippedType = baseType->stripType();
                         ROSE_ASSERT(strippedType != NULL);
                      // printf ("strippedType = %p = %s \n",strippedType,strippedType->class_name().c_str());

                      // This should not be required
                         requiredNodesList.insert(strippedType);
                         addAssociatedNodes(strippedType,requiredNodesList,false);

                      // This allows the three cases of enum, class, and typedef to be generalized (though a typedef can't be defined in a typedef)
                         SgNamedType* namedType = isSgNamedType(strippedType);
                         ROSE_ASSERT(namedType != NULL);
                         SgDeclarationStatement* declaration = namedType->get_declaration();

                      // printf ("hidden declaration = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());

                         requiredNodesList.insert(declaration);
                         addAssociatedNodes(declaration,requiredNodesList,false);

                         ROSE_ASSERT(declaration->get_definingDeclaration() != NULL);
                         set<SgNode*> islandNodes = buildRequiredNodeList(declaration->get_definingDeclaration());
#if 0
                         printf ("Island nodes found: \n");
                         displaySet(islandNodes,"case V_SgVariableDeclaration: island nodes");
#endif
                      // Insert the islandNode list into the requiredNodesList
                         requiredNodesList.insert(islandNodes.begin(),islandNodes.end());

                      // increment to the next variable in the list
                         i++;
                      }
                 }
              break;
            }

          default:
             {
            // ignore all other cases ...
             }
        }
   }


set<SgNode*>
buildRequiredNodeList ( SgNode* node )
   {
     RequiredIRNodes t;
     t.traverse(node,preorder);

  // t.display();

#if 0
  // DQ (2/15/2007): Symbols for un-named classes (and likely enums) are a special problem since
  // they generate multiple symbols with empty names that are then put into the symbol table.
  // The merge operations tend to share the declarations associated with the multiple symbols
  // but there is nothing to remove the redundant symbols from the symbol table.  So here we take
  // all the symbols and just add them to the required node set as a way to handle this problem.
     SgProject* project = isSgProject(node);
     if (project != NULL)
        {
          printf ("Found the project, include the first file's global scope symbols from it's symbol table \n");
          set<SgNode*> symbolSet = project->get_file(0).get_globalScope()->get_symbol_table()->get_symbols();

          printf ("Insert the symbol table symbols into the required node set: t.requiredNodesList.size() = %ld \n",t.requiredNodesList.size());
          t.requiredNodesList.insert(symbolSet.begin(),symbolSet.end());

          printf ("After insert: t.requiredNodesList.size() = %ld \n",t.requiredNodesList.size());
        }     
#endif

     return t.requiredNodesList;
   }

