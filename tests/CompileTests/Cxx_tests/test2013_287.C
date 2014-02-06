namespace std
   {
     class vector
        {
        };   
   }


namespace MyCFG {

  class CFGEdge {};

  class CFGPath {
    std::vector edges;
    public:
#if 1
 // Either of these line cause the error: STL type names are in the
 // scope of MyCFG and referenced before we visit the MyCFG namespace.
 // CFGPath(CFGEdge e): edges(1, e) {}
    CFGPath(): edges() {}
#else
    CFGPath(CFGEdge e) {}
#endif
  };

}
