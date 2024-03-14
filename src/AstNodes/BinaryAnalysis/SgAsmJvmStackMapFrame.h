#include <sageContainer.h>

/** Represents a frame in a stack map table.
 *
 */
class SgAsmJvmStackMapFrame: public SgAsmJvmNode {
private:
public:
    /** Property: List of verification_type_info stack variables. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmStackMapVerificationTypePtrList stack;

    /** Property: List of verification_type_info local variables. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmStackMapVerificationTypePtrList locals;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types (for documentation purposes only)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef DOCUMENTATION
    /** The following discriminated union is documented below and indicates which
     *    item of the union is in use.
     */
    union stack_map_frame {
        same_frame;
        same_locals_1_stack_item_frame;
        same_locals_1_stack_item_frame_extended;
        chop_frame;
        same_frame_extended;
        append_frame;
        full_frame;
    };

    struct same_frame {
        uint8_t frame_type; /* 0-63 */
    };
    struct same_locals_1_stack_item_frame {
        uint8_t frame_type; /* 64-127 */
        verification_type_info stack;
    };
    struct same_locals_1_stack_item_extended {
        uint8_t frame_type; /* 247 */
        uint16_t offset_delta;
        verification_type_info stack;
    };
    struct chop_frame {
        uint8_t frame_type; /* 248-250 */
        uint16_t offset_delta;
    };
    struct same_frame_extended {
        uint8_t frame_type; /* 251 */
        uint16_t offset_delta;
    };
    struct append_frame {
        uint8_t frame_type; /* 252-254 */
        uint16_t offset_delta;
        verification_type_info stack;
    };
    struct full_frame {
        uint8_t frame_type; /* 252-254 */
        uint16_t offset_delta;
        uint16_t number_of_locals;
        verification_type_info stack[number_of_locals];
        uint16_t number_of_stack_items;
        verification_type_info stack[number_of_stack_items];
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    /** Property: frame_type
     *
     *  See the JVM specification. */
    [[using Rosebud: rosetta]]
    uint8_t frame_type = 0;

    /** Property: offset_delta
     *
     *  See the JVM specification. */
    [[using Rosebud: rosetta]]
    uint16_t offset_delta = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /**
     * Initialize the object before parsing. This is the preferred constructor
     * as it sets the parent. */
    explicit SgAsmJvmStackMapFrame(SgAsmJvmStackMapTable* table);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmStackMapFrame* parse(SgAsmJvmConstantPool* pool);

    /** Write the stack map table frame to a binary file */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
