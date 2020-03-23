#ifndef ROSE_BinaryAnalysis_VxcoreParser_H
#define ROSE_BinaryAnalysis_VxcoreParser_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemanticsRegisterState.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <Diagnostics.h>
#include <MemoryMap.h>
#include <StringUtility.h>

namespace Rose {
namespace BinaryAnalysis {

/** Parser for Vxcore format files.
 *
 *  The vxcore format is a format defined by ROSE itself for loading memory dumps from simple hardware devices.
 *  The format is intended to be as simple as possible so that it's easy to produce on the low-memory, low-powered
 *  device.
 *
 *  @section vxcore_v1 Version 1
 *
 *  Version 1 of this file format consists of two kinds of records: memory and registers.
 *
 *  A memory record begins with header which is line of plain ASCII text terminated by a colon and line-feed. The line contains
 *  the starting address, the size in bytes, and the optional memory protection bits. These fields are separated from one
 *  another by horizontal white space. The address and size are hexadecimal values without a leading "0x". The optional
 *  protection bits are introduced with an equal sign "=" followed by zero or more of the upper-case letters "R" (read), "W"
 *  (write), "X" (execute) in any order. Within the protection bits, a hyphen "-" can be used as a placeholder if desired. The
 *  header's line-feed is immediately followed by the indicated number of bytes stored in binary format.  The bytes are not
 *  terminated by any special sequence, but rather lead directly into the next header.
 *
 *  A register record (at most one per file) begins with the word "registers", a space, and the instruction set architecture name
 *  recognized by ROSE.  Following the header is one line per register, each line being a register name recognized by ROSE, a colon,
 *  optional horizontal white space, and a hexadecimal value, this time with a leading "0x". */
class VxcoreParser {
public:
    /** Settings that control the parser and unparser. */
    struct Settings {
        unsigned version;                               /**< Format version number. */
        Sawyer::Optional<unsigned> protOverride;        /**< Memory protection to overrides parsed values. */
        unsigned protDefault;                           /**< Default memory protection when none are specified or overridden. */

        Settings()
            : version(1) {}
    };
    
    /** Exception thrown for parse errors. */
    class Exception: public Rose::Exception {
        boost::filesystem::path fileName_;
        size_t offset_;

    public:
        /** Create a new exception with a location and error message. */
        Exception(const boost::filesystem::path &fileName, size_t offset, const std::string &what)
            : Rose::Exception(what), fileName_(fileName), offset_(offset) {}

        ~Exception() throw () {}

        /** Render this exception as a string. */
        std::string toString() const {
            return StringUtility::cEscape(fileName_.string()) + ":" + boost::lexical_cast<std::string>(offset_) + ": " + what();
        }

        /** Emit this exception to a stream.
         *
         *  No trailing linefeed is emitted. */
        void print(std::ostream &out) const {
            out <<toString();
        }

        friend std::ostream& operator<<(std::ostream &out, const Exception &e) {
            e.print(out);
            return out;
        }
    };

private:
    Settings settings_;
    std::string isaName_;                               // Parsed instruction set architecture name

public:
    static Sawyer::Message::Facility mlog;              /**< Diagnostic facility for vxcore file format. */
    static void initDiagnostics();                      // used internally

public:
    /** Property: Settings for parsing and unparsing.
     *
     * @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    void settings(const Settings &s) { settings_ = s; }
    /** @} */

    /** Parse a URL and adjust the settings.
     *
     *  Parses the specified URL which should not include the leading "vxcore:", adjusts this object's @ref settings, and
     *  returns the file name part of the URL.  Throws an @ref Exception for any parsing errors.
     *
     *  Note that this function doesn't actually parse the file, only the URL that contains the file name. */
    boost::filesystem::path parseUrl(const std::string&);

    /** Parse input file.
     *
     *  The specified file is opened and parsed and the results used to initialize memory and/or registers. If a non-null
     *  memory map is passed as an argument, then then file data is copied into the map. If a non-null register state is
     *  provided (along with a required RiscOperators object for extraction and concatenation), then registers are set to
     *  values contained in the file.
     *
     *  Parse errors throw an @ref Exception.
     *
     * @{ */
    void parse(const boost::filesystem::path&, const MemoryMap::Ptr&);
    void parse(const boost::filesystem::path&, const InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
               const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops);
    void parse(const boost::filesystem::path&, const MemoryMap::Ptr&,
               const InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
               const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops);
    void parse(std::istream&, const MemoryMap::Ptr&, const InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
               const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops, const std::string &inputName = "input");
    /** @} */

    /** Unparse memory and/or registers to this format.
     *
     *  If a non-null memory map is supplied, then the memory that's contained within the specified @p memoryLimit
     *  address range is output (pass <code>AddressInterval::whole()</code> to emit all mapped memory).
     *
     *  If a non-null register state is specified, then output values for all the registers contained in that state, skipping
     *  registers that don't have a concrete value. In this case, a RiscOperators object must also be supplied in order to
     *  concat and/or extract register parts.
     *
     *  The optional @p outputName is used to identify the output stream in error messages and exceptions.
     *
     * @{ */
    void unparse(std::ostream&, const MemoryMap::Ptr&, const AddressInterval &memoryLimit,
                 const std::string &outputName = "output");
    void unparse(std::ostream&, const InstructionSemantics2::BaseSemantics::RegisterStatePtr&,
                 const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                 const std::string &outputName = "output");
    void unparse(std::ostream&, const MemoryMap::Ptr&, const AddressInterval &memoryLimit,
                 const InstructionSemantics2::BaseSemantics::RegisterStatePtr&,
                 const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                 const std::string &outputName = "output");
    /** @} */

    /** Parsed instruction set architecture name.
     *
     *  Latest encountered instruction set architecture name parsed from the input. */
    const std::string& isaName() const { return isaName_; }
    
private:
    // Parse a memory section of the input when given a header and the input stream positioned at the first byte after the
    // header.  Returns false if the header is invalid, throws an Exception if there's a problem reading the data, and returns
    // true if the memory section was parsed. The memory map can be null, in which case all the normal parsing occurs but
    // nothing is updated. The inputName is only for the exceptions.
    bool parseMemory(const std::string &header, std::istream&, const MemoryMap::Ptr&, const std::string &inputName,
                     size_t headerOffset);

    // Similar to parseMemory except for registers.
    bool parseRegisters(const std::string &header, std::istream&, const InstructionSemantics2::BaseSemantics::RegisterStatePtr&,
                        const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops,
                        const std::string &inputName, size_t headerOffset);
};

} // namespace
} // namespace

#endif
#endif
