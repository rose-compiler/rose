#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Formatter_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Formatter_H

#include <BaseSemanticsTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Format for printing things. Some semantic domains may want to pass some additional information to print methods on a
 *  per-call basis.  This base class provides something they can subclass to do that.  A reference is passed to all print()
 *  methods for semantic objects. */
class Formatter {
public:
    Formatter(): regdict(NULL), suppress_initial_values(false), indentation_suffix("  "), show_latest_writers(true),
                 show_properties(true) {}
    virtual ~Formatter() {}

    /** The register dictionary which is used for printing register names.
     * @{ */
    const RegisterDictionary *get_register_dictionary() const { return regdict; }
    void set_register_dictionary(const RegisterDictionary *rd) { regdict = rd; }
    /** @} */

    /** Whether register initial values should be suppressed.  If a register's value has a comment that is equal to the
     * register name with "_0" appended, then that value is assumed to be the register's initial value.
     * @{ */
    bool get_suppress_initial_values() const { return suppress_initial_values; }
    void set_suppress_initial_values(bool b=true) { suppress_initial_values=b; }
    void clear_suppress_initial_values() { set_suppress_initial_values(false); }
    /** @} */

    /** The string to print at the start of each line. This only applies to objects that occupy more than one line.
     * @{ */
    std::string get_line_prefix() const { return line_prefix; }
    void set_line_prefix(const std::string &s) { line_prefix = s; }
    /** @} */

    /** Indentation string appended to the line prefix for multi-level, multi-line outputs.
     * @{ */
    std::string get_indentation_suffix() const { return indentation_suffix; }
    void set_indentation_suffix(const std::string &s) { indentation_suffix = s; }
    /** @} */

    /** Whether to show latest writer information for register and memory states.
     * @{ */
    bool get_show_latest_writers() const { return show_latest_writers; }
    void set_show_latest_writers(bool b=true) { show_latest_writers = b; }
    void clear_show_latest_writers() { show_latest_writers = false; }
    /** @} */

    /** Whether to show register properties.
     * @{ */
    bool get_show_properties() const { return show_properties; }
    void set_show_properties(bool b=true) { show_properties = b; }
    void clear_show_properties() { show_properties = false; }
    /** @} */

protected:
    const RegisterDictionary *regdict;
    bool suppress_initial_values;
    std::string line_prefix;
    std::string indentation_suffix;
    bool show_latest_writers;
    bool show_properties;
};

/** Adjusts a Formatter for one additional level of indentation.  The formatter's line prefix is adjusted by appending the
 * formatter's indentation suffix.  When this Indent object is destructed, the formatter's line prefix is reset to its original
 * value. */
class Indent {
private:
    Formatter &fmt;
    std::string old_line_prefix;
public:
    Indent(Formatter &fmt): fmt(fmt) {
        old_line_prefix = fmt.get_line_prefix();
        fmt.set_line_prefix(old_line_prefix + fmt.get_indentation_suffix());
    }
    ~Indent() {
        fmt.set_line_prefix(old_line_prefix);
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
