class CFGNode 
   {
     public:
          bool operator==(const CFGNode& o) const; // {return node == o.node && index == o.index;}
   };

class CFGEdge 
   {
     private:
          CFGNode src, tgt;

     public:
          bool operator==(const CFGEdge& o) const
             {
               return src == o.src && tgt == o.tgt;
             }
  };
