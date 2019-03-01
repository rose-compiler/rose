// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_ProgressBar_H
#define Sawyer_ProgressBar_H

#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Sawyer.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <cmath>
#include <sstream>

namespace Sawyer {

// used internally by the ProgressBar<> classes. Since this is called only from ProgressBar objects and ProgressBar has
// a synchronized API, we don't need any synchronization at this level.
class SAWYER_EXPORT ProgressBarImpl {
public:
#include <Sawyer/WarningsOff.h>
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
    Optional<int> oldPercent_;                          // old percent value used when updating a non-color progress bar
#include <Sawyer/WarningsRestore.h>

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

/** Global settings for progress bars. 
 *
 * @sa The @ref ProgressBar class contains the top-level documentation for progress bars. */
namespace ProgressBarSettings {
    /** Delay before first message is emitted. A relatively large delay produces fewer messages by avoiding messages when
     *  the entire task can be completed quickly.  The default is 5 seconds, although some libraries that use %Sawyer change
     *  the default. This value is global, applying to all progress
     *  bars.
     * @{ */
    SAWYER_EXPORT double initialDelay();
    SAWYER_EXPORT void initialDelay(double s);
    /** @} */
    

    /** Minimum time between updates.  Measured in seconds.
     *  @{ */
    SAWYER_EXPORT double minimumUpdateInterval();
    SAWYER_EXPORT void minimumUpdateInterval(double s);
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
 *      ProgressBar<int> progress(total, mlog[MARCH], "test");
 *      for (int i=0; i<total; ++i, ++progress)
 *          do_some_work();
 *  }
 * @endcode
 *
 *  The progress bar is created with a name and capacity. As the progress bar is incremented the bar will increase.  Messages
 *  printed while the progress bar is active do not interfere with the progress bar. When the progress bar object is destroyed
 *  the progress bar disappears.
 *
 *  Progress bars should be output to the @c MARCH stream rather than @c INFO. If all progress bars follow this policy then
 *  it's easy to enable or disable them without affecting other diagnostic output.
 *
 *  The entire progress bar public interface is synchronized--it can be called from multiple threads all accessing the same
 *  object and their operations will be serialized. */
template<typename T, typename S=std::string>
class ProgressBar {
public:
    typedef S Suffix;
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

    mutable boost::mutex mutex_;                        // locks the following data members
    Position value_;
    ProgressBarImpl bar_;
    bool showValue_;
    Suffix suffix_;

public:
    /** Construct spinning progress bar.  A progress bar without a capacity results in a "spinner" that moves back and
     *  forth instead of a 0 to 100% bar. This kind of progress bar can be used when the number of iterations to complete
     *  a task is unknown. */
    explicit ProgressBar(const Message::SProxy &stream, const std::string &name="progress")
        : value_(0, 0, 0), bar_(stream), showValue_(true) {
        bar_.shouldSpin_ = true;
        bar_.prefix_ = name;
    }

    /** Construct a progress bar incrementing from zero to some limit.  The progress bar is initialized to zero and constructed
     *  so that when the value reaches @p rightValue the bar will read 100%. */
    ProgressBar(ValueType rightValue, const Message::SProxy &stream, const std::string &name="progress")
        : value_(0, 0, rightValue), bar_(stream), showValue_(true) {
        bar_.shouldSpin_ = isEmptyNS();
        bar_.prefix_ = name;
    }

    /** Construct a progress bar with left and right limits.  The progress bar is set so that @p leftValue represents the zero
     *  percent point and rightValue represents the 100% point.  The @p curValue is the current value of the progress bar,
     *  which need not be between the the limits (the percent indications will be clipped the the specified interval). */
    ProgressBar(ValueType leftValue, ValueType curValue, ValueType rightValue, const Message::SProxy &stream,
                const std::string &name="progress")
        : value_(leftValue, curValue, rightValue), bar_(stream), showValue_(true) {
        bar_.shouldSpin_ = isEmptyNS();
        bar_.prefix_ = name;
    }

    /** Value for the progress bar.
     *  @{ */
    ValueType value() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return value_.curValue;
    }
    void value(ValueType curValue) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        value_.curValue = curValue;
        valueUpdatedNS();
    }

    void value(ValueType curValue, ValueType rightValue) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        value_.curValue = curValue;
        value_.rightValue = rightValue;
        bar_.shouldSpin_ = isEmptyNS();
        valueUpdatedNS();
    }
    void value(ValueType leftValue, ValueType curValue, ValueType rightValue) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        value_ = Position(leftValue, curValue, rightValue);
        bar_.shouldSpin_ = isEmptyNS();
        valueUpdatedNS();
    }
    /** @} */

    /** Property: suffix.
     *
     *  This user-defined object provides the suffix for the progress bar.  It will be appended to the progress bar string with
     *  the <code>std::ostream</code>'s <code><<</code> operator. It should not contain any line feeds.
     *
     *  @{ */
    Suffix suffix() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return suffix_;
    }
    Suffix suffix() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return suffix_;
    }
    ProgressBar& suffix(const Suffix &suffix) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        suffix_ = suffix;
        return *this;
    }
    /** @} */

    /** Value of progress bar as a ratio of completeness clipped between 0 and 1.  A progress bar that is backward (min value
     *  is greater than max value) also returns a value between zero and one, and also is a measurement of how far the progress
     *  bar should be drawn from the left side toward the right. */
    double ratio() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return ratioNS();
    }
    
    /** True if the distance between the minimum and maximum is zero. */
    bool isEmpty() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return isEmptyNS();
    }

    /** True if the minimum value is greater than the maximum value. */
    bool isBackward() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return isBackwardNS();
    }

    /** Possible values. These indicate the zero and 100% end points.
     *  @{ */
    std::pair<ValueType, ValueType> domain() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return std::make_pair(value_.leftValue, value_.rightValue);
    }
    void domain(const std::pair<ValueType, ValueType> &p) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        value_.leftValue = p.first;
        value_.rightValue = p.second;
        configUpdatedNS();
    }
    void domain(ValueType leftValue, ValueType rightValue) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        value_.leftValue = leftValue;
        value_.rightValue = rightValue;
        configUpdatedNS();
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
        boost::lock_guard<boost::mutex> lock(mutex_);
        return bar_.width_;
    }
    void width(size_t width) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        bar_.width_ = width;
        configUpdatedNS();
    }
    /** @} */

    /** String to show before the beginning of the bar.  This should be something very short, like "processing input".
     * @{ */
    const std::string& prefix() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return bar_.prefix_;
    }
    void prefix(const std::string &s) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        bar_.prefix_ = s;
        configUpdatedNS();
    }
    /** @} */

    /** Characters to use for the bar. The first is from zero to the current ratio() and the second is the character with which
     *  to fill the rest of the bar's area.  The defaults are '#' and '-'.
     *  @{ */
    std::pair<char, char> barchars() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return std::make_pair(bar_.barChar_, bar_.nonBarChar_);
    }
    void barchars(char bar, char nonBar) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        bar_.barChar_ = bar;
        bar_.nonBarChar_ = nonBar;
        configUpdatedNS();
    }
    /** @} */

    /** Characters to use for the left and right ends of the bar.  The default is '[' and ']'.
     *  @{ */
    std::pair<std::string, std::string> endchars() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return std::make_pair(bar_.leftEnd_, bar_.rightEnd_);
    }
    void endchars(const std::string &lt, const std::string &rt) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        bar_.leftEnd_ = lt;
        bar_.rightEnd_ = rt;
        configUpdatedNS();
    }
    /** @} */

    /** Whether to show the percent indication.  The default is true.
     * @{ */
    bool showPercent() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return bar_.showPercent_;
    }
    void showPercent(bool b) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        bar_.showPercent_ = b;
        configUpdatedNS();
    }
    /** @} */

    /** Whether to show the current value.  The is true.
     * @{ */
    bool showValue() const {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return showValue_;
    }
    void showValue(bool b) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        showValue_ = b;
        configUpdatedNS();
    }
    /** @} */

private:
    bool isEmptyNS() const {                            // "NS" means "not synchronized" -- the caller must do that.
        return value_.leftValue == value_.rightValue;
    }

    double ratioNS() const;

    bool isBackwardNS() const {
        return value_.leftValue > value_.rightValue;
    }

    void valueUpdatedNS() {
        if (showValue_) {
            std::ostringstream ss;
            ss <<value_.curValue <<suffix_;
            bar_.suffix_ = ss.str();
        } else {
            bar_.suffix_.clear();
        }
        bar_.valueUpdate(ratioNS(), isBackwardNS());
    }

    void configUpdatedNS() {
        if (showValue_) {
            std::ostringstream ss;
            ss <<value_.curValue <<suffix_;
            bar_.suffix_ = ss.str();
        } else {
            bar_.suffix_.clear();
        }
        bar_.configUpdate(ratioNS(), isBackwardNS());
    }
};

// try not to get negative values when subtracting because they might behave strangely if T is something weird.
template <typename T, typename S>
double ProgressBar<T, S>::ratioNS() const {
    if (isEmptyNS()) {
        return value_.curValue <= value_.leftValue ? 0.0 : 1.0;
    } else if (isBackwardNS()) {
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

template <typename T, typename S>
void ProgressBar<T, S>::increment(ValueType delta) {
    boost::lock_guard<boost::mutex> lock(mutex_);
    ValueType oldValue = value_.curValue;
    value_.curValue += delta;
    if (oldValue!=value_.curValue)
        valueUpdatedNS();
}

template <typename T, typename S>
void ProgressBar<T, S>::decrement(ValueType delta) {
    boost::lock_guard<boost::mutex> lock(mutex_);
    ValueType oldValue = value_.curValue;
    value_.curValue -= delta;
    if (oldValue!=value_.curValue)
        valueUpdatedNS();
}

} // namespace

#endif
