#include "sage3basic.h"
#include "fixupSelfReferentialMacros.h"

void fixupSelfReferentialMacrosInAST( SgNode* node )
   {
  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup known self-referential macros:");

     FixupSelfReferentialMacrosInAST astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }


void
FixupSelfReferentialMacrosInAST::visit ( SgNode* node )
   {
  // DQ (3/11/2006): Set NULL pointers where we would like to have none.
  // printf ("In FixupSelfReferentialMacrosInAST::visit(): node = %s \n",node->class_name().c_str());

     ROSE_ASSERT(node != NULL);
     switch (node->variantT())
        {
          case V_SgInitializedName:
             {
               SgInitializedName* initializedName = isSgInitializedName(node);
               ROSE_ASSERT(initializedName != NULL);
               SgType* type = initializedName->get_type()->stripType();
               SgClassType* classType = isSgClassType(type);
               if (classType != NULL)
                  {
                    SgClassDeclaration* targetClassDeclaration = isSgClassDeclaration(classType->get_declaration());
                    SgName className = targetClassDeclaration->get_name();

                 // printf ("In FixupSelfReferentialMacrosInAST::visit(): Found a class declaration name = %s \n",className.str());

                    if (className == "sigaction")
                       {
                      // printf ("In FixupSelfReferentialMacrosInAST::visit(): Found a sigaction type \n");

                      // Note we could also check that the declaration came from a known header file.
                         SgStatement* associatedStatement = isSgStatement(initializedName->get_parent());
                         if (associatedStatement != NULL)
                            {
                           // Add a macro to undefine the "#define sa_handler __sigaction_handler.sa_handler" macro.
                           // printf ("In FixupSelfReferentialMacrosInAST::visit(): Add a macro to undefine the macro #define sa_handler __sigaction_handler.sa_handler \n");

                           // PreprocessingInfo* macro = new PreprocessingInfo(DirectiveType, const std::string & inputString,const std::string & filenameString, int line_no , int col_no,int nol, RelativePositionType relPos );

                              PreprocessingInfo::DirectiveType directiveType = PreprocessingInfo::CpreprocessorUndefDeclaration;
                              std::string macroString = "#undef sa_handler\n";
                              std::string filenameString = "macro_call_fixupSelfReferentialMacrosInAST";
                              int line_no = 1;
                              int col_no  = 1;
                              int nol     = 1;
                              PreprocessingInfo::RelativePositionType relPos = PreprocessingInfo::before;

                              PreprocessingInfo* macro = new PreprocessingInfo(directiveType,macroString,filenameString,line_no,col_no,nol,relPos);

                           // printf ("Attaching CPP directive %s to IR node %p as attributes. \n",PreprocessingInfo::directiveTypeName(macro->getTypeOfDirective()).c_str(),associatedStatement);
                              associatedStatement->addToAttachedPreprocessingInfo(macro);
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
                  }
             }

          default:
             {
            // printf ("Not handled in FixupSelfReferentialMacrosInAST::visit(%s) \n",node->class_name().c_str());
             }
        }

   }



extern std::set<SgVariableDeclaration*> nodesAddedWithinFieldUseSet;

void fixupEdgBugDuplicateVariablesInAST()
   {
  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup known EDG bug where some variable declarations are dropped from the source sequence lists:");

     std::set<SgVariableDeclaration*> declarations_to_remove;

  // Loop over all variables added using the convert_field_use() function.
     std::set<SgVariableDeclaration*>::iterator i = nodesAddedWithinFieldUseSet.begin();
     while (i != nodesAddedWithinFieldUseSet.end())
        {
          SgVariableDeclaration* var_decl = *i;
          SgName name = var_decl->get_variables()[0]->get_name();

          SgClassDefinition* classDefinition = isSgClassDefinition(var_decl->get_parent());
          ROSE_ASSERT(classDefinition != NULL);

          std::vector<SgDeclarationStatement*> & members = classDefinition->get_members();

       // Loop over all data members in the class.
          std::vector<SgDeclarationStatement*>::iterator j = members.begin();
          while (j != members.end())
             {
               SgVariableDeclaration* possible_matching_variable_declaration = isSgVariableDeclaration(*j);
               if (possible_matching_variable_declaration != NULL && possible_matching_variable_declaration != var_decl)
                  {
                    if (possible_matching_variable_declaration->get_variables()[0]->get_name() == name)
                       {
#if 0
                         printf ("matching variable declaration found for name = %s \n",name.str());
#endif
                         declarations_to_remove.insert(var_decl);
                       }
                  }
 
               j++;
             }

          i++;
        }

  // Now remove all of the variable declarations that we detected to be duplicates.
     std::set<SgVariableDeclaration*>::iterator k = declarations_to_remove.begin();
     while (k != declarations_to_remove.end())
        {
          SgDeclarationStatement* var_decl = *k;

          SgClassDefinition* classDefinition = isSgClassDefinition(var_decl->get_parent());
          ROSE_ASSERT(classDefinition != NULL);

          std::vector<SgDeclarationStatement*> myvector;
          myvector.push_back(*k);

          std::vector<SgDeclarationStatement*> & members = classDefinition->get_members();

       // members.erase(*k);
       // members.erase(myvector.begin(),myvector.end());

       // This is the remove/erase idiom.
          members.erase(remove(members.begin(), members.end(), *k), members.end());

          k++;
        }

   }


