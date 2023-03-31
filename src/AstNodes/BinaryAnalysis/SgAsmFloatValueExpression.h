/** Floating-point value.
 *
 *  The bits are stored in the super-class (SgAsmConstantExpression) and interpretted as various kinds of floating-point
 *  values. */
class SgAsmFloatValueExpression: public SgAsmConstantExpression {
private:
    // This node stores its primary representation of the value in the p_bitVector of a parent class.  However, since we
    // often access the value as a native "double", and since converting to/from a bit vector is costly, we want to cache
    // the native double value whenever we compute it.  ROSETTA does not need to be aware of the cached value since it can
    // be recomputed from the bit vector, and in fact, ROSETTA can't handle cache data members because it doesn't
    // understand "mutable".
    //
    // However, Rosebud handles this just fine.
    [[using Rosebud: data(p_nativeValue), accessors(), mutators()]]
    mutable double nativeValue = 0.0;

    [[using Rosebud: data(p_nativeValueIsValid), accessors(), mutators()]]
    mutable bool nativeValueIsValid = true;

public:
    /** Construct specified floating-point value.
     *
     *  Creates a new floating-point constant AST node having the specified native value and type. See also, the
     *  constructor that takes a bit vector as its first argument, which is useful in cases where the native representation
     *  of a floating point value does not match the target machine's representation.
     *
     *  On the other hand, the default constructor creates a new floating-point value of unspecified type initialized to positive
     *  zero.  We recommend using a constructor that takes a @ref SgAsmType argument instead. */
    SgAsmFloatValueExpression(double nativeValue, SgAsmType*);

    /** Construct specified floating-point value.
     *
     *  Creates a new floating-point constant AST node having the specified value and type.  See also, the constructor that
     *  takes a @c double argument for those cases when the native representation matches the target machine's
     *  representation. */
    SgAsmFloatValueExpression(const Sawyer::Container::BitVector&, SgAsmType*);

    /** Set AST node value to specified native value. */
    void set_nativeValue(double);

    /** Get value in native format. */
    double get_nativeValue() const;

    /** Update bit vector from native representation.
     *
     *  Resets the bit vector using the cached native representation.  The bit vector is considered to be the canonical
     *  value of this AST node. */
    void updateBitVector();

    /** Update cached native value from bit vector.
     *
     *  Resets the cached native value from the bit vector. The bit vector is considered to be the canonical value of this
     *  AST node, and the native format value is a cached representation that can be used in arithmetic. */
    void updateNativeValue() const;
};
