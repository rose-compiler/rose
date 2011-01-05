// vim:sw=4 ts=4:
#ifndef RSTYPES_H
#define RSTYPES_H

#include "Util.h"
#include <string>
#include <iostream>
#include <cassert>

#include "ptrops.h"


/**
 * Abstract Top-Class for all types, which are known to the Runtimesystem
 * A type may consist of subtypes: a subtype is basically just a member when the current type is a class
 * the subtypes of an array is the type it consists of
 */
class RsType
{
    public:

        explicit
        RsType(const std::string& name)
        : stringId(name)
        {}

        virtual ~RsType() {}

        /// Returns the size in byte for this type
        virtual size_t       getByteSize() const = 0;

        /// Returns the number of subtypes (i.e. number of members, or array size)
        virtual int          getSubtypeCount() const = 0;

        /// Returns the number of subtypes overlapping the given range that are
        /// not RsType::UnknownType
        virtual int          getKnownSubtypesOverlappingRange(
                                size_t range_start, size_t range_end ) const = 0;

        /// Returns the info class for the i'th subtype
        virtual RsType *     getSubtype(int i) const = 0;

        /// Returns the offset in bytes where a subtype is located
        /// the subtypes are ordered by their offset, i.e. larger id means larger offset
        /// offset(id) + getSubtype(id)->size() + padding = offset(id+1)
        virtual int          getSubtypeOffset(int id) const = 0;

        /// Returns the subtype-id which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual int          getSubtypeIdAt(size_t offset) const = 0;

        /// Returns the subtype which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual RsType *     getSubtypeAt(size_t offset) const = 0;

        /// Returns the subtype at an offset, which is of specified size
        /// recursively resolves subtypes
        /// nav is a string output parameter filled with: "typename.member1.submember" etc.
        /// return NULL if no such subtype exists
        virtual RsType *     getSubtypeRecursive(size_t offset,
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
        bool checkSubtypeRecursive( size_t offset, RsType* type);

        /// Checks if a given offset is valid (not too big, and not in a padding region)
        virtual bool  isValidOffset(size_t offset) const =0;

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

#if OBSOLETE_CODE
        /// Refines a subtype (i.e. member of classes)
        /// the template parameter specifies at which RsType should be stopped
        /// i.e. of which type   the refinedType is (mostly used SgBasicType an SgArrayType)
        /// @param courseType    the type which should be refined, for example class with many members
        /// @param offset        the offset where to refine
        /// @param refinedType   output parameter, the refined type, or NULL if invalid (if offset lies in padding)
        /// @param refinedOffset the offset relative to new type
        template<class T>
        static
        std::pair<RsType*, size_t> RsType::getTypeAt(RsType * coarseType,  size_t offset);
#endif /* OBSOLETE_CODE */

        static RsType& UnknownType;

    protected:
        virtual bool checkConsistencyWith( const RsType &other ) const { return &other == this; }
        std::string stringId;
};


#if OBSOLETE_CODE

template<class T>
std::pair<RsType*, size_t> RsType::getTypeAt(RsType * coarseType,  size_t offset)
{
    // make sure that offset lies in range at beginning
    assert(offset < coarseType->getByteSize() );

    std::pair<RsType*, size_t> res(coarseType, offset);

    while(true)
    {
        // Stop criterions:
        if( dynamic_cast<T*>(res.first) != NULL) return res; // type matches
        if( res.first->getSubtypeCount() ==0 )   return res; // no refinement possible

        // Refine
        int subTypeId = res.first->getSubtypeIdAt(res.second);

        if(subTypeId < 0) // error - offset at padded area
        {
            res.first = NULL;
            return res;
        }

        // make sure that offset stays positive
        assert( offset >= res.first->getSubtypeOffset(subTypeId));

        // \note pp/ should that not be res.second instead of offset?
        offset -= res.first->getSubtypeOffset(subTypeId);
        res.first = res.first->getSubtype(subTypeId);
    }
}

#endif /* OBSOLETE_CODE */




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
                                size_t range_start, size_t range_end ) const;

        /// Returns the info class for the an element (equal for all i's)
        virtual RsType *     getSubtype(int i) const;

        /// Checks if id<membercount, then returns the baseType
        virtual int          getSubtypeOffset(int id) const ;

        /// returns the element id at specific offset
        virtual int          getSubtypeIdAt(size_t offset) const;

        /// checks for valid offset then returns basetype
        virtual RsType *     getSubtypeAt  (size_t offset) const;

        /// Checks if a given offset is valid (not too big, and not in a padding region)
        virtual bool  isValidOffset(size_t offset) const;

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
        int      arrayIndex(size_t offset) const;

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
        RsClassType(const std::string & name, size_t byteSize, bool isUnionType);

        /// Adds a member, and checks if typeinfo is consistent (i.e. does not exceed byteSize)
        /// @param name name of the member variable
        /// @param type type of the member variable
        /// @param offset offset from the start-address of the class, if no offset is given, it is appended on the back
        /// @return id of added member
        // was: pp/ virtual int addMember(const std::string & name, RsType * type, size_t offset=-1);
        virtual int addMember(const std::string& name, RsType * type, size_t offset);

        /// adds a new member at the back of the type
        /// \note is this only used for testing?
        int addMember(const std::string& name, RsType * type)
        {
          size_t offset=0;

          if(members.size() > 0)
            offset = members.back().offset + members.back().type->getByteSize();

          return addMember(name, type, offset);
        }

        // ------------------ Type Interface -------------------------------

        /// Bytesize of this class
        virtual size_t       getByteSize() const { return byteSize; }

        // FIXME 3: should check that the size is legal, i.e. that its members
        // fit
        void                 setByteSize( size_t sz ) { byteSize = sz; }

        // FIXME 3: should check that class is legal after doing this
        void                 setUnionType( bool is_union );

        /// Number of member variables
        virtual int          getSubtypeCount() const;

        /// Returns the number of subtypes overlapping the given range that are
        /// not RsType::UnknownType
        virtual int          getKnownSubtypesOverlappingRange(size_t range_start, size_t range_end) const;

        /// Returns the info class for the i'th member
        virtual RsType *     getSubtype(int i) const;

        /// Returns the offset in bytes where a member is located
        /// the members are ordered by their offset, i.e. larger id means larger offset
        /// offset(id) + getSubtype(id)->size() + padding = offset(id+1)
        virtual int          getSubtypeOffset(int id) const;

        /// Returns the member-id which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual int  getSubtypeIdAt(size_t offset) const;
        std::vector<int>  getSubtypeUnionIdAt(size_t offset) const;

        /// Returns the member which is located at a specific offset
        /// or -1 if the offset lies in a padding area, or is too big
        virtual RsType *     getSubtypeAt  (size_t offset) const;

        /// Checks if a given offset is valid (not too big, and not in a padding region)
        virtual bool         isValidOffset(size_t offset) const;

        /// Returns member-name of id'th member
        virtual std::string getSubTypeString(int id) const;


        /// Checks if all members have been registered (all member-sizes add up to byteSize)
        /// @param verbose if true all padding areas are written to stdout
        virtual bool         isComplete(bool verbose=false) const;

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const;

        // Check whether memory that looks like this type could be @e other
        virtual bool checkConsistencyWith( const RsType &other ) const;

		bool getIsUnionType(){return isunionType;}

    protected:
        bool relaxed;
        size_t byteSize;
        bool isunionType;


        struct Member
        {
                Member(const std::string& name_, RsType* type_, size_t offset_)
                : name(name_), type(type_), offset(offset_)
                {}

                std::string name;
                RsType*     type;
                size_t      offset;
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
        virtual int          getKnownSubtypesOverlappingRange(size_t range_start, size_t range_end) const
                                                                  { return refType->getKnownSubtypesOverlappingRange( range_start, range_end );}
        virtual RsType *     getSubtype(int i) const              { return refType->getSubtype(i);     }
        virtual int          getSubtypeOffset(int id) const       { return refType->getSubtypeOffset(id); }
        virtual int          getSubtypeIdAt(size_t offset)        { return refType->getSubtypeIdAt(offset);}
        virtual RsType *     getSubtypeAt  (size_t offset) const  { return refType->getSubtypeAt(offset);}
        virtual bool         isValidOffset(size_t offset) const   { return refType->isValidOffset(offset);}
        std::string          getSubTypeString(int id) const       { return refType->getSubTypeString(id); }

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const;

    protected:
        RsType * refType;
};




class RsBasicType : public RsType
{
    public:

     // DQ (10/7/2009): The use of this specific name for enum values could be a problem for ROSE
     // since they are typename isn ROSE and enum names in this section of code.

        /// Enumeration of all basic types
        enum SgType
        {
            SgTypeBool,
            SgTypeChar,
            SgTypeWchar,
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
            //SgReferenceType,
            Unknown //Unknown always has to be last entry
        };


        RsBasicType(const std::string & typeStr);
        RsBasicType(SgType type);

        virtual ~RsBasicType() {}

        SgType               getSgType()       const               { return type;     }

        virtual size_t       getByteSize()     const               { return byteSize; }
        virtual int          getSubtypeCount() const               { return 0;        }
        virtual int          getKnownSubtypesOverlappingRange(size_t, size_t) const
                                                                   { return 0; }

        virtual RsType *     getSubtype(int) const                 { return NULL;     }
        virtual int          getSubtypeOffset(int) const           { return -1;       }
        virtual int          getSubtypeIdAt(size_t) const          { return -1;       }
        virtual RsType *     getSubtypeAt  (size_t) const          { return NULL;     }
        virtual bool         isValidOffset(size_t offset) const    { return offset < byteSize;}
        std::string          getSubTypeString(int) const           { return ""; }

        virtual std::string getDisplayName() const;


        /// Interprets memory at specified address as this basic type
        /// and writes value into string for displaying
        std::string readValueAt(Address address) const;

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
/// (conceptually of type RsType::UnknownType).  It is used when the type of
/// memory is partially known (for instance, if the user has been writing to
/// member variables in an array of structs on the heap).
class RsCompoundType : public RsClassType {

    public:
        RsCompoundType( size_t byteSize )
            : RsClassType( "CompoundType", byteSize, false ) { this -> relaxed = true; }

        RsCompoundType(const std::string & name, size_t byteSize)
            : RsClassType( name, byteSize, false ) { this -> relaxed = true; }

        // was: pp/ int addMember(const std::string & name, RsType * type, addr_type offset=-1);
        int addMember(const std::string& name, RsType * type, size_t offset);

        virtual RsType*  getSubtypeAt( size_t offset ) const;
};








#include <cassert>
/// Class with has a valid name
/// mostly used for lookup as comparison object
class InvalidType : public RsType
{
    public:
        InvalidType(const std::string& typeStr) : RsType(typeStr) {}

        virtual size_t       getByteSize()     const               { assert(false); return 0;     }
        virtual int          getSubtypeCount() const               { assert(false); return 0;     }
        virtual int          getKnownSubtypesOverlappingRange(size_t, size_t) const
                                                                   { assert( false ); return 0; }
        virtual RsType *     getSubtype(int) const                 { assert(false); return NULL;  }
        virtual int          getSubtypeOffset(int) const           { assert(false); return -1;    }
        virtual int          getSubtypeIdAt(size_t)const           { assert(false); return -1;    }
        virtual RsType *     getSubtypeAt  (size_t) const          { assert(false); return NULL;  }
        virtual bool         isValidOffset(size_t) const           { assert(false); return false; }
        std::string          getSubTypeString(int) const           { assert(false); return ""; }

        /// Print type information to a stream
        virtual void  print(std::ostream & os) const               { os << "Invalid Type" << std::endl; }
};


#endif
