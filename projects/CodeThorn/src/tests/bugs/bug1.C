

/* BUG: (*classDeclaration)->get_name() : produces "::_Class::*" and lacks "..." 

returned by above function: "_Mem_fn < _Res (::_Class::*)(_ArgTypes   ) >"
correct results should be : "_Mem_fn < _Res (  _Class::*)(_ArgTypes...) >"

Note that the unparser does generate the correct classname (identityTranslator)

 */

template<typename _MemberPointer>
    class _Mem_fn;

template<typename _Res, typename _Class, typename... _ArgTypes>
class _Mem_fn<_Res (_Class::*)(_ArgTypes...)>
{
  int x;
};

