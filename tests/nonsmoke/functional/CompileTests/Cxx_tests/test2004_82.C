template < class T > class ClassWithFunctionDefinition
   {
     public:
          int getNumberOfDimensions()
             { return 3; }
   };

template < class T > class ClassWithoutFunctionDefinition
   {
     public:
         int getNumberOfDimensions();
   };

// DQ (4/20/2005): This is tested in later test codes after it was supported properly (see test2005_57.C)
// Explicit Template Instatiation (Test the older style template instantiation) (works with and without "<>")
// template int ClassWithFunctionDefinition<int>::getNumberOfDimensions();
// template int ClassWithFunctionDefinition<int>::getNumberOfDimensions();

// This can not be instantiated since it has been explicitly specialized
// template int ClassWithoutFunctionDefinition<int>::getNumberOfDimensions();
// Template specialization of prototype
// template <> int ClassWithFunctionDefinition<int>::getNumberOfDimensions();

// Explicit Template Specialization of function prototype
template <> int ClassWithoutFunctionDefinition<int>::getNumberOfDimensions();

// Explicit Template Specialization of function
template <> int ClassWithoutFunctionDefinition<int>::getNumberOfDimensions() { return 3; }



// DQ (4/9/2005): I fixed this while debugging the GNU extensions added to EDG and ROSE

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
