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

std::ostream& operator<<(std::ostream &out, const rose::BinaryAnalysis::Partitioner2::Configuration &c) {
    c.print(out);
    return out;
}

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

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
                        BOOST_FOREACH (const YAML::Node &successor, successors) {
#if 1 // DEBUGGING [Robb P. Matzke 2015-01-23]
                            std::cerr <<"ROBB: inserting successor " <<StringUtility::addrToString(successor.as<rose_addr_t>()) <<"\n";
#endif
                            config.successorVas().insert(successor.as<rose_addr_t>());
                        }
                    }
                    if (!insertConfiguration(config)) {
                        SAWYER_MESG(mlog[WARN]) <<"multiple configuration records for basic block "
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
    out <<YAML::EndMap;
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

    emitter <<YAML::EndMap;                             // end of "rose" map
    emitter <<YAML::EndMap;                             // end of document containing "rose"

    out <<emitter.c_str();
#endif
}

BasicBlockConfig&
Configuration::insertMaybeBasicBlock(rose_addr_t va) {
    return bblockConfigs_.insertMaybe(va, BasicBlockConfig(va));
}

FunctionConfig&
Configuration::insertMaybeFunction(rose_addr_t va, const std::string &name) {
    return functionConfigsByAddress_.insertMaybe(va, FunctionConfig(va, name));
}

FunctionConfig&
Configuration::insertMaybeFunction(const std::string &name) {
    return functionConfigsByName_.insertMaybe(name, FunctionConfig(name));
}

bool
Configuration::insertConfiguration(const BasicBlockConfig &config) {
    bool retval = !bblockConfigs_.exists(config.address());
    bblockConfigs_.insert(config.address(), config);
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

} // namespace
} // namespace
} // namespace
