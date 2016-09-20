// Test code variation of test2015_85.c

struct memory_map
   {
     struct X
        {
          int abc;
        } map;
   };

void foobar()
   {
     int y;

  // Bug: unparses to:
  //    struct memory_map fmap = {.map{.abc = y}};
     struct memory_map fmap = 
        {

          .map = { .abc = y }
        };
   }

