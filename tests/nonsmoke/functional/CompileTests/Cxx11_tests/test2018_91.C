template<typename _Rep>
struct duration {
  constexpr duration() = default;
};


template<typename _Rep1, typename _Rep2>
constexpr duration<int>
operator/(const duration<_Rep1>& __d,
          const          _Rep2&  __s);

void func1()
{
   duration<int> var1;
   duration<int> var2;
   int                        var3;
   var1 = var2 / ( var3 - 1 );
}

 
#if 0
gets this error:

lt-identityTranslator: /g/g17/charles/code/ROSE/rose-develop/src/backend/unparser/nameQualificationSupport.C:5362: virtual NameQualificationInheritedAttribute NameQualificationTraversal::evaluateInheritedAttribute(SgNode*, NameQualificationInheritedAttribute): Assertion `functionDeclaration->get_parameterList_syntax() != __null failed.
/usr/tce/packages/mvapich2/mvapich2-2.2-gcc-4.9.3/bin/mpic++: line 292: 141378 Aborted                 (core dumped) $Show $CXX $PROFILE_INCPATHS "${allargs[@]}" -I$includedir

#endif
