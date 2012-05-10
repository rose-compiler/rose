

#ifndef TRAVERSALS_H
#define TRAVERSALS_H

class FlowGraphTraversal
{
	BasicNode * iter;
	
	Direction D;
	
	bool next();
	
protected:
	
	bool termination;
	
public:
	
	FlowGraphTraversal( Direction d = FORWARD );
	void traverse( BasicNode * start );
	
	virtual void visit( BasicNode * aNode ) =0;
};

class RosePollyCustom;

class PollyEvaluateTraversal : public FlowGraphTraversal
{
	RosePollyCustom * policy;
	
public:
	
	bool result;
	PollyEvaluateTraversal( RosePollyCustom * c );
	virtual void visit( BasicNode * aNode );
};

class PollyModelExtractionTraversal : public FlowGraphTraversal
{
	vector<pollyDomain*> DomainStack;
	vector<string> params;
	vector<Statement*> toBeReplaced;
	RosePollyCustom * policy;
	
	vector<int> schBuf;
	
public:

	vector<affineStatement*> stmts;
	PollyModelExtractionTraversal( vector<string> p, RosePollyCustom * c );
	~PollyModelExtractionTraversal();
	virtual void visit( BasicNode * aNode );
	void consolidate();
};

class DependenceTraversal : public FlowGraphTraversal
{
	
public:
	
	typedef enum {
		HEAD = 0,
		TAIL = 1,
		COMPLETE = 2
	} depRegions;
	
	pollyDomain * Domain;
	affineStatement * Dest;
	int * llv;
	int llvSize;
	int llvIndex;
	int destRef;
	string destName;
	bool liveness;
	vector<depRegions> regions;
	vector<string> params;
	dependence_type type;
	
	DependenceTraversal();
	
	void init( affineStatement * s, dependence_type t );
	
	~DependenceTraversal();
	
	virtual void visit( BasicNode * aNode );
	
	void set_ref( int ref );
	
	void dependence_test( affineStatement * source, int ref );
	
};

#endif

