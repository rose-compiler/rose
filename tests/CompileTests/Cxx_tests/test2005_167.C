/*

Hi Dan,
here is a new bug-report. It is a little bit bigger than some of the
earlier ones I have send (97 lines), but I hope it will suffice.

When compiling I get the error-message:
"/home/saebjornsen2/ROSE-October/gcc3.3.3/g++_HEADERS/hdrs1/bits/stl_set.h",
line 89: warning:
          extra ";" ignored
    __glibcpp_class_requires4(_Compare, bool, _Key, _Key,
_BinaryFunctionConcept);
                                                                                 ^

"/home/saebjornsen2/ROSE-October/gcc3.3.3/g++_HEADERS/hdrs1/bits/stl_multiset.h",
line 88: warning:
          extra ";" ignored
    __glibcpp_class_requires4(_Compare, bool, _Key, _Key,
_BinaryFunctionConcept);
                                                                                 ^

"../../../../ROSE/src/util/graphs/TransAnalysis.C", line 92: warning:
variable
          "n" was set but never used
        for (GraphNode* n; n = iter.Current(); iter++)
                        ^

The loop body is already a SgBasicBlock
identityTranslator:
../../../../NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:1991:
SgExpression* sage_gen_routine_name(an_expr_node*): Assertion
`originalTemplate->text != __null' failed.
Aborted

I get that "The loop body is already a SgBasicBlock" all over. ROSE
compiles fine with this message in all the other cases, but I just wanted
to make you aware of this.

Thanks
Andreas

*/


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
            // This line causes an error
               iter.Current();
#endif
             }
   };

