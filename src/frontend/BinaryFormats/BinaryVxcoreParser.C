#include <sage3basic.h>
#include <BinaryVxcoreParser.h>

#include <BaseSemanticsRiscOperators.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <rose_getline.h>
#include <rose_strtoull.h>

using namespace Sawyer::Message::Common;
namespace BaseSemantics = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility VxcoreParser::mlog;

// class method
void
VxcoreParser::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::VxcoreParser");
        mlog.comment("parsing and unparsing vxcore format");
    }
}

boost::filesystem::path
VxcoreParser::parseUrl(const std::string &spec) {
    std::vector<std::string> parts = StringUtility::split(":", spec, 3);
    if (parts.size() != 3)
        throw Exception("URL", 0, "not enough colons in vxcore URL (expected \"[MEM_ATTRS]:[FILE_ATTRS]:FILENAME\")");

    // Memory protection
    settings_.protDefault = MemoryMap::READ_WRITE_EXECUTE;
    settings_.protOverride = Sawyer::Nothing();
    for (const char *s = parts[0].c_str(); *s; /*void*/) {
        if ('=' == *s) {
            unsigned perms = 0;
            for (++s; *s && strchr("rwx", *s); ++s) {
                switch (*s) {
                    case 'r': perms |= MemoryMap::READABLE; break;
                    case 'w': perms |= MemoryMap::WRITABLE; break;
                    case 'x': perms |= MemoryMap::EXECUTABLE; break;
                }
            }
            settings_.protOverride = perms;
        } else {
            throw Exception("URL", 0, "invalid memory attribute for vxcore URL: \"" + StringUtility::cEscape(s) + "\"");
        }
    }

    // File attributes
    settings_.version = 1;
    if (!parts[1].empty()) {
        boost::regex versionRe("version=(\\d+)");
        boost::smatch matched;
        if (boost::regex_match(parts[1], matched, versionRe)) {
            settings_.version = boost::lexical_cast<unsigned>(matched[1]);
        } else {
            throw Exception("URL", 0, "invalid file attribute for vxcore URL: \"" + StringUtility::cEscape(parts[1]) + "\"");
        }
    }
    if (settings_.version != 1)
        throw Exception("URL", 0, "vxcore version " + boost::lexical_cast<std::string>(settings_.version) + " is not supported");

    return parts[2];
}

void
VxcoreParser::parse(const boost::filesystem::path &fileName, const MemoryMap::Ptr &memory) {
    return parse(fileName, memory, BaseSemantics::RegisterStatePtr(), BaseSemantics::RiscOperatorsPtr());
}

void
VxcoreParser::parse(const boost::filesystem::path &fileName, const BaseSemantics::RegisterStatePtr &registers,
                    const BaseSemantics::RiscOperatorsPtr &ops) {
    return parse(fileName, MemoryMap::Ptr(), registers, ops);
}

void
VxcoreParser::parse(const boost::filesystem::path &fileName, const MemoryMap::Ptr &memory,
                    const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::RiscOperatorsPtr &ops) {
    std::ifstream input(fileName.native().c_str(), std::ios::binary);
    parse(input, memory, registers, ops, fileName.native());
}

void
VxcoreParser::parse(std::istream &input, const MemoryMap::Ptr &memory, const BaseSemantics::RegisterStatePtr &registers,
                    const BaseSemantics::RiscOperatorsPtr &ops, const std::string &inputName) {
    for (size_t segmentIdx = 0; input; ++segmentIdx) {
        size_t headerOffset = input.tellg();
        std::string header = rose_getline(input);
        if (header.empty())
            break;                                      // EOF

        std::string name = inputName + " segment #" + boost::lexical_cast<std::string>(segmentIdx);
        if (!parseMemory(header, input, memory, name, headerOffset) &&
            !parseRegisters(header, input, registers, ops, name, headerOffset)) {
            throw Exception(inputName, input.tellg(), "invalid header: \"" + StringUtility::cEscape(header.substr(0, 30)) + "\"" +
                            (header.size() > 30 ? "..." : ""));
        }
    }
}

bool
VxcoreParser::parseMemory(const std::string &header, std::istream &input, const MemoryMap::Ptr &memory,
                          const std::string &inputName, size_t headerOffset) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Parse the header
    //                         address           size                  protection
    boost::regex headerRe("\\s*([0-9a-fA-F]+)\\s+([0-9a-fA-F]+)(?:\\s+=([-RWX]*))\\s*:\\s*");
    boost::smatch matched;
    if (!boost::regex_match(header, matched, headerRe))
        return false;

    std::string vaStr = matched[1];
    rose_addr_t va = rose_strtoull(vaStr.c_str(), NULL, 16);

    std::string nBytesStr = matched[2];
    rose_addr_t nBytes = rose_strtoull(nBytesStr.c_str(), NULL, 16);

    std::string memProtStr = matched[3];
    unsigned memProt = settings_.protDefault;
    if (!memProtStr.empty()) {
        memProt = 0;
        BOOST_FOREACH (char ch, memProtStr) {
            switch (ch) {
                    case 'R': memProt |= MemoryMap::READABLE; break;
                    case 'W': memProt |= MemoryMap::WRITABLE; break;
                    case 'X': memProt |= MemoryMap::EXECUTABLE; break;
                    case '-': break;
            }
        }
    }
    settings_.protOverride.assignTo(memProt);
        
    // Read the binary data from the input stream
    struct Resources {
        uint8_t *bytes;
        Resources(): bytes(NULL) {}
        ~Resources() { delete[] bytes; }
    } r;
    r.bytes = new uint8_t[nBytes];
    input.read((char*)r.bytes, nBytes);
    size_t nRead = input.gcount();
    if (nRead != nBytes) {
        throw Exception(inputName, headerOffset,
                        (boost::format("short read (expected %1%, got only %2%) at %3%") % nBytes % nRead % headerOffset).str());
    }

    // Place the binary data into the map
    if (memory) {
        AddressInterval where = AddressInterval::baseSize(va, nBytes);
        SAWYER_MESG(debug) <<"vxcore: addresses " <<StringUtility::addrToString(where) <<" at " <<headerOffset <<"\n";
        memory->insert(where, MemoryMap::Segment::anonymousInstance(nBytes, memProt, inputName));
        size_t nCopied = memory->at(va).limit(nBytes).write(r.bytes).size();
        ASSERT_always_require(nCopied == nBytes);
    }

    return true;
}

bool
VxcoreParser::parseRegisters(const std::string &header, std::istream &input, const BaseSemantics::RegisterStatePtr &registers,
                             const BaseSemantics::RiscOperatorsPtr &ops, const std::string &inputName, size_t headerOffset) {
    // Match the register section's header
    //                                      ISA
    boost::regex headerRe("\\s*registers\\s+(\\w+)\\s*");
    boost::smatch matched;
    if (!boost::regex_match(header, matched, headerRe))
        return false;
    isaName_ = matched[1];

    // Parse all the lines in the register section. This section doesn't have a size, but rather ends at either the end of the file or
    // after the line-feed of a line containing only the word "end".
    boost::regex registerRe("\\s*(\\S+)\\s*:\\s*(0x[0-9a-fA-F]+)\\s*");
    boost::regex endRe("\\s*end\\s*");
    while (true) {
        std::string line = rose_getline(input);
        if (line.empty() || boost::regex_match(line, endRe)) {
            return true;
        } else if (!boost::regex_match(line, matched, registerRe)) {
            throw Exception(inputName, headerOffset, "syntax error in register specification: "
                            "\"" + StringUtility::cEscape(line.substr(0, 30)) + "\"" +
                            (line.size() > 30 ? "..." : ""));
        } else if (registers != NULL) {
            ASSERT_not_null(ops);
            // FIXME[Robb Matzke 2020-02-12]: check register name, parse value, and save value in register state.
            mlog[ERROR] <<"saving of register values is not implemented: " <<line <<"\n";
        }
    }

    return true;
}

void
VxcoreParser::unparse(std::ostream &out, const MemoryMap::Ptr &memory, const AddressInterval &memoryLimit,
                      const std::string &outputName) {
    unparse(out, memory, memoryLimit, BaseSemantics::RegisterStatePtr(), BaseSemantics::RiscOperatorsPtr(), outputName);
}

void
VxcoreParser::unparse(std::ostream &out, const InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
                      const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops, const std::string &outputName) {
    unparse(out, MemoryMap::Ptr(), AddressInterval(), registers, ops, outputName);
}

void
VxcoreParser::unparse(std::ostream &out, const MemoryMap::Ptr &memory, const AddressInterval &memoryLimit,
                      const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::RiscOperatorsPtr &ops,
                      const std::string &outputName) {
    if (memory && !memoryLimit.isEmpty()) {
        rose_addr_t va = memoryLimit.least();
        while (const AddressInterval selected = memory->atOrAfter(va).singleSegment().available() & memoryLimit) {
            MemoryMap::ConstNodeIterator inode = memory->at(selected.least()).nodes().begin();
            ASSERT_forbid(inode == memory->nodes().end()); // because of the while loop's condition
            ASSERT_require(inode->key().isContaining(selected));
            const MemoryMap::Segment &segment = inode->value();

            // Header
            out <<StringUtility::addrToString(selected.least()).substr(2)
                <<" " <<StringUtility::addrToString(selected.size()).substr(2)
                <<" =" <<(0 != (segment.accessibility() & MemoryMap::READABLE) ? "R" : "-")
                <<(0 != (segment.accessibility() & MemoryMap::WRITABLE) ? "W" : "-")
                <<(0 != (segment.accessibility() & MemoryMap::EXECUTABLE) ? "X" : "-")
                <<"\n";

            // Data output one buffer-full at a time since the memory map' underlying buffer might not be storing the bytes
            // contiguously, but we need contiguous bytes for std::ostream::write.
            uint8_t buffer[4096];
            AddressInterval remaining = selected;
            while (!remaining.isEmpty()) {
                // What to read
                rose_addr_t bufferVa = remaining.least();
                size_t nBytes = std::min(remaining.size(), (rose_addr_t)sizeof(buffer));
                AddressInterval toRead = AddressInterval::baseSize(bufferVa, nBytes);

                // Read from map, write to std::ostream
                size_t nRead = memory->at(toRead).read(buffer).size();
                size_t offset = out.tellp();
                ASSERT_always_require(nRead == nBytes); // cannot fail since the extent is from mmNode.key
                out.write((const char*)buffer, nBytes);
                if (!out.good())
                    throw Exception(outputName, offset, "write failed");

                // What remains to transfer
                remaining = remaining.split(toRead.greatest()).second;
            }

            if (selected.greatest() == memoryLimit.greatest())
                break;
            va = selected.greatest() + 1;
        }
    }

    if (registers) {
        ASSERT_not_null(ops);
        out <<"registers " <<registers->get_register_dictionary()->get_architecture_name() <<"\n";
        RegisterDictionary::RegisterDescriptors regs = registers->get_register_dictionary()->get_largest_registers();
        RegisterNames registerName(registers->get_register_dictionary());
        BOOST_FOREACH (RegisterDescriptor reg, regs) {
            BaseSemantics::SValuePtr val = registers->peekRegister(reg, ops->undefined_(reg.nBits()), ops.get());
            if (val->is_number())
                out <<(boost::format("%s 0x%x\n") % registerName(reg) % val->get_number());
        }
        out <<"end\n";
    }
}

} // namespace
} // namespace
