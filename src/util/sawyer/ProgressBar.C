// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#include <sawyer/ProgressBar.h>

#include <boost/numeric/conversion/cast.hpp>
#include <cmath>
#include <cstdio>
#include <sawyer/Sawyer.h>
#include <sstream>

namespace Sawyer {

SAWYER_EXPORT double ProgressBarImpl::initialDelay_ = 5.0;
SAWYER_EXPORT double ProgressBarImpl::minUpdateInterval_ = 0.1;


SAWYER_EXPORT double
ProgressBarSettings::initialDelay() {
    return ProgressBarImpl::initialDelay_;
}

SAWYER_EXPORT void
ProgressBarSettings::initialDelay(double s) {
    ProgressBarImpl::initialDelay_ = s;
}

SAWYER_EXPORT double
ProgressBarSettings::minimumUpdateInterval() {
    return ProgressBarImpl::minUpdateInterval_;
}

SAWYER_EXPORT void
ProgressBarSettings::minimumUpdateInterval(double s) {
    ProgressBarImpl::minUpdateInterval_ = s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
ProgressBarImpl::init() {
    static const char *clearLine = "\r\033[K";          // clear entire line of an ANSI terminal
    overridesAnsi_.isBuffered = false;
    overridesAnsi_.completionStr = clearLine;
    overridesAnsi_.interruptionStr = clearLine;
    overridesAnsi_.cancelationStr = clearLine;
    overridesAnsi_.lineTermination = "";                // empty because we erase lines instead
    lastUpdateTime_ = Message::now() + initialDelay_ - minUpdateInterval_;
}

SAWYER_EXPORT void
ProgressBarImpl::cleanup() {
    if (*stream_ && nUpdates_>0) {
        Message::BakedDestinations baked;
        stream_->destination()->bakeDestinations(stream_->properties(), baked);
        textMesg_.complete();
        for (size_t i=0; i<baked.size(); ++i) {
            if (baked[i].second.useColor) {
                Message::MesgProps props = overridesAnsi_.merge(baked[i].second);
                Message::Mesg mesg(props, "done");      // any non-empty message
                baked[i].first->post(mesg, props);
            } else {
                baked[i].first->post(textMesg_, baked[i].second);
            }
        }
    }
}

SAWYER_EXPORT std::string
ProgressBarImpl::makeBar(double ratio, bool isBackward) {
    std::string prefix, bar, suffix;
    if (!prefix_.empty())
        prefix = prefix_ + ": ";
    if (shouldSpin_) {
        int centerIdx = nUpdates_ % (2*width_);
        if (centerIdx >= (int)width_)
            centerIdx = 2*width_ - centerIdx - 1;
        assert(centerIdx>=0 && (size_t)centerIdx < width_);
        int indicatorWidth = boost::numeric::converter<int, double>::convert(ceil(0.3 * width_));
        int indicatorIdx = centerIdx - indicatorWidth/2;
        bar = std::string(width_, nonBarChar_);
        for (int i=std::max(indicatorIdx, 0); i<std::min(indicatorIdx+indicatorWidth, (int)width_); ++i)
            bar[i] = barChar_;
    } else {
        if (showPercent_) {
            char buf[16];
#include <sawyer/WarningsOff.h>
            sprintf(buf, "%3.0f%% ", 100.0*ratio);
#include <sawyer/WarningsRestore.h>
            prefix += buf;
        }
        // Microsoft doesn't define round(double) in <cmath>
        size_t barLen = boost::numeric::converter<size_t, double>::convert(floor(ratio * width_ + 0.5));
        if (isBackward) {
            bar += std::string(width_-barLen, nonBarChar_) + std::string(barLen, barChar_);
        } else {
            bar += std::string(barLen, barChar_) + std::string(width_-barLen, nonBarChar_);
        }
    }
    if (!suffix_.empty())
        suffix = " " + suffix_;

    return prefix + leftEnd_ + bar + rightEnd_ + suffix;
}

SAWYER_EXPORT void
ProgressBarImpl::updateTextMesg(double ratio) {
    if (shouldSpin_) {
        if (textMesg_.isEmpty() || textMesg_.text().size() > width_) {
            textMesg_ = Message::Mesg();
            if (!prefix_.empty())
                textMesg_.insert(prefix_ + ": ");
        }
        if (suffix_.empty()) {
            textMesg_.insert('.');
        } else {
            textMesg_.insert(" " + suffix_);
        }
    } else {
        int pct = boost::numeric::converter<int, double>::convert(floor(ratio * 10.0 + 0.5) * 10);
        if (!oldPercent_ || pct != *oldPercent_) {
            bool needElipsis = !textMesg_.isEmpty();
            if (textMesg_.isEmpty() || textMesg_.text().size() > width_) {
                textMesg_ = Message::Mesg();
                if (!prefix_.empty())
                    textMesg_.insert(prefix_ + ": ");
                needElipsis = false;
            }
            oldPercent_ = pct;
            if (needElipsis)
                textMesg_.insert("...");
            std::ostringstream ss;
            ss <<pct <<"%";
            if (suffix_.empty()) {
                textMesg_.insert(ss.str());
            } else {
                textMesg_.insert(suffix_ + "(" + ss.str() + ")");
            }
        }
    }
}

SAWYER_EXPORT void
ProgressBarImpl::update(double ratio, bool isBackward) {
    if (*stream_ && width_>0) {
        Message::BakedDestinations baked;
        stream_->destination()->bakeDestinations(stream_->properties(), baked);
        if (!baked.empty()) {
            for (size_t i=0; i<baked.size(); ++i) {
                if (baked[i].second.useColor) {
                    Message::MesgProps props = overridesAnsi_.merge(baked[i].second);
                    Message::Mesg mesg(props);
                    mesg.insert(makeBar(ratio, isBackward));
                    baked[i].first->post(mesg, props);
                } else {
                    updateTextMesg(ratio);
                    baked[i].first->post(textMesg_, baked[i].second);
                }
            }
            ++nUpdates_;
        }
    }
}

SAWYER_EXPORT void
ProgressBarImpl::configUpdate(double ratio, bool isBackward) {
    update(ratio, isBackward);
}

SAWYER_EXPORT void
ProgressBarImpl::valueUpdate(double ratio, bool isBackward) {
    double curTime = Message::now();
    if (curTime - lastUpdateTime_ >= minUpdateInterval_) {
        update(ratio, isBackward);
        lastUpdateTime_ = curTime;
    }
}

} // namespace

