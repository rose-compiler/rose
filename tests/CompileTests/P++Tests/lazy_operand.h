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





class doubleSerialArray_Operand_Storage : public Operand_Storage
   {
     public:
        doubleSerialArray* Original_Referenced_Array;

       ~doubleSerialArray_Operand_Storage ();
        doubleSerialArray_Operand_Storage ();
        doubleSerialArray_Operand_Storage ( const doubleSerialArray & X );
        doubleSerialArray_Operand_Storage ( doubleSerialArray *X );

        int Array_ID () const;
        void Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer );

        void *Get_Original_Array_Pointer ();

        doubleSerialArray_Operand_Storage ( const doubleSerialArray_Operand_Storage & X );
        doubleSerialArray_Operand_Storage & operator= ( const doubleSerialArray_Operand_Storage & X );
   };

class floatSerialArray_Operand_Storage : public Operand_Storage
   {
     public:
        floatSerialArray* Original_Referenced_Array;

       ~floatSerialArray_Operand_Storage ();
        floatSerialArray_Operand_Storage ();
        floatSerialArray_Operand_Storage ( const floatSerialArray & X );
        floatSerialArray_Operand_Storage ( floatSerialArray *X );

        int Array_ID () const;
        void Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer );

        void *Get_Original_Array_Pointer ();

        floatSerialArray_Operand_Storage ( const floatSerialArray_Operand_Storage & X );
        floatSerialArray_Operand_Storage & operator= ( const floatSerialArray_Operand_Storage & X );
   };

class intSerialArray_Operand_Storage : public Operand_Storage
   {
     public:
        intSerialArray* Original_Referenced_Array;

       ~intSerialArray_Operand_Storage ();
        intSerialArray_Operand_Storage ();
        intSerialArray_Operand_Storage ( const intSerialArray & X );
        intSerialArray_Operand_Storage ( intSerialArray *X );

        int Array_ID () const;
        void Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer );

        void *Get_Original_Array_Pointer ();

        intSerialArray_Operand_Storage ( const intSerialArray_Operand_Storage & X );
        intSerialArray_Operand_Storage & operator= ( const intSerialArray_Operand_Storage & X );
   };


#endif  /* !defined(_APP_LAZY_OPERAND_H) */





