#include <sage3basic.h>
#include "rosePublicConfig.h"

#include <boost/foreach.hpp>
#include <Diagnostics.h>
#include <Partitioner2/Config.h>
#include <Partitioner2/Modules.h>
#include <Partitioner2/Utility.h>

#ifdef ROSE_HAVE_LIBYAML
#include <yaml-cpp/yaml.h>
#endif

std::ostream& operator<<(std::ostream &out, const Rose::BinaryAnalysis::Partitioner2::Configuration &c) {
    c.print(out);
    return out;
}

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

// class method
std::string
Configuration::fileFormatDoc() {
    return "The configuration file is either YAML or JSON format. This documentation uses YAML syntax since it is more user "
        "oriented than JSON.  Configuration files can only be parsed if ROSE was configured with YAML support."
        "The top-level object in the file is named \"rose\", within which all other objects will appear. This allows a "
        "configuration file to contain other data that won't be interpreted by the ROSE library. The following objects "
        "may appear within the \"rose\" object:"

        "@named{functions}{The \"functions\" object holds a list of settings, one per function. For each function, the "
        "following settings are recognized (others are ignored):"
        "@named{address}{The virtual address where the function starts.}"
        "@named{name}{The name of the function.}"
        "@named{default_name}{A default name to give the function if a name is not obtained from any other source.}"
        "@named{comment}{A comment to associated with the whole function.}"
        "@named{stack_delta}{A signed integer that describes the net change to the stack pointer caused by this function.}"
        "@named{may_return}{A boolean value (\"true\", \"yes\", \"t\", \"y\", \"1\" for true; other for false) that indicates "
        "whether this function could return. Functions like \"abort\" would have a false value since they can never return.}"
        "@named{source_location}{A string describing the file, line number, and column where this function is defined in the "
        "source code. The line and column are each introduced by a colon. The column or both the line and column are optional. "
        "The bytes before the line and column, if present, are a file name. If the file name is surrounded by double quotes then "
        "it is parsed like a C string literal, otherwise the name is used as-is.}"
        "}"

        "@named{bblocks}{The \"bblocks\" object holds a list of settings, one per basic block. Each basic block has the "
        "following settings:"
        "@named{address}{The required virtual address for the start of the basic block.}"
        "@named{comment}{An optional comment for the basic block.}"
        "@named{final_instruction}{The address of the final instruction. If the basic block reaches this address then that "
        "instruction is the last instruction of the basic block even if such an instruction would not normally terminate a "
        "basic block.}"
        "@named{source_location}{A string describing the file, line number, and column where this basic block is defined in the "
        "source code. The line and column are each introduced by a colon. The column or both the line and column are optional. "
        "The bytes before the line and column, if present, are a file name. If the file name is surrounded by double quotes then "
        "it is parsed like a C string literal, otherwise the name is used as-is.}"
        "}"

        "@named{dblocks}{The \"dblocks\" object holds a list of settings, one per data block. Each data block has the following "
        "settings:"
        "@named{address}{The required virtual address for the start of the data block.}"
        "@named{name}{The name to use for the data block.}"
        "@named{comment}{A comment to associate with the data block.}"
        "@named{source_location}{A string describing the file, line number, and column where this data block is defined in the "
        "source code. The line and column are each introduced by a colon. The column or both the line and column are optional. "
        "The bytes before the line and column, if present, are a file name. If the file name is surrounded by double quotes then "
        "it is parsed like a C string literal, otherwise the name is used as-is.}"
        "}"

        "@named{addresses}{The \"addresses\" object holds a list of settings, one per address. These settings are intended for "
        "addresses that aren't associated with the start of a function, basic block, or data block. Each address has the following "
        "settings:"
        "@named{address}{The required address to be configured.}"
        "@named{name}{A name associated with this address.}"
        "@named{comment}{A comment to associate with the address.}"
        "@named{source_location}{A string describing the file, line number, and column where this address is defined in the "
        "source code. The line and column are each introduced by a colon. The column or both the line and column are optional. "
        "The bytes before the line and column, if present, are a file name. If the file name is surrounded by double quotes then "
        "it is parsed like a C string literal, otherwise the name is used as-is.}"
        "}"

        "In addition to the top-level \"rose\" object, a schema defined by the Softawre Engineering Institute is also recognized. "
        "That schema consists of a top-level \"config\" object with a child \"exports\" object that contains a list of \"function\" "
        "objects.  Each \"function\" object has a single field named \"delta\" which is a signed integer that describes how the "
        "function adjusts the stack pointer, not counting the return address that gets popped by the RET instruction. Function "
        "names of the form \"@v{lib}:@v{func}\" are translated to the ROSE names \"@v{func}@@@v{lib}\".";
}

FunctionConfig&
FunctionConfig::name(const std::string &s) {
    if (!address()) {
        // Can't change the name since the name is being used as the lookup key.
        throw std::runtime_error("function config name cannot be changed if it has no address");
    }
    name_ = s;
    return *this;
}

void
Configuration::loadFromFile(const FileSystem::Path &fileName) {
    using namespace FileSystem;
    if (isDirectory(fileName)) {
        BOOST_FOREACH (const Path &name, findNamesRecursively(fileName, isFile)) {
            if (baseNameMatches(boost::regex(".*\\.json$"))(name))
                loadFromFile(name);
        }
    } else if (isFile(fileName)) {
#ifndef ROSE_HAVE_LIBYAML
        throw std::runtime_error("cannot open \"" + fileName.string() + "\": no YAML support" +
                                 " (use --with-yaml when configuring ROSE)");
#else
        SAWYER_MESG(mlog[TRACE]) <<"loading configuration from " <<fileName <<"\n";
        YAML::Node configFile = YAML::LoadFile(fileName.string());
        if (configFile["config"] && configFile["config"]["exports"]) {
            // This is a CMU/SEI configuration file.
            const YAML::Node &exports = configFile["config"]["exports"];
            for (YAML::const_iterator iter=exports.begin(); iter!=exports.end(); ++iter) {
                std::string functionName = Modules::canonicalFunctionName(iter->first.as<std::string>());
                YAML::Node functionInfo = iter->second;
                if (functionInfo["function"] && functionInfo["function"]["delta"]) {
                    FunctionConfig config(functionName);
                    static const int wordSize = 4;      // these files don't include popping the return address
                    int delta = functionInfo["function"]["delta"].as<int>() + wordSize;
                    config.stackDelta(delta);
                    if (!insertConfiguration(config)) {
                        SAWYER_MESG(mlog[WARN]) <<"multiple configuration records for function \""
                                                <<StringUtility::cEscape(functionName) <<"\"\n";
                    }
                }
            }
        } else if (configFile["rose"]) {
            // This is a ROSE configuration file.
            if (YAML::Node functions = configFile["rose"]["functions"]) {
                BOOST_FOREACH (const YAML::Node &function, functions) {
                    Sawyer::Optional<rose_addr_t> addr;
                    std::string name;
                    if (function["address"])
                        addr = function["address"].as<rose_addr_t>();
                    if (function["name"])
                        name = Modules::canonicalFunctionName(function["name"].as<std::string>());
                    FunctionConfig config(addr, name);
                    if (function["default_name"])
                        config.defaultName(Modules::canonicalFunctionName(function["default_name"].as<std::string>()));
                    if (function["comment"])
                        config.comment(function["comment"].as<std::string>());
                    if (function["stack_delta"])
                        config.stackDelta(function["stack_delta"].as<int64_t>());
                    if (function["may_return"]) {
                        std::string val = function["may_return"].as<std::string>();
                        if (val == "true" || val == "yes" || val == "t" || val == "y" || val == "1") {
                            config.stackDelta(true);
                        } else {
                            config.stackDelta(false);
                        }
                    }
                    if (function["source_location"])
                        config.sourceLocation(SourceLocation::parse(function["source_location"].as<std::string>()));
                    if (!insertConfiguration(config)) {
                        SAWYER_MESG(mlog[WARN]) <<"multiple configuration records for function "
                                                <<(addr?StringUtility::addrToString(*addr)+" ":std::string())
                                                <<"\"" <<StringUtility::cEscape(name) <<"\"\n";
                    }
                }
            }
            if (YAML::Node bblocks = configFile["rose"]["bblocks"]) {
                BOOST_FOREACH (const YAML::Node &bblock, bblocks) {
                    if (!bblock["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for basic block configuration record\n";
                        continue;
                    }
                    rose_addr_t addr = bblock["address"].as<rose_addr_t>();
                    BasicBlockConfig config(addr);
                    if (bblock["comment"])
                        config.comment(bblock["comment"].as<std::string>());
                    if (bblock["final_instruction"])
                        config.finalInstructionVa(bblock["final_instruction"].as<rose_addr_t>());
                    if (const YAML::Node &successors = bblock["successors"]) {
                        BOOST_FOREACH (const YAML::Node &successor, successors)
                            config.successorVas().insert(successor.as<rose_addr_t>());
                    }
                    if (bblock["source_location"])
                        config.sourceLocation(SourceLocation::parse(bblock["source_location"].as<std::string>()));
                    if (!insertConfiguration(config)) {
                        SAWYER_MESG(mlog[WARN]) <<"multiple configuration records for basic block "
                                                <<StringUtility::addrToString(addr) <<"\n";
                    }
                }
            }
            if (YAML::Node dblocks = configFile["rose"]["dblocks"]) {
                BOOST_FOREACH (const YAML::Node &dblock, dblocks) {
                    if (!dblock["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for data block configuration record\n";
                        continue;
                    }
                    rose_addr_t addr = dblock["address"].as<rose_addr_t>();
                    DataBlockConfig config(addr);
                    if (dblock["name"])
                        config.name(dblock["name"].as<std::string>());
                    if (dblock["comment"])
                        config.comment(dblock["comment"].as<std::string>());
                    if (dblock["source_location"])
                        config.sourceLocation(SourceLocation::parse(dblock["source_location"].as<std::string>()));
                    if (!insertConfiguration(config)) {
                        SAWYER_MESG(mlog[WARN]) <<"multiple configuration records for data block "
                                                <<StringUtility::addrToString(addr) <<"\n";
                    }
                }
            }
            if (YAML::Node addrs = configFile["rose"]["addresses"]) {
                BOOST_FOREACH (const YAML::Node &detail, addrs) {
                    if (!detail["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for address configuration record\n";
                        continue;
                    }
                    rose_addr_t addr = detail["address"].as<rose_addr_t>();
                    AddressConfig config(addr);
                    if (detail["name"])
                        config.name(detail["name"].as<std::string>());
                    if (detail["comment"])
                        config.comment(detail["comment"].as<std::string>());
                    if (detail["source_location"])
                        config.sourceLocation(SourceLocation::parse(detail["source_location"].as<std::string>()));
                    if (!insertConfiguration(config)) {
                        SAWYER_MESG(mlog[WARN]) <<"multiple configuration records for address "
                                                <<StringUtility::addrToString(addr) <<"\n";
                    }
                }
            }

        } else {
            SAWYER_MESG(mlog[ERROR]) <<"not a valid configuration file: \"" <<StringUtility::cEscape(fileName.string()) <<"\"\n";
        }
#endif
    }
}

#ifdef ROSE_HAVE_LIBYAML
static void
printFunctionConfig(YAML::Emitter &out, const FunctionConfig &config) {
    out <<YAML::BeginMap;
    if (config.address())
        out <<YAML::Key <<"address" <<YAML::Value <<StringUtility::addrToString(*config.address());
    if (!config.name().empty())
        out <<YAML::Key <<"name" <<YAML::Value <<config.name();
    if (!config.defaultName().empty())
        out <<YAML::Key <<"default_name" <<YAML::Value <<config.defaultName();
    if (!config.comment().empty())
        out <<YAML::Key <<"comment" <<YAML::Value <<config.comment();
    if (config.stackDelta())
        out <<YAML::Key <<"stack_delta" <<YAML::Value <<*config.stackDelta();
    if (config.mayReturn())
        out <<YAML::Key <<"may_return" <<YAML::Value <<(*config.mayReturn()?"yes":"no");
    if (!config.sourceLocation().isEmpty())
        out <<YAML::Key <<"source_location" <<YAML::Value <<config.sourceLocation().printableName();
    out <<YAML::EndMap;
}

static void
printBasicBlockConfig(YAML::Emitter &out, const BasicBlockConfig &config) {
    out <<YAML::BeginMap;
    out <<YAML::Key <<"address" <<YAML::Value <<StringUtility::addrToString(config.address());
    if (!config.comment().empty())
        out <<YAML::Key <<"comment" <<YAML::Value <<config.comment();
    if (config.finalInstructionVa())
        out <<YAML::Key <<"final_instruction" <<YAML::Value <<StringUtility::addrToString(*config.finalInstructionVa());
    if (!config.successorVas().empty()) {
        out <<YAML::Key <<"successors" <<YAML::Value <<YAML::BeginSeq;
        BOOST_FOREACH (rose_addr_t va, config.successorVas())
            out <<StringUtility::addrToString(va);
        out <<YAML::EndSeq;
    }
    if (!config.sourceLocation().isEmpty())
        out <<YAML::Key <<"source_location" <<YAML::Value <<config.sourceLocation().printableName();
    out <<YAML::EndMap;
}

static void
printDataBlockConfig(YAML::Emitter &out, const DataBlockConfig &config) {
    out <<YAML::BeginMap;
    out <<YAML::Key <<"address" <<YAML::Value <<StringUtility::addrToString(config.address());
    if (!config.name().empty())
        out <<YAML::Key <<"name" <<YAML::Value <<config.name();
    if (!config.comment().empty())
        out <<YAML::Key <<"comment" <<YAML::Value <<config.comment();
    if (!config.sourceLocation().isEmpty())
        out <<YAML::Key <<"source_location" <<YAML::Value <<config.sourceLocation().printableName();
    out <<YAML::EndMap;
}

static void
printAddressConfig(YAML::Emitter &out, const AddressConfig &config) {
    out <<YAML::BeginMap;
    out <<YAML::Key <<"address" <<YAML::Value <<StringUtility::addrToString(config.address());
    if (!config.name().empty())
        out <<YAML::Key <<"name" <<YAML::Value <<config.name();
    if (!config.comment().empty())
        out <<YAML::Key <<"comment" <<YAML::Value <<config.comment();
    if (!config.sourceLocation().isEmpty())
        out <<YAML::Key <<"source_location" <<YAML::Value <<config.sourceLocation().printableName();
    out<<YAML::EndMap;
}
#endif

void
Configuration::print(std::ostream &out) const {
#ifndef ROSE_HAVE_LIBYAML
    throw std::runtime_error("cannot produce YAML output: no YAML support (use --with-yaml when configuring ROSE)");
#else
    YAML::Emitter emitter;
    emitter <<YAML::BeginMap;
    emitter <<YAML::Key <<"rose" <<YAML::Value;
    emitter <<YAML::BeginMap;
    
    if (!functionConfigsByAddress_.isEmpty() || !functionConfigsByName_.isEmpty()) {
        emitter <<YAML::Key <<"functions" <<YAML::Value;
        emitter <<YAML::BeginSeq;
        BOOST_FOREACH (const FunctionConfig &config, functionConfigsByAddress_.values())
            printFunctionConfig(emitter, config);
        BOOST_FOREACH (const FunctionConfig &config, functionConfigsByName_.values())
            printFunctionConfig(emitter, config);
        emitter <<YAML::EndSeq;
    }

    if (!bblockConfigs_.isEmpty()) {
        emitter <<YAML::Key <<"bblocks" <<YAML::Value;
        emitter <<YAML::BeginSeq;
        BOOST_FOREACH (const BasicBlockConfig &config, bblockConfigs_.values())
            printBasicBlockConfig(emitter, config);
        emitter <<YAML::EndSeq;
    }

    if (!dblockConfigs_.isEmpty()) {
        emitter <<YAML::Key <<"dblocks" <<YAML::Value;
        emitter <<YAML::BeginSeq;
        BOOST_FOREACH (const DataBlockConfig &config, dblockConfigs_.values())
            printDataBlockConfig(emitter, config);
        emitter <<YAML::EndSeq;
    }
    if (!addressConfigs_.isEmpty()) {
        emitter <<YAML::Key <<"addresses" <<YAML::Value;
        emitter <<YAML::BeginSeq;
        BOOST_FOREACH (const AddressConfig &config, addressConfigs_.values())
            printAddressConfig(emitter, config);
        emitter <<YAML::EndSeq;
    }

    emitter <<YAML::EndMap;                             // end of "rose" map
    emitter <<YAML::EndMap;                             // end of document containing "rose"

    out <<emitter.c_str();
#endif
}

BasicBlockConfig&
Configuration::insertMaybeBasicBlock(rose_addr_t va) {
    return bblockConfigs_.insertMaybe(va, BasicBlockConfig(va));
}

DataBlockConfig&
Configuration::insertMaybeDataBlock(rose_addr_t va) {
    return dblockConfigs_.insertMaybe(va, DataBlockConfig(va));
}

FunctionConfig&
Configuration::insertMaybeFunction(rose_addr_t va, const std::string &name) {
    return functionConfigsByAddress_.insertMaybe(va, FunctionConfig(va, name));
}

FunctionConfig&
Configuration::insertMaybeFunction(const std::string &name) {
    return functionConfigsByName_.insertMaybe(name, FunctionConfig(name));
}

AddressConfig&
Configuration::insertMaybeAddress(rose_addr_t va) {
    return addressConfigs_.insertMaybe(va, AddressConfig(va));
}

const BasicBlockConfig&
Configuration::basicBlock(rose_addr_t va) const {
    return bblockConfigs_.getOrDefault(va);
}

const DataBlockConfig&
Configuration::dataBlock(rose_addr_t va) const {
    return dblockConfigs_.getOrDefault(va);
}

const AddressConfig&
Configuration::address(rose_addr_t va) const {
    return addressConfigs_.getOrDefault(va);
}

const FunctionConfig&
Configuration::function(rose_addr_t va) const {
    return functionConfigsByAddress_.getOrDefault(va);
}

const FunctionConfig&
Configuration::function(const std::string &name) const {
    return functionConfigsByName_.getOrDefault(name);
}

bool
Configuration::insertConfiguration(const BasicBlockConfig &config) {
    bool retval = !bblockConfigs_.exists(config.address());
    bblockConfigs_.insert(config.address(), config);
    return retval;
}

bool
Configuration::insertConfiguration(const DataBlockConfig &config) {
    bool retval = !dblockConfigs_.exists(config.address());
    dblockConfigs_.insert(config.address(), config);
    return retval;
}

bool
Configuration::insertConfiguration(const FunctionConfig &config) {
    if (config.address()) {
        bool retval = !functionConfigsByAddress_.exists(*config.address());
        functionConfigsByAddress_.insert(*config.address(), config);
        return retval;
    } else {
        bool retval = !functionConfigsByName_.exists(config.name());
        functionConfigsByName_.insert(config.name(), config);
        return retval;
    }
}

bool
Configuration::insertConfiguration(const AddressConfig &config) {
    bool retval = !addressConfigs_.exists(config.address());
    addressConfigs_.insert(config.address(), config);
    return retval;
}

std::string
Configuration::basicBlockComment(rose_addr_t bblockVa) const {
    return bblockConfigs_.getOptional(bblockVa).orDefault().comment();
}

Sawyer::Optional<rose_addr_t>
Configuration::basicBlockFinalInstructionVa(rose_addr_t bblockVa) const {
    return bblockConfigs_.getOptional(bblockVa).orDefault().finalInstructionVa();
}

std::set<rose_addr_t>
Configuration::basicBlockSuccessorVas(rose_addr_t bblockVa) const {
    static const BasicBlockConfig emptyConfig;
    return bblockConfigs_.getOptional(bblockVa).orElse(emptyConfig).successorVas();
}

std::string
Configuration::dataBlockName(rose_addr_t dblockVa) const {
    return dblockConfigs_.getOptional(dblockVa).orDefault().name();
}

std::string
Configuration::dataBlockComment(rose_addr_t dblockVa) const {
    return dblockConfigs_.getOptional(dblockVa).orDefault().comment();
}

std::string
Configuration::functionName(rose_addr_t functionVa) const {
    return functionConfigsByAddress_.getOptional(functionVa).orDefault().name();
}

std::string
Configuration::functionDefaultName(rose_addr_t functionVa) const {
    return functionConfigsByAddress_.getOptional(functionVa).orDefault().defaultName();
}

std::string
Configuration::functionComment(rose_addr_t functionVa) const {
    return functionConfigsByAddress_.getOptional(functionVa).orDefault().comment();
}

std::string
Configuration::functionComment(const std::string &functionName) const {
    return functionConfigsByName_.getOptional(functionName).orDefault().comment();
}

std::string
Configuration::functionComment(const Function::Ptr &function) const {
    if (!function)
        return "";
    std::string s = functionComment(function->address());
    if (s.empty())
        s = functionComment(function->name());
    return s;
}

Sawyer::Optional<int64_t>
Configuration::functionStackDelta(rose_addr_t functionVa) const {
    return functionConfigsByAddress_.getOptional(functionVa).orDefault().stackDelta();
}

Sawyer::Optional<int64_t>
Configuration::functionStackDelta(const std::string &functionName) const {
    return functionConfigsByName_.getOptional(functionName).orDefault().stackDelta();
}

Sawyer::Optional<int64_t>
Configuration::functionStackDelta(const Function::Ptr &function) const {
    if (!function)
        return Sawyer::Nothing();
    if (Sawyer::Optional<int64_t> retval = functionStackDelta(function->address()))
        return retval;
    return functionStackDelta(function->name());
}

Sawyer::Optional<bool>
Configuration::functionMayReturn(rose_addr_t functionVa) const {
    return functionConfigsByAddress_.getOptional(functionVa).orDefault().mayReturn();
}

Sawyer::Optional<bool>
Configuration::functionMayReturn(const std::string &functionName) const {
    return functionConfigsByName_.getOptional(functionName).orDefault().mayReturn();
}

Sawyer::Optional<bool>
Configuration::functionMayReturn(const Function::Ptr &function) const {
    if (!function)
        return Sawyer::Nothing();
    if (Sawyer::Optional<bool> retval = functionMayReturn(function->address()))
        return retval;
    return functionMayReturn(function->name());
}

std::string
Configuration::addressComment(rose_addr_t va) const {
    return addressConfigs_.getOptional(va).orDefault().comment();
}

std::string
Configuration::comment(rose_addr_t va) const {
    std::string s = addressComment(va);
    if (!s.empty())
        return s;
    s = basicBlockComment(va);
    if (!s.empty())
        return s;
    s = dataBlockComment(va);
    if (!s.empty())
        return s;
    return functionComment(va);
}

} // namespace
} // namespace
} // namespace
