template<class K, class T>
class Map 
   {
     public:
          typedef K Key;
          typedef T Value;

          class Node {};

          template<class Derived, class Value>
          class BidirectionalIterator
             {
               public:
                    BidirectionalIterator();
             };

          class NodeIterator: public BidirectionalIterator<NodeIterator, Node>
             {
#if 1
               typedef BidirectionalIterator<NodeIterator, Node> Super;
#endif
#if 1
               public:
                    NodeIterator();
#endif
             };
   };
