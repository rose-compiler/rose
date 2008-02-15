// This exmaple demonstrates that the parent of a class has 
// nothing to do with the scope of it's declaration!
// Since the parent pointrs represent the traversal and the 
// traversal is of the AST which represents the structure 
// of the program, we need an additional scope concept since
// the parent information of the structure of the source code 
// is not enough. (Or so I'm thinking at the moment).

class PolyMesh
   {
     public:
       // secondary declaration (forward)
          static int x;
          class Zone;
          void foobar();
   };

void PolyMesh::foobar()
   {
   };

// Primary declaration (defining declaration)
class PolyMesh::Zone
   {
     public:
          void foo () 
             {
               x++;
             }
   };


