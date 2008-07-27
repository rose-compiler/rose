#include "sage3.h"
#include "roseInternal.h"

#include "AstAttributeMechanism.h"

// Moved function definitions from header file to simplify debugging

// ********************************************
//                AstAttribute
// ********************************************

AstAttribute::AstAttribute()
   {
   }

AstAttribute::~AstAttribute()
   {
   }

std::string
AstAttribute::toString()
   {
     return "";
   }

AstAttribute* 
AstAttribute::constructor()
   {
     return new AstAttribute();
   }

std::string 
AstAttribute::attribute_class_name()
   {
     return "AstAttribute";
   }

int 
AstAttribute::packed_size()
   {
     return 0;
   }

char* 
AstAttribute::packed_data()
   {
     return NULL;
   }

void 
AstAttribute::unpacked_data( int size, char* data )
   {
   }

std::string 
AstAttribute::additionalNodeOptions()
   {
     return "";
   }

std::vector<AstAttribute::AttributeEdgeInfo> 
AstAttribute::additionalEdgeInfo()
   {
     std::vector<AttributeEdgeInfo> v;
     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo> 
AstAttribute::additionalNodeInfo()
   {
     std::vector<AttributeNodeInfo> v;
     return v;
   }

AstAttribute* 
AstAttribute::copy()
   {
     return new AstAttribute(*this);
   }

bool 
AstAttribute::commentOutNodeInGraph()
   {
     return false;
   }


// ********************************************
//          AstAttributeMechanism
// ********************************************

AstAttributeMechanism::AstAttributeMechanism ()
   {
  // Nothing to do here!
   }

AstAttributeMechanism::AstAttributeMechanism ( const AstAttributeMechanism & X )
   {
  // This is the copy constructor to support deep copies of AST attribute containers.
  // this is important for the support of the AST Copy mechanism (used all over the place,
  // but being tested in new ways within the bug seeding project).

  // Note that AstAttributeMechanism is derived from: AttributeMechanism<std::string,AstAttribute*> 
  // Which is defined as: template<class Key,class Value> class AttributeMechanism : protected std::map<Key,Value>
#if 1
  // Iterate over all the elements of the map in X and copy them to the current map (this)
     for (const_iterator iter = X.begin(); iter != X.end(); iter++)
        {
       // Call the copy mechanism on the AstAttribute (virtual copy constructor)
          this->insert(std::pair<std::string,AstAttribute*>( iter->first , iter->second->copy() ) );
        }
#else
  // ((const AttributeMechanism<std::string,AstAttribute*>*) this) = X;
     *this = X;
#endif
   }


