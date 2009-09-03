// vim:sw=4 ts=4:
#ifndef RSTYPES_H
#define RSTYPES_H

#include "Util.h"
#include <string>
#include <iostream>
#include <cassert>


/**
 * Abstract Top-Class for all types, which are known to the Runtimesystem
 * A type may consist of subtypes: a subtype is basically just a member when the current type is a class
 * the subtypes of an array is the type it consists of
 */
class RsType
{
    public:
        RsType(const std::string & name) : stringId(name) {}
        virtual ~RsType() {}


        /// Returns the size in byte for this type
        virtual size_t       getByteSize() const = 0;

        /// Returns the number of subtypes (i.e. number of members, or array size)
        virtual int          getSubtypeCount() const = 0;

        /// Returns the number of subtypes overlapping the given range that are
        /// not RsType::UnknownType
        virtual int          getKnownSubtypesOverlappingRange(
                                addr_type range_start, addr_type range_end ) const = 0;

        /// Returns the info class for the i'th subtype
        virtual RsType *     getSubtype(int i) const = 0;

        /// Returns the offset in bytes where a subtype is located
        /// the subtypes are ordered by their offset, i.e. larger id means larger offset
        /// offset(id) + getSubtype(id)->size() + padding = offset(id+1)
        virtual int          getSubtypeOffset(int id) const = 0;

        /// Returns the subtype-id which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual int          getSubtypeIdAt(addr_type offset) const = 0;

        /// Returns the subtype which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual RsType *     getSubtypeAt  (addr_type offset) const = 0;

        /// Returns the subtype at an offset, which is of specified size
        /// recursively resolves subtypes
        /// nav is a string output parameter filled with: "typename.member1.submember" etc.
        /// return NULL if no such subtype exists
        virtual RsType *     getSubtypeRecursive(addr_type offset,
                                                 size_t size,
                                                 bool stopAtArray=false,
                                                 std::string * nav = NULL ) ;

        /**
         * Recursively checks to see if @c type has been written at @c offset,
         * Handles cases with subtypes of arbitrary depth.  For example, if @c
         * TypeA contains @c TypeB contains @c TypeC , and they are all of the
         * same size, than any one of those types would be considered “ok”.
         *
         * @return  @c true @b iff @c type has been written at @c offset .
         */
        bool checkSubtypeRecursive( addr_type offset, RsType* type);

        /// Checks if a given offset is valid (not too big, and not in a padding region)
        virtual bool  isValidOffset(addr_type offset) const =0;

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const = 0;

        /// Returns the type-name (class name, or for basic types Sg*)
        virtual const std::string& getName() const { return stringId; }
        /// Possibility to display another (more userfriendly) string than getName()
        virtual std::string getDisplayName() const { return stringId; }

        /// Less operator uses stringId
        virtual bool operator< (const RsType & other) const { return stringId < other.stringId; }

        /// For classes returns name of member, for arrays just "[subtypeId]"
        virtual std::string getSubTypeString(int id) const =0;

        /// Returns @c true @b iff this type and @c other could be the same.
        /// Note that for classes, this does not merely mean that they
        /// have the same subtypes at the same positions.  Different classes
        /// will generally be inconsistent, unless one is only partially known
        /// (i.e. an @c RsCompoundType).
        virtual bool isConsistentWith( const RsType &other ) const;

        /// Refines a subtype (i.e. member of classes)
        /// the template parameter specifies at which RsType should be stopped
        /// i.e. of which type   the refinedType is (mostly used SgBasicType an SgArrayType)
        /// @param courseType    the type which should be refined, for example class with many members
        /// @param offset        the offset where to refine
        /// @param refinedType   output parameter, the refined type, or NULL if invalid (if offset lies in padding)
        /// @param refinedOffset the offset relative to new type
        template<class T>
        static void getTypeAt(RsType * courseType,  addr_type offset,
                              RsType *& refinedType,addr_type & refinedOffset);

        static RsType& UnknownType;

    protected:
        virtual bool checkConsistencyWith( const RsType &other ) const { return &other == this; }
        std::string stringId;
};

template<class T>
void RsType::getTypeAt(RsType * courseType,  addr_type offset,
                       RsType *& refinedType,addr_type & refinedOffset)
{
    // make sure that offset lies in range at beginning
    assert(offset>=0 && offset < courseType->getByteSize() );

    refinedType   = courseType;
    refinedOffset = offset;

    while(true)
    {
        // Stop criterions:
        if( dynamic_cast<T*>(refinedType) != NULL) return; // type matches
        if( refinedType->getSubtypeCount() ==0 )   return; // no refinement possible

        // Refine
        int subTypeId = refinedType->getSubtypeIdAt(refinedOffset);
        if(subTypeId < 0) // error - offset at padded area
        {
            refinedType = NULL;
            return;
        }
        assert( offset >= refinedType->getSubtypeOffset(subTypeId)); //make sure that offset stays positive
        offset -= refinedType->getSubtypeOffset(subTypeId);
        refinedType = refinedType->getSubtype(subTypeId);
    }
}






class RsArrayType : public RsType
{
    public:
        RsArrayType(RsType * baseType, size_t size);
        virtual ~RsArrayType() {}


        /// Returns the size in byte for this type
        virtual size_t       getByteSize() const;

        /// Returns the number of elements
        virtual int          getSubtypeCount() const;

        /// Returns the number of subtypes overlapping the given range that are
        /// not RsType::UnknownType
        virtual int          getKnownSubtypesOverlappingRange(
                                addr_type range_start, addr_type range_end ) const;

        /// Returns the info class for the an element (equal for all i's)
        virtual RsType *     getSubtype(int i) const;

        /// Checks if id<membercount, then returns the baseType
        virtual int          getSubtypeOffset(int id) const ;

        /// returns the element id at specific offset
        virtual int          getSubtypeIdAt(addr_type offset) const;

        /// checks for valid offset then returns basetype
        virtual RsType *     getSubtypeAt  (addr_type offset) const;

        /// Checks if a given offset is valid (not too big, and not in a padding region)
        virtual bool  isValidOffset(addr_type offset) const;

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const;

        /// Returns "[id]"
        virtual std::string getSubTypeString(int id) const;


        /// Each type has a name, only arrays don't have one
        /// therefore a pseudo-name is generated __array_baseTypeName_size;
        /// this is done by this function
        static std::string getArrayTypeName(RsType * basetype, size_t size);


        virtual std::string getDisplayName() const;


        // Check whether memory that looks like this type could be @e other
        virtual bool checkConsistencyWith( const RsType &other ) const;

        RsType * getBaseType() const          { return baseType; }
        int      arrayIndex(addr_type offset) const;

    protected:
        RsType * baseType;
        size_t   elementCount;

};


#include <vector>

class RsClassType : public RsType
{
    public:
        /// Creates new ClassType whith specified name and size in bytes
        /// members-info is added via addMember()
        RsClassType(const std::string & name, size_t byteSize);

        /// Adds a member, and checks if typeinfo is consistent (i.e. does not exceed byteSize)
        /// @param name name of the member variable
        /// @param type type of the member variable
        /// @param offset offset from the start-address of the class, if not offset is given, it is appended on the back
        /// @return id of added member
        virtual int addMember(const std::string & name, RsType * type, addr_type offset=-1);


        // ------------------ Type Interface -------------------------------

        /// Bytesize of this class
        virtual size_t       getByteSize() const { return byteSize; }

        /// Number of member variables
        virtual int          getSubtypeCount() const;

        /// Returns the number of subtypes overlapping the given range that are
        /// not RsType::UnknownType
        virtual int          getKnownSubtypesOverlappingRange(
                                addr_type range_start, addr_type range_end ) const;

        /// Returns the info class for the i'th member
        virtual RsType *     getSubtype(int i) const;

        /// Returns the offset in bytes where a member is located
        /// the members are ordered by their offset, i.e. larger id means larger offset
        /// offset(id) + getSubtype(id)->size() + padding = offset(id+1)
        virtual int          getSubtypeOffset(int id) const;

        /// Returns the member-id which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual int          getSubtypeIdAt(addr_type offset) const;

        /// Returns the member which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual RsType *     getSubtypeAt  (addr_type offset) const;

        /// Checks if a given offset is valid (not too big, and not in a padding region)
        virtual bool         isValidOffset(addr_type offset) const;

        /// Returns member-name of id'th member
        virtual std::string getSubTypeString(int id) const;


        /// Checks if all members have been registered (all member-sizes add up to byteSize)
        /// @param verbose if true all padding areas are written to stdout
        virtual bool         isComplete(bool verbose=false) const;

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const;

        // Check whether memory that looks like this type could be @e other
        virtual bool checkConsistencyWith( const RsType &other ) const;

    protected:
        bool relaxed;
        size_t byteSize;

        struct Member
        {
                Member(const std::string & name_, RsType * type_, addr_type offset_):
                           name(name_), type(type_), offset(offset_)
                 {}

                std::string name;
                RsType * type;
                addr_type offset;
        };

        std::vector<Member>  members;
};
std::ostream& operator<< (std::ostream &os, const RsType * m);
std::ostream& operator<< (std::ostream &os, const RsType & m);



class RsTypeDef : public RsType
{
    public:
        /// Creates a typedef info "typedef unsigned int uint"
        /// then name would be "uint" and refType the type-information for unsigned int
        RsTypeDef(const std::string & name, RsType * refType);


        virtual size_t       getByteSize()     const              { return refType->getByteSize();     }
        virtual int          getSubtypeCount() const              { return refType->getSubtypeCount(); }
        virtual int          getKnownSubtypesOverlappingRange(
                                addr_type range_start, addr_type range_end ) const 
                                                                  { return refType->getKnownSubtypesOverlappingRange( range_start, range_end );}
        virtual RsType *     getSubtype(int i) const              { return refType->getSubtype(i);     }
        virtual int          getSubtypeOffset(int id) const       { return refType->getSubtypeOffset(id); }
        virtual int          getSubtypeIdAt(addr_type offset)     { return refType->getSubtypeIdAt(offset);}
        virtual RsType *     getSubtypeAt  (addr_type offset)const{ return refType->getSubtypeAt(offset);}
        virtual bool         isValidOffset(addr_type offset) const{ return refType->isValidOffset(offset);}
        std::string          getSubTypeString(int id) const       { return refType->getSubTypeString(id); }

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const;

    protected:
        RsType * refType;
};




class RsBasicType : public RsType
{
    public:
        /// Enumeration of all basic types
        enum SgType
        {
            SgTypeBool,
            SgTypeChar,
            // TODO 2: Remove typedef as a basic type, and either expand the
            // type or treat it as a user-defined type.
            //
            // The effect of treating typedef as a basic type is to collapse all
            // typedef-d types into one.
            SgTypedefType,
            SgTypeDouble,
            SgTypeFloat,
            SgTypeInt,
            SgTypeLong,
            SgTypeLongDouble,
            SgTypeLongLong,
            SgTypeShort,
            SgTypeSignedChar,
            SgTypeSignedInt,
            SgTypeSignedLong,
            SgTypeSignedLongLong,
            SgTypeSignedShort,
            SgTypeUnsignedChar,
            SgTypeUnsignedInt,
            SgTypeUnsignedLong,
            SgTypeUnsignedLongLong,
            SgTypeUnsignedShort,
            SgTypeString,
            SgTypeVoid,
            SgPointerType,
            Unknown //Unknown always has to be last entry
        };


        RsBasicType(const std::string & typeStr);
        RsBasicType(SgType type);

        virtual ~RsBasicType() {}

        SgType               getSgType()       const               { return type;     }

        virtual size_t       getByteSize()     const               { return byteSize; }
        virtual int          getSubtypeCount() const               { return 0;        }
        virtual int          getKnownSubtypesOverlappingRange(
                                addr_type range_start, addr_type range_end ) const 
                                                                   { return 0; }
        virtual RsType *     getSubtype(int i) const               { return NULL;     }
        virtual int          getSubtypeOffset(int id) const        { return -1;       }
        virtual int          getSubtypeIdAt(addr_type offset)const { return -1;       }
        virtual RsType *     getSubtypeAt  (addr_type offset)const { return NULL;     }
        virtual bool         isValidOffset(addr_type offset) const { return offset < byteSize;}
        std::string          getSubTypeString(int id) const        { return ""; }

        virtual std::string getDisplayName() const;


        /// Interprets memory at specified address as this basic type
        /// and writes value into string for displaying
        std::string readValueAt(addr_type address) const;

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const;

        static int    getBaseTypeCount()  { return Unknown;   }
        static SgType getBaseType(int i);

    protected:

        void resolveTypeInfo(SgType type_);
        void resolveTypeInfo(const std::string & typeStr);

        void setTypeInfo(SgType type_, size_t size_);
        void setTypeInfo(const std::string & typeStr, size_t size);

        size_t      byteSize;
        SgType      type;
};

/**
 * Info structure for pointers
 *     the type of the BasicType still stays SgPointerType ( so the size information is still correct)
 *     additionally the target/base-type is stored
 */
class RsPointerType : public RsBasicType
{
    public:
        RsPointerType(RsType * baseType);
        virtual ~RsPointerType() {}


        virtual std::string getDisplayName() const;

        RsType * getBaseType() const  { return baseType; }

    protected:
        /// Type the pointer points to
        /// pointer may point to other RsPointerType's
        /// to represent double/multiple pointers
        RsType * baseType;
};

/// An @c RsCompoundType is a loose collection of subtypes, with gaps
/// (conceptually of type RsType::UnknownType.  It is used when the type of
/// memory is partially known (for instance, if the user has been writing to
/// member variables in an array of structs on the heap).
class RsCompoundType : public RsClassType {

    public:
        RsCompoundType(const std::string & name, size_t byteSize)
            : RsClassType( name, byteSize ) { this -> relaxed = true; }
        int addMember(const std::string & name, RsType * type, addr_type offset=-1);

        virtual RsType*  getSubtypeAt( addr_type offset ) const;
};








#include <cassert>
/// Class with has a valid name
/// mostly used for lookup as comparison object
class InvalidType : public RsType
{
    public:
        InvalidType(const std::string & typeStr) : RsType(typeStr) {}

        virtual size_t       getByteSize()     const               { assert(false); return 0;     }
        virtual int          getSubtypeCount() const               { assert(false); return 0;     }
        virtual int          getKnownSubtypesOverlappingRange(
                                addr_type range_start, addr_type range_end ) const 
                                                                   { assert( false ); return 0; }
        virtual RsType *     getSubtype(int i) const               { assert(false); return NULL;  }
        virtual int          getSubtypeOffset(int id) const        { assert(false); return -1;    }
        virtual int          getSubtypeIdAt(addr_type offset)const { assert(false); return -1;    }
        virtual RsType *     getSubtypeAt  (addr_type offset) const{ assert(false); return NULL;  }
        virtual bool         isValidOffset(addr_type offset) const { assert(false); return false; }
        std::string          getSubTypeString(int id) const        { assert(false); return ""; }

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const               { os << "Invalid Type" << std::endl; }
};


#endif
