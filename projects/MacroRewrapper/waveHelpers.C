// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure
#include "rose.h"

#include "waveHelpers.h"
#define VERBOSE_MESSAGES_OF_WAVE true
//build the map of macro defs to macro calls to that def
//using the global map of filenames to PreprocessingInfo's.
macro_def_call_type 
allCallsToMacro2( std::list<std::string> internalIncludePathList ){
     std::cout << "allCallsToMacro" << std::endl;


  // Build the traversal object and call "traverse" member function

     macro_def_call_type mapOfDefsToCalls;


     typedef std::map<std::string, std::vector<PreprocessingInfo*>* > attached_attr_type;
     typedef  std::vector<PreprocessingInfo*> rose_attr_list_type;

     std::cout << "The size:" << mapFilenameToAttributes.size() << std::endl;

     for( std::map<std::string,ROSEAttributesList* > ::iterator  it_files = mapFilenameToAttributes.begin();
		     it_files != mapFilenameToAttributes.end();
		     ++it_files){

	  std::string filename = it_files->first;

	  if(SgProject::get_verbose() >= 1){
	       std::cout << "Filename to check " << filename << std::endl;
	  }

	  for (rose_attr_list_type::iterator it_1 =
			  it_files->second->getList().begin(); it_1 != it_files->second->getList().end();
			  ++it_1)
	     {

	       PreprocessingInfo *currentInfo = *it_1;
               PreprocessingInfo::DirectiveType typeOfDirective =
		       currentInfo->getTypeOfDirective ();

	    // Here attach include directives to a separate graph
	       if (typeOfDirective ==
			       PreprocessingInfo::CMacroCall)
		  {

		    PreprocessingInfo::r_macro_call* macro_call = currentInfo->get_macro_call();

		    const token_container* tok_cont = currentInfo->get_token_stream();

		 //Handle macro expansions which maps to tokens without a located node
		 //Examples: types
		    bool streamPossiblyMapsToAST = true;

		 //Ignore macro calls in system header files or rose_edg_macros_and_functions_required_for_gnu
		    std::string filenameMacroCall = currentInfo->get_file_info()->get_filenameString();

		    if(filenameMacroCall.find("rose_edg_required_macros_and_functions.h")
				    != std::string::npos)
			 streamPossiblyMapsToAST = false;

		    for(std::list<std::string>::iterator it_beg = internalIncludePathList.begin(); 
				    it_beg != internalIncludePathList.end(); ++it_beg){
		      //std::cout << filenameMacroCall << std::endl;
			 if(filenameMacroCall.find(*it_beg) != std::string::npos)
			      streamPossiblyMapsToAST = false;

		    }

		    if(streamPossiblyMapsToAST == true){
			 streamPossiblyMapsToAST = false;
			      for(token_container::const_iterator it_tok = macro_call->expanded_macro.begin();
					      it_tok != macro_call->expanded_macro.end(); ++it_tok){

				   if( tokenHasNoCorrespondingASTLocatedConstruct(*it_tok) == false ){
					streamPossiblyMapsToAST = true;
					if(SgProject::get_verbose() >= 1)
					     std::cout << it_tok->get_value() << "  maps to AST" << std::endl;
					break;
				   }else{
					using namespace boost::wave;
					if(SgProject::get_verbose() >= 1)
					     std::cout << get_token_name(token_id(*it_tok)) << " do not match to AST" << std::endl;

				   }
			      };

		    }

		    if( streamPossiblyMapsToAST == true ){ 
			 PreprocessingInfo* macro_def   = macro_call->macro_def;

			 /*if(SgProject::get_verbose() >= 1)*/{
			      std::cout << "macro def: " <<  macro_def->get_file_info()->get_filenameString() << " l" << macro_def->get_file_info()->get_line()
				      << " c" << macro_def->get_file_info()->get_col() << std::endl; 
			      std::cout << macro_def->getString() << std::endl;
			      std::cout << "macro call: " <<  currentInfo->get_file_info()->get_filenameString() << " l" << currentInfo->get_file_info()->get_line()
				      << " c" << currentInfo->get_file_info()->get_col() << std::endl; 
			      std::cout << currentInfo->getString() << std::endl;

                              token_list_container& macroDefinition = macro_def->get_macro_def()->definition;
                              for(token_list_container::iterator expMacroIt = macroDefinition.begin();
                                  expMacroIt != macroDefinition.end(); ++expMacroIt){
                                std::cout << "\nPosition:" << expMacroIt->get_value() << " l" << expMacroIt->get_position().get_line()
                                  << " c" << expMacroIt->get_position().get_column() << std::endl;


                              }

			 }


			 ROSE_ASSERT(macro_def!=NULL);
			 macro_def_call_type::iterator current_def = mapOfDefsToCalls.find(macro_def);
			 if( current_def != mapOfDefsToCalls.end() ){
			      current_def->second.push_back(currentInfo);
			 }else{
			      mapOfDefsToCalls[macro_def] = ( std::list<PreprocessingInfo*>()/*.push_back(currentInfo)*/ );
			      mapOfDefsToCalls.find(macro_def)->second.push_back(currentInfo);
			 }
		    }

		  }else if (typeOfDirective ==
			       PreprocessingInfo::CpreprocessorDefineDeclaration)
                  {
                    //Macro defs with no macro calls
	            if( mapOfDefsToCalls.find(currentInfo) == mapOfDefsToCalls.end() ){
                       //std::cout << "Adding def" << currentInfo << std::endl;
	               mapOfDefsToCalls[currentInfo] = ( std::list<PreprocessingInfo*>());

                       token_list_container& macroDefinition = currentInfo->get_macro_def()->definition;
                       for(token_list_container::iterator expMacroIt = macroDefinition.begin();
                           expMacroIt != macroDefinition.end(); ++expMacroIt){
                         std::cout << "\nPosition1:" << expMacroIt->get_value() << " l" << expMacroIt->get_position().get_line()
                           << " c" << expMacroIt->get_position().get_column() << std::endl;


                       }

                    }


                  }
	     }




     }


     std::cout << "The mapOfDefsToCalls SIZE " << mapOfDefsToCalls.size() << std::endl;

     return mapOfDefsToCalls;


};

bool
tokenHasNoCorrespondingASTLocatedConstruct(token_type tok){

	bool a_type = false;

	switch(boost::wave::token_id(tok)){
		case boost::wave::T_CHAR:
		case boost::wave::T_CONST:
		case boost::wave::T_DOUBLE: 		
		case boost::wave::T_INT:
		case boost::wave::T_LONG:
		case boost::wave::T_SHORT:
		case boost::wave::T_SIGNED:
		case boost::wave::T_UNSIGNED:
		case boost::wave::T_VOID:
		case boost::wave::T_SPACE:
		case boost::wave::T_SPACE2:
		case boost::wave::T_CONTLINE:
		case boost::wave::T_CCOMMENT:
		case boost::wave::T_CPPCOMMENT:
			a_type = true;
			break;

                case boost::wave::T_TYPEDEF:
                        a_type = true;
		default:
			break;

	};

	return a_type;

} 


	bool
checkIfNodeMaps(token_type tok, SgNode* node)
{
	bool nodeMaps = false;

	using namespace boost::wave;

        //By definition a compiler generated node can
        //not map to the token stream
        SgLocatedNode* compilerGeneratedNode = isSgLocatedNode(node);
        if( compilerGeneratedNode != NULL  )
          if(compilerGeneratedNode->get_file_info()->isCompilerGenerated() == true)
            return false;

//	     std::cout << get_token_name(tok) << " " << std::string(tok.get_value().c_str()) << " " << node->class_name() << " " << node->get_file_info()->get_line() << std::endl;
	//Mapping literal token id's
	switch(token_id(tok)){
		case T_PP_NUMBER:{

					 if(isSgValueExp(node)!=NULL)
						 nodeMaps=true;
					 break;
				 }
		case T_CHARLIT:{
				       if( ( isSgCharVal(node) != NULL ) |
						       ( isSgUnsignedCharVal(node) != NULL )
					 )
					       nodeMaps = true;
				       break;
			       }
		case T_FLOATLIT:{
					if( isSgFloatVal(node) != NULL )
						nodeMaps = true;

					break;
				}
		case T_INTLIT:{
				      if( ( isSgIntVal(node) != NULL ) || 
						      ( isSgUnsignedIntVal(node) != NULL ) 	       
					)
					      nodeMaps = true;
				      break;
			      }
		case T_LONGINTLIT:
			      {
				      if( ( isSgLongIntVal(node) != NULL ) |
						      ( isSgLongLongIntVal(node) != NULL ) |
						      ( isSgUnsignedLongLongIntVal(node) != NULL )

					)
					      nodeMaps = true;
				      break;
			      }
		case T_STRINGLIT:
			      {
				      if( isSgStringVal(node) != NULL )
					      nodeMaps = true;
				      break;
			      }
                case T_QUESTION_MARK:
                              {
                   		      if( isSgConditionalExp(node) != NULL )
                                        nodeMaps = true;
                                      break;
                              }
		case T_FALSE:
		case T_TRUE:
			      if( isSgBoolValExp(node) != NULL  )
				      nodeMaps = true;
			      break;
		default:
			      break;
	}

	//map keyword token id's
	switch(token_id(tok)){

		case T_ASM:
			if( isSgAsmStmt(node) != NULL  )
				nodeMaps = true;
			break;

		case T_AUTO: //auto
			//dont know
			break;
			/*case T_BOOL:
			//dont think this can be mapped
			break;*/
		case T_BREAK:
			if( isSgBreakStmt(node) != NULL  )
				nodeMaps = true;
			break;

		case T_CASE:
			if( isSgCaseOptionStmt(node) != NULL  )
				nodeMaps = true;
			break;

		case T_CATCH:
			if( isSgCatchOptionStmt(node) != NULL )
				nodeMaps = true;
			break;
			/*
			   case T_CHAR:
			//dont know
			break;
			*/	    

		case boost::wave::T_CLASS:
			if( isSgClassDeclaration(node) != NULL )
				nodeMaps = true;
			break;

		case T_CONST:
			// is it SgConstVolatileModifier?
			//dont know
			break;
		case T_CONTINUE:
			if( isSgContinueStmt(node) != NULL )
				nodeMaps = true;
			break;

			//case T_DEFAULT:
			//Dont know
			//  break;


		case T_DELETE:
			if( isSgDeleteExp(node) != NULL  )
				nodeMaps = true;
			break;

		case T_DO:
			if( isSgDoWhileStmt(node) != NULL  )
				nodeMaps = true;
			break;

		case T_ELSE:
			//dont know
			break;

		case T_EXPLICIT:
			//dont know
			break;
		case T_EXPORT:
		case T_EXTERN:
			break;
		case T_FOR:

			if( isSgForStatement(node) != NULL  )
				nodeMaps = true;
			break;
		case T_FRIEND:
			//dont know
			break;
		case T_GOTO:

			if( isSgGotoStatement(node) != NULL  )
				nodeMaps = true;
			break;

		case T_IF:
			//dont know how to handle this because if if-else
			break;
		case T_INLINE:
			//dont know
			break;
		case T_MUTABLE:
			//dont know
			break;
		case T_NAMESPACE:

			if( ( isSgNamespaceAliasDeclarationStatement(node) != NULL  ) |
					(isSgNamespaceDeclarationStatement(node) != NULL )
			  )
				nodeMaps = true;
			break;
		case T_NEW:

			if( isSgNewExp(node) != NULL  )
				nodeMaps = true;
			break;
		case T_OPERATOR:
		case T_PRIVATE:
		case T_PROTECTED:
		case T_PUBLIC:
		case T_REGISTER:
		case T_REINTERPRETCAST:
			//dont know
			break;

		case T_RETURN:
			if( isSgReturnStmt(node) != NULL  )
				nodeMaps = true;
			break;

		case T_SIZEOF:
			if( isSgSizeOfOp(node) != NULL  )
				nodeMaps = true;
			break;
		case T_STATIC:
		case T_STATICCAST:
			//dont know
			break;
		case T_STRUCT:
			if( isSgClassDeclaration(node) != NULL  )
				nodeMaps = true;
			break;
		case T_SWITCH:
			if( isSgSwitchStatement(node) != NULL  )
				nodeMaps = true;
			break;
			//case T_TEMPLATE:
			//dont know
			//   break;
		case T_THIS:

			if( isSgThisExp(node) != NULL  )
				nodeMaps = true;

			break;
		case T_THROW:

			if( isSgThrowOp(node) != NULL  )
				nodeMaps = true;

			break;
		case T_TRY:

			if( isSgTryStmt(node) != NULL  )
				nodeMaps = true;

			break;
		case boost::wave::T_TYPEDEF:

			if( isSgTypedefDeclaration(node) != NULL  )
				nodeMaps = true;

			break;

		case T_TYPEID:
			if( isSgTypeIdOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_TYPENAME:
			//dont know
			break;
		case T_UNION:
			if( isSgClassDeclaration(node) != NULL  )
				nodeMaps = true;

			break;
		case T_USING:
			if( isSgUsingDeclarationStatement(node) != NULL  )
				nodeMaps = true;

			break;
		case T_VIRTUAL:
			//dont know
			break;
		case T_VOLATILE:

			//is it SgConstVolatileModifier ?
			break;

		case T_WHILE:
			if( isSgWhileStmt(node) != NULL  )
				nodeMaps = true;

			break;

		default:
			break;

	}


	//map operator token id's
	switch(token_id(tok)){
		case T_AND:
          	case T_ANDAND:
			if( isSgAndOp(node) != NULL | isSgBitAndOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_ASSIGN:
			if ( isSgAssignOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_ANDASSIGN:
			//do not know
			break;
		case T_OR:
			if ( isSgBitOrOp(node) != NULL || isSgOrOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_ORASSIGN:
			//do not know
			break;
		case T_XOR:
			if ( isSgBitXorOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_XORASSIGN:
			if ( isSgXorAssignOp(node) != NULL )
				nodeMaps = true;
			break;	
		case T_COMMA:
			if ( isSgCommaOpExp(node) != NULL )
				nodeMaps = true;
			break;
		case T_COLON:
			//dont know
			break;

		case T_DIVIDE:
			if ( isSgDivideOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_DIVIDEASSIGN:
			if ( isSgDivAssignOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_DOT:
			if ( isSgDotExp(node) != NULL )
				nodeMaps = true;
			break;


		case T_DOTSTAR:
			if ( isSgDotExp(node) != NULL )
				nodeMaps = true;
			break;


		case T_ELLIPSIS:
			//Dont know
			break;
		case T_EQUAL:
			if ( isSgEqualityOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_GREATER:
			if ( isSgGreaterThanOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_GREATEREQUAL:
			if ( isSgGreaterOrEqualOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_LEFTBRACE:
			//Dont know
			break;
		case T_LESS:
			if ( isSgLessThanOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_LESSEQUAL:
			if ( isSgLessOrEqualOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_LEFTPAREN:
			//Dont know
			break;

		case T_LEFTBRACKET:
		case T_RIGHTBRACKET:
			if ( isSgPntrArrRefExp(node) != NULL ) 
                             nodeMaps = true;
			break;
		case T_MINUS:
			if ( ( isSgSubtractOp(node) != NULL ) |
					( isSgMinusOp(node) != NULL ) )
				nodeMaps = true;
			break;

		case T_MINUSASSIGN:
			if ( isSgMinusAssignOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_MINUSMINUS:
			if ( isSgMinusMinusOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_PERCENT:
			if ( isSgModOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_PERCENTASSIGN:
			if ( isSgModAssignOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_NOT:
			if ( isSgNotOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_NOTEQUAL:
			if ( isSgNotEqualOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_OROR:
			if ( isSgOrOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_PLUS:
			if ( isSgAddOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_PLUSASSIGN:
			if ( isSgPlusAssignOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_PLUSPLUS:
			if ( isSgPlusPlusOp(node) != NULL )
				nodeMaps = true;
			break;


		case T_ARROW:
			if ( isSgArrowExp(node) != NULL )
				nodeMaps = true;
			break;

		case T_ARROWSTAR:
			if ( isSgArrowStarOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_QUESTION_MARK:
			//dont know
			break;

		case T_RIGHTBRACE:
		case T_RIGHTPAREN:
		case T_COLON_COLON:
		case T_SEMICOLON:
			//dont know
			break;
		case T_SHIFTLEFT:
			if ( isSgLshiftOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_SHIFTLEFTASSIGN:
			if ( isSgLshiftAssignOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_SHIFTRIGHT:
			if ( isSgRshiftOp(node) != NULL )
				nodeMaps = true;
			break;
		case T_SHIFTRIGHTASSIGN:
			if ( isSgRshiftAssignOp(node) != NULL )
				nodeMaps = true;
			break;

		case T_STAR:
			//dont know
                        if ( isSgMultiplyOp(node) != NULL || isSgPointerType(node) )
                          nodeMaps = true;
			break;
		case T_COMPL://~

                        if( isSgBitComplementOp(node)  != NULL  )
                          nodeMaps = true;
			//Dont know
			break;
		case T_STARASSIGN:
			if ( isSgMultAssignOp(node) != NULL )
				nodeMaps = true;

			break;
		case T_POUND_POUND:
		case T_POUND:
			//dont know
			break;

		case T_AND_ALT:
		case T_ANDASSIGN_ALT:
		case T_OR_ALT:
		case T_ORASSIGN_ALT:
		case T_XOR_ALT:
		case T_XORASSIGN_ALT:
		case T_LEFTBRACE_ALT:
		case T_LEFTBRACKET_ALT:
		case T_NOT_ALT:
		case T_NOTEQUAL_ALT:
		case T_RIGHTBRACE_ALT:
		case T_RIGHTBRACKET_ALT:
		case T_COMPL_ALT:
		case T_POUND_POUND_ALT:
		case T_POUND_ALT:
		case T_OR_TRIGRAPH:
		case T_XOR_TRIGRAPH:
		case T_LEFTBRACE_TRIGRAPH:
		case T_LEFTBRACKET_TRIGRAPH:
		case T_RIGHTBRACE_TRIGRAPH:
		case T_RIGHTBRACKET_TRIGRAPH:
		case T_COMPL_TRIGRAPH:
		case T_POUND_POUND_TRIGRAPH:
		case T_POUND_TRIGRAPH:
			//dont know
			break;
		default:
			break;
	}

	switch(token_id(tok)){
		case T_FALSE:
		case T_TRUE:
			break;
		case T_CHARLIT:
			if(SgProject::get_verbose() >= 1)
				std::cout << "char " << std::string(tok.get_value().c_str()) << std::endl;

			if( isSgCharVal(node) != NULL ){
				SgCharVal* charVal = isSgCharVal(node);
				if(SgProject::get_verbose() >= 1)
					std::cout << std::string(tok.get_value().c_str()) << std::endl;
				char tmp = charVal->get_value();
				if(SgProject::get_verbose() >= 1)
					std::cout << "From charlit: " << tmp << std::endl;

				if(("\""+std::string(&tmp)+"\"") == std::string(tok.get_value().c_str()) )
					nodeMaps = true;


			}
			break;
		case T_STRINGLIT:
			{
				if(SgProject::get_verbose() >= 1)
					std::cout << "string " <<std::string(tok.get_value().c_str()) << std::endl;

				if( isSgStringVal(node) != NULL ){
					SgStringVal* stringVal = isSgStringVal(node);
					if(SgProject::get_verbose() >= 1){
						std::cout << std::string(tok.get_value().c_str()) << std::endl;
						std::cout << "stringlit: " << stringVal->get_value() << std::endl;
					}
					if(("\""+stringVal->get_value()+"\"") == std::string(tok.get_value().c_str()) )
						nodeMaps = true;


				}
				break;
			}
			/*case V_SgWcharVal:

			  if( isSgWcharVal(node) != NULL )
			  if( std::string(isSgWcharVal(node)->get_value()) == std::string(tok.get_value().c_str()) )
			  nodeMaps = true;

			  break;*/
		default:
			break; 

	}

	if( token_id(tok) == T_IDENTIFIER ){
		if( isSgInitializedName(node) != NULL ){

			std::cout << "Identifier" << std::endl;
			SgInitializedName* initName = isSgInitializedName(node);
			std::cout <<  initName->get_name().getString() << " " << std::string(tok.get_value().c_str()) << std::endl;
			if( initName->get_name().getString() == std::string(tok.get_value().c_str()) ){
				nodeMaps = true;
			}
		}else if( isSgVarRefExp(node) != NULL ){
			SgVariableSymbol* varSymbol = isSgVarRefExp(node)->get_symbol();
			SgInitializedName* initName = varSymbol->get_declaration();
			if( initName->get_name().getString() == std::string(tok.get_value().c_str()) ){
				nodeMaps = true;
                                std::cout << "Maps:" << initName->get_name().getString() << " " << std::string(tok.get_value().c_str()) 
                                          << std::endl; 
                        }else
                          std::cout << "DONT Map:" << initName->get_name().getString() << " " << std::string(tok.get_value().c_str()) 
                                          << std::endl; 

		}else if( isSgFunctionRefExp(node) != NULL){
			SgFunctionRefExp* funcRef = isSgFunctionRefExp(node);
			SgFunctionSymbol* funcSymbol = funcRef->get_symbol_i();
			if( funcSymbol->get_declaration()->get_name().getString() == std::string(tok.get_value().c_str()) )
				nodeMaps = true;

		}else if( isSgMemberFunctionRefExp(node) != NULL){
			SgMemberFunctionRefExp* funcRef = isSgMemberFunctionRefExp(node);
			SgMemberFunctionSymbol* funcSymbol = funcRef->get_symbol();
			if( funcSymbol->get_declaration()->get_name().getString() == std::string(tok.get_value().c_str()) )
				nodeMaps = true;

		}

	}

        //Exceptions to the general rule

		switch(token_id(tok)){
                  case T_CHARLIT:
                    {
                      switch(node->variantT())
                      {
                        case V_SgCharVal:
                        case V_SgComplexVal:
                        case V_SgDoubleVal:
                        case V_SgEnumVal:
                        case V_SgFloatVal:
                        case V_SgIntVal:
                        case V_SgLongDoubleVal:
                        case V_SgLongIntVal:
                        case V_SgLongLongIntVal:
                        case V_SgShortVal:
                        case V_SgStringVal:
                        case V_SgUnsignedCharVal:
                        case V_SgUnsignedIntVal:
                        case V_SgUnsignedLongLongIntVal:
                        case V_SgUnsignedLongVal:
                        case V_SgUnsignedShortVal:
                        case V_SgWcharVal: 
                          {
                            nodeMaps=true;
                            break;
                          }
                        default:
                          break;

                      };

                      break;
                    };
                }
	switch(boost::wave::token_id(tok)){
		case boost::wave::T_CHAR:
                  {
                  if(isSgTypeChar(node) != NULL)
                    nodeMaps=true;
                  break;
                  }
		case boost::wave::T_CONST:
                  break;
		case boost::wave::T_DOUBLE: 		
                  if(isSgTypeDouble(node) != NULL)
                    nodeMaps=true;
                  break;

		case boost::wave::T_INT:
                  if(isSgTypeInt(node) != NULL)
                    nodeMaps=true;
                  break;

		case boost::wave::T_LONG:
                  if(isSgTypeLong(node) != NULL)
                    nodeMaps=true;
                  break;

		case boost::wave::T_SHORT:
                 if(isSgTypeShort(node) != NULL)
                    nodeMaps=true;
                  break;

		case boost::wave::T_SIGNED:
		case boost::wave::T_UNSIGNED:
		case boost::wave::T_VOID:
                  if(isSgTypeVoid(node) != NULL)
                    nodeMaps=true;
                  break;
		default:
			break;

	};

	return nodeMaps;
}


