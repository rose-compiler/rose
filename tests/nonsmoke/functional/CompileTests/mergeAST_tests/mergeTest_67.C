// Example showing that we have to gather the statements in a block to build the delete list!
// Note that these fail for the case of variable declarations.
namespace X
   {
     struct
        {
          int x;
        } a;

     struct
        {
          int x;
        } b;
   }

// Example showing that we have to gather the statements in a block to build the delete list!
// Note that these fail for the case of variable declarations.
namespace X
   {
     struct
        {
          int x;
        };

     struct
        {
          int x;
        };
   }

