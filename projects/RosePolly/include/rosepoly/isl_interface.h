

#ifndef ISL_INTERFACE_H
#define ISL_INTERFACE_H

class simple_matrix;

typedef struct visitor {
	int pos;
	void * user;
	int index;
	bool dimBased;
	
	visitor( int p, bool d=false );
	
	void reset( int p ) { pos = p; index = 0; }
	
} Visitor;

typedef enum {
	dim_in,
	dim_out,
	dim_param
} dim_type;

isl_set * isl_interface_build_integer_set( vector<string> params, isl_ctx * ctx );

isl_set * isl_interface_integer_set_add_loop( isl_set * set, string start, string end, string sym );

void isl_interface_print_integer_set( isl_set * set );

void isl_interface_print_integer_set( isl_basic_set * set );

void isl_interface_print_integer_map( isl_map * map );

void isl_interface_print_integer_basic_map( isl_basic_map * map );

isl_map * isl_interface_integer_map_undo( isl_map * map );

void isl_interface_print_constraint( isl_constraint * cst );

void isl_interface_integer_set_free( isl_set * set );

void isl_interface_integer_set_free( isl_basic_set * set );

CloogDomain * isl_interface_integer_set_get_cloog_domain( isl_set * set );

CloogScattering * isl_interface_integer_map_get_cloog_scatter( isl_map * map );

isl_set * isl_interface_append_inequalities( isl_set * set, simple_matrix * mat );

isl_map * isl_interface_append_inequalities( isl_map * map, simple_matrix * mat );

isl_set * isl_interface_integer_set_add_dim( isl_set * set, string u, dim_type t );

isl_map * isl_interface_integer_set_add_dim( isl_map * map, string u, dim_type t );

isl_constraint * get_equality_cst( int pos, isl_map * map );

isl_set * isl_interface_build_universal_set( isl_ctx * ctx );

int isl_interface_integer_map_get_entry( __isl_keep isl_map * map, int i, int j );

isl_map * isl_interface_build_universal_schedule( isl_set * set );

isl_set * isl_interface_integer_set_add_condition( isl_set * set, SgExpression * exp, bool path );

string isl_interface_unparse_op( VariantT varT, bool path );

string isl_interface_unparse_exp( SgExpression * exp, bool path );

simple_matrix * isl_interface_build_matrix( isl_map * map, bool ineq );

simple_matrix * isl_interface_build_matrix( isl_set * set, bool ineq );

isl_map * isl_interface_integer_map_append_mapping( isl_map * map, int * coeffs, int width );

isl_set * isl_interface_build_integer_set_from_matrix( isl_set * set, simple_matrix * mat, bool ineq );

isl_map * isl_interface_build_integer_set_from_matrix( isl_map * map, simple_matrix * mat, bool ineq );

isl_set * isl_interface_integer_set_set_name( isl_set * set, string name );

isl_map * isl_interface_build_access_map( isl_set * set, vector<SgExpression*>& subs );

vector<isl_map*> isl_interface_gather_disjoint_maps( isl_map * map );

isl_set * isl_interface_build_integer_set_from_cst_matrices( isl_set * set, simple_matrix * ineq, simple_matrix * eq );

isl_map * isl_interface_build_integer_set_from_cst_matrices( isl_map * map, simple_matrix * ineq, simple_matrix * eq );

isl_map * isl_interface_build_irregular_access_map( isl_set * set, vector<SgExpression*>& subs );

isl_map * isl_interface_build_schedule_map( isl_set * set, vector<int>& pos );

isl_map * isl_interface_build_map_with_order_csts( isl_set * src, isl_set * dest, int * llv );

isl_map * isl_interface_integer_map_intersect_domain( isl_map * map, isl_set * set );

isl_map * isl_interface_integer_map_intersect_range( isl_map * map, isl_set * set );

isl_map * isl_interface_integer_map_add_conflict_cst( isl_map * map, isl_map * lhs, isl_map * rhs );

bool isl_interface_order_satisfaction_test( int level, isl_map * map, isl_map * src, isl_map * dest );

#endif

