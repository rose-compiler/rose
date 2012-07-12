#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <iomanip>

#include <rosepoly/simple_matrix.h>

#define UB 0
#define LB 1
#define NB 2

using namespace std;

#ifdef ISL

simple_matrix * isl_interface_matrix_product( const simple_matrix& lhs, const simple_matrix& rhs );

simple_matrix * isl_interface_matrix_inverse( const simple_matrix& mat );

#endif

int gcd( int a, int b )
{
	assert(a>=1 && b>=1);
	
	if (a % b == 0) return b;
	if (b % a == 0) return a;
	if ( a == b ) return a;
	
	return ((a > b)? gcd(a-b,b): gcd(a,b-a));
}

int lcm( int a, int b )
{
	return (a*b)/gcd(a,b);
}

// SIMPLE MATRIX

simple_matrix::simple_matrix() : Rows(0), Cols(0), entries(NULL) {}

simple_matrix::simple_matrix( const simple_matrix& m )
{
	Rows = m.Rows;
	Cols = m.Cols;
	entries = (int*) calloc(Rows*Cols,sizeof(int));
	for ( int i = 0 ; i < Rows ; i++ )
		for ( int j = 0 ; j < Cols ; j++ )
			entries[i*Cols+j] = m.entries[i*Cols+j];
}

simple_matrix::simple_matrix( int i, int j ) : Rows(i), Cols(j)
{
	int s = i*j;
	if ( s )
		entries = (int*) calloc(i*j,sizeof(int));
	else
		entries = NULL;
}

simple_matrix::~simple_matrix()
{
	free(entries);
	entries = NULL;
}

void simple_matrix::set_entry( int i, int j, int value )
{
	assert(( i >= 0 ) && ( i < Rows ) && ( j >= 0 ) && ( j < Cols ));
	entries[ i * Cols + j ] = value;
}

int simple_matrix::entry( int i, int j ) const
{
	assert(i<Rows&&i>=0);
	assert(j<Cols&&j>=0);
	return entries[i*Cols+j];
}

int simple_matrix::get_rows() const { return Rows; }

int simple_matrix::get_columns() const { return Cols; }

void simple_matrix::add_zero_row( int pos )
{
	simple_matrix m(1,Cols);
	append(m);
	
	for ( int i = Rows-1 ; i > pos ; i-- )
		swap_rows( i, i-1 );
}

void simple_matrix::swap_rows( int r1, int r2 )
{
	assert( ( 0 <= r1 ) && ( r1 < Rows ) );
	assert( ( 0 <= r2 ) && ( r2 < Rows ) );
	
	for ( int i = 0 ; i < Cols ; i++ ) {
		int swap = entry( r1, i );
		set_entry(r1,i,entry(r2,i));
		set_entry(r2,i,swap);
	}
}

void simple_matrix::add_sigma_negative( int pos )
{
	if ( pos >= Rows )
		add_zero_row(pos);
	
	for ( int i = 0 ; i < Cols ; i++ ) {
		int value = 0;
		for ( int j = 0 ; j < pos ; j++ )
			value -= entry(j,i);
		set_entry(pos,i,value);
	}
}

void simple_matrix::eliminate_columns( int begin, int end )
{
	int range = end-begin;
	
	for ( int i = 0 ; i < range ; i++ )
		fourier_motzkin_eliminate( best_elim_candidate(begin,end-i) );
}

int simple_matrix::best_elim_candidate( int begin, int end )
{
	int min_cost = Rows*Rows/4;
	int best_candidate = end-1;
	int ub,lb,cost;
	
	for ( int i = end-1 ; i >= begin ; i-- ) {
		ub=0;
		lb=0;
		for ( int j = 0 ; j < Rows ; j++ ) {
			if (entry(j,i)>0) ub++;
			else if (entry(j,i)<0) lb++;
		}
		cost = lb*ub;
		if ( cost < min_cost ) {
			min_cost = cost;
			best_candidate = i;
		}
	}
	return best_candidate;
}

void simple_matrix::fourier_motzkin_eliminate( int pos )
{
	int lb, ub, nb;
	int l,q,value;
	int * bound;

	assert(pos >= 0);
	assert(pos <= Cols-2);
	
	for ( l = 0 ; l < Rows ; l++ ) {
		if (entry(l,pos) != 0) break;
	}
	
	if ( l==Rows ) {
		remove_column(pos);
	} else {
		bound = new int[Rows];
		
		simple_matrix newCst(0,Cols-1);
		
		lb=0;
		ub=0;
		nb=0;
		/* Variable does appear */
		for ( int i = 0 ; i < Rows ; i++ ) {
			if ( entry(i,pos) == 0 ) {
				bound[i] = NB;
				nb++;
			} else if ( entry(i,pos) > 0 ) {
				bound[i] = LB;
				lb++;
			} else {
				bound[i] = UB;
				ub++;
			}
		}
		
		for ( int i = 0 ; i < Rows ; i++ ) {
			simple_matrix temp(1,Cols-1);
			if (bound[i] == UB) {
				for ( int j = 0 ; j < Rows ; j++ ) {
					if (bound[j] == LB) {
						q = 0;
						for ( int k = 0 ; k < Cols ; k++ ) {
							if ( k!=pos ) {
								value = 
									entry(i,k)*(lcm(entry(j,pos),
										-entry(i,pos))/(-entry(i,pos)))
									+ entry(j,k)*(lcm(-entry(i,pos),
										entry(j,pos))/entry(j,pos));
								
								temp.set_entry(0,q,value);
								q++;
							}
						}
						newCst.append(temp);
					}
				}
			} else if ( bound[i] == NB ) {
				q = 0;
				for ( int j = 0 ; j < Cols ; j++ ) {
					if ( j!=pos ) {
						temp.set_entry(0,q,entry(i,j));
						q++;
					}
				}
				newCst.append(temp);
			}
		}
		delete [] bound;
		newCst.simplify();
		*this = newCst;
	}
}

void simple_matrix::remove_column( int pos )
{
	assert(pos<Cols-1);
	
	int * temp = (int*) calloc( Rows*(Cols-1), sizeof(int) );
	
	int q = 0;
	for ( int i = 0 ; i < Rows ; i++ ) {
		for ( int j = 0 ; j < Cols ; j++ ) {
			if ( j!=pos ) {
				temp[i*(Cols-1)+q] = entry(i,j);
				q++;
			}
		}
	}
	free(entries);
	entries = temp;
	Cols--;
}

void simple_matrix::simplify()
{
	if ( Rows == 0 )
		return;
	
	int * is_redun = new int[Rows];
	int j;
	for ( int i = 0 ; i < Rows ; i++ )
		is_redun[i] = 0;
	
	for ( int i = 0 ; i < Rows ; i++ ) {
		if ( is_redun[i] == 1 ) continue;
		
		for ( j = 0 ; j < Cols ; j++ )
			if ( entry(i,j) != 0 ) break;
		
		if ( j == Cols ) {
			is_redun[i] = 1;
			continue;
		}
		
		for ( int k = i+1 ; k < Rows ; k++ ) {
			if ( is_redun[k] == 1 ) continue;
			
			for ( j = 0 ; j < Cols ; j++ )
				if ( entry(i,j) != entry(k,j) ) break;
			
			if ( j == Cols )
				is_redun[k] = 1;
		}
	}
	
	simple_matrix glob(0,Cols);
	for ( int i = 0 ; i < Rows ; i++ ) {
		if ( is_redun[i] != 1 ) {
			simple_matrix temp(1,Cols);
			for ( int j = 0 ; j < Cols ; j++ )
				temp.set_entry(0,j,entry(i,j));
			glob.append(temp);
		}
	}
	*this = glob;
	
	delete [] is_redun;
}

void simple_matrix::append( const simple_matrix& m )
{
	if ( m.entries == NULL )
		return;
	
	assert( Cols == m.Cols );
	
	simple_matrix Total( Rows+m.Rows, Cols );
	
	for ( int i = 0 ; i < Rows+m.Rows ; i++ ) {
		for ( int j = 0 ; j < Cols ; j++ ) {
			if ( i < Rows ) {
				Total.set_entry(i,j,this->entry(i,j));
			} else {
				Total.set_entry(i,j,m.entry(i-Rows,j));
			}
		}
	}
	Rows+=m.Rows;
	free(entries);
	entries = (int*) calloc(Rows*Cols, sizeof(int) );
	memcpy(entries,Total.entries,Rows*Cols*sizeof(int));
}

simple_matrix& simple_matrix::operator=( const simple_matrix& m )
{
	Rows = m.Rows;
	Cols = m.Cols;
	if (entries)
		free(entries);
	entries = (int*)calloc(Rows*Cols,sizeof(int));
	memcpy(entries,m.entries,Rows*Cols*sizeof(int));
	return *this;
}

simple_matrix * simple_matrix::transpose() const
{
	simple_matrix * trans = new simple_matrix(Cols,Rows);
	
	for ( int i = 0 ; i < Cols ; i++ )
		for ( int j = 0 ; j < Rows ; j++ )
			trans->set_entry(i,j,entry(j,i));
	return trans;
}

simple_matrix * simple_matrix::inverse() const
{
	return matrix_inverse(*this);
}

simple_matrix * simple_matrix::product( simple_matrix * right ) const
{
	return matrix_product(*this,*right);
}

void simple_matrix::print( int ident ) const
{	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<Rows<<" "<<Cols<<endl;
	
	for ( int i = 0 ; i < Rows ; i++ ) {
		for ( int j = 0 ; j < Cols ; j++ ) {
			
			if ( j == 0 )
				cout<<setw(ident+1);
			else
				cout<<setw(6);
			
			cout<<entry(i,j);
			
		}
		cout<<endl;
	}
}


#ifdef ISL

simple_matrix * isl_interface_matrix_product( const simple_matrix& lhs, const simple_matrix& rhs )
{	
	isl_ctx * ctx = isl_ctx_alloc();
	
	isl_mat * mat1 = isl_mat_alloc(ctx,lhs.get_rows(),lhs.get_columns());
	isl_mat * mat2 = isl_mat_alloc(ctx,rhs.get_rows(),rhs.get_columns());
	
	for ( int i = 0 ; i < lhs.get_rows() ; i++ )
		for ( int j = 0 ; j < lhs.get_columns() ; j++ )
			mat1 = isl_mat_set_element_si(mat1,i,j,lhs.entry(i,j));
	
	for ( int i = 0 ; i < rhs.get_rows() ; i++ )
		for ( int j = 0 ; j < rhs.get_columns() ; j++ )
			mat2 = isl_mat_set_element_si(mat2,i,j,rhs.entry(i,j));
	
	isl_mat * mat3 = isl_mat_product(mat1,mat2);
	
	simple_matrix * result = new simple_matrix(isl_mat_rows(mat3),isl_mat_cols(mat3));
	
	for ( int i = 0 ; i < result->get_rows() ; i++ ) {
		for ( int j = 0 ; j < result->get_columns() ; j++ ) {
			isl_int v;
			isl_int_init(v);
			isl_mat_get_element(mat3,i,j,&v);
			result->set_entry(i,j,isl_int_get_si(v));
			isl_int_clear(v);
		}
	}
	
	isl_mat_free(mat3);
	isl_ctx_free(ctx);
	
	return result;
}

simple_matrix * isl_interface_matrix_inverse( const simple_matrix& mat )
{
	isl_ctx * ctx = isl_ctx_alloc();
	isl_mat * mat1 = isl_mat_alloc(ctx,mat.get_rows(),mat.get_columns());
	
	for ( int i = 0 ; i < mat.get_rows() ; i++ )
		for ( int j = 0 ; j < mat.get_columns() ; j++ )
			mat1 = isl_mat_set_element_si(mat1,i,j,mat.entry(i,j));
	
	isl_mat * mat2 = isl_mat_right_inverse(mat1);
	
	simple_matrix * result = new simple_matrix(isl_mat_rows(mat2),isl_mat_cols(mat2));
	
	for ( int i = 0 ; i < result->get_rows() ; i++ ) {
		for ( int j = 0 ; j < result->get_columns() ; j++ ) {
			isl_int v;
			isl_int_init(v);
			isl_mat_get_element(mat2,i,j,&v);
			result->set_entry(i,j,isl_int_get_si(v));
			isl_int_clear(v);
		}
	}
	
	isl_mat_free(mat2);
	isl_ctx_free(ctx);
	
	return result;
}

#endif






