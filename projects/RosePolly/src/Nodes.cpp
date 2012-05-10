/*
 *  Nodes.cpp
 *  
 *
 *  Created by Konstantinidis, Athanasios on 1/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <rose.h>

using namespace SageBuilder;
using namespace std;

#include <rosepoly/data.h>
#include <rosepoly/FlowGraph.h>
#include <rosepoly/error.h>
#include <rosepoly/access_pattern.h>

// BASIC NODE

BasicNode::BasicNode( NodeType aType ) : myType(aType) {}

NodeType BasicNode::get_type() const { return myType; }

BasicNode::~BasicNode() {}

// SIMPLE NODE

SimpleNode::SimpleNode( NodeType aType )
: BasicNode(aType), succ(NULL), pred(NULL) {}

SimpleNode::~SimpleNode() {}

void SimpleNode::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"[ERROR --> NO PRINT FUNCTION FOR THIS SIMPLE NODE]"<<endl;
}

void SimpleNode::replace( SimpleNode * n )
{
	succ = n->succ;
	succ->link(this,BACKWARD);
	pred = n->pred;
	pred->link(this,FORWARD);
	delete(n);
}

BasicNode * SimpleNode::next( Direction d )
{
	return (d==FORWARD) ? succ : pred;
}

void SimpleNode::link( BasicNode * aNode, Direction d )
{
	if ( d == FORWARD )
		succ = aNode;
	else 
		pred = aNode;
}


// STATEMENT

Statement::Statement( const Statement& s ) :
SimpleNode(STATEMENT), myStatement(s.get_statement()), 
Reads(s.Reads), Writes(s.Writes), ID(s.get_ID()), l_types(s.l_types) {}

Statement::Statement( SgExprStatement * stm, int anID ) : 
SimpleNode(STATEMENT), myStatement(stm), ID(anID) {}

Statement::~Statement() {}

void Statement::set_patterns( symbol_table& data )
{
	SgExpression * exp = myStatement->get_expression();
	IO lhsIO;
	vector<AccessPattern*> patList;
	
	if ( !isSgBinaryOp(exp) ) {
		if ( isSgUnaryOp(exp) )
			search_for_access_patterns( isSgUnaryOp(exp)->get_operand(), patList, INOUT );
		else
			report_error("Unrecognized expression statement",exp);

	} else {
		lhsIO = ( isSgAssignOp(exp) ) ? OUT : INOUT;

		/* Left hand side */
		search_for_access_patterns( isSgBinaryOp(exp)->get_lhs_operand(), patList, lhsIO );
		
		/* Right hand side */
		search_for_access_patterns( isSgBinaryOp(exp)->get_rhs_operand(), patList, IN );
	}
	
	for ( int i = 0 ; i < patList.size() ; i++ ) { 
		
		int dim = patList[i]->get_dim();
		SgExpression * exp = patList[i]->get_refExp();
		
		for ( int j = 0 ; j < dim ; j++ )
			exp = isSgPntrArrRefExp(exp)->get_lhs_operand();
		
		string ap_name = isSgVarRefExp(exp)->get_symbol()->get_name().getString();
		
		symbol_table::iterator it = data.find(ap_name);
		it->second.add_pattern(patList[i]);
		add_pattern(patList[i]);
	}
}

void Statement::push_loop_type( loop_type t ) { l_types.push_back(t); }

int Statement::get_ID() const { return ID; }

AccessPattern * Statement::get_read( int pos ) const { return Reads[pos]; }

AccessPattern * Statement::get_write( int pos ) const { return Writes[pos]; }

SgExprStatement * Statement::get_statement() const { return myStatement; }

string Statement::get_name() const
{
	stringstream out;
	out<<"S"<<ID;
	return out.str();
}

polly_iterator<AccessPattern> Statement::get_reads() 
{
	return polly_iterator<AccessPattern>( Reads.begin(), Reads.end() );
}

polly_iterator<AccessPattern> Statement::get_writes()
{
	return polly_iterator<AccessPattern>( Writes.begin(), Writes.end() );
}

void Statement::add_pattern( AccessPattern * pat )
{
	switch( pat->get_IO() )
	{
		case INOUT:
			Writes.push_back(pat);
		case IN:
			Reads.push_back(pat);
			break;
		case OUT:
			Writes.push_back(pat);
			break;
	}
}

void Statement::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"[STATEMENT("<<ID<<") : "<<myStatement->unparseToString()<<"]"<<endl;
	
	for ( int i = 0 ; i < ident+5 ; i++ )
		cout<<" ";
	
	cout<<"[Writes]"<<endl;
	for ( int i = 0 ; i < Writes.size() ; i++ )
		Writes[i]->print(ident+5);
	
	for ( int i = 0 ; i < ident+5 ; i++ )
		cout<<" ";
	
	cout<<"[Reads]"<<endl;
	for ( int i = 0 ; i < Reads.size() ; i++ )
		Reads[i]->print(ident+5);
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"__loop_types__"<<endl;
	for ( int i = 0 ; i < l_types.size() ; i++ ) {
		for ( int j = 0 ; j < ident ; j++ )
			cout<<" ";
		switch (l_types[i])
		{
			case PARALLEL:
				cout<<"PARALLEL"<<endl;
				break;
			case PIP_PARALLEL:
				cout<<"PIPELINE"<<endl;
				break;
			case SEQ:
				cout<<"SEQUENTIAL"<<endl;
				break;
			default:
				cout<<"UNDEFINED"<<endl;
				break;
		}
	}
}

// CONDITIONAL

Conditional::Conditional( SgExpression * anExp, NodeType t ) :
BasicNode(t), myExp(anExp), true_branch(NULL), false_branch(NULL),
out(NULL), path(true), true_active(true)
{
	if ( t == CONDITIONHEAD ) {
		dual = new Conditional(anExp,CONDITIONTAIL);
		dual->set_dual(this);
		true_branch = dual;
		false_branch = dual;
		dual->set_true_node(this);
		dual->set_false_node(this);
	}
}

Conditional::~Conditional() {}

void Conditional::set_true( FlowGraph * g )
{
	if ( g->get_head() == NULL )
		return;	
	
	if ( myType == CONDITIONHEAD ) {
		true_branch = g->get_head();
		dual->set_true_node(g->get_tail());
		g->get_head()->link(this,BACKWARD);
		g->get_tail()->link(dual,FORWARD);
	} else {
		dual->set_true(g);
	}
}

void Conditional::set_false( FlowGraph * g )
{
	if ( g->get_head() == NULL )
		return;
	
	if ( myType == CONDITIONHEAD ) {
		false_branch = g->get_head();
		dual->set_false_node(g->get_tail());
		g->get_head()->link(this,BACKWARD);
		g->get_tail()->link(dual,FORWARD);
	} else {
		dual->set_false(g);
	}
}

void Conditional::set_dual( Conditional * c ) { dual = c; }

void Conditional::set_out( BasicNode * o ) { out = o; }

void Conditional::set_true_node( BasicNode * aNode ) { true_branch = aNode; }

void Conditional::set_false_node( BasicNode * aNode ) { false_branch = aNode; }

bool Conditional::done() const { return path; }

Conditional * Conditional::get_dual() const { return dual; }

SgExpression * Conditional::get_exp() const { return myExp; }

BasicNode * Conditional::get_out() const { return out; }

void Conditional::print( int ident ) const
{
	if ( myType == CONDITIONHEAD ) {
		if ( path ) {
			for ( int i = 0 ; i < ident ; i++ )
				cout<<" ";
			
			cout<<"[CONDITIONAL : "<<myExp->unparseToString()<<"]"<<endl;
			
			for ( int i = 0 ; i < ident ; i++ )
				cout<<" ";
			
			cout<<"__true__"<<endl;
		} else {
			for ( int i = 0 ; i < ident ; i++ )
				cout<<" ";
			
			cout<<"__false__"<<endl;
		}
	} else {
		if ( dual->done() ) {
			for ( int i = 0 ; i < ident ; i++ )
				cout<<" ";
			
			cout<<"[CONDITION END]"<<endl;
		}
	}
}

BasicNode * Conditional::next( Direction d )
{
	if ( myType == CONDITIONHEAD ) {
		if ( d == FORWARD ) {
			if (path) {
				path = false;
				return true_branch;
			} else {
				path = true;
				return false_branch;
			}
		} else {
			if ( !dual->done() ) {
				return dual;
			} else {
				return out;
			}
		}
	} else {
		if ( d == BACKWARD ) {
			if (path) {
				path = false;
				return true_branch;
			} else {
				path = true;
				return false_branch;
			}
		} else {
			if ( path ) {
				path = false;
				return dual;
			} else {
				path = true;
				return out;
			}
		}
	}
}

void Conditional::link( BasicNode * aNode, Direction d )
{
	ROSE_ASSERT( (myType == CONDITIONHEAD && d == BACKWARD) || (myType == CONDITIONTAIL && d == FORWARD) );
	out = aNode;
}

// FOR LOOP

ForLoop::ForLoop( const ForLoop& l ) : 
BasicNode(LOOPHEAD), myLoop(l.get_loop()), myLoopType(UNDEFINED), symbol(l.get_symbol()),
start(l.get_start()), end(l.get_end()), body(NULL), out(NULL), Iter(false)
{
	back_edge = new ForLoop(start,end,symbol,myLoop,this,this,LOOPTAIL);
	body = back_edge;
}

ForLoop::ForLoop( SgForStatement * l ) 
: BasicNode(LOOPHEAD), myLoop(l), myLoopType(UNDEFINED), start(NULL), end(NULL),
body(NULL), back_edge(NULL), out(NULL), Iter(false)
{
	/* STEP 1 : Get initialization expression and symbol */
	SgStatementPtrList stmList = myLoop->get_init_stmt();
	if ( stmList.size() != 1 ) {
		report_error("Too many init statements",l);
	} else if ( isSgVariableDeclaration(stmList[0]) ) {
		
		SgInitializedNamePtrList initList = isSgVariableDeclaration(stmList[0])->get_variables();
		if ( initList.size() != 1 ) {
			report_error("To many induction variables",l);
		} else {
			SgInitializedName * initName = initList[0];
			if ( isSgAssignInitializer(initName->get_initializer()) ) {
				symbol = initName->get_name().getString();
				start = isSgAssignInitializer(initName->get_initializer())->get_operand();
			} else {
				report_error("Loop initializer is too complecated",initName);
			}
		}
		
	} else if ( isSgExprStatement(stmList[0]) ) {
		SgExpression * exp = isSgExprStatement(stmList[0])->get_expression();
		if ( isSgAssignOp(exp) ) {
			SgExpression * lhs = isSgAssignOp(exp)->get_lhs_operand();
			SgExpression * rhs = isSgAssignOp(exp)->get_rhs_operand();
			if ( isSgVarRefExp(lhs) ) {
				symbol = isSgVarRefExp(lhs)->get_symbol()->get_name().getString();
				start = rhs;
			} else {
				report_error("LHS of expression must be a single variable",exp);
			}
		} else {
			report_error("Init expression must be an Assign operation",exp);
		}
	} else {
		report_error("Loop initialization is not recognized",l);
	}
	
	/* STEP 2 : Get the test expression */
	SgExprStatement * expStm = isSgExprStatement(myLoop->get_test());
	if ( expStm ) {
		SgExpression * exp = expStm->get_expression();
		if ( isSgLessOrEqualOp(exp) ) {
			SgBinaryOp * binOp = isSgBinaryOp(exp);
			string name = isSgVarRefExp(isSgBinaryOp(exp)->get_lhs_operand())->get_symbol()->get_name().getString();
			
			if ( name != symbol )
				report_error("Loop init and test variable miss-match",exp);
			
			end = binOp->get_rhs_operand();
			
		} else if ( isSgLessThanOp(exp) ) {
			
			SgBinaryOp * binOp = isSgBinaryOp(exp);
			string name = isSgVarRefExp(binOp->get_lhs_operand())->get_symbol()->get_name().getString();
			
			if ( name != symbol )
				report_error("Loop init and test variable miss-match",exp);
			
			SgExpression * tempExp = SageInterface::copyExpression(binOp->get_rhs_operand());
			end = buildSubtractOp( tempExp, buildIntVal(1) );
			end->set_need_paren(true);
			tempExp = buildLessOrEqualOp( SageInterface::copyExpression(binOp->get_lhs_operand()), end );
			
			SageInterface::replaceExpression(exp, tempExp, false);
			
		} else {
			report_error("Test expression is not recognized. Re-write the loop or normilize it accordingly",exp);
		}
	} else {
		report_error("Test expression is not recognized. Sorry !", l);
	}
	
	/* STEP 3 : Check the stride */
	if ( !isSgPlusPlusOp(l->get_increment()) )
		report_error("Increment expression is not recognized. Re-write the loop or normilize it accordingly. Note: Only \"++\" operator supported.",l);
	
	/* STEP 4 : Link with Loop Tail node */
	back_edge = new ForLoop(start,end,symbol,l,this,this,LOOPTAIL);
	body = back_edge;
}

ForLoop::ForLoop( SgExpression * st, SgExpression * e, string sy )
: BasicNode(LOOPHEAD), start(st), end(e), symbol(sy), myLoop(NULL), myLoopType(UNDEFINED),
out(NULL), Iter(false)
{
	back_edge = new ForLoop(start,end,symbol,NULL,this,this,LOOPTAIL);
	body = back_edge;
}

ForLoop::ForLoop( SgExpression * st, SgExpression * e, string sy, SgForStatement * l, 
				 ForLoop * bd, ForLoop * be, NodeType t )
: BasicNode(t), start(st), end(e), symbol(sy), myLoop(l), myLoopType(UNDEFINED), 
out(NULL), body(bd), back_edge(be), Iter(false) {}

ForLoop::~ForLoop() {}

void ForLoop::set_body( FlowGraph * g ) 
{
	if ( g->get_head() == NULL )
		return;
	
	if ( myType == LOOPHEAD ) {
		body = g->get_head();
		back_edge->set_body_node( g->get_tail() );
		g->get_head()->link(this,BACKWARD);
		g->get_tail()->link(back_edge,FORWARD);
	} else {
		back_edge->set_body(g);
	}
}

bool ForLoop::is_head() const { return myType==LOOPHEAD; }

SgExpression * ForLoop::get_start() const { return start; }

SgExpression * ForLoop::get_end() const { return end; }

string ForLoop::unparse_start() const { return start->unparseToString(); }

string ForLoop::unparse_end() const { return end->unparseToString(); }

SgForStatement * ForLoop::get_loop() const { return myLoop; }
													  
loop_type ForLoop::get_loop_type() const { return myLoopType; }

string ForLoop::get_symbol() const { return symbol; }

void ForLoop::set_body_node( BasicNode * b ) { body = b; }

void ForLoop::iterate() { Iter = true; }

void ForLoop::print( int ident ) const 
{
	string type_string;
	switch(myLoopType)
	{
		case PARALLEL:
			type_string = "PARALLEL";
			break;
		case PIP_PARALLEL:
			type_string = "PIP_PARALLEL";
			break;
		case SEQ:
			type_string = "SEQUENCIAL";
			break;
		default:
			type_string = "NOT RECOGNIZED";
	}
	
	if ( myType == LOOPHEAD ) {
		for ( unsigned int i = 0 ; i < ident ; i++ )
			cout<<" ";
		
		cout<<"[ "<<type_string<<" LOOP("<<symbol<<"), start:"<<start->unparseToString()<<", end:"<<end->unparseToString()<<"]"<<endl;
	} else {
		for ( unsigned int i = 0 ; i < ident ; i++ )
			cout<<" ";
		
		cout<<"[LOOP END]"<<endl;
	}
}

BasicNode * ForLoop::next( Direction d ) 
{ 
	if ( myType == LOOPHEAD ) {
		if ( d == FORWARD ) {
			return body;
		} else {
			if (Iter) {
				Iter = false;
				return back_edge;
			} else {
				return out;
			}
		}
	} else {
		if ( d == FORWARD ) {
			if (Iter) {
				Iter = false;
				return back_edge;
			} else {
				return out;
			}
		} else {
			return body;
		}
	}
}

void ForLoop::link( BasicNode * aNode, Direction d ) 
{
	if ( (myType == LOOPHEAD && d == BACKWARD) || (myType == LOOPTAIL && d == FORWARD) )
		out = aNode;
	else
		body = aNode;
}







