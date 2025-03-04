#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/Configuration.h>

#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/Diagnostics.h>
#include <Rose/Yaml.h>

#ifdef ROSE_HAVE_YAMLCPP
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
        "oriented than JSON. The top-level object in the file is named \"rose\", within which all other objects will appear. "
        "This allows a configuration file to contain other data that won't be interpreted by the ROSE library. The following "
        "objects may appear within the \"rose\" object:"

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

FunctionConfiguration&
FunctionConfiguration::name(const std::string &s) {
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
        for (const Path &name: findNamesRecursively(fileName, isFile)) {
            if (baseNameMatches(boost::regex(".*\\.json$"))(name))
                loadFromFile(name);
        }
    } else if (isFile(fileName)) {
#if defined(ROSE_HAVE_YAMLCPP)
        SAWYER_MESG(mlog[TRACE]) <<"loading configuration from " <<fileName <<"\n";
        YAML::Node configFile = YAML::LoadFile(fileName.string());
        if (configFile["config"] && configFile["config"]["exports"]) {
            // This is a CMU/SEI configuration file.
            const YAML::Node &exports = configFile["config"]["exports"];
            for (YAML::const_iterator iter=exports.begin(); iter!=exports.end(); ++iter) {
                std::string functionName = Modules::canonicalFunctionName(iter->first.as<std::string>());
                YAML::Node functionInfo = iter->second;
                if (functionInfo["function"] && functionInfo["function"]["delta"]) {
                    FunctionConfiguration config(functionName);
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
                for (const YAML::Node &function: functions) {
                    Sawyer::Optional<Address> addr;
                    std::string name;
                    if (function["address"])
                        addr = function["address"].as<Address>();
                    if (function["name"])
                        name = Modules::canonicalFunctionName(function["name"].as<std::string>());
                    FunctionConfiguration config(addr, name);
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
                for (const YAML::Node &bblock: bblocks) {
                    if (!bblock["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for basic block configuration record\n";
                        continue;
                    }
                    Address addr = bblock["address"].as<Address>();
                    BasicBlockConfiguration config(addr);
                    if (bblock["comment"])
                        config.comment(bblock["comment"].as<std::string>());
                    if (bblock["final_instruction"])
                        config.finalInstructionVa(bblock["final_instruction"].as<Address>());
                    if (const YAML::Node &successors = bblock["successors"]) {
                        for (const YAML::Node &successor: successors)
                            config.successorVas().insert(successor.as<Address>());
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
                for (const YAML::Node &dblock: dblocks) {
                    if (!dblock["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for data block configuration record\n";
                        continue;
                    }
                    Address addr = dblock["address"].as<Address>();
                    DataBlockConfiguration config(addr);
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
                for (const YAML::Node &detail: addrs) {
                    if (!detail["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for address configuration record\n";
                        continue;
                    }
                    Address addr = detail["address"].as<Address>();
                    AddressConfiguration config(addr);
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
#else
        SAWYER_MESG(mlog[TRACE]) <<"loading configuration from " <<fileName <<"\n";
        Yaml::Node configFile = Yaml::parse(fileName);

        if (configFile["config"] && configFile["config"]["exports"]) {
            // This is a CMU/SEI configuration file.
            Yaml::Node &exports = configFile["config"]["exports"];
            for (const auto &node: exports) {
                const std::string functionName = Modules::canonicalFunctionName(node.first);
                Yaml::Node &functionInfo = node.second;

                // This non-idiomatic "if" condition is because Yaml::Node has no bool operator, so we have to negate a
                // predicate in the negative (i.e., "is not nothing" instead of "is something".
                if (functionInfo["function"] && functionInfo["function"]["delta"]) {
                    FunctionConfiguration config(functionName);
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
            Yaml::Node &functions = configFile["rose"]["functions"];
            if (functions) {
                for (const auto &node: functions) {
                    Yaml::Node &function = node.second;
                    Sawyer::Optional<Address> addr;
                    std::string name;
                    if (function["address"])
                        addr = function["address"].as<Address>();
                    if (function["name"])
                        name = Modules::canonicalFunctionName(function["name"].as<std::string>());
                    FunctionConfiguration config(addr, name);
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
            Yaml::Node &bblocks = configFile["rose"]["bblocks"];
            if (bblocks) {
                for (const auto &node: bblocks) {
                    Yaml::Node &bblock = node.second;
                    if (!bblock["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for basic block configuration record\n";
                        continue;
                    }
                    Address addr = bblock["address"].as<Address>();
                    BasicBlockConfiguration config(addr);
                    if (bblock["comment"])
                        config.comment(bblock["comment"].as<std::string>());
                    if (bblock["final_instruction"])
                        config.finalInstructionVa(bblock["final_instruction"].as<Address>());
                    Yaml::Node &successors = bblock["successors"];
                    if (successors) {
                        for (const auto &sucNode: successors) {
                            Yaml::Node &successor = sucNode.second;
                            config.successorVas().insert(successor.as<Address>());
                        }
                    }
                    if (bblock["source_location"])
                        config.sourceLocation(SourceLocation::parse(bblock["source_location"].as<std::string>()));
                    if (!insertConfiguration(config)) {
                        SAWYER_MESG(mlog[WARN]) <<"multiple configuration records for basic block "
                                                <<StringUtility::addrToString(addr) <<"\n";
                    }
                }
            }
            Yaml::Node &dblocks = configFile["rose"]["dblocks"];
            if (dblocks) {
                for (const auto &node: dblocks) {
                    Yaml::Node &dblock = node.second;
                    if (!dblock["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for data block configuration record\n";
                        continue;
                    }
                    Address addr = dblock["address"].as<Address>();
                    DataBlockConfiguration config(addr);
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
            Yaml::Node &addrs = configFile["rose"]["addresses"];
            if (addrs) {
                for (const auto &node: addrs) {
                    Yaml::Node &detail = node.second;
                    if (!detail["address"]) {
                        SAWYER_MESG(mlog[ERROR]) <<"missing address for address configuration record\n";
                        continue;
                    }
                    Address addr = detail["address"].as<Address>();
                    AddressConfiguration config(addr);
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
            SAWYER_MESG(mlog[ERROR]) <<"not a valid configuration file: " <<fileName <<"\n";
        }
#endif
    }
}

static void
printFunctionConfiguration(std::ostream &out, const FunctionConfiguration &config) {
    if (config.address())
        out <<"    - address:" <<StringUtility::addrToString(*config.address()) <<"\n";
    if (!config.name().empty())
        out <<"      name: " <<StringUtility::yamlEscape(config.name()) <<"\n";
    if (!config.defaultName().empty())
        out <<"      default_name: " <<StringUtility::yamlEscape(config.defaultName()) <<"\n";
    if (!config.comment().empty())
        out <<"      comment: " <<StringUtility::yamlEscape(config.comment()) <<"\n";
    if (config.stackDelta())
        out <<"      stack_delta: " <<*config.stackDelta() <<"\n";
    if (config.mayReturn())
        out <<"      may_return: " <<(*config.mayReturn()?"yes":"no") <<"\n";
    if (!config.sourceLocation().isEmpty())
        out <<"      source_location: " <<StringUtility::yamlEscape(config.sourceLocation().printableName()) <<"\n";
}

static void
printBasicBlockConfiguration(std::ostream &out, const BasicBlockConfiguration &config) {
    out <<"    - address:" <<StringUtility::addrToString(config.address()) <<"\n";
    if (!config.comment().empty())
        out <<"      comment: " <<StringUtility::yamlEscape(config.comment()) <<"\n";
    if (config.finalInstructionVa())
        out <<"      final_instruction: " <<StringUtility::addrToString(*config.finalInstructionVa()) <<"\n";
    if (!config.successorVas().empty()) {
        out <<"      successors:" <<"\n";
        for (Address va: config.successorVas())
            out <<"          - " <<StringUtility::addrToString(va) <<"\n";
    }
    if (!config.sourceLocation().isEmpty())
        out <<"      source_location: " <<StringUtility::yamlEscape(config.sourceLocation().printableName()) <<"\n";
}

static void
printDataBlockConfiguration(std::ostream &out, const DataBlockConfiguration &config) {
    out <<"    - address: " <<StringUtility::addrToString(config.address()) <<"\n";
    if (!config.name().empty())
        out <<"      name: " <<StringUtility::yamlEscape(config.name()) <<"\n";
    if (!config.comment().empty())
        out <<"      comment: " <<StringUtility::yamlEscape(config.comment()) <<"\n";
    if (!config.sourceLocation().isEmpty())
        out <<"      source_location: " <<StringUtility::yamlEscape(config.sourceLocation().printableName()) <<"\n";
}

static void
printAddressConfiguration(std::ostream &out, const AddressConfiguration &config) {
    out <<"    - address: " <<StringUtility::addrToString(config.address()) <<"\n";
    if (!config.name().empty())
        out <<"      name: " <<StringUtility::yamlEscape(config.name()) <<"\n";
    if (!config.comment().empty())
        out <<"      comment: " <<StringUtility::yamlEscape(config.comment()) <<"\n";
    if (!config.sourceLocation().isEmpty())
        out <<"      source_location: " <<StringUtility::yamlEscape(config.sourceLocation().printableName()) <<"\n";
}

void
Configuration::print(std::ostream &out) const {
    if (!functionConfigurationsByAddress_.isEmpty() || !functionConfigurationsByName_.isEmpty()) {
        out <<"functions:\n";
        for (const FunctionConfiguration &config: functionConfigurationsByAddress_.values())
            printFunctionConfiguration(out, config);
        for (const FunctionConfiguration &config: functionConfigurationsByName_.values())
            printFunctionConfiguration(out, config);
    }

    if (!bblockConfigurations_.isEmpty()) {
        out <<"bblocks:\n";
        for (const BasicBlockConfiguration &config: bblockConfigurations_.values())
            printBasicBlockConfiguration(out, config);
    }

    if (!dblockConfigurations_.isEmpty()) {
        out <<"dblocks:\n";
        for (const DataBlockConfiguration &config: dblockConfigurations_.values())
            printDataBlockConfiguration(out, config);
    }
    if (!addressConfigurations_.isEmpty()) {
        out <<"addresses:\n";
        for (const AddressConfiguration &config: addressConfigurations_.values())
            printAddressConfiguration(out, config);
    }
}

BasicBlockConfiguration&
Configuration::insertMaybeBasicBlock(Address va) {
    return bblockConfigurations_.insertMaybe(va, BasicBlockConfiguration(va));
}

DataBlockConfiguration&
Configuration::insertMaybeDataBlock(Address va) {
    return dblockConfigurations_.insertMaybe(va, DataBlockConfiguration(va));
}

FunctionConfiguration&
Configuration::insertMaybeFunction(Address va, const std::string &name) {
    return functionConfigurationsByAddress_.insertMaybe(va, FunctionConfiguration(va, name));
}

FunctionConfiguration&
Configuration::insertMaybeFunction(const std::string &name) {
    return functionConfigurationsByName_.insertMaybe(name, FunctionConfiguration(name));
}

AddressConfiguration&
Configuration::insertMaybeAddress(Address va) {
    return addressConfigurations_.insertMaybe(va, AddressConfiguration(va));
}

const BasicBlockConfiguration&
Configuration::basicBlock(Address va) const {
    return bblockConfigurations_.getOrDefault(va);
}

const DataBlockConfiguration&
Configuration::dataBlock(Address va) const {
    return dblockConfigurations_.getOrDefault(va);
}

const AddressConfiguration&
Configuration::address(Address va) const {
    return addressConfigurations_.getOrDefault(va);
}

const FunctionConfiguration&
Configuration::function(Address va) const {
    return functionConfigurationsByAddress_.getOrDefault(va);
}

const FunctionConfiguration&
Configuration::function(const std::string &name) const {
    return functionConfigurationsByName_.getOrDefault(name);
}

bool
Configuration::insertConfiguration(const BasicBlockConfiguration &config) {
    bool retval = !bblockConfigurations_.exists(config.address());
    bblockConfigurations_.insert(config.address(), config);
    return retval;
}

bool
Configuration::insertConfiguration(const DataBlockConfiguration &config) {
    bool retval = !dblockConfigurations_.exists(config.address());
    dblockConfigurations_.insert(config.address(), config);
    return retval;
}

bool
Configuration::insertConfiguration(const FunctionConfiguration &config) {
    if (config.address()) {
        bool retval = !functionConfigurationsByAddress_.exists(*config.address());
        functionConfigurationsByAddress_.insert(*config.address(), config);
        return retval;
    } else {
        bool retval = !functionConfigurationsByName_.exists(config.name());
        functionConfigurationsByName_.insert(config.name(), config);
        return retval;
    }
}

bool
Configuration::insertConfiguration(const AddressConfiguration &config) {
    bool retval = !addressConfigurations_.exists(config.address());
    addressConfigurations_.insert(config.address(), config);
    return retval;
}

std::string
Configuration::basicBlockComment(Address bblockVa) const {
    return bblockConfigurations_.getOptional(bblockVa).orDefault().comment();
}

Sawyer::Optional<Address>
Configuration::basicBlockFinalInstructionVa(Address bblockVa) const {
    return bblockConfigurations_.getOptional(bblockVa).orDefault().finalInstructionVa();
}

std::set<Address>
Configuration::basicBlockSuccessorVas(Address bblockVa) const {
    static const BasicBlockConfiguration emptyConfiguration;
    return bblockConfigurations_.getOptional(bblockVa).orElse(emptyConfiguration).successorVas();
}

std::string
Configuration::dataBlockName(Address dblockVa) const {
    return dblockConfigurations_.getOptional(dblockVa).orDefault().name();
}

std::string
Configuration::dataBlockComment(Address dblockVa) const {
    return dblockConfigurations_.getOptional(dblockVa).orDefault().comment();
}

std::string
Configuration::functionName(Address functionVa) const {
    return functionConfigurationsByAddress_.getOptional(functionVa).orDefault().name();
}

std::string
Configuration::functionDefaultName(Address functionVa) const {
    return functionConfigurationsByAddress_.getOptional(functionVa).orDefault().defaultName();
}

std::string
Configuration::functionComment(Address functionVa) const {
    return functionConfigurationsByAddress_.getOptional(functionVa).orDefault().comment();
}

std::string
Configuration::functionComment(const std::string &functionName) const {
    return functionConfigurationsByName_.getOptional(functionName).orDefault().comment();
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
Configuration::functionStackDelta(Address functionVa) const {
    return functionConfigurationsByAddress_.getOptional(functionVa).orDefault().stackDelta();
}

Sawyer::Optional<int64_t>
Configuration::functionStackDelta(const std::string &functionName) const {
    return functionConfigurationsByName_.getOptional(functionName).orDefault().stackDelta();
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
Configuration::functionMayReturn(Address functionVa) const {
    return functionConfigurationsByAddress_.getOptional(functionVa).orDefault().mayReturn();
}

Sawyer::Optional<bool>
Configuration::functionMayReturn(const std::string &functionName) const {
    return functionConfigurationsByName_.getOptional(functionName).orDefault().mayReturn();
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
Configuration::addressComment(Address va) const {
    return addressConfigurations_.getOptional(va).orDefault().comment();
}

std::string
Configuration::comment(Address va) const {
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

#endif
