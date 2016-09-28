#define TEMPLATE_CASE 0

template <class T>
class PtrSetWrap  
   {
     public:
#if TEMPLATE_CASE
          template <class T1>
#endif
       // template <class T1 = T>
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

       // void ToString() const
          void ToString()
             {
#if TEMPLATE_CASE
               PtrSetWrap<GraphNode>::Iterator<int> iter;
#else
               PtrSetWrap<GraphNode>::Iterator iter;
#endif
             }
   };


#endif

#if 0
void ToString()
   {
     PtrSetWrap<GraphNode>::Iterator iter;
   }
#endif

