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
      // Iterator GetIterator() const {};
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
//             iter.PtrSetWrap<GraphNode>::Iterator::Current();
             }
   };


#if 0
// DQ (4/28/2012): Added more detail about the current error.
// This class will be output by the unparser:
//   1) It should not be output
//   2) It should be output before the use in "void ToString() const" above (or a template class and/or member function prototype defined)
//   3) It should be output with the "template<>" syntax (though I would have hoped this would be optional under old syntax rules)
//   4) The member function declaration (name = Current) is skipped (not unparsed).
template<>
class PtrSetWrap< GraphNode > ::Iterator
   {
  /* Skipped output of member function declaration (name = Current) */ 
   };
#endif
