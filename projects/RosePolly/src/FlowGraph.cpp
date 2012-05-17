/*
 *  FlowGraph.cpp
 *  
 *
 *  Created by Konstantinidis, Athanasios on 1/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <rose.h>

using namespace std;

#include <rosepoly/data.h>
#include <rosepoly/FlowGraph.h>

FlowGraph::FlowGraph( BasicNode * h, BasicNode * t, int i )
: Head(h), Tail(t), temp(false), id(i) {}

FlowGraph::~FlowGraph() 
{
	if (temp) return;
	
	if (Head) {
		BasicNode * iter1 = Head;
		BasicNode * iter2;
			
		while ( iter1 ) {
			iter2 = iter1->next(FORWARD);
			NodeType t = iter1->get_type();
			if ( t == CONDITIONHEAD || t == CONDITIONTAIL ) {
					
				Conditional * cond = (Conditional*)iter1;
				if ( cond->done() )
					delete(iter1);

			} else {
				delete(iter1);
			}
			iter1 = iter2;
		}
	}
}

BasicNode * FlowGraph::get_head() { return Head; }

BasicNode * FlowGraph::get_tail() { return Tail; }

int FlowGraph::get_id() const { return id; }

void FlowGraph::is_temp() { temp = true; }

void FlowGraph::insertNode( BasicNode * aNode, Direction D )
{
	if ( D == FORWARD ) {
		BasicNode * iter;
		if ( Head != NULL ) {
			Tail->link( aNode, FORWARD );
			if ( aNode != NULL ) {
				aNode->link( Tail, BACKWARD );
			}
		} else {
			Head = aNode;
			Tail = aNode;
		}
		iter = Tail;
		while ( iter != NULL ) {
			Tail = iter;
			iter = iter->next(FORWARD);
		}
	} else {
		BasicNode * iter;
		if ( Head != NULL ) {
			Head->link( aNode, BACKWARD );
			if ( aNode != NULL )
				aNode->link( Head, FORWARD );
		} else {
			Head = aNode;
			Tail = aNode;
		}
		iter = Head;
		while ( iter != NULL ) {
			Head = iter;
			iter = iter->next(BACKWARD);
		}
	}
}

void FlowGraph::insertSubGraph( FlowGraph * subGraph ) 
{
	if ( Head != NULL ) {
		Tail->link( subGraph->get_head(), FORWARD );
		if ( subGraph->get_head() != NULL )
			subGraph->get_head()->link( Tail, BACKWARD );
		Tail = subGraph->get_tail();
	} else {
		Head = subGraph->get_head();
		Tail = subGraph->get_tail();
	}
}

void FlowGraph::print( int ident, Direction d ) const
{
	if ( Head == NULL )
		return;
	
	BasicNode * Start = (d==FORWARD) ? Head : Tail;
	BasicNode * iter = Start;
	int totalIdent = ident;
	
	do {
		
		switch (iter->get_type())
		{
			case LOOPHEAD:
			case CONDITIONHEAD:
				iter->print(totalIdent);
				totalIdent += 5;
				break;
			case LOOPTAIL:
			case CONDITIONTAIL:
				totalIdent -= 5;
				iter->print(totalIdent);
				break;
			default:
				iter->print(totalIdent);
		}
		
		iter = iter->next(d);
	} while( iter );
}


