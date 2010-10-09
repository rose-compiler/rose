#include "mesgExpr.h"

class mesgExpr;
class OneDmesgExpr;
#include "contProcMatchAnalysis.h"

/****************
 * OneDmesgExpr *
 ****************/

/*OneDmesgExpr()::~OneDmesgExpr()
{}*/

// Creates a message expression from the given SgExpression
OneDmesgExpr::OneDmesgExpr(SgExpression* expr, ConstrGraph* cg)
{
	// If the source/destination process expression is parseable as i op j op c
	varID i, j;
	bool negI, negJ;
	short op;
	long c;

	// i+c or i*c
	if(cfgUtils::parseExpr(expr, op, i, negI, j, negJ, c) && 
		(op==cfgUtils::none || (op==cfgUtils::add && j==zeroVar) || (op==cfgUtils::mult && j==oneVar)))
	{
		if(op==cfgUtils::none)
		{
			// [i, i]
			contRangeProcSet procs(i, 1, 1, 0, i, 1, 1, 0, cg);
			mRange = procs;
		}
		else if(op==cfgUtils::add)
		{
			// [i+c, i+c]
			contRangeProcSet procs(i, 1, 1, c, i, 1, 1, c, cg);
			mRange = procs;
		}
		else if(op==cfgUtils::mult)
		{
			// [i*c, i*c]
			contRangeProcSet procs(i, 1, c, 0, i, 1, c, 0, cg);
			mRange = procs;
		}
	}
	else
	{
		fprintf(stderr, "[mesgExpr] OneDmesgExpr::OneDmesgExpr() ERROR: expression  <%s | %s> is not a simple parseable expression for creating a OneDmesgExpr!\n", expr->class_name().c_str(), expr->unparseToString().c_str());
		ROSE_ASSERT(0);
	}
	/*else
	{ fprintf(stderr, "[mesgExpr] OneDmesgExpr::OneDmesgExpr() ERROR: destination process argument \"%s\" of function call \"%s\" is not a simple parseable expression!\n", expr->unparseToString().c_str(), n.getNode()->unparseToString().c_str()); exit(-1); }*/
}

// Creates a message expression from the given SgExpression, annotating the message expression
// bounds with the given process set.
OneDmesgExpr::OneDmesgExpr(SgExpression* expr, ConstrGraph* cg, int pSet)
{
	// If the source/destination process expression is parseable as i op j op c
	varID i, j;
	bool negI, negJ;
	short op;
	long c;

	// i+c or i*c
	if(cfgUtils::parseExpr(expr, op, i, negI, j, negJ, c) && 
		(op==cfgUtils::none || (op==cfgUtils::add && j==zeroVar) || (op==cfgUtils::mult && j==oneVar)))
	{
		if(i == zeroVar) i.addAnnotation("pCFG_common", (void*)1);
		else             i.addAnnotation(pCFG_contProcMatchAnalysis::getVarAnn(pSet), (void*)1);
		if(j == zeroVar) j.addAnnotation("pCFG_common", (void*)1);
		else             j.addAnnotation(pCFG_contProcMatchAnalysis::getVarAnn(pSet), (void*)1);
		
		if(op==cfgUtils::none)
		{
			// [i, i]
			contRangeProcSet procs(i, 1, 1, 0, i, 1, 1, 0, cg);
			mRange = procs;
		}
		else if(op==cfgUtils::add)
		{
			// [i+c, i+c]
			contRangeProcSet procs(i, 1, 1, c, i, 1, 1, c, cg);
			mRange = procs;
		}
		else if(op==cfgUtils::mult)
		{
			// [i*c, i*c]
			contRangeProcSet procs(i, 1, c, 0, i, 1, c, 0, cg);
			mRange = procs;
		}
	}
	else
	{
		fprintf(stderr, "[mesgExpr] OneDmesgExpr::OneDmesgExpr() ERROR: expression  <%s | %s> is not a simple parseable expression for creating a OneDmesgExpr!\n", expr->class_name().c_str(), expr->unparseToString().c_str());
		ROSE_ASSERT(0);
	}
	/*else
	{ fprintf(stderr, "[mesgExpr] OneDmesgExpr::OneDmesgExpr() ERROR: destination process argument \"%s\" of function call \"%s\" is not a simple parseable expression!\n", expr->unparseToString().c_str(), n.getNode()->unparseToString().c_str()); exit(-1); }*/
}

// Returns the a reference to a heap-allocated subset of domain on which the message expression 
// recv o send is the identity function or an invalid subdomain is no such subdomain exists
procSet& OneDmesgExpr::getIdentityDomain(const mesgExpr& recv_arg, const mesgExpr& send, 
                                        const procSet& domain_arg) const
{
	//const contRangeProcSet& domain = (const contRangeProcSet&)domain_arg;
	const contRangeProcSet& domain = dynamic_cast<const contRangeProcSet&>(domain_arg);
	const OneDmesgExpr& recv   = dynamic_cast<const OneDmesgExpr&>(recv_arg);
	
//cout << "^^^^^^^^^^^^^^^^^^^^^\n";
//cout << "getIdentityDomain: domain.size()="<<domain.size()<<", domain="<<domain.str()<<"\n";
	// if its a broadcast, with one process sending to a specific set of receiver processes
	if(domain.size() == 1)
	{
//cout << "getIdentityDomain: send="<<send.str()<<"\n";
		contRangeProcSet& sendImage = dynamic_cast<contRangeProcSet&>(send.getImage(domain));
//cout << "getIdentityDomain: sendImage="<<sendImage.str()<<"\n";
		// the intersection of senders matched by receivers and the actual sender domain
		// is the subset of the senders on which recv o send is the identity
		contRangeProcSet& recvImage = recv.getImage(sendImage);
//cout << "getIdentityDomain: recvImage="<<recvImage.str()<<", recvImage.getConstr()="<<recvImage.getConstr()<<"\n";
		sendImage.cgDisconnect();
		delete &sendImage;
		recvImage.intersectUpd(domain);
/*cout << "getIdentityDomain: recvImage I domain="<<recvImage.str()<<", recvImage.getConstr()="<<recvImage.getConstr()<<"\n";
cout << "cg = "<<recvImage.getConstr()->str()<<"\n";*/
		return recvImage;
	}
	// If multiple processes are sending to the same set of receivers, each receiver will see multiple
	// incoming messages. As such, the only way we can get an identity mapping is if we restrict 
	// the domain to just one sender.
	else
	{
		contRangeProcSet& sendImage = dynamic_cast<contRangeProcSet&>(send.getImage(domain));
		contRangeProcSet& recvImage = recv.getImage(sendImage);
		sendImage.cgDisconnect();
		delete &sendImage;
		recvImage.intersectUpd(domain);
		if(!recvImage.validSet()) return recvImage;
		
		// if the receivers mapping overlaps the senders domain on 
		// exactly one spot
		if(recvImage.size()==1)
		{
			return recvImage;
		}
		else
		{
			recvImage.invalidate();
			return recvImage;
		}
	}
}

// Maps the given domain to its corresponding range, returning a reference to a 
// heap-allocated range process set 
procSet& OneDmesgExpr::getImage(const procSet& domain_arg) const
{
	//const contRangeProcSet& domain = (const contRangeProcSet&)domain_arg;
	const contRangeProcSet& domain = dynamic_cast<const contRangeProcSet&>(domain_arg);
	contRangeProcSet& range = getImage(domain);
	//delete &domain;
	return range;
}

// Maps the given domain to its corresponding range, returning a reference to a 
// heap-allocated range process set 
contRangeProcSet& OneDmesgExpr::getImage(const contRangeProcSet& domain_arg) const
{
	const contRangeProcSet& domain = dynamic_cast<const contRangeProcSet&>(domain_arg);
	contRangeProcSet* procs = new contRangeProcSet(mRange, true);
	//cout << "OneDmesgExpr::getImage: procs="<<procs->str()<<"\n";
	//cout << "OneDmesgExpr::getImage: mRange="<<mRange.str()<<"\n";
	return *procs;
}

// Removes the portion of the message expression that maps the domain to the range,
// as defined by cg (nodeConstant).
// Returns true if successful, false if not.
bool OneDmesgExpr::removeMap(const procSet& domain_arg, const procSet& remR_arg)
{
	const contRangeProcSet& domain = dynamic_cast<const contRangeProcSet&>(domain_arg);
	const contRangeProcSet& remR = dynamic_cast<const contRangeProcSet&>(remR_arg);
	ROSE_ASSERT(domain.getConstr() == remR.getConstr());
	// OneDmesgExpr can only map singleton domains to useful ranges 
	// since their range does not depend on the input domain
	ROSE_ASSERT(domain.size()==1);
	bool success = false;
	bool containsLB = mRange.getConstr()->containsVar(remR.getLB());
	bool containsUB = mRange.getConstr()->containsVar(remR.getUB());
	
	// Update/copy mRange.cg's knowledge of remR from remR.cg
	mRange.getConstr()->copyVar(remR.getConstr(), remR.getLB());
	mRange.getConstr()->copyVar(remR.getConstr(), remR.getUB());
	
	// Trim remR in mRange.cg
	
	// If [(mRange.lb==remR.lb) <= (remR.ub==mRange.ub)]
	if(mRange.getConstr()->eqVars(mRange.getLB(), remR.getLB()) && 
	   mRange.getConstr()->eqVars(mRange.getUB(), remR.getUB()))
	{
		mRange.emptify();
		success = true;
	}
	// If [mRange.lb<remR.lb <= (remR.ub==mRange.ub)]
	else if(mRange.getConstr()->ltVars(mRange.getLB(), remR.getLB()) && 
	   mRange.getConstr()->eqVars(mRange.getUB(), remR.getUB()))
	{
		// then [mRange.lb, range.lb): mRange.ub < range.lb
		mRange.getConstr()->eraseVarConstr(mRange.getUB());
		mRange.getConstr()->assertCond(mRange.getUB(), remR.getLB(), 1, 1, -1);
		// if mRange.ub appears in remR's constraints, update its relationship with remR.lb
		if(remR.getConstr()->containsVar(mRange.getUB()))
		{
			remR.getConstr()->eraseVarConstr(mRange.getUB());
			remR.getConstr()->assertCond(mRange.getUB(), remR.getLB(), 1, 1, -1);
		}
		//modified = mRange.ub != remR.ub;
		success = true;
	}
	// If [(mRange.lb=remR.lb) <= remR.ub<mRange.ub]
	else if(mRange.getConstr()->eqVars(mRange.getLB(), remR.getLB()) && 
	        mRange.getConstr()->ltVars(remR.getUB(), mRange.getUB()))
	{
		// then (remR.ub, mRange.ub]: remR.ub < mRange.lb
		mRange.getConstr()->eraseVarConstr(mRange.getLB());
		mRange.getConstr()->assign(remR.getUB(), mRange.getLB(), 1, 1, -1);
		// if mRange.ub appears in remR's constraints, update its relationship with remR.lb
		if(remR.getConstr()->containsVar(mRange.getLB()))
		{
			remR.getConstr()->eraseVarConstr(mRange.getLB());
			remR.getConstr()->assign(remR.getUB(), mRange.getLB(), 1, 1, -1);
		}
		//modified = mRange.lb != remR.lb;
		success = true;
	}
	
	// Remove remR.lb and remR.ub from mRange.cg after the comparison
	// if they were not originally in mRange.cg
	if(!containsLB) mRange.getConstr()->eraseVarConstr(remR.getLB());
	if(!containsUB) mRange.getConstr()->eraseVarConstr(remR.getUB());

	return success;
}

// Returns true if this message expression can be merged with that message expression
// with NO loss of information, in all possible executions (i.e. its a must-statement,
// not a may-statement)
bool OneDmesgExpr::mustMergeable(const mesgExpr& that_arg) const
{
	const OneDmesgExpr& that = dynamic_cast<const OneDmesgExpr&>(that_arg);
	ROSE_ASSERT(mRange.getConstr() == that.mRange.getConstr());
	
	/*cout << "mustMergeable: eqVars("<<mRange.getUB().str()<<", "<<that.mRange.getLB().str()<<", 1, 1, -1)="<<mRange.getConstr()->eqVars(mRange.getUB(), that.mRange.getLB(), 1, 1, -1)<<"\n";
	cout << "mustMergeable: eqVars("<<that.mRange.getUB().str()<<", "<<mRange.getLB().str()<<", 1, 1, -1)="<<mRange.getConstr()->eqVars(mRange.getUB(), that.mRange.getLB(), 1, 1, -1)<<"\n";*/
	
	       // [range][that.range] == mRange.ub*1 = that.mRange.lb*1 - 1
	return mRange.getConstr()->eqVars(mRange.getUB(), that.mRange.getLB(), 1, 1, -1) ||
	       // [that.range][range] == that.mRange.ub*1 = mRange.lb*1 - 1
	       mRange.getConstr()->eqVars(that.mRange.getUB(), mRange.getLB(), 1, 1, -1);
}

// Merges this and that and updates this with the result.
// Returns true if this causes this message expression to change, false otherwise.
bool OneDmesgExpr::mergeUpd(const mesgExpr& that_arg)
{
	const OneDmesgExpr& that = dynamic_cast<const OneDmesgExpr&>(that_arg);
	ROSE_ASSERT(mRange.getConstr() == that.mRange.getConstr());
	
	if(analysisDebugLevel>=1) 
		cout << "mRange.getConstr()->eqVars("<<mRange.getUB().str()<<", "<<that.mRange.getLB().str()<<", 1, 1, -1)="<<mRange.getConstr()->eqVars(mRange.getUB(), that.mRange.getLB(), 1, 1, -1)<<"\n";
	// [range][that.range] == mRange.ub*1 = that.mRange.lb*1 - 1
	if(mRange.getConstr()->eqVars(mRange.getUB(), that.mRange.getLB(), 1, 1, -1))
		mRange.assignUB(that.mRange.getUB());
	if(analysisDebugLevel>=1) 
		cout << "mRange.getConstr()->eqVars("<<that.mRange.getLB().str()<<", "<<mRange.getUB().str()<<", 1, 1, -1)="<<mRange.getConstr()->eqVars(that.mRange.getLB(), mRange.getLB(), 1, 1, -1)<<"\n";
	// [that.range][range] == that.rangeUB*1 = rangeLB*1 - 1
	if(mRange.getConstr()->eqVars(that.mRange.getUB(), mRange.getLB(), 1, 1, -1))
		mRange.assignLB(that.mRange.getLB());
}

// The number of different communication operations represented by this message expression
int OneDmesgExpr::numCommOps() const
{
	return mRange.size();
}

// Transition from using the current constraint graph to using newCG, while annotating
// the lower and upper bound variables of the range with the given annotation annotName->annot.
// Return true if this causes this set to change, false otherwise.
bool OneDmesgExpr::setConstrAnnot(ConstrGraph* newCG, string annotName, void* annot)
{
	return mRange.setConstrAnnot(newCG, annotName, annot);
}

// Returns the constraint graph used by the mRange process set
ConstrGraph* OneDmesgExpr::getConstr() const
{
	return mRange.getConstr();
}

// Sets the constraint graph used by the mRange process set
// Returns true if this causes the message expression to change, false otherwise
bool OneDmesgExpr::setConstr(ConstrGraph* cg)
{
	return mRange.setConstr(cg);
}

// Assigns this to that
mesgExpr& OneDmesgExpr::operator=(const mesgExpr& that_arg)
{
	const OneDmesgExpr& that = dynamic_cast<const OneDmesgExpr&>(that_arg);
	mRange = that.mRange;
	return *this;
}

mesgExpr& OneDmesgExpr::operator=(const OneDmesgExpr& that_arg)
{
	const OneDmesgExpr& that = dynamic_cast<const OneDmesgExpr&>(that_arg);
	mRange = that.mRange;
	return *this;
}

// Comparison
bool OneDmesgExpr::operator==(const mesgExpr& that_arg) const
{
	const OneDmesgExpr& that = dynamic_cast<const OneDmesgExpr&>(that_arg);
	return mRange == that.mRange;
}

void OneDmesgExpr::transClosure()
{
	mRange.getConstr()->localTransClosure(mRange.getLB());
	mRange.getConstr()->localTransClosure(mRange.getUB());
}

// Returns a string representation of this set
string OneDmesgExpr::str(string indent) const
{ 
	ostringstream outs;
	outs << indent << "[OneDmesgExpr: mRange="<<mRange.str()<<"]";
	return outs.str();
}
