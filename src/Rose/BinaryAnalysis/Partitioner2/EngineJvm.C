#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>                                 // needed for `::CommandlineProcessing`
#include <Rose/BinaryAnalysis/Partitioner2/EngineJvm.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/ByteCode/Jvm.h>
#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Configuration.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>

#include <SgAsmBlock.h>
#include <SgAsmGenericFile.h>
#include <SgAsmGenericFileList.h>
#include <SgAsmGenericFormat.h>
#include <SgAsmGenericHeader.h>
#include <SgAsmGenericHeaderList.h>
#include <SgAsmInstructionList.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmInterpretation.h>
#include <SgAsmInterpretationList.h>
#include <SgAsmJvmConstantPool.h>
#include <SgAsmJvmConstantPoolEntry.h>
#include <SgAsmJvmFileHeader.h>
#include <SgAsmJvmInstruction.h>
#include <SgAsmJvmMethod.h>

#include <Cxx_GrammarDowncast.h>

#include <Sawyer/FileSystem.h>
#include <boost/filesystem.hpp>

constexpr bool DEBUG_WITH_DUMP = false;

using namespace Rose::Diagnostics;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using opcode = Rose::BinaryAnalysis::JvmInstructionKind;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

EngineJvm::EngineJvm(const Settings &settings)
    : Engine("JVM", settings), nextFunctionVa_{static_cast<rose_addr_t>(-1)} {
}

EngineJvm::~EngineJvm() {}

EngineJvm::Ptr
EngineJvm::instance(const Settings &settings) {
    auto engine = Ptr(new EngineJvm(settings));

    // Turn off all post function analysis regardless of what user requested
    engine->settings().partitioner.doingPostAnalysis = false;
    engine->settings().partitioner.doingPostFunctionNoop = false;
    engine->settings().partitioner.doingPostFunctionMayReturn = false;
    engine->settings().partitioner.doingPostFunctionStackDelta = false;
    engine->settings().partitioner.doingPostCallingConvention = false;

    return engine;
}

EngineJvm::Ptr
EngineJvm::factory() {
    return Ptr(new EngineJvm(Settings()));
}

bool
EngineJvm::matchFactory(const std::vector<std::string> &specimen) const {
    // All files in the specimen must be JVM related (.class or .jar)
    for (auto file: specimen) {
        if (!CommandlineProcessing::isJavaJvmFile(file)) {
            return false;
        }
    }
    // Use another engine type if there is no specimen
    return specimen.size()>0;
}

Engine::Ptr
EngineJvm::instanceFromFactory(const Settings &settings) {
    ASSERT_require(isFactory());
    return instance(settings);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Top-level, do everything functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
EngineJvm::frontend(const std::vector<std::string> &args, const std::string &purpose, const std::string &description) {
    try {
        std::vector<std::string> specimenNames = parseCommandLine(args, purpose, description).unreachedArgs();
        if (specimenNames.empty())
            throw std::runtime_error("no binary specimen specified; see --help");
        return buildAst(specimenNames);
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::pair<std::string, std::string>
EngineJvm::specimenNameDocumentation() {
    return {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        "JVM specimens",
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        "The Rose::BinaryAnalysis::Partitioner2::EngineJvm class recognizes the following command-line positional arguments "
        "for specifying Java Virtual Machine specimens:"

        "@bullet{Any positional argument which looks like a file name having a \".class\" extension.}"
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner high-level functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary container parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
EngineJvm::isNonContainer(const std::string &name) {
  if (isRbaFile(name)) { // ROSE Binary Analysis file
      return true;
  }
  return false;
}

bool
EngineJvm::areContainersParsed() const {
    return interpretation() != nullptr;
}

SgAsmInterpretation*
EngineJvm::parseContainers(const std::vector<std::string> &fileNames) {
    try {
        interpretation(nullptr);
        memoryMap(MemoryMap::Ptr());
        checkSettings();

        // Prune away things we recognize as not being Java files.
        std::vector<boost::filesystem::path> javaFiles;
        for (const std::string &fileName: fileNames) {
          //TODO: jar files
            if (ModulesJvm::isJavaClassFile(fileName)) {
                javaFiles.push_back(fileName);
            }
        }

        // Process through ROSE's frontend()
        if (!javaFiles.empty()) {
            SgProject *project = roseFrontendReplacement(javaFiles);
            ASSERT_not_null(project);                       // an exception should have been thrown

            std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
            if (interps.empty())
                throw std::runtime_error("a binary specimen container must have at least one SgAsmInterpretation");
            interpretation(interps.back());    // windows PE is always after DOS
            ASSERT_require(areContainersParsed());
        }
        else {
          mlog[WARN] << "No Java class files in specimen\n";
        }

        ASSERT_require(!areSpecimensLoaded());
        return interpretation();
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

boost::filesystem::path
EngineJvm::pathToClass(const std::string &className) {
    boost::filesystem::path classPath{boost::filesystem::current_path().string() + "/" + className + ".class"};
    if (!boost::filesystem::exists(classPath)) {
        if (!ByteCode::JvmContainer::isJvmSystemReserved(className)) {
            mlog[WARN] << "path to class " << className + ".class" << " does not exist\n";
        }
    }
    return classPath;
}

// Load class and super classes
rose_addr_t
EngineJvm::loadClass(uint16_t classIndex, SgAsmJvmConstantPool* pool, SgAsmGenericFileList* fileList, rose_addr_t baseVa) {
    std::string superName{};
    std::string className{ByteCode::JvmClass::name(classIndex, pool)};

    // Don't load classes from java/lang/... or bootstrap_methods
    if (ByteCode::JvmContainer::isJvmSystemReserved(className)) {
        return baseVa;
    }

    auto path = pathToClass(className);
    if (boost::filesystem::exists(path)) {
        baseVa = loadClassFile(path, fileList, baseVa);
    }

    return baseVa;
}

rose_addr_t
EngineJvm::loadClassFile(boost::filesystem::path path, SgAsmGenericFileList* fileList, rose_addr_t baseVa) {
    if (!boost::filesystem::exists(path)) {
        return baseVa;
    }

    // Make sure the class has not already been processed
    if (classes_.find(path.stem().string()) != classes_.end()) {
        return baseVa;
    }

    std::string fileName = path.string();
    SAWYER_MESG(mlog[TRACE]) << "loading and parsing " << fileName <<"\n";

    auto file = new SgAsmGenericFile{};
    file->parse(fileName); /* this loads file into memory, does no reading of file */

    auto jfh = new SgAsmJvmFileHeader(file);
    jfh->set_baseVa(baseVa);

    // Check AST
    ASSERT_require(jfh == file->get_header(SgAsmGenericFile::FAMILY_JVM));
    ASSERT_require(jfh->get_parent() == file);

    jfh->parse();

    auto pool = jfh->get_constant_pool();
    std::string className = ByteCode::JvmClass::name(jfh->get_this_class(), pool);
    classes_[className] = file;

    fileList->get_files().push_back(file);
    file->set_parent(fileList);

    // Increase base virtual address for the next class
    baseVa += file->get_originalSize() + vaDefaultIncrement;
    baseVa -= baseVa % vaDefaultIncrement;

    // Decode instructions for usage downstream
    std::set<std::string> discoveredClasses{};
    auto disassembler = Architecture::findByName("jvm").orThrow()->newInstructionDecoder();
    for (auto sgMethod: jfh->get_method_table()->get_methods()) {
      ByteCode::JvmMethod method{jfh, sgMethod, jfh->get_baseVa()};
      method.decode(disassembler);
      discoverFunctionCalls(sgMethod, jfh->get_constant_pool(), functions_, discoveredClasses);
    }

    // Find and load super classes
    baseVa = loadSuperClasses(className, fileList, baseVa);

    // Load classes discovered during function call search
    for (auto discovered: discoveredClasses) {
      auto path = pathToClass(discovered);
      if (boost::filesystem::exists(path)) {
        baseVa = loadClassFile(path, fileList, baseVa);
      }
    }

    return baseVa;
}

rose_addr_t
EngineJvm::loadDiscoverableClasses(SgAsmGenericFileList* fileList, rose_addr_t baseVa) {
    for (auto file: fileList->get_files()) {
        auto jfh = dynamic_cast<SgAsmJvmFileHeader*>(file->get_header(SgAsmGenericFile::FAMILY_JVM));
        auto pool = jfh->get_constant_pool();

        for (auto sgMethod: jfh->get_method_table()->get_methods()) {
          // Examine instructions for classes
            for (auto insn: sgMethod->get_instruction_list()->get_instructions()) {
              switch (isSgAsmJvmInstruction(insn)->get_kind()) {
                case opcode::checkcast: // class, array, or interface type
                case opcode::instanceof: // class, array, or interface type
                  // TODO: don't try to load an array
                case opcode::new_: // can reference a class or interface type
                  if (auto expr = isSgAsmIntegerValueExpression(insn->get_operandList()->get_operands()[0])) {
                    std::string className = ByteCode::JvmClass::name(expr->get_value(), pool);
                    baseVa = loadClassFile(pathToClass(className), fileList, baseVa);
                  }
                  break;
                case opcode::getfield: // fetch field from object
                case opcode::getstatic: // get static field from class
                case opcode::putfield: // set field in object
                case opcode::putstatic: // set static field in class
                  if (auto expr = isSgAsmIntegerValueExpression(insn->get_operandList()->get_operands()[0])) {
                    auto fieldEntry = pool->get_entry(expr->get_value());
                    if (fieldEntry->get_tag() == SgAsmJvmConstantPoolEntry::CONSTANT_Fieldref) {
                      uint16_t classIndex = fieldEntry->get_class_index();
                      baseVa = loadClass(classIndex, pool, fileList, baseVa);
                    }
                  }
                  break;
                default: ;
              }
            }
        }
    }
    return baseVa;
}

rose_addr_t
EngineJvm::loadSuperClasses(const std::string &className, SgAsmGenericFileList* fileList, rose_addr_t baseVa) {
    // Make sure the class has been processed and the SgAsmGenericFile for it exists
    if (classes_.find(className) == classes_.end()) {
        return baseVa;
    }
    SgAsmGenericFile* file = classes_[className];

    auto jfh = dynamic_cast<SgAsmJvmFileHeader*>(file->get_header(SgAsmGenericFile::FAMILY_JVM));
    auto pool = jfh->get_constant_pool();

    // Load interfaces
    for (auto interface: jfh->get_interfaces()) {
        std::string interfaceName = ByteCode::JvmClass::name(interface, pool);
        auto interfacePath = pathToClass(interfaceName);
        if (boost::filesystem::exists(interfacePath)) {
            baseVa = loadClassFile(interfacePath, fileList, baseVa);
        }
    }

    std::string superName = ByteCode::JvmClass::name(jfh->get_super_class(), pool);
    if (superName.substr(0,10) == "java/lang/") {
        return baseVa;
    }

    // Load super class
    auto superPath = pathToClass(superName);
    if (boost::filesystem::exists(superPath)) {
        baseVa = loadClassFile(superPath, fileList, baseVa);
    }
    return baseVa;
}

void
EngineJvm::discoverFunctionCalls(SgAsmJvmMethod* sgMethod, SgAsmJvmConstantPool* pool,
                                 std::map<std::string,rose_addr_t> &fnm, std::set<std::string> &classes) {
    for (auto insn: sgMethod->get_instruction_list()->get_instructions()) {
        switch (isSgAsmJvmInstruction(insn)->get_kind()) {
          case opcode::invokedynamic:
          case opcode::invokeinterface:
          case opcode::invokespecial:
          case opcode::invokestatic:
          case opcode::invokevirtual:
            if (auto expr = isSgAsmIntegerValueExpression(insn->get_operandList()->get_operands()[0])) {
              std::string functionName = ByteCode::JvmClass::name(expr->get_value(), pool);
              if (fnm.find(functionName) == fnm.end()) {
                fnm[functionName] = nextFunctionVa_;
                nextFunctionVa_ -= 1024;

                // Also store corresponding class
                auto pos = functionName.find_first_of("::");
                if (pos != std::string::npos) {
                  classes.emplace(functionName.substr(0,pos));
                }
              }
            }
            break;
          default: ;
        }
    }
}

// Replacement for ::frontend, which is a complete mess, in order create a project containing multiple files. Nothing
// special happens to any of the input file names--that should have already been done by this point. All the fileNames
// are expected to be names of existing Java class or jar files.
SgProject*
EngineJvm::roseFrontendReplacement(const std::vector<boost::filesystem::path> &paths) {
    ASSERT_forbid(paths.empty());

    // Load class files starting at this virtual address
    rose_addr_t baseVa = 0;

    auto fileList = new SgAsmGenericFileList;

    // Load classes
    for (auto path: paths) {
        baseVa = loadClassFile(path, fileList, baseVa);
    }

    // Scan instructions for classes
    baseVa = loadDiscoverableClasses(fileList, baseVa);

    SAWYER_MESG(mlog[DEBUG]) <<"parsed " <<StringUtility::plural(fileList->get_files().size(), "container files") <<"\n";

    // DQ (11/25/2020): Add support to set this as a binary file (there is at least one binary file processed by ROSE).
    Rose::is_binary_executable = true;

    // The SgBinaryComposite (type of SgFile) points to the list of SgAsmGenericFile nodes created above.
    // FIXME[Robb Matzke 2019-01-29]: The defaults set here should be set in the SgBinaryComposite constructor instead.
    // FIXME[Robb Matzke 2019-01-29]: A SgBinaryComposite represents many files, not just one, so some of these settings
    //                                don't make much sense.
    auto jvmComposite = new SgJvmComposite;
    jvmComposite->initialization(); // SgFile::initialization
    jvmComposite->set_skipfinalCompileStep(true);
    jvmComposite->set_genericFileList(fileList); fileList->set_parent(jvmComposite);
    jvmComposite->set_sourceFileUsesBinaryFileExtension(true);
    jvmComposite->set_outputLanguage(SgFile::e_Binary_language);
    jvmComposite->set_inputLanguage(SgFile::e_Binary_language);
    jvmComposite->set_binary_only(true);
    jvmComposite->set_requires_C_preprocessor(false);
    jvmComposite->set_isObjectFile(false);
    jvmComposite->set_sourceFileNameWithPath(boost::filesystem::absolute(paths[0]).string()); // best we can do
    jvmComposite->set_sourceFileNameWithoutPath(paths[0].filename().string());                // best we can do
    jvmComposite->initializeSourcePosition(paths[0].string());                                // best we can do
    jvmComposite->set_originalCommandLineArgumentList(std::vector<std::string>(1, paths[0].string())); // best we can do
    ASSERT_not_null(jvmComposite->get_file_info());

    // Create one or more SgAsmInterpretation nodes. If all the SgAsmGenericFile objects are ELF files, then there's one
    // SgAsmInterpretation that points to them all. If all the SgAsmGenericFile objects are PE files, then there's two
    // SgAsmInterpretation nodes: one for all the DOS parts of the files, and one for all the PE parts of the files.
    std::vector<std::pair<SgAsmExecutableFileFormat::ExecFamily, SgAsmInterpretation*>> interpretations;
    for (SgAsmGenericFile *file: fileList->get_files()) {
        SgAsmGenericHeaderList *headerList = file->get_headers();
        ASSERT_not_null(headerList);
        for (SgAsmGenericHeader *header: headerList->get_headers()) {
            SgAsmGenericFormat *format = header->get_executableFormat();
            ASSERT_not_null(format);

            // Find or create the interpretation that holds this family of headers.
            SgAsmInterpretation *interpretation = nullptr;
            for (size_t i = 0; i < interpretations.size() && !interpretation; ++i) {
                if (interpretations[i].first == format->get_family())
                    interpretation = interpretations[i].second;
            }
            if (!interpretation) {
                interpretation = new SgAsmInterpretation;
                interpretations.push_back(std::make_pair(format->get_family(), interpretation));
            }

            // Add the header to the interpretation. This isn't an AST parent/child link, so don't set the parent ptr.
            SgAsmGenericHeaderList *interpHeaders = interpretation->get_headers();
            ASSERT_not_null(interpHeaders);
            interpHeaders->get_headers().push_back(header);
        }
    }
    SAWYER_MESG(mlog[DEBUG]) <<"created " <<StringUtility::plural(interpretations.size(), "interpretation nodes") <<"\n";

    // Put all the interpretations in a list
    SgAsmInterpretationList *interpList = new SgAsmInterpretationList;
    for (size_t i=0; i<interpretations.size(); ++i) {
        SgAsmInterpretation *interpretation = interpretations[i].second;
        interpList->get_interpretations().push_back(interpretation);
        interpretation->set_parent(interpList);
    }
    ASSERT_require(interpList->get_interpretations().size() == interpretations.size());

    // Add the interpretation list to the SgBinaryComposite node
    jvmComposite->set_interpretations(interpList);
    interpList->set_parent(jvmComposite);

    // The project
    SgProject *project = new SgProject;
    project->get_fileList().push_back(jvmComposite);
    jvmComposite->set_parent(project);

    return project;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory map creation (loading)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
EngineJvm::areSpecimensLoaded() const {
    // map will be empty for JVM specimens
    return memoryMap()!=nullptr;
}

MemoryMap::Ptr
EngineJvm::loadSpecimens(const std::vector<std::string> &fileNames) {
    try {
        if (!areContainersParsed()) {
            parseContainers(fileNames);
        }
        if (!memoryMap()) {
            memoryMap(MemoryMap::instance());
        }
        // No loading required for JVM files just return the MemoryMap.
        return memoryMap();
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Disassembler creation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner high-level functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
EngineJvm::checkCreatePartitionerPrerequisites() const {}

Partitioner::Ptr
EngineJvm::createPartitioner() {
    return createBarePartitioner();
}

void
EngineJvm::runPartitionerInit(const Partitioner::Ptr &partitioner) {
    Sawyer::Message::Stream where(mlog[WHERE]);
    SAWYER_MESG(where) <<"EngineJvm::runPartitionerInit\n";

    // Assume the JVM will return from a function call
    partitioner->autoAddCallReturnEdges(true);
}

void
EngineJvm::runPartitionerRecursive(const Partitioner::Ptr &partitioner) {
    Sawyer::Message::Stream where(mlog[WHERE]);
    SgAsmGenericHeaderList *interpHeaders = interpretation()->get_headers();
    ASSERT_not_null(interpHeaders);

    // Attach empty functions as targets for invoke of "java/" or bootstrap_method functions (reserved names)
    auto rit = functions_.rbegin();
    while (rit != functions_.rend()) {
        rose_addr_t va = rit->second;
        std::string name = rit->first;
        if (ByteCode::JvmContainer::isJvmSystemReserved(name)) {
            auto function = Partitioner2::Function::instance(va, name);
            auto block = Partitioner2::BasicBlock::instance(va, partitioner);
            function->insertBasicBlock(va);
            partitioner->attachBasicBlock(block);
            partitioner->attachFunction(function);
            rit++;
        }
        else {
            unresolvedFunctions_[name] = va;
            functions_.erase(std::next(rit).base());
        }
    }

    // Discover functions in the reverse order of headers because classes are loaded from derived to base
    // and functions should be "discovered" from the base class to derived.  Then the Partitioner2::Function
    // (e.g.) Base::<init> will be available for derived classes to call when they are created.
    for (auto rit = interpHeaders->get_headers().rbegin(); rit != interpHeaders->get_headers().rend(); rit++) {
        auto header = *rit;

        // This is strange construction a ByteCode::Class needs information from its ByteCode::Namespace, like name
        // Thus a cycle (is it more than strange, is it bad?)
        std::shared_ptr<ByteCode::Namespace> ns{};
        auto jvmClass{std::make_shared<ByteCode::JvmClass>(ns, isSgAsmJvmFileHeader(header))};
        ns->append(jvmClass);

        // Start discovering instructions and forming them into basic blocks and functions
        SAWYER_MESG(where) <<"discovering and populating functions\n";
        jvmClass->partition(partitioner, functions_);

        if (DEBUG_WITH_DUMP) {
            jvmClass->dump();
            jvmClass->digraph();
        }
    }
}

void
EngineJvm::runPartitionerFinal(const Partitioner::Ptr &partitioner) {
    Sawyer::Message::Stream where(mlog[WHERE]);
    SAWYER_MESG(where) <<"EngineJvm::runPartitionerFinal\n";

    // Primary task is to transfer call edges to the indeterminate vertex.
    // First, for efficiency create a map of function names to addresses.
    std::map<std::string,rose_addr_t> functionNameMap;
    for (auto function: partitioner->functions()) {
      functionNameMap[function->name()] = function->address();
    }

    // By this point all of the functions should have been found
    for (auto pair: unresolvedFunctions_) {
        rose_addr_t va = pair.second;
        std::string name = pair.first;
        if (functionNameMap.find(name) == functionNameMap.end()) {
          mlog[WARN] << "unresolved function: " << name << " address: " << StringUtility::addrToString(va) <<"\n";
        }
    }

  // Mapping of basic blocks (with edge to indeterminate) to callee basic block
  std::map<BasicBlock::Ptr, rose_addr_t> sourceBlocks;

  auto indeterminate = partitioner->indeterminateVertex();
  for (auto incoming: indeterminate->inEdges()) {
    if (incoming.value().type() == E_FUNCTION_CALL) {
      auto source = incoming.source();
      if (source->value().type() == V_BASIC_BLOCK) {
        auto value = source->value();
        if (BasicBlock::Ptr bb = source->value().bblock()) {
          SgAsmInstruction* last = bb->instructions().back();
          std::string functionName = last->get_comment();
          std::cout << "... indeterminate incoming: " << functionName << " at " << StringUtility::addrToString(bb->address()) << std::endl;
          if (functionNameMap.find(functionName) != functionNameMap.end()) {
            rose_addr_t functionVa = functionNameMap[functionName];
            sourceBlocks[bb] = functionVa;
          }
        }
      }
    }
  }

  std::cout << "\n";

  // Replace successor edge to the indeterminate node with a new edge to the discovered function.
  // The source basic block must be detached before the successor edge can be replaced.
  for (auto itr = sourceBlocks.begin(); itr != sourceBlocks.end(); itr++) {
    BasicBlock::Ptr bb = itr->first;
    rose_addr_t va = itr->second;
    auto detached = partitioner->detachBasicBlock(bb);
    std::cout << "[INFO] replacing successor (to indeterminate) from bb(va): " << StringUtility::addrToString(bb->address())
              << " : to " << StringUtility::addrToString(va) << "\n";
    detached->clearSuccessors();
    detached->insertSuccessor(va, 64, EdgeType::E_FUNCTION_CALL, Confidence::PROVED);
    partitioner->attachBasicBlock(detached);
  }

  // partitioner->dumpCfg(std::cout, "Worker:", true, false);

}

Partitioner::Ptr
EngineJvm::partition(const std::vector<std::string> &fileNames) {
    try {
        for (const std::string &fileName: fileNames) {
            if (isRbaFile(fileName) && fileNames.size() != 1)
                throw Exception("specifying an RBA file excludes all other inputs");
        }
        if (fileNames.size() == 1 && isRbaFile(fileNames[0])) {
            auto partitioner = Partitioner::instanceFromRbaFile(fileNames[0], SerialIo::BINARY);
            interpretation(partitioner->interpretation());
            return partitioner;
        } else {
            if (!areSpecimensLoaded()) {
                loadSpecimens(fileNames); }
            obtainArchitecture();
            Partitioner::Ptr partitioner = createPartitioner();
            runPartitioner(partitioner);
            return partitioner;
        }
    } catch (const std::runtime_error &e) {
        if (settings().engine.exitOnError) {
            mlog[FATAL] <<e.what() <<"\n";
            exit(1);
        } else {
            throw;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner mid-level operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
EngineJvm::discoverBasicBlocks(const PartitionerPtr& partitioner, const ByteCode::Method* m) {
    //TODO: Fix this mess please.

    const ByteCode::JvmMethod* method{dynamic_cast<const ByteCode::JvmMethod*>(m)};
    const SgAsmInstructionList* instructions{method->instructions()};
    rose_addr_t startVa{method->code().offset()};
    rose_addr_t va{startVa};

    // Not sure where this goes (I suppose this discovers basic blocks), at least part of it, as it disassembles
    method->decode(architecture()->newInstructionDecoder());

    //TODO:
    // 1. create basic block
    // 2. add vertices
    BasicBlockPtr bb = BasicBlock::instance(va, partitioner);
    //TODO: bb should be attached after instructions are added
    partitioner->attachBasicBlock(bb);

    for (auto insn: instructions->get_instructions()) {
        bb->append(partitioner, insn);
        va += insn->get_size();

        //TODO: void insertSuccessor(rose_addr_t va, size_t nBits, EdgeType type=E_NORMAL, Confidence confidence=ASSUMED);

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner low-level stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build AST
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmBlock*
EngineJvm::buildAst(const std::vector<std::string> &fileNames) {
#ifdef IMPLEMENT_BUILD_AST
  using std::cout;
  using namespace ModulesJvm;
//TODO: More ...
  std::string fileName{fileNames[0]};

  if (isJavaClassFile(fileName)) {
#ifdef DEBUG_ON
    // assumes args[0] is the file name for now
    cout << "EngineJvm::parseCommandLine for file " << fileName << endl;
    cout << "  Purpose: " << purpose << ": Description: " << description << "\n";
#endif
  }
  else {
    return nullptr;
  }

  auto gf = new SgAsmGenericFile{};
  gf->parse(fileName); /* this loads file into memory, does no reading of file */
  auto header = new SgAsmJvmFileHeader(gf);

  // Check AST
  ASSERT_require(header == gf->get_header(SgAsmGenericFile::FAMILY_JVM));
  ASSERT_requirea(header->get_parent() == gf);

  header->parse();
#ifdef DEBUG_ON
  cout << "\n --- JVM file header ---\n";
  header->dump(stdout, "    jfh:", 0);

  cout << "\n---------- JVM Analysis -----------------\n\n";
#endif

  Rose::BinaryAnalysis::ByteCode::JvmClass* jvmClass = new ByteCode::JvmClass(header);
#ifdef DEBUG_ON
  cout << "class '" << jvmClass->name() << "'" << endl;
  cout << "----------------\n";
  cout << "   super: " << jvmClass->super_name() << "\n\n";

  cout << "constant pool\n";
  cout << "-----------\n";
  jvmClass->constant_pool()->dump(stdout, "", 1);
  cout << "-----------\n\n";

  if (jvmClass->interfaces().size() > 0) {
    cout << "interfaces\n";
    cout << "-----------\n";
    for (auto interface : jvmClass->interfaces()) {
      cout << "   interface: " << interface->name() << endl;
    }
    cout << "-----------\n\n";
  }

  if (jvmClass->fields().size() > 0) {
    cout << "fields\n";
    cout << "-----------\n";
    for (auto field : jvmClass->fields()) {
      cout << "   field: " << field->name() << endl;
    }
    cout << "-----------\n\n";
  }

  if (jvmClass->attributes().size() > 0) {
    cout << "attributes\n";
    cout << "-----------\n";
    for (auto attribute : jvmClass->attributes()) {
      cout << "   attribute: " << attribute->name() << endl;
    }
    cout << "-----------\n\n";
  }
#endif

  for (auto method : jvmClass->methods()) {
#ifdef DEBUG_ON
    cout << "method '" << method->name() << endl;
    cout << "-----------\n";
#endif

    Disassembler::Base::Ptr disassembler = obtainDisassembler();
    ASSERT_not_null(disassembler);

    method->decode(disassembler);
#ifdef DEBUG_ON
    for (auto insn : method->instructions()->get_instructions()) {
      cout << "   : " << insn->get_anyKind() << ": " << insn->get_mnemonic() << ": '"
           << insn->description() << "' " << " size:" << insn->get_size()
           << " va:" << insn->get_address();
      if (insn->terminatesBasicBlock()) cout << " :terminates";
      cout << endl;
    }
    cout << "-----------\n\n";
#endif
  }

  // Bonus
#ifdef DEBUG_ON
  cout << "--- strings ---\n";
  for (auto str : jvmClass->strings()) {
    cout << "   " << str << endl;
  }
  cout << "-----------\n\n";
#endif

  // Run the partitioner
  jvmClass->partition();

  // Dump diagnostics from the partition
#ifdef DEBUG_ON
  for (auto method : jvmClass->methods()) {
    cout << "\nmethod: " << method->name() << endl;
    for (auto block : method->blocks()) {
      cout << "--------------block------------\n";
      for (auto insn : block->instructions()) {
        auto va = insn->get_address();
        cout << "... insn: " << insn << " va:" << va << " :" << insn->get_mnemonic()
             << " nOperands:" << insn->nOperands()
             << " terminates:" << insn->terminatesBasicBlock() << endl;
        for (auto op : insn->get_operandList()->get_operands()) {
          if (op->asUnsigned()) {
            cout << "      unsigned operand:" << *(op->asUnsigned()) << endl;
          }
          else if (op->asSigned()) {
            cout << "       signed operand:" << *(op->asSigned()) << endl;
          }
        }
      }
      // Explore block methods
      cout << "      :nInstructions:" << block->nInstructions() << endl;
      cout << "      :address:" << block->address() << endl;
      cout << "      :fallthroughVa:" << block->fallthroughVa() << endl;
      cout << "      :isEmpty:" << block->isEmpty() << endl;
      cout << "      :nDataBlocks:" << block->nDataBlocks() << endl;
      if (block->isFunctionCall().isCached()) cout << "      :isFunctionCall:" << block->isFunctionCall().get() << endl;
      if (block->isFunctionReturn().isCached()) cout << "      :isFunctionReturn:" << block->isFunctionReturn().get() << endl;
      if (block->successors().isCached()) cout << "      :#successors:" << block->successors().get().size() << endl;
    }
  }
#endif

  // Create graphviz DOT file
  jvmClass->digraph();
#endif // IMPLEMENT...

  return nullptr;
}

const std::string&
EngineJvm::isaName() const {
    return settings().disassembler.isaName;
}

void
EngineJvm::isaName(const std::string &s) {
    settings().disassembler.isaName = s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Settings and Properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      JVM Module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Disassembly and partitioning utility functions for JVM. */
namespace ModulesJvm {

/** True if named file is a Java class file.
 *
 *  Class files usually have names with a ".class" extension, although this function actually tries to open the file and parse
 *  the file header to make that determination. */
bool isJavaClassFile(const boost::filesystem::path &file) {
  if (!boost::filesystem::exists(file)) {
    return false; // file doesn't exist
  }

  MemoryMap::Ptr map = MemoryMap::instance();
  if (0 == map->insertFile(file.string(), 0)) {
    return false; // file cannot be mmap'd
  }

  uint8_t magic[4];
  if (4 == map->at(0).limit(4).read(magic).size()) {
    if (magic[0] == 0xCA && magic[1] == 0xFE && magic[2] == 0xBA && magic[3] == 0xBE) {
      // 0xCAFEBABE
      return true;
    }
  }
  return false;
}  

} // namespace


} // namespace
} // namespace
} // namespace

#endif
