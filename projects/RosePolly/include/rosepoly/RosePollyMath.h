
#ifndef POLLY_MATH_H
#define POLLY_MATH_H

#include <cloog/cloog.h>
#include <cloog/isl/cloog.h>

#ifdef ISL
#include <isl/set.h>
#include <isl/map.h>

#include <rosepoly/isl_interface.h>

	typedef isl_set * integer_set;
	typedef isl_map * integer_map;
	typedef isl_ctx * polly_context;

	#define init_polly_context() isl_ctx_alloc()
	#define free_polly_context(ctx) isl_ctx_free(ctx)
    #define build_integer_set(p,c) isl_interface_build_integer_set(p,c)
	#define copy_integer_set(s) isl_set_copy(s)
	#define copy_integer_map(m) isl_map_copy(m)
	#define integer_set_add_loop(set,start,end,sym) isl_interface_integer_set_add_loop(set,start,end,sym)
	#define print_integer_set(set) isl_interface_print_integer_set(set)
	#define integer_set_add_condition(set,exp,path) isl_interface_integer_set_add_condition(set,exp,path)
	#define free_integer_set(set) isl_interface_integer_set_free(set)
	#define integer_set_set_name(set,name) isl_interface_integer_set_set_name(set,name)
	#define print_integer_map(map) isl_interface_print_integer_map(map)
	#define free_integer_map(map) isl_map_free(map)
	#define print_constraint(cst) isl_interface_print_constraint(cst)

#ifdef MPI_EXPS
	#define build_access_map(d,s) isl_interface_build_irregular_access_map(d,s)
#endif

#ifndef MPI_EXPS
	#define build_access_map(d,s) isl_interface_build_access_map(d,s)
#endif

	#define build_schedule_map(d,p) isl_interface_build_schedule_map(d,p)
	#define integer_set_is_empty(s)	isl_set_fast_is_empty(s)
	#define integer_map_is_empty(m) isl_map_fast_is_empty(m)
	#define build_map_with_order_csts(s,d,l) isl_interface_build_map_with_order_csts(s,d,l)
	#define integer_map_intersect_domain(m,s) isl_interface_integer_map_intersect_domain(m,s)
	#define integer_map_intersect_range(m,s) isl_interface_integer_map_intersect_range(m,s)
	#define integer_map_add_conflict_cst(m,l,r) isl_interface_integer_map_add_conflict_cst(m,l,r)
	#define integer_map_lexmax(m) isl_map_lexmax(m)
	#define integer_set_subtract_domains(d1,d2) isl_set_subtract(d1,d2)
	#define integer_map_get_domain(m) isl_map_domain(m)
	#define build_universal_schedule(s) isl_interface_build_universal_schedule(s)
	#define order_satisfaction_test(l,m,s,d) isl_interface_order_satisfaction_test(l,m,s,d)
	#define build_universal_set(c) isl_interface_build_universal_set(c)
	#define integer_set_add_dim(s,u,t) isl_interface_integer_set_add_dim(s,u,t)
	#define build_integer_set_matrix(s,i) isl_interface_build_matrix(s,i)
	#define gather_disjoint_maps(m) isl_interface_gather_disjoint_maps(m)
	#define integer_set_append_inequalities(s,m) isl_interface_append_inequalities(s,m)
	#define integer_set_lexmin(s) isl_set_lexmin(s)
	#define build_integer_set_from_matrix(s,m,i) isl_interface_build_integer_set_from_matrix(s,m,i)
	#define integer_map_append_mapping(m,c,s) isl_interface_integer_map_append_mapping(m,c,s)
	#define integer_map_get_entry(m,i,j) isl_interface_integer_map_get_entry(m,i,j)
	#define build_integer_set_from_cst_matrices(s,i,e) isl_interface_build_integer_set_from_cst_matrices(s,i,e)
	#define integer_map_undo(m) isl_interface_integer_map_undo(m)

	#define integer_set_get_cloog_domain(s) isl_interface_integer_set_get_cloog_domain(s)
	#define integer_map_get_cloog_scatter(m) isl_interface_integer_map_get_cloog_scatter(m)

#endif

class simple_matrix;

class RosePollyBase
{
	
protected:
	
	static int num_childs;
	
	vector<string> in_vars;
	vector<string> out_vars;
	vector<string> params;
	
	static void init_context();
	static void free_context();
		
public:
	
	static polly_context context;
	
	RosePollyBase();
	RosePollyBase( vector<string> u, vector<string> p );
	RosePollyBase( vector<string> p );
	
	virtual ~RosePollyBase();
		
	vector<string> get_dims( dim_type t ) const;
	int get_total_size() const;
	int get_dim_size( dim_type t ) const;
	
	virtual void print( int ident ) const=0;
};

class pollyDomain : public RosePollyBase
{
	integer_set Set;
	
public:
	
	pollyDomain();
	pollyDomain( const pollyDomain& p );
	pollyDomain( vector<string> p );
	
	virtual ~pollyDomain();
	
	bool is_empty() const;
	integer_set get_integer_set() const;
	
	simple_matrix * get_matrix( bool ineq ) const;
	void set_matrix( simple_matrix * mat, bool ineq );
	void set_constraints( simple_matrix * ineq, simple_matrix * eq );
	
	CloogDomain * get_cloog_domain() const;
	
	void add_loop( ForLoop * loop );
	void add_conditional( SgExpression * exp, bool path );
	void set_name( string name );
	void subtract( integer_set set );
	void simplify();
	void append_inequalities( simple_matrix * mat );
	void lexmin( bool non_neg );
	
	void add_dim( dim_type t, string var );
	
	virtual void print( int ident ) const;
	void print_matrix( int ident, bool ineq ) const;
};

class pollyMap : public RosePollyBase
{	
	integer_map Map;
	
public:
	
	pollyMap( const pollyMap& m );
	pollyMap( const pollyMap& m, integer_map map ); // this constructor is used to instantiate disjoint maps
	pollyMap( const pollyDomain& d );
	pollyMap( pollyDomain * d, vector<SgExpression*>& s );
	pollyMap( pollyDomain * d, vector<int> pos );  // this constructor is used to generate the sequential schedule
	pollyMap( pollyDomain * src, pollyDomain * dest, int * llv ); // create a pollyMap object with order constraints
	
	virtual ~pollyMap();
	
	bool is_empty() const;
	bool is_valid() const;
	integer_map get_integer_map() const;
	integer_set get_map_domain() const;
	
	simple_matrix * get_matrix( bool ineq ) const;
	
	vector<pollyMap*> get_disjoints() const;
	int get_entry( int dim, int pos ) const;
	
	CloogScattering * get_cloog_scatter() const;
	
	void add_hyperplane( int * coeffs, int size );
	bool satisfaction_test( int level, pollyMap * src, pollyMap * dest );
	
	void intersect_domain( pollyDomain * dom );
	void intersect_range( pollyDomain * dom );
	void add_conflict_constraint( pollyMap * lhs, pollyMap * rhs );
	void lexmax();
	void set_matrix( simple_matrix * mat, bool ineq );
	void set_constraints( simple_matrix * ineq, simple_matrix * eq );
	void append_inequalities( simple_matrix * mat );

	void skew( vector<int> lines, int width, bool top );
	void undo();
	
	void add_dim( dim_type t, string var );
	
	virtual void print( int ident ) const;
	void print_matrix( int ident, bool ineq ) const;
};

#endif

