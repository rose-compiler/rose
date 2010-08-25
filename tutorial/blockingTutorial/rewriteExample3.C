#include "rose.h"
#include "rewrite.h"
#include "AstRestructure.h"

// only for debugging output
SgProject* globalProjPointer; // DEBUG


// store information about for scopes
typedef struct {
	SgForStatement *forStmt; // the for statment itself
	string varName;          // the name of the loop variable
	SgType *varType;         // and its type
	bool   blocked;					 // was this loop blocked?
	int    blockSize;				 // if yes, what blocking size?
} forBlockInfo;

typedef vector<forBlockInfo *> forBlockVector;


// search for for statements
NodeQuerySynthesizedAttributeType forStatementNodeQuery(SgNode* node)
{
	ROSE_ASSERT(node != 0);
	NodeQuerySynthesizedAttributeType returnNodeList;
	if(isSgForStatement(node)) returnNodeList.push_back(node);
	return returnNodeList;
}


// debugging function of a node location
string printPosition(SgNode *node) {
	ostringstream out;
	out << "(";
	Sg_File_Info *startinf  = node->get_startOfConstruct();
	Sg_File_Info   *endinf  = node->get_endOfConstruct();
	if(startinf) out << startinf->get_filename() << ":" << startinf->get_line();
	if(  endinf) {
		out << "- " << startinf->get_filename() << ":" << startinf->get_line();
	}
	out << ") ";
	return out.str();
}

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttribute
{
	public:
		MyInheritedAttribute() :
	 		mFors(),
			mLoopKernel( false )	{ };

			MyInheritedAttribute & operator= (const MyInheritedAttribute& X) {
				mFors = X.mFors;
				mLoopKernel = X.mLoopKernel;
				return *this;
			}
			
		//void addForStatement(SgForStatement *fors) {
		void addForStatement(forBlockInfo *inf) {
			mFors.push_back( inf ); }
		forBlockVector &getForScopes() { return mFors; }

		void setLoopKernel(bool set){ mLoopKernel = set; }
		bool getLoopKernel(void) { return mLoopKernel; }
		
	private:
		
		//! vector with scopes of for statments to identify loop nests
		forBlockVector mFors;

		//! is the loop kernel found?
		bool mLoopKernel;
		
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

// tree traversal performing the blocking transformation
class MyTraversal
: public HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttribute,MySynthesizedAttribute>
{
	public:
		MyTraversal () {
			mReplaceVariable = 0; 
			mpLoopBody = NULL;
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

		//! remember the loop body
		SgNode *mpLoopBody;
		
		//! keep track of all for statements that were found
		forBlockVector mAllFors;
};


// Functions required by the tree traversal mechanism
MyInheritedAttribute
MyTraversal::evaluateRewriteInheritedAttribute (
		SgNode* astNode,
		MyInheritedAttribute inheritedAttribute )
{
	MyInheritedAttribute returnAttribute = inheritedAttribute;

	switch(astNode->variantT())
	{

		case V_SgForStatement: {
				cout << " found V_SgForStatement " << printPosition(astNode) << "" <<  endl;
				SgForStatement *forStat = isSgForStatement(astNode); 

				forBlockInfo *inf = new forBlockInfo;
				inf->forStmt = forStat;
				SgInitializedName *varin = isSgInitializedName( forStat->get_traversalSuccessorContainer()[0] //for init
						->get_traversalSuccessorContainer()[0] // var decl
						->get_traversalSuccessorContainer()[0] // initialized name
						);
				assert( varin );
				inf->varName = varin->get_name().str();
				inf->varType = varin->get_type();
				inf->blocked = false;
				inf->blockSize = 0;

				returnAttribute.addForStatement( inf );
				mAllFors.push_back( inf );

				list<SgNode*> forList = NodeQuery::querySubTree( astNode, forStatementNodeQuery );
				if( (forList.size()==1) && // only the current loop?
						(returnAttribute.getForScopes().size()>1) ) {
					cerr << "   it is the innermost for loop " << endl;
					mpLoopBody = forStat->get_loop_body();
				}
			} break;

		default:
			break;
	}

	if(astNode == mpLoopBody) {
		bool loopsValid = true;
		
		// do some basic checks for validity
		forBlockVector fors = returnAttribute.getForScopes();
		for(size_t i=0;i<fors.size(); i++) {
			SgExpression *testExpr = fors[i]->forStmt->get_test_expr();
			SgExpression *incExpr  = fors[i]->forStmt->get_increment_expr();
			if(isSgPlusPlusOp(incExpr) ==NULL)  loopsValid = false;
			if(isSgLessThanOp(testExpr) ==NULL) loopsValid = false;
			else {
				if(! isSgVarRefExp(isSgLessThanOp(testExpr)->get_lhs_operand()) ) loopsValid = false;
			}
		}

		// this is the basic block of the innermost loop
		// only do trafos, if more than two nested loop nests, and we found the inner one
		if(loopsValid) {
			returnAttribute.setLoopKernel( true );
		} else {
			cout << " loop nest not valid, skipping transformation..." << endl;
		}
	}
	
	if(isSgScopeStatement(astNode)) {
		// dont replace variable in higher scopes...
		if(mReplaceVariable > 0) {
			mReplaceVariable++;
			cerr << "nested scope found, #" << mReplaceVariable << endl;
		}
	} 

	return returnAttribute;
}


// function to handle the piecing together of the new loop body
void insertTransformedLoopBody(SgNode *astNode, MySynthesizedAttribute &synAttr, forBlockVector &fors) {
	ostringstream trafo;
	int blocksize = 10;

	//trafo << "{ "; // FIXME use basic block instead

	// build the blocked loops
	for(size_t i=0;i<fors.size(); i++) {
		cout << " FOR FOUND " << fors[i]->varName << endl;
		string loopVarName("blocked_");
		loopVarName += fors[i]->varName;
		SgLessThanOp *testExpr = isSgLessThanOp(fors[i]->forStmt->get_test_expr());
		string loopMaxExpr = testExpr->get_rhs_operand()->unparseToString();
		trafo <<fors[i]->varType->unparseToString() <<" loopMax_"<< fors[i]->varName << 
			" = ( ("<<loopMaxExpr<<")<("<<fors[i]->varName<<"+"<<blocksize<<") ? "<<"("<<loopMaxExpr<<") : ("<<fors[i]->varName<<"+"<<blocksize<<") ); \n";
		trafo << " for("<<fors[i]->varType->unparseToString()
	 		<<"	"<<loopVarName<<"="<<fors[i]->varName<<";"<<
			loopVarName<<"<loopMax_"<<fors[i]->varName<<";"<<
			loopVarName<<"++) { \n";
		fors[i]->blocked = true;
		fors[i]->blockSize = blocksize;
	}
	trafo << astNode->unparseToString() << "\n";
	//trafo << " }"; // FIXME use basic block instead
	
	for(size_t i=0;i<fors.size(); i++) {
		trafo << "} \n";
	}

	cout << " replacing loop body '" << astNode->unparseToString() <<"' with '"<< trafo.str() <<"' " << endl;
	synAttr.replace( astNode, trafo.str() );
	cout << " new '" << astNode->unparseToString() <<"' " << endl;

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

		case V_SgForStatement: {
				SgForStatement *forStat = isSgForStatement(astNode); 
				cout << " found V_SgForStatement " << printPosition(astNode) << "" <<  endl;
				for(size_t i=0; i<mAllFors.size(); i++) {
					if((mAllFors[i]->blocked)&&(astNode == mAllFors[i]->forStmt)) {
						ostringstream newFor;
						newFor << "for(";
						newFor << (*(forStat->get_init_stmt().begin()))->unparseToString();
						newFor << forStat->get_test_expr()->unparseToString() << ";" ;
						newFor << mAllFors[i]->varName << "+=" << mAllFors[i]->blockSize << ")\n" ;
						newFor << forStat->get_loop_body()->unparseToString();
						cout << " is blocked loop..." << endl;
						returnAttribute.replace( astNode, newFor.str() );
					}
				}
			} break;

		case V_SgVarRefExp: {
				cout << " found V_SgVarRefExp " << printPosition(astNode) << astNode->unparseToString() <<  endl;
				forBlockVector fors = inheritedAttribute.getForScopes();

				// replace variable occurrences in the loop kernel
				if(inheritedAttribute.getLoopKernel()) {
					for(size_t i=0;i<fors.size(); i++) {
						if( ( strcmp( astNode->unparseToString().c_str(), fors[i]->varName.c_str() )==0 ) &&
								( isSgVarRefExp(astNode)->get_type() == fors[i]->varType )
							) {
							string blockedVarName("blocked_");
							blockedVarName += fors[i]->varName;
							AstRestructure::unparserReplace( isSgVarRefExp(astNode), blockedVarName );
							cout << "   replacing with '"<<blockedVarName<<"' " << endl;
						}
					}
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
			cout << "   new statement " << printPosition(astNode) << " : '" << astNode->unparseToString() << "' " << endl;
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

	//if(astNode == mpLoopBody) { // FIXME why doesnt replace basic block work?
	if( (astNode->get_parent() == mpLoopBody)&&(inheritedAttribute.getLoopKernel()) ) {
		// we're back at the loop kernel block, now replace and insert new loops...
		insertTransformedLoopBody( astNode, returnAttribute, inheritedAttribute.getForScopes() );
	}

	return returnAttribute;
}

int main ( int argc, char** argv )
{
	SgProject* project = frontend(argc,argv);
	MyTraversal treeTraversal;
	MyInheritedAttribute inheritedAttribute;
	globalProjPointer = project;

	cout << "Start traversal..." << endl;
	treeTraversal.traverse(project,inheritedAttribute);
	cout << "End of traversal..." << endl;

	// done...
	return backend(project);
}







