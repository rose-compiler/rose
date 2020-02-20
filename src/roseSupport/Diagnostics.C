#include "sage3basic.h"                                 // only because some header files need it

#include "Sawyer/Assert.h"
#include "Sawyer/ProgressBar.h"

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "AsmUnparser.h"                                // Rose::BinaryAnalysis::AsmUnparser
#include "BinaryBestMapAddress.h"                       // Rose::BinaryAnalysis::BestMapAddress
#include "BinaryCodeInserter.h"                         // Rose::BinaryAnalysis::CodeInserter
#include "BinaryConcolic.h"                             // Rose::BinaryAnalysis::Concolic
#include "BinaryDataFlow.h"                             // Rose::BinaryAnalysis::DataFlow
#include "BinaryDebugger.h"                             // Rose::BinaryAnalysis::Debugger
#include "BinaryFeasiblePath.h"                         // Rose::BinaryAnalysis::FeasiblePath
#include "BinaryFunctionSimilarity.h"                   // Rose::BinaryAnalysis::FunctionSimilarity
#include "BinaryHotPatch.h"                             // Rose::BinaryAnalysis::HotPatch
#include "BinaryLoader.h"                               // Rose::BinaryAnalysis::BinaryLoader
#include "BinaryNoOperation.h"                          // Rose::BinaryAnalysis::NoOperation
#include "BinaryReachability.h"                         // Rose::BinaryAnalysis::Reachability
#include "BinarySmtSolver.h"                            // Rose::BinaryAnalysis::SmtSolver
#include "BinarySymbolicExprParser.h"                   // Rose::BinaryAnalysis::SymbolicExprParser
#include "BinaryTaintedFlow.h"                          // Rose::BinaryAnalysis::TaintedFlow
#include "BinaryToSource.h"                             // Rose::BinaryAnalysis::BinaryToSource
#include "BinaryVxcoreParser.h"                         // Rose::BinaryAnalysis::VxcoreParser
#include "Disassembler.h"                               // Rose::BinaryAnalysis::Disassembler

namespace Rose {
namespace BinaryAnalysis {
    namespace CallingConvention { void initDiagnostics(); }
    namespace InstructionSemantics2 { void initDiagnostics(); }
    namespace Partitioner2 { void initDiagnostics(); }
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

#include "Diagnostics.h"                                // Rose::Diagnostics
#include <EditDistance/EditDistance.h>                  // Rose::EditDistance

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
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
        BinaryAnalysis::BinaryLoader::initDiagnostics();
        BinaryAnalysis::AsmUnparser::initDiagnostics();
        BinaryAnalysis::BestMapAddress::initDiagnostics();
        BinaryAnalysis::Debugger::initDiagnostics();
        BinaryAnalysis::CallingConvention::initDiagnostics();
        BinaryAnalysis::CodeInserter::initDiagnostics();
#ifdef ROSE_ENABLE_CONCOLIC_TESTING                     // conditionally defined in BinaryConcolic.h
        BinaryAnalysis::Concolic::initDiagnostics();
#endif
        BinaryAnalysis::DataFlow::initDiagnostics();
        BinaryAnalysis::Disassembler::initDiagnostics();
        BinaryAnalysis::FeasiblePath::initDiagnostics();
        BinaryAnalysis::FunctionSimilarity::initDiagnostics();
        BinaryAnalysis::HotPatch::initDiagnostics();
        BinaryAnalysis::InstructionSemantics2::initDiagnostics();
        BinaryAnalysis::NoOperation::initDiagnostics();
        BinaryAnalysis::Partitioner2::initDiagnostics();
        BinaryAnalysis::PointerDetection::initDiagnostics();
        BinaryAnalysis::Reachability::initDiagnostics();
        BinaryAnalysis::ReturnValueUsed::initDiagnostics();
        BinaryAnalysis::SerialIo_initDiagnostics();
        BinaryAnalysis::SmtSolver::initDiagnostics();
        BinaryAnalysis::StackDelta::initDiagnostics();
        BinaryAnalysis::Strings::initDiagnostics();
        BinaryAnalysis::SymbolicExprParser::initDiagnostics();
        BinaryAnalysis::TaintedFlow::initDiagnostics();
        BinaryAnalysis::BinaryToSource::initDiagnostics();
        BinaryAnalysis::Unparser::initDiagnostics();
        BinaryAnalysis::Variables::initDiagnostics();
        BinaryAnalysis::VxcoreParser::initDiagnostics();
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

#if 1
     // DQ (3/5/2017): Adding message stream to support diagnostic message from the ROSE IR nodes.
        Rose::initDiagnostics();
#endif
    }
}

bool isInitialized() {
    return isInitialized_;
}

// [Robb P Matzke 2017-02-16]: deprecated
void
initAndRegister(Facility &mlog, const std::string &name) {
    initAndRegister(&mlog, name);
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
