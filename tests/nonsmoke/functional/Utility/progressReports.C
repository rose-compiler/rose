// Tests multi-threaded progress reporting.
#include <Progress.h>

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <Sawyer/Synchronization.h>
#include <string>

// Pretends to do some real work consisting of multiple phases each having some number of steps.
static void
worker_main(size_t nPhases, size_t nStepsPerPhase, boost::chrono::milliseconds timePerStep, Rose::Progress::Ptr progress) {
    for (size_t phase = 0; phase < nPhases; ++phase) {
        std::string phaseName = "phase " + boost::lexical_cast<std::string>(phase);
        for (size_t step = 0; step < nStepsPerPhase; ++step) {
            boost::this_thread::sleep_for(timePerStep); // sleeping on the job, but this is where real work should happen
            if (progress) // check is optional (and pointless in these tests); may be used to avoid expensive reporting
                progress->update(Rose::Progress::Report(phaseName, (double)step / nStepsPerPhase));
        }
    }

    // Tell listeners that there will be no more progress updates.  This isn't strictly necessary if you've arranged other ways
    // to inform the listeners (which we haven't done in these tests).  In any case, notice that the worker results (even if
    // it's just an indication of success or failure) are never reported via progress objects; use the usual result reporting
    // mechanisms such as promises and futures for that.  This should be obvious in light of the fact that we've protected
    // "progress" with "if" statements!
    if (progress)
        progress->finished();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test that the user can poll the progress object

static void
test_poll() {
    // This boiler-plate is the same in all the tests. It simply runs worker_main in a new thread.
    std::cout <<"test user polling mechanism\n";
    Rose::Progress::Ptr progress = Rose::Progress::instance();
    boost::thread worker(worker_main, 2, 5, boost::chrono::milliseconds(1500), progress);

    // Polling is the easiest method to understand.
    while (!progress->isFinished()) {
        // Non-atomic, so progress might have finished, but that's okay.
        std::pair<Rose::Progress::Report, double> rpt = progress->reportLatest();
        printf("    %s: %3.0f%% (%0.3f seconds ago)\n", rpt.first.name.c_str(), rpt.first.completion*100, rpt.second);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    }

    // Release resources. The thread needs to be joined, but the progress object will be deleted automatically when the last
    // pointer goes out of scope. If this weren't the case, it might be hard to decide when to delete it since there might be
    // multiple worker threads and listener threads using it.
    worker.join();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test that a user-defined callback can be invoked on a regular schedule

// This user-defined callback will be invoked with the latest available progress report and the amount of time since it was
// reported by the worker thread.
static bool
report_progress_with_age(const Rose::Progress::Report &report, double ageInSeconds) {
    printf("    %s: %3.0f%% (%0.3f seconds ago)\n", report.name.c_str(), report.completion*100, ageInSeconds);
    return true; // keep listening
}

static void
test_callback_scheduled() {
    std::cout <<"test user-defined callback at regular intervals\n";
    Rose::Progress::Ptr progress = Rose::Progress::instance();
    boost::thread worker(worker_main, 2, 5, boost::chrono::milliseconds(1500), progress);

    boost::chrono::milliseconds reportingInterval(500); // call report_progress_with_age twice per second
    progress->reportRegularly(reportingInterval, report_progress_with_age);

    worker.join();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test that a user-defined callback can be invoked when the progress is updated

// User-defined callback. This time we don't need the age because it's being called immediately after a progress report
// arrives from a worker thread.
static bool
report_progress(const Rose::Progress::Report &report) {
    printf("    %s: %3.0f%%\n", report.name.c_str(), report.completion*100);
    return true; // keep listening
}

static void
test_callback_signaled() {
    std::cout <<"test user-defined callback when progress changes\n";
    Rose::Progress::Ptr progress = Rose::Progress::instance();
    boost::thread worker(worker_main, 2, 5, boost::chrono::milliseconds(1500), progress);

    boost::chrono::milliseconds maxRate(500); // at most, one message per 500 ms; but less if reports are slow to arrive.
    progress->reportChanges(maxRate, report_progress);

    worker.join();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test that lambdas work

static void
test_lambda_signaled() {
    std::cout <<"test progress changes handled by lambda\n";
#if __cplusplus >= 201103L
    Rose::Progress::Ptr progress = Rose::Progress::instance();
    boost::thread worker(worker_main, 2, 5, boost::chrono::milliseconds(1500), progress);

    boost::chrono::milliseconds maxRate(500); // at most, one message per 500 ms; fewer if workers report slower
    progress->reportChanges(maxRate, [](const Rose::Progress::Report &report) -> bool {
            printf("    %s: %3.0f%%\n", report.name.c_str(), report.completion*100);
            return true; // keep listening
        });
    
    worker.join();
#else
    std::cout <<"    not tested (needs C++11 or better)\n";
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test recursive analyzers. Normally the progress object would be a private data member and the analysis would be some method
// such as "run" defined in a class.

static void
innerAnalysis(size_t nSteps, boost::chrono::milliseconds workPerStep, const Rose::Progress::Ptr &progress) {
    for (size_t i=0; i<nSteps; ++i) {
        std::string phase = "step_" + boost::lexical_cast<std::string>(i+1);
        progress->update(Rose::Progress::Report(phase, (double)i / nSteps));
        boost::this_thread::sleep_for(workPerStep);
    }
    progress->finished(1.0);
}

static void
outerAnalysis(boost::chrono::milliseconds workPerStep, const Rose::Progress::Ptr &progress) {
    progress->update(0/6.0);

    boost::this_thread::sleep_for(workPerStep);
    progress->update(1/6.0);

    {
        Rose::ProgressTask t(progress, "inner", 4/6.0);
        innerAnalysis(3, workPerStep, progress);
    }

    boost::this_thread::sleep_for(workPerStep);
    progress->update(5/6.0);

    boost::this_thread::sleep_for(workPerStep);
    progress->finished(6/6.0);
}

static void
test_nested_tasks() {
    std::cout <<"test nested progress\n";
    Rose::Progress::Ptr progress = Rose::Progress::instance();
    boost::thread worker(outerAnalysis, boost::chrono::seconds(1), progress);

    boost::chrono::seconds maxRate(0); //  as many progress reports as possible (not a CPU hog since it blocks)
    progress->reportChanges(maxRate, report_progress);

    worker.join();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The above examples all had one worker reporting progress and the main thread printing the progress, but this isn't
// necessary. Any number of workers can report progress, and any number of listeners can print progress.  The entire API is
// thread safe.

int
main() {
    test_poll();
    test_callback_scheduled();
    test_callback_signaled();
    test_lambda_signaled();
    test_nested_tasks();
}
