#include "affineInequality.h"

extern int analysisDebugLevel;

// GCD implementation from Wikipedia
static long gcd(long u, long v)
{
	int shift;
	
	/* GCD(0,x) := x */
	if (u == 0 || v == 0)
		return u | v;
	
	/* Let shift := lg K, where K is the greatest power of 2
	dividing both u and v. */
	for (shift = 0; ((u | v) & 1) == 0; ++shift) {
		u >>= 1;
		v >>= 1;
	}
	
	while ((u & 1) == 0)
		u >>= 1;
	
	/* From here on, u is always odd. */
	do {
		while ((v & 1) == 0)  /* Loop X */
		v >>= 1;
		
		/* Now u and v are both odd, so diff(u, v) is even.
		Let u = min(u, v), v = diff(u, v)/2. */
		if (u <= v) {
			v -= u;
		} else {
			int diff = u - v;
			u = v;
			v = diff;
		}
		v >>= 1;
	} while (v != 0);
	
	return u << shift;
}

/*
class affineInequalityAttr : public AstAttribute 
{
	public:
	set<varAffineInequality> ineqs;
	
	affineInequalityAttr()
	{}
	
	affineInequalityAttr(const affineInequalityAttr& that)
	{
		this->ineqs = that.ineqs;
	}
};

// annotates the given subtree with the given affineInequalityAttr
static void annotateSubTreeIneq(SgNode* tree, affineInequalityAttr* ineqAttr);

// points trueIneqAttr and falseIneqAttr to freshly allocated objects that represent the true and false
// branches of the control flow guarded by the given expression. They are set to NULL if our representation
// cannot represent one of the expressions.
// doFalseBranch - if =true, falseIneqAttr is set to the correct false-branch condition and to NULL otherwise
static void setTrueFalseIneq(SgExpression* expr, 
                             affineInequalityAttr **trueIneqAttr, affineInequalityAttr **falseIneqAttr, 
                             bool doFalseBranch);

// Looks over all the conditional statements in the application and associates appropriate 
//    affine inequalities with the SgNodes that depend on these statements. Each inequality
//    is a must: it must be true of the node that it is associated with.
void initAffineIneqs(SgProject* project)
{
	Rose_STL_Container<SgNode*> ifs = NodeQuery::querySubTree(project, V_SgIfStmt);
	for(Rose_STL_Container<SgNode*>::iterator it = ifs.begin(); it!=ifs.end(); it++)
	{
		SgIfStmt* ifStmt = isSgIfStmt(*it);
		ROSE_ASSERT(isSgExprStatement(ifStmt->get_conditional()));
		
		// generate the inequality attributes that will be associated with this if statement's 
		// true and false branches
		affineInequalityAttr *trueIneqAttr=NULL, *falseIneqAttr=NULL;
		
		setTrueFalseIneq(isSgExprStatement(ifStmt->get_conditional())->get_expression(),
                       &trueIneqAttr, &falseIneqAttr, true);
		
		if(trueIneqAttr)  annotateSubTreeIneq(ifStmt->get_true_body(),  trueIneqAttr);
		if(falseIneqAttr) annotateSubTreeIneq(ifStmt->get_false_body(), falseIneqAttr);
		
		if(trueIneqAttr) delete trueIneqAttr;
		if(falseIneqAttr) delete falseIneqAttr;
	}
	
	Rose_STL_Container<SgNode*> fors = NodeQuery::querySubTree(project, V_SgForStatement);
	for(Rose_STL_Container<SgNode*>::iterator it = fors.begin(); it!=fors.end(); it++)
	{
		SgForStatement* forStmt = isSgForStatement(*it);
		
		// generate the inequality attributes that will be associated with this if statement's 
		// true and false branches
		affineInequalityAttr *trueIneqAttr=NULL, *falseIneqAttr=NULL;
		
		setTrueFalseIneq(forStmt->get_test_expr(), 
                       &trueIneqAttr, &falseIneqAttr, false);
		
		if(trueIneqAttr)
		{
			annotateSubTreeIneq(forStmt->get_loop_body(), trueIneqAttr);
			delete trueIneqAttr;
		}
	}
}

// points trueIneqAttr and falseIneqAttr to freshly allocated objects that represent the true and false
// branches of the control flow guarded by the given expression. They are set to NULL if our representation
// cannot represent one of the expressions.
// doFalseBranch - if =true, falseIneqAttr is set to the correct false-branch condition and to NULL otherwise
static void setTrueFalseIneq(SgExpression* expr, 
                             affineInequalityAttr **trueIneqAttr, affineInequalityAttr **falseIneqAttr, 
                             bool doFalseBranch)
{
	varID x, y;
	bool negX, negY;
	long c;
	*trueIneqAttr=NULL;
	*falseIneqAttr=NULL;
	
	// if this is a valid inequality condition of the form x <= y + c
	if(cfgUtils::computeTermsOfIfCondition_LTEQ(expr, x, negX, y, negY, c))
	{
		// x <= y + c
		*trueIneqAttr = new affineInequalityAttr();
		affineInequality ineqTrue(negX?-1:1, negY?-1:1, c);
		varAffineInequality tt(x, y, ineqTrue);
		(*trueIneqAttr)->ineqs.insert(tt);
		
		// x > y + c => y < x + -c => y <= x + -c-1
		if(doFalseBranch)
		{
			*falseIneqAttr = new affineInequalityAttr();
			affineInequality ineqFalse(negY?-1:1, negX?-1:1, (0-c)-1);
			varAffineInequality tf(y, x, ineqFalse);
			(*falseIneqAttr)->ineqs.insert(tf);
		}
	}
	// if this is a valid equality condition of the form x == y + c
	else if(cfgUtils::computeTermsOfIfCondition_EQ(expr, x, negX, y, negY, c))
	{
		// for equalities only bother with the true branch, since the false branch corresponds to
		// a disjunction of two affine inequalities and we don't support disjunctions right now
		*trueIneqAttr = new affineInequalityAttr();
		
		// x <= y + c
		affineInequality ineqLTEQ(negX?-1:1, negY?-1:1, c);
		varAffineInequality tLTEQ(x, y, ineqLTEQ);
		(*trueIneqAttr)->ineqs.insert(tLTEQ);
		
		// x >= y + c => y <= x -c 
		affineInequality ineqGTEQ(negY?1:-1, negX?1:-1, 0-c);
		varAffineInequality tGTEQ(y, x, ineqGTEQ);
		(*trueIneqAttr)->ineqs.insert(tGTEQ);
	}
}

// annotates the given subtree with the given affineInequalityAttr
static void annotateSubTreeIneq(SgNode* tree, affineInequalityAttr* ineqAttr)
{
	Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(tree, V_SgNode);
	for(Rose_STL_Container<SgNode*>::iterator it = nodes.begin(); it!=nodes.end(); it++)
	{
		SgNode* n = *it;
		
		// if there is already an inequality attribute associated with n
		if(n->attributeExists("affineInequality"))
		{
			AstAttribute *curAttr = n->getAttribute("affineInequality");
			
			ROSE_ASSERT(curAttr);
			affineInequalityAttr* curIneqAttr = (affineInequalityAttr*)curAttr;
			set<varAffineInequality> newIneqs;
			
cout << "Merging at node <"<<n->class_name()<<" | "<<n->unparseToString()<<">\n";
cout << "    Current inequalities:\n";
for(set<varAffineInequality>::iterator it = curIneqAttr->ineqs.begin(); it!=curIneqAttr->ineqs.end(); it++)
{
	varAffineInequality varIneq = *it;
	cout << varIneq.getIneq().str(varIneq.getX(), varIneq.getY(), "        ") << "\n";
}
cout << "    New inequalities:\n";
for(set<varAffineInequality>::iterator it = ineqAttr->ineqs.begin(); it!=ineqAttr->ineqs.end(); it++)
{
	varAffineInequality varIneq = *it;
	cout << varIneq.getIneq().str(varIneq.getX(), varIneq.getY(), "        ") << "\n";
}
			
			// initialize newIneqs with all the inequalities in curIneqAttr
			
			// iterate over curIneqAttr's inequalities
			for(set<varAffineInequality>::iterator itCur = curIneqAttr->ineqs.begin(); itCur!=curIneqAttr->ineqs.end(); itCur++)
			{
				varAffineInequality curIneq = *itCur;
				
				// check if the new ineqAttr has an inequality for the same variables as curIneq
				set<varAffineInequality>::iterator itNew;
				for(itNew = ineqAttr->ineqs.begin(); itNew!=ineqAttr->ineqs.end(); itNew++)
				{
					varAffineInequality newIneq = *itNew;
					
					// if we have a match
					if(curIneq.getX() == newIneq.getX() && curIneq.getY() == newIneq.getY())
					{
						// replace the constraint in itCur with the intersection of the two constraints,
						// since both are known to be true at this node
						//curIneq.getIneq() *= newIneq.getIneq();
						
						affineInequality newi(curIneq.getIneq());
						newi *= newIneq.getIneq();
						varAffineInequality newVarIneq(curIneq.getX(), curIneq.getY(), newi);
						newIneqs.insert(newVarIneq);
						
						break;
					}
				}
				
				// if there is no varAffineInequality in ineqAttr that corresponds to curIneq
				if(itNew == ineqAttr->ineqs.end())
					// add curIneq with no modifications to newIneqs
					newIneqs.insert(curIneq);
				cout << "    current inequality "<<curIneq.str("")<<", found match in new="<<(itNew == ineqAttr->ineqs.end())<<"\n";
				cout << "    newIneqs.size()="<<newIneqs.size()<<"\n";
			}
			
			// now add to newIneqs any varAffineInequalities in ineqAttr for variable pairs that do not 
			// appear in curIneqAttr
			
			for(set<varAffineInequality>::iterator itNew = ineqAttr->ineqs.begin(); itNew!=ineqAttr->ineqs.end(); itNew++)
			{
				varAffineInequality newIneq = *itNew;
				
				// check if the current ineqAttr has an inequality for the same variables as newIneq
				set<varAffineInequality>::iterator itCur;
				for(itCur = curIneqAttr->ineqs.begin(); itCur!=curIneqAttr->ineqs.end(); itCur++)
				{
					varAffineInequality curIneq = *itCur;
					
					// if we have a match
					if(curIneq.getX() == newIneq.getX() && curIneq.getY() == newIneq.getY())
						// break out
						break;
				}
				
				// if there is no varAffineInequality in curIneqAttr that corresponds to newIneq
				if(itCur == curIneqAttr->ineqs.end())
					// add curIneq with no modifications to newIneqs
					newIneqs.insert(newIneq);
			}
			
			// replace this attribute's current inequalities with the new set
			curIneqAttr->ineqs = newIneqs;
			
cout << "    Merged inequalities:\n";
for(set<varAffineInequality>::iterator it = curIneqAttr->ineqs.begin(); it!=curIneqAttr->ineqs.end(); it++)
{
	varAffineInequality varIneq = *it;
	cout << varIneq.getIneq().str(varIneq.getX(), varIneq.getY(), "        ") << "\n";
}
		}
		else
		{
			// associate ineqAttr with the current node
			n->addNewAttribute("affineInequality", new affineInequalityAttr(*ineqAttr));
		}
	}
}

// return the set of varAffineInequalities associated with this node or NULL if none are available
set<varAffineInequality>* getAffineIneq(SgNode* n)
{
	if(n->attributeExists("affineInequality"))
	{
		AstAttribute *curAttr = n->getAttribute("affineInequality");
		ROSE_ASSERT(curAttr);
	
		return &(((affineInequalityAttr*)curAttr)->ineqs);
	}
	else
		return NULL;
}*/

/****************************
 *** affineInequalityFact ***
 ****************************/

NodeFact* affineInequalityFact::copy() const
{
	return new affineInequalityFact(*this);
}


string affineInequalityFact::str(string indent)
{
	stringstream outs;
	
	outs << indent << "affineInequalityFact:";
	if(ineqs.size()>0); outs << "\n";
		
	for(set<varAffineInequality>::const_iterator it = ineqs.begin(); it!=ineqs.end(); )
	{
		outs << indent << "    " << (*it).str("");
		it++;
		if(it!=ineqs.end()) outs << "\n";
	}
	
	return outs.str();
}

string affineInequalityFact::str(string indent) const
{
	stringstream outs;
	
	outs << indent << "affineInequalityFact:";
	if(ineqs.size()>0); outs << "\n";
		
	for(set<varAffineInequality>::const_iterator it = ineqs.begin(); it!=ineqs.end(); )
	{
		outs << indent << "    " << (*it).str("");
		it++;
		if(it!=ineqs.end()) outs << "\n";
	}
	
	return outs.str();
}

/********************************
 *** affineInequalitiesPlacer ***
 ********************************/

// points trueIneqFact and falseIneqFact to freshly allocated objects that represent the true and false
// branches of the control flow guarded by the given expression. They are set to NULL if our representation
// cannot represent one of the expressions.
// doFalseBranch - if =true, falseIneqFact is set to the correct false-branch condition and to NULL otherwise
void affineInequalitiesPlacer::setTrueFalseIneq(SgExpression* expr, 
                             affineInequalityFact **trueIneqFact, affineInequalityFact **falseIneqFact, 
                             bool doFalseBranch)
{
	varID x, y;
	bool negX, negY;
	long c;
	*trueIneqFact=NULL;
	*falseIneqFact=NULL;
	
	// if this is a valid inequality condition of the form x <= y + c
	if(cfgUtils::computeTermsOfIfCondition_LTEQ(expr, x, negX, y, negY, c))
	{
		// x <= y + c
		*trueIneqFact = new affineInequalityFact();
// !!!! WE MAY WANT TO BE MORE PRECISE HERE BY ADDING SIGN INFORMATION TO THESE INEQUALITIES !!!!
		affineInequality ineqTrue(negX?-1:1, negY?-1:1, c, x==zeroVar, y==zeroVar, affineInequality::unknownSgn, affineInequality::unknownSgn);
		varAffineInequality tt(x, y, ineqTrue);
		(*trueIneqFact)->ineqs.insert(tt);
		
		//cout << "    tt="<<tt.str()<<"\n";

		// x > y + c => y < x + -c => y <= x + -c-1
		if(doFalseBranch)
		{
			*falseIneqFact = new affineInequalityFact();
			affineInequality ineqFalse(negY?-1:1, negX?-1:1, (0-c)-1, y==zeroVar, x==zeroVar, affineInequality::unknownSgn, affineInequality::unknownSgn);
			varAffineInequality tf(y, x, ineqFalse);
			(*falseIneqFact)->ineqs.insert(tf);
			//cout << "    tf="<<tf.str()<<"\n";
		}
	}
	// if this is a valid equality condition of the form x == y + c
	else if(cfgUtils::computeTermsOfIfCondition_EQ(expr, x, negX, y, negY, c))
	{
		// for equalities only bother with the true branch, since the false branch corresponds to
		// a disjunction of two affine inequalities and we don't support disjunctions right now
		*trueIneqFact = new affineInequalityFact();
		
		// x <= y + c
		affineInequality ineqLTEQ(negX?-1:1, negY?-1:1, c, x==zeroVar, y==zeroVar, affineInequality::unknownSgn, affineInequality::unknownSgn);
		varAffineInequality tLTEQ(x, y, ineqLTEQ);
		(*trueIneqFact)->ineqs.insert(tLTEQ);
		
		// x >= y + c => y <= x -c 
		if(doFalseBranch)
		{
			affineInequality ineqGTEQ(negY?-1:1, negX?-1:1, 0-c, y==zeroVar, x==zeroVar, affineInequality::unknownSgn, affineInequality::unknownSgn);
			varAffineInequality tGTEQ(y, x, ineqGTEQ);
			(*trueIneqFact)->ineqs.insert(tGTEQ);
		}
	}
}

void affineInequalitiesPlacer::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	if(analysisDebugLevel>0)
		printf("affineInequalitiesPlacer::visit() function %s() node=<%s | %s>\n", 
		       func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	
	affineInequalityFact *trueIneqFact=NULL, *falseIneqFact=NULL;
	bool doFalseBranch;
	// this conditional statement's test expression
	SgExpression* testExpr;
	
	if(isSgIfStmt(n.getNode()))
	{
		SgIfStmt* ifStmt = isSgIfStmt(n.getNode());
		ROSE_ASSERT(isSgExprStatement(ifStmt->get_conditional()));
		testExpr = isSgExprStatement(ifStmt->get_conditional())->get_expression();
		doFalseBranch = true;
	}
	else if(isSgForStatement(n.getNode()))
	{
		SgForStatement* forStmt = isSgForStatement(n.getNode());
		ROSE_ASSERT(isSgExprStatement(forStmt->get_test()));
		testExpr = isSgExprStatement(forStmt->get_test())->get_expression();
		doFalseBranch = true;
	}
	else
		return;
		
	// generate the inequality attributes that will be associated with this if/for statement's 
	// true and false branches
	setTrueFalseIneq(testExpr, &trueIneqFact, &falseIneqFact, doFalseBranch);
				
	// iterate over both the descendants
	vector<DataflowEdge> edges = n.outEdges();
	//printf("edges.size()=%d, trueIneqFact=%p, falseIneqFact=%p\n", edges.size(), trueIneqFact, falseIneqFact);
	if(analysisDebugLevel>0)
	{
		cout << "trueIneqFact="<<trueIneqFact->str()<<"\n";
		if(doFalseBranch)
			cout << "falseIneqFact="<<falseIneqFact->str()<<"\n";
	}
	for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
	{
		if((*ei).condition() == eckTrue && trueIneqFact)
		{
			NodeState* trueNodeState = NodeState::getNodeState((*ei).target());
//printf("    Adding true side fact %s, this=%p\n", trueIneqFact->str().c_str(), this);
			trueNodeState->addFact(this, 0, trueIneqFact);
		}
		else if((*ei).condition() == eckFalse && falseIneqFact)
		{
			NodeState* falseNodeState = NodeState::getNodeState((*ei).target());
//printf("    Adding false side fact %s, this=%p\n", falseIneqFact->str().c_str(), this);
			falseNodeState->addFact(this, 0, falseIneqFact);
		}
	}
}

/*printAffineInequalities::printAffineInequalities(affineInequalitiesPlacer *placer)
{
	this->placer = placer;	
}

void printAffineInequalities::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	//set<varAffineInequality> *ineqs = getAffineIneq(n.getNode());
	affineInequalityFact* ineqFact = (affineInequalityFact*)state.getFact(placer, 0);
	if(ineqFact)
	{
		cout << "Node <"<<n.getNode()->class_name()<<" | "<<n.getNode()->unparseToString()<<">\n";
		for(set<varAffineInequality>::iterator it = ineqFact->ineqs.begin(); it!=ineqFact->ineqs.end(); it++)
		{
			varAffineInequality varIneq = *it;
			cout << varIneq.getIneq().str(varIneq.getX(), varIneq.getY(), "    ") << "\n";
		}
	}
}*/

// prints the inequality facts set by the given affineInequalityPlacer
void printAffineInequalities(affineInequalitiesPlacer* aip, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	factNames.push_back(0);
	printAnalysisStates pas(aip, factNames, latticeNames, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}

// Runs the Affine Inequality Placer analysis
static affineInequalitiesPlacer* aip = NULL;
void runAffineIneqPlacer(bool printStates)
{
	if(aip == NULL)
	{
		aip = new affineInequalitiesPlacer();
		UnstructuredPassInterAnalysis upia_aip(*aip);
		upia_aip.runAnalysis();
		
		if(printStates)
			printAffineInequalities(aip, ":");
	}
}

static set<varAffineInequality> emptySet;

// returns the set of inequalities known to be true at the given DataflowNode
const set<varAffineInequality>& getAffineIneq(const DataflowNode& n)
{
	NodeState* ns = NodeState::getNodeState(n);
	affineInequalityFact* fact = dynamic_cast<affineInequalityFact*>(ns->getFact(aip, 0));
	//printf("getAffineIneq: fact=%p, aip=%p, ns=%p\n", fact, aip, ns);
	if(fact)
		return fact->ineqs;
	else
		return emptySet;
}


// represents a full affine inequality, including the variables involved in the inequality
/***************************
 *** varAffineInequality ***
 ***************************/

varAffineInequality::varAffineInequality(const varAffineInequality& that):
	x(that.x), y(that.y), ineq(that.ineq)
{
}

varAffineInequality::varAffineInequality(const varID& x, const varID& y, const affineInequality& ineq) :
	x(x), y(y), ineq(ineq)
{
}

varAffineInequality::varAffineInequality(const varID& x, const varID& y, int a, int b, int c, 
	                                      bool xZero, bool yZero)
{
	// one of the variables must be non-zero
	ROSE_ASSERT(!xZero || !yZero);
	
	if(xZero)
	{
		a = 1;
		this->ineq.xSign = affineInequality::eqZero;
	}
	else
		this->ineq.xSign = affineInequality::posZero;
		
	if(yZero)
	{
		b = 1;
		this->ineq.ySign = affineInequality::eqZero;
	}
	else
		this->ineq.ySign = affineInequality::posZero;
		
	// if both a and b are <0, negate the inequality
	if(a<0 && b<0)
	{
		// -ax <= -by + c == 
		// ax >= by -c ==
		// by <= ax + c
		this->x = y;
		this->y = x;
		a = 0-a;
		b = 0-b;
		if(this->ineq.xSign==affineInequality::posZero)      this->ineq.xSign = affineInequality::negZero;
		else if(this->ineq.xSign==affineInequality::negZero) this->ineq.xSign = affineInequality::posZero;
		if(this->ineq.ySign==affineInequality::posZero)      this->ineq.ySign = affineInequality::negZero;
		else if(this->ineq.ySign==affineInequality::negZero) this->ineq.ySign = affineInequality::posZero;
	}
	else
	{
		this->x = x;
		this->y = y;
	}
	this->ineq.a = a;
	this->ineq.b = b;
	this->ineq.c = c;
	this->ineq.xZero = xZero;
	this->ineq.yZero = yZero;
}

// get methods
/*varID& varAffineInequality::getX()
{ return x; }

varID& varAffineInequality::getY()
{ return y; }

affineInequality& varAffineInequality::getIneq()
{ return ineq; }*/

const varID& varAffineInequality::getX() const
{ return x; }

const varID& varAffineInequality::getY() const
{ return y; }

int varAffineInequality::getA() const
{ return ineq.getA(); }
	
int varAffineInequality::getB() const
{ return ineq.getB(); }

int varAffineInequality::getC() const
{ return ineq.getC(); }

const affineInequality& varAffineInequality::getIneq() const
{ return ineq; }

// set methods, return true if this object changes
const bool varAffineInequality::setX(const varID& x)
{
	bool modified = (x != this->x);
	this->x = x;
	return modified;
}

const bool varAffineInequality::setY(const varID& y)
{
	bool modified = (y != this->y);
	this->y = y;
	return modified;
}

const bool varAffineInequality::setA(int a)
{ return this->ineq.setA(a); }

const bool varAffineInequality::setB(int b)
{ return this->ineq.setB(b); }

const bool varAffineInequality::setC(int c)
{ return this->ineq.setC(c); }

const bool varAffineInequality::setIneq(affineInequality& ineq)
{
	return this->ineq.set(ineq);
}

// comparison methods
bool varAffineInequality::operator==(const varAffineInequality& that) const
{
	return x == that.x &&
	       y == that.y &&
	       ineq == that.ineq;
}

bool varAffineInequality::operator<(const varAffineInequality& that) const
{
	return (x < that.x) ||
	       (x == that.x && y < that.y) ||
	       (x == that.x && y == that.y && ineq < that.ineq);
}

string varAffineInequality::str(string indent)// const
{
	return ineq.str(x, y, indent);
}

string varAffineInequality::str(string indent) const
{
	return ineq.str(x, y, indent);
}

// Parses expr and returns the corresponding varAffineInequality if expr can be represented
// as such and NULL otherwise.
/*static varAffineInequality* varAffineInequality::parseIneq(SgExpression* expr)
{
	// if the destination process expression is parseable as i op j op c
	varID i, j;
	bool negI, negJ;
	short op;
	long c;
	if(parseExpr(expr, i, negI, j, negJ, c))
}*/

// the basic logical operations that must be supported by any implementation of 
// a logical condition: NOT, AND and OR
/*void varAffineInequality::notUpd()
{
	//ineq.notUpd();
	// The negation of [x*a <= y*b + c] is [x*a > y*b + c] == [y*b + c < x*a] == [y*b <= x*a - c - 1]

	// x<=y-infinity => x>y-infinity == y<=x+infinity
	if(level == falseConstr)
		setToTop();
	else * /if(ineq.level == affineInequality::constrKnown)
	{
		int tmp=ineq.a;
		ineq.a = ineq.b;
		ineq.b = tmp;
		ineq.c = -1-ineq.c;
		
		varID tmpVar = x;
		x=y;
		y=tmpVar;
	}
}*/

/*void varAffineInequality::andUpd(LogicalCond& that_arg)
{
	varAffineInequality& that = (varAffineInequality&)that_arg;

	//cout << "varAffineInequality::andUpd() x="<<x.str()<<" that.x="<<that.x.str()<<" y="<<y.str()<<" that.y="<<that.y.str()<<"\n";
	cout << "varAffineInequality::andUpd() \n";
	cout << "     this="<<str()<<"\n";
	cout << "     that="<<that.str()<<"\n";
	// The two varAffineInequalities must refer to the same pair of variables
	ROSE_ASSERT((x==that.x && y==that.y) ||
	            (x==that.y && y==that.x));
	
	// a*x <= b*y + c AND a'*x <= b'*y + c'
	if(x==that.x && y==that.y)
	{
		//ineq.andUpd(that.ineq);
		ineq *= that.ineq;
	}
	// a*x <= b*y + c          AND a'*y <= b'*x + c' 
	// a*x - c <= b*y          AND a'*y <= b'*x + c'
	// a*b'*x - c*b' <= b*b'*y AND a'*a*y <= b'*a*x + c'*a
	// a*b'*x - c*b' <= b*b'*y AND -a'*a*y >= -b'*a*x - c'*a
	// a*b'*x <= b*b'*y + c*b' AND b'*a*x <= -a'*a*y + c'*a
	// a*b'*x <= b*b'*y + c*b' AND 
	//        <= -a'*a*y + c'*a
	else if(x==that.y && y==that.x)
	{
		ROSE_ASSERT(0);
		// This is too complex for now
		ineq.setToTop();
	}
}

void varAffineInequality::orUpd(LogicalCond& that_arg)
{
	varAffineInequality& that = (varAffineInequality&)that_arg;
	
	// The two varAffineInequalities must refer to the same pair of variables
	ROSE_ASSERT((x==that.x && y==that.y) ||
	            (x==that.y && y==that.x));
	
	// a*x <= b*y + c OR a'*x <= b'*y + c'
	if(x==that.x && y==that.y)
	{
		//ineq.orUpd(that.ineq);
		ineq += that.ineq;
	}
	// a*x <= b*y + c          OR a'*y <= b'*x + c' 
	// a*x - c <= b*y          OR a'*y <= b'*x + c'
	// a*a'*x - c*a' <= b*a'*y OR a'*b*y <= b'*b*x + c'*b
	else if(x==that.y && y==that.x)
	{
		// We can't compare these two expressions for information content
		ineq.setToTop();
	}
}

// returns a copy of this LogicalCond object
LogicalCond* varAffineInequality::copy()
{
	return (LogicalCond*)(new varAffineInequality(*this));
}*/

/************************
 *** affineInequality ***
 ************************/

// no constraint known since current constraints are not representable using a single affine inequality
const short affineInequality::top;
// it is known that the affine constraints are inconsistent
//const short affineInequality::falseConstr;
// some constraint is known
const short affineInequality::constrKnown;
// the values of the variables are not constrained relative to each other
// (this object would not even be created in this case)
const short affineInequality::bottom;

affineInequality::affineInequality()
{
	setToBottom();
	this->xZero = false;
	this->yZero = false;
	this->xSign = unknownSgn;
	this->ySign = unknownSgn;
}

affineInequality::affineInequality(const affineInequality& that)
{
	//this->y = that.y;
	this->a = that.a;
	this->b = that.b;
	this->c = that.c;
	this->xZero = that.xZero;
	this->yZero = that.yZero;
	this->xSign = that.xSign;
	this->ySign = that.ySign;
	this->level = that.level;
}

affineInequality::affineInequality(int a, int b, int c, bool xZero, bool yZero, signs xSign, signs ySign)
{
	ROSE_ASSERT(!xZero || !yZero); 
	if(xZero) a = 1;
	if(yZero) b = 1;
	
	this->a = a;
	this->b = b;
	this->c = c;
	this->xZero = xZero;
	this->yZero = yZero;
	this->xSign = xSign;
	this->ySign = ySign;
	this->level = constrKnown;
	normalize();
}

// given a constraint on x, z and a constraint on z, y, infers the corresponding constraint on x, y 
// and sets this constraint to it
affineInequality::affineInequality(const affineInequality& xz, const affineInequality& zy/*, bool xZero, bool yZero, DivLattice* divX, DivLattice* divY, varID z*/)
{
	this->xZero = xz.xZero;
	this->yZero = zy.yZero;
	ROSE_ASSERT(!xZero || !yZero);
	this->xSign = xz.xSign;
	this->ySign = zy.ySign;
	
	if(xz.level == bottom || zy.level == bottom)
		setToBottom();
	else if(xz.level == top || zy.level == top)
		setToTop();
/*	else if(xz.level == falseConstr || zy.level == falseConstr)
		setToFalse();*/
	else
	{
		// x*a <= y*b + c && y*a' <= z*b' + c'
		// implies that 
		// x*a*a' <= y*b*a' + c*a' && y*a'*b <= z*b'*b + c'*b
		// x*a*a' <= z*b'*b + c'*b + c*a'
		a = xz.getA()*zy.getA();
		b = xz.getB()*zy.getB();
		c = xz.getB()*zy.getC() + xz.getC()*zy.getA();
		
		/* // if the connection is being made through Zero, incorporate the divisibility information
		if(z == zeroVar && divX->getLevel()==DivLattice::divKnown && divX->getRem()==0 &&
		                   divY->getLevel()==DivLattice::divKnown && divY->getRem()==0 &&
		                	 c==0)
		{
			a*=divX->getDiv();
			b*=divY->getDiv();
		}*/
	
		level = constrKnown;
		
		normalize();
	}
}

bool affineInequality::operator=(const affineInequality& that)
{
	//this->y=that.y;
	this->a=that.a;
	this->b=that.b;
	this->c=that.c;
	this->xZero = that.xZero;
	this->yZero = that.yZero;
	this->xSign = that.xSign;
	this->ySign = that.ySign;
	this->level = that.level;
}

bool affineInequality::operator==(const affineInequality& that) const
{
	return //y==that.y &&
	       a==that.a &&
	       b==that.b &&
	       c==that.c &&
	       level==that.level;
}

bool affineInequality::operator!=(const affineInequality& that) const
{
	return !(*this == that);
}

// lexicographic ordering (total order)
bool affineInequality::operator<(const affineInequality& that) const
{
	return  /*y<that.y ||*/
	       (/*y==that.y && */a<that.a) ||
	       (/*y==that.y && */a==that.a && b<that.b) ||
	       (/*y==that.y && */a==that.a && b==that.b && c<that.c) ||
	       (/*y==that.y && */a==that.a && b==that.b && c==that.c && level<that.level);
}

// semantic affineInequality ordering (partial order)
// returns true if this affineInequality represents more information (less information is 
// top, more information is bottom) than that for all values of x and y and false otherwise
//bool affineInequality::operator<<(const affineInequality& that) const
//bool affineInequality::semLessThan(const affineInequality& that) const
bool affineInequality::semLessThan(const affineInequality& that, bool xEqZero, bool yEqZero) const
{
	ROSE_ASSERT(xZero==that.xZero && yZero==that.yZero);
//printf("affineInequality::operator<< this=%p that=%p\n", this, &that);

	// Case 1: different levels
	if(level < that.level)
		return true;
/*		// top corresponds to an unrepresentable constraint, so we can't make judgements about how that relates to this
		if(that.level == top) return false;
		else return true;*/
	
	if(level > that.level) 
		return false;
		
	// Case 2: level == that.level but the level is not constrKnown 
	// (i.e. a, b and c are not used, meaning that this == that)
	if(level == bottom || level == top/* || level == falseConstr*/) return false;
	
	// Case 3: level == that.level == constrKnown, so we worry about a, b and c
	
	// if the two constraints are equal, then one is not strictly less than the other
	if(*this == that) return false;
	
	// if both the lhs and the rhs of x*a <= y*b + c are equal to 0
	/*if(xEqZero && yEqZero)
	{
		// The constraint that has multiplier information is considered to be tighter.
		// Technically, either constraint is equivalent, since 0*a=0*b=0. However, it is
		// important to explicitly maintain a and b for the future
		if(a==1 && b==1) return false;
		if(that.a==1 && that.b==1) return true;
		
		// if both constraints have multiplier information pick the current constraint as the more informative one
		return true;
		
		if(a==1 && b==1) return true;
		if(that.a==1 && that.b==1) return false;
		
		// if both constraints have multiplier information pick the current constraint as the more informative one
		return true;		
	}*/
	
	// if neither x nor y are constants
	if(!xZero && !yZero/* && xSign!=eqZero && ySign!=eqZero*/)
	{
		//      this                that
		// x*a <= y*b + c AND x*a' <= y*b' + c'
		// x*a*a' <= y*b*a' + c*a' AND x*a'*a <= y*b'*a + c'*a
		
		// if the two lines have the same slope
		if(b*that.a == a*that.b)
		{
	//printf("affineInequality::operator<<  c=%d, that.c=%d\n", c, that.c);
			// this has more information than that if its' y-intercept is lower, since it leaves x 
			// with fewer valid points relative to y
			return c*that.a < that.c*a;
		}

		/*// if x*a*a' >= 0
		if((xSign==posZero && a*that.a>0) || 
			(xSign==negZero && a*that.a<0))
		{
			// if both lines have the same y-intercept, the line with the smaller slope is lower, 
			// since it leaves x with fewer valid points relative to y
			return b*that.a < that.b*a;
		}*/			
	}
	else if(xZero/* || xSign==eqZero*/)
	{
		// 0 <= y*b + c AND 0 <= y*b' + c'
		// -c <= y*b        -c' <= y*b'
		// if c == c', then the constraint with the smaller b has more info, since this inequality 
		// admits fewer valid values of x
		if(c == that.c)
			return b<that.b;
		// -c <= y*b AND -c' <= y*b'
		// The smaller constant (larger -c/-c') represents the tighter constraint
		else if(b == that.b)
			return c<that.c;
	}
	else if(yZero/* || ySign==eqZero*/)
	{
		//printf("yZero, c == that.c=%d, a<that.a=%d, a==1 && that.a==1=%d, c<that.c=%d\n", c == that.c, a<that.a, a==1 && that.a==1, c<that.c);
		// x*a <= c AND x*a' <= c'
		// if c == c', then the constraint with the smaller a has more info, since the inequality 
		// admits fewer valid values of x
		if(c == that.c)
			return a<that.a;
		// x <= c AND x <= c'
		// The smaller constant represents the tighter constraint
		else if(a == that.a)
			return c<that.c;
	}
	
	// if the slopes are not the same, these two affineInequalities are not strictly ordered for all values of x and y
	return false;
}
	
bool affineInequality::set(const affineInequality& that)
{
	bool modified = (this->a     != that.a) ||
	                (this->b     != that.b) ||
	                (this->c     != that.c) ||
	                (this->xZero != that.xZero) ||
	                (this->yZero != that.yZero) ||
	                (this->xSign != that.xSign) ||
	                (this->ySign != that.ySign) ||
	                (this->level != that.level);
	                
	this->a = that.a;
	this->b = that.b;
	this->c = that.c;
	this->xZero = that.xZero;
	this->yZero = that.yZero;
	this->xSign = that.xSign;
	this->ySign = that.ySign;
	this->level = that.level;
	
	return modified;
}

bool affineInequality::set(int a, int b, int c)
{
	bool modified = (this->a     != a) ||
	                (this->b     != b) ||
	                (this->c     != c) ||
	                (this->level != constrKnown);
	this->a = a;
	this->b = b;
	this->c = c;
	this->level = constrKnown;
	return normalize() || modified;
}

bool affineInequality::set(int a, int b, int c, bool xZero, bool yZero, signs xSign, signs ySign)
{
	bool modified = (this->a     != a) ||
	                (this->b     != b) ||
	                (this->c     != c) ||
	                (this->xZero != xZero) ||
	                (this->yZero != yZero) ||
	                (this->xSign != xSign) ||
	                (this->ySign != ySign) ||
	                (this->level != constrKnown);
	this->a = a;
	this->b = b;
	this->c = c;
	this->xZero = xZero;
	this->yZero = yZero;
	this->xSign = xSign;
	this->ySign = ySign;
	this->level = constrKnown;
	return normalize() || modified;
}

bool affineInequality::setA(int a)
{
	if(this->level == constrKnown)
	{
		bool modified = (this->a == a);
		this->a = a;
		return normalize() || modified;
	}
	// if you change only one constant on a false constraint, the constraint
	// must become top (unknown constraints) because we have no idea how the
	// change of this one constant affects the validity of the constraint
	/*else if(this->level == falseConstr)
		setToTop();*/
	return false;
}

bool affineInequality::setB(int b)
{
	if(this->level == constrKnown)
	{
		bool modified = (this->b == b);
		this->b = b;
		return normalize() || modified;
	}
	// if you change only one constant on a false constraint, the constraint
	// must become top (unknown constraints) because we have no idea how the
	// change of this one constant affects the validity of the constraint
	/*else if(this->level == falseConstr)
		setToTop();*/
	return false;
}

bool affineInequality::setC(int c)
{
	if(this->level == constrKnown)
	{
		bool modified = (this->c == c);
		this->c = c;
		return normalize() || modified;
	}
	// if you change only one constant on a false constraint, the constraint
	// must become top (unknown constraints) because we have no idea how the
	// change of this one constant affects the validity of the constraint
	/*else if(this->level == falseConstr)
		setToTop();*/
	return false;
}

// sets this constraint object to bottom
bool affineInequality::setToBottom()
{
	if(level!=bottom)
	{
		level = bottom;
		a = 0;
		b = 0;
		c = 0;
		return true;
	}
	return false;
}

// sets this constraint object to false
/*bool affineInequality::setToFalse()
{
	if(level!=top)
	{
		level = falseConstr;
		a = 0;
		b = 0;
		c = 0;
		return true;
	}
	return false;
}*/

// sets this constraint object to top
bool affineInequality::setToTop()
{
	if(level!=top)
	{
		level = top;
		a = 0;
		b = 0;
		c = 0;
		return true;
	}
	return false;
}

// returns y, a, b or c
int affineInequality::getA() const
{ return a; }

int affineInequality::getB() const
{ return b; }

int affineInequality::getC() const
{ return c; }

short affineInequality::getLevel() const
{ return level; }

bool affineInequality::isXZero() const
{ return xZero; }

bool affineInequality::isYZero() const
{ return yZero; }

affineInequality::signs affineInequality::getXSign() const
{ return xSign; }

affineInequality::signs affineInequality::getYSign() const
{ return ySign; }

// divide out from a, b and c any common factors, reducing the triple to its normal form
// return true if this modifies this constraint and false otherwise
bool affineInequality::normalize()
{
	bool modified = false;
	ROSE_ASSERT(this->level == constrKnown);
	//printf("normalize(%d, %d, %d)\n", a, b, c);
	
	/*if(xSign==eqZero && ySign==eqZero)
	{
		a=1;
		b=1;
		c=0;
	}
	// 0 <= y*b + c
	else */if(xZero/* || xSign==eqZero*/)
	{
		a=1;
		int gcd_bc = gcd(abs(b), abs(c));
		b = b/gcd_bc;
		c = c/gcd_bc;
		
		// if c=0 or -1, [0 <= y*b + c] implies [0 <= y + c], which is a stronger constraint
		/*if(c==0 || c==-1)
			b=1;*/
	}
	// x*a <= 0 + c
	else if(yZero/* || ySign==eqZero*/)
	{
		b=1;
		int gcd_ac = gcd(abs(a), abs(c));
		a = a/gcd_ac;
		c = c/gcd_ac;
		
		// if c=0 or c=-1, [x*a <= c] implies [x <= c], which is a stronger constraint
		/*if(c==0 || c==-1)
			a=1;*/
	}
	// x!=zeroVar and y!=zeroVar
	else if(c == 0)
	{
		int gcd_ab = gcd(abs(a), abs(b));
		modified = (a == a/gcd_ab) &&
		           (b == b/gcd_ab);

		a = a/gcd_ab;
		b = b/gcd_ab;
		//printf("        B(%d, %d, %d)\n", a, b, c);
	}
	else
	{
		int gcd_ab = gcd(abs(a), abs(b));
		int gcd_abc = gcd(gcd_ab, abs(c));
		modified = (a == a/gcd_abc) &&
                 (b == b/gcd_abc) &&
                 (c == c/gcd_abc);
		
		a = a/gcd_abc;
		b = b/gcd_abc;
		c = c/gcd_abc;
		//printf("        C(%d, %d, %d)\n", a, b, c);
	}
	
/*	// Now consider the fact that x and y are integers. Thus, if 
	// if a=l*m, b=l*n, c=l*p+q 
	// then [x*a <= y*b + c] implies [x*l*m <= y*l*n + l*p] == [x*m <= y*n + p]
	long gcd_ab = gcd(a,b);
	int div, rem;
	div = c/gcd_ab;
	rem = c%gcd_ab;
	//printf("gcd_ab=%d div=%d rem=%d, c-((gcd_ab+rem)%%gcd_ab)=%d\n", gcd_ab, div, rem, c-((gcd_ab+rem)%gcd_ab));
	
	//printf("normalize(%d, %d, %d) ==> ", a, b, c);
	if(gcd_ab>1)
	{
		modified = (a == a/gcd_ab) &&
                 (b == b/gcd_ab) &&
                 (c == (c-((gcd_ab+rem)%gcd_ab))/gcd_ab);
		
		a=a/gcd_ab;
		b=b/gcd_ab;
		c = (c-((gcd_ab+rem)%gcd_ab))/gcd_ab;
	}
	//printf("(%d, %d, %d)\n", a, b, c);
	*/
}

// INTERSECT this with that, saving the result in this
// Intersection = the affine inequality that contains the constraint information of both
//         this AND that (i.e. the line that is lower than both lines)
// (moves this/that upward in the lattice)
void affineInequality::operator*=(const affineInequality& that)
{
	intersectUpd(that);
}

// Just like *=, except intersectUpd() returns true if the operation causes this
// affineInequality to change and false otherwise.
bool affineInequality::intersectUpd(const affineInequality& that)
{
	bool modified=false;
	
	// top -> top or top->lower
	// constrKnown -> constrKnown or constrKnown->lower
	// falseConstr -> falseConstr
	// bottom -> bottom
	// if either operand is bottom, the result is bottom
	if(level == bottom || that.level == bottom)
		modified = setToBottom() || modified;
	// if one is = top, the result is the other one
	// and if both are top, then this doesn't change
	else if(level == top && that.level != top)
	{
		modified = modified || level!=that.level;
		level = that.level;
		modified = modified || a!=that.a;
		a = that.a;
		modified = modified || b!=that.b;
		b = that.b;
		modified = modified || c!=that.c;
		c = that.c;
	}
	else if(that.level == top)
	{ }
	// if either level is falseConstr, then their intersection is also falseConstr
	/*else if(level == falseConstr || that.level == falseConstr)
		setToFalse();*/
	// if both levels are constrKnown
	else
	{
		//      this                that
		// x*a <= y*b + c AND x*a' <= y*b' + c'
		// x*a*a' <= y*b*a' + c*a' AND x*a'*a <= y*b'*a + c'*a
		
		// the only way to lower-bound both lines is if they have the same slope
		if(b*that.a == a*that.b)
		{
			if(c*that.a <= that.c*a)
			{
				modified = modified || b!=b*that.a;
				b = b*that.a;
				modified = modified || c!=c*that.a;
				c = c*that.a;
			}
			else
			{
				modified = modified || b!=that.b*a;
				b = that.b*a;
				modified = modified || c!=that.c*a;
				c = that.c*a;
			}
			modified = modified || a!=a*that.a;
			a = a*that.a;
			modified = normalize() || modified ;
		}
		// otherwise, the lower-bound cannot be represented as a line
		else
		{
			//setToTop();
			// This lattice is conservative in the sense of containg no more information
			// than is known to be true. As such, because we know that either this' or 
			// that's constraints are true, we'll leave this with its current constraints.
		}
	}
	
	return modified;
}

// UNION this with that, saving the result in this
// Union = the affine inequality that contains no more information than either
//         this OR that (i.e. the line that is higher than both lines)
// (moves this/that downward in the lattice)
void affineInequality::operator+=(const affineInequality& that)
{
	unionUpd(that);
}

// Just like +=, except unionUpd() returns true if the operation causes this
// affineInequality to change and false otherwise.
bool affineInequality::unionUpd(const affineInequality& that)
{
	bool modified=false;

	// top -> top
	// constrKnown -> constrKnown or constrKnown->higher
	// falseConstr -> falseConstr or higher
	// bottom -> bottom or higher
	
	// if either operand is top, the result is top
	if(level == top || that.level == top)
		modified = setToTop() || modified;
	// if one is = bottom or == falseConstr, the result is the other one
	else if((level == bottom      && that.level != bottom)/* ||
		     (level == falseConstr && that.level != falseConstr)*/)
	{
		modified = modified || level!=that.level;
		level = that.level;
		modified = modified || a!=that.a;
		a = that.a;
		modified = modified || b!=that.b;
		b = that.b;
		modified = modified || c!=that.c;
		c = that.c;
	}
	else if(that.level == bottom/* || that.level == falseConstr*/)
	{ }
	// if both levels are constrKnown
	else
	{	
		//      this                that
		// x*a <= y*b + c OR x*a' <= y*b' + c'
		// x*a*a' <= y*b*a' + c*a' OR x*a'*a <= y*b'*a + c'*a
		
		// the only way to upper-bound both lines is if they have the same slope
		if(b*that.a == a*that.b)
		{
			modified = modified || a!=a*that.a;
			a = a*that.a;
			modified = modified || b!=b*that.a;
			b = b*that.a;
			modified = modified || c!=max(c*that.a, a*that.c);
			c = max(c*that.a, a*that.c);
			modified = normalize() || modified;
		}
		// otherwise, the upper-bound cannot be represented as a line
		else
		{
			// set the constraint to x<=y+infinity, since this is a conservative
			// approximation (doesn't contain more information that is defninitely true)
			modified = setToTop() || modified;
		}
	}
	return modified;
}

// WIDEN this with that, saving the result in this
/*void affineInequality::operator^=(const affineInequality& that)
{
	// if this constraint is being widened to something that has more information (higher in the lattice) 
	// than it currently has, shift directly to top to avoid passing this constraint through an infinite 
	// number of lattice steps on its way to top
	if(*this << that)
		setToTop();
}*/

// returns true if the x-y constraint constrXY is consistent with the y-x constraint constrYX for 
// some values of x and y and false otherwise. Since affineInequalities are conservative in the
// sense that they don't contain any more information than is definitely true, it may be
// that the true constraints are in fact inconsistent but we do not have enough information to
// prove this.
bool affineInequality::mayConsistent(const affineInequality& constrXY, const affineInequality& constrYX)
{
	// it is meaningless to call this function with bottom arguments
	ROSE_ASSERT(constrXY.level != bottom);
	ROSE_ASSERT(constrYX.level != bottom);
	
	// x<=y+infinity and y*a<=x*b+c
	if(constrXY.level == top)
		return constrYX.level>=constrKnown;
	// x*a<=y*b+c and y<=x+infinity
	else if(constrYX.level == top)
		return constrXY.level>=constrKnown;
	// if either constraint is false, their conjunction is inconsistent
	/*else if(constrXY.level==falseConstr || constrYX.level==falseConstr)
		return false;*/
	
	ROSE_ASSERT(constrXY.level == constrKnown && constrYX.level == constrKnown);
	// x*a <= y*b + c AND y*a' <= x*b' + c'
	// x*a*a' <= y*b*a' + c*a' AND y*a'*b <= x*b'*b + c'*b
	// x*a*a' <= x*b'*b + c'*b + c*a'
	// x*(a*a'-b'*b) <= c'*b + c*a'
	// violation: a*a'-b'*b==0  and 0-c'*b > c*a'
	if((constrXY.getA()*constrYX.getA()-constrXY.getB()*constrYX.getB() == 0) &&
		(0-constrYX.getC()*constrXY.getB()) > constrXY.getC()*constrYX.getA())
	{
		if(analysisDebugLevel>=1)
		{
			cout << "constrXY = "<<constrXY.str("")<<"\n";
			cout << "constrYX = "<<constrYX.str("")<<"\n";
			printf("constrXY.getA()*constrYX.getA() = %d\n", constrXY.getA()*constrYX.getA());
			printf("constrXY.getB()*constrYX.getB() = %d\n", constrXY.getB()*constrYX.getB());
			printf("constrXY.getA()*constrYX.getA()-constrXY.getB()*constrYX.getB() = %d\n", (constrXY.getA()*constrYX.getA()-constrXY.getB()*constrYX.getB()));
			printf("constrYX.getC()*constrXY.getB() = %d\n", constrYX.getC()*constrXY.getB());
			printf("constrXY.getC()*constrXY.getA() = %d\n", constrXY.getC()*constrYX.getA());
		}
	   return false;
	}
	
	return true;
}

string affineInequality::signToString(signs sign)
{
	if(sign==unknownSgn)
		return "?Sgn";
	else if(sign==eqZero)
		return "eqZero";
	else if(sign==posZero)
		return "posZero";
	else if(sign==negZero)
		return "negZero";
	else
		return "???";
}
		
string affineInequality::str(string indent)// const
{
	ostringstream outs;
	
	if(level==bottom)
		//outs << "<constraint: ["<<y.str()<<"] bottom>";
		outs << indent << "<aI: bottom>";
	else if(level==constrKnown)
		//outs << "<affineInequality: x*"<<a<<" <= "<<y.str<<"*"<<b<<" + "<<c<<">";
		outs << indent << "<aI: x*"<<a<<" <= y*"<<b<<" + "<<c<<"|"<<signToString(xSign)<<"|"<<signToString(ySign)<<">";
	else if(level==top)
		//outs << "<affineInequality: ["<<y.str()<<"] top>";
		outs << indent << "<aI: top>";
	
	return outs.str();
}

string affineInequality::str(string indent) const
{
	ostringstream outs;
	
	if(level==bottom)
		//outs << "<constraint: ["<<y.str()<<"] bottom>";
		outs << indent << "<aI: bottom>";
	else if(level==constrKnown)
		//outs << "<affineInequality: x*"<<a<<" <= "<<y.str<<"*"<<b<<" + "<<c<<">";
		outs << indent << "<aI: x*"<<a<<" <= y*"<<b<<" + "<<c<<"|"<<signToString(xSign)<<"|"<<signToString(ySign)<<">";
	else if(level==top)
		//outs << "<affineInequality: ["<<y.str()<<"] top>";
		outs << indent << "<aI: top>";
	
	return outs.str();
}

string affineInequality::str(varID x, varID y, string indent) const
{
	ostringstream outs;
	
	if(level==bottom)
		outs << indent << "<aI: ["<<x.str()<<"->"<<y.str()<<"] bottom>";
	/*else if(level==falseConstr)
		outs << indent << "<aI: ["<<x.str()<<"->"<<y.str()<<"] falseConstr>";*/
	else if(level==constrKnown)
		outs << indent << "<aI: "<<x.str()<<"*"<<a<<" <= "<<y.str()<<"*"<<b<<" + "<<c<<"|"<<signToString(xSign)<<"|"<<signToString(ySign)<<">";
	else if(level==top)
		outs << indent << "<aI: ["<<x.str()<<"->"<<y.str()<<"] top>";
	
	return outs.str();
}

/*// the basic logical operations that must be supported by any implementation of 
// a logical condition: NOT, AND and OR
void affineInequality::notUpd()
{
	// The negation of [x*a <= y*b + c] is [x*a > y*b + c] == [y*b + c < x*a] == [y*b <= x*a - c - 1]

	// x<=y-infinity => x>y-infinity == y<=x+infinity
	if(level == falseConstr)
		setToTop();
	else if(level == constrKnown)
	{
		int tmp=a;
		a = b;
		b = tmp;
		c = -1-c;
	}
}

void affineInequality::andUpd(LogicalCond& that)
{
	*this *= (affineInequality&)that;
}

void affineInequality::orUpd(LogicalCond& that)
{
	*this += (affineInequality&)that;
}*/

