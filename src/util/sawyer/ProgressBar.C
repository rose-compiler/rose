#include "ProgressBar.h"
#include <cstdio>
#include <sstream>

namespace Sawyer {

double ProgressBarImpl::initialDelay_ = 5.0;
double ProgressBarImpl::minUpdateInterval_ = 0.1;


double ProgressBarSettings::initialDelay() {
    return ProgressBarImpl::initialDelay_;
}

void ProgressBarSettings::initialDelay(double s) {
    ProgressBarImpl::initialDelay_ = s;
}

double ProgressBarSettings::minimumUpdateInterval() {
    return ProgressBarImpl::minUpdateInterval_;
}

void ProgressBarSettings::minimumUpdateInterval(double s) {
    ProgressBarImpl::minUpdateInterval_ = s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProgressBarImpl::init() {
    static const char *clearLine = "\r\033[K";          // clear entire line of an ANSI terminal
    overridesAnsi_.isBuffered = false;
    overridesAnsi_.completionStr = clearLine;
    overridesAnsi_.interruptionStr = clearLine;
    overridesAnsi_.cancelationStr = clearLine;
    overridesAnsi_.lineTermination = "";                // empty because we erase lines instead

    if (initialDelay_ > 0)
        lastUpdateTime_ = Message::now() + initialDelay_;
}

void ProgressBarImpl::cleanup() {
    if (*stream_) {
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

std::string ProgressBarImpl::makeBar(double ratio, bool isBackward) {
    std::string prefix, bar, suffix;
    if (!prefix_.empty())
        prefix = prefix_ + ": ";
    if (shouldSpin_) {
        int centerIdx = nUpdates_ % (2*width_);
        if (centerIdx >= (int)width_)
            centerIdx = 2*width_ - centerIdx - 1;
        assert(centerIdx>=0 && (size_t)centerIdx < width_);
        int indicatorWidth = ceil(0.3 * width_);
        int indicatorIdx = centerIdx - indicatorWidth/2;
        bar = std::string(width_, nonBarChar_);
        for (int i=std::max(indicatorIdx, 0); i<std::min(indicatorIdx+indicatorWidth, (int)width_); ++i)
            bar[i] = barChar_;
    } else {
        if (showPercent_) {
            char buf[16];
            sprintf(buf, "%3.0f%% ", 100.0*ratio);
            prefix += buf;
        }
        size_t barLen = round(ratio * width_);
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

void ProgressBarImpl::updateTextMesg(double ratio) {
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
        int pct = round(ratio*10.0) * 10;
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

void ProgressBarImpl::update(double ratio, bool isBackward) {
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

void ProgressBarImpl::configUpdate(double ratio, bool isBackward) {
    update(ratio, isBackward);
}

void ProgressBarImpl::valueUpdate(double ratio, bool isBackward) {
    double curTime = Message::now();
    if (curTime - lastUpdateTime_ >= minUpdateInterval_) {
        update(ratio, isBackward);
        lastUpdateTime_ = curTime;
    }
}

} // namespace

