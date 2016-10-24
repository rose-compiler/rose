namespace pov
   {
  // using namespace std;

#if 1
#define BBOX_TREE int
  // typedef int BBOX_TREE;
#else
     typedef struct BBox_Tree_Struct BBOX_TREE;

     struct BBox_Tree_Struct
        {
        };
#endif

     extern BBOX_TREE *Root_Object;
   }

namespace pov
   {
  // using namespace std;

     BBOX_TREE *Root_Object;
   }

#if 0
namespace pov
   {
     int x;
   }

namespace pov
   {
     int y;
   }
#endif

