#include "rose.h"
#include "rewrite.h"
#include "AstRestructure.h"


// name of the variable to replace
#define VARIABLE_NAME "i"

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttribute
{
	public:
		MyInheritedAttribute() { };
		void addForStatement() { };
	private:
};

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class MySynthesizedAttribute
: public HighLevelRewrite::SynthesizedAttribute
{
	public:
		MySynthesizedAttribute() {
			varRefFound = false;
		};

		MySynthesizedAttribute(const MySynthesizedAttribute& X) {
			((MySynthesizedAttribute*) this)->operator= (X);
		}

		MySynthesizedAttribute & operator= (const MySynthesizedAttribute& X) {
			varRefFound	= X.varRefFound;
			HighLevelRewrite::SynthesizedAttribute::operator=(X);
			return *this;
		}
		MySynthesizedAttribute & operator+= (const MySynthesizedAttribute& X) {
			operator=(X);
			return *this;
		}

		bool getVarRefFound() { return varRefFound; }
		void setVarRefFound(bool set){ varRefFound = set; }

	private:
		bool varRefFound;
};

// tree traversal to test the rewrite mechanism
//! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
class MyTraversal
: public HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttribute,MySynthesizedAttribute>
{
	public:
		MyTraversal () {
			mReplaceVariable = 0; 
		};

		// Functions required by the tree traversal mechanism
		MyInheritedAttribute evaluateRewriteInheritedAttribute (
				SgNode* astNode,
				MyInheritedAttribute inheritedAttribute );

		MySynthesizedAttribute evaluateRewriteSynthesizedAttribute (
				SgNode* astNode,
				MyInheritedAttribute inheritedAttribute,
				SubTreeSynthesizedAttributes synthesizedAttributeList );

	private:

		//! replace only occurrences in the correct scope
		int mReplaceVariable;
};

// Functions required by the tree traversal mechanism
MyInheritedAttribute
MyTraversal::evaluateRewriteInheritedAttribute (
		SgNode* astNode,
		MyInheritedAttribute inheritedAttribute )
{
	MyInheritedAttribute returnAttribute;

	if(isSgScopeStatement(astNode)) {
		// dont replace variable in higher scopes...
		if(mReplaceVariable > 0) {
			mReplaceVariable++;
			cerr << "nested scope found, #" << mReplaceVariable << endl;
		}
	} 

	return returnAttribute;
}



MySynthesizedAttribute
MyTraversal::evaluateRewriteSynthesizedAttribute (
		SgNode* astNode,
		MyInheritedAttribute inheritedAttribute,
		SubTreeSynthesizedAttributes synthesizedAttributeList )
{
	MySynthesizedAttribute returnAttribute;

	switch(astNode->variantT())
	{

		case V_SgVarRefExp: {
				cout << " found V_SgVarRefExp " << astNode->unparseToString() <<  endl;
				if(mReplaceVariable) {
					if( ( strcmp( astNode->unparseToString().c_str(), VARIABLE_NAME )==0 ) &&
							( isSgTypeInt(isSgVarRefExp(astNode)->get_type()) )
						) {
						returnAttribute.setVarRefFound( true );
						AstRestructure::unparserReplace( isSgVarRefExp(astNode), "newInt" );
						cout << "   replacing with 'newInt' " << endl;
					}
				}
			} break;

		case V_SgVariableDeclaration: {
				SgVariableDeclaration *varDecl = isSgVariableDeclaration(astNode);
				cout << " found V_SgVariableDeclaration " << astNode->unparseToString() <<  endl;
				// replace only integer variables called "i"
				if( (varDecl->get_traversalSuccessorContainer().size() > 0) &&
                                    isSgInitializedName( varDecl->get_traversalSuccessorContainer()[0]) && 
                                                ( strcmp(isSgInitializedName(varDecl->get_traversalSuccessorContainer()[0])->get_name().str(), VARIABLE_NAME )==0 ) &&
						( isSgTypeInt(isSgInitializedName(varDecl->get_traversalSuccessorContainer()[0])->get_type()) )
					) {
					// found declaration of "int i"
					string newDeclaration("int newInt = i + 100;");
					returnAttribute.insert( varDecl, newDeclaration, HighLevelCollectionTypedefs::LocalScope, HighLevelCollectionTypedefs::AfterCurrentPosition );
					assert( mReplaceVariable==0 ); // if it's true, there is another "int i", and this transformation wont work...
					mReplaceVariable = 1;
					cout << "   inserted: '" << newDeclaration <<"' , starting variable replacement " <<  endl;
				}
			} break;

		default:
			break;
	} // node type

	bool synth = false;
	for( SubTreeSynthesizedAttributes::iterator i=synthesizedAttributeList.begin();
             i!= synthesizedAttributeList.end(); i++ ) {
                if( (*i).getVarRefFound() ) synth = true;
	}
	if( synth ) {
		returnAttribute.setVarRefFound( true );
		if(isSgStatement( astNode )) {
			cout << "   new statement " << " : '" << astNode->unparseToString() << "' " << endl;
			returnAttribute.setVarRefFound( false );
			//if(!isSgReturnStmt( astNode )) { // DEBUG, this should work!??!?
				returnAttribute.replace( astNode, astNode->unparseToString(), HighLevelCollectionTypedefs::LocalScope );
			//}
		}
	}

	if(isSgScopeStatement(astNode)) {
		// dont replace variable in higher scopes...
		if(mReplaceVariable > 0) {
			mReplaceVariable--;
			cerr << "end of scope " << mReplaceVariable << endl;
		}
	} 

	return returnAttribute;
}

int main ( int argc, char** argv )
{
	SgProject* project = frontend(argc,argv);
	MyTraversal treeTraversal;
	MyInheritedAttribute inheritedAttribute;

	cout << "Start traversal..." << endl;
	treeTraversal.traverse(project,inheritedAttribute);
	cout << "End of traversal..." << endl;

	// done...
	return backend(project);
}







