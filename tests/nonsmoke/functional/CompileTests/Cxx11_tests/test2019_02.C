// This test demonstrates the unparsing of the CPP directives in a template.
// Unless unparsing of the template declarations is used (new feature) then
// the template declarations are unparsed as a string and the CPP directives 
// will be missing.  ROSE will unparsing the CPP directives as closely as 
// possible before the location were they may have actually appeared, in
// this case immediately before the templeat declaration.

// There was a bug that was fixed: the case of a CPP directive in a lambda 
// function in a template declaration. the CPP directive in the lambda function 
// was being dropped.  This ws the bug that was fixed, but the output of the 
// CPP directive before the template declaration is a consequence of the template 
// declaration being unparsed as a string and the more recent work on template 
// declaration unparsing from the AST is what fixes this with hight fadility.

template<typename TI, typename BODY>
void forallN(TI const &is_i, BODY const &body)
   {
  // Issue: the comment and CPP directive will be unparsed before the template declaration.
#if 1
     int x;
     int y;
     int z;
#endif
   }

