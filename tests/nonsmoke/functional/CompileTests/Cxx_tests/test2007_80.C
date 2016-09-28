/* 

Email from Andreas (high priority bug)

Hi Dan,
here is the bug I have been hunting down. It is an elusive beast, but
if you could give priority to it that would be great
namespace std
{
 template<typename _Tp>
   class vector
   {
   };

 template<typename _Tp>
   inline bool
   operator==(const vector<_Tp>& __x, const vector<_Tp>& __y)
   { return true; }
}



class CFGEdge {
};

class CFGPath {
 std::vector<CFGEdge> edges;
 public:

 bool operator==(const CFGPath& o) const {return edges == edges;}
};


the error message is:

identityTranslator: Cxx_Grammar.C:5583: bool Sg_File_Info::isCompilerGenerated() const: Assertion this != __null failed.
Aborted 
*/

namespace std
{
 template<typename _Tp>
   class vector
   {
   };

 template<typename _Tp>
   inline bool
   operator==(const vector<_Tp>& __x, const vector<_Tp>& __y)
   { return true; }
}


#if 1
class CFGEdge {
};
#endif

class CFGPath {
 std::vector<CFGEdge> edges;
 public:

#if 1
      bool operator==(const CFGPath& o) const {return edges == edges;}
#endif
};


