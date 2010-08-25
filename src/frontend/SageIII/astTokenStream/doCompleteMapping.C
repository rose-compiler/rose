// Example ROSE Translator: used within ROSE/tutorial
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "doCompleteMapping.h"



using namespace std;
using namespace boost;

separator::separator(int bp, int ep){
        begin_pos = bp;
        end_pos   = ep;
}

separator::separator(int bp, int ep, std::vector<separator> ss ){
        begin_pos = bp;
        end_pos   = ep;
        sub_separators = ss;
} 

// #ifndef USE_ROSE
// If we are using ROSE to compile ROSE source code then the Wave support is not present.
void separator::outputValues(token_container& tokenStream, int counter ){
        string prefix = "";
        for(int i = 0; i < counter; i++){
                prefix += " ";

        }
        cout << prefix + "Left brace " <<  tokenStream[begin_pos].get_position().get_file() << " "
        <<  tokenStream[begin_pos].get_position().get_line() 
                << " matches " << tokenStream[end_pos].get_position().get_line() << endl;


        for(std::vector<separator>::iterator sub_it= sub_separators.begin(); 
                        sub_it != sub_separators.end(); ++sub_it  ){
                sub_it->outputValues(tokenStream,counter+1);
        }
}
// #endif

void separator::outputValues(std::vector<SgNode*>& linearizedAST, int counter ){
        string prefix = "";
        for(int i = 0; i < counter; i++){
                prefix += " ";

        }
        cout << prefix  << linearizedAST[begin_pos]->class_name() 
                << " ROSE Scope Statement BEGIN" << linearizedAST[begin_pos]->get_file_info()->get_filenameString() << " at position in the linearized AST: "
        << linearizedAST[begin_pos]->get_file_info()->get_line() << endl;
   
                //<< " matches " << linearizedAST[end_pos]->get_file_info()->get_line() << endl;

    if( linearizedAST[begin_pos]->get_file_info()->isCompilerGenerated() == true)
    cout << " is compiler generated" << std::endl;
        for(std::vector<separator>::iterator sub_it= sub_separators.begin(); 
                        sub_it != sub_separators.end(); ++sub_it  ){
                sub_it->outputValues(linearizedAST,counter+1);
        }

}



separator* mapSeparatorsAST(std::vector<SgNode*>& linearizedAST){

    separator* globalScope = new separator(0,0);


        //Find all Scope Statements in linearized AST
        std::vector<int> curlyBraceStackAST;
        SgNode* lastScopeStmt = NULL;
        std::cout << "Size of linearized AST: " << linearizedAST.size() << endl;

        vector<vector<separator> > tmp_curlyBraceStackAST;
        tmp_curlyBraceStackAST.push_back( vector<separator>()  );

    //Some scopes have a basic block attached to them. I want to ignore those
    //basic blocks 
    std::vector<SgNode*> ignoreScopes;


        for(unsigned int i = 0; i < linearizedAST.size(); i++){

                if( ( isSgScopeStatement(linearizedAST[i]) != NULL ) &&
          ( isSgFunctionDefinition(linearizedAST[i]) == NULL ) &&
          ( isSgSwitchStatement(linearizedAST[i]) == NULL ) &&
            ( isSgIfStmt(linearizedAST[i]) == NULL ) &&
            ( isSgWhileStmt(linearizedAST[i]) == NULL)  &&
            ( find(ignoreScopes.begin(), ignoreScopes.end(),linearizedAST[i]) == ignoreScopes.end() )  
            //&& ( isSgScopeStatement(linearizedAST[i])->get_file_info()->get_filenameString().find("rose_edg_required_macros_and_functions.h") == string::npos)
                                //( isSgBasicBlock(linearizedAST[i])     == NULL ) //&&  
                                //( (isSgScopeStatement(linearizedAST[i])->get_file_info()->isCompilerGenerated() == false) ||
                                  //Because of a bug which marks namespace definition statements in rose_edg_required_macros_and_functions.h
                                  //as compiler generated even when they are not I am putting in this fix
                                  //Can a namespace be compiler generated?
                                //  (isSgNamespaceDefinitionStatement(linearizedAST[i]) != NULL )
                                ///)
                  ){
                        if(    ( lastScopeStmt != NULL )
                                        && ( lastScopeStmt == linearizedAST[i] )
                          ){
                                int begin_sep_pos = curlyBraceStackAST[curlyBraceStackAST.size()-1];
                                int end_sep_pos   = i;

                                tmp_curlyBraceStackAST[curlyBraceStackAST.size()-1].push_back( 
                                                separator(begin_sep_pos,end_sep_pos, 
                                                        tmp_curlyBraceStackAST[curlyBraceStackAST.size()]) );
                                tmp_curlyBraceStackAST.pop_back();

                                //Make sure that the new last scope statement is the one before the previous one
                                curlyBraceStackAST.pop_back();

                                begin_sep_pos = curlyBraceStackAST[curlyBraceStackAST.size()-1];

                                lastScopeStmt = linearizedAST[begin_sep_pos];
                        }else{
                switch(linearizedAST[i]->variantT()){
                  case V_SgFunctionDefinition:
                    ignoreScopes.push_back( isSgFunctionDefinition(linearizedAST[i])->get_body()  );
                    break;
                  case V_SgSwitchStatement:
                    ignoreScopes.push_back( isSgSwitchStatement(linearizedAST[i])->get_body()  );
                    break;
                  default:
                    break;
                      
                }
                                curlyBraceStackAST.push_back(i);
                                if( tmp_curlyBraceStackAST.size() == curlyBraceStackAST.size()  )
                                        tmp_curlyBraceStackAST.push_back( vector<separator>() );
                                else if( tmp_curlyBraceStackAST.size() != ( curlyBraceStackAST.size()+1 ) )
                                        ROSE_ASSERT(false);
                                lastScopeStmt = linearizedAST[i];
                        }
                }
        }

        ROSE_ASSERT(tmp_curlyBraceStackAST.size()==1);
        ROSE_ASSERT(tmp_curlyBraceStackAST[0].size()==1);
        globalScope->sub_separators =   (tmp_curlyBraceStackAST[0])[0].sub_separators;

        return globalScope; 

}




// #ifndef USE_ROSE
// If we are using ROSE to compile ROSE source code then the Wave support is not present.
separator* mapSeparatorsTokenStream(token_container& tokenStream){

        separator* globalScope = new separator(0,0);

        std::vector<separator > curlyBraceMatchStackTokenStream;

        //Match all Left and Right braces in the tokenStream
        std::vector<int> curlyLeftBraceStackTokenStream;
        //stack of the separators scopes we are currently in

        //Each '{' is assigned an integer level. When the matching '}' is
        //found an instance of the class Separator is created. What the variable
        //tmp_stackOfSeparators represents is that a scope may contain several
        //other scope which may contain other scopes again.
        std::vector<std::vector<separator> > tmp_stackOfSeparators;
        tmp_stackOfSeparators.push_back( vector<separator>() );

        for(unsigned int i=0; i < tokenStream.size() ; i++)
        {
                if( (boost::wave::token_id(tokenStream[i]) == boost::wave::T_LEFTBRACE)
                  ){
                        curlyLeftBraceStackTokenStream.push_back(i);

                        if( tmp_stackOfSeparators.size() == curlyLeftBraceStackTokenStream.size()  )
                                tmp_stackOfSeparators.push_back( vector<separator>() );
                        else if( tmp_stackOfSeparators.size() != ( curlyLeftBraceStackTokenStream.size()+1 ) )
                                ROSE_ASSERT(false);

                }else if( boost::wave::token_id(tokenStream[i]) == boost::wave::T_RIGHTBRACE )
                {

                        int begin_sep_pos = curlyLeftBraceStackTokenStream[curlyLeftBraceStackTokenStream.size()-1];
                        int end_sep_pos   = i;

                        //cout << " Left brace " << tokenStream[begin_sep_pos].get_position().get_line() 
                        //     << " matches " << tokenStream[end_sep_pos].get_position().get_line() << endl;
                        tmp_stackOfSeparators[curlyLeftBraceStackTokenStream.size()-1].push_back( 
                                        separator(begin_sep_pos,end_sep_pos, 
                                                tmp_stackOfSeparators[curlyLeftBraceStackTokenStream.size()]) );
                        tmp_stackOfSeparators.pop_back();

                        curlyLeftBraceStackTokenStream.pop_back();
                }
        }
        //Match scope statements in the AST to left and right braces in the token stream
        ROSE_ASSERT(tmp_stackOfSeparators.size()==1);

        globalScope->sub_separators = tmp_stackOfSeparators[0];

        return globalScope; 
}
// #endif


