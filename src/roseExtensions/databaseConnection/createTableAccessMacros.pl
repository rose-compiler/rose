#!/usr/bin/perl

# generate templated rowdat class generating macro code
# dont use //-style comments here :) 

# fix constructor - use CONST not CMP
# remove ##'s
# add set/get

# no. of macro templates to generate
$generateMax = 16;

open (OUT, ">TableAccessMacros.h");

print OUT << "---";

// This file is generated from the perl script createTableAccessMacros.pl.pl, please do 
// not modify this file directly.

#ifndef __sql_custom_h__
#define __sql_custom_h__

#include <vector>
#include <string>
using std::vector;
using std::string;

#include <mysql++.h>
using namespace mysqlpp;

using namespace std;

//! this datatype should be used for columns that are of SQL type "TEXT" instead of VARCHAR
typedef string textString;

//! basic interface class, that all tables inherit from
class TableRowdataInterface {
public: 

	//! we need a virtual desctructor
	virtual ~TableRowdataInterface();

	//! is it a custom table? only the customRowdataInterface should return true here
	virtual bool custom( void ) = 0;

	//! init data from row
	virtual void set(const mysqlpp::Row &row) = 0;

	//! get the name of the table
	virtual char *getTableName() = 0;

	//! get a vector of strings of the column datatypes
	virtual vector<string> getColumnDatatypes( void ) = 0;

	//! get a vector of strings of the column names
	virtual vector<string> getColumnNames( void ) = 0;

};
	


//! implementation of the TableRowdataInterface for custom tables that dont use this interface
class customTableRowdata : public TableRowdataInterface {
public: 
	//! constructor
	customTableRowdata() {};

	//! we need a virtual desctructor
	virtual ~customTableRowdata() {};

	//! this is a custom table, so the following functions shouldnt be called
	virtual bool custom( void ) { return true; }

	//! init data from row
	virtual void set(const mysqlpp::Row &row) { assert(false); };

	//! get the name of the table
	virtual char *getTableName() { assert(false); return "undefined"; };

	//! get a vector of strings of the column datatypes
	virtual vector<string> getColumnDatatypes( void ) { assert(false); vector<string> ret; return ret; }

	//! get a vector of strings of the column names
	virtual vector<string> getColumnNames( void ) { assert(false); vector<string> ret; return ret; };

};
	

enum sql_dummy_type {sql_dummy};
enum sql_cmp_type {sql_use_compare};

inline int sql_cmp(const string &a, const string &b) {
  return a.compare(b);
}
---

@types = ("char", "unsigned char", "mysqlpp::tiny_int<unsigned char>", "int", "unsigned int",
	  "short int", "unsigned short int");
foreach $type (@types) {
    print OUT << "---";

inline int sql_cmp($type a,$type b) {
  return a-b;
}
---
}

@types = ("double", "float");
foreach $type (@types) {
    print OUT << "---";

inline int sql_cmp($type a,$type b) {
  if (a == b) return 0;
  if (a <  b) return -1;
  return 1;
}
---
}	

print OUT << "---";

#ifndef NO_LONG_LONGS
---

@types = ("mysqlpp::longlong", "mysqlpp::ulonglong");
foreach $type (@types) {
    print OUT << "---";

inline int sql_cmp($type a,$type b) {
  if (a == b) return 0;
  if (a <  b) return -1;
  return 1;
}
---
}	

print OUT << "---";

#endif // NO_LONG_LONGS



---

# old start of the custom-macros file

print OUT << "---";





// ---------------------------------------------------
//                Begin Mandatory Compare 
// ---------------------------------------------------

#define sql_compare_define(NAME) \\
    bool operator == (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other) == 0;} \\
    bool operator != (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other) != 0;} \\
    bool operator > (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other) >  0;} \\
    bool operator < (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other) <  0;} \\
    bool operator >= (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other) >= 0;} \\
    bool operator <= (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other) <= 0;} \\
    int cmp (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other);} \\
    int compare (const NAME &other) const \\
      {return sql_compare_##NAME<sql_dummy>(*this,other);}
---

foreach $j (1..$generateMax) {
    $parm0 .= "T$j, C$j";
    $parm0 .= ", " unless $j == $generateMax;
    $parm1 .= "C$j";
    $parm1 .= ", " unless $j == $generateMax;
}

print OUT << "---";

#define sql_compare_define_0(NAME, $parm0)

#define sql_construct_define_0(NAME, $parm0)

#define sql_COMPARE__0(NAME, $parm1)

#define sql_compare_type_def_0(NAME, WHAT, NUM) \\
  sql_compare_type_def_##NUM (NAME, WHAT, NUM)

#define sql_compare_type_defe_0(NAME, WHAT, NUM) \\
  sql_compare_type_defe_##NUM (NAME, WHAT, NUM)

// ---------------------------------------------------
//                 End Mandatory Compare 
// ---------------------------------------------------
---

foreach $i (1..$generateMax) {
  $compr = ""; 
	$parm2 = ""; 
	$define = "";
  $compr = "    int cmp; \\\n" unless $i == 1;
	$set = "";
  $compp = "";
  foreach $j (1..$i) {
      $compr .= "    if (cmp = sql_cmp(x.get_##C$j() , y.get_##C$j() )) return cmp; \\\n"
                                                              unless $j == $i;
      $compr .= "    return sql_cmp(x.get_##C$j() , y.get_##C$j() );"   if $j == $i;
      $parm2 .= "const T$j &p$j";
      $parm2 .= ", "  unless $j == $i;
      $define.= "C$j (p$j)";
      $define.= ", "  unless $j == $i;
      $set   .= "    C$j = p$j;\\\n";
      $compp .= "true";
      $compp .= ", " unless $j == $i;
  }
  print OUT << "---";

// ---------------------------------------------------
//                   Begin Compare $i
// ---------------------------------------------------


#define sql_constructor_$i(NAME, $parm0) \\
  NAME ($parm2) : $define {}; \\


#define sql_compare_define_$i(NAME, $parm0) \\
  void set  ($parm2) { \\
$set \\
  } \\
  sql_compare_define(NAME)

#define sql_compare_type_def_$i(NAME, WHAT, NUM) \\
  return WHAT##_list(d, m, $compp)

#define sql_compare_type_defe_$i(NAME, WHAT, NUM) \\
  return WHAT##_list(d, c, m, $compp)

#define sql_COMPARE__$i(NAME, $parm1) \\
  template <sql_dummy_type dummy> \\
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \\
$compr \\
  } \\
  template <sql_dummy_type dummy> \\
  int compare (const NAME &x, const NAME &y) { \\
$compr \\
  }

// ---------------------------------------------------
//                   End Compare $i
// ---------------------------------------------------

---
}

print OUT << "---";

---


foreach $i (1..$generateMax) {
	$parm_complete = ""; 
	$parm_order = ""; $parm_order2c = "";
	$last_parm_simple = $parm_simple;
	$last_parm_simple2c = $parm_simple2c;
	$parm_simple = ""; $parm_simple2c = "";
	$parm_simple_b = ""; $parm_simple2c_b = "";
	$parm_names = ""; $parm_names2c = "";
	$defs = ""; $popul = ""; $parmc = ""; $parmC = "";
	$coltypes = "";
	$value_list = ""; $field_list = ""; $equal_list = "";
	$value_list_cus = ""; $cus_field_list = ""; $cus_equal_list = "";
	$create_bool = ""; $create_list = "";
	$cusparms1 = ""; $cusparms2 = ""; $cusparmsv = "";    
	$cusparms11 = ""; $cusparms22 = "";
	$names = "";$enums = "";
	$deffnc = ""; # member var access functions
	$opeq = "";   # operator= decl
	foreach $j (1 .. $i) {
		$parm_complete .= "T$j, I$j, N$j, O$j";
		$parm_complete .= ", " unless $j == $i;
		$parm_order    .= "T$j, I$j, O$j";
		$parm_order    .= ", " unless $j == $i;
		$parm_order2c  .= "T$j, I$j, #I$j, O$j";
		$parm_order2c  .= ", " unless $j == $i;
		$parm_names    .= "T$j, I$j, N$j";
		$parm_names    .= ", " unless $j == $i;
		$parm_names2c  .= "T$j, I$j, N$j, ". ($j-1);
		$parm_names2c  .= ", " unless $j == $i;
		$parm_simple   .= "T$j, I$j";
		$parm_simple   .= ", " unless $j == $i;
		$parm_simple2c .= "T$j, I$j, #I$j, ". ($j-1);
		$parm_simple2c .= ", " unless $j == $i;
		$parm_simple_b   .= "T$j, I$j";
		$parm_simple_b   .= ", " unless $j == $i;
		$parm_simple2c_b .= "T$j, I$j, ". ($j-1);
		$parm_simple2c_b .= ", " unless $j == $i;

		$defs  .= "    T$j I$j;";
		$defs  .= "\n" unless $j == $i;
		$deffnc  .= "    inline T$j get_##I$j( void ) const { return I$j; };\n";
		$deffnc  .= "    inline void set_##I$j( T$j set ){ I$j = set; };\n";
		$deffnc  .= "    inline void set_##I$j( mysqlpp::String set ){ I$j = (T$j)set; };\n";
		$opeq  .= "    I$j = rhs.I$j;\n";

		$coltypes .= "    #T$j";
		$coltypes .= ",\n" unless $j == $i;
		$popul .= "    s->set_##I$j( row[ O$j ] );";
		$popul .= "\n" unless $j == $i;
		$names .= "    N$j ";
		$names .= ",\n" unless $j == $i;
		$enums .= "    NAME##_##I$j";
		$enums .= ",\n" unless $j == $i;
		$field_list .= "    s << obj.manip << obj.obj->names[".($j-1)."]";
		$field_list .= " << obj.delem;\n" unless $j == $i;
		$value_list .= "    s << obj.manip << obj.obj->get_##I$j()";
		$value_list .= " << obj.delem;\n" unless $j == $i;
		$create_bool .= "    if (i$j) (*include)[".($j-1)."]=true;\n";
		$create_list .= "    if (i$j == NAME##_NULL) return;\n" unless $i == 1;
		$create_list .= "    (*include)[i$j]=true;\n";

		$value_list_cus .= "    if ((*obj.include)[".($j-1)."]) { \n";
		$value_list_cus .= "      if (before) s << obj.delem;\n" unless $j == 1;
		$value_list_cus .= "      s << obj.manip << obj.obj->get_##I$j();\n";
		$value_list_cus .= "      before = true; \n" unless $j == $i;
		$value_list_cus .= "     } \n";

		$cus_field_list .= "    if ((*obj.include)[".($j-1)."]) { \n";
		$cus_field_list .= "      if (before) s << obj.delem;\n" unless $j == 1;
		$cus_field_list .= "      s << obj.manip << obj.obj->names[".($j-1)."];\n";
		$cus_field_list .= "      before = true; \n" unless $j == $i;
		$cus_field_list .= "     } \n";

		$cus_equal_list .= "    if ((*obj.include)[".($j-1)."]) { \n";
		$cus_equal_list .= "      if (before) s << obj.delem;\n" unless $j == 1;
		$cus_equal_list .= "      s << obj.obj->names[".($j-1)."] << obj.comp";
		$cus_equal_list .=        " << obj.manip << obj.obj->get_I$j();\n";
		$cus_equal_list .= "      before = true; \n" unless $j == $i;
		$cus_equal_list .= "     } \n";

		$equal_list .= "    s << obj.obj->names[".($j-1)."] << obj.comp";
		$equal_list .= " << obj.manip << obj.obj->get_##I$j()";
		$equal_list .= " << obj.delem;\n" unless $j == $i;
		$cusparms1  .= "bool i$j"         if     $j == 1;
		$cusparms1  .= "bool i$j = false" unless $j == 1;
		$cusparms1  .= ", " unless $j == $i;
		$cusparms11  .= "bool i$j" ;
		$cusparms11  .= ", " unless $j == $i;
		$cusparms2  .= "  NAME##_enum i$j" if $j == 1;
		$cusparms2  .= "  NAME##_enum i$j =   NAME##_NULL" unless $j == 1;
		$cusparms2  .= ", " unless $j == $i;
		$cusparms22  .= "  NAME##_enum i$j";
		$cusparms22  .= ", " unless $j == $i;
		$cusparmsv  .= "i$j";
		$cusparmsv  .= ", " unless $j == $i;
		$parmC .= "T$j, I$j";
		$parmC .= ", " unless $j == $generateMax;
		$parmc .= "I$j";
		$parmc .= ", " unless $j == $generateMax;
	}
	foreach $j ($i+1 .. $generateMax) {
		$parmC .= "0, 0";
		$parmC .= ", " unless $j == $generateMax;
		$parmc .= "0";
		$parmc .= ", " unless $j == $generateMax;
	}

	# add function declarations
	$getfnct_defs = " \n".
	"/*! this isnt a custom table */ \n".
	"virtual bool custom( void ) { return false; } \n".
	"/*! get a vector of the $i strings of the column datatypes */\n".
	"virtual vector<string> getColumnDatatypes( void ); \n".
	"/*! get a vector of strings of the column names */\n".
	"virtual vector<string> getColumnNames( void );\n".
	"/*! get the name of the table */ \n".
  "virtual  char *getTableName(); \n".
  "inline NAME & operator = (NAME const &rhs) \n".
  "    {\n".
	$opeq.		
	"		 return *this; };\n".
	"";

	# add column type function
	$getfnct_impl = "\n";

	$getfnct_impl .= 
	"/*! get the name of the table */ \n".
  "inline  char *NAME::getTableName() { return _table; } \n".
	"\n";
	$getfnct_impl .= 
	"/* get a vector of the $i strings of the column datatypes */\n".
	"inline vector<string> NAME::getColumnDatatypes( void ) { \n".
  "  vector<string> ret;\n";
	for($k=0;$k<$i;$k++) {
		$getfnct_impl .= "  ret.push_back( coltypes[$k] );\n";
	}
		
	$getfnct_impl .= 
	"  return ret;\n".
	"} ";

	# add column name function
	$getfnct_impl .= "\n";
	$getfnct_impl .= 
	"/*! get a vector of strings of the column names */\n".
	"inline vector<string> NAME::getColumnNames( void ) {\n".
  "  vector<string> ret;\n";
	for($k=0;$k<$i;$k++) {
		$getfnct_impl .= "  ret.push_back( names[$k] );\n";
	}
		
	$getfnct_impl .= 
	"  return ret;\n".
	"} ";


    print OUT << "---";
// ---------------------------------------------------
//                  Begin Create $i
// ---------------------------------------------------
---
    $out = <<"---";
#define sql_create_basic_c_order_$i(NAME, CMP, CONTR, $parm_order)

  class NAME; 

  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);

  class NAME : public TableRowdataInterface { 
	public: 
    NAME () {} 
    NAME (const mysqlpp::Row &row);
		virtual ~NAME() {};
    sql_compare_define_##CMP(NAME, $parmC)
    sql_constructor_##CONTR(NAME, $parmC)
$getfnct_defs
$deffnc 

private:  
$defs 
  }; 

  template <sql_dummy_type dummy> 
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { 
$popul 
  } 

  inline NAME::NAME (const mysqlpp::Row &row) 
    {populate_##NAME<sql_dummy>(this, row);} 

  sql_COMPARE__##CMP(NAME, $parmc )
---
    print OUT &prepare($out);

    $out = <<"---";
#define sql_create_complete_$i(NAME, CMP, CONTR, $parm_complete) 
  class NAME; 

  enum NAME##_enum { 
$enums 
    , NAME##_NULL 
  }; 

  template <class Manip>
  class NAME##_value_list { 
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */
  public:  
    const NAME *obj;
    const char *delem;
    Manip manip;
  public:  
    NAME##_value_list (const NAME *o, const char *d, Manip m) 
      : obj(o), delem(d), manip(m) {} 
  };

  template <class Manip>
  class NAME##_##field_list {
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */
  public:  
    const NAME *obj; 
    const char *delem;
    Manip manip;
  public:  
    NAME##_field_list (const NAME *o, const char *d, Manip m) 
      : obj(o), delem(d), manip(m) {} 
  };

  template <class Manip>
  class NAME##_equal_list { 
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */
  public:  
    const NAME *obj;
    const char *delem;
    const char *comp;
    Manip manip;
  public:  
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) 
      : obj(o), delem(d), comp(c), manip(m) {}
  };

  template <class Manip>
  class NAME##_cus_value_list {
  /* friend ostream& operator << <> (ostream&, 
  				  const NAME##_cus_value_list<Manip>&); */
  public: 
    const NAME *obj;
    const char *delem;
    Manip manip;
    vector<bool> *include;
    bool del_vector;
  public:  
    ~NAME##_cus_value_list () {if (del_vector) delete include;} 
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, $cusparms11);
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, $cusparms22); 
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}
  };

  template <class Manip>
  class NAME##_cus_field_list { 
  /* friend ostream& operator << <> (ostream&, 
     				  const NAME##_cus_field_list<Manip>&); */
  public: 
    const NAME *obj; 
    const char *delem;
    Manip manip;
    vector<bool> *include; 
    bool del_vector; 
  public:  
    ~NAME##_cus_field_list () {if (del_vector) delete include;} 
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, $cusparms11); 
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, $cusparms22); 
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) 
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}
  };

 template <class Manip>
 class NAME##_cus_equal_list {
 /* friend ostream& operator << <> (ostream&, 
				  const NAME##_cus_equal_list<Manip>&); */
  public: 
    const NAME *obj;
    vector<bool> *include;
    bool del_vector;
    const char *delem;
    const char *comp;
    Manip manip;
  public: 
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, $cusparms11); 
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, $cusparms22); 
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) 
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}
  };

  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);

  class NAME : public TableRowdataInterface { 
	public: 
    NAME () {} 
    NAME (const mysqlpp::Row &row);
		virtual ~NAME() {};
    void set (const mysqlpp::Row &row);
    sql_compare_define_##CMP(NAME, $parmC)
    sql_constructor_##CONTR(NAME, $parmC)
    static char *names[];
    static char *coltypes[];
    static char *_table;
    static char *& table() {return _table;}
$getfnct_defs
$deffnc 


private:  
$defs 

public: 
    NAME##_value_list<quote_type0> value_list() const {
      return value_list(",", mysqlpp::quote);}
    NAME##_value_list<quote_type0> value_list(const char *d) const {
      return value_list(d, mysqlpp::quote);}
    template <class Manip> 
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; 

    NAME##_field_list<do_nothing_type0> field_list() const {
      return field_list(",", mysqlpp::do_nothing);}
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {
      return field_list(d, mysqlpp::do_nothing);}
    template <class Manip>
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; 

    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", 
                                              const char *c = " = ") const{
      return equal_list(d, c, mysqlpp::quote);}
    template <class Manip>
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; 
    
    /* cus_data */

    NAME##_cus_value_list<quote_type0> value_list($cusparms1) const {
      return value_list(",", mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_value_list<quote_type0> value_list($cusparms2) const {
      return value_list(",", mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {
      return value_list(",", mysqlpp::quote, i);
    }
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {
      return value_list(",", mysqlpp::quote, sc);
    }

    NAME##_cus_value_list<quote_type0> value_list(const char *d, $cusparms1) const {
      return value_list(d, mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_value_list<quote_type0> value_list(const char *d, $cusparms2) const {
      return value_list(d, mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_value_list<quote_type0> value_list(const char *d, 
						vector<bool> *i) const {
      return value_list(d, mysqlpp::quote, i);
    }
    NAME##_cus_value_list<quote_type0> value_list(const char *d, 
						mysqlpp::sql_cmp_type sc) const {
      return value_list(d, mysqlpp::quote, sc);
    }

    template <class Manip>
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,
					  $cusparms1) const; 
    template <class Manip>
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,
					  $cusparms2) const; 
    template <class Manip>
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, 
					  vector<bool> *i) const;
    template <class Manip>
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, 
					  mysqlpp::sql_cmp_type sc) const;
    /* cus field */

    NAME##_cus_field_list<do_nothing_type0> field_list($cusparms1) const {
      return field_list(",", mysqlpp::do_nothing, $cusparmsv);
    }
    NAME##_cus_field_list<do_nothing_type0> field_list($cusparms2) const {
      return field_list(",", mysqlpp::do_nothing, $cusparmsv);
    }
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {
      return field_list(",", mysqlpp::do_nothing, i);
    }
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const
    {
      return field_list(",", mysqlpp::do_nothing, sc);
    }

    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, 
						       $cusparms1) const {
      return field_list(d, mysqlpp::do_nothing, $cusparmsv);
    }
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,
						       $cusparms2) const {
      return field_list(d, mysqlpp::do_nothing, $cusparmsv);
    }
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, 
						vector<bool> *i) const {
      return field_list(d, mysqlpp::do_nothing, i);
    }
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, 
						mysqlpp::sql_cmp_type sc) const {
      return field_list(d, mysqlpp::do_nothing, sc);
    }

    template <class Manip>
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,
					    $cusparms1) const; 
    template <class Manip>
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,
					    $cusparms2) const; 
    template <class Manip>
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,
					    vector<bool> *i) const;
    template <class Manip>
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, 
					    mysqlpp::sql_cmp_type sc) const;

    /* cus equal */

    NAME##_cus_equal_list<quote_type0> equal_list($cusparms1) const {
      return equal_list(",", " = ", mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_equal_list<quote_type0> equal_list($cusparms2) const {
      return equal_list(",", " = ", mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {
      return equal_list(",", " = ", mysqlpp::quote, i);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {
      return equal_list(",", " = ", mysqlpp::quote, sc);
    }

    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, $cusparms1) const {
      return equal_list(d, " = ", mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, $cusparms2) const {
      return equal_list(d, " = ", mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, 
						vector<bool> *i) const {
      return equal_list(d, " = ", mysqlpp::quote, i);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, 
						mysqlpp::sql_cmp_type sc) const {
      return equal_list(d, " = ", mysqlpp::quote, sc);
    }

    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,
                                                $cusparms1) const {
      return equal_list(d, c, mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,
                                                $cusparms2) const {
      return equal_list(d, c, mysqlpp::quote, $cusparmsv);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,
						vector<bool> *i) const {
      return equal_list(d, c, mysqlpp::quote, i);
    }
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,
						mysqlpp::sql_cmp_type sc) const {
      return equal_list(d, c, mysqlpp::quote, sc);
    }

    template <class Manip>
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, 
					    $cusparms1) const; 
    template <class Manip>
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, 
					    $cusparms2) const; 
    template <class Manip>
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, 
					    vector<bool> *i) const;
    template <class Manip>
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, 
					    mysqlpp::sql_cmp_type sc) const;
  }; 


  template <class Manip>
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list
  (const NAME *o, const char *d, Manip m, $cusparms11) 
  { 
    delem = d;
    manip = m;
    del_vector = true;
    obj = o; 
    include = new vector<bool>($i, false);
$create_bool 
  } 

  template <class Manip>
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list
  (const NAME *o, const char *d, Manip m, $cusparms22) { 
    delem = d;
    manip = m;
    del_vector = true; 
    obj = o; 
    include = new vector<bool>($i, false); 
$create_list 
  }

  template <class Manip>
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list
  (const NAME *o, const char *d, Manip m, $cusparms11) {
    delem = d;
    manip = m;
    del_vector = true; 
    obj = o; 
    include = new vector<bool>($i, false); 
$create_bool 
  } 

  template <class Manip>
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list
  (const NAME *o, const char *d, Manip m, $cusparms22) { 
    delem = d;
    manip = m;
    del_vector = true; 
    obj = o; 
    include = new vector<bool>($i, false); 
$create_list 
  }

  template <class Manip>
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list
  (const NAME *o, const char *d, const char *c, Manip m, $cusparms11) { 
    delem = d;
    comp = c;
    manip = m;
    del_vector = true; 
    obj = o; 
    include = new vector<bool>($i, false); 
$create_bool 
  } 

  template <class Manip>
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list
  (const NAME *o, const char *d, const char *c, Manip m, $cusparms22) { 
    delem = d;
    comp = c;
    manip = m;
    del_vector = true; 
    obj = o; 
    include = new vector<bool>($i, false); 
$create_list 
  }

  template <class Manip>
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { 
$value_list; 
    return s; 
  } 

  template <class Manip>
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { 
$field_list; 
    return s; 
  } 

  template <class Manip>
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { 
$equal_list; 
    return s; 
  } 

  template <class Manip>
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { 
    bool before = false; 
$value_list_cus 
    return s; 
  } 

  template <class Manip>
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { 
    bool before = false; 
$cus_field_list 
    return s; 
  } 

  template <class Manip>
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { 
    bool before = false; 
$cus_equal_list 
    return s; 
  } 

  template <class Manip>
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { 
    return NAME##_value_list<Manip> (this, d, m); 
  } 

  template <class Manip>
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { 
    return NAME##_field_list<Manip> (this, d, m); 
  } 

  template <class Manip>
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { 
    return NAME##_equal_list<Manip> (this, d, c, m); 
  } 
 
  template <class Manip>
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,
						       $cusparms11) const {
    return NAME##_cus_value_list<Manip> (this, d, m, $cusparmsv); 
  } 

  template <class Manip>
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,
							 $cusparms11) const { 
    return NAME##_cus_field_list<Manip> (this, d, m, $cusparmsv); 
  } 

  template <class Manip>
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,
							 $cusparms11) const { 
    return NAME##_cus_equal_list<Manip> (this, d, c, m, $cusparmsv); 
  } 

  template <class Manip>
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,
						       $cusparms22) const { 
    return NAME##_cus_value_list<Manip> (this, d, m, $cusparmsv); 
  } 

  template <class Manip>
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,
							 $cusparms22) const {
    return NAME##_cus_field_list<Manip> (this, d, m, $cusparmsv); 
  } 

  template <class Manip>
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, 
							 $cusparms22) const { 
    return NAME##_cus_equal_list<Manip> (this, d, c, m, $cusparmsv); 
  } 

  template <class Manip>
  inline NAME##_cus_value_list<Manip> 
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {
    sql_compare_type_def_##CMP (NAME, value, NUM);
  }

  template <class Manip>
  inline NAME##_cus_field_list<Manip> 
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {
    sql_compare_type_def_##CMP (NAME, field, NUM);
  }

  template <class Manip>
  inline NAME##_cus_equal_list<Manip> 
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {
    sql_compare_type_defe_##CMP (NAME, equal, NUM);
  }

  template <sql_dummy_type dummy> 
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { 
$popul
  } 

  inline NAME::NAME (const mysqlpp::Row &row) 
                                        {populate_##NAME<sql_dummy>(this, row);}
  inline void NAME::set (const mysqlpp::Row &row)
                                        {populate_##NAME<sql_dummy>(this, row);}

$getfnct_impl

  sql_COMPARE__##CMP(NAME, $parmc )
---
print OUT &prepare($out);


# things that should appear only once
    $out = <<"---";

#define sql_define_basic_$i(NAME, $parm_complete, TABLENAME)
  char *NAME::names[] = { 
$names 
  }; 
  char *NAME::coltypes[] = { 
$coltypes 
  }; 
  /*char *NAME::_table = #TABLENAME ; */
	char *NAME::_table = #TABLENAME; 

---
print OUT &prepare($out);


#
# short cut defs
#

$ninc = $i+1;
print OUT << "---";
#define DONTUSE_sql_create_basic_$i(NAME, CMP, CONTR, $parm_simple_b) \\
  sql_create_basic_c_order_$i(NAME, CMP, CONTR, $parm_simple2c_b)

#define sql_create_$i(NAME, CMP, CONTR, $parm_simple) \\
  sql_create_complete_$i(NAME, CMP, CONTR, $parm_simple2c) \\

#define DONTUSE_sql_create_c_order_$i(NAME, CMP, CONTR, $parm_order) \\
  sql_create_complete_$i(NAME, CMP, CONTR, $parm_order2c)

#define DONTUSE_sql_create_c_names_$i(NAME, CMP, CONTR, $parm_names) \\
  sql_create_complete_$i(NAME, CMP, CONTR, $parm_names2c)
---

if($i>1) {
$n = $i-1;
# only compare ID for these tables?
print OUT << "---";

#define CREATE_TABLE_$n(NAME, $last_parm_simple) \\
	sql_create_$i(NAME##Rowdata, 1, $i, int,id, $last_parm_simple); \\


#define DEFINE_TABLE_$n(NAME, $last_parm_simple) \\
	sql_define_basic_$i(NAME##Rowdata, int,id,"id",, $last_parm_simple2c, NAME); \\


---
}


print OUT << "---";
// ---------------------------------------------------
//                  End Create $i
// ---------------------------------------------------

---

}


print OUT << "---";


#endif
// end of automatically created file

---

# file created

sub prepare {
    local $_ = @_[0];
    s/\n+$//;
    s/\n[\n ]*\n/\n/g; 
    s/\n+/\\\n/g;
    $_ .= "\n\n";
    return $_;
}

