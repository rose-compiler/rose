#include "rose.h"
#include "argumentFilterer.h"
#include "helpFunctions.h"

ComparisonLinearizationAttribute::ComparisonLinearizationAttribute()
{

};



ComparisonLinearization::ComparisonLinearization(std::vector<SgNode*> argNodes, std::vector<SgNode*> defNodes, Sg_File_Info* argFileInfo){
ROSE_ASSERT(argFileInfo!=NULL);
nodeToFilter = argNodes;
nodeToKeep  = defNodes;
posOfMacroCall = argFileInfo;
//constFoldTraverse = TraverseBoth;
for(int i =0; i < argNodes.size() ; i++){
  std::cout << "node to filter " << argNodes[i]->class_name() ;
  if(isSgValueExp(argNodes[i])!= NULL )
    std::cout << " " << argNodes[i]->unparseToString();
  std::cout  <<std::endl;

}

}


std::vector<SgNode*> 
ComparisonLinearization::get_ordered_nodes(){
return nodes;
}

#if 0
bool 
ComparisonLinearization::skipNode(SgNode* node) {
  bool skip = false;

  if(isSgValueExp(node) != NULL )
  {
  
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

  if( find(nodeToFilter.begin(),nodeToFilter.end(),node) != nodeToFilter.end() ){
    skip = true;
  }

 
  if(isSgPointerDerefExp(node)!=NULL || isSgCastExp(node) != NULL)
    skip = true;

  //For expressions which are constant folded choose the original expression tree
  SgValueExp* valExp = isSgValueExp(node);
  if( ( valExp != NULL ) &&
      ( valExp->get_originalExpressionTree() !=NULL   )
    ){
      skip=true;
  }

  //The variablerefexp will represent the variable access itself so this is unnecessary
  //and causes false positives
  if(isSgArrowExp(node) || isSgDotExp(node) )
    skip = true;





  return skip;
}

#endif
ComparisonLinearizationAttribute ComparisonLinearization::evaluateSynthesizedAttribute (
    SgNode* astNode,
    SubTreeSynthesizedAttributes synthesizedAttributeList )
{
  ComparisonLinearizationAttribute synAttrib;


#if 0
  if( synthesizedAttributeList.size() > 2)
  {
    std::string correspondingString =astNode->class_name()+ " ";
    correspondingString+=(astNode)->unparseToString()+" ";
    correspondingString+=boost::lexical_cast<std::string>((astNode)->get_file_info()->get_line());
    correspondingString+= " ";
    correspondingString+=boost::lexical_cast<std::string>((astNode)->get_file_info()->get_col());

    std::cout << "Node with more that two synthesized attributes: " << correspondingString << std::endl;

  }

  ROSE_ASSERT(synthesizedAttributeList.size() <= 2);
#endif



  for( SubTreeSynthesizedAttributes::iterator iItr = synthesizedAttributeList.begin() ; iItr != synthesizedAttributeList.end() ; iItr++  )
  {
    for(int j = 0; j < iItr->nodes.size(); j++)
      synAttrib.nodes.push_back(iItr->nodes[j]);

  }
  nodes = synAttrib.nodes;

  //The variablerefexp will represent the variable access itself so this is unnecessary
  //and causes false positives
  if(isSgArrowExp(astNode) || isSgDotExp(astNode) ||
      isSgPointerDerefExp(astNode)!=NULL || isSgCastExp(astNode) != NULL ||
      isSgExprStatement(astNode) || isSgCastExp(astNode) != NULL ||
      isSgPntrArrRefExp(astNode) != NULL || isSgFunctionRefExp(astNode)!=NULL 
      || isSgExprListExp(astNode) != NULL
    )
    return synAttrib;

  //By definition a compiler generated node does not map to the
  //token stream and it is therefore not interesting for our
  //notion of macro inconsistencies
  SgLocatedNode* compilerGeneratedNode = isSgLocatedNode(astNode);
  if( compilerGeneratedNode != NULL  )
    if(compilerGeneratedNode->get_file_info()->isCompilerGenerated() == true)
      return synAttrib;


  if(SgValueExp* valueExp = isSgValueExp(astNode) )
  { // Catch the constant folded part of a constant folded expression
    if(  valueExp->get_originalExpressionTree() !=NULL ) 
    {
      return synAttrib;
    }
  }

#if 0

  if( find(nodeToFilter.begin(),nodeToFilter.end(),astNode) != nodeToFilter.end() )
    std::cout << "Found IN filter list " << astNode->unparseToString() << std::endl;
  else
    std::cout << "NOT Found IN filter list " << astNode->unparseToString() << std::endl;
#endif

  ///  std::cout << "AA" << std::endl;
  bool nodeFromMacroDef =  ( queryForLine(astNode, posOfMacroCall).size() > 0 ? true : false );

  bool hasKeptNodeInSubtree = false;
  int  numberOfKeptSubtrees = 0;
  for( SubTreeSynthesizedAttributes::iterator iItr = synthesizedAttributeList.begin() ; iItr != synthesizedAttributeList.end() ; iItr++  )
  {
    if( iItr->nodes.size() > 0 )
    {
      hasKeptNodeInSubtree = true;
      numberOfKeptSubtrees++;
    }
  }

  if( hasKeptNodeInSubtree == true )
  {

    if(numberOfKeptSubtrees == 1 &&
       nodeFromMacroDef == false )
    {
    }else
      synAttrib.nodes.push_back(astNode);

  }else if( 
      find(nodeToFilter.begin(), nodeToFilter.end(), astNode) == nodeToFilter.end() &&
      
      find(nodeToKeep.begin(),nodeToKeep.end(),astNode) != nodeToKeep.end() )
  {
    synAttrib.nodes.push_back(astNode);
    std::cout << "Pushing back " << astNode->class_name() << " "<< astNode->unparseToString() << std::endl;
  }


  nodes = synAttrib.nodes;
  return synAttrib;
};





std::vector<SgNode*> queryForNotOnLine(SgNode* node, Sg_File_Info* compareFileInfo){
    std::vector<SgNode*> thisNode = queryForLine(node, compareFileInfo);  

    std::vector<SgNode*> returnList;

    if(thisNode.size()==0)
        returnList.push_back(node);

    return returnList;

};


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
                 //         std::cout << "Start of contruct " << initName->get_name().getString() << " l" << fileInfo->get_line() << " " << fileInfo->get_col() << std::endl;       
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
          std::cout << " construct: " << node->class_name() /*<<  " filename: " << fileInfo->get_filenameString() */
                    << "line " << line << " col " << col 
                    << /*"c-filename" << compareFilename <<*/ " c-line " << compareLine << " c-col " << compareCol << std::endl;

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

//          if(fileInfo->isCompilerGenerated() == true)
//            return returnList;

          if( (line == compareLine) && ( col == compareCol ) && (filename == compareFilename) ){


            
 //              std::cout << "Construct was found at node position:" << node->class_name() << " filename: " << filename << " l" << line << " c" << col << std::endl;
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





