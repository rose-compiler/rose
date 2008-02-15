#ifndef MAP_SCOPES_IN_TOKEN_STREAM_AND_AST
#define MAP_SCOPES_IN_TOKEN_STREAM_AND_AST

#include "rose.h"
#include <functional>
#include "linearizeAST.h"
#include <vector>
#include <boost/tuple/tuple.hpp>



struct separator{
	int begin_pos;
	int end_pos;
	std::vector<separator> sub_separators;

	separator(int bp, int ep, std::vector<separator> ss );
 	separator(int bp, int ep);

	void printValues(PreprocessingInfo::token_container& tokenStream, int counter = 0);

	void printValues(std::vector<SgNode*>& linearizedAST, int counter = 0 );
};


separator* mapSeparatorsAST(std::vector<SgNode*>& linearizedAST);

separator* mapSeparatorsTokenStream(PreprocessingInfo::token_container& tokenStream);

#endif
