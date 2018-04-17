// Error: original code:
//    Field<Region, Zone, Tensor> const &stressDeviator = partItr->getStrength().getStressDeviator();
// is unparsed as:
//    const class Geometry::Field< 
//         Part < Geometry::PolygonalMesh > ::RegionType  , 
//         Part < Geometry::PolygonalMesh > ::ZoneHandle  , 
//         Part < Geometry::PolygonalMesh > ::TensorType  > & stressDeviator = ( *partItr.operator->()).getStrength().getStressDeviator();
// The fix is that we need global scope qualifier "::" before the reference to the type "Part"

class myNumberType
   {
     public:
         void foobar();
   };

typedef ::myNumberType myNumberType;

typedef myNumberType numberType;

class Y
   {
     public:
          typedef int numberType;
   };

class Z : public Y
   {
     public:
       // Such typedefs are significant (but force 
       // "typedef ::numberType numberType;" to be an error)
       // typedef numberType numberType;

       // Example of where "::" is significant, without it numberType 
       // will be an "int" instead of a "myNumberType"
          typedef ::numberType numberType;

       // Note also that if Z were not derived from Y then 
          typedef numberType numberType;
       // could have been used to refer to myNumberType

          void foo()
             {
               numberType numberValue;

           // function "foobar()" not defined for "int"
               numberValue.foobar();
             }
   };

