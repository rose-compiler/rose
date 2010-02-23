// Test of template directive

class PolygonalMesh
   {
   };

// Templated class
template <class MeshType>
class ArtificialViscosity
   {
     public:
       // member functions
          void computeZonalLengthScale();
   };

template<class T>
class DereferenceVector
   {
     public:

   };

// Templated class
template<class MeshType>
class ArtificialViscosityList : public DereferenceVector< ArtificialViscosity<MeshType> >
   {
     private:
          typedef DereferenceVector< ArtificialViscosity<MeshType> > VectorType;
   };

// Template Forward declaration
template<class MeshType> class ArtificialViscosity;

#if 0
// Template specialization
template <> 
class ArtificialViscosity<PolygonalMesh>
   {
     public:
       // member functions
          void computeZonalLengthScale();
   };
#endif

template<class MeshType>
void
ArtificialViscosity<MeshType>::computeZonalLengthScale()
   {
   }

// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ == 4)
template<>
#endif
// Template specialization
void
ArtificialViscosity<PolygonalMesh>::computeZonalLengthScale()
   {
   }

// Template Instantiation Directive
template class ArtificialViscosity<PolygonalMesh>;


