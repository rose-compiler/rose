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
                   orderedNodes.push_back(*it_type);
               };

	       orderedNodes.push_back(var);
	  }
     }else
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

    if(isSgBinaryOp(node)!=NULL){
        SgBinaryOp* binOp = isSgBinaryOp(node);
        std::vector<SgNode*> vectorOfNodesAtPos;

        vectorOfNodesAtPos =   NodeQuery::querySubTree(binOp->get_lhs_operand(),std::bind2nd(std::ptr_fun(queryForLine), posOfMacroCall));

        //SKIP Right Operand?
        bool skipLeft = false;
        if(vectorOfNodesAtPos.size() == 0 ){
            skipLeft = true;
            skipNodeAndSubTree.push_back(binOp->get_lhs_operand());            
        }

        //SKIP Left Operand?
        bool skipRight = false;
        vectorOfNodesAtPos =   NodeQuery::querySubTree(binOp->get_rhs_operand(),std::bind2nd(std::ptr_fun(queryForLine),  posOfMacroCall));
        if(vectorOfNodesAtPos.size() == 0 ){
            skipRight = true;
             std::cout <<"SkipRight" << std::endl;
            skipNodeAndSubTree.push_back(binOp->get_rhs_operand());            
        }


        //SKIP Operand?
        bool binaryOpNotPartOfMacroCall = false;
        if( queryForLine(node,posOfMacroCall).size() == 0  ){
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
            
            if( queryForLine(node->get_parent(),posOfMacroCall).size() == 0  ){
            skip = true;
            std::cout << "The binary op itself filtered out : Pos" << std::endl;
            }


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
            

    if( find( skipNodeAndSubTree.begin(), skipNodeAndSubTree.end(), node) != skipNodeAndSubTree.end() )
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
		 //	    std::cout << "Start of contruct " << initName->get_name().getString() << " l" << fileInfo->get_line() << " " << fileInfo->get_col() << std::endl;       
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





