// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Time_H
#define Sawyer_Time_H

#include <Sawyer/Optional.h>
#include <Sawyer/Result.h>

#include <ctime>
#include <ostream>

namespace Sawyer {

/** Represents an ISO 8601 time point. */
class Time {
    Optional<unsigned> year_;                           // year, >= 1583
    Optional<unsigned> month_;                          // month of year, 1 through 12, only if year_ is defined
    Optional<unsigned> day_;                            // day of month, 1 through 31, only if month_ is defined

    Optional<unsigned> hour_;                           // 0 through 23
    Optional<unsigned> minute_;                         // 0 through 59, only if hour_ is defined
    Optional<unsigned> second_;                         // 0 through 60 (for a leap second), only if minute_ is defined

    Optional<int> tz_hour_;                             // -23 through +23
    Optional<int> tz_minute_;                           // -59 through 59, only if tz_hour_ is defined and having the same sign

public:
    /** Construct an empty time point.
     *
     *  The constructed object has no date, no time, and no timezone. */
    Time();

    /** Parse an ISO 8601 time string.
     *
     *  Returns either a time or an error description. */
    static Result<Time, std::string> parse(const std::string&);

    /** Current time.
     *
     *  Returns the current time. */
    static Time now();

    /** Test whether this object is empty.
     *
     *  Returns true if this time point has no date or time. It does not matter if a timezone is present. */
    bool isEmpty() const;

    /** Test whether date is present.
     *
     *  Returns true if a date (year, month, or day) is present. */
    bool hasDate() const;

    /** Test whether time is present.
     *
     *  Returns true if a time (hour, minute, second) is present. */
    bool hasTime() const;

    /** Test whether a timezone is present.
     *
     *  Returns true if a timezone is present. */
    bool hasZone() const;

    /** Test whether a date is fully specified.
     *
     *  Returns true if a year, month, and day are all present. */
    bool hasSpecificDate() const;

    /** Test whether a time is fully specified.
     *
     *  Returns true if a time is fully specified. I.e., if a year, month, day, hour, minute, and second are all present. */
    bool hasSpecificTime() const;

    /** Fill in missing fields.
     *
     *  Returns a new time whose fields are individually initialized to be from this object or another object. If this object
     *  has a value for a field then it's used, otherwise the value comes from the other object. */
    Time resolve(const Time&) const;

    /** Returns the lower bound for the time.
     *
     *  Returns a time point that has a fully specified time (year, month, day, hour, minute, and second) such that it is the
     *  latest time not past the time specified by this time point.  For instance, if this time point is "2022-06-30 11:30" (no
     *  seconds specified) then the lower bound is created by adding "00" as the seconds to give "2022-06-30 11:30:00".
     *
     *  If this time point has only a time but no date, then the returned time will also not have a date portion. For instance,
     *  if this time point is "11:30" then the return value is "11:30:00". If this time has only a timezone, then the result
     *  also has only a timezone.
     *
     *  The returned timezone is identical to the time zone from the input time point. */
    Time lowerBound() const;

    /** Returns an upper bound for the time.
     *
     *  Returns a time point that has a fully specified time (year, month, day, hour, minute, and second) such that it is the
     *  earliest time that is past the time specified by this time point.  For instance, if this time point is fully specified
     *  as "2022-06-30 11:30:00" then the return value is the next second, namely "2022-06-30 11:30:01". If this time point is
     *  partially specified as "2022-06-30 11:30" then the return value is the next minute, namely "2022-06-30 11:31:00".
     *
     *  If this time point has no date, then the return value also has no date, in which case it's possible for the return
     *  value to be earlier than the input value. For instance, if this time point is the last second of the day "23:59:59",
     *  then the return value will be the first second of the (next) day: "00:00:00". Similarly, a partially specified last
     *  minute of a day gets converted to the first minute of the next day, "23:59" to "00:00:00".
     *
     *  If the return value would have a year that's larger 9999, then an error is returned. For instance, the upper bound for
     *  "9999-12-31" is "10000-01-01 00:00:00", which exceeds what can be represented by ISO 8601.
     *
     *  The returned timezone is identical to the time zone from the input time point. */
    Result<Time, std::string> upperBound() const;

    /** Removes the date portion of a time point.
     *
     *  Returns a copy of this time but without a year, month, and day. */
    Time noDate() const;

    /** Removes the time portion of a time point.
     *
     *  Returns a copy of this time but without an hour, minute, or second. */
    Time noTime() const;

    /** Removes the timezone portion of a time point.
     *
     *  Returns a copy of this time but without a timezone. */
    Time noZone() const;

    /** Returns the year, if any.
     *
     *  A year can be anything between 1583 and 9999, inclusive. */
    const Optional<unsigned>& year() const;

    /** Returns the month, if any.
     *
     *  A month can be in the range 1 through 12, inclusive. If a month is present, then a year is also present. */
    const Optional<unsigned>& month() const;

    /** Returns the day of the month, if any.
     *
     *  The day of the month can be in the range 1 through 28, 29, 30, or 31 depending on the year and month. If a day of month
     *  is present, then the year and month are also present. */
    const Optional<unsigned>& day() const;

    /** Returns the hour, if any.
     *
     *  An hour is in the range 0 through 23, inclusive. */
    const Optional<unsigned>& hour() const;

    /** Returns the minute, if any.
     *
     *  A minute is in the range 0 through 59, inclusive. If a minute is present, then an hour is also present. */
    const Optional<unsigned>& minute() const;

    /** Returns the second, if any.
     *
     *  A second is in the range 0 through 60, inclusive. A value of 60 represents a leap second. If a second is present, then
     *  an hour and minute are also present. */
    const Optional<unsigned>& second() const;

    /** Returns a timezone hour, if any.
     *
     *  The timezone hour is a signed integer between -23 and 23, inclusive and represents the distance east of the prime
     *  meridian.  Positive values are east; negative values are west. */
    const Optional<int>& timeZoneHour() const;

    /** Returns a timezone minute, if any.
     *
     *  The timezone minute is a value between -59 and 59, inclusive and has the same sign as the timezone hour. If a timezone
     *  minute is present then a timezone hour is also present. */
    const Optional<int>& timeZoneMinute() const;

    /** Convert to timezone +0000.
     *
     *  The hours and minutes of this time's timezone is subtracted from this time in order to create a new time whose time
     *  zone is +0000. If the input time has no timezone or has no time, then the output is the same as the input.  The seconds
     *  field is always unchanged. If the time zone minute field is unspecified, then it is assumed to be zero for the purpose
     *  of subtraction. If the time minute field is unspecified, then it is assumed to be zero for the purpose of the
     *  subtraction and will remain unspecified in the result. If the input has a time but no date, then the time will be
     *  adjusted but the return value will also not have a date.
     *
     *  It's possible that the normalized time does not fall within the range of times representable by this type. If that
     *  happens, then an error string is returned instead of a valid time. */
    Result<Time, std::string> toZulu() const;

    /** Convert a time point to ISO 8601 format.
     *
     *  Returns this time point as an ISO 8601 string that can be parsed by this same class. This function generates the string
     *  using the extended format for ISO 8601. That is, the parts of a date are separated by a hyphen, the parts of the time
     *  and timezone are separated by colons, and the time is separated from the date by a space. Only the parts that are
     *  present are output. An empty time point without a timezone will result in an empty string being returned. */
    std::string toString() const;

    /** Convert the time point to a Unix system time.
     *
     *  Returns the number of seconds since "1970-01-01 00:00:00Z". Returns an error string if this time point does not have a
     *  specific time. If no timezone is present, then the return value is local time. */
    Result<time_t, std::string> toUnix() const;

    /** Compare two times for equality or inequality.
     *
     *  Two times are equal if, after converting them both to the same timezone, they have the same year, month, day, hour,
     *  minute, and second for all such fields where either or both times have a value. Fields are considered equal if neither
     *  time has a value for that field.
     *
     *  If either or both times fail to convert to Zulu due to overflows, then they are neither equal nor unequal.
     *
     * @{ */
    bool operator==(const Time&) const;
    bool operator!=(const Time&) const;
    /** @} */

    /** Compare two times for less-than.
     *
     *  Time @p a is less than time @p b if @p a occurs earlier than @p b after normalizing both to Zulu.  If the conversion to
     *  Zulu fails for both times, then the result is the comparison of the error strings. If only one of the times fails to
     *  convert, then that one is less than the other. If a field is missing from both times, then that field is considered to
     *  be equal (e.g., if both times are missing the month). If the field is missing from @p a but not @p b, then @p a is less
     *  than @p b. The fields are compared in order from year to second, followed by the timezone hour and then timezone minute
     *  (although the timezone comparisons are only based on their presence or absense since they've both been converted to
     *  Zulu). */
    bool operator<(const Time&) const;

private:
    // Normalization functions
    void normalizeSecond();
    void normalizeMinute();
    void normalizeHour();
    void normalizeMonth();
    void normalize();
};

std::ostream& operator<<(std::ostream&, const Time&);

} // namespace
#endif
