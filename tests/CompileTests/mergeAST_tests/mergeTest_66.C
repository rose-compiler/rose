// Example showing that we have to gather the statements in a block to build the delete list!
// Note that these fail for the case of variable declarations.
namespace X
   {
     typedef struct
        {
          int x;
        } a_type;

     typedef struct
        {
          int x;
        } b_type;
   }
