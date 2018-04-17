/*
The following file compiler in ROSE

template <class CharT>
CharT
GetFindInSetFilter( const CharT* set)
 {
   CharT filter;
   while (true) {
   }
   return filter;
 }

template <class CharT> struct nsBufferRoutines {};

template <>
struct nsBufferRoutines<char>
 {
   static
   void get_find_in_set_filter( const char* set )
     {
       GetFindInSetFilter(set);
     }
 };

gives the following error:

Error: locatedNode->get_parent() == NULL locatedNode = 0x8b12960 =
SgExprStatement
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-29a-Unsafe/NEW_ROSE/src/midend/astDiagnostics/AstConsistencyTests.C:2982:
virtual void TestParentPointersInMemoryPool::visit(SgNode*): Assertion locatedNode->get_parent() != __null failed. 
*/


template <class CharT>
CharT
GetFindInSetFilter( const CharT* set)
 {
   CharT filter;
   while (true) {
   }
   return filter;
 }

template <class CharT> struct nsBufferRoutines {};

template <>
struct nsBufferRoutines<char>
 {
   static
   void get_find_in_set_filter( const char* set )
     {
       GetFindInSetFilter(set);
     }
 };
