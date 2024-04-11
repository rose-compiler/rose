#include <Rose/Progress.h>

#include <boost/algorithm/string/join.hpp>

namespace Rose {

// class method
Progress::Ptr
Progress::instance() {
    Progress *self = new Progress;
    self->reports_.push_back(Report(0.0));
    return Ptr(self);
}

void
Progress::update(double completion, double maximum) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    reports_.back().completion = completion;
    reports_.back().maximum = maximum;
    ++reportNumber_;
    reportAge_.restart();
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
}

void
Progress::update(const Report &report) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    reports_.back() = report;
    ++reportNumber_;
    reportAge_.restart();
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
}

Progress::Report
Progress::push() {
    ASSERT_forbid(reports_.empty());
    Report retval = reports_.back();
    reports_.push_back(Report());
    ++reportNumber_;
    reportAge_.restart();
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
    return retval;
}

Progress::Report
Progress::push(double completion, double maximum) {
    return push(Report(completion, maximum));
}

Progress::Report
Progress::push(const Report &report) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    Report retval = reports_.back();
    reports_.back() = report;
    reports_.push_back(Report());
    ++reportNumber_;
    reportAge_.restart();
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
    return retval;
}

void
Progress::pop() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    if (reports_.size() > 1) {
        reports_.pop_back();
        ++reportNumber_;
    } else {
        reportNumber_ = TERMINATING;
    }
    reportAge_.restart();
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
}

void
Progress::pop(double completion, double maximum) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    if (reports_.size() > 1) {
        reports_.pop_back();
        ++reportNumber_;
    } else {
        reportNumber_ = TERMINATING;
    }
    reports_.back().completion = completion;
    reports_.back().maximum = maximum;
    reportAge_.restart();
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
}

void
Progress::pop(const Report &report) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    if (reports_.size() > 1) {
        reports_.pop_back();
        ++reportNumber_;
    } else {
        reportNumber_ = TERMINATING;
    }
    reports_.back() = report;
    reportAge_.restart();
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
}
        
void
Progress::finished() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    if (1 == reports_.size()) {
        reportNumber_ = TERMINATING;
#if SAWYER_MULTI_THREADED
        cv_.notify_all();
#endif
    }
}

void
Progress::finished(double completion, double maximum) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    reports_.back().completion = completion;
    reports_.back().maximum = maximum;
    if (reports_.size() > 1) {
        ++reportNumber_;
    } else {
        reportNumber_ = TERMINATING;
    }
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
}

void
Progress::finished(const Report &report) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    reports_.back() = report;
    if (reports_.size() > 1) {
        ++reportNumber_;
    } else {
        reportNumber_ = TERMINATING;
    }
#if SAWYER_MULTI_THREADED
    cv_.notify_all();
#endif
}

std::pair<Progress::Report, double /*seconds*/>
Progress::reportLatest(const std::string &nameSeparator) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    std::pair<Report, double> retval =  std::make_pair(reports_.back(), reportAge_.report());
    retval.first.name = reportNameNS(nameSeparator);
    return retval;
}

bool
Progress::isFinished() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(reports_.empty());
    return TERMINATING == reportNumber_;
}

std::string
Progress::reportNameNS(const std::string &nameSeparator) const {
    ASSERT_forbid(reports_.empty());
    if (1 == reports_.size() || nameSeparator.empty())
        return reports_.back().name;
    std::vector<std::string> names;
    for (const Report &report: reports_) {
        if (!report.name.empty())
            names.push_back(report.name);
    }
    return boost::join(names, nameSeparator);
}

} // namespace
