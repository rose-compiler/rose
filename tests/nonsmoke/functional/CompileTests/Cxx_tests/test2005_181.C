#if 0
Finally I managed to fit the KOLAH bug on one page.  There are several
ways to get the file to compile with ROSE by removing different parts, so
please be aware of that. Compiling the code:

I get the following error message:
identityTranslator:
../../../../NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_il_to_str.C:2395:
SgType* sage_form_type_first_part(a_type*, int, int, unsigned int, int,
an_il_to_str_output_control_block*, SgClassDefinition*,
SgDeclarationStatement*&): Assertion another == false failed.
/home/saebjornsen2/links/g++3.3.3: line 4:  1194 Aborted                
/home/saebjornsen2/ROSE-October/gcc3.3.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator

#endif

#include <vector>

namespace Geometry
{

namespace Internals
{

template <typename ElementType>
class Descriptor
{  };


} // end namespace Internals

class Vector3d {};

template <typename CoreMesh>
class ZoneBase
{
      public:
   typedef ZoneBase<CoreMesh>               ZoneType;
   typedef typename Internals::Descriptor<ZoneType>      ZoneDescriptor;
};

template <typename CoreMesh>
class MeshBase
{

   public:
   explicit MeshBase();
   protected:
   mutable std::vector<ZoneBase<CoreMesh> >       mZoneCache;

};

template <class Dimension>
class PolyMesh {
};

template <>
MeshBase<PolyMesh<Vector3d> >::
MeshBase()
{}

}

