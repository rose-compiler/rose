// ROSE-1780

template<typename _MemberPointer>
class _Mem_fn;

template<typename _Res, typename _Class>
class _Mem_fn<_Res _Class::*> {
    _Mem_fn(_Res _Class::*__pm) noexcept : __pm(__pm) { }

    _Res _Class::*__pm;
};

// causes this error:
// identityTranslator: /g/g17/charles/code/ROSE/rose-master-0.9.10.170/src/backend/unparser/nameQualificationSupport.C:5368: NameQualificationInheritedAttribute NameQualificationTraversal::evaluateInheritedAttribute(SgNode *, NameQualificationInheritedAttribute): Assertion `isSgPointerMemberType(initializedName->get_type()) == __null' failed.
// 

