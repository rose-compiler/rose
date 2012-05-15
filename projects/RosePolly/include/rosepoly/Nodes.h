

#ifndef NODES_H
#define NODES_H

extern SgGlobal * GlobalScope;

typedef enum {
	FORWARD = 0,
	BACKWARD = 1
} Direction;

typedef enum {
	STATEMENT     = 0,
	LOOPHEAD      = 1,
	LOOPTAIL      = 2,
	CONDITIONHEAD = 3,
	CONDITIONTAIL = 4
} NodeType;

typedef enum {
	UNDEFINED = 0,
	PARALLEL,
	PIP_PARALLEL,
	SEQ
} loop_type;

class FlowGraph;
class AccessPattern;

template <class T>
class polly_iterator {
	
	typename vector<T*>::iterator start;
	typename vector<T*>::iterator i;
	typename vector<T*>::iterator end;
	
public:
	
	polly_iterator( typename vector<T*>::iterator a, typename vector<T*>::iterator e )
	: start(a), end(e), i(a) {}
	
	bool next();
	void reset() { i = start; }
	bool test() const { return i == end; }
	bool empty() const { return start == end; }
	
	T* operator->() { return *i; }
	T* operator *() { return *i; }
};

template <class T>
bool polly_iterator<T>::next()
{
	if (i != end) ++i;
	return i != end;
}

class BasicNode {
	
protected:
	
	NodeType myType;
	
public:
	
	BasicNode( NodeType aType );
	
	virtual ~BasicNode();
	
	NodeType get_type() const;
	
	virtual void print( int ident )         const=0;
	virtual BasicNode * next( Direction d )  =0;
	virtual void link( BasicNode * aNode, Direction d ) =0;
	
};

class SimpleNode : public BasicNode
{
protected:
	
	BasicNode * succ;
	BasicNode * pred;
	
public:
	
	SimpleNode( NodeType aType );
	
	virtual ~SimpleNode();
	
	void replace( SimpleNode * n );
	
	virtual void print( int ident ) const;
	virtual BasicNode * next( Direction d );
	virtual void link( BasicNode * aNode, Direction d );
};


class Statement : public SimpleNode
{
	
protected:
	
	vector<AccessPattern*> Reads;
	vector<AccessPattern*> Writes;
	
	vector<loop_type> l_types;
	
	int ID;
	SgExprStatement * myStatement;
	
public:
	
	Statement( const Statement& s );
	Statement( SgExprStatement * stm, int anID );
	
	virtual ~Statement();
	
	void set_patterns( symbol_table& data );
	void add_pattern( AccessPattern * pat );
	void push_loop_type( loop_type t );
	
	int get_ID() const;
	SgExprStatement * get_statement() const;
	AccessPattern * get_read( int pos ) const;
	AccessPattern * get_write( int pos ) const;
	string get_name() const;
	
	polly_iterator<AccessPattern> get_reads();
	polly_iterator<AccessPattern> get_writes();
	
	virtual void print( int ident ) const;
	
};

class Conditional : public BasicNode
{
	SgExpression * myExp;
	
	BasicNode * true_branch;
	BasicNode * false_branch;
	BasicNode * out;
	Conditional * dual;
	
	bool path;
	bool true_active;
	
public:
	
	Conditional( SgExpression * anExp, NodeType t = CONDITIONHEAD );
	
	virtual ~Conditional();
	
	void set_true( FlowGraph * g );
	void set_false( FlowGraph * g );
	void set_dual( Conditional * c );
	void set_out( BasicNode * o );
	void set_true_node( BasicNode * aNode );
	void set_false_node( BasicNode * aNode );
	
	bool done() const;
	SgExpression * get_exp() const;
	Conditional * get_dual() const;
	BasicNode * get_out() const;
	
	virtual void print( int ident ) const;
	virtual BasicNode * next( Direction d );
	virtual void link( BasicNode * aNode, Direction d );
};

class ForLoop : public BasicNode
{
	
protected:
	
	SgExpression * start;
	SgExpression * end;
	string	symbol;
	
	SgForStatement * myLoop;
	
	ForLoop * back_edge;
	BasicNode * body;
	BasicNode * out;
	
	loop_type myLoopType;
	
	bool Iter;
	
public:
	
	ForLoop( const ForLoop& l );
	ForLoop( SgForStatement * l );
	ForLoop( SgExpression * st, SgExpression * e, string sy, SgForStatement * l, 
			ForLoop * bd, ForLoop * be, NodeType t );
	
	ForLoop( SgExpression * st, SgExpression * e, string sy );
	
	virtual ~ForLoop();
	
	void set_body( FlowGraph * g );
	void set_body_node( BasicNode * b );
	void iterate();
	
	SgExpression * get_start() const;
	SgExpression * get_end() const;
	string unparse_start() const;
	string unparse_end() const;
	SgForStatement * get_loop() const;
	string get_symbol() const;
	loop_type get_loop_type() const;
	bool is_head() const;
	
	virtual void print( int ident ) const;
	virtual BasicNode * next( Direction d );
	virtual void link( BasicNode * aNode, Direction d );
	
};

#endif








