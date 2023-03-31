/** Base class for binary types. */
class SgAsmType: public SgAsmNode {
private:
    static Sawyer::Container::Map<std::string, SgAsmType*> p_typeRegistry;

public:
    /** Validate properties collectively.
     *
     *  This method collectively validates the properties of a type since validation is not always possible or convenient
     *  when the user modifies an individual property.  For instance, when changing the bit fields in a floating-point
     *  type, it would be very inconvenient to the user if those properties individually validated there settings because
     *  it would require the user to shuffle fields around in such a way that they never overlap. This way, the overlap
     *  validation can occur after the user is all done moving the fields. */
    virtual void check() const;

    /** Convert a type to a string.
     *
     *  The output is intended mostly for debugging since it contains more details than what would be typically useful. For
     *  instance, instead of the word "double", this will probably print all the details about where the various
     *  floating-point fields are located, how the exponent field works, etc. */
    virtual std::string toString() const {
        abort();                                    // ROSETTA limitation: intended pure virtual
        return NULL;                                // Windows limitation: return value required [Too, 2014-08-11]
    }

    /** Width of type in bits. */
    virtual size_t get_nBits() const {
        abort();                                    // ROSETTA limitation: intended pure virtual
        return (size_t)-1;                          // Windows limitation: return value required [Too, 2014-08-11]
    }

    /** Width of type in bytes. */
    virtual size_t get_nBytes() const;

    /** Registers a type with the type system.
     *
     *  This method registers the specified type by its @ref toString value so it can be found later.  If a type by the
     *  same name is already registered then the specified one is deleted.  The return value is the type that is ultimately
     *  in the registry (either one that existed their previously or the specified type). */
    template<class Type>                                    // Type is a subclass of SgAsmType
    static Type* registerOrDelete(Type *toInsert) {
        ASSERT_not_null(toInsert);
        std::string key = toInsert->toString();
        Type *retval = dynamic_cast<Type*>(p_typeRegistry.insertMaybe(key, toInsert));
        ASSERT_not_null(retval);
        if (retval!=toInsert)
            delete toInsert;
        return retval;
    }
};
