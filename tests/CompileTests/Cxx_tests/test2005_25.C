
/*
Hi Dan,

I think I've found a bug in the overloadedOperatorRepresentation binary in
projects/highLevelGrammars.  Unfortunately, trying to track it down has
brought me into the bowels of ROSE and I've gotten a bit lost.

This fails:

./overloadedOperatorRepresentation ./overloadedOperatorTestCode.C

But I suggest you copy the header and source file I've included here in to
your compile tree and do:

./overloadedOperatorRepresentation ./simplerOverloadedOperatorTestCode.C

This is an even simpler example which fails.  The header file declares a
doubleArray with only 2 methods: foo and operator().  You can manipulate
the source file to see that both cause the same error.  I've added foo to
show that this isn't anything particular to an operator method.

The problem is an assertion failure:

overloadedOperatorRepresentation:
/home/bwhite/ROSE/src/roseSupport/transformationSupport.C:821: static
std::string TransformationSupport::getTypeName(SgType*): Assertion
`classDefinition != __null' failed.

Which occurs because

memberFunctionType->get_struct_name()

returns NULL in the V_SgMemberFunctionType case of
TransformationSupport::getTypeName.

As far as I (and gdb) can tell, neither set_struct_name nor any of the
SgMemberFunctionType constructors ever set struct_name to anything but
NULL.

Of minor interest:  after replacing the offending line in
overloadedOperatorRepresentation

string functionTypeName =
TransformationSupport::getFunctionTypeName(functionCallExpression);

the binary attempts to link, which fails because the header file doesn't
define the methods it declares.  Don't think I've even send backend()
before.

Thanks,
Brian
*/




// define include guards (SIMPLE_APP_HEADER_FILE)
#ifndef SIMPLER_APP_HEADER_FILE
#define SIMPLER_APP_HEADER_FILE

#define APP_RESTRICT

#define NULL 0L

class doubleArray
   {
     public:
       // destructor and constructors

       // declaration of scalar indexing operators
          double operator()( int i );
	  double foo();
   };


// endif for SIMPLE_APP_HEADER_FILE (include guard)
#endif


