
#ifndef SG_CLOOG_H
#define SG_CLOOG_H

class RoseCloog : public RosePollyModel {
	
	CloogState * state;
	CloogOptions * opts;
	
	FlowGraph * cloog_graph;
	
	struct clast_stmt * ast;
	
	Statement * unparse_clast_stmt( struct clast_user_stmt * s ) const;
	ForLoop * unparse_clast_for( struct clast_for * f ) const;
	SgExpression * unparse_clast_expr( struct clast_expr * e ) const;
	SgVarRefExp * unparse_clast_expr_name( struct clast_name * n ) const;
	SgExpression * unparse_clast_expr_term( struct clast_term * t ) const;
	SgExpression * unparse_clast_expr_red( struct clast_reduction * r ) const;
	SgIntVal * unparse_clast_integer( cloog_int_t i ) const;
	SgExpression * unparse_clast_sum( struct clast_reduction * r ) const;
	
	CloogDomain * get_param_context() const;
	FlowGraph * unparse_clast( struct clast_stmt * s ) const;
	
public:
	
	RoseCloog( const RosePollyModel& model );
	
	static CloogOptions * init_default_options();
	
	FlowGraph * print_to_flow_graph();
	void print_to_screen() const;
	void print_to_file( const char * name ) const;
	
	CloogDomain * get_cloog_domain( pollyDomain * dom ) const;
	CloogScattering * get_cloog_scatter( pollyMap * map ) const;
	
	void apply( CloogOptions * opts );
	
	virtual void print( int ident ) const;
	
	~RoseCloog();
	
};

#endif

