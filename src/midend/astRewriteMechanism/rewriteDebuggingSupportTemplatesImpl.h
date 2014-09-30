#ifndef AST_REWRITE_DEBUGGING_SUPPORT_TEMPLATES_C
#define AST_REWRITE_DEBUGGING_SUPPORT_TEMPLATES_C

// #include "rose.h"

#include "nameQuery.h"
#include "rewrite.h"

// **********************************************************
//      Display functions orginally from prefixGeneration.C
// **********************************************************

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::PrefixSuffixGenerationTraversal::
DeclarationOrCommentListElement::display( std::string s ) const
   {
     printf ("          Inside of DeclarationOrCommentListElement::display(%s) \n",s.c_str());

#if 1
     if (prefixStatement != NULL)
        {
       // printf("          prefixStatement = %p  prefixStatement = %s prefixStatement = %s \n",
       //      prefixStatement,prefixStatement->sage_class_name(),prefixStatement->unparseToString().c_str());
          printf("               prefixStatement = %p  prefixStatement (class name) = %s \n",
               prefixStatement,prefixStatement->sage_class_name());
          printf("                    prefixStatement (string) = %s \n",
               prefixStatement->unparseToString().c_str());
        }
       else
        {
          printf("               prefixStatement == NULL \n");
        }

     if (comments != NULL)
        {
       // output comments!
          printf ("               Found attached comments: \n");
          ROSE_ASSERT (originatingStatement != NULL);
          printf ("                    originatingStatement = %s \n",originatingStatement->sage_class_name());
          printf ("                    originatingStatement = %s \n",originatingStatement->unparseToString().c_str());
          typename AttachedPreprocessingInfoType::iterator i;
          for (i = comments->begin(); i != comments->end(); i++)
             {
               ROSE_ASSERT ( (*i) != NULL );
               printf ("                    Attached Comment (relativePosition=%s): %s",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",(*i)->getString().c_str());
             }
        }
       else
        {
          printf("               comments == NULL \n");
        }
#endif
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::PrefixSuffixGenerationTraversal::
display( std::string label ) const
   {
     printf ("Inside of PrefixSynthesizedAttribute::display(%s) \n",label.c_str());

     printf ("Current scope (currentScope.size() = %" PRIuPTR "): \n",currentScope.size());
     for (typename ListOfStatementsType::const_iterator i = currentScope.begin(); i != currentScope.end(); i++)
          (*i).display("Current Scope");

     printf ("Stack Of Scopes: \n");
#if 1
     display(stackOfScopes,label);
#else
     StackOfListsType tempStack = stackOfScopes;
     int counter = 0;
     while (tempStack.empty() == false)
        {
          ListOfStatementsType topOfStack = tempStack.top();

          char buffer[256];
          sprintf (buffer,"counter = %d topOfStack.size() = %" PRIuPTR "",counter,topOfStack.size());
          std::string displayString = buffer;
          printf ("     Display new scope: %s \n",displayString.c_str());

          typename ListOfStatementsType::iterator j;
          for (j = topOfStack.begin(); j != topOfStack.end(); j++)
               (*j).display(displayString);
          tempStack.pop();
          counter++;
        }
#endif
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::PrefixSuffixGenerationTraversal::
display ( const ListOfStatementsType & s, std::string label ) const
   {
     printf ("     In display(ListOfStatementsType,%s): s.size() = %" PRIuPTR " \n",label.c_str(),s.size());

#if 1
     typename ListOfStatementsType::const_iterator j;
     for (j = s.begin(); j != s.end(); j++)
          (*j).display(label);
#else
     for (typename ListOfStatementsType::const_iterator i = currentScope.begin(); i != currentScope.end(); i++)
        {
          (*i).display(label);
        }
#endif
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::PrefixSuffixGenerationTraversal::
display ( StackOfListsType tempStack, std::string label ) const
   {
  // This function takes a stack by value and distroys it while displaying its elements

     printf ("***** Display for stack of scopes (label = %s) tempStack.size() = %" PRIuPTR " ***** \n",
          label.c_str(),tempStack.size());

     int counter = 0;
     while (tempStack.empty() == false)
        {
          ListOfStatementsType topOfStack = tempStack.top();

          char buffer[256];
          sprintf (buffer,"counter = %d topOfStack.size() = %" PRIuPTR "",counter,topOfStack.size());
          std::string displayString = buffer;
          printf ("     Display new scope: %s \n",displayString.c_str());
#if 1
          display (topOfStack,displayString);
#else
          typename ListOfStatementsType::iterator j;
          for (j = topOfStack.begin(); j != topOfStack.end(); j++)
               (*j).display(displayString);
#endif
          tempStack.pop();
          counter++;
        }
   }


// endif for AST_REWRITE_DEBUGGING_SUPPORT_TEMPLATES_C
#endif
