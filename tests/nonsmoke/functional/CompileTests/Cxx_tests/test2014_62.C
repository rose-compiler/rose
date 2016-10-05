// This is similar to test2011_76.C bu with TEMPLATE_CASE == 0
template <class T>
class PtrSetWrap
   {
     public:
          class Iterator
             {
               public:
#if 0
                    Iterator();
#else
                    Iterator() {}
#endif
//                  T* Current() const;
             };

      // This causes an error!
      // Iterator GetIterator() const {};
   };

// This line will compile just fine.
// template <class T> PtrSetWrap<T>::Iterator::Iterator() {}
// template <class T> PtrSetWrap<T>::Iterator::Iterator() {}

class GraphNode 
   {
     public:
//        virtual ~GraphNode();
   };

#if 1
// class GroupGraphNode : public GraphNode
class GroupGraphNode
   {
     public:

       // Removing this line causes a different error
       // PtrSetWrap <GraphNode> nodeSet;

       // PtrSetWrap<GraphNode>::Iterator iter;
#if 0
               PtrSetWrap<GraphNode>::Iterator iter;
#endif
#if 1
       // void ToString() const
          void ToString()
             {
                int x;
                x++;
#if 1
               PtrSetWrap<GraphNode>::Iterator iter;
#endif
                x++;
             }
#endif
   };
#else

#if 0
               PtrSetWrap<GraphNode>::Iterator iter;
#endif

#endif
