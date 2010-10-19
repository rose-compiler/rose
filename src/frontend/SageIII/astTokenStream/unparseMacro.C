// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "unparseMacro.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"


//Preorder travesal to find smallest subtrees containing macro call
// Build an inherited attribute for the tree traversal to test the rewrite mechanism

class FindSmallestStatementsInh
{
  public:
    //This variable communicates to subtree that the topmost node has already been found
    bool inSubtreeOfStatement;

    FindSmallestStatementsInh () : inSubtreeOfStatement(false) {};

    FindSmallestStatementsInh (bool inSubtree = false) : inSubtreeOfStatement(inSubtree) {};
    FindSmallestStatementsInh ( const FindSmallestStatementsInh & X ) : inSubtreeOfStatement(X.inSubtreeOfStatement){};
};

//Find set of topmost nodes that match a macro call. By topmost nodes we mean the first nodes that are encountered in
//a subtree from the macro call.
class FindSmallestStatements : public AstTopDownProcessing<FindSmallestStatementsInh>
{
  public:
    bool find_all;
    //Set of nodes matching macro call
    std::vector<SgNode*> matchingCalls;
    //The position info that we should find the topmost nodes for
    Sg_File_Info* macroCallInfo;
    virtual FindSmallestStatementsInh evaluateInheritedAttribute(SgNode* n, FindSmallestStatementsInh inheritedAttribute);

    std::vector<SgNode*> traverseSubTree(SgNode* n, PreprocessingInfo* macroCall );
    


};

FindSmallestStatementsInh
FindSmallestStatements::evaluateInheritedAttribute(SgNode* n, FindSmallestStatementsInh inheritedAttribute)
{



  //If the node should be consired as a topmost node
  if(inheritedAttribute.inSubtreeOfStatement == false && isSgLocatedNode(n))
  {

    Sg_File_Info*  thisNodePos = n->get_file_info();
#if 0

    std::string pos;
    thisNodePos->display(pos);

    std::cerr  << "************************\n";
    std::cerr << "NodePosDisp " << n->class_name() << pos <<  std::endl;
    std::cerr  << "************************\n";

#endif
    //If positions are the same add to list of statements
    switch(n->variantT())
    {
      case V_SgExprStatement:
        std::cout << "Skipping this node" << std::endl;
        break;
      default:
        {
          if( macroCallInfo->get_file_id() == thisNodePos->get_file_id() &&
              macroCallInfo->get_line()    == thisNodePos->get_line()    &&
              macroCallInfo->get_col()     == thisNodePos->get_col()
            )
          {
            if( find_all == false )
              inheritedAttribute.inSubtreeOfStatement = true;
            matchingCalls.push_back(n);
          }
          break;
        }


    }


  }

  return inheritedAttribute;
}

std::vector<SgNode*> 
FindSmallestStatements::traverseSubTree(SgNode* n, PreprocessingInfo* macroCall)
{

  macroCallInfo = macroCall->get_file_info();

  FindSmallestStatementsInh inh(false);
  this->traverse(n, inh);

 
  return matchingCalls;
}

//Preorder travesal to find the nodes PreprocessingInfo pairs.
class findPreprocInfo : public AstSimpleProcessing
{
  public:
    std::vector< std::pair<SgNode*, PreprocessingInfo*> > wherePreprocIsAttached;
    virtual void visit(SgNode* n);
};

void findPreprocInfo::visit(SgNode* n)
{

  // Add in the information from the current node
  SgLocatedNode* locatedNode = isSgLocatedNode(n);
  if (locatedNode != NULL)
  {
    AttachedPreprocessingInfoType* commentsAndDirectives = locatedNode->getAttachedPreprocessingInfo();

    if (commentsAndDirectives != NULL)
    {
      // printf ("Found attached comments (to IR node at %p of type: %s): \n",locatedNode,locatedNode->class_name().c_str());
      // int counter = 0;

      // Use a reverse iterator so that we preserve the order when using push_front to add each directive to the accumulatedList
      AttachedPreprocessingInfoType::reverse_iterator i;
      for (i = commentsAndDirectives->rbegin(); i != commentsAndDirectives->rend(); i++)
      {
        // The different classifications of comments and directives are in ROSE/src/frontend/SageIII/rose_attributes_list.h
        if ((*i)->getTypeOfDirective() == PreprocessingInfo::CMacroCall)
        {
          wherePreprocIsAttached.push_back(std::pair<SgNode*,PreprocessingInfo*>(n,*i) );
        }
      }
    }
  }


}



namespace UnparseMacro {

  bool matchMacroToSubtrees(SgNode* searchTree, PreprocessingInfo* macroCall, std::vector<SgNode*>& matchingSubtree)
  {
    bool macroMatchesSubtrees = true;

#ifndef CXX_IS_ROSE_CODE_GENERATION
    using namespace std;

    //Find topmost nodes
    bool find_all = false;
    FindSmallestStatements stmtMatch;
    stmtMatch.find_all = find_all;
    matchingSubtree = stmtMatch.traverseSubTree(searchTree, macroCall);

    //Find nodes containing more than one topmost node bounded by statement boundaries

did_removal:

    std::cout << "Size of subtree:" << matchingSubtree.size() << std::endl;
    for(unsigned int i = 0 ; i < matchingSubtree.size(); i++)
        std::cout  << matchingSubtree[i]->unparseToString() << std::endl;


    for(unsigned int i = 0 ; i < matchingSubtree.size(); i++)
    {
      SgNode* parent =  matchingSubtree[i]->get_parent();
      while( isSgStatement(parent) == NULL )
      {

        stmtMatch.matchingCalls.clear();
        stmtMatch.traverseSubTree(parent, macroCall);
        stmtMatch.find_all = true;
        std::vector<SgNode*>& nodeAtPosInSubTree = stmtMatch.matchingCalls;

        sort(nodeAtPosInSubTree.begin(),nodeAtPosInSubTree.end());
        sort(matchingSubtree.begin(),matchingSubtree.end());

        std::vector<SgNode*> temp;
        set_intersection(nodeAtPosInSubTree.begin(),nodeAtPosInSubTree.end(), matchingSubtree.begin(), matchingSubtree.end(),
            inserter(temp, temp.begin()));
        //The subtree of parent must contain at least two nodes from matchingSubtree
        if( temp.size() > 1 )
        {

          temp.clear();
          set_difference(nodeAtPosInSubTree.begin(),nodeAtPosInSubTree.end(), matchingSubtree.begin(), matchingSubtree.end(),
            inserter(temp, temp.begin()));

          matchingSubtree = temp;
          matchingSubtree.push_back(parent);

          parent =  parent->get_parent();

          goto did_removal;

        }

        parent =  parent->get_parent();
      }

    }

#endif

    //Do some fancy check to see if macro really matches either a subtree or a set of subtrees



   return macroMatchesSubtrees;
  };

  //Replace the unparsing of expanded macro calls with the actual macro call wherever possible
  void unparseMacroCalls(SgNode* searchTree)
  {
    //Traverse AST to find all macro calls and the nodes they are attached to
    findPreprocInfo findPre;
    findPre.traverse(searchTree, preorder);

    std::vector< std::pair<SgNode*, PreprocessingInfo*> >& wherePreprocIsAttached = findPre.wherePreprocIsAttached;

    //Replace expanded macro calls with actual macro call from pre-cpp wherever possible
    for( std::vector< std::pair<SgNode*, PreprocessingInfo*> >::iterator iItr = wherePreprocIsAttached.begin(); 
        iItr != wherePreprocIsAttached.end(); ++iItr)
    {
      SgStatement*       currentNode = isSgStatement( (*iItr).first );
      PreprocessingInfo* curPreproc  = (*iItr).second;

      ROSE_ASSERT(currentNode != NULL);

      std::vector<SgNode*> matchingSubTree;

      if ( matchMacroToSubtrees(currentNode->get_scope(), curPreproc, matchingSubTree) )
      {
        for(unsigned int i = 0; i < matchingSubTree.size(); i++)
        {
          SgLocatedNode* macroNode = isSgLocatedNode(matchingSubTree[i]);
          ROSE_ASSERT(macroNode != NULL);
          std::string replacementString = ( i ==0 ? curPreproc->getString() : "" );

          if( isSgExpression(macroNode) == NULL )
          {
#ifndef USE_ROSE
         // If we are using ROSE to compile ROSE source code then the Wave support is not present.
            PreprocessingInfo::rose_macro_call* macroCall = curPreproc->get_macro_call();

            if(macroCall->expanded_macro.size() > 0 && boost::wave::token_id(macroCall->expanded_macro.back()) != boost::wave::T_COLON)
              replacementString +=";";
#endif
          }

          std::cout << "Doing line replacement " << macroNode->unparseToString() << " with " << replacementString << std::endl;

#if 0
          std::string pos;
          curPreproc->display(pos);
          std::cout << macroNode->class_name() << " "<<  pos << std::endl;
#endif

          macroNode->addToAttachedPreprocessingInfo(new PreprocessingInfo(PreprocessingInfo::LineReplacement,
                replacementString,macroNode->get_file_info()->get_filenameString(),1,1,1,PreprocessingInfo::before));
        }

        
      };

       

    }
    
  };

}
