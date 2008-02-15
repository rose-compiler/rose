/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#include "rose.h"
#include "unparser_opt.h"
#include "unparse_format.h"
#include "unparser.h"
#include "CToProMeLaUnparser.h"
#include "CToProMeLaASTAttrib.h"
#include "CStateASTAttrib.h"
#include <typeinfo>
#include <cassert>
#include "tools.h"
using namespace std;

bool CToProMeLaUnparser::toUnparse(SgStatement * node)
{
	return statementFromFile (node,getFileName());
}

CToProMeLaUnparser::CToProMeLaUnparser( std::ostream* localStream, std::string filename, Unparser_Opt info, int lineNumberToUnparse, UnparseFormatHelp *h, UnparseDelegate* repl):Unparser(localStream,filename,info,lineNumberToUnparse,h,repl),promCur(localStream)
{
	std::cout <<"MyUnparser!!!"<<std::endl;
//	promCur=*localStream;
//	(*promCur)=*localStream;
}
CToProMeLaUnparser::~CToProMeLaUnparser()
{
}

void CToProMeLaUnparser::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info)
{
	if (toConvert(stmt))// has to be promela
	{
	std::cout <<"Whilestatement"<<std::endl;
	SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
	ROSE_ASSERT(while_stmt != NULL);

	(*promCur) << "do\n\t::(";
	info.set_inConditional();
	if (toConvert(while_stmt->get_condition()))
	{
		unparseStatement(while_stmt->get_condition(), info);
	}
	else
	{
		(*promCur) << "c_expr{";
		unparseStatement(while_stmt->get_condition(), info);
		(*promCur)<<"}";
	
	}
	info.unset_inConditional();
	(*promCur) << ")->";
	if(while_stmt->get_body())
		unparseStatement(while_stmt->get_body(), info);
	else if (!info.SkipSemiColon()) { (*promCur) << ";"; }
	(*promCur)<<"\n\t::else break;\n\tod;";
	}
	else
		Unparser::unparseWhileStmt(stmt,info);
}

void CToProMeLaUnparser::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
{
	if (toConvert(stmt))// has to be promela
	{
		// DQ (12/13/2005): I don't like this implementation with the while loop...

		// printf ("Unparse if statement \n");
		SgIfStmt* if_stmt = isSgIfStmt(stmt);
		assert (if_stmt != NULL);

		while (if_stmt != NULL)
		{
			SgStatement *tmp_stmt = NULL;
			(*promCur) << "if\n\t::(";
			SgUnparse_Info testInfo(info);
			testInfo.set_SkipSemiColon();
			testInfo.set_inConditional();
			// info.set_inConditional();
			if ( (tmp_stmt = if_stmt->get_conditional()) )
			{
				if (toConvert(tmp_stmt))
				{
					
					unparseStatement(tmp_stmt, testInfo);
				}
				else
				{
					(*promCur) << "c_expr{";
					unparseStatement(tmp_stmt, testInfo);
					(*promCur) << "}";
				}
			}
			testInfo.unset_inConditional();
			(*promCur) << ")->";

			if ( (tmp_stmt = if_stmt->get_true_body()) )
			{
				// printf ("Unparse the if true body \n");
				// (*promCur) << "\n/* Unparse the if true body */ \n";
				unparseStatement(tmp_stmt, info);
				// (*promCur) << "\n/* DONE: Unparse the if true body */ \n";
			}
			// in promela there must be an elso or the if will block if the conditional is false
			(*promCur) <<"\t:: else ->";

			if ( (tmp_stmt = if_stmt->get_false_body()) )
			{
//				(*promCur).format(if_stmt, info, FORMAT_BEFORE_STMT);
//				(*promCur) << "else ";
				if_stmt = isSgIfStmt(tmp_stmt);
				if (if_stmt == NULL)
				{
					//check if the else is empty
					cout <<"else statement is: "<<tmp_stmt->unparseToString()<<endl;
					if (isSgBasicBlock(tmp_stmt) && isSgBasicBlock(tmp_stmt)->get_statements().size()>0)
					unparseStatement(tmp_stmt, info);
					else
					(*promCur)<<"skip\n";
				}
				else
					unparseIfStmt(tmp_stmt, info);
			}
			else
			{
				(*promCur)<<"skip\n";
			}
			(*promCur) <<"\n\tfi\n";
			
		}
	}
	else
	{
		Unparser::unparseIfStmt(stmt,info);
	}
}
void CToProMeLaUnparser::unparseBasicBlockStmt (SgStatement* node, SgUnparse_Info& info)
{
//	cout <<"processing Basic Block
	SgBasicBlock *block=isSgBasicBlock(node);
	if (toConvert(block))
	{
		// if the block contains more than one stmt it hase to have curly-brackets
		bool multiStmtBlock=false;
		// .. or if it is the top most block in a functions
		if (isSgFunctionDefinition(node->get_parent()))
		{
			multiStmtBlock=true;
		}
		else if (block->get_statements().size()>1)
		{
			multiStmtBlock=true;
		}
		// now start unparsing
		
		
		bool inCBlock=false;
		// setup block
		if (multiStmtBlock)
		{
			(*promCur)<<"{\n";
		}
		
		// if the block is empty, insert a skip statement
		if (block->get_statements().size()==0) (*promCur)<<"skip\n";
		// else ..
		SgStatementPtrList stmts=block->get_statements();
		// for all statements in this block
		for (SgStatementPtrList::iterator i=stmts.begin();i!=stmts.end();i++)
		{
			bool writeSemiColon=false;
			// this is original c-code
			if (!toConvert(*i))
			{
				// if we are not in a c_code-block open one
				if (!inCBlock)
				{
					(*promCur)<<"c_code\n{\n";
					inCBlock=true;
				}

				if (toUnparse(isSgStatement(*i))){
				cout <<"unparsing c-statement of type "<<(*i)->class_name()<<": "<<(*i)->unparseToString(&info)<<endl;
				unparseStatement(*i,info);
				writeSemiColon=true;
				}
				//	(*promCur)<<"}";
			}
			else
			{
				// if we are in a c-block close is
				if (inCBlock)
				{
					(*promCur)<<"\n};";
					inCBlock=false;
				}
/*				SgStatementPtrList::iterator tmpIt=i;
				if (tmpIt==stmts.end())
					info.set_SkipSemiColon ();*/
					
				unparseStatement(*i,info);
				if (!isSgLabelStatement(*i))
				
				writeSemiColon=true;
				(*promCur)<<"\n";
			}

			SgStatementPtrList::iterator tmpIt=i;
			tmpIt++;
			// are we at the end of this block
			if (tmpIt==stmts.end())
			{
				// if there is an open c-block close is
				if (inCBlock)
				{
					(*promCur)<<"\n}";
				}
			}
			// not the last statement
			else
			{
				// if we are not in a c-block we have to concatenate the promela statements with a ;
				if (!inCBlock && writeSemiColon)
					(*promCur)<<";\n";			
			}
		}
		// ending of block
		if (multiStmtBlock)
		{
			(*promCur)<<"}\n";
		}
	}
	
/*	if (toConvert(node))
	{
		std::cout<<"unparsing block to promela"<<endl;
		SgStatementPtrList stmts=block->get_statements();
		if (stmts.size()==0) (*promCur)<<"skip;\n";
		for(SgStatementPtrList::iterator i=stmts.begin();i!=stmts.end();i++)
		{
			SgStatement * stmt=isSgStatement(*i);
			unparseStatement(stmt,info);
			(*promCur)<<";\n";
		}
		
	}*/
	else
	{
		Unparser::unparseBasicBlockStmt(node,info);
	}
	
}


void CToProMeLaUnparser::unparseGlobalStmt       (SgStatement* stmt, SgUnparse_Info& info)
{
	// only one global scope is in the main file, which will receive global defs and decls
	if (toConvert(stmt))
	{	
		cout <<"processing global scope for main-PROMELA-file:\n";
		SgGlobal* globalScope = isSgGlobal(stmt);
		ROSE_ASSERT(globalScope != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
		printf ("\n\n Unparsing the global Scope \n\n \n");
#endif
		(*promCur)<<"c_decl{typedef enum{false=0,true} bool;}"<<endl;
		
		// Setup an iterator to go through all the statements in the top scope of the file.
		SgDeclarationStatementPtrList & globalStatementList = globalScope->get_declarations();
		SgDeclarationStatementPtrList::iterator statementIterator = globalStatementList.begin();
		while ( statementIterator != globalStatementList.end() )
		{
			SgStatement* currentStatement = *statementIterator;
			ROSE_ASSERT(currentStatement != NULL);

			bool unparseStatementIntoSourceFile = statementFromFile (currentStatement,getFileName());

			if (unparseStatementIntoSourceFile == true)
			{
				bool toConvertFlag=toConvert(currentStatement);
				if (!toConvertFlag)
				{
					(*promCur)<<"c_code\n{\n";
				}
					
					unparseStatement(currentStatement, info);
				if (!toConvertFlag)
				{
					(*promCur)<<"\n}\n";
				}
			}
			
			else
			{
			}
			// Go to the next statement
			statementIterator++;
		}
		cur.insert_newline(1);
	}
	else
		Unparser::unparseGlobalStmt(stmt,info);
}

void CToProMeLaUnparser::unparseFuncDefnStmt(SgStatement* node,SgUnparse_Info& info)
{
/*	bool tc=toConvert(node);
	cout <<"tc = "<<tc<<endl;
	cout <<"attriExists " <<node->attributeExists(C2PASTATTRIBSTR)<<endl;
	cout <<"AttribName is "<<C2PASTATTRIBSTR<<" and its return is "<<node->getAttribute(C2PASTATTRIBSTR)<<endl;

	cout <<"typeid "<<typeid(node->getAttribute(C2PASTATTRIBSTR)).name()<<endl;
	AstAttribute *aa =node->getAttribute(C2PASTATTRIBSTR);
	C2PASTAttribute * attr=(C2PASTAttribute*)(node->getAttribute(C2PASTATTRIBSTR));
	
	cout <<"the converted attri is "<<attr->toString()<<endl;
*/										
	if (toConvert(node))
//	if(node->attributeExists(C2PASTATTRIBSTR))
	{
		SgFunctionDeclaration * dec;
		SgFunctionDefinition * def;
		cout <<"converting function def to promela function"<<endl;
		def=isSgFunctionDefinition(node);
		dec=def->get_declaration();
		//check if there is the CStateASTAttrib for this proc
		if (nodeHasCStateAttrib(node))
		{
			cout << "unparsing state vector definition"<<endl;
			CStateASTAttrib * attr=getCStateAttrib(node);
			std::vector<SgInitializedName *> iniNameList=attr->getIniNameList();
			for (int i=0;i<iniNameList.size();i++)
			{
				// new idea, just unparse the declaration -> but kick the initinalizer
				                                                             
				SgDeclarationStatement * decl;
				SgInitializedName * iniName=iniNameList[i];
				decl=iniName->get_declaration();
				
				SgInitializer* ini=iniName->get_initializer();
				iniName->set_initializer(NULL);
				SgUnparse_Info myInfo=info;
				myInfo.set_SkipSemiColon ();
				(*promCur)<<"c_state \"";
				unparseVarDeclStmt(decl,myInfo);
/*


				
				SgName name=iniName->get_name ();
				SgType *type=iniName->get_type () ;
				SgName typeName=type->get_mangled ();

				
				switch (type->variant())
				{
					case T_UNKNOWN:            (*promCur) << get_type_name(type) << " ";(*promCur)<<" "<<name.getString();          break;
					case T_CHAR:
					case T_SIGNED_CHAR:
					case T_UNSIGNED_CHAR:
					case T_SHORT:
					case T_SIGNED_SHORT:
					case T_UNSIGNED_SHORT:
					case T_INT:
					case T_SIGNED_INT:
					case T_UNSIGNED_INT:
					case T_LONG:
					case T_SIGNED_LONG:
					case T_UNSIGNED_LONG:
					case T_VOID:
					case T_GLOBAL_VOID:
					case T_WCHAR:
					case T_FLOAT:
					case T_DOUBLE:
					case T_LONG_LONG:
					case T_UNSIGNED_LONG_LONG:
					case T_LONG_DOUBLE:
					case T_STRING:
					case T_BOOL:
					case T_COMPLEX:
					case T_IMAGINARY:
					case T_DEFAULT:
					case T_ELLIPSE:
					{
						if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )
						{
					; 
						}
						else
						{ (*promCur) << get_type_name(type)<<" "<<name.getString(); }
						break;
					}
					case T_POINTER:
						// this is dangerous, but ok... so far...
						unparsePointerType(type, info);
						(*promCur)<<" "<<name.getString();
					break;
					case T_MEMBER_POINTER:   unparseMemberPointerType(type, info);    break;
					case T_REFERENCE:        
						unparseReferenceType(type, info);        
						(*promCur)<<" "<<name.getString();
					break;
																		 // case T_NAME:               unparseNameType(type, info);             break;
					case T_CLASS:            
						unparseClassType(type, info);            
						(*promCur)<<" "<<name.getString();
					break;
					case T_ENUM:             
						unparseEnumType(type, info);
						(*promCur)<<" "<<name.getString();
					break;
					case T_TYPEDEF:          
						unparseTypedefType(type, info);          
						(*promCur)<<" "<<name.getString();
					break;
					case T_ARRAY:
						unparseArrayType(type, info);       
					break;
					case T_MODIFIER:
						unparseModifierType(type, info);
						(*promCur)<<" "<<name.getString();
					break;
#if 0
					{
						string name=get_type_name(type);
						// if it has leading colons, get rid of them, they hurt!!!
						while(name.find("::",0)==0)
						{
							name=name.substr(2,name.length());
						}
					cout <<"can't handle those type"<<endl;
					(*promCur) <<name;
					}
				break;
#endif
					// this is bad, not supported....
					case T_FUNCTION://         unparseFunctionType(type, info);         break;
					case T_QUALIFIED_NAME: //  unparseQualifiedNameType(type, info);    break;
					case T_PARTIAL_FUNCTION:
					case T_MEMBERFUNCTION:  // unparseMemberFunctionType(type, info);   break;
						cerr<<"ERROR: promela does not understand this type:"<<endl;
						ROSE_ASSERT(false);
					break;

					default:
					{
						printf("Error: Unparser::unparseType(): Default case reached in switch: Unknown type \n");
						ROSE_ASSERT(false);
						break;
					}
				}
*/
				(*promCur)<<"\" \"Local "<<dec->get_name().getString()<<"\""<<endl;

			}
		}

		
		// this is a function
		(*promCur) <<"active proctype "<<dec->get_name().getString()<<"()";
		unparseStatement(def->get_body(),info);
		
	}
	else
	{
		cout <<"Unparsing C-FuncDefStmt"<<endl;
		Unparser::unparseFuncDefnStmt(node,info);
	}
}


void CToProMeLaUnparser::unparseAttachedPreprocessingInfo(SgStatement* stmt, SgUnparse_Info& info,                    PreprocessingInfo::RelativePositionType whereToUnparse)
{
	Unparser::unparseAttachedPreprocessingInfo(stmt,info,whereToUnparse);
}	






// Later we might want to move this to the SgProject or SgFile support class (generated by ROSETTA)
void unparsePromelaFile ( SgFile* file, UnparseFormatHelp *unparseHelp, UnparseDelegate* unparseDelegate )
   {
  // Call the unparser mechanism
     ROSE_ASSERT(file != NULL);
  // DQ (4/22/2006): This can be true when the "-E" option is used, but then we should not have called unparse()!
     ROSE_ASSERT(file->get_skip_unparse() == false);

  // If we did unparse an intermediate file then we want to compile that 
  // file instead of the original source file.
     if (file->get_unparse_output_filename().empty() == true)
        {
          std::string outputFilename = "rose_" + file->get_sourceFileNameWithoutPath()+".pml";
          file->set_unparse_output_filename(outputFilename);
          ROSE_ASSERT (file->get_unparse_output_filename().empty() == false);
       // printf ("Inside of SgFile::unparse(UnparseFormatHelp*,UnparseDelegate*) outputFilename = %s \n",outputFilename.c_str());
        }

     if (file->get_skip_unparse() == true)
        {
       // MS: commented out the following output
        }
       else
        {
       // Open the file where we will put the generated code
          string outputFilename = get_output_filename(*file);
					if (toConvert(file))
					{
						outputFilename+=".pml";
					}
          fstream ROSE_OutputFile(outputFilename.c_str(),ios::out);
       // all options are now defined to be false. When these options can be passed in
       // from the prompt, these options will be set accordingly.
          bool _auto                         = false;
          bool linefile                      = false;
          bool useOverloadedOperators        = false;
          bool num                           = false;

       // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
          bool _this                         = true;

          bool caststring                    = false;
          bool _debug                        = false;
          bool _class                        = false;
          bool _forced_transformation_format = false;

       // control unparsing of include files into the source file (default is false)
          bool _unparse_includes             = file->get_unparse_includes();

          Unparser_Opt roseOptions( _auto,
                                    linefile,
                                    useOverloadedOperators,
                                    num,
                                    _this,
                                    caststring,
                                    _debug,
                                    _class,
                                    _forced_transformation_format,
                                    _unparse_includes );

          int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

          CToProMeLaUnparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, lineNumber, unparseHelp, unparseDelegate );

       // Location to turn on unparser specific debugging data that shows up in the output file
       // This prevents the unparsed output file from compiling properly!
       // ROSE_DEBUG = 0;

       // DQ (12/5/2006): Output information that can be used to colorize properties of generated code (useful for debugging).
          roseUnparser.set_embedColorCodesInGeneratedCode ( file->get_embedColorCodesInGeneratedCode() );
          roseUnparser.set_generateSourcePositionCodes    ( file->get_generateSourcePositionCodes() );

       // information that is passed down through the tree (inherited attribute)
       // SgUnparse_Info inheritedAttributeInfo (NO_UNPARSE_INFO);
          SgUnparse_Info inheritedAttributeInfo;

       // Call member function to start the unparsing process
       // roseUnparser.run_unparser();
          roseUnparser.unparseFile(file,inheritedAttributeInfo);

       // And finally we need to close the file (to flush everything out!)
          ROSE_OutputFile.close();
        }
   }


void unparsePromelaProject(SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     ROSE_ASSERT(project != NULL);
     for (int i=0; i < project->numberOfFiles(); ++i)
        {
          SgFile & file = project->get_file(i);
          unparsePromelaFile(&file,unparseFormatHelp,unparseDelegate);
        }
   }



void CToProMeLaUnparser::unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info)
{
	Unparser::unparsePragmaDeclStmt(stmt,info);
}
