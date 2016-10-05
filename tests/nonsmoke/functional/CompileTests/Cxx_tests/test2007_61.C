#if 0
Bug name:	template-invoked-from-class-defined-method

Reported by:	Brian White

Date:		June 13, 2007

Occurs in:  	KOLAH src/boost/detail/shared_count.hpp throw_exception 
                    in add_ref
                KOLAH src/geom/PolyMesh/Lite2PolyFace.hh operator== 
                    in Lite2PolyFace::polydID
                KOLAH src/geom/PolyMesh/Lite2PolyFace.hh operator!= 
                    in Lite2PolyFace::dump
                KOLAH src/utilities/Attribute.hh operator==
                    in Attribute::operator==
                KOLAH src/utilities/Attribute.hh operator!=
                    in Attribute::operator!=
                KOLAH src/utilities/Name.hh operator== in Name::operator==
                KOLAH src/utilities/Name.hh operator!= in Name::operator!=
                SPEC CPU 2006 450.soplex/src/ssvector.h spx_allox
                    in 3 SSVector constructors
                SPEC CPU 2006 450.soplex/src/ssvector.h spx_free
                    in ~SSVector

Brief description:  	Invocation of a templated function (spx_alloc)
			from within a method (func) defined within
			a class (SSVector) leads to a NULL Sg_File_Info
			for spx_alloc's declaration.  This leads to
			an assertion failure

rosec: Cxx_Grammar.C:5575: bool Sg_File_Info::isCompilerGenerated() const: Assertion `this != __null' failed.

			where the assertion occurs during an invocation
                        of fixupTemplateInstantiations.

Files
-----
bug.cc		Manifests the above bug when DEFINE_INLINE is #define'ed.
                When it is not, func is declared within SSVector, but
                defined external to it and there is no assertion failure.

cat bug.cc 
#endif

  // defineing DEFINE_INLINE leads to a ROSE bug:  a SgFunctionDeclaration
  // for spx_alloc with a NULL SgFileInfo.
#define DEFINE_INLINE

template <class T>
void spx_alloc(T& p, int n)
{

}

class SSVector 
{
public:

#ifdef DEFINE_INLINE
  int func() 
  {
      int len = 1;
      int *idx;
      spx_alloc(idx, len);
  }
#else
  func();
#endif

};

#ifndef DEFINE_INLINE
int SSVector::
func() 
{
      int len = 1;
      int *idx;
      spx_alloc(idx, len);
}
#endif
