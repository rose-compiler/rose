#ifndef Rose_MultiSemantics_H
#define Rose_MultiSemantics_H

#include "x86InstructionSemantics.h"
#include "BaseSemantics.h"
#include "NullSemantics.h"

namespace BinaryAnalysis {                      // documented elsewhere
    namespace InstructionSemantics {            // documented elsewhere

        /** Semantic domain composed of sub-domains.
         *
         * The MultiSemantics semantic domain is a pseudo domain composed of one or more sub-domains.  Each of the RISC-like
         * operations implemented in this policy invoke the operation in each of the sub-policies.  The type of values
         * manipulated by this policy are the union of values from each of the sub-policies. One could accomplish the same
         * thing by instantiating multiple instruction semantic objects and calling each one for each instruction.  However,
         * using this MultiSemantics policy is cleaner and easier to specialize.
         *
         * A multi-policy is created by listing all the sub-policy value types, state types, and policy types in the
         * MultiPolicy template class.  For instance, to declare a multi-policy that does both full-symbolic semantics
         * (SymbolicSemantics) and partial-symbolic semantics (PartialSymbolicSemantics) do this:
         *
         * @code
         *  #include "MultiSemantics.h"
         *  #include "SymbolicSemantics.h"
         *  #include "PartialSymbolicSemantics.h"
         *
         *  typedef MultiSemantics<
         *      SymbolicSemantics::ValueType,           // first data type
         *      SymbolicSemantics::State,               // first state
         *      SymbolicSemantics::Policy,              // first policy
         *      PartialSymbolicSemantics::ValueType,    // second data type
         *      PartialSymbolicSemantics::State,        // second state
         *      PartialSymbolicSemantics::Policy        // second policy
         *      // etc.
         *                         >::Policy Policy;
         * @endcode
         *
         * Note that MultiSemantics is a class while many of the other semantic domains are implemented as name spaces. We used
         * a class here so that template arguments could be specified for the MultiSemantics as a whole rather than specifying
         * them repeatedly for the Policy, State, and ValueType classes.  Those inner classes also take template arguments
         * similar to their counterparts in other semantic domains.
         *
         * The sub-policies are identified by the tag types SP0, SP1, etc.:
         *
         * @code
         *  SymbolicSemantics::Policy<>        &sp0 = get_subpolicy(SP0());
         *  PartialSymbolicSemantics::Policy<> &sp1 = get_subpolicy(SP1());
         * @endcode
         *
         * A multi-value has one sub-value for each sub-policy. Like the sub-policies themselves, the sub-values are identified
         * by the SP0, SP1, etc., tags:
         * 
         * @code
         *  ValueType<32> v = ....; // the multi-policy value, or "multi-value"
         *  SymbolicSemantics::ValueType<32>        v0 = v.get_subvalue(SP0());
         *  PartialSymbolicSemantics::ValueType<32> v1 = v.get_subvalue(SP1());
         * @endcode
         *
         * Individual sub-values of a multi-value can be marked as invalid.  A sub-value must still necessarily be stored at
         * that slot in the multi-value:
         *
         * @code
         *  ValueType<32> v = ....; // the multi-value
         *  if (v.is_valid(SP0())) {
         *      std::cout <<"sub-value 0 is " <<v.get_subvalue(SP0()) <<std::endl;
         *      v.set_valid(SP0(), false); // or v.clear_valid(SP0())
         *  }
         * @endcode
         *
         * A sub-value can be modified with the set_subvalue() method, which also (by default) makes that sub-value slot
         * valid.
         *
         * Similar to marking a sub-value as invalid, a sub-policy can be marked as inactive.  When the a RISC operation is
         * called in the multi-policy, the same operation will be invoked in each of the active sub-policies (provided the
         * operation's inputs are valid for that sub-policy).  If the operation returns a multi-value (as most do), then the
         * sub-values corresponding to called sub-policies will be marked valid and the other sub-values are marked invalid.
         *
         * Using a multi-policy directly is not all that interesting.  Where the real convenience comes is in specializing the
         * multi-policy to do things like convert values from one domain to another.  For example, consider two semantic
         * domains called Alpha and Beta implemented by policies, AlphaPolicy and BetaPolicy, and operating on values of type
         * AlphaValue and BetaValue. Assume that for some reason, and ADD operation in BetaPolicy is expensive and that a
         * BetaValue can be constructed from an AlphaValue.  Therefore, it is more efficient to skip the ADD operation in
         * BetaPolicy and instead compute the Beta sum from the Alpha sum, like this:
         *
         * @code
         *  typedef MultiPolicy<AlphaType, AlphaState, AlphaPolicy, BetaType, BetaState, BetaPolicy> BaseClass;
         *  class MyPolicy: public BaseClass {
         *  public:
         *      template<size_t nBits>
         *      ValueType<nBits> add(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
         *          ActiveMask old_active = get_active_policies(); // what sub-policies are active?
         *          try {
         *              disable_policy(SP1()); // temporarily disable BetaPolicy
         *              ValueType<nBits> retval = BaseClass::add<nBits>(a, b);
         *              assert(!retval.is_valid(SP1())); // the beta value is not valid, its "add" having not been called
         *              BetaValue betaValue = retval.get_subvalue(SP1()); // construct the beta value from alpha
         *              retval.set_subvalue(betaValue); // define the beta value and make it valid
         *              set_active_policies(old_active); // restore original value
         *          } catch(...) {
         *              set_active_policies(old_active); // restore on error
         *          }
         *          return retval;
         *      }
         *  };
         * @endcode
         *
         * The convenience of using a MultiPolicy increases with the number of sub-policies it contains.
         */
        template<
            // Sub-domain 0
            template <size_t nBits> class ValueType0,
            template <template <size_t> class ValueType> class State0,
            template <template <template <size_t> class ValueType> class State,
                      template <size_t> class ValueType> class Policy0,

            // Sub-domain 1
            template <size_t nBits> class ValueType1 = NullSemantics::ValueType,
            template <template <size_t> class ValueType> class State1 = NullSemantics::State,
            template <template <template <size_t> class ValueType> class State,
                      template <size_t> class ValueType> class Policy1=NullSemantics::Policy,

            // Sub-domain 2
            template <size_t nBits> class ValueType2 = NullSemantics::ValueType,
            template <template <size_t> class ValueType> class State2 = NullSemantics::State,
            template <template <template <size_t> class ValueType> class State,
                      template <size_t> class ValueType> class Policy2=NullSemantics::Policy,

            // Sub-domain 3
            template <size_t nBits> class ValueType3 = NullSemantics::ValueType,
            template <template <size_t> class ValueType> class State3 = NullSemantics::State,
            template <template <template <size_t> class ValueType> class State,
                      template <size_t> class ValueType> class Policy3=NullSemantics::Policy
            >
        class MultiSemantics {
        public:

            /** A tag to identify a certain sub-policy or sub-value.  Type type or value of type SP<i>n</i> corresponds to
             *  sub-policy <i>n</i>.  The numbering is zero-origin.  Even if a multi-policy declaration secifies only two
             *  sub-policies, all other sub-policy slots are initialized to the NullSemantics policy. */
            // "@{" grouping doesn't work here
            struct SP0 { enum { n=0, mask=0x01 }; };
            struct SP1 /**< See SP0. */ { enum { n=1, mask=0x02 }; };
            struct SP2 /**< See SP0. */ { enum { n=2, mask=0x04 }; };
            struct SP3 /**< See SP0. */ { enum { n=3, mask=0x08 }; };
            // @}

            /******************************************************************************************************************
             *                                  ValueType
             ******************************************************************************************************************/
        public:
            /** Type of each MultiSemantic value.  A multi-semantic value is just a set of values, one from each of the
             *  sub-policies, and a bit vector that indicates which values are valid.  The bit vector is accessed by the
             *  multi-policy RISC operators and new values are created by those operators with appropriate validities, but the
             *  multi-policy generally does not modify the validity setting for existing values--that's up to the user-defined,
             *  inter-operation callbacks. */
            template<size_t nBits>
            class ValueType {
            protected:
                unsigned valid_values ;/**< Bitmask to indicate which values are valid. */
                ValueType0<nBits> value0;
                ValueType1<nBits> value1;
                ValueType2<nBits> value2;
                ValueType3<nBits> value3;

            public:
                /** Sub-policy values are born invalid. */
                ValueType()
                    : valid_values(0) {}

                /** Initialize all values and indicate whether they are valid. */
                ValueType(bool valid0, const ValueType0<nBits> &v0,
                          bool valid1, const ValueType1<nBits> &v1,
                          bool valid2, const ValueType2<nBits> &v2,
                          bool valid3, const ValueType3<nBits> &v3)
                    : value0(v0), value1(v1), value2(v2), value3(v3) {
                    set_valid(SP0(), valid0);
                    set_valid(SP1(), valid1);
                    set_valid(SP2(), valid2);
                    set_valid(SP3(), valid3);
                }

                /** Returns true if a sub-value is valid.
                 * @{ */
                bool is_valid(const SP0 &sp) const { return 0 != (valid_values & sp.mask); }
                bool is_valid(const SP1 &sp) const { return 0 != (valid_values & sp.mask); }
                bool is_valid(const SP2 &sp) const { return 0 != (valid_values & sp.mask); }
                bool is_valid(const SP3 &sp) const { return 0 != (valid_values & sp.mask); }
                /** @}*/

                /** Set the validity of a sub-value.
                 * @{ */
                void set_valid(const SP0 &sp, bool valid=true) { set_valid(sp.mask, valid); }
                void set_valid(const SP1 &sp, bool valid=true) { set_valid(sp.mask, valid); }
                void set_valid(const SP2 &sp, bool valid=true) { set_valid(sp.mask, valid); }
                void set_valid(const SP3 &sp, bool valid=true) { set_valid(sp.mask, valid); }
                /** @}*/

                /** Clear the validity of a sub-value.
                 * @{ */
                void clear_valid(const SP0 &sp) { set_valid(sp.mask, false); }
                void clear_valid(const SP1 &sp) { set_valid(sp.mask, false); }
                void clear_valid(const SP2 &sp) { set_valid(sp.mask, false); }
                void clear_valid(const SP3 &sp) { set_valid(sp.mask, false); }
                /** @} */
                
                /** Return a sub-value. Sub-values are identified by sub-policy tags, SP0, SP1, etc.
                 * @{ */
                ValueType0<nBits>& get_subvalue(const SP0&) { return value0; }
                ValueType1<nBits>& get_subvalue(const SP1&) { return value1; }
                ValueType2<nBits>& get_subvalue(const SP2&) { return value2; }
                ValueType3<nBits>& get_subvalue(const SP3&) { return value3; }
                const ValueType0<nBits>& get_subvalue(const SP0&) const { return value0; }
                const ValueType1<nBits>& get_subvalue(const SP1&) const { return value1; }
                const ValueType2<nBits>& get_subvalue(const SP2&) const { return value2; }
                const ValueType3<nBits>& get_subvalue(const SP3&) const { return value3; }
                /** @} */

                /** Set a sub-policy value.  Setting a sub-policy value also makes it a valid sub-value (unless the @p valid
                 *  argument is false.
                 * @{ */
                void set_subvalue(const SP0 &sp, const ValueType0<nBits> &v, bool valid=true) {
                    value0 = v;
                    set_valid(sp, valid);
                }
                void set_subvalue(const SP1 &sp, const ValueType1<nBits> &v, bool valid=true) {
                    value1 = v;
                    set_valid(sp, valid);
                }
                void set_subvalue(const SP2 &sp, const ValueType2<nBits> &v, bool valid=true) {
                    value2 = v;
                    set_valid(sp, valid);
                }
                void set_subvalue(const SP3 &sp, const ValueType3<nBits> &v, bool valid=true) {
                    value3 = v;
                    set_valid(sp, valid);
                }
                /** @} */

                /** Print the value. Prints subvalues that are marked as valid. */
                void print(std::ostream &o) const {
                    std::string sep="";
                    if (is_valid(SP0())) {
                        o <<sep <<"SP0: " <<get_subvalue(SP0());
                        sep = "; ";
                    }
                    if (is_valid(SP1())) {
                        o <<sep <<"SP1: " <<get_subvalue(SP1());
                        sep = "; ";
                    }
                    if (is_valid(SP2())) {
                        o <<sep <<"SP2: " <<get_subvalue(SP2());
                        sep = "; ";
                    }
                    if (is_valid(SP3())) {
                        o <<sep <<"SP3: " <<get_subvalue(SP3());
                    }
                }
                friend std::ostream& operator<<(std::ostream &o, const ValueType<nBits> &e) {
                    e.print(o);
                    return o;
                }

            protected:
                // Internal function to help set sub-value validity.
                void set_valid(unsigned mask, bool valid) {
                    if (valid) {
                        valid_values |= mask;
                    } else {
                        valid_values &= mask;
                    }
                }
            };

            /******************************************************************************************************************
             *                                  State
             ******************************************************************************************************************/
        public:
            /** MultiSemanics global state.
             *
             *  The MultiSemantics does not define its own machine state, but rather stores the states in the individual
             *  sub-policies. However, this class can be replaced/subclassed with something that stores state across all the
             *  sub-policies. */
            template <template <size_t> class ValueType>
            class State {}; // only for documentation


            /******************************************************************************************************************
             *                                  Policy
             ******************************************************************************************************************/
        public:
            /** Implements the RISC operations.  Invoking a RISC operation in the multi-policy causes the same operation to be
             *  invoked in each of the sub-policies provided that the sub-policy is active and the input operands are valid.
             *  Each sub-policy maintains its own state.  The register dictionary operations are inherited from
             *  BaseSemantics::Policy, which means that they apply only in the base policy (except set_register_dictionary()
             *  also calls the sub-policies.   See NullSemantics::Policy for documentation of the RISC-like API. */
            template <
                template <template <size_t> class ValueType> class State, // unused
                template <size_t nBits> class ValueType
            >
            class Policy: public BaseSemantics::Policy {
            protected:
                unsigned active;             /**< Bit vector to indicate which policies are active. */
                Policy0<State0, ValueType0> policy0;
                Policy1<State1, ValueType1> policy1;
                Policy2<State2, ValueType2> policy2;
                Policy3<State3, ValueType3> policy3;

            public:
                /** Default constructor.  The default constructor initializes the multi-policy's register dictionary to be the
                 *  same as the dictionary used by the first sub-policy.  This can be changed by calling
                 *  set_register_dictionary(), although that will set the dictionary for the multi-policy and all
                 *  sub-policies. */
                Policy(): active(0xffffffff) {
                    const RegisterDictionary *rd = get_policy(SP0()).get_register_dictionary();
                    BaseSemantics::Policy::set_register_dictionary(rd); // don't change the sub-policies
                }

                virtual ~Policy() {}

                /** Get a sub-policy.  Sub policies are identified with the tags SP0, SP1, etc., just as for sub-values and
                 *  sub-states.
                 * @{ */
                Policy0<State0, ValueType0>& get_policy(const SP0&) { return policy0; }
                Policy1<State1, ValueType1>& get_policy(const SP1&) { return policy1; }
                Policy2<State2, ValueType2>& get_policy(const SP2&) { return policy2; }
                Policy3<State3, ValueType3>& get_policy(const SP3&) { return policy3; }
                const Policy0<State0, ValueType0>& get_policy(const SP0&) const { return policy0; }
                const Policy1<State1, ValueType1>& get_policy(const SP1&) const { return policy1; }
                const Policy2<State2, ValueType2>& get_policy(const SP2&) const { return policy2; }
                const Policy3<State3, ValueType3>& get_policy(const SP3&) const { return policy3; }
                /** @} */

                /** Returns whether a sub-policy is active.  If a sub-policy is inactive then the RISC operations for that
                 *  sub-policy are not invoked by the corresponding RISC operation in the multi-policy.  (It is also not
                 *  invoked if one or more of its input operands are invalid.)
                 * @{ */
                bool is_active(const SP0 &sp) const { return 0 != (active & sp.mask); }
                bool is_active(const SP1 &sp) const { return 0 != (active & sp.mask); }
                bool is_active(const SP2 &sp) const { return 0 != (active & sp.mask); }
                bool is_active(const SP3 &sp) const { return 0 != (active & sp.mask); }
                /** @} */

                /** Change the active state of a sub-policy.  See also, is_active().
                 * @{ */
                void set_active(const SP0 &sp, bool b=true) { set_active(sp.mask, b); }
                void set_active(const SP1 &sp, bool b=true) { set_active(sp.mask, b); }
                void set_active(const SP2 &sp, bool b=true) { set_active(sp.mask, b); }
                void set_active(const SP3 &sp, bool b=true) { set_active(sp.mask, b); }
                /** @} */

                /** Make a sub-policy inactive.  See also, is_active().  This is just a convenience for calling set_active()
                 *  with a (non-default) false state.  Both versions are defined since "set" for a Boolean value usually means
                 *  something different than "set" for a non-Boolean value.
                 * @{ */
                void clear_active(const SP0 &sp) { set_active(sp.mask, false); }
                void clear_active(const SP1 &sp) { set_active(sp.mask, false); }
                void clear_active(const SP2 &sp) { set_active(sp.mask, false); }
                void clear_active(const SP3 &sp) { set_active(sp.mask, false); }
                /** @} */

                /** Return a bitmask of active policies.  Each bit corresponds to a sub-policy, with the least significant bit
                 *  representing the SP0 policy.  A bit is set if the policy is enabled. See also, is_active(). */
                unsigned get_active_policies() const { return active; }

                /** Bulk setting of policy active states.  Each bit corresponds to a sub-policy, with the least significant bit
                 *  representing the SP0 policy.  The policies corresponding to set bits are enabled, and the others are
                 *  disabled.  Bits not corresponding to a policy are ignored.  See also, set_active() and clear_active(). */
                void set_active_policies(unsigned mask) { active = mask; }

                /** Called before any sub-policy operations.  Subclasses can override this to do something useful. */
                virtual void before() {};

                /** Called after all sub-policy operations.  Subclasses can override this to do something useful. */
                virtual void after() {};

            protected:
                // Helper for changing sub-policy activity state.
                void set_active(unsigned mask, bool b) {
                    if (b) {
                        active |= mask;
                    } else {
                        active &= ~mask;
                    }
                }

                //  Helper functions for the CALL_SUBS_* macros.
                template<typename SPx>
                bool should_call(const SPx &sp) const {
                    return is_active(sp);
                }
                template<typename SPx, typename T0>
                bool should_call(const SPx &sp, const T0 &a) const {
                    return is_active(sp) && a.is_valid(sp);
                }
                template<typename SPx, typename T0, typename T1>
                bool should_call(const SPx &sp, const T0 &a, const T1 &b) const {
                    return is_active(sp) && a.is_valid(sp) && b.is_valid(sp);
                }
                template<typename SPx, typename T0, typename T1, typename T2>
                bool should_call(const SPx &sp, const T0 &a, const T1 &b, const T2 &c) const {
                    return is_active(sp) && a.is_valid(sp) && b.is_valid(sp) && c.is_valid(sp);
                }

                /**************************************************************************************************************
                 *                                      Macros
                 *
                 * Sorry about all the messy macros, but they do make the RISC operations _much_ simpler and a whole lot less
                 * cut-n-pasted code.  All these macros ultimately make calls to the sub-policy RISC operations.
                 *
                 * The macros come in two broad categories:  those with "_SUBS_" in their name call the sub-policy operations for
                 * all sub-policies that are enabled and whose input argument values are valid.  Those with "_SUB_" in their
                 * name call only one sub-policy (indicated by the first argument) and only when that sub-policy is enabled and
                 * its input arguments are valid.
                 *
                 * Within each of those two broad categories, there are three subcategories:  All names have a digit that
                 * indicates the number of input (not total) arguments to the RISC operation.  If the number is followed by an
                 * "x" then the last macro argument is the argument list for the RISC operation.  If the macro name ends with
                 * "NR" then the RISC operation does not return a value.
                 *
                 * The OPAND_x arguments must be multi-policy values.  A sub-policy is called only if all the inputs are marked
                 * as valid and the sub-policy is enabled.
                 *
                 * For the "x" versions of these macros, the last macro argument, ARGLIST, should be the argument list passed
                 * to the sub-policy.  If any of those arguments are multi-policy values then they should be appended with
                 * ".get_subvalue(tag)", which selects the appropriate sub-value depending on the sub-policy being called (see
                 * the writeRegister() operation for an example).
                 *
                 * Finally, if a macro has a "b" in the last component (like CALL_SUBS_1b) then it handles a RISC operation
                 * that takes two template arguments.  It's done this way because an operation like "extract<from,to>" gets
                 * parsed as two macro arguments that have to be reassembled in the macro body.
                 **************************************************************************************************************/

/* Macros to invoke a RISC operation in a single sub-policy.  The arguments to pass to the sub-policy's operation are listed
 * explicitly. */
#define CALL_SUB_0x(TAG, OPERATION, RETVAL,                            ARGLIST)                                                \
                do {                                                                                                           \
                    if (should_call(TAG))                                                                                      \
                        (RETVAL).set_subvalue(TAG, get_policy(TAG).OPERATION ARGLIST);                                         \
                } while (0)
#define CALL_SUB_1x(TAG, OPERATION, RETVAL, OPAND_A,                   ARGLIST)                                                \
                do {                                                                                                           \
                    if (should_call(TAG, (OPAND_A)))                                                                           \
                        (RETVAL).set_subvalue(TAG, get_policy(TAG).OPERATION ARGLIST);                                         \
                } while (0)
#define CALL_SUB_1bx(TAG, OPERATION, TPL0, RETVAL, OPAND_A,            ARGLIST)                                                \
                do {                                                                                                           \
                    if (should_call(TAG, (OPAND_A)))                                                                           \
                        (RETVAL).set_subvalue(TAG, get_policy(TAG).OPERATION, TPL0 ARGLIST);                                   \
                } while (0)
#define CALL_SUB_2x(TAG, OPERATION, RETVAL, OPAND_A, OPAND_B,          ARGLIST)                                                \
                do {                                                                                                           \
                    if (should_call(TAG, (OPAND_A), (OPAND_B)))                                                                \
                        (RETVAL).set_subvalue(TAG, get_policy(TAG).OPERATION ARGLIST);                                         \
                } while (0)
#define CALL_SUB_3x(TAG, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C, ARGLIST)                                                \
                do {                                                                                                           \
                    if (should_call(TAG, (OPAND_A), (OPAND_B), (OPAND_C)))                                                     \
                        (RETVAL).set_subvalue(TAG, get_policy(TAG).OPERATION ARGLIST);                                         \
                } while (0)

/* Like the non-NR versions, except these don't try to fill in a return value */
#define CALL_SUB_0xNR(TAG, OPERATION,          ARGLIST)                                                                        \
                do {                                                                                                           \
                    if (should_call(TAG))                                                                                      \
                        get_policy(TAG).OPERATION ARGLIST;                                                                     \
                } while (0)
#define CALL_SUB_1xNR(TAG, OPERATION, OPAND_A, ARGLIST)                                                                        \
                do {                                                                                                           \
                    if (should_call(TAG, (OPAND_A)))                                                                           \
                        get_policy(TAG).OPERATION ARGLIST;                                                                     \
                } while (0)
#define CALL_SUB_2xNR(TAG, OPERATION, OPAND_A, OPAND_B, ARGLIST)                                                               \
                do {                                                                                                           \
                    if (should_call(TAG, (OPAND_A), (OPAND_B)))                                                                \
                        get_policy(TAG).OPERATION ARGLIST;                                                                     \
                } while (0)
#define CALL_SUB_3xNR(TAG, OPERATION, OPAND_A, OPAND_B, OPAND_C, ARGLIST)                                                      \
                do {                                                                                                           \
                    if (should_call(TAG, (OPAND_A), (OPAND_B), (OPAND_C)))                                                     \
                        get_policy(TAG).OPERATION ARGLIST;                                                                     \
                } while (0)

/* Macros to invoke a RISC operation in a single sub-policy.  All operands must be ValueType instances in the multi-policy. */
#define CALL_SUB_0(TAG, OPERATION, RETVAL)                                                                                     \
                CALL_SUB_0x(TAG, OPERATION, RETVAL,                                                                            \
                            ())
#define CALL_SUB_1(TAG, OPERATION, RETVAL, OPAND_A)                                                                            \
                CALL_SUB_1x(TAG, OPERATION, RETVAL, OPAND_A,                                                                   \
                            ((OPAND_A).get_subvalue(TAG)))
#define CALL_SUB_1b(TAG, OPERATION, TPL0, RETVAL, OPAND_A)                                                                     \
                CALL_SUB_1bx(TAG, OPERATION, TPL0, RETVAL, OPAND_A,                                                            \
                            ((OPAND_A).get_subvalue(TAG)))
#define CALL_SUB_2(TAG, OPERATION, RETVAL, OPAND_A, OPAND_B)                                                                   \
                CALL_SUB_2x(TAG, OPERATION, RETVAL, OPAND_A, OPAND_B,                                                          \
                            ((OPAND_A).get_subvalue(TAG), (OPAND_B).get_subvalue(TAG)))
#define CALL_SUB_3(TAG, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C)                                                          \
                CALL_SUB_3x(TAG, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C,                                                 \
                            ((OPAND_A).get_subvalue(TAG), (OPAND_B).get_subvalue(TAG), (OPAND_C).get_subvalue(TAG)))
                
/* Macros to call sub-policies. Operands are multi-policy values. */
#define CALL_SUBS_0(OPERATION, RETVAL)                                                                                         \
                { SP0 tag; CALL_SUB_0(tag, OPERATION, RETVAL);                            }                                    \
                { SP1 tag; CALL_SUB_0(tag, OPERATION, RETVAL);                            }                                    \
                { SP2 tag; CALL_SUB_0(tag, OPERATION, RETVAL);                            }                                    \
                { SP3 tag; CALL_SUB_0(tag, OPERATION, RETVAL);                            }
#define CALL_SUBS_1(OPERATION, RETVAL, OPAND_A)                                                                                \
                { SP0 tag; CALL_SUB_1(tag, OPERATION, RETVAL, OPAND_A);                   }                                    \
                { SP1 tag; CALL_SUB_1(tag, OPERATION, RETVAL, OPAND_A);                   }                                    \
                { SP2 tag; CALL_SUB_1(tag, OPERATION, RETVAL, OPAND_A);                   }                                    \
                { SP3 tag; CALL_SUB_1(tag, OPERATION, RETVAL, OPAND_A);                   }        
#define CALL_SUBS_1b(OPERATION, TPL0, RETVAL, OPAND_A)                                                                         \
                { SP0 tag; CALL_SUB_1b(tag, OPERATION, TPL0, RETVAL, OPAND_A); }                                               \
                { SP1 tag; CALL_SUB_1b(tag, OPERATION, TPL0, RETVAL, OPAND_A); }                                               \
                { SP2 tag; CALL_SUB_1b(tag, OPERATION, TPL0, RETVAL, OPAND_A); }                                               \
                { SP3 tag; CALL_SUB_1b(tag, OPERATION, TPL0, RETVAL, OPAND_A); }
#define CALL_SUBS_2(OPERATION, RETVAL, OPAND_A, OPAND_B)                                                                       \
                { SP0 tag; CALL_SUB_2(tag, OPERATION, RETVAL, OPAND_A, OPAND_B);          }                                    \
                { SP1 tag; CALL_SUB_2(tag, OPERATION, RETVAL, OPAND_A, OPAND_B);          }                                    \
                { SP2 tag; CALL_SUB_2(tag, OPERATION, RETVAL, OPAND_A, OPAND_B);          }                                    \
                { SP3 tag; CALL_SUB_2(tag, OPERATION, RETVAL, OPAND_A, OPAND_B);          }        
#define CALL_SUBS_3(OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C)                                                              \
                { SP0 tag; CALL_SUB_3(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C); }                                    \
                { SP1 tag; CALL_SUB_3(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C); }                                    \
                { SP2 tag; CALL_SUB_3(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C); }                                    \
                { SP3 tag; CALL_SUB_3(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C); }

/* Macros to call sub-policies. The first set of operands are mult-policy values that are inputs to the operation.  The second
 * set of arguments is what will actually be passed to the operation.  Any arguments passed in ARGLIST that are multi-policy
 * values will need to be appended with ".get_value(tag)". */
#define CALL_SUBS_0x(OPERATION, RETVAL,                            ARGLIST)                                                    \
                { SP0 tag; CALL_SUB_0x(tag, OPERATION, RETVAL,                            ARGLIST); }                          \
                { SP1 tag; CALL_SUB_0x(tag, OPERATION, RETVAL,                            ARGLIST); }                          \
                { SP2 tag; CALL_SUB_0x(tag, OPERATION, RETVAL,                            ARGLIST); }                          \
                { SP3 tag; CALL_SUB_0x(tag, OPERATION, RETVAL,                            ARGLIST); }
#define CALL_SUBS_1x(OPERATION, RETVAL, OPAND_A,                   ARGLIST)                                                    \
                { SP0 tag; CALL_SUB_1x(tag, OPERATION, RETVAL, OPAND_A,                   ARGLIST); }                          \
                { SP1 tag; CALL_SUB_1x(tag, OPERATION, RETVAL, OPAND_A,                   ARGLIST); }                          \
                { SP2 tag; CALL_SUB_1x(tag, OPERATION, RETVAL, OPAND_A,                   ARGLIST); }                          \
                { SP3 tag; CALL_SUB_1x(tag, OPERATION, RETVAL, OPAND_A,                   ARGLIST); }
#define CALL_SUBS_2x(OPERATION, RETVAL, OPAND_A, OPAND_B,          ARGLIST)                                                    \
                { SP0 tag; CALL_SUB_2x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B,          ARGLIST); }                          \
                { SP1 tag; CALL_SUB_2x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B,          ARGLIST); }                          \
                { SP2 tag; CALL_SUB_2x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B,          ARGLIST); }                          \
                { SP3 tag; CALL_SUB_2x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B,          ARGLIST); }
#define CALL_SUBS_3x(OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C, ARGLIST)                                                    \
                { SP0 tag; CALL_SUB_3x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }                          \
                { SP1 tag; CALL_SUB_3x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }                          \
                { SP2 tag; CALL_SUB_3x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }                          \
                { SP3 tag; CALL_SUB_3x(tag, OPERATION, RETVAL, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }

/* Like the non-NR versions, except these don't try to fill in a return value. */
#define CALL_SUBS_0xNR(OPERATION,                            ARGLIST)                                                          \
                { SP0 tag; CALL_SUB_0xNR(tag, OPERATION,                            ARGLIST); }                                \
                { SP1 tag; CALL_SUB_0xNR(tag, OPERATION,                            ARGLIST); }                                \
                { SP2 tag; CALL_SUB_0xNR(tag, OPERATION,                            ARGLIST); }                                \
                { SP3 tag; CALL_SUB_0xNR(tag, OPERATION,                            ARGLIST); }
#define CALL_SUBS_1xNR(OPERATION, OPAND_A,                   ARGLIST)                                                          \
                { SP0 tag; CALL_SUB_1xNR(tag, OPERATION, OPAND_A,                   ARGLIST); }                                \
                { SP1 tag; CALL_SUB_1xNR(tag, OPERATION, OPAND_A,                   ARGLIST); }                                \
                { SP2 tag; CALL_SUB_1xNR(tag, OPERATION, OPAND_A,                   ARGLIST); }                                \
                { SP3 tag; CALL_SUB_1xNR(tag, OPERATION, OPAND_A,                   ARGLIST); }
#define CALL_SUBS_2xNR(OPERATION, OPAND_A, OPAND_B,          ARGLIST)                                                          \
                { SP0 tag; CALL_SUB_2xNR(tag, OPERATION, OPAND_A, OPAND_B,          ARGLIST); }                                \
                { SP1 tag; CALL_SUB_2xNR(tag, OPERATION, OPAND_A, OPAND_B,          ARGLIST); }                                \
                { SP2 tag; CALL_SUB_2xNR(tag, OPERATION, OPAND_A, OPAND_B,          ARGLIST); }                                \
                { SP3 tag; CALL_SUB_2xNR(tag, OPERATION, OPAND_A, OPAND_B,          ARGLIST); }
#define CALL_SUBS_3xNR(OPERATION, OPAND_A, OPAND_B, OPAND_C, ARGLIST)                                                          \
                { SP0 tag; CALL_SUB_3xNR(tag, OPERATION, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }                                \
                { SP1 tag; CALL_SUB_3xNR(tag, OPERATION, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }                                \
                { SP2 tag; CALL_SUB_3xNR(tag, OPERATION, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }                                \
                { SP3 tag; CALL_SUB_3xNR(tag, OPERATION, OPAND_A, OPAND_B, OPAND_C, ARGLIST); }

                /**************************************************************************************************************
                 *                                      Semantic API
                 * These are the functions that are needed by the semantic translation class, such as X86InstructionSemantics.
                 **************************************************************************************************************/
            public:

                /** See BaseSemantics::set_register_dictionary().  Setting the register dictionary in the multi-policy will
                 *  also set it in all sub-policies.  However, get_register_dictionary() and findRegister() are only executed
                 *  in the multi-policy. */
                void set_register_dictionary(const RegisterDictionary *regdict) {
                    unsigned saved_status = get_active_policies();
                    set_active_policies((unsigned)(-1)); // enable all sub policies
                    before();
                    CALL_SUBS_0xNR(set_register_dictionary, (regdict));
                    after();
                    set_active_policies(saved_status);
                }

                /** See NullSemantics::Policy::startInstruction() */
                void startInstruction(SgAsmInstruction *insn) {
                    before();
                    CALL_SUBS_0xNR(startInstruction, (insn));
                    after();
                }

                /** See NullSemantics::Policy::finishInstruction() */
                void finishInstruction(SgAsmInstruction *insn) {
                    before();
                    CALL_SUBS_0xNR(finishInstruction, (insn));
                    after();
                }
                
                /** See NullSemantics::Policy::true_() */
                ValueType<1> true_() {
                    ValueType<1> retval;
                    before();
                    CALL_SUBS_0(true_, retval);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::false_() */
                ValueType<1> false_() {
                    ValueType<1> retval;
                    before();
                    CALL_SUBS_0(false_, retval);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::false_() */
                template<size_t nBits>
                ValueType<nBits> undefined_() {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_0(undefined_<nBits>, retval);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::number() */
                template<size_t nBits>
                ValueType<nBits> number(uint64_t n) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_0x(number<nBits>, retval, (n));
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::filterCallTarget() */
                ValueType<32> filterCallTarget(const ValueType<32> &a) {
                    ValueType<32> retval;
                    before();
                    CALL_SUBS_1(filterCallTarget, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::filterReturnTarget() */
                ValueType<32> filterReturnTarget(const ValueType<32> &a) {
                    ValueType<32> retval;
                    before();
                    CALL_SUBS_1(filterReturnTarget, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::filterIndirectJumpTarget() */
                ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) {
                    ValueType<32> retval;
                    before();
                    CALL_SUBS_1(filterIndirectJumpTarget, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::hlt() */
                void hlt() {
                    before();
                    CALL_SUBS_0xNR(hlt, ());
                    after();
                }

                /** See NullSemantics::Policy::cpuid() */
                void cpuid() {
                    before();
                    CALL_SUBS_0xNR(cpuid, ());
                    after();
                }

                /** See NullSemantics::Policy::rdtsc() */
                ValueType<64> rdtsc() {
                    ValueType<64> retval;
                    before();
                    CALL_SUBS_0(rdtsc, retval);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::interrupt() */
                void interrupt(uint8_t n) {
                    before();
                    CALL_SUBS_0xNR(interrupt, (n));
                    after();
                }

                /** See NullSemantics::Policy::sysenter() */
                void sysenter() {
                    before();
                    CALL_SUBS_0xNR(sysenter, ());
                    after();
                }

                /** See NullSemantics::Policy::add() */
                template <size_t nBits>
                ValueType<nBits> add(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_2(add, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::addWithCarries() */
                template <size_t nBits>
                ValueType<nBits> addWithCarries(const ValueType<nBits> &a, const ValueType<nBits> &b, const ValueType<1> &c,
                                                ValueType<nBits> &carry_out) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_3x(addWithCarries, retval, a, b, c,
                                 (a.get_subvalue(tag), b.get_subvalue(tag), c.get_subvalue(tag), carry_out.get_subvalue(tag)));
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::and_() */
                template <size_t nBits>
                ValueType<nBits> and_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_2(and_, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::equalToZero() */
                template<size_t nBits>
                ValueType<1> equalToZero(const ValueType<nBits> &a) {
                    ValueType<1> retval;
                    before();
                    CALL_SUBS_1(equalToZero, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::invert() */
                template<size_t nBits>
                ValueType<nBits> invert(const ValueType<nBits> &a) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_1(invert, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::extract() */
                template<size_t BeginBit, size_t EndBit, size_t nBitsA>
                ValueType<EndBit-BeginBit> extract(const ValueType<nBitsA> &a) {
                    ValueType<EndBit-BeginBit> retval;
                    before();
                    CALL_SUBS_1b(extract<BeginBit, EndBit>, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::concat() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> concat(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA+nBitsB> retval;
                    before();
                    CALL_SUBS_2(concat, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::ite() */
                template<size_t nBits>
                ValueType<nBits> ite(const ValueType<1> &cond, const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_1x(ite, retval, cond,
                                 (cond.get_subvalue(tag), a.get_subvalue(tag), b.get_subvalue(tag)));
                    after();
                    return retval;
                }
                
                /** See NullSemantics::Policy::leastSignificantSetBit() */
                template<size_t nBits>
                ValueType<nBits> leastSignificantSetBit(const ValueType<nBits> &a) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_1(leastSignificantSetBit, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::mostSignificantSetBit() */
                template<size_t nBits>
                ValueType<nBits> mostSignificantSetBit(const ValueType<nBits> &a) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_1(mostSignificantSetBit, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::negate() */
                template<size_t nBits>
                ValueType<nBits> negate(const ValueType<nBits> &a) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_1(negate, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::or_() */
                template<size_t nBits>
                ValueType<nBits> or_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_2(or_, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::rotateLeft() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> rotateLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA> retval;
                    before();
                    CALL_SUBS_2(rotateLeft, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::rotateRight() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> rotateRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA> retval;
                    before();
                    CALL_SUBS_2(rotateRight, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::shiftLeft() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA> retval;
                    before();
                    CALL_SUBS_2(shiftLeft, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::shiftRight() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA> retval;
                    before();
                    CALL_SUBS_2(shiftRight, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::shiftRightArithmetic() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftRightArithmetic(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA> retval;
                    before();
                    CALL_SUBS_2(shiftRightArithmetic, retval, a, b);
                    after();
                    return retval;
                }

                /** Extend (or shrink) from @p FromLen bits to @p ToLen bits by adding or removing high-order bits from the
                 *  input. Added bits are always zeros. */
                template <size_t From, size_t To>
                ValueType<To> unsignedExtend(const ValueType<From> &a) {
                    ValueType<To> retval;
                    before();
                    CALL_SUBS_1b(unsignedExtend<From, To>, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::signExtend() */
                template<size_t From, size_t To>
                ValueType<To> signExtend(const ValueType<From> &a) {
                    ValueType<To> retval;
                    before();
                    CALL_SUBS_1b(signExtend<From, To>, retval, a);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::signedDivide() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> signedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA> retval;
                    before();
                    CALL_SUBS_2(signedDivide, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::signedModulo() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsB> signedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsB> retval;
                    before();
                    CALL_SUBS_2(signedModulo, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::signedMultiply() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> signedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA+nBitsB> retval;
                    before();
                    CALL_SUBS_2(signedMultiply, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::unsignedDivide() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> unsignedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA> retval;
                    before();
                    CALL_SUBS_2(unsignedDivide, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::unsignedModulo() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsB> unsignedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsB> retval;
                    before();
                    CALL_SUBS_2(unsignedModulo, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::unsignedMultiply() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> unsignedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    ValueType<nBitsA+nBitsB> retval;
                    before();
                    CALL_SUBS_2(unsignedMultiply, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::xor_() */
                template<size_t nBits>
                ValueType<nBits> xor_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_2(xor_, retval, a, b);
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::readRegister() */
                template<size_t nBits>
                ValueType<nBits> readRegister(const char *regname) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_0x(readRegister<nBits>, retval, (regname));
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::readRegister() */
                template<size_t nBits>
                ValueType<nBits> readRegister(const RegisterDescriptor &reg) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_0x(readRegister<nBits>, retval, (reg));
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::writeRegister() */
                template<size_t nBits>
                void writeRegister(const char *regname, const ValueType<nBits> &a) {
                    before();
                    CALL_SUBS_1xNR(writeRegister, a, (regname, a.get_subvalue(tag)));
                    after();
                }

                /** See NullSemantics::Policy::writeRegister() */
                template<size_t nBits>
                void writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &a) {
                    before();
                    CALL_SUBS_1xNR(writeRegister, a, (reg, a.get_subvalue(tag)));
                    after();
                }

                /** See NullSemantics::Policy::readMemory() */
                template<size_t nBits>
                ValueType<nBits> readMemory(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<1> &cond) {
                    ValueType<nBits> retval;
                    before();
                    CALL_SUBS_2x(readMemory<nBits>, retval, addr, cond,
                                 (sr, addr.get_subvalue(tag), cond.get_subvalue(tag)));
                    after();
                    return retval;
                }

                /** See NullSemantics::Policy::writeMemory() */
                template<size_t nBits>
                void writeMemory(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<nBits> &data,
                                 const ValueType<1> &cond) {
                    before();
                    CALL_SUBS_3xNR(writeMemory, addr, data, cond,
                                   (sr, addr.get_subvalue(tag), data.get_subvalue(tag), cond.get_subvalue(tag)));
                    after();
                }

                /**************************************************************************************************************
                 *                                      Additional useful stuff
                 *
                 * This stuff isn't part of the translator's policy API, but is useful for other reasons.
                 **************************************************************************************************************/
            public:

                /** Print each sub-policy.  The print method of each active sub-policy is called. */
                void print(std::ostream &o) const {
                    //before() -- not really a RISC operation
                    CALL_SUBS_0xNR(print, (o));
                    //after()
                }

                /** Print each sub-policy.  The print method of each active sub-policy is called. */
                friend std::ostream& operator<<(std::ostream &o, const Policy &policy) {
                    policy.print(o);
                    return o;
                }
            };

        };

    } /*namespace*/
} /*namespace*/

#endif
