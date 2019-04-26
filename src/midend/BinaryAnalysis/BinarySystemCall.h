#ifndef Rose_BinaryAnalysis_LinuxSyscall_H
#define Rose_BinaryAnalysis_LinuxSyscall_H

#include <AbstractLocation.h>
#include <BinarySmtSolver.h>
#include <boost/filesystem.hpp>
#include <Partitioner2/BasicTypes.h>
#include <RoseException.h>
#include <Sawyer/Map.h>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

/** Analyzes basic blocks to get system call names.
 *
 *  After being initialized with information about how system calls operate, this analysis is able to analyze a basic block
 *  that contains a system call and return information about the call such as its name. */
class SystemCall {
public:
    /** Exceptions for system call analysis. */
    class Exception: public Rose::Exception {
    public:
        /** Construct an exception with a message. */
        Exception(const std::string &mesg): Rose::Exception(mesg) {}

        /** Destructor. */
        ~Exception() throw () {}
    };
        
    /** Declarative information about a system call. */
    struct Declaration {
        uint64_t id;                                    /**< Identification number. */
        std::string name;                               /**< Name of system call. */

        /** Default constructor. */
        Declaration()
            : id(0) {}
        
        /** Constructor. */
        Declaration(uint64_t id, const std::string &name)
            : id(id), name(name) {}
    };

    /** Table of syscall declarations indexed by syscall IDs. */
    typedef Sawyer::Container::Map<uint64_t /*id*/, Declaration> Declarations; 

private:
    Declarations declarations_;                         // table describing all known system calls index by system call number
    AbstractLocation ident_;                            // location that identifies the system call

public:
    virtual ~SystemCall() {}

    /** Declare a system call. */
    void declare(const Declaration &declaration);

    /** Declare multiple system calls. */
    void declare(const std::vector<Declaration> &declarations);

    /** Declare multiple system calls by parsing a Linux header file. */
    void declare(const boost::filesystem::path &headerFileName);

    /** All declarations. */
    const Declarations& declarations() const { return declarations_; }

    /** Look up a system call by ID number. There is at most one system call declaration per ID. */
    Sawyer::Optional<Declaration> lookup(uint64_t id) const;

    /** Look up system calls by name. There can be more than one system call with the same name, although this is uncommon. */
    std::vector<Declaration> lookup(const std::string &name) const;
    
    /** Parse a Linux header file to obtain declarations for system calls. */
    static std::vector<Declaration> parseHeaderFile(const boost::filesystem::path &headerFileName);

    /** Determine whether a basic block has a system call.
     *
     *  If the specified basic block has a system call instruction, then the pointer to that instruction is returned. Otherwise
     *  a null pointer is returned. The determination is made by invoking @ref isSystemCall for each instruction, which is
     *  intended to be a fairly inexpensive operation (certainly less expensive than @ref analyze). */
    SgAsmInstruction* hasSystemCall(const Partitioner2::BasicBlockPtr&) const;

    /** Property: Abstract location that identifies the particular system call.
     *
     *  This is a register or memory location that identifies which system call is being invoked.  For instance, for a specimen
     *  that's intended to run on 32-bit i386 Linux, the identifying location is the EAX register.
     *
     * @{ */
    const AbstractLocation& ident() const { return ident_; }
    void ident(const AbstractLocation &location) { ident_ = location; }
    /** @} */

    /** Analyze a basic block and return the system calls.
     *
     *  Analyzes the basic block using the same instruction semantics as the partitioner.  If an instruction is specified then
     *  it should be the instruction that is the actual system call within the specified basic block. If no instruction is
     *  specified then @ref isSystemCall is invoked for each instruction and the first for which it returns true is considered
     *  to be the system call instruction.
     *
     *  If a system call instruction is specified or found, then the pre-instruction semantic state is queried according to @p
     *  ident to obtain a concrete value of a register or memory location. This value is looked up in the list of known system
     *  calls to return either a system call declaration or nothing.
     *
     *  Throws an exception if semantic analysis fails. */
    virtual Sawyer::Optional<Declaration>
    analyze(const Partitioner2::Partitioner&, const Partitioner2::BasicBlockPtr&, SgAsmInstruction*,
            const SmtSolver::Ptr &solver = SmtSolver::Ptr()) const;

    /** Predicate to determine whether an instruction is a system call. */
    virtual bool isSystemCall(SgAsmInstruction*) const;
};

} // namespace
} // namespace

#endif
