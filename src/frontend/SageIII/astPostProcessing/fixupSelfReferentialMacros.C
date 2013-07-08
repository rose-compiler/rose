#include "sage3basic.h"
#include "fixupSelfReferentialMacros.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"



/*
There are a collection of self referential macros in C, all 
related to signal handling. we have to detect variables of
this type and then undefine all of these. 

These are from sigaction.h (included by signal.h):
# define sa_handler     __sigaction_handler.sa_handler
# define sa_sigaction   __sigaction_handler.sa_sigaction

These are from siginfo.h (included by signal.h):
# define si_pid         _sifields._kill.si_pid
# define si_uid         _sifields._kill.si_uid
# define si_timerid     _sifields._timer.si_tid
# define si_overrun     _sifields._timer.si_overrun
# define si_status      _sifields._sigchld.si_status
# define si_utime       _sifields._sigchld.si_utime
# define si_stime       _sifields._sigchld.si_stime
# define si_value       _sifields._rt.si_sigval
# define si_int         _sifields._rt.si_sigval.sival_int
# define si_ptr         _sifields._rt.si_sigval.sival_ptr
# define si_addr        _sifields._sigfault.si_addr
# define si_band        _sifields._sigpoll.si_band
# define si_fd          _sifields._sigpoll.si_fd

*/

void fixupSelfReferentialMacrosInAST( SgNode* node )
   {
  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup known self-referential macros:");

     FixupSelfReferentialMacrosInAST astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

void
addMacro(SgStatement* associatedStatement, std::string macroString, PreprocessingInfo::DirectiveType directiveType)
   {
  // DQ (11/5/2012): Fixup for test2012_17.c and test2012_163.c
  // This function handled the details of adding a macro associated with the input string to the specfied SgStatement.

     std::string filenameString = "macro_call_fixupSelfReferentialMacrosInAST";
     int line_no = 1;
     int col_no  = 1;
     int nol     = 1;
     PreprocessingInfo::RelativePositionType relPos = PreprocessingInfo::before;

  // DQ (11/5/2012): the fix for test2012_163.c (sudo) requires another macro...
     PreprocessingInfo* macro = new PreprocessingInfo(directiveType,macroString,filenameString,line_no,col_no,nol,relPos);

  // printf ("Attaching CPP directive %s to IR node %p as attributes. \n",PreprocessingInfo::directiveTypeName(macro->getTypeOfDirective()).c_str(),associatedStatement);
     associatedStatement->addToAttachedPreprocessingInfo(macro);
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

                 // For sudo_exec_pty.c also look for siginfo
                    if (className == "sigaction" || className == "siginfo")
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

                           // We are puting out all macros anytime we see either type.  This might be too much...

                           // From the sigaction.h file (included by signal.h):
                              addMacro(associatedStatement,"#undef sa_handler\n",directiveType);
                              addMacro(associatedStatement,"#undef sa_sigaction\n",directiveType);

                           // From the siginfo.h file (included by signal.h):
                              addMacro(associatedStatement,"#undef si_pid\n",    directiveType);
                              addMacro(associatedStatement,"#undef si_uid\n",    directiveType);
                              addMacro(associatedStatement,"#undef si_timerid\n",directiveType);
                              addMacro(associatedStatement,"#undef si_overrun\n",directiveType);
                              addMacro(associatedStatement,"#undef si_status\n", directiveType);
                              addMacro(associatedStatement,"#undef si_utime\n",  directiveType);
                              addMacro(associatedStatement,"#undef si_stime\n",  directiveType);
                              addMacro(associatedStatement,"#undef si_value\n",  directiveType);
                              addMacro(associatedStatement,"#undef si_int\n",    directiveType);
                              addMacro(associatedStatement,"#undef si_ptr\n",    directiveType);
                              addMacro(associatedStatement,"#undef si_addr\n",   directiveType);
                              addMacro(associatedStatement,"#undef si_band\n",   directiveType);
                              addMacro(associatedStatement,"#undef si_fd\n",     directiveType);
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



#ifndef ROSE_USE_CLANG_FRONTEND

// The definition of this variable is only available to the EDG 4.x work.
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

#endif

