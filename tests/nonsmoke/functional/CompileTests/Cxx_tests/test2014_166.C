#include <memory>

// namespace {

class MatOpCommand {};

class CleanMatOpCommand : public MatOpCommand 
   {
     public:
       // explicit CleanMatOpCommand(double min_volume_fraction);
       // CleanMatOpCommand(double min_volume_fraction);
       // explicit CleanMatOpCommand(double min_volume_fraction) : m_min_volume_fraction(min_volume_fraction) {}
       // CleanMatOpCommand(int min_volume_fraction);
          CleanMatOpCommand(int min_volume_fraction) {}
   };

std::auto_ptr<MatOpCommand> parseCleanCommand()
   {
     double min_volume_fraction;
  // Bug: this unparses to be:
  // return ((std::auto_ptr< MatOpCommand > ::auto_ptr((new CleanMatOpCommand (42))) . operator std::auto_ptr_ref<MatOpCommand>()));
  // and should be:
  // return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(42));

  // return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(min_volume_fraction));
     return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(42));
   }
// }
