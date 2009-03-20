#include "rose.h"
#include "argumentFilterer.h"
#include "helpFunctions.h"



std::vector<SgNode*> 
ComparisonLinearization::get_ordered_nodes(){
return orderedNodes;
}

ComparisonLinearization::ComparisonLinearization(std::vector<SgNode*> argNodes, Sg_File_Info* argFileInfo){
ROSE_ASSERT(argFileInfo!=NULL);
nodeToFilter = argNodes;
posOfMacroCall = argFileInfo;
constFoldTraverse = TraverseOriginalExpressionTree;
//constFoldTraverse = TraverseBoth;
for(int i =0; i < argNodes.size() ; i++)
std::cout << "node to filter " << argNodes[i]->class_name() <<std::endl;

}


void 
ComparisonLinearization::preOrderVisit(SgNode* node){

     if(isSgFunctionDeclaration(node)!=NULL){
          SgFunctionDeclaration* funcDecl    = isSgFunctionDeclaration(node);

	  //Add the function return type
	  SgFunctionType* funcType = funcDecl->get_type();

	  vector<SgType*> typeVec = typeVectorFromType(funcType);
          for(vector<SgType*>::iterator it_type = typeVec.begin(); 
			  it_type != typeVec.end(); ++it_type){
            if( skipNode(*it_type) == false )
                orderedNodes.push_back(*it_type);
	  };

	  //Add the function declaration itself
	  orderedNodes.push_back(node);

	  //Add the arguments of the function declarations
	  SgInitializedNamePtrList argsList = funcDecl->get_args();

	  for(SgInitializedNamePtrList::iterator it_func = argsList.begin();
			  it_func != argsList.end(); ++it_func){
	       SgInitializedName* funcArg = *it_func;
	       SgType* argType = funcArg->get_type();
               typeVec = typeVectorFromType(funcType);

               for(vector<SgType*>::iterator it_type = typeVec.begin(); 
			  it_type != typeVec.end(); ++it_type){
            if( skipNode(*it_type) == false )

                   orderedNodes.push_back(*it_type);
               };

	       orderedNodes.push_back(funcArg);
	  }
     }else if(isSgVariableDeclaration(node)!=NULL){
          SgVariableDeclaration* varDecl = isSgVariableDeclaration(node);
	  SgInitializedNamePtrList varList = varDecl->get_variables();
          
	  //Add the variable declarations and their types
	  vector<SgType*> typeVec;
	  for(SgInitializedNamePtrList::iterator it_func = varList.begin();
			  it_func != varList.end(); ++it_func){
	       SgInitializedName* var = *it_func;
	       SgType* argType = var->get_type();
               typeVec = typeVectorFromType(argType);

               for(vector<SgType*>::iterator it_type = typeVec.begin(); 
			  it_type != typeVec.end(); ++it_type){
                 if( skipNode(*it_type) == false )

                   orderedNodes.push_back(*it_type);
               };
               if( skipNode(var) == false )

	       orderedNodes.push_back(var);
	  }
     }else
        if( skipNode(node) == false )
         orderedNodes.push_back(node);

      
}


std::vector<SgNode*> queryForNotOnLine(SgNode* node, Sg_File_Info* compareFileInfo){
    std::vector<SgNode*> thisNode = queryForLine(node, compareFileInfo);  

    std::vector<SgNode*> returnList;

    if(thisNode.size()==0)
        returnList.push_back(node);

    return returnList;

};

bool 
ComparisonLinearization::skipNode(SgNode* node) {
  bool skip = false;
  
  if( find( skipNodeAndSubTree.begin(), skipNodeAndSubTree.end(), node) != skipNodeAndSubTree.end() |
      find(nodeToFilter.begin(),nodeToFilter.end(),node) != nodeToFilter.end() |
      isSgType(node) != NULL

      )
  {
    std::cout << "Skipped node: " << node->class_name() << std::endl;
    return true;
  }

  if(isSgValueExp(node) != NULL )
  {
    if( SgCastExp* castExp =  isSgCastExp(isSgValueExp(node)->get_parent()) )
    {
      if(castExp->get_operand() == node)
      {
        if(isSgCastExp(castExp->get_parent()) == NULL) 
        {
          std::cout << "Skipping node 111" << std::endl;
          return true;
        }
      }
    }
   
    //Because of the structure of the code '(0)' where there will be two
    //SgIntVal's where none is marked compiler generated I have to do this
    //condition
    SgValueExp* valExp = isSgValueExp(node);
    if(valExp->get_originalExpressionTree() != NULL)
      return true;


  }

  //By definition a compiler generated node does not map to the
  //token stream and it is therefore not interesting for our
  //notion of macro inconsistencies
  SgLocatedNode* compilerGeneratedNode = isSgLocatedNode(node);
  if( compilerGeneratedNode != NULL  )
    if(compilerGeneratedNode->get_file_info()->isCompilerGenerated() == true)
      return true;

  if(node!=NULL)
    std::cout << "Node is " << node->class_name() << std::endl;

  //  if(isSgType(node) != NULL )
  //      return true;

  if(isSgExprStatement(node))
    skip = true;

  if(isSgBinaryOp(node)!=NULL && isSgArrowExp(node) == NULL && isSgDotExp(node) == NULL  ){
    SgBinaryOp* binOp = isSgBinaryOp(node);
    std::vector<SgNode*> vectorOfNodesAtPos;

    vectorOfNodesAtPos =   NodeQuery::querySubTree(binOp->get_lhs_operand(),std::bind2nd(std::ptr_fun(queryForLine), posOfMacroCall));

    std::cout << "Looking at binary op: " << node->unparseToString() << std::endl;
    //SKIP Right Operand?
    bool skipLeft = false;

    //See if all variables in the subtre is skippable
    bool allNodesInSubTreeSkipped = true;
    for(int i=0; i < vectorOfNodesAtPos.size(); i++ )
    {
      if(find(nodeToFilter.begin(),nodeToFilter.end(),vectorOfNodesAtPos[i]) == nodeToFilter.end())
        allNodesInSubTreeSkipped = false;
    }

    //If all nodes in subtree is skippable or there are no nodes in subtree from macro skip
    if(vectorOfNodesAtPos.size() == 0 || allNodesInSubTreeSkipped == true ){
      skipLeft = true;
      skipNodeAndSubTree.push_back(binOp->get_lhs_operand());            
    }
    for(int i=0; i < vectorOfNodesAtPos.size() ; i++)
      std::cout << vectorOfNodesAtPos[i]->unparseToString() << std::endl;

    //SKIP Left Operand?
    bool skipRight = false;
    vectorOfNodesAtPos =   NodeQuery::querySubTree(binOp->get_rhs_operand(),std::bind2nd(std::ptr_fun(queryForLine),  posOfMacroCall));

    //See if all nodes in subtree is skippable
    allNodesInSubTreeSkipped = true;
    for(int i=0; i < vectorOfNodesAtPos.size(); i++ )
    {
      if(find(nodeToFilter.begin(),nodeToFilter.end(),vectorOfNodesAtPos[i]) == nodeToFilter.end())
        allNodesInSubTreeSkipped = false;
    }

    //If all nodes in subtree oor there are no nodes in the subtree from macro skip
    if(vectorOfNodesAtPos.size() == 0 || allNodesInSubTreeSkipped == true ){
      skipRight = true;
      std::cout <<"SkipRight" << std::endl;
      skipNodeAndSubTree.push_back(binOp->get_rhs_operand());      
    }



    for(int i=0; i < vectorOfNodesAtPos.size() ; i++){
      std::cout << vectorOfNodesAtPos[i]->unparseToString() << vectorOfNodesAtPos[i]->get_file_info()->get_line() << std::endl;
    }

    std::cout <<"SkipLeft" << skipLeft << std::endl;
    std::cout <<"SkipRight" << skipRight << std::endl;


    //SKIP Operand?
    bool binaryOpNotPartOfMacroCall = false;
    if( queryForLine(node,posOfMacroCall).size() == 0  || 
        find(nodeToFilter.begin(),nodeToFilter.end(),node) != nodeToFilter.end()  ){
      binaryOpNotPartOfMacroCall  = true;
    }

    //SKIP Operand and SubTree?
    if( (skipLeft == true) &&
        (skipRight == true) &&
        ( binaryOpNotPartOfMacroCall == true )
      ){
      skip = true;
      skipNodeAndSubTree.push_back(node);
      std::cout << "The whole binary op filtered out" << std::endl;
    }else if( ((skipLeft == true) | (skipRight == true)) &&
        ( binaryOpNotPartOfMacroCall == true ) 
        ){
      //SKIP Operand and One Of The Subtrees?

      std::cout << "The binary op itself filtered out" << std::endl;

      //if( queryForLine(node->get_parent(),posOfMacroCall).size() == 0  ){
      skip = true;
      //std::cout << "The binary op itself filtered out : Pos" << std::endl;
      //}


    }

    if( (isSgArrowExp(node) != NULL ) ||
        (isSgDotExp(node) != NULL  )
      ){
      //ROSE has a bug such that if we have a member variable in a class which is
      //references in a member function of that class and the this-> is not in front of
      //the variable reference the "this->" is not marked correctly as compiler generated.

      SgBinaryOp* binOp = isSgBinaryOp(node);
      if(isSgThisExp(binOp->get_lhs_operand()) != NULL){
        skip=true;
        nodeToFilter.push_back(binOp->get_lhs_operand());
      }
    }

  }else if( find(nodeToFilter.begin(),nodeToFilter.end(),node) != nodeToFilter.end() ){
    skip = true;
  }

  if(isSgUnaryOp(node) != NULL )
  {

    if( find(nodeToFilter.begin(),nodeToFilter.end(),isSgUnaryOp(node)->get_operand()) != nodeToFilter.end() |
        queryForLine(node,posOfMacroCall).size() == 0 ){
      skip = true;
    }

    if(isSgPointerDerefExp(node)!=NULL || isSgCastExp(node) != NULL)
      skip = true;

  }else  if( (isSgArrowExp(node) != NULL ) )
    node->get_file_info()->display(std::string(" debug ")) ;

  //For expressions which are constant folded choose the original expression tree
  SgValueExp* valExp = isSgValueExp(node);
  if( ( valExp != NULL ) &&
      ( valExp->get_originalExpressionTree() !=NULL   )
    ){
    if( constFoldTraverse == TraverseOriginalExpressionTree ){
      skip=true;
    }

  }

  //The variablerefexp will represent the variable access itself so this is unnecessary
  //and causes false positives
  if(isSgArrowExp(node) || isSgDotExp(node) )
    skip = true;





  return skip;
}

bool
ComparisonLinearization::skipSubTreeOfNode(SgNode* node) { 

    bool skip = false;
    if( find( skipNodeAndSubTree.begin(), skipNodeAndSubTree.end(), node) != skipNodeAndSubTree.end() )
        skip = true;






    SgValueExp* valExp = isSgValueExp(node);
    if( ( valExp != NULL ) &&
        ( valExp->get_originalExpressionTree() !=NULL   )
      ){
 
          if( constFoldTraverse == TraverseConstantFoldedExpression )
                 skip=true;

       }
 
return skip;
}





/*****************************************************************
  * The function
  *  std::vector<SgNode*> queryForLine(SgNode* node, Sg_File_Info* compareFileInfo){
  * finds all AST constructs at a given positions (filename, line, column).
  ***************************************************************/
std::vector<SgNode*> queryForAllNodes(SgNode* node){

     std::vector<SgNode*> returnList;

     returnList.push_back(node);
     return returnList;

};

NodesAtLineNumber::result_type NodesAtLineNumber::operator()(first_argument_type node, const second_argument_type accumulatedList ) const
   {
     second_argument_type::first_type returnList = (second_argument_type::first_type) accumulatedList.first;
     Sg_File_Info*  compareFileInfo = accumulatedList.second;
     SgLocatedNode* locNode = isSgLocatedNode(node);

     int compareLine = compareFileInfo->get_line();
     int compareCol  = compareFileInfo->get_col();
     std::string compareFilename = compareFileInfo->get_filenameString();

     if( isSgIfStmt(node) != NULL ){
	  SgIfStmt* ifStmt         = isSgIfStmt(node);
	  SgStatement* conditional = ifStmt->get_conditional();

	  Sg_File_Info* fileInfo = conditional->get_file_info();

	  int line = fileInfo->get_line();
	  int col  = fileInfo->get_col();

       //std::cout << "Conditional has pos l"<< line << " c" << col << std::endl;
	  AstQueryNamespace::querySubTree(conditional,std::bind2nd(*this,accumulatedList));
       //returnList =   NodeQuery::querySubTree(conditional,std::bind2nd(std::ptr_fun(queryForLine),  compareFileInfo));

     } else if( isSgFunctionDeclaration(node) != NULL ){

	  SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node);
	  SgInitializedNamePtrList args = funcDecl->get_args();

	  for(SgInitializedNamePtrList::iterator it_begin = args.begin();
			  it_begin != args.end(); ++it_begin){

	       SgInitializedName* initName = *it_begin;

	       Sg_File_Info* fileInfo = initName->get_file_info();

	       int line = fileInfo->get_line();
	       int col  = fileInfo->get_col();
	       std::string filename = fileInfo->get_filenameString();

	       if( fileInfo->isCompilerGenerated()==false ){
		 //	    std::cout << "Start of contruct " << initName->get_name().getString() << " l" << fileInfo->get_line() << " " << fileInfo->get_col() << std::endl;       
		    if( (line == compareLine) && ( col == compareCol ) && (filename == compareFilename) ){

		      //std::cout << " filename: " << filename << " l" << line << " c" << col << std::endl;

			 returnList->push_back(node);
		    }
	       }


	  }


     }else if(locNode != NULL){

	  Sg_File_Info* fileInfo = locNode->get_file_info();

	  int line = fileInfo->get_line();
	  int col  = fileInfo->get_col();
	  std::string filename = fileInfo->get_filenameString();

	  if( (line == compareLine) && ( col == compareCol ) && (filename == compareFilename) ){


	    //std::cout << "Construct was found at node position:" << node->class_name() << " filename: " << filename << " l" << line << " c" << col << std::endl;

	       returnList->push_back(node);

	  }

     }


     //returnList->push_back(node);
   };




/*****************************************************************
 * The function
 *  std::vector<SgNode*> queryForLine(SgNode* node, Sg_File_Info* compareFileInfo){
 * finds all AST constructs at a given positions (filename, line, column).
 ***************************************************************/
std::vector<SgNode*> queryForLine(SgNode* node, Sg_File_Info* compareFileInfo){

     std::vector<SgNode*> returnList;

     SgLocatedNode* locNode = isSgLocatedNode(node);

     int compareLine = compareFileInfo->get_line();
     int compareCol  = compareFileInfo->get_col();
     std::string compareFilename = compareFileInfo->get_filenameString();

	  Sg_File_Info* fileInfo = node->get_file_info();


	  int line = fileInfo->get_line();
	  int col  = fileInfo->get_col();
          std::cout << " construct: " << node->class_name() <<  "line " << line << " col " << col << std::endl;

     if( isSgIfStmt(node) != NULL ){
	  SgIfStmt* ifStmt         = isSgIfStmt(node);
	  SgStatement* conditional = ifStmt->get_conditional();

	  Sg_File_Info* fileInfo = conditional->get_file_info();

	  int line = fileInfo->get_line();
	  int col  = fileInfo->get_col();

       //std::cout << "Conditional has pos l"<< line << " c" << col << std::endl;
	  returnList =   NodeQuery::querySubTree(conditional,std::bind2nd(std::ptr_fun(queryForLine),  compareFileInfo));

     } else if( isSgFunctionDeclaration(node) != NULL ){

	  SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node);
	  SgInitializedNamePtrList args = funcDecl->get_args();

	  for(SgInitializedNamePtrList::iterator it_begin = args.begin();
			  it_begin != args.end(); ++it_begin){

	       SgInitializedName* initName = *it_begin;

	       Sg_File_Info* fileInfo = initName->get_file_info();

	       int line = fileInfo->get_line();
	       int col  = fileInfo->get_col();
	       std::string filename = fileInfo->get_filenameString();

	       if( fileInfo->isCompilerGenerated()==false ){
		 	    std::cout << "Start of contruct " << initName->get_name().getString() << " l" << fileInfo->get_line() << " " << fileInfo->get_col() << std::endl;       
		    if( (line == compareLine) && ( col == compareCol ) && (filename == compareFilename) ){

		      //std::cout << " filename: " << filename << " l" << line << " c" << col << std::endl;

			 returnList.push_back(node);
		    }
	       }
	  }


     }else if(locNode != NULL){

	  Sg_File_Info* fileInfo = locNode->get_file_info();

	  int line = fileInfo->get_line();
	  int col  = fileInfo->get_col();


	  std::string filename = fileInfo->get_filenameString();
std::cout << "Construct was found at node position:" << node << " " << node->class_name() << " Compiler generated: "<< (fileInfo->isCompilerGenerated() ? "true" : "false") << " filename: " << filename << " l" << line << " c" << col << std::endl;

	  if( (line == compareLine) && ( col == compareCol ) && (filename == compareFilename) ){


            
               std::cout << "Construct was found at node position:" << node->class_name() << " filename: " << filename << " l" << line << " c" << col << std::endl;
               returnList.push_back(node);

	  }else if(isSgVarRefExp(node) != NULL){
              //For    int arrRefExp[2]; arrRefExp[1];
              //the position of the second arrRefExp is compiler generated.
              //But the position of the wrapping PntrArrExp has a position which
              //begins at the beginning of the expression. 
              SgNode* parent = locNode->get_parent();
              if( isSgPntrArrRefExp(parent) != NULL ){
                 if(  queryForLine(parent,compareFileInfo).size()>0   ){
                     returnList.push_back(node);
                 }

              }

          }

           

     }


     return returnList;

};





