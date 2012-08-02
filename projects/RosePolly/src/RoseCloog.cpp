
#include <rose.h>
#include <SageBuilder.h>

using namespace std;

#include <rosepoly/RosePollyModel.h>
#include <rosepoly/RoseCloog.h>
#include <rosepoly/simple_matrix.h>

using namespace SageBuilder;

string charToString( const char * c )
{
	int i = 0;
	string outS;
	if ( c == NULL )
		return outS;
	
	while ( c[i] != '\0' )
		outS.push_back(c[i++]);
	
	return outS;
}

// CLOOG

RoseCloog::RoseCloog( const RosePollyModel& model )
: RosePollyModel(model), state(cloog_state_malloc()), 
cloog_graph(NULL), ast(NULL), opts(NULL)
{
	free_polly_context( state->backend->ctx );
	state->backend->ctx = RosePollyBase::context;
}

RoseCloog::~RoseCloog() 
{ 
	state->backend->ctx = NULL;
	cloog_state_free(state);
	delete(cloog_graph);
	free_clast_stmt(ast);
	cloog_options_free(opts);
}

CloogOptions * RoseCloog::init_default_options()
{
	CloogState * s = cloog_state_malloc();
	CloogOptions * opts = cloog_options_malloc(s);
	opts->state = NULL;
	cloog_state_free(s);
	return opts;
}

void RoseCloog::apply( CloogOptions * o )
{
	cout<<"[CLOOG]"<<endl;
	cout<<endl;
	
	o->state = state;
	
	if ( ast )
		free_clast_stmt(ast);
	
	if ( cloog_graph )
		delete(cloog_graph);
	
	opts = o;
	
	CloogUnionDomain * scop = cloog_union_domain_alloc(parameters.size());
	
	for ( int i = 0 ; i < parameters.size() ; i++ )
		cloog_union_domain_set_name(scop,CLOOG_PARAM,i,parameters[i].c_str());
	
	CloogDomain * param_ctx = get_param_context();
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		CloogDomain * dom = get_cloog_domain(stmts[i]->get_domain());
		CloogScattering * sch = get_cloog_scatter(stmts[i]->get_transformation());
		string name = stmts[i]->get_name();
		cloog_union_domain_add_domain(scop,name.c_str(),dom,sch,stmts[i]);
	}
	
	CloogInput * cl_input = cloog_input_alloc(param_ctx,scop);
	
	ast = cloog_clast_create_from_input(cl_input, opts);
}

CloogDomain * RoseCloog::get_cloog_domain( pollyDomain * dom ) const
{
	return dom->get_cloog_domain();
}

CloogScattering * RoseCloog::get_cloog_scatter( pollyMap * map ) const
{
	return map->get_cloog_scatter();
}

FlowGraph * RoseCloog::print_to_flow_graph()
{
	if ( cloog_graph )
		return cloog_graph;
	
	cloog_graph = unparse_clast(ast);
	
	return cloog_graph;
}

void RoseCloog::print_to_screen() const
{
	if ( ast )
		clast_pprint(stdout, ast, 0, opts);
}

void RoseCloog::print_to_file( const char * name ) const
{
	if (!ast)
		return;
	
	FILE * pFile = fopen(name,"w");
	clast_pprint(pFile, ast, 0, opts);
	fclose(pFile);
}

CloogDomain * RoseCloog::get_param_context() const
{
	int pSize = parameters.size();
	simple_matrix mat(pSize,pSize+1); // +1 for the cst
	for ( int i = 0 ; i < pSize ; i++ ) {
		mat.set_entry(i,i,1);
		mat.set_entry(i,pSize,-50);
	}
	
	pollyDomain param_ctx(parameters);
	param_ctx.append_inequalities(&mat);
	
	return param_ctx.get_cloog_domain();
}

FlowGraph * RoseCloog::unparse_clast( struct clast_stmt * s ) const
{
	FlowGraph * flow = new FlowGraph(NULL,NULL,ID);
	
	for ( ; s ; s = s->next ) {
		if ( CLAST_STMT_IS_A(s, stmt_root) )
			continue;
		
		if ( CLAST_STMT_IS_A(s, stmt_ass) ) {
			cout<<"[CLAST NODE NOT HANDLED]"<<endl;
		} else if ( CLAST_STMT_IS_A(s, stmt_user) ) {
			flow->insertNode( unparse_clast_stmt((struct clast_user_stmt*)s) );
		} else if ( CLAST_STMT_IS_A(s, stmt_for) ) {
			flow->insertNode( unparse_clast_for((struct clast_for*)s) );
		} else if ( CLAST_STMT_IS_A(s, stmt_guard) ) {
			cout<<"[CLAST NODE NOT HANDLED]"<<endl;
		} else if ( CLAST_STMT_IS_A(s, stmt_block) ) {
			cout<<"[CLAST NODE NOT HANDLED]"<<endl;
		} else {
			assert(0);
		}
	}
	return flow;
}

Statement * RoseCloog::unparse_clast_stmt( struct clast_user_stmt * s ) const
{
	struct clast_stmt * t;
	CloogStatement * stm = s->statement;
	return new Statement(*(affineStatement*)stm->usr);
}

ForLoop * RoseCloog::unparse_clast_for( struct clast_for * f ) const
{
	SgExpression * start = unparse_clast_expr(f->LB);
	SgExpression * end = unparse_clast_expr(f->UB);
	string sym = charToString(f->iterator);
	ForLoop * loop = new ForLoop(start,end,sym);
	FlowGraph * tempGraph = unparse_clast(f->body);
	tempGraph->is_temp();
	loop->set_body( tempGraph );
	delete(tempGraph);
	
	return loop;
}

SgExpression * RoseCloog::unparse_clast_expr( struct clast_expr * e ) const
{
	if (!e)
		return NULL;
	
	switch (e->type) {
		case clast_expr_name:
			return unparse_clast_expr_name( (struct clast_name*)e );
		case clast_expr_term:
			return unparse_clast_expr_term( (struct clast_term*)e );
		case clast_expr_red:
			return unparse_clast_expr_red( (struct clast_reduction*)e );
		case clast_expr_bin:
			return NULL;
		default:
			assert(0);
	}
	return NULL;
}

SgVarRefExp * RoseCloog::unparse_clast_expr_name( struct clast_name * n ) const
{
	return buildVarRefExp(n->name, GlobalScope);
}

SgExpression * RoseCloog::unparse_clast_expr_term( struct clast_term * t ) const
{
	SgExpression * temp;
	if (t->var) {
		SgExpression * exp;
		
		int group = t->var->type == clast_expr_red &&
		((struct clast_reduction*) t->var)->n > 1;
		
		exp = unparse_clast_expr(t->var);
		
		if (group)
			exp->set_need_paren(true);
		
		if (cloog_int_is_one(t->val))
	    	;
		else if (cloog_int_is_neg_one(t->val))
	    	exp = buildMinusOp(exp);
        else {
			exp = buildMultiplyOp(unparse_clast_integer(t->val), exp);
		}
		return exp;
    } else {
		return unparse_clast_integer(t->val);
    }
	
	return NULL;
}


SgExpression * RoseCloog::unparse_clast_expr_red( struct clast_reduction * r ) const
{
	
	switch (r->type) {
    	case clast_red_sum:
			return unparse_clast_sum(r);
    	case clast_red_min:
    	case clast_red_max:
			if (r->n == 1) {
	    		return unparse_clast_expr(r->elts[0]);
			}
			break;
    	default:
			assert(0);
    }
	
	return NULL;
}

SgIntVal * RoseCloog::unparse_clast_integer( cloog_int_t i ) const
{								
	char *s;
	SgIntVal * val;						
	cloog_int_print_gmp_free_t gmp_free;			
	s = mpz_get_str(0, 10, i);				
	val = buildIntVal( atoi(s) );					
	mp_get_memory_functions(NULL, NULL, &gmp_free);		
	(*gmp_free)(s, strlen(s)+1);
	return val;				
}

SgExpression * RoseCloog::unparse_clast_sum( struct clast_reduction * r ) const
{
	
	int i;
	struct clast_term *t;
	SgExpression * exp;
	
    assert(r->n >= 1);
    assert(r->elts[0]->type == clast_expr_term);
    t = (struct clast_term *) r->elts[0];
    exp = unparse_clast_expr_term(t);
	
    for (i = 1; i < r->n; ++i) {
		assert(r->elts[i]->type == clast_expr_term);
		t = (struct clast_term *) r->elts[i];
		SgExpression * temp = unparse_clast_expr_term(t);
		if (cloog_int_is_pos(t->val)) {
			exp = buildAddOp(exp, temp);
	    } else {
			if ( isSgMinusOp(temp) != NULL ) {
				exp = buildSubtractOp(exp, isSgMinusOp(temp)->get_operand());
			} else if ( isSgIntVal(temp) != NULL ) {
				int value = isSgIntVal(temp)->get_value();
				exp = buildSubtractOp(exp, buildIntVal(-value));
				delete(temp);
			}
		}
    }
	
	return exp;
}

void RoseCloog::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"[CLOOG GRAPH]"<<endl;
	
	cloog_graph->print(ident);
}






