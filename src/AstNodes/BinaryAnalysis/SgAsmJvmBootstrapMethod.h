#include <sageContainer.h>

/** JVM BootstrapMethod.
 *
 *  Each bootstrap_method entry contains an index to a CONSTANT_MethodHandle_info structure which specifies
 *  a bootstrap method, and a sequence (perhaps empty) of indexes to static arguments for the bootstrap method.
 */
class SgAsmJvmBootstrapMethod: public SgAsmJvmNode {
public:
    /** Property: bootstrap_method_ref.
     *
     *  The value of the bootstrap_method_ref item must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be a CONSTANT_MethodHandle_info structure (Section 4.4.8).
     */
    [[using Rosebud: rosetta]]
    uint16_t bootstrap_method_ref = 0;

    /** Property: bootstrap_arguments.
     *
     *  Each entry in the bootstrap_arguments list must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be loadable (Section 4.4).
     */
    [[using Rosebud: rosetta]]
    SgUnsigned16List bootstrap_arguments;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmBootstrapMethod(SgAsmJvmBootstrapMethods*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmBootstrapMethod* parse(SgAsmJvmConstantPool*);

    /** Write object to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
