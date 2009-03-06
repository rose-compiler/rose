#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H



using namespace std; 
typedef std::vector <SgNode *, std::allocator <SgNode * > > SgNodePtrVector;
extern string findPragmaStringUnionControl (string pragmaString, string unionName);
extern SgNodePtrVector findScopes (SgNode * astNode);
extern string getScopeString(SgNode* astNode);
extern SgNode* findClassDeclarationFromType (SgNodePtrVector nodeVector, SgType * sageType);
extern SgTypedefDeclaration* findTypedefFromTypeName (SgNodePtrVector nodeVector, const string sageName);
extern vector<SgNode * > findClassDeclarationsFromTypeName (SgNodePtrVector nodeVector,
				   const string sageName);
extern vector<SgNode * > 
queryNodeClassDeclarationFromTypedefName (SgNode * astNode,
					  SgNode * nameNode);
SgType* findBaseType(SgType* sageType);
vector<SgType*> typeVectorFromType(SgType* sageType);

string typeStringFromType(SgType* sageType);

class typeInterpreter {
	public: 
		//operator<<;
		//operator>>
		typeInterpreter(){};
		~typeInterpreter(){};
		vector<SgType*> typeVectorFromTypedef(SgTypedefDeclaration* typedefDeclaration);

		string typeFromTypedef(SgTypedefDeclaration* typedefDeclaration);
		
		void initialize(SgProject* project);
	
        private:
                map< SgTypedefDeclaration*, SgType*> typedefTranslationTable;
		map< SgTypedefDeclaration*, SgType*> 
		    buildTypedefTranslationTable(SgProject* project);
                SgType* findBaseType(SgType* sageType);


};

#endif
