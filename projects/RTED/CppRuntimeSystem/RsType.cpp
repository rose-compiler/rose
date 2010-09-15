// vim:sw=4 ts=4:
#include "RsType.h"
#include "support.h"

#include <boost/foreach.hpp>
#include <typeinfo>
#include <cassert>
#include <sstream>

using namespace std;


RsType &RsType::UnknownType = *(new RsCompoundType( "Unknown", 0 ));


RsType *  RsType::getSubtypeRecursive(addr_type offset,  size_t size, bool stopAtArray, string * navString)
{
    RsType * result = this;

    if(navString)
        *navString = getName();

    //cout << "Refining " << getName() << " at offset " << offset <<  " search of size " << size << endl;

    while(result->getByteSize() >= size)
    {
        if(stopAtArray)
        {
            RsArrayType * arr = dynamic_cast<RsArrayType*> (result);
            if(arr && arr->getBaseType()->getByteSize() == size)
            {
                int index = arr->arrayIndex(offset);
                return index == -1 ? NULL : result;
            }
        }

        int subTypeId = result->getSubtypeIdAt(offset);
        if(subTypeId == -1)
            return result -> getByteSize() == size
                    // The subtype completely fills the parent type, and no
                    // further refinement is possible.
                    ? result
                    // The subtype is larger than the type requested, and cannot
                    // be refined.
                    : NULL;

        if(navString)
            (*navString) += "." + result->getSubTypeString(subTypeId);

        offset -= result->getSubtypeOffset(subTypeId);
        RsType* subtype = result->getSubtype(subTypeId);

        if( subtype -> getByteSize() < size )
            break;

        result = subtype;
    }

    if (result->getByteSize() != size || offset != 0)
        return NULL;

    //if(navString)
    //    cout << "NavString" << *navString << endl;

    //cout << "Refinement successful " << result->getName() << " Offset" << offset<< endl;

    return result;
}

bool  RsType::checkSubtypeRecursive(addr_type offset,  RsType* type)
{
  //  RuntimeSystem * rs = RuntimeSystem::instance();
    //rs->printMessage("    >>> checkSubtypeRecursive ");
    RsType * result = this;
    size_t size = type -> getByteSize();

    bool isunion = false;
    unsigned int resultsize = result->getByteSize();
    while(resultsize >= size)
    {
      //rs->printMessage("   >> while result->getByteSize() >= size "+
      //		       ToString(resultsize)+" : " +ToString(size));
      //rs->printMessage("        >>> checking  result == type :   "+result->getName()+
      //		       "  "+type->getName());
        if( result == type )
            return true;

	// tps (09/10/09) Handle union type
	RsClassType* ct_result = dynamic_cast<RsClassType*>( result);
	if (ct_result && ct_result->getIsUnionType()) isunion=true;

        int subTypeId = -1;
	vector<int> subTypeIdvec;
	if (isunion==false)
            subTypeId = result->getSubtypeIdAt(offset);
	else 
            subTypeIdvec = ct_result->getSubtypeUnionIdAt(offset);

	//rs->printMessage("   >> subTypeId: "+ToString(subTypeId)+"  isunion:"+
	//		 ToString(isunion));
	if (isunion && subTypeIdvec.size()==0 ||
	    !isunion && subTypeId == -1 ) {
            // no refinement is possible
	    //rs->printMessage("    >>> subTypeId == -1 .");
            return false;
        }
	if (isunion) {
            vector<int>::const_iterator it = subTypeIdvec.begin();
	    // iterate over the members and find the matching one
            for (;it!=subTypeIdvec.end();++it) {
                subTypeId = *it;
  //              addr_type temp_offset = offset- result->getSubtypeOffset(subTypeId);
                RsType* temp_result =  result->getSubtype(subTypeId);
                if (temp_result==type)
                    break;
            }
	}

	// continue as before and get the subtype
	offset -= result->getSubtypeOffset(subTypeId);
	//rs->printMessage("       >> new offset :  offset -= result->getSubtypeOffset(subTypeId); "+
	//	       ToString(offset));
        result  = result->getSubtype(subTypeId);
	//rs->printMessage("       >> result  = result->getSubtype(subTypeId) : "+
	//		 result->getName()+"\n");
	if (isunion==false)
	resultsize = result->getByteSize();
    }
    //rs->printMessage("    >>> result: bytesize: " + ToString( result -> getByteSize())+
    //                " !=  size: "+ToString(size));

    assertme(  result == NULL || result -> getByteSize() != size,
               "RsType::checkSubtypeRecursive - result == NULL || result -> getByteSize() != size",
	       "",ToString(size));
    assert( result == NULL || result -> getByteSize() != size );
    return false;
}

bool RsType::isConsistentWith( const RsType &other ) const {
    if( checkConsistencyWith( other ))
        return true;
    if( typeid( *this ) != typeid( other ))
        return other.checkConsistencyWith( *this );

    return false;
}


std::ostream& operator<< (std::ostream &os, const RsType * m)
{
    m->print(os);
    return os;
}

std::ostream& operator<< (std::ostream &os, const RsType & m)
{
    m.print(os);
    return os;
}


// ---------------------------------- RsArrayType ------------------------------------------


RsArrayType::RsArrayType(RsType * baseType_, size_t size__)
    : RsType(getArrayTypeName(baseType_,size__)),
      baseType(baseType_)
{
    assert(baseType != NULL);

    size_t base_size = baseType -> getByteSize();
    assertme( 0 == size__ % base_size," RsArrayType::RsArrayType - 0 == size__ % base_size ",
              "0",ToString(size__ % base_size));
    assert( 0 == size__ % base_size );
    elementCount = size__ / base_size;

    assertme(elementCount>0, "RsArrayType::RsArrayType - elementCount>0",
             ToString(elementCount),"0");
    assert(elementCount>0);
}

size_t RsArrayType::getByteSize() const
{
    return elementCount * baseType->getByteSize();
}

int RsArrayType::getSubtypeCount() const
{
    return elementCount;
}

RsType * RsArrayType::getSubtype(int i) const
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

int RsArrayType::getSubtypeIdAt(addr_type offset) const
{
    if( ! isValidOffset(offset))
        return -1;
    return offset / baseType->getByteSize();
}

RsType * RsArrayType::getSubtypeAt ( addr_type offset) const
{
    if( ! isValidOffset(offset))
        return NULL;
    else
        return baseType;
}



bool  RsArrayType::isValidOffset(addr_type offset) const
{
   // RuntimeSystem * rs = RuntimeSystem::instance();
    //rs->printMessage("        ... isValidOffset: offset >= getByteSize()   "+
    //		     ToString(offset)+ " >= "+ToString(getByteSize()));
    if(offset >= getByteSize())
        return false;

    int inTypeOffset = offset % baseType->getByteSize();

    //rs->printMessage(" baseType->isValidOffset(inTypeOffset)==false   "+
    //		     ToString( baseType->isValidOffset(inTypeOffset)));
    if(! baseType->isValidOffset(inTypeOffset))
        return false;
    else
        return true;
}

string RsArrayType::getArrayTypeName(RsType * basetype, size_t size)
{
    stringstream s;
    s << "__array_" << basetype->getName() << "_" << size;
    return s.str();
}


string RsArrayType::getSubTypeString(int id) const
{
    assertme(id >=0 && id < (int)elementCount,"RsArrayType::getSubTypeString( - id >=0 && id < elementCount",
             ToString(id),ToString(elementCount));
    assert(id >=0 && id < (int)elementCount);
    stringstream ss;
    ss << "[" << id << "]";
    return ss.str();
}


int RsArrayType::arrayIndex(addr_type offset) const
{
    size_t baseSize = baseType->getByteSize();

    if(offset % baseSize != 0)
        return -1; //invalid (in between elements)

    if( offset >= elementCount * baseSize)
        return -1; //invalid (behind array)

    return offset / baseSize;
}


void RsArrayType::print(ostream & os) const
{
    os << "ArrayType Size " << elementCount << " of type: "  << baseType->getName() << endl;
}


std::string RsArrayType::getDisplayName() const
{
    stringstream s;
    s << "Array of " << baseType->getDisplayName() << " Size:" << elementCount;
    return s.str();
}

bool RsArrayType::checkConsistencyWith( const RsType &other ) const {
    return RsType::checkConsistencyWith( other )
            || other.isConsistentWith( *getBaseType() );
}

int RsArrayType::getKnownSubtypesOverlappingRange(
        addr_type range_start, addr_type range_end ) const {

    int rv = 0;

    int span = range_end - range_start;
    if( span > 0 )
        rv += span / (getBaseType() -> getByteSize());

    return rv;
}


// ---------------------------------------- RsClassType ---------------------------------------


RsClassType::RsClassType(const string & name, size_t byteSize_, bool isUnion)
    : RsType(name),
      byteSize(byteSize_), isunionType(isUnion)
{
    assert( !name.empty() );
}


int RsClassType::addMember(const std::string & name, RsType * type, addr_type offset)
{
    if(type==NULL)
    {
        cerr << "Tried to register MemberPointer with NULL type" << endl;
        assert( false );
        return -1;
    }

#ifdef ROSE_WITH_ROSEQT
    if( RuntimeSystem::instance() -> isQtDebuggerEnabled() ) {
        string mess = "  adding member : "+name+"  type: "+type->getName()+
          "  offset: " + ToString(offset);
        RtedDebug::instance()->addMessage(mess);
    }
#endif

    if(offset<0)
    {
        offset=0;
        if(members.size() > 0)
            offset = members.back().offset + members.back().type->getByteSize();
    }

    if(members.size() >0)
    {
        Member & last = members.back();
        // do not assert if the class is a unionType
        //cerr << " is union type ? : " << isunionType << endl;
        if (isunionType==false) {
        assertme(last.offset + last.type->getByteSize() <= offset,
                 "RsClassType::addMember - last.offset + last.type->getByteSize() <= offset",
		 ToString(last.offset + last.type->getByteSize()),
		 ToString(offset));
        assert(last.offset + last.type->getByteSize() <= offset);
        }
    }

    members.push_back(Member(name,type,offset));

    // tps (09/09/2009) This test does not apply when the SgClassType is a union
    assertme(members.back().offset + members.back().type->getByteSize() <= byteSize,
             "RsClassType::addMember - members.back().offset + members.back().type->getByteSize() <= byteSize)",
	     ToString(members.back().offset + members.back().type->getByteSize() ),
	     ToString(byteSize));
    assert(members.back().offset + members.back().type->getByteSize() <= byteSize);

    return members.size()-1;
}

void RsClassType::setUnionType( bool is_union ) {
    isunionType = is_union;
}


bool RsClassType::isComplete(bool verbose) const
{
    if(verbose)
    {
        cout << "Padding info for type " << stringId << endl;
        for(unsigned int i=1; i<members.size(); i++)
        {

            int diff = members[i].offset;
            diff -= members[i-1].offset + members[i-1].type->getByteSize();
            if(diff > 0)
            {
                cout << "Padding between "
                     << members[i-1].name  <<  " (" << members[i-1].type->getName() << ")  and "
                     << members[i]  .name  <<  " (" << members[i].  type->getName() << ") "
                     << diff << " Bytes" << endl;
            }
         }
    }

    if(members.size()==0)
        return false;

    return (members.back().offset + members.back().type->getByteSize() == byteSize);

}


int RsClassType::getSubtypeCount() const
{
    return members.size();
}


RsType * RsClassType::getSubtype(int i) const
{
    assert(i>=0 && i<(int)members.size());
    return members[i].type;
}

int RsClassType::getSubtypeOffset(int id) const
{
    assert(id>=0 && id<(int)members.size());
    return members[id].offset;
}

int RsClassType::getSubtypeIdAt(addr_type offset) const
{
   // RuntimeSystem * rs = RuntimeSystem::instance();
    //rs->printMessage("      ....... isValidOffset(offset >=getByteSize) : "+
    //		     ToString(offset)+"  >= "+ToString(getByteSize()));
    if( offset >= getByteSize())
        return -1;


    for(int i=members.size()-1 ;  i >= 0; i--)
    {
        if(offset >= members[i].offset)
        {
	    // TODO register privates - this check fails if not all members are registered
	    // and currently privates are not registered -> so this check fails when trying to access privates
	    if (! members[i].type->isValidOffset(offset - members[i].offset) ) {
                return -1;
            }   else 
                return i;
	}
    }
    return -1;
}

std::vector<int> RsClassType::getSubtypeUnionIdAt(addr_type offset) const
{
    vector<int>retvalvec;
   // RuntimeSystem * rs = RuntimeSystem::instance();
    //rs->printMessage("      ....... isValidOffset(offset >=getByteSize) : "+
    //		     ToString(offset)+"  >= "+ToString(getByteSize()));
    if( offset >= getByteSize())
        return retvalvec;

    //rs->printMessage("      ....... iterate through members : "+
    //		     ToString(members.size()));

    for(int i=members.size()-1 ;  i >= 0; i--)
    {
        if(offset >= members[i].offset)
        {
            // TODO register privates - this check fails if not all members are registered
            // and currently privates are not registered -> so this check fails when trying to access privates
	    //rs->printMessage("      .... iterate member : "+ToString(i)+
            //		   "! members[i].type->isValidOffset(offset - members[i].offset)  : "+
            //		   " members[i].offset: "+ToString(members[i].offset) 
            //		   +" offset - members[i].offset : " + ToString(offset - members[i].offset));
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

RsType * RsClassType::getSubtypeAt(addr_type offset) const
{
    int id = getSubtypeIdAt(offset);
    if(id==-1)
        return NULL;
    else
        return members[id].type;
}


bool RsClassType::isValidOffset(addr_type offset) const
{
    if(offset >= byteSize)
        return false;

    return getSubtypeIdAt(offset) != -1;
}

string RsClassType::getSubTypeString(int id) const
{
    assert(id >=0 && id < (int)members.size());
    return members[id].name;
}



void RsClassType::print(ostream & os) const
{
    os << "Class " << getName() << " Size: " << byteSize <<endl;

    for(unsigned int i=0; i < members.size(); i++)
    {
        os << "  " << members[i].offset << " " <<
                      members[i].name << "\t" <<
                      members[i].type->getName() << endl;
    }
}

int RsClassType::getKnownSubtypesOverlappingRange(
        addr_type range_start, addr_type range_end ) const {

    int rv = 0;

    int span = range_end - range_start;
    if( span > 0 )
        BOOST_FOREACH( Member m, members ) {
            addr_type m_start = m.offset;
            addr_type m_end = m.offset + m.type -> getByteSize();

            if( m_start < range_end && m_end > range_start )
                ++rv;
        }

    return rv;
}

bool RsClassType::checkConsistencyWith( const RsType &other ) const {

    if( !relaxed )
        return RsType::checkConsistencyWith( other );

    BOOST_FOREACH( Member m, members ) {
        RsType &type = *(m.type);

        // Range is m.offset..m.offset=m.size
        // if other has 1 known type there, and >= 0 unknown types, check consistency
        // if other has > 1 non-unknown types
        //      my type must be consistent with all of other's types
        // if other has 0 types, offset is illegal, fail
        RsType *partner = other.getSubtypeAt( m.offset );

        if( !partner )
            // Other has no type at that offset
            return false;
        if( other.getKnownSubtypesOverlappingRange(
                    m.offset + partner -> getByteSize(),
                    m.offset + m.type -> getByteSize()) > 0 )
            // Our single type overlaps multiple other non-unknown types
            // e.g. int[2] overlapping int, char or int, int
            return false;

        // Our member type is the same width as other's, minus space of unknown
        // type, but it may still be inconsistent (e.g. char[4] vs. int)
        if( !type.isConsistentWith( *partner ))
            return false;
    }

    // we didn't find any inconsistency
    return true;
}

// ----------------------------------- RsCompoundType-----------------------------------

int RsCompoundType::addMember(const std::string & name, RsType * type, addr_type offset) {
    if( this -> byteSize < offset + type -> getByteSize() )
        this -> byteSize += type -> getByteSize();
    return RsClassType::addMember( name, type, offset );
}

RsType* RsCompoundType::getSubtypeAt( addr_type offset ) const {
    RsType* type = RsClassType::getSubtypeAt( offset );
    if( type )
        return type;

    return &RsType::UnknownType;
}


// ----------------------------------- RsTypeDef --------------------------------------

void RsTypeDef::print(ostream & os) const
{
    os << "Typedef from " << refType->getName() << " to " << getName() << endl;
}



// ----------------------------------- RsBasicType --------------------------------------


RsBasicType::RsBasicType(const std::string & typeStr)
    : RsType(typeStr)
{
    resolveTypeInfo(typeStr);
}

RsBasicType::RsBasicType(SgType t)
    : RsType("UnresolvedBasicType"),type(t)
{
    resolveTypeInfo(type);
}



void RsBasicType::print(ostream & os) const
{
    os << "Basic Type: " << getName() << endl;
}


string RsBasicType::getDisplayName() const
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
        case SgTypeString:           return "string";
        case SgTypeVoid:             return "void";
        case SgPointerType:          return "pointer";
       // case SgReferenceType:        return "reference";
        default:                     return "Unknown";
    }
}



void RsBasicType::resolveTypeInfo(const std::string & typeStr)
{
    stringId = typeStr;

    if      (typeStr=="SgTypeBool")             setTypeInfo(SgTypeBool,             sizeof(bool));
    else if (typeStr=="SgTypeChar")             setTypeInfo(SgTypeChar,             sizeof(char));
    else if (typeStr=="SgTypeWchar")            setTypeInfo(SgTypeWchar,            sizeof(wchar_t));
    // FIXME 2: typedef should be removed (see todo in RsType.h)
    else if (typeStr=="SgTypedefType")          setTypeInfo(SgTypedefType,          sizeof(void*));
    else if (typeStr=="SgTypeDouble")           setTypeInfo(SgTypeDouble,           sizeof(double));
    else if (typeStr=="SgTypeFloat")            setTypeInfo(SgTypeFloat,            sizeof(float));
    else if (typeStr=="SgTypeInt")              setTypeInfo(SgTypeInt,              sizeof(int));
    else if (typeStr=="SgTypeLong")             setTypeInfo(SgTypeLong,             sizeof(long));
    else if (typeStr=="SgTypeLongDouble")       setTypeInfo(SgTypeLongDouble,       sizeof(long double));
    else if (typeStr=="SgTypeLongLong")         setTypeInfo(SgTypeLongLong,         sizeof(long long));
    else if (typeStr=="SgTypeShort")            setTypeInfo(SgTypeShort,            sizeof(short));
    else if (typeStr=="SgTypeSignedChar")       setTypeInfo(SgTypeSignedChar,       sizeof(signed char));
    else if (typeStr=="SgTypeSignedInt")        setTypeInfo(SgTypeSignedInt,        sizeof(signed int));
    else if (typeStr=="SgTypeSignedLong")       setTypeInfo(SgTypeSignedLong,       sizeof(signed long));
    else if (typeStr=="SgTypeSignedLongLong")   setTypeInfo(SgTypeSignedLongLong,   sizeof(signed long long));
    else if (typeStr=="SgTypeSignedShort")      setTypeInfo(SgTypeSignedShort,      sizeof(signed short));
    else if (typeStr=="SgTypeUnsignedChar")     setTypeInfo(SgTypeUnsignedChar,     sizeof(unsigned char));
    else if (typeStr=="SgTypeUnsignedInt")      setTypeInfo(SgTypeUnsignedInt,      sizeof(unsigned int));
    else if (typeStr=="SgTypeUnsignedLong")     setTypeInfo(SgTypeUnsignedLong,     sizeof(unsigned long));
    else if (typeStr=="SgTypeUnsignedLongLong") setTypeInfo(SgTypeUnsignedLongLong, sizeof(unsigned long long));
    else if (typeStr=="SgTypeUnsignedShort")    setTypeInfo(SgTypeUnsignedShort,    sizeof(unsigned short));
    else if (typeStr=="SgTypeString")           setTypeInfo(SgTypeString,           sizeof(char*));
    else if (typeStr=="SgTypeVoid")             setTypeInfo(SgTypeVoid,             0);
    else if (typeStr=="SgPointerType")          setTypeInfo(SgPointerType,          sizeof(void*));
   // else if (typeStr=="SgReferenceType")        setTypeInfo(SgReferenceType,        sizeof(void*));
    else                                        setTypeInfo(Unknown, 0);
}

void RsBasicType::resolveTypeInfo(RsBasicType::SgType type_)
{
    type = type_;

    switch(type)
    {
        case SgTypeBool:             setTypeInfo("SgTypeBool",             sizeof(bool));            break;
        case SgTypeChar:             setTypeInfo("SgTypeChar",             sizeof(char));            break;
        case SgTypeWchar:            setTypeInfo("SgTypeWchar",            sizeof(wchar_t));         break;
        // FIXME 2: typedef should be removed (see todo in RsType.h)
        case SgTypedefType:          setTypeInfo("SgTypedefType",          sizeof(void*));           break;
        case SgTypeDouble:           setTypeInfo("SgTypeDouble",           sizeof(double));          break;
        case SgTypeFloat:            setTypeInfo("SgTypeFloat",            sizeof(float));           break;
        case SgTypeInt:              setTypeInfo("SgTypeInt",              sizeof(int));             break;
        case SgTypeLong:             setTypeInfo("SgTypeLong",             sizeof(long));            break;
        case SgTypeLongDouble:       setTypeInfo("SgTypeLongDouble",       sizeof(long double));     break;
        case SgTypeLongLong:         setTypeInfo("SgTypeLongLong",         sizeof(long long));       break;
        case SgTypeShort:            setTypeInfo("SgTypeShort",            sizeof(short));           break;
        case SgTypeSignedChar:       setTypeInfo("SgTypeSignedChar",       sizeof(signed char));     break;
        case SgTypeSignedInt:        setTypeInfo("SgTypeSignedInt",        sizeof(signed int));      break;
        case SgTypeSignedLong:       setTypeInfo("SgTypeSignedLong",       sizeof(signed long));     break;
        case SgTypeSignedLongLong:   setTypeInfo("SgTypeSignedLongLong",   sizeof(signed long long));break;
        case SgTypeSignedShort:      setTypeInfo("SgTypeSignedShort",      sizeof(signed short));    break;
        case SgTypeUnsignedChar:     setTypeInfo("SgTypeUnsignedChar",     sizeof(unsigned char));   break;
        case SgTypeUnsignedInt:      setTypeInfo("SgTypeUnsignedInt",      sizeof(unsigned int));    break;
        case SgTypeUnsignedLong:     setTypeInfo("SgTypeUnsignedLong",     sizeof(unsigned long));   break;
        case SgTypeUnsignedLongLong: setTypeInfo("SgTypeUnsignedLongLong", sizeof(unsigned long long)); break;
        case SgTypeUnsignedShort:    setTypeInfo("SgTypeUnsignedShort",    sizeof(unsigned short));  break;
        case SgTypeString:           setTypeInfo("SgTypeString",           sizeof(char*));           break;
        case SgTypeVoid:             setTypeInfo("SgTypeVoid",             0);                       break;
        case SgPointerType:          setTypeInfo("SgPointerType",          sizeof(void*));           break;
     //   case SgReferenceType:        setTypeInfo("SgReferenceType",        sizeof(void*));           break;
        default:                     setTypeInfo("Unknown", 0);
    }
}

#include "CppRuntimeSystem.h"

string RsBasicType::readValueAt(addr_type addr) const
{
    stringstream str;

    MemoryManager * m = RuntimeSystem::instance()->getMemManager();
    if(! m->isInitialized(addr, getByteSize()))
        return "Not initialized";


    switch(type)
    {
        case SgTypeBool:             str <<  *m->readMemory<bool>(addr);                break;
        case SgTypeChar:             str <<  *m->readMemory<char>(addr);                break;
        case SgTypeWchar:            str <<  *m->readMemory<wchar_t>(addr);             break;
        case SgTypeDouble:           str <<  *m->readMemory<double>(addr);              break;
        case SgTypeFloat:            str <<  *m->readMemory<float>(addr);               break;
        case SgTypeInt:              str <<  *m->readMemory<int>(addr);                 break;
        case SgTypeLong:             str <<  *m->readMemory<long>(addr);                break;
        case SgTypeLongDouble:       str <<  *m->readMemory<long double>(addr);         break;
        case SgTypeLongLong:         str <<  *m->readMemory<long long>(addr);           break;
        case SgTypeShort:            str <<  *m->readMemory<short>(addr);               break;
        case SgTypeSignedChar:       str <<  *m->readMemory<signed char>(addr);         break;
        case SgTypeSignedInt:        str <<  *m->readMemory<signed int>(addr);          break;
        case SgTypeSignedLong:       str <<  *m->readMemory<signed long>(addr);         break;
        case SgTypeSignedLongLong:   str <<  *m->readMemory<signed long long>(addr);    break;
        case SgTypeSignedShort:      str <<  *m->readMemory<signed short>(addr);        break;
        case SgTypeUnsignedChar:     str <<  *m->readMemory<unsigned char>(addr);       break;
        case SgTypeUnsignedInt:      str <<  *m->readMemory<unsigned int>(addr);        break;
        case SgTypeUnsignedLong:     str <<  *m->readMemory<unsigned long>(addr);       break;
        case SgTypeUnsignedLongLong: str <<  *m->readMemory<unsigned long long>(addr);  break;
        case SgTypeUnsignedShort:    str <<  *m->readMemory<unsigned short>(addr);      break;
        case SgTypeString:           str <<  *m->readMemory<char*>(addr);               break;
        default:                     cerr << "RsBasicType::readValueAt with unknown type";
    }

    return str.str();
}




void RsBasicType::setTypeInfo(const string & name, size_t size)
{
    stringId = name;
    byteSize = size;
}

void RsBasicType::setTypeInfo(SgType type_, size_t size_)
{
    type = type_;
    byteSize = size_;
}

RsBasicType::SgType RsBasicType::getBaseType(int i)
{
    assert(i>=0 && i < RsBasicType::Unknown);
    return (SgType)i;
}




// ----------------------------------- RsPointerType --------------------------------------


RsPointerType::RsPointerType(RsType * baseType_)
    : RsBasicType(SgPointerType),
      baseType(baseType_)
{
}

string RsPointerType::getDisplayName() const
{
    return baseType->getDisplayName() + "*";
}





