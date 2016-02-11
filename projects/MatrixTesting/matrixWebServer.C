#include <rosePublicConfig.h>

#include <Sawyer/Message.h>

using namespace Sawyer::Message::Common;
static Sawyer::Message::Facility mlog;

#ifdef ROSE_USE_WT

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <Color.h>                                      // ROSE
#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <SqlDatabase.h>                                // ROSE
#include <string>
#include <vector>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WAbstractTableModel>
#include <Wt/WApplication>
#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WLength>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WTable>
#include <Wt/WTableView>
#include <Wt/WTabWidget>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

static const char* WILD_CARD_STR = "*";
enum ChartType { BAR_CHART, LINE_CHART };
enum ChartValueType { CVT_COUNT, CVT_PERCENT, CVT_PASS_RATIO, CVT_WARNINGS_AVE, CVT_DURATION_AVE };
static int END_STATUS_POSITION = 999;                   // test_names.position where name = 'end'

typedef Sawyer::Container::Map<std::string, int> StringIndex;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Represents a bucket of values by storing a min and max value.
template<typename T>
class Bucket {
    T minValue_, maxValue_;
    bool isEmpty_;

public:
    Bucket(): isEmpty_(true) {}
    
    Bucket(const T &value): minValue_(value), maxValue_(value), isEmpty_(false) {}

    bool isEmpty() const { return isEmpty_; }
    T minValue() const { return minValue_; }
    T maxValue() const { return maxValue_; }

    // Insert another item into the bucket by adjusting the min/max values
    void operator<<(const T &value) {
        if (isEmpty_) {
            minValue_ = maxValue_ = value;
        } else {
            minValue_ = std::min(minValue_, value);
            maxValue_ = std::max(maxValue_, value);
        }
        isEmpty_ = false;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Info about a dependency.
struct Dependency {
    typedef Sawyer::Container::Map<std::string, Bucket<std::string> > Choices;

    std::string name;                                   // name of dependency, such as "boost"
    Choices humanValues;                                // human-readable values and how they map to the database values
    Wt::WComboBox *comboBox;                            // choices available to the user

    Dependency()
        : comboBox(NULL) {}

    explicit Dependency(const std::string &name)
        : name(name), comboBox(NULL) {}
};

typedef Sawyer::Container::Map<std::string /*name*/, Dependency> Dependencies;
typedef Sawyer::Container::Map<std::string /*name*/, std::string /*column*/> DependencyNames;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stuff initialized before the first server thread is created, and which remains constant once the serving starts.
struct GlobalState {
    std::string docRoot;
    std::string httpAddress;
    unsigned short httpPort;
    std::string dbUrl;
    DependencyNames dependencyNames;
    std::vector<std::string> testNames;
    StringIndex testNameIndex;
    SqlDatabase::TransactionPtr tx;

    GlobalState()
        : docRoot("."), httpAddress("0.0.0.0"), httpPort(80), dbUrl(DEFAULT_DATABASE) {}
};
static GlobalState gstate;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::string
sqlFromClause() {
    return (" from test_results"
            " join users on test_results.reporting_user = users.uid"
            " left outer join test_names on test_results.status = test_names.name");
}

static std::string
sqlWhereClause(const Dependencies &deps, std::vector<std::string> &args) {
    std::string where;
    BOOST_FOREACH (const Dependency &dep, deps.values()) {
        std::string humanValue = dep.comboBox->currentText().narrow();
        Bucket<std::string> bucket;
        if (humanValue.compare(WILD_CARD_STR) != 0 && dep.humanValues.getOptional(humanValue).assignTo(bucket)) {
            std::string depColumn = gstate.dependencyNames[dep.name];
            where += std::string(where.empty() ? " where " : " and ");
            if (bucket.minValue() == bucket.maxValue()) {
                where += depColumn + " = ?";
                args.push_back(bucket.minValue());
            } else {
                where += depColumn + " >= ? and " + depColumn + " <= ?";
                args.push_back(bucket.minValue());
                args.push_back(bucket.maxValue());
            }
        }
    }
    if (where.empty())
        where = " where true";
    return where;
}

static void
bindSqlVariables(const SqlDatabase::StatementPtr &q, const std::vector<std::string> &args) {
    for (size_t i=0; i<args.size(); ++i)
        q->bind(i, args[i]);
}

// Sorts dependency values
class DependencyValueSorter {
    std::string depName_;

public:
    explicit DependencyValueSorter(const std::string &depName)
        : depName_(depName) {}

    bool operator()(const std::string &a, const std::string &b) {
        if (depName_ == "status") {
            // Status (failed test names) should be sorted in the order that the tests run.
            int ai = gstate.testNameIndex.getOrElse(a, 900);
            int bi = gstate.testNameIndex.getOrElse(b, 900);
            return ai < bi;
        } else if (depName_ == "compiler") {
            // Compilers have three-part names: VENDOR-VERSION-LANGUAGE like "gcc-4.8.4-c++11". We should sort these are three
            // columns and the LANGUAGE should be sorted so "default" is less than everything but the empty string.
            std::vector<std::string> ac = StringUtility::split("-", a, 3, true);
            std::vector<std::string> bc = StringUtility::split("-", b, 3, true);
            ac.resize(3);
            bc.resize(3);
            if (ac[0] != bc[0])
                return ac[0] < bc[0];
            if (ac[1] != bc[1])
                return ac[1] < bc[1];
            if (ac[2] == "" || bc[2] == "")
                return ac[2] < bc[2];
            if (ac[2] == "default")
                return bc[2] != "default";
            if (bc[2] == "default")
                return false;
            return ac[2] < bc[2];
        } else {
            return a < b;
        }
    }
};

// What does it mean to "pass"?  The special virtual dependency "pass/fail" returns the word "pass" or "fail" depending
// on our current definition of pass/fail.  The default definition is that any test whose status = "end" is considered to have
// passed and any other status is a failure.  However, we can change the definition to be any test whose status is greater than
// or equal to some specified value is a pass. By "greater than or equal" we mean the result position from teh "test_names"
// table.
static void
setPassDefinition(const std::string &minimumPassStatus) {
    int position = gstate.testNameIndex.getOrElse(minimumPassStatus, END_STATUS_POSITION);
    std::string passDefinition = "case"
                                 " when test_names.position >= " + StringUtility::numberToString(position) +
                                 " then 'pass' else 'fail' end";
    gstate.dependencyNames.insert("pass/fail", passDefinition);
}


enum HumanFormat { HUMAN_TERSE, HUMAN_VERBOSE };

static std::string
humanLocalTime(unsigned long when, HumanFormat fmt = HUMAN_VERBOSE) {
    struct tm tm;
    time_t t = when;
    localtime_r(&t, &tm);

    char buf[256];
    if (HUMAN_VERBOSE == fmt) {
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d %+ld:%02d",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                tm.tm_gmtoff / 3600, abs(tm.tm_gmtoff / 60 % 60));
    } else {
        sprintf(buf, "%04d-%02d-%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
    }
    return buf;
}

static std::string
humanDuration(unsigned long seconds, HumanFormat fmt = HUMAN_VERBOSE) {
    unsigned hours = seconds / 3600;
    unsigned minutes = seconds / 60 % 60;
    seconds %= 60;

    if (HUMAN_VERBOSE == fmt) {
        std::string retval = boost::lexical_cast<std::string>(seconds) + " second" + (1==seconds?"":"s");
        if (hours > 0 || minutes > 0)
            retval = boost::lexical_cast<std::string>(minutes) + " minute" + (1==minutes?"":"s") + " " + retval;
        if (hours > 0)
            retval = boost::lexical_cast<std::string>(hours) + " hour" + (1==hours?"":"s") + " " + retval;
        return retval;
    } else {
        char buf[256];
        sprintf(buf, "%2d:%02d:%02u", hours, minutes, (unsigned)seconds);
        return buf;
    }
}

static std::string
humanSha1(const std::string &sha1, HumanFormat fmt = HUMAN_VERBOSE) {
    if (HUMAN_VERBOSE == fmt)
        return sha1;
    std::string s;
    int pos = 0;
    BOOST_FOREACH (char ch, sha1) {
        if (!isxdigit(ch))
            break;
        if (++pos <= 8)
            s += ch;
    }
    s += sha1.substr(pos);
    return s;
}

static std::string
humanDepValue(const std::string &depName, const std::string &depValue, HumanFormat fmt = HUMAN_VERBOSE) {
    if (depName == "reporting_time")
        return humanLocalTime(boost::lexical_cast<long>(depValue), fmt);
    if (depName == "rose_date")
        return humanLocalTime(boost::lexical_cast<long>(depValue), fmt);
    if (depName == "duration")
        return humanDuration(boost::lexical_cast<long>(depValue), fmt);
    if (depName == "rose")
        return humanSha1(depValue, fmt);
    return depValue;
}

// Clip val to be in the interval [minVal,maxVal]
template<typename T>
static T
clip(T val, T minVal, T maxVal) {
    return std::min(std::max(minVal, val), maxVal);
}

// Returns a CSS class name "redgreen-X-Y" where X is an integer in [0,100] depending on where val falls in the interval
// [minVal,maxVal] and Y is an integer in the range [0,4] based on how many samples are present.
static Wt::WString
redToGreen(double val, double minVal, double maxVal, int nSamples=4) {
    int fade = std::min((int)round(nSamples), 4);
    if (0 == nSamples) {
        return Wt::WString("chart-zero");
    } else if (maxVal - minVal < 1.0) {
        return Wt::WString("redgreen-50-" + StringUtility::numberToString(fade));
    } else {
        val = clip(val, minVal, maxVal);
        int percentile = round(100.0 * (val - minVal) / (maxVal - minVal));
        return Wt::WString("redgreen-" + StringUtility::numberToString(percentile) +
                           "-" + StringUtility::numberToString(fade));
    }
}

// Returns a CSS class name "redgreen-X-Y" where X is an integer in [100,0] depending on where val falls in the interval
// [minVal,maxVal] and Y is an integer in the range [0,4] based on how many samples are present.
static Wt::WString
greenToRed(double val, double minVal, double maxVal, int nSamples=4) {
    int fade = std::min((int)round(nSamples), 4);
    if (0 == nSamples) {
        return Wt::WString("chart-zero");
    } else if (maxVal - minVal < 1.0) {
        return Wt::WString("redgreen-50-" + StringUtility::numberToString(fade));
    } else {
        val = clip(val, minVal, maxVal);
        int percentile = round(100.0 * (maxVal - val) / (maxVal - minVal));
        return Wt::WString("redgreen-" + StringUtility::numberToString(percentile) +
                           "-" + StringUtility::numberToString(fade));
    }
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model of test results.  This is a two dimensional table. The rows of the table correspond to values of the major dependency
// and the columns of the table correspond to values of the minor dependency.  If the minor dependency name is the empty string
// then the various test names (database "status" column of the "test_results" table) are used. The major and/or minor values
// can be part of the model's data by setting depMajorIsData and/or depMinorIsData. If depMajorIsData is set then column zero
// of the model contains the values of the major dependency; if depMinorIsData is set then row zero of the model contains the
// values of the minor dependency. If both are set then model element (0,0) has no data.  In any case, the horizontal and
// vertical headers are also the minor and major dependency values, respectively.
//
// The data stored at index (i,j), at least when dependency values are not stored as data (otherwise these statements only
// apply to the non-dependency part of the model) are floating point values that count the number of rows of the "test_results"
// table that matched major dependency i and minor dependency j.
class StatusModel: public Wt::WAbstractTableModel {
private:
    typedef std::vector<double> TableRow;
    typedef std::vector<TableRow> Table;
    Table counts_;                                      // counts of matching rows
    double minCounts_, maxCounts_;                      // min/max values in counts_ (excluding zeros)
    Table passes_;                                      // portion of counts_ where pass_fail = 'passed'
    Table aveWarnings_;                                 // average number of compiler warnings per run
    double minAveWarnings_, maxAveWarnings_;            // min/max value in aveWarnings_
    Table aveDuration_;                                 // average wall-clock duration per run
    double minAveDuration_, maxAveDuration_;            // min/max value in aveDuration_
    ChartValueType chartValueType_;                     // whether to show counts, percents, or pass ratios
    bool roundToInteger_;                               // round cell values to nearest integer
    bool humanReadable_;                                // return data as human-radable strings instead of doubles

    std::string depMajorName_;                          // dependency for major axis
    std::vector<std::string> depMajorValues_;           // human-format values of major dependency
    StringIndex depMajorIndex_;                         // maps depMajorValues_ to counts_ row numbers
    bool depMajorIsData_;                               // if true then depMajorValues are column zero of the model

    std::string depMinorName_;                          // dependency for minor axis (often "status")
    std::vector<std::string> depMinorValues_;           // values of minor dependency or status values
    StringIndex depMinorIndex_;                         // maps depMinorValues_ to counts_ column numbers
    bool depMinorIsData_;                               // if true then depMinorValues are row zero of the model
    
public:
    explicit StatusModel(Wt::WObject *parent = NULL)
        : Wt::WAbstractTableModel(parent), chartValueType_(CVT_COUNT), roundToInteger_(false), humanReadable_(false),
          depMajorName_("rose_date"), depMajorIsData_(false), depMinorName_("pass/fail"), depMinorIsData_(false) {}

    const std::string& depMajorName() const {
        return depMajorName_;
    }

    void setDepMajorName(const std::string &s) {
        depMajorName_ = s;
    }

    const std::string& depMinorName() const {
        return depMinorName_;
    }

    void setDepMinorName(const std::string &s) {
        depMinorName_ = s;
    }

    ChartValueType chartValueType() const {
        return chartValueType_;
    }

    void setChartValueType(ChartValueType t) {
        chartValueType_ = t;
    }

    bool depMajorIsData() const {
        return depMajorIsData_;
    }

    void setDepMajorIsData(bool b) {
        depMajorIsData_ = b;
    }

    bool depMinorIsData() const {
        return depMinorIsData_;
    }

    void setDepMinorIsData(bool b) {
        depMinorIsData_ = b;
    }

    bool roundToInteger() const {
        return roundToInteger_;
    }

    void setRoundToInteger(bool b) {
        roundToInteger_ = b;
    }

    bool humanReadable() const {
        return humanReadable_;
    }

    void setHumanReadable(bool b) {
        humanReadable_ = b;
    }

    const std::string depMajorValue(size_t modelRow) {
        size_t i = modelRow - (depMinorIsData_ ? 1 : 0);
        return i < depMajorValues_.size() ? depMajorValues_[i] : std::string();
    }

    const std::string depMinorValue(size_t modelColumn) {
        size_t j = modelColumn - (depMajorIsData_ ? 1 : 0);
        return j < depMinorValues_.size() ? depMinorValues_[j] : std::string();
    }

    void updateModel(const Dependencies &deps) {
        Sawyer::Message::Stream debug(::mlog[DEBUG] <<"StatusModel::updateModel...\n");
        updateDepMajor(deps);
        updateDepMinor(deps);
        resetTable(counts_);
        minCounts_ = maxCounts_ = 0.0;
        resetTable(passes_);
        resetTable(aveWarnings_);
        minAveWarnings_ = maxAveWarnings_ = 0.0;
        resetTable(aveDuration_);
        minAveDuration_ = maxAveDuration_ = 0.0;
        
        if (counts_.empty())
            return modelReset().emit();

        // Build the SQL query
        ASSERT_require(!depMajorName_.empty());
        std::string depMajorColumn = gstate.dependencyNames[depMajorName_];
        std::string depMinorColumn = gstate.dependencyNames[depMinorName_];
        std::string passFailColumn = gstate.dependencyNames["pass/fail"];
        std::vector<std::string> args;
        std::string sql = "select " +
                          depMajorColumn + ", " +       // 0
                          depMinorColumn + ", " +       // 1
                          passFailColumn + " as pf, "   // 2
                          "count(*), " +                // 3
                          "sum(nwarnings)," +           // 4
                          "sum(duration)" +             // 5
                          sqlFromClause() +
                          sqlWhereClause(deps, args /*out*/) +
                          " group by " + depMajorColumn + ", " + depMinorColumn + ", pf";
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, args);

        // Iterate over the query results to update the table. Remember that the row and column numbers are the human-style
        // values (e.g., yyyy-mm-dd rather than Unix time, etc.)
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            std::string majorValue = humanDepValue(depMajorName_, row.get<std::string>(0), HUMAN_TERSE);
            std::string minorValue = humanDepValue(depMinorName_, row.get<std::string>(1), HUMAN_TERSE);
            std::string pf = row.get<std::string>(2);
            int count = row.get<int>(3);
            double nwarn = row.get<int>(4);
            double duration = row.get<int>(5);

            int i = depMajorIndex_.getOrElse(majorValue, -1);
            int j = depMinorIndex_.getOrElse(minorValue, -1);
            if (i >= 0 && j >= 0) {
                counts_[i][j] += count;
                aveWarnings_[i][j] += nwarn;            // sum here; adjusted below
                aveDuration_[i][j] += duration;         // sum here; adjusted below
                if (pf == "pass")
                    passes_[i][j] += count;
            }
        }

        // Adjust aveWarnings and aveDuration to be averages instead of sums
        {
            int n = 0;
            for (size_t i=0; i<aveWarnings_.size(); ++i) {
                for (size_t j=0; j<aveWarnings_[i].size(); ++j) {
                    if (counts_[i][j] > 0) {
                        aveWarnings_[i][j] /= counts_[i][j];
                        aveDuration_[i][j] /= counts_[i][j];
                        if (0 == n++) {
                            minCounts_ = maxCounts_ = counts_[i][j];
                            minAveWarnings_ = maxAveWarnings_ = aveWarnings_[i][j];
                            minAveDuration_ = maxAveDuration_ = aveDuration_[i][j];
                        } else {
                            minCounts_      = std::min(minCounts_,      counts_[i][j]);
                            maxCounts_      = std::max(maxCounts_,      counts_[i][j]);
                            minAveWarnings_ = std::min(minAveWarnings_, aveWarnings_[i][j]);
                            maxAveWarnings_ = std::max(maxAveWarnings_, aveWarnings_[i][j]);
                            minAveDuration_ = std::min(minAveDuration_, aveDuration_[i][j]);
                            maxAveDuration_ = std::max(maxAveDuration_, aveDuration_[i][j]);
                        }
                    }
                }
            }
        }

        if (debug) {
            debug <<"  major \"" <<StringUtility::cEscape(depMajorName_) <<"\" =";
            BOOST_FOREACH (const std::string &v, depMajorValues_)
                debug <<" " <<StringUtility::cEscape(v);
            debug <<"\n";
            debug <<"  minor \"" <<StringUtility::cEscape(depMinorName_) <<"\" =";
            BOOST_FOREACH (const std::string &v, depMinorValues_)
                debug <<" " <<StringUtility::cEscape(v);
            debug <<"\n";

            for (size_t i=0; i<counts_.size(); ++i) {
                debug <<"[" <<i <<"] =";
                for (size_t j=0; j<counts_[i].size(); ++j) {
                    debug <<" " <<counts_[i][j];
                }
                debug <<"\n";
            }
        }
        
        modelReset().emit();
    }

    int rowCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const ROSE_OVERRIDE {
        if (parent.isValid())
            return 0;
        return depMajorValues_.size() + (depMinorIsData_ ? 1 : 0);
    }
    
    int columnCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const ROSE_OVERRIDE {
        if (parent.isValid())
            return 0;
        return depMinorValues_.size() + (depMajorIsData_ ? 1 : 0);
    }

    boost::any data(const Wt::WModelIndex &index, int role = Wt::DisplayRole) const ROSE_OVERRIDE {
        Sawyer::Message::Stream debug(::mlog[DEBUG]);
        ASSERT_require(index.isValid());
        if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
            return boost::any();

        // i and j are indexes relative to counts_[0][0]
        int i = index.row()    - (depMinorIsData_ ? 1 : 0);
        int j = index.column() - (depMajorIsData_ ? 1 : 0);
        ASSERT_require(-1 == i || (size_t)i < counts_.size());
        ASSERT_require(-1 == j || (size_t)j < counts_[i].size());

        if (Wt::DisplayRole == role) {
            if (-1 == i && -1 == j) {
                // model index (0,0) is not used when major and minor dependency values are both stored as data
                return std::string("origin");
            } else if (-1 == j) {                       // querying a depMajorValue
                ASSERT_require(i >= 0 && (size_t)i < depMajorValues_.size());
                return depMajorValues_[i];
            } else if (-1 == i) {                       // querying a depMinorValue
                ASSERT_require(j >= 0 && (size_t)j < depMinorValues_.size());
                return depMinorValues_[j];
            } else if (0 == counts_[i][j]) {
                return 0.0;
            } else if (CVT_PERCENT == chartValueType_) {// counts divided by total for entire row as a percent
                double rowTotal = 0;
                BOOST_FOREACH (double count, counts_[i])
                    rowTotal += count;
                double percent = 100.0 * counts_[i][j] / rowTotal;
                return roundToInteger_ ? round(percent) : percent;
            } else if (CVT_PASS_RATIO == chartValueType_) {// number of passes divided by count as a percent
                double percent = 100.0 * passes_[i][j] / counts_[i][j];
                return roundToInteger_ ? round(percent) : percent;
            } else if (CVT_WARNINGS_AVE == chartValueType_) {// average number of warnings per test
                return roundToInteger_ ? round(aveWarnings_[i][j]) : aveWarnings_[i][j];
            } else if (CVT_DURATION_AVE == chartValueType_) {// average run duration per test in seconds
                if (humanReadable_)
                    return humanDuration(aveDuration_[i][j], HUMAN_TERSE);
                return roundToInteger_ ? round(aveDuration_[i][j]) : aveDuration_[i][j];
            } else {                                    // raw counts
                return counts_[i][j];
            }

        } else if (Wt::StyleClassRole == role) {
            if (i >= 0 && j >= 0) {
                switch (chartValueType_) {
                    case CVT_COUNT:
                        return redToGreen(counts_[i][j], minCounts_, maxCounts_);
                    case CVT_PERCENT: {
                        double rowTotal = 0;
                        BOOST_FOREACH (double count, counts_[i])
                            rowTotal += count;
                        double percent = rowTotal > 0 ? 100.0 * counts_[i][j] / rowTotal : 0.0;
                        return redToGreen(percent, 0.0, 100.0);
                    }
                    case CVT_PASS_RATIO: {
                        double pfratio = 0.0;
                        if (counts_[i][j] > 0)
                            pfratio = passes_[i][j] / counts_[i][j];
                        return redToGreen(pfratio, 0.0, 1.0, counts_[i][j]);
                    }
                    case CVT_WARNINGS_AVE:
                        return greenToRed(aveWarnings_[i][j], minAveWarnings_, maxAveWarnings_, counts_[i][j]);
                    case CVT_DURATION_AVE:
                        return greenToRed(aveDuration_[i][j], minAveDuration_, maxAveDuration_, counts_[i][j]);
                }
            }
        }
        
        return boost::any();
    }

    boost::any headerData(int section, Wt::Orientation orientation = Wt::Horizontal,
                          int role = Wt::DisplayRole) const ROSE_OVERRIDE {
        if (Wt::DisplayRole == role) {
            if (Wt::Horizontal == orientation) {
                if (depMajorIsData_)
                    --section;
                if (-1 == section)
                    return depMajorName_;
                if (section >= 0 && (size_t)section < depMinorValues_.size())
                    return depMinorValues_[section];
            } else {
                if (depMinorIsData_)
                    --section;
                if (section >= 0 && (size_t)section < depMajorValues_.size())
                    return depMajorValues_[section];
            }
        }
        return boost::any();                            // not used by 2-d or 3-d charts?
    }

private:
    // Resizes the table according to the depMajorValues_ and depMinorValues_ and resets all entries to zero.
    void resetTable(Table &t) {
        TableRow row(depMinorValues_.size(), 0.0);
        t.clear();
        t.resize(depMajorValues_.size(), row);
    }

    // Update the depMajorValues_ and depMajorIndex_ according to depMajorName_
    void updateDepMajor(const Dependencies &deps) {
        if (depMajorName_.empty()) {
            depMajorValues_.clear();
            depMajorIndex_.clear();
        } else {
            fillVectorIndexFromTable(deps, depMajorName_, depMajorValues_, depMajorIndex_);
        }
    }

    // Update the depMinorValues_ and depMinorIndex_ according to depMinorName_
    void updateDepMinor(const Dependencies &deps) {
        if (depMajorName_.empty()) {
            depMinorValues_.clear();
            depMinorIndex_.clear();
        } else {
            fillVectorIndexFromTable(deps, depMinorName_, depMinorValues_, depMinorIndex_);
        }
    }
    
    // Fill an value vector and its index with data from the table.
    void fillVectorIndexFromTable(const Dependencies &deps, const std::string &depName,
                                  std::vector<std::string> &values /*out*/, StringIndex &index /*out*/) {
        values.clear();
        index.clear();

        std::string columnName = gstate.dependencyNames[depName];
        std::string sql = "select distinct " +  columnName + sqlFromClause();
        std::vector<std::string> args;
        sql += sqlWhereClause(deps, args /*out*/);
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, args);

        std::set<std::string, DependencyValueSorter> humanValues =
            std::set<std::string, DependencyValueSorter>(DependencyValueSorter(depName));
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
            humanValues.insert(humanDepValue(depName, row.get<std::string>(0), HUMAN_TERSE));
        
        BOOST_FOREACH (const std::string &humanValue, humanValues) {
            index.insert(humanValue, values.size());
            values.push_back(humanValue);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Text widget to display comma-separated values (CSV) for a model.
class WCommaSeparatedValues: public Wt::WContainerWidget {
    StatusModel *model_;
    Wt::WText *text_;

public:
    explicit WCommaSeparatedValues(StatusModel *model, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), model_(model) {
        ASSERT_not_null(model);
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        setLayout(vbox);

        text_ = new Wt::WText("Nothing to show.");
        text_->setTextFormat(Wt::PlainText);
        text_->setWordWrap(false);
        vbox->addWidget(text_);

        model_->modelReset().connect(this, &WCommaSeparatedValues::updateText);
    }

private:
    void updateText() {
        std::string s;
        for (int i=0; i<model_->columnCount(); ++i) {
            s += i ? ", " : "";
            boost::any header = model_->headerData(i);
            s += "\"" + StringUtility::cEscape(boost::any_cast<std::string>(header)) + "\"";
        }
        s += "\n";

        for (int i=0; i<model_->rowCount(); ++i) {
            for (int j=0; j<model_->columnCount(); ++j) {
                s += j ? ", " : "";
                boost::any value = model_->data(model_->index(i, j));
                if (value.type() == typeid(std::string)) {
                    s += "\"" + StringUtility::cEscape(boost::any_cast<std::string>(value)) + "\"";
                } else if (value.type() == typeid(double)) {
                    s += boost::lexical_cast<std::string>(boost::any_cast<double>(value));
                } else {
                    s += "unknown";
                }
            }
            s += "\n";
        }
        text_->setText(s);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Chart showing whatever results that are presently stored in the StatusModel (which must be in 2d mode).  The X axis is the
// values of some dependency (e.g., boost version numbers) and the Y axis are the number of configurations that failed in some
// specified test. Bar charts are rotated 90 degrees so the X axis is vertical and Y is horizontal, and the bars are stacked
// end-to-end.
class WStatusChart2d: public Wt::Chart::WCartesianChart {
    StatusModel *model_;
    ChartType chartType_;

public:
    WStatusChart2d(StatusModel *model, ChartType chartType, Wt::WContainerWidget *parent = NULL)
        : Wt::Chart::WCartesianChart(parent), model_(model), chartType_(chartType) {
        ASSERT_not_null(model);
        ASSERT_require(model->depMajorIsData());
        ASSERT_require(!model->depMinorIsData());

        // Make room around the graph for titles, labels, and legend.
        if (BAR_CHART == chartType_) {
            setPlotAreaPadding(200, Wt::Left);
            setPlotAreaPadding(200, Wt::Right);
            setPlotAreaPadding(20, Wt::Top);
            setPlotAreaPadding(0, Wt::Bottom);
            setOrientation(Wt::Horizontal);
        } else {
            setPlotAreaPadding(40, Wt::Left);
            setPlotAreaPadding(200, Wt::Right);
            setPlotAreaPadding(0, Wt::Top);
            setPlotAreaPadding(60, Wt::Bottom);
            setOrientation(Wt::Vertical);
            axis(Wt::Chart::XAxis).setLabelAngle(22.5);
        }
        setLegendEnabled(true);
        setModel(model);
        setXSeriesColumn(0);
        axis(Wt::Chart::YAxis).setMinimum(0);
    }

    void modelReset() ROSE_OVERRIDE {
        Wt::Chart::WCartesianChart::modelReset();

        std::vector<Wt::Chart::WDataSeries> series;
        for (int j=1; j<model_->columnCount(); ++j) {
            if (BAR_CHART == chartType_) {
                series.push_back(Wt::Chart::WDataSeries(j, Wt::Chart::BarSeries));
                series.back().setStacked(true);
            } else {
                series.push_back(Wt::Chart::WDataSeries(j, Wt::Chart::LineSeries));
                series.back().setMarker(Wt::Chart::SquareMarker);
            }
            Wt::WColor color = dependencyValueColor(model_->depMinorName(), model_->depMinorValue(j), j);
            series.back().setBrush(Wt::WBrush(color));
            series.back().setPen(Wt::WPen(color));
        }
        setSeries(series);

        if (BAR_CHART == chartType_) {
            int height = std::max(40 + 25 * std::min(model()->rowCount(), 15), 130);
            setHeight(height);
        } else {
            setHeight(230);
        }

        if (LINE_CHART == chartType_) {
            axis(Wt::Chart::YAxis).setAutoLimits(Wt::Chart::MaximumValue);
        } else {
            switch (model_->chartValueType()) {
                case CVT_COUNT:
                case CVT_PASS_RATIO:
                case CVT_WARNINGS_AVE:
                case CVT_DURATION_AVE:
                    axis(Wt::Chart::YAxis).setAutoLimits(Wt::Chart::MaximumValue);
                    break;
                case CVT_PERCENT:
                    axis(Wt::Chart::YAxis).setMaximum(100);
                    break;
            }
        }
    }

    Wt::WColor dependencyValueColor(const std::string &depName, const std::string &depHumanValue, size_t i) {
        if (depName == "pass/fail") {
            if (depHumanValue == "pass")
                return Wt::WColor(52, 147, 19);         // green
            return Wt::WColor(156, 21, 21);             // red
        } else if (depName == "status" && depHumanValue == "end") {
            // Use a fairly bright green to contrast with the other colors.
            return Wt::WColor(52, 147, 19);
        } else {
            switch (i % 8) {
                case 0: return Wt::WColor(137, 52, 174); // purple
                case 1: return Wt::WColor(173, 75, 51);  // tomato
                case 2: return Wt::WColor(51, 149, 173); // cyan
                case 3: return Wt::WColor(174, 52, 144); // dark pink
                case 4: return Wt::WColor(173, 142, 51); // ochre
                case 5: return Wt::WColor(51, 82, 173);  // blue
                case 6: return Wt::WColor(174, 52, 99);  // rose
                case 7: return Wt::WColor(65, 51, 173);  // purple
            }
        }
        ASSERT_not_reachable("stupid compiler");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The combo boxes, etc. for constraining which tests appear in the result model.
class WConstraints: public Wt::WContainerWidget {
    Dependencies dependencies_;
    Wt::Signal<> constraintsChanged_;

public:
    explicit WConstraints(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent) {

        // Build info about each dependency
        std::vector<std::string> depNames;
        BOOST_FOREACH (const std::string &depName, gstate.dependencyNames.keys()) {
            Dependency &dep = dependencies_.insertMaybe(depName, Dependency(depName));
            depNames.push_back(depName);

            // Find all values that the dependency can have. Depending on the dependency, we might want to use human-readable
            // values (like yyyy-mm-dd instead of a unix time stamp), in which case the "select distinct" and "order by" SQL
            // clauses won't really do what we want. Regardless of whether we use human-readalbe names and buckets of values,
            // we need to store the original value from the SQL table so we can construct "where" clauses later.
            std::string depColumn = gstate.dependencyNames[depName];
            SqlDatabase::StatementPtr q = gstate.tx->statement("select distinct " + depColumn + sqlFromClause());
            for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
                std::string rawValue = row.get<std::string>(0);
                std::string humanValue = humanDepValue(depName, rawValue, HUMAN_TERSE);
                dep.humanValues.insertMaybeDefault(humanValue) <<rawValue;
            }

            // Combo box so we can pick a human value (i.e., bucket of database values) by which to limit queries later.  Add
            // entries to the combo box, but make sure they're sorted. The default sort for the dep.humanValues.keys() is
            // alphabetical, but that's not always what we want. For instance, "status" should be sorted in the order that the
            // individual tests run, not their names.
            dep.comboBox = new Wt::WComboBox;
            dep.comboBox->addItem(WILD_CARD_STR);
            std::vector<std::string> comboValues(dep.humanValues.keys().begin(), dep.humanValues.keys().end());
            std::sort(comboValues.begin(), comboValues.end(), DependencyValueSorter(depName));
            BOOST_FOREACH (const std::string &comboValue, comboValues)
                dep.comboBox->addItem(comboValue);
            dep.comboBox->activated().connect(this, &WConstraints::emitConstraintsChanged);
        }

        static const size_t nDepCols = 2;               // number of columns for dependencies
        size_t nDepRows = (depNames.size() + nDepCols - 1) / nDepCols;
        Wt::WGridLayout *grid = new Wt::WGridLayout;
        setLayout(grid);

        // Fill the grid in row-major order
        for (size_t i=0; i<depNames.size(); ++i) {
            int row = i % nDepRows;
            int col = i / nDepRows;
            std::string depName = depNames[i];
            grid->addWidget(new Wt::WLabel(depName), row, 2*col+0, Wt::AlignRight | Wt::AlignMiddle);
            grid->addWidget(dependencies_[depName].comboBox, row, 2*col+1);
        }
    }

    const Dependencies& dependencies() const {
        return dependencies_;
    }

    // Emitted whenever a constraint combo box value is changed.
    Wt::Signal<>& constraintsChanged() {
        return constraintsChanged_;
    }

    void resetConstraints() {
        ::mlog[DEBUG] <<"WConstraints::resetConstraints\n";
        BOOST_FOREACH (Dependency &dep, dependencies_.values())
            dep.comboBox->setCurrentIndex(0);
        emitConstraintsChanged();
    }

private:
    void emitConstraintsChanged() {
        ::mlog[DEBUG] <<"WConstraints::emitConstraintsChanged\n";
        constraintsChanged_.emit();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Content of the "Overview" tab showing the chart of results and the constraint input area.
class WResultsConstraintsTab: public Wt::WContainerWidget {
    WConstraints *constraints_;
    StatusModel *chartModel_, *tableModel_;
    WStatusChart2d *statusCharts_[2];                   // BAR_CHART, LINE_CHART
    Wt::WTableView *tableView_;
    WCommaSeparatedValues *csvView_;
    Wt::WStackedWidget *chartStack_;
    Wt::WComboBox *majorAxisChoices_, *minorAxisChoices_;
    Wt::WComboBox *chartChoice_;
    Wt::WComboBox *absoluteRelative_;                   // whether to show percents or counts

public:
    explicit WResultsConstraintsTab(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent) {
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        setLayout(vbox);

        //-----------------
        // Results area
        //-----------------

        Wt::WPanel *results = new Wt::WPanel;
        results->setTitle("Test results");
        Wt::WContainerWidget *resultsWidget = new Wt::WContainerWidget;
        Wt::WVBoxLayout *resultsBox = new Wt::WVBoxLayout;
        resultsWidget->setLayout(resultsBox);
        results->setCentralWidget(resultsWidget);
        vbox->addWidget(results);

        // The resultsBox has two rows: the top row is the charts (in a WStackedWidget), and the bottom is the settings to
        // choose which chart to display and how.
        chartModel_ = new StatusModel;
        chartModel_->setDepMajorIsData(true);
        resultsBox->addWidget(chartStack_ = new Wt::WStackedWidget);
        chartStack_->addWidget(statusCharts_[0] = new WStatusChart2d(chartModel_, BAR_CHART));
        chartStack_->addWidget(statusCharts_[1] = new WStatusChart2d(chartModel_, LINE_CHART));

        // Data can be viewed as a table instead of a chart.
        tableModel_ = new StatusModel;
        tableModel_->setDepMajorIsData(true);
        tableModel_->setRoundToInteger(true);
        tableModel_->setHumanReadable(true);
        tableView_ = new Wt::WTableView;
        tableView_->setModel(tableModel_);
        tableView_->setAlternatingRowColors(false);         // true interferes with our custom background colors
        tableView_->setSortingEnabled(false);
        tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
        chartStack_->addWidget(tableView_);

        // Data can be viewed as a list of comma-separated values.
        csvView_ = new WCommaSeparatedValues(tableModel_);
        chartStack_->addWidget(csvView_);

        // The chartSettingsBox holds the various buttons and such for adjusting the charts.
        Wt::WHBoxLayout *chartSettingsBox = new Wt::WHBoxLayout;
        chartSettingsBox->addSpacing(300);

        // Combo box to choose what to display as the X axis for the test status chart
        majorAxisChoices_ = new Wt::WComboBox;
        minorAxisChoices_ = new Wt::WComboBox;
        int i = 0;
        BOOST_FOREACH (const std::string &depName, gstate.dependencyNames.keys()) {
            majorAxisChoices_->addItem(depName);
            minorAxisChoices_->addItem(depName);
            if (depName == chartModel_->depMajorName())
                majorAxisChoices_->setCurrentIndex(i);
            if (depName == chartModel_->depMinorName())
                minorAxisChoices_->setCurrentIndex(i);
            ++i;
        }
        chartSettingsBox->addWidget(majorAxisChoices_);
        chartSettingsBox->addWidget(new Wt::WLabel("versus"));
        chartSettingsBox->addWidget(minorAxisChoices_);

        // Combo box to choose which chart to show.
        chartSettingsBox->addWidget(new Wt::WLabel("Chart type:"));
        chartSettingsBox->addWidget(chartChoice_ = new Wt::WComboBox);
        chartChoice_->addItem("bars");
        chartChoice_->addItem("lines");
        chartChoice_->addItem("table");
        chartChoice_->addItem("csv");
        chartChoice_->activated().connect(this, &WResultsConstraintsTab::switchCharts);

        // Combo box to choose whether the model stores percents or counts
        chartSettingsBox->addWidget(absoluteRelative_ = new Wt::WComboBox);
        absoluteRelative_->addItem("runs (#)");
        absoluteRelative_->addItem("runs (%)");
        absoluteRelative_->addItem("pass / runs (%)");
        absoluteRelative_->addItem("ave warnings (#)");
        absoluteRelative_->addItem("ave duration (sec)");
        absoluteRelative_->activated().connect(this, &WResultsConstraintsTab::switchAbsoluteRelative);

        // Update button to reload data from the database
        Wt::WPushButton *updateButton = new Wt::WPushButton("Update");
        updateButton->setToolTip("Update chart with latest database changes.");
        updateButton->clicked().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        chartSettingsBox->addWidget(updateButton);

        chartSettingsBox->addStretch(1);
        resultsBox->addLayout(chartSettingsBox);

        //------------------
        // Constraints area
        //------------------

        Wt::WPanel *constraints = new Wt::WPanel;
        constraints->setTitle("Constraints");
        Wt::WContainerWidget *constraintsWidget = new Wt::WContainerWidget;
        Wt::WVBoxLayout *constraintsBox = new Wt::WVBoxLayout;
        constraintsWidget->setLayout(constraintsBox);
        constraints->setCentralWidget(constraintsWidget);
        vbox->addWidget(constraints);

        // Constraints
        constraintsBox->addWidget(constraints_ = new WConstraints);
        constraints_->constraintsChanged().connect(this, &WResultsConstraintsTab::updateStatusCounts);

        // Button to reset everything to the initial state.
        Wt::WHBoxLayout *constraintButtonBox = new Wt::WHBoxLayout;
        constraintsBox->addLayout(constraintButtonBox);
        Wt::WPushButton *reset = new Wt::WPushButton("Clear");
        reset->clicked().connect(this, &WResultsConstraintsTab::resetConstraints);
        constraintButtonBox->addWidget(reset);
        constraintButtonBox->addStretch(1);

        //---------
        // Wiring
        //---------
        vbox->addStretch(1);
        majorAxisChoices_->activated().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        minorAxisChoices_->activated().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        updateStatusCounts();
    }

    WConstraints* constraints() const {
        return constraints_;
    }

    void updateStatusCounts() {
        ::mlog[DEBUG] <<"WApplication::updateStatusCounts\n";
        chartModel_->setDepMajorName(majorAxisChoices_->currentText().narrow());
        chartModel_->setDepMinorName(minorAxisChoices_->currentText().narrow());
        chartModel_->updateModel(constraints_->dependencies());

        tableModel_->setDepMajorName(majorAxisChoices_->currentText().narrow());
        tableModel_->setDepMinorName(minorAxisChoices_->currentText().narrow());
        tableModel_->updateModel(constraints_->dependencies());
    }

private:
    void resetConstraints() {
        ::mlog[DEBUG] <<"WApplication::resetConstraints\n";
        constraints_->resetConstraints();
    }

    void switchCharts() {
        chartStack_->setCurrentIndex(chartChoice_->currentIndex());
    }

    void switchAbsoluteRelative() {
        ChartValueType cvt = CVT_COUNT;
        switch (absoluteRelative_->currentIndex()) {
            case 0: cvt = CVT_COUNT; break;
            case 1: cvt = CVT_PERCENT; break;
            case 2: cvt = CVT_PASS_RATIO; break;
            case 3: cvt = CVT_WARNINGS_AVE; break;
            case 4: cvt = CVT_DURATION_AVE; break;
            default: ASSERT_not_reachable("invalid chart value type");
        }

        chartModel_->setChartValueType(cvt);
        chartModel_->updateModel(constraints_->dependencies());

        tableModel_->setChartValueType(cvt);
        tableModel_->updateModel(constraints_->dependencies());
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The content of the "Details" tab.
class WDetails: public Wt::WContainerWidget {
    std::vector<int> testIds_;
    Wt::WComboBox *testIdChoices_;
    int testId_;
    Wt::Signal<> testIdChanged_;
    Wt::WText *error_, *config_, *commands_, *testOutput_;

public:
    explicit WDetails(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), testId_(-1) {
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        setLayout(vbox);

        vbox->addWidget(new Wt::WLabel("Details about the configurations selected in the \"Overview\" tab."));

        // Combo box to choose which test to display
        Wt::WHBoxLayout *choiceBox = new Wt::WHBoxLayout;
        choiceBox->addWidget(new Wt::WLabel("Configuration"));
        testIdChoices_ = new Wt::WComboBox;
        testIdChoices_->activated().connect(this, &WDetails::selectTestId);
        choiceBox->addWidget(testIdChoices_);
        choiceBox->addStretch(1);
        vbox->addLayout(choiceBox);

        // Error message cached in database test_results.first_error
        vbox->addWidget(error_ = new Wt::WText);
        error_->setStyleClass("compiler-error");

        // Configuration
        vbox->addWidget(new Wt::WText("<h2>Detailed status</h2>"));
        config_ = new Wt::WText;
        config_->setTextFormat(Wt::PlainText);
        config_->setWordWrap(false);
        vbox->addWidget(config_);

        // Commands that were executed
        vbox->addWidget(new Wt::WText("<h2>Commands executed</h2>"));
        commands_ = new Wt::WText;
        commands_->setTextFormat(Wt::XHTMLText);
        commands_->setWordWrap(true);
        vbox->addWidget(commands_);

        // Tests final output
        vbox->addWidget(new Wt::WText("<h2>Command output</h2>"));
        testOutput_ = new Wt::WText;
        testOutput_->setTextFormat(Wt::XHTMLText);
        testOutput_->setWordWrap(false);
        vbox->addWidget(testOutput_, 1);
    }

    void queryTestIds(const Dependencies &deps) {
        std::vector<std::string> args;
        std::string sql = "select id, status" + sqlFromClause() + sqlWhereClause(deps, args) + " order by id";
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, args);

        testIds_.clear();
        testIdChoices_->clear();
        testIdChoices_->setWidth(200);
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            int testId = row.get<int>(0);
            std::string status = row.get<std::string>(1);
            testIds_.push_back(testId);
            testIdChoices_->addItem(boost::lexical_cast<std::string>(testId) + ": " + status);
        }
        selectTestId();
    }

    int testId() const {
        return testId_;
    }

    void setTestId(int id) {
        if (testId_ != id) {
            testId_ = id;
            testIdChanged_.emit();
        }
    }
    
    Wt::Signal<>& testIdChanged() {
        return testIdChanged_;
    }

    void updateDetails(const Dependencies &deps) {
        ::mlog[DEBUG] <<"WDetails::updateDetails(testId=" <<testId_ <<")\n";

        // What columns to query?
        DependencyNames columns = gstate.dependencyNames;
        columns.insert("status", "status");
        columns.insert("duration", "duration");
        columns.insert("noutput", "noutput");
        columns.insert("nwarnings", "nwarnings");

        std::string sql;
        BOOST_FOREACH (const std::string &colName, columns.values())
            sql += std::string(sql.empty()?"select ":", ") + colName;
        sql += ", coalesce(first_error,'')";

        sql += sqlFromClause();
        std::vector<std::string> args;
        std::string where = sqlWhereClause(deps, args) + " and id = ?";
        args.push_back(boost::lexical_cast<std::string>(testId_));
        sql += where;

        config_->setText("");
        std::string config, first_error;
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, args);
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            int column = 0;
            BOOST_FOREACH (const std::string &name, columns.keys())
                config += name + "=" + humanDepValue(name, row.get<std::string>(column++)) + "\n";
            first_error = row.get<std::string>(columns.size());
            break;
        }
        config_->setText(config);
        error_->setText(first_error);

        updateCommands();
        updateOutput();
    }

private:
    void selectTestId() {
        int i = testIdChoices_->currentIndex();
        if (i >= 0 && (size_t)i < testIds_.size()) {
            setTestId(testIds_[i]);
        } else {
            setTestId(-1);
        }
    }

    void updateCommands() {
        commands_->setText("");
        if (testId_ >= 0) {
            SqlDatabase::StatementPtr q = gstate.tx->statement("select content from attachments where test_id = ? and name = ?");
            q->bind(0, testId_);
            q->bind(1, "Commands");
            SqlDatabase::Statement::iterator row = q->begin();
            std::string content = row != q->end() ? row.get<std::string>(0) : std::string();
            if (content.empty()) {
                content = "Commands were not saved for this test.\n";
            } else {
                content = escapeHtml(content);
                boost::replace_all(content, "\n", "<br/><br/>");
            }
            commands_->setText(content);
        }
    }

    std::string escapeHtml(const std::string &s) {
        std::string t;
        int col = 0;
        BOOST_FOREACH (char ch, s) {
            ++col;
            switch (ch) {
                case '<': t += "&lt;"; break;
                case '>': t += "&gt;"; break;
                case '&': t += "&amp;"; break;
                case '\r': col = 0; break;
                case '\n': t += ch; col = 0; break; // leave linefeeds alone for now for easier matching below
                case '\t': {
                    int n = 8 - (col-1) % 8;
                    t += std::string(n, ' ');
                    col += n;
                    break;
                }
                default: {
                    if (iscntrl(ch)) {
                        --col;
                    } else {
                        t += ch;
                    }
                    break;
                }
            }
        }
        return t;
    }

    void updateOutput() {
        testOutput_->setText("");
        if (testId_ >= 0) {
            SqlDatabase::StatementPtr q = gstate.tx->statement("select content from attachments where test_id = ? and name = ?");
            q->bind(0, testId_);
            q->bind(1, "Final output");
            SqlDatabase::Statement::iterator row = q->begin();
            std::string s = row != q->end() ? row.get<std::string>(0) : std::string();
            if (s.empty())
                s = "Command output was not saved for this test.\n";
            std::string t = escapeHtml(s);

            // Look for special compiler output lines for errors and warnings
            boost::regex compilerRegex("(^[^\\n]*?(?:"
                                       // Errors
                                       "\\berror:"
                                       "|\\[ERROR\\]"
                                       "|\\bwhat\\(\\): [^\\n]+\\n[^\\n]*Aborted$" // fatal exception in shell command
                                       "|\\bwhat\\(\\): [^\\n]+\\n[^\\n]*command died" // fatal exception from $(RTH_RUN)
                                       "|\\[err\\]: terminated after \\d+ seconds"

                                       ")[^\\n]*$)|"
                                       "(^[^\\n]*?(?:"

                                       // Warnings
                                       "\\bwarning:"

                                       ")[^\\n]*$)|"
                                       "(^={17}-={17}[^\\n]+={17}-={17}$)");

            const char *compilerFormat = "(?1<span class=\"compiler-error\">$&</span>)"
                                         "(?2<span class=\"compiler-warning\">$&</span>)"
                                         "(?3<span class=\"output-separator\"><hr/>$&</span>)";

            std::ostringstream out(std::ios::out | std::ios::binary);
            std::ostream_iterator<char, char> oi(out);
            boost::regex_replace(oi, t.begin(), t.end(), compilerRegex, compilerFormat,
                                 boost::match_default|boost::format_all);
            t = out.str();

            // Now fix the linefeeds
            boost::replace_all(t, "\n", "<br/>");
            testOutput_->setText(t);
        }
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For prioritizing errors to be fixed
class WErrors: public Wt::WContainerWidget {
    bool outOfDate_;                                    // need to query database again?
    Wt::WTable *grid_;
public:
    explicit WErrors(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), outOfDate_(true) {
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        setLayout(vbox);
        vbox->addWidget(new Wt::WText("These are the most prevalent errors in the failing configurations selected in the "
                                      "\"Overview\" tab.  The definition of \"failing\" can be found in the \"Settings\" "
                                      "tab. The information below each error is the list of constraints, in addition to "
                                      "those in the \"Overview\" tab, which all the errors satisfy."));
        vbox->addWidget(grid_ = new Wt::WTable);
        grid_->setHeaderCount(1);

        vbox->addStretch(1);
    }

    void changeConstraints() {
        outOfDate_ = true;
    }

    // Update the error list if it's outdated
    void updateErrorList(const Dependencies &deps) {
        if (!outOfDate_)
            return;
        outOfDate_ = false;
        std::vector<std::string> args;
        std::string passFailExpr = gstate.dependencyNames["pass/fail"];
        std::string sql = "select count(*) as n, status, first_error, " + passFailExpr +
                          sqlFromClause() +
                          sqlWhereClause(deps, args) +
                          " and first_error is not null"
                          " and " + passFailExpr + " = 'fail'"
                          " group by status, first_error, test_names.position"
                          " order by n desc"
                          " limit 15";
        SqlDatabase::StatementPtr q1 = gstate.tx->statement(sql);
        bindSqlVariables(q1, args);

        grid_->clear();
        grid_->elementAt(0, 0)->addWidget(new Wt::WText("Count"));
        grid_->elementAt(0, 1)->addWidget(new Wt::WText("Status"));
        grid_->elementAt(0, 2)->addWidget(new Wt::WText("Error"));

        grid_->columnAt(0)->setWidth(Wt::WLength(4.0, Wt::WLength::FontEm));
        grid_->columnAt(1)->setWidth(Wt::WLength(6.0, Wt::WLength::FontEm));

        Sawyer::Container::Map<std::string, std::string> statusCssClass;
        int i = 1;
        for (SqlDatabase::Statement::iterator iter1 = q1->begin(); iter1 != q1->end(); ++iter1) {
            int nErrors = iter1.get<int>(0);
            std::string status = iter1.get<std::string>(1);
            std::string message = iter1.get<std::string>(2);
            grid_->elementAt(i, 0)->addWidget(new Wt::WText(StringUtility::numberToString(nErrors)));
            grid_->elementAt(i, 1)->addWidget(new Wt::WText(status));
            grid_->elementAt(i, 2)->addWidget(new Wt::WText(message, Wt::PlainText));

            grid_->elementAt(i, 0)->setRowSpan(2);
            grid_->elementAt(i, 0)->setStyleClass("error-count-cell");
            grid_->elementAt(i, 1)->setRowSpan(2);
            if (!statusCssClass.exists(status))
                statusCssClass.insert(status, "error-status-"+StringUtility::numberToString(statusCssClass.size()%8));
            grid_->elementAt(i, 1)->setStyleClass(statusCssClass[status]);
            grid_->elementAt(i, 2)->setStyleClass("error-message-cell");
            ++i;

            // Figure out the dependencies that are in common for all tests of this error
            typedef Sawyer::Container::Map<std::string /*depname*/, std::string /*human*/> Dependencies;
            Dependencies dependencies;
            args.clear();
            sql = "select distinct " + boost::join(gstate.dependencyNames.values(), ", ") +
                  sqlFromClause() + sqlWhereClause(deps, args) + " and first_error = ?";
            args.push_back(message);
            SqlDatabase::StatementPtr q2 = gstate.tx->statement(sql);
            bindSqlVariables(q2, args);
            for (SqlDatabase::Statement::iterator iter2 = q2->begin(); iter2 != q2->end(); ++iter2) {
                int colNumber = 0;
                BOOST_FOREACH (const std::string &depname, gstate.dependencyNames.keys()) {
                    std::string depval = humanDepValue(depname, iter2.get<std::string>(colNumber++), HUMAN_TERSE);
                    if (!dependencies.exists(depname)) {
                        dependencies.insert(depname, depval);
                    } else if (dependencies[depname]!=depval) {
                        dependencies[depname] = "";
                    }
                }
            }

            // Show dependencies that have the same value for all error, but for which the user has more than one choice of
            // setting (well, two since the first item is always the wildcard).
            std::vector<std::string> allSameDeps;
            BOOST_FOREACH (const Dependencies::Node &node, dependencies.nodes()) {
                if (node.value().empty())
                    continue;                           // conflicting values found above
                if (node.key() == "pass/fail" || node.key() == "status")
                    continue;                           // not useful information
                if (deps[node.key()].comboBox->count() <= 2)
                    continue;                           // user had only one value choice (plus the wildcard)
                if (deps[node.key()].comboBox->currentText().narrow() == node.value())
                    continue;                           // this dependency is already constrained
                allSameDeps.push_back(node.key() + "=" + node.value());
            }
            if (allSameDeps.empty())
                allSameDeps.push_back("No additional constraints.");
            grid_->elementAt(i, 2)->addWidget(new Wt::WText(boost::join(allSameDeps, ", ")));
            grid_->elementAt(i, 2)->setStyleClass("error-dependencies-cell");
            ++i;
        }
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global settings
class WSettings: public Wt::WContainerWidget {
    Wt::Signal<> settingsChanged_;
    Wt::WComboBox *passCriteria_;

public:
    explicit WSettings(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent) {
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        setLayout(vbox);

        //------------------------------
        // Criteria for passing a test.
        //------------------------------
#if 0 // [Robb Matzke 2016-02-10]
        Wt::WHBoxLayout *passBox = new Wt::WHBoxLayout;
        vbox->addLayout(passBox);
#else
        Wt::WContainerWidget *passBox = new Wt::WContainerWidget;
        vbox->addWidget(passBox);
#endif

        passBox->addWidget(new Wt::WText("A configuration is defined to have passed if it makes it to the "));

        passCriteria_ = new Wt::WComboBox;
        passBox->addWidget(passCriteria_);
        BOOST_FOREACH (const std::string &testName, gstate.testNames) {
            passCriteria_->addItem(testName);
            if (testName == "end")
                passCriteria_->setCurrentIndex(passCriteria_->count()-1);
        }

        passBox->addWidget(new Wt::WText("step, otherwise it is considered to have failed. This rule generates "
                                         "the 'pass' or 'fail' values for the \"pass/fail\" property used throughout "
                                         "this application."));

#if 0 // [Robb Matzke 2016-02-10]
        passBox->addStretch(1);
#endif

        vbox->addStretch(1);

        //----------
        // Wiring
        //----------
        passCriteria_->activated().connect(this, &WSettings::updatePassCriteria);
    }

    Wt::Signal<>& settingsChanged() {
        return settingsChanged_;
    }

private:
    void updatePassCriteria() {
        setPassDefinition(passCriteria_->currentText().narrow());
        settingsChanged_.emit();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// One application object is created per user session.
class WApplication: public Wt::WApplication {
    WResultsConstraintsTab *resultsConstraints_;
    WDetails *details_;
    WErrors *errors_;
    WSettings *settings_;
    Wt::WTabWidget *tabs_;

public:
    explicit WApplication(const Wt::WEnvironment &env)
        : Wt::WApplication(env) {
        setTitle("ROSE testing matrix");
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        root()->setLayout(vbox);

        styleSheet().addRule(".compiler-error",   "color:#680000; background-color:#ffc0c0;"); // reds
        styleSheet().addRule(".compiler-warning", "color:#8f4000; background-color:#ffe0c7;"); // oranges
        styleSheet().addRule(".output-separator", "background-color:#808080;");

        // Colors for pass-ratios.
        //   Classes redgreen-N vary from red to green as N goes from integer 0 through 100.
        //   Classes redgreen-N-S are similar except S is a saturation amount from 0 through 4 (desaturated).
        rose::Color::Gradient redgreen;
        redgreen.insert(0.0, rose::Color::HSV(0.00, 0.50, 0.50));
        redgreen.insert(0.5, rose::Color::HSV(0.17, 0.40, 0.50));
        redgreen.insert(1.0, rose::Color::HSV(0.33, 0.50, 0.50));
        for (int i=0; i<=100; ++i) {
            rose::Color::RGB c = redgreen.interpolate(i/100.0);
            std::string cssClass = ".redgreen-" + StringUtility::numberToString(i);
            std::string bgColor = "background-color:" + c.toHtml() + ";";
            styleSheet().addRule(cssClass, bgColor);
            for (int j=0; j<5; ++j) {
                std::string cssClass2 = cssClass + "-" + StringUtility::numberToString(j);
                bgColor = "background-color:" + rose::Color::fade(c, 1.0-j*0.25).toHtml() + ";";
                styleSheet().addRule(cssClass2, bgColor);
            }
        }
        styleSheet().addRule(".chart-zero", "background-color:" + rose::Color::HSV(0, 0, 0.3).toHtml() + ";");

        // Styles of error priority table cells
        styleSheet().addRule(".error-count-cell", "border:1px solid black;");
        styleSheet().addRule(".error-dependencies-cell", "border:1px solid black;");
        styleSheet().addRule(".error-message-cell", "border:1px solid black; color:#680000; background-color:#ffc0c0;");
        styleSheet().addRule(".error-status-0", "border:1px solid black; background-color:#d0aae0;");// light purple
        styleSheet().addRule(".error-status-1", "border:1px solid black; background-color:#e1c2ba;");// light tomato
        styleSheet().addRule(".error-status-2", "border:1px solid black; background-color:#aed5df;");// light cyan
        styleSheet().addRule(".error-status-3", "border:1px solid black; background-color:#dfb9cd;");// light pink
        styleSheet().addRule(".error-status-4", "border:1px solid black; background-color:#dfd5b8;");// light ochre
        styleSheet().addRule(".error-status-5", "border:1px solid black; background-color:#bbc4df;");// light blue
        styleSheet().addRule(".error-status-6", "border:1px solid black; background-color:#edc7d5;");// light rose
        styleSheet().addRule(".error-status-7", "border:1px solid black; background-color:#bebadf;");// light purple

        tabs_ = new Wt::WTabWidget();
        tabs_->addTab(resultsConstraints_ = new WResultsConstraintsTab, "Overview");
        tabs_->addTab(details_ = new WDetails, "Details");
        tabs_->addTab(errors_ = new WErrors, "Errors");
        tabs_->addTab(settings_ = new WSettings, "Settings");
        vbox->addWidget(tabs_);

        // Wiring
        resultsConstraints_->constraints()->constraintsChanged().connect(this, &WApplication::getMatchingTests);
        details_->testIdChanged().connect(this, &WApplication::updateDetails);
        settings_->settingsChanged().connect(this, &WApplication::updateAll);
        tabs_->currentChanged().connect(this, &WApplication::switchTabs);
        getMatchingTests();
    }

private:
    void switchTabs(int idx) {
        if (tabs_->widget(idx) == errors_)
            errors_->updateErrorList(resultsConstraints_->constraints()->dependencies());
    }
    
    void getMatchingTests() {
        ::mlog[DEBUG] <<"WApplication::getMatchingTests\n";
        details_->queryTestIds(resultsConstraints_->constraints()->dependencies());
        errors_->changeConstraints();
    }

    void updateDetails() {
        ::mlog[DEBUG] <<"WApplication::updateDetails\n";
        details_->updateDetails(resultsConstraints_->constraints()->dependencies());
    }

    void updateAll() {
        errors_->changeConstraints();
        resultsConstraints_->updateStatusCounts();
        getMatchingTests();
        updateDetails();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    if (const char *dbUrl = getenv("ROSE_MATRIX_DATABASE"))
        gstate.dbUrl = dbUrl;

    // General switches
    parser.with(Switch("help", 'h')
                .doc("Show this documentation.")
                .action(showHelpAndExit(0)));

#ifndef USING_FASTCGI
    parser.with(Switch("log", 'L')
                .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
                .argument("config")
                .whichValue(SAVE_ALL)
                .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));
#endif

    // Switches for HTTP server
#ifndef USING_FASTCGI
    parser.with(Switch("http-address")
                .argument("IP-address", anyParser(gstate.httpAddress))
                .doc("IP address to bind to server listening socket. The default is " + gstate.httpAddress));

    parser.with(Switch("http-port")
                .argument("TCP-port", nonNegativeIntegerParser(gstate.httpPort))
                .doc("TCP port at which the HTTP server will listen. The default is " +
                     boost::lexical_cast<std::string>(gstate.httpPort) + "."));

    parser.with(Switch("docroot")
                .argument("directory", anyParser(gstate.docRoot))
                .doc("Name of root directory for serving HTTP documents.  The default is \"" + gstate.docRoot + "\"."));

    parser.with(Switch("database", 'd')
                .argument("uri", anyParser(gstate.dbUrl))
                .doc("Uniform resource locator for the database." + SqlDatabase::uriDocumentation()));
#endif

    parser.purpose("serves matrix testing results as HTML");

#ifdef USING_FASTCGI
    parser.doc("description",
               "This is a FastCGI program for querying ROSE matrix testing results, and as such its command-line is "
               "processed by libwtfcgi. Users don't normally run this program directly; instead, it's run by a web "
               "server like Apache or Nginx. Here are the instructions for using FastCGI on Apache: "
               "http://redmine.webtoolkit.eu/projects/wt/wiki/Fastcgi_on_apache.  The database URI is provided "
               "by the ROSE_MATRIX_DATABASE environment variable instead of a command line. It defaults to "
               "\"" + StringUtility::cEscape(gstate.dbUrl) + "\"");
#else
    parser.doc("description",
               "This is a standalone HTTP web server that serves up an application showing ROSE matrix testing results. It "
               "uses a default database (\"" + StringUtility::cEscape(gstate.dbUrl) + "\") unless a different one is "
               "provided with the @s{database} switch.  To use this program, run it and specify a port number (@s{http-port}), "
               "then start a web browser and point it at http://@v{hostname}:@v{port} where @v{hostname} is where you "
               "ran this program (perhaps \"localhost\" is sufficient) and @v{port} is the value specified for the "
               "@s{http-port} switch.");
#endif

    std::vector<std::string> positionalArgs = parser.parse(argc, argv).apply().unreachedArgs();

#ifndef USING_FASTCGI
    if (positionalArgs.size() != 0) {
        ::mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
#endif
}

static void
loadTestNames() {
    gstate.testNameIndex.clear();
    gstate.testNames.clear();

    SqlDatabase::StatementPtr q = gstate.tx->statement("select distinct status from test_results");
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
        gstate.testNames.push_back(row.get<std::string>(0));

    q = gstate.tx->statement("select name, position from test_names");
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
        std::string statusName = row.get<std::string>(0);
        int position = row.get<int>(1);
        gstate.testNameIndex.insert(statusName, position);
        if (statusName == "end")
            END_STATUS_POSITION = position;
    }

    std::sort(gstate.testNames.begin(), gstate.testNames.end(), DependencyValueSorter("status"));
}

// These are the dependencies that will show up as constraints that the user can adjust.
static void
loadDependencyNames() {
    gstate.dependencyNames.clear();
    SqlDatabase::StatementPtr q = gstate.tx->statement("select distinct name from dependencies");
    for (SqlDatabase::Statement::iterator row=q->begin(); row!=q->end(); ++row) {
        std::string key = row.get<std::string>(0);
        gstate.dependencyNames.insert(key, "test_results.rmc_"+key);
    }

    // Additional key/column relationships
    gstate.dependencyNames.insert("reporting_user", "users.name");
    gstate.dependencyNames.insert("reporting_time", "reporting_time");
    gstate.dependencyNames.insert("tester", "tester");
    gstate.dependencyNames.insert("os", "os");
    gstate.dependencyNames.insert("rose", "rose");
    gstate.dependencyNames.insert("rose_date", "rose_date");
    gstate.dependencyNames.insert("status", "status");
}

static WApplication*
createApplication(const Wt::WEnvironment &env) {
    return new WApplication(env);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    Sawyer::initializeLibrary();
    Sawyer::Message::mfacilities.control("none,>=info");
    ::mlog = Sawyer::Message::Facility("tool");
    Sawyer::Message::mfacilities.insertAndAdjust(::mlog);

#ifdef ROSE_USE_WT
    // Initialized global state shared by all serving threads.
    parseCommandLine(argc, argv);
    gstate.tx = SqlDatabase::Connection::create(gstate.dbUrl)->transaction();
    loadTestNames();
    loadDependencyNames();
    setPassDefinition("end");                           // a configuration passes if its status is >= "end"

    // Start the web server
#ifdef USING_FASTCGI
    return Wt::WRun(argc, argv, createApplication);
#else
    int wtArgc = 0;
    char *wtArgv[8];
    wtArgv[wtArgc++] = strdup(argv[0]);
    wtArgv[wtArgc++] = strdup("--docroot");
    wtArgv[wtArgc++] = strdup(gstate.docRoot.c_str());
    wtArgv[wtArgc++] = strdup("--http-address");
    wtArgv[wtArgc++] = strdup(gstate.httpAddress.c_str());
    wtArgv[wtArgc++] = strdup("--http-port");
    wtArgv[wtArgc++] = strdup(boost::lexical_cast<std::string>(gstate.httpPort).c_str());
    wtArgv[wtArgc] = NULL;
    return Wt::WRun(wtArgc, wtArgv, createApplication);
#endif
#else
    ::mlog[FATAL] <<"this program must be configured with Wt web toolkit support\n";
    exit(1);
#endif
}
