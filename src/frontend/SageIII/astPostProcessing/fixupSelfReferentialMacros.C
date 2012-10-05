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

