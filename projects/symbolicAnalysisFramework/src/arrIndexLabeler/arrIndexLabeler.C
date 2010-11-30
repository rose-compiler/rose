#include "rose.h"
#include "arrIndexLabeler.h"

using namespace std;

namespace arrIndexLabeler {

/*
 SgPntrArrRefExp are stored as follows: 
                     node (SgPntrArrRefExp)
                    /                      \ 
       name subtree (SgPntrArrRefExp)      index subtree (SgExpression)
               /                  \ 
 name subtree (SgPntrArrRefExp)   index subtree (SgExpression)
            ...
 until we get to something 
 other than SgPntrArrRefExp
*/

// the traversal's inherited attribute that determines whether a given SgNode is part of an array index expression
class aiRecord
{
	public:
	// Pointers to the subtrees of SgPntrArrRefExp that corresponds to its index part and its array name part. 
	// This field is =NULL in all other nodes.
	SgExpression* indexSubtree;
	SgExpression* arrayNameSubtree;
	
	// true to represent array index expressions, false otherwise
	bool arrayIndexFlag;
	
	// true to represent that this is a top-level SgPntrArrRefExp that is not part of a larger SgPntrArrRefExp
	// (i.e. given a[b[i][j]][k][l], it is either a[b[i][j]][k][l] or b[i][j])
	bool topArrayRefExpFlag;
	
	// if arrayIndexFlag==true, the depth of the current index expression (i.e. its dimension)
	//int indexDim;
	
	// a default aiRecord (the one at the root of a tree)
	aiRecord()
	{
		indexSubtree = NULL;
		arrayNameSubtree = NULL;
		arrayIndexFlag=false;
		topArrayRefExpFlag=false;
		//indexDim=-1;
	}
};

// the traversal's synthesizes attribute that determines the dimensionality of a given array expression
// and records a pointer to the array name expression
class adRecord
{
	public:
	// points to the node from which this synthesized attribute came
	SgNode* origin;
		
	// true to represent array index expressions, false otherwise
	bool arrayIndexFlag;
	
	// true to represent that this is a top-level SgPntrArrRefExp that is not part of a larger SgPntrArrRefExp
	// (i.e. given a[b[i][j]][k][l], it is either a[b[i][j]][k][l] or b[i][j])
	bool topArrayRefExpFlag;
		
	// points to the array name expression
	SgExpression* arrayNameExp;
	
	// the dimensionality of the current array expression
	int arrayDim;
	
	// the list of indexes of this SgPntrArrRefExp, from left to right
	list<SgExpression*> indexExprs;
	
	adRecord()
	{
		this->origin = NULL;
		arrayIndexFlag = false;
		arrayNameExp = NULL;
		arrayDim = 0;
	}
	
	adRecord(SgNode* origin)
	{
		this->origin = origin;
		// copy n's inherited attributes directly from the persistent attribute left by the Top-Down pass
		this->arrayIndexFlag = isArrayIndex(origin);
		this->topArrayRefExpFlag = isTopArrayRefExp(origin);
		this->arrayNameExp = NULL;
		this->arrayDim = 0;
	}
	
	adRecord (SgNode* origin, bool arrayIndexFlag, bool topArrayRefExpFlag, SgExpression* arrayNameExp, int arrayDim)
	{
		this->origin = origin;
		this->arrayIndexFlag = arrayIndexFlag;
		this->topArrayRefExpFlag = topArrayRefExpFlag;
		this->arrayNameExp = arrayNameExp;
		this->arrayDim = arrayDim;
	}
};

class arrIndexAttribute : public AstAttribute
{
	public:
		
	// true to represent array index expressions, false otherwise
	bool arrayIndexFlag;
	
	// true to represent that this is a top-level SgPntrArrRefExp that is not part of a larger SgPntrArrRefExp
	// (i.e. given a[b[i][j]][k][l], it is either a[b[i][j]][k][l] or b[i][j])
	bool topArrayRefExpFlag;
	
	// points to the array name expression
	SgExpression* arrayNameExp;
	
	// the dimensionality of the current array expression
	int arrayDim;
	
	// list of this SgPntrArrRefExp's index expressions, from left to right
	list<SgExpression*> indexExprs;
	
	arrIndexAttribute(bool arrayIndexFlag, bool topArrayRefExpFlag, SgExpression* arrayNameExp, int arrayDim)
	{
		this->arrayIndexFlag = arrayIndexFlag;
		this->topArrayRefExpFlag = topArrayRefExpFlag;
		this->arrayNameExp = arrayNameExp;
		this->arrayDim = arrayDim;
	}
	
	arrIndexAttribute(arrIndexAttribute &that)
	{
		arrayIndexFlag = that.arrayIndexFlag;
		topArrayRefExpFlag = that.topArrayRefExpFlag;
		//indexDim = that.indexDim;
		arrayNameExp = that.arrayNameExp;
		arrayDim = that.arrayDim;
	}
	
	arrIndexAttribute(aiRecord &ir)
	{
		arrayIndexFlag = ir.arrayIndexFlag;
		topArrayRefExpFlag = ir.topArrayRefExpFlag;
		//indexDim = ir.indexDim;
		arrayNameExp = NULL;
		arrayDim = 0;
	}
	
	arrIndexAttribute(adRecord &dr)
	{
		arrayIndexFlag = dr.arrayIndexFlag;
		topArrayRefExpFlag = dr.topArrayRefExpFlag;
		arrayNameExp = dr.arrayNameExp;
		arrayDim = dr.arrayDim;
		indexExprs = dr.indexExprs;
	}
};

class arrayIndexLabeler : public SgTopDownBottomUpProcessing<aiRecord, adRecord>
{
	public:
	aiRecord evaluateInheritedAttribute(SgNode* n, aiRecord inherited )
	{
		//aiRecord ir(inherited, n);
		aiRecord ir;
		SgPntrArrRefExp* arrRef;
		
		if((arrRef = isSgPntrArrRefExp(n)))
		{
			ir.arrayNameSubtree = arrRef->get_lhs_operand();
			ir.indexSubtree = arrRef->get_rhs_operand();
			
			// this SgPntrArrRefExp is the top-most isSgPntrArrRefExp if it is not the top of 
			// the arrayNameSubtree of a higher-level SgPntrArrRefExp
			ir.topArrayRefExpFlag = (n!=inherited.arrayNameSubtree);
		}
		
		// if this node is the top of the index subtree of an SgPntrArrRefExp, record this in ir
		if(inherited.indexSubtree == n)
			ir.arrayIndexFlag = true;
		// otherwise, inherit from the parent node
		else
			ir.arrayIndexFlag = inherited.arrayIndexFlag;
		
		arrIndexAttribute* aiAttr = new arrIndexAttribute(ir);
		n->addNewAttribute("ArrayIndex", aiAttr);
		
/*		if(isSgPntrArrRefExp(n))
			printf("SgPntrArrRefExp:0x%x DOWN arrayIndexFlag=%d indexSubtree=0x%x\n", n, aiAttr->arrayIndexFlag, ir.indexSubtree);
		else
			printf("SgNode:0x%x DOWN arrayIndexFlag=%d indexSubtree=0x%x\n", n, aiAttr->arrayIndexFlag, ir.indexSubtree);*/
		
		return ir;
	}
	
	adRecord evaluateSynthesizedAttribute (SgNode* n, aiRecord inherited, 
	                                       SubTreeSynthesizedAttributes synthesized )
	{
		//printf("evaluateSynthesizedAttribute(n=%p)\n");
		adRecord dr(n);
		SgPntrArrRefExp* arrRef;
		
		if((arrRef = isSgPntrArrRefExp(n)))
		{
			//printf("evaluateSynthesizedAttribute() arrRef->lhs=%p, arrRef->rhs=%p\n", arrRef->get_lhs_operand(), );
			bool found=false;
			// look through both the synthesized attributes 
			// (one from the SgPntrArrRefExp's array name subtree and one from the index subtree)
			
			// get the name subtree
			for(SubTreeSynthesizedAttributes::iterator it=synthesized.begin(); it!=synthesized.end(); it++)
			{
				// if this synthesized attribute comes from SgPntrArrRefExp's array name subtree
				// (there should be only one such subtree)
				if((*it).origin == arrRef->get_lhs_operand())
				{
					// copy over the info from the name subtree
					found = true;
					dr.arrayDim=(*it).arrayDim+1;
					// if the array name expression hasn't been found yet, this 
					// SgPntrArrRefExp's array name subtree must be it
					if((*it).arrayNameExp==NULL)
						dr.arrayNameExp = arrRef->get_lhs_operand();
					// otherwise, the real array name expression is deeper in the array name subtree, so just copy it over
					else
						dr.arrayNameExp = (*it).arrayNameExp;
					
					// initialize this SgPntrArrRefExp's list of indexes from the lower-level list
					dr.indexExprs = (*it).indexExprs;
//					break;
				}
			}
			ROSE_ASSERT(found);
			
			found = false;
			// get the index subtree
			for(SubTreeSynthesizedAttributes::iterator it=synthesized.begin(); it!=synthesized.end(); it++)
			{
				// if this synthesized attribute comes from SgPntrArrRefExp's index subtree
				// (there should be only one such subtree)
				if((*it).origin == arrRef->get_rhs_operand())
				{
					found = true;
					// add the current index expression to this SgPntrArrRefExp's list of indexes
					dr.indexExprs.push_front(isSgExpression((*it).origin));
				}
			}
			ROSE_ASSERT(found);
		}
		
/*		if(isSgPntrArrRefExp(n))
			printf("SgPntrArrRefExp:0x%x UP arrayIndexFlag=%d arrayNameExp=0x%x arrayDim=%d\n", n, dr.arrayIndexFlag, dr.arrayNameExp, dr.arrayDim);
		else
			printf("SgNode:0x%x UP arrayIndexFlag=%d arrayNameExp=0x%x arrayDim=%d\n", n, dr.arrayIndexFlag, dr.arrayNameExp, dr.arrayDim);
		printf("         <%s>\n", n->unparseToString().c_str());*/
				
		// label this node with its read/write access information
		arrIndexAttribute* aiAttr = new arrIndexAttribute(dr);
		char attrName[100];
		sprintf(attrName, "ArrayIndex: [%d, %d, %p, %d]", dr.arrayIndexFlag, dr.topArrayRefExpFlag, dr.arrayNameExp, dr.arrayDim);
		n->addNewAttribute(attrName, aiAttr);
		n->updateAttribute("ArrayIndex", aiAttr);
		
		return dr;
	}
};

// labels the portion of the AST rooted at root with annotations that identify the various portions
// the read/write sides of SgAssignOp nodes (if they are)
void addArrayIndexAnnotations(SgNode* root)
{
	static bool called=false;
	
	if(!called)
	{
		arrayIndexLabeler ail;
		aiRecord	r;
		
		ail.traverse(root, r);
	}
	called = true;
}

// returns true if the given node is part of an array index expression and false otherwise
bool isArrayIndex(SgNode* n)
{
	ROSE_ASSERT(n->getAttribute("ArrayIndex"));
	return ((arrIndexAttribute*)(n->getAttribute("ArrayIndex")))->arrayIndexFlag;
}

// returns true the given SgPntrArrRefExp node this is a top-level SgPntrArrRefExp that is not part 
// of a larger SgPntrArrRefExp
// (i.e. given a[b[i][j]][k][l], it is either a[b[i][j]][k][l] or b[i][j])
bool isTopArrayRefExp(SgNode* n)
{
	ROSE_ASSERT(n->getAttribute("ArrayIndex"));
	return ((arrIndexAttribute*)(n->getAttribute("ArrayIndex")))->topArrayRefExpFlag;
}

// returns the SgExpression node that contains the name of the array in the given SgPntrArrRefExp or 
// NULL if the node is not a SgPntrArrRefExp
SgExpression* getArrayNameExp(SgNode* n)
{
	ROSE_ASSERT(n->getAttribute("ArrayIndex"));
	return ((arrIndexAttribute*)(n->getAttribute("ArrayIndex")))->arrayNameExp;
}

// returns the dimensionality of the array reference in the given SgPntrArrRefExp
int getArrayDim(SgPntrArrRefExp* n)
{
	ROSE_ASSERT(n->getAttribute("ArrayIndex"));
	return ((arrIndexAttribute*)(n->getAttribute("ArrayIndex")))->arrayDim;
}

// returns the list of index expressionf in the given SgPntrArrRefExp
list<SgExpression*>& getArrayIndexExprs(SgPntrArrRefExp* n)
{
	ROSE_ASSERT(n->getAttribute("ArrayIndex"));
	return ((arrIndexAttribute*)(n->getAttribute("ArrayIndex")))->indexExprs;
}

}
