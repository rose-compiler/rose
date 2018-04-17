namespace sc_dt
{

// classes defined in this module
class sc_lv_base;


// ----------------------------------------------------------------------------
//  CLASS : sc_lv_base
//
//  Arbitrary size logic vector base class.
// ----------------------------------------------------------------------------

class sc_lv_base
//    : public sc_proxy<sc_lv_base>
{
   //  friend class sc_bv_base;


public:
   sc_lv_base();
   sc_lv_base(int x);
   sc_lv_base(int x, int y);

   sc_lv_base(char x);

};
}




namespace sc_dt
{

// classes defined in this module
template <int W> class sc_lv;


// ----------------------------------------------------------------------------
//  CLASS TEMPLATE : sc_lv<W>
//
//  Arbitrary size logic vector class.
// ----------------------------------------------------------------------------

template <int W>
class sc_lv
    : public sc_lv_base
{
public:

    // constructors

    sc_lv()
     : sc_lv_base( W )
	{}

//    explicit sc_lv( const sc_logic& init_value )  	: sc_lv_base( init_value, W )	{}

    explicit sc_lv( bool init_value )
    //: sc_lv_base( sc_logic( init_value ), W )
	{}

    explicit sc_lv( char init_value )
    // : sc_lv_base( sc_logic( init_value ), W )
	{}

    sc_lv( const char* a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( const bool* a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }
#if 0
    sc_lv( const sc_logic* a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }
#endif
#if 0
    sc_lv( const sc_unsigned& a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( const sc_signed& a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( const sc_uint_base& a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( const sc_int_base& a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }
#endif

    sc_lv( unsigned long a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( long a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( unsigned int a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( int a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

#if 0
    sc_lv( uint64 a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }

    sc_lv( int64 a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }
#endif

#if 0
    template <class X>
    sc_lv( const sc_proxy<X>& a )
	: sc_lv_base( W )
	{ /* sc_lv_base::operator = ( a ); */ }
#endif

    sc_lv( const sc_lv<W>& a )
	: sc_lv_base( a )
	{}


    // assignment operators
#if 0
    template <class X>
    sc_lv<W>& operator = ( const sc_proxy<X>& a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }
#endif

    sc_lv<W>& operator = ( const sc_lv<W>& a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( const char* a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( const bool* a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

#if 0
    sc_lv<W>& operator = ( const sc_logic* a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( const sc_unsigned& a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( const sc_signed& a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( const sc_uint_base& a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( const sc_int_base& a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }
#endif

    sc_lv<W>& operator = ( unsigned long a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( long a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( unsigned int a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( int a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

#if 0
    sc_lv<W>& operator = ( uint64 a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }

    sc_lv<W>& operator = ( int64 a )
	{ /* sc_lv_base::operator = ( a ); */ return *this; }
#endif

};
   

}

   
namespace sc_core {

class sc_process_b;


// ----------------------------------------------------------------------------
//  CLASS sc_lv_resolve<W>
//
//  Resolution function for sc_dt::sc_lv<W>.
// ----------------------------------------------------------------------------

// extern const sc_dt::sc_logic_value_t sc_logic_resolution_tbl[4][4];


template <int W>
class sc_lv_resolve
{
public:

    // resolves sc_dt::sc_lv<W> values and returns the resolved value
//  static void resolve(sc_dt::sc_lv<W>&, const std::vector<sc_dt::sc_lv<W>*>&);
   static void resolve(sc_dt::sc_lv<W>&);
};
}
