// vim:sw=4 ts=4:

#include <typeinfo>
#include <cassert>
#include <sstream>
#include <iostream>
#include <boost/foreach.hpp>

#include "RsType.h"

#include "CppRuntimeSystem.h"


RsType& RsType::UnknownType = *(new RsCompoundType( "Unknown", 0 ));

const RsType*
RsType::getSubtypeRecursive(size_t offset, size_t size, bool stopAtArray, std::string * navString) const
{
    const RsType* result = this;

    if(navString)
        *navString = getName();

    while(result->getByteSize() >= size)
    {
        if(stopAtArray)
        {
            const RsArrayType * arr = dynamic_cast<const RsArrayType*>(result);
            if (arr && arr->getBaseType()->getByteSize() == size)
            {
                int index = arr->arrayIndex(offset);
                return index == -1 ? NULL : result;
            }
        }

        int subTypeId = result->getSubtypeIdAt(offset);
        if(subTypeId == -1)
        {
            return result -> getByteSize() == size
                    // The subtype completely fills the parent type, and no
                    // further refinement is possible.
                    ? result
                    // The subtype is larger than the type requested, and cannot
                    // be refined.
                    : NULL;
        }

        if(navString)
            (*navString) += "." + result->getSubTypeString(subTypeId);

        offset -= result->getSubtypeOffset(subTypeId);
        const RsType* subtype = result->getSubtype(subTypeId);

        if( subtype -> getByteSize() < size )
            break;

        result = subtype;
    }

    if (result->getByteSize() != size || offset != 0)
    {
        return NULL;
    }

    return result;
}

bool RsType::checkSubtypeRecursive(size_t offset, const RsType* type) const
{
  //  RuntimeSystem * rs = RuntimeSystem::instance();
  //rs->printMessage("    >>> checkSubtypeRecursive ");
  const RsType* result = this;
  size_t        size = type -> getByteSize();


  unsigned int resultsize = result->getByteSize();
  while(resultsize >= size) {
    //rs->printMessage("   >> while result->getByteSize() >= size "+
    //           ToString(resultsize)+" : " +ToString(size));
    //rs->printMessage("        >>> checking  result == type :   "+result->getName()+
    //           "  "+type->getName());
    if( result == type )
      return true;

    // tps (09/10/09) Handle union type
    const RsClassType* ct_result = dynamic_cast<const RsClassType*>( result);
    const bool isunion = (ct_result && ct_result->isUnionType());

    int subTypeId = -1;
    if (isunion)
    {
      std::vector<int> subTypeIdvec = ct_result->getSubtypeUnionIdAt(offset);

      if (subTypeIdvec.size() == 0) return false;

      std::vector<int>::const_iterator it = subTypeIdvec.begin();
      // iterate over the members and find the matching one
      for (; it!=subTypeIdvec.end(); ++it) {
        subTypeId = *it;

        const RsType* temp_result = result->getSubtype(subTypeId);
        if (temp_result==type)
          break;
      }
    }
    else
    {
      subTypeId = result->getSubtypeIdAt(offset);

      if (subTypeId == -1) return false;
    }

    //rs->printMessage("   >> subTypeId: "+ToString(subTypeId)+"  isunion:"+
    //     ToString(isunion));

    // continue as before and get the subtype
    offset -= result->getSubtypeOffset(subTypeId);
    //rs->printMessage("       >> new offset :  offset -= result->getSubtypeOffset(subTypeId); "+
    //         ToString(offset));
    result  = result->getSubtype(subTypeId);
    //rs->printMessage("       >> result  = result->getSubtype(subTypeId) : "+
    //     result->getName()+"\n");
    if (!isunion)
      resultsize = result->getByteSize();
  }
  //rs->printMessage("    >>> result: bytesize: " + ToString( result -> getByteSize())+
  //                " !=  size: "+ToString(size));

  assert( result == NULL || result -> getByteSize() != size );
  return false;
}

bool RsType::isConsistentWith( const RsType &other ) const
{
    return (  checkConsistencyWith(other)
           || (  typeid(*this) != typeid(other)
              && other.checkConsistencyWith(*this)
              )
           );
}


std::ostream& operator<< (std::ostream &os, const RsType* m)
{
    m->print(os);
    return os;
}

std::ostream& operator<< (std::ostream &os, const RsType& m)
{
    m.print(os);
    return os;
}


// ---------------------------------- RsArrayType ------------------------------------------


RsArrayType::RsArrayType(const RsType* baseType_, size_t size__)
    : RsType(getArrayTypeName(baseType_, size__)),
      baseType(baseType_)
{
    size_t base_size = baseType -> getByteSize();

    assert( 0 != base_size );
    assert( 0 == size__ % base_size );

    elementCount = size__ / base_size;
    assert( elementCount>0 );
}

size_t RsArrayType::getByteSize() const
{
    return elementCount * baseType->getByteSize();
}

int RsArrayType::getSubtypeCount() const
{
    return elementCount;
}

const RsType* RsArrayType::getSubtype(int i) const
{
    if(i>=0 && i<(int)elementCount)
        return baseType;
    else
        return NULL;
}

int RsArrayType::getSubtypeOffset(int id) const
{
    if(id >= 0 && id < (int)elementCount)
        return id*baseType->getByteSize(); //no padding because homogenous types
    else
        return -1;
}

int RsArrayType::getSubtypeIdAt(size_t offset) const
{
    if( ! isValidOffset(offset))
        return -1;
    return offset / baseType->getByteSize();
}

const RsType* RsArrayType::getSubtypeAt ( size_t offset) const
{
    if( ! isValidOffset(offset))
        return NULL;
    else
        return baseType;
}



bool  RsArrayType::isValidOffset(size_t offset) const
{
   // RuntimeSystem * rs = RuntimeSystem::instance();
    //rs->printMessage("        ... isValidOffset: offset >= getByteSize()   "+
    //         ToString(offset)+ " >= "+ToString(getByteSize()));
    if(offset >= getByteSize())
        return false;

    int inTypeOffset = offset % baseType->getByteSize();

    //rs->printMessage(" baseType->isValidOffset(inTypeOffset)==false   "+
    //         ToString( baseType->isValidOffset(inTypeOffset)));
    return baseType->isValidOffset(inTypeOffset);
}

std::string RsArrayType::getArrayTypeName(const RsType* basetype, size_t size)
{
    assert(basetype);

    std::stringstream s;
    s << "__array_" << basetype->getName() << "_" << size;
    return s.str();
}


std::string RsArrayType::getSubTypeString(int id) const
{
    assert(id >=0 && id < (int)elementCount);
    std::stringstream ss;
    ss << "[" << id << "]";
    return ss.str();
}


int RsArrayType::arrayIndex(size_t offset) const
{
    size_t baseSize = baseType->getByteSize();

    //~ std::cerr << "ofs: " << offset
              //~ << " base: " << baseSize
              //~ << " elems: " << elementCount
              //~ << std::endl;

    if ((offset % baseSize) != 0)
        return -1; //invalid (in between elements)

    if (offset >= (elementCount * baseSize))
        return -1; //invalid (behind array)

    return offset / baseSize;
}


void RsArrayType::print(std::ostream & os) const
{
    os << "ArrayType Size " << elementCount << " of type: "  << baseType->getName() << std::endl;
}


std::string RsArrayType::getDisplayName() const
{
    std::stringstream s;
    s << "Array of " << baseType->getDisplayName() << " Size:" << elementCount;
    return s.str();
}

bool RsArrayType::checkConsistencyWith( const RsType &other ) const {
    return RsType::checkConsistencyWith( other )
            || other.isConsistentWith( *getBaseType() );
}

int RsArrayType::getKnownSubtypesOverlappingRange(size_t range_start, size_t range_end ) const
{
    int rv = 0;

    int span = range_end - range_start;
    if( span > 0 )
        rv += span / (getBaseType() -> getByteSize());

    return rv;
}


// ---------------------------------------- RsClassType ---------------------------------------


RsClassType::RsClassType(const std::string & name, size_t byteSize_, bool isUnion)
: RsType(name), relaxed(false), byteSize(byteSize_), isunionType(isUnion), members()
{
    assert( !name.empty() );
    // assert( byteSize_ != 0 );
}


int RsClassType::addMember(const std::string & name, const RsType* type, size_t offset)
{
    assert(type!=NULL); // Tried to register MemberPointer with NULL type
    assert((int)offset != -1);

    if (diagnostics::message(diagnostics::type))
    {
      std::stringstream msg;

      msg << "  adding member : " << name
          << "  type: " << type->getName()
          << "  offset: " << ToString(offset);

      RuntimeSystem::instance()->printMessage(msg.str());
    }

    if (members.size() > 0)
    {
        Member& last = members.back();
        // do not assert if the class is a unionType
        //cerr << " is union type ? : " << isunionType << std::endl;
        assert(isunionType || (last.offset + last.type->getByteSize() <= offset));
    }

    // tps (09/09/2009) This test does not apply when the SgClassType is a union
    // \pp why not?
    assert(offset + type->getByteSize() <= byteSize);
    members.push_back(Member(name, type, offset));

    return members.size()-1;
}

void RsClassType::setUnionType( bool is_union ) {
    isunionType = is_union;
}


bool RsClassType::isComplete(bool verbose) const
{
    if ( diagnostics::message(diagnostics::memory) )
    {
        std::stringstream stream;

        stream << "Padding info for type " << stringId << std::endl;
        for(unsigned int i=1; i<members.size(); i++)
        {
            int diff = members[i].offset;
            diff -= members[i-1].offset + members[i-1].type->getByteSize();

            if(diff > 0)
            {
              stream << "Padding between "
                     << members[i-1].name  <<  " (" << members[i-1].type->getName() << ")  and "
                     << members[i]  .name  <<  " (" << members[i].  type->getName() << ") "
                     << diff << " Bytes" << std::endl;
            }
         }

         RuntimeSystem::instance()->printMessage(stream.str());
    }

    if(members.size()==0)
        return false;

    return (members.back().offset + members.back().type->getByteSize() == byteSize);

}


int RsClassType::getSubtypeCount() const
{
    return members.size();
}


const RsType* RsClassType::getSubtype(int i) const
{
    assert(i>=0 && i<(int)members.size());
    return members[i].type;
}

int RsClassType::getSubtypeOffset(int id) const
{
    assert(id>=0 && id<(int)members.size());
    return members[id].offset;
}

int RsClassType::getSubtypeIdAt(size_t offset) const
{
  // RuntimeSystem * rs = RuntimeSystem::instance();
  //rs->printMessage("      ....... isValidOffset(offset >=getByteSize) : "+
  //         ToString(offset)+"  >= "+ToString(getByteSize()));
  if( offset >= getByteSize())
    return -1;

  for(int i=members.size()-1 ;  i >= 0; i--) {
    if(offset >= members[i].offset) {
      // TODO register privates - this check fails if not all members are registered
      // and currently privates are not registered -> so this check fails when trying to access privates
      return members[i].type->isValidOffset(offset - members[i].offset) ? i : -1;
    }
  }

  return -1;
}

std::vector<int> RsClassType::getSubtypeUnionIdAt(size_t offset) const
{
    std::vector<int> retvalvec;
    // RuntimeSystem * rs = RuntimeSystem::instance();
    // rs->printMessage("      ....... isValidOffset(offset >=getByteSize) : "+
    //         ToString(offset)+"  >= "+ToString(getByteSize()));
    if( offset >= getByteSize())
        return retvalvec;

    //rs->printMessage("      ....... iterate through members : "+
    //         ToString(members.size()));

    for(int i=members.size()-1 ;  i >= 0; i--)
    {
        if(offset >= members[i].offset)
        {
            // TODO register privates - this check fails if not all members are registered
            // and currently privates are not registered -> so this check fails when trying to access privates
      //rs->printMessage("      .... iterate member : "+ToString(i)+
            //       "! members[i].type->isValidOffset(offset - members[i].offset)  : "+
            //       " members[i].offset: "+ToString(members[i].offset)
            //       +" offset - members[i].offset : " + ToString(offset - members[i].offset));
            if (! members[i].type->isValidOffset(offset - members[i].offset) ) {
                //rs->printMessage("     .. didnt work : "+ToString(i));
                return retvalvec;
            }   else {
                //rs->printMessage("     .. worked : "+ToString(i));
                // because the union (class) needs the largest member to perform
                // this operation successfully, we need to return the largest member
                retvalvec.push_back(i);
            }
        }
    }
    return retvalvec;
}

const RsType* RsClassType::getSubtypeAt(size_t offset) const
{
    int id = getSubtypeIdAt(offset);
    if (id==-1)
        return NULL;
    else
        return members[id].type;
}


bool RsClassType::isValidOffset(size_t offset) const
{
    if (offset >= byteSize)
        return false;

    return getSubtypeIdAt(offset) != -1;
}

std::string RsClassType::getSubTypeString(int id) const
{
    assert(id >=0 && id < (int)members.size());
    return members[id].name;
}



void RsClassType::print(std::ostream & os) const
{
    os << "Class " << getName() << " Size: " << byteSize <<std::endl;

    for(size_t i=0; i < members.size(); i++)
    {
        os << "  " << members[i].offset << " " <<
                      members[i].name << "\t" <<
                      members[i].type->getName() << std::endl;
    }
}

int RsClassType::getKnownSubtypesOverlappingRange(size_t range_start, size_t range_end) const
{
    int rv = 0;

    int span = range_end - range_start;
    if( span > 0 )
    {
        BOOST_FOREACH( Member m, members ) {
            size_t m_start = m.offset;
            size_t m_end = m.offset + m.type->getByteSize();

            if( m_start < range_end && m_end > range_start )
                ++rv;
        }
    }

    return rv;
}

bool RsClassType::checkConsistencyWith(const RsType& other) const {

    if( !relaxed )
        return RsType::checkConsistencyWith( other );

    BOOST_FOREACH( Member m, members )
    {
        const RsType& type = *(m.type);

        // Range is m.offset..m.offset=m.size
        // if other has 1 known type there, and >= 0 unknown types, check consistency
        // if other has > 1 non-unknown types
        //      my type must be consistent with all of other's types
        // if other has 0 types, offset is illegal, fail
        const RsType* partner = other.getSubtypeAt( m.offset );

        if ( !partner )
            // Other has no type at that offset
        {
            return false;
        }

        const size_t overlaprange = other.getKnownSubtypesOverlappingRange( m.offset + partner->getByteSize(),
                                                                            m.offset + m.type -> getByteSize()
                                                                          );
        if ( overlaprange > 0 )
            // Our single type overlaps multiple other non-unknown types
            // e.g. int[2] overlapping int, char or int, int
        {
            return false;
        }

        // Our member type is the same width as other's, minus space of unknown
        // type, but it may still be inconsistent (e.g. char[4] vs. int)
        if ( !type.isConsistentWith(*partner) )
        {
            return false;
        }
    }

    // we didn't find any inconsistency
    return true;
}

// ----------------------------------- RsCompoundType-----------------------------------

int RsCompoundType::addMember(const std::string & name, const RsType* type, size_t offset) {
    if( this -> byteSize < offset + type -> getByteSize() )
        this -> byteSize += type -> getByteSize();
    return RsClassType::addMember( name, type, offset );
}

const RsType* RsCompoundType::getSubtypeAt( size_t offset ) const {
    const RsType* type = RsClassType::getSubtypeAt( offset );

    return (type != NULL) ? type : &RsType::UnknownType;
}


// ----------------------------------- RsBasicType --------------------------------------


void RsBasicType::print(std::ostream & os) const
{
    os << "Basic Type: " << getName() << std::endl;
}

std::string RsBasicType::getDisplayName() const
{
    switch(type)
    {
        case SgTypeBool:             return "bool";
        case SgTypeChar:             return "char";
        case SgTypeWchar:            return "wchar_t";
        // FIXME 2: typedef should be removed (see todo in RsType.h)
        case SgTypedefType:          return "typedef";
        case SgTypeDouble:           return "double";
        case SgTypeFloat:            return "float";
        case SgTypeInt:              return "int";
        case SgTypeLong:             return "long";
        case SgTypeLongDouble:       return "long double";
        case SgTypeLongLong:         return "long long";
        case SgTypeShort:            return "short";
        case SgTypeSignedChar:       return "signed char";
        case SgTypeSignedInt:        return "signed int";
        case SgTypeSignedLong:       return "signed long";
        case SgTypeSignedLongLong:   return "signed long long";
        case SgTypeSignedShort:      return "signed short";
        case SgTypeUnsignedChar:     return "unsigned char";
        case SgTypeUnsignedInt:      return "unsigned int";
        case SgTypeUnsignedLong:     return "unsigned long";
        case SgTypeUnsignedLongLong: return "unsigned long long";
        case SgTypeUnsignedShort:    return "unsigned short";
        case SgTypeString:           return "std::string";
        case SgTypeVoid:             return "void";
        case SgPointerType:          return "pointer";
       // case SgReferenceType:        return "reference";
        default:                     return "Unknown";
    }
}


std::string RsBasicType::readValueAt(Address addr) const
{
    std::string          res;
    const MemoryManager* m = rtedRTS(this)->getMemManager();

    switch (type)
    {
        case SgTypeBool:             res = m->readMemory<bool>(addr);                break;
        case SgTypeChar:             res = m->readMemory<char>(addr);                break;
        case SgTypeWchar:            res = m->readMemory<wchar_t>(addr);             break;
        case SgTypeDouble:           res = m->readMemory<double>(addr);              break;
        case SgTypeFloat:            res = m->readMemory<float>(addr);               break;
        case SgTypeInt:              res = m->readMemory<int>(addr);                 break;
        case SgTypeLong:             res = m->readMemory<long>(addr);                break;
        case SgTypeLongDouble:       res = m->readMemory<long double>(addr);         break;
        case SgTypeLongLong:         res = m->readMemory<long long>(addr);           break;
        case SgTypeShort:            res = m->readMemory<short>(addr);               break;
        case SgTypeSignedChar:       res = m->readMemory<signed char>(addr);         break;
        case SgTypeSignedInt:        res = m->readMemory<signed int>(addr);          break;
        case SgTypeSignedLong:       res = m->readMemory<signed long>(addr);         break;
        case SgTypeSignedLongLong:   res = m->readMemory<signed long long>(addr);    break;
        case SgTypeSignedShort:      res = m->readMemory<signed short>(addr);        break;
        case SgTypeUnsignedChar:     res = m->readMemory<unsigned char>(addr);       break;
        case SgTypeUnsignedInt:      res = m->readMemory<unsigned int>(addr);        break;
        case SgTypeUnsignedLong:     res = m->readMemory<unsigned long>(addr);       break;
        case SgTypeUnsignedLongLong: res = m->readMemory<unsigned long long>(addr);  break;
        case SgTypeUnsignedShort:    res = m->readMemory<unsigned short>(addr);      break;
        case SgTypeString:           res = m->readMemory<char*>(addr);               break;
        default:                     std::cerr << "RsBasicType::readValueAt with unknown type";
                                     assert(false);
    }

    return res;
}

RsBasicType RsBasicType::create(SgType ty)
{
  const char* name = NULL;
  size_t      sz = 0;

  switch(ty)
  {
    case SgTypeBool:             name = "SgTypeBool";             sz = sizeof(bool);               break;
    case SgTypeChar:             name = "SgTypeChar";             sz = sizeof(char);               break;
    case SgTypeWchar:            name = "SgTypeWchar";            sz = sizeof(wchar_t);            break;
    case SgTypeDouble:           name = "SgTypeDouble";           sz = sizeof(double);             break;
    case SgTypeFloat:            name = "SgTypeFloat";            sz = sizeof(float);              break;
    case SgTypeInt:              name = "SgTypeInt";              sz = sizeof(int);                break;
    case SgTypeLong:             name = "SgTypeLong";             sz = sizeof(long);               break;
    case SgTypeLongDouble:       name = "SgTypeLongDouble";       sz = sizeof(long double);        break;
    case SgTypeLongLong:         name = "SgTypeLongLong";         sz = sizeof(long long);          break;
    case SgTypeShort:            name = "SgTypeShort";            sz = sizeof(short);              break;
    case SgTypeSignedChar:       name = "SgTypeSignedChar";       sz = sizeof(signed char);        break;
    case SgTypeSignedInt:        name = "SgTypeSignedInt";        sz = sizeof(signed int);         break;
    case SgTypeSignedLong:       name = "SgTypeSignedLong";       sz = sizeof(signed long);        break;
    case SgTypeSignedLongLong:   name = "SgTypeSignedLongLong";   sz = sizeof(signed long long);   break;
    case SgTypeSignedShort:      name = "SgTypeSignedShort";      sz = sizeof(signed short);       break;
    case SgTypeUnsignedChar:     name = "SgTypeUnsignedChar";     sz = sizeof(unsigned char);      break;
    case SgTypeUnsignedInt:      name = "SgTypeUnsignedInt";      sz = sizeof(unsigned int);       break;
    case SgTypeUnsignedLong:     name = "SgTypeUnsignedLong";     sz = sizeof(unsigned long);      break;
    case SgTypeUnsignedLongLong: name = "SgTypeUnsignedLongLong"; sz = sizeof(unsigned long long); break;
    case SgTypeUnsignedShort:    name = "SgTypeUnsignedShort";    sz = sizeof(unsigned short);     break;
    case SgTypeString:           name = "SgTypeString";           sz = sizeof(char*);              break;
    case SgPointerType:          name = "SgPointerType";          sz = sizeof(void*);              break;

    // FIXME 2: typedef should be removed (see todo in RsType.h)
    case SgTypedefType:          name = "SgTypedefType";          sz = sizeof(void*);              break;

    case SgTypeVoid:             name = "SgTypeVoid";             sz = 0;                          break;
    default:                     name = "Unknown";                sz = 0;
  }

  return RsBasicType(name, ty, sz);
}


// ----------------------------------- RsPointerType --------------------------------------


RsPointerType::RsPointerType(const RsType* baseType_)
: RsBasicType( RsBasicType::create(RsBasicType::SgPointerType) ), baseType(baseType_)
{}

std::string RsPointerType::getDisplayName() const
{
    return baseType->getDisplayName() + "*";
}

const char RsClassType::invalidname[] = "-";


#if CURRENTLY_NOT_USED

// ----------------------------------- RsTypeDef --------------------------------------



void RsTypeDef::print(std::ostream & os) const
{
    os << "Typedef from " << refType->getName() << " to " << getName() << std::endl;
}

#endif /* CURRENTLY_NOT_USED */
