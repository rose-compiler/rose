
namespace Geometry
   {
  // Forward declaration of class that will be defined in an alternative re-entrant namespace definition of: Geometry
     class Zone;
   }

namespace Geometry
   {
  // Now we define the class Zone defined within Geometry::PolyMesh
  // This causes the symbol for Zone class to be placed into global scope (a mistake/bug).
     class Zone
        {
          public:
        };
   }


Geometry::Zone sp_1;

