/*
DQ: This bug is similar to test2005_181.C (if that helps).

When compiling the following code:
namespace Geometry
{

class PolyMesh {
public:
};

template <typename CoreMesh>
class MeshBase;

template <typename CoreMesh>
void
getCoreMesh(MeshBase<CoreMesh>& mesh)
{
}

template <typename CoreMesh>
class MeshBase
{

   public:
   //#1 BUG MESSAGE GONE WHEN: remove the following line and the definition
   explicit MeshBase(const char* meshFile);
   protected:
   //#2 BUG MESSAGE GONE WHEN: remove the following line
   const CoreMesh   mCoreMeshPtr;
   //#3 BUG MESSAGE GONE WHEN: remove following line
   friend void getCoreMesh<CoreMesh>(MeshBase<CoreMesh>& mesh);
}; // end class MeshBase

//#1 BUG MESSAGE GONE WHEN: remove the following line and the declaration
template <>
MeshBase<PolyMesh >::
MeshBase(const char* meshFile);

} // end namespace Geometry

//#4 BUG MESSAGE GONE WHEN: remove the following line
namespace Geometry
{
} // end namespace Geometry

using namespace Geometry;
template void
Geometry::getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);

with ROSE I get the following error:
Error in scope: declaration = 0x4006c3a8 = SgFunctionParameterList
definingDeclaration ========= 0x4006c3a8 = default name =
SgFunctionParameterList: get_scope() = 0x88e63a0 =
SgNamespaceDefinitionStatement = Geometry
firstNondefiningDeclaration = 0x4006bff8 = default name =
SgFunctionParameterList: get_scope() = 0x88e63d0 =
SgNamespaceDefinitionStatement = Geometry
definingDeclaration parent = 0x4032e2f8 =
SgTemplateInstantiationFunctionDecl = getCoreMesh < Geometry::PolyMesh >
identityTranslator:
../../../../NEW_ROSE/src/midend/astDiagnostics/AstConsistencyTests.C:1721:
virtual void
TestAstForProperlySetDefiningAndNondefiningDeclarations::visit(SgNode*):
Assertion definingDeclaration->get_scope() ==
firstNondefiningDeclaration->get_scope() failed.
/home/saebjornsen1/links/g++3.4.3: line 4: 24691 Aborted
/home/saebjornsen1/ROSE-October/gcc3.4.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator
*/



#if 1
// #4 BUG MESSAGE GONE WHEN: remove the following line
// This causes the scope of subsequent declarations to reference this scope
// instead of the scope of the firstDefiningDeclaration.
namespace Geometry
   {
   } // end namespace Geometry
#endif

namespace Geometry
   {
     class PolyMesh
        {
          public:
        };

     template <typename CoreMesh>
     class MeshBase;

     template <typename CoreMesh>
     void getCoreMesh(MeshBase<CoreMesh>& mesh) {}

     template <typename CoreMesh>
     class MeshBase
        {
          public:
            // #1 BUG MESSAGE GONE WHEN: remove the following line and the definition
               explicit MeshBase(const char* meshFile);
          protected:
            // #2 BUG MESSAGE GONE WHEN: remove the following line
               const CoreMesh   mCoreMeshPtr;
            // #3 BUG MESSAGE GONE WHEN: remove following line
               friend void getCoreMesh<CoreMesh>(MeshBase<CoreMesh>& mesh);
        }; // end class MeshBase

  // #1 BUG MESSAGE GONE WHEN: remove the following line and the declaration
     template <>
     MeshBase<PolyMesh >::MeshBase(const char* meshFile);

   } // end namespace Geometry

#if 0
// #4 BUG MESSAGE GONE WHEN: remove the following line
// This causes the scope of subsequent declarations to reference this scope
// instead of the scope of the firstDefiningDeclaration.
namespace Geometry
   {
   } // end namespace Geometry
#endif

using namespace Geometry;

// Note that specification of template argument as in getCoreMesh<Geometry::PolyMesh>
// is optional but only one instantiation directive is allowed
// template void Geometry::getCoreMesh<Geometry::PolyMesh>(Geometry::MeshBase<Geometry::PolyMesh >&);

// The following is not allowed:
// template void Geometry::getCoreMesh<Geometry::PolyMesh>(Geometry::MeshBase<>&);

// This is permitted (i.e. without the template argument explicitly
// specified as in getCoreMesh<Geometry::PolyMesh>
template void Geometry::getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);
