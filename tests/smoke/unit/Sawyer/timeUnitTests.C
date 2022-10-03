// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Time.h>

#include <Sawyer/Assert.h>
#include <boost/lexical_cast.hpp>
#include <iostream>

static void
testNow() {
    auto now = Sawyer::Time::now();
    std::cerr <<"tests run at " <<now <<"\n";
}

static void
testConstruct() {
    Sawyer::Time t;
    ASSERT_always_require(t.isEmpty());
    ASSERT_always_forbid(t.hasDate());
    ASSERT_always_forbid(t.hasTime());
    ASSERT_always_forbid(t.hasZone());
    ASSERT_always_forbid(t.hasSpecificDate());
    ASSERT_always_forbid(t.hasSpecificTime());
    ASSERT_always_forbid(t.year());
    ASSERT_always_forbid(t.month());
    ASSERT_always_forbid(t.day());
    ASSERT_always_forbid(t.hour());
    ASSERT_always_forbid(t.minute());
    ASSERT_always_forbid(t.second());
    ASSERT_always_forbid(t.timeZoneHour());
    ASSERT_always_forbid(t.timeZoneMinute());
    ASSERT_always_require(t.toString().empty());
    ASSERT_always_require(t.toUnix().isError());
#if 0 // [Robb Matzke 2022-09-28]: not implemented yet
    ASSERT_always_require(t == t);
    ASSERT_always_require(t != t);
    ASSERT_always_forbid(t < t);
    ASSERT_always_forbid(t > t);
    ASSERT_always_require(t <= t);
    ASSERT_always_require(t >= t);
#endif
}

static void
testParseDate() {
    auto t1 = Sawyer::Time::parse("2022");
    ASSERT_always_require2(t1.isOk(), t1.unwrapError());
    ASSERT_always_forbid(t1.unwrap().isEmpty());
    ASSERT_always_require(t1.unwrap().hasDate());
    ASSERT_always_forbid(t1.unwrap().hasTime());
    ASSERT_always_forbid(t1.unwrap().hasZone());
    ASSERT_always_forbid(t1.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t1.unwrap().hasSpecificTime());
    ASSERT_always_require(*t1.unwrap().year() == 2022);
    ASSERT_always_forbid(t1.unwrap().month());
    ASSERT_always_forbid(t1.unwrap().day());
    ASSERT_always_forbid(t1.unwrap().hour());
    ASSERT_always_forbid(t1.unwrap().minute());
    ASSERT_always_forbid(t1.unwrap().second());
    ASSERT_always_forbid(t1.unwrap().timeZoneHour());
    ASSERT_always_forbid(t1.unwrap().timeZoneMinute());
    ASSERT_always_require(t1.unwrap().toString() == "2022");
    ASSERT_always_require(t1.unwrap().toUnix().isError());

    auto t2 = Sawyer::Time::parse("2022-06");
    ASSERT_always_require2(t2.isOk(), t2.unwrapError());
    ASSERT_always_forbid(t2.unwrap().isEmpty());
    ASSERT_always_require(t2.unwrap().hasDate());
    ASSERT_always_forbid(t2.unwrap().hasTime());
    ASSERT_always_forbid(t2.unwrap().hasZone());
    ASSERT_always_forbid(t2.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t2.unwrap().hasSpecificTime());
    ASSERT_always_require(*t2.unwrap().year() == 2022);
    ASSERT_always_require(*t2.unwrap().month() == 6);
    ASSERT_always_forbid(t2.unwrap().day());
    ASSERT_always_forbid(t2.unwrap().hour());
    ASSERT_always_forbid(t2.unwrap().minute());
    ASSERT_always_forbid(t2.unwrap().second());
    ASSERT_always_forbid(t2.unwrap().timeZoneHour());
    ASSERT_always_forbid(t2.unwrap().timeZoneMinute());
    ASSERT_always_require(t2.unwrap().toString() == "2022-06");
    ASSERT_always_require(t2.unwrap().toUnix().isError());

    auto t3 = Sawyer::Time::parse("202206");
    ASSERT_always_require(t3.unwrapError() == "invalid time specification \"202206\"");

    auto t4 = Sawyer::Time::parse("2022-06-30");
    ASSERT_always_require2(t4.isOk(), t4.unwrapError());
    ASSERT_always_forbid(t4.unwrap().isEmpty());
    ASSERT_always_require(t4.unwrap().hasDate());
    ASSERT_always_forbid(t4.unwrap().hasTime());
    ASSERT_always_forbid(t4.unwrap().hasZone());
    ASSERT_always_require(t4.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t4.unwrap().hasSpecificTime());
    ASSERT_always_require(*t4.unwrap().year() == 2022);
    ASSERT_always_require(*t4.unwrap().month() == 6);
    ASSERT_always_require(*t4.unwrap().day() == 30);
    ASSERT_always_forbid(t4.unwrap().hour());
    ASSERT_always_forbid(t4.unwrap().minute());
    ASSERT_always_forbid(t4.unwrap().second());
    ASSERT_always_forbid(t4.unwrap().timeZoneHour());
    ASSERT_always_forbid(t4.unwrap().timeZoneMinute());
    ASSERT_always_require(t4.unwrap().toString() == "2022-06-30");
    ASSERT_always_require(t4.unwrap().toUnix().isError());

    auto t5 = Sawyer::Time::parse("20220630");
    ASSERT_always_require2(t5.isOk(), t5.unwrapError());
    ASSERT_always_forbid(t5.unwrap().isEmpty());
    ASSERT_always_require(t5.unwrap().hasDate());
    ASSERT_always_forbid(t5.unwrap().hasTime());
    ASSERT_always_forbid(t5.unwrap().hasZone());
    ASSERT_always_require(t5.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t5.unwrap().hasSpecificTime());
    ASSERT_always_require(*t5.unwrap().year() == 2022);
    ASSERT_always_require(*t5.unwrap().month() == 6);
    ASSERT_always_require(*t5.unwrap().day() == 30);
    ASSERT_always_forbid(t5.unwrap().hour());
    ASSERT_always_forbid(t5.unwrap().minute());
    ASSERT_always_forbid(t5.unwrap().second());
    ASSERT_always_forbid(t5.unwrap().timeZoneHour());
    ASSERT_always_forbid(t5.unwrap().timeZoneMinute());
    ASSERT_always_require(t5.unwrap().toString() == "2022-06-30");
    ASSERT_always_require(t5.unwrap().toUnix().isError());

    ASSERT_always_require(Sawyer::Time::parse("20220631").unwrapError() == "day of month is out of range in \"20220631\"");
    ASSERT_always_require(Sawyer::Time::parse("20220600").unwrapError() == "day of month is out of range in \"20220600\"");
    ASSERT_always_require(Sawyer::Time::parse("20220001").unwrapError() == "month is out of range in \"20220001\"");
    ASSERT_always_require(Sawyer::Time::parse("20221301").unwrapError() == "month is out of range in \"20221301\"");
    ASSERT_always_require(Sawyer::Time::parse("1583").unwrap().year().get() == 1583);
    ASSERT_always_require(Sawyer::Time::parse("1582").unwrapError() == "year must be 1583 or later in \"1582\"");
}

static void
testParseTime() {
    auto t1 = Sawyer::Time::parse("T10");
    ASSERT_always_require2(t1.isOk(), t1.unwrapError());
    ASSERT_always_forbid(t1.unwrap().isEmpty());
    ASSERT_always_forbid(t1.unwrap().hasDate());
    ASSERT_always_require(t1.unwrap().hasTime());
    ASSERT_always_forbid(t1.unwrap().hasZone());
    ASSERT_always_forbid(t1.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t1.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t1.unwrap().year());
    ASSERT_always_forbid(t1.unwrap().month());
    ASSERT_always_forbid(t1.unwrap().day());
    ASSERT_always_require(*t1.unwrap().hour() == 10);
    ASSERT_always_forbid(t1.unwrap().minute());
    ASSERT_always_forbid(t1.unwrap().second());
    ASSERT_always_forbid(t1.unwrap().timeZoneHour());
    ASSERT_always_forbid(t1.unwrap().timeZoneMinute());
    ASSERT_always_require(t1.unwrap().toString() == "T10");
    ASSERT_always_require(t1.unwrap().toUnix().isError());

    auto t2 = Sawyer::Time::parse("T1030");
    ASSERT_always_require2(t2.isOk(), t2.unwrapError());
    ASSERT_always_forbid(t2.unwrap().isEmpty());
    ASSERT_always_forbid(t2.unwrap().hasDate());
    ASSERT_always_require(t2.unwrap().hasTime());
    ASSERT_always_forbid(t2.unwrap().hasZone());
    ASSERT_always_forbid(t2.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t2.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t2.unwrap().year());
    ASSERT_always_forbid(t2.unwrap().month());
    ASSERT_always_forbid(t2.unwrap().day());
    ASSERT_always_require(*t2.unwrap().hour() == 10);
    ASSERT_always_require(*t2.unwrap().minute() == 30);
    ASSERT_always_forbid(t2.unwrap().second());
    ASSERT_always_forbid(t2.unwrap().timeZoneHour());
    ASSERT_always_forbid(t2.unwrap().timeZoneMinute());
    ASSERT_always_require(t2.unwrap().toString() == "10:30");
    ASSERT_always_require(t2.unwrap().toUnix().isError());

    auto t3 = Sawyer::Time::parse("T10:30");
    ASSERT_always_require2(t3.isOk(), t3.unwrapError());
    ASSERT_always_forbid(t3.unwrap().isEmpty());
    ASSERT_always_forbid(t3.unwrap().hasDate());
    ASSERT_always_require(t3.unwrap().hasTime());
    ASSERT_always_forbid(t3.unwrap().hasZone());
    ASSERT_always_forbid(t3.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t3.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t3.unwrap().year());
    ASSERT_always_forbid(t3.unwrap().month());
    ASSERT_always_forbid(t3.unwrap().day());
    ASSERT_always_require(*t3.unwrap().hour() == 10);
    ASSERT_always_require(*t3.unwrap().minute() == 30);
    ASSERT_always_forbid(t3.unwrap().second());
    ASSERT_always_forbid(t3.unwrap().timeZoneHour());
    ASSERT_always_forbid(t3.unwrap().timeZoneMinute());
    ASSERT_always_require(t3.unwrap().toString() == "10:30");
    ASSERT_always_require(t3.unwrap().toUnix().isError());

    auto t4 = Sawyer::Time::parse("T103045");
    ASSERT_always_require2(t4.isOk(), t4.unwrapError());
    ASSERT_always_forbid(t4.unwrap().isEmpty());
    ASSERT_always_forbid(t4.unwrap().hasDate());
    ASSERT_always_require(t4.unwrap().hasTime());
    ASSERT_always_forbid(t4.unwrap().hasZone());
    ASSERT_always_forbid(t4.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t4.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t4.unwrap().year());
    ASSERT_always_forbid(t4.unwrap().month());
    ASSERT_always_forbid(t4.unwrap().day());
    ASSERT_always_require(*t4.unwrap().hour() == 10);
    ASSERT_always_require(*t4.unwrap().minute() == 30);
    ASSERT_always_require(*t4.unwrap().second() == 45);
    ASSERT_always_forbid(t4.unwrap().timeZoneHour());
    ASSERT_always_forbid(t4.unwrap().timeZoneMinute());
    ASSERT_always_require(t4.unwrap().toString() == "10:30:45");
    ASSERT_always_require(t4.unwrap().toUnix().isError());

    auto t5 = Sawyer::Time::parse("T10:30:45");
    ASSERT_always_require2(t5.isOk(), t5.unwrapError());
    ASSERT_always_forbid(t5.unwrap().isEmpty());
    ASSERT_always_forbid(t5.unwrap().hasDate());
    ASSERT_always_require(t5.unwrap().hasTime());
    ASSERT_always_forbid(t5.unwrap().hasZone());
    ASSERT_always_forbid(t5.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t5.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t5.unwrap().year());
    ASSERT_always_forbid(t5.unwrap().month());
    ASSERT_always_forbid(t5.unwrap().day());
    ASSERT_always_require(*t5.unwrap().hour() == 10);
    ASSERT_always_require(*t5.unwrap().minute() == 30);
    ASSERT_always_require(*t5.unwrap().second() == 45);
    ASSERT_always_forbid(t5.unwrap().timeZoneHour());
    ASSERT_always_forbid(t5.unwrap().timeZoneMinute());
    ASSERT_always_require(t5.unwrap().toString() == "10:30:45");
    ASSERT_always_require(t5.unwrap().toUnix().isError());

    auto t6 = Sawyer::Time::parse("10:30");
    ASSERT_always_require2(t6.isOk(), t6.unwrapError());
    ASSERT_always_forbid(t6.unwrap().isEmpty());
    ASSERT_always_forbid(t6.unwrap().hasDate());
    ASSERT_always_require(t6.unwrap().hasTime());
    ASSERT_always_forbid(t6.unwrap().hasZone());
    ASSERT_always_forbid(t6.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t6.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t6.unwrap().year());
    ASSERT_always_forbid(t6.unwrap().month());
    ASSERT_always_forbid(t6.unwrap().day());
    ASSERT_always_require(*t6.unwrap().hour() == 10);
    ASSERT_always_require(*t6.unwrap().minute() == 30);
    ASSERT_always_forbid(t6.unwrap().second());
    ASSERT_always_forbid(t6.unwrap().timeZoneHour());
    ASSERT_always_forbid(t6.unwrap().timeZoneMinute());
    ASSERT_always_require(t6.unwrap().toString() == "10:30");
    ASSERT_always_require(t6.unwrap().toUnix().isError());

    auto t7 = Sawyer::Time::parse("T10:30:45");
    ASSERT_always_require2(t7.isOk(), t7.unwrapError());
    ASSERT_always_forbid(t7.unwrap().isEmpty());
    ASSERT_always_forbid(t7.unwrap().hasDate());
    ASSERT_always_require(t7.unwrap().hasTime());
    ASSERT_always_forbid(t7.unwrap().hasZone());
    ASSERT_always_forbid(t7.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t7.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t7.unwrap().year());
    ASSERT_always_forbid(t7.unwrap().month());
    ASSERT_always_forbid(t7.unwrap().day());
    ASSERT_always_require(*t7.unwrap().hour() == 10);
    ASSERT_always_require(*t7.unwrap().minute() == 30);
    ASSERT_always_require(*t7.unwrap().second() == 45);
    ASSERT_always_forbid(t7.unwrap().timeZoneHour());
    ASSERT_always_forbid(t7.unwrap().timeZoneMinute());
    ASSERT_always_require(t7.unwrap().toString() == "10:30:45");
    ASSERT_always_require(t7.unwrap().toUnix().isError());

    ASSERT_always_require(Sawyer::Time::parse("12:15:61").unwrapError() == "second is out of range in \"12:15:61\"");
    ASSERT_always_require(Sawyer::Time::parse("12:15:60").unwrap().second().get() == 60); // leap second
    ASSERT_always_require(Sawyer::Time::parse("12:60:00").unwrapError() == "minute is out of range in \"12:60:00\"");
    ASSERT_always_require(Sawyer::Time::parse("24:00:00").unwrapError() == "hour is out of range in \"24:00:00\"");
}

static void
testParseZone() {
    auto t1 = Sawyer::Time::parse("+04");
    ASSERT_always_require2(t1.isOk(), t1.unwrapError());
    ASSERT_always_require(t1.unwrap().isEmpty());
    ASSERT_always_forbid(t1.unwrap().hasDate());
    ASSERT_always_forbid(t1.unwrap().hasTime());
    ASSERT_always_require(t1.unwrap().hasZone());
    ASSERT_always_forbid(t1.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t1.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t1.unwrap().year());
    ASSERT_always_forbid(t1.unwrap().month());
    ASSERT_always_forbid(t1.unwrap().day());
    ASSERT_always_forbid(t1.unwrap().hour());
    ASSERT_always_forbid(t1.unwrap().minute());
    ASSERT_always_forbid(t1.unwrap().second());
    ASSERT_always_require(*t1.unwrap().timeZoneHour() == 4);
    ASSERT_always_forbid(t1.unwrap().timeZoneMinute());
    ASSERT_always_require2(t1.unwrap().toString() == "+04", t1.unwrap().toString());
    ASSERT_always_require(t1.unwrap().toUnix().isError());

    auto t2 = Sawyer::Time::parse("+0430");
    ASSERT_always_require2(t2.isOk(), t2.unwrapError());
    ASSERT_always_require(t2.unwrap().isEmpty());
    ASSERT_always_forbid(t2.unwrap().hasDate());
    ASSERT_always_forbid(t2.unwrap().hasTime());
    ASSERT_always_require(t2.unwrap().hasZone());
    ASSERT_always_forbid(t2.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t2.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t2.unwrap().year());
    ASSERT_always_forbid(t2.unwrap().month());
    ASSERT_always_forbid(t2.unwrap().day());
    ASSERT_always_forbid(t2.unwrap().hour());
    ASSERT_always_forbid(t2.unwrap().minute());
    ASSERT_always_forbid(t2.unwrap().second());
    ASSERT_always_require(*t2.unwrap().timeZoneHour() == 4);
    ASSERT_always_require(*t2.unwrap().timeZoneMinute() == 30);
    ASSERT_always_require2(t2.unwrap().toString() == "+04:30", t2.unwrap().toString());
    ASSERT_always_require(t2.unwrap().toUnix().isError());

    auto t3 = Sawyer::Time::parse("+04:30");
    ASSERT_always_require2(t3.isOk(), t3.unwrapError());
    ASSERT_always_require(t3.unwrap().isEmpty());
    ASSERT_always_forbid(t3.unwrap().hasDate());
    ASSERT_always_forbid(t3.unwrap().hasTime());
    ASSERT_always_require(t3.unwrap().hasZone());
    ASSERT_always_forbid(t3.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t3.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t3.unwrap().year());
    ASSERT_always_forbid(t3.unwrap().month());
    ASSERT_always_forbid(t3.unwrap().day());
    ASSERT_always_forbid(t3.unwrap().hour());
    ASSERT_always_forbid(t3.unwrap().minute());
    ASSERT_always_forbid(t3.unwrap().second());
    ASSERT_always_require(*t3.unwrap().timeZoneHour() == 4);
    ASSERT_always_require(*t3.unwrap().timeZoneMinute() == 30);
    ASSERT_always_require2(t3.unwrap().toString() == "+04:30", t3.unwrap().toString());
    ASSERT_always_require(t3.unwrap().toUnix().isError());

    auto t4 = Sawyer::Time::parse("-04:30");
    ASSERT_always_require2(t4.isOk(), t4.unwrapError());
    ASSERT_always_require(t4.unwrap().isEmpty());
    ASSERT_always_forbid(t4.unwrap().hasDate());
    ASSERT_always_forbid(t4.unwrap().hasTime());
    ASSERT_always_require(t4.unwrap().hasZone());
    ASSERT_always_forbid(t4.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t4.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t4.unwrap().year());
    ASSERT_always_forbid(t4.unwrap().month());
    ASSERT_always_forbid(t4.unwrap().day());
    ASSERT_always_forbid(t4.unwrap().hour());
    ASSERT_always_forbid(t4.unwrap().minute());
    ASSERT_always_forbid(t4.unwrap().second());
    ASSERT_always_require(*t4.unwrap().timeZoneHour() == -4);
    ASSERT_always_require(*t4.unwrap().timeZoneMinute() == -30);
    ASSERT_always_require2(t4.unwrap().toString() == "-04:30", t4.unwrap().toString());
    ASSERT_always_require(t4.unwrap().toUnix().isError());

    ASSERT_always_require(Sawyer::Time::parse("+00:60").unwrapError() == "timezone minute is out of range in \"+00:60\"");
    ASSERT_always_require(Sawyer::Time::parse("+24:00").unwrapError() == "timezone hour is out of range in \"+24:00\"");
}

static void
testParseFull() {
    auto t1 = Sawyer::Time::parse("2022-06-30 12:30:45-0430");
    ASSERT_always_require2(t1.isOk(), t1.unwrapError());
    ASSERT_always_forbid(t1.unwrap().isEmpty());
    ASSERT_always_require(t1.unwrap().hasDate());
    ASSERT_always_require(t1.unwrap().hasTime());
    ASSERT_always_require(t1.unwrap().hasZone());
    ASSERT_always_require(t1.unwrap().hasSpecificDate());
    ASSERT_always_require(t1.unwrap().hasSpecificTime());
    ASSERT_always_require(*t1.unwrap().year() == 2022);
    ASSERT_always_require(*t1.unwrap().month() == 6);
    ASSERT_always_require(*t1.unwrap().day() == 30);
    ASSERT_always_require(*t1.unwrap().hour() == 12);
    ASSERT_always_require(*t1.unwrap().minute() == 30);
    ASSERT_always_require(*t1.unwrap().second() == 45);
    ASSERT_always_require(*t1.unwrap().timeZoneHour() == -4);
    ASSERT_always_require(*t1.unwrap().timeZoneMinute() == -30);
    ASSERT_always_require2(t1.unwrap().toString() == "2022-06-30 12:30:45-04:30", t1.unwrap().toString());
    ASSERT_always_require(t1.unwrap().toUnix().isOk());

    auto t2 = Sawyer::Time::parse("20220630T123045-0430");
    ASSERT_always_require2(t2.isOk(), t2.unwrapError());
    ASSERT_always_forbid(t2.unwrap().isEmpty());
    ASSERT_always_require(t2.unwrap().hasDate());
    ASSERT_always_require(t2.unwrap().hasTime());
    ASSERT_always_require(t2.unwrap().hasZone());
    ASSERT_always_require(t2.unwrap().hasSpecificDate());
    ASSERT_always_require(t2.unwrap().hasSpecificTime());
    ASSERT_always_require(*t2.unwrap().year() == 2022);
    ASSERT_always_require(*t2.unwrap().month() == 6);
    ASSERT_always_require(*t2.unwrap().day() == 30);
    ASSERT_always_require(*t2.unwrap().hour() == 12);
    ASSERT_always_require(*t2.unwrap().minute() == 30);
    ASSERT_always_require(*t2.unwrap().second() == 45);
    ASSERT_always_require(*t2.unwrap().timeZoneHour() == -4);
    ASSERT_always_require(*t2.unwrap().timeZoneMinute() == -30);
    ASSERT_always_require2(t2.unwrap().toString() == "2022-06-30 12:30:45-04:30", t2.unwrap().toString());
    ASSERT_always_require(t2.unwrap().toUnix().isOk());

    auto t3 = Sawyer::Time::parse("20220630 T123045-0430");
    ASSERT_always_require2(t3.isOk(), t3.unwrapError());
    ASSERT_always_forbid(t3.unwrap().isEmpty());
    ASSERT_always_require(t3.unwrap().hasDate());
    ASSERT_always_require(t3.unwrap().hasTime());
    ASSERT_always_require(t3.unwrap().hasZone());
    ASSERT_always_require(t3.unwrap().hasSpecificDate());
    ASSERT_always_require(t3.unwrap().hasSpecificTime());
    ASSERT_always_require(*t3.unwrap().year() == 2022);
    ASSERT_always_require(*t3.unwrap().month() == 6);
    ASSERT_always_require(*t3.unwrap().day() == 30);
    ASSERT_always_require(*t3.unwrap().hour() == 12);
    ASSERT_always_require(*t3.unwrap().minute() == 30);
    ASSERT_always_require(*t3.unwrap().second() == 45);
    ASSERT_always_require(*t3.unwrap().timeZoneHour() == -4);
    ASSERT_always_require(*t3.unwrap().timeZoneMinute() == -30);
    ASSERT_always_require2(t3.unwrap().toString() == "2022-06-30 12:30:45-04:30", t3.unwrap().toString());
    ASSERT_always_require(t3.unwrap().toUnix().isOk());
}

static void
testResolve() {
    auto t1 = *Sawyer::Time::parse("2022-06-30");
    auto t2 = *Sawyer::Time::parse("10:30");
    auto t3 = t1.resolve(t2);
    ASSERT_always_require2(t3.toString() == "2022-06-30 10:30", t3.toString());

    auto t4 = *Sawyer::Time::parse("+04");
    auto t5 = t3.resolve(t4);
    ASSERT_always_require2(t5.toString() == "2022-06-30 10:30+04", t3.toString());

    auto t6 = *Sawyer::Time::parse("-06:30");
    auto t7 = t5.resolve(t6); // tz was already partly resolved and has a different sign, so don't resolve
    ASSERT_always_require2(t5.toString() == "2022-06-30 10:30+04", t3.toString());
}

static void
testLowerBound() {
    auto t1 = Sawyer::Time::parse("2022").unwrap().lowerBound();
    ASSERT_always_require2(t1.toString() == "2022-01-01 00:00:00", t1.toString());

    auto t2 = Sawyer::Time::parse("10:45").unwrap().lowerBound();
    ASSERT_always_require2(t2.toString() == "10:45:00", t2.toString());

    auto t3 = Sawyer::Time::parse("-0700").unwrap().lowerBound();
    ASSERT_always_require2(t3.toString() == "-07:00", t3.toString());
}

static void
tub(const std::string &in, const std::string &ans) {
    auto t1 = Sawyer::Time::parse(in).unwrap().upperBound().unwrap();
    auto out = t1.toString();
    ASSERT_always_require2(out == ans, "in=\"" + in + "\", out=\"" + out + "\", ans=\"" + ans + "\"");
}

static void
testUpperBound() {
    tub("2022-06-29 12:30:58", "2022-06-29 12:30:59");
    tub("2022-06-29 12:30:59", "2022-06-29 12:31:00");
    tub("2022-06-29 12:30:60", "2022-06-29 12:31:00");
    tub("2022-06-29 12:30:59", "2022-06-29 12:31:00");
    tub("2022-06-29 12:59:59", "2022-06-29 13:00:00");
    tub("2022-06-29 23:59:59", "2022-06-30 00:00:00");
    tub("2022-06-30 23:59:59", "2022-07-01 00:00:00");
    tub("2022-12-31 23:59:59", "2023-01-01 00:00:00");

    tub("12:30:59", "12:31:00");
    tub("12:30:60", "12:31:00");
    tub("12:59:59", "13:00:00");
    tub("23:59:59", "00:00:00");

    tub("12:30", "12:31:00");
    tub("12:59", "13:00:00");
    tub("23:59", "00:00:00");

    tub("T12", "13:00:00");
    tub("T23", "00:00:00");

    tub("10:30-0400", "10:31:00-04:00");

    tub("2022-06-29", "2022-06-30 00:00:00");
    tub("2022-06-30", "2022-07-01 00:00:00");
    tub("2022-12-31", "2023-01-01 00:00:00");

    tub("2022-06", "2022-07-01 00:00:00");
    tub("2022-12", "2023-01-01 00:00:00");

    tub("2022", "2023-01-01 00:00:00");

    auto t1 = Sawyer::Time::parse("9999").unwrap();
    ASSERT_always_require(t1.upperBound().unwrapError() == "cannot represent years greater than 9999");
}

static void
testRemove() {
    auto t1 = Sawyer::Time::parse("2022-06-30 23:30:45-0400").unwrap().noDate();
    ASSERT_always_require2(t1.toString() == "23:30:45-04:00", t1.toString());

    auto t2 = Sawyer::Time::parse("2022-06-30 23:30:45-0400").unwrap().noTime();
    ASSERT_always_require2(t2.toString() == "2022-06-30-04:00", t2.toString());

    auto t3 = Sawyer::Time::parse("2022-06-30 23:30:45-0400").unwrap().noZone();
    ASSERT_always_require2(t3.toString() == "2022-06-30 23:30:45", t3.toString());
}

static void
tu(const std::string &in, time_t ans) {
    auto t1 = Sawyer::Time::parse(in).expect("cannot parse " + in);
    time_t u = t1.toUnix().expect("toUnix failed for " + in);
    ASSERT_always_require2(u == ans, "in=\"" + in + "\", parsed=\"" + t1.toString() + "\", out=" +
                           boost::lexical_cast<std::string>(u) + ", ans=" + boost::lexical_cast<std::string>(ans));
}

static void
testUnix() {
    tu("1970-01-01T00:00:00Z", time_t{0});
    tu("1970-01-01T00:00:00-0400", time_t{4*3600});
    tu("2004-09-16T00:00:00Z", time_t{1095292800ul});

    tu("2038-01-19T03:14:07Z", time_t{2147483647ul});
    tu("2038-01-19T03:14:08Z", time_t{2147483648ul});

    tu("2106-02-07 06:28:15Z", time_t{4294967295ul});
    tu("2106-02-07 06:28:16Z", time_t{4294967296ul});
}

int main() {
    testNow();
    testConstruct();
    testParseDate();
    testParseTime();
    testParseZone();
    testParseFull();
    testResolve();
    testLowerBound();
    testUpperBound();
    testRemove();
    testUnix();
}
