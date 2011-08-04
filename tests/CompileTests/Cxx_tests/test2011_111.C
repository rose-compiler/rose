
namespace pov
{
// using namespace std;

// typedef int BBOX_TREE;
typedef struct BBox_Tree_Struct BBOX_TREE;

struct BBox_Tree_Struct
{
  short Infinite;   /* Flag if node is infinite            */
  short Entries;    /* Number of sub-nodes in this node    */
//  BBOX BBox;        /* Bounding box of this node           */
  BBOX_TREE **Node; /* If node: children; if leaf: element */
};

extern BBOX_TREE *Root_Object;
}



namespace pov
{
// using namespace std;

#if 1
/*****************************************************************************
* Local preprocessor defines
******************************************************************************/
static const int BUNCHING_FACTOR = 4;
/* Initial number of entries in a priority queue. */
static const int INITIAL_PRIORITY_QUEUE_SIZE = 256;
/*****************************************************************************
* Local typedefs
******************************************************************************/
/*****************************************************************************
* Static functions
******************************************************************************/
static BBOX_TREE *create_bbox_node(int size);
static int find_axis(BBOX_TREE **Finite,long first,long last);
// static void calc_bbox(BBOX *BBox,BBOX_TREE **Finite,long first,long last);
static void build_area_table(BBOX_TREE **Finite,long a,long b,double *areas);
static int sort_and_split(BBOX_TREE **Root,BBOX_TREE **&Finite,long *numOfFiniteObjects,long first,long last);
// static void priority_queue_insert(PRIORITY_QUEUE *Queue,double Depth,BBOX_TREE *Node);
static int compboxes(void *in_a,void *in_b);
/*****************************************************************************
* Global variables
******************************************************************************/
// GLOBAL VARIABLE
long numberOfFiniteObjects;
long numberOfInfiniteObjects;
long numberOfLightSources;
/*****************************************************************************
* Local variables
******************************************************************************/
/* Current axis to sort along. */
// GLOBAL VARIABLE
static int Axis = 0;
/* Number of finite elements. */
// GLOBAL VARIABLE
static long maxfinitecount = 0;
/* Priority queue used for frame level bouning box hierarchy. */
// GLOBAL VARIABLE
// static PRIORITY_QUEUE *Frame_Queue;
/* Top node of bounding hierarchy. */
// GLOBAL VARIABLE

BBOX_TREE *Root_Object;
#endif

}
