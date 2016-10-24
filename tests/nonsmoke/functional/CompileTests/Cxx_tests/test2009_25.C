#include <map>
#include <set>

template < class X > class Y {};


class DependenceGraph
   {
     enum EdgeType
        {
          DO_NOT_FOLLOW
        };

  // std::map < int, EdgeType > edgeMap;
  // Y < EdgeType > edgeMap;

     std::map < int, EdgeType > edgeMap;
   };
