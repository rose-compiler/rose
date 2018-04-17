
#include <mpi.h>
#include "A++.h"

//-------------------------------------------------------------------
// Prototypes for pgslib
//
extern "C"
{
#include "pgslib-types.h"
// include <../PADRE/PGSLIB/pgslib-types.h>
// include "../PADRE/PGSLIB/pgslib-types.h"
// put in .C file
// include "../pgslib/pgslib-include-c.h"

   void pgslib_gs_init_trace_c (GS_TRACE_STRUCT **);
   void pgslib_prep_supplement_c ( int*, int[], GS_TRACE_STRUCT **);
   void pgslib_setup_n_duplicate_c ( int*, GS_TRACE_STRUCT **);
   void pgslib_setup_duplicate_buffer_c ( GS_TRACE_STRUCT ** );
   void pgslib_gs_release_trace_c( GS_TRACE_STRUCT**);

   void pgslib_scatter_buf_int_c
      ( int*, int*, int*, GS_TRACE_STRUCT **);
   void pgslib_scatter_buf_real_c
      ( float*, float*, int*, GS_TRACE_STRUCT **);
   void pgslib_scatter_buf_double_c
      ( double*, double*, int*, GS_TRACE_STRUCT **);

   void pgslib_gather_buf_int_c
      ( int*, int*, int*, GS_TRACE_STRUCT **);
   void pgslib_gather_buf_real_c
      ( float*, float*, int*, GS_TRACE_STRUCT **);
   void pgslib_gather_buf_double_c
      ( double*, double*, int*, GS_TRACE_STRUCT **);
}

//-------------------------------------------------------------------
class Indirect_Addressing_Support
   {
     public:
        GS_TRACE_STRUCT *messageSchedule;  // this is the PGSLib trace structure
        int numberOfDuplicates;
        int numberOfSupplements;
        int numberOfLocal;

        int* supplementLocations;
        int* duplicateLocations;
        int* localLocations;

        static int VALUE;
        static int POSITION;
        static int SUPPLEMENT;
        static int DUPLICATE;
        int setupType;

        int local_I_base;

     // Data
        int* duplicateDataArray[MAX_ARRAY_DIMENSION];
        int* supplementDataArray[MAX_ARRAY_DIMENSION];
        int* localDataArray[MAX_ARRAY_DIMENSION];
        int blockSize;

     // Required processor location and global index information
        int* processorList;

     // Destructor and constructors
       ~Indirect_Addressing_Support ();
        Indirect_Addressing_Support ();
        Indirect_Addressing_Support ( const Array_Domain_Type & X,
                                      const int type = VALUE);
        Indirect_Addressing_Support ( const Array_Domain_Type & X,
                                      intSerialArray** Index_Arrays,
                                      int* global_I_base,
                                      const int type = VALUE);
        Indirect_Addressing_Support ( const Array_Domain_Type & X,
                                      const Indirect_Addressing_Support & I_A,
                                      const int type);
        Indirect_Addressing_Support ( const Array_Domain_Type & X,
                                      int** I_A_Data, int* I_A_Locations,
                                      int I_base,int list_size, const int type);

     // setup routine for pgslib
        void initialize_pgslib();

     // main gather scatter functions
        void gatherData ( );

        int* gatherData 
           (const intSerialArray & A, const int type, int& merged_size, 
            int* position_offset);
        float* gatherData 
           (const floatSerialArray & A, const int type, int& merged_size,
            int* position_offset);
        double* gatherData 
           (const doubleSerialArray & A, const int type, int& merged_size,
            int* position_offset);

        int* gatherData 
           (const intSerialArray & A, const int type, int& merged_size );
        float* gatherData 
           (const floatSerialArray & A, const int type, int& merged_size);
        double* gatherData 
           (const doubleSerialArray & A, const int type, int& merged_size);

        int** gatherData 
           (intSerialArray** Index_Arrays, const int type, int& merged_size);
        float** gatherData 
           (floatSerialArray** Index_Arrays, const int type, int& merged_size);
        double** gatherData 
           (doubleSerialArray** Index_Arrays, const int type, int& merged_size);

        void scatterData ();
        int* scatterData 
           (const intSerialArray & A, const int type, int& merged_size);
        float* scatterData 
           (const floatSerialArray & A, const int type, int& merged_size);
        double* scatterData 
           (const doubleSerialArray & A, const int type, int& merged_size);

     // other functions
        int mergeIndexData 
           (const int merge_type, int** Data, int**Locations) const;
        int mergeData 
           (const int merge_type, int* other_local, int* other_data,
            int** Data, const int usePosnFlag = FALSE );
        int mergeData 
           (const int merge_type, float* other_local, float* other_data,
            float** Data, const int usePosnFlag = FALSE);
        int mergeData 
           (const int merge_type, double* other_local, double* other_data,
            double** Data, const int usePosnFlag = FALSE);
        
       // Indirect_Addressing_Support ( const Indirect_Addressing_Support & X );
       // Indirect_Addressing_Support & operator= 
        //   ( const Indirect_Addressing_Support & X );

   };

//-------------------------------------------------------------------



