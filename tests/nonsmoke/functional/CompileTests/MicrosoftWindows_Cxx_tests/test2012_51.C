template <class T>
class PtrSetWrap  
   {
     public:
          class Iterator 
             {
               public:
                 // Iterator(); // This is part of a bug in test2005_163.C
                    T* Current() const;
             };

      // This causes an error!
         Iterator GetIterator() const {};
   };

// template <class T> PtrSetWrap<T>::Iterator::Iterator() {}


class GraphNode 
   {
     public:
          virtual ~GraphNode();
   };

class GroupGraphNode : public GraphNode
   {
     public:

       // Removing this line causes a different error
          PtrSetWrap <GraphNode> nodeSet;

          void ToString() const
             { 
               PtrSetWrap<GraphNode>::Iterator iter;

#if 1
            // This line causes an error: should be unparsed as "iter.Current();"
               iter.Current();
#endif
            // Or alternatively...(commented out for testing...)
               iter.PtrSetWrap<GraphNode>::Iterator::Current();
             }
   };


