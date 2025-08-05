#include <sage3basic.h>                                 // only because some header files need it

#include <Sawyer/Assert.h>
#include <Sawyer/ProgressBar.h>

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BestMapAddress.h>
#include <Rose/BinaryAnalysis/BinaryLoader.h>
#include <Rose/BinaryAnalysis/CodeInserter.h>
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/DataFlow.h>
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/FeasiblePath.h>
#include <Rose/BinaryAnalysis/FunctionSimilarity.h>
#include <Rose/BinaryAnalysis/HotPatch.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/BinaryAnalysis/LibraryIdentification.h>
#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>
#include <Rose/BinaryAnalysis/NoOperation.h>
#include <Rose/BinaryAnalysis/Reachability.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/SymbolicExpressionParser.h>
#include <Rose/BinaryAnalysis/TaintedFlow.h>
#include <Rose/BinaryAnalysis/ToSource.h>
#include <Rose/BinaryAnalysis/VxworksTerminal.h>

#include <BinaryVxcoreParser.h>                         // Rose::BinaryAnalysis::VxcoreParser

namespace Rose {
namespace BinaryAnalysis {
    namespace Architecture { void initDiagnostics(); }
    namespace CallingConvention { void initDiagnostics(); }
    namespace Cil { void initDiagnostics(); }
    namespace Dwarf { void initDiagnostics(); }
    namespace Partitioner2 {
        void initDiagnostics();
        namespace IndirectControlFlow { void initDiagnostics(); }
    }
    namespace PointerDetection { void initDiagnostics(); }
    namespace ReturnValueUsed { void initDiagnostics(); }
    namespace StackDelta { void initDiagnostics(); }
    namespace Strings { void initDiagnostics(); }
    namespace Unparser { void initDiagnostics(); }
    namespace Variables { void initDiagnostics(); }
    void SerialIo_initDiagnostics();
} // namespace
} // namespace

#endif

namespace VxUtilFuncs { void initDiagnostics(); }

#include <Rose/Diagnostics.h>
#include <EditDistance/EditDistance.h>                  // Rose::EditDistance
#include <midend/astUtil/astInterface/AstInterface.h>

// DQ (3/24/2016): Adding support for EDG/ROSE frontend message logging.
#ifndef ROSE_USE_CLANG_FRONTEND
// DQ (2/5/2017): This is only used with the EDG frontend, not for use when configured to use Clang.
namespace EDG_ROSE_Translation
   {
     void initDiagnostics();
   }
#endif

// DQ (3/24/2016): Adding support for AstDiagnostics / AstConsistancy tests message logging.
#include "AstDiagnostics.h"
// #include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"
#include "astPostProcessing.h"
#include "nameQualificationSupport.h"
// #include "unparseLanguageIndependentConstructs.h"
#include "unparser.h"
#include "sageBuilder.h"
// rose_config.h is needed for ROSE_USE_CLANG_FRONTEND
#include "rose_config.h"

#include <cstdarg>

namespace Rose {
namespace Diagnostics {

ROSE_DLL_API Sawyer::Message::DestinationPtr destination;
ROSE_DLL_API Sawyer::Message::PrefixPtr mprefix;
ROSE_DLL_API Sawyer::Message::Facility mlog;
static bool isInitialized_ = false;

void initialize() {
    if (!isInitialized()) {
        isInitialized_ = true;

        // How do failed assertions behave?  Don't make any changes if this is already initialized by the user.
        if (!Sawyer::Assert::assertFailureHandler)
            failedAssertionBehavior(NULL);              // sets it to a default behavior based on configuration

        // Allow libsawyer to initialize itself if necessary.  Among other things, this makes Saywer::Message::merr actually
        // point to something.  This is also the place where one might want to assign some other message plumbing to
        // Rose::Diagnostics::destination (such as sending messages to additional locations).
        Sawyer::initializeLibrary();
        if (mprefix==NULL)
            mprefix = Sawyer::Message::Prefix::instance();
        if (destination==NULL) {
            // use FileSink or FdSink because StreamSink can't tell whether output is a tty or not.
            destination = Sawyer::Message::FileSink::instance(stderr)->prefix(mprefix);
        }

        // Force certain facilities to be enabed or disabled. This might be different than the Sawyer default. If user wants
        // something else then use mfacilities.control("...") after we return. This doesn't affect any Facility object that's
        // already registered (such as any added already by the user or Sawyer's own, but we could use mfacilities.renable() if
        // we wanted that).
        mfacilities.impset(DEBUG, false);
        mfacilities.impset(TRACE, false);
        mfacilities.impset(WHERE, false);
        mfacilities.impset(MARCH, true);
        mfacilities.impset(INFO,  true);
        mfacilities.impset(WARN,  true);
        mfacilities.impset(ERROR, true);
        mfacilities.impset(FATAL, true);

        // (Re)construct the main librose Facility.  A Facility is constructed with all Stream objects enabled, but
        // insertAndAdjust will change that based on mfacilities' settings.
        initAndRegister(&mlog, "Rose");
        mlog.comment("top-level ROSE diagnostics");

        // Where should failed assertions go for the Sawyer::Assert macros like ASSERT_require()?
        Sawyer::Message::assertionStream = mlog[FATAL];

        // Turn down the progress bar rates
        Sawyer::ProgressBarSettings::initialDelay(1.0);
        Sawyer::ProgressBarSettings::minimumUpdateInterval(0.2);

        // Register logging facilities from other software layers.  Calling these initializers should make all the streams
        // point to the Rose::Diagnostics::destination that we set above.  Generally speaking, if a frontend language is
        // disabled there should be a dummy initDiagnostics that does nothing so we don't need lots of #ifdefs here.
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
        BinaryAnalysis::Architecture::initDiagnostics();
        BinaryAnalysis::BestMapAddress::initDiagnostics();
        BinaryAnalysis::BinaryLoader::initDiagnostics();
        BinaryAnalysis::CallingConvention::initDiagnostics();
        BinaryAnalysis::Cil::initDiagnostics();
        BinaryAnalysis::CodeInserter::initDiagnostics();
#ifdef ROSE_ENABLE_CONCOLIC_TESTING                     // conditionally defined in Rose/BinaryAnalysis/Concolic.h
        BinaryAnalysis::Concolic::initDiagnostics();
#endif
        BinaryAnalysis::DataFlow::initDiagnostics();
        BinaryAnalysis::Debugger::initDiagnostics();
        BinaryAnalysis::Disassembler::initDiagnostics();
        BinaryAnalysis::Dwarf::initDiagnostics();
        BinaryAnalysis::FeasiblePath::initDiagnostics();
        BinaryAnalysis::FunctionSimilarity::initDiagnostics();
        BinaryAnalysis::HotPatch::initDiagnostics();
        BinaryAnalysis::InstructionSemantics::initDiagnostics();
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION
        BinaryAnalysis::LibraryIdentification::initDiagnostics();
#endif
#ifdef ROSE_ENABLE_MODEL_CHECKER
        BinaryAnalysis::ModelChecker::initDiagnostics();
#endif
        BinaryAnalysis::NoOperation::initDiagnostics();
        BinaryAnalysis::Partitioner2::initDiagnostics();
        BinaryAnalysis::Partitioner2::IndirectControlFlow::initDiagnostics();
        BinaryAnalysis::PointerDetection::initDiagnostics();
        BinaryAnalysis::Reachability::initDiagnostics();
        BinaryAnalysis::ReturnValueUsed::initDiagnostics();
        BinaryAnalysis::SerialIo_initDiagnostics();
        BinaryAnalysis::SmtSolver::initDiagnostics();
        BinaryAnalysis::StackDelta::initDiagnostics();
        BinaryAnalysis::Strings::initDiagnostics();
        BinaryAnalysis::SymbolicExpressionParser::initDiagnostics();
        BinaryAnalysis::TaintedFlow::initDiagnostics();
        BinaryAnalysis::BinaryToSource::initDiagnostics();
        BinaryAnalysis::Unparser::initDiagnostics();
        BinaryAnalysis::Variables::initDiagnostics();
        BinaryAnalysis::VxcoreParser::initDiagnostics();
        BinaryAnalysis::VxworksTerminal::initDiagnostics();
        SgAsmExecutableFileFormat::initDiagnostics();
#endif
        EditDistance::initDiagnostics();
#ifdef ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#ifndef ROSE_USE_CLANG_FRONTEND
     // DQ (2/5/2017): This is only used with the EDG frontend, not for use when configured to use Clang.
        EDG_ROSE_Translation::initDiagnostics();
#endif
#endif
        TestChildPointersInMemoryPool::initDiagnostics();
        FixupAstSymbolTablesToSupportAliasedSymbols::initDiagnostics();
        FixupAstDeclarationScope::initDiagnostics();
        NameQualificationTraversal::initDiagnostics();
        UnparseLanguageIndependentConstructs::initDiagnostics();
        SageBuilder::initDiagnostics();
        AstInterface::initDiagnostics();
        VxUtilFuncs::initDiagnostics();

     // DQ (3/5/2017): Adding message stream to support diagnostic message from the ROSE IR nodes.
        Rose::initDiagnostics();
    }
}

bool isInitialized() {
    return isInitialized_;
}

void
initAndRegister(Facility *mlog, const std::string &name) {
    ASSERT_not_null(mlog);
    *mlog = Facility(name, destination);
    mfacilities.insertAndAdjust(*mlog);
}

void
deregister(Facility *mlog) {
    if (mlog != NULL)
        mfacilities.erase(*mlog);
}

StreamPrintf mfprintf(std::ostream &stream) {
    return StreamPrintf(stream);
}

int StreamPrintf::operator()(const char *fmt, ...) {
    char buf_[1024];                                    // arbitrary size; most strings will be smaller
    char *buf = buf_;
    int bufsz = sizeof buf_, need = 0;

    while (1) {
        va_list ap;
        va_start(ap, fmt);
        need = vsnprintf(buf, bufsz, fmt, ap);          // "need" does not include NUL termination
        va_end(ap);
        if (need >= bufsz) {
            if (buf!=buf_)
                delete[] buf;
            bufsz = need + 1;                           // +1 for NUL termination
            buf = new char[bufsz];
        } else {
            break;
        }
    }
    stream <<buf;
    if (buf!=buf_)
        delete[] buf;

    return need;
}

        
    


} // namespace
} // namespace
