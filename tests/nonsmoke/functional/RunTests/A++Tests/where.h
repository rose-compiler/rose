// Allow repeated includes of where.h without error
#ifndef _APP_WHERE_H
#define _APP_WHERE_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

#define STOP_WHERE_MACRO  1
#define START_WHERE_MACRO 0

#define where(MASK)     for(Where_Statement_Support::APP_Global_Where_Var=where_function(MASK); \
                            Where_Statement_Support::APP_Global_Where_Var == START_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#define elsewhere(MASK) for(Where_Statement_Support::APP_Global_Where_Var=elsewhere_function(MASK); \
                            Where_Statement_Support::APP_Global_Where_Var != STOP_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#define otherwise() for(Where_Statement_Support::APP_Global_Where_Var=elsewhere_function(); \
                            Where_Statement_Support::APP_Global_Where_Var != STOP_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#if 0
#ifdef CRAY
#define elsewhere_mask(MASK) for(Where_Statement_Support::APP_Global_Where_Var=elsewhere_function(MASK); \
                            Where_Statement_Support::APP_Global_Where_Var != STOP_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#define elsewhere() for(Where_Statement_Support::APP_Global_Where_Var=elsewhere_function(); \
                            Where_Statement_Support::APP_Global_Where_Var != STOP_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#else
#ifdef GNU
#define elsewhere_mask(MASK) for(Where_Statement_Support::APP_Global_Where_Var=elsewhere_function(MASK); \
                            Where_Statement_Support::APP_Global_Where_Var != STOP_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#define elsewhere() for(Where_Statement_Support::APP_Global_Where_Var=elsewhere_function(); \
                            Where_Statement_Support::APP_Global_Where_Var != STOP_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#else
#define elsewhere(MASK) for(Where_Statement_Support::APP_Global_Where_Var=elsewhere_function(MASK); \
                            Where_Statement_Support::APP_Global_Where_Var != STOP_WHERE_MACRO; \
                            Where_Statement_Support::APP_Global_Where_Var=endwhere_function())
#endif
#endif
#endif

class Where_Statement_Support
   {
  // We could implement a scope mechanism to permit
  // nesting of where (then it could be more easily
  // supported accross function calls (even if the function 
  // implementation used a where statement).
  // A stack would be required for this, then the constructors and
  // destructors would modify the stack to determine buld the final
  // mask (so the local mask is always on top of the stack).

  // Does CMF support function calls within where statements (functions
  // which contian where statements themselves?).

  // Later we want all the Arrays (below) to be Array<int>s

     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

#if !defined(APP)
  // Bugfix (9/19/96) there should be no reference to the SerialArray objects within A++
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
#endif

  // Prototype classes to support defered evaluation!

#if 1
     friend class doubleArray_Function_0;
     friend class doubleArray_Function_1;
     friend class doubleArray_Function_2;
     friend class doubleArray_Function_3;
     friend class doubleArray_Function_4;
     friend class doubleArray_Function_5;
     friend class doubleArray_Function_6;
     friend class doubleArray_Function_7;
     friend class doubleArray_Function_8;
     friend class doubleArray_Function_9;
     friend class doubleArray_Function_10;
     friend class doubleArray_Function_11;
     friend class doubleArray_Function_12;
     friend class doubleArray_Function_13;

     friend class floatArray_Function_0;
     friend class floatArray_Function_1;
     friend class floatArray_Function_2;
     friend class floatArray_Function_3;
     friend class floatArray_Function_4;
     friend class floatArray_Function_5;
     friend class floatArray_Function_6;
     friend class floatArray_Function_7;
     friend class floatArray_Function_8;
     friend class floatArray_Function_9;
     friend class floatArray_Function_10;
     friend class floatArray_Function_11;
     friend class floatArray_Function_12;
     friend class floatArray_Function_13;

     friend class intArray_Function_0;
     friend class intArray_Function_1;
     friend class intArray_Function_2;
     friend class intArray_Function_3;
     friend class intArray_Function_4;
     friend class intArray_Function_5;
     friend class intArray_Function_6;
     friend class intArray_Function_7;
     friend class intArray_Function_8;
     friend class intArray_Function_9;
     friend class intArray_Function_10;
     friend class intArray_Function_11;
     friend class intArray_Function_12;
     friend class intArray_Function_13;
#endif

#if 0
     friend class doubleSerialArray_Function_0;
     friend class doubleSerialArray_Function_1;
     friend class doubleSerialArray_Function_2;
     friend class doubleSerialArray_Function_3;
     friend class doubleSerialArray_Function_4;
     friend class doubleSerialArray_Function_5;
     friend class doubleSerialArray_Function_6;
     friend class doubleSerialArray_Function_7;
     friend class doubleSerialArray_Function_8;
     friend class doubleSerialArray_Function_9;
     friend class doubleSerialArray_Function_10;
     friend class doubleSerialArray_Function_11;
     friend class doubleSerialArray_Function_12;
     friend class doubleSerialArray_Function_13;

     friend class floatSerialArray_Function_0;
     friend class floatSerialArray_Function_1;
     friend class floatSerialArray_Function_2;
     friend class floatSerialArray_Function_3;
     friend class floatSerialArray_Function_4;
     friend class floatSerialArray_Function_5;
     friend class floatSerialArray_Function_6;
     friend class floatSerialArray_Function_7;
     friend class floatSerialArray_Function_8;
     friend class floatSerialArray_Function_9;
     friend class floatSerialArray_Function_10;
     friend class floatSerialArray_Function_11;
     friend class floatSerialArray_Function_12;
     friend class floatSerialArray_Function_13;

     friend class intSerialArray_Function_0;
     friend class intSerialArray_Function_1;
     friend class intSerialArray_Function_2;
     friend class intSerialArray_Function_3;
     friend class intSerialArray_Function_4;
     friend class intSerialArray_Function_5;
     friend class intSerialArray_Function_6;
     friend class intSerialArray_Function_7;
     friend class intSerialArray_Function_8;
     friend class intSerialArray_Function_9;
     friend class intSerialArray_Function_10;
     friend class intSerialArray_Function_11;
     friend class intSerialArray_Function_12;
     friend class intSerialArray_Function_13;
#endif

  // private:
     public:

     // For P++ the where statement implies a parallel where
     // statement.  And the where statement for the <type>SerialArray
     // is disabled.  Later we can allowboth to exist together.
     // Nesting of where statements can also be added later.
// if defined(SERIAL_APP)
#if defined(PPP) || defined(SERIAL_APP)
        static intSerialArray *Serial_Where_Statement_Mask;
        static intSerialArray *Serial_Previous_Where_Statement_Mask;
        static intSerialArray *Serial_Previous_Where_Statement_Mask_History;
#endif
        static intArray *Where_Statement_Mask;
        static intArray *Previous_Where_Statement_Mask;
        static intArray *Previous_Where_Statement_Mask_History;

     public:

     // Used as index in for lop used for macro implementation!
        static int APP_Global_Where_Var;
// if defined(SERIAL_APP)
#if defined(PPP) || defined(SERIAL_APP)
        friend int where_function ( const intSerialArray & Mask );
#endif
        friend int where_function ( const intArray & Mask );

     // We can support this at present since the input is evaluated under the 
     // influence of the previous where function.  We can handle it once the 
     // where statement has a macro implementation so that it can use a for loop
     // to execute something at the end of where.  Then endwhere can be made to work!
     // friend void elsewhere ( const intArray & Mask );

// if defined(SERIAL_APP)
#if defined(PPP) || defined(SERIAL_APP)
        friend int elsewhere_function ( const intSerialArray & Mask );
#endif
        friend int elsewhere_function ( const intArray & Mask );

        friend int elsewhere_function ();
        friend int endwhere_function  ();

     // return a count of the number of arrays stored internally
        static int numberOfInternalArrays();

        static void cleanup_after_Where();
// if defined(SERIAL_APP)
#if defined(PPP) || defined(SERIAL_APP)
        static void cleanup_after_Serial_Where();
#endif
   };

#endif  /* !defined(_APP_WHERE_H) */



