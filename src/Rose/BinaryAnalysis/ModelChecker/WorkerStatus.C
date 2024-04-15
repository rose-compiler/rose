#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/WorkerStatus.h>

#include <Rose/BinaryAnalysis/ModelChecker/Exception.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

// Use this to suppress compiler warnings about not using the return value from functions like "write"
template<typename T>
static T ignore_return(T v) {
    return v;
}

WorkerStatus::WorkerStatus(const boost::filesystem::path &fileName)
    : fileName_(fileName) {
    if ((fd_ = open(fileName.c_str(), O_CREAT|O_TRUNC|O_RDWR|O_DSYNC, 0666)) < 0) {
        throw Exception("cannot create file for worker status: \"" + StringUtility::cEscape(fileName.string()) + "\""
                        " (" + strerror(errno) + ")");
    }
}

WorkerStatus::~WorkerStatus() {
    if (fd_ >= 0)
        close(fd_);
}

WorkerStatus::Ptr
WorkerStatus::instance(const boost::filesystem::path &fileName) {
    return Ptr(new WorkerStatus(fileName));
}

void
WorkerStatus::insert(size_t workerIdx, const Progress::Ptr &progress) {
    const time_t now = time(nullptr);
    Status blank;
    blank.stateChange = now;
    if (workers_.size() <= workerIdx)
        workers_.resize(workerIdx + 1);
    workers_[workerIdx].stateChange = now;
    workers_[workerIdx].progress = progress;
    updateFile();
}

void
WorkerStatus::updateFile() {
    time_t now = time(nullptr);
    if (now - lastFileUpdate_ >= fileUpdatePeriod_) {
        updateFileNow();
        lastFileUpdate_ = now;
    }
}

void
WorkerStatus::updateFileNow() {
    if (!workers_.empty() && lseek(fd_, 0, SEEK_SET) == 0) {
        const time_t now = time(nullptr);

        time_t earliestChange = workers_[0].stateChange;
        for (size_t i = 1; i < workers_.size(); ++i)
            earliestChange = std::min(earliestChange, workers_[i].stateChange);
        const time_t maxDelta = std::max((time_t)10, earliestChange < now ? now - earliestChange : 0);

        std::ostringstream key;
        key <<"Status updated "
#if defined(__GNUC__) && __GNUC__ < 5 && !defined(__clang__)
            // GCC-4 doesn't fully support C++11, so just say something generic. GCC-4 is used by RHEL-6 which is no longer supported anyway.
            // LLVM clang++ compiler advertises itself as GCC-4.2.1, which is not a full C++11 compiler.
            <<"some time ago (sorry, compiler lacks full C++11 support)\n"
#else
            <<std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S %Z") <<"\n"
#endif
            <<"Key: \033[34mstarting\033[33m waiting\033[32m running\033[31m finished\033[0m\n"
            <<"Each bar represents " <<StringUtility::plural(maxDelta, "seconds") <<" of elapsed time since last state change\n"
            <<"\n";
        ignore_return(write(fd_, key.str().data(), key.str().size()));

        for (size_t workerIdx = 0; workerIdx < workers_.size(); ++workerIdx) {
            int fgColorCode = 90;                       // gray
            switch (workers_[workerIdx].state) {
                case WorkerState::STARTING: fgColorCode = 34; break;            // blue
                case WorkerState::WAITING:  fgColorCode = 33; break;            // yellow
                case WorkerState::WORKING:  fgColorCode = 32; break;            // green
                case WorkerState::FINISHED: fgColorCode = 31; break;            // red
            }

            size_t w = round((double)barWidth_ * (now - workers_[workerIdx].stateChange) / maxDelta);
            const std::string bar = "[" + std::string(w, '#') + std::string(barWidth_ - w, '-') + "]";

            std::string reportName;
            if (Progress::Ptr progress = workers_[workerIdx].progress) {
                Progress::Report report = progress->reportLatest().first;

                std::vector<std::string> parts;
                boost::split(parts /*out*/, report.name, boost::is_any_of("."));
                std::reverse(parts.begin(), parts.end());
                for (size_t i = 0; i < 2 && i <parts.size(); ++i) {
                    if (!reportName.empty())
                        reportName = "." + reportName;
                    if ("smt-prepare" == parts[i]) {
                        reportName = "pre" + reportName;
                    } else if ("smt-check" == parts[i]) {
                        reportName = "chk" + reportName;
                    } else if ("smt-memoize" == parts[i]) {
                        reportName = "mem" + reportName;
                    } else if ("smt-evidence" == parts[i]) {
                        reportName = "evd" + reportName;
                    } else if ("nextUnits" == parts[i]) {
                        reportName = "nxt" + reportName;
                    } else if ("findNext" == parts[i]) {
                        reportName = "bb" + reportName;
                    } else if ("nullptr" == parts[i]) {
                        reportName = "nul" + reportName;
                    } else if ("oob" == parts[i]) {
                        reportName = "oob" + reportName;
                    } else if ("uninit" == parts[i]) {
                        reportName = "uni" + reportName;
                    } else {
                        reportName = parts[i].substr(0, 3) + reportName;
                    }
                }
            }

            const std::string pathHash = workers_[workerIdx].pathHash ?
                                         (boost::format("%08x") % workers_[workerIdx].pathHash).str().substr(0, 4) :
                                         std::string(4, ' ');

            //                                    worker   fg  bar h name       sep
            const std::string s = (boost::format("%4d \033[%2dm%s %s %-7s\033[0m%c")
                                   % workerIdx
                                   % fgColorCode
                                   % bar
                                   % pathHash
                                   % reportName
                                   % ((workerIdx + 1) % workersPerLine_ ? ' ' : '\n')
                                   ).str();
            ignore_return(write(fd_, s.data(), s.size()));
        }

        const std::string legend =
            std::string(workers_.size() % workersPerLine_ ? "\n\n" : "\n") +
            "-------------------------------- -------------------------------- --------------------------------\n"
            "pre = SMT solver preparation     chk = SMT solver check           mem = SMT solver memoization\n"
            "evd = SMT solver evidence parse  nxt = find next basic block      bb  = eval basic block for loops\n"
            "nul = nullptr check              oob = out of bounds check        uni = uninitialized variable check\n";

        ignore_return(write(fd_, legend.data(), legend.size()));
    }
}

void
WorkerStatus::setState(size_t workerIdx, WorkerState state, uint64_t pathHash) {
    if (workerIdx < workers_.size()) {
        workers_[workerIdx].state = state;
        workers_[workerIdx].stateChange = time(nullptr);
        ASSERT_require((WorkerState::WORKING == state && pathHash != 0) || (WorkerState::WORKING != state && pathHash == 0));
        workers_[workerIdx].pathHash = pathHash;
        updateFile();
    }
}

const boost::filesystem::path&
WorkerStatus::fileName() const {
    return fileName_;
}

} // namespace
} // namespace
} // namespace

#endif
