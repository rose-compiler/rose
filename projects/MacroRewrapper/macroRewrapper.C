#include "macroRewrapper.h"
//#define VERBOSE_MESSAGES_OF_WAVE true



AnalyzeMacroCalls::AnalyzeMacroCalls(SgProject* p){
     std::string internalIncludePaths          = "CXX_INCLUDE_STRING";
     if(SgProject::get_verbose() >= 1)
	  std::cout << "INTERNAL INCLUDE PATHS " << "CXX_INCLUDE_STRING" << std::endl;
     std::vector<std::string>  tmpIncludePathList = CommandlineProcessing::generateArgListFromString(internalIncludePaths);

     std::cout << "Test" << std::endl;

  //internalIncludePathList.push_back("-I"+string(CurrentPath)+"/");

     for (std::vector<std::string>::iterator i = tmpIncludePathList.begin(); i != tmpIncludePathList.end(); ++i)
	{
	  if (i->substr(0,2) == "-I")
	     {
	       internalIncludePathList.push_back((*i).substr(2,(*i).length())); 
	     }
	  if (i->substr(0,13) == "--sys_include")
	     {
               ++i;
	       if ( ( i != tmpIncludePathList.end() )
			       && ( (i->substr(0,8) != "--sys_include") &&  (i->substr(0,2) != "-I") )
		  )
		    internalIncludePathList.push_back(*i); 
	     }
	  else
	     {
	       printf ("Found a non include path in the internal include path list substring = %s *i = %s \n",i->substr(0,2).c_str(),i->c_str());
	     }
	}


     project = p;
     mapDefsToCalls = map_def_call();
     std::cout << "allCallsToMacro" << std::endl;
     macro_def_call_type macrosCallsMappedToDefs = allCallsToMacro2(internalIncludePathList);
     iterate_over_all_macro_calls(macrosCallsMappedToDefs);
     //compare_all_macro_calls();


};

macro_def_call_type&
AnalyzeMacroCalls::getMapOfDefsToCalls(){
     return macroDefToCalls;
};

map_def_call&
AnalyzeMacroCalls::getMapDefsToCalls(){
     return mapDefsToCalls;
};


SgNode*
AnalyzeMacroCalls::findSmallestStmtMatchingMacroCallUsingPositions(PreprocessingInfo* currentInfo){

     SgNode* smallestStmt = NULL;
     PreprocessingInfo::r_macro_call* macro_call = currentInfo->get_macro_call();
     ROSE_ASSERT(macro_call != NULL);
     token_type a_call = macro_call->macro_call;


     //Try to see if the current macrodefinition is only consisting of macro arguments
     PreprocessingInfo::r_macro_def* macro_def   = macro_call->macro_def->get_macro_def();
     if( macro_def->is_functionlike == true )
     {
       token_container& paramaters      = macro_def->paramaters;
       token_list_container& definition = macro_def->definition;

       int tokens_not_arguments =0;
       for(token_list_container::iterator defIt = definition.begin(); 
           defIt != definition.end(); defIt++ )
       {
         //Check to see if token maps to uninteresting elements of the grammar
         using namespace boost;
         switch(boost::wave::token_id(*defIt) /* defIt->token_id() */ )
         {
           case boost::wave::T_CCOMMENT:
           case boost::wave::T_CPPCOMMENT:
           case boost::wave::T_SPACE:
           case boost::wave::T_SPACE2:
           case boost::wave::T_CONTLINE:
           case boost::wave::T_NEWLINE:
           case boost::wave::T_LEFTPAREN:
           case boost::wave::T_RIGHTPAREN:
             continue;
         }

         if(macro_def->macro_name.get_value().substr(0,2) == "__")
           return NULL;


         //Check to see if the token maps to an argument
         bool is_uninteresting = false;
         for(token_container::iterator paramIt = paramaters.begin(); 
             paramIt != paramaters.end(); paramIt++ )
         {
           if( defIt->get_value() == paramIt->get_value() )
           {
             is_uninteresting = true;
             break;
           }

         }

         if(is_uninteresting == true ) continue;

         //Othewise, this is an interestin token, and as long as
         //we have found at least one interesting token this macro
         //is interesting
         tokens_not_arguments++;
         break;
         


       }
       //No interesting tokens in this macro
       if(tokens_not_arguments==0) return NULL;


     }



     //See if any nodes from the macro call can be found in the AST

     int line = a_call.get_position().get_line();
     int col  = a_call.get_position().get_column();

     Sg_File_Info* posOfMacroCall = new Sg_File_Info(a_call.get_position().get_file().c_str(),line,col);

    std::vector<SgNode*> nodesAtPosition;// =   NodeQuery::querySubTree(project,std::bind2nd(std::ptr_fun(queryForLine),  file_info));
     NodesAtLineNumber nodePos;
//     std::pair<std::vector<SgNode*>*,Sg_File_Info>* nodePosPair = new std::pair<std::vector<SgNode*>*,Sg_File_Info*>(&nodesAtPosition,file_info);
     AstQueryNamespace::querySubTree(project,std::bind2nd( nodePos,  std::pair<std::vector<SgNode*>*,Sg_File_Info*>(&nodesAtPosition,posOfMacroCall)));
     if(nodesAtPosition.size()==0){

       if(SgProject::get_verbose() >= 1){
         std::cout << "filename:" << a_call.get_position().get_file().c_str() << " l" << line << " c" << col << std::endl;
         std::cout << currentInfo->getString() << std::endl;
       }
       return NULL;
     }

     ROSE_ASSERT(nodesAtPosition.size() > 0);
     smallestStmt = *nodesAtPosition.begin();
     SgNode* backupNode=smallestStmt;
  //ROSE_ASSERT(nodesAtPostion.size()>0);
     int counter=0;
     ROSE_ASSERT(smallestStmt != NULL);
     while(true){

          std::cout << "The smallest stmt is a: " << smallestStmt->class_name() << std::endl;
	  ROSE_ASSERT(smallestStmt!=NULL);
	  if(SgProject::get_verbose() >= 1)
	       std::cout << smallestStmt->class_name() << " " << ++counter << " \n";

	  std::vector<SgNode*> allNodesInSubTree = NodeQuery::querySubTree(smallestStmt,queryForAllNodes);

       //std::cout << allNodesInSubTree.size() << " " << nodesAtPostion.size() << std::endl;
	  std::list<SgNode*> unionOfSets;
       //Does not work. Why?
       //	       std::set_union(nodesAtPostion.begin(),nodesAtPostion.end(),allNodesInSubTree.begin(),allNodesInSubTree.end(),unionOfSets.begin());

       //Finding the union of the two sets
	  for(std::vector<SgNode*>::iterator it_beg = nodesAtPosition.begin(); it_beg != nodesAtPosition.end();
			  ++it_beg){
	       SgNode* nodeAtPos = *it_beg;
	    //std::cout << "nodeAtPos " << nodeAtPos->class_name() << " ";
               bool noMatch = true;
	       for(std::vector<SgNode*>::iterator it_beg2 = allNodesInSubTree.begin(); it_beg2 != allNodesInSubTree.end();
			       ++it_beg2){
		    SgNode* nodeInSubtree = *it_beg2;

		 //std::cout << nodeInSubtree->class_name();
		    if(nodeInSubtree == nodeAtPos){
			 unionOfSets.push_back(nodeAtPos);
                         if(SgProject::get_verbose() >= 1)
         		         std::cout << " MATCH " << nodeAtPos->class_name() << std::endl;
                         noMatch =false;
			 break;
		    }
	       }
               if(noMatch==true)
	       if(SgProject::get_verbose() >= 1)
		       std::cout << " NO MATCH " << nodeAtPos->class_name() << std::endl;

	       //std::cout << std::endl;
	  }

	  std::cout << "nodesAtPostion " << nodesAtPosition.size() << " allNodesInSubTree " << allNodesInSubTree.size() << 
	             "unionOfSets " << unionOfSets.size() << std::endl;

	  //std::cout << "END TRYING TO MATCH" << std::endl;
	  if( unionOfSets.size() == nodesAtPosition.size() ){

		  // std::cout << "Found all nodes" << std::endl;
		  break;
	  }else{
		  backupNode = smallestStmt->get_parent();

		  if(backupNode==NULL){
			  std::cerr << "Error: parent of smallest statment is NULL" << std::endl;
			  std::cout << smallestStmt->class_name() << std::endl;
			  std::cout << smallestStmt->get_file_info()->get_filenameString() << " l" 
				  << smallestStmt->get_file_info()->get_line() << " c" <<  smallestStmt->get_file_info()->get_col() << std::endl;
			  std::cout << smallestStmt->unparseToString() << std::endl;                        
		  }

		  //ROSE_ASSERT( backupNode!=NULL );
		  //FIXME: Handle the case where the parent of a SgAddressOfOp is NULL
		  if(backupNode == NULL)
			  break;
		  smallestStmt = backupNode;
	  }

     };


     //For expressions which are constant folded choose the original expression tree
     SgValueExp* valExp = isSgValueExp(smallestStmt);
     if( valExp != NULL)
         if( valExp->  get_originalExpressionTree () != NULL)
             smallestStmt = valExp->  get_originalExpressionTree ();

    SgNode* node = smallestStmt;  

    std::cout << "Smallest stmt is: " << node->class_name() << std::endl; 
    if(isSgBinaryOp(node)!=NULL){
        SgBinaryOp* binOp = isSgBinaryOp(node);
        std::vector<SgNode*> vectorOfNodesAtPos;

        vectorOfNodesAtPos =   NodeQuery::querySubTree(binOp->get_lhs_operand(),std::bind2nd(std::ptr_fun(queryForLine), posOfMacroCall));
        std::cout << "Node is a binary op" << std::endl;
        //SKIP Right Operand?
        bool skipLeft = false;
        if(vectorOfNodesAtPos.size() == 0 ){
            skipLeft = true;
        }

        //SKIP Left Operand? 
        bool skipRight = false;
        vectorOfNodesAtPos =   NodeQuery::querySubTree(binOp->get_rhs_operand(),std::bind2nd(std::ptr_fun(queryForLine),  posOfMacroCall));
        if(vectorOfNodesAtPos.size() == 0 ){
            skipRight = true;
        }


        //SKIP Operand?
        bool binaryOpNotPartOfMacroCall = false;
        if( queryForLine(node,posOfMacroCall).size() == 0  ){
             binaryOpNotPartOfMacroCall  = true;
        }

        //SKIP Operand and SubTree?
        if( (skipLeft == true) &&
            (skipRight == false) &&
            ( binaryOpNotPartOfMacroCall == true )
          ){
            std::cout << "Going for rhs :" << smallestStmt->class_name() << std::endl;
            smallestStmt =  binOp->get_rhs_operand();

        }else
           if( (skipLeft == false) &&
             (skipRight == true) &&
            ( binaryOpNotPartOfMacroCall == true )
          ){
            std::cout << "Going for lhs :" << smallestStmt->class_name() << std::endl;
            smallestStmt =  binOp->get_lhs_operand();

        }
       
        
     }
        


     return smallestStmt;

};


//The function 
//    AnalyzeMacroCalls::findMappingOfTokensToAST(SgNode* node, PreprocessingInfo* currentInfo )
//will return a
std::vector<SgNode*> 
AnalyzeMacroCalls::findMappingOfTokensToAST(SgNode* node, PreprocessingInfo* currentInfo ){
	PreprocessingInfo::r_macro_call* macro_call = currentInfo->get_macro_call();
	token_type a_call = macro_call->macro_call;

	int line = a_call.get_position().get_line();
	int col  = a_call.get_position().get_column();

	Sg_File_Info* file_info = new Sg_File_Info(a_call.get_position().get_file().c_str(),line,col);


	std::cout <<"POS TO LOOK FOR:" << file_info->get_filenameString() << " " << file_info->get_line() << " " << 
		file_info->get_col() << std::endl;
	//find a linearization of the nodes at the position we are interested in
	std::vector<SgNode*> linearized_subtree = linearize_subtree(node);
        std::cout << "Linearizing " << node->class_name() << " " << node->unparseToString() << std::endl;

	for(std::vector<SgNode*>::iterator it_beg = linearized_subtree.begin(); it_beg != linearized_subtree.end();
			++it_beg){
		SgNode* nodeAtPos = *it_beg;

         std::cout << nodeAtPos->class_name() << " ";


        }

	std::vector<SgNode*> linNodesAtPost;

	for(std::vector<SgNode*>::iterator it_beg = linearized_subtree.begin(); it_beg != linearized_subtree.end();
			++it_beg){
		SgNode* nodeAtPos = *it_beg;
       		if(isSgStringVal(nodeAtPos)!= NULL){
			std::cout << "STring val at:" << nodeAtPos->get_file_info()->get_line() << std::endl;

		}

                //Filter out the constant folded part of a constant folded expression
                if(isSgValueExp(*it_beg) != NULL )
                {
                  if( SgCastExp* castExp =  isSgCastExp(isSgValueExp(*it_beg)->get_parent()) )
                  {
                    if(castExp->get_operand() == *it_beg)
                    {
                      if(isSgCastExp(castExp->get_parent()) == NULL) 
                      {
                      std::cout << "Skipping node 111" << std::endl;
                      continue;
                      }
                    }
                  }
                }

                if(SgLocatedNode* locatedNode = isSgLocatedNode(nodeAtPos) )
                {
                  if( locatedNode->get_file_info()->isCompilerGenerated() == true )
                    continue;

                }

                /*
                if( isSgType(nodeAtPos) != NULL )
                {
                  std::cout << "ADDED:" << nodeAtPos->class_name() << std::endl;

			linNodesAtPost.push_back(nodeAtPos);
                }*/

		if( queryForLine(nodeAtPos,file_info).size()>0 ){
			linNodesAtPost.push_back(nodeAtPos);
			//Fix for the position of the SgInitializedName 
			if(isSgVariableDeclaration(nodeAtPos)!=NULL){
				SgVariableDeclaration* varDec = isSgVariableDeclaration(nodeAtPos);
				SgInitializedNamePtrList& varDecLst = varDec->get_variables();
				for(SgInitializedNamePtrList::iterator var_it = varDecLst.begin(); var_it != varDecLst.end();
						++var_it){
					if( queryForLine(nodeAtPos,file_info).size()>0 )
						linNodesAtPost.push_back(*var_it);
				}
			}

                        if(SgProject::get_verbose() >=1){
                          std::cout << "ADDED:" << nodeAtPos->class_name() << " " << nodeAtPos->get_file_info()->get_filenameString() << " " << nodeAtPos->get_file_info()->get_line() <<
                            " " << nodeAtPos->get_file_info()->get_col() << std::endl;
                        }

		}else if(SgProject::get_verbose() >=1){

			std::cout << "NOT ADDED:" << nodeAtPos->class_name() << " " << nodeAtPos->get_file_info()->get_filenameString() << " " << nodeAtPos->get_file_info()->get_line() <<
				" " << nodeAtPos->get_file_info()->get_col() << std::endl;
                }
	}

        std::cout << "After added: " << std::endl;

	for(std::vector<SgNode*>::iterator it_beg = linNodesAtPost.begin(); it_beg != linNodesAtPost.end();
			++it_beg){
		SgNode* nodeAtPos = *it_beg;

         std::cout << nodeAtPos->class_name() << " ";


        }


	//find the mapping between the linearized nodes and the macro
	//PreprocessingInfo::token_type a_call = macro_call->macro_call;


	token_container expanded_macro = macro_call->expanded_macro;
	std::vector<SgNode*> returnVector;
	for(unsigned int i=0; i != expanded_macro.size(); ++i)
		returnVector.push_back(NULL);


	PreprocessingInfo::r_macro_def* macro_def = macro_call->macro_def->get_macro_def();
	ROSE_ASSERT(macro_def != NULL);
	unsigned int macro_def_line = macro_def->macro_name.get_position().get_line();

	int counter = 0;


        std::cout << "\nToken list: ";
        for(token_container::iterator it_exp = expanded_macro.begin(); it_exp != expanded_macro.end();
			++it_exp){
		std::cout <<it_exp->get_value() << " " << it_exp->get_position().get_line() <<  " " << it_exp->get_position().get_column() << " " ;


        }


        std::cout << "\nLinearized nodes:";
        for(std::vector<SgNode*>::iterator it_beg2 = linNodesAtPost.begin(); it_beg2 != linNodesAtPost.end();
				++it_beg2){
			SgNode* nodeAtPos = *it_beg2;
                        if(isSgValueExp(nodeAtPos) != NULL )
                            std::cout << nodeAtPos->unparseToString() << " ";
                        else
                            std::cout << nodeAtPos->class_name()      << " ";
        }
        std::cout << std::endl;

	for(token_container::iterator it_exp = expanded_macro.begin(); it_exp != expanded_macro.end();
			++it_exp){
		for(std::vector<SgNode*>::iterator it_beg2 = linNodesAtPost.begin(); it_beg2 != linNodesAtPost.end();
				++it_beg2){
			SgNode* nodeAtPos = *it_beg2;

                        if(isSgValueExp(nodeAtPos) != NULL ) 
                        {
                          SgValueExp* valExp = isSgValueExp(nodeAtPos);
                          if(valExp->get_originalExpressionTree() != NULL)
                            continue;
                        }

                        if(SgLocatedNode* locNode = isSgLocatedNode(nodeAtPos))
                        std::cout << "The pos is: " << nodeAtPos->class_name() << " ";
			//std::cout << nodeAtPos->class_name() << std::endl; 
			if( checkIfNodeMaps(*it_exp,nodeAtPos) == true ){
				//if(SgProject::get_verbose() >= 1){
                          {
					std::cout << " Found mapping" << it_exp->get_value() << " to " << nodeAtPos->class_name() << std::endl;

					if(macro_def_line != it_exp->get_position().get_line())
						std::cout << "Found something not from the macro " << it_exp->get_value() << std::endl;
				}
				linNodesAtPost.erase(it_beg2);
				returnVector[counter] = nodeAtPos;
				break;
			}//else
			//std::cout << "Node does not map:" << nodeAtPos->class_name() << " " << it_exp->get_value()  <<std::endl;

		}

		++counter;

	}


	return returnVector;

};


void 
AnalyzeMacroCalls::iterate_over_all_macro_calls(macro_def_call_type& macro_def){
	std::cout << " Iterate over all macro calls" << std::endl;

	for(macro_def_call_type::iterator def_it = macro_def.begin();
			def_it != macro_def.end(); ++def_it ){

		std::vector<MappedMacroCall> callsToThisDef;

		std::string compareASTStringMatchingMacroCall;


		PreprocessingInfo::r_macro_def* macro_def = def_it->first->get_macro_def();
		ROSE_ASSERT(macro_def != NULL);


		Sg_File_Info* macro_def_pos = def_it->first->get_file_info();




		if(SgProject::get_verbose() >= 1)
			std::cout << "Macro def found at: " << macro_def_pos->get_filenameString() << " l" <<
				macro_def_pos->get_line() << " c" << macro_def_pos->get_col() << std::endl;


		for(macro_call_list::iterator call_it = def_it->second.begin();
				call_it != def_it->second.end(); ++call_it ){

			Sg_File_Info* macro_call_pos = (*call_it)->get_file_info();

			ROSE_ASSERT(macro_call_pos!=NULL);

                        //If we are using the database it does not make sense to reanalyze a macro call which
                        //has already been entered into the database. This optimization detects when the macro
                        //call has already been entered.
			bool analyze_macro = true;
			if(do_db_optimization == true)
				if(exist_macro_call(macro_call_pos->get_filenameString(), macro_call_pos->get_line(),macro_call_pos->get_col()) == true)
					analyze_macro = false;

                        //if(call_it->expanded_macro.size()>1)
                        //        analyze_macro = false;

			if( analyze_macro == true ){  //always true if the database is not used

				SgNode* stmtContainingMacroCall = findSmallestStmtMatchingMacroCallUsingPositions(*call_it);

				if(stmtContainingMacroCall != NULL){
                                        //The object of this true statement of the block is to find the smallest set of
                                        //statements matching the macro call and filter out the arguments for usage in the
                                        //macro inconsistency filtering

					//map_tokens_to_AST(stmtContainingMacroCall, (*call_it)->get_macro_call());

					ROSE_ASSERT((*call_it)->get_macro_call()!=NULL);
					token_container expanded_macro = (*call_it)->get_macro_call()->expanded_macro;

					std::vector< SgNode*> tokenMapToAST = findMappingOfTokensToAST(stmtContainingMacroCall,(*call_it));

					std::vector<SgNode*> nodeFromMacroArgument;
					int counter = 0;
					using namespace boost::wave;

                                        //Discover which nodes come from the macro argument


                                        bool last_token_was_argument = false;
					for(token_container::iterator expMacroIt = expanded_macro.begin();
							expMacroIt != expanded_macro.end(); ++expMacroIt){
						if( ( (unsigned int) expMacroIt->get_position().get_line() != (unsigned int) macro_def_pos->get_line() ) &&
								( token_id(*expMacroIt) != T_SPACE ) &&
								( token_id(*expMacroIt) != T_SEMICOLON )
						  ){
                                                  last_token_was_argument = true;

								std::cout << "Node from macro argument:" << expMacroIt->get_value() << std::endl;

							if(tokenMapToAST[counter]==NULL){
									std::cout << "Error: the corresponding AST node to the argument token is NULL" << std::endl;
							}else
								nodeFromMacroArgument.push_back(tokenMapToAST[counter]);
						}else if(tokenMapToAST[counter]!=NULL && isSgBinaryOp(tokenMapToAST[counter]) && last_token_was_argument  )
                                                {
                                                    nodeFromMacroArgument.push_back(tokenMapToAST[counter]);

								std::cout << "Node from macro argument:" << expMacroIt->get_value() << std::endl;

                                                }else{

								std::cout << "NOT Node from macro argument:" << expMacroIt->get_value() << std::endl;

                                                  last_token_was_argument = false;
                                                }

						++counter;
					};


					std::string ASTStringMatchingMacroCall;

					if(SgProject::get_verbose() >= 1)
						std::cout << "Finding smallest set of statements" << std::endl;


                                        //Smallest set of statments containing macro call
                                        std::vector<SgNode*> smallestSetOfStmtsContainingMacro;

                                        //If the smallest statement containing a macro call is a scope statement and at the
                                        //same time was not part of the macro, find the set of smallest statements containing
                                        //macro.
					if( (isSgScopeStatement(stmtContainingMacroCall)!=NULL) && 
							(queryForLine(stmtContainingMacroCall,macro_call_pos).size()==0) ) {
						std::cout << " In false scope stmt branch" << std::endl;
						SgScopeStatement*  scopeStmt = isSgScopeStatement(stmtContainingMacroCall);
						ROSE_ASSERT(scopeStmt!=NULL);
						SgStatementPtrList scopeStmtPtrLst = scopeStmt->generateStatementList();
						//SgStatementPtrList scopeStmtPtrLst = scopeStmt->getStatementList();
						NodesAtLineNumber nodePos;

						for(SgStatementPtrList::iterator it_stmt = scopeStmtPtrLst.begin(); 
								it_stmt != scopeStmtPtrLst.end(); ++it_stmt){

							std::vector<SgNode*> vectorOfNodesAtPos;
							//NodeQuery::querySubTree(*it_stmt,std::bind2nd(std::ptr_fun(queryForLine), macro_call_pos));
							AstQueryNamespace::querySubTree(*it_stmt,std::bind2nd( nodePos,  std::pair<std::vector<SgNode*>*,Sg_File_Info*>(&vectorOfNodesAtPos,macro_call_pos)));
							if(vectorOfNodesAtPos.size()>0){
								ASTStringMatchingMacroCall= ASTStringMatchingMacroCall+(*it_stmt)->unparseToString();
		                                                                
                                                                smallestSetOfStmtsContainingMacro.push_back(*it_stmt);
							}


						}

					}else{

						ASTStringMatchingMacroCall= stmtContainingMacroCall->unparseToString();
 
                                                smallestSetOfStmtsContainingMacro.push_back(stmtContainingMacroCall);

					}

                                        //Create a linearization where the nodes corresponding to macro arguments is
                                        //filtered out.
					if(SgProject::get_verbose() >= 1)
						std::cout << "Comparison linerization " << nodeFromMacroArgument.size() << std::endl;
					ComparisonLinearization* compLin = new ComparisonLinearization(nodeFromMacroArgument, macro_call_pos);
                                        for(std::vector<SgNode*>::iterator it_stmt = smallestSetOfStmtsContainingMacro.begin();
                                                   it_stmt != smallestSetOfStmtsContainingMacro.end(); ++it_stmt)
                				compLin->traverse(*it_stmt);
        

					if(SgProject::get_verbose() >= 1)
						std::cout << "Ordering nodes" << std::endl;

                                        //Create a container representing the information we have discovered in this function
                                        //for usage elsewhere
					MappedMacroCall macroCall;
					macroCall.macro_call = (*call_it);
                                        macroCall.comparisonLinearization = compLin->get_ordered_nodes();
					macroCall.ASTStringMatchingMacroCall = ASTStringMatchingMacroCall;
                                        macroCall.stmtsContainingMacro = smallestSetOfStmtsContainingMacro;

                                        for(std::vector<SgNode*>::iterator iItr = macroCall.comparisonLinearization.begin();
                                            iItr!=macroCall.comparisonLinearization.end(); iItr++)
                                          std::cout << "The Node is:" << (*iItr)->class_name() << std::endl;


                                        callsToThisDef.push_back(macroCall);

				}else{
					if(SgProject::get_verbose() >= 1)
						std::cout << "Macro call found DID NOT have a matching statement in AST: " ;

				}

			}

			if(SgProject::get_verbose() >= 1)
				std::cout << "Macro call found at: " << macro_call_pos->get_filenameString() << " l" <<
					macro_call_pos->get_line() << " c" << macro_call_pos->get_col() << std::endl;


		}

		if(SgProject::get_verbose() >= 1)
			std::cout << callsToThisDef.size() << " calls to this def" << std::endl;

		mapDefsToCalls[def_it->first] = callsToThisDef;


	}
	std::cout << " END Iterate over all macro calls" << std::endl;


}


void
AnalyzeMacroCalls::add_all_macro_calls_to_db(){

	if(SgProject::get_verbose() >= 1){
		std::cout << "Beginning to add to databse" << std::endl;
	}
	for(map_def_call::iterator def_it = mapDefsToCalls.begin();
			def_it != mapDefsToCalls.end(); ++def_it ){
		PreprocessingInfo* macroDef = def_it->first;
		Sg_File_Info* macro_def_pos = def_it->first->get_file_info();


		bool macro_def_added = false;
		if(SgProject::get_verbose() >= 1){
			std::cout << "Macro def at: " << macroDef->get_file_info()->get_filenameString() << std::endl;
			std::cout << "It has " << def_it->second.size() << " calls to it" << std::endl; 
		}


		for(std::vector<MappedMacroCall>::iterator call_it = def_it->second.begin();
				call_it != def_it->second.end(); ++call_it ){
			PreprocessingInfo* macroCall = call_it->macro_call;
			std::vector<std::string> correspondingAstNodes;// = call_it->second;

			for(std::vector<SgNode*>::iterator it_lin = call_it->comparisonLinearization.begin(); it_lin != call_it->comparisonLinearization.end();
					++it_lin){
				correspondingAstNodes.push_back( (*it_lin)->class_name() );
			}



			std::string ASTStringMatchingMacroCall = call_it->ASTStringMatchingMacroCall;

			Sg_File_Info* macro_call_pos = macroCall->get_file_info();


			//Add to database
			if(SgProject::get_verbose() >= 1)
				std::cout << "Adding macro call to SQL database" << std::endl;



			if( ( macroCall->get_macro_call()->expanded_macro.size() > 1  )){
				//macro_def_t macroDefSQL = find_macro_def(macro_def_pos->get_filenameString(), macro_def_pos->get_line()); // if not found, set the .id to -1;
				if( macro_def_added == false) {
					if(SgProject::get_verbose() >= 1)
						std::cout << "Adding macro def to SQL database" << std::endl;

					if(exist_macro_def(macro_def_pos->get_filenameString(), macro_def_pos->get_line()) == false ){

						PreprocessingInfo::rose_macro_definition* macro_def = macroDef->get_macro_def();
						ROSE_ASSERT( macro_def != NULL );

						std::string macroBodyStr;
						for(token_list_container::iterator tok_it = macro_def->definition.begin();
								tok_it != macro_def->definition.end(); ++tok_it ){
							macroBodyStr = macroBodyStr + std::string(tok_it->get_value().c_str());
						}

						std::vector<std::string> argsVec;

						add_macro_def(macro_def_pos->get_filenameString(), macro_def_pos->get_line(), macro_def_pos->get_col(), std::string(macro_def->macro_name.get_value().c_str()), 
								macroBodyStr, argsVec);
					}

					macro_def_added = true;

					if(SgProject::get_verbose() >= 1)
						std::cout << "DONE Adding macro def to SQL database" << std::endl;


				}


				if( 
						(exist_macro_call(macro_call_pos->get_filenameString(), macro_call_pos->get_line(),macro_call_pos->get_col()) == false) ){





					std::vector<std::string> args;
					std::string expanded_string;
					for(token_container::iterator it_exp = macroCall->get_macro_call()->expanded_macro.begin();
							it_exp != macroCall->get_macro_call()->expanded_macro.end(); ++it_exp){
						expanded_string = expanded_string + std::string(it_exp->get_value().c_str());
					}
					add_macro_call(macro_call_pos->get_filenameString(), macro_call_pos->get_line(), macro_call_pos->get_col(), correspondingAstNodes, 
							args, macro_def_pos->get_filenameString(),macro_def_pos->get_line(), expanded_string, ASTStringMatchingMacroCall);

				}
			}
			if(SgProject::get_verbose() >= 1)
				std::cout << "DONE adding to sql database" << std::endl;






		}
	}

	if(SgProject::get_verbose() >= 1){
		std::cout << "Ending add to databse" << std::endl;
	}

};

std::pair<int,int>
AnalyzeMacroCalls::check_for_inconsistencies(){

    int inconsistencies = 0;
    int consistencies   = 0;

    for(map_def_call::iterator def_it = mapDefsToCalls.begin();
			def_it != mapDefsToCalls.end(); ++def_it ){
		PreprocessingInfo* macroDef = def_it->first;
		Sg_File_Info* macro_def_pos = def_it->first->get_file_info();

        bool macro_def_added = false;
        PreprocessingInfo::rose_macro_definition* macro_def = macroDef->get_macro_def();
        ROSE_ASSERT( macro_def != NULL );

        std::string macroBodyStr;
        for(token_list_container::iterator tok_it = macro_def->definition.begin();
            tok_it != macro_def->definition.end(); ++tok_it ){
            macroBodyStr = macroBodyStr + std::string(tok_it->get_value().c_str());
        }

//        if(SgProject::get_verbose() >= 1){
          //  std::cout << "Macro def at: " << macroDef->get_file_info()->get_filenameString() << std::endl;
          //  std::cout << "It has " << def_it->second.size() << " calls to it" << std::endl; 
          //  std::cout << std::endl;
  //      }
          std::cout << std::endl;

          if(SgProject::get_verbose() >= 1){

            std::cout << "Macro def at: " << macroDef->get_file_info()->get_filenameString() 
              << " l " << macro_def_pos->get_line() << " c " 
              << macro_def_pos->get_col() << std::endl;
            std::cout << "FORMAL BODY: " << macroBodyStr << std::endl;
            std::cout << "It has " << def_it->second.size() << " calls to it" << std::endl; 
            std::cout << std::endl;
          }


        std::string first_expanded_string;
        std::string first_ASTStringMatchingMacroCall;
        Sg_File_Info* first_macro_call_pos;
        std::vector<std::string> first_correspondingAstNodes;

	for(std::vector<MappedMacroCall>::iterator call_it = def_it->second.begin();
			call_it != def_it->second.end(); ++call_it ){
		PreprocessingInfo* macroCall = call_it->macro_call;
		std::vector<std::string> correspondingAstNodes;

                for(std::vector<SgNode*>::iterator it_nodes = call_it->comparisonLinearization.begin(); 
                    it_nodes != call_it->comparisonLinearization.end(); ++it_nodes)
                           correspondingAstNodes.push_back((*it_nodes)->class_name());

		std::string ASTStringMatchingMacroCall = call_it->ASTStringMatchingMacroCall;

		Sg_File_Info* macro_call_pos = macroCall->get_file_info();
		std::string expanded_string;
		if( ( macroCall->get_macro_call()->expanded_macro.size() > 1  )){

			for(token_container::iterator it_exp = macroCall->get_macro_call()->expanded_macro.begin();
					it_exp != macroCall->get_macro_call()->expanded_macro.end(); ++it_exp){
				expanded_string = expanded_string + std::string(it_exp->get_value().c_str());
			}
		}

		//check if the macro call is consistent 
		if(call_it == def_it->second.begin()){
			first_expanded_string           = expanded_string;
			first_macro_call_pos            = macro_call_pos;
			first_ASTStringMatchingMacroCall= ASTStringMatchingMacroCall;
			first_correspondingAstNodes = correspondingAstNodes;
		}else if( compare_macro_tokenlists(first_correspondingAstNodes, correspondingAstNodes, true) ==  false ){
			//std::cout << "Macro def at: " << macroDef->get_file_info()->get_filenameString() 
			//	<< " l " << macro_def_pos->get_line() << " c " 
			//	<< macro_def_pos->get_col() << std::endl;
			//std::cout << "FORMAL BODY: " << macroBodyStr << std::endl;
			//std::cout << "It has " << def_it->second.size() << " calls to it" << std::endl; 

                        if(SgProject::get_verbose() >= 1 )
                        {
                          std::cout << "Macro def at "  
                            << macro_call_pos->get_filenameString()
                            << " l " << macro_call_pos->get_line()
                            << " c " << macro_call_pos->get_col()
                            << "EXPANDED: " << expanded_string << std::endl
                            << "Matching AST: " << ASTStringMatchingMacroCall << std::endl;   
                          std::cout << "Is inconsistent with:" << std::endl;

                          std::cout << "Macro def at "  
                            << first_macro_call_pos->get_filenameString()
                            << " l " << first_macro_call_pos->get_line()
                            << " c " << first_macro_call_pos->get_col()
                            << "EXPANDED: " << first_expanded_string << std::endl
                            << "Matching AST: " << first_ASTStringMatchingMacroCall << std::endl;
                          std::cout << std::endl;   
                        }

			inconsistencies++;
		}else{

			consistencies++;
		}

	}
    }
    std::cout << "We have " << consistencies << " consistencies and " <<  inconsistencies << " inconsistencies " << std::endl;
    return std::pair<int,int>(consistencies,inconsistencies);
};

void
AnalyzeMacroCalls::print_out_all_macros(std::ostream& outStream){

  int inconsistencies = 0;
  int consistencies   = 0;

  for(map_def_call::iterator def_it = mapDefsToCalls.begin();
      def_it != mapDefsToCalls.end(); ++def_it ){
    PreprocessingInfo* macroDef = def_it->first;
    Sg_File_Info* macro_def_pos = def_it->first->get_file_info();

    bool macro_def_added = false;
    PreprocessingInfo::rose_macro_definition* macro_def = macroDef->get_macro_def();
    ROSE_ASSERT( macro_def != NULL );

    std::string macroBodyStr;
    for(token_list_container::iterator tok_it = macro_def->definition.begin();
        tok_it != macro_def->definition.end(); ++tok_it ){
      macroBodyStr = macroBodyStr + std::string(tok_it->get_value().c_str());
    }

    outStream << std::endl;


    outStream << "Macro def at: " << macroDef->get_file_info()->get_filenameString() 
      << " l " << macro_def_pos->get_line() << " c " 
      << macro_def_pos->get_col() << std::endl;
    outStream << "FORMAL BODY: " << macroBodyStr << std::endl;
    outStream << "It has " << def_it->second.size() << " calls to it" << std::endl; 
    outStream << std::endl;


    std::string first_expanded_string;
    std::string first_ASTStringMatchingMacroCall;
    Sg_File_Info* first_macro_call_pos;
    std::vector<std::string> first_correspondingAstNodes;

    for(std::vector<MappedMacroCall>::iterator call_it = def_it->second.begin();
        call_it != def_it->second.end(); ++call_it ){
      PreprocessingInfo* macroCall = call_it->macro_call;
      std::vector<std::string> correspondingAstNodes;

      for(std::vector<SgNode*>::iterator it_nodes = call_it->comparisonLinearization.begin(); 
          it_nodes != call_it->comparisonLinearization.end(); ++it_nodes)
        correspondingAstNodes.push_back((*it_nodes)->class_name());

      std::string ASTStringMatchingMacroCall = call_it->ASTStringMatchingMacroCall;

      Sg_File_Info* macro_call_pos = macroCall->get_file_info();
      std::string expanded_string;
      if( ( macroCall->get_macro_call()->expanded_macro.size() > 1  )){

        for(token_container::iterator it_exp = macroCall->get_macro_call()->expanded_macro.begin();
            it_exp != macroCall->get_macro_call()->expanded_macro.end(); ++it_exp){
          expanded_string = expanded_string + std::string(it_exp->get_value().c_str());
        }
      }


      outStream << "Macro Call at "  
        << macro_call_pos->get_filenameString()
        << " l " << macro_call_pos->get_line()
        << " c " << macro_call_pos->get_col()
        << "EXPANDED: " << expanded_string << std::endl
        << "Matching AST: " << ASTStringMatchingMacroCall << std::endl;   
      outStream << "Is inconsistent with:" << std::endl;

    }
  }
};

