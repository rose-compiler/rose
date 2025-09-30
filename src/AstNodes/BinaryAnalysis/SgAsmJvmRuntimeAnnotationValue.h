/** JVM RuntimeAnnotationValue.
 *
 *  Each value of the element_value_pairs table represents a single element-value pair in the annotation
 *  represented by this annotation structure. See section 4.7.16.1 of the JVM specification. */

/**
 *  From 4.7.16.1, for documentation purposes only, not directly used.
 *
 * element_value {
 *   u1 tag;
 *   union {
 *       u2 const_value_index;
 *
 *       {   u2 type_name_index;
 *           u2 const_name_index;
 *       } enum_const_value;
 *
 *       u2 class_info_index;
 *
 *       annotation annotation_value;
 *
 *       {   u2            num_values;
 *           element_value values[num_values];
 *       } array_value;
 *   } value;
 * }
 */
class SgAsmJvmRuntimeAnnotationValue: public SgAsmJvmNode {
public:
    /** Property: tag
     *
     *  The tag item uses a single ASCII character to indicate the type of the value of the
     *   element-value pair. */
    [[using Rosebud: rosetta]]
    uint8_t tag = 0;

    /** Property: const_value_index
     *
     *  The const_value_index item denotes a constant of either a primitive type or the type String
     *  as the value of this element-value pair.
     *
     *  This property is active for tags: 'B', 'C', 'D', 'F', 'I', 'J', 'S', 'Z', 's' */
    [[using Rosebud: rosetta]]
    uint16_t const_value_index = 0;

    /** Property: type_name_index
     *
     *  The value of the type_name_index item must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be a CONSTANT_Utf8_info structure (section 4.4.7)
     *  representing a field descriptor (section 4.3.2).
     *  This property is active for tag 'e', an enum_const_value */
    [[using Rosebud: rosetta]]
    uint16_t type_name_index = 0;

    /** Property: const_name_index
     *
     *  The value of the const_name_index item must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be a CONSTANT_Utf8_info structure (section 4.4.7).
     *  This property is active for tag 'e', an enum_const_value */
    [[using Rosebud: rosetta]]
    uint16_t const_name_index = 0;

     /** Property: class_info_index
     *
     *  The class_info_index item must be a valid index into the constant_pool table. The constant_pool
     *  entry at that index must be a CONSTANT_Utf8_info structure (section 4.4.7) representing a
     *  return descriptor (section 4.3.3).
     *  This property is active for tag 'c' */
    [[using Rosebud: rosetta]]
    uint16_t class_info_index = 0;

     /** Property: is_annotation_value
     *
     *  An annotation_value item denotes a "nested" annotation as the value of this element-value pair.
     *
     *  The is_annotation_value property is true for tag '@' */
    [[using Rosebud: rosetta]]
    bool is_annotation_value = false;

     /** Property: annotation_value
     *
     *  The annotation_value item denotes a "nested" annotation as the value of this element-value pair.
     *
     *  The value of the annotation_value item is an annotation structure (section 4.7.16) that gives the
     *  annotation represented by this element_value structure.
     *  This property is active for tag '@' */
    [[using Rosebud: rosetta]]
    SgAsmJvmRuntimeAnnotation* annotation_value = nullptr;

    /** Property: values
     *
     *  The values property is represented by a list of pointers to annotation array entries.
     *  The annotation structure is specified in section 4.7.16.  This property is active for tags: '[' */
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmJvmRuntimeAnnotationValue*> values;

 public:
    /** Initialize the object before parsing.
     *
     *  One of the preferred constructors to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeAnnotationValue(SgAsmJvmRuntimeAnnotationValue*);

    /** Initialize the object before parsing.
     *
     *  One of the preferred constructors to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeAnnotationValue(SgAsmJvmRuntimeAnnotationPair*);

    /** Initialize the object before parsing.
     *
     *  One of the preferred constructors to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeAnnotationValue(SgAsmJvmAnnotationDefault*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmRuntimeAnnotationValue* parse(SgAsmJvmConstantPool*);

    /** Write line number entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
