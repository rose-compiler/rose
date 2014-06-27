template<class T>
class Map 
   {
     public:
          template<class Derived>
          class BidirectionalIterator
             {
#if 1
               public:
                    BidirectionalIterator();
#endif
             };

          class NodeIterator: public BidirectionalIterator<NodeIterator>
             {
#if 1
               typedef BidirectionalIterator<NodeIterator> Super;
#endif
             };
   };
