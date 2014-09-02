#include <vector>

namespace {

typedef std::vector<std::vector<int> > SpeciesMap;

class ReplaceMatOpCommand
   {
     private:
          virtual bool replaceMaterials(unsigned zone_index) const;

          std::vector<int> m_materials_to_replace;
          std::vector<int> m_replacement_materials;
   };

bool ReplaceMatOpCommand::replaceMaterials(unsigned zone_index) const
   {
     typedef std::vector<int>::const_iterator VIIter;

     VIIter mat_iter;

  // Bug: is unparsed as: "int replacement_material =  * ((this) -> m_replacement_materials . begin() + mat_iter-(this) -> m_materials_to_replace . begin());"
  // This appears to be an operator presidence issue between operator+() and operator-().
     int replacement_material = *(m_replacement_materials.begin() + (mat_iter - m_materials_to_replace.begin()));

     return false;
   }

}

