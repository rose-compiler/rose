
#ifndef FLOW_GRAPH_H
#define FLOW_GRAPH_H

#include <rosepoly/Nodes.h>

class FlowGraph {
	
	BasicNode * Head;
	BasicNode * Tail;
	
	bool temp;
	int id;
	
public:
	
	FlowGraph( BasicNode * h, BasicNode * t, int i );
	
	~FlowGraph();
	
	BasicNode * get_head();
	BasicNode * get_tail();
	
	int get_id() const;
	
	void is_temp();
	
	virtual void insertNode( BasicNode * aNode, Direction D = FORWARD );
	virtual void insertSubGraph( FlowGraph * subGraph );
	
	virtual void print( int ident, Direction d = FORWARD ) const;
	
};

#endif

