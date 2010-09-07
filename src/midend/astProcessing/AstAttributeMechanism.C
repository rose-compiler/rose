

#include "sage3basic.h"
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
	//Implementations need to overload this to make a proper copy.
	return NULL;
}

bool
AstAttribute::commentOutNodeInGraph()
   {
     return false;
   }


// ********************************************
//          MetricAttribute
// ********************************************

MetricAttribute::MetricAttribute() :
    is_derived_(false),value_(0)
{
}

MetricAttribute::MetricAttribute(double value, bool is_derived) :
    is_derived_(is_derived),value_(value)
{
}

MetricAttribute& MetricAttribute::operator+= (const MetricAttribute & other)
{
    is_derived_ = true;
    value_ += other.value_;
    return *this;
}

MetricAttribute& MetricAttribute::operator-= (const MetricAttribute & other)
{
    is_derived_ = true;
    value_ -= other.value_;
    return *this;
}

MetricAttribute& MetricAttribute::operator*= (const MetricAttribute & other)
{
    is_derived_ = true;
    value_ *= other.value_;
    return *this;
}

MetricAttribute& MetricAttribute::operator/= (const MetricAttribute & other)
{
    is_derived_ = true;
    value_ /= other.value_;
    return *this;
}

AstAttribute* MetricAttribute::constructor()
{
    return new MetricAttribute();
}

AstAttribute* MetricAttribute::copy()
{
    return new MetricAttribute(*this);
}

std::string MetricAttribute::attribute_class_name()
{
    return "MetricAttribute";
}

bool MetricAttribute::isDerived() const
{
    return is_derived_;
}

double MetricAttribute::getValue() const
{
    return value_;
}

void MetricAttribute::setValue(double newVal)
{
    value_ = newVal;
}

std::string MetricAttribute::toString ()
{
    std::ostringstream ostr;
    ostr << value_;
    std::string retval = ostr.str ();

    if (is_derived_)
        retval += "**";
    return retval;
}


int MetricAttribute::packed_size()
{
    // +1 because of null-character
    return toString().size()+1;
}

char* MetricAttribute::packed_data()
{
    std::string result;
    //toString() will entail the value string with ** if it is derived.
    result = toString();
    return const_cast<char *>(result.c_str());
}

void MetricAttribute::unpacked_data(int size, char* data)
{
    if (size <=0) return;
    // check tail **
    char * head = data;
    char * tail = head + strlen(head) - 1;
    if (*tail=='*')
    is_derived_ = true;

    // retrieve value
    // strtod() is smart enough to skip tab and ignore tailing **
    char * endp;
    value_=strtod(head,&endp);
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

// ********************************************
//              AstRegExAttribute
// ********************************************

AstRegExAttribute::AstRegExAttribute()
   : expression("")
   {
   }

AstRegExAttribute::AstRegExAttribute(const std::string & s)
   {
     expression = s;
   }

