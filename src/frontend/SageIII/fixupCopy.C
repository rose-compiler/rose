#include "rose.h"

// DQ (10/15/2007): This controls the output of debugging information within the fixupCopy() member function of many IR nodes.
// These member functions are used to handle custom IR node specific details of correcting references made by the AST copy 
// mechanism between the original AST and the copy of the AST.
#define DEBUG_FIXUP_COPY FALSE

#define DEBUG_FIXUP_COPY_OUTPUT_MAP FALSE

void outputMap ( SgCopyHelp & help )
   {
     int counter = 0;
     SgCopyHelp::copiedNodeMapTypeIterator i = help.get_copiedNodeMap().begin();

     printf ("COPY MAP: \n");
     while (i != help.get_copiedNodeMap().end())
        {
          if ( (i->first->get_startOfConstruct() != NULL) && (i->first->get_startOfConstruct()->isFrontendSpecific() == false) )
             {
               printf ("entry %4d: i->first = %p  i->second = %p = %s\n",counter,i->first,i->second,i->first->class_name().c_str());
             }

       // printf ("entry %4d: i->first = %p  i->second = %p = %s\n",counter,i->first,i->second,i->first->class_name().c_str());
          ROSE_ASSERT(i->first->variantT() == i->second->variantT());

          counter++;
          i++;
        }
   }

// DQ (10/16/2007): This is a macro to simplify the code associated with fixing up data member in the AST copy.
#if DEBUG_FIXUP_COPY
#define FixupCopyDataMemberMacro(IR_node_copy,IR_node_type,get_accessFunctionName,set_accessFunctionName)                \
     if (IR_node_copy->get_accessFunctionName() == this->get_accessFunctionName())                                       \
        {                                                                                                                \
          SgCopyHelp::copiedNodeMapTypeIterator i = help.get_copiedNodeMap().find(this->get_accessFunctionName());       \
          printf ("SgCopyHelp::copiedNodeMapTypeIterator i != help.get_copiedNodeMap().end() = %s (using %s()) \n",i != help.get_copiedNodeMap().end() ? "true" : "false",#get_accessFunctionName); \
          if (i != help.get_copiedNodeMap().end())                                                                       \
             {                                                                                                           \
               SgNode* associated_node_copy = i->second;                                                                 \
               ROSE_ASSERT(associated_node_copy != NULL);                                                                \
               IR_node_type* local_copy = is##IR_node_type(associated_node_copy);                                        \
               ROSE_ASSERT(local_copy != NULL);                                                                          \
               printf ("Resetting using %s->%s(local_copy) = %p = %s \n",#IR_node_copy,#set_accessFunctionName,local_copy,local_copy->class_name().c_str()); \
               IR_node_copy->set_accessFunctionName(local_copy);                                                         \
             }                                                                                                           \
          ROSE_ASSERT(IR_node_copy->get_accessFunctionName()->variantT() == this->get_accessFunctionName()->variantT()); \
        }                                                                                                                \
       else                                                                                                              \
        {                                                                                                                \
           printf ("%s->%s() = %p != this->%s() = %p (so %s is already set for %p) \n",#IR_node_copy,#get_accessFunctionName,IR_node_copy,#get_accessFunctionName,this,#get_accessFunctionName,IR_node_copy); \
        }
#else
#define FixupCopyDataMemberMacro(IR_node_copy,IR_node_type,get_accessFunctionName,set_accessFunctionName)                \
     if (IR_node_copy->get_accessFunctionName() == this->get_accessFunctionName())                                       \
        {                                                                                                                \
          SgCopyHelp::copiedNodeMapTypeIterator i = help.get_copiedNodeMap().find(this->get_accessFunctionName());       \
          if (i != help.get_copiedNodeMap().end())                                                                       \
             {                                                                                                           \
               SgNode* associated_node_copy = i->second;                                                                 \
               ROSE_ASSERT(associated_node_copy != NULL);                                                                \
               IR_node_type* local_copy = is##IR_node_type(associated_node_copy);                                        \
               ROSE_ASSERT(local_copy != NULL);                                                                          \
               IR_node_copy->set_accessFunctionName(local_copy);                                                         \
             }                                                                                                           \
          ROSE_ASSERT(IR_node_copy->get_accessFunctionName()->variantT() == this->get_accessFunctionName()->variantT()); \
        }
#endif


void
resetVariableDefnitionSupport ( const SgInitializedName* originalInitializedName, SgInitializedName* copyInitializedName, SgDeclarationStatement* targetDeclaration )
   {
  // DQ (10/8/2007): This is s supporting function to the SgInitializedName::fixupCopy() member function.

  // Where the scope is reset, also build a new SgVariableDefinition (I forget why).
     ROSE_ASSERT(copyInitializedName->get_declptr() != NULL);

     SgNode* originalDeclaration = originalInitializedName->get_declptr();
     switch(originalDeclaration->variantT())
        {
          case V_SgVariableDefinition:
             {
            // In this case the target declaration is a SgVariableDefinition, and it has to be constructed, so the input paremter is NULL.
               ROSE_ASSERT(targetDeclaration == NULL);

               SgVariableDefinition* variableDefinition_original = isSgVariableDefinition(originalInitializedName->get_declptr());
               ROSE_ASSERT(variableDefinition_original != NULL);
               SgUnsignedLongVal* bitfield = variableDefinition_original->get_bitfield();
               SgVariableDefinition* variableDefinition_copy = new SgVariableDefinition(copyInitializedName,bitfield);
               ROSE_ASSERT(variableDefinition_copy != NULL);
               copyInitializedName->set_declptr(variableDefinition_copy);
               variableDefinition_copy->set_parent(copyInitializedName);

            // This is the same way that Sg_File_Info objects are built in the copy mechanism.
               Sg_File_Info* newStartOfConstruct = new Sg_File_Info(*(variableDefinition_original->get_startOfConstruct()));
               Sg_File_Info* newEndOfConstruct   = new Sg_File_Info(*(variableDefinition_original->get_endOfConstruct()));

               variableDefinition_copy->set_startOfConstruct(newStartOfConstruct);
               variableDefinition_copy->set_endOfConstruct(newEndOfConstruct);

               newStartOfConstruct->set_parent(variableDefinition_copy);
               newEndOfConstruct->set_parent(variableDefinition_copy);

               break;
             }

          case V_SgEnumDeclaration:
             {
            // In this case the target declaration is a SgEnumDeclaration, and it has already been copied so it need not be created.
               ROSE_ASSERT(targetDeclaration != NULL);

               SgEnumDeclaration* enumDeclaration_original = isSgEnumDeclaration(originalInitializedName->get_declptr());
               if (copyInitializedName->get_declptr() == enumDeclaration_original)
                  {
                 // Then reset to the targetDeclaration.
                    copyInitializedName->set_declptr(targetDeclaration);
                  }

               break;
             }

          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
          case V_SgProcedureHeaderStatement:
          case V_SgProgramHeaderStatement:
             {
            // In this case the target declaration is a SgFunctionDeclaration, and it has already been copied so it need not be created.
               ROSE_ASSERT(targetDeclaration != NULL);

               SgFunctionDeclaration* functionDeclaration_original = isSgFunctionDeclaration(originalInitializedName->get_declptr());
               if (copyInitializedName->get_declptr() == functionDeclaration_original)
                  {
                 // Then reset to the targetDeclaration.
                    copyInitializedName->set_declptr(targetDeclaration);
                  }

               break;
             }

          default:
             {
               printf ("Error: default reached in resetVariableDefnitionSupport() originalDeclaration = %p = %s \n",originalDeclaration,originalDeclaration->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

   }


// DQ (10/5/2007): Added IR node specific function to permit copies, via AST copy(), to be fixedup
// Usually this will correct scopes and in a few cases build child IR nodes that are not traversed
// (and thus shared in the result from the automatically generated copy function).
void
SgInitializedName::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // This is the empty default inplementation, not a problem if it is called!

#if DEBUG_FIXUP_COPY
  // printf ("Inside of SgInitializedName::fixupCopy() %p = %s \n",this,SageInterface::get_name(this).c_str());
     printf ("Inside of SgInitializedName::fixupCopy() %p = %s \n",this,this->get_name().str());
#endif

  // Need to fixup the scope and perhaps build the SgVariableDefinition object!

     SgInitializedName* initializedName_copy = isSgInitializedName(copy);
     ROSE_ASSERT(initializedName_copy != NULL);

  // ROSE_ASSERT(this->get_symbol_from_symbol_table() != NULL);

     if (initializedName_copy->get_scope() == this->get_scope())
        {
          FixupCopyDataMemberMacro(initializedName_copy,SgScopeStatement,get_scope,set_scope)

          SgNode* parent = initializedName_copy->get_parent();

       // printf ("In SgInitializedName::fixupCopy(): parent = %p \n",parent);
 
       // Since the parent might not have been set yet we have to allow for this case. In the case of a 
       // SgInitializedName in a SgVariableDeclaration the SgInitializedName objects have their parents 
       // set after the SgInitializedName is copied and in the copy function for the parent (SgVariableDeclaration).
          if (parent != NULL)
             {
               ROSE_ASSERT(parent != NULL);
            // printf ("In SgInitializedName::fixupCopy(): parent = %p = %s \n",parent,parent->class_name().c_str());

               switch(parent->variantT())
                  {
                    case V_SgVariableDeclaration:
                       {
                         resetVariableDefnitionSupport(this,initializedName_copy,NULL);
                         break;
                       }

                    case V_SgEnumDeclaration:
                       {
                         SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(parent);
                         ROSE_ASSERT(enumDeclaration != NULL);
                         resetVariableDefnitionSupport(this,initializedName_copy,enumDeclaration);
                         break;
                       }

                    case V_SgFunctionParameterList:
                       {
                         SgNode* parentFunction = parent->get_parent();
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentFunction);

                      // The parent of the SgFunctionParameterList might not have been set yet, so allow for this!
                         if (functionDeclaration != NULL)
                            {
                              resetVariableDefnitionSupport(this,initializedName_copy,functionDeclaration);
                            }
                         break;
                       }

                    default:
                       {
                         printf ("default reached in SgInitializedName::fixupCopy() parent = %p = %s \n",parent,parent->class_name().c_str());
                         ROSE_ASSERT(false);
                         break;
                       }
                  }
             }
        }
       else
        {
       // printf ("Skipping resetting the scope for initializedName_copy = %p = %s \n",initializedName_copy,initializedName_copy->get_name().str());
        }


     if (this->get_prev_decl_item() != NULL)
        {
          FixupCopyDataMemberMacro(initializedName_copy,SgInitializedName,get_prev_decl_item,set_prev_decl_item)
        }

  // DQ (10/16/2007): Check this!
     if (this->get_symbol_from_symbol_table() != NULL)
        {
       // It is still too early to expect the symbol table for any new scope to have been setup so we can't really enforce this here.
#if 0
       // Note that initialized name object in function prototypes don't have associated symbols since they have no scope to put them.
          if (initializedName_copy->get_symbol_from_symbol_table() == NULL)
             {
               printf ("ERROR: initializedName_copy->get_symbol_from_symbol_table() == NULL  initializedName_copy = %p = %s \n",initializedName_copy,SageInterface::get_name(initializedName_copy).c_str());
             }
       // ROSE_ASSERT(initializedName_copy->get_symbol_from_symbol_table() != NULL);
#endif
        }

#if DEBUG_FIXUP_COPY
     printf ("Leaving SgInitializedName::fixupCopy() \n\n");
#endif
   }

#if 0
// DQ (10/5/2007): Added IR node specific function to permit copies, via AST copy(), to be fixedup
// Usually this will correct scopes and in a few cases build child IR nodes that are not traversed
// (and thus shared in the result from the automatically generated copy function).
void
SgStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgVariableDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // printf ("Inside of SgStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);

  // Define a traversal to update the references to symbols (per statement)
     class StatementTraversal : public AstSimpleProcessing
        {
          private:
              SgCopyHelp & helpSupport;

          public:
               StatementTraversal (SgCopyHelp & help) : helpSupport(help) {}

               void visit(SgNode* n)
                  {
                    switch(n->variantT())
                       {
                         case V_SgVarRefExp:
                            {
                              SgVarRefExp* varRefExp = isSgVarRefExp(n);
                              ROSE_ASSERT(varRefExp != NULL);
                              SgVariableSymbol* variableSymbol_original = varRefExp->get_symbol();
                              ROSE_ASSERT(variableSymbol_original != NULL);
                              SgInitializedName* initializedName_original = variableSymbol_original->get_declaration();
                              ROSE_ASSERT(initializedName_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(initializedName_original);

                           // printf ("Inside of SgStatement::fixupCopy(): i != helpSupport.get_copiedNodeMap().end() = %s \n",
                           //      (i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgInitializedName* initializedName_copy = isSgInitializedName(i->second);
                                   ROSE_ASSERT(initializedName_copy != NULL);
                                   SgSymbol* symbol_copy = initializedName_copy->get_symbol_from_symbol_table();
                                // printf ("Inside of SgStatement::fixupCopy(): symbol_copy = %p \n",symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgVariableSymbol* variableSymbol_copy = isSgVariableSymbol(symbol_copy);
                                        ROSE_ASSERT(variableSymbol_copy != NULL);
                                        varRefExp->set_symbol(variableSymbol_copy);
                                      }
                                 }
                              break;
                            }

                         case V_SgFunctionRefExp:
                            {
                              SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(n);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgFunctionSymbol* functionSymbol_original = functionRefExp->get_symbol();
                              ROSE_ASSERT(functionSymbol_original != NULL);
                              SgFunctionDeclaration* functionDeclaration_original = functionSymbol_original->get_declaration();
                              ROSE_ASSERT(functionDeclaration_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(functionDeclaration_original);

                           // printf ("Inside of SgStatement::fixupCopy(): (case SgFunctionRefExp) i != helpSupport.get_copiedNodeMap().end() = %s \n",
                           //      (i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgFunctionDeclaration* functionDeclaration_copy = isSgFunctionDeclaration(i->second);
                                   ROSE_ASSERT(functionDeclaration_copy != NULL);
                                   SgSymbol* symbol_copy = functionDeclaration_copy->get_symbol_from_symbol_table();
                                // printf ("Inside of SgStatement::fixupCopy(): symbol_copy = %p \n",symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgFunctionSymbol* functionSymbol_copy = isSgFunctionSymbol(symbol_copy);
                                        ROSE_ASSERT(functionSymbol_copy != NULL);
                                        functionRefExp->set_symbol(functionSymbol_copy);
                                      }
                                 }
                              break;
                            }

                         case V_SgMemberFunctionRefExp:
                            {
                              SgMemberFunctionRefExp* functionRefExp = isSgMemberFunctionRefExp(n);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgMemberFunctionSymbol* functionSymbol_original = functionRefExp->get_symbol();
                              ROSE_ASSERT(functionSymbol_original != NULL);
                              SgMemberFunctionDeclaration* functionDeclaration_original = functionSymbol_original->get_declaration();
                              ROSE_ASSERT(functionDeclaration_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(functionDeclaration_original);

                           // printf ("Inside of SgStatement::fixupCopy(): (case SgMemberFunctionRefExp) i != helpSupport.get_copiedNodeMap().end() = %s \n",
                           //      (i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgMemberFunctionDeclaration* functionDeclaration_copy = isSgMemberFunctionDeclaration(i->second);
                                   ROSE_ASSERT(functionDeclaration_copy != NULL);
                                   SgSymbol* symbol_copy = functionDeclaration_copy->get_symbol_from_symbol_table();
                                // printf ("Inside of SgStatement::fixupCopy(): symbol_copy = %p \n",symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgMemberFunctionSymbol* functionSymbol_copy = isSgMemberFunctionSymbol(symbol_copy);
                                        ROSE_ASSERT(functionSymbol_copy != NULL);
                                        functionRefExp->set_symbol(functionSymbol_copy);
                                      }
                                 }
                              break;
                            }

                      // DQ (10/16/2007): Note that labels are handled by the SgLabelStatement and the SgGotoStatement 
                      // directly and are not required to be processed here in the SgStatement.
                         case V_SgLabelStatement:
                            {
                           // printf ("Inside of SgStatement::fixupCopy(): we might have to handle SgLabelStatement \n");
                              break;
                            }

                          default:
                            {
                           // Nothing to do for this case
                            }
                       }
                  }
        };

  // Build an run the traversal defined above.
     StatementTraversal t(help);
     t.traverse(copy,preorder);

     SgStatement* copyStatement = isSgStatement(copy);
     ROSE_ASSERT(copyStatement != NULL);

  // DQ (10/24/2007): New test.
     ROSE_ASSERT(copyStatement->variantT() == this->variantT());

  // DQ (7/15/2007): Added assertion...
  // ROSE_ASSERT(this->get_parent() != NULL);
  // ROSE_ASSERT(copyStatement->get_parent() != NULL);

  // DQ (10/15/2007): If the parent of the original AST is not set then we will not process theparent in the copy, 
  // thus the AST copy mechanism can handle incompletely setup AST (as required for use in the EDG/Sage translation) 
  // yet only return an AST of similare quality.
     if (this->get_parent() != NULL)
        {
       // Make sure that the copy sets the scopes to be the same type
       // if (copyStatement->get_parent() == NULL || copyStatement->get_parent()->variantT() != this->get_parent()->variantT())
       // printf ("copyStatement->get_parent() = %p \n",copyStatement->get_parent());

       // printf ("this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
       // printf ("copyStatement->get_parent() = %p = %s \n",copyStatement->get_parent(),copyStatement->get_parent() == NULL ? "NULL" : copyStatement->get_parent()->class_name().c_str());
       // printf ("In SgStatement::fixupCopy(): this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
       // printf ("In SgStatement::fixupCopy(): copyStatement->get_parent() = %p = %s \n",copyStatement->get_parent(),copyStatement->get_parent()->class_name().c_str());
       // ROSE_ASSERT(copyStatement->get_parent() != NULL);

          FixupCopyDataMemberMacro(copyStatement,SgNode,get_parent,set_parent)

       // DQ (7/15/2007): Added assertion...
          ROSE_ASSERT(this->get_parent() != NULL);

       // I don't think we can force that a copy has a valid parent set
       // ROSE_ASSERT(copyStatement->get_parent() != NULL);
       // if (copyStatement->get_parent()->variantT() != this->get_parent()->variantT())

       // Debugging information
          if (copyStatement->get_parent() == NULL)
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("In SgStatement::fixupCopy(): this->get_parent() != NULL, but copyStatement->get_parent() == NULL for copyStatement = %p = %s \n",copyStatement,copyStatement->class_name().c_str());
               printf ("     this                        = %p = %s \n",this,this->class_name().c_str());
               printf ("     this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
#endif
             }

       // Debugging information
          if (copyStatement->get_parent() != NULL && copyStatement->get_parent()->variantT() != this->get_parent()->variantT())
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("Warning: In SgStatement::fixupCopy(): the parent of this and copyStatement are different \n");
               printf ("     this                        = %p = %s \n",this,this->class_name().c_str());
               printf ("     copyStatement               = %p = %s \n",copyStatement,copyStatement->class_name().c_str());
               printf ("     this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
               printf ("     copyStatement->get_parent() = %p = %s \n",copyStatement->get_parent(),copyStatement->get_parent()->class_name().c_str());

               this->get_startOfConstruct()->display("this->get_startOfConstruct(): debug");
#endif
             }
       // ROSE_ASSERT(copyStatement->get_parent()->variantT() == this->get_parent()->variantT());
        }
       else
        {
       // printf ("In SgStatement::fixupCopy(): parent not set for original AST at %p = %s, thus copy left similarly incomplete \n",this,this->class_name().c_str());
        }

#if DEBUG_FIXUP_COPY
     printf ("Leaving SgStatement::fixupCopy() \n\n");
#endif
   }
#else


// DQ (11/1/2007): Build lighter weight versions of SgStatement::fixupCopy() and SgExpression::fixupCopy() 
// and refactor code into the SgLocatedNode::fixupCopy().

void
SgStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgVariableDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgLocatedNode::fixupCopy(copy,help);
   }

void
SgExpression::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgExpression::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgLocatedNode::fixupCopy(copy,help);
   }

// DQ (11/1/2007): Make this work on SgLocatedNode (so that it can work on SgStatement and SgExpression).

// DQ (10/5/2007): Added IR node specific function to permit copies, via AST copy(), to be fixedup
// Usually this will correct scopes and in a few cases build child IR nodes that are not traversed
// (and thus shared in the result from the automatically generated copy function).
void
SgLocatedNode::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // Fixup references in SgStatements and SgExpressions

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgLocatedNode::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // printf ("Inside of SgLocatedNode::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);

  // Define a traversal to update the references to symbols (per statement)
     class Traversal : public AstSimpleProcessing
        {
          private:
              SgCopyHelp & helpSupport;

          public:
               Traversal (SgCopyHelp & help) : helpSupport(help) {}

               void visit(SgNode* n)
                  {
                    switch(n->variantT())
                       {
                         case V_SgVarRefExp:
                            {
                              SgVarRefExp* varRefExp = isSgVarRefExp(n);
                              ROSE_ASSERT(varRefExp != NULL);
                              SgVariableSymbol* variableSymbol_original = varRefExp->get_symbol();
                              ROSE_ASSERT(variableSymbol_original != NULL);
                              SgInitializedName* initializedName_original = variableSymbol_original->get_declaration();
                              ROSE_ASSERT(initializedName_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(initializedName_original);
  
			      if (i == helpSupport.get_copiedNodeMap().end()) {
				cerr << "We did not copy initialized name for " << initializedName_original->get_name().getString() << endl;
			      }
                           // printf ("Inside of SgStatement::fixupCopy(): i != helpSupport.get_copiedNodeMap().end() = %s \n",
                           //      (i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgInitializedName* initializedName_copy = isSgInitializedName(i->second);
                                   ROSE_ASSERT(initializedName_copy != NULL);
                                   SgSymbol* symbol_copy = initializedName_copy->get_symbol_from_symbol_table();
                                // printf ("Inside of SgStatement::fixupCopy(): symbol_copy = %p \n",symbol_copy);
				   ROSE_ASSERT (symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgVariableSymbol* variableSymbol_copy = isSgVariableSymbol(symbol_copy);
                                        ROSE_ASSERT(variableSymbol_copy != NULL);
                                        varRefExp->set_symbol(variableSymbol_copy);
                                      }
                                 }
                              break;
                            }

                         case V_SgFunctionRefExp:
                            {
                              SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(n);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgFunctionSymbol* functionSymbol_original = functionRefExp->get_symbol();
                              ROSE_ASSERT(functionSymbol_original != NULL);
                              SgFunctionDeclaration* functionDeclaration_original = functionSymbol_original->get_declaration();
                              ROSE_ASSERT(functionDeclaration_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(functionDeclaration_original);

                           // printf ("Inside of SgStatement::fixupCopy(): (case SgFunctionRefExp) i != helpSupport.get_copiedNodeMap().end() = %s \n",
                           //      (i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgFunctionDeclaration* functionDeclaration_copy = isSgFunctionDeclaration(i->second);
                                   ROSE_ASSERT(functionDeclaration_copy != NULL);
                                   SgSymbol* symbol_copy = functionDeclaration_copy->get_symbol_from_symbol_table();
                                // printf ("Inside of SgStatement::fixupCopy(): symbol_copy = %p \n",symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgFunctionSymbol* functionSymbol_copy = isSgFunctionSymbol(symbol_copy);
                                        ROSE_ASSERT(functionSymbol_copy != NULL);
                                        functionRefExp->set_symbol(functionSymbol_copy);
                                      }
                                 }
                              break;
                            }

                         case V_SgMemberFunctionRefExp:
                            {
                              SgMemberFunctionRefExp* functionRefExp = isSgMemberFunctionRefExp(n);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgMemberFunctionSymbol* functionSymbol_original = functionRefExp->get_symbol();
                              ROSE_ASSERT(functionSymbol_original != NULL);
                              SgMemberFunctionDeclaration* functionDeclaration_original = functionSymbol_original->get_declaration();
                              ROSE_ASSERT(functionDeclaration_original != NULL);
                              SgCopyHelp::copiedNodeMapTypeIterator i = helpSupport.get_copiedNodeMap().find(functionDeclaration_original);

                              printf ("Inside of SgStatement::fixupCopy(): (case SgMemberFunctionRefExp) functionSymbol_original = %p i != helpSupport.get_copiedNodeMap().end() = %s \n",
                                   functionSymbol_original,(i != helpSupport.get_copiedNodeMap().end()) ? "true" : "false");

                           // If the declaration is in the map then it is because we have copied it previously
                           // and thus it symbol should be updated to reflect the copied declaration.
                           // ROSE_ASSERT(i != help.get_copiedNodeMap().end());
                              if (i != helpSupport.get_copiedNodeMap().end())
                                 {
                                   SgMemberFunctionDeclaration* functionDeclaration_copy = isSgMemberFunctionDeclaration(i->second);
                                   ROSE_ASSERT(functionDeclaration_copy != NULL);
                                   SgSymbol* symbol_copy = functionDeclaration_copy->get_symbol_from_symbol_table();
                                   printf ("Inside of SgStatement::fixupCopy(): symbol_copy = %p \n",symbol_copy);
                                   if (symbol_copy != NULL)
                                      {
                                        SgMemberFunctionSymbol* functionSymbol_copy = isSgMemberFunctionSymbol(symbol_copy);
                                        ROSE_ASSERT(functionSymbol_copy != NULL);
                                        functionRefExp->set_symbol(functionSymbol_copy);
                                      }
                                     else
                                      {
                                        printf ("Error: could not find symbol associated with functionDeclaration_copy = %p = %s = %s \n",functionDeclaration_copy,functionDeclaration_copy->class_name().c_str(),SageInterface::get_name(functionDeclaration_copy).c_str());
                                     // ROSE_ASSERT(false);
                                      }
                                 }
                              break;
                            }

                      // DQ (10/16/2007): Note that labels are handled by the SgLabelStatement and the SgGotoStatement 
                      // directly and are not required to be processed here in the SgStatement.
                         case V_SgLabelStatement:
                            {
                           // printf ("Inside of SgStatement::fixupCopy(): we might have to handle SgLabelStatement \n");
                              break;
                            }

                          default:
                            {
                           // Nothing to do for this case
                            }
                       }
                  }
        };

  // Build an run the traversal defined above.
     Traversal t(help);
     t.traverse(copy,preorder);

     SgLocatedNode* copyLocatedNode = isSgLocatedNode(copy);
     ROSE_ASSERT(copyLocatedNode != NULL);

  // DQ (10/24/2007): New test.
     ROSE_ASSERT(copyLocatedNode->variantT() == this->variantT());

  // DQ (7/15/2007): Added assertion...
  // ROSE_ASSERT(this->get_parent() != NULL);
  // ROSE_ASSERT(copyStatement->get_parent() != NULL);

  // DQ (10/15/2007): If the parent of the original AST is not set then we will not process theparent in the copy, 
  // thus the AST copy mechanism can handle incompletely setup AST (as required for use in the EDG/Sage translation) 
  // yet only return an AST of similare quality.
     if (this->get_parent() != NULL)
        {
       // Make sure that the copy sets the scopes to be the same type
       // if (copyStatement->get_parent() == NULL || copyStatement->get_parent()->variantT() != this->get_parent()->variantT())
       // printf ("copyStatement->get_parent() = %p \n",copyStatement->get_parent());

       // printf ("this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
       // printf ("copyStatement->get_parent() = %p = %s \n",copyStatement->get_parent(),copyStatement->get_parent() == NULL ? "NULL" : copyStatement->get_parent()->class_name().c_str());
       // printf ("In SgStatement::fixupCopy(): this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
       // printf ("In SgStatement::fixupCopy(): copyStatement->get_parent() = %p = %s \n",copyStatement->get_parent(),copyStatement->get_parent()->class_name().c_str());
       // ROSE_ASSERT(copyStatement->get_parent() != NULL);

          FixupCopyDataMemberMacro(copyLocatedNode,SgNode,get_parent,set_parent)

       // DQ (7/15/2007): Added assertion...
          ROSE_ASSERT(this->get_parent() != NULL);

       // I don't think we can force that a copy has a valid parent set
       // ROSE_ASSERT(copyStatement->get_parent() != NULL);
       // if (copyStatement->get_parent()->variantT() != this->get_parent()->variantT())

       // Debugging information
          if (copyLocatedNode->get_parent() == NULL)
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("In SgLocatedNode::fixupCopy(): this->get_parent() != NULL, but copyLocatedNode->get_parent() == NULL for copyLocatedNode = %p = %s \n",copyLocatedNode,copyLocatedNode->class_name().c_str());
               printf ("     this                        = %p = %s \n",this,this->class_name().c_str());
               printf ("     this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
#endif
             }

       // Debugging information
          if (copyLocatedNode->get_parent() != NULL && copyLocatedNode->get_parent()->variantT() != this->get_parent()->variantT())
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("Warning: In SgLocatedNode::fixupCopy(): the parent of this and copyStatement are different \n");
               printf ("     this                        = %p = %s \n",this,this->class_name().c_str());
               printf ("     copyLocatedNode             = %p = %s \n",copyLocatedNode,copyLocatedNode->class_name().c_str());
               printf ("     this->get_parent()          = %p = %s \n",this->get_parent(),this->get_parent()->class_name().c_str());
               printf ("     copyStatement->get_parent() = %p = %s \n",copyLocatedNode->get_parent(),copyLocatedNode->get_parent()->class_name().c_str());

               this->get_startOfConstruct()->display("this->get_startOfConstruct(): debug");
#endif
             }
       // ROSE_ASSERT(copyStatement->get_parent()->variantT() == this->get_parent()->variantT());
        }
       else
        {
       // printf ("In SgStatement::fixupCopy(): parent not set for original AST at %p = %s, thus copy left similarly incomplete \n",this,this->class_name().c_str());
        }

#if DEBUG_FIXUP_COPY
     printf ("Leaving SgLocatedNode::fixupCopy() \n\n");
#endif
   }
#endif

void
SgScopeStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgVariableDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgScopeStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // printf ("\nInside of SgScopeStatement::fixupCopy() for %p = %s copy = %p \n\n",this,this->class_name().c_str(),copy);
  // this->get_startOfConstruct()->display("Inside of SgScopeStatement::fixupCopy()");

     SgScopeStatement* copyScopeStatement = isSgScopeStatement(copy);
     ROSE_ASSERT(copyScopeStatement != NULL);

  // The symbol table should not have been setup yet!
     ROSE_ASSERT(copyScopeStatement->get_symbol_table()->size() == 0);

  // DQ (10/24/2007): New test.
     ROSE_ASSERT(copyScopeStatement->variantT() == this->variantT());

#if 0
  // DQ (10/24/2007): The scope does not have any list of statements so handle this in the derived classes where such lists are defined.

  // DQ (10/24/2007): Let the SgIfStmt be setup as its separate true and false bodies are setup instead of using all
  // the statements generated from the true and fallse blocks using the generateStatementList() member function.
  // Note that exiting eary may be a problem if there is a declaration in the conditional test, which can be a declaration statement.
     const SgIfStmt* ifStatement = isSgIfStmt(this);
     const SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(this);
     if (ifStatement != NULL || functionDeclaration != NULL)
        {
          printf ("Skipping processing of the SgStatementPtrList generated from the SgScopeStatement::generateStatementList() member function. \n");
        }
       else
        {
          const SgStatementPtrList & statementList_original = this->generateStatementList();
          const SgStatementPtrList & statementList_copy     = copyScopeStatement->generateStatementList();

       // Check that this need not be handled as a special case such as SgIfStmt.
          if (this->containsOnlyDeclarations() == true)
             {
               ROSE_ASSERT(this->getDeclarationList().size() == statementList_original.size());
             }
            else
             {
               ROSE_ASSERT(this->getStatementList().size() == statementList_original.size());
             }

          SgStatementPtrList::const_iterator i_original = statementList_original.begin();
          SgStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

       // Iterate over both lists to match up the correct pairs of SgStatement objects
          while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
             {
               (*i_original)->fixupCopy(*i_copy,help);

               i_original++;
               i_copy++;
             }
        }
#endif

  // The symbol table should not have been setup yet!
     ROSE_ASSERT(copyScopeStatement->get_symbol_table()->size() == 0);

  // Call the base class fixupCopy member function
     SgStatement::fixupCopy(copy,help);

#if 0
     printf ("Setting up the call to SageInterface::rebuildSymbolTable(copyScopeStatement) from SgScopeStatement::fixupCopy() for %p = %s \n",this,this->class_name().c_str());
     printf ("this->generateStatementList().size()               = %zu \n",this->generateStatementList().size());
     printf ("copyScopeStatement->generateStatementList().size() = %zu \n",copyScopeStatement->generateStatementList().size());
     printf ("this->get_symbol_table()->size()               = %d \n",(int)this->get_symbol_table()->size());
     printf ("copyScopeStatement->get_symbol_table()->size() = %d \n",(int)copyScopeStatement->get_symbol_table()->size());

  // printf ("Output the symbol table for this \n");
  // this->get_symbol_table()->print("debugging copy problem");
  // printf ("Output the symbol table for copyScopeStatement \n");
  // copyScopeStatement->get_symbol_table()->print("debugging copy problem");

     printf ("Calling SageInterface::rebuildSymbolTable(copyScopeStatement) from SgScopeStatement::fixupCopy() (This is as late as possible!) \n");

     if (copyScopeStatement->get_symbol_table()->size() > 0)
          this->get_startOfConstruct()->display("Calling SageInterface::rebuildSymbolTable(copyScopeStatement) from SgScopeStatement::fixupCopy(): debug");
#endif

  // The symbol table should not have been setup yet!
     ROSE_ASSERT(copyScopeStatement->get_symbol_table()->size() == 0);

     SageInterface::rebuildSymbolTable(copyScopeStatement); 

  // printf ("\nLeaving SgScopeStatement::fixupCopy() for %p = %s copy = %p \n\n",this,this->class_name().c_str(),copy);
   }



void
SgGlobal::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // This is the empty default inplementation, not a problem if it is called!

     SgGlobal* global_copy = isSgGlobal(copy);
     ROSE_ASSERT(global_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgGlobal::fixupCopy() for %p copy = %p \n",this,copy);
#endif

  // const SgStatementPtrList & statementList_original = this->generateStatementList();
  // const SgStatementPtrList & statementList_copy     = block_copy->generateStatementList();
     const SgDeclarationStatementPtrList & statementList_original = this->getDeclarationList();
     const SgDeclarationStatementPtrList & statementList_copy     = global_copy->getDeclarationList();

     SgDeclarationStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgDeclarationStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
          (*i_original)->fixupCopy(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy(copy,help);

  // printf ("\nLeaving SgGlobal::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

// DQ (10/6/2007): Added fixup function to set scopes not set properly by the ROSETTA generated copy!
void
SgBasicBlock::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgBasicBlock* block_copy = isSgBasicBlock(copy);
     ROSE_ASSERT(block_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgBasicBlock::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // const SgStatementPtrList & statementList_original = this->generateStatementList();
  // const SgStatementPtrList & statementList_copy     = block_copy->generateStatementList();
     const SgStatementPtrList & statementList_original = this->getStatementList();
     const SgStatementPtrList & statementList_copy     = block_copy->getStatementList();

  // Check that this need not be handled as a special case such as SgIfStmt.
     if (this->containsOnlyDeclarations() == true)
        {
          ROSE_ASSERT(this->getDeclarationList().size() == statementList_original.size());
        }
       else
        {
          ROSE_ASSERT(this->getStatementList().size() == statementList_original.size());
        }

     SgStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
          (*i_original)->fixupCopy(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy(copy,help);

  // printf ("\nLeaving SgBasicBlock::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

void
SgDeclarationStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // This is the empty default inplementation, not a problem if it is called!
  // printf ("Inside of SgDeclarationStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDeclarationStatement::fixupCopy() for %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif
#if DEBUG_FIXUP_COPY_OUTPUT_MAP
     outputMap(help);
#endif

  // Need to fixup the scopes and defining and non-defining declaration.

     SgDeclarationStatement* copyDeclarationStatement = isSgDeclarationStatement(copy);
     ROSE_ASSERT(copyDeclarationStatement != NULL);

  // DQ (10/20/2007): This is an essential piece of the fixup of the AST copy.  This is a recursive construction of IR nodes that have
  // defining and non-defining parts used when either or both were not copied in the initial call to the copy member functions.
  // Since we can visit the IR nodes in any order, and some declaration can have multiple parts (defining and non-defining declaration)
  // We have to make sure that these are processed first (in some cases they may have escaped being copied by the AST Copy mechanism
  // when they were not part of the traversed AST). So we have to build them if they don't exist (triggered by existance in the copy map).

  // Check if this IR node has been copied?  If so then it is in the copy map.
  // bool hasBeenCopied = (help.get_copiedNodeMap().find(this) != help.get_copiedNodeMap().end());
  // printf ("hasBeenCopied = %s \n",hasBeenCopied ? "true" : "false");
  // if (hasBeenCopied == false)

     bool definingDeclarationCopied         = (help.get_copiedNodeMap().find(this->get_definingDeclaration()) != help.get_copiedNodeMap().end());
     bool firstNondefiningDeclarationCopied = (help.get_copiedNodeMap().find(this->get_firstNondefiningDeclaration()) != help.get_copiedNodeMap().end());
  // printf ("definingDeclarationCopied = %s firstNondefiningDeclarationCopied = %s \n",definingDeclarationCopied ? "true" : "false", firstNondefiningDeclarationCopied ? "true" : "false");
     if (definingDeclarationCopied == true && firstNondefiningDeclarationCopied == true)
        {
       // We can process the current non-defining declaration (which is not the first non-defining declaration)
        }
       else
        {
       // Note sure if we need these variables
       // SgDeclarationStatement* copyOfDefiningDeclaration         = NULL;
       // SgDeclarationStatement* copyOfFirstNondefiningDeclaration = NULL;

       // Note: This will cause the first non-defining declaration to be copied first when neither have been processed.
          if (this->get_firstNondefiningDeclaration() != NULL && firstNondefiningDeclarationCopied == false)
             {
            // Setup the firstNondefining declaration 
            // ROSE_ASSERT(definingDeclarationCopied == true);

            // We could build vitual constructors, but then that is what the copy function is so call copy!
            // This also added the firstNondefiningDeclaration to the copy map 

            // DQ (10/21/2007): Use the copy help object so that it can control copying of defining vs. non-defining declaration.
            // SgNode* copyOfFirstNondefiningDeclarationNode = this->get_firstNondefiningDeclaration()->copy(help);
               SgNode* copyOfFirstNondefiningDeclarationNode = help.copyAst(this->get_firstNondefiningDeclaration()); 

               ROSE_ASSERT(copyOfFirstNondefiningDeclarationNode != NULL);
               ROSE_ASSERT(copyOfFirstNondefiningDeclarationNode->get_parent() == NULL);

            // Must reset the parent (semantics of AST copy), but this will be done by reset
               ROSE_ASSERT(this->get_firstNondefiningDeclaration()->get_parent() != NULL);
               copyOfFirstNondefiningDeclarationNode->set_parent(this->get_firstNondefiningDeclaration()->get_parent());

               this->get_firstNondefiningDeclaration()->fixupCopy(copyOfFirstNondefiningDeclarationNode,help);

            // copyOfFirstNondefiningDeclaration = isSgDeclarationStatement(copyOfFirstNondefiningDeclarationNode);
             }
            else
             {
            // Note: This needs to be in the else case to handle the recursion properly (else this case would be procesed twice)
               if (this->get_definingDeclaration() != NULL && definingDeclarationCopied == false)
                  {
                 // Setup the defining declaration 
                    ROSE_ASSERT(firstNondefiningDeclarationCopied == true);

                 // DQ (10/21/2007): Use the copy help object so that it can control copying of defining vs. non-defining declaration.
                 // SgNode* copyOfDefiningDeclarationNode = this->get_definingDeclaration()->copy(help);
                    SgNode* copyOfDefiningDeclarationNode = help.copyAst(this->get_definingDeclaration());

                    ROSE_ASSERT(copyOfDefiningDeclarationNode != NULL);

                 // If we didn't make a copy of the definingDeclaration then this is still a valid pointer, so there is no need to reset the parent or call 
                    if (copyOfDefiningDeclarationNode != this->get_definingDeclaration())
                       {
                         ROSE_ASSERT(copyOfDefiningDeclarationNode->get_parent() == NULL);

                      // Must reset the parent (semantics of AST copy), but this will be done by reset
                         ROSE_ASSERT(this->get_definingDeclaration()->get_parent() != NULL);
                         copyOfDefiningDeclarationNode->set_parent(this->get_definingDeclaration()->get_parent());

                      // DQ (10/21/2007): I think this was a bug!
                      // this->get_firstNondefiningDeclaration()->fixupCopy(copyOfDefiningDeclarationNode,help);
                         this->get_definingDeclaration()->fixupCopy(copyOfDefiningDeclarationNode,help);
                       }

                 // copyOfDefiningDeclaration = isSgDeclarationStatement(copyOfDefiningDeclarationNode);
                  }
                 else
                  {
                 // This is an acceptable case, but only if:
                    ROSE_ASSERT(this->get_definingDeclaration() == NULL || this->get_firstNondefiningDeclaration() == NULL);
                  }
             }
        }

  // If this is a declaration which is a defining declaration, then the copy should be as well.
     if (this->get_definingDeclaration() == this)
        {
       // printf ("This is a DEFINING declaration this %p = %s copyDeclarationStatement = %p = %s \n",this,this->class_name().c_str(),copyDeclarationStatement,copyDeclarationStatement->class_name().c_str());
          copyDeclarationStatement->set_definingDeclaration(copyDeclarationStatement);

       // If this is the defining declaration the we can reset the defining declaration on the firstNondefiningDeclaration (if it has been updated)
          if (this->get_firstNondefiningDeclaration() != copyDeclarationStatement->get_firstNondefiningDeclaration())
             {
            // printf ("This is the defining declaration, so we can reset the defining declaration on the firstNondefiningDeclaration \n");
               FixupCopyDataMemberMacro(copyDeclarationStatement->get_firstNondefiningDeclaration(),SgDeclarationStatement,get_definingDeclaration,set_definingDeclaration)
             }
        }
       else
        {
       // DQ (10/19/2007): If there was a defining declaration then the copy's defining declaration using the map
       // printf ("NOT a DEFINING declaration: this->get_definingDeclaration() = %p \n",this->get_definingDeclaration());
          if (this->get_definingDeclaration() != NULL)
             {
               ROSE_ASSERT(copyDeclarationStatement->get_definingDeclaration() != NULL);

               FixupCopyDataMemberMacro(copyDeclarationStatement,SgDeclarationStatement,get_definingDeclaration,set_definingDeclaration)
#if 0
               printf ("On the firstNondefiningDeclaration %p setting the definingDeclaration to %p \n",
                    copyDeclarationStatement->get_firstNondefiningDeclaration(),copyDeclarationStatement->get_definingDeclaration());
               copyDeclarationStatement->get_firstNondefiningDeclaration()->set_definingDeclaration(copyDeclarationStatement->get_definingDeclaration());
#endif
             }
            else
             {
            // printf ("In SgDeclarationStatement::fixupCopy(): this->get_definingDeclaration() == NULL \n");
             }
        }

  // DQ (10/12/2007): It is not always clear if this is a great idea.  This uncovered a bug in the 
  // SageInterface::isOverloaded() function. Having two declarations marked as the firstNondefiningDeclaration
  // could be a problem at some point.  But for now this preserves the concept of an exact copy, so I am 
  // leaving it as is.

  // If this is a declaration which is a nondefining declaration, then the copy should be as well.
     if (this->get_firstNondefiningDeclaration() == this)
        {
       // printf ("This is the FIRST-NON-DEFINING declaration this %p = %s copyDeclarationStatement = %p = %s \n",this,this->class_name().c_str(),copyDeclarationStatement,copyDeclarationStatement->class_name().c_str());
          copyDeclarationStatement->set_firstNondefiningDeclaration(copyDeclarationStatement);

       // If this is the firstNondefining declaration the we can reset the firstNondefiningDeclaration on the definingDeclaration (if it has been updated)
          if (this->get_definingDeclaration() != copyDeclarationStatement->get_definingDeclaration())
             {
            // printf ("This is the firstNondefining declaration, so  we can reset the firstNondefiningDeclaration on the definingDeclaration \n");
               FixupCopyDataMemberMacro(copyDeclarationStatement->get_definingDeclaration(),SgDeclarationStatement,get_firstNondefiningDeclaration,set_firstNondefiningDeclaration)
             }
        }
       else
        {
       // DQ (10/19/2007): If there was a defining declaration then the copy's defining declaration using the map
       // printf ("NOT a FIRST_NON_DEFINING declaration: this->get_firstNondefiningDeclaration() = %p \n",this->get_firstNondefiningDeclaration());
          if (this->get_firstNondefiningDeclaration() != NULL)
             {
               ROSE_ASSERT(copyDeclarationStatement->get_firstNondefiningDeclaration() != NULL);
               FixupCopyDataMemberMacro(copyDeclarationStatement,SgDeclarationStatement,get_firstNondefiningDeclaration,set_firstNondefiningDeclaration)
             }
            else
             {
            // printf ("In SgDeclarationStatement::fixupCopy(): this->get_firstNondefiningDeclaration() == NULL \n");
             }
        }
     

  // DQ (10/16/2007): Added assertion (see copytest2007_17.C)
     if (this->get_definingDeclaration() != NULL && this->get_firstNondefiningDeclaration() != NULL)
        {
       // printf ("this->get_definingDeclaration()         = %p = %s \n",this->get_definingDeclaration(),this->get_definingDeclaration()->class_name().c_str());
       // printf ("this->get_firstNondefiningDeclaration() = %p = %s \n",this->get_firstNondefiningDeclaration(),this->get_firstNondefiningDeclaration()->class_name().c_str());
          ROSE_ASSERT(this->get_definingDeclaration()->variantT() == this->get_firstNondefiningDeclaration()->variantT());
        }

  // DQ (10/16/2007): Added assertion (see copytest2007_17.C)
     if (copyDeclarationStatement->get_definingDeclaration() != NULL && copyDeclarationStatement->get_firstNondefiningDeclaration() != NULL)
        {
       // printf ("copyDeclarationStatement->get_definingDeclaration()         = %p = %s \n",copyDeclarationStatement->get_definingDeclaration(),copyDeclarationStatement->get_definingDeclaration()->class_name().c_str());
       // printf ("copyDeclarationStatement->get_firstNondefiningDeclaration() = %p = %s \n",copyDeclarationStatement->get_firstNondefiningDeclaration(),copyDeclarationStatement->get_firstNondefiningDeclaration()->class_name().c_str());
          ROSE_ASSERT(copyDeclarationStatement->get_definingDeclaration()->variantT() == copyDeclarationStatement->get_firstNondefiningDeclaration()->variantT());
        }

  // DQ (10/12/2007): Set the scope for those SgDeclarationStatements which store their scope explicitly.
  // printf ("this->hasExplicitScope() = %s \n",this->hasExplicitScope() ? "true" : "false");
     if (this->hasExplicitScope() == true)
        {
       // printf ("Reset the scope of the copy for this = %p = %s \n",this,this->class_name().c_str());
          FixupCopyDataMemberMacro(copyDeclarationStatement,SgScopeStatement,get_scope,set_scope)

#if 0
       // DQ (10/21/2007): This can be OK for where a function prototype is being generated from a defining function declaration.

       // ROSE_ASSERT(copyDeclarationStatement->get_scope() != this->get_scope());
          if (copyDeclarationStatement->get_scope() == this->get_scope())
             {
               printf ("##########  ERROR: SCOPES NOT SET CORRECTLY! (%p = %s)  ########## \n",copy,copy->class_name().c_str());
             }
#endif

       // Make sure that the copy sets the scopes to be the same type
          ROSE_ASSERT(copyDeclarationStatement->get_scope()->variantT() == this->get_scope()->variantT());
        }

  // DQ (10/19/2007): Added test...
     if (this->get_definingDeclaration() != NULL && this->get_firstNondefiningDeclaration() != NULL)
        {
       // DQ (11/6/2007): If these are in the same namespace but different in different instances of the namespace 
       // definition (SgNamespaceDefinitionStatement objects), then the test for the same scope is more complex.
          SgNamespaceDefinitionStatement* definingNamespace         = isSgNamespaceDefinitionStatement(this->get_definingDeclaration()->get_scope());
          SgNamespaceDefinitionStatement* firstNondefiningNamespace = isSgNamespaceDefinitionStatement(this->get_firstNondefiningDeclaration()->get_scope());
          if (definingNamespace != NULL && firstNondefiningNamespace != NULL)
             {
            // printf ("Test for same namespace is more complex: definingNamespace = %p firstNondefiningNamespace = %p \n",definingNamespace,firstNondefiningNamespace);
               SgNamespaceDeclarationStatement* definingNamespaceDeclaration         = definingNamespace->get_namespaceDeclaration();
               SgNamespaceDeclarationStatement* firstNondefiningNamespaceDeclaration = firstNondefiningNamespace->get_namespaceDeclaration();
               ROSE_ASSERT(definingNamespaceDeclaration != NULL);
               ROSE_ASSERT(firstNondefiningNamespaceDeclaration != NULL);
               ROSE_ASSERT(definingNamespaceDeclaration->get_firstNondefiningDeclaration() == firstNondefiningNamespaceDeclaration->get_firstNondefiningDeclaration());
             }
            else
             {
               if (this->get_definingDeclaration()->get_scope() != this->get_firstNondefiningDeclaration()->get_scope() )
                  {
                    printf ("Error in matching scopes: this = %p = %s = %s \n",this,this->class_name().c_str(),SageInterface::get_name(this).c_str());
                    printf ("     this->get_definingDeclaration()         = %p \n",this->get_definingDeclaration());
                    printf ("     this->get_firstNondefiningDeclaration() = %p \n",this->get_firstNondefiningDeclaration());
                    printf ("     this->get_definingDeclaration()->get_scope()         = %p = %s \n",this->get_definingDeclaration()->get_scope(),this->get_definingDeclaration()->get_scope()->class_name().c_str());
                    printf ("     this->get_firstNondefiningDeclaration()->get_scope() = %p = %s \n",this->get_firstNondefiningDeclaration()->get_scope(),this->get_firstNondefiningDeclaration()->get_scope()->class_name().c_str());
                 // this->get_startOfConstruct()->display("Error: this scope mismatch: debug");
                 // this->get_definingDeclaration()->get_startOfConstruct()->display("Error: definingDeclaration scope mismatch: debug");
                 // this->get_firstNondefiningDeclaration()->get_startOfConstruct()->display("Error: firstNondefiningDeclaration scope mismatch: debug");
                  }

               ROSE_ASSERT(this->get_definingDeclaration()->get_scope() == this->get_firstNondefiningDeclaration()->get_scope() );
             }

       // ROSE_ASSERT(this->get_definingDeclaration()->get_scope() == this->get_firstNondefiningDeclaration()->get_scope() );
        }

  // DQ (10/19/2007): Added test...
     if (copyDeclarationStatement->get_definingDeclaration() != NULL && copyDeclarationStatement->get_firstNondefiningDeclaration() != NULL)
        {
       // DQ (10/19/2007): Check the loop (not passible until both have been processed)
          ROSE_ASSERT(copyDeclarationStatement->get_firstNondefiningDeclaration()->get_definingDeclaration() != NULL);
          ROSE_ASSERT(copyDeclarationStatement->get_definingDeclaration()->get_firstNondefiningDeclaration() != NULL);

       // ROSE_ASSERT(copyDeclarationStatement->get_firstNondefiningDeclaration()->get_definingDeclaration() == copyDeclarationStatement->get_definingDeclaration());

       // We can't assert this yet since this is part of the copy of the defining declaration within the processing of the non-defining declaration (recurssively called!)
       // ROSE_ASSERT(copyDeclarationStatement->get_definingDeclaration()->get_firstNondefiningDeclaration() == copyDeclarationStatement->get_firstNondefiningDeclaration());

       // If we can get to this point then we can perform this additional test
       // ROSE_ASSERT(copyDeclarationStatement->get_definingDeclaration()->get_scope() == copyDeclarationStatement->get_firstNondefiningDeclaration()->get_scope() );
        }

  // Call the base class fixupCopy member function (this will setup the parent)
     SgStatement::fixupCopy(copy,help);
   }


void
SgFunctionDeclaration::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgFunctionDeclaration* functionDeclaration_copy = isSgFunctionDeclaration(copy);
     ROSE_ASSERT(functionDeclaration_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("\nIn SgFunctionDeclaration::fixupCopy(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

  // printf ("\nIn SgFunctionDeclaration::fixupCopy(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy(copy,help);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_parameterList() != NULL);
     get_parameterList()->fixupCopy(functionDeclaration_copy->get_parameterList(),help);

  // Setup the details in the SgFunctionDefinition (this may have to rebuild the sysmbol table)
  // printf ("In SgFunctionDeclaration::fixupCopy(): this->get_definition() = %p \n",this->get_definition());
     if (this->get_definition() != NULL)
        {
          ROSE_ASSERT(isForward() == false);
          this->get_definition()->fixupCopy(functionDeclaration_copy->get_definition(),help);

       // If this is a declaration with a definition then it is a defining declaration
       // functionDeclaration_copy->set_definingDeclaration(functionDeclaration_copy);
        }

  // printf ("\nLeaving SgFunctionDeclaration::fixupCopy(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
   }

// DQ (10/6/2007): Added fixup function to set scopes not set properly by the ROSETTA generated copy!
void
SgFunctionParameterList::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFunctionParameterList::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy(copy,help);

     SgFunctionParameterList* copyFunctionParameterList = isSgFunctionParameterList(copy);
     ROSE_ASSERT(copyFunctionParameterList != NULL);

     const SgInitializedNamePtrList & parameterList_original = this->get_args();
     SgInitializedNamePtrList & parameterList_copy           = copyFunctionParameterList->get_args();

     SgInitializedNamePtrList::const_iterator i_original = parameterList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = parameterList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != parameterList_original.end()) && (i_copy != parameterList_copy.end()) )
        {
          (*i_original)->fixupCopy(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }


void
SgMemberFunctionDeclaration::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgMemberFunctionDeclaration* memberFunctionDeclaration_copy = isSgMemberFunctionDeclaration(copy);
     ROSE_ASSERT(memberFunctionDeclaration_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("In SgMemberFunctionDeclaration::fixupCopy(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
  // SgFunctionDeclaration::fixupCopy(copy,help);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_CtorInitializerList() != NULL);
     get_CtorInitializerList()->fixupCopy(memberFunctionDeclaration_copy->get_CtorInitializerList(),help);

  // Call the base class fixupCopy member function
     SgFunctionDeclaration::fixupCopy(copy,help);
   }


void
SgTemplateDeclaration::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("\nInside of SgTemplateDeclaration::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgTemplateDeclaration* templateDeclaration_copy = isSgTemplateDeclaration(copy);
     ROSE_ASSERT(templateDeclaration_copy != NULL);

#if 0
  // DQ (11/6/2007): This should usually be false (with a default SgCopyHelp).
     ROSE_ASSERT(this != copy);
#endif

#if 0
  // DQ (11/6/2007): Make sure that the "this" is a part of the original AST 
  // and that the "copy" is not a part of the original AST
     ROSE_ASSERT(help.get_copiedNodeMap().find(this) != help.get_copiedNodeMap().end());
     ROSE_ASSERT(help.get_copiedNodeMap().find(copy) == help.get_copiedNodeMap().end());

     printf ("SgTemplateDeclaration::fixupCopy(): this->get_definingDeclaration() = %p this->get_firstNondefiningDeclaration() = %p \n",
          this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
     printf ("SgTemplateDeclaration::fixupCopy(): copy->get_definingDeclaration() = %p copy->get_firstNondefiningDeclaration() = %p \n",
          templateDeclaration_copy->get_definingDeclaration(),templateDeclaration_copy->get_firstNondefiningDeclaration());
#endif
  // DQ (10/19/2007): Need to call this first and then setup parent of defining declaration using that already 
  // setup for the non-defining declaration because the copy function will return a copy with a NULL parent 
  // (semantics of AST copy mechanism).

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy(copy,help);

#if 1
  // I don't think that this applies to the defining declaration...
     if (this != this->get_definingDeclaration())
        {
          ROSE_ASSERT(templateDeclaration_copy->get_firstNondefiningDeclaration()->get_definingDeclaration() == templateDeclaration_copy->get_definingDeclaration());

       // DQ (10/20/2007): this is a problem with copytest2007_12.C, I think it should not be assered yet.
       // We can't assert this yet since this is part fo the copy of the defining declaration within the processing of the non-defining declaration (recurssively called!)
       // ROSE_ASSERT(templateDeclaration_copy->get_definingDeclaration()->get_firstNondefiningDeclaration() == templateDeclaration_copy->get_firstNondefiningDeclaration());

       // If we can get to this point then we can perform this additional test
       // ROSE_ASSERT(templateDeclaration_copy->get_definingDeclaration()->get_scope() == templateDeclaration_copy->get_firstNondefiningDeclaration()->get_scope() );
        }
#endif
   }


void
SgTemplateInstantiationDefn::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

     ROSE_ASSERT(this->get_declaration() != NULL);

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTemplateInstantiationDefn::fixupCopy() for class = %s class definition %p = %s copy = %p \n",this->get_declaration()->get_name().str(),this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgClassDefinition::fixupCopy(copy,help);
   }


void
SgTemplateInstantiationDecl::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgTemplateInstantiationDecl* templateClassDeclaration_copy = isSgTemplateInstantiationDecl(copy);
     ROSE_ASSERT(templateClassDeclaration_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationDecl::fixupCopy(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif
#if DEBUG_FIXUP_COPY_OUTPUT_MAP
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgClassDeclaration::fixupCopy(copy,help);

#if 0
     SgTemplateDeclaration* templateDeclaration_original = this->get_templateDeclaration();
     SgTemplateDeclaration* templateDeclaration_copy     = templateClassDeclaration_copy->get_templateDeclaration();

  // DQ (11/5/2007): Call fixupCopy on the template declaration.
     templateDeclaration_original->fixupCopy(templateDeclaration_copy,help);
#else
     FixupCopyDataMemberMacro(templateClassDeclaration_copy,SgTemplateDeclaration,get_templateDeclaration,set_templateDeclaration)
#endif

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateClassDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateClassDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        }
#endif
   }

void
SgTemplateInstantiationMemberFunctionDecl::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgTemplateInstantiationMemberFunctionDecl* templateMemberFunctionDeclaration_copy = isSgTemplateInstantiationMemberFunctionDecl(copy);
     ROSE_ASSERT(templateMemberFunctionDeclaration_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationMemberFunctionDecl::fixupCopy(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif
#if DEBUG_FIXUP_COPY_OUTPUT_MAP
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgMemberFunctionDeclaration::fixupCopy(copy,help);

#if 0
     SgTemplateDeclaration* templateDeclaration_original = this->get_templateDeclaration();
     SgTemplateDeclaration* templateDeclaration_copy     = templateMemberFunctionDeclaration_copy->get_templateDeclaration();

  // DQ (11/5/2007): Call fixupCopy on the template declaration.
     templateDeclaration_original->fixupCopy(templateDeclaration_copy,help);
#else
     FixupCopyDataMemberMacro(templateMemberFunctionDeclaration_copy,SgTemplateDeclaration,get_templateDeclaration,set_templateDeclaration)
#endif

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateMemberFunctionDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateMemberFunctionDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        }
#endif
   }


void
SgTemplateInstantiationFunctionDecl::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration_copy = isSgTemplateInstantiationFunctionDecl(copy);
     ROSE_ASSERT(templateFunctionDeclaration_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateInstantiationFunctionDecl::fixupCopy(): for function = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif
#if DEBUG_FIXUP_COPY_OUTPUT_MAP
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgFunctionDeclaration::fixupCopy(copy,help);

#if 0
     SgTemplateDeclaration* templateDeclaration_original = this->get_templateDeclaration();
     SgTemplateDeclaration* templateDeclaration_copy     = templateFunctionDeclaration_copy->get_templateDeclaration();

  // DQ (11/5/2007): Call fixupCopy on the template declaration.
     templateDeclaration_original->fixupCopy(templateDeclaration_copy,help);
#else
     FixupCopyDataMemberMacro(templateFunctionDeclaration_copy,SgTemplateDeclaration,get_templateDeclaration,set_templateDeclaration)
#endif

#if 0
  // DQ (11/6/2007): I have returned to sharing SgTemplateArgument objects, fix this up later.

  // DQ (11/5/2007): Until we use the SgTemplateArgumentList object directly, we have to handl the list explicitly.
     SgTemplateArgumentPtrList::const_iterator templateArguments_iterator_original = this->get_templateArguments()->begin();
     SgTemplateArgumentPtrList::iterator       templateArguments_iterator_copy     = templateFunctionDeclaration_copy->get_templateArguments()->begin();
     while ( templateArguments_iterator_original != this->get_templateArguments()->end() )
        {
          ROSE_ASSERT(templateArguments_iterator_original != this->get_templateArguments()->end());
          ROSE_ASSERT(templateArguments_iterator_copy     != templateFunctionDeclaration_copy->get_templateArguments()->end());

          (*templateArguments_iterator_original)->fixupCopy(*templateArguments_iterator_copy,help);

          templateArguments_iterator_original++;
          templateArguments_iterator_copy++;
        } 
#endif
   }


void
SgFunctionDefinition::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgFunctionDefinition* functionDefinition_copy = isSgFunctionDefinition(copy);
     ROSE_ASSERT(functionDefinition_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFunctionDefinition::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // printf ("\nInside of SgFunctionDefinition::fixupCopy() for %p = %s copy = %p \n\n",this,this->class_name().c_str(),copy);

  // Setup the scopes of the SgInitializedName objects in the paraleter list
     ROSE_ASSERT(get_body() != NULL);
     get_body()->fixupCopy(functionDefinition_copy->get_body(),help);


#if 0
  // DQ (10/23/2007): This is now done in the SageInterface::rebuildSymbolTable() function. This function
  // is also now called in the SgScopeStatment::fixupCopy() function (base class of this SgFunctionDefinition).
  // So we don't have to call this here since it will be handled in SageInterface::rebuildSymbolTable().

  // Do we need to re-setup the symbol table???

  // DQ (10/8/2007): We don't have to re-setup the symbol table, but we do have to add the function declaration's parameters to it.
     SgFunctionDeclaration* functionDeclaration = functionDefinition_copy->get_declaration();
  // printf ("In SgFunctionDefinition::fixupCopy(): functionDefinition = %p functionDeclaration = %p \n",this,functionDeclaration);
     if (functionDeclaration != NULL)
        {
          SgSymbolTable* symbolTable = functionDefinition_copy->get_symbol_table();

          SgInitializedNamePtrList & argumentList = functionDeclaration->get_args();
          SgInitializedNamePtrList::iterator i = argumentList.begin();
          while (i != argumentList.end())
             {
               SgInitializedName* variable = *i;
               ROSE_ASSERT(variable != NULL);
               SgName name = variable->get_name();

            // If the symbol exists, as a variable, then don't add it.
               if (symbolTable->find_variable(name) == NULL)
                  {
                    SgSymbol* symbol = new SgVariableSymbol(variable);
                    ROSE_ASSERT(symbol != NULL);
                    symbolTable->insert(name,symbol);
                  }

               i++;
             }
        }
#endif

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy(copy,help);

  // printf ("\nLeaving SgFunctionDefinition::fixupCopy() for %p = %s copy = %p \n\n",this,this->class_name().c_str(),copy);
   }

void
SgVariableDeclaration::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgVariableDeclaration::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Also call the base class version of the fixupCopycopy() member function
     SgDeclarationStatement::fixupCopy(copy,help);

     SgVariableDeclaration* variableDeclaration_copy = isSgVariableDeclaration(copy);
     ROSE_ASSERT(variableDeclaration_copy != NULL);

  // DQ (10/14/2007): Handle the case of a type defined in the base type of the typedef (similar problem for SgVariableDeclaration).
  // printf ("this = %p this->get_variableDeclarationContainsBaseTypeDefiningDeclaration() = %s \n",this,this->get_variableDeclarationContainsBaseTypeDefiningDeclaration() ? "true" : "false");
     if (this->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true)
        {
          ROSE_ASSERT(variableDeclaration_copy->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true);
          SgDeclarationStatement* baseTypeDeclaration_original = this->get_baseTypeDefiningDeclaration();
          SgDeclarationStatement* baseTypeDeclaration_copy     = variableDeclaration_copy->get_baseTypeDefiningDeclaration();

       // printf ("baseTypeDeclaration_original = %p = %s \n",baseTypeDeclaration_original,baseTypeDeclaration_original->class_name().c_str());
       // printf ("baseTypeDeclaration_copy = %p = %s \n",baseTypeDeclaration_copy,baseTypeDeclaration_copy->class_name().c_str());

       // DQ (10/17/2007): This is now fixed!
       // I think that calling get_baseTypeDefiningDeclaration() is a problem because it calls compute_baseTypeDefiningDeclaration() 
       // which uses the symbol table and other infor which is not setup correctly.
       // printf ("Need to compute the baseTypeDeclaration_copy better (perhaps we shoul look into the map of copies? \n");
       // ROSE_ASSERT(false);

          ROSE_ASSERT(baseTypeDeclaration_original != NULL);
          ROSE_ASSERT(baseTypeDeclaration_copy != NULL);

       // printf ("In SgVariableDeclaration::fixupCopy(): Calling fixupCopy on %p = %s \n",baseTypeDeclaration_original,baseTypeDeclaration_original->class_name().c_str());

          baseTypeDeclaration_original->fixupCopy(baseTypeDeclaration_copy,help);
        }

     const SgInitializedNamePtrList & variableList_original = this->get_variables();
     SgInitializedNamePtrList & variableList_copy           = variableDeclaration_copy->get_variables();

  // printf ("Inside of SgVariableDeclaration::fixupCopy(): variableList_original.size() = %ld \n",(long)variableList_original.size());

     ROSE_ASSERT(variableList_original.size() == variableList_copy.size());

     SgInitializedNamePtrList::const_iterator i_original = variableList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = variableList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != variableList_original.end()) && (i_copy != variableList_copy.end()) )
        {
       // printf ("Looping over the initialized names in the variable declaration variable = %p = %s \n",(*i_copy),(*i_copy)->get_name().str());

          (*i_original)->fixupCopy(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }

void
SgClassDeclaration::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgVariableDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgClassDeclaration::fixupCopy() for class = %s = %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this->get_name().str(),this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

     SgClassDeclaration* classDeclaration_copy = isSgClassDeclaration(copy);
     ROSE_ASSERT(classDeclaration_copy != NULL);

     SgClassDefinition* classDefinition_original = this->get_definition();
     SgClassDefinition* classDefinition_copy     = classDeclaration_copy->get_definition();

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy(copy,help);

     if (isForward() == false)
        {
          classDefinition_original->fixupCopy(classDefinition_copy,help);
        }
   }

void
SgClassDefinition::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

     ROSE_ASSERT(this->get_declaration() != NULL);

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgClassDefinition::fixupCopy() for class = %s class definition %p = %s copy = %p \n",this->get_declaration()->get_name().str(),this,this->class_name().c_str(),copy);
#endif

     SgClassDefinition* classDefinition_copy = isSgClassDefinition(copy);
     ROSE_ASSERT(classDefinition_copy != NULL);

     SgBaseClassPtrList::const_iterator i_original = this->get_inheritances().begin();
     SgBaseClassPtrList::iterator i_copy           = classDefinition_copy->get_inheritances().begin();
     ROSE_ASSERT(this->get_inheritances().size() == classDefinition_copy->get_inheritances().size());

     while ( (i_original != this->get_inheritances().end()) && (i_copy != classDefinition_copy->get_inheritances().end()) )
        {
       // Check the parent pointer to make sure it is properly set
          ROSE_ASSERT( (*i_original)->get_parent() != NULL);
          ROSE_ASSERT( (*i_original)->get_parent() == this);

          (*i_original)->fixupCopy(*i_copy,help);

          (*i_copy)->set_parent(classDefinition_copy);

          ROSE_ASSERT( (*i_copy)->get_parent() != NULL);
          ROSE_ASSERT( (*i_copy)->get_parent() == classDefinition_copy);

          i_original++;
          i_copy++;
        }

     const SgDeclarationStatementPtrList & statementList_original = this->getDeclarationList();
     const SgDeclarationStatementPtrList & statementList_copy     = classDefinition_copy->getDeclarationList();

     SgDeclarationStatementPtrList::const_iterator j_original = statementList_original.begin();
     SgDeclarationStatementPtrList::const_iterator j_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (j_original != statementList_original.end()) && (j_copy != statementList_copy.end()) )
        {
          (*j_original)->fixupCopy(*j_copy,help);

          j_original++;
          j_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy(copy,help);

  // printf ("\nLeaving SgClassDefinition::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }


void
SgBaseClass::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgBaseClass::fixupCopy() for baseclass = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgBaseClass* baseClass_copy = isSgBaseClass(copy);
     ROSE_ASSERT(baseClass_copy != NULL);

     ROSE_ASSERT(this->get_base_class() != NULL);
     this->get_base_class()->fixupCopy(baseClass_copy->get_base_class(),help);
   }


void
SgLabelStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgLabelStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgLabelStatement* labelStatement_copy = isSgLabelStatement(copy);
     ROSE_ASSERT(labelStatement_copy != NULL);

  // I don't think there is anything to do here since we already make sure that there is a new SgLabelSymbol
  // and it is the SgGotoStatement that has to have it's reference to the label fixed up if the label and the
  // goto statement have been copied.

  // DQ (10/25/2007): Added handling for the new explicit scope in SgLabelStatement
     ROSE_ASSERT(this->hasExplicitScope() == true);

     FixupCopyDataMemberMacro(labelStatement_copy,SgScopeStatement,get_scope,set_scope)

  // Make sure that the copy sets the scopes to be the same type
     ROSE_ASSERT(labelStatement_copy->get_scope()->variantT() == this->get_scope()->variantT());

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy(copy,help);
   }

void
SgGotoStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
#if DEBUG_FIXUP_COPY
     printf ("Inside of SgGotoStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgGotoStatement* gotoStatement_copy = isSgGotoStatement(copy);
     ROSE_ASSERT(gotoStatement_copy != NULL);

     SgLabelStatement* labelStatement_original = get_label();

     SgCopyHelp::copiedNodeMapTypeIterator i = help.get_copiedNodeMap().find(labelStatement_original);

  // printf ("Inside of SgGotoStatement::fixupCopy(): i != help.get_copiedNodeMap().end() = %s \n",(i != help.get_copiedNodeMap().end()) ? "true" : "false");

  // If the declaration is in the map then it is because we have copied it previously
  // and thus it should be updated to reflect the copied declaration.
     if (i != help.get_copiedNodeMap().end())
        {
          SgLabelStatement* labelStatement_copy = isSgLabelStatement(i->second);
          ROSE_ASSERT(labelStatement_copy != NULL);
          gotoStatement_copy->set_label(labelStatement_copy);
        }

  // Also call the base class version of the fixupCopycopy() member function
     SgStatement::fixupCopy(copy,help);
   }

void
SgTypedefDeclaration::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgTypedefDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTypedefDeclaration::fixupCopy() for typedef name = %s = %p = %s copy = %p \n",this->get_name().str(),this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy(copy,help);

     SgTypedefDeclaration* typedefDeclaration_copy = isSgTypedefDeclaration(copy);
     ROSE_ASSERT(typedefDeclaration_copy != NULL);

  // DQ (10/14/2007): Handle the case of a type defined in the base type of the typedef (similar problem for SgVariableDeclaration).
     if (this->get_typedefBaseTypeContainsDefiningDeclaration() == true)
        {
          ROSE_ASSERT(typedefDeclaration_copy->get_typedefBaseTypeContainsDefiningDeclaration() == true);
          SgDeclarationStatement* baseTypeDeclaration_original = this->get_baseTypeDefiningDeclaration();
          SgDeclarationStatement* baseTypeDeclaration_copy     = typedefDeclaration_copy->get_baseTypeDefiningDeclaration();
          ROSE_ASSERT(baseTypeDeclaration_original != NULL);
          ROSE_ASSERT(baseTypeDeclaration_copy != NULL);

          baseTypeDeclaration_original->fixupCopy(baseTypeDeclaration_copy,help);
        }

#if 0
  // DQ (10/20/2007): I don't think we need this now!

  // If this is a defining declaration then there should also be a nondefining declaration.
     if (this == this->get_definingDeclaration())
        {
       // This is the defining class declaration, we want to build a non-defiing declaration to go with this.
       // Let the copying of the defining declaration trigger the construction of a non-defining declaration.

          printf ("This is the defining declaration, so build a copy of the nondefining SgTypedefDeclaration declaration! \n");

       // DQ (10/17/2007): Get the SgClassType from the copy since it is a copy of the original SgClassType (and not the same).
          SgTypedefType* typedef_type_copy = typedefDeclaration_copy->get_type();
          ROSE_ASSERT(typedef_type_copy != NULL);

       // DQ (10/17/2007): Modified to use the copy of the SgType ...
       // SgTypedefDeclaration* firstNondefiningDeclaration_copy = new SgTypedefDeclaration(this->get_name(),this->get_base_type(),this->get_type(),NULL,NULL);
          SgTypedefDeclaration* firstNondefiningDeclaration_copy = new SgTypedefDeclaration(this->get_name(),this->get_base_type(),typedef_type_copy,NULL,NULL);

       // DQ (10/17/2007): Also have to reset the declaration used in the SgType to reference 
       // the copy instead of the original non-defining class declaration.
          typedef_type_copy->set_declaration(firstNondefiningDeclaration_copy);

       // Details that have to be set when building a SgClassDeclaration IR node.

          printf ("***** Setting the scope in the firstNondefiningDeclaration_copy = %p \n",firstNondefiningDeclaration_copy);
          ROSE_ASSERT(typedefDeclaration_copy->get_scope() != NULL);
          firstNondefiningDeclaration_copy->set_scope(typedefDeclaration_copy->get_scope());

          printf ("***** Setting the parent in the firstNondefiningDeclaration_copy = %p \n",firstNondefiningDeclaration_copy);
          ROSE_ASSERT(typedefDeclaration_copy->get_parent() != NULL);
          firstNondefiningDeclaration_copy->set_parent(typedefDeclaration_copy->get_parent());

          firstNondefiningDeclaration_copy->setForward();
          firstNondefiningDeclaration_copy->set_definingDeclaration(typedefDeclaration_copy);

          SgTypedefDeclaration* firstNondefiningDeclaration_original = isSgTypedefDeclaration(this->get_firstNondefiningDeclaration());

          firstNondefiningDeclaration_copy->set_startOfConstruct( new Sg_File_Info( *(firstNondefiningDeclaration_original->get_startOfConstruct()) ) );
          firstNondefiningDeclaration_copy->set_endOfConstruct( new Sg_File_Info( *(firstNondefiningDeclaration_original->get_endOfConstruct()) ) );

          firstNondefiningDeclaration_copy->get_startOfConstruct()->set_parent(firstNondefiningDeclaration_copy);
          firstNondefiningDeclaration_copy->get_endOfConstruct()->set_parent(firstNondefiningDeclaration_copy);

       // If the original firstNondefiningDeclaration points to itself as the firstNondefiningDeclaration then set the copy to point to itself as well.
          ROSE_ASSERT(firstNondefiningDeclaration_original != NULL);
          if (firstNondefiningDeclaration_original == firstNondefiningDeclaration_original->get_firstNondefiningDeclaration())
             {
               firstNondefiningDeclaration_copy->set_firstNondefiningDeclaration(firstNondefiningDeclaration_copy);
             }

          ROSE_ASSERT(firstNondefiningDeclaration_copy != NULL);
          typedefDeclaration_copy->set_firstNondefiningDeclaration(firstNondefiningDeclaration_copy);
        }
       else
        {
       // DQ (10/17/2007): fixup the type used to make sure it has the declaration set the AST copy.
          SgTypedefType* typedef_type_original = this->get_type();
          ROSE_ASSERT(typedef_type_original != NULL);

          SgTypedefType* typedef_type_copy = typedefDeclaration_copy->get_type();
          ROSE_ASSERT(typedef_type_copy != NULL);

          printf ("This is the non-defining declaration, so just fixup the SgTypedefType = %p with the correct SgTypedefDeclaration declaration! \n",typedef_type_copy);

       // FixupCopyDataMemberMacro_local_debug(typedef_type_copy,SgDeclarationStatement,get_declaration,set_declaration)
          SgCopyHelp::copiedNodeMapTypeIterator i = help.get_copiedNodeMap().find(typedef_type_original->get_declaration());
          printf ("SgCopyHelp::copiedNodeMapTypeIterator i != help.get_copiedNodeMap().end() = %s \n",i != help.get_copiedNodeMap().end() ? "true" : "false");
          if (i != help.get_copiedNodeMap().end())
             {
               SgNode* associated_node_copy = i->second;
               ROSE_ASSERT(associated_node_copy != NULL);
               SgDeclarationStatement* local_copy = isSgDeclarationStatement(associated_node_copy);
               ROSE_ASSERT(local_copy != NULL);
               printf ("Resetting using local_copy = %p = %s \n",local_copy,local_copy->class_name().c_str());
               typedef_type_copy->set_declaration(local_copy);
             }

       // Call fixup on the declaration when it is used in the typedef.
          SgDeclarationStatement* declaration_original = this->get_declaration();
          SgDeclarationStatement* declaration_copy     = typedefDeclaration_copy->get_declaration();

          if (declaration_original != NULL)
             {
               ROSE_ASSERT(declaration_original  != NULL);
               ROSE_ASSERT(declaration_copy      != NULL);

               printf ("Calling fixupCopy on declaration_original = %p = %s \n",declaration_original,declaration_original->class_name().c_str());
               declaration_original->fixupCopy(declaration_copy,help);
             }
            else
             {
               ROSE_ASSERT(declaration_copy == NULL);
             }
        }
#endif
   }


void
SgEnumDeclaration::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/18/2007): We have to call fixup on all the enum fields (to reset there scope!)

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgEnumDeclaration::fixupCopy() for %p = %s copy = %p (defining = %p firstNondefining = %p) \n",
          this,this->class_name().c_str(),copy,this->get_definingDeclaration(),this->get_firstNondefiningDeclaration());
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy(copy,help);

     SgEnumDeclaration* enumDeclaration_copy = isSgEnumDeclaration(copy);
     ROSE_ASSERT(enumDeclaration_copy != NULL);
     
  // DQ (10/17/2007): fixup the type used to make sure it has the declaration set the AST copy.
     SgEnumType* enum_type_original = this->get_type();
     ROSE_ASSERT(enum_type_original != NULL);

     SgEnumType* enum_type_copy = enumDeclaration_copy->get_type();
     ROSE_ASSERT(enum_type_copy != NULL);

  // printf ("This is the non-defining declaration, so just fixup the SgEnumType = %p with the correct SgEnumDeclaration declaration! \n",enum_type_copy);

  // FixupCopyDataMemberMacro_local_debug(enum_type_copy,SgDeclarationStatement,get_declaration,set_declaration)
     SgCopyHelp::copiedNodeMapTypeIterator i = help.get_copiedNodeMap().find(enum_type_original->get_declaration());
  // printf ("SgCopyHelp::copiedNodeMapTypeIterator i != help.get_copiedNodeMap().end() = %s \n",i != help.get_copiedNodeMap().end() ? "true" : "false");
     if (i != help.get_copiedNodeMap().end())
        {
          SgNode* associated_node_copy = i->second;
          ROSE_ASSERT(associated_node_copy != NULL);
          SgDeclarationStatement* local_copy = isSgDeclarationStatement(associated_node_copy);
          ROSE_ASSERT(local_copy != NULL);
       // printf ("Resetting using local_copy = %p = %s \n",local_copy,local_copy->class_name().c_str());
          enum_type_copy->set_declaration(local_copy);
        }

  // Now reset the enum fields.
     const SgInitializedNamePtrList & enumFieldList_original = this->get_enumerators();
     SgInitializedNamePtrList & enumFieldList_copy           = enumDeclaration_copy->get_enumerators();

     SgInitializedNamePtrList::const_iterator i_original = enumFieldList_original.begin();
     SgInitializedNamePtrList::iterator i_copy           = enumFieldList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgInitializedName objects
     while ( (i_original != enumFieldList_original.end()) && (i_copy != enumFieldList_copy.end()) )
        {
          (*i_original)->fixupCopy(*i_copy,help);

          i_original++;
          i_copy++;
        }
   }




void
SgNamespaceDeclarationStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgVariableDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgNamespaceDeclarationStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy(copy,help);

     SgNamespaceDeclarationStatement* namespaceDeclaration_copy = isSgNamespaceDeclarationStatement(copy);
     ROSE_ASSERT(namespaceDeclaration_copy != NULL);

     SgNamespaceDefinitionStatement* namespaceDefinition_original = this->get_definition();
     SgNamespaceDefinitionStatement* namespaceDefinition_copy     = namespaceDeclaration_copy->get_definition();

     ROSE_ASSERT(namespaceDefinition_original != NULL);
     ROSE_ASSERT(namespaceDefinition_copy != NULL);

     namespaceDefinition_original->fixupCopy(namespaceDefinition_copy,help);
   }


void
SgNamespaceDefinitionStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgNamespaceDefinitionStatement* namespaceDefinition_copy = isSgNamespaceDefinitionStatement(copy);
     ROSE_ASSERT(namespaceDefinition_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgNamespaceDefinitionStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     FixupCopyDataMemberMacro(namespaceDefinition_copy,SgNamespaceDeclarationStatement,get_namespaceDeclaration,set_namespaceDeclaration)

     const SgDeclarationStatementPtrList & statementList_original = this->getDeclarationList();
     const SgDeclarationStatementPtrList & statementList_copy     = namespaceDefinition_copy->getDeclarationList();

     SgDeclarationStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgDeclarationStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
          (*i_original)->fixupCopy(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Call the base class fixupCopy member function
     SgScopeStatement::fixupCopy(copy,help);
   }


void
SgTemplateInstantiationDirectiveStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // We need to call the fixupCopy function from the parent of a SgVariableDeclaration because the 
  // copy function in the parent of the variable declaration sets the parent of the SgVariableDeclaration
  // and we need this parent in the fixupCopy function in the SgInitializedName.

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTemplateInstantiationDirectiveStatement::fixupCopy() for %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirectiveStatement_copy = isSgTemplateInstantiationDirectiveStatement(copy);

  // Call the base class fixupCopy member function
     SgDeclarationStatement::fixupCopy(copy,help);

     ROSE_ASSERT(this->get_declaration() != NULL);

#if 0
  // DQ (10/15/2007): This is a simpler approach using the map stored in the SgCopyHelp object.
     SgCopyHelp::copiedNodeMapTypeIterator i = help.get_copiedNodeMap().find(this->get_declaration());
     if (i != help.get_copiedNodeMap().end())
        {
       // Use the associated value from the map as the scope.
          SgNode* node_copy = i->second;
          ROSE_ASSERT(node_copy != NULL);
          SgDeclarationStatement* declarationStatement_copy = isSgDeclarationStatement(node_copy);
          templateInstantiationDirectiveStatement_copy->set_declaration(declarationStatement_copy);
        }
       else
        {
       // Else set the scope in the copyDeclarationStatement to be the same as this->get_scope()
       // printf ("In SgTemplateInstantiationDirectiveStatement::fixupCopy() this = %p = %s this->get_parent() = %p = %s \n",
       //      this,this->class_name().c_str(),this->get_declaration(),this->get_declaration()->class_name().c_str());
          templateInstantiationDirectiveStatement_copy->set_declaration(this->get_declaration());
        }
#else
     FixupCopyDataMemberMacro(templateInstantiationDirectiveStatement_copy,SgDeclarationStatement,get_declaration,set_declaration)
#endif
   }



void
SgProject::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // This is the empty default inplementation, not a problem if it is called!

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgProject::fixupCopy() \n");
#endif

     SgProject* project_copy = isSgProject(copy);
     ROSE_ASSERT(project_copy != NULL);

  // Call fixup on all fo the files (SgFile objects)
     for (int i = 0; i < numberOfFiles(); i++)
        {
          SgFile & file = get_file(i);
          SgFile & file_copy = project_copy->get_file(i);
          file.fixupCopy(&file_copy,help);
        }
   }

void
SgFile::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // This is the empty default inplementation, not a problem if it is called!

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgFile::fixupCopy() \n");
#endif

     SgFile* file_copy = isSgFile(copy);
     ROSE_ASSERT(file_copy != NULL);

  // Call fixup on the global scope
     ROSE_ASSERT(get_globalScope() != NULL);
     ROSE_ASSERT(file_copy->get_globalScope() != NULL);
     get_globalScope()->fixupCopy(file_copy->get_globalScope(),help);
   } 


void
SgIfStmt::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names
   
    ROSE_ASSERT (!"fixupCopy on if");

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgIfStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // printf ("Inside of SgIfStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);

  // SgStatement::fixupCopy(copy,help);
     SgScopeStatement::fixupCopy(copy,help);

     SgIfStmt* ifStatement_copy = isSgIfStmt(copy);
     ROSE_ASSERT(ifStatement_copy != NULL);

  // The symbol table should not have been setup yet!
  // ROSE_ASSERT(ifStatement_copy->get_symbol_table()->size() == 0);

     this->get_conditional()->fixupCopy(ifStatement_copy->get_conditional(),help);

#if 0
     ROSE_ASSERT(ifStatement_copy->get_true_body() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_true_body()->get_symbol_table() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_true_body()->get_symbol_table()->size()  == 0);

     ROSE_ASSERT(ifStatement_copy->get_false_body() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_false_body()->get_symbol_table() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_false_body()->get_symbol_table()->size()  == 0);

     const SgVariableDeclaration* variableDeclarationCondition = isSgVariableDeclaration(this->get_conditional());
     if (variableDeclarationCondition != NULL)
        {
       // There is a variable declaration in the conditional, it needs to be added to the symbol table.
          printf ("There is a variable declaration in the conditional, it needs to be added to the symbol table \n");
          ROSE_ASSERT(false);
        }
#endif

     ROSE_ASSERT(this->get_true_body() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_true_body() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_true_body()->get_symbol_table() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_true_body()->get_symbol_table()->size()  == 0);

  // printf ("\nProcess the TRUE body of the SgIfStmt \n\n");

     this->get_true_body()->fixupCopy(ifStatement_copy->get_true_body(),help);

     ROSE_ASSERT(this->get_false_body() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_false_body() != NULL);
     ROSE_ASSERT(ifStatement_copy->get_false_body()->get_symbol_table()->size() == 0);
     ROSE_ASSERT(ifStatement_copy->get_false_body()->get_symbol_table() != NULL);

  // printf ("\nProcess the FALSE body of the SgIfStmt \n\n");

     this->get_false_body()->fixupCopy(ifStatement_copy->get_false_body(),help);

  // printf ("\nLeaving SgIfStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
   }

void
SgForStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgForStatement::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

  // Use three files to organize the separate functions (parent/scope setup, symbol table setup, and symbol references).
  // Order of operations:
  //    1) Setup scopes on all declaration (e.g. SgInitializedName objects).
  //    2) Setup the symbol table.
  //        template instantiations must be added to to the symbol tables as defined by their scope
  //        because they may be located outside of their scope (indicated by their template declaration).
  //        We might need a test and set policy.
  //        Use the help map to support error checking in the symbol table construction.  Check that 
  //        scopes are not in the original AST (not keys in the help map).
  //    3) Setup the references (SgVarRefExp objects pointers to SgVariableSymbol objects)

     SgForStatement* forStatement_copy = isSgForStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

  // This could generate a vaiable declaration, so wait to build the sysmbol table.
     ROSE_ASSERT(this->get_for_init_stmt() != NULL);
     this->get_for_init_stmt()->fixupCopy(forStatement_copy->get_for_init_stmt(),help);

  // DQ (11/1/2007): Force the symbol table to be setup so that references can be made to it later.
  // If we built it too early then the scope (on The SgInitializedName objects) have not be setup, 
  // and if we build it too late then we don't have the symbols in place to reset the references.
  // printf ("Calling SgScopeStatement::fixupCopy() \n");
     SgScopeStatement::fixupCopy(copy,help);
  // printf ("DONE: SgScopeStatement::fixupCopy() \n");

  // This could generate a vaiable declaration, so wait to build the sysmbol table.
     ROSE_ASSERT(this->get_test() != NULL);
     this->get_test()->fixupCopy(forStatement_copy->get_test(),help);

  // If the test was a variable declaration then we have to update the scope (special case)!
  // ?????

     ROSE_ASSERT(this->get_increment() != NULL);
     this->get_increment()->fixupCopy(forStatement_copy->get_increment(),help);

     ROSE_ASSERT(this->get_loop_body() != NULL);
     this->get_loop_body()->fixupCopy(forStatement_copy->get_loop_body(),help);

#if 0
  // SgScopeStatement::fixupCopy(copy,help);
     printf ("Calling SgScopeStatement::fixupCopy() (2nd time) \n");
     SgScopeStatement::fixupCopy(copy,help);
     printf ("DONE: SgScopeStatement::fixupCopy() (2nd time) \n");
#endif
   }

void
SgForInitStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgForInitStatement::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy(copy,help);

     SgForInitStatement* forStatement_copy = isSgForInitStatement(copy);
     ROSE_ASSERT(forStatement_copy != NULL);

  // printf ("SgForInitStatement::fixupCopy(): Sorry not implemented \n");

     const SgStatementPtrList & statementList_original = this->get_init_stmt();
     const SgStatementPtrList & statementList_copy     = forStatement_copy->get_init_stmt();

     SgStatementPtrList::const_iterator i_original = statementList_original.begin();
     SgStatementPtrList::const_iterator i_copy     = statementList_copy.begin();

  // Iterate over both lists to match up the correct pairs of SgStatement objects
     while ( (i_original != statementList_original.end()) && (i_copy != statementList_copy.end()) )
        {
        // printf ("In SgForInitStatement::fixupCopy(): Calling fixup for *i_copy = %p = %s \n",(*i_copy),(*i_copy)->class_name().c_str());
          (*i_original)->fixupCopy(*i_copy,help);

          i_original++;
          i_copy++;
        }

  // Relavant data member is: SgStatementPtrList 	p_init_stmt
   }

void
SgCatchStatementSeq::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCatchStatementSeq::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy(copy,help);

     SgCatchStatementSeq* catchStatement_copy = isSgCatchStatementSeq(copy);
     ROSE_ASSERT(catchStatement_copy != NULL);

     printf ("SgCatchStatementSeq::fixupCopy(): Sorry not implemented \n");

  // The relavant data member here is a SgStatementPtrList 	p_catch_statement_seq

  // ROSE_ASSERT(this->get_body() != NULL);
  // this->get_body()->fixupCopy(catchStatement_copy->get_body(),help);
   }

void
SgWhileStmt::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgWhileStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy(copy,help);

     SgWhileStmt* whileStatement_copy = isSgWhileStmt(copy);
     ROSE_ASSERT(whileStatement_copy != NULL);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy(whileStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy(whileStatement_copy->get_body(),help);
   }

void
SgDoWhileStmt::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDoWhileStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy(copy,help);

     SgDoWhileStmt* doWhileStatement_copy = isSgDoWhileStmt(copy);
     ROSE_ASSERT(doWhileStatement_copy != NULL);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy(doWhileStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy(doWhileStatement_copy->get_body(),help);
   }


void
SgSwitchStatement::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgSwitchStatement::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgScopeStatement::fixupCopy(copy,help);

     SgSwitchStatement* switchStatement_copy = isSgSwitchStatement(copy);
     ROSE_ASSERT(switchStatement_copy != NULL);

     ROSE_ASSERT(this->get_item_selector() != NULL);
     this->get_item_selector()->fixupCopy(switchStatement_copy->get_item_selector(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy(switchStatement_copy->get_body(),help);
   }



void
SgTryStmt::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgTryStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy(copy,help);

     SgTryStmt* tryStatement_copy = isSgTryStmt(copy);
     ROSE_ASSERT(tryStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy(tryStatement_copy->get_body(),help);
   }

void
SgCatchOptionStmt::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCatchOptionStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     printf ("SgCatchOptionStmt::fixupCopy(): Sorry not implemented \n");

     SgScopeStatement::fixupCopy(copy,help);

     SgCatchOptionStmt* catchOptionStatement_copy = isSgCatchOptionStmt(copy);
     ROSE_ASSERT(catchOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_trystmt() != NULL);
  // I think this might cause endless recursion, so comment out for now!
  // this->get_trystmt()->fixupCopy(catchOptionStatement_copy->get_trystmt(),help);

     ROSE_ASSERT(this->get_condition() != NULL);
     this->get_condition()->fixupCopy(catchOptionStatement_copy->get_condition(),help);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy(catchOptionStatement_copy->get_body(),help);
   }

void
SgCaseOptionStmt::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgCaseOptionStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy(copy,help);

     SgCaseOptionStmt* caseOptionStatement_copy = isSgCaseOptionStmt(copy);
     ROSE_ASSERT(caseOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy(caseOptionStatement_copy->get_body(),help);
   }

void
SgDefaultOptionStmt::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
  // DQ (10/19/2007): Added support to fixup the base class names

#if DEBUG_FIXUP_COPY
     printf ("Inside of SgDefaultOptionStmt::fixupCopy() this = %p = %s  copy = %p \n",this,this->class_name().c_str(),copy);
#endif

     SgStatement::fixupCopy(copy,help);

     SgDefaultOptionStmt* defaultOptionStatement_copy = isSgDefaultOptionStmt(copy);
     ROSE_ASSERT(defaultOptionStatement_copy != NULL);

     ROSE_ASSERT(this->get_body() != NULL);
     this->get_body()->fixupCopy(defaultOptionStatement_copy->get_body(),help);
   }

void
SgTemplateArgument::fixupCopy(SgNode* copy, SgCopyHelp & help) const
   {
     SgTemplateArgument* templateArgument_copy = isSgTemplateArgument(copy);
     ROSE_ASSERT(templateArgument_copy != NULL);

#if DEBUG_FIXUP_COPY
     printf ("\nIn SgTemplateArgument::fixupCopy(): this = %p = %s copy = %p \n",this,this->class_name().c_str(),copy);
#endif
   }


