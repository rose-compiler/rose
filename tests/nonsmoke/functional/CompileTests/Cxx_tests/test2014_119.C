#include <memory>

namespace {

class MatOpCommand 
   {
     private:
          virtual bool replaceMaterials(unsigned zone_index) const = 0;
   };

class CleanMatOpCommand : public MatOpCommand 
   {
     public:
          explicit CleanMatOpCommand(double min_volume_fraction) : m_min_volume_fraction(min_volume_fraction) {}

     private:
          virtual bool replaceMaterials(unsigned zone_index) const;

          double m_min_volume_fraction;
   };

bool CleanMatOpCommand::replaceMaterials(unsigned zone_index) const
   {
     return false;
   }

std::auto_ptr<MatOpCommand> parseCleanCommand()
   {
     double min_volume_fraction;
  // Bug: this unparses to be:
  // return ((std::auto_ptr< MatOpCommand > ::auto_ptr((new CleanMatOpCommand (min_volume_fraction))) . operator std::auto_ptr_ref<<unnamed>::MatOpCommand>()));
  // and should be:
  // return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(min_volume_fraction));
     return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(min_volume_fraction));
   }
}
