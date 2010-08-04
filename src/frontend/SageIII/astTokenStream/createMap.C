// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "createMap.h"
#include <algorithm>
using namespace std;

// #ifndef USE_ROSE
// If we are using ROSE to compile ROSE source code then the Wave support is not present.
createMap::createMap(std::vector<SgNode*>& linAST, token_container& tokStream)
   : linearizedAST(linAST), tokenStream(tokStream)
   {
     buildMaps();
   }
// #endif


void
createMap::buildMaps()
   {
#ifndef USE_ROSE
// If we are using ROSE to compile ROSE source code then the Wave support is not present.
     separator* astScopes         = mapSeparatorsAST(linearizedAST);
     separator* curlyBraceScopes  = mapSeparatorsTokenStream(tokenStream);

     astScopes->outputValues(linearizedAST);
     curlyBraceScopes->outputValues(tokenStream);
     internalMatchBetweenASTandTokenStreamSeparator(astScopes,curlyBraceScopes);
#endif
   }

void
createMap::internalMatchBetweenASTandTokenStreamSeparator(separator*
                                                          ast, separator* token){

#ifndef USE_ROSE
// If we are using ROSE to compile ROSE source code then the Wave support is not present.
    ROSE_ASSERT(ast != NULL);
    ROSE_ASSERT(token != NULL);
    vector<separator>& astScopeMapForThisScope   = ast->sub_separators;
    vector<separator>& tokenBraceMapForThisScope = token->sub_separators;


    //std::cout << "SubSeparator AST size: " << astScopeMapForThisScope.size() << std::endl;
    //std::cout << "SubSeparator token size: " << tokenBraceMapForThisScope.size() << std::endl;
    //Find the AST scopes that we want to ignore
    vector<separator> astScopesToIgnore;
    for(vector<separator>::iterator sep_it = astScopeMapForThisScope.begin(); 
        sep_it != astScopeMapForThisScope.end(); ++sep_it){
        SgNode* currentScope = linearizedAST[sep_it->begin_pos];

        if( currentScope->get_file_info()->isCompilerGenerated() == true){
            astScopesToIgnore.push_back(*sep_it);
            //std::cout << "IGNORING NODE" << std::endl;
            //sep_it->printValues(linearizedAST,5);
        };
    }

    //Erase from the vector the elements we want to ignore
    for(vector<separator>::iterator eq_it = astScopesToIgnore.begin(); 
        eq_it != astScopesToIgnore.end(); ++eq_it){
        for(vector<separator>::iterator sep_it = astScopeMapForThisScope.begin(); 
            sep_it != astScopeMapForThisScope.end(); ++sep_it){

            if(eq_it->begin_pos == sep_it->begin_pos){
                astScopeMapForThisScope.erase(sep_it);
                break;
            }
        }


    }

    //Find the tokens we want to ignore
    vector<separator> tokensToIgnore;
    for(vector<separator>::iterator sep_it = tokenBraceMapForThisScope.begin(); 
        sep_it != tokenBraceMapForThisScope.end(); ++sep_it){

        if( tokenStream[sep_it->begin_pos].get_position().get_file().find("rose_edg_required_macros_and_functions.h") != string::npos ){
            tokensToIgnore.push_back(*sep_it);
            //std::cout << "IGNORING" << std::endl;
            //sep_it->printValues(tokenStream,5);
        }
        //else
        //if(tokenStream[sep_it->end_pos].get_position().get_file().find("rose_edg_required_macros_and_functions.h") == string::npos ){
        //    tokensToIgnore.push_back(*sep_it);
        // };
    }

    for(vector<separator>::iterator eq_it = tokensToIgnore.begin(); 
        eq_it != tokensToIgnore.end(); ++eq_it){

        for(vector<separator>::iterator sep_it = tokenBraceMapForThisScope.begin(); 
            sep_it != tokenBraceMapForThisScope.end(); ++sep_it){

            if( 
               (eq_it->begin_pos == sep_it->begin_pos)

              ){
                tokenBraceMapForThisScope.erase(sep_it);
                break;
            }
        }
    }



    //std::cout << "Ignoring " << astScopesToIgnore.size() << " nodes." <<std::endl;
    //std::cout << "Ignoring " << tokensToIgnore.size() << " tokens." <<std::endl;
    //Create the map between nodes and tokens
    //std::cout <<   (tokenBraceMapForThisScope.size()-tokensToIgnore.size()) << " : " 
    //n    <<   (astScopeMapForThisScope.size()-astScopesToIgnore.size()) << std::endl;   
    if((tokenBraceMapForThisScope.size()) ==
       (astScopeMapForThisScope.size())){

        vector<separator>::iterator ast_it = 
            astScopeMapForThisScope.begin();
        for(vector<separator>::iterator sep_it = tokenBraceMapForThisScope.begin(); 
            sep_it != tokenBraceMapForThisScope.end(); ++sep_it){




            //sep_it->printValues(tokenStream,20);
            //ast_it->printValues(linearizedAST,20);
            ROSE_ASSERT(ast_it !=astScopeMapForThisScope.end() ); 


            //add node to the map
            //sep_it->printValues(tokenStream,20);
            SgNode* currentScope =
                linearizedAST[ast_it->begin_pos];

            int     currentTokenBeginPos = sep_it->begin_pos;
            int     currentTokenEndPos   = sep_it->end_pos;
            nodeToTokenMap[currentScope] = std::pair<int,int>(currentTokenBeginPos,currentTokenEndPos)  ;
            tokenToNodeMap[ std::pair<int,int>(currentTokenBeginPos,currentTokenEndPos) ] = currentScope;

            internalMatchBetweenASTandTokenStreamSeparator(&(*ast_it), &(*sep_it));    

            //Next ast_token
            ++ast_it;
        }
    }

#endif
}


// #ifndef USE_ROSE
// If we are using ROSE to compile ROSE source code then the Wave support is not present.
//get the token stream provided in the constructor
token_container& createMap::get_tokenStream()
{
    return tokenStream;
};
// #endif

//get the linearized AST provided in the constructor
std::vector<SgNode*>&               createMap::get_linearizedAST()
{
    return linearizedAST;
};

//the integer int refers to an index in the tokenStream vector
std::map<SgNode*, std::pair<int,int> > createMap::get_mapFromNodeToToken()
{
    return nodeToTokenMap;

};
std::map<std::pair<int,int>,SgNode*> createMap::get_mapFromTokenToNode()
{
    return tokenToNodeMap;
};

void 
createMap::printNodeToTokenMap()
   {
#ifndef USE_ROSE
  // If we are using ROSE to compile ROSE source code then the Wave support is not present.
    std::cout << "BEGIN printing node to token map" << std::endl;
    std::cout << "SIZE: " << nodeToTokenMap.size() << std::endl;
    for(map<SgNode*,std::pair<int,int> >::iterator map_it = nodeToTokenMap.begin();
        map_it != nodeToTokenMap.end(); ++map_it   ){
        Sg_File_Info* fileInfo = map_it->first->get_file_info();
        token_type currentBeginToken = tokenStream[map_it->second.first];
        token_type currentEndToken   = tokenStream[map_it->second.second];

        string nodeFilename  = fileInfo->get_filenameString();
        int nodeLine   = fileInfo->get_line();
        int nodeColumn = fileInfo->get_col();  

        std::cout << "Position       of node  is filename " << nodeFilename << " line "  
            << nodeLine << " column " << nodeColumn << std::endl; 


        string beginTokenFilename = currentBeginToken.get_position().get_file().c_str();
        int beginTokenLine   = currentBeginToken.get_position().get_line();
        int beginTokenColumn = currentBeginToken.get_position().get_column();

        std::cout << "Begin position of token is filename " << beginTokenFilename << " line "  
            << beginTokenLine << " column " << beginTokenColumn << std::endl; 

        string endTokenFilename = currentEndToken.get_position().get_file().c_str();
        int endTokenLine   = currentEndToken.get_position().get_line();
        int endTokenColumn = currentEndToken.get_position().get_column();

        std::cout << "End   position of token is filename " << endTokenFilename << " line "  
            << endTokenLine << " column " << endTokenColumn << std::endl; 

        std::cout << std::endl;
    };
    std::cout << "END printing node to token map" << std::endl;   
#endif
   }

void 
createMap::printTokenToNodeMap()
   {
  // Nothing implemented here.
   }


