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

    auto t5 = Sawyer::Time::parse("+00:30");
    ASSERT_always_require2(t5.isOk(), t5.unwrapError());
    ASSERT_always_require(t5.unwrap().isEmpty());
    ASSERT_always_forbid(t5.unwrap().hasDate());
    ASSERT_always_forbid(t5.unwrap().hasTime());
    ASSERT_always_require(t5.unwrap().hasZone());
    ASSERT_always_forbid(t5.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t5.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t5.unwrap().year());
    ASSERT_always_forbid(t5.unwrap().month());
    ASSERT_always_forbid(t5.unwrap().day());
    ASSERT_always_forbid(t5.unwrap().hour());
    ASSERT_always_forbid(t5.unwrap().minute());
    ASSERT_always_forbid(t5.unwrap().second());
    ASSERT_always_require(*t5.unwrap().timeZoneHour() == 0);
    ASSERT_always_require(*t5.unwrap().timeZoneMinute() == 30);
    ASSERT_always_require2(t5.unwrap().toString() == "+00:30", t5.unwrap().toString());
    ASSERT_always_require(t5.unwrap().toUnix().isError());

    auto t6 = Sawyer::Time::parse("+0030");
    ASSERT_always_require2(t6.isOk(), t6.unwrapError());
    ASSERT_always_require(t6.unwrap().isEmpty());
    ASSERT_always_forbid(t6.unwrap().hasDate());
    ASSERT_always_forbid(t6.unwrap().hasTime());
    ASSERT_always_require(t6.unwrap().hasZone());
    ASSERT_always_forbid(t6.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t6.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t6.unwrap().year());
    ASSERT_always_forbid(t6.unwrap().month());
    ASSERT_always_forbid(t6.unwrap().day());
    ASSERT_always_forbid(t6.unwrap().hour());
    ASSERT_always_forbid(t6.unwrap().minute());
    ASSERT_always_forbid(t6.unwrap().second());
    ASSERT_always_require(*t6.unwrap().timeZoneHour() == 0);
    ASSERT_always_require(*t6.unwrap().timeZoneMinute() == 30);
    ASSERT_always_require2(t6.unwrap().toString() == "+00:30", t6.unwrap().toString());
    ASSERT_always_require(t6.unwrap().toUnix().isError());

    auto t7 = Sawyer::Time::parse("-00:30");
    ASSERT_always_require2(t7.isOk(), t7.unwrapError());
    ASSERT_always_require(t7.unwrap().isEmpty());
    ASSERT_always_forbid(t7.unwrap().hasDate());
    ASSERT_always_forbid(t7.unwrap().hasTime());
    ASSERT_always_require(t7.unwrap().hasZone());
    ASSERT_always_forbid(t7.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t7.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t7.unwrap().year());
    ASSERT_always_forbid(t7.unwrap().month());
    ASSERT_always_forbid(t7.unwrap().day());
    ASSERT_always_forbid(t7.unwrap().hour());
    ASSERT_always_forbid(t7.unwrap().minute());
    ASSERT_always_forbid(t7.unwrap().second());
    ASSERT_always_require(*t7.unwrap().timeZoneHour() == 0);
    ASSERT_always_require(*t7.unwrap().timeZoneMinute() == -30);
    ASSERT_always_require2(t7.unwrap().toString() == "-00:30", t7.unwrap().toString());
    ASSERT_always_require(t7.unwrap().toUnix().isError());

    auto t8 = Sawyer::Time::parse("-0030");
    ASSERT_always_require2(t8.isOk(), t8.unwrapError());
    ASSERT_always_require(t8.unwrap().isEmpty());
    ASSERT_always_forbid(t8.unwrap().hasDate());
    ASSERT_always_forbid(t8.unwrap().hasTime());
    ASSERT_always_require(t8.unwrap().hasZone());
    ASSERT_always_forbid(t8.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t8.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t8.unwrap().year());
    ASSERT_always_forbid(t8.unwrap().month());
    ASSERT_always_forbid(t8.unwrap().day());
    ASSERT_always_forbid(t8.unwrap().hour());
    ASSERT_always_forbid(t8.unwrap().minute());
    ASSERT_always_forbid(t8.unwrap().second());
    ASSERT_always_require(*t8.unwrap().timeZoneHour() == 0);
    ASSERT_always_require(*t8.unwrap().timeZoneMinute() == -30);
    ASSERT_always_require2(t8.unwrap().toString() == "-00:30", t8.unwrap().toString());
    ASSERT_always_require(t8.unwrap().toUnix().isError());

    auto t9 = Sawyer::Time::parse("Z");
    ASSERT_always_require2(t9.isOk(), t9.unwrapError());
    ASSERT_always_require(t9.unwrap().isEmpty());
    ASSERT_always_forbid(t9.unwrap().hasDate());
    ASSERT_always_forbid(t9.unwrap().hasTime());
    ASSERT_always_require(t9.unwrap().hasZone());
    ASSERT_always_forbid(t9.unwrap().hasSpecificDate());
    ASSERT_always_forbid(t9.unwrap().hasSpecificTime());
    ASSERT_always_forbid(t9.unwrap().year());
    ASSERT_always_forbid(t9.unwrap().month());
    ASSERT_always_forbid(t9.unwrap().day());
    ASSERT_always_forbid(t9.unwrap().hour());
    ASSERT_always_forbid(t9.unwrap().minute());
    ASSERT_always_forbid(t9.unwrap().second());
    ASSERT_always_require(*t9.unwrap().timeZoneHour() == 0);
    ASSERT_always_require(*t9.unwrap().timeZoneMinute() == 0);
    ASSERT_always_require2(t9.unwrap().toString() == "Z", t9.unwrap().toString());
    ASSERT_always_require(t9.unwrap().toUnix().isError());

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
zulu(const std::string &in, const std::string &ans) {
    auto t = Sawyer::Time::parse(in).expect("cannot parse " + in);
    auto z = t.toZulu().expect("convertToZulu failed for " + in);
    ASSERT_always_require2(z.toString() == ans,
                           "input string: \"" + in + "\"\n"
                           "  parsed as:    \"" + t.toString() + "\"\n"
                           "  converted to: \"" + z.toString() + "\"\n"
                           "  expected ans: \"" + ans + "\"");
}

static void
testTimezoneConversion() {
    // Converting a time with no timezone or time is a no-op
    zulu("2022-06-30",          "2022-06-30");
    zulu("12:30:00",            "12:30:00");
    zulu("-0400",               "-04:00");
    zulu("2022-06-30-0400",     "2022-06-30-04:00");
    zulu("2022-06-30 12:30:00", "2022-06-30 12:30:00");

    // Converting from west to east adds the zone offset and vice versa
    zulu("2022-06-30 12:00:00-04:00", "2022-06-30 16:00:00Z");
    zulu("2022-06-30 12:00:00+04:00", "2022-06-30 08:00:00Z");

    // Missing dates are still missing in the output
    zulu("12:00:00-04:00",         "16:00:00Z");
    zulu("2022 12:00:00-04:00",    "2022 16:00:00Z");
    zulu("2022-06 12:00:00-04:00", "2022-06 16:00:00Z");

    // Missing seconds and/or minutes are missing in the output
    zulu("12:10-04:30", "16:40Z");
    zulu("T12-04:30",   "T16Z");

    // Overflow resulting in carrying
    zulu("12:45:00-00:30", "13:15:00Z");                // carry minutes to hours
    zulu("12:45-00:30", "13:15Z");                      // carry minutes to hours, no seconds
    zulu("T12-00:30", "T12Z");                          // hours only, so no carrying minute timezone
    zulu("23:45:00-00:30", "00:15:00Z");                // carry minutes, wrapping hours
    zulu("23:45-00:30", "00:15Z");                      // carry minutes, wrapping hours, no seconds
    zulu("T23-00:30", "T23Z");                          // hours only, so no carrying minute timzeone
    zulu("12:59-00:59", "13:58Z");                      // carry minutes to non-zero minutes
    zulu("23:00:00-01", "00:00:00+00");                 // carry one minute to wrap hours
    zulu("2022-11-29 23:59:00-00:02", "2022-11-30 00:01:00Z");// carry to date
    zulu("2022-11-30 23:59:00-00:02", "2022-12-01 00:01:00Z");// carry to month
    zulu("2022-11-30 23:59:00-23:59", "2022-12-01 23:58:00Z");// carry large tz to month
    zulu("2022-12-01 00:00:00-0002", "2022-12-01 00:02:00Z");// no carrying
    zulu("2022-02-28 23:59:00-0002", "2022-03-01 00:01:00Z");// carry to march 1st, non-leap year
    zulu("2020-02-28 23:59:00-0002", "2020-02-29 00:01:00Z");// carry to feb 29th on leap year

    // Overflow resulting in borrow
    zulu("13:15:00+00:30", "12:45:00Z");                // borrow from hours
    zulu("13:15+00:30", "12:45Z");                      // borrow from hours, no seconds
    zulu("T12+00:30", "T12Z");                          // hours only, no borrow necessary
    zulu("00:15:00+00:30", "23:45:00Z");                // borrow hours, wrapping
    zulu("00:15+00:30", "23:45Z");                      // borrow hours, wrapping, no seconds
    zulu("T23+00:30", "T23Z");                          // hours only, no borrow necessary
    zulu("13:58+00:59", "12:59Z");                      // borrow hours
    zulu("00:00:00+0002", "23:58:00Z");                 // borrow to two minutes before midnight
    zulu("00:00:00+02", "22:00:00+00");                 // borrow to two hours before midnight
    zulu("2022-12-01 00:00:00+0002", "2022-11-30 23:58:00Z");
    zulu("2022-01-01 00:00:00+0002", "2021-12-31 23:58:00Z");
    zulu("2022-03-01 00:00:00+0002", "2022-02-28 23:58:00Z");// borrow to Feb 28, non-leap year
    zulu("2020-03-01 00:00:00+0002", "2020-02-29 23:58:00Z");// borrow to Feb 29, leap year

    // Year overflow results in errors
    auto t1 = Sawyer::Time::parse("9999-12-31 23:59:00-00:01").unwrap().toZulu();
    ASSERT_always_require(t1.isError());
    ASSERT_always_require(t1.unwrapError() == "cannot represent years after 9999");

    auto t2 = Sawyer::Time::parse("1583-01-01 00:00:00+00:01").unwrap().toZulu();
    ASSERT_always_require(t2.isError());
    ASSERT_always_require(t2.unwrapError() == "cannot represent years before 1583");
}

static void
tu(const std::string &in, time_t ans) {
    auto t1 = Sawyer::Time::parse(in).expect("cannot parse " + in);
    time_t u = t1.toUnix().expect("toUnix failed for " + in);
    ASSERT_always_require2(u == ans, "in=\"" + in + "\", parsed=\"" + t1.toString() + "\", u=" +
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

static void
eq(const std::string &lhs, const std::string &rhs, bool ansEq, bool ansNe) {
    auto a = Sawyer::Time::parse(lhs).expect("cannot parse lhs " + lhs);
    auto b = Sawyer::Time::parse(rhs).expect("cannot parse rhs " + rhs);
    bool eq = a == b;
    bool ne = a != b;
    ASSERT_always_require2(eq == ansEq,
                           "lhs input:      " + lhs + "\n"
                           "  lhs parsed:     " + a.toString() + "\n"
                           "  rhs input:      " + rhs + "\n"
                           "  rhs parsed:     " + b.toString() + "\n"
                           "  tested equal:   " + (eq ? "true" : "false") + "\n"
                           "  expected equal: " + (ansEq ? "true" : "false"));
    ASSERT_always_require2(eq == ansEq,
                           "lhs input:        " + lhs + "\n"
                           "  lhs parsed:       " + a.toString() + "\n"
                           "  rhs input:        " + rhs + "\n"
                           "  rhs parsed:       " + b.toString() + "\n"
                           "  tested unequal:   " + (ne ? "true" : "false") + "\n"
                           "  expected unequal: " + (ansNe ? "true" : "false"));
}

static void
testEquality() {
    // equal
    eq("2022-10-11 11:49:58Z", "2022-10-11 11:49:58Z", true, false);
    eq("2022-10-11 11:49:58", "2022-10-11 11:49:58", true, false); // no zones
    eq("2022-10-11 11:49Z", "2022-10-11 11:49Z", true, false);     // no seconds
    eq("2022-10-11 T11Z", "2022-10-11 T11Z", true, false);         // no seconds or minutes
    eq("2022-10-11", "2022-10-11", true, false);                   // no time
    eq("2022-10 11:49:58Z", "2022-10 11:49:58Z", true, false);     // no day
    eq("2022 11:49:58Z", "2022 11:49:58Z", true, false);           // no day or month
    eq("11:49:58Z", "11:49:58Z", true, false);                     // no date
    eq("12:00-0400", "16:00Z", true, false);                       // different timezones

    // not equal
    eq("2022-10-11 11:49:58Z", "2022-10-11 11:49:59Z", false, true); // different seconds
    eq("2022-10-11 11:49:58Z", "2022-10-11 11:50:58Z", false, true); // different minutes
    eq("2022-10-11 11:49:58Z", "2022-10-11 12:49:58Z", false, true); // different hours
    eq("2022-10-11 11:49:58Z", "2022-10-12 11:49:58Z", false, true); // different days
    eq("2022-10-11 11:49:58Z", "2022-11-11 11:49:58Z", false, true); // different months
    eq("2022-10-11 11:49:58Z", "2023-10-11 11:49:58Z", false, true); // different years
    eq("2022-10-11 12:00:00-04:00", "2022-10-11 12:00:00+04:00", false, true); // different timezones
    eq("2022-10-11 11:49:58Z", "2022-10-11 11:49Z", false, true);    // missing seconds
    eq("2022-10-11 11:49:58Z", "2022-10-11 T11Z", false, true);      // missing minutes and seconds
    eq("2022-10-11 11:49:58Z", "2022-10-11Z", false, true); // missing time
    eq("2022-10-11 11:49:58Z", "2022-10 11:49:58Z", false, true); // missing day
    eq("2022-10-11 11:49:58Z", "2022 11:49:58Z", false, true); // missing month and day
    eq("2022-10-11 11:49:58Z", "11:49:58Z", false, true); // missing date
    eq("2022-10-11 11:49:58Z", "2022-10-11 11:49:58", false, true); // missing zone
    eq("2022-10-11 12:00:00-04:00", "2022-10-11 12:00:00+04:00", false, true); // different timezones
}


static void
lt(const std::string &lhs, const std::string &rhs, bool ansLt, bool ansGt) {
    auto a = Sawyer::Time::parse(lhs).expect("cannot parse lhs " + lhs);
    auto b = Sawyer::Time::parse(rhs).expect("cannot parse rhs " + rhs);
    bool lt = a < b;
    bool gt = b < a;
    ASSERT_always_require2(lt == ansLt,
                           "lhs input:   " + lhs + "\n"
                           "  lhs parsed:  " + a.toString() + "\n"
                           "  rhs input:   " + rhs + "\n"
                           "  rhs parsed:  " + b.toString() + "\n"
                           "  tested lt:   " + (lt ? "true" : "false") + "\n"
                           "  expected lt: " + (ansLt ? "true" : "false"));
    ASSERT_always_require2(gt == ansGt,
                           "lhs input:   " + lhs + "\n"
                           "  lhs parsed:  " + a.toString() + "\n"
                           "  rhs input:   " + rhs + "\n"
                           "  rhs parsed:  " + b.toString() + "\n"
                           "  tested gt:   " + (gt ? "true" : "false") + "\n"
                           "  expected gt: " + (ansGt ? "true" : "false"));
}

static void
testLessThan() {
    // equal
    lt("2022-10-11 11:49:58Z", "2022-10-11 11:49:58Z", false, false);
    lt("2022-10-11 11:49:58", "2022-10-11 11:49:58", false, false); // no zones
    lt("2022-10-11 11:49Z", "2022-10-11 11:49Z", false, false);     // no seconds
    lt("2022-10-11 T11Z", "2022-10-11 T11Z", false, false);         // no seconds or minutes
    lt("2022-10-11", "2022-10-11", false, false);                   // no time
    lt("2022-10 11:49:58Z", "2022-10 11:49:58Z", false, false);     // no day
    lt("2022 11:49:58Z", "2022 11:49:58Z", false, false);           // no day or month
    lt("11:49:58Z", "11:49:58Z", false, false);                     // no date
    lt("12:00-0400", "16:00Z", false, false);                       // different timezones

    // One item changed
    lt("2022-10-11 11:49:58Z", "2022-10-11 11:49:59Z", true, false); // second
    lt("2022-10-11 11:49:58Z", "2022-10-11 11:50:58Z", true, false); // minute
    lt("2022-10-11 11:49:58Z", "2022-10-11 12:49:58Z", true, false); // hour
    lt("2022-10-11 11:49:58Z", "2022-10-12 11:49:58Z", true, false); // day
    lt("2022-10-11 11:49:58Z", "2022-11-11 11:49:58Z", true, false); // month
    lt("2022-10-11 11:49:58Z", "2023-10-11 11:49:58Z", true, false); // year

    // Item missing
    lt("2022-10-11 11:49:58-04", "2022-10-11 11:49:58-04:00", true, false); // timezone minute
    lt("2022-10-11 11:49:58", "2022-10-11 11:49:58Z", true, false);      // timezone
    lt("2022-10-11 11:49Z", "2022-10-11 11:49:58Z", true, false);        // seconds
    lt("2022-10-11 T11Z", "2022-10-11 11:49:58Z", true, false);          // minutes and seconds
    lt("2022-10-11Z", "2022-10-11 11:49:58Z", true, false);              // time
    lt("2022-10 11:49:58Z", "2022-10-11 11:49:58Z", true, false);        // day
    lt("2022 11:49:58Z", "2022-10-11 11:49:58Z", true, false);           // month
    lt("11:49:58Z", "2022-10-11 11:49:58Z", true, false);                // date

    // Time zone conversion
    lt("2022-10-11 12:00+0400", "2022-10-11 11:00Z", true, false);
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
    testTimezoneConversion();
    testUnix();
    testEquality();
    testLessThan();
}
