#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H


//#include "rose.h"

// using namespace std; 
typedef std::vector <SgNode *, std::allocator <SgNode * > > SgNodePtrVector;
extern std::string findPragmaStringUnionControl (std::string pragmaString, std::string unionName);
extern SgNodePtrVector findScopes (SgNode * astNode);
extern std::string getScopeString(SgNode* astNode);
extern SgNode* findClassDeclarationFromType (SgNodePtrVector nodeVector, SgType * sageType);
extern SgTypedefDeclaration* findTypedefFromTypeName (SgNodePtrVector nodeVector, const std::string sageName);
extern Rose_STL_Container<SgNode * > findClassDeclarationsFromTypeName (SgNodePtrVector nodeVector,
				   const std::string sageName);
extern Rose_STL_Container<SgNode * > queryNodeClassDeclarationFromTypedefName (SgNode * astNode,
					  SgNode * nameNode);
SgType* findBaseType(SgType* sageType);
std::vector<SgType*> typeVectorFromType(SgType* sageType);
std::string typeStringFromType(SgType* sageType);

class typeInterpreter {
	public: 
		//operator<<;
		//operator>>
		typeInterpreter(){};
		~typeInterpreter(){};
		std::vector<SgType*> typeVectorFromTypedef(SgTypedefDeclaration* typedefDeclaration);

		std::string typeFromTypedef(SgTypedefDeclaration* typedefDeclaration);
		
		void initialize(SgProject* project);
	
        private:
		std::map< SgTypedefDeclaration*, SgType*> typedefTranslationTable;
		std::map< SgTypedefDeclaration*, SgType*> 
		    buildTypedefTranslationTable(SgProject* project);
                SgType* findBaseType(SgType* sageType);


};

#endif
