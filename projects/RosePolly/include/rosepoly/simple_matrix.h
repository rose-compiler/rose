

#ifndef SIMPLE_MATRIX_H
#define SIMPLE_MATRIX_H

#ifdef ISL

#include <isl/mat.h>

#define matrix_product(l,r) isl_interface_matrix_product(l,r)
#define matrix_inverse(m) isl_interface_matrix_inverse(m)

#endif

int gcd( int a, int b );
int lcm( int a, int b );

class simple_matrix {
	
	int Rows;
	int Cols;
	int * entries;
	
public:
	
	simple_matrix();
	simple_matrix( int r, int c );
	simple_matrix( const simple_matrix& m );
	
	simple_matrix& operator=( const simple_matrix& m );
	
	~simple_matrix();
	
	void set_entry( int i, int j, int value );
	int entry( int i, int j ) const;
	
	int get_rows() const;
	int get_columns() const;
	
	void add_zero_row( int pos );
	void add_sigma_negative( int pos );  // this converts an equalities matrix into inequalities
	void eliminate_columns( int begin, int endif ); // i=begin, i<end
	int best_elim_candidate( int begin, int end ); // i=begin, i<end
	void fourier_motzkin_eliminate( int pos );
	void append( const simple_matrix& m );
	void swap_rows( int r1, int r2 );
	void remove_column( int pos );
	void simplify();
	
	simple_matrix * transpose() const;
	simple_matrix * product( simple_matrix * right ) const;
	simple_matrix * inverse() const;
	
	void print( int ident ) const;
};

#endif

