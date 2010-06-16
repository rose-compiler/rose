
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

inline int sql_cmp(char a,char b) {
  return a-b;
}

inline int sql_cmp(unsigned char a,unsigned char b) {
  return a-b;
}

inline int sql_cmp(mysqlpp::tiny_int<unsigned char> a,mysqlpp::tiny_int<unsigned char> b) {
  return a-b;
}

inline int sql_cmp(int a,int b) {
  return a-b;
}

inline int sql_cmp(unsigned int a,unsigned int b) {
  return a-b;
}

inline int sql_cmp(short int a,short int b) {
  return a-b;
}

inline int sql_cmp(unsigned short int a,unsigned short int b) {
  return a-b;
}

inline int sql_cmp(double a,double b) {
  if (a == b) return 0;
  if (a <  b) return -1;
  return 1;
}

inline int sql_cmp(float a,float b) {
  if (a == b) return 0;
  if (a <  b) return -1;
  return 1;
}

#ifndef NO_LONG_LONGS

inline int sql_cmp(mysqlpp::longlong a,mysqlpp::longlong b) {
  if (a == b) return 0;
  if (a <  b) return -1;
  return 1;
}

inline int sql_cmp(mysqlpp::ulonglong a,mysqlpp::ulonglong b) {
  if (a == b) return 0;
  if (a <  b) return -1;
  return 1;
}

#endif // NO_LONG_LONGS








// ---------------------------------------------------
//                Begin Mandatory Compare 
// ---------------------------------------------------

#define sql_compare_define(NAME) \
    bool operator == (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other) == 0;} \
    bool operator != (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other) != 0;} \
    bool operator > (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other) >  0;} \
    bool operator < (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other) <  0;} \
    bool operator >= (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other) >= 0;} \
    bool operator <= (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other) <= 0;} \
    int cmp (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other);} \
    int compare (const NAME &other) const \
      {return sql_compare_##NAME<sql_dummy>(*this,other);}

#define sql_compare_define_0(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16)

#define sql_construct_define_0(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16)

#define sql_COMPARE__0(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16)

#define sql_compare_type_def_0(NAME, WHAT, NUM) \
  sql_compare_type_def_##NUM (NAME, WHAT, NUM)

#define sql_compare_type_defe_0(NAME, WHAT, NUM) \
  sql_compare_type_defe_##NUM (NAME, WHAT, NUM)

// ---------------------------------------------------
//                 End Mandatory Compare 
// ---------------------------------------------------

// ---------------------------------------------------
//                   Begin Compare 1
// ---------------------------------------------------


#define sql_constructor_1(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1) : C1 (p1) {}; \


#define sql_compare_define_1(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1) { \
    C1 = p1;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_1(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true)

#define sql_compare_type_defe_1(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true)

#define sql_COMPARE__1(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    return sql_cmp(x.get_##C1() , y.get_##C1() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    return sql_cmp(x.get_##C1() , y.get_##C1() ); \
  }

// ---------------------------------------------------
//                   End Compare 1
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 2
// ---------------------------------------------------


#define sql_constructor_2(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2) : C1 (p1), C2 (p2) {}; \


#define sql_compare_define_2(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2) { \
    C1 = p1;\
    C2 = p2;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_2(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true)

#define sql_compare_type_defe_2(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true)

#define sql_COMPARE__2(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    return sql_cmp(x.get_##C2() , y.get_##C2() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    return sql_cmp(x.get_##C2() , y.get_##C2() ); \
  }

// ---------------------------------------------------
//                   End Compare 2
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 3
// ---------------------------------------------------


#define sql_constructor_3(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3) : C1 (p1), C2 (p2), C3 (p3) {}; \


#define sql_compare_define_3(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_3(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true)

#define sql_compare_type_defe_3(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true)

#define sql_COMPARE__3(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    return sql_cmp(x.get_##C3() , y.get_##C3() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    return sql_cmp(x.get_##C3() , y.get_##C3() ); \
  }

// ---------------------------------------------------
//                   End Compare 3
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 4
// ---------------------------------------------------


#define sql_constructor_4(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4) : C1 (p1), C2 (p2), C3 (p3), C4 (p4) {}; \


#define sql_compare_define_4(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_4(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true)

#define sql_compare_type_defe_4(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true)

#define sql_COMPARE__4(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    return sql_cmp(x.get_##C4() , y.get_##C4() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    return sql_cmp(x.get_##C4() , y.get_##C4() ); \
  }

// ---------------------------------------------------
//                   End Compare 4
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 5
// ---------------------------------------------------


#define sql_constructor_5(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5) {}; \


#define sql_compare_define_5(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_5(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true)

#define sql_compare_type_defe_5(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true)

#define sql_COMPARE__5(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    return sql_cmp(x.get_##C5() , y.get_##C5() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    return sql_cmp(x.get_##C5() , y.get_##C5() ); \
  }

// ---------------------------------------------------
//                   End Compare 5
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 6
// ---------------------------------------------------


#define sql_constructor_6(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6) {}; \


#define sql_compare_define_6(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_6(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true)

#define sql_compare_type_defe_6(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true)

#define sql_COMPARE__6(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    return sql_cmp(x.get_##C6() , y.get_##C6() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    return sql_cmp(x.get_##C6() , y.get_##C6() ); \
  }

// ---------------------------------------------------
//                   End Compare 6
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 7
// ---------------------------------------------------


#define sql_constructor_7(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7) {}; \


#define sql_compare_define_7(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_7(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true)

#define sql_compare_type_defe_7(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true)

#define sql_COMPARE__7(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    return sql_cmp(x.get_##C7() , y.get_##C7() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    return sql_cmp(x.get_##C7() , y.get_##C7() ); \
  }

// ---------------------------------------------------
//                   End Compare 7
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 8
// ---------------------------------------------------


#define sql_constructor_8(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8) {}; \


#define sql_compare_define_8(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_8(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_8(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true)

#define sql_COMPARE__8(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    return sql_cmp(x.get_##C8() , y.get_##C8() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    return sql_cmp(x.get_##C8() , y.get_##C8() ); \
  }

// ---------------------------------------------------
//                   End Compare 8
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 9
// ---------------------------------------------------


#define sql_constructor_9(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9) {}; \


#define sql_compare_define_9(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_9(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_9(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__9(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    return sql_cmp(x.get_##C9() , y.get_##C9() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    return sql_cmp(x.get_##C9() , y.get_##C9() ); \
  }

// ---------------------------------------------------
//                   End Compare 9
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 10
// ---------------------------------------------------


#define sql_constructor_10(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9), C10 (p10) {}; \


#define sql_compare_define_10(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
    C10 = p10;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_10(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_10(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__10(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    return sql_cmp(x.get_##C10() , y.get_##C10() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    return sql_cmp(x.get_##C10() , y.get_##C10() ); \
  }

// ---------------------------------------------------
//                   End Compare 10
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 11
// ---------------------------------------------------


#define sql_constructor_11(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9), C10 (p10), C11 (p11) {}; \


#define sql_compare_define_11(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
    C10 = p10;\
    C11 = p11;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_11(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_11(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__11(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    return sql_cmp(x.get_##C11() , y.get_##C11() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    return sql_cmp(x.get_##C11() , y.get_##C11() ); \
  }

// ---------------------------------------------------
//                   End Compare 11
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 12
// ---------------------------------------------------


#define sql_constructor_12(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9), C10 (p10), C11 (p11), C12 (p12) {}; \


#define sql_compare_define_12(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
    C10 = p10;\
    C11 = p11;\
    C12 = p12;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_12(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_12(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__12(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    return sql_cmp(x.get_##C12() , y.get_##C12() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    return sql_cmp(x.get_##C12() , y.get_##C12() ); \
  }

// ---------------------------------------------------
//                   End Compare 12
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 13
// ---------------------------------------------------


#define sql_constructor_13(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9), C10 (p10), C11 (p11), C12 (p12), C13 (p13) {}; \


#define sql_compare_define_13(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
    C10 = p10;\
    C11 = p11;\
    C12 = p12;\
    C13 = p13;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_13(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_13(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__13(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    return sql_cmp(x.get_##C13() , y.get_##C13() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    return sql_cmp(x.get_##C13() , y.get_##C13() ); \
  }

// ---------------------------------------------------
//                   End Compare 13
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 14
// ---------------------------------------------------


#define sql_constructor_14(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13, const T14 &p14) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9), C10 (p10), C11 (p11), C12 (p12), C13 (p13), C14 (p14) {}; \


#define sql_compare_define_14(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13, const T14 &p14) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
    C10 = p10;\
    C11 = p11;\
    C12 = p12;\
    C13 = p13;\
    C14 = p14;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_14(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_14(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__14(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C13() , y.get_##C13() )) return cmp; \
    return sql_cmp(x.get_##C14() , y.get_##C14() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C13() , y.get_##C13() )) return cmp; \
    return sql_cmp(x.get_##C14() , y.get_##C14() ); \
  }

// ---------------------------------------------------
//                   End Compare 14
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 15
// ---------------------------------------------------


#define sql_constructor_15(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13, const T14 &p14, const T15 &p15) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9), C10 (p10), C11 (p11), C12 (p12), C13 (p13), C14 (p14), C15 (p15) {}; \


#define sql_compare_define_15(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13, const T14 &p14, const T15 &p15) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
    C10 = p10;\
    C11 = p11;\
    C12 = p12;\
    C13 = p13;\
    C14 = p14;\
    C15 = p15;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_15(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_15(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__15(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C13() , y.get_##C13() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C14() , y.get_##C14() )) return cmp; \
    return sql_cmp(x.get_##C15() , y.get_##C15() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C13() , y.get_##C13() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C14() , y.get_##C14() )) return cmp; \
    return sql_cmp(x.get_##C15() , y.get_##C15() ); \
  }

// ---------------------------------------------------
//                   End Compare 15
// ---------------------------------------------------


// ---------------------------------------------------
//                   Begin Compare 16
// ---------------------------------------------------


#define sql_constructor_16(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  NAME (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13, const T14 &p14, const T15 &p15, const T16 &p16) : C1 (p1), C2 (p2), C3 (p3), C4 (p4), C5 (p5), C6 (p6), C7 (p7), C8 (p8), C9 (p9), C10 (p10), C11 (p11), C12 (p12), C13 (p13), C14 (p14), C15 (p15), C16 (p16) {}; \


#define sql_compare_define_16(NAME, T1, C1, T2, C2, T3, C3, T4, C4, T5, C5, T6, C6, T7, C7, T8, C8, T9, C9, T10, C10, T11, C11, T12, C12, T13, C13, T14, C14, T15, C15, T16, C16) \
  void set  (const T1 &p1, const T2 &p2, const T3 &p3, const T4 &p4, const T5 &p5, const T6 &p6, const T7 &p7, const T8 &p8, const T9 &p9, const T10 &p10, const T11 &p11, const T12 &p12, const T13 &p13, const T14 &p14, const T15 &p15, const T16 &p16) { \
    C1 = p1;\
    C2 = p2;\
    C3 = p3;\
    C4 = p4;\
    C5 = p5;\
    C6 = p6;\
    C7 = p7;\
    C8 = p8;\
    C9 = p9;\
    C10 = p10;\
    C11 = p11;\
    C12 = p12;\
    C13 = p13;\
    C14 = p14;\
    C15 = p15;\
    C16 = p16;\
 \
  } \
  sql_compare_define(NAME)

#define sql_compare_type_def_16(NAME, WHAT, NUM) \
  return WHAT##_list(d, m, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_compare_type_defe_16(NAME, WHAT, NUM) \
  return WHAT##_list(d, c, m, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true)

#define sql_COMPARE__16(NAME, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16) \
  template <sql_dummy_type dummy> \
  int sql_compare_##NAME (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C13() , y.get_##C13() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C14() , y.get_##C14() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C15() , y.get_##C15() )) return cmp; \
    return sql_cmp(x.get_##C16() , y.get_##C16() ); \
  } \
  template <sql_dummy_type dummy> \
  int compare (const NAME &x, const NAME &y) { \
    int cmp; \
    if (cmp = sql_cmp(x.get_##C1() , y.get_##C1() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C2() , y.get_##C2() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C3() , y.get_##C3() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C4() , y.get_##C4() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C5() , y.get_##C5() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C6() , y.get_##C6() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C7() , y.get_##C7() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C8() , y.get_##C8() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C9() , y.get_##C9() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C10() , y.get_##C10() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C11() , y.get_##C11() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C12() , y.get_##C12() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C13() , y.get_##C13() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C14() , y.get_##C14() )) return cmp; \
    if (cmp = sql_cmp(x.get_##C15() , y.get_##C15() )) return cmp; \
    return sql_cmp(x.get_##C16() , y.get_##C16() ); \
  }

// ---------------------------------------------------
//                   End Compare 16
// ---------------------------------------------------


// ---------------------------------------------------
//                  Begin Create 1
// ---------------------------------------------------
#define sql_create_basic_c_order_1(NAME, CMP, CONTR, T1, I1, O1)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 1 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
private:  \
    T1 I1; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_1(NAME, CMP, CONTR, T1, I1, N1, O1) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 1 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
private:  \
    T1 I1; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1) const {\
      return value_list(",", mysqlpp::quote, i1);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1) const {\
      return value_list(",", mysqlpp::quote, i1);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1) const {\
      return value_list(d, mysqlpp::quote, i1);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1) const {\
      return value_list(d, mysqlpp::quote, i1);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1) const {\
      return field_list(",", mysqlpp::do_nothing, i1);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1) const {\
      return field_list(",", mysqlpp::do_nothing, i1);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1) const {\
      return field_list(d, mysqlpp::do_nothing, i1);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1) const {\
      return field_list(d, mysqlpp::do_nothing, i1);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1) const {\
      return equal_list(d, c, mysqlpp::quote, i1);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1) const {\
      return equal_list(d, c, mysqlpp::quote, i1);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(1, false);\
    if (i1) (*include)[0]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(1, false); \
    (*include)[i1]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(1, false); \
    if (i1) (*include)[0]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(1, false); \
    (*include)[i1]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(1, false); \
    if (i1) (*include)[0]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(1, false); \
    (*include)[i1]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 1 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_1(NAME, T1, I1, N1, O1, TABLENAME)\
  char *NAME::names[] = { \
    N1  \
  }; \
  char *NAME::coltypes[] = { \
    #T1 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_1(NAME, CMP, CONTR, T1, I1) \
  sql_create_basic_c_order_1(NAME, CMP, CONTR, T1, I1, 0)

#define sql_create_1(NAME, CMP, CONTR, T1, I1) \
  sql_create_complete_1(NAME, CMP, CONTR, T1, I1, #I1, 0) \

#define DONTUSE_sql_create_c_order_1(NAME, CMP, CONTR, T1, I1, O1) \
  sql_create_complete_1(NAME, CMP, CONTR, T1, I1, #I1, O1)

#define DONTUSE_sql_create_c_names_1(NAME, CMP, CONTR, T1, I1, N1) \
  sql_create_complete_1(NAME, CMP, CONTR, T1, I1, N1, 0)
// ---------------------------------------------------
//                  End Create 1
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 2
// ---------------------------------------------------
#define sql_create_basic_c_order_2(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 2 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
private:  \
    T1 I1;\
    T2 I2; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_2(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 2 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
private:  \
    T1 I1;\
    T2 I2; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(2, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(2, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(2, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(2, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(2, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(2, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 2 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_2(NAME, T1, I1, N1, O1, T2, I2, N2, O2, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_2(NAME, CMP, CONTR, T1, I1, T2, I2) \
  sql_create_basic_c_order_2(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1)

#define sql_create_2(NAME, CMP, CONTR, T1, I1, T2, I2) \
  sql_create_complete_2(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1) \

#define DONTUSE_sql_create_c_order_2(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2) \
  sql_create_complete_2(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2)

#define DONTUSE_sql_create_c_names_2(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2) \
  sql_create_complete_2(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1)

#define CREATE_TABLE_1(NAME, T1, I1) \
	sql_create_2(NAME##Rowdata, 1, 2, int,id, T1, I1); \


#define DEFINE_TABLE_1(NAME, T1, I1) \
	sql_define_basic_2(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, NAME); \


// ---------------------------------------------------
//                  End Create 2
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 3
// ---------------------------------------------------
#define sql_create_basic_c_order_3(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 3 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_3(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 3 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(3, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(3, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(3, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(3, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(3, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(3, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 3 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_3(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_3(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3) \
  sql_create_basic_c_order_3(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2)

#define sql_create_3(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3) \
  sql_create_complete_3(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2) \

#define DONTUSE_sql_create_c_order_3(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3) \
  sql_create_complete_3(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3)

#define DONTUSE_sql_create_c_names_3(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3) \
  sql_create_complete_3(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2)

#define CREATE_TABLE_2(NAME, T1, I1, T2, I2) \
	sql_create_3(NAME##Rowdata, 1, 3, int,id, T1, I1, T2, I2); \


#define DEFINE_TABLE_2(NAME, T1, I1, T2, I2) \
	sql_define_basic_3(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, NAME); \


// ---------------------------------------------------
//                  End Create 3
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 4
// ---------------------------------------------------
#define sql_create_basic_c_order_4(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 4 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_4(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 4 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(4, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(4, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(4, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(4, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(4, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(4, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 4 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_4(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_4(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4) \
  sql_create_basic_c_order_4(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3)

#define sql_create_4(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4) \
  sql_create_complete_4(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3) \

#define DONTUSE_sql_create_c_order_4(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4) \
  sql_create_complete_4(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4)

#define DONTUSE_sql_create_c_names_4(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4) \
  sql_create_complete_4(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3)

#define CREATE_TABLE_3(NAME, T1, I1, T2, I2, T3, I3) \
	sql_create_4(NAME##Rowdata, 1, 4, int,id, T1, I1, T2, I2, T3, I3); \


#define DEFINE_TABLE_3(NAME, T1, I1, T2, I2, T3, I3) \
	sql_define_basic_4(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, NAME); \


// ---------------------------------------------------
//                  End Create 4
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 5
// ---------------------------------------------------
#define sql_create_basic_c_order_5(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 5 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_5(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 5 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(5, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(5, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(5, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(5, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(5, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(5, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 5 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_5(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_5(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5) \
  sql_create_basic_c_order_5(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4)

#define sql_create_5(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5) \
  sql_create_complete_5(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4) \

#define DONTUSE_sql_create_c_order_5(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5) \
  sql_create_complete_5(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5)

#define DONTUSE_sql_create_c_names_5(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5) \
  sql_create_complete_5(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4)

#define CREATE_TABLE_4(NAME, T1, I1, T2, I2, T3, I3, T4, I4) \
	sql_create_5(NAME##Rowdata, 1, 5, int,id, T1, I1, T2, I2, T3, I3, T4, I4); \


#define DEFINE_TABLE_4(NAME, T1, I1, T2, I2, T3, I3, T4, I4) \
	sql_define_basic_5(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, NAME); \


// ---------------------------------------------------
//                  End Create 5
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 6
// ---------------------------------------------------
#define sql_create_basic_c_order_6(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 6 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_6(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 6 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(6, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(6, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(6, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(6, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(6, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(6, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 6 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_6(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_6(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6) \
  sql_create_basic_c_order_6(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5)

#define sql_create_6(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6) \
  sql_create_complete_6(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5) \

#define DONTUSE_sql_create_c_order_6(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6) \
  sql_create_complete_6(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6)

#define DONTUSE_sql_create_c_names_6(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6) \
  sql_create_complete_6(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5)

#define CREATE_TABLE_5(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5) \
	sql_create_6(NAME##Rowdata, 1, 6, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5); \


#define DEFINE_TABLE_5(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5) \
	sql_define_basic_6(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, NAME); \


// ---------------------------------------------------
//                  End Create 6
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 7
// ---------------------------------------------------
#define sql_create_basic_c_order_7(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 7 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_7(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 7 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(7, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(7, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(7, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(7, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(7, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(7, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 7 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_7(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_7(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7) \
  sql_create_basic_c_order_7(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6)

#define sql_create_7(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7) \
  sql_create_complete_7(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6) \

#define DONTUSE_sql_create_c_order_7(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7) \
  sql_create_complete_7(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7)

#define DONTUSE_sql_create_c_names_7(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7) \
  sql_create_complete_7(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6)

#define CREATE_TABLE_6(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6) \
	sql_create_7(NAME##Rowdata, 1, 7, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6); \


#define DEFINE_TABLE_6(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6) \
	sql_define_basic_7(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, NAME); \


// ---------------------------------------------------
//                  End Create 7
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 8
// ---------------------------------------------------
#define sql_create_basic_c_order_8(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 8 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, 0, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_8(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 8 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(8, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(8, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(8, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(8, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(8, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(8, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 8 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, 0, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_8(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_8(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8) \
  sql_create_basic_c_order_8(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7)

#define sql_create_8(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8) \
  sql_create_complete_8(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7) \

#define DONTUSE_sql_create_c_order_8(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8) \
  sql_create_complete_8(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8)

#define DONTUSE_sql_create_c_names_8(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8) \
  sql_create_complete_8(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7)

#define CREATE_TABLE_7(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7) \
	sql_create_8(NAME##Rowdata, 1, 8, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7); \


#define DEFINE_TABLE_7(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7) \
	sql_define_basic_8(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, NAME); \


// ---------------------------------------------------
//                  End Create 8
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 9
// ---------------------------------------------------
#define sql_create_basic_c_order_9(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 9 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, 0, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_9(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 9 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(9, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(9, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(9, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(9, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(9, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(9, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 9 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, 0, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_9(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_9(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9) \
  sql_create_basic_c_order_9(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8)

#define sql_create_9(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9) \
  sql_create_complete_9(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8) \

#define DONTUSE_sql_create_c_order_9(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9) \
  sql_create_complete_9(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9)

#define DONTUSE_sql_create_c_names_9(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9) \
  sql_create_complete_9(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8)

#define CREATE_TABLE_8(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8) \
	sql_create_9(NAME##Rowdata, 1, 9, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8); \


#define DEFINE_TABLE_8(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8) \
	sql_define_basic_9(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, NAME); \


// ---------------------------------------------------
//                  End Create 9
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 10
// ---------------------------------------------------
#define sql_create_basic_c_order_10(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 10 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, 0, 0, 0, 0, 0, 0 )

#define sql_create_complete_10(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9,\
    NAME##_##I10 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 10 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(10, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(10, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(10, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(10, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(10, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(10, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.manip << obj.obj->get_##I10(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8] << obj.delem;\
    s << obj.manip << obj.obj->names[9]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_##I10(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I10();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[9];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_I10();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 10 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  ret.push_back( coltypes[9] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  ret.push_back( names[9] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, 0, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_10(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9 ,\
    N10  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9,\
    #T10 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_10(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10) \
  sql_create_basic_c_order_10(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8, T10, I10, 9)

#define sql_create_10(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10) \
  sql_create_complete_10(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9) \

#define DONTUSE_sql_create_c_order_10(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10) \
  sql_create_complete_10(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9, T10, I10, #I10, O10)

#define DONTUSE_sql_create_c_names_10(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9, T10, I10, N10) \
  sql_create_complete_10(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8, T10, I10, N10, 9)

#define CREATE_TABLE_9(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9) \
	sql_create_10(NAME##Rowdata, 1, 10, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9); \


#define DEFINE_TABLE_9(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9) \
	sql_define_basic_10(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, NAME); \


// ---------------------------------------------------
//                  End Create 10
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 11
// ---------------------------------------------------
#define sql_create_basic_c_order_11(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 11 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, 0, 0, 0, 0, 0 )

#define sql_create_complete_11(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9,\
    NAME##_##I10,\
    NAME##_##I11 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 11 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(11, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(11, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(11, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(11, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(11, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(11, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.manip << obj.obj->get_##I11(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8] << obj.delem;\
    s << obj.manip << obj.obj->names[9] << obj.delem;\
    s << obj.manip << obj.obj->names[10]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_##I11(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I11();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[9];\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[10];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_I11();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 11 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  ret.push_back( coltypes[9] );\
  ret.push_back( coltypes[10] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  ret.push_back( names[9] );\
  ret.push_back( names[10] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, 0, 0, 0, 0, 0 )

\
#define sql_define_basic_11(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9 ,\
    N10 ,\
    N11  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9,\
    #T10,\
    #T11 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_11(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11) \
  sql_create_basic_c_order_11(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8, T10, I10, 9, T11, I11, 10)

#define sql_create_11(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11) \
  sql_create_complete_11(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10) \

#define DONTUSE_sql_create_c_order_11(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11) \
  sql_create_complete_11(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9, T10, I10, #I10, O10, T11, I11, #I11, O11)

#define DONTUSE_sql_create_c_names_11(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9, T10, I10, N10, T11, I11, N11) \
  sql_create_complete_11(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8, T10, I10, N10, 9, T11, I11, N11, 10)

#define CREATE_TABLE_10(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10) \
	sql_create_11(NAME##Rowdata, 1, 11, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10); \


#define DEFINE_TABLE_10(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10) \
	sql_define_basic_11(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, NAME); \


// ---------------------------------------------------
//                  End Create 11
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 12
// ---------------------------------------------------
#define sql_create_basic_c_order_12(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, 0, 0, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 12 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, 0, 0, 0, 0 )

#define sql_create_complete_12(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9,\
    NAME##_##I10,\
    NAME##_##I11,\
    NAME##_##I12 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, 0, 0, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, 0, 0, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 12 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(12, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(12, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(12, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(12, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(12, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(12, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.manip << obj.obj->get_##I12(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8] << obj.delem;\
    s << obj.manip << obj.obj->names[9] << obj.delem;\
    s << obj.manip << obj.obj->names[10] << obj.delem;\
    s << obj.manip << obj.obj->names[11]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_##I12(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I12();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[9];\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[10];\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[11];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_I12();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 12 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  ret.push_back( coltypes[9] );\
  ret.push_back( coltypes[10] );\
  ret.push_back( coltypes[11] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  ret.push_back( names[9] );\
  ret.push_back( names[10] );\
  ret.push_back( names[11] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, 0, 0, 0, 0 )

\
#define sql_define_basic_12(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9 ,\
    N10 ,\
    N11 ,\
    N12  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9,\
    #T10,\
    #T11,\
    #T12 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_12(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12) \
  sql_create_basic_c_order_12(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8, T10, I10, 9, T11, I11, 10, T12, I12, 11)

#define sql_create_12(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12) \
  sql_create_complete_12(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11) \

#define DONTUSE_sql_create_c_order_12(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12) \
  sql_create_complete_12(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9, T10, I10, #I10, O10, T11, I11, #I11, O11, T12, I12, #I12, O12)

#define DONTUSE_sql_create_c_names_12(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9, T10, I10, N10, T11, I11, N11, T12, I12, N12) \
  sql_create_complete_12(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8, T10, I10, N10, 9, T11, I11, N11, 10, T12, I12, N12, 11)

#define CREATE_TABLE_11(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11) \
	sql_create_12(NAME##Rowdata, 1, 12, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11); \


#define DEFINE_TABLE_11(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11) \
	sql_define_basic_12(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, NAME); \


// ---------------------------------------------------
//                  End Create 12
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 13
// ---------------------------------------------------
#define sql_create_basic_c_order_13(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, 0, 0, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 13 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, 0, 0, 0 )

#define sql_create_complete_13(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9,\
    NAME##_##I10,\
    NAME##_##I11,\
    NAME##_##I12,\
    NAME##_##I13 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, 0, 0, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, 0, 0, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 13 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(13, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(13, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(13, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(13, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(13, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(13, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.manip << obj.obj->get_##I13(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8] << obj.delem;\
    s << obj.manip << obj.obj->names[9] << obj.delem;\
    s << obj.manip << obj.obj->names[10] << obj.delem;\
    s << obj.manip << obj.obj->names[11] << obj.delem;\
    s << obj.manip << obj.obj->names[12]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_##I13(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I13();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[9];\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[10];\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[11];\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[12];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_I13();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 13 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  ret.push_back( coltypes[9] );\
  ret.push_back( coltypes[10] );\
  ret.push_back( coltypes[11] );\
  ret.push_back( coltypes[12] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  ret.push_back( names[9] );\
  ret.push_back( names[10] );\
  ret.push_back( names[11] );\
  ret.push_back( names[12] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, 0, 0, 0 )

\
#define sql_define_basic_13(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9 ,\
    N10 ,\
    N11 ,\
    N12 ,\
    N13  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9,\
    #T10,\
    #T11,\
    #T12,\
    #T13 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_13(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13) \
  sql_create_basic_c_order_13(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8, T10, I10, 9, T11, I11, 10, T12, I12, 11, T13, I13, 12)

#define sql_create_13(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13) \
  sql_create_complete_13(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, T13, I13, #I13, 12) \

#define DONTUSE_sql_create_c_order_13(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13) \
  sql_create_complete_13(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9, T10, I10, #I10, O10, T11, I11, #I11, O11, T12, I12, #I12, O12, T13, I13, #I13, O13)

#define DONTUSE_sql_create_c_names_13(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9, T10, I10, N10, T11, I11, N11, T12, I12, N12, T13, I13, N13) \
  sql_create_complete_13(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8, T10, I10, N10, 9, T11, I11, N11, 10, T12, I12, N12, 11, T13, I13, N13, 12)

#define CREATE_TABLE_12(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12) \
	sql_create_13(NAME##Rowdata, 1, 13, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12); \


#define DEFINE_TABLE_12(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12) \
	sql_define_basic_13(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, NAME); \


// ---------------------------------------------------
//                  End Create 13
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 14
// ---------------------------------------------------
#define sql_create_basic_c_order_14(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13, T14, I14, O14)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, 0, 0, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 14 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
    I14 = rhs.I14;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
    inline T14 get_##I14( void ) const { return I14; };\
    inline void set_##I14( T14 set ){ I14 = set; };\
    inline void set_##I14( mysqlpp::String set ){ I14 = (T14)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13;\
    T14 I14; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] );\
    s->set_##I14( row[ O14 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, 0, 0 )

#define sql_create_complete_14(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13, T14, I14, N14, O14) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9,\
    NAME##_##I10,\
    NAME##_##I11,\
    NAME##_##I12,\
    NAME##_##I13,\
    NAME##_##I14 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, 0, 0, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, 0, 0, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 14 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
    I14 = rhs.I14;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
    inline T14 get_##I14( void ) const { return I14; };\
    inline void set_##I14( T14 set ){ I14 = set; };\
    inline void set_##I14( mysqlpp::String set ){ I14 = (T14)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13;\
    T14 I14; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(14, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(14, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(14, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(14, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(14, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(14, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.manip << obj.obj->get_##I13() << obj.delem;\
    s << obj.manip << obj.obj->get_##I14(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8] << obj.delem;\
    s << obj.manip << obj.obj->names[9] << obj.delem;\
    s << obj.manip << obj.obj->names[10] << obj.delem;\
    s << obj.manip << obj.obj->names[11] << obj.delem;\
    s << obj.manip << obj.obj->names[12] << obj.delem;\
    s << obj.manip << obj.obj->names[13]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_##I13() << obj.delem;\
    s << obj.obj->names[13] << obj.comp << obj.manip << obj.obj->get_##I14(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I13();\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I14();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[9];\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[10];\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[11];\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[12];\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[13];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_I13();\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[13] << obj.comp << obj.manip << obj.obj->get_I14();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] );\
    s->set_##I14( row[ O14 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 14 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  ret.push_back( coltypes[9] );\
  ret.push_back( coltypes[10] );\
  ret.push_back( coltypes[11] );\
  ret.push_back( coltypes[12] );\
  ret.push_back( coltypes[13] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  ret.push_back( names[9] );\
  ret.push_back( names[10] );\
  ret.push_back( names[11] );\
  ret.push_back( names[12] );\
  ret.push_back( names[13] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, 0, 0 )

\
#define sql_define_basic_14(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13, T14, I14, N14, O14, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9 ,\
    N10 ,\
    N11 ,\
    N12 ,\
    N13 ,\
    N14  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9,\
    #T10,\
    #T11,\
    #T12,\
    #T13,\
    #T14 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_14(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14) \
  sql_create_basic_c_order_14(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8, T10, I10, 9, T11, I11, 10, T12, I12, 11, T13, I13, 12, T14, I14, 13)

#define sql_create_14(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14) \
  sql_create_complete_14(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, T13, I13, #I13, 12, T14, I14, #I14, 13) \

#define DONTUSE_sql_create_c_order_14(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13, T14, I14, O14) \
  sql_create_complete_14(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9, T10, I10, #I10, O10, T11, I11, #I11, O11, T12, I12, #I12, O12, T13, I13, #I13, O13, T14, I14, #I14, O14)

#define DONTUSE_sql_create_c_names_14(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9, T10, I10, N10, T11, I11, N11, T12, I12, N12, T13, I13, N13, T14, I14, N14) \
  sql_create_complete_14(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8, T10, I10, N10, 9, T11, I11, N11, 10, T12, I12, N12, 11, T13, I13, N13, 12, T14, I14, N14, 13)

#define CREATE_TABLE_13(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13) \
	sql_create_14(NAME##Rowdata, 1, 14, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13); \


#define DEFINE_TABLE_13(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13) \
	sql_define_basic_14(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, T13, I13, #I13, 12, NAME); \


// ---------------------------------------------------
//                  End Create 14
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 15
// ---------------------------------------------------
#define sql_create_basic_c_order_15(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13, T14, I14, O14, T15, I15, O15)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, 0, 0)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 15 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
    I14 = rhs.I14;\
    I15 = rhs.I15;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
    inline T14 get_##I14( void ) const { return I14; };\
    inline void set_##I14( T14 set ){ I14 = set; };\
    inline void set_##I14( mysqlpp::String set ){ I14 = (T14)set; };\
    inline T15 get_##I15( void ) const { return I15; };\
    inline void set_##I15( T15 set ){ I15 = set; };\
    inline void set_##I15( mysqlpp::String set ){ I15 = (T15)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13;\
    T14 I14;\
    T15 I15; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] );\
    s->set_##I14( row[ O14 ] );\
    s->set_##I15( row[ O15 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, 0 )

#define sql_create_complete_15(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13, T14, I14, N14, O14, T15, I15, N15, O15) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9,\
    NAME##_##I10,\
    NAME##_##I11,\
    NAME##_##I12,\
    NAME##_##I13,\
    NAME##_##I14,\
    NAME##_##I15 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, 0, 0)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, 0, 0)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 15 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
    I14 = rhs.I14;\
    I15 = rhs.I15;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
    inline T14 get_##I14( void ) const { return I14; };\
    inline void set_##I14( T14 set ){ I14 = set; };\
    inline void set_##I14( mysqlpp::String set ){ I14 = (T14)set; };\
    inline T15 get_##I15( void ) const { return I15; };\
    inline void set_##I15( T15 set ){ I15 = set; };\
    inline void set_##I15( mysqlpp::String set ){ I15 = (T15)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13;\
    T14 I14;\
    T15 I15; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(15, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
    if (i15) (*include)[14]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(15, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
    if (i15 == NAME##_NULL) return;\
    (*include)[i15]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(15, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
    if (i15) (*include)[14]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(15, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
    if (i15 == NAME##_NULL) return;\
    (*include)[i15]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(15, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
    if (i15) (*include)[14]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(15, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
    if (i15 == NAME##_NULL) return;\
    (*include)[i15]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.manip << obj.obj->get_##I13() << obj.delem;\
    s << obj.manip << obj.obj->get_##I14() << obj.delem;\
    s << obj.manip << obj.obj->get_##I15(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8] << obj.delem;\
    s << obj.manip << obj.obj->names[9] << obj.delem;\
    s << obj.manip << obj.obj->names[10] << obj.delem;\
    s << obj.manip << obj.obj->names[11] << obj.delem;\
    s << obj.manip << obj.obj->names[12] << obj.delem;\
    s << obj.manip << obj.obj->names[13] << obj.delem;\
    s << obj.manip << obj.obj->names[14]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_##I13() << obj.delem;\
    s << obj.obj->names[13] << obj.comp << obj.manip << obj.obj->get_##I14() << obj.delem;\
    s << obj.obj->names[14] << obj.comp << obj.manip << obj.obj->get_##I15(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I13();\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I14();\
      before = true; \
     } \
    if ((*obj.include)[14]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I15();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[9];\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[10];\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[11];\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[12];\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[13];\
      before = true; \
     } \
    if ((*obj.include)[14]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[14];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_I13();\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[13] << obj.comp << obj.manip << obj.obj->get_I14();\
      before = true; \
     } \
    if ((*obj.include)[14]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[14] << obj.comp << obj.manip << obj.obj->get_I15();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] );\
    s->set_##I14( row[ O14 ] );\
    s->set_##I15( row[ O15 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 15 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  ret.push_back( coltypes[9] );\
  ret.push_back( coltypes[10] );\
  ret.push_back( coltypes[11] );\
  ret.push_back( coltypes[12] );\
  ret.push_back( coltypes[13] );\
  ret.push_back( coltypes[14] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  ret.push_back( names[9] );\
  ret.push_back( names[10] );\
  ret.push_back( names[11] );\
  ret.push_back( names[12] );\
  ret.push_back( names[13] );\
  ret.push_back( names[14] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, 0 )

\
#define sql_define_basic_15(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13, T14, I14, N14, O14, T15, I15, N15, O15, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9 ,\
    N10 ,\
    N11 ,\
    N12 ,\
    N13 ,\
    N14 ,\
    N15  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9,\
    #T10,\
    #T11,\
    #T12,\
    #T13,\
    #T14,\
    #T15 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_15(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15) \
  sql_create_basic_c_order_15(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8, T10, I10, 9, T11, I11, 10, T12, I12, 11, T13, I13, 12, T14, I14, 13, T15, I15, 14)

#define sql_create_15(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15) \
  sql_create_complete_15(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, T13, I13, #I13, 12, T14, I14, #I14, 13, T15, I15, #I15, 14) \

#define DONTUSE_sql_create_c_order_15(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13, T14, I14, O14, T15, I15, O15) \
  sql_create_complete_15(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9, T10, I10, #I10, O10, T11, I11, #I11, O11, T12, I12, #I12, O12, T13, I13, #I13, O13, T14, I14, #I14, O14, T15, I15, #I15, O15)

#define DONTUSE_sql_create_c_names_15(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9, T10, I10, N10, T11, I11, N11, T12, I12, N12, T13, I13, N13, T14, I14, N14, T15, I15, N15) \
  sql_create_complete_15(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8, T10, I10, N10, 9, T11, I11, N11, 10, T12, I12, N12, 11, T13, I13, N13, 12, T14, I14, N14, 13, T15, I15, N15, 14)

#define CREATE_TABLE_14(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14) \
	sql_create_15(NAME##Rowdata, 1, 15, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14); \


#define DEFINE_TABLE_14(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14) \
	sql_define_basic_15(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, T13, I13, #I13, 12, T14, I14, #I14, 13, NAME); \


// ---------------------------------------------------
//                  End Create 15
// ---------------------------------------------------

// ---------------------------------------------------
//                  Begin Create 16
// ---------------------------------------------------
#define sql_create_basic_c_order_16(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13, T14, I14, O14, T15, I15, O15, T16, I16, O16)\
  class NAME; \
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, T16, I16)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, T16, I16)\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 16 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
    I14 = rhs.I14;\
    I15 = rhs.I15;\
    I16 = rhs.I16;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
    inline T14 get_##I14( void ) const { return I14; };\
    inline void set_##I14( T14 set ){ I14 = set; };\
    inline void set_##I14( mysqlpp::String set ){ I14 = (T14)set; };\
    inline T15 get_##I15( void ) const { return I15; };\
    inline void set_##I15( T15 set ){ I15 = set; };\
    inline void set_##I15( mysqlpp::String set ){ I15 = (T15)set; };\
    inline T16 get_##I16( void ) const { return I16; };\
    inline void set_##I16( T16 set ){ I16 = set; };\
    inline void set_##I16( mysqlpp::String set ){ I16 = (T16)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13;\
    T14 I14;\
    T15 I15;\
    T16 I16; \
  }; \
  template <sql_dummy_type dummy> \
    void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] );\
    s->set_##I14( row[ O14 ] );\
    s->set_##I15( row[ O15 ] );\
    s->set_##I16( row[ O16 ] ); \
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
    {populate_##NAME<sql_dummy>(this, row);} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, I16 )

#define sql_create_complete_16(NAME, CMP, CONTR, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13, T14, I14, N14, O14, T15, I15, N15, O15, T16, I16, N16, O16) \
  class NAME; \
  enum NAME##_enum { \
    NAME##_##I1,\
    NAME##_##I2,\
    NAME##_##I3,\
    NAME##_##I4,\
    NAME##_##I5,\
    NAME##_##I6,\
    NAME##_##I7,\
    NAME##_##I8,\
    NAME##_##I9,\
    NAME##_##I10,\
    NAME##_##I11,\
    NAME##_##I12,\
    NAME##_##I13,\
    NAME##_##I14,\
    NAME##_##I15,\
    NAME##_##I16 \
    , NAME##_NULL \
  }; \
  template <class Manip>\
  class NAME##_value_list { \
  /*friend ostream& operator << <> (ostream&, const NAME##_value_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_value_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_##field_list {\
  /* friend ostream& operator << <> (ostream&, const NAME##_field_list&); */\
  public:  \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
  public:  \
    NAME##_field_list (const NAME *o, const char *d, Manip m) \
      : obj(o), delem(d), manip(m) {} \
  };\
  template <class Manip>\
  class NAME##_equal_list { \
  /* friend ostream& operator << <> (ostream&, const NAME##_equal_list&); */\
  public:  \
    const NAME *obj;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public:  \
    NAME##_equal_list (const NAME *o, const char *d, const char *c, Manip m) \
      : obj(o), delem(d), comp(c), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_value_list {\
  /* friend ostream& operator << <> (ostream&, \
  				  const NAME##_cus_value_list<Manip>&); */\
  public: \
    const NAME *obj;\
    const char *delem;\
    Manip manip;\
    vector<bool> *include;\
    bool del_vector;\
  public:  \
    ~NAME##_cus_value_list () {if (del_vector) delete include;} \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16);\
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16); \
    NAME##_cus_value_list (const NAME *o, const char *d, Manip m ,vector<bool> i)\
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
  template <class Manip>\
  class NAME##_cus_field_list { \
  /* friend ostream& operator << <> (ostream&, \
     				  const NAME##_cus_field_list<Manip>&); */\
  public: \
    const NAME *obj; \
    const char *delem;\
    Manip manip;\
    vector<bool> *include; \
    bool del_vector; \
  public:  \
    ~NAME##_cus_field_list () {if (del_vector) delete include;} \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16); \
    NAME##_cus_field_list (const NAME *o, const char *d, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), manip(m) {}\
  };\
 template <class Manip>\
 class NAME##_cus_equal_list {\
 /* friend ostream& operator << <> (ostream&, \
				  const NAME##_cus_equal_list<Manip>&); */\
  public: \
    const NAME *obj;\
    vector<bool> *include;\
    bool del_vector;\
    const char *delem;\
    const char *comp;\
    Manip manip;\
  public: \
    ~NAME##_##cus_equal_list () {if (del_vector) delete include;}\
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16); \
    NAME##_##cus_equal_list (const NAME *o, const char *d, const char *c, Manip m, vector<bool> *i) \
      : obj(o), include(i), del_vector(false), delem(d), comp(c), manip(m) {}\
  };\
  template <sql_dummy_type dummy> int sql_compare_##NAME (const NAME &, const NAME &);\
  class NAME : public TableRowdataInterface { \
	public: \
    NAME () {} \
    NAME (const mysqlpp::Row &row);\
		virtual ~NAME() {};\
    void set (const mysqlpp::Row &row);\
    sql_compare_define_##CMP(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, T16, I16)\
    sql_constructor_##CONTR(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, T16, I16)\
    static char *names[];\
    static char *coltypes[];\
    static char *_table;\
    static char *& table() {return _table;}\
/*! this isnt a custom table */ \
virtual bool custom( void ) { return false; } \
/*! get a vector of the 16 strings of the column datatypes */\
virtual vector<string> getColumnDatatypes( void ); \
/*! get a vector of strings of the column names */\
virtual vector<string> getColumnNames( void );\
/*! get the name of the table */ \
virtual  char *getTableName(); \
inline NAME & operator = (NAME const &rhs) \
    {\
    I1 = rhs.I1;\
    I2 = rhs.I2;\
    I3 = rhs.I3;\
    I4 = rhs.I4;\
    I5 = rhs.I5;\
    I6 = rhs.I6;\
    I7 = rhs.I7;\
    I8 = rhs.I8;\
    I9 = rhs.I9;\
    I10 = rhs.I10;\
    I11 = rhs.I11;\
    I12 = rhs.I12;\
    I13 = rhs.I13;\
    I14 = rhs.I14;\
    I15 = rhs.I15;\
    I16 = rhs.I16;\
		 return *this; };\
    inline T1 get_##I1( void ) const { return I1; };\
    inline void set_##I1( T1 set ){ I1 = set; };\
    inline void set_##I1( mysqlpp::String set ){ I1 = (T1)set; };\
    inline T2 get_##I2( void ) const { return I2; };\
    inline void set_##I2( T2 set ){ I2 = set; };\
    inline void set_##I2( mysqlpp::String set ){ I2 = (T2)set; };\
    inline T3 get_##I3( void ) const { return I3; };\
    inline void set_##I3( T3 set ){ I3 = set; };\
    inline void set_##I3( mysqlpp::String set ){ I3 = (T3)set; };\
    inline T4 get_##I4( void ) const { return I4; };\
    inline void set_##I4( T4 set ){ I4 = set; };\
    inline void set_##I4( mysqlpp::String set ){ I4 = (T4)set; };\
    inline T5 get_##I5( void ) const { return I5; };\
    inline void set_##I5( T5 set ){ I5 = set; };\
    inline void set_##I5( mysqlpp::String set ){ I5 = (T5)set; };\
    inline T6 get_##I6( void ) const { return I6; };\
    inline void set_##I6( T6 set ){ I6 = set; };\
    inline void set_##I6( mysqlpp::String set ){ I6 = (T6)set; };\
    inline T7 get_##I7( void ) const { return I7; };\
    inline void set_##I7( T7 set ){ I7 = set; };\
    inline void set_##I7( mysqlpp::String set ){ I7 = (T7)set; };\
    inline T8 get_##I8( void ) const { return I8; };\
    inline void set_##I8( T8 set ){ I8 = set; };\
    inline void set_##I8( mysqlpp::String set ){ I8 = (T8)set; };\
    inline T9 get_##I9( void ) const { return I9; };\
    inline void set_##I9( T9 set ){ I9 = set; };\
    inline void set_##I9( mysqlpp::String set ){ I9 = (T9)set; };\
    inline T10 get_##I10( void ) const { return I10; };\
    inline void set_##I10( T10 set ){ I10 = set; };\
    inline void set_##I10( mysqlpp::String set ){ I10 = (T10)set; };\
    inline T11 get_##I11( void ) const { return I11; };\
    inline void set_##I11( T11 set ){ I11 = set; };\
    inline void set_##I11( mysqlpp::String set ){ I11 = (T11)set; };\
    inline T12 get_##I12( void ) const { return I12; };\
    inline void set_##I12( T12 set ){ I12 = set; };\
    inline void set_##I12( mysqlpp::String set ){ I12 = (T12)set; };\
    inline T13 get_##I13( void ) const { return I13; };\
    inline void set_##I13( T13 set ){ I13 = set; };\
    inline void set_##I13( mysqlpp::String set ){ I13 = (T13)set; };\
    inline T14 get_##I14( void ) const { return I14; };\
    inline void set_##I14( T14 set ){ I14 = set; };\
    inline void set_##I14( mysqlpp::String set ){ I14 = (T14)set; };\
    inline T15 get_##I15( void ) const { return I15; };\
    inline void set_##I15( T15 set ){ I15 = set; };\
    inline void set_##I15( mysqlpp::String set ){ I15 = (T15)set; };\
    inline T16 get_##I16( void ) const { return I16; };\
    inline void set_##I16( T16 set ){ I16 = set; };\
    inline void set_##I16( mysqlpp::String set ){ I16 = (T16)set; };\
private:  \
    T1 I1;\
    T2 I2;\
    T3 I3;\
    T4 I4;\
    T5 I5;\
    T6 I6;\
    T7 I7;\
    T8 I8;\
    T9 I9;\
    T10 I10;\
    T11 I11;\
    T12 I12;\
    T13 I13;\
    T14 I14;\
    T15 I15;\
    T16 I16; \
public: \
    NAME##_value_list<quote_type0> value_list() const {\
      return value_list(",", mysqlpp::quote);}\
    NAME##_value_list<quote_type0> value_list(const char *d) const {\
      return value_list(d, mysqlpp::quote);}\
    template <class Manip> \
    NAME##_value_list<Manip> value_list(const char *d, Manip m) const; \
    NAME##_field_list<do_nothing_type0> field_list() const {\
      return field_list(",", mysqlpp::do_nothing);}\
    NAME##_field_list<do_nothing_type0> field_list(const char *d) const {\
      return field_list(d, mysqlpp::do_nothing);}\
    template <class Manip>\
    NAME##_field_list<Manip> field_list(const char *d, Manip m) const; \
    NAME##_equal_list<quote_type0> equal_list(const char *d = ",", \
                                              const char *c = " = ") const{\
      return equal_list(d, c, mysqlpp::quote);}\
    template <class Manip>\
    NAME##_equal_list<Manip> equal_list(const char *d, const char *c, Manip m) const; \
    /* cus_data */\
    NAME##_cus_value_list<quote_type0> value_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const {\
      return value_list(",", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(vector<bool> *i) const {\
      return value_list(",", mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(mysqlpp::sql_cmp_type sc) const {\
      return value_list(",", mysqlpp::quote, sc);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const {\
      return value_list(d, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						vector<bool> *i) const {\
      return value_list(d, mysqlpp::quote, i);\
    }\
    NAME##_cus_value_list<quote_type0> value_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return value_list(d, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					  bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m,\
					    NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_value_list<Manip> value_list(const char *d, Manip m, \
					  mysqlpp::sql_cmp_type sc) const;\
    /* cus field */\
    NAME##_cus_field_list<do_nothing_type0> field_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const {\
      return field_list(",", mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(vector<bool> *i) const {\
      return field_list(",", mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(mysqlpp::sql_cmp_type sc) const\
    {\
      return field_list(",", mysqlpp::do_nothing, sc);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						       bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d,\
						         NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const {\
      return field_list(d, mysqlpp::do_nothing, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						vector<bool> *i) const {\
      return field_list(d, mysqlpp::do_nothing, i);\
    }\
    NAME##_cus_field_list<do_nothing_type0> field_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return field_list(d, mysqlpp::do_nothing, sc);\
    }\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m,\
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_field_list<Manip> field_list(const char *d, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
    /* cus equal */\
    NAME##_cus_equal_list<quote_type0> equal_list(bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const {\
      return equal_list(",", " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(vector<bool> *i) const {\
      return equal_list(",", " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(mysqlpp::sql_cmp_type sc) const {\
      return equal_list(",", " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d,   NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const {\
      return equal_list(d, " = ", mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						vector<bool> *i) const {\
      return equal_list(d, " = ", mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, \
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, " = ", mysqlpp::quote, sc);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
                                                  NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const {\
      return equal_list(d, c, mysqlpp::quote, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						vector<bool> *i) const {\
      return equal_list(d, c, mysqlpp::quote, i);\
    }\
    NAME##_cus_equal_list<quote_type0> equal_list(const char *d, const char *c,\
						mysqlpp::sql_cmp_type sc) const {\
      return equal_list(d, c, mysqlpp::quote, sc);\
    }\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    bool i1, bool i2 = false, bool i3 = false, bool i4 = false, bool i5 = false, bool i6 = false, bool i7 = false, bool i8 = false, bool i9 = false, bool i10 = false, bool i11 = false, bool i12 = false, bool i13 = false, bool i14 = false, bool i15 = false, bool i16 = false) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					      NAME##_enum i1,   NAME##_enum i2 =   NAME##_NULL,   NAME##_enum i3 =   NAME##_NULL,   NAME##_enum i4 =   NAME##_NULL,   NAME##_enum i5 =   NAME##_NULL,   NAME##_enum i6 =   NAME##_NULL,   NAME##_enum i7 =   NAME##_NULL,   NAME##_enum i8 =   NAME##_NULL,   NAME##_enum i9 =   NAME##_NULL,   NAME##_enum i10 =   NAME##_NULL,   NAME##_enum i11 =   NAME##_NULL,   NAME##_enum i12 =   NAME##_NULL,   NAME##_enum i13 =   NAME##_NULL,   NAME##_enum i14 =   NAME##_NULL,   NAME##_enum i15 =   NAME##_NULL,   NAME##_enum i16 =   NAME##_NULL) const; \
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    vector<bool> *i) const;\
    template <class Manip>\
    NAME##_cus_equal_list<Manip> equal_list(const char *d, const char *c, Manip m, \
					    mysqlpp::sql_cmp_type sc) const;\
  }; \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16) \
  { \
    delem = d;\
    manip = m;\
    del_vector = true;\
    obj = o; \
    include = new vector<bool>(16, false);\
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
    if (i15) (*include)[14]=true;\
    if (i16) (*include)[15]=true;\
  } \
  template <class Manip>\
  NAME##_cus_value_list<Manip>::NAME##_cus_value_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(16, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
    if (i15 == NAME##_NULL) return;\
    (*include)[i15]=true;\
    if (i16 == NAME##_NULL) return;\
    (*include)[i16]=true;\
  }\
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16) {\
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(16, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
    if (i15) (*include)[14]=true;\
    if (i16) (*include)[15]=true;\
  } \
  template <class Manip>\
  NAME##_cus_field_list<Manip>::NAME##_cus_field_list\
  (const NAME *o, const char *d, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16) { \
    delem = d;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(16, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
    if (i15 == NAME##_NULL) return;\
    (*include)[i15]=true;\
    if (i16 == NAME##_NULL) return;\
    (*include)[i16]=true;\
  }\
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m, bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(16, false); \
    if (i1) (*include)[0]=true;\
    if (i2) (*include)[1]=true;\
    if (i3) (*include)[2]=true;\
    if (i4) (*include)[3]=true;\
    if (i5) (*include)[4]=true;\
    if (i6) (*include)[5]=true;\
    if (i7) (*include)[6]=true;\
    if (i8) (*include)[7]=true;\
    if (i9) (*include)[8]=true;\
    if (i10) (*include)[9]=true;\
    if (i11) (*include)[10]=true;\
    if (i12) (*include)[11]=true;\
    if (i13) (*include)[12]=true;\
    if (i14) (*include)[13]=true;\
    if (i15) (*include)[14]=true;\
    if (i16) (*include)[15]=true;\
  } \
  template <class Manip>\
  NAME##_cus_equal_list<Manip>::NAME##_cus_equal_list\
  (const NAME *o, const char *d, const char *c, Manip m,   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16) { \
    delem = d;\
    comp = c;\
    manip = m;\
    del_vector = true; \
    obj = o; \
    include = new vector<bool>(16, false); \
    if (i1 == NAME##_NULL) return;\
    (*include)[i1]=true;\
    if (i2 == NAME##_NULL) return;\
    (*include)[i2]=true;\
    if (i3 == NAME##_NULL) return;\
    (*include)[i3]=true;\
    if (i4 == NAME##_NULL) return;\
    (*include)[i4]=true;\
    if (i5 == NAME##_NULL) return;\
    (*include)[i5]=true;\
    if (i6 == NAME##_NULL) return;\
    (*include)[i6]=true;\
    if (i7 == NAME##_NULL) return;\
    (*include)[i7]=true;\
    if (i8 == NAME##_NULL) return;\
    (*include)[i8]=true;\
    if (i9 == NAME##_NULL) return;\
    (*include)[i9]=true;\
    if (i10 == NAME##_NULL) return;\
    (*include)[i10]=true;\
    if (i11 == NAME##_NULL) return;\
    (*include)[i11]=true;\
    if (i12 == NAME##_NULL) return;\
    (*include)[i12]=true;\
    if (i13 == NAME##_NULL) return;\
    (*include)[i13]=true;\
    if (i14 == NAME##_NULL) return;\
    (*include)[i14]=true;\
    if (i15 == NAME##_NULL) return;\
    (*include)[i15]=true;\
    if (i16 == NAME##_NULL) return;\
    (*include)[i16]=true;\
  }\
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_value_list<Manip>& obj) { \
    s << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.manip << obj.obj->get_##I13() << obj.delem;\
    s << obj.manip << obj.obj->get_##I14() << obj.delem;\
    s << obj.manip << obj.obj->get_##I15() << obj.delem;\
    s << obj.manip << obj.obj->get_##I16(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_field_list<Manip>& obj) { \
    s << obj.manip << obj.obj->names[0] << obj.delem;\
    s << obj.manip << obj.obj->names[1] << obj.delem;\
    s << obj.manip << obj.obj->names[2] << obj.delem;\
    s << obj.manip << obj.obj->names[3] << obj.delem;\
    s << obj.manip << obj.obj->names[4] << obj.delem;\
    s << obj.manip << obj.obj->names[5] << obj.delem;\
    s << obj.manip << obj.obj->names[6] << obj.delem;\
    s << obj.manip << obj.obj->names[7] << obj.delem;\
    s << obj.manip << obj.obj->names[8] << obj.delem;\
    s << obj.manip << obj.obj->names[9] << obj.delem;\
    s << obj.manip << obj.obj->names[10] << obj.delem;\
    s << obj.manip << obj.obj->names[11] << obj.delem;\
    s << obj.manip << obj.obj->names[12] << obj.delem;\
    s << obj.manip << obj.obj->names[13] << obj.delem;\
    s << obj.manip << obj.obj->names[14] << obj.delem;\
    s << obj.manip << obj.obj->names[15]; \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_equal_list<Manip>& obj) { \
    s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_##I1() << obj.delem;\
    s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_##I2() << obj.delem;\
    s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_##I3() << obj.delem;\
    s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_##I4() << obj.delem;\
    s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_##I5() << obj.delem;\
    s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_##I6() << obj.delem;\
    s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_##I7() << obj.delem;\
    s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_##I8() << obj.delem;\
    s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_##I9() << obj.delem;\
    s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_##I10() << obj.delem;\
    s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_##I11() << obj.delem;\
    s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_##I12() << obj.delem;\
    s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_##I13() << obj.delem;\
    s << obj.obj->names[13] << obj.comp << obj.manip << obj.obj->get_##I14() << obj.delem;\
    s << obj.obj->names[14] << obj.comp << obj.manip << obj.obj->get_##I15() << obj.delem;\
    s << obj.obj->names[15] << obj.comp << obj.manip << obj.obj->get_##I16(); \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_value_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->get_##I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I13();\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I14();\
      before = true; \
     } \
    if ((*obj.include)[14]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I15();\
      before = true; \
     } \
    if ((*obj.include)[15]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->get_##I16();\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_field_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.manip << obj.obj->names[0];\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[1];\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[2];\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[3];\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[4];\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[5];\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[6];\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[7];\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[8];\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[9];\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[10];\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[11];\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[12];\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[13];\
      before = true; \
     } \
    if ((*obj.include)[14]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[14];\
      before = true; \
     } \
    if ((*obj.include)[15]) { \
      if (before) s << obj.delem;\
      s << obj.manip << obj.obj->names[15];\
     } \
    return s; \
  } \
  template <class Manip>\
  ostream& operator << (ostream& s, const NAME##_cus_equal_list<Manip>& obj) { \
    bool before = false; \
    if ((*obj.include)[0]) { \
      s << obj.obj->names[0] << obj.comp << obj.manip << obj.obj->get_I1();\
      before = true; \
     } \
    if ((*obj.include)[1]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[1] << obj.comp << obj.manip << obj.obj->get_I2();\
      before = true; \
     } \
    if ((*obj.include)[2]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[2] << obj.comp << obj.manip << obj.obj->get_I3();\
      before = true; \
     } \
    if ((*obj.include)[3]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[3] << obj.comp << obj.manip << obj.obj->get_I4();\
      before = true; \
     } \
    if ((*obj.include)[4]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[4] << obj.comp << obj.manip << obj.obj->get_I5();\
      before = true; \
     } \
    if ((*obj.include)[5]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[5] << obj.comp << obj.manip << obj.obj->get_I6();\
      before = true; \
     } \
    if ((*obj.include)[6]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[6] << obj.comp << obj.manip << obj.obj->get_I7();\
      before = true; \
     } \
    if ((*obj.include)[7]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[7] << obj.comp << obj.manip << obj.obj->get_I8();\
      before = true; \
     } \
    if ((*obj.include)[8]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[8] << obj.comp << obj.manip << obj.obj->get_I9();\
      before = true; \
     } \
    if ((*obj.include)[9]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[9] << obj.comp << obj.manip << obj.obj->get_I10();\
      before = true; \
     } \
    if ((*obj.include)[10]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[10] << obj.comp << obj.manip << obj.obj->get_I11();\
      before = true; \
     } \
    if ((*obj.include)[11]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[11] << obj.comp << obj.manip << obj.obj->get_I12();\
      before = true; \
     } \
    if ((*obj.include)[12]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[12] << obj.comp << obj.manip << obj.obj->get_I13();\
      before = true; \
     } \
    if ((*obj.include)[13]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[13] << obj.comp << obj.manip << obj.obj->get_I14();\
      before = true; \
     } \
    if ((*obj.include)[14]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[14] << obj.comp << obj.manip << obj.obj->get_I15();\
      before = true; \
     } \
    if ((*obj.include)[15]) { \
      if (before) s << obj.delem;\
      s << obj.obj->names[15] << obj.comp << obj.manip << obj.obj->get_I16();\
     } \
    return s; \
  } \
  template <class Manip>\
  inline NAME##_value_list<Manip> NAME::value_list(const char *d, Manip m) const { \
    return NAME##_value_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_field_list<Manip> NAME::field_list(const char *d, Manip m) const { \
    return NAME##_field_list<Manip> (this, d, m); \
  } \
  template <class Manip>\
  inline NAME##_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m) const { \
    return NAME##_equal_list<Manip> (this, d, c, m); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						       bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16) const {\
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16) const { \
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m,\
							 bool i1, bool i2, bool i3, bool i4, bool i5, bool i6, bool i7, bool i8, bool i9, bool i10, bool i11, bool i12, bool i13, bool i14, bool i15, bool i16) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> NAME::value_list(const char *d, Manip m,\
						         NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16) const { \
    return NAME##_cus_value_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16); \
  } \
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> NAME::field_list(const char *d, Manip m,\
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16) const {\
    return NAME##_cus_field_list<Manip> (this, d, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16); \
  } \
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> NAME::equal_list(const char *d, const char *c, Manip m, \
							   NAME##_enum i1,   NAME##_enum i2,   NAME##_enum i3,   NAME##_enum i4,   NAME##_enum i5,   NAME##_enum i6,   NAME##_enum i7,   NAME##_enum i8,   NAME##_enum i9,   NAME##_enum i10,   NAME##_enum i11,   NAME##_enum i12,   NAME##_enum i13,   NAME##_enum i14,   NAME##_enum i15,   NAME##_enum i16) const { \
    return NAME##_cus_equal_list<Manip> (this, d, c, m, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16); \
  } \
  template <class Manip>\
  inline NAME##_cus_value_list<Manip> \
  NAME::value_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, value, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_field_list<Manip> \
  NAME::field_list(const char *d, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_def_##CMP (NAME, field, NUM);\
  }\
  template <class Manip>\
  inline NAME##_cus_equal_list<Manip> \
  NAME::equal_list(const char *d, const char *c, Manip m, mysqlpp::sql_cmp_type sc) const {\
    sql_compare_type_defe_##CMP (NAME, equal, NUM);\
  }\
  template <sql_dummy_type dummy> \
  void populate_##NAME (NAME *s, const mysqlpp::Row &row) { \
    s->set_##I1( row[ O1 ] );\
    s->set_##I2( row[ O2 ] );\
    s->set_##I3( row[ O3 ] );\
    s->set_##I4( row[ O4 ] );\
    s->set_##I5( row[ O5 ] );\
    s->set_##I6( row[ O6 ] );\
    s->set_##I7( row[ O7 ] );\
    s->set_##I8( row[ O8 ] );\
    s->set_##I9( row[ O9 ] );\
    s->set_##I10( row[ O10 ] );\
    s->set_##I11( row[ O11 ] );\
    s->set_##I12( row[ O12 ] );\
    s->set_##I13( row[ O13 ] );\
    s->set_##I14( row[ O14 ] );\
    s->set_##I15( row[ O15 ] );\
    s->set_##I16( row[ O16 ] );\
  } \
  inline NAME::NAME (const mysqlpp::Row &row) \
                                        {populate_##NAME<sql_dummy>(this, row);}\
  inline void NAME::set (const mysqlpp::Row &row)\
                                        {populate_##NAME<sql_dummy>(this, row);}\
/*! get the name of the table */ \
inline  char *NAME::getTableName() { return _table; } \
/* get a vector of the 16 strings of the column datatypes */\
inline vector<string> NAME::getColumnDatatypes( void ) { \
  vector<string> ret;\
  ret.push_back( coltypes[0] );\
  ret.push_back( coltypes[1] );\
  ret.push_back( coltypes[2] );\
  ret.push_back( coltypes[3] );\
  ret.push_back( coltypes[4] );\
  ret.push_back( coltypes[5] );\
  ret.push_back( coltypes[6] );\
  ret.push_back( coltypes[7] );\
  ret.push_back( coltypes[8] );\
  ret.push_back( coltypes[9] );\
  ret.push_back( coltypes[10] );\
  ret.push_back( coltypes[11] );\
  ret.push_back( coltypes[12] );\
  ret.push_back( coltypes[13] );\
  ret.push_back( coltypes[14] );\
  ret.push_back( coltypes[15] );\
  return ret;\
} \
/*! get a vector of strings of the column names */\
inline vector<string> NAME::getColumnNames( void ) {\
  vector<string> ret;\
  ret.push_back( names[0] );\
  ret.push_back( names[1] );\
  ret.push_back( names[2] );\
  ret.push_back( names[3] );\
  ret.push_back( names[4] );\
  ret.push_back( names[5] );\
  ret.push_back( names[6] );\
  ret.push_back( names[7] );\
  ret.push_back( names[8] );\
  ret.push_back( names[9] );\
  ret.push_back( names[10] );\
  ret.push_back( names[11] );\
  ret.push_back( names[12] );\
  ret.push_back( names[13] );\
  ret.push_back( names[14] );\
  ret.push_back( names[15] );\
  return ret;\
} \
  sql_COMPARE__##CMP(NAME, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, I16 )

\
#define sql_define_basic_16(NAME, T1, I1, N1, O1, T2, I2, N2, O2, T3, I3, N3, O3, T4, I4, N4, O4, T5, I5, N5, O5, T6, I6, N6, O6, T7, I7, N7, O7, T8, I8, N8, O8, T9, I9, N9, O9, T10, I10, N10, O10, T11, I11, N11, O11, T12, I12, N12, O12, T13, I13, N13, O13, T14, I14, N14, O14, T15, I15, N15, O15, T16, I16, N16, O16, TABLENAME)\
  char *NAME::names[] = { \
    N1 ,\
    N2 ,\
    N3 ,\
    N4 ,\
    N5 ,\
    N6 ,\
    N7 ,\
    N8 ,\
    N9 ,\
    N10 ,\
    N11 ,\
    N12 ,\
    N13 ,\
    N14 ,\
    N15 ,\
    N16  \
  }; \
  char *NAME::coltypes[] = { \
    #T1,\
    #T2,\
    #T3,\
    #T4,\
    #T5,\
    #T6,\
    #T7,\
    #T8,\
    #T9,\
    #T10,\
    #T11,\
    #T12,\
    #T13,\
    #T14,\
    #T15,\
    #T16 \
  }; \
  /*char *NAME::_table = #TABLENAME ; */\
	char *NAME::_table = #TABLENAME; 

#define DONTUSE_sql_create_basic_16(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, T16, I16) \
  sql_create_basic_c_order_16(NAME, CMP, CONTR, T1, I1, 0, T2, I2, 1, T3, I3, 2, T4, I4, 3, T5, I5, 4, T6, I6, 5, T7, I7, 6, T8, I8, 7, T9, I9, 8, T10, I10, 9, T11, I11, 10, T12, I12, 11, T13, I13, 12, T14, I14, 13, T15, I15, 14, T16, I16, 15)

#define sql_create_16(NAME, CMP, CONTR, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15, T16, I16) \
  sql_create_complete_16(NAME, CMP, CONTR, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, T13, I13, #I13, 12, T14, I14, #I14, 13, T15, I15, #I15, 14, T16, I16, #I16, 15) \

#define DONTUSE_sql_create_c_order_16(NAME, CMP, CONTR, T1, I1, O1, T2, I2, O2, T3, I3, O3, T4, I4, O4, T5, I5, O5, T6, I6, O6, T7, I7, O7, T8, I8, O8, T9, I9, O9, T10, I10, O10, T11, I11, O11, T12, I12, O12, T13, I13, O13, T14, I14, O14, T15, I15, O15, T16, I16, O16) \
  sql_create_complete_16(NAME, CMP, CONTR, T1, I1, #I1, O1, T2, I2, #I2, O2, T3, I3, #I3, O3, T4, I4, #I4, O4, T5, I5, #I5, O5, T6, I6, #I6, O6, T7, I7, #I7, O7, T8, I8, #I8, O8, T9, I9, #I9, O9, T10, I10, #I10, O10, T11, I11, #I11, O11, T12, I12, #I12, O12, T13, I13, #I13, O13, T14, I14, #I14, O14, T15, I15, #I15, O15, T16, I16, #I16, O16)

#define DONTUSE_sql_create_c_names_16(NAME, CMP, CONTR, T1, I1, N1, T2, I2, N2, T3, I3, N3, T4, I4, N4, T5, I5, N5, T6, I6, N6, T7, I7, N7, T8, I8, N8, T9, I9, N9, T10, I10, N10, T11, I11, N11, T12, I12, N12, T13, I13, N13, T14, I14, N14, T15, I15, N15, T16, I16, N16) \
  sql_create_complete_16(NAME, CMP, CONTR, T1, I1, N1, 0, T2, I2, N2, 1, T3, I3, N3, 2, T4, I4, N4, 3, T5, I5, N5, 4, T6, I6, N6, 5, T7, I7, N7, 6, T8, I8, N8, 7, T9, I9, N9, 8, T10, I10, N10, 9, T11, I11, N11, 10, T12, I12, N12, 11, T13, I13, N13, 12, T14, I14, N14, 13, T15, I15, N15, 14, T16, I16, N16, 15)

#define CREATE_TABLE_15(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15) \
	sql_create_16(NAME##Rowdata, 1, 16, int,id, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15); \


#define DEFINE_TABLE_15(NAME, T1, I1, T2, I2, T3, I3, T4, I4, T5, I5, T6, I6, T7, I7, T8, I8, T9, I9, T10, I10, T11, I11, T12, I12, T13, I13, T14, I14, T15, I15) \
	sql_define_basic_16(NAME##Rowdata, int,id,"id",, T1, I1, #I1, 0, T2, I2, #I2, 1, T3, I3, #I3, 2, T4, I4, #I4, 3, T5, I5, #I5, 4, T6, I6, #I6, 5, T7, I7, #I7, 6, T8, I8, #I8, 7, T9, I9, #I9, 8, T10, I10, #I10, 9, T11, I11, #I11, 10, T12, I12, #I12, 11, T13, I13, #I13, 12, T14, I14, #I14, 13, T15, I15, #I15, 14, NAME); \


// ---------------------------------------------------
//                  End Create 16
// ---------------------------------------------------



#endif
// end of automatically created file

