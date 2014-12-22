#ifndef ROSE_BINARY_CALLCONV_H
#define ROSE_BINARY_CALLCONV_H

#include <cassert>
#include <vector>

class BinaryCallingConvention {


    /**************************************************************************************************************************
     *                                  Calling convention
     **************************************************************************************************************************/
public:

    /** Describes a particular calling convention. */
    class Convention {
    public:
        /** Constructor. All calling conventions must have a non-zero word size and non-empty name. The name need not be
         * unique, but it is often helpful if it is. */
        Convention(size_t wordsize, const std::string &name)
            : wordsize(wordsize), name(name),
              param_order(ORDER_UNKNOWN), this_location(THIS_UNKNOWN), retval_location(RETVAL_UNKNOWN), stack_alignment(0) {
            assert(wordsize>0);
            assert(!name.empty());
        }

        /** Word size for the architecture for which this calling convention applies.  The size is measured in bits.  When
         *  search a calling convention dictionary, only conventions matching the specified word size are considered.
         * @{ */
    public:
        size_t get_wordsize() const { return wordsize; }
        void set_wordsize(size_t wordsize) { this->wordsize = wordsize; }
        /** @} */
    private:
        size_t wordsize;

        /** Convention name.  Certain calling conventions have well-defined names, such as "cdecl" and can be looked up by
         *  their name in the calling convention dictionary.  Other ad hoc calling conventions have arbitrary names, or even no
         *  name at all.  The names need not be unique and are mainly for human consumtion.
         * @{ */
    public:
        const std::string& get_name() const { return name; }
        void set_name(const std::string &name) { this->name = name; }
        /** @}*/
    private:
        std::string name;

        /** Comment.  The comment field is for human consumption and lists things that might be of interest, such as which
         *  compilers use this convention.  The comment is optional.
         * @{ */
    public:
        const std::string &get_comment() const { return comment; }
        void set_comment(const std::string &comment) { this->comment = comment; }
        /** @} */
    private:
        std::string comment;

        /** Registers used for initial arguments.  This vector lists the registers (if any) that are used for initial
         *  parameters. The length of the vector doesn't correspond to the number of parameters actually passed.  In other
         *  words, a particular function call might pass only two parameters even though this vector has four entries.  If a
         *  function call has more arguments than the number of registers available to the calling convention, the additional
         *  arguments are passed on the stack.
         * @{ */
    public:
        const std::vector<const RegisterDescriptor*>& get_reg_params() const { return reg_params; }
        void add_reg_param(const RegisterDescriptor *reg);
        /** @} */
    private:
        std::vector<const RegisterDescriptor*> reg_params;

        /** The order that arguments are pushed onto the stack.  If a particular call has more arguments than the size of the
         *  reg_params vector then the extra (last) arguments are pushed onto the stack either in left-to-right or
         *  right-to-left lexical order.
         * @{ */
    public:
        enum ParameterOrder {
            ORDER_LTR,                      /**< Parameters pushed in left to right (Pascal) order. */
            ORDER_RTL,                      /**< Parameters pushed in right to left (C) order. */
            ORDER_UNKNOWN,                  /**< Parameter order is not known. */
        };
        ParameterOrder get_param_order() const { return param_order; }
        void set_param_order(ParameterOrder order) { param_order = order; }
        /** @} */
    private:
        ParameterOrder param_order;

        /** Location of "this" argument.  For object oriented code, the "this" object of class methods needs to be passed to
         *  the function implementing the method.  Calling conventions vary in where they put the "this" pointer.
         * @{ */
    public:
        enum ThisPointerLocation {
            THIS_FIRST_PARAM,               /**< The "this" pointer is treated as an implicit first parameter. */
            THIS_REGISTER,                  /**< A dedicated register is used for the "this" pointer. */
            THIS_NOT_APPLICABLE,            /**< Calling convention is not valid for class methods. */
            THIS_UNKNOWN,                   /**< Location of "this" pointer is not known. */
        };
        ThisPointerLocation get_this_location() const { return this_location; }
        void set_this_location(ThisPointerLocation loc) { this_location = loc; }
        /** @} */
    private:
        ThisPointerLocation this_location;

        /** Dedicated register for "this" pointer when this_location is THIS_REGISTER. Returns the null pointer if the location
         *  is other than THIS_REGISTER or if the register is unknown, and sets the location to THIS_REGISTER when setting a
         *  register. When setting the register, a null pointer means that the "this" pointer is stored in a register but it is
         *  unknown which register that is.
         * @{ */
    public:
        const RegisterDescriptor* get_this_register() const { return THIS_REGISTER==this_location ? this_reg : NULL; }
        void set_this_register(const RegisterDescriptor *reg) {
            this_location = THIS_REGISTER;
            this_reg = reg;
        }
        /** @} */
    private:
        const RegisterDescriptor *this_reg;

        /** Location of return value.
         * @{ */
    public:
        enum ReturnValueLocation {
            RETVAL_STACK,                      /**< The return value is placed on the stack. (FIXME: more specificity) */
            RETVAL_REGISTER,                   /**< Return value is stored in a register specified by the ret_reg member. */
            RETVAL_NOT_APPLICABLE,             /**< Function does not return a value. */
            RETVAL_UNKNOWN,                    /**< Location of return value is not known. */
        };
        ReturnValueLocation get_retval_location() const { return retval_location; }
        void set_retval_location(ReturnValueLocation loc) { retval_location = loc; }
        /** @} */
    private:
        ReturnValueLocation retval_location;


        /** Dedicated register for return value when return_location is RETVAL_REGISTER.  Returns the null pointer if the
         *  lcoation is other than RETVAL_REGISTER or if the register is unknown, and sets the location to RETVAL_REGISTER when
         *  setting a register.  When setting the register, a null pointer means that the return value is stored in a register
         *  but it is unknown which register that is.
         * @{ */
    public:
        const RegisterDescriptor* get_retval_register() const { return RETVAL_REGISTER==retval_location ? retval_reg : NULL; }
        void set_retval_register(const RegisterDescriptor *reg) {
            retval_location = RETVAL_REGISTER;
            retval_reg = reg;
        }
        /** @} */
    private:
        const RegisterDescriptor *retval_reg;

        /** Specifies how the stack is cleaned up.
         * @{ */
    public:
        enum StackCleanup {
            CLEANUP_CALLER,                 /**< The caller cleans up the stack. */
            CLEANUP_CALLEE,                 /**< The called function cleans up the stack. */
            CLEANUP_NOT_APPLICABLE,         /**< No need to clean up the stack because the stack is not used. */
            CLEANUP_UNKNOWN,                /**< It is not known how the stack is cleaned up. */
        };
        StackCleanup get_stack_cleanup() const { return stack_cleanup; }
        void set_stack_cleanup(StackCleanup cleanup) { stack_cleanup = cleanup; }
        /** @} */
    private:
        StackCleanup stack_cleanup;

        /** Stack alignment.  Some functions adjust the stack pointer so local variables are aligned on a certain byte
         *  boundary.  The alignment is measured in bytes and is usually a power of two. A value of one indicates that no
         *  alignment is necessary; a value of zero indicates that the alignment is unknown.
         * @{ */
    public:
        size_t get_stack_alignment() const { return stack_alignment; }
        void set_stack_alignment(size_t alignment) { stack_alignment = alignment; }
        /** @} */
    private:
        size_t stack_alignment;
    };

    /**************************************************************************************************************************
     *                                  Dictionary of Calling Conventions
     **************************************************************************************************************************/
private:
    std::vector<const Convention*> cconvs;

public:
    /** Appends the specified calling convention to this dictionary.  No check is made for whether the calling convention
     * is already present in the dictionary--it is added regardless. */
    void append(const Convention *cconv) { cconvs.push_back(cconv); }

    /** Finds a calling convention.  The dictionary is scanned to find a calling convention having the specified name and
     *  wordsize.  If the specified wordsize is zero then wordsize is not considered when searching.  An optional @p start_at
     *  value can be specified, in which case the search starts at the specified index in the dictionary and returns the index
     *  at which the matching calling convention was found (or the dictionary size when no match was found).  Returns a pointer
     *  to a matching calling convention, or the null pointer if no match was found. */
    const Convention *find(size_t wordsize, const std::string &name, size_t *start_at=NULL);

    /** Deletes the specified calling convention from the dictionary.
     * @{ */
    void erase(const Convention *conv);
    void erase(size_t index);
    /** @}*/

    /** Clears the dictionary.  Removes all calling conventions from the dictionary without destroying them. */
    void clear() { cconvs.clear(); }

    /** Returns the size of the dictionary.  Returns the number of calling conventions in the dictionary. Duplicates are
     *  counted multiple times. */
    size_t size() { return cconvs.size(); }

    /**************************************************************************************************************************
     *                                  Analysis
     **************************************************************************************************************************/
public:
    BinaryCallingConvention::Convention *analyze_callee(SgAsmFunction*);

};
    
#endif
