#ifndef Sawyer_ProgressBar_H
#define Sawyer_ProgressBar_H

#include "Message.h"
#include <cmath>
#include <sstream>

namespace Sawyer {

// used internally by the ProgressBar<> classes
class ProgressBarImpl {
public:
    double value_;                                      // between zero and one, inclusive
    size_t width_;                                      // width of bar in characters
    bool showPercent_;                                  // show the percent after the progress bar
    std::string leftEnd_, rightEnd_;                    // strings for left and right ends of progress bar
    char barChar_, nonBarChar_;                         // characters for the bar and non-bar parts
    std::string prefix_;                                // extra text before the bar (usually a short message)
    std::string suffix_;                                // extra text to show after the bar (usually the value)
    static double minUpdateInterval_;                   // min number of seconds between update
    static double initialDelay_;                        // time to delay before emitting the first message
    double lastUpdateTime_;                             // time of previous update
    Message::MesgProps overridesAnsi_;                  // properties we override from the stream_ when using color
    Message::SProxy stream_;                            // stream to which messages are sent
    size_t nUpdates_;                                   // number of times a message was emitted
    bool shouldSpin_;                                   // spin instead of progress
    Message::Mesg textMesg_;                            // message used when ANSI escape sequences are not available
    boost::optional<int> oldPercent_;                   // old percent value used when updating a non-color progress bar

    ProgressBarImpl(const Message::SProxy &stream)
        : value_(0.0), width_(15), showPercent_(true), leftEnd_("["), rightEnd_("]"), barChar_('#'), nonBarChar_('-'),
          lastUpdateTime_(0.0), stream_(stream), nUpdates_(0), shouldSpin_(false) {
        init();
    }
    ~ProgressBarImpl() {
        cleanup();
    }

    void init();
    void cleanup();                                     // deletes the progress bar from the screen
    void update(double ratio, bool backward);           // update regardless of time (if stream is enabled)
    void configUpdate(double ratio, bool backward);     // update for configuration changes
    void valueUpdate(double ratio, bool backward);      // update for changes in value
    std::string makeBar(double ratio, bool backward);   // make the bar itself
    void updateTextMesg(double ratio);                  // update the textMesg_
};

/** Global settings for progress bars. */
namespace ProgressBarSettings {
    /** Delay before first message is emitted. A relatively large delay produces fewer messages by avoiding messages when
     *  the entire task can be completed quickly.  The default is 5 seconds. This value is global, applying to all progress
     *  bars.
     * @{ */
    double initialDelay();
    void initialDelay(double s);
    /** @} */
    

    /** Minimum time between updates.  Measured in seconds.
     *  @{ */
    double minimumUpdateInterval();
    void minimumUpdateInterval(double s);
    /** @} */
} // namespace


/** Progress bars.
 *
 *  Progress bars are fully integrated with the Sawyer logging system so that they behave with respect to other messages.
 *
 *  Example usage:
 * @code
 *  void f() {
 *      int total = 200;
 *      ProgressBar<int> progress("test", total);
 *      for (int i=0; i<total; ++i, ++progress)
 *          do_some_work();
 *  }
 * @endcode
 *
 *  The progress bar is created with a name and capacity. As the progress bar is incremented the bar will increase.  Messages
 *  printed while the progress bar is active do not interfere with the progress bar. When the progress bar object is destructed
 *  the progress bar disappears. */
template<typename T>
class ProgressBar {
public:
    typedef T ValueType;
private:
    struct Position {
        ValueType leftValue, curValue, rightValue;
        Position(ValueType leftValue, ValueType curValue, ValueType rightValue)
            : leftValue(leftValue), curValue(curValue), rightValue(rightValue) {}
        bool operator==(const Position &other) const {
            return curValue==other.curValue && leftValue==other.leftValue && rightValue==other.rightValue;
        }
    };

    Position value_;
    ProgressBarImpl bar_;
    bool showValue_;

public:
    explicit ProgressBar(const Message::SProxy &stream, const std::string &name="progress")
        : value_(0, 0, 0), bar_(stream), showValue_(true) {
        bar_.shouldSpin_ = true;
        bar_.prefix_ = name;
    }
    ProgressBar(ValueType rightValue, const Message::SProxy &stream, const std::string &name="progress")
        : value_(0, 0, rightValue), bar_(stream), showValue_(true) {
        bar_.shouldSpin_ = isEmpty();
        bar_.prefix_ = name;
    }
    ProgressBar(ValueType leftValue, ValueType curValue, ValueType rightValue, const Message::SProxy &stream,
                const std::string &name="progress")
        : value_(leftValue, curValue, rightValue), bar_(stream), showValue_(true) {
        bar_.shouldSpin_ = isEmpty();
        bar_.prefix_ = name;
    }

    /** Value for the progress bar.
     *  @{ */
    ValueType value() const {
        return value_.curValue;
    }
    void value(ValueType curValue) {
        value_.curValue = curValue;
        valueUpdated();
    }

    void value(ValueType curValue, ValueType rightValue) {
        value_.curValue = curValue;
        value_.rightValue = rightValue;
        bar_.shouldSpin_ = isEmpty();
        configUpdated();
    }
    void value(ValueType leftValue, ValueType curValue, ValueType rightValue) {
        value_ = Position(leftValue, curValue, rightValue);
        bar_.shouldSpin_ = isEmpty();
        configUpdated();
    }
    /** @} */

    /** Value of progress bar as a ratio of completeness clipped between 0 and 1.  A progress bar that is backward (min value
     *  is greater than max value) also returns a value between zero and one, and also is a measurement of how far the progress
     *  bar should be drawn from the left side toward the right. */
    double ratio() const;
    
    /** True if the distance between the minimum and maximum is zero. */
    bool isEmpty() const {
        return value_.leftValue == value_.rightValue;
    }

    /** True if the minimum value is greater than the maximum value. */
    bool isBackward() const {
        return value_.leftValue > value_.rightValue;
    }

    /** Possible values. These indicate the zero and 100% end points.
     *  @{ */
    std::pair<ValueType, ValueType> domain() const {
        return std::make_pair(value_.leftValue, value_.rightValue);
    }
    void domain(const std::pair<ValueType, ValueType> &p) {
        value_.leftValue = p.first;
        value_.rightValue = p.second;
        configUpdated();
    }
    void domain(ValueType leftValue, ValueType rightValue) {
        value_.leftValue = leftValue;
        value_.rightValue = rightValue;
        configUpdated();
    }
    /** @} */

    /** Increment or decrement the progress bar.
     *  @{ */
    void increment(ValueType delta=1);
    void decrement(ValueType delta=1);
    ProgressBar& operator++() {
        increment(1);
        return *this;
    }
    ProgressBar& operator++(int) {                      // same as a pre-increment
        increment(1);
        return *this;
    }
    ProgressBar& operator--() {
        decrement(1);
        return *this;
    }
    ProgressBar& operator--(int) {                      // same as pre-decrement
        decrement(1);
        return *this;
    }
    ProgressBar& operator+=(ValueType delta) {
        increment(delta);
        return *this;
    }
    ProgressBar& operator-=(ValueType delta) {
        decrement(delta);
        return *this;
    }
    /** @} */

    /** Width of progress bar in characters at 100%
     *  @{ */
    size_t width() const {
        return bar_.width_;
    }
    void width(size_t width) {
        bar_.width_ = width;
        configUpdated();
    }
    /** @} */

    /** String to show before the beginning of the bar.  This should be something very short, like "processing input".
     * @{ */
    const std::string& prefix() const {
        return bar_.prefix_;
    }
    void prefix(const std::string &s) {
        bar_.prefix_ = s;
        configUpdated();
    }
    /** @} */

    /** Characters to use for the bar. The first is from zero to the current ratio() and the second is the character with which
     *  to fill the rest of the bar's area.  The defaults are '#' and '-'.
     *  @{ */
    std::pair<char, char> barchars() const {
        return std::make_pair(bar_.barChar_, bar_.nonBarChar_);
    }
    void barchars(char bar, char nonBar) {
        bar_.barChar_ = bar;
        bar_.nonBarChar_ = nonBar;
        configUpdated();
    }
    /** @} */

    /** Characters to use for the left and right ends of the bar.  The default is '[' and ']'.
     *  @{ */
    std::pair<std::string, std::string> endchars() const {
        return std::make_pair(bar_.leftEnd_, bar_.rightEnd_);
    }
    void endchars(const std::string &lt, const std::string &rt) {
        bar_.leftEnd_ = lt;
        bar_.rightEnd_ = rt;
        configUpdated();
    }
    /** @} */

    /** Whether to show the percent indication.  The default is true.
     * @{ */
    bool showPercent() const {
        return bar_.showPercent_;
    }
    void showPercent(bool b) {
        bar_.showPercent_ = b;
        configUpdated();
    }
    /** @} */

    /** Whether to show the current value.  The is true.
     * @{ */
    bool showValue() const {
        return showValue_;
    }
    void showValue(bool b) {
        showValue_ = b;
        configUpdated();
    }
    /** @} */

protected:
    void valueUpdated() {
        if (showValue_) {
            std::ostringstream ss;
            ss <<value_.curValue;
            bar_.suffix_ = ss.str();
        } else {
            bar_.suffix_.clear();
        }
        bar_.valueUpdate(ratio(), isBackward());
    }
    void configUpdated() {
        if (showValue_) {
            std::ostringstream ss;
            ss <<value_.curValue;
            bar_.suffix_ = ss.str();
        } else {
            bar_.suffix_.clear();
        }
        bar_.configUpdate(ratio(), isBackward());
    }
};

// try not to get negative values when subtracting because they might behave strangely if T is something weird.
template <typename T>
double ProgressBar<T>::ratio() const {
    if (isEmpty()) {
        return value_.curValue <= value_.leftValue ? 0.0 : 1.0;
    } else if (isBackward()) {
        if (value_.curValue >= value_.leftValue) {
            return 0.0;
        } else if (value_.curValue <= value_.rightValue) {
            return 1.0;
        } else {
            return 1.0 * (value_.leftValue - value_.curValue) / (value_.leftValue - value_.rightValue);
        }
    } else {
        if (value_.curValue <= value_.leftValue) {
            return 0.0;
        } else if (value_.curValue >= value_.rightValue) {
            return 1.0;
        } else {
            return 1.0 * (value_.curValue - value_.leftValue) / (value_.rightValue - value_.leftValue);
        }
    }
}

template <typename T>
void ProgressBar<T>::increment(ValueType delta) {
    ValueType oldValue = value_.curValue;
    value_.curValue += delta;
    if (oldValue!=value_.curValue)
        valueUpdated();
}

template <typename T>
void ProgressBar<T>::decrement(ValueType delta) {
    ValueType oldValue = value_.curValue;
    value_.curValue -= delta;
    if (oldValue!=value_.curValue)
        valueUpdated();
}

} // namespace

#endif
