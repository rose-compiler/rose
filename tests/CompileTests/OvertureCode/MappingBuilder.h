#ifndef MAPPING_BUILDER_H
#define MAPPING_BUILDER_H

#include "Mapping.h"
#include "GenericGraphicsInterface.h"

class MappingInformation;

//----------------------------------------------------------------------
//  Class to build mulitple Mappings on (composite) surfaces.
//----------------------------------------------------------------------
class MappingBuilder 
{
public:

  MappingBuilder();
  ~MappingBuilder();
  
  int build(  MappingInformation & mapInfo, Mapping *surface = NULL );

};


#endif  
