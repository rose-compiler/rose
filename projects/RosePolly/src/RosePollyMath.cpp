

#include <rose.h>

using namespace std;

#include <rosepoly/data.h>
#include <rosepoly/Nodes.h>
#include <rosepoly/RosePollyMath.h>
#include <rosepoly/simple_matrix.h>


// POLLY ELEMENT

RosePollyBase::RosePollyBase() 
{
	if ( num_childs == 0 )
		init_context();
	num_childs++;
}

RosePollyBase::RosePollyBase( vector<string> u, vector<string> p )
: in_vars(u), params(p) { num_childs++; }

RosePollyBase::RosePollyBase( vector<string> p )
: params(p) 
{
	if ( num_childs == 0 )
		init_context();
	num_childs++;
}

RosePollyBase::~RosePollyBase() 
{
	if ( num_childs == 1 )
		free_context();
	num_childs--;
}

int RosePollyBase::num_childs = 0;

polly_context RosePollyBase::context = NULL;

void RosePollyBase::init_context()
{
	context = init_polly_context();
}

void RosePollyBase::free_context()
{
	free_polly_context(context);
}

vector<string> RosePollyBase::get_dims( dim_type t ) const
{
	switch (t)
	{
		case dim_in:
			return in_vars;
		case dim_out:
			return out_vars;
		case dim_param:
			return params;
	}
}

int RosePollyBase::get_total_size() const { return in_vars.size()+out_vars.size()+params.size()+1; }

int RosePollyBase::get_dim_size( dim_type t ) const 
{ 
	switch (t)
	{
		case dim_in:
			return in_vars.size();
		case dim_out:
			return out_vars.size();
		case dim_param:
			return params.size();
	}
}


// POLLY DOMAIN

pollyDomain::pollyDomain()
{	
	Set = build_universal_set(RosePollyBase::context);
}

pollyDomain::pollyDomain( const pollyDomain& p ) : 
RosePollyBase(p.get_dims(dim_in), p.get_dims(dim_param)) 
{
	Set = copy_integer_set(p.get_integer_set());
}

pollyDomain::pollyDomain( vector<string> p ) 
: RosePollyBase(p) 
{
	Set = build_integer_set(p,RosePollyBase::context);
}

integer_set pollyDomain::get_integer_set() const { return Set; }

bool pollyDomain::is_empty() const
{
	return integer_set_is_empty(Set);
}

simple_matrix * pollyDomain::get_matrix( bool ineq ) const
{	
	return build_integer_set_matrix(Set,ineq);
}

void pollyDomain::set_matrix( simple_matrix * mat, bool ineq ) 
{
	Set = build_integer_set_from_matrix(Set,mat,ineq);
}

void pollyDomain::set_constraints( simple_matrix * ineq, simple_matrix * eq )
{
	Set = build_integer_set_from_cst_matrices(Set,ineq,eq);
}

pollyDomain::~pollyDomain() { free_integer_set(Set); }
														  
void pollyDomain::add_loop( ForLoop * loop )
{
	if ( !Set )
		ROSE_ASSERT(false);  // if I ever reach that I need to handle it properly
	
	string start = loop->unparse_start();
	string end = loop->unparse_end();
	string symbol = loop->get_symbol();
	Set = integer_set_add_loop(Set,start,end,symbol);
	
	in_vars.push_back(symbol);
}

CloogDomain * pollyDomain::get_cloog_domain() const
{
	return integer_set_get_cloog_domain(Set);
}

void pollyDomain::add_dim( dim_type t, string var )
{
	switch (t)
	{
		case dim_in:
			in_vars.push_back(var);
			Set = integer_set_add_dim(Set,var,t);
			return;
		case dim_out:
			return;
		case dim_param:
			params.push_back(var);
			Set = integer_set_add_dim(Set,var,t);
			return;
	}
}

void pollyDomain::lexmin( bool non_neg )
{
	if ( non_neg ) {  // all unknowns are non-negative
		int width = get_total_size();
		simple_matrix * temp = new simple_matrix(width,width);
		for ( int i = 0 ; i < width ; i++ )
			temp->set_entry(i,i,1);
		append_inequalities(temp);
		delete(temp);
	}
	
	Set = integer_set_lexmin(Set);
}

void pollyDomain::add_conditional( SgExpression * exp, bool path )
{
	Set = integer_set_add_condition(Set,exp,path);
}

void pollyDomain::subtract( integer_set set )
{
	Set = integer_set_subtract_domains(Set,set);
}

void pollyDomain::append_inequalities( simple_matrix * mat )
{
	Set = integer_set_append_inequalities(Set,mat);
}

void pollyDomain::set_name( string name )
{
	Set = integer_set_set_name(Set,name);
}

void pollyDomain::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	print_integer_set(Set);
	cout<<endl;
}

void pollyDomain::print_matrix( int ident, bool ineq ) const
{
	cout<<setw(ident+1);
	for ( int i = 0 ; i < in_vars.size() ; i++ ) {
		cout<<in_vars[i];
		cout<<setw(6);
	}
	for ( int i = 0 ; i < params.size() ; i++ ) {
		cout<<params[i];
		cout<<setw(6);
	}
	cout<<"C"<<endl;
	
	cout<<setw(ident+1);
	for ( int i = 0 ; i < in_vars.size() ; i++ )
		for ( int j = 0 ; j < 6 ; j++ )
			cout<<"-";
	for ( int i = 0 ; i < params.size() ; i++ )
		for ( int j = 0 ; j < 6 ; j++ )
			cout<<"-";
	cout<<"-"<<endl;
	
	simple_matrix * mat = get_matrix(ineq);
	mat->print(ident);
	delete(mat);
	cout<<endl;
}


// POLLY MAP

pollyMap::pollyMap( const pollyMap& m )
: RosePollyBase(m.get_dims(dim_in),m.get_dims(dim_param)), Map(NULL)
{
	Map = copy_integer_map(m.get_integer_map());
}

pollyMap::pollyMap( const pollyMap& m, integer_map map )
: RosePollyBase(m.get_dims(dim_in),m.get_dims(dim_param)), Map(map) {}

pollyMap::pollyMap( const pollyDomain& d )
: RosePollyBase(d.get_dims(dim_in),d.get_dims(dim_param))
{
	Map = build_universal_schedule(d.get_integer_set());
}

pollyMap::pollyMap( pollyDomain * d, vector<SgExpression*>& s )
: RosePollyBase(d->get_dims(dim_in),d->get_dims(dim_param)), Map(NULL) 
{
	Map = build_access_map(d->get_integer_set(),s);
}

pollyMap::pollyMap( pollyDomain * d, vector<int> pos )
: RosePollyBase(d->get_dims(dim_in),d->get_dims(dim_param)), Map(NULL)
{
	Map = build_schedule_map(d->get_integer_set(),pos);
}

pollyMap::pollyMap( pollyDomain * src, pollyDomain * dest, int * llv )
: RosePollyBase(dest->get_dims(dim_in),dest->get_dims(dim_param)), Map(NULL)
{
	Map = build_map_with_order_csts(src->get_integer_set(), dest->get_integer_set(), llv);
}

void pollyMap::add_hyperplane( int * coeffs, int size )
{
	Map = integer_map_append_mapping(Map,coeffs,size);
}

simple_matrix * pollyMap::get_matrix( bool ineq ) const
{	
	return build_integer_set_matrix(Map,ineq);
}

void pollyMap::undo()
{
	Map = integer_map_undo(Map);
}

vector<pollyMap*> pollyMap::get_disjoints() const
{
	vector<integer_map> p_vect = gather_disjoint_maps(Map);
	vector<pollyMap*> total;
	for ( int i = 0 ; i < p_vect.size() ; i++ )
		total.push_back(new pollyMap(*this,p_vect[i]));
	
	return total;
}

CloogScattering * pollyMap::get_cloog_scatter() const
{
	return integer_map_get_cloog_scatter(Map);
}

void pollyMap::set_matrix( simple_matrix * mat, bool ineq ) 
{
	Map = build_integer_set_from_matrix(Map,mat,ineq);
}

void pollyMap::set_constraints( simple_matrix * ineq, simple_matrix * eq )
{
	Map = build_integer_set_from_cst_matrices(Map,ineq,eq);
}

void pollyMap::append_inequalities( simple_matrix * mat )
{
	Map = integer_set_append_inequalities(Map,mat);
}

int pollyMap::get_entry( int dim, int pos ) const
{
	return integer_map_get_entry(Map,dim,pos);
}

void pollyMap::skew( vector<int> lines, int width, bool top )
{	
	simple_matrix * mat = get_matrix(false);
		
	int start = top ? lines.size()-1 : width-1;
	int end = top ? start-width+1 : 0;
	
	int rows = mat->get_rows();
	/* start += rows-1;
	end += rows-1; */
	
	/* for ( int i = 0 ; i < lines.size() ; i++ )
		cout<<lines[i]<<" ";
	cout<<endl; */
	
	cout<<"[STATEMENT]"<<endl<<endl;
	
	mat->print(2);
	
	for ( int i = 0 ; i < in_vars.size() ; i++ ) {
		int total = 0;
		for ( int j = start ; j >= end ; j-- )
			total += mat->entry(rows-lines[j]-1,i);
		
		mat->set_entry( rows-lines[end]-1, i, total?-1:0 );
	}
	
	mat->print(2);
	
	set_matrix(mat,false);
	delete(mat);
}

integer_map pollyMap::get_integer_map() const { return Map; }

bool pollyMap::is_empty() const { integer_map_is_empty(Map); }

bool pollyMap::is_valid() const { return Map!=NULL; }

integer_set pollyMap::get_map_domain() const
{
	return integer_map_get_domain(copy_integer_map(Map));
}

bool pollyMap::satisfaction_test( int level, pollyMap * src, pollyMap * dest )
{	
	return order_satisfaction_test(level,Map,src->get_integer_map(),dest->get_integer_map());
}

void pollyMap::intersect_domain( pollyDomain * dom ) 
{
	Map = integer_map_intersect_domain(Map,dom->get_integer_set());
}

void pollyMap::intersect_range( pollyDomain * dom )
{
	Map = integer_map_intersect_range(Map,dom->get_integer_set());
}

void pollyMap::lexmax() 
{
	Map = integer_map_lexmax(Map);
}

void pollyMap::add_conflict_constraint( pollyMap * lhs, pollyMap * rhs )
{
	Map = integer_map_add_conflict_cst(Map, lhs->get_integer_map(), rhs->get_integer_map() );
}

void pollyMap::add_dim( dim_type t, string var )
{
	switch (t)
	{
		case dim_in:
			in_vars.push_back(var);
			Map = integer_set_add_dim(Map,var,t);
			return;
		case dim_out:
			out_vars.push_back(var);
			Map = integer_set_add_dim(Map,var,t);
			return;
		case dim_param:
			params.push_back(var);
			Map = integer_set_add_dim(Map,var,t);
			return;
	}
}

void pollyMap::print_matrix( int ident, bool ineq ) const
{
	cout<<setw(ident+1);
	for ( int i = 0 ; i < in_vars.size() ; i++ ) {
		cout<<in_vars[i];
		cout<<setw(6);
	}
	for ( int i = 0 ; i < params.size() ; i++ ) {
		cout<<params[i];
		cout<<setw(6);
	}
	cout<<"C"<<endl;
	
	cout<<setw(ident+1);
	for ( int i = 0 ; i < in_vars.size() ; i++ )
		for ( int j = 0 ; j < 6 ; j++ )
			cout<<"-";
	for ( int i = 0 ; i < params.size() ; i++ )
		for ( int j = 0 ; j < 6 ; j++ )
			cout<<"-";
	cout<<"-"<<endl;
	
	simple_matrix * mat = get_matrix(ineq);
	mat->print(ident);
	delete(mat);
	cout<<endl;
}

void pollyMap::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	if (Map)
		print_integer_map(Map);
	else
		cout<<"Map empty.";
	cout<<endl;
}

pollyMap::~pollyMap() { free_integer_map(Map); }






