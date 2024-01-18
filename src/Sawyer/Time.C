// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#include <Sawyer/Time.h>

#include <Sawyer/Assert.h>
#include <Sawyer/Parse.h>

#include <boost/format.hpp>
#include <regex>
#include <type_traits>

namespace Sawyer {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private support functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
isLeapYear(unsigned year) {
    ASSERT_require(year <= 9999);
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static unsigned
daysInMonth(unsigned year, unsigned month) {
    // Assume this formula is even correct for years before 1583 and after 9999.
    ASSERT_require(month >= 1 && month <= 12);
    if (2 == month) {
        return isLeapYear(year) ? 29 : 28;
    } else {
        return 4 == month || 6 == month || 9 == month || 11 == month ? 30 : 31;
    }
}

// Conditionally increment a value.
//    Value CarryIn Return CarryOut
//    -----------------------------
//    Exists False  Value    False
//    Exists True   Value+1  Overflowed?
//    Absent False  zero     True
//    Absent True   zero     True
template<class T>
static std::pair<T, bool>
increment(const Optional<T> &value, const T maxValue, T base, bool carryIn) {
    if (value) {
        if (carryIn) {
            const T next = *value >= maxValue ? base : *value + T{1};
            return std::make_pair(next, next < *value);
        } else {
            return std::make_pair(*value, false);
        }
    } else {
        const T next = base;
        return std::make_pair(next, true);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class Time
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Time::Time() {}

Time
Time::now() {
    time_t now = std::time(nullptr);
    std::tm tm;
#ifdef BOOST_WINDOWS
    tm = *gmtime(&now);                                 // not thread safe, but Windows doesn't have gmtime_r.
#else
    gmtime_r(&now, &tm);
#endif

    Time t;
    t.year_ = 1900 + tm.tm_year;
    t.month_ = 1 + tm.tm_mon;
    t.day_ = tm.tm_mday;
    t.hour_ = tm.tm_hour;
    t.minute_ = tm.tm_min;
    t.second_ = tm.tm_sec;
    t.tz_hour_ = 0;
    t.tz_minute_ = 0;
    return t;
}

Result<Time, std::string>
Time::parse(const std::string &origStr) {
    // No template parameter deduction in constructors before C++17, so make aliases
    using Error = Sawyer::Error<std::string>;
    using Ok = Sawyer::Ok<Time>;

    Time t;
    std::smatch found;
    std::string str = origStr;

    // Parse the date part if present
    std::regex basicDateRe("([0-9]{8})(.*)");
    if (std::regex_match(str, found /*out*/, basicDateRe)) {
        // yyyy         (handled below)
        // yyyymm       (not allowed by standard)
        // yyyymmdd
        const std::string s = found.str(1);
        ASSERT_require(s.size() == 8);
        t.year_ = *Sawyer::parse<unsigned>(s.substr(0, 4));
        t.month_ = *Sawyer::parse<unsigned>(s.substr(4, 2));
        t.day_ = *Sawyer::parse<unsigned>(s.substr(6, 2));
        str = found.str(2);
    } else {
        std::regex extendedDateRe("([0-9]{4}(-[0-9]{2}(-[0-9]{2})?)?)(.*)");
        if (std::regex_match(str, found /*out*/, extendedDateRe)) {
            // yyyy
            // yyyy-mm
            // yyyy-mm-dd
            const std::string s = found.str(1);
            ASSERT_require(s.size() == 4 || s.size() == 7 || s.size() == 10);
            t.year_ = *Sawyer::parse<unsigned>(s.substr(0, 4));
            if (s.size() >= 7) {
                t.month_ = *Sawyer::parse<unsigned>(s.substr(5, 2));
                if (s.size() >= 10)
                    t.day_ = *Sawyer::parse<unsigned>(s.substr(8, 2));
            }
            str = found.str(4);
        }
    }

    std::regex basicTimeRe1(std::string(t.hasDate() ? " ?T" : "T") + "([0-9]{4}([0-9]{2})?)(.*)");
    if (std::regex_match(str, found /*out*/, basicTimeRe1)) {
        // Thh          (handled below)
        // Thhmm
        // Thhmmss
        const std::string s = found.str(1);
        ASSERT_require(s.size() == 4 || s.size() == 6);
        t.hour_ = *Sawyer::parse<unsigned>(s.substr(0, 2));
        t.minute_ = *Sawyer::parse<unsigned>(s.substr(2, 2));
        if (s.size() >= 6)
            t.second_ = *Sawyer::parse<unsigned>(s.substr(4, 2));
        str = found.str(3);
    } else {
        std::regex extendedTimeRe1(std::string(t.hasDate() ? " ?T" : "T") + "([0-9]{2}(:[0-9]{2}(:[0-9]{2})?)?)(.*)");
        if (std::regex_match(str, found /*out*/, extendedTimeRe1)) {
            // Thh
            // Thh:mm
            // Thh:mm:ss
            const std::string s = found.str(1);
            ASSERT_require(s.size() == 2 || s.size() == 5 || s.size() == 8);
            t.hour_ = *Sawyer::parse<unsigned>(s.substr(0, 2));
            if (s.size() >= 5) {
                t.minute_ = *Sawyer::parse<unsigned>(s.substr(3, 2));
                if (s.size() >= 8)
                    t.second_ = *Sawyer::parse<unsigned>(s.substr(6, 2));
            }
            str = found.str(4);
        } else {
            std::regex extendedTimeRe2(std::string(t.hasDate() ? " " : "") + "([0-9]{4}(:[0-9]{2})?)(.*)");
            if (std::regex_match(str, found /*out*/, extendedTimeRe2)) {
                // hh           (not allowed by standard)
                // hh:mm
                // hh:mm:ss
                const std::string s = found.str(1);
                ASSERT_require(s.size() == 5 || s.size() == 8);
                t.hour_ = *Sawyer::parse<unsigned>(s.substr(0, 2));
                t.minute_ = *Sawyer::parse<unsigned>(s.substr(3, 2));
                if (s.size() >= 8)
                    t.second_ = *Sawyer::parse<unsigned>(s.substr(6, 2));
                str = found.str(3);
            }
        }
    }

    // Parse the time part if present
    //                       1        2        3             4
    std::regex basicTimeRe("T([0-9]{2}([0-9]{2}([0-9]{2})?)?)(.*)");
    if (std::regex_match(str, found /*out*/, basicTimeRe)) {
        // Thhmmss
        // hhmmss (not allowed)
        // Thhmm
        // hhmm (not allowed)
        // Thh
        // hh (handled below)
        const std::string s = found.str(1);
        ASSERT_require(s.size() == 2 || s.size() == 4 || s.size() == 6);
        t.hour_ = *Sawyer::parse<unsigned>(s.substr(0, 2));
        if (s.size() >= 5) {
            t.minute_ = *Sawyer::parse<unsigned>(s.substr(4, 2));
            if (s.size() >= 8)
                t.second_ = *Sawyer::parse<unsigned>(s.substr(4, 2));
        }
        str = found.str(4);
    } else {
        //                                                    1        1         2        3         4              5
        std::regex extendedTimeRe(std::string(t.hasDate()?"([ T])":"(T?)") + "([0-9]{2}(:[0-9]{2}(:[0-9]{2})?)?)(.*)");
        if (std::regex_match(str, found /*out*/, extendedTimeRe)) {
            // Thh:mm:ss
            // Thh:mm
            // Thh
            // hh:mm:ss
            // hh:mm
            // hh
            const std::string s = found.str(2);
            ASSERT_require(s.size() == 2 || s.size() == 5 || s.size() == 8);
            t.hour_ = *Sawyer::parse<unsigned>(s.substr(0, 2));
            if (s.size() >= 5) {
                t.minute_ = *Sawyer::parse<unsigned>(s.substr(3, 2));
                if (s.size() >= 8)
                    t.second_ = *Sawyer::parse<unsigned>(s.substr(6, 2));
            }
            str = found.str(5);
        }
    }

    // Parse time zone part if present
    if (str.empty()) {
        // no zone
    } else if (str == "Z") {
        // same as +0000
        t.tz_hour_ = 0;
        t.tz_minute_ = 0;
    } else {
        std::regex extendedZoneRe("([-+][0-9]{2}(:[0-9]{2})?)");
        if (std::regex_match(str, found /*out*/, extendedZoneRe)) {
            // +hh:mm
            // -hh:mm
            // +hh
            // -hh
            // -00    (not allowed, use +00)
            // -00:00 (not allowed, use +00:00)
            const std::string s = found.str(1);
            ASSERT_require(s.size() == 3 || s.size() == 6);
            t.tz_hour_ = *Sawyer::parse<int>(s.substr(0, 3));
            if (s.size() >= 6)
                t.tz_minute_ = *Sawyer::parse<int>(s.substr(0, 1) + s.substr(4, 2));
            if ('-' == s[0] && 0 == *t.tz_hour_ && 0 == t.tz_minute_.orElse(0))
                return Error("timezone cannot be \"" + s + "\"");

        } else {
            std::regex basicZoneRe("([-+][0-9]{4})");
            if (std::regex_match(str, found /*out*/, basicZoneRe)) {
                // +hhmm
                // -hhmm
                // +hh  (handled above)
                // -hh  (handled above)
                // -0000 (not allowed, use +0000)
                const std::string s = found.str(1);
                ASSERT_require(s.size() == 5);
                t.tz_hour_ = *Sawyer::parse<int>(s.substr(0, 3));
                t.tz_minute_ = *Sawyer::parse<int>(s.substr(0, 1) + s.substr(3, 2));
                if ("-0000" == s)
                    return Error("timezone cannot be \"-0000\"");
            } else {
                return Error("invalid time specification \"" + origStr + "\"");
            }
        }
    }

    // Check ranges
    if (t.year_.orElse(1900) < 1583)
        return Error("year must be 1583 or later in \"" + origStr + "\"");
    if (t.month_ && (*t.month_ < 1 || *t.month_ > 12))
        return Error("month is out of range in \"" + origStr + "\"");
    if (t.day_) {
        const unsigned m = daysInMonth(*t.year_, *t.month_);
        if (*t.day_ < 1 || *t.day_ > m)
            return Error("day of month is out of range in \"" + origStr + "\"");
    }
    if (t.hour_.orElse(0) > 23)
        return Error("hour is out of range in \"" + origStr + "\"");
    if (t.minute_.orElse(0) > 59)
        return Error("minute is out of range in \"" + origStr + "\"");
    if (t.second_.orElse(0) > 60)
        return Error("second is out of range in \"" + origStr + "\"");
    if (t.tz_hour_ && (*t.tz_hour_ < -23 || *t.tz_hour_ > 23))
        return Error("timezone hour is out of range in \"" + origStr + "\"");
    if (t.tz_minute_ && (*t.tz_minute_ < -59 || *t.tz_minute_ > 59))
        return Error("timezone minute is out of range in \"" + origStr + "\"");

    return Ok(t);
}

bool
Time::isEmpty() const {
    // do not check for timezone
    return !hasDate() && !hasTime();
}

bool
Time::hasDate() const {
    return !!year_;
}

bool
Time::hasTime() const {
    return !!hour_;
}

bool
Time::hasZone() const {
    return !!tz_hour_;
}

bool
Time::hasSpecificDate() const {
    return !!day_;
}

bool
Time::hasSpecificTime() const {
    return hasSpecificDate() && !!second_;
}

Time
Time::resolve(const Time &dflt) const {
    Time t;
    t.year_ = year_.orElse(dflt.year_);
    t.month_ = month_.orElse(dflt.month_);
    t.day_ = day_.orElse(dflt.day_);
    t.hour_ = hour_.orElse(dflt.hour_);
    t.minute_ = minute_.orElse(dflt.minute_);
    t.second_ = second_.orElse(dflt.second_);

    if (!t.tz_hour_) {
        t.tz_hour_ = dflt.tz_hour_;
        t.tz_minute_ = dflt.tz_minute_;
    }

    return t;
}

Time
Time::lowerBound() const {
    Time t;

    if (hasTime() || hasDate()) {
        t.second_ = second_.orElse(0);
        t.minute_ = minute_.orElse(0);
        t.hour_ = hour_.orElse(0);
    }

    if (hasDate()) {
        t.day_ = day_.orElse(1);
        t.month_ = month_.orElse(1);
        t.year_ = *year_;
    }

    t.tz_hour_ = tz_hour_;
    t.tz_minute_ = tz_minute_;
    return t;
}

Result<Time, std::string>
Time::upperBound() const {
    // No template parameter deduction in constructors before C++17, so make aliases
    using Error = Sawyer::Error<std::string>;
    using Ok = Sawyer::Ok<Time>;

    Time t;

    // Update the time part if present.
    //    12:30:45 -> 12:30:46
    //    12:30:59 -> 12:31:00
    //    12:30:60 -> 12:31:00 (leap second)
    //
    //    12:30    -> 12:31:00
    //    12:59    -> 13:00:00
    //    23:59    -> 00:00:00 (and increment date if present)
    //
    //    12       -> 13:00:00
    //    23       -> 00:00:00 (and increment date if present)
    const auto seconds = increment(second_, 59u, 0u, true);
    t.second_ = seconds.first;

    const auto minutes = increment(minute_, 59u, 0u, seconds.second);
    t.minute_ = minutes.first;

    const auto hours = increment(hour_, 23u, 0u, minutes.second);
    t.hour_ = hours.first;

    // Update the date part if present
    if (hasDate()) {
        const auto days = day_ ?
                          increment(day_, daysInMonth(*year_, *month_), 1u, hours.second) :
                          std::make_pair(1u, true);
        t.day_ = days.first;

        const auto months = increment(month_, 12u, 1u, days.second);
        t.month_ = months.first;

        const auto years = increment(year_, 9999u, 0u, months.second);
        t.year_ = years.first;
        if (years.second)
            return Error("cannot represent years greater than 9999");
    }

    t.tz_hour_ = tz_hour_;
    t.tz_minute_ = tz_minute_;
    return Ok(t);
}

Time
Time::noDate() const {
    Time t;
    t.hour_ = hour_;
    t.minute_ = minute_;
    t.second_ = second_;
    t.tz_hour_ = tz_hour_;
    t.tz_minute_ = tz_minute_;
    return t;
}

Time
Time::noTime() const {
    Time t;
    t.day_ = day_;
    t.month_ = month_;
    t.year_ = year_;
    t.tz_hour_ = tz_hour_;
    t.tz_minute_ = tz_minute_;
    return t;
}

Time
Time::noZone() const {
    Time t;
    t.hour_ = hour_;
    t.minute_ = minute_;
    t.second_ = second_;
    t.day_ = day_;
    t.month_ = month_;
    t.year_ = year_;
    return t;
}

const Optional<unsigned>&
Time::year() const {
    return year_;
}

const Optional<unsigned>&
Time::month() const {
    return month_;
}

const Optional<unsigned>&
Time::day() const {
    return day_;
}

const Optional<unsigned>&
Time::hour() const {
    return hour_;
}

const Optional<unsigned>&
Time::minute() const {
    return minute_;
}

const Optional<unsigned>&
Time::second() const {
    return second_;
}

const Optional<int>&
Time::timeZoneHour() const {
    return tz_hour_;
}

const Optional<int>&
Time::timeZoneMinute() const {
    return tz_minute_;
}

// Normalize seconds by borrowing from or carrying to the minutes
void
Time::normalizeSecond() {
    if (second_ && (int)*second_ < 0) {
        ASSERT_require(minute_);
        minute_ = (int)*minute_ + (int)*second_ / 60 - 1;
        second_ = 60 + (int)*second_ % 60;
    } else if (second_.orElse(0) >= 60) {
        ASSERT_require(minute_);
        minute_ = *minute_ + *second_ / 60;
        second_ = *second_ % 60;
    }
    ASSERT_require(second_.orElse(0) < 60);
}

// Normalize minutes by borrowing from or carrying to the hours
void
Time::normalizeMinute() {
    if (minute_ && (int)*minute_ < 0) {
        ASSERT_require(hour_);
        hour_ = (int)*hour_ + (int)*minute_ / 60 - 1;
        minute_ = 60 + (int)*minute_ % 60;
    } else if (minute_.orElse(0) >= 60) {
        ASSERT_require(hour_);
        hour_ = *hour_ + *minute_ / 60;
        minute_ = *minute_ % 60;
    }
    ASSERT_require(minute_.orElse(0) < 60);
}

// Normalize hours by borrowing from or carrying to the day (if any)
void
Time::normalizeHour() {
    if (hour_ && (int)*hour_ < 0) {
        if (day_)
            day_ = (int)*day_ + (int)*hour_ / 24 - 1;
        hour_ = 24 + (int)*hour_ % 24;
    } else if (hour_.orElse(0) >= 24) {
        if (day_)
            day_ = *day_ + *hour_ / 24;
        hour_ = *hour_ % 24;
    }
    ASSERT_require(hour_.orElse(0) < 24);
}

// Normalize the month by borrowing from or carrying to the year.
void
Time::normalizeMonth() {
    if (month_ && (int)*month_ < 1) {
        const int zmonth = (int)*month_ - 1;
        ASSERT_require(year_);
        year_ = (int)*year_ + zmonth / 12 - 1;
        month_ = 13 + zmonth % 12;
    } else if (month_.orElse(0) > 12) {
        const int zmonth = *month_ - 1;
        ASSERT_require(year_);
        year_ = *year_ + zmonth / 12;
        month_ = zmonth % 12 + 1;
    }
    ASSERT_require(month_.orElse(1) >= 1 && month_.orElse(1) <= 12);
}

// Normalize date and/or time. After calling this, the year might be out of range and should be checked.
void
Time::normalize() {
    normalizeSecond();
    normalizeMinute();
    normalizeHour();

    // Normalize days
    if (day_) {
        ASSERT_require(month_);
        ASSERT_require(year_);

        // Normalize negative days by borrowing from the month
        while ((int)*day_ < 1) {
            month_ = *month_ - 1;
            normalizeMonth();
            day_ = *day_ + daysInMonth(*year_, *month_);
        }

        // Normalize overflow days by carrying to the month
        while (true) {
            normalizeMonth();
            const unsigned dim = daysInMonth(*year_, *month_);
            if (*day_ > dim) {
                day_ = *day_ - dim;
                month_ = *month_ + 1;
            } else {
                break;
            }
        }
    } else {
        normalizeMonth();
    }
}

Result<Time, std::string>
Time::toZulu() const {
    using Ok = Sawyer::Ok<Time>;
    using Error = Sawyer::Error<std::string>;

    if (!hasTime() || !hasZone())
        return Ok(*this);

    Time result = *this;
    result.tz_minute_ = tz_minute_.andThen(0);
    result.tz_hour_ = 0;

    if (result.minute_ && tz_minute_)
        result.minute_ = (int)*result.minute_ - *tz_minute_;
    if (result.hour_ && tz_hour_)
        result.hour_ = (int)*result.hour_ - *tz_hour_;

    result.normalize();
    if (result.year_.orElse(1583) < 1583)
        return Error("cannot represent years before 1583");
    if (result.year_.orElse(9999) > 9999)
        return Error("cannot represent years after 9999");

    return Ok(result);
}

std::string
Time::toString() const {
    std::string s;

    // Date
    if (year_)
        s += (boost::format("%04d") % *year_).str();
    if (month_)
        s += (boost::format("-%02d") % *month_).str();
    if (day_)
        s += (boost::format("-%02d") % *day_).str();

    // Date-time separator
    if (hasDate() && hasTime()) {
        if (!month_ && !minute_) {
            s += "T";
        } else if (!minute_) {
            s += " T";
        } else {
            s += " ";
        }
    } else if (hasTime() && !minute_) {
        s += "T";
    }

    // Time
    if (hour_)
        s += (boost::format("%02d") % *hour_).str();
    if (minute_)
        s += (boost::format(":%02d") % *minute_).str();
    if (second_)
        s += (boost::format(":%02d") % *second_).str();

    // Zone
    if (tz_hour_) {
        if (0 == *tz_hour_ && tz_minute_ && 0 == *tz_minute_) {
            s += "Z";
        } else {
            if (0 == *tz_hour_) {
                s += tz_minute_ && *tz_minute_ < 0 ? "-00" : "+00";
            } else {
                s += (boost::format("%+03d") % *tz_hour_).str();
            }
            if (tz_minute_)
                s += (boost::format(":%02d") % (*tz_minute_ < 0 ? -*tz_minute_ : *tz_minute_)).str();
        }
    }

    return s;
}

Result<time_t, std::string>
Time::toUnix() const {
    // No template parameter deduction in constructors before C++17, so make aliases
    using Error = Sawyer::Error<std::string>;
    using Ok = Sawyer::Ok<time_t>;

    if (!hasSpecificTime())
        return Error("cannot convert non-specific time to time_t");
    if (*year_ < 1970)
        return Error("time point is not representable as a time_t value");

    // Full years since 1970
    const unsigned long nYears = *year_ - 1970;

    // Number of leap days since 1970-01-01.
    const unsigned long nLeapDays = (*year_ / 4) - (*year_ / 100) + (*year_ / 400) - 477 -
                                    (isLeapYear(*year_) && *month_ <= 2 ? 1 : 0); // don't count this year yet?

    // Number of complete days
    static const unsigned long cumulativeMonthDays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    const unsigned long nDays = nYears*365ul + nLeapDays + cumulativeMonthDays[*month_-1] + (*day_-1);

    // Adjust for the time zone
    const unsigned long tzs = tz_hour_.orElse(0)*3600 + tz_minute_.orElse(0)*60;

    const unsigned long t = nDays * 86400ul + *hour_ * 3600ul + *minute_ * 60ul + *second_ - tzs;
    if (t > std::numeric_limits<time_t>::max())
        return Error("time point is not representable as a time_t value");
    return Ok((time_t)t);
}

bool
Time::operator==(const Time &other) const {
    const auto za = toZulu();
    const auto zb = other.toZulu();
    if (za.isError() || zb.isError())
        return false;
    const Time a = *za;
    const Time b = *zb;

    return a.year_.isEqual(b.year_) &&
        a.month_.isEqual(b.month_) &&
        a.day_.isEqual(b.day_) &&
        a.hour_.isEqual(b.hour_) &&
        a.minute_.isEqual(b.minute_) &&
        a.second_.isEqual(b.second_) &&
        a.tz_hour_.isEqual(b.tz_hour_) &&
        a.tz_minute_.isEqual(b.tz_minute_);
}

bool
Time::operator!=(const Time &other) const {
    const auto za = toZulu();
    const auto zb = other.toZulu();
    if (za.isError() || zb.isError())
        return false;
    const Time a = *za;
    const Time b = *zb;

    return !(a.year_.isEqual(b.year_) &&
             a.month_.isEqual(b.month_) &&
             a.day_.isEqual(b.day_) &&
             a.hour_.isEqual(b.hour_) &&
             a.minute_.isEqual(b.minute_) &&
             a.second_.isEqual(b.second_) &&
             a.tz_hour_.isEqual(b.tz_hour_) &&
             a.tz_minute_.isEqual(b.tz_minute_));
}

bool
Time::operator<(const Time &other) const {
    const auto za = toZulu();
    const auto zb = other.toZulu();
    if (za.isError() && zb.isError()) {
        return za.unwrapError() < zb.unwrapError();
    } else if (za.isError() || zb.isError()) {
        return za.isError();
    }

    const Time a = *za;
    const Time b = *zb;

    if (a.year_.isEqual(b.year_)) {
        if (a.month_.isEqual(b.month_)) {
            if (a.day_.isEqual(b.day_)) {
                if (a.hour_.isEqual(b.hour_)) {
                    if (a.minute_.isEqual(b.minute_)) {
                        if (a.second_.isEqual(b.second_)) {
                            if (a.tz_hour_.isEqual(b.tz_hour_)) {
                                if (a.tz_minute_.isEqual(b.tz_minute_)) {
                                    return false;       // they're equal!
                                } else if (!a.tz_minute_ || !b.tz_minute_) {
                                    return !a.tz_minute_;
                                } else {
                                    return *a.tz_minute_ < *b.tz_minute_;
                                }
                            } else if (!a.tz_hour_ || !b.tz_hour_) {
                                return !a.tz_hour_;
                            } else {
                                return *a.tz_hour_ < *b.tz_hour_;
                            }
                        } else if (!a.second_ || !b.second_) {
                            return !a.second_;
                        } else {
                            return *a.second_ < *b.second_;
                        }
                    } else if (!a.minute_ || !b.minute_) {
                        return !a.minute_;
                    } else {
                        return *a.minute_ < *b.minute_;
                    }
                } else if (!a.hour_ || !b.hour_) {
                    return !a.hour_;
                } else {
                    return *a.hour_ < *b.hour_;
                }
            } else if (!a.day_ || !b.day_) {
                return !a.day_;
            } else {
                return *a.day_ < *b.day_;
            }
        } else if (!a.month_ || !b.month_) {
            return !a.month_;
        } else {
            return *a.month_ < *b.month_;
        }
    } else if (!a.year_ || !b.year_) {
        return !a.year_;
    } else {
        return *a.year_ < *b.year_;
    }
}

std::ostream&
operator<<(std::ostream &out, const Time &t) {
    out <<t.toString();
    return out;
}

} // namespace
