#ifndef MAPPING_INFORMATION
#define MAPPING_INFORMATION

#include "Mapping.h"
#include "ListOfMappingRC.h"
#include "GenericGraphicsInterface.h"

class MappingInformation
{
 public:
  ListOfMappingRC mappingList;
  GenericGraphicsInterface *graphXInterface;

  enum commandOptions
  {
    interactive,      // accept commands interactively
    readOneCommand,   // read one command from the aString command
    readUntilEmpty    // read commands until an empty string is encountered
  };
  commandOptions commandOption;
  aString *command;   // Array of strings with possible commands  

  DialogData *interface; // pointer to a sibling window, where an interface could be built
  GraphicsParameters *gp_; // pointer to a graphics parameter object
  SelectionInfo *selectPtr; // pointer to a selection object

  MappingInformation(){ commandOption=interactive; command=NULL; interface=NULL; gp_=NULL; selectPtr = NULL;};
  ~MappingInformation(){};
  

}; 


#endif
