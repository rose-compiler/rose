
#include <rose.h>

using namespace std;

#include <cloog/isl/cloog.h>
#include <isl/set.h>
#include <isl/map.h>

#include <rosepoly/isl_interface.h>
#include <rosepoly/simple_matrix.h>

Visitor::visitor( int p, bool d ) : pos(p), index(0), user(NULL), dimBased(d) {}

isl_set * isl_interface_build_integer_set( vector<string> params, isl_ctx * ctx )
{
	stringstream out;
	out<<"[";
	int Size = params.size();
	for ( int i = 0 ; i < Size-1 ; i++ )
		out<<params[i]<<",";
	out<<params[Size-1]<<"]->{[]:}";
	
	return isl_set_read_from_str(ctx,out.str().c_str());
}

isl_set * isl_interface_integer_set_add_loop( isl_set * set, string start, string end, string sym )
{
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	set = isl_set_add_dims(set,isl_dim_set,1);
	unsigned int pos = isl_set_dim(set,isl_dim_set)-1;
	const char * c = sym.c_str();
	set = isl_set_set_dim_name(set,isl_dim_set,pos,c);
	
	stringstream out;
	out<<"[";
	int pSize = isl_set_dim(set,isl_dim_param);
	for ( int i = 0 ; i < pSize-1 ; i++ )
		out<<isl_set_get_dim_name(set,isl_dim_param,i)<<",";
	out<<isl_set_get_dim_name(set,isl_dim_param,pSize-1)<<"]->{[";
	
	int uSize = isl_set_dim(set,isl_dim_set);
	for ( int i = 0 ; i < uSize-1 ; i++ )
		out<<isl_set_get_dim_name(set,isl_dim_set,i)<<",";
	out<<isl_set_get_dim_name(set,isl_dim_set,uSize-1)<<"] : ";
	
	out<<sym<<">="<<start<<" and ";
	out<<sym<<"<="<<end<<"}";
		
	isl_set * temp = isl_set_read_from_str(ctx,out.str().c_str());
	
	return isl_set_intersect(set,temp);
}

void isl_interface_print_integer_set( isl_set * set )
{
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	isl_printer * printer = isl_printer_to_file(ctx,stdout);
	
	isl_printer_print_set(printer,set);
	isl_printer_free(printer);
}

int fCst( __isl_take isl_constraint * c, void * user )
{
	visitor * v = (visitor*)user;
	if ( isl_constraint_is_equality(c) ) {
		if ( v->dimBased ) {
			int dim_in = isl_constraint_dim(c,isl_dim_in);
			if ( isl_constraint_involves_dims(c,isl_dim_in,0,dim_in) ) {
				if ( v->pos == v->index )
					v->user = (void*)c;
				else
					isl_constraint_free(c);
				v->index++;
			} else {
				isl_constraint_free(c);
			}
		} else {
			if ( v->pos == v->index )
				v->user = (void*)c;
			else
				isl_constraint_free(c);
			v->index++;
		}
	} else {
		isl_constraint_free(c);
	}
	return 0;
}

int fMap( __isl_take isl_basic_map * bmap, void * user )
{
	isl_basic_map_foreach_constraint(bmap,fCst,user);
	isl_basic_map_free(bmap);
	
	return 0;
}

int f_gather( __isl_take isl_basic_map * bmap, void * user )
{
	vector<isl_map*> * total = (vector<isl_map*>*)user;
	total->push_back(isl_map_from_basic_map(bmap));
	
	return 0;
}

vector<isl_map*> isl_interface_gather_disjoint_maps( isl_map * map )
{
	vector<isl_map*> total;
	
	isl_map_foreach_basic_map(map,f_gather,(void*)(&total));
	
	return total;
}

isl_constraint * get_equality_cst( int pos, isl_map * map )
{
	visitor visit_t(pos);
	
	isl_map_foreach_basic_map(map,fMap,(void*)(&visit_t));
	
	return (isl_constraint*)visit_t.user;
}

int f_basic_map( __isl_take isl_basic_map * bmap, void * user )
{
	Visitor * V = (Visitor*)user;
	V->user = (void*)bmap;
	return 0;
}

int f_basic_set( __isl_take isl_basic_set * bSet, void * user )
{
	Visitor * V = (Visitor*)user;
	V->user = (void*)bSet;
	return 0;
}

simple_matrix * isl_interface_build_matrix( isl_map * map, bool ineq )
{
	isl_set * set = isl_map_domain(isl_map_copy(map));
		
	if ( isl_set_n_basic_set(set) > 1 ) {
		isl_set_free(set);
		return NULL;
	}
	
	Visitor v(0);
	isl_mat * temp = NULL;
	
	isl_map_foreach_basic_map(map,f_basic_map,(void*)&v);
	isl_basic_map * bMap = (isl_basic_map*)v.user;
	int cols;
	
	if ( ineq ) {
		temp = isl_basic_map_inequalities_matrix(bMap,isl_dim_in,isl_dim_param,isl_dim_cst,isl_dim_div,isl_dim_out);
		cols = isl_mat_cols(temp) - isl_map_dim(map,isl_dim_out);
	} else {
		temp = isl_basic_map_equalities_matrix(bMap,isl_dim_in,isl_dim_out,isl_dim_param,isl_dim_cst,isl_dim_div);
		cols = isl_mat_cols(temp);
	}
		
	int rows = isl_mat_rows(temp);
	
	simple_matrix * matrix = new simple_matrix( rows, cols );
	
	isl_int value;
	isl_int_init(value);
	for ( int i = 0 ; i < rows ; i++ ) {
		for ( int j = 0 ; j < cols ; j++ ) {
			isl_mat_get_element(temp,i,j,&value);
			matrix->set_entry(i,j,isl_int_get_si(value));
		}
	}
	isl_int_clear(value);
	
	isl_basic_map_free(bMap);
	isl_set_free(set);
	isl_mat_free(temp); 
			
	return matrix;
}

simple_matrix * isl_interface_build_matrix( isl_set * set, bool ineq )
{
	
	if ( isl_set_n_basic_set(set) > 1 )
		return NULL;
	
	Visitor v(0);
	isl_mat * temp = NULL;
	
	isl_set_foreach_basic_set(set,f_basic_set,(void*)&v);
	isl_basic_set * bSet = (isl_basic_set*)v.user;
	
	if ( ineq )
		temp = isl_basic_set_inequalities_matrix(bSet,isl_dim_set,isl_dim_param,isl_dim_cst,isl_dim_div);
	else
		temp = isl_basic_set_equalities_matrix(bSet,isl_dim_set,isl_dim_param,isl_dim_cst,isl_dim_div);
		
	int rows = isl_mat_rows(temp);
	int cols = isl_mat_cols(temp);
	
	simple_matrix * matrix = new simple_matrix( rows, cols );
	
	isl_int value;
	isl_int_init(value);
	for ( int i = 0 ; i < rows ; i++ ) {
		for ( int j = 0 ; j < cols ; j++ ) {
			isl_mat_get_element(temp,i,j,&value);
			matrix->set_entry(i,j,isl_int_get_si(value));
		}
	}
	isl_int_clear(value);
	
	isl_basic_set_free(bSet);
	isl_mat_free(temp); 
	
	return matrix;
}

isl_set * isl_interface_append_inequalities( isl_set * set, simple_matrix * mat )
{
	if (!mat||!mat->get_rows())
		return set;
	
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	isl_mat * i_mat = isl_mat_alloc(ctx, mat->get_rows(), mat->get_columns());
	isl_mat * e_mat = isl_mat_alloc(ctx,0,mat->get_columns());
	
	for ( int i = 0 ; i < mat->get_rows() ; i++ ) {
		for ( int j = 0 ; j < mat->get_columns() ; j++ ) {
			i_mat = isl_mat_set_element_si(i_mat,i,j,mat->entry(i,j));
		}
	} 
	isl_basic_set * bset = isl_basic_set_from_constraint_matrices(
																  isl_set_get_space(set),
																  e_mat, i_mat, isl_dim_set,
																  isl_dim_param,isl_dim_cst,isl_dim_div);
	return isl_set_intersect(set,isl_set_from_basic_set(bset));
}

isl_map * isl_interface_append_inequalities( isl_map * map, simple_matrix * mat )
{
	if (!mat||!mat->get_rows())
		return map;
	
	isl_ctx * ctx = isl_map_get_ctx(map);
	
	isl_mat * i_mat = isl_mat_alloc(ctx, mat->get_rows(), mat->get_columns());
	isl_mat * e_mat = isl_mat_alloc(ctx,0,mat->get_columns());
	
	for ( int i = 0 ; i < mat->get_rows() ; i++ ) {
		for ( int j = 0 ; j < mat->get_columns() ; j++ ) {
			i_mat = isl_mat_set_element_si(i_mat,i,j,mat->entry(i,j));
		}
	} 
	isl_basic_map * bmap = isl_basic_map_from_constraint_matrices(
																  isl_map_get_space(map),
																  e_mat, i_mat, isl_dim_set, isl_dim_out,
																  isl_dim_param,isl_dim_cst,isl_dim_div);
	return isl_map_intersect(map,isl_map_from_basic_map(bmap));
}

bool isl_interface_order_satisfaction_test( int level, isl_map * map, isl_map * src, isl_map * dest )
{
	isl_ctx * ctx = isl_map_get_ctx(map);
	
	isl_constraint * src_cst;
	isl_constraint * dest_cst;
	isl_constraint * dep_cst;
	isl_constraint * core_cst;
	isl_set * total;
	int src_nestL = isl_map_dim(src,isl_dim_out);
	int dest_nestL = isl_map_dim(dest,isl_dim_out);
	isl_space * space = isl_space_domain(isl_map_get_space(map));
	
	src_cst = get_equality_cst(src_nestL-level-1,src);
		
	dest_cst = get_equality_cst(dest_nestL-level-1,dest);
	
	core_cst = isl_inequality_alloc(isl_local_space_from_space(isl_space_copy(space)));
	int src_dim = isl_map_dim(map,isl_dim_out);
	int dest_dim = isl_map_dim(map,isl_dim_in);
	isl_int value;
	isl_int coeff;
	isl_int_init(value);
	isl_int_init(coeff);
	for ( int i = 0 ; i < src_dim ; i++ ) {
		dep_cst = get_equality_cst(src_dim-i-1,map);
		
		isl_int mult;
		isl_int_init(mult);
		isl_constraint_get_coefficient(src_cst,isl_dim_in,i,&mult);
		for ( int j = 0 ; j < dest_dim ; j++ ) {
			isl_constraint_get_coefficient(dep_cst,isl_dim_in,j,&coeff);
			isl_int_mul(value,mult,coeff);
			isl_constraint_get_coefficient(core_cst,isl_dim_set,j,&coeff);
			isl_int_add(value,coeff,value);
			isl_constraint_set_coefficient(core_cst,isl_dim_set,j,value);
		}
		isl_constraint_get_constant(dep_cst,&value);
		isl_int_mul(value,mult,value);
		isl_constraint_get_constant(core_cst,&coeff);
		isl_int_add(value,coeff,value);
		isl_constraint_set_constant(core_cst,value);
		
		isl_constraint_free(dep_cst);
		isl_int_clear(mult);
	}
	
	isl_constraint_get_constant(src_cst,&value);
	isl_constraint_get_constant(core_cst,&coeff);
	isl_int_neg(value,value);
	isl_int_add(value,coeff,value);
	isl_constraint_set_constant(core_cst,value);
	
	for ( int i = 0 ; i < dest_dim ; i++ ) {
		isl_constraint_get_coefficient(dest_cst,isl_dim_in,i,&value);
		isl_constraint_get_coefficient(core_cst,isl_dim_set,i,&coeff);
		isl_int_add(value,coeff,value);
		isl_constraint_set_coefficient(core_cst,isl_dim_set,i,value);
	}
	isl_constraint_get_constant(dest_cst,&value);
	isl_constraint_get_constant(core_cst,&coeff);
	isl_int_add(value,coeff,value);
	isl_constraint_set_constant(core_cst,value);
	
	isl_constraint_free(src_cst);
	isl_constraint_free(dest_cst);
	
	total = isl_set_universe(space);
	total = isl_set_add_constraint(total,core_cst);
	total = isl_set_intersect(total,isl_map_domain(isl_map_copy(map)));
	
	isl_int_clear(value);
	isl_int_clear(coeff);
	
	if ( isl_set_is_empty(total) ) {
		isl_set_free(total);
		return true;
	} else {
		isl_set_free(total);
		return false;
	}
}

isl_map * isl_interface_build_universal_schedule( isl_set * set )
{
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	int nParams = isl_set_dim(set,isl_dim_param);
	int dims = isl_set_dim(set,isl_dim_set);
	
	isl_space * space = isl_space_alloc(ctx,nParams,dims,0);
	for ( int i = 0 ; i < nParams ; i++ ) {
		const char * p = isl_set_get_dim_name(set,isl_dim_param,i);
		isl_space_set_dim_name(space,isl_dim_param,i,p);
	}
	for ( int i = 0 ; i < dims ; i++ ) {
		const char * u = isl_set_get_dim_name(set,isl_dim_set,i);
		isl_space_set_dim_name(space,isl_dim_in,i,u);
	}
	
	return isl_map_universe(space);
}

isl_set * isl_interface_build_universal_set( isl_ctx * ctx )
{
	return isl_set_universe(isl_space_set_alloc(ctx,0,0));
}

isl_set * isl_interface_integer_set_add_dim( isl_set * set, string u, dim_type t )
{
	isl_dim_type d_type;
	switch(t)
	{
		case dim_in:
			d_type = isl_dim_set;
			break;
		case dim_out:
			d_type = isl_dim_out;
			break;
		case dim_param:
			d_type = isl_dim_param;
			break;
	}
	set = isl_set_add_dims(set,d_type,1);
	int dim = isl_set_dim(set,d_type);
	return isl_set_set_dim_name(set,d_type,dim-1,u.c_str());
}

isl_map * isl_interface_integer_set_add_dim( isl_map * map, string u, dim_type t )
{
	isl_dim_type d_type;
	switch(t)
	{
		case dim_in:
			d_type = isl_dim_set;
			break;
		case dim_out:
			d_type = isl_dim_out;
			break;
		case dim_param:
			d_type = isl_dim_param;
			break;
	}
	
	map = isl_map_add_dims(map,d_type,1);
	int dim = isl_map_dim(map,d_type);
	return isl_map_set_dim_name(map,d_type,dim-1,u.c_str());
}

isl_map * isl_interface_build_integer_set_from_matrix( isl_map * map, simple_matrix * mat, bool ineq )
{
	if ( !map )
		return NULL;
	
	isl_ctx * ctx = isl_map_get_ctx(map);
	
	isl_space * space = isl_map_get_space(map);
	
	isl_map_free(map);
	
	int rows = mat->get_rows();
	int cols = mat->get_columns();
	
	isl_mat * i_mat = isl_mat_alloc(ctx,0,cols);
	isl_mat * e_mat = isl_mat_alloc(ctx,rows,cols);
	
	for ( int i = 0 ; i < rows ; i++ ) {
		for ( int j = 0 ; j < cols ; j++ ) {
			if ( ineq )
				i_mat = isl_mat_set_element_si(i_mat,i,j,mat->entry(i,j));
			else
				e_mat = isl_mat_set_element_si(e_mat,i,j,mat->entry(i,j));
		}
	}
	
	isl_basic_map * bmap = isl_basic_map_from_constraint_matrices(
																  space,
																  e_mat, i_mat, 
																  isl_dim_in,isl_dim_out,isl_dim_param,
																  isl_dim_cst,isl_dim_div);
	
	return isl_map_from_basic_map(bmap);
}


isl_set * isl_interface_build_integer_set_from_matrix( isl_set * set, simple_matrix * mat, bool ineq )
{
	if ( !set )
		return NULL;
	
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	isl_space * space = isl_set_get_space(set);
	
	isl_set_free(set);
	
	int rows = mat->get_rows();
	int cols = mat->get_columns();
	
	isl_mat * i_mat = isl_mat_alloc(ctx,ineq?rows:0,cols);
	isl_mat * e_mat = isl_mat_alloc(ctx,ineq?0:rows,cols);
	
	for ( int i = 0 ; i < rows ; i++ ) {
		for ( int j = 0 ; j < cols ; j++ ) {
			if ( ineq )
				i_mat = isl_mat_set_element_si(i_mat,i,j,mat->entry(i,j));
			else
				e_mat = isl_mat_set_element_si(e_mat,i,j,mat->entry(i,j));
		}
	}
	
	isl_basic_set * bset = isl_basic_set_from_constraint_matrices(
																  space,
																  e_mat, i_mat, isl_dim_set,
																  isl_dim_param,isl_dim_cst,isl_dim_div);
	
	return isl_set_from_basic_set(bset);
}

isl_set * isl_interface_build_integer_set_from_cst_matrices( isl_set * set, simple_matrix * ineq, simple_matrix * eq )
{
	if ( !set )
		return NULL;
	
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	isl_space * space = isl_set_get_space(set);
	
	isl_set_free(set);
	
	int i_rows = ineq->get_rows();
	int i_cols = ineq->get_columns();
	int e_rows = eq->get_rows();
	int e_cols = eq->get_columns();
	
	isl_mat * i_mat = isl_mat_alloc(ctx,i_rows,i_cols);
	isl_mat * e_mat = isl_mat_alloc(ctx,e_rows,e_cols);
	
	for ( int i = 0 ; i < i_rows ; i++ )
		for ( int j = 0 ; j < i_cols ; j++ )
			i_mat = isl_mat_set_element_si(i_mat,i,j,ineq->entry(i,j));
	
	for ( int i = 0 ; i < e_rows ; i++ )
		for ( int j = 0 ; j < e_cols ; j++ )
			e_mat = isl_mat_set_element_si(e_mat,i,j,eq->entry(i,j));
	
	isl_basic_set * bset = isl_basic_set_from_constraint_matrices(
																  space,
																  e_mat, i_mat, isl_dim_set,
																  isl_dim_param,isl_dim_cst,isl_dim_div);
	
	return isl_set_from_basic_set(bset);
}

isl_map * isl_interface_build_integer_set_from_cst_matrices( isl_map * map, simple_matrix * ineq, simple_matrix * eq )
{
	if ( !map )
		return NULL;
	
	isl_ctx * ctx = isl_map_get_ctx(map);
	
	isl_space * space = isl_map_get_space(map);
	
	isl_map_free(map);
	
	int i_rows = ineq->get_rows();
	int i_cols = ineq->get_columns();
	int e_rows = eq->get_rows();
	int e_cols = eq->get_columns();
	
	isl_mat * i_mat = isl_mat_alloc(ctx,i_rows,i_cols);
	isl_mat * e_mat = isl_mat_alloc(ctx,e_rows,e_cols);
	
	for ( int i = 0 ; i < i_rows ; i++ )
		for ( int j = 0 ; j < i_cols ; j++ )
			i_mat = isl_mat_set_element_si(i_mat,i,j,ineq->entry(i,j));
	
	for ( int i = 0 ; i < e_rows ; i++ )
		for ( int j = 0 ; j < e_cols ; j++ )
			e_mat = isl_mat_set_element_si(e_mat,i,j,eq->entry(i,j));
	
	isl_basic_map * bmap = isl_basic_map_from_constraint_matrices(
																  space,
																  e_mat, i_mat, isl_dim_set,isl_dim_out,
																  isl_dim_param,isl_dim_cst,isl_dim_div);
	
	return isl_map_from_basic_map(bmap);
}

isl_map * isl_interface_integer_map_append_mapping( isl_map * map, int * coeffs, int width )
{
	map = isl_map_add_dims(map,isl_dim_out,1);
	int n_out = isl_map_dim(map,isl_dim_out);
	int n_in = isl_map_dim(map,isl_dim_in);
	stringstream out;
	out<<"t"<<n_out-1;
	isl_map_set_dim_name(map,isl_dim_out,n_out-1,out.str().c_str());
	isl_space * space = isl_map_get_space(map);
	isl_local_space * ls = isl_local_space_from_space(space);
	isl_constraint * c = isl_equality_alloc(ls);
	
	assert(n_in==width);

	for ( int i = 0 ; i < width ; i++ )
		c = isl_constraint_set_coefficient_si(c,isl_dim_in,i,-coeffs[i]);
	
	c = isl_constraint_set_coefficient_si(c,isl_dim_out,n_out-1,1);
	c = isl_constraint_set_constant_si(c,-coeffs[width]);
	
	return isl_map_add_constraint(map,c);
}

isl_map * isl_interface_integer_map_undo( isl_map * map )
{
	int n_out = isl_map_dim(map,isl_dim_out);
	return isl_map_remove_dims(map,isl_dim_out,n_out-1,1);
}

int isl_interface_integer_map_get_entry( __isl_keep isl_map * map, int i, int j )
{
	Visitor v(i,true);
	
	isl_map_foreach_basic_map(map,fMap,(void*)(&v));
	isl_constraint * cst = (isl_constraint*)v.user;
	
	isl_int value;
	isl_int_init(value);
	int n_in = isl_map_dim(map,isl_dim_in);
	assert(j<=n_in);
	isl_constraint_get_coefficient(cst,isl_dim_in,j,&value);
	
	isl_int_neg(value,value);
	
	isl_constraint_free(cst);
	int final = isl_int_get_si(value);
	isl_int_clear(value);
	return final;
}

void isl_interface_integer_set_free( isl_set * set ) { isl_set_free(set); }

void isl_interface_integer_set_free( isl_basic_set * set ) { isl_basic_set_free(set); }

void isl_interface_print_integer_set( isl_basic_set * set )
{
	isl_ctx * ctx = isl_basic_set_get_ctx(set);
	isl_printer * printer = isl_printer_to_file(ctx,stdout);
	
	isl_printer_print_basic_set(printer,set);
	isl_printer_free(printer);
}

void isl_interface_print_integer_map( isl_map * map )
{
	isl_ctx * ctx = isl_map_get_ctx(map);
	
	isl_printer * printer = isl_printer_to_file(ctx,stdout);
	
	isl_printer_print_map(printer,map);
	isl_printer_free(printer);
}

CloogDomain * isl_interface_integer_set_get_cloog_domain( isl_set * set )
{
	return cloog_domain_from_isl_set(isl_set_copy(set));
}

CloogScattering * isl_interface_integer_map_get_cloog_scatter( isl_map * map )
{
	return cloog_scattering_from_isl_map(isl_map_copy(map));
}

void isl_interface_print_integer_basic_map( isl_basic_map * map )
{
	isl_ctx * ctx = isl_basic_map_get_ctx(map);
	
	isl_printer * printer = isl_printer_to_file(ctx,stdout);
	
	isl_printer_print_basic_map(printer,map);
	isl_printer_free(printer);
}

void isl_interface_print_constraint( isl_constraint * cst )
{
	isl_ctx * ctx = isl_constraint_get_ctx(cst);
	
	isl_printer * printer = isl_printer_to_file(ctx,stdout);
	
	isl_printer_print_constraint(printer,cst);
	isl_printer_free(printer);
}

isl_set * isl_interface_integer_set_add_condition( isl_set * set, SgExpression * exp, bool path )
{
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	VariantT varT = exp->variantT();
	stringstream cond;
	
	SgBinaryOp * binOp = isSgBinaryOp(exp);
	if (!binOp)
		ROSE_ASSERT(false);		// This should never happen. Nevertheless, if it does I need to handle it
	
	SgExpression * lhs = binOp->get_lhs_operand();
	SgExpression * rhs = binOp->get_rhs_operand();
	
	cond<<"("<<isl_interface_unparse_exp(lhs,path)<<")";
	
	cond<<isl_interface_unparse_op(varT,path);
	
	cond<<"("<<isl_interface_unparse_exp(rhs,path)<<")";
	
	stringstream main;
	main<<"[";
	int pSize = isl_set_dim(set,isl_dim_param);
	for ( int i = 0 ; i < pSize-1 ; i++ )
		main<<isl_set_get_dim_name(set,isl_dim_param,i)<<",";
	main<<isl_set_get_dim_name(set,isl_dim_param,pSize-1)<<"]->{[";
	
	int uSize = isl_set_dim(set,isl_dim_set);
	for ( int i = 0 ; i < uSize-1 ; i++ )
		main<<isl_set_get_dim_name(set,isl_dim_set,i)<<",";
	main<<isl_set_get_dim_name(set,isl_dim_set,uSize-1)<<"] : ";
	
	main<<cond.str().c_str()<<"}";
		
	isl_set * temp = isl_set_read_from_str(ctx,main.str().c_str());
	
	return isl_set_intersect(set,temp);
}

string isl_interface_unparse_exp( SgExpression * exp, bool path )
{
	SgBinaryOp * binOp = isSgBinaryOp(exp);
	stringstream out;
	
	if (binOp) {
		
		VariantT varT = binOp->variantT();
		
		SgExpression * lhs = binOp->get_lhs_operand();
		SgExpression * rhs = binOp->get_rhs_operand();
		
		out<<"("<<isl_interface_unparse_exp(lhs,path)<<")";
		
		out<<isl_interface_unparse_op(varT,path);
		
		out<<"("<<isl_interface_unparse_exp(rhs,path)<<")";
		
	} else {
		out<<exp->unparseToString();
	}
	
	return out.str();
}

string isl_interface_unparse_op( VariantT varT, bool path )
{
	stringstream out;
	
	switch (varT)
	{
		case V_SgGreaterThanOp:
			if (path)
				out<<">";
			else
				out<<"<=";
			break;
		case V_SgLessThanOp:
			if (path)
				out<<"<";
			else
				out<<">=";
			break;
		case V_SgGreaterOrEqualOp:
			if (path)
				out<<">=";
			else
				out<<"<";
			break;
		case V_SgLessOrEqualOp:
			if (path)
				out<<"<=";
			else
				out<<">";
			break;
		case V_SgAndOp:
			if (path)
				out<<"and";
			else
				out<<"or";
			break;
		case V_SgOrOp:
			if (path)
				out<<"or";
			else
				out<<"and";
			break;
		default:
			ROSE_ASSERT(false);
	}
	
	return out.str();
}

isl_map * isl_interface_build_access_map( isl_set * set, vector<SgExpression*>& subs )
{
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	int parDim = isl_set_dim(set,isl_dim_param);
	int dim = isl_set_dim(set,isl_dim_set);
	int subDim = subs.size();
	stringstream out;
	out<<"[";
	for ( int i = 0 ; i < parDim-1 ; i++ )
		out<<isl_set_get_dim_name(set,isl_dim_param,i)<<",";
	out<<isl_set_get_dim_name(set,isl_dim_param,parDim-1)<<"]->{[";
	
	for ( int i = 0 ; i < dim-1 ; i++ )
		out<<isl_set_get_dim_name(set,isl_dim_set,i)<<",";
	out<<isl_set_get_dim_name(set,isl_dim_set,dim-1)<<"]->[";
	
	if ( subDim == 0 ) { // Scalar
		out<<"] : }";
	} else {
		for ( int i = 0 ; i < subDim-1 ; i++ )
			out<<"t"<<i<<",";
		out<<"t"<<subDim-1<<"] : ";
	
		for ( int i = 0 ; i < subDim-1 ; i++ )
			out<<"t"<<i<<"="<<subs[i]->unparseToString()<<" and ";
		out<<"t"<<subDim-1<<"="<<subs[subDim-1]->unparseToString()<<"}";
	}
	
	isl_map * map = isl_map_read_from_str(ctx,out.str().c_str());
	
	return isl_map_intersect_domain(map,isl_set_copy(set));
}

isl_map * isl_interface_build_schedule_map( isl_set * set, vector<int>& pos )
{
	isl_ctx * ctx = isl_set_get_ctx(set);
	
	int parDim = isl_set_dim(set,isl_dim_param);
	int dim = isl_set_dim(set,isl_dim_set);
	int outDim = 2*dim+1;
	stringstream out;
	out<<"[";
	for ( int i = 0 ; i < parDim-1 ; i++ )
		out<<isl_set_get_dim_name(set,isl_dim_param,i)<<",";
	out<<isl_set_get_dim_name(set,isl_dim_param,parDim-1)<<"]->{[";
	
	for ( int i = 0 ; i < dim-1 ; i++ )
		out<<isl_set_get_dim_name(set,isl_dim_set,i)<<",";
	out<<isl_set_get_dim_name(set,isl_dim_set,dim-1)<<"]->[";
	
	for ( int i = 0 ; i < outDim-1 ; i++ )
		out<<"t"<<i<<",";
	out<<"t"<<outDim-1<<"] : ";
	
	for ( int i = 0 ; i < outDim-1 ; i++ ) {
		if (i%2)
			out<<"t"<<i<<"="<<isl_set_get_dim_name(set,isl_dim_set,i/2)<<" and ";
		else
			out<<"t"<<i<<"="<<pos[i/2]<<" and ";
	}
	out<<"t"<<outDim-1<<"="<<pos[pos.size()-1]<<"}";
	
	return isl_map_read_from_str(ctx,out.str().c_str());
}

isl_map * isl_interface_build_map_with_order_csts( isl_set * src, isl_set * dest, int * llv )
{
	isl_ctx * ctx = isl_set_get_ctx(src);
	
	stringstream out;
	int parSize = isl_set_dim(src,isl_dim_param);
	int uSize = isl_set_dim(dest,isl_dim_set);
	int outSize = isl_set_dim(src,isl_dim_set);
	
	out<<"[";
	for ( int i = 0 ; i < parSize-1 ; i++ )
		out<<isl_set_get_dim_name(src,isl_dim_param,i)<<",";
	out<<isl_set_get_dim_name(src,isl_dim_param,parSize-1)<<"]->{[";
	
	for ( int i = 0 ; i < uSize-1 ; i++ )
		out<<isl_set_get_dim_name(dest,isl_dim_set,i)<<",";
	out<<isl_set_get_dim_name(dest,isl_dim_set,uSize-1)<<"]->[";
	
	for ( int i = 0 ; i < outSize-1 ; i++ )
		out<<"t"<<i<<",";
	out<<"t"<<outSize-1<<"] : ";
	
	for ( int i = 0 ; i < uSize ; i++ ) {
		if ( llv[i] == 2 ) {
			for ( int j = i ; j < uSize-1 ; j++ ) {
				out<<"t"<<uSize-1-j<<"="<<isl_set_get_dim_name(dest,isl_dim_set,uSize-1-j)<<" and ";
			}
			out<<"t0="<<isl_set_get_dim_name(dest,isl_dim_set,0)<<"}";
			break;
		} else if ( llv[i] == 1 ) {
			out<<"t"<<uSize-1-i<<"<"<<isl_set_get_dim_name(dest,isl_dim_set,uSize-1-i);
			if ( i == uSize-1 )
				out<<"}";
			else
				out<<" and ";
		} else { 
			if ( i == uSize-1 )
				out<<"}";
		}
	}
	
	return isl_map_read_from_str(ctx,out.str().c_str());
}

int fCst_lhs( __isl_take isl_constraint * c, void * user )
{	
	int inDim = isl_constraint_dim(c,isl_dim_in);
	visitor * cd = (visitor*)user;
	if ( isl_constraint_is_equality(c) ) {
		if ( cd->pos == cd->index ) {
			isl_int v;
			isl_int_init(v);
			for ( int i = 0 ; i < inDim ; i++ ) {
				isl_constraint_get_coefficient(c,isl_dim_in,i,&v);
				isl_int_neg(v,v);
				cd->user = (void*)isl_constraint_set_coefficient((isl_constraint*)cd->user,isl_dim_out,i,v);
			}
			isl_constraint_get_constant(c,&v);
			isl_int_neg(v,v);
			cd->user = (void*)isl_constraint_set_constant((isl_constraint*)cd->user,v);
			isl_int_clear(v);
		}
		cd->index++;
	}
	
	isl_constraint_free(c);
	return 0;
}

int fCst_rhs( __isl_take isl_constraint * c, void * user )
{
	int inDim = isl_constraint_dim(c,isl_dim_in);
	visitor * cd = (visitor*)user;
	if ( isl_constraint_is_equality(c) ) { 
		if ( cd->pos == cd->index ) {
			isl_int v;
			isl_int v2;
			isl_int_init(v);
			isl_int_init(v2);
			for ( int i = 0 ; i < inDim ; i++ ) {
				isl_constraint_get_coefficient(c,isl_dim_in,i,&v);
				cd->user = (void*)isl_constraint_set_coefficient((isl_constraint*)cd->user,isl_dim_in,i,v);
			}
			isl_constraint_get_constant(c,&v);
			isl_constraint_get_constant((isl_constraint*)cd->user,&v2);
			isl_int_add(v,v,v2);
			cd->user = (void*)isl_constraint_set_constant((isl_constraint*)cd->user,v);
			isl_int_clear(v);
			isl_int_clear(v2);
		}
		cd->index++;
	}
	
	isl_constraint_free(c);
	return 0;
}

int fMap_lhs( __isl_take isl_basic_map * bmap, void * user )
{
	isl_basic_map_foreach_constraint(bmap,fCst_lhs,user);
	isl_basic_map_free(bmap);
	return 0;
}

int fMap_rhs( __isl_take isl_basic_map * bmap, void * user )
{
	isl_basic_map_foreach_constraint(bmap,fCst_rhs,user);
	isl_basic_map_free(bmap);
	return 0;
}

isl_map * isl_interface_integer_map_add_conflict_cst( isl_map * map, isl_map * lhs, isl_map * rhs )
{
	int dim = isl_map_dim(lhs,isl_dim_out);
	isl_space * space = isl_map_get_space(map);
	isl_local_space * ls = isl_local_space_from_space(space);
	for ( int i = 0 ; i < dim ; i++ )
	{
		Visitor * c_data = new Visitor(i);
		c_data->user = (void*)isl_equality_alloc(isl_local_space_copy(ls));
		isl_map_foreach_basic_map(lhs,fMap_lhs,(void*)c_data);
		c_data->reset(i);
		isl_map_foreach_basic_map(rhs,fMap_rhs,(void*)c_data);
		map = isl_map_add_constraint(map,(isl_constraint*)c_data->user);
		delete(c_data);
	}
		
	isl_local_space_free(ls);
	return map;
}

isl_map * isl_interface_integer_map_intersect_domain( isl_map * map, isl_set * set )
{
	return isl_map_intersect_domain(map,isl_set_copy(set));
}

isl_map * isl_interface_build_irregular_access_map( isl_set * set, vector<SgExpression*>& subs )
{
	return NULL;
}


isl_map * isl_interface_integer_map_intersect_range( isl_map * map, isl_set * set )
{
	return isl_map_intersect_range(map,isl_set_copy(set));
}

isl_set * isl_interface_integer_set_set_name( isl_set * set, string name )
{
	return isl_set_set_tuple_name(set,name.c_str());
}



