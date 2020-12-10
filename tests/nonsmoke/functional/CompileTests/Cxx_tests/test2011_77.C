// This is similar to test2011_76.C but with TEMPLATE_CASE == 0
template <class T>
class PtrSetWrap
   {
     public:
          class Iterator
             {
               public:
                    Iterator();
                    T* Current() const;
             };

      // This causes an error!
      // Iterator GetIterator() const {};
   };

// This line will compile just fine.
// template <class T> PtrSetWrap<T>::Iterator::Iterator() {}

class GraphNode 
   {
     public:
          virtual ~GraphNode();
   };

#if 1
// class GroupGraphNode : public GraphNode
class GroupGraphNode
   {
     public:

       // Removing this line causes a different error
       // PtrSetWrap <GraphNode> nodeSet;

       // PtrSetWrap<GraphNode>::Iterator iter;

       // void ToString() const
          void ToString()
             {
               PtrSetWrap<GraphNode>::Iterator iter;
             }
   };


#endif

