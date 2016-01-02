#include <rosePublicConfig.h>

#include <Sawyer/Message.h>

using namespace Sawyer::Message::Common;
static Sawyer::Message::Facility mlog;

#ifdef ROSE_USE_WT

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
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
#include <Wt/WTabWidget>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

static const char* WILD_CARD_STR = "<any>";

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

// Stuff initialized before the first server thread is created, and which remains constant once the serving starts.
struct GlobalState {
    std::string docRoot;
    std::string httpAddress;
    unsigned short httpPort;
    std::string dbUrl;
    DependencyNames dependencyNames;
    std::vector<std::string> testNames;
    SqlDatabase::TransactionPtr tx;

    GlobalState()
        : docRoot("."), httpAddress("0.0.0.0"), httpPort(80), dbUrl(DEFAULT_DATABASE) {}
};
static GlobalState gstate;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    return where;
}

static void
bindSqlVariables(const SqlDatabase::StatementPtr &q, const std::vector<std::string> &args) {
    for (size_t i=0; i<args.size(); ++i)
        q->bind(i, args[i]);
}

enum HumanFormat { HUMAN_TERSE, HUMAN_VERBOSE };

std::string
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

std::string
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

std::string
humanDepValue(const std::string &depName, const std::string &depValue, HumanFormat fmt = HUMAN_VERBOSE) {
    if (depName == "reporting_time")
        return humanLocalTime(boost::lexical_cast<long>(depValue), fmt);
    if (depName == "rose_date")
        return humanLocalTime(boost::lexical_cast<long>(depValue), fmt);
    if (depName == "duration")
        return humanDuration(boost::lexical_cast<long>(depValue), fmt);
    return depValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Column 0: dependency value (e.g., boost version number)
// Column 1 - N: counts for the various status values
class StatusModel: public Wt::WAbstractTableModel {
    typedef Sawyer::Container::Map<std::string, int> StringIndex;

    std::string dependencyName_;                        // type of dependency to store in column-0
    std::vector<std::string> columnTitles_;             // title for each model column
    std::vector<std::string> column0_;                  // value per table row for configurable dependency
    StringIndex column0Index_;                          // map column-0 value to a table row number
    std::vector<std::vector<int> > table_;              // model columns 1 and greater (zero origin)
    StringIndex statusColumnIndex_;                     // table_ column per status

public:
    explicit StatusModel(Wt::WObject *parent = 0)
        : Wt::WAbstractTableModel(parent) {
        columnTitles_.resize(1);
        columnTitles_[0] = dependencyName_;
        BOOST_FOREACH (const std::string &testName, gstate.testNames) {
            statusColumnIndex_.insert(testName, statusColumnIndex_.size());
            columnTitles_.push_back(testName);
        }
        ASSERT_require(columnTitles_.size() == statusColumnIndex_.size() + 1);
    }

    std::string dependencyName() const {
        return dependencyName_;
    }
        
    void setDependencyName(const std::string &dependencyName) {
        if (dependencyName_ == dependencyName)
            return;
        dependencyName_ = dependencyName;
        columnTitles_[0] = dependencyName;
    }

    void updateModel(const Dependencies &deps) {
        Sawyer::Message::Stream debug(::mlog[DEBUG] <<"StatusModel::updateModel...\n");
        column0_.clear();
        column0Index_.clear();
        table_.clear();
        if (!dependencyName_.empty()) {
            // Build the empty table: fill column-0 with dependency values, build the column zero index, and fill the rest of
            // the model (table_) with zeros. Be careful to convert the dependency values to their human-readable format when
            // sorting and removing dups.
            std::string depColumnName = gstate.dependencyNames[dependencyName_];
            SqlDatabase::StatementPtr q = gstate.tx->statement("select distinct " + depColumnName + " from test_results"
                                                               " join users on test_results.reporting_user = users.uid"
                                                               " order by " + depColumnName);
            for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
                column0Index_.insert(humanDepValue(dependencyName_, row.get<std::string>(0), HUMAN_TERSE), 0);
            BOOST_FOREACH (StringIndex::Node &node, column0Index_.nodes()) {
                node.value() = column0_.size();
                column0_.push_back(node.key());
                table_.push_back(std::vector<int>(statusColumnIndex_.size(), 0));
            }
            ASSERT_require(column0_.size() == column0Index_.size());
            ASSERT_require(column0_.size() == table_.size());

            // Build the database query
            std::string sql = "select " +  depColumnName + ", status, count(*) from test_results"
                              " join users on test_results.reporting_user = users.uid";
            std::vector<std::string> args;
            sql += sqlWhereClause(deps, args /*out*/) +
                   " group by " + depColumnName + ", status"
                   " order by " + depColumnName + ", status";
            q = gstate.tx->statement(sql);
            bindSqlVariables(q, args);

            // Fill in the model's table with data from the database
            for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
                std::string depValue = humanDepValue(dependencyName_, row.get<std::string>(0), HUMAN_TERSE);
                std::string status = row.get<std::string>(1);
                int count = row.get<int>(2);

                int i = column0Index_.getOrElse(depValue, -1);
                int j = statusColumnIndex_.getOrElse(status, -1);
                if (i >= 0 && j >= 0 && (size_t)j < table_[i].size())
                    table_[i][j] += count;
            }
        }
        modelReset().emit();
    }

    int rowCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : column0_.size();
    }
    
    int columnCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : columnTitles_.size();
    }

    boost::any data(const Wt::WModelIndex &index, int role = Wt::DisplayRole) const ROSE_OVERRIDE {
        ASSERT_require(index.isValid());
        ASSERT_require(index.row() >= 0 && index.row() < rowCount());
        ASSERT_require(index.column() >= 0 && index.column() < columnCount());
        if (Wt::DisplayRole == role) {
            if (0 == index.column())
                return column0_[index.row()];
            return table_[index.row()][index.column()-1];
        }
        return boost::any();
    }

    boost::any headerData(int section, Wt::Orientation orientation = Wt::Horizontal,
                          int role = Wt::DisplayRole) const ROSE_OVERRIDE {
        if (orientation != Wt::Horizontal || role != Wt::DisplayRole)
            return boost::any();
        return columnTitles_[section];
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WStatusChart: public Wt::Chart::WCartesianChart {
public:
    explicit WStatusChart(StatusModel *model, Wt::WContainerWidget *parent = NULL)
        : Wt::Chart::WCartesianChart(parent) {

        setPlotAreaPadding(400, Wt::Left);              // make room for the dependency name (Git SHA1 hashes are long)
        setPlotAreaPadding(200, Wt::Right);             // make room for the legend
        setPlotAreaPadding(20, Wt::Top);                // make room for axis labels
        setPlotAreaPadding(0, Wt::Bottom);
        setOrientation(Wt::Horizontal);
        setLegendEnabled(true);

        setModel(model);
        setXSeriesColumn(0);

        for (int column = 1; column < model->columnCount(); ++column) {
            Wt::Chart::WDataSeries series(column, Wt::Chart::BarSeries);
#if 1
            series.setStacked(true);
#else
            series.setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));
#endif
            addSeries(series);
        }
    }

    void modelReset() ROSE_OVERRIDE {
        Wt::Chart::WCartesianChart::modelReset();
        int height = std::max(40 + 25 * std::min(model()->rowCount(), 15), 130);
        setHeight(height);
    }
};
        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

            // Combo box so we can pick a human value (i.e., bucket of database values) by which to limit queries later.
            dep.comboBox = new Wt::WComboBox;
            dep.comboBox->addItem(WILD_CARD_STR);
            dep.comboBox->activated().connect(this, &WConstraints::emitConstraintsChanged);

            // Find all values that the dependency can have. Depending on the dependency, we might want to use human-readable
            // values (like yyyy-mm-dd instead of a unix time stamp), in which case the "select distinct" and "order by" SQL
            // clauses won't really do what we want. Regardless of whether we use human-readalbe names and buckets of values,
            // we need to store the original value from the SQL table so we can construct "where" clauses later.
            std::string depColumn = gstate.dependencyNames[depName];
            SqlDatabase::StatementPtr q = gstate.tx->statement("select distinct " + depColumn +
                                                               " from test_results"
                                                               " join users on test_results.reporting_user = users.uid");
            for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
                std::string rawValue = row.get<std::string>(0);
                std::string humanValue = humanDepValue(depName, rawValue, HUMAN_TERSE);
                dep.humanValues.insertMaybeDefault(humanValue) <<rawValue;
            }

            BOOST_FOREACH (const std::string &humanValue, dep.humanValues.keys())
                dep.comboBox->addItem(humanValue);
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

class WResultsConstraintsTab: public Wt::WContainerWidget {
    WConstraints *constraints_;
    StatusModel *statusModel_;
    WStatusChart *statusChart_;
    Wt::WComboBox *xAxisChoices_;
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

        // Model and chart of test status totals
        statusModel_ = new StatusModel;
        resultsBox->addWidget(statusChart_ = new WStatusChart(statusModel_));

        // Combo box to choose what to display as the X axis for the test status chart
        Wt::WHBoxLayout *xAxisBox = new Wt::WHBoxLayout;
        xAxisBox->addSpacing(300);
        xAxisBox->addWidget(new Wt::WLabel("Axis:"));
        xAxisChoices_ = new Wt::WComboBox;
        int i = 0;
        BOOST_FOREACH (const std::string &depName, gstate.dependencyNames.keys()) {
            xAxisChoices_->addItem(depName);
            if (depName == "boost")
                xAxisChoices_->setCurrentIndex(i);
            ++i;
        }
        xAxisBox->addWidget(xAxisChoices_);
        xAxisBox->addStretch(1);
        resultsBox->addLayout(xAxisBox);

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

        // Button to reset everything to the initial state.
        Wt::WPushButton *reset = new Wt::WPushButton("Reset");
        reset->clicked().connect(this, &WResultsConstraintsTab::resetConstraints);
        constraintsBox->addWidget(reset);

        // Constraints
        constraintsBox->addWidget(constraints_ = new WConstraints);
        constraints_->constraintsChanged().connect(this, &WResultsConstraintsTab::updateStatusCounts);

        // Update button to reload data from the database
        Wt::WHBoxLayout *updateBox = new Wt::WHBoxLayout;
        Wt::WPushButton *updateButton = new Wt::WPushButton("Update");
        updateButton->clicked().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        updateBox->addWidget(updateButton);
        updateBox->addStretch(1);
        constraintsBox->addLayout(updateBox);

        //---------
        // Wiring
        //---------
        vbox->addStretch(1);
        xAxisChoices_->activated().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        updateStatusCounts();
    }

    WConstraints* constraints() const {
        return constraints_;
    }

private:
    void updateStatusCounts() {
        ::mlog[DEBUG] <<"WApplication::updateStatusCounts\n";
        statusModel_->setDependencyName(xAxisChoices_->currentText().narrow());
        statusModel_->updateModel(constraints_->dependencies());
    }

    void resetConstraints() {
        ::mlog[DEBUG] <<"WApplication::resetConstraints\n";
        constraints_->resetConstraints();
    }
};

        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WDetails: public Wt::WContainerWidget {
    std::vector<int> testIds_;
    Wt::WComboBox *testIdChoices_;
    int testId_;
    Wt::Signal<> testIdChanged_;
    Wt::WText *config_, *testOutput_;

public:
    explicit WDetails(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), testId_(-1) {
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        setLayout(vbox);

        // Combo box to choose which test to display
        Wt::WHBoxLayout *choiceBox = new Wt::WHBoxLayout;
        choiceBox->addWidget(new Wt::WLabel("Test"));
        testIdChoices_ = new Wt::WComboBox;
        testIdChoices_->activated().connect(this, &WDetails::selectTestId);
        choiceBox->addWidget(testIdChoices_);
        choiceBox->addStretch(1);
        vbox->addLayout(choiceBox);

        // Configuration
        config_ = new Wt::WText;
        config_->setTextFormat(Wt::PlainText);
        config_->setWordWrap(false);
        vbox->addWidget(config_);

        // Tests final output
        testOutput_ = new Wt::WText;
        testOutput_->setTextFormat(Wt::XHTMLText);
        testOutput_->setWordWrap(false);
        vbox->addWidget(testOutput_, 1);
    }

    void queryTestIds(const Dependencies &deps) {
        std::vector<std::string> args;
        std::string sql = "select id, status from test_results join users on test_results.reporting_user = users.uid" +
                          sqlWhereClause(deps, args) +
                          " order by id";
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
        sql += " from test_results join users on test_results.reporting_user = users.uid";

        std::vector<std::string> args;
        std::string where = sqlWhereClause(deps, args);
        where += std::string(where.empty() ? " where " : " and ") + "id = ?";
        args.push_back(boost::lexical_cast<std::string>(testId_));
        sql += where;

        config_->setText("");
        std::string config;
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, args);
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            int column = 0;
            BOOST_FOREACH (const std::string &name, columns.keys())
                config += name + "=" + humanDepValue(name, row.get<std::string>(column++)) + "\n";
            break;
        }
        config_->setText(config);

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

    void updateOutput() {
        testOutput_->setText("");
        if (testId_ >= 0) {
            SqlDatabase::StatementPtr q = gstate.tx->statement("select content from attachments where test_id = ? and name = ?");
            q->bind(0, testId_);
            q->bind(1, "Final output");
            for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
                std::string s = row.get<std::string>(0);
                std::string t;

                // Replace characters that are special for HTML
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

                // Look for special compiler output lines for errors and warnings
                boost::regex compilerRegex("(^[^\\n]*?(?:"
                                           // Errors
                                           "\\berror:"
                                           "|\\[ERROR\\]"

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
                break;
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WApplication: public Wt::WApplication {
    WResultsConstraintsTab *resultsConstraints_;
    WDetails *details_;
    Wt::WTabWidget *tabs_;

public:
    WApplication(const Wt::WEnvironment &env)
        : Wt::WApplication(env) {
        setTitle("ROSE testing matrix");
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        root()->setLayout(vbox);

        styleSheet().addRule(".compiler-error",   "color:#680000; background-color:#ffc0c0;"); // reds
        styleSheet().addRule(".compiler-warning", "color:#8f4000; background-color:#ffe0c7;"); // oranges
        styleSheet().addRule(".output-separator", "background-color:#808080;");

        tabs_ = new Wt::WTabWidget();
        tabs_->addTab(resultsConstraints_ = new WResultsConstraintsTab, "Overview");
        tabs_->addTab(details_ = new WDetails, "Details");
        vbox->addWidget(tabs_);

        // Wiring
        resultsConstraints_->constraints()->constraintsChanged().connect(this, &WApplication::getMatchingTests);
        details_->testIdChanged().connect(this, &WApplication::updateDetails);
        getMatchingTests();
    }

private:
    void getMatchingTests() {
        ::mlog[DEBUG] <<"WApplication::getMatchingTests\n";
        details_->queryTestIds(resultsConstraints_->constraints()->dependencies());
    }

    void updateDetails() {
        ::mlog[DEBUG] <<"WApplication::updateDetails\n";
        details_->updateDetails(resultsConstraints_->constraints()->dependencies());
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    // General switches
    parser.with(Switch("help", 'h')
                .doc("Show this documentation.")
                .action(showHelpAndExit(0)));

    parser.with(Switch("log", 'L')
                .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
                .argument("config")
                .whichValue(SAVE_ALL)
                .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));

    // Switches for HTTP server
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

    parser
        .purpose("serves matrix testing results as HTML")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{database}")
        .doc("description",
             "This is a web server for querying matrix testing results.");

    std::vector<std::string> positionalArgs = parser.parse(argc, argv).apply().unreachedArgs();
    if (positionalArgs.size() > 1) {
        ::mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    } else if (positionalArgs.size() == 1) {
        gstate.dbUrl = positionalArgs[0];
    }
}

class StatusSorter {
    const Sawyer::Container::Map<std::string, int> &positions;
public:
    explicit StatusSorter(const Sawyer::Container::Map<std::string, int> &positions)
        : positions(positions) {}

    bool operator()(const std::string &a, const std::string &b) {
        int ai = positions.getOrElse(a, 99999999);
        int bi = positions.getOrElse(b, 99999999);
        if (ai != bi)
            return ai < bi;
        return a < b;
    }
};

static std::vector<std::string>
loadTestNames(const SqlDatabase::TransactionPtr &tx) {
    std::vector<std::string> retval;
    SqlDatabase::StatementPtr q = tx->statement("select distinct status from test_results");
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
        retval.push_back(row.get<std::string>(0));

    Sawyer::Container::Map<std::string, int> positions;
    q = tx->statement("select name, position from test_names");
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
        positions.insert(row.get<std::string>(0), row.get<int>(1));

    std::sort(retval.begin(), retval.end(), StatusSorter(positions));
    return retval;
}

// These are the dependencies that will show up as constraints that the user can adjust.
static DependencyNames
loadDependencyNames(const SqlDatabase::TransactionPtr &tx) {
    DependencyNames retval;
    SqlDatabase::StatementPtr q = tx->statement("select distinct name from dependencies");
    for (SqlDatabase::Statement::iterator row=q->begin(); row!=q->end(); ++row) {
        std::string key = row.get<std::string>(0);
        retval.insert(key, "rmc_"+key);
    }

    // Additional key/column relationships
    retval.insert("reporting_user", "users.name");
    retval.insert("reporting_time", "reporting_time");
    retval.insert("tester", "tester");
    retval.insert("os", "os");
    retval.insert("rose", "rose");
    retval.insert("rose_date", "rose_date");

    return retval;
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
    gstate.dependencyNames = loadDependencyNames(gstate.tx);
    gstate.testNames = loadTestNames(gstate.tx);
    
    // Start the web server
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
#else
    ::mlog[FATAL] <<"this program must be configured with Wt web toolkit support\n";
    exit(1);
#endif
}
