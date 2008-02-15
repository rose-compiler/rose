// Allow repeated includes of lazy_operand.h without error
#ifndef _APP_LAZY_OPERAND_H
#define _APP_LAZY_OPERAND_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif


#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
class Operand_Storage
   {
     public:
        Operand_Storage* Next_Array_Defered_Operand;
        Operand_Storage* Head_Of_List;
        Operand_Storage* Tail_Of_List;

        virtual ~Operand_Storage ();
        inline Operand_Storage ();
#if defined(GNU)
     // The use of the pragmas implement and interface
     // make the use of inlined function more complicated
     // so here we just skip there use.
        Operand_Storage ( const doubleArray & X );
        Operand_Storage ( const floatArray  & X );
        Operand_Storage ( const intArray    & X );
#else
        INLINE Operand_Storage ( const doubleArray & X );
        INLINE Operand_Storage ( const floatArray  & X );
        INLINE Operand_Storage ( const intArray    & X );
#endif

#if defined(SERIAL_APP)
        INLINE Operand_Storage ( const doubleSerialArray & X );
        INLINE Operand_Storage ( const floatSerialArray  & X );
        INLINE Operand_Storage ( const intSerialArray    & X );
#endif

     // This function in envoked on local variables to move the
     // data to a variable declared off the heap and then
     // change the reference in the defered storage to be the new variable!
     // virtual void Make_Local_Variable_Persistant_And_Update_Storage ( doubleArray * X_Pointer );
     // virtual void Make_Local_Variable_Persistant_And_Update_Storage ( floatArray  * X_Pointer );
     // virtual void Make_Local_Variable_Persistant_And_Update_Storage ( intArray    * X_Pointer );

        virtual void Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer );

        void Process_List_Of_Operands ();
     // virtual int Array_ID () const = 0;

        virtual void *Get_Original_Array_Pointer () = 0;

        Operand_Storage ( const Operand_Storage & X );
        Operand_Storage & operator= ( const Operand_Storage & X );
   };
#endif





class doubleArray_Operand_Storage : public Operand_Storage
   {
     public:
        doubleArray* Original_Referenced_Array;

       ~doubleArray_Operand_Storage ();
        doubleArray_Operand_Storage ();
        doubleArray_Operand_Storage ( const doubleArray & X );
        doubleArray_Operand_Storage ( doubleArray *X );

        int Array_ID () const;
        void Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer );

        void *Get_Original_Array_Pointer ();

        doubleArray_Operand_Storage ( const doubleArray_Operand_Storage & X );
        doubleArray_Operand_Storage & operator= ( const doubleArray_Operand_Storage & X );
   };

class floatArray_Operand_Storage : public Operand_Storage
   {
     public:
        floatArray* Original_Referenced_Array;

       ~floatArray_Operand_Storage ();
        floatArray_Operand_Storage ();
        floatArray_Operand_Storage ( const floatArray & X );
        floatArray_Operand_Storage ( floatArray *X );

        int Array_ID () const;
        void Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer );

        void *Get_Original_Array_Pointer ();

        floatArray_Operand_Storage ( const floatArray_Operand_Storage & X );
        floatArray_Operand_Storage & operator= ( const floatArray_Operand_Storage & X );
   };

class intArray_Operand_Storage : public Operand_Storage
   {
     public:
        intArray* Original_Referenced_Array;

       ~intArray_Operand_Storage ();
        intArray_Operand_Storage ();
        intArray_Operand_Storage ( const intArray & X );
        intArray_Operand_Storage ( intArray *X );

        int Array_ID () const;
        void Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer );

        void *Get_Original_Array_Pointer ();

        intArray_Operand_Storage ( const intArray_Operand_Storage & X );
        intArray_Operand_Storage & operator= ( const intArray_Operand_Storage & X );
   };


#endif  /* !defined(_APP_LAZY_OPERAND_H) */





