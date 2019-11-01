#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_HotPatch_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_HotPatch_H

#include <BaseSemanticsTypes.h>
#include <BinaryHotPatch.h>
#include <BinarySmtSolver.h>
#include <Sawyer/Message.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Describes how to modify machine state after each instruction.
 *
 *  The hot patching works by examining the hot patch records one by one trying to match them to the current machine state. When
 *  a record matches, the machine state is updated according to the matched record. */
class HotPatch {
public:
    /** Diagnostic output for hot patching. */
    static Sawyer::Message::Facility mlog;

    /** Describes a single hot patch. */
    class Record {
    public:
        /** Type of patch record. */
        enum Type {
            PATCH_REGISTER,                             /**< Change the value of a register. */
            PATCH_NONE                                  /**< Type for default-constructed records. */
        };

        /** Behavior when a record matches. */
        enum Behavior {
            MATCH_CONTINUE,                             /**< Try to match additional subsequent records. */
            MATCH_BREAK                                 /**< Don't try to match more records after a match is found. */
        };

    private:
        Type type_;                                     // record type
        RegisterDescriptor register_;                   // register to be modified for PATCH_REGISTER types.
        InstructionSemantics2::BaseSemantics::SValuePtr oldValue_; // value to match
        InstructionSemantics2::BaseSemantics::SValuePtr newValue_; // replacement value
        Behavior behavior_;                             // whether to continue matching more records

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(type_);
            s & BOOST_SERIALIZATION_NVP(register_);
            s & BOOST_SERIALIZATION_NVP(oldValue_);
            s & BOOST_SERIALIZATION_NVP(newValue_);
            s & BOOST_SERIALIZATION_NVP(behavior_);
        }
#endif

    public:
        /** Construct a no-op record. */
        Record()
            : type_(PATCH_NONE), behavior_(MATCH_CONTINUE) {}

        /** Construct a record that substitutes a register.
         *
         *  If @p reg is an empty (default constructed) register descriptor or if the @p oldValue is null then the returned record will
         *  never match any state.  The @p newValue must be non-null. */
        Record(RegisterDescriptor reg, const InstructionSemantics2::BaseSemantics::SValuePtr &oldValue,
               const InstructionSemantics2::BaseSemantics::SValuePtr &newValue, Behavior behavior = MATCH_CONTINUE)
            : type_(PATCH_REGISTER), register_(reg), oldValue_(oldValue), newValue_(newValue), behavior_(behavior) {
            ASSERT_not_null(newValue);
        }

        /** Property: Type of record.
         *
         *  The record type is read-only, set when the record was constructed. */
        Type type() const {
            return type_;
        }

        /** Property: Register to be matched.
         *
         *  For @ref PATCH_REGISTER records, this is the register that is to be examined and possibly changed. An empty (default
         *  constructed) register descriptor doesn't ever match anything.
         *
         * @{ */
        RegisterDescriptor reg() const {
            return register_;
        }
        void reg(RegisterDescriptor r) {
            register_ = r;
        }
        /** @} */

        /** Property: Value to match.
         *
         *  This is the value that must be matched in order for this patch record to be applied. A null value never matches
         *  anything.
         *
         * @{ */
        InstructionSemantics2::BaseSemantics::SValuePtr oldValue() const {
            return oldValue_;
        }
        void oldValue(const InstructionSemantics2::BaseSemantics::SValuePtr &v) {
            oldValue_ = v;
        }
        /** @} */

        /** Property: Replacement value.
         *
         *  This is the value that will be substituted into the state if the hot patch record matches.  The value must not
         *  be null.
         *
         * @{ */
        InstructionSemantics2::BaseSemantics::SValuePtr newValue() const {
            return newValue_;
        }
        void newValue(const InstructionSemantics2::BaseSemantics::SValuePtr &v) {
            newValue_ = v;
        }
        /** @} */

        /** Property: Behavior after matching.
         *
         *  When a record matches, the behavior property determines if the hot patch mechanism continues to search for additional
         *  matching records, or stops trying to match.
         *
         * @{ */
        Behavior behavior() const {
            return behavior_;
        }
        void behavior(Behavior b) {
            behavior_ = b;
        }
        /** @} */
    };
    

    /** Ordered list of hot patch records. */
    typedef std::vector<Record> Records;

private:
    Records records_;

    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(records_);
    }
#endif

public:
    /** Property: Hot patch records.
     *
     *  Hot patch records are matched in the order they're defined here.
     *
     * @{ */
    const Records& records() const {
        return records_;
    }
    Records& records() {
        return records_;
    }
    /** @} */

    /** Number of hot-patch records in this object. */
    size_t nRecords() const {
        return records_.size();
    }

    /** Remove all records from this object. */
    void clear() {
        records_.clear();
    }
    
    /** Append a hot-patch record.
     *
     *  This is just a convenience function that's equivalent to <code>records.push_back(x)</code>. It returns the index
     *  of the new record. */
    size_t append(const Record &record) {
        records_.push_back(record);
        return records_.size() - 1;
    }

    /** Reference a particular record.
     *
     *  The @p idx must be less than @ref nRecords.
     *
     * @{ */
    const Record& operator[](size_t idx) const {
        ASSERT_require(idx < records_.size());
        return records_[idx];
    }
    Record& operator[](size_t idx) {
        ASSERT_require(idx < records_.size());
        return records_[idx];
    }
    /** @} */

    /** Apply records to a machine state.
     *
     *  Scans through the records of this object and tries to match each against the specified machine state. If a match occurs, the
     *  state is modified according to the record. After a match is found and a change is made, the process continues according to
     *  the matched record's @ref Record::behavior "behavior" property.
     *
     *  Returns the number of matching records that were applied.
     *
     * @{ */
    size_t apply(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) const;
    size_t operator()(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops) const {
        return apply(ops);
    }
    /** @} */

    /** Initialize diagnostic output. This is called automatically when ROSE is initialized. */
    static void initDiagnostics();
};


} // namespace
} // namespace

#endif
