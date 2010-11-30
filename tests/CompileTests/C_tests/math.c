/* DQ (8/20/2006): This example demonstrated infinite 
   recursion in the computation of a mangled name!
*/
typedef enum
{
#if __cplusplus
/* A C++ enum can be empty */
#else
/* C is required to have at least one enum field value */
  enum_field
#endif
} enum_typedef_type;

#if __cplusplus
typedef enum
{
/* Test use of single enum field in C++ (so that both C and C++ are tested the same) */
  enum_field
} enum_typedef_type_cxx;
#endif
