
template < class T > class MeshBase
   {
     public:
          int getNumberOfDimensions();
   };

template <>
int
MeshBase<int>::getNumberOfDimensions()
   {
     return 3;
   }

// set SKIP_ERROR to 0 to demonstrate error in EDG.
#define SKIP_ERROR 0
#if SKIP_ERROR
// DQ (9/12/2004): With this commented out an error is generated where 
// ROSE_ASSERT (curr_source_sequence_entry != NULL);
// is false.  This is left as an error to fix later, since I'm not clear
// on how to handle it right now!  This happens in sage_gen_routine_decl()
// in the construction of:
//      ROSE_ASSERT (curr_source_sequence_entry != NULL);
//      Sg_File_Info *fileInfo = generateFileInfo(curr_source_sequence_entry);

int main()
   {
     return 0;
   }
#endif


