#ifndef ROSE_BABEL_PROGRAM_TRANSFORMATION_H
#define ROSE_BABEL_PROGRAM_TRANSFORMATION_H

#include "transformationSupport.h"

class BabelTransformationSynthesizedAttributeType
{
        public:
                // These are a list of the classes declared in the application. Later we will define a more
                // complex representation of the class which will include the member function declarations
                // and all for their information required to generate each entry in a SIDL file.
                list<string> sidlDeclarationList;
                bool baseTypeNeeded;
        public:

                // ########################################
                // Access functions for global declarations
                // ########################################
                list<string> getSIDLDeclaraionStringList() const
                { return sidlDeclarationList; };

                void setSIDLDeclarationStringList ( list<string> inputList )
                { sidlDeclarationList = inputList; };

                void addSIDLDeclarationString ( string X )
                {
                        sidlDeclarationList.push_back(X);
                };

                const string getSIDLDeclarationString () const
                {
                        // Build a single string from the strings in the list
                        string sidlString;
                        list<string>::const_iterator i;
                        for (i = sidlDeclarationList.begin(); i != sidlDeclarationList.end(); i++)
                        {
                                // printf ("sidlString = \n%s\n",(*i).c_str());
                                sidlString += *i;
                        }
                        
                        return sidlString;
                };

                // ##############################################
                // Function used accumulate variable declarations
                // ##############################################
                BabelTransformationSynthesizedAttributeType & operator+=
                        ( const BabelTransformationSynthesizedAttributeType & X )
                        {
                                // sidlDeclarationList.merge(X.getSIDLDeclaraionStringList());
                                list<string> list = X.getSIDLDeclaraionStringList();
                                sidlDeclarationList.splice(sidlDeclarationList.end(), list);
                                //sidlDeclarationList.merge(list);
                                return *this;
                        }
};

class overloadInformation{
	int order;
	int count;

	vector<SgType*> importantTypes;

	public:
		overloadInformation(int a, int b)
		{
			order = a;
			count = b;
		}

		void setTypes(vector<SgType*> a)
		{
			importantTypes.insert(importantTypes.end(),a.begin(),a.end());
		}

		vector<SgType*> get_types(void)
		{
			return importantTypes;
		}
		
		int get_order(void)
		{
			return order;
		}

		int get_count(void)
		{
			return count;
		}

		bool unique(void)
		{
			return importantTypes.empty();
		}

};

class SIDL_TreeTraversal : public SgBottomUpProcessing<BabelTransformationSynthesizedAttributeType>
{
	private:
		string constructorName;
	public:
		SIDL_TreeTraversal(string ctorName) : constructorName(ctorName) {}

		// Functions required by the global tree traversal mechanism
		BabelTransformationSynthesizedAttributeType evaluateSynthesizedAttribute ( SgNode* astNode, SubTreeSynthesizedAttributes synthesizedAttributeList );

		// Support for mechanism for processing hints to preprocessor
		string getPackageName  ( list<OptionDeclaration> OptionList );
		string getVersionNumber( list<OptionDeclaration> OptionList );

		// General support which might be moved to ROSE/src/transformationSupport.C
		overloadInformation isOverloaded ( SgClassDefinition* classDefinition, string functionName, string mangledFunctionName );
		string stringifyOperatorWithoutSymbols (string name);

		// Main function that generates SIDL function declarations
		string generateSIDLFunctionDeclaration ( SgFunctionDeclaration* functionDeclarationStatement );
};

// endif for ROSE_BABEL_PROGRAM_TRANSFORMATION_H
#endif
