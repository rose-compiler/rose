#include "sage3basic.h"
#include <Partitioner2/Utility.h>

#include "Diagnostics.h"
#include "rose_strtoull.h"
#include <Partitioner2/Partitioner.h>


namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Partitioner2");
        mlog.comment("partitioning insns to basic blocks and functions");
    }
}

bool
sortBasicBlocksByAddress(const BasicBlock::Ptr &a, const BasicBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

bool
sortDataBlocks(const DataBlock::Ptr &a, const DataBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a!=b) {
        if (a->address() != b->address())
            return a->address() < b->address();
        if (a->size() < b->size())
            return true;
    }
    return false;
}

bool
sortFunctionsByAddress(const Function::Ptr &a, const Function::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

bool
sortFunctionNodesByAddress(const SgAsmFunction *a, const SgAsmFunction *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->get_entry_va() < b->get_entry_va();
}


// Sort by the successor expressions. If both expressions are concrete then disregard their widths and treat them as unsigned
// when comparing.
bool
sortByExpression(const BasicBlock::Successor &a, const BasicBlock::Successor &b) {
    if (a.expr()->is_number() && b.expr()->is_number())
        return a.expr()->get_number() < b.expr()->get_number();
    if (a.expr()->is_number() || b.expr()->is_number())
        return a.expr()->is_number();                   // concrete values are less than abstract expressions
    return a.expr()->get_expression()->compareStructure(b.expr()->get_expression()) < 0;
}

bool
sortBlocksForAst(SgAsmBlock *a, SgAsmBlock *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a->get_address() != b->get_address())
        return a->get_address() < b->get_address();
    if (a->get_statementList().size()>0 && b->get_statementList().size()>0) {
        // Sort so basic blocks come before data blocks when they start at the same address, regardless of size
        bool a_isBasicBlock = NULL!=isSgAsmInstruction(a->get_statementList()[0]);
        bool b_isBasicBlock = NULL!=isSgAsmInstruction(b->get_statementList()[0]);
        if (a_isBasicBlock != b_isBasicBlock)
            return a_isBasicBlock;
    }
    return false;
}

bool
sortInstructionsByAddress(SgAsmInstruction *a, SgAsmInstruction *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->get_address() < b->get_address();
}

size_t
serialNumber() {
    static size_t nCalls = 0;
    return ++nCalls;
}

// class method
std::string
AddressIntervalParser::docString() {
    return ("An address interval can be specified as a single address, or a first and inclusive last address separated by a "
            "comma, or a begin and exclusive end address separated by a hyphen, or a begin address and size in bytes "
            "separated by a plus sign, or an empty string to indicate an empty interval.  The upper address must always be "
            "greater than or equal to the lower address. Addresses and sizes can be specified in decimal, hexadecimal "
            "(leading \"0x\"), octal (leading \"0\"), or binary (leading \"0b\").");
}

Sawyer::CommandLine::ParsedValue
AddressIntervalParser::operator()(const char *input, const char **rest, const Sawyer::CommandLine::Location &loc) {
    AddressInterval val = parse(input, rest);
    std::string parsed(input, *rest-input);
    return Sawyer::CommandLine::ParsedValue(val, loc, parsed, valueSaver());
}

AddressInterval
AddressIntervalParser::parse(const char *input, const char **rest) {
    const char *s = input;
    char *r = NULL;
    bool hadRangeError = false, isEmpty = false;
    while (isspace(*s)) ++s;

    // Minimum
    errno = 0;
    rose_addr_t least = rose_strtoull(s, &r, 0);
    if (r==s)
        throw std::runtime_error("unsigned integer expected for interval minimum");
    if (ERANGE==errno)
        hadRangeError = true;
    s = r;

    // Maximum, end, size, or nothing
    rose_addr_t greatest = least;
    while (isspace(*s)) ++s;
    if (','==*s) {                                  // ',' means a max value is specified
        ++s;
        errno = 0;
        greatest = rose_strtoull(s, &r, 0);
        if (r==s)
            throw std::runtime_error("unsigned integer expected for interval maximum");
        if (ERANGE==errno)
            hadRangeError = true;
        s = r;
    } else if ('-'==*s) {                           // '-' means an exclusive end address is specified (think "-" 1)
        ++s;
        errno = 0;
        greatest = rose_strtoull(s, &r, 0);
        if (greatest == least)
            isEmpty = true;
        --greatest;
        if (r==s)
            throw std::runtime_error("unsigned integer expected for interval end");
        if (ERANGE==errno)
            hadRangeError = true;
        s = r;
    } else if ('+'==*s) {                           // '+' means a size follows (zero is allowed)
        ++s;
        errno = 0;
        rose_addr_t size = rose_strtoull(s, &r, 0);
        if (r==s)
            throw std::runtime_error("unsigned integer expected for interval size");
        if (ERANGE==errno)
            hadRangeError = true;
        if (0==size)
            isEmpty = true;
        greatest = least + size - 1;
        s = r;
    } else if (!*s) {                               // end-of-string means the interval is a singleton
        /*void*/
    }

    // Successful parsing?
    *rest = r;
    std::string parsed(input, *rest-input);
    if (hadRangeError)
        throw std::range_error("overflow when parsing \""+parsed+"\"");
    if (greatest < least)
        throw std::range_error("interval seems backward: \""+parsed+"\"");

    if (!isEmpty) {
        return AddressInterval::hull(least, greatest);
    } else {
        return AddressInterval();
    }
}

AddressInterval
AddressIntervalParser::parse(const std::string &str) {
    const char *s = str.c_str();
    const char *rest = NULL;
    AddressInterval retval = parse(s, &rest);
    while (isspace(*rest)) ++rest;
    if (*rest) {
        throw std::runtime_error("extra text after end of address or address interval: "
                                 "\"" + StringUtility::cEscape(rest) + "\"");
    }
    return retval;
}

AddressIntervalParser::Ptr
addressIntervalParser(AddressInterval &storage) {
    return AddressIntervalParser::instance(Sawyer::CommandLine::TypedSaver<AddressInterval>::instance(storage));
}

AddressIntervalParser::Ptr
addressIntervalParser(std::vector<AddressInterval> &storage) {
    return AddressIntervalParser::instance(Sawyer::CommandLine::TypedSaver<std::vector<AddressInterval> >::instance(storage));
}

AddressIntervalParser::Ptr
addressIntervalParser() {
    return AddressIntervalParser::instance();
}

// class method
Sawyer::CommandLine::SwitchGroup
Trigger::switches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    using namespace StringUtility;
    SwitchGroup switches;
    switches.insert(Switch("activate")
                    .argument("interval", addressIntervalParser(settings.when))
                    .doc("Restricts when this action should be triggered in terms of number of calls.  When an action's "
                         "other settings indicate that the action should be performed, the specified interval is consulted "
                         "to determine whether to actually invoke the action.  Whether the action is invoked or not, its "
                         "number-of-calls counter is incremented.  The default is to " +
                         std::string(settings.when.isEmpty() ? "never invoke the action." :
                                     settings.when.isSingleton() ? ("invoke the action only on call " +
                                                                    numberToString(settings.when.least()) + ".") :
                                     settings.when.least()==0 ?("invoke the action " + plural(settings.when.size(), "times") +
                                                                " beginning immediately.") :
                                     ("invoke the action on calls " + numberToString(settings.when.least()) + " through " +
                                      numberToString(settings.when.greatest()) + ", inclusive."))));
    return switches;
}

// class method
std::string
Trigger::docString() {
    Settings settings;
    return Sawyer::CommandLine::Parser().with(switches(settings)).docForSwitches();
}

std::ostream&
operator<<(std::ostream &out, const AddressUser &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressUsers &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressUsageMap &x) {
    x.print(out);
    return out;
}


} // namespace
} // namespace
} // namespace
