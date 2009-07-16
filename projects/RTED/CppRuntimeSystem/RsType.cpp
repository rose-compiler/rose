#include "RsType.h"

#include <cassert>
#include <sstream>

using namespace std;



RsType *  RsType::getSubtypeRecursive(addr_type offset,  size_t size, bool stopAtArray)
{
    RsType * result = this;

    //cout << "Refining " << getName() << " at offset " << offset <<  " search of size " << size << endl;

    while(result->getByteSize() > size)
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
            return NULL;

        offset -= result->getSubtypeOffset(subTypeId);
        result  = result->getSubtype(subTypeId);

        //cout << "Refined to type " << result->getName() << " Offset:" << offset << endl;

    }

    if (result->getByteSize() != size || offset != 0)
        return NULL;

    //cout << "Refinement successful " << result->getName() << " Offset" << offset<< endl;

    return result;
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


RsArrayType::RsArrayType(RsType * baseType_, size_t elementCount_)
    : RsType(getArrayTypeName(baseType_,elementCount_)),
      baseType(baseType_),
      elementCount(elementCount_)
{
    assert(elementCount>0);
    assert(baseType != NULL);
}

size_t RsArrayType::getByteSize() const
{
    return elementCount * baseType->getByteSize();
}

int RsArrayType::getSubtypeCount() const
{
    return elementCount;
}

RsType * RsArrayType::getSubtype(int i)
{
    if(i>=0 && i<elementCount)
        return baseType;
    else
        return NULL;
}

int RsArrayType::getSubtypeOffset(int id) const
{
    if(id >= 0 && id < elementCount)
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

RsType * RsArrayType::getSubtypeAt ( addr_type offset)
{
    if( ! isValidOffset(offset))
        return NULL;
    else
        return baseType;
}



bool  RsArrayType::isValidOffset(addr_type offset) const
{
    if(offset >= getByteSize())
        return false;

    int inTypeOffset = offset % baseType->getByteSize();

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



// ---------------------------------------- RsClassType ---------------------------------------


RsClassType::RsClassType(const string & name, size_t byteSize_)
    : RsType(name),
      byteSize(byteSize_)
{
    assert(byteSize > 0);
}


int RsClassType::addMember(const std::string & name, RsType * type, addr_type offset)
{
    if(type==NULL)
    {
        cerr << "Tried to register MemberPointer with NULL type" << endl;
        return -1;
    }

    if(members.size() >0)
    {
        Member & last = members.back();
        assert(last.offset + last.type->getByteSize() <= offset);
    }

    members.push_back(Member(name,type,offset));

    assert(members.back().offset + members.back().type->getByteSize() <= byteSize);

    return members.size()-1;
}

bool RsClassType::isComplete(bool verbose) const
{
    if(verbose)
    {
        cout << "Padding info for type " << stringId << endl;
        for(int i=1; i<members.size(); i++)
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


RsType * RsClassType::getSubtype(int i)
{
    assert(i>=0 && i<members.size());
    return members[i].type;
}

int RsClassType::getSubtypeOffset(int id) const
{
    assert(id>=0 && id<members.size());
    return members[id].offset;
}

int RsClassType::getSubtypeIdAt(addr_type offset) const
{
    if( offset >= getByteSize())
        return -1;

    for(int i=members.size()-1 ;  i >= 0; i--)
    {
        if(offset >= members[i].offset)
        {
            // TODO register privates - this check fails if not all members are registered
            // and currently privates are not registered -> so this check fails when trying to access privates
            if (! members[i].type->isValidOffset(offset - members[i].offset) )
                return -1;
            else
                return i;
        }
    }
    assert(false); // this case should be caught by offset>= getByteSize()
    return -1;
}

RsType * RsClassType::getSubtypeAt(addr_type offset)
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


void RsClassType::print(ostream & os) const
{
    os << "Class " << getName() << " Size: " << byteSize <<endl;

    for(int i=0; i < members.size(); i++)
    {
        os << "  " << members[i].offset << " " <<
                      members[i].name << "\t" <<
                      members[i].type->getName() << endl;
    }
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
    : RsType(""),type(t)
{
    resolveTypeInfo(type);
}



void RsBasicType::print(ostream & os) const
{
    os << "Basic Type: " << getName() << endl;
}


void RsBasicType::resolveTypeInfo(const std::string & typeStr)
{
    stringId = typeStr;

    if      (typeStr=="SgTypeBool")             setTypeInfo(SgTypeBool,             sizeof(bool));
    else if (typeStr=="SgTypeChar")             setTypeInfo(SgTypeChar,             sizeof(char));
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
    else if (typeStr=="SgPointerType")          setTypeInfo(SgPointerType,          sizeof(void*));
    else if (typeStr=="SgArrayType")            setTypeInfo(SgArrayType,            sizeof(void*));
    else                                        setTypeInfo(Unknown, 0);
}

void RsBasicType::resolveTypeInfo(RsBasicType::SgType type_)
{
    type = type;

    if      (type==SgTypeBool)             setTypeInfo("SgTypeBool",             sizeof(bool));
    else if (type==SgTypeChar)             setTypeInfo("SgTypeChar",             sizeof(char));
    else if (type==SgTypeDouble)           setTypeInfo("SgTypeDouble",           sizeof(double));
    else if (type==SgTypeFloat)            setTypeInfo("SgTypeFloat",            sizeof(float));
    else if (type==SgTypeInt)              setTypeInfo("SgTypeInt",              sizeof(int));
    else if (type==SgTypeLong)             setTypeInfo("SgTypeLong",             sizeof(long));
    else if (type==SgTypeLongDouble)       setTypeInfo("SgTypeLongDouble",       sizeof(long double));
    else if (type==SgTypeLongLong)         setTypeInfo("SgTypeLongLong",         sizeof(long long));
    else if (type==SgTypeShort)            setTypeInfo("SgTypeShort",            sizeof(short));
    else if (type==SgTypeSignedChar)       setTypeInfo("SgTypeSignedChar",       sizeof(signed char));
    else if (type==SgTypeSignedInt)        setTypeInfo("SgTypeSignedInt",        sizeof(signed int));
    else if (type==SgTypeSignedLong)       setTypeInfo("SgTypeSignedLong",       sizeof(signed long));
    else if (type==SgTypeSignedLongLong)   setTypeInfo("SgTypeSignedLongLong",   sizeof(signed long long));
    else if (type==SgTypeSignedShort)      setTypeInfo("SgTypeSignedShort",      sizeof(signed short));
    else if (type==SgTypeUnsignedChar)     setTypeInfo("SgTypeUnsignedChar",     sizeof(unsigned char));
    else if (type==SgTypeUnsignedInt)      setTypeInfo("SgTypeUnsignedInt",      sizeof(unsigned int));
    else if (type==SgTypeUnsignedLong)     setTypeInfo("SgTypeUnsignedLong",     sizeof(unsigned long));
    else if (type==SgTypeUnsignedLongLong) setTypeInfo("SgTypeUnsignedLongLong", sizeof(unsigned long long));
    else if (type==SgTypeUnsignedShort)    setTypeInfo("SgTypeUnsignedShort",    sizeof(unsigned short));
    else if (type==SgTypeString)           setTypeInfo("SgTypeString",           sizeof(char*));
    else if (type==SgPointerType)          setTypeInfo("SgPointerType",          sizeof(void*));
    else if (type==SgArrayType)            setTypeInfo("SgArrayType",            sizeof(void*));
    else                                   setTypeInfo("Unknown", 0);

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







