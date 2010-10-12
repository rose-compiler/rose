#include <math.h>
#include "intArithLogical.h"

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

/************
 * exprLeaf *
 ************/
IntArithLogical::exprLeaf::exprLeaf(SpearOp cmp, int a, varID x, int b, varID y, int c): cmp(cmp)
{
	this->a = a;
	this->x = x;
	this->b = b;
	this->y = y;
	this->c = c;
	normalize();
	varsExprInitialized = false;
	level = exprKnown;
	outVar=NULL;
}
 
IntArithLogical::exprLeaf::exprLeaf(cmpOps cmp, int a, varID x, int b, varID y, int c): cmp(cmp)
{
	this->a = a;
	this->x = x;
	this->b = b;
	this->y = y;
	this->c = c;
	normalize();
	varsExprInitialized = false;
	level = exprKnown;
	outVar=NULL;
}

IntArithLogical::exprLeaf::exprLeaf(const exprLeaf& that): cmp(that.cmp)
{
	this->a = that.a;
	this->x = that.x;
	this->b = that.b;
	this->y = that.y;
	this->c = that.c;
	varsExprInitialized = false;
	level = that.level;
	outVar=NULL;
}

// divide out from a, b and c any common factors, reducing the triple to its normal form
// return true if this modifies this constraint and false otherwise
bool IntArithLogical::exprLeaf::normalize()
{
	//printf("IntArithLogical::exprLeaf::normalize() a=%d, b=%d, c=%d\n", a, b, c);
	
	bool modified = false;
	if(c == 0)
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
		//printf("    gcd_ab=%d, gcd_abc=%d\n", gcd_ab, gcd_abc);

		modified = (a == a/gcd_abc) &&
                 (b == b/gcd_abc) &&
                 (c == c/gcd_abc);
		
		a = a/gcd_abc;
		b = b/gcd_abc;
		c = c/gcd_abc;
	}
	//printf("After: a=%d, b=%d, c=%d\n", a, b, c);
	return modified;
}

// computes vas and logExpr
void IntArithLogical::exprLeaf::computeVarsExpr()
{
	if(!varsExprInitialized)
	{
		bool outVarInVars=false;
		// if vars contains SpearVars from the lass call to computeVarsExpr, delete these SpearVars
		for(list<SpearAbstractVar*>::iterator it = vars.begin(); it != vars.end(); it++)
		{
			if(outVar == *it) outVarInVars = true;
			delete *it;
		}
		vars.clear();
		// if outVar was not in vars (i.e. its a constant set when level==itTrue or level==isFalse), delete it
		if(outVarInVars)
			delete outVar;

		if(level==isTrue)
		{
			logExpr = "";
			SpearConst* one = new SpearConst(1, 1);
			outVar = one;
		}
		else if(level==isFalse)
		{
			logExpr = "";
			SpearConst* zero = new SpearConst(0, 1);
			outVar = zero;
		}
		else
		{	
			SpearVar* xSVar = new SpearVar(x, 32);
			vars.push_back(xSVar);
			// a*x
			SpearVar* aMxSVar = new SpearVar("aMx", 32, getUID());
			vars.push_back(aMxSVar);
		
			SpearVar* ySVar = new SpearVar(y, 32);
			vars.push_back(ySVar);
			// b*y
			SpearVar* bMySVar = new SpearVar("bMy", 32, getUID());
			vars.push_back(bMySVar);
		
			// b*y + c
			SpearVar* bMyPcSVar = new SpearVar("bMyPc", 32, getUID());
			vars.push_back(bMyPcSVar);
			
			// a*x cmp b*y + c
			SpearVar* relSVar = new SpearVar("rel", 1, getUID());
			vars.push_back(relSVar);
			
			ostringstream ss;
			ss << "c " << aMxSVar->getName()   << " "<<SpearOp::opStr(SpearOp::Multiply)<<" " << xSVar->getName()<<" "<<a<<":"<<SpearType::typeName(32)<<"\n";
			ss << "c " << bMySVar->getName()   << " "<<SpearOp::opStr(SpearOp::Multiply)<<" " << ySVar->getName()<<" "<<b<<":"<<SpearType::typeName(32)<<"\n";
			ss << "c " << bMyPcSVar->getName() << " "<<SpearOp::opStr(SpearOp::Add)<<" "      << bMySVar->getName()<<" "<<c<<":"<<SpearType::typeName(32)<<"\n";
			ss << "c " << relSVar->getName()   << " "<<cmp.opStr()<<" "                       << aMxSVar->getName()<<" "<<bMyPcSVar->getName()<<"\n";
			logExpr = ss.str();
			
			//printf("exprLeaf::computeVarsExpr() vars.size()=%d\n", vars.size());
			
			outVar = relSVar;	
		}
		
		varsExprInitialized = true;
	}
}

// Sets this expression to True, returning true if this causes
// the expression to be modified and false otherwise.
bool IntArithLogical::exprLeaf::setToTrue()
{
	if(level == isTrue) return false;

	a = 1;
	b = 1;
	c = 0;
	level = isTrue;
	cmp.setOp(SpearOp::None);
	varsExprInitialized=false;
	return true;
}

// Sets this expression to False, returning true if this causes
// the expression to be modified and false otherwise.
bool IntArithLogical::exprLeaf::setToFalse()
{
	if(level == isFalse) return false;

	a = 1;
	b = 1;
	c = 0;
	level = isFalse;
	cmp.setOp(SpearOp::None);
	varsExprInitialized=false;
	return true;
}

IntArithLogical::exprLeafOrNode::infContent IntArithLogical::exprLeaf::getLevel()
{
	return level;
}

// returns a list of the names of all the variables needed to represent the logical 
// expression in this object
const list<SpearAbstractVar*>& IntArithLogical::exprLeaf::getVars()
{
	computeVarsExpr();
	//printf("exprLeaf::getVars() vars.size()=%d\n", vars.size());
	return vars;
}

// returns the variable that represents the result of this expression
SpearAbstractVar* IntArithLogical::exprLeaf::getOutVar()
{
	computeVarsExpr();
	return outVar;
}

// returns the SPEAR Format expression that represents the constraint in this object
const string& IntArithLogical::exprLeaf::getExpr()
{ 
	computeVarsExpr();
	return logExpr;
}

// Returns a human-readable string representation of this expression.
// Every line of this string must be prefixed by indent.
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string IntArithLogical::exprLeaf::str(string indent)
{ return genStr(); }

string IntArithLogical::exprLeaf::str(string indent) const
{ return genStr(); }

string IntArithLogical::exprLeaf::genStr(string indent) const	
{
	ostringstream ss;

	if(level==isTrue)
	{
		ss << indent << "exprLeaf[isTrue]";
	}
	else if(level==isFalse)
	{
		ss << indent << "exprLeaf[isFalse]";
	}
	else
	{	
		ss << indent << "exprLeaf[" << a << "*" << x.str();
		
		if(cmp.isOp(SpearOp::Equal)) ss << " == ";
		else if(cmp.isOp(SpearOp::NotEqual)) ss << " != ";
		else if(cmp.isOp(SpearOp::SgnLTE)) ss << " <= ";
		else if(cmp.isOp(SpearOp::SgnGT)) ss << " > ";
		else ROSE_ASSERT(0);
		
		ss << b << "*" << y.str() << " + " << c << "]";
	}
	
	return ss.str();
}
	
SpearExpr* IntArithLogical::exprLeaf::copy()
{
	return new exprLeaf(*this);
}

// Negate this logical expression
// Returns true if this causes this logicNode to change, false otherwise.
bool IntArithLogical::exprLeaf::notUpd()
{
	if(cmp.getOp() == SpearOp::Equal)
	{
		cmp.setOp(SpearOp::NotEqual);
	}
	else if(cmp.getOp() == SpearOp::SgnLTE)
	{
		// ~(a*x <= b*y + c) = a*x > b*y + c
		cmp.setOp(SpearOp::SgnGT);
	}
	else if(cmp.getOp() == SpearOp::SgnGT)
	{
		// ~(a*x > b*y + c) = a*x <= b*y + c
		cmp.setOp(SpearOp::SgnLTE);
	}
	else
		ROSE_ASSERT(0);
	varsExprInitialized = false;

	return true;
}

// And-s this expression with the given expression, if possible
// Return true if the conjunction is possible and this now contains the conjunction expression and
//        false if it is not possible.
// If the conjunction is possible, modified is set to true if the conjunction causes
//    this to change and false otherwise.
bool IntArithLogical::exprLeaf::andExprs(const exprLeaf& that, bool &modified)
{
	modified = false;
	//if(x==that.x && y==that.y && cmp==that.cmp)
	if(andExprsTest(that))
	{
		//cout << "        exprLeaf::andExprs() conjoining        \n            this="<<str("        ")<<"            \n            that="<<that.str()<<"\n";
		
		// if the expressions are the same, their conjunction is equal to this
		if(a==that.a && b==that.b && c==that.c)
			return true;
		
		// else, if the expressions are different
		if(cmp.isOp(SpearOp::Equal))
		{
			// if the two lines never intersect (same slope, different y-intercept)
			if(a*that.b == that.a*b && c!=that.c)
			{
				// we set this object to False because in reality this conjunction
				// eliminates all points (x,y)
				modified = setToFalse() || modified;
			}
			// else, if the two equalities intersect on a single point, we leave this 
			// as it is since both lines conservatively capture the point
		}
		else if(cmp.isOp(SpearOp::NotEqual))
		{
			// The conjunction of two different not-equality expressions can be conservatively
			// represented (i.e. without introducing new information) as either of the 
			// not-equality expressions.
		}
		else if(cmp.isOp(SpearOp::SgnLTE))
		{
			// a*x <= b*y + c AND a'*x <= b'*y + c'
			// if a/b==a'/b' (a*b' == a'*b, AKA same slope), the AND is the tighter affine inequality: 
			// the one with the smaller x-intercept
			if(a*that.b == b*that.a)
			{
				// x-intercept = c/a, c'/a'. 
				//     c/a <= c'/a' == c*a' <= c'*a
				
				//printf("c*that.a=%d, that.c*a=%d\n", c*that.a, that.c*a);
				// that has smaller x-intercept
				if(that.c*a < c*that.a)
				{
					//printf("b*that.a=%d\n", b*that.a);
					a = that.a;
					b = that.b;
					c = that.c;
					modified = true;
				}
			}
			// Otherwise, the two lines cross each other, meaning that either line conservatively approximates 
			// the set of impossible points (x,y) in that neither line accepts rejects any point accepted
			// by the conjunction. Thus, we leave this alone
		}
		else if(cmp.isOp(SpearOp::SgnGT))
		{
			// a*x > b*y + c AND a'*x > b'*y + c'
			// if a/b==a'/b' (a*b' == a'*b, AKA same slope), the AND is the tighter affine inequality: 
			// the one with the larger x-intercept
			if(a*that.b == b*that.a)
			{
				// x-intercept = c/a, c'/a'. 
				//     c/a <= c'/a' == c*a' <= c'*a
				
				// that has larger x-intercept
				if(that.c*a > c*that.a)
				{
					//printf("b*that.a=%d\n", b*that.a);
					a = that.a;
					b = that.b;
					c = that.c;
					modified = true;
				}
			}
			// Otherwise, the two lines cross each other, meaning that either line conservatively approximates 
			// the set of impossible points (x,y) in that neither line accepts rejects any point accepted
			// by the conjunction. Thus, we leave this alone
		}
		//cout << "        exprLeaf::andExprs() result="<<str()<<"\n";
		
		//cout << "andExprs() returning true\n";
		return true;
	}
	else
	{
		//cout << "andExprs() returning false\n";
		return false;
	}
}

// returns the same thing as andExprs but doesn't actually perform the conjunction
bool IntArithLogical::exprLeaf::andExprsTest(const exprLeaf& that)
{
	return (x==that.x && y==that.y && cmp==that.cmp);
}

// Or-s this expression with the given expression, if possible
// Return true if the disjunction is possible and this now contains the disjunction expression and
//        false if it is not possible.
// If the conjunction is possible, modified is set to true if the conjunction causes
//    this to change and false otherwise.
bool IntArithLogical::exprLeaf::orExprs(const exprLeaf& that, bool &modified)
{
	modified = false;
	//if(x==that.x && y==that.y && cmp==that.cmp)
	if(orExprsTest(that))
	{
		// if the expressions are the same, their disjunction is equal to this
		if(a==that.a && b==that.b && c==that.c)
			return true;
		
		// else, if the expressions are different
		if(cmp.isOp(SpearOp::Equal))
		{
			// if the two lines never intersect (same slope, different y-intercept)
			if(a*that.b == that.a*b && c!=that.c)
			{
				// The two lines can be represented as an inequality relation that includes
				// in its acceptance set all the points on both lines. Thus, we simply need
				// to use the a, b and c of the line with the larger x-intercept.

				// x-intercept = c/a, c'/a'. 
				//     c/a <= c'/a' == c*a' <= c'*a
									
				// that has larger x-intercept
				if(that.c*a > c*that.a)
				{
					//printf("b*that.a=%d\n", b*that.a);
					a = that.a;
					b = that.b;
					c = that.c;
				}
				cmp.setOp(SpearOp::SgnLTE);
				modified = true;
			}
			
			// The disjunction of two different equality expressions can only be conservatively
			// represented as True 
		}
		else if(cmp.isOp(SpearOp::NotEqual))
		{
			// the disjunction of two different not-equality expressions is True (i.e. all points (x,y) are accepted)
			modified = setToTrue() || modified;
		}
		else if(cmp.isOp(SpearOp::SgnLTE))
		{
			// a*x <= b*y + c OR a'*x <= b'*y + c'
			// if a/b==a'/b' (a*b' == a'*b, AKA same slope), the OR is the looser affine inequality: the one with the larger x-intercept
			if(a*that.b == b*that.a)
			{
				// x-intercept = c/a, c'/a'. 
				//     c/a <= c'/a' == c*a' <= c'*a

				// that has larger x-intercept
				if(that.c*a > c*that.a)
				{
					// x-intercept = c/a, c'/a'. 
					//     c/a <= c'/a' == c*a' <= c'*a
				
					//printf("b*that.a=%d\n", b*that.a);
					a = that.a;
					b = that.b;
					c = that.c;
					modified = true;
				}
			}
			// Otherwise, the two lines cross each other, meaning that the only conservative affine relationship
			// that covers both is one where c = infinity == True
			else
				modified = setToTrue() || modified;
		}
		else if(cmp.isOp(SpearOp::SgnGT))
		{
			// a*x > b*y + c AND a'*x > b'*y + c'
			// if a/b==a'/b' (a*b' == a'*b, AKA same slope), the AND is the looser affine inequality: the one with the smaller x-intercept
			if(a*that.b == b*that.a)
			{
				// x-intercept = c/a, c'/a'. 
				//     c/a <= c'/a' == c*a' <= c'*a

				// that has smaller x-intercept
				if(that.c*a < c*that.a)
				{
					// x-intercept = c/a, c'/a'. 
					//     c/a <= c'/a' == c*a' <= c'*a
				
					//printf("b*that.a=%d\n", b*that.a);
					a = that.a;
					b = that.b;
					c = that.c;
					modified = true;
				}
			}
			// Otherwise, the two lines cross each other, meaning that the only conservative affine relationship
			// that covers both is one where c = -infinity == True
			else
				modified = setToTrue() || modified;
		}
		return true;
	}
	else
		return false;
}

// returns the same thing as orExprs but doesn't actually perform the disjunction
bool IntArithLogical::exprLeaf::orExprsTest(const exprLeaf& that)
{
	return (x==that.x && y==that.y && cmp==that.cmp);
}

bool IntArithLogical::exprLeaf::operator==(exprLeafOrNode& that_arg)
{
	ROSE_ASSERT(that_arg.elType()==IntArithLogical::exprLeafOrNode::eLeaf);
	exprLeaf& that = dynamic_cast<exprLeaf&>(that_arg);

	return a     == that.a   && 
	       b     == that.b   && 
	       c     == that.c   && 
	       x     == that.x   && 
	       y     == that.y   && 
	       cmp   == that.cmp && 
	       level == that.level;
}

// Information order on affine relationships with the least information (True) being highest
// and most information (False) being lowest. Some expressions are not comparable since
// their sets of accepted points are not strictly related to each other.
bool IntArithLogical::exprLeaf::operator<=(exprLeafOrNode& that_arg)
{
	ROSE_ASSERT(that_arg.elType()==IntArithLogical::exprLeafOrNode::eLeaf);
	exprLeaf& that = dynamic_cast<exprLeaf&>(that_arg);
	
	if(*this == that) return true;
	
	if(that.level == isTrue) return true;
	if(level == isTrue) return false;
	if(that.level == isFalse) return false;
	if(level == isFalse) return true;
		
	ROSE_ASSERT(level==exprKnown && that.level==exprKnown);
	
	// The cases that are actually comparable, given that this != that:
	// (this vs that) LTE vs LTE, Eq vs LTE, LTE vs Eq, GT vs GT, Eq vs GT, GT vs Eq, where the slopes are the same
	// The comparable cases where this may contain the the same amount or more information than that:
	// (this vs that) LTE vs LTE, Eq vs LTE,            GT vs GT, Eq vs GT,           where the slopes are the same
	// The comparable cases that are also non-trivially handled by andExprs and orExprs:
	// (this vs that) LTE vs LTE,                       GT vs GT,                     where the slopes are the same
	// if the slopes are the same
	if(a*that.b == that.a*b)
	{
		if(cmp.isOp(SpearOp::SgnLTE))
		{
			if(that.cmp.isOp(SpearOp::SgnLTE))
			{
				// this <= that (i.e. that accepts a larger than or equal set of points)
				// if this represents a line with an x-intercept <= that's x-intercept
				return c*that.a <= that.c*a;
			}
		}
		/*else if(cmp.isOp(SpearOp::Eq)
		{
			if(that.cmp.isOp(SpearOp::SgnLTE))
			{
				// this < that (i.e. that accepts a larger set of points)
				// if this represents a line with an x-intercept <= that's x-intercept
				return c*that.a <= that.c*a;
			}
			else if(that.cmp.isOp(SpearOp::SgnGT))
			{
				// this < that (i.e. that accepts a larger set of points)
				// if this represents a line with an x-intercept > that's x-intercept
				return c*that.a > that.c*a;
			}
		}*/
		else if(cmp.isOp(SpearOp::SgnGT))
		{
			if(that.cmp.isOp(SpearOp::SgnGT))
			{
				// this <= that (i.e. that accepts a larger than or equal set of points)
				// if this represents a line with an x-intercept >= that's x-intercept
				return c*that.a >= that.c*a;
			}
		}
	}

	// If the two expressions are not comparable
	return false;
}

/*************
 * logicNode *
 *************/
// An internal node that represents a logical connective between the affine relations

IntArithLogical::logicNode::logicNode(logOps logOp): logOp(logOp)
{
	level = exprKnown;
	outVar = NULL;
	varsExprInitialized = false;

/*	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
	            ((level == isTrue || level == isFalse) && children.size() == 0));*/
}

// constructor for a logicNode with a single child
IntArithLogical::logicNode::logicNode(logOps logOp, exprLeafOrNode* child): logOp(logOp)
{
	// Currently not supporting not nodes
	//ROSE_ASSERT(0);
	//ROSE_ASSERT(logOp==notOp);
	children.push_back(child);
	level = exprKnown;
	outVar = NULL;
	varsExprInitialized = false;
	
	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
	            ((level == isTrue || level == isFalse) && children.size() == 0));
}

// constructor for a logicNode with two children
IntArithLogical::logicNode::logicNode(logOps logOp, exprLeafOrNode* childA, exprLeafOrNode* childB): logOp(logOp)
{
	//ROSE_ASSERT(logOp==SpearOp::AndOp || logOp==SpearOp::OrOp/* || logOp==SpearOp::XorOp*/);
	children.push_back(childA);
	children.push_back(childB);
	level = exprKnown;
	outVar = NULL;
	varsExprInitialized = false;
	
	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
	            ((level == isTrue || level == isFalse) && children.size() == 0));
}

// constructor for a logicNode with two or more children
IntArithLogical::logicNode::logicNode(logOps logOp, const list<exprLeafOrNode*>& children): logOp(logOp)
{
	ROSE_ASSERT(logOp==SpearOp::AndOp || logOp==SpearOp::OrOp/* || logOp==SpearOp::XorOp*/);
	this->children = children;
	level = exprKnown;
	outVar = NULL;
	varsExprInitialized = false;
	
	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
	            ((level == isTrue || level == isFalse) && children.size() == 0));
}

IntArithLogical::logicNode::logicNode(const logicNode& that): logOp(that.logOp)
{
	level = that.level;
	// set children to be a deep copy of that.children
	for(list<exprLeafOrNode*>::const_iterator it = that.children.begin(); it != that.children.end(); it++)
		children.push_back(dynamic_cast<exprLeafOrNode*>((*it)->copy()));
	varsExprInitialized = false;
	outVar = NULL;
	logExpr = "";

	//printf("level=%d that.level=%d logOp=%d that.logOp=%d children.size()=%d\n", level, that.level, logOp.getOp(), that.logOp.getOp(), children.size());
	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
	            ((level == isTrue || level == isFalse) && children.size() == 0));
}

IntArithLogical::logicNode::~logicNode()
{
	// delete all the children
	for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
		delete *it;
}

// add a new child to this node
void IntArithLogical::logicNode::addChild(exprLeafOrNode* child)
{
	if(level == exprKnown ||
	   (level == isTrue && logOp.isOp(SpearOp::AndOp)) ||
	   (level == isFalse && logOp.isOp(SpearOp::OrOp)))
	{
		children.push_back(child);
		level = exprKnown;
		varsExprInitialized = false;
	}

	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
	            ((level == isTrue || level == isFalse) && children.size() == 0));
}

// propagates all the true and false sub-terms upwards through the tree, pruning the appropriate portions
// returns true if this causes the logicNode to change and false otherwise
bool IntArithLogical::logicNode::normalize()
{
	if(level==isTrue)
		return false;
	else if(level==isFalse)
		return false;
	else
	{
		bool modified = false;
		list<list<exprLeafOrNode*>::iterator> toDelete;
		for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
		{
			if(logOp.isOp(SpearOp::OrOp))
			{
				// first, normalize the current child conjunct so that we can know whether this child is true or false
				ROSE_ASSERT((*it)->elType()==IntArithLogical::exprLeafOrNode::lNode);
				logicNode* ln = dynamic_cast<logicNode*>(*it);
	
				ln->normalize();
			}
			else ROSE_ASSERT(logOp.isOp(SpearOp::AndOp));
			
			// any true conjuncts should be deleted, since they carry no information
			if((*it)->getLevel() == isTrue)
				toDelete.push_back(it);
			// a False conjunct implies that the entire conjunction is false
			else if((*it)->getLevel() == isFalse)
				return setToFalse() || modified;
		}
		modified = toDelete.size()>0 || modified;
		
		for(list<list<exprLeafOrNode*>::iterator>::iterator it = toDelete.begin();
		    it != toDelete.end(); it++)
		{
			delete **it;
			children.erase(*it);
		}
		
		// if we've emptied out the children list
		if(children.size()==0)
		{
			if(logOp.isOp(SpearOp::AndOp))
				// Conjunctions get emptied out because all the children are  = true
				return setToTrue() || modified;
			else if(logOp.isOp(SpearOp::OrOp))
				// Disjunctions get emptied out because all the children are = false
				return setToFalse() || modified;
			else
				ROSE_ASSERT(0);
		}
		
		ROSE_ASSERT((level == exprKnown && children.size()>0) || 
            ((level == isTrue || level == isFalse) && children.size() == 0));
		
		return modified;
	}
}

// Sets this expression to True, returning true if this causes
// the expression to be modified and false otherwise.
bool IntArithLogical::logicNode::setToTrue()
{
	bool modified = level!=isTrue;
	
	level = isTrue;
	// delete all the children
	for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
		delete *it;
	children.clear();
	varsExprInitialized = false;

	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
         ((level == isTrue || level == isFalse) && children.size() == 0));
   
   return modified;
}

// Sets this expression to False, returning true if this causes
// the expression to be modified and false otherwise.
bool IntArithLogical::logicNode::setToFalse()
{
	bool modified = level!=isFalse;
	
	level = isFalse;
	// delete all the children
	for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
		delete *it;
	children.clear();
	varsExprInitialized = false;

	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
         ((level == isTrue || level == isFalse) && children.size() == 0));
   
   return modified;
}

IntArithLogical::exprLeafOrNode::infContent IntArithLogical::logicNode::getLevel()
{
	return level;
}


// returns a list of the names of all the variables needed to represent the logical 
// expression in this object
const list<SpearAbstractVar*>& IntArithLogical::logicNode::getVars()
{
	computeVarsExpr();
	return vars;
}

// returns the variable that represents the result of this expression
SpearAbstractVar* IntArithLogical::logicNode::getOutVar()
{
	computeVarsExpr();
	return outVar;
}

// returns the SPEAR Format expression that represents the constraint in this object
const string& IntArithLogical::logicNode::getExpr()
{
	computeVarsExpr();
	return logExpr;
}

// Returns a human-readable string representation of this expression.
// Every line of this string must be prefixed by indent.
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string IntArithLogical::logicNode::str(string indent)
{ return genStr(); }

string IntArithLogical::logicNode::str(string indent) const
{ return genStr(); }

string IntArithLogical::logicNode::genStr(string indent) const
{
	ostringstream ss;
	
	if(level == exprKnown)
	{
		//printf("logOp.opStr()=%s\n", logOp.opStr().c_str());
		ss << indent << "logicNode[logOp="<<logOp.opStr()<<", \n";
		for(list<exprLeafOrNode*>::const_iterator it = children.begin(); it != children.end(); )
		{
			ss << indent << "    " << (*it)->str(indent + "        ");
			it++;
			if(it != children.end())
				ss << "\n";
			else
				ss << "]";
		}
	}
	else if(level == isTrue)
	{ ss << indent << "logicNode[logOp="<<logOp.opStr()<<", isTrue]"; }
	else if(level == isFalse)
	{ ss << indent << "logicNode[log="<<logOp.opStr()<<", isFalse]"; }
	
	return ss.str();	
}

// Aggregates the conjunction or disjunction of the results of a number of expressions.
// Specifically, we will use a binary reduction tree, where n expressions will be
// reduced as (in the idealized case of n=2**k:
// l{k}_{1} cmp e{1} e{2}
// l{k}_{2} cmp e{3} e{4}
// ... 
// l{k}_{n/2} cmp e{n-1} e{n}
// l{k-1}_{1} cmp l{k}_{1} l{k}_{2}
// l{k-1}_{1} cmp l{k}_{3} l{k}_{4}
// ...
// l{k-1}_{n/4} cmp l{k}_{n/2-1} l{k}_{n/2}
// ...
// l{1}_{1} = cmp l{2}_{1} l{2}_{2}

// op [IN] - Spear operation we'll use to aggregate individual expression results
// ss [OUT] - the stringstream that we'll be writing our final result to
// baseRedVarName [IN] - prefix of all the reduction variable names
// newVars [OUT] - all the new vars generated as part of the reduction tree will be added to newVars
// inputVars [IN] - list of variables to be reduced
// uid [IN] - the unique ID to be assigned to all the variables generated inside this call or -1 if no unique id is to be used
// returns a pointer to the variable that represents the entire reduction tree
SpearAbstractVar* genReductionTree(SpearOp op, ostream& ss, string baseRedVarName, 
                                   list<SpearAbstractVar*>& newVars, 
                                   list<SpearAbstractVar*>& inputVars, int uid)
{
	// Number of levels in the reduction tree
	int log2N = (int)ceil(log(inputVars.size())/log(2));

	// list of variables to be reduced in the current level of the reduction hierarchy
	list<SpearAbstractVar*> redVars = inputVars;

	// Iterate through the reduction tree, printing out expressions and adding the reduction
	// variables to vars
	int curLevel = log2N;
	while(redVars.size()>1)
	{
		int varIndex=0;
		// The reduction variables for this level. Will become redVars for the level above it.
		list<SpearAbstractVar*> newRedVars;
		for(list<SpearAbstractVar*>::iterator it = redVars.begin(); it!=redVars.end(); it++, varIndex++)
		{
			// For every pair of variables, create a reduction variable
			if(varIndex%2 == 0)
			{
				ostringstream ss2;
				ss2 << baseRedVarName << curLevel << "_" << ((int)(varIndex/2));
				string redVarName = ss2.str();
				SpearVar *redSVar;
				if(uid==-1)
					redSVar = new SpearVar(redVarName, 1);
				else
					redSVar = new SpearVar(redVarName, 1, uid);
				
				ss << "c " << redSVar->getName() << " "<<op.opStr()<<" "<<(*it)->getName();
				
				// add the reduction variable to the list of variables that we'll reduce
				// at the next higher level of the tree
				newRedVars.push_back(redSVar);
				
				// store a reference to the reduction variable in vars, since this is
				// a real variable of this logical expression
				newVars.push_back(redSVar);
			}
			else
				// all the second variable in the pair to the reduction expression
				ss << " "<<(*it)->getName()<<"\n";;
		}
		
		// if the number of variables at this level is odd, the last reduction variable
		// reduces only one variable, so its second argument must be a noop constant
		if(varIndex%2 == 1)
		{
			if(op.isOp(SpearOp::AndOp))
			{
				SpearConst one(1, 1);
				ss << " "<<one.getName()<<"\n";;;
			}
			else if(op.isOp(SpearOp::OrOp))
			{
				SpearConst zero(0, 1);
				ss << " "<<zero.getName()<<"\n";
			}
	/*		else if(op.isOp(SpearOp::XorOp))
				baseRedVarName = "XOR_l";
			else if(op.isOp(SpearOp::NotOp))
				baseRedVarName = "NOT_l";*/
		}
		
		// reset redVars to refer to the preceding level's variables
		redVars = newRedVars;
		
		curLevel--;
	}

	return redVars.front();
}


void IntArithLogical::logicNode::computeVarsExpr()
{
	if(!varsExprInitialized)
	{
		varsExprInitialized = true;
		ostringstream ss;
		
		bool outVarInVars=false;
		// if vars contains SpearVars from the last call to computeVarsExpr, delete these SpearVars
		for(list<SpearAbstractVar*>::iterator it = vars.begin(); it != vars.end(); it++)
		{
			if(outVar == *it) outVarInVars = true;
			delete *it;
		}
		vars.clear();
		// if outVar was not in vars (i.e. its a constant set when level==itTrue or level==isFalse), delete it
		if(!outVarInVars)
			delete outVar;
		
		if(level == isTrue)
		{
			SpearConst* one = new SpearConst(1, 1);
			outVar = one;
			logExpr = "";
		}
		else if(level == isFalse)
		{ 
			SpearConst* zero = new SpearConst(0, 1);
			outVar = zero;
			logExpr = "";
		}
		else if(level == exprKnown)
		{
			// First, copy the variables from the children into vars 
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
			{
				const list<SpearAbstractVar*>& sv = (*it)->getVars();
				for(list<SpearAbstractVar*>::const_iterator itSV = sv.begin(); itSV != sv.end(); itSV++)
				{
					vars.push_back(*itSV);
				}
			}
			
			// Next, print out the children's logical expressions
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
				ss << (*it)->getExpr();
			
			// We'll need to aggregate the conjunction or disjunction of the results of all the children's
			// expressions. Specifically, we will use a binary reduction tree, using the function genReductionTree()
			string baseRedVarName="";
			if(logOp.isOp(SpearOp::AndOp))
				baseRedVarName = "AND_l";
			else if(logOp.isOp(SpearOp::OrOp))
				baseRedVarName = "OR_l";
	/*		else if(logOp.isOp(SpearOp::XorOp))
				baseRedVarName = "XOR_l";
			else if(logOp.isOp(SpearOp::NotOp))
				baseRedVarName = "NOT_l";*/
			
			list<SpearAbstractVar*> inputVars;
			// initialize inputVars to be the children's output variables
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
				inputVars.push_back((*it)->getOutVar());
			
			// Generate the reduction tree
			outVar = genReductionTree(logOp, ss, baseRedVarName, vars, inputVars, getUID());
			
			logExpr = ss.str();
		}
	}	
}

SpearExpr* IntArithLogical::logicNode::copy()
{
	return new logicNode(*this);
	
/*	if(logOp.isOp(SpearOp::NotOp))
	{
		return new logicNode(notOp, children[0]->copy());
	}
	else
	{
		return new logicNode(notOp, children[0]->copy(), children[1]->copy());
	}*/
}

// Negate this logical expression
// Returns true if this causes this logicNode to change, false otherwise.
bool IntArithLogical::logicNode::notUpd()
{
	if(level == exprKnown)
	{
		if(logOp.isOp(SpearOp::AndOp))
		{
			//logOp.setOp(SpearOp::OrOp);
	
			// negate the children
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
			{
				(*it)->notUpd();
			}
		}
		else if(logOp.isOp(SpearOp::OrOp))
		{
			// negate the children
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
				(*it)->notUpd();
			
			// We now have a tree that got turned from disjunctive normal form
			// to conjunctive normal form. We need to reorganize it to get it back in form.
			
			// First, collect the grand-children
			list<exprLeafOrNode*> newChildren;
			list<exprLeafOrNode*> newConjOrig;
			// generate the list of new conjuncts
			genChildrenConj(newChildren, newConjOrig, children.begin());
	
			// delete all the old children and grandchildren
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
				delete *it;
			children.clear();
			
			// children now refers to the newly negated children
			children = newChildren;
		}
		else if(logOp.isOp(SpearOp::NotOp))
			ROSE_ASSERT(0);
		else
			ROSE_ASSERT(0);
		
		// normalize the newly-negated expression
		normalize();
	}
	else if(level == isTrue)
	{ setToFalse(); }
	else if(level == isFalse)
	{ setToTrue(); }

	varsExprInitialized = false;
	
	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
         ((level == isTrue || level == isFalse) && children.size() == 0));
	return true;
}

// given a 2-level expression tree, collects all the permutations of grandchildren
// from each child branch into conjunct logicNodes and saves them in newChildren.
// Thus, (A^B^C)v(D^E^F) would become the list (A^D),(A^E),(A^F),(B^D),(B^E),(B^F),(C^D),(C^E),(C^F).
// The function works recursively, one child at a time.
// newChildren: The list where we'll save the new conjuncts
// newConjOrig: Accumulated stack of grandchildren that will be combined to form each conjunct
//              Thus, for input (A^B)v(C^D)v(D^E)v(F^G)v(H^I), the contents of newConjOrig
//              may be A,C,E,F, with H or I to be added when the function recurses again.
// curChild: Iterator that refers to the current child we're processing. The conjuncts
//           are formed whenever curChild reaches the end of children.
void IntArithLogical::logicNode::genChildrenConj(list<exprLeafOrNode*>& newChildren, list<exprLeafOrNode*> newConjOrig, 
                                                 list<exprLeafOrNode*>::const_iterator curChild)
{
	// if newConj contains pointers to all the leaves that will make up this new conjunct
	if(curChild == children.end())
	{
		// create a set of new children that corresponds to this conjunct
		logicNode* conj = new logicNode(andOp);
		for(list<exprLeafOrNode*>::iterator it = newConjOrig.begin(); it != newConjOrig.end(); it++)
		{
			conj->addChild(dynamic_cast<exprLeafOrNode*>((*it)->copy()));
		}
		// add the new conjunct to the list of conjuncts we'll use to make the
		// new disjunctive normal form representation of this object
		newChildren.push_back(conj);
	}
	// else, if we're only partly through the children list
	else
	{
		// add all the children of the current child to newConjOrig
		ROSE_ASSERT((*curChild)->elType()==IntArithLogical::exprLeafOrNode::lNode);
		logicNode* ln = dynamic_cast<logicNode*>(*curChild);
		// advance the child iterator so that for all the recursive calls in this function
		// it refers to the next child
		curChild++;
		for(list<exprLeafOrNode*>::iterator it = ln->children.begin(); it != ln->children.end(); it++)
		{
			// create all the conjuncts that contain the current grandchild
			ROSE_ASSERT((*it)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
			//exprLeaf* el = \dynamic_cast<exprLeaf*>(*it);
			//newConjOrig.push_back(el);
			newConjOrig.push_back(*it);
			
			genChildrenConj(newChildren, newConjOrig, curChild);

			newConjOrig.pop_back();
		}
	}
}

// ANDs this and that, storing the result in this.
// Returns true if this causes this logicNode to change, false otherwise.
bool IntArithLogical::logicNode::andUpd(logicNode& that)
{
	bool modified=false;
	
	// we may only combine logicNodes of the same type
	//printf("andUpd() logOp=%d that.logOp=%d\n", logOp.getOp(), that.logOp.getOp());
	//printf("andUpd() level=%d that.level=%d\n", level, that.level);
	ROSE_ASSERT(logOp == that.logOp);
	
	if(level == exprKnown && that.level == exprKnown)
	{
		printf("logOp = %d\n", logOp.getOp());
		if(logOp.isOp(SpearOp::AndOp))
		{
			// (A^B^C) ^ (D^E^F) = (A^B^C^D^E^F)
			// insert copies of all of that's children into this->children
			for(list<exprLeafOrNode*>::iterator itThat = that.children.begin(); itThat != that.children.end(); itThat++)
			{
				bool mergeable=false;
				ROSE_ASSERT((*itThat)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
				exprLeaf* thatEL = dynamic_cast<exprLeaf*>(*itThat);
					
				// first, check if this new child can be merged with this' existing children
				for(list<exprLeafOrNode*>::iterator itThis = children.begin(); itThis != children.end(); itThis++)
				{
					ROSE_ASSERT((*itThis)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
					exprLeaf* thisEL = dynamic_cast<exprLeaf*>(*itThis);
					bool modifiedAnd=false;
					
					// if thisEL and thatEL can be conjoined into a single expression
					if(thisEL->andExprs(*thatEL, modifiedAnd))
					{
						mergeable = true;
						modified = modified || modifiedAnd;
						break;
					}
				}
				
				// if thatEL cannot be merged with any expression in thisEL
				if(!mergeable)
				{
					// add thatEL to children as it is
					children.push_back(dynamic_cast<exprLeafOrNode*>((*itThat)->copy()));
					
					modified = true;
				}
			}
		}
		else if(logOp.isOp(SpearOp::OrOp))
		{
			// ((A^B^C)v(D^E^F)) ^ ((A'^B'^C')v(D'^E'^F')) = 
			// (A^B^C^A'^B'^C')v(A^B^C^D'^E'^F')v(D^E^F^A'^B'^C')v(D^E^F^D'^E'^F')
			
			// list containing this object's new set of children
			list<exprLeafOrNode*> newChildren;
			
			// Iterate through all pairs of children from this and that
			// and create a new conjunct for each pair.
			for(list<exprLeafOrNode*>::iterator itThis = children.begin(); itThis != children.end(); itThis++)
			{
				ROSE_ASSERT((*itThis)->elType()==IntArithLogical::exprLeafOrNode::lNode);
				logicNode* thisC = dynamic_cast<logicNode*>(*itThis);
				ROSE_ASSERT(thisC->logOp.isOp(SpearOp::AndOp));
				//cout << "thisC = "<<thisC->str("                  ")<<"\n";
				//printf("that.children.size() = %d\n", that.children.size());
				
				for(list<exprLeafOrNode*>::iterator itThat = that.children.begin(); itThat != that.children.end(); itThat++)
				{
					ROSE_ASSERT((*itThat)->elType()==IntArithLogical::exprLeafOrNode::lNode);
					logicNode* thatC = dynamic_cast<logicNode*>(*itThat);
					ROSE_ASSERT(thatC->logOp.isOp(SpearOp::AndOp));
					//cout << "    thatC = "<<thatC->str("                  ")<<"\n";
					
					// create a copy of thisC and AND it with thatC
//cout << "andUpd::Or creating a copy of thisC="<<thisC->str()<<"\n";
					logicNode* newTerm = dynamic_cast<logicNode*>(thisC->copy());
//printf("thisC=%p thatC=%p newTerm=%p\n", thisC, thatC, newTerm);
//printf("thisC->logOp=%d thatC->logOp=%d newTerm->logOp=%d\n", thisC->logOp.getOp(), thatC->logOp.getOp(), newTerm->logOp.getOp());
//cout << "andUpd::Or recursive call to andUpd::And\n";
					newTerm->andUpd(*thatC);
					//cout << "    newTerm = "<<newTerm->str("                  ")<<"\n";
					
					// insert newTerm into newChildren, taking into account any redundancies relative
					// to previously inserted terms
					insertNewChildOr(newChildren, newTerm);
				}
			}
			
			// this object has been modified iff the new children list differs from the old children list
			modified = eqChildren(children, newChildren);
			
			// delete all the old conjuncts
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
				delete *it;
			children.clear();
			
			// replace the old children list with the new
			children = newChildren;
			
			normalize();
		}
		/*else if(logOp.isOp(SpearOp::NotOp))
			ROSE_ASSERT(0);*/
		else
			ROSE_ASSERT(0);
	}
	/*else if(logOp.isOp(SpearOp::AndOp))
	{*/
		// the cases where that doesn't change this
		else if(level == isFalse || that.level == isTrue)
		{}
		// the cases where this must become False
		else if(that.level == isFalse)
		{ return setToFalse(); }
		// the cases where this must become a copy of that
		else if(level == isTrue)
		{
			//printf ("Copying!\n");
			ROSE_ASSERT(that.children.size()>0);
			for(list<exprLeafOrNode*>::iterator it = that.children.begin(); it != that.children.end(); it++)
				children.push_back(dynamic_cast<exprLeafOrNode*>((*it)->copy()));
			level = exprKnown;
		}
		else
			ROSE_ASSERT(0);
	/*}
	else if(logOp.isOp(SpearOp::OrOp))
	{
		// the cases where that doesn't change this
		if(level == isTrue || that.level == isFalse)
		{}
		// the cases where this must become True
		else if(that.level == isTrue)
		{ return setToTrue(); }
		// the cases where this must become a copy of that
		else if(level == isFalse)
		{
			ROSE_ASSERT(that.children.size()>0);
			for(list<exprLeafOrNode*>::iterator it = that.children.begin(); it != that.children.end(); it++)
				children.push_back(dynamic_cast<exprLeafOrNode*>((*it)->copy()));
			level = exprKnown;
		}
		else
			ROSE_ASSERT(0);
	}
	else
		ROSE_ASSERT(0);*/
		
	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
         ((level == isTrue || level == isFalse) && children.size() == 0));

	varsExprInitialized = false;
	return modified;
}

bool IntArithLogical::logicNode::orUpd(logicNode& that)
{
	bool modified=false;
	
	// we may only combine logicNodes of the same type
	ROSE_ASSERT(logOp == that.logOp);
	
	if(level == exprKnown && that.level == exprKnown)
	{
		if(logOp.isOp(SpearOp::AndOp))
		{
			// we currently don't use this
			ROSE_ASSERT(0);
		}
		else if(logOp.isOp(SpearOp::OrOp))
		{
			list<exprLeafOrNode*> newChildren = children;
			
			//printf("orUpd(): children.size()=%d that.children.size()=%d\n", children.size(), that.children.size());
			// (AvBvC) v (DvEvF) = (AvBvCvDvEvF)
			// insert copies of all of that's children into this->children
			for(list<exprLeafOrNode*>::iterator it = that.children.begin(); it != that.children.end(); it++)
			{
				// insert newTerm into newChildren, taking into account any redundancies relative
				// to previously inserted terms
				insertNewChildOr(newChildren, dynamic_cast<exprLeafOrNode*>((*it)->copy()));
				
				//children.push_back(dynamic_cast<exprLeafOrNode*>((*it)->copy()));
				modified = true;
			}
			
			// this object has been modified iff the new children list differs from the old children list
			modified = eqChildren(children, newChildren);
			
			children = newChildren;
			
			normalize();
		}
		else if(logOp.isOp(SpearOp::NotOp))
			ROSE_ASSERT(0);
		else
			ROSE_ASSERT(0);
	}
	// the cases where that doesn't change this
	else if(level == isTrue || that.level == isFalse)
	{}
	// the cases where this must become True
	else if(/*logOp.isOp(SpearOp::OrOp) && */that.level == isTrue)
	{ return setToTrue(); }
	// the cases where this must become a copy of that
	else if(level == isFalse)
	{
		ROSE_ASSERT(that.children.size()>0);
		for(list<exprLeafOrNode*>::iterator it = that.children.begin(); it != that.children.end(); it++)
			children.push_back(dynamic_cast<exprLeafOrNode*>((*it)->copy()));
		level = exprKnown;
	}
	else
		ROSE_ASSERT(0);
		
	ROSE_ASSERT((level == exprKnown && children.size()>0) || 
         ((level == isTrue || level == isFalse) && children.size() == 0));
	
	varsExprInitialized = false;
	
	return modified;
}

// Removes all facts that relate to the given variable, possibly replacing them 
// with other facts that do not involve the variable but could be inferred through
// the removed facts. (i.e. if we have x<y ^ y<z and wish to remove y, the removed 
// expressions may be replaced with x<z or just True)
// Returns true if this causes the logicNode object to change and false otherwise.
bool IntArithLogical::logicNode::removeVar(varID var)
{
	printf("IntArithLogical::logicNode::removeVar(%s)\n", var.str().c_str());
	if(level == exprKnown)
	{
		if(logOp.isOp(SpearOp::AndOp))
		{
			// list of iterators that refer to the conjuncts that involve var
			list<list<exprLeafOrNode*>::iterator> taintedExprs;
			
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
			{
				ROSE_ASSERT((*it)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
				exprLeaf* e = dynamic_cast<exprLeaf*>(*it);
				
				// if the current conjunct involves the given variable, record it
				if(e->x==var || e->y==var)
					taintedExprs.push_back(it);
			}
			
			list<exprLeaf*> inferredExprs;
				
			// Look at each pair of tainted conjuncts. Each of these conjuncts relates
			// var to some other variable. Thus, for each pair (x rel var), (y rel var)
			// we will check whether we can derive a replacement expression (x rel y).
			for(list<list<exprLeafOrNode*>::iterator>::iterator it1 = taintedExprs.begin();
			    it1 != taintedExprs.end(); it1++)
			{
				ROSE_ASSERT((**it1)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
				exprLeaf* one = dynamic_cast<exprLeaf*>(**it1);
				
				for(list<list<exprLeafOrNode*>::iterator>::iterator it2 = taintedExprs.begin();
				    it2 != taintedExprs.end(); it2++)
				{
					ROSE_ASSERT((**it2)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
					exprLeaf* two =dynamic_cast<exprLeaf*>(**it2);
					if(it1 != it2)
					{
						exprLeaf* inferred = NULL;
						// Examine all the cases where we can infer something useful
						
						// ----------------------------------------
						// Cases where we can infer False
						//              [one]              [two]
						// Case 0a: a*x <= b*var+c ^ a*x > b*var+c
						// Case 0b: a*var <= b*x+c ^ a*var > b*x+c
						if(one->cmp.isOp(SpearOp::SgnLTE) && two->cmp.isOp(SpearOp::SgnGT))
						{
							if(((one->x!=var && one->y==var) || (one->x==var && one->y!=var)) && 
							   two->x==one->x && two->y==one->y)
							{
								// This conjunction is now false and we're done
								// First, delete all the newly-inferred expressions
								for(list<exprLeaf*>::iterator it=inferredExprs.begin(); it!=inferredExprs.end(); it++)
								{ delete *it; }
								// And then convert this conjunction to false
								return setToFalse();
							}
						}
						
						// -----------------------------------------------
						//              [one]                   [two]
						// Case 1:  a*x <= b*var+c      ^ a'*var <= b'*y+c'
						// Case 2a: a*x == b*var+c      ^ a'*var == b'*y+c' 
						// Case 2b: a*x == b*var+c      ^ a'*y == b'*var+c' 
						// Case 2c: a*var == b*x+c      ^ a'*var == b'*y+c' [Equivalent to Case 2a for two ^ one combination]
						// Case 2d: a*var == b*x+c      ^ a'*y == b'*var+c' [Equivalent to Case 2b for two ^ one combination]
						// Case 2w: a*x <=/>/!= b*var+c ^ a'*var == b'*y+c' [ the flip case two ^ one ]
						// Case 2x: a*x <=/>/!= b*var+c ^ a'*y == b'*var+c' [ is handled when that    ]
						// Case 2y: a*var <=/>/!= b*x+c ^ a'*var == b'*y+c' [ combination comes up in ]
						// Case 2z: a*var <=/>/!= b*x+c ^ a'*y == b'*var+c' [ this inner loop         ]
						// Case 3:  a*x > b*var+c       ^ a'*var > b'*y+c'
						// Case 4a: a*x != b*var+c      ^ a'*var != b'*y+c  
						// Case 4b: a*x != b*var+c      ^ a'*y != b'*var+c  
						// Case 4c: a*var != b*x+c      ^ a'*var != b'*y+c  [Equivalent to Case 4a for two ^ one combination]
						// Case 4d: a*var != b*x+c      ^ a'*y != b'*var+c  [Equivalent to Case 4b for two ^ one combination]
						
						// Thus, we provide code for cases 1, 2a, 2b, 2w, 2x, 2y, 2z, 3, 4a, 4b
											
						// Cases 1, 2a, 2w, 3, 4a:
						// rel1 and rel2 are the two relationships in the cases above
						// a*x rel1 b*var+c          ^ a'*var rel1/== b'*y+c'
						// a*a'*x rel1 b*a'*var+c*a' ^ a'*b*var rel1/== b'*b*y+c'*b
						// a*a'*x rel1 b'*b*y+c'*b+c*a'
						else if((one->x != var && one->y == var && one->y == two->x && two->y != var) &&
						   (one->cmp == two->cmp || two->cmp.isOp(SpearOp::Equal)))
						{
							inferred = new exprLeaf(one->cmp, one->a*two->a, one->x, 
							                                  one->b*two->b, two->y, 
							                                  two->c*one->b + one->c*two->a);
						}
						// Cases 2b, 2x, 2y, 2z
						else if(two->cmp.isOp(SpearOp::Equal))
						{
							// Cases 2b, 2x
							// a*x rel1 b*var+c          ^ a'*y == b'*var+c'
							// a*b'*x rel1 b*b'*var+c*b' ^ a'*b*y-c'*b == b'*b*var
							// a*b'*x rel1 a'*b*y-c'*b+c*b'
							if((one->x != var && one->y == var && one->y == two->y && two->x != var))
							{
								inferred = new exprLeaf(one->cmp, one->a*two->b, one->x, 
							                                     one->b*two->a, two->y, 
							                                     one->c*two->b - two->c*one->b);
							}
							// Cases 2y, 2z
							else if(!one->cmp.isOp(SpearOp::Equal) &&
							        (one->x == var && one->y != var))
							{
								// Case 2y
								// a*var rel1 b*x+c          ^ a'*var == b'*y+c'
								// a*a'*var rel1 b*a'*x+c*a' ^ a'*a*var == b'*a*y+c'*a
								// b'*a*y+c'*a rel1 b*a'*x+c*a'
								// b'*a*y rel1 b*a'*x+c*a'-c'*a
								if(one->x == two->x && two->y != var)
								{
									inferred = new exprLeaf(one->cmp, one->a*two->b, two->y, 
									                                  one->b*two->a, one->y, 
									                                  one->c*two->a + two->c*one->a);
								}
								// Case 2z
								// a*var rel1 b*x+c          ^ a'*y == b'*var+c
								// a*b'*var rel1 b*b'*x+c*b' ^ a'*a*y-c*a == b'*a*var
								// a'*a*y-c*a rel1 b*b'*x+c*b'
								// a'*a*y rel1 b*b'*x+c*b'+c*a
								else if(one->x == two->y && two->x != var)
								{
									inferred = new exprLeaf(one->cmp, one->a*two->a, two->x, 
									                                  one->b*two->b, one->y, 
									                                  one->c*two->b + one->c*two->a);
								}
							}
						}
						
						// if we were able to infer something, add it to the list of inferred expressions
						if(inferred) inferredExprs.push_back(inferred);
					}
				}
			}
			
			// Now that we've found the expressions inferred via var, we must
			// 1. Delete all conjuncts that involve var (taintedExprs)
			for(list<list<exprLeafOrNode*>::iterator>::iterator it = taintedExprs.begin();
			    it != taintedExprs.end(); it++)
			{
				delete **it;
				children.erase(*it);
			}
			
			// 2. Insert all the expressions that were inferred via var (inferredExprs)
			for(list<exprLeaf*>::iterator it = inferredExprs.begin(); it != inferredExprs.end(); it++)
				children.push_back(*it);

			// if we've added anything, normalize this conjunction to remove any redundancies
			if(inferredExprs.size()>0)
				normalize();
			
			// This logicNode was modified iff it contained any conjuncts that used var
			// If it did, these conjuncts got removed and the other logic above definitely
			//    did not bring them back, so the logicNode was definitely modified.
			// If it didn't, no conjuncts got removed, inferredExprs is empty and normalize 
			//    was not called.
			return taintedExprs.size()>0;
		}
		else if(logOp.isOp(SpearOp::OrOp))
		{
			bool modified = false;
			// iterate over this disjunction's children, telling each child to remove 
			// any conjuncts that involve the variable
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
			{
				ROSE_ASSERT((*it)->elType()==IntArithLogical::exprLeafOrNode::lNode);
				logicNode* c = dynamic_cast<logicNode*>(*it);
				
				modified = c->removeVar(var) || modified;
			}
			return modified;
		}
	}
	else
		// There are no sub-expressions, so there's nothing to remove
		return false;
}

// Removes all instances of p*var with (q*var + r) and adjusts all relations that involve var
// accordingly.
// Returns true if this causes the logicNode object to change and false otherwise.
bool IntArithLogical::logicNode::replaceVar(varID var, int p, int q, int r)
{
	printf("IntArithLogical::logicNode::replaceVar(%s, %d, %d, %d)\n", var.str().c_str(), p, q, r);
	bool modified = false;
	if(level == exprKnown)
	{
		if(logOp.isOp(SpearOp::AndOp))
		{
			// list of iterators that refer to the conjuncts that involve var
			list<list<exprLeafOrNode*>::iterator> taintedExprs;
			
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
			{
				ROSE_ASSERT((*it)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
				exprLeaf* e = dynamic_cast<exprLeaf*>(*it);
				int a_old=e->a, b_old=e->b, c_old=e->c;
				
				// if the current conjunct involves the given variable, adjust it
				if(e->x==var)
				{
					// a*x rel b*y + c && p*x --> q*x + r
					// a*x rel b*y + c && x --> (q/p)*x + r/p
					// a*q*x rel b*q*y + c*q
					// a*(q*x+r) rel b*q*y + c*q + a*r
					// a*p*((q/p)*x+(r/p)) rel b*q*y + c*q + a*r
					e->a *= p; 
					e->b *= q; 
					e->c = e->c*q + e->a*r;
					e->normalize();
				}
				else if(e->y==var)
				{
					// a*x rel b*y + c && p*y --> q*y + r
					// a*x rel b*y + c && y --> (q/p)*y + (r/p)
					// a*q*x rel b*q*y + c*q
					// a*q*x + b*r rel b*(q*y+r)+ c*q
					// a*q*x rel b*p*((q/p)*y+(r/p))+ c*q - b*r
					e->a *= q;
					e->b *= p;
					e->c = e->c*q - e->b*r;
					e->normalize();
				}
				
				modified = modified || e->a!=a_old || e->b!=b_old || e->c!=c_old;
			}
		}
		else if(logOp.isOp(SpearOp::OrOp))
		{
			// iterate over this disjunction's children, telling each child to perform the given replacement 
			for(list<exprLeafOrNode*>::iterator it = children.begin(); it != children.end(); it++)
			{
				ROSE_ASSERT((*it)->elType()==IntArithLogical::exprLeafOrNode::lNode);
				logicNode* n = dynamic_cast<logicNode*>(*it);
				
				modified = n->replaceVar(var, p, q, r) || modified;
			}
		}
	}
	// Else, there are no sub-expressions, so there's nothing to replace
		
	return modified;
}

// generic function used by orUpd and andUpd to insert new term into a disjunction
// newChildren - The new list of children that will eventually replace children. We will be trying to
//               insert newTerm into newChildren.
void IntArithLogical::logicNode::insertNewChildOr(list<exprLeafOrNode*>& newChildren, exprLeafOrNode* newTerm)
{
	// Compare the new term to the previously-generated terms.
	// If it contains more information than any of them, it should not be added.
	// If any of them contain more information than it, they should be removed.
	// It is possible to both remove term and not add newTerm because it may be
	//    the case that A <= newConj <= B but not A<=B, where are already in newChildren.
	//    This may happen due to inaccuracies in the <= operator or our formula representation.
	bool addNewTerm = true;
	list<list<exprLeafOrNode*>::iterator> newToDelete;
	for(list<exprLeafOrNode*>::iterator it = newChildren.begin(); it != newChildren.end(); it++)
	{
		/*cout << "insertNewChildOr() *it = "<<(*it)->str()<<"\n";
		cout << "      newTerm="<<newTerm->str()<<", <= "<<(*newTerm <= *(*it))<<", >= "<<(*(*it) <= *newTerm)<<"\n";*/
		if(*newTerm <= *(*it)) addNewTerm = false;
		else if(*(*it) <= *newTerm) newToDelete.push_back(it);
	}
	// add the newly-created term as another child of this
	if(addNewTerm) newChildren.push_back(newTerm);
	// delete any newly-added terms that had more information than newConj and remove
	// them from newChildren
	for(list<list<exprLeafOrNode*>::iterator>::iterator it = newToDelete.begin(); it != newToDelete.end(); it++)
	{
		delete *(*it);
		newChildren.erase(*it);
	}
}

// compares two different children lists, returning true if they're equal and false otherwise
bool IntArithLogical::logicNode::eqChildren(list<exprLeafOrNode*>& one, list<exprLeafOrNode*>& two)
{
	// now compare children and newChildren to see of this formula has changed
	if(one.size() == two.size())
	{
		// elements of one that have already been matched to some element of two
		set<exprLeafOrNode*> matched;
		for(list<exprLeafOrNode*>::iterator itNew = two.begin(); itNew != two.end(); itNew++)
		{
			list<exprLeafOrNode*>::iterator it;
			for(it = one.begin(); it != one.end(); it++)
				if(*it == *itNew) break;
			if(it != one.end())
			{ matched.insert(*it); }
		}
		// if we didn't match all the members of one to all the members of two, this expression was modified
		if(matched.size() != one.size())
			return true;
	}
	else
		return false;
}

bool IntArithLogical::logicNode::operator==(exprLeafOrNode& that_arg)
{
	ROSE_ASSERT(that_arg.elType()==IntArithLogical::exprLeafOrNode::lNode);
	logicNode& that = dynamic_cast<logicNode&>(that_arg);
	
	if(logOp == that.logOp && level == that.level && children.size() == that.children.size())
	{
		list<exprLeafOrNode*>::iterator itThis, itThat;
		for(itThis = children.begin(), itThat = that.children.begin(); itThis != children.end(); itThis++, itThat)
		{
			if(!(*(*itThis) == *(*itThat)))
			{ return false; }
		}
	}
	else
		return false;
}

// Information order on affine relationships with the least information (True) being highest
// and most information (False) being lowest. Some expressions are not comparable since
// their sets of accepted points are not strictly related to each other.
bool IntArithLogical::logicNode::operator<=(exprLeafOrNode& that_arg)
{
	ROSE_ASSERT(that_arg.elType()==IntArithLogical::exprLeafOrNode::lNode);
	logicNode& that = dynamic_cast<logicNode&>(that_arg);
	
	ROSE_ASSERT(logOp == that.logOp);
	if(*this == that) return true;
	
	if(that.level == isTrue) return true;
	if(level == isTrue) return false;
	if(that.level == isFalse) return false;
	if(level == isFalse) return true;
		
	ROSE_ASSERT(level==exprKnown && that.level==exprKnown);

	if(logOp.isOp(SpearOp::AndOp))
	{
		// this <= that iff this contains the same amount or more information than that,
		// iff for every element of that there exist one or more elements of this s.t.
		// thisEL <= thatEL (thisEL contains same or more info than thatEL)
		// the simplest example of this is (A^B1^B2^C^D^...^Z) <= (A^B3^C), where B1<=B3 and B2<=B3
		for(list<exprLeafOrNode*>::iterator itThat = that.children.begin(); itThat != that.children.end(); itThat++)
		{
			ROSE_ASSERT((*itThat)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
			exprLeaf* thatEL = dynamic_cast<exprLeaf*>(*itThat);

			// compare thatEL to every element in this
			bool foundLE=false;
			for(list<exprLeafOrNode*>::iterator itThis = children.begin(); itThis != children.end(); itThis++)
			{
				ROSE_ASSERT((*itThis)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
				exprLeaf* thisEL = dynamic_cast<exprLeaf*>(*itThis);
				
				if((*thisEL <= *thatEL))
				{
					foundLE = true;
					break;
				}
			}
			// if we couldn't find an element of this <= thatEL, then we cannot prove: this <= that
			if(!foundLE) return false;
		}
		// since we got here without finding counter-examples to the assertion this <= that, it must be true
		return true;
	}
	else if(logOp.isOp(SpearOp::OrOp))
	{
		// this <= that iff this contains the same amount or more information than that,
		// iff for every element of this there exist one or more elements of that s.t.
		// thisEL <= thatEL (thisEL contains same or more info than thatEL)
		// the simplest example of this is (A1vA2vA3vB1vB2) <= (AvBvCv...vZ), where Ai<=A and Bi<=B
		for(list<exprLeafOrNode*>::iterator itThis = children.begin(); itThis != children.end(); itThis++)
		{
			ROSE_ASSERT((*itThis)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
			exprLeaf* thisEL = dynamic_cast<exprLeaf*>(*itThis);
				
			// compare thatEL to every element in this
			bool foundLE=false;
			for(list<exprLeafOrNode*>::iterator itThat = that.children.begin(); itThat != that.children.end(); itThat++)
			{
				ROSE_ASSERT((*itThat)->elType()==IntArithLogical::exprLeafOrNode::eLeaf);
				exprLeaf* thatEL = dynamic_cast<exprLeaf*>(*itThat);
				
				if((*thisEL <= *thatEL))
				{
					foundLE = true;
					break;
				}
			}
			// if we couldn't find an element of that >= thisEL, then we cannot prove: this <= that
			if(!foundLE) return false;
		}
		// since we got here without finding counter-examples to the assertion this <= that, it must be true
		return true;
	}
}

/*******************
 * IntArithLogical *
 *******************/
 // Creates an uninitialized logical expression
IntArithLogical::IntArithLogical()
{
	level = uninitialized;
	expr = NULL;
}

// Creates a logical expression that is either True or False, depending on the value argument
IntArithLogical::IntArithLogical(bool value)
{
	level = known;
	expr = new logicNode(orOp);
	if(value)
		expr->setToTrue();
	else
		expr->setToFalse();
}

// create an IntArithLogical that corresponds to a single affine relationship
IntArithLogical::IntArithLogical(cmpOps cmp, int a, varID x, int b, varID y, int c)
{
	level = known;
	expr = new logicNode(orOp, new logicNode(andOp, new exprLeaf(cmp, a, x, b, y, c)));
	/*exprLeaf* el = new exprLeaf(cmp, a, x, b, y, c);
	logicNode* lnAnd = new logicNode(andOp, el);
	expr = new logicNode(orOp, lnAnd);
	printf("IntArithLogical::IntArithLogical el=%p, lnAnd=%p, expr=%p\n", el, lnAnd, expr);*/
}

IntArithLogical::IntArithLogical(const IntArithLogical& that)
{
	level = that.level;
	if(level==known)
		expr = new logicNode(*that.expr);
	else
		expr = NULL;
}

// initializes this Lattice to its default bottom state if it has not yet been initialized
// if the given new level is higher than bottom, expr is also initialized to a new object
//void IntArithLogical::initialize(IntArithLogical::infContent newLevel)
void IntArithLogical::initialize()
{ 
	if(level == uninitialized)
	{
		level = bottom;
		expr = NULL;
	}
	/*level = newLevel;
	if(level == known)
		expr = new logicNode(orOp);
	else
		expr = NULL;*/
}

// initializes this Lattice to its default state, with a specific value (true or false)
void IntArithLogical::initialize(bool value)
{
	level = known;
	expr = new logicNode(orOp);
	if(value)
		expr->setToTrue();
	else
		expr->setToFalse();
}

Lattice* IntArithLogical::copy() const
{
	return new IntArithLogical(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void IntArithLogical::copy(Lattice* that_arg)
{
	IntArithLogical* that = dynamic_cast<IntArithLogical*>(that_arg);
	level = that->level;
	if(level==known)
		expr = dynamic_cast<IntArithLogical::logicNode*>(that->expr->copy());
	else
		expr = NULL;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool IntArithLogical::meetUpdate(Lattice* that_arg)
{
	IntArithLogical* that = dynamic_cast<IntArithLogical*>(that_arg);
	
	printf("IntArithLogical::meetUpdate()\n");
	printf("   this=%s\n", str("").c_str());
	printf("   that=%s\n", that->str("").c_str());
	
	if(level==uninitialized || level==bottom)
	{
		// If that doesn't have any data, upgrade this to bottom
		if(that->level==uninitialized)
		{
			initialize();
			return level == uninitialized;
		}
		// Else, if that has real data, copy it over to this
		else
		{
			bool modified = that->level > level;
			copy(that_arg);
			return modified;
		}
		
	}
	else if(that->level == known)
	{
		/*printf("IntArithLogical::meetUpdate\n");
		printf("    this=%s\n", expr->str("").c_str());
		printf("    that=%s\n", that->expr->str("").c_str());*/
		return expr->orUpd(*(that->expr));
	}

	return false;
}

bool IntArithLogical::operator==(Lattice* that_arg)
{
	const IntArithLogical* that = dynamic_cast<IntArithLogical*>(that_arg);
	return this->level == that->level &&
	       this->expr == that->expr;
}
	
// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string IntArithLogical::str(string indent)
{
	if(level == uninitialized)
		return indent + "IntArithLogical:uninitialized";
	else if(level == bottom)
		return indent + "IntArithLogical:bottom";
	else
		return "IntArithLogical: " + expr->str(indent);
}

// Returns true if this causes the IntArithLogical object to change and false otherwise.
bool IntArithLogical::notUpd()
{
	if(level != known)
		return false;
	else
		return expr->notUpd();
}

// Returns true if this causes the IntArithLogical object to change and false otherwise.
bool IntArithLogical::andUpd(LogicalCond& that_arg)
{
	IntArithLogical& that = dynamic_cast<IntArithLogical&>(that_arg);
	if(level==uninitialized || level==bottom)
	{
		// If that doesn't have any data, upgrade this to bottom
		if(that.level==uninitialized)
		{
			initialize();
			return level == uninitialized;
		}
		// Else, if that has real data, copy it over to this
		else
		{
			bool modified = that.level > level;
			copy(&that);
			return modified;
		}
		
	}
	else if(that.level == known)
	{
		/*printf("IntArithLogical::andUpd\n");
		printf("    this=%s\n", expr->str("").c_str());
		printf("    that=%s\n", that.expr->str("").c_str());*/
		return expr->andUpd(*(that.expr));
	}

	return false;
}

// Returns true if this causes the IntArithLogical object to change and false otherwise.
bool IntArithLogical::orUpd(LogicalCond& that_arg)
{
	IntArithLogical& that = dynamic_cast<IntArithLogical&>(that_arg);
	if(level==uninitialized || level==bottom)
	{
		// If that doesn't have any data, upgrade this to bottom
		if(that.level==uninitialized)
		{
			initialize();
			return level == uninitialized;
		}
		// Else, if that has real data, copy it over to this
		else
		{
			bool modified = that.level > level;
			copy(&that);
			return modified;
		}
		
	}
	else if(that.level == known)
	{
		/*printf("IntArithLogical::orUpd\n");
		printf("    this=%s\n", expr->str("").c_str());
		printf("    that=%s\n", that.expr->str("").c_str());*/
		return expr->orUpd(*(that.expr));
	}
	
	return false;
}

// Sets this expression to True, returning true if this causes
// the expression to be modified and false otherwise.
////// If onlyIfNotInit=true, this is only done if the expression is currently uninitialized
bool IntArithLogical::setToTrue(/*bool onlyIfNotInit*/)
{
	/*if(!onlyIfNotInit || level==uninitialized)
	{
		if(level==uninitialized)
			initialize(true);
		else*/
			expr->setToTrue();
	//}
}

// Sets this expression to False, returning true if this causes
// the expression to be modified and false otherwise.
////// If onlyIfNotInit=true, this is only done if the expression is currently uninitialized
bool IntArithLogical::setToFalse(/*bool onlyIfNotInit*/)
{
	/*if(!onlyIfNotInit || level==uninitialized)
	{
		if(level==uninitialized)
			initialize(false);
		else*/
			expr->setToFalse();
	//}
}

// Removes all facts that relate to the given variable, possibly replacing them 
// with other facts that do not involve the variable but could be inferred through
// the removed facts. (i.e. if we have x<y ^ y<z and wish to remove y, the removed 
// expressions may be replaced with x<z or just True)
// Returns true if this causes the IntArithLogical object to change and false otherwise.
bool IntArithLogical::removeVar(varID var)
{
	printf("IntArithLogical::removeVar(%s)\n", var.str().c_str());
	if(level!=known)
		return false;
	else
		return expr->removeVar(var);
}

// returns a copy of this LogicalCond object
LogicalCond* IntArithLogical::copy()
{
	return new IntArithLogical(*this);
}

bool SAV_LT(SpearAbstractVar* v1, SpearAbstractVar* v2)
{
	return *v1 < *v2;
}

bool SAV_EQ(SpearAbstractVar* v1, SpearAbstractVar* v2)
{
	return *v1 == *v2;
}

// Writes the full expression that corresponds to this object, including any required
// declarations and range constraints to os. Returns that variable that summarizes this expression.
// otherVars - list of variables that also need to be declared and ranged
// createProposition - if true, outputSpearExpr() creates a self-contained proposition. If false, no 
//       proposition is created; it is presumed that the caller will be using the expression as part 
//       of a larger proposition.
SpearVar IntArithLogical::outputSpearExpr(exprLeaf* otherExpr, ofstream &os, bool createProposition)
{
	const list<SpearAbstractVar*>* otherVars = NULL;
	if(otherExpr) otherVars = &(otherExpr->getVars());
	
	// The summary variable for the expression itself
	SpearVar summaryVar("summary", 1);
	
	list<SpearAbstractVar*> vars = expr->getVars();
	if(otherExpr)
	{	
		// add to vars any variables in otherVars
		for(list<SpearAbstractVar*>::const_iterator it = otherVars->begin(); it != otherVars->end(); it++)
			vars.push_back(*it);
	}
	// remove any duplicates
	vars.sort(SAV_LT);
	vars.unique(SAV_EQ);
	
	// Generate the variables that define the ranges of all the non-binary variables
	list<SpearAbstractVar*> rangeVars;
	for(list<SpearAbstractVar*>::const_iterator it = vars.begin(); it != vars.end(); it++)
	{
		// Non-binary variables
		if((*it)->getType().numBits()>1)
			rangeVars.push_back(new SpearVar((*it)->getName() + "_Range", 1));
	}
	
	// Generate the reduction tree for the range variables
	list<SpearAbstractVar*> reductionVars;
	//printf("reductionVars.size()=%d\n", rangeVars.size());
	ostringstream redSS;
	SpearAbstractVar* rangesVar = genReductionTree(SpearOp::AndOp, redSS, "Ranges", vars, rangeVars, -1);
	
	// Declare all the variables
	os << "d ";
	for(list<SpearAbstractVar*>::const_iterator it = vars.begin(); it != vars.end(); it++)
	{
		// The variables used in the expressions
		os << (*it)->varDecl() << " ";
	
		// The variables that define the ranges of all the non-binary variables
		if((*it)->getType().numBits()>1)
		{
			SpearVar minBound((*it)->getName() + "_MinBound", 32);
			SpearVar maxRVar((*it)->getName() + "_Max", 1);
			SpearVar minRVar((*it)->getName() + "_Min", 1);
			SpearVar rangeVar((*it)->getName() + "_Range", 1);
			os << minBound.varDecl() << " " << maxRVar.varDecl() << " " 
			   << minRVar.varDecl() << " " << rangeVar.varDecl() << " ";
		}
	}
	// declare the summary variable, if it is needed
	if(createProposition) os << "\n";
	else os << summaryVar.varDecl() << "\n";
	
	// Generate the expression itself
	os << expr->getExpr();

	// Generate the bounds on all non-binary variables
	for(list<SpearAbstractVar*>::const_iterator it = vars.begin(); it != vars.end(); it++)
	{
		if((*it)->getType().numBits()>1)
		{
			long long maxRange = 1;
			long long minRange = -1;
			for(int i=0; i<(*it)->getType().numBits()-2; i++)
			{ minRange*=2; }
			maxRange = 0-(minRange+1);
			//SpearConst minRConst(minRange, (*it)->getType().numBits());
			SpearConst minRConst(maxRange, (*it)->getType().numBits());
			SpearConst maxRConst(maxRange, (*it)->getType().numBits());
			SpearVar minBound((*it)->getName() + "_MinBound", 32);
			SpearVar maxRVar((*it)->getName() + "_Max", 1);
			SpearVar minRVar((*it)->getName() + "_Min", 1);
			SpearVar rangeVar((*it)->getName() + "_Range", 1);
			//os << "d " << maxRVar.varDecl() << " " << minRVar.varDecl() << " " << rangeVar->varDecl() << " " << "\n";
			os << "c " << minBound.getName() << " " << SpearOp::opStr(SpearOp::Subtract) << " 0:i32 " << maxRange << ":" << minRConst.getType().typeName() << "\n";
			os << "c " << maxRVar.getName() << " " << SpearOp::opStr(SpearOp::SgnLTE) << " " << (*it)->getName() << " " << maxRange << ":" << maxRConst.getType().typeName() << "\n";
			os << "c " << minRVar.getName() << " " << SpearOp::opStr(SpearOp::SgnGTE) << " " << (*it)->getName() << " " << minBound.getName() << "\n";
			os << "c " << rangeVar.getName() << " " << SpearOp::opStr(SpearOp::AndOp) << " " << (*it)->getName() << "_Min" << " " << (*it)->getName() << "_Max" << "\n";
		}
	}
	// output the reduction tree for the range variables
	os << redSS.str();
	
	// delete all the newly-generated variables
	for(list<SpearAbstractVar*>::iterator it = rangeVars.begin(); it != rangeVars.end(); it++)
		delete *it;
	for(list<SpearAbstractVar*>::iterator it = reductionVars.begin(); it != reductionVars.end(); it++)
		delete *it;
	
	// Generate the formulas
	if(otherExpr) os << otherExpr->getExpr();

	SpearAbstractVar* outVar = expr->getOutVar();
	//printf("outVar = %p\n", outVar);
	if(createProposition)
	{ os << "p " << SpearOp::opStr(SpearOp::AndOp) << " " << outVar->getName() << " " << rangesVar->getName() << "\n"; }
	else
	{ os << "c " << summaryVar.getName() << " " << SpearOp::opStr(SpearOp::AndOp) << " " << outVar->getName() << " " << rangesVar->getName() << "\n"; }

	return summaryVar;
}

// Runs Spear on the given input file. Returns true if the file's conditions are satisfiable and false otherwise.
bool IntArithLogical::runSpear(string inputFile)
{
	ostringstream cmd;
	cmd << getenv("ANALYSIS_ROOT") << "/src/lattice/runSpear.pl "<<inputFile;
//cout <<"cmd = "<<cmd.str()<<"\n";
	int satisfiable = system(cmd.str().c_str());
	//cout << cmd.str();
	//printf("satisfiable=%d\n", satisfiable);
	return satisfiable;
}

// Queries whether the given affine relation is implied by this arithmetic/logical constrains.
// Returns true if yes and false otherwise
bool IntArithLogical::isImplied(cmpOps cmp, int a, varID x, int b, varID y, int c)
{
	if(level == uninitialized)
		return false;
	else if(expr->getLevel() == exprLeafOrNode::isFalse)
		return true;
	else if(expr->getLevel() == exprLeafOrNode::isTrue)
		return false;
	
	exprLeaf tgtExpr(cmp, a, x, b, y, c);
	SpearAbstractVar* tgtOutVar = tgtExpr.getOutVar();
	
	ofstream spearInput;
	spearInput.open (".spearInput");
  
	spearInput << "v 1.0\n";
	// The summary variable that represents the implication itself
	SpearVar finalVar("final", 1);
	// Declare the final variable
	spearInput << "d " << finalVar.varDecl() << "\n";
	
	// Output the expression, including any required declarations.
	// precondVar summarizes this expression and is the precondition for the implication created by this function
	SpearVar precondVar = outputSpearExpr(&tgtExpr, spearInput, false);
		
	spearInput << "c " << finalVar.getName() << " " << SpearOp::opStr(SpearOp::Implies) << " " <<precondVar.getName() << " " << tgtOutVar->getName() << "\n";
	spearInput << "p "<< SpearOp::opStr(SpearOp::NotOp) << " final\n";
	
	spearInput.close();
	
	bool satisfiable = runSpear(".spearInput");
	return !satisfiable;
}

// returns true if this logical condition must be true and false otherwise
bool IntArithLogical::mayTrue()
{
	if(level == uninitialized)
		return false;
	else if(expr->getLevel() == exprLeafOrNode::isFalse)
		return false;
	else if(expr->getLevel() == exprLeafOrNode::isTrue)
		return true;
	
	list<SpearAbstractVar*> emptyVars;
	
	ofstream spearInput;
	spearInput.open (".spearInput");
  
	spearInput << "v 1.0\n";

	// Output the expression, including any required declarations.
	// precondVar summarizes this expression and is the precondition for the implication created by this function
	SpearVar summaryVar = outputSpearExpr(NULL, spearInput, true);
		
	//spearInput << "p "<< summaryVar.getName() << "\n";
	
	spearInput.close();
	
	bool mayTrue = runSpear(".spearInput");
	// if this expression may not be true, set it to false
	if(!mayTrue)
		setToFalse();
	return mayTrue;
}

// Queries whether the arithmetic/logical constrains may be consistent with the given affine relation.
// Returns true if yes and false otherwise
bool IntArithLogical::mayConsistent(cmpOps cmp, int a, varID x, int b, varID y, int c)
{
	if(level == uninitialized)
		return true;
	else if(expr->getLevel() == exprLeafOrNode::isFalse)
		return false;
	else if(expr->getLevel() == exprLeafOrNode::isTrue)
		return true;
		
	exprLeaf tgtExpr(cmp, a, x, b, y, c);
	SpearAbstractVar* tgtOutVar = tgtExpr.getOutVar();
	
	ofstream spearInput;
	spearInput.open (".spearInput");
  
	spearInput << "v 1.0\n";
	// The summary variable that represents the implication itself
	SpearVar finalVar("final", 1);
	// Declare the final variable
	spearInput << "d " << finalVar.varDecl() << "\n";

	// Output the expression, including any required declarations.
	// precondVar summarizes this expression and is the precondition for the implication created by this function
	SpearVar precondVar = outputSpearExpr(NULL, spearInput, false);
		
	spearInput << "c " << finalVar.getName() << " " << SpearOp::opStr(SpearOp::AndOp) << " " <<precondVar.getName() << " " << tgtOutVar->getName() << "\n";
	spearInput << "p "<< SpearOp::opStr(SpearOp::NotOp) << " final\n";
	
	spearInput.close();
//printf("Done writing to .spearInput.\n")
	
	bool satisfiable = runSpear(".spearInput");
	return !satisfiable;
}

// Updates the expression with the information that x*a has been assigned to y*b+c
// returns true if this causes the expression to change and false otherwise
bool IntArithLogical::assign(int a, varID x, int b, varID y, int c)
{
	bool modified = false;
	/*if(level == uninitialized)
		return false;*/
	if(level != known)
		initialize(true);
	
	if(x != y)
	{
		printf("assign x!=y: before: %s\n", str("").c_str());
			
		// First, remove any mappings to x in anticipation of it's old value dissapearing
		removeVar(x);
		
		printf("assign x!=y: after: %s\n", str("").c_str());
		
		// Create a new constraint that corresponds to a*x = b*y + c
		logicNode* newEq = new logicNode(orOp, new logicNode(andOp, new exprLeaf(IntArithLogical::eq, a, x, b, y, c)));
		
		// Update the current expression with the new relationship between x and y
		modified = expr->andUpd(*newEq);
		
		delete newEq;
	}
	// x == y
	else
	{
		printf("assign x=y: before: %s\n", str("").c_str());
		modified = expr->replaceVar(x, a, b, c);
		printf("assign x=y: after: %s\n", str("").c_str());
	}
	
	return modified;
}

/****************************
 *** IntArithLogicalFact ***
 ****************************/

NodeFact* IntArithLogicalFact::copy() const
{
	return new IntArithLogicalFact(*this);
}


string IntArithLogicalFact::str(string indent)
{
	stringstream outs;
	
	outs << indent << "IntArithLogicalFact:\n";
	outs << indent << "    "<<expr.str(indent+"    ");
	
	return outs.str();
}

/********************************
 *** IntArithLogicalPlacer ***
 ********************************/

// points trueFact and falseFact to freshly allocated objects that represent the true and false
// branches of the control flow guarded by the given expression. They are set to NULL if our representation
// cannot represent one of the expressions.
// doFalseBranch - if =true, falseFact is set to the correct false-branch condition and to NULL otherwise
void IntArithLogicalPlacer::setTrueFalseBranches(SgExpression* expr, 
                             IntArithLogicalFact **trueFact, IntArithLogicalFact **falseFact, 
                             bool doFalseBranch)
{
	varID x, y;
	bool negX, negY;
	long c;
	*trueFact=NULL;
	*falseFact=NULL;
	
	IntArithLogical::cmpOps relOp;
	
	// if this is a valid inequality condition of the form x <= y + c
	if(cfgUtils::computeTermsOfIfCondition_LTEQ(expr, x, negX, y, negY, c))
		relOp = IntArithLogical::le;
	// if this is a valid equality condition of the form x == y + c
	else if(cfgUtils::computeTermsOfIfCondition_EQ(expr, x, negX, y, negY, c))
		relOp = IntArithLogical::eq;
	else
		ROSE_ASSERT(0);
	
	IntArithLogical ialExpr(relOp, negX?-1:1, x, negY?-1:1, y, c);
	*trueFact = new IntArithLogicalFact(ialExpr);
	//cout << "    trueFact="<<trueFact.str()<<"\n";
	
	if(doFalseBranch)
	{
		ialExpr.notUpd();
		*falseFact = new IntArithLogicalFact(ialExpr);
		//cout << "    falseFact="<<falseFact.str()<<"\n";
	}
}

void IntArithLogicalPlacer::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	if(analysisDebugLevel>0)
		printf("IntArithLogicalPlacer::visit() function %s() node=<%s | %s>\n", 
		       func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	
	IntArithLogicalFact *trueFact=NULL, *falseFact=NULL;
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
	setTrueFalseBranches(testExpr, &trueFact, &falseFact, doFalseBranch);
	
	// iterate over both the descendants
	vector<DataflowEdge> edges = n.outEdges();
	//printf("edges.size()=%d, trueFact=%p, falseFact=%p\n", edges.size(), trueFact, falseFact);
	if(analysisDebugLevel>0)
	{
		cout << "trueFact="<<trueFact->str()<<"\n";
		if(doFalseBranch)
			cout << "falseFact="<<falseFact->str()<<"\n";
	}
	for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
	{
		if((*ei).condition() == eckTrue && trueFact)
		{
			NodeState* trueNodeState = NodeState::getNodeState((*ei).target());
//printf("    Adding true side fact %s, this=%p\n", trueFact->str().c_str(), this);
			trueNodeState->addFact(this, 0, trueFact);
		}
		else if((*ei).condition() == eckFalse && falseFact)
		{
			NodeState* falseNodeState = NodeState::getNodeState((*ei).target());
//printf("    Adding false side fact %s, this=%p\n", falseFact->str().c_str(), this);
			falseNodeState->addFact(this, 0, falseFact);
		}
	}
}


// prints the inequality facts set by the given IntArithLogicalPlacer
void printIntArithLogicals(IntArithLogicalPlacer* aip, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	factNames.push_back(0);
	printAnalysisStates pas(aip, factNames, latticeNames, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}

// Runs the Affine Inequality Placer analysis
static IntArithLogicalPlacer* aip = NULL;
void runIntArithLogicalPlacer(bool printStates)
{
	if(aip == NULL)
	{
		aip = new IntArithLogicalPlacer();
		UnstructuredPassInterAnalysis upia_aip(*aip);
		upia_aip.runAnalysis();
		
		if(printStates)
			printIntArithLogicals(aip, ":");
	}
}

static IntArithLogical defaultIAL(true);

// returns the set of IntArithLogical expressions known to be true at the given DataflowNode
const IntArithLogical& getIntArithLogical(const DataflowNode& n)
{
	NodeState* ns = NodeState::getNodeState(n);
	IntArithLogicalFact* fact = dynamic_cast<IntArithLogicalFact*>(ns->getFact(aip, 0));
	//printf("getAffineIneq: fact=%p, aip=%p, ns=%p\n", fact, aip, ns);
	if(fact)
		return fact->expr;
	else
		return defaultIAL;
}
