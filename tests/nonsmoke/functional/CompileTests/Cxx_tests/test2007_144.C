/*
Bug name:       omitted-qualifier3

Reported by:    Brian White

Date:           Sep 30, 2007

Date sent:      Sep 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:      SPEC CPU 2006 483.xalancbmk/src/XercesParserLiaison.cpp
                              483.xalancbmk/src/XercesDocumentWrapper.cpp
                              483.xalancbmk/src/XalanSourceTreeParserLiasion.cpp


Brief description:      A class DOMImplementationType within scope foo
                        is typedef'ed to DOMImplementation in global scope.
                        When DOMImplementation is used as a qualifier,
                        the backend substitutes DOMImplementationType
                        (i.e., the base of the typedef, but without the
                        qualifier foo).

Files
-----
bug.cc          Invocation of static method with qualifier:

                DOMImplementation::getImplementation();

                where we have

                typedef  foo::DOMImplementationType DOMImplementation;

                Resulting error is:

rose_bug.cc:20: error: `DOMImplementationType' undeclared (first use this 
   function)
rose_bug.cc:20: error: (Each undeclared identifier is reported only once for 
   each function it appears in.)
rose_bug.cc:20: error: syntax error before `::' token


rose_bug.cc     Contains the backend output of bug.cc.  Notice that the
                invocation does not have the required qualifier foo:

                DOMImplementationType::getImplementation();

                Notice that the original invocation in the source
                had a qualifier of DOMImplementation _not_
                DOMImplementationType.
*/


namespace foo {

class DOMImplementationType
{
public:
  static DOMImplementationType  *getImplementation() { return 0; }
};

}

typedef  foo::DOMImplementationType DOMImplementation;

int main()
{
  DOMImplementation::getImplementation();
  return 0;
}

