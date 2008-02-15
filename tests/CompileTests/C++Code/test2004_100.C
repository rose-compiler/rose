// this test code is similar to test2004_99.C
// the difference is that this one tests multiple forward 
// declarations of template specializations.  It has the
// same problem and the same solution.

// These presently result in multile SgTemplateDeclarations 
// since thee is no SgTemplateDefintion IR node in Sage III.
// There will be once we explicitly represent the uninstantiated 
// template in the IR.
template <typename T> class X;
template <typename T> class X;

template <typename T> class X { class FIND_ME_IN_TEMPALTE_DECLARATION {}; };

template <typename T> class X;

template <> class X<int>;
template <> class X<int>;
template <> class X<int> { class FIND_ME_IN_TEMPLATE_INSTANTIATION_DECLARATION {}; };

X<int> x_int;

