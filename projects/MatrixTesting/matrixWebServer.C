#include <rosePublicConfig.h>

#include <Sawyer/Message.h>

using namespace Sawyer::Message::Common;
static Sawyer::Message::Facility mlog;

#ifdef ROSE_USE_WT

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <Color.h>                                      // ROSE
#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <Sawyer/Set.h>
#include <SqlDatabase.h>                                // ROSE
#include <string>
#include <vector>
#include <Wt/Auth/AuthService>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/Auth/Dbo/UserDatabase>
#include <Wt/Auth/HashFunction>
#include <Wt/Auth/Login>
#include <Wt/Auth/PasswordService>
#include <Wt/Auth/PasswordStrengthValidator>
#include <Wt/Auth/PasswordVerifier>
#include <Wt/Chart/WCartesianChart>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/Dbo/Dbo>
#include <Wt/WAbstractTableModel>
#include <Wt/WApplication>
#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WInPlaceEdit>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WLength>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WScrollArea>
#include <Wt/WStackedWidget>
#include <Wt/WStringListModel>
#include <Wt/WTable>
#include <Wt/WTableView>
#include <Wt/WTabWidget>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

using namespace rose;

static const char* WILD_CARD_STR = "*";
enum ChartType { BAR_CHART, LINE_CHART };
enum ChartValueType { CVT_COUNT, CVT_PERCENT, CVT_PASS_RATIO, CVT_WARNINGS_AVE, CVT_DURATION_AVE };
enum HumanFormat { HUMAN_TERSE, HUMAN_VERBOSE };
static int END_STATUS_POSITION = 999;                   // test_names.position where name = 'end'

typedef Sawyer::Container::Map<std::string, int> StringIndex;
typedef Sawyer::Container::Map<std::string, std::string> StringString;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Combo box with extra data. Type T should have a "display" method that returns an std::string that will become part of the
// value displayed by the combo box.  T should also be copyable.  We might have been able to implement this using only the
// default Wt::WComboBox model with extra columns.

// Stub type for a WComboBoxWithData that has no data.
struct ComboBoxNoData {
    std::string display() const {
        return "";
    }
};

// Model for a WComboBoxWithData
template<class T = ComboBoxNoData>
class ComboBoxModel: public Wt::WStringListModel {
    std::vector<T> extraData_;
public:
    static const int BaseTextRole = Wt::UserRole;

    explicit ComboBoxModel(Wt::WObject *parent = NULL)
        : Wt::WStringListModel(parent) {}
    explicit ComboBoxModel(const std::vector<Wt::WString> &strings, Wt::WObject *parent = NULL)
        : Wt::WStringListModel(strings, parent) {}

    // The base string (without any extra data attached to the end)
    Wt::WString baseString(const Wt::WModelIndex &idx) const {
        boost::any v = data(idx, BaseTextRole);
        if (v.empty())
            return "";
        return boost::any_cast<Wt::WString>(v);
    }

    // The full string: base string plus attached data.
    Wt::WString fullString(const Wt::WModelIndex &idx) const {
        boost::any v = data(idx, Wt::DisplayRole);
        if (v.empty())
            return "";
        return boost::any_cast<Wt::WString>(v);
    }
    
    // Associate some data with an item.
    void setItemExtraData(const Wt::WModelIndex &idx, const T &data) {
        if (idx.row() >= extraData_.size())
            extraData_.resize(idx.row()+1);
        std::string oldDisplay = extraData_[idx.row()].display();
        std::string newDisplay = data.display();
        bool displayChanged = oldDisplay != newDisplay;
        extraData_[idx.row()] = data;
        if (displayChanged)
            dataChanged().emit(idx, idx);
    }

    // Get data for an item.
    const T& itemExtraData(const Wt::WModelIndex &idx) const {
        static const T dflt;
        return idx.row() < extraData_.size() ? extraData_[idx.row()] : dflt;
    }

    // Find first item with specified data. Returns -1 if not found.
    int findData(const T &data) {
        for (size_t i=0; i<extraData_.size(); ++i) {
            if (extraData_[i] == data)
                return i;
        }
        return -1;
    }

    // Remove some data
    virtual bool removeRows(int row, int count, const Wt::WModelIndex &parent = Wt::WModelIndex()) {
        ASSERT_require(row + count <= extraData_.size());
        if (Wt::WStringListModel::removeRows(row, count, parent)) {
            extraData_.erase(extraData_.begin()+row, extraData_.begin()+row+count);
            return true;
        }
        return false;
    }

    virtual boost::any data(const Wt::WModelIndex &idx, int role = Wt::DisplayRole) const ROSE_OVERRIDE {
        if (idx.isValid() && BaseTextRole == role) {
            return Wt::WStringListModel::data(idx, Wt::DisplayRole);
        } else if (idx.isValid() && Wt::DisplayRole == role) {
            boost::any v = Wt::WStringListModel::data(idx, Wt::DisplayRole);
            Wt::WString s1 = v.empty() ? Wt::WString() : boost::any_cast<Wt::WString>(v);
            Wt::WString s2 = (idx.row() < extraData_.size() ? extraData_[idx.row()] : T()).display();
            return s1 + (s1.empty() || s2.empty() ? "" : " ") + s2;
        } else {
            return Wt::WStringListModel::data(idx, role);
        }
    }
};

// Combo box that shows text items but also has extra (hidden) data with each item. This is mostly compatible with
// Wt::WComboBox.
template<typename T = ComboBoxNoData>
class WComboBoxWithData: public Wt::WContainerWidget {
    typedef Sawyer::Container::Map<std::string, T> DataMap;
    ComboBoxModel<T> *model_;
    Wt::WComboBox *comboBox_;
public:
    explicit WComboBoxWithData(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), model_(new ComboBoxModel<T>), comboBox_(new Wt::WComboBox) {
        comboBox_->setModel(model_);
        addWidget(comboBox_);
        setInline(true);
    }

    void addItem(const std::string &item, const T &data = T()) {
        comboBox_->addItem(item);
        int rowIdx = comboBox_->count() - 1;
        ASSERT_require(rowIdx >= 0);
        model_->setItemExtraData(model_->index(rowIdx, 0), data);
    }

    void setItemData(int idx, const T &data) {
        if (idx >= 0 && idx < comboBox_->count())
            model_->setItemExtraData(model_->index(idx, 0), data);
    }

    int currentIndex() const {
        return comboBox_->currentIndex();
    }

    void setCurrentIndex(int idx) {
        comboBox_->setCurrentIndex(idx);
    }

    Wt::WString itemFullText(int idx) const {
        return comboBox_->itemText(idx);
    }
        
    Wt::WString currentFullText() const {
        return itemFullText(currentIndex());
    }

    Wt::WString itemBaseText(int idx) const {
        if (idx < 0 || idx >= comboBox_->count())
            return "";
        return model_->baseString(model_->index(idx, 0));
    }

    Wt::WString currentBaseText() const {
        return itemBaseText(currentIndex());
    }

    const T& itemData(int idx) const {
        return model_->itemExtraData(model_->index(idx, 0));
    }

    const T& currentData() const {
        return itemData(currentIndex());
    }

    // First item with data equal to specified data, or -1
    int findData(const T &data) const {
        return model_->findData(data);
    }

    Wt::Signal<int>& activated() {
        return comboBox_->activated();
    }

    int count() const {
        return comboBox_->count();
    }

    void clear() {
        model_->removeRows(0, count());
    }
    
    void setMinimumSize(const Wt::WLength &width, const Wt::WLength &height) {
        comboBox_->setMinimumSize(width, height);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Authentication and authorization stuff

// The user table holds information about who submitted tests, who can view the private parts of the web app, and who can
// modify the public parts of the web app, etc.
class User {
public:
    // Authorizations
    std::string fullName;                               // Full name of user, as in "Robb Matzke"
    std::string testSubmissionToken;                    // Token require to submit test results; empty means not permitted
    bool isPublisher;                                   // Is user allowed to modify the public-facing web interface
    bool isAdministrator;                               // Administrator account bypasses all security
    bool pwChangeRequired;                              // If true, then a password change is required

    User()
        : isPublisher(false), isAdministrator(false) {}

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, fullName, "full_name");
        Wt::Dbo::field(a, testSubmissionToken, "test_submission_token");
        Wt::Dbo::field(a, isPublisher, "is_publisher");
        Wt::Dbo::field(a, isAdministrator, "is_administrator");
        Wt::Dbo::field(a, pwChangeRequired, "update_password");
    }
};

// We use the database (Dbo) components of Wt::Auth. The AuthInfo contains info about each user and a foreign key to our User
// instances defined above. Wt::Auth allows multiple "identities" per user where an identity is, e.g., a login name, although
// we only use one identity per user.
typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;
typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

// Session-specific stuff, such as database connections.  Most of the database heavy lifting is done with ROSE's SqlDatabase
// layer, but some of the lighter stuff (like user management) is done with Wt::Dbo. This complicates things a bit because
// while ROSE's SqlDatabase doesn't cache anything (it communicates directly with the server), the Wt::Dbo API does cache. We
// have to jump through extra hoops to cause Wt::Dbo to not cache, and we do that by creating and destroying lots of
// transactions.
class Session: public Wt::Dbo::Session {
    Wt::Dbo::backend::Postgres connection_;
    UserDatabase *users_;
    Wt::Auth::Login login_;
    Wt::Auth::AuthService authenticationService_;       // we really only need one across all sessions
    Wt::Auth::PasswordService passwordService_;         // we really only need one across all sessions
public:
    Session(const std::string& dbUrl)
        : connection_(SqlDatabase::Connection::connectionSpecification(dbUrl)), users_(NULL),
          passwordService_(authenticationService_) {
        authenticationService_.setAuthTokensEnabled(true, "logincookie");
        authenticationService_.setEmailVerificationEnabled(false);

        Wt::Auth::PasswordVerifier *verifier = new Wt::Auth::PasswordVerifier;
        verifier->addHashFunction(new Wt::Auth::BCryptHashFunction(7));
        passwordService_.setVerifier(verifier);
        passwordService_.setAttemptThrottlingEnabled(true);
        passwordService_.setStrengthValidator(new Wt::Auth::PasswordStrengthValidator);

        setConnection(connection_);

        mapClass<User>("auth_users");                                   // our own user information
        mapClass<AuthInfo>("auth_info");                                // Wt::Auth's user information
        mapClass<AuthInfo::AuthIdentityType>("auth_identities");        // Identities for each usr (we only use one per user)
        mapClass<AuthInfo::AuthTokenType>("auth_tokens");               // Login tokens, such as cookies

        try {
            createTables();
            mlog[INFO] <<"created new user database tables\n";
        } catch (Wt::Dbo::Exception& e) {
            mlog[INFO] <<"using existing user database tables\n";
        }

        users_ = new UserDatabase(*this);
    }

    ~Session() {
        delete users_;
    }

public:
    // Returns the authentication service
    Wt::Auth::AuthService& authenticationService() {
        return authenticationService_;
    }

    // Returns the password service.
    Wt::Auth::PasswordService& passwordService() {
        return passwordService_;
    }

    // Returns the user database. WARNING: do not use this directly; rather use the functions in this class to manage the
    // transactions in order to coordinate with database access that doesn't go through Wt::Dbo.
    UserDatabase& userDatabase() {
        return *users_;
    }

    // Returns the login manager.
    Wt::Auth::Login& loginService() {
        return login_;
    }
    
    // Returns true if the specified user is valid and is a publisher (or administrator) and not disabled.
    bool isPublisher(const Wt::Auth::User &authUser) {
        bool retval = false;
        if (!authUser.isValid())
            return false;
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        Wt::Dbo::ptr<AuthInfo> authInfo = findAuthInfo(authUser, tx);
        if (authInfo && authInfo->status() == Wt::Auth::User::Normal) {
            if (Wt::Dbo::ptr<User> user = authInfo->user())
                retval = user->isPublisher || user->isAdministrator;
        }
        tx->commit();
        return retval;
    }

    // Returns true if the specified user is valid and is an administrator and not disabled.
    bool isAdministrator(const Wt::Auth::User &authUser) {
        bool retval = false;
        if (!authUser.isValid())
            return false;
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        Wt::Dbo::ptr<AuthInfo> authInfo = findAuthInfo(authUser, tx);
        if (authInfo && authInfo->status() == Wt::Auth::User::Normal) {
            if (Wt::Dbo::ptr<User> user = findUser(authUser, tx))
                retval = user->isAdministrator;
        }
        tx->commit();
        return retval;
    }

    // Returns the full name of the user if valid, or an empty string.
    std::string fullName(const Wt::Auth::User &authUser) {
        std::string retval;
        if (!authUser.isValid())
            return retval;
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        if (Wt::Dbo::ptr<User> user = findUser(authUser, tx))
            retval = user->fullName;
        tx->commit();
        return retval;
    }

    // Returns the login name of the user if valid, or an empty string.
    std::string loginName(const Wt::Auth::User &authUser) {
        if (!authUser.isValid())
            return "";
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        std::string retval = authUser.identity("loginname").narrow();
        tx->commit();
        return retval;
    }

    // Returns the (unverified) email of the user if valid, or an empty string.
    std::string email(const Wt::Auth::User &authUser) {
        if (!authUser.isValid())
            return "";
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        std::string retval = authUser.unverifiedEmail();
        tx->commit();
        return retval;
    }

    // Look up a user by login name, or return an invalid user.
    Wt::Auth::User findLogin(const std::string &loginName) {
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        Wt::Auth::User user = users_->findWithIdentity("loginname", loginName);
        tx->commit();
        return user;
    }

    // Returns info about the currently logged in user, or the invalid user if nobody is logged in. The returned user's account
    // might be in a disabled state.
    Wt::Auth::User currentUser() {
        Wt::Auth::User user;
        if (login_.loggedIn())
            user = login_.user();
        return user;
    }

    // Create a new user. The loginName must be unique.
    Wt::Auth::User
    createUser(const std::string &fullName, const std::string &loginName, const std::string &email,
               const std::string &password, bool isAdministrator = false, bool isPublisher = false) {
        ASSERT_require(!loginName.empty());
        ASSERT_forbid(findLogin(loginName).isValid());

        Wt::Auth::User retval;
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();

        // Create the info needed by our own queries
        User *user = new User;
        user->fullName = fullName;
        user->isPublisher = isPublisher;
        user->isAdministrator = isAdministrator;
        Wt::Dbo::ptr<User> userRecord = add(user);

        // Create the user used by Wt::Auth for authentication
        retval = users_->registerNew();
        Wt::Dbo::ptr<AuthInfo> authInfoRecord = users_->find(retval);
        ASSERT_require(authInfoRecord);
        authInfoRecord.modify()->setUser(userRecord);
        authInfoRecord.modify()->setStatus(Wt::Auth::User::Normal);
        authInfoRecord.modify()->setUnverifiedEmail(email);
        passwordService_.updatePassword(retval, password);
        users_->addIdentity(retval, "loginname", loginName);

        tx->commit();
        return retval;
    }

    // Update info for an existing user specified by the "authUser" argument, which must be valid.
    Wt::Auth::User
    updateUser(Wt::Auth::User authUser, const std::string &fullName, const std::string &email,
               const std::string &password = "", bool isAdministrator = false, bool isPublisher = false) {
        ASSERT_require(authUser.isValid());
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();

        Wt::Dbo::ptr<AuthInfo> authInfoRecord = findAuthInfo(authUser, tx);
        ASSERT_require(authInfoRecord);
        Wt::Dbo::ptr<User> user = authInfoRecord->user();
        ASSERT_require(user);

        user.modify()->fullName = fullName;
        user.modify()->isAdministrator = isAdministrator;
        user.modify()->isPublisher = isPublisher;

        authInfoRecord.modify()->setUnverifiedEmail(email);

        if (!password.empty())
            passwordService_.updatePassword(authUser, password);

        authUser = users_->find(authInfoRecord);
        ASSERT_require(authUser.isValid());
        tx->commit();

        return authUser;
    }

    // Cause the specified user to be logged in (or out if user is invalid).
    void login(const Wt::Auth::User &authUser) {
        login_.login(authUser);
    }

    // True if the user needs to change his password
    bool
    pwChangeRequired(const Wt::Auth::User &authUser) {
        bool retval = false;
        if (!authUser.isValid())
            return retval;
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        Wt::Dbo::ptr<User> user = findUser(authUser, tx);
        retval = user && user->pwChangeRequired;
        tx->commit();
        return retval;
    }

    // Make a password change required at next login
    void
    setPwChangeRequired(const Wt::Auth::User &authUser, bool b = true) {
        if (!authUser.isValid())
            return;
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        Wt::Dbo::ptr<User> user = findUser(authUser, tx);
        user.modify()->pwChangeRequired = b;
        tx->commit();
    }
    
    // Verify the user's password
    Wt::Auth::PasswordResult
    verifyPassword(const Wt::Auth::User &authUser, const std::string &password) {
        ASSERT_require(authUser.isValid());
        Wt::Auth::PasswordResult retval = Wt::Auth::PasswordInvalid;
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        retval = passwordService_.verifyPassword(authUser, password);
        tx->commit();
        return retval;
    }

    // Set user password
    void
    setPassword(const Wt::Auth::User &authUser, const std::string &password) {
        ASSERT_require(authUser.isValid());
        Wt::Auth::AbstractUserDatabase::Transaction *tx = users_->startTransaction();
        passwordService_.updatePassword(authUser, password);
        findUser(authUser, tx).modify()->pwChangeRequired = false;
        tx->commit();
    }
    
private:
    Wt::Dbo::ptr<AuthInfo>
    findAuthInfo(const Wt::Auth::User &authUser, Wt::Auth::AbstractUserDatabase::Transaction *tx) {
        ASSERT_not_null(tx);
        return users_->find(authUser);
    }

    Wt::Dbo::ptr<User>
    findUser(const Wt::Dbo::ptr<AuthInfo> &authInfo, Wt::Auth::AbstractUserDatabase::Transaction *tx) {
        ASSERT_not_null(tx);
        Wt::Dbo::ptr<User> retval;
        if (authInfo)
            retval = authInfo->user();
        return retval;
    }

    Wt::Dbo::ptr<User>
    findUser(const Wt::Auth::User &authUser, Wt::Auth::AbstractUserDatabase::Transaction *tx) {
        ASSERT_not_null(tx);
        Wt::Dbo::ptr<User> retval;
        if (!authUser.isValid())
            return retval;
        if (Wt::Dbo::ptr<AuthInfo> authInfo = users_->find(authUser))
            retval = findUser(authInfo, tx);
        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Represents a bucket of values by storing a min and max value. New values can be inserted into the bucket with operator <<,
// which adjusts the min and/or max but doesn't explicitly store the value.
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

struct DependencyComboBoxData {
    int nPass;                                          // number of these dependencies that pass

    explicit DependencyComboBoxData(int nPass = -1)
        : nPass(nPass) {}

    std::string display() const {
        if (nPass >= 0)
            return "(" + boost::lexical_cast<std::string>(nPass) + ")";
        return "";
    }
};

typedef WComboBoxWithData<DependencyComboBoxData> DependencyComboBox;
    
struct Dependency {
    typedef Sawyer::Container::Map<std::string, Bucket<std::string> > Choices;

    std::string name;                                   // name of dependency, such as "boost"
    Choices humanValues;                                // human-readable values and how they map to the database values
    DependencyComboBox *comboBox;                       // choices available to the user
    std::string sqlExpression;                          // optional SQL to override the column name from gstate

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
    return (" from test_results as test"
            " join auth_identities as auth_user on test.reporting_user = auth_user.id"
            " left outer join test_names on test.status = test_names.name ");
}

static std::string
sqlDependencyExpression(const Dependency &dep, const std::string &depName) {
    std::string retval = dep.sqlExpression;
    if (retval.empty())
        retval = gstate.dependencyNames[depName];
    ASSERT_require(!retval.empty());
    return retval;
}

static std::string
sqlDependencyExpression(const Dependencies &deps, const std::string &depName) {
    if (!deps.exists(depName)) {
        ASSERT_require(gstate.dependencyNames.exists(depName));
        return gstate.dependencyNames[depName];
    }
    return sqlDependencyExpression(deps[depName], depName);
}

static std::string
sqlWhereClause(const Dependencies &deps, std::vector<std::string> &args) {
    std::string where = " where test.enabled";
    BOOST_FOREACH (const Dependency &dep, deps.values()) {
        // Get the human value from the combo box. Sometimes a combo box will display (Wt::DisplayRole) a different value than
        // what should be used as the human value. In this case, the underlying model will support Wt::UserRole to return the
        // human value.
        std::string humanValue = dep.comboBox->currentBaseText().narrow();

        Bucket<std::string> bucket;
        if (humanValue.compare(WILD_CARD_STR) != 0 && dep.humanValues.getOptional(humanValue).assignTo(bucket)) {
            std::string depColumn = sqlDependencyExpression(dep, dep.name);
            where += " and ";
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
    return where + " ";
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

// Sorts human-friendly values of a dependency
static std::vector<std::string>
sortedHumanValues(const Dependency &dep) {
    std::vector<std::string> retval(dep.humanValues.keys().begin(), dep.humanValues.keys().end());
    std::sort(retval.begin(), retval.end(), DependencyValueSorter(dep.name));
    return retval;
}

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
humanDuration(long seconds, HumanFormat fmt = HUMAN_VERBOSE) {
    bool isNegative = seconds < 0;
    if (seconds < 0)
        seconds = -seconds;
    unsigned hours = seconds / 3600;
    unsigned minutes = seconds / 60 % 60;
    seconds %= 60;

    if (HUMAN_VERBOSE == fmt) {
        std::string retval = boost::lexical_cast<std::string>(seconds) + " second" + (1==seconds?"":"s");
        if (hours > 0 || minutes > 0)
            retval = boost::lexical_cast<std::string>(minutes) + " minute" + (1==minutes?"":"s") + " " + retval;
        if (hours > 0)
            retval = boost::lexical_cast<std::string>(hours) + " hour" + (1==hours?"":"s") + " " + retval;
        return (isNegative?"-":"") + retval;
    } else {
        char buf[256];
        sprintf(buf, "%s%2d:%02d:%02u", isNegative?"-":"", hours, minutes, (unsigned)seconds);
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

// Loads information about the possible values of the specified dependencies.  E.g., if depNames contains the word "compiler"
// then the test_results table is queried to obtain a list of all values for the corresponding compiler expression (probably
// just the column named "test.rmc_compiler" according to gstate.dependencyNames).
static Dependencies
loadDependencyValues(const std::vector<std::string> &depNames,
                     const std::string &whereClause = "",
                     const std::vector<std::string> &whereClauseArgs = std::vector<std::string>()) {
    Dependencies retval;
    BOOST_FOREACH (const std::string &depName, depNames) {
        Dependency &dep = retval.insertMaybe(depName, Dependency(depName));
        
        // Find all values that the dependency can have. Depending on the dependency, we might want to use human-readable
        // values (like yyyy-mm-dd instead of a unix time stamp), in which case the "select distinct" and "order by" SQL
        // clauses won't really do what we want. Regardless of whether we use human-readalbe names and buckets of values, we
        // need to store the original value from the SQL table so we can construct "where" clauses later.
        ASSERT_require(gstate.dependencyNames.exists(depName));
        std::string depExpr = gstate.dependencyNames[depName];
        std::string sql = "select distinct " + depExpr +
                          sqlFromClause() +
                          whereClause;
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, whereClauseArgs);
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            std::string rawValue = row.get<std::string>(0);
            std::string humanValue = humanDepValue(depName, rawValue, HUMAN_TERSE);
            dep.humanValues.insertMaybeDefault(humanValue) <<rawValue;
        }
    }
    return retval;
}

// Loads info about possible values for all known dependencies.
static Dependencies
loadDependencyValues() {
    std::vector<std::string> depNames;
    BOOST_FOREACH (const std::string &depName, gstate.dependencyNames.keys())
        depNames.push_back(depName);
    return loadDependencyValues(depNames);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extra data attached to combo boxes that show ROSE version information. The combo box's base text is a date string (usually
// sorted by date) and a human-readable version number extends the string. Therefore this struct holds the version string.
struct ComboBoxVersion {
    std::string version;                                // full-length version that appears in the database

    ComboBoxVersion() {}

    explicit ComboBoxVersion(const std::string &v)
        : version(v) {}

    std::string display() const {
        if (version.empty())
            return "";
        return humanSha1(version, HUMAN_TERSE);
    }
};

// Fill the version selection combo box, returning the first entry that matches the needle version (or -1)
int
fillVersionComboBox(WComboBoxWithData<ComboBoxVersion> *comboBox, const std::string &needle = "") {
    int found = -1;
    SqlDatabase::StatementPtr q = gstate.tx->statement("select distinct rose, rose_date"
                                                       " from test_results"
                                                       " order by rose_date");
    Sawyer::Container::Set<std::string> uniqueValues;
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
        std::string version = row.get<std::string>(0);
        std::string date = humanLocalTime(row.get<unsigned long>(1), HUMAN_TERSE);
        uniqueValues.insert(date + "\t" + version);
    }

    BOOST_FOREACH (const std::string &s, uniqueValues.values()) {
        size_t tab = s.find('\t');
        ASSERT_require(tab != std::string::npos);
        std::string date = s.substr(0, tab);
        std::string version = s.substr(tab+1);
        comboBox->addItem(date, ComboBoxVersion(version));
        if (-1 == found && version == needle)
            found = comboBox->count()-1;
    }
    return found;
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

    struct DataSet {
        Table counts;                                   // counts of matching rows
        double minCounts, maxCounts;                    // min/max values in counts_ (excluding zeros)
        Table passes;                                   // portion of counts_ where pass_fail = 'passed'
        Table aveWarnings;                              // average number of compiler warnings per run
        double minAveWarnings, maxAveWarnings;          // min/max value in aveWarnings_
        Table aveDuration;                              // average wall-clock duration per run
        double minAveDuration, maxAveDuration;          // min/max value in aveDuration_
    };

    std::string baselineVersion_;                       // software version to use as the baseline, or empty for none
    DataSet baseline_;                                  // data for baseline version if there is one
    DataSet current_;                                   // the non-baseline data
    Table delta_;                                       // difference between baseline_ and current_
    double minDelta_, maxDelta_;                        // min/max value in delta_

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
        : Wt::WAbstractTableModel(parent), chartValueType_(CVT_PERCENT), roundToInteger_(false), humanReadable_(false),
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

    const std::string& baselineVersion() const {
        return baselineVersion_;
    }

    void setBaselineVersion(const std::string &version) {
        baselineVersion_ = version;
    }

    bool hasBaseline() const {
        return !baselineVersion_.empty();
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
        resetDataset(current_);
        resetDataset(baseline_);
        resetTable(delta_);
        
        if (current_.counts.empty())
            return modelReset().emit();

        loadDataset(deps, current_, "");

        if (hasBaseline()) {
            Dependencies limited = deps;
            limited.erase("rose");
            limited.erase("rose_date");
            loadDataset(limited, baseline_, baselineVersion_);
            for (size_t i=0; i<current_.counts.size(); ++i) {
                for (size_t j=0; j<current_.counts[i].size(); ++j) {
                    delta_[i][j] = getDataValue(current_, i, j) - getDataValue(baseline_, i, j);
                    if (0==i && 0==j) {
                        minDelta_ = maxDelta_ = delta_[0][0];
                    } else {
                        minDelta_ = std::min(minDelta_, delta_[i][j]);
                        maxDelta_ = std::max(maxDelta_, delta_[i][j]);
                    }
                }
            }
        }
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
        ASSERT_require(-1 == i || (size_t)i < current_.counts.size());
        ASSERT_require(-1 == j || (size_t)j < current_.counts[i].size());

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
            } else {
                double value = hasBaseline() ? delta_[i][j] : getDataValue(current_, i, j);
                int nSamples = current_.counts[i][j];
                if (hasBaseline())
                    nSamples = std::min(nSamples, (int)baseline_.counts[i][j]);
                if (roundToInteger_)
                    value = round(value);
                if (humanReadable_) {
                    std::string humanValue;
                    switch (chartValueType_) {
                        case CVT_COUNT:
                        case CVT_WARNINGS_AVE:
                            if (hasBaseline() && value > 0)
                                humanValue = "+";
                            humanValue += boost::lexical_cast<std::string>(value);
                            break;
                        case CVT_PERCENT:
                        case CVT_PASS_RATIO:
                            if (0 == nSamples) {
                                humanValue = "n/a";
                            } else {
                                if (hasBaseline() && value > 0)
                                    humanValue = "+";
                                humanValue += boost::lexical_cast<std::string>(value) + "%";
                            }
                            break;
                        case CVT_DURATION_AVE:
                            if (hasBaseline() && value > 0)
                                humanValue = "+";
                            humanValue += humanDuration(value, HUMAN_TERSE);
                            break;
                    }
                    return humanValue;
                }
                return value;
            }

        } else if (Wt::StyleClassRole == role) {
            if (i >= 0 && j >= 0) {
                double value = hasBaseline() ? delta_[i][j] : getDataValue(current_, i, j);
                std::pair<double, double> mm = getDataMinMax(current_, i, j);
                int nSamples = current_.counts[i][j];
                if (hasBaseline()) {
                    nSamples = std::min(nSamples, (int)baseline_.counts[i][j]);
                    mm = std::make_pair(minDelta_, maxDelta_);
                }

                switch (chartValueType_) {
                    case CVT_COUNT:
                        return redToGreen(value, mm.first, mm.second, value?5:0);
                    case CVT_PERCENT:
                    case CVT_PASS_RATIO:
                        if (hasBaseline())
                            mm = std::make_pair(-100.0, 100.0);
                        return redToGreen(value, mm.first, mm.second, nSamples);
                    case CVT_WARNINGS_AVE:
                    case CVT_DURATION_AVE:
                        return greenToRed(value, mm.first, mm.second, nSamples);
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

    void resetDataset(DataSet &d) {
        resetTable(d.counts);
        d.minCounts = d.maxCounts = 0.0;
        resetTable(d.passes);
        resetTable(d.aveWarnings);
        d.minAveWarnings = d.maxAveWarnings = 0.0;
        resetTable(d.aveDuration);
        d.minAveDuration = d.maxAveDuration = 0.0;
    }
    
    void loadDataset(const Dependencies &deps, DataSet &ds /*out*/, const std::string &version) {
        // Build the SQL query
        ASSERT_require(!depMajorName_.empty());
        std::string depMajorColumn = sqlDependencyExpression(deps, depMajorName_);
        std::string depMinorColumn = sqlDependencyExpression(deps, depMinorName_);
        std::string passFailColumn = sqlDependencyExpression(deps, "pass/fail");
        std::vector<std::string> args;
        std::string sql = "select " +
                          depMajorColumn + ", " +       // 0
                          depMinorColumn + ", " +       // 1
                          passFailColumn + " as pf, "   // 2
                          "count(*), " +                // 3
                          "sum(test.nwarnings)," +      // 4
                          "sum(test.duration)" +        // 5
                          sqlFromClause() +
                          sqlWhereClause(deps, args /*out*/) +
                          (version.empty() ? "" : "and rose = ?") +
                          " group by " + depMajorColumn + ", " + depMinorColumn + ", pf";
        if (!version.empty())
            args.push_back(version);
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, args);

        // If we're looking at a specific version of ROSE and the model has a version (number or time) based major axis, then
        // each datum applies to all rows of the table.  Similarly, for columns if the minor axis is based on a version.
        bool applyToAllRows = !version.empty() && (depMajorName_ == "rose" || depMajorName_ == "rose_date");
        bool applyToAllCols = !version.empty() && (depMinorName_ == "rose" || depMinorName_ == "rose_date");

        // Iterate over the query results to update the table. Remember that the row and column numbers are the human-style
        // values (e.g., yyyy-mm-dd rather than Unix time, etc.)
        for (SqlDatabase::Statement::iterator queryRow = q->begin(); queryRow != q->end(); ++queryRow) {
            std::string majorValue = humanDepValue(depMajorName_, queryRow.get<std::string>(0), HUMAN_TERSE);
            std::string minorValue = humanDepValue(depMinorName_, queryRow.get<std::string>(1), HUMAN_TERSE);
            std::string pf = queryRow.get<std::string>(2);
            int count = queryRow.get<int>(3);
            double nwarn = queryRow.get<int>(4);
            double duration = queryRow.get<int>(5);

            int row = depMajorIndex_.getOrElse(majorValue, -1);
            int col = depMinorIndex_.getOrElse(minorValue, -1);
            if (row >= 0 && col >= 0) {
                // aveWarnings and aveDuration are summed here and divided after this loop.
                if (applyToAllRows && applyToAllCols) {
                    for (size_t i=0; i<ds.counts.size(); ++i) {
                        for (size_t j=0; j<ds.counts[i].size(); ++j) {
                            ds.counts[i][j] += count;
                            ds.aveWarnings[i][j] += nwarn;
                            ds.aveDuration[i][j] += duration;
                            if (pf == "pass")
                                ds.passes[i][j] += count;
                        }
                    }
                } else if (applyToAllRows) {
                    for (size_t i=0; i<ds.counts.size(); ++i) {
                        ds.counts[i][col] += count;
                        ds.aveWarnings[i][col] += nwarn;
                        ds.aveDuration[i][col] += duration;
                        if (pf == "pass")
                            ds.passes[i][col] += count;
                    }
                } else if (applyToAllCols) {
                    for (size_t j=0; j<ds.counts[row].size(); ++j) {
                        ds.counts[row][j] += count;
                        ds.aveWarnings[row][j] += nwarn;
                        ds.aveDuration[row][j] += duration;
                        if (pf == "pass")
                            ds.passes[row][j] += count;
                    }
                } else {
                    ds.counts[row][col] += count;
                    ds.aveWarnings[row][col] += nwarn;
                    ds.aveDuration[row][col] += duration;
                    if (pf == "pass")
                        ds.passes[row][col] += count;
                }
            }
        }

        // Adjust aveWarnings and aveDuration to be averages instead of sums
        {
            int n = 0;
            for (size_t i=0; i<ds.aveWarnings.size(); ++i) {
                for (size_t j=0; j<ds.aveWarnings[i].size(); ++j) {
                    if (ds.counts[i][j] > 0) {
                        ds.aveWarnings[i][j] /= ds.counts[i][j];
                        ds.aveDuration[i][j] /= ds.counts[i][j];
                        if (0 == n++) {
                            ds.minCounts      = ds.maxCounts      = ds.counts[i][j];
                            ds.minAveWarnings = ds.maxAveWarnings = ds.aveWarnings[i][j];
                            ds.minAveDuration = ds.maxAveDuration = ds.aveDuration[i][j];
                        } else {
                            ds.minCounts      = std::min(ds.minCounts,      ds.counts[i][j]);
                            ds.maxCounts      = std::max(ds.maxCounts,      ds.counts[i][j]);
                            ds.minAveWarnings = std::min(ds.minAveWarnings, ds.aveWarnings[i][j]);
                            ds.maxAveWarnings = std::max(ds.maxAveWarnings, ds.aveWarnings[i][j]);
                            ds.minAveDuration = std::min(ds.minAveDuration, ds.aveDuration[i][j]);
                            ds.maxAveDuration = std::max(ds.maxAveDuration, ds.aveDuration[i][j]);
                        }
                    }
                }
            }
        }
        
        modelReset().emit();
    }

    // Returns a value from the dataset.
    double getDataValue(const DataSet &ds, int rowIdx, int colIdx) const {
        switch (chartValueType_) {
            case CVT_COUNT:
                return ds.counts[rowIdx][colIdx];

            case CVT_PERCENT:
                if (double count = ds.counts[rowIdx][colIdx]) {
                    double rowTotal = 0.0;
                    BOOST_FOREACH (double n, ds.counts[rowIdx])
                        rowTotal += n;
                    return 100.0 * count / rowTotal;
                }
                return 0.0;

            case CVT_PASS_RATIO:
                if (double count = ds.counts[rowIdx][colIdx])
                    return 100.0 * ds.passes[rowIdx][colIdx] / count;
                return 0.0;

            case CVT_WARNINGS_AVE:
                return ds.aveWarnings[rowIdx][colIdx];

            case CVT_DURATION_AVE:
                return ds.aveDuration[rowIdx][colIdx];
        }
        ASSERT_not_reachable("invalid chart value type");
    }

    std::pair<double, double> getDataMinMax(const DataSet &ds, int rowIdx, int colIdx) const {
        switch (chartValueType_) {
            case CVT_COUNT:
                return std::make_pair(ds.minCounts, ds.maxCounts);
            case CVT_PERCENT:
            case CVT_PASS_RATIO:
                if (hasBaseline()) {
                    return std::make_pair(-100.0, 100.0);
                } else {
                    return std::make_pair(0.0, 100.0);
                }
            case CVT_WARNINGS_AVE:
                return std::make_pair(ds.minAveWarnings, ds.maxAveWarnings);
            case CVT_DURATION_AVE:
                return std::make_pair(ds.minAveDuration, ds.maxAveDuration);
        }
        ASSERT_not_reachable("invalid chart value type");
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

        std::string columnName = sqlDependencyExpression(deps, depName);
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
            setOrientation(Wt::Horizontal);
        } else {
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

        // Figure out the height of the bars. If the legend is very tall we'll have to make the chart tall so the legend
        // fits. But if we do that and there's only a few bars, then the bars will be very tall also. We'd like the bars to
        // always be the same height regardless of how tall we make the chart, but the API doesn't have a method to set the bar
        // to a particular height -- only methods to adjust the margins around the bars.
        static const int BAR_HEIGHT = 25;               // height in pixels of each bar including margins
        static const int LEGEND_ITEM_HEIGHT = 20;       // height in pixels of each legend item including margins
        double barHeightRatio = 0.9;                    // height of colored part of bar as a ratio of total bar height
        if (BAR_CHART == chartType_) {
            double barsToLegend = (1.0 * model()->rowCount() * BAR_HEIGHT) / (model()->columnCount() * LEGEND_ITEM_HEIGHT);
            barHeightRatio = std::max(0.01, std::min(barsToLegend, 0.8));
        }

        // Build the data series, one per model column.
        std::vector<Wt::Chart::WDataSeries> series;
        size_t maxMinorValueLength = 0;
        for (int j=1; j<model_->columnCount(); ++j) {
            if (BAR_CHART == chartType_) {
                series.push_back(Wt::Chart::WDataSeries(j, Wt::Chart::BarSeries));
                series.back().setStacked(true);
                series.back().setBarWidth(barHeightRatio); // chart is rotated 90 degrees
            } else {
                series.push_back(Wt::Chart::WDataSeries(j, Wt::Chart::LineSeries));
                series.back().setMarker(Wt::Chart::SquareMarker);
            }
            std::string minorValue = model_->depMinorValue(j);
            maxMinorValueLength = std::max(maxMinorValueLength, minorValue.size());
            Wt::WColor color = dependencyValueColor(model_->depMinorName(), minorValue, j);
            series.back().setBrush(Wt::WBrush(color));
            series.back().setPen(Wt::WPen(color));
        }
        setSeries(series);

        // What is the maximum length of the major axis labels
        size_t maxMajorValueLength = 0;
        for (int i=0; i<model_->rowCount(); ++i) {
            std::string majorValue = model_->depMajorValue(i);
            maxMajorValueLength = std::max(maxMajorValueLength, majorValue.size());
        }

        // Adjust axis labels, ranges, and legend size.
        setPlotAreaPadding(35 + 7*maxMinorValueLength, Wt::Right);
        if (BAR_CHART == chartType_) {
            int topAxisHeight = 20;
            int bottomAxisHeight = 0;
            int leftAxisWidth = 20 + 7*maxMajorValueLength;
            int barsHeight = model()->rowCount() * BAR_HEIGHT - (/*correction*/6*(model()->rowCount()-3));
            int legendHeight = model()->columnCount() * LEGEND_ITEM_HEIGHT;
            int totalHeight = std::max(barsHeight, legendHeight) + topAxisHeight + bottomAxisHeight;
            setHeight(totalHeight);

            setPlotAreaPadding(topAxisHeight, Wt::Top);
            setPlotAreaPadding(bottomAxisHeight, Wt::Bottom);
            setPlotAreaPadding(leftAxisWidth, Wt::Left);

            axis(Wt::Chart::YAxis).setMinimum(0);
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
        } else {
            setHeight(400);
            setPlotAreaPadding(0, Wt::Top);
            setPlotAreaPadding(20 + 2.65 * maxMajorValueLength, Wt::Bottom);
            setPlotAreaPadding(40, Wt::Left);
            switch (model_->chartValueType()) {
                case CVT_COUNT:
                case CVT_WARNINGS_AVE:
                case CVT_DURATION_AVE:
                    if (model_->hasBaseline()) {
                        axis(Wt::Chart::YAxis).setAutoLimits(Wt::Chart::MinimumValue | Wt::Chart::MaximumValue);
                    } else {
                        axis(Wt::Chart::YAxis).setMinimum(0);
                        axis(Wt::Chart::YAxis).setAutoLimits(Wt::Chart::MaximumValue);
                    }
                    break;
                case CVT_PERCENT:
                case CVT_PASS_RATIO:
                    if (model_->hasBaseline()) {
                        axis(Wt::Chart::YAxis).setMinimum(-100);
                    } else {
                        axis(Wt::Chart::YAxis).setMinimum(0);
                    }
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
        dependencies_ = loadDependencyValues();
        BOOST_FOREACH (Dependency &dep, dependencies_.values()) {
            // Combo box so we can pick a human value (i.e., bucket of database values) by which to limit queries later.  Add
            // entries to the combo box, but make sure they're sorted. The default sort for the dep.humanValues.keys() is
            // alphabetical, but that's not always what we want. For instance, "status" should be sorted in the order that the
            // individual tests run, not their names.
            dep.comboBox = new DependencyComboBox;
            dep.comboBox->addItem(WILD_CARD_STR);
            dep.comboBox->setMinimumSize(Wt::WLength(20, Wt::WLength::FontEm), Wt::WLength::Auto);
            std::vector<std::string> comboValues = sortedHumanValues(dep);
            BOOST_FOREACH (const std::string &comboValue, comboValues)
                dep.comboBox->addItem(comboValue);
            dep.comboBox->activated().connect(this, &WConstraints::emitConstraintsChanged);
        }

        static const size_t nDepCols = 2;               // number of columns for dependencies
        size_t nDepRows = (dependencies_.size() + nDepCols - 1) / nDepCols;
        Wt::WTable *grid = new Wt::WTable;
        grid->columnAt(0)->setWidth(Wt::WLength(25, Wt::WLength::Percentage));
        grid->columnAt(1)->setWidth(Wt::WLength(25, Wt::WLength::Percentage));
        grid->columnAt(2)->setWidth(Wt::WLength(25, Wt::WLength::Percentage));
        grid->columnAt(3)->setWidth(Wt::WLength(25, Wt::WLength::Percentage));
        addWidget(grid);

        // Fill the grid in row-major order
        size_t i = 0;
        BOOST_FOREACH (const Dependencies::Node &depNode, dependencies_.nodes()) {
            int row = i % nDepRows;
            int col = i / nDepRows;
            std::string depLabel = depNode.key();
            grid->elementAt(row, 2*col+0)->addWidget(new Wt::WText(depLabel + "&nbsp;"));
            grid->elementAt(row, 2*col+0)->setStyleClass("constraint-name");
            grid->elementAt(row, 2*col+1)->addWidget(depNode.value().comboBox);
            grid->elementAt(row, 2*col+1)->setStyleClass("constraint-value");
            ++i;
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
    WComboBoxWithData<ComboBoxVersion> *chartBaselineChoices_;

public:
    explicit WResultsConstraintsTab(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent) {

        //------------
        // Chart area
        //------------

        addWidget(new Wt::WText("<h2>Test results</h2>"));
        chartStack_ = new Wt::WStackedWidget;           // added after settings

        // Bar and lines charts, which need to be in a container widget in order to span the entire width.
        chartModel_ = new StatusModel;
        chartModel_->setDepMajorIsData(true);

        WStatusChart2d *barChart = new WStatusChart2d(chartModel_, BAR_CHART);
        Wt::WHBoxLayout *barChartLayout = new Wt::WHBoxLayout;
        barChartLayout->addWidget(barChart, 1);
        Wt::WContainerWidget *barChartContainer = new Wt::WContainerWidget;
        barChartContainer->setLayout(barChartLayout);
        chartStack_->addWidget(barChartContainer);

        WStatusChart2d *lineChart = new WStatusChart2d(chartModel_, LINE_CHART);
        Wt::WHBoxLayout *lineChartLayout = new Wt::WHBoxLayout;
        lineChartLayout->addWidget(lineChart, 1);
        Wt::WContainerWidget *lineChartContainer = new Wt::WContainerWidget;
        lineChartContainer->setLayout(lineChartLayout);
        chartStack_->addWidget(lineChartContainer);

        // Table charts
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

        // Chart of plain text comma-separated values
        csvView_ = new WCommaSeparatedValues(tableModel_);
        chartStack_->addWidget(csvView_);

        //----------------
        // Chart settings
        //----------------

        // The chartSettingsBox holds the various buttons and such for adjusting the charts.
        Wt::WContainerWidget *chartSettingsBox = new Wt::WContainerWidget;

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
        chartSettingsBox->addWidget(new Wt::WLabel("&nbsp;Chart type:"));
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
        absoluteRelative_->setCurrentIndex(1);
        absoluteRelative_->activated().connect(this, &WResultsConstraintsTab::switchAbsoluteRelative);

        // Combo box to choose a baseline
        chartBaselineChoices_ = new WComboBoxWithData<ComboBoxVersion>;
        chartBaselineChoices_->addItem("None");
        fillVersionComboBox(chartBaselineChoices_);
        chartSettingsBox->addWidget(new Wt::WLabel("&nbsp;Baseline:"));
        chartSettingsBox->addWidget(chartBaselineChoices_);

        // Update button to reload data from the database
        Wt::WPushButton *updateButton = new Wt::WPushButton("Update");
        updateButton->setToolTip("Update chart with latest database changes.");
        updateButton->clicked().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        chartSettingsBox->addWidget(updateButton);

        addWidget(chartSettingsBox);
        addWidget(chartStack_);

        //------------------
        // Constraints area
        //------------------

        addWidget(new Wt::WText("<h2>Constraints</h2>"));

        // Constraints
        addWidget(constraints_ = new WConstraints);
        constraints_->constraintsChanged().connect(this, &WResultsConstraintsTab::updateStatusCounts);

        // Button to reset everything to the initial state.
        Wt::WPushButton *reset = new Wt::WPushButton("Clear");
        reset->clicked().connect(this, &WResultsConstraintsTab::resetConstraints);
        addWidget(reset);

        //---------
        // Wiring
        //---------
        majorAxisChoices_->activated().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        minorAxisChoices_->activated().connect(this, &WResultsConstraintsTab::updateStatusCounts);
        chartBaselineChoices_->activated().connect(this, &WResultsConstraintsTab::switchBaselineVersion);
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

    void switchBaselineVersion(int idx) {
        chartModel_->setBaselineVersion(chartBaselineChoices_->currentData().version);
        chartModel_->updateModel(constraints_->dependencies());

        tableModel_->setBaselineVersion(chartBaselineChoices_->currentData().version);
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
    Wt::WText *error_, *commands_, *testOutput_;
    Wt::WTable *humanConfig_, *rmcConfig_;

public:
    explicit WDetails(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), testId_(-1) {

        {
            Wt::WLabel *w = new Wt::WLabel("Details about the configurations selected in the \"Overview\" tab.");
            addWidget(w);
        }

        // Combo box to choose which test to display
        {
            Wt::WContainerWidget *c = new Wt::WContainerWidget;
            c->addWidget(new Wt::WLabel("Configuration "));
            testIdChoices_ = new Wt::WComboBox;
            testIdChoices_->activated().connect(this, &WDetails::selectTestId);
            c->addWidget(testIdChoices_);
            addWidget(c);
        }

        // Error message cached in database test_results.first_error
        {
            addWidget(new Wt::WText("<div><h2>First error</h2></div>"));
            error_ = new Wt::WText;
            error_->setInline(false);
            addWidget(error_);
        }

        // Configuration and detailed status
        {
            addWidget(new Wt::WText("<div><h2>Detailed status</h2></div>"));
            addWidget(new Wt::WText("<p>This list includes configuration and results. Note that the configuration items "
                                    "are the versions requested by the test, but might not be the versions actually used "
                                    "by ROSE due to possible bugs in ROSE's \"configure\" or \"cmake\" system or in the "
                                    "scripts used to run these tests.</p>"));
            Wt::WComboBox *configChoice = new Wt::WComboBox;
            addWidget(configChoice);
            Wt::WStackedWidget *configStack = new Wt::WStackedWidget;
            addWidget(configStack);

            configChoice->addItem("All details");
            configStack->addWidget(humanConfig_ = new Wt::WTable);

            configChoice->addItem("RMC configuration");
            configStack->addWidget(rmcConfig_ = new Wt::WTable);

            configChoice->activated().connect(boost::bind(&WDetails::displayConfig, this, configStack, _1));
        }

        // Commands that were executed
        {
            addWidget(new Wt::WText("<div><h2>Commands executed</h2></div>"));
            commands_ = new Wt::WText;
            commands_->setTextFormat(Wt::XHTMLText);
            commands_->setWordWrap(true);
            commands_->setInline(false);
            addWidget(commands_);
        }

        // Tests final output
        {
            addWidget(new Wt::WText("<div><h2>Command output</h2></div>"));
            testOutput_ = new Wt::WText;
            testOutput_->setTextFormat(Wt::XHTMLText);
            testOutput_->setWordWrap(false);
            testOutput_->setInline(false);
            addWidget(testOutput_);
        }
    }

    void queryTestIds(const Dependencies &deps) {
        std::vector<std::string> args;
        std::string sql = "select test.id, test.status" + sqlFromClause() + sqlWhereClause(deps, args) + " order by id";
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

            // Make sure the combo box shows the correct ID
            std::string pattern = boost::lexical_cast<std::string>(id) + ": ";
            int cbIdx = testIdChoices_->findText(pattern, Wt::MatchStartsWith);
            if (cbIdx >= 0)
                testIdChoices_->setCurrentIndex(cbIdx);
        }
    }
    
    Wt::Signal<>& testIdChanged() {
        return testIdChanged_;
    }

    void updateDetails(const Dependencies &deps) {
        ::mlog[DEBUG] <<"WDetails::updateDetails(testId=" <<testId_ <<")\n";
        StringString rmcCharacteristics = queryRmcCharacteristics();

        // What columns to query?
        DependencyNames columns = gstate.dependencyNames;
        columns.insert("status", "test.status");
        columns.insert("duration", "test.duration");
        columns.insert("noutput", "test.noutput");
        columns.insert("nwarnings", "test.nwarnings");

        std::string sql;
        BOOST_FOREACH (const std::string &colName, columns.values())
            sql += std::string(sql.empty()?"select ":", ") + colName;
        sql += ", coalesce(test.first_error,'')";            // +0
        sql += ", " + gstate.dependencyNames["status"]; // +1

        sql += sqlFromClause();
        std::vector<std::string> args;
        std::string where = sqlWhereClause(deps, args) + " and test.id = ?";
        args.push_back(boost::lexical_cast<std::string>(testId_));
        sql += where;

        // Just in case the test has been removed and thus the query iterator returns no results.
        humanConfig_->clear();
        humanConfig_->elementAt(0, 0)->addWidget(new Wt::WText("Test has been removed from the database."));

        // Iterate over the query. This "for" loop is executed only zero or one time.
        std::string first_error;
        SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
        bindSqlVariables(q, args);
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            int queryColumn = 0, humanConfigRow = 0, rmcConfigRow = 0;

            humanConfig_->clear();
            humanConfig_->columnAt(0)->setWidth(Wt::WLength(10, Wt::WLength::FontEm));
            rmcConfig_->clear();
            rmcConfig_->columnAt(0)->setWidth(Wt::WLength(10, Wt::WLength::FontEm));
            rmcConfig_->elementAt(rmcConfigRow, 0)->addWidget(new Wt::WText("rmc_rosesrc"));
            rmcConfig_->elementAt(rmcConfigRow, 1)->addWidget(new Wt::WText("/path/to/your/ROSE/source/tree"));
            ++rmcConfigRow;
            rmcConfig_->elementAt(rmcConfigRow, 0)->addWidget(new Wt::WText("rmc_parallelism"));
            rmcConfig_->elementAt(rmcConfigRow, 1)->addWidget(new Wt::WText("system"));
            ++rmcConfigRow;
            rmcConfig_->elementAt(rmcConfigRow, 0)->addWidget(new Wt::WText("rmc_code_coverage"));
            rmcConfig_->elementAt(rmcConfigRow, 1)->addWidget(new Wt::WText("no"));
            ++rmcConfigRow;

            // The known columns for the "config"
            BOOST_FOREACH (const std::string &name, columns.keys()) {
                std::string depRawVal = row.get<std::string>(queryColumn++);
                std::string depHumanVal = humanDepValue(name, depRawVal);
                humanConfig_->elementAt(humanConfigRow, 0)->addWidget(new Wt::WText(name));
                humanConfig_->elementAt(humanConfigRow, 1)->addWidget(new Wt::WText(depHumanVal));
                ++humanConfigRow;

                std::string rmcCharacteristic;
                if (rmcCharacteristics.getOptional(name).assignTo(rmcCharacteristic)) {
                    rmcConfig_->elementAt(rmcConfigRow, 0)->addWidget(new Wt::WText(rmcCharacteristic));
                    rmcConfig_->elementAt(rmcConfigRow, 1)->addWidget(new Wt::WText(depRawVal));
                    ++rmcConfigRow;
                }
            }

            // Additional information from the query
            first_error = boost::trim_copy(row.get<std::string>(columns.size()+0));
            std::string status = row.get<std::string>(columns.size()+1);
            if (first_error.empty() && status != "end") {
                error_->setText("<p>No error pattern matched (see output below).  The best way to fix this is to change the "
                                "error message so it begins with the string \"error:\" followed by a space and an error "
                                "message. If that's not possible, send the configuration number (above) and the error "
                                "message (below) to Robb.</p>");
                error_->setWordWrap(true);
            } else if (first_error.empty()) {
                error_->setText("<p>None found.</p>");
            } else {
                first_error = StringUtility::htmlEscape(first_error);
                boost::replace_all(first_error, "\n", "<br/>");
                error_->setText("<div><span class=\"output-error\">" + first_error + "</span></div>");
                error_->setWordWrap(false);
            }
            break;
        }
        updateCommands();
        updateOutput();
    }

private:
    void displayConfig(Wt::WStackedWidget *configStack, int index) {
        configStack->setCurrentIndex(index);
    }

    void selectTestId() {
        int i = testIdChoices_->currentIndex();
        if (i >= 0 && (size_t)i < testIds_.size()) {
            setTestId(testIds_[i]);
        } else {
            setTestId(-1);
        }
    }

    StringString queryRmcCharacteristics() {
        StringString retval;
        SqlDatabase::StatementPtr q = gstate.tx->statement("select distinct name from dependencies");
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            std::string characteristic = row.get<std::string>(0);
            retval.insert(characteristic, "rmc_" + characteristic);
        }
        return retval;
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
            if (s.empty()) {
                s = "Command output was not saved for this test.\n";
                error_->setText("<p>Output was not saved.</p>");
            }

            std::string t = escapeHtml(s);

            // Look for special output lines for errors and warnings so we can highlight them
            boost::regex highlightRegex("(^[^\\n]*?(?:"
                                        // Errors
                                        "\\b(?:error|ERROR):"                           // generic errors
                                        "|\\[(?:ERROR|FATAL) *\\]"                      // Sawyer message streams
                                        "|\\bwhat\\(\\): [^\\n]+\\n[^\\n]*Aborted$"     // fatal exception in shell command
                                        "|\\bwhat\\(\\): [^\\n]+\\n[^\\n]*command died" // fatal exception from $(RTH_RUN)
                                        "|\\[err\\]: terminated after \\d+ seconds"     // timeout from $(RTH_RUN)
                                        "|: Assertion `[^\\n]+' failed\\."              // failed <cassert> assertion
                                        "|: undefined reference to `"                   // GNU linker error

                                        ")[^\\n]*$)|"
                                        "(^[^\\n]*?(?:"

                                        // Warnings
                                        "\\b(?:warning|WARNING):"                       // generic warnings
                                        "|\\[WARN *\\]"                                 // Sawyer message streams

                                        ")[^\\n]*$)|"
                                        "(^={17}-={17}[^\\n]+={17}-={17}$)");

            const char *highlightFormat = "(?1<span class=\"output-error\">$&</span>)"
                                          "(?2<span class=\"output-warning\">$&</span>)"
                                          "(?3<span class=\"output-separator\"><hr/>$&</span>)";

            std::ostringstream out(std::ios::out | std::ios::binary);
            std::ostream_iterator<char, char> oi(out);
            boost::regex_replace(oi, t.begin(), t.end(), highlightRegex, highlightFormat,
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
    Wt::WText *summary_;                                // summary about what's displayed
    Wt::WTable *grid_;
    Wt::Signal<int> testIdChanged_;                     // emitted when user selects a test ID number
public:
    explicit WErrors(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), outOfDate_(true) {

        addWidget(new Wt::WText("<p>These are the most prevalent errors in the failing configurations selected in the "
                                "\"Overview\" tab.  The definition of \"failing\" can be found in the \"Settings\" "
                                "tab. The information below each error is the list of constraints, in addition to "
                                "those in the \"Overview\" tab, which all the errors satisfy. "
                                "<b>Guide for commentary:</b> when commenting, remember that the same error might "
                                "occur in other configurations as well and your comment will apply to them also even "
                                "if they're not shown in this table.</p>"));

        addWidget(summary_ = new Wt::WText);

        addWidget(grid_ = new Wt::WTable);
        grid_->setHeaderCount(1);
    }

    Wt::Signal<int>& testIdChanged() {
        return testIdChanged_;
    }

    void changeConstraints() {
        outOfDate_ = true;
    }

    // Update the error list if it's outdated
    void updateErrorList(const Dependencies &deps) {
        if (!outOfDate_)
            return;
        outOfDate_ = false;

        // Summary
        std::vector<std::string> args;
        SqlDatabase::StatementPtr q0 = gstate.tx->statement("select count(*)" + sqlFromClause() + sqlWhereClause(deps, args));
        bindSqlVariables(q0, args);
        int nTests = q0->execute_int();
        int nFails = 0;
        if (0 == nTests) {
            summary_->setText("<p>No tests match the \"Overview\" constraints.</p>");
            grid_->setHidden(true);
            return;
        } else {
            args.clear();
            q0 = gstate.tx->statement("select count(*)" + sqlFromClause() + sqlWhereClause(deps, args) +
                                      "and " + sqlDependencyExpression(deps, "pass/fail") + " = 'fail'");
            bindSqlVariables(q0, args);
            nFails = q0->execute_int();
            if (0 == nFails) {
                summary_->setText("<p>" + StringUtility::plural(nTests, "tests") + " selected but none failed.</p>");
                grid_->setHidden(true);
                return;
            }

            summary_->setText("<p>" + StringUtility::numberToString(nFails) + " of " + StringUtility::numberToString(nTests) +
                              " selected " + (1 == nFails ? "test fails." : "tests fail") +
                              " (" + StringUtility::numberToString((int)round(100.0*nFails/nTests)) + "%).</p>");
            grid_->setHidden(false);
        }

        // Build the SQL query for finding the errors
        args.clear();
        std::string passFailExpr = sqlDependencyExpression(deps, "pass/fail");
        std::string sql = "select count(*) as n, status, coalesce(first_error,''), " + passFailExpr +
                          sqlFromClause() +
                          sqlWhereClause(deps, args) +
                          " and " + passFailExpr + " = 'fail'"
                          " group by status, coalesce(first_error,''), test_names.position"
                          " order by n desc"
                          " limit 15";
        SqlDatabase::StatementPtr q1 = gstate.tx->statement(sql);
        bindSqlVariables(q1, args);

        // Reset the table
        grid_->clear();
        grid_->elementAt(0, 0)->addWidget(new Wt::WText("Count"));
        grid_->elementAt(0, 1)->addWidget(new Wt::WText("Status"));
        grid_->elementAt(0, 2)->addWidget(new Wt::WText("Error"));
        grid_->columnAt(0)->setWidth(Wt::WLength(4.0, Wt::WLength::FontEm));
        grid_->columnAt(1)->setWidth(Wt::WLength(6.0, Wt::WLength::FontEm));

        // Fill the table
        Sawyer::Container::Map<std::string, std::string> statusCssClass;
        int bigRow = 1;                                 // leave room for the header
        for (SqlDatabase::Statement::iterator iter1 = q1->begin(); iter1 != q1->end(); ++iter1, bigRow+=3) {
            int nErrors = iter1.get<int>(0);
            int errorsPercent = round(100.0*nErrors/nTests);
            std::string status = iter1.get<std::string>(1);
            std::string message = iter1.get<std::string>(2);

            // Error count and failure rate
            grid_->elementAt(bigRow+0, 0)->addWidget(new Wt::WText(StringUtility::numberToString(nErrors) + "<br/>" +
                                                                   StringUtility::numberToString(errorsPercent) + "%"));
            grid_->elementAt(bigRow+0, 0)->setRowSpan(3);
            grid_->elementAt(bigRow+0, 0)->setStyleClass("error-count-cell");

            // Test status for these errors.  An error is always identified by a unique (status,message) pair so that we can
            // distinguish between, for example, the same compiler error message for the ROSE library vs. a test case.
            grid_->elementAt(bigRow+0, 1)->addWidget(new Wt::WText(status));
            grid_->elementAt(bigRow+0, 1)->setRowSpan(3);
            if (!statusCssClass.exists(status))
                statusCssClass.insert(status, "error-status-"+StringUtility::numberToString(statusCssClass.size()%8));
            grid_->elementAt(bigRow+0, 1)->setStyleClass(statusCssClass[status]);

            // Create a combo box of all the test ID's that have this error so we can select an ID and be taken directly to the
            // details for that test.
            Wt::WComboBox *wTestIds = new Wt::WComboBox;
            wTestIds->activated().connect(boost::bind(&WErrors::emitTestIdChanged, this, wTestIds));
            wTestIds->addItem("View details");
            args.clear();
            SqlDatabase::StatementPtr q4 = gstate.tx->statement("select test.id" + sqlFromClause() +
                                                                sqlWhereClause(deps, args) +
                                                                " and test.status = ?"
                                                                " and coalesce(test.first_error,'') = ?"
                                                                " and " + passFailExpr + " = 'fail'"
                                                                " order by test.id");
            args.push_back(status);
            args.push_back(message);
            bindSqlVariables(q4, args);
            for (SqlDatabase::Statement::iterator iter4 = q4->begin(); iter4 != q4->end(); ++iter4)
                wTestIds->addItem(boost::lexical_cast<std::string>(iter4.get<int>(0)));
            grid_->elementAt(bigRow+0, 2)->addWidget(wTestIds);

            // Error message
            grid_->elementAt(bigRow+0, 2)->addWidget(new Wt::WText(message.empty() ? "Undetermined error(s)" : message,
                                                                   Wt::PlainText));
            grid_->elementAt(bigRow+0, 2)->setStyleClass("error-message-cell");

            // Accumulate and show counts for the various configuration characteristics.
            typedef Sawyer::Container::Map<std::string /*depvalue*/, size_t /*count*/> DepValueCounts;
            typedef Sawyer::Container::Map<std::string /*depname*/, DepValueCounts> Characteristics;
            Characteristics characteristics;
            args.clear();
            sql = "select " + boost::join(gstate.dependencyNames.values(), ", ") + ", count(*)" +
                  sqlFromClause() +
                  sqlWhereClause(deps, args) +
                  " and test.status = ?"
                  " and coalesce(test.first_error,'') = ?"
                  " and " + passFailExpr + " = 'fail'"
                  " group by " + boost::join(gstate.dependencyNames.values(), ", ");
            args.push_back(status);
            args.push_back(message);
            SqlDatabase::StatementPtr q2 = gstate.tx->statement(sql);
            bindSqlVariables(q2, args);
            for (SqlDatabase::Statement::iterator iter2 = q2->begin(); iter2 != q2->end(); ++iter2) {
                size_t count = iter2.get<size_t>(gstate.dependencyNames.size());
                int colNumber = 0;
                BOOST_FOREACH (const std::string &depName, gstate.dependencyNames.keys()) {
                    std::string depval = humanDepValue(depName, iter2.get<std::string>(colNumber++), HUMAN_TERSE);
                    characteristics.insertMaybeDefault(depName).insertMaybe(depval, 0) += count;
                }
            }
            BOOST_FOREACH (const Characteristics::Node &characteristic, characteristics.nodes()) {
                const std::string &depname = characteristic.key();
                if (depname == "status" || depname == "pass/fail")
                    continue;
                Wt::WComboBox *combos = new Wt::WComboBox;
                BOOST_FOREACH (const DepValueCounts::Node &valcount, characteristic.value().nodes()) {
                    combos->addItem(depname + " = " + valcount.key() +
                                    " (" + StringUtility::numberToString(valcount.value()) + ")");
                }
                grid_->elementAt(bigRow+1, 2)->addWidget(combos);
            }
            grid_->elementAt(bigRow+1, 2)->setStyleClass("error-dependencies-cell");

            // Is there commentary about this error? Do not allow comments to be added for undetermined errors.
            if (message.empty()) {
                grid_->elementAt(bigRow+2, 2)->addWidget(new Wt::WText("No comment."));
                grid_->elementAt(bigRow+2, 2)->setStyleClass("error-comment-cell");
            } else {
                SqlDatabase::StatementPtr q3 = gstate.tx->statement("select commentary, issue_name from errors"
                                                                    " where status = ? and message = ?");
                q3->bind(0, status);
                q3->bind(1, message);
                do {
                    SqlDatabase::Statement::iterator iter3 = q3->begin();
                    std::string commentary, issueName;
                    if (iter3 != q3->end()) {
                        commentary = iter3.get<std::string>(0);
                        issueName = iter3.get<std::string>(1);
                    }

                    // Link to JIRA. This is where most comments will be kept.
                    Wt::WAnchor *jiraLink = new Wt::WAnchor;
                    jiraLink->setTarget(Wt::TargetNewWindow);
                    if (!issueName.empty()) {
                        jiraLink->setLink(Wt::WLink(issueUrl(issueName)));
                        jiraLink->setText(issueName + " ");
                    } else {
                        jiraLink->setHidden(true);
                    }
                    grid_->elementAt(bigRow+2, 2)->addWidget(jiraLink);

                    // User-defined commentary within the database
                    Wt::WInPlaceEdit *wCommentary = new Wt::WInPlaceEdit(commentary);
                    wCommentary->setPlaceholderText("No comment (click to add).");
                    wCommentary->lineEdit()->setTextSize(80);
                    wCommentary->valueChanged().connect(boost::bind(&WErrors::setComment, this, status, message,
                                                                    wCommentary, jiraLink));
                    grid_->elementAt(bigRow+2, 2)->addWidget(wCommentary);
                    grid_->elementAt(bigRow+2, 2)->setStyleClass("error-comment-cell");
                } while (0);
            }
        }
    }

private:
    // Emit a signal indicating that the user wants to see the details for a particular configuration.
    void emitTestIdChanged(Wt::WComboBox *wTestIds) {
        std::string s = wTestIds->currentText().narrow();
        if (s.empty() || !isdigit(s[0]))
            return;                                     // not a test ID number
        int testId = boost::lexical_cast<int>(s);
        testIdChanged_.emit(testId);
    }

    // URL for issue name
    std::string issueUrl(const std::string &issueName) {
        if (issueName.empty())
            return "";
        return "https://rosecompiler.atlassian.net/browse/" + issueName;
    }

    // Set comment for an error message
    void setComment(const std::string &status, const std::string &message, Wt::WInPlaceEdit *wEdit, Wt::WAnchor *jiraLink) {
        // Avoid doing anything if we're called recursively. This is because this function is called when wEdit is modified,
        // but this function also modifies that value.
        static size_t callDepth = 0;
        struct CallDepthGuard {
            size_t &counter_;
            CallDepthGuard(size_t &counter): counter_(counter) { ++counter; }
            ~CallDepthGuard() {
                ASSERT_require(counter_ > 0);
                --counter_;
            }
        } callDepthGuard(callDepth);
        if (callDepth > 1)
            return;                                     // this is a recursive call

        std::string commentary = wEdit->text().narrow();
        int mtime = time(NULL);
        bool restoreGuiComment = false;

        // We need a temporary transaction since our main transaction will never be committed.
        SqlDatabase::TransactionPtr tx = gstate.tx->connection()->transaction();

        if (commentary.empty() || commentary == "no comment") {
            // If the commentary is empty, then delete any comment that's in the database, but leave the JIRA issue alone if
            // there is one.
            tx->statement("update errors set commentary = '' where status = ? and message = ?")
                ->bind(0, status)
                ->bind(1, message)
                ->execute();
            wEdit->setText("");
        } else if (commentary == "no issue") {
            // Delete the JIRA issue link, but leave the comment alone.
            tx->statement("update errors set issue_name = '' where status = ? and message = ?")
                ->bind(0, status)
                ->bind(1, message)
                ->execute();
            restoreGuiComment = true;
            jiraLink->setHidden(true);
        } else {
            // We're modifying an existing record or inserting a new one. We can't use "insert ... on conflict" because the
            // database might be older than PostgreSQL 9.5.
            bool recordExists = 0 < (tx->statement("select count(*) from errors where status = ? and message = ?")
                                     ->bind(0, status)
                                     ->bind(1, message)
                                     ->execute_int());
            if (boost::regex_match(commentary, boost::regex("[A-Z]+-[0-9]+"))) {
                // Looks like a JIRA issue name, so update the issue and leave the comment alone.
                if (recordExists) {
                    tx->statement("update errors set issue_name = ?, mtime = ? where status = ? and message = ?")
                        ->bind(0, commentary)
                        ->bind(1, mtime)
                        ->bind(2, status)
                        ->bind(3, message)
                        ->execute();
                } else {
                    tx->statement("insert into errors (status, message, issue_name, mtime) values (?, ?, ?, ?)")
                        ->bind(0, status)
                        ->bind(1, message)
                        ->bind(2, commentary)
                        ->bind(3, mtime)
                        ->execute();
                }
                jiraLink->setLink(Wt::WLink(issueUrl(commentary)));
                jiraLink->setText(commentary + " ");
                jiraLink->setHidden(false);
                restoreGuiComment = true;
            } else {
                // Update the commentary
                if (recordExists) {
                    tx->statement("update errors set commentary = ?, mtime = ? where status = ? and message = ?")
                        ->bind(0, commentary)
                        ->bind(1, mtime)
                        ->bind(2, status)
                        ->bind(3, message)
                        ->execute();
                } else {
                    tx->statement("insert into errors (status, message, commentary, mtime) values (?, ?, ?, ?)")
                        ->bind(0, status)
                        ->bind(1, message)
                        ->bind(2, commentary)
                        ->bind(3, mtime)
                        ->execute();
                }
            }
        }

        if (restoreGuiComment) {
            SqlDatabase::StatementPtr q = tx->statement("select commentary from errors where status = ? and message = ?")
                                          ->bind(0, status)
                                          ->bind(1, message);
            SqlDatabase::Statement::iterator iter = q->begin();
            if (iter == q->end()) {
                wEdit->setText("");
            } else {
                wEdit->setText(iter.get<std::string>(0));
            }
        }

        // Cleanup by deleting records that aren't needed
        tx->statement("delete from errors where commentary = '' and issue_name = ''")->execute();
        tx->commit();
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tab to help select a working ROSE configuration

// Extra data attached to combo boxes that show configuration values
struct WorkingConfig {
    size_t nPass;
    std::string display() const {
        return nPass ? boost::lexical_cast<std::string>(nPass) : std::string();
    }
};

class WFindWorkingConfig: public Wt::WContainerWidget {
    std::string roseVersion_;                           // version of ROSE that we're investigating
    unsigned long roseDate_;                            // date of ROSE commit
    std::vector<std::string> depNames_;                 // names of dependencies that we're interested in
    std::vector<std::string> depLabels_;                // label for each dependency
    Dependencies deps_;                                 // dependencies that might be constrained
    bool suppressCountUpdates_;                         // skip updating counts
    std::string passDefinition_;                        // SQL condition defining what it means for ROSE to "pass"

    StatusModel *tableModel_;                           // model for our table of results
    Wt::WTableView *tableView_;                         // table of results

    WComboBoxWithData<ComboBoxVersion> *wVersions_;
    Wt::WTable *wTable_;
    Wt::WText *wSummary_;
    Wt::WText *wPassDefinition_;

public:
    explicit WFindWorkingConfig(Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), roseDate_(0), suppressCountUpdates_(false) {

        wPassDefinition_ = new Wt::WText;
        std::string userPassDef = gstate.tx->statement("select pass_criteria from interface_settings")->execute_string();
        setPassCriteria(userPassDef);

        depNames_.push_back("rose");
        depLabels_.push_back("ROSE version");

        depNames_.push_back("os");
        depLabels_.push_back("Operating system");

        depNames_.push_back("compiler");
        depLabels_.push_back("Compiler");

        depNames_.push_back("boost");
        depLabels_.push_back("Boost version");

        depNames_.push_back("languages");
        depLabels_.push_back("Frontend languages");

        depNames_.push_back("edg");
        depLabels_.push_back("EDG version");

        depNames_.push_back("pass/fail");
        depLabels_.push_back("");                       // used in queries but not shown in interface

        // Build a combo box so we can choose a version of ROSE. The combo box text will be human readable version numbers
        // (short SHA1) and the full version number used in the SQL query is stored in the attached data.  The combo box is not
        // for public consumption, so the app will place it in the "Settings" tab instead, which will be accessible only to
        // logged-in users.
        std::string penultimateVersion = findRoseVersion();
        wVersions_ = new WComboBoxWithData<ComboBoxVersion>;
        int idx = fillVersionComboBox(wVersions_, penultimateVersion);
        if (idx >= 0)
            wVersions_->setCurrentIndex(idx);
        wVersions_->activated().connect(this, &WFindWorkingConfig::selectNewVersion);

        // Build a table showing some results
        addWidget(new Wt::WText("<p>This table shows the number of tests that pass as a percent of the number of tests that "
                                "were run subject to the constraints listed below.  The table is organized so each row is a "
                                "compiler and each column is a boost version since these are the two most sensitive ROSE "
                                "dependencies. Green represents cases where all tested configurations passed, and red "
                                "represents where all failed, with a spectrum of colors between those two extremes. Cells "
                                "that are dark gray indicate that no tests were run, and cells that are partly desaturated "
                                "(i.e., between a bright color and gray) represent configurations where only a few tests "
                                "were performed.</p>"));
        addWidget(wPassDefinition_);
        tableModel_ = new StatusModel;
        tableModel_->setDepMajorIsData(true);
        tableModel_->setRoundToInteger(true);
        tableModel_->setHumanReadable(true);
        tableModel_->setDepMajorName("compiler");
        tableModel_->setDepMinorName("boost");
        tableModel_->setChartValueType(CVT_PASS_RATIO);
        tableView_ = new Wt::WTableView;
        tableView_->setModel(tableModel_);
        tableView_->setAlternatingRowColors(false);     // true interferes with our custom background colors
        tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
        addWidget(tableView_);

        addWidget(new Wt::WText("<p>Choose your configuration below. The numbers in parentheses indicate how many tests "
                                "passed for your chosen configuration and are adjusted as you change the constraints.</p>"));
        addWidget(wTable_ = new Wt::WTable);

        Wt::WPushButton *wClear = new Wt::WPushButton("Clear");
        wClear->clicked().connect(this, &WFindWorkingConfig::clearConstraints);
        addWidget(wClear);

        addWidget(wSummary_ = new Wt::WText);

        setRoseVersion(findRoseVersion());
    }

    // Combo box to choose which version of ROSE to display to users.
    WComboBoxWithData<ComboBoxVersion>* roseVersionChoices() const {
        return wVersions_;
    }

    // Set which version of ROSE we're looking at.
    void setRoseVersion(const std::string &roseVersion) {
        bool changed = roseVersion_ != roseVersion;
        roseVersion_ = roseVersion;
        if (changed) {
            buildTable();
            updateCounts();
        }
    }

    void clearConstraints() {
        bool needUpdate = false;
        suppressCountUpdates_ = true;
        BOOST_FOREACH (Dependency &dep, deps_.values()) {
            if (dep.comboBox) {
                needUpdate = needUpdate || dep.comboBox->currentIndex() != 0;
                dep.comboBox->setCurrentIndex(0);
            }
        }
        suppressCountUpdates_ = false;
        if (needUpdate)
            updateCounts();
        tableModel_->updateModel(deps_);
    }

    void setPassCriteria(const std::string &reachedTestName) {
        int position = gstate.testNameIndex.getOrElse(reachedTestName, END_STATUS_POSITION);
        passDefinition_ = "case"
                          " when test_names.position >= " + StringUtility::numberToString(position) +
                          " then 'pass' else 'fail' end";
        if (reachedTestName == "end") {
            wPassDefinition_->setText("");
        } else {
            wPassDefinition_->setText("<p>A ROSE configuration is considered to have passed if it reaches "
                                      "the \"" + reachedTestName + "\" step of testing.</p>");
        }
    }

    // Find the version of ROSE whose information will be presented and update this object with that info. The version is
    // stored as interface_settings.rose_public_version in the database, but if this setting is empty then use the
    // penultimate version since the last version is probably undergoing testing right now.
    std::string findRoseVersion() {
        std::string version;

        // Look at the interface settings
        SqlDatabase::StatementPtr q = gstate.tx->statement("select rose_public_version"
                                                           " from interface_settings"
                                                           " limit 1");
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
            version = row.get<std::string>(0);

        // Or use the penultimate version
        if (version.empty()) {
            q = gstate.tx->statement("select distinct rose, rose_date"
                                     " from test_results"
                                     " order by rose_date desc"
                                     " offset 1 limit 1");
            for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
                version = row.get<std::string>(0);
        }

        return version;
    }

    // (Re)build the table of combo boxes and initialize them with all possible values for this version of ROSE, including
    // those values that never pass.
    void buildTable() {
        wTable_->clear();
        deps_ = loadDependencyValues(depNames_, "where rose = ?", std::vector<std::string>(1, roseVersion_));
        std::string roseVersionHuman = humanSha1(roseVersion_, HUMAN_TERSE);

        int tableRow = 0;
        for (size_t i = 0; i < depNames_.size(); ++i) {
            // Build a combo box for each dependency
            Dependency &dep = deps_[depNames_[i]];
            dep.comboBox = new DependencyComboBox;
            if (depNames_[i] != "rose")
                dep.comboBox->addItem(WILD_CARD_STR);
            dep.comboBox->activated().connect(this, &WFindWorkingConfig::updateCounts);
            dep.comboBox->setMinimumSize(Wt::WLength(15, Wt::WLength::FontEm), Wt::WLength::Auto);
            std::vector<std::string> humanValues = sortedHumanValues(dep);
            BOOST_FOREACH (const std::string &s, humanValues) {
                dep.comboBox->addItem(s);
                if (depNames_[i] == "rose" && s == roseVersionHuman)
                    dep.comboBox->setCurrentIndex(dep.comboBox->count()-1);
            }

            // Insert table row
            if (!depLabels_[i].empty()) {
                wTable_->elementAt(tableRow, 0)->addWidget(new Wt::WText(depLabels_[i] + "&nbsp;"));
                wTable_->elementAt(tableRow, 1)->addWidget(dep.comboBox);
                ++tableRow;
            }
        }
    }
                
    // Update the counts stored in the combo boxes.
    void updateCounts() {
        if (suppressCountUpdates_)
            return;
        deps_["pass/fail"].sqlExpression = passDefinition_;
        for (size_t i=0; i<depNames_.size(); ++i) {
            Dependency &dep = deps_[depNames_[i]];

            // Count the number of times the dependency value occurs in a passing test for this version of ROSE, and accumulate
            // those counts based on the human-friendly representation of the dependency value. Note that multiple raw
            // dependency values can map to the same human-friendly value.
            Dependencies otherDeps = deps_;
            otherDeps.erase(dep.name);
            std::vector<std::string> args;
            std::string sql = "select " + sqlDependencyExpression(dep, dep.name) + ", count(*)" +
                              sqlFromClause() +
                              sqlWhereClause(otherDeps, args) +
                              "and " + passDefinition_ + " = 'pass' "
                              "group by " + sqlDependencyExpression(dep, dep.name);
            SqlDatabase::StatementPtr q = gstate.tx->statement(sql);
            bindSqlVariables(q, args);
            Sawyer::Container::Map<std::string, size_t> depCounts;
            for (SqlDatabase::Statement::iterator iter = q->begin(); iter != q->end(); ++iter) {
                std::string depHumanValue = humanDepValue(dep.name, iter.get<std::string>(0), HUMAN_TERSE);
                size_t count = iter.get<size_t>(1);
                depCounts.insertMaybeDefault(depHumanValue) += count;
            }
            
            // Update the combo box with new counts, setting things to zero where we didn't find anything.
            ASSERT_not_null(dep.comboBox);
            for (int i=0; i<dep.comboBox->count(); ++i) {
                std::string depName = dep.comboBox->itemBaseText(i).narrow();
                int count = depCounts.getOrElse(depName, 0);
                if (depName == WILD_CARD_STR)
                    count = -1;                         // turn off count display
                dep.comboBox->setItemData(i, DependencyComboBoxData(count));
            }
        }

        // Summarize what was tested.
        std::vector<std::string> args;
        SqlDatabase::StatementPtr q = gstate.tx->statement("select count(*)" + sqlFromClause() +
                                                           sqlWhereClause(deps_, args) +
                                                           "and " + passDefinition_ + " = 'pass'");
        bindSqlVariables(q, args);
        if (int nPass = q->execute_int()) {
            wSummary_->setText("<p>Our automated testing system has found " +
                               StringUtility::plural(nPass, "passing configurations") +
                               " that are similar to your chosen configuration. The ROSE team tests many more configurations "
                               "than what are represented by this simple interface, which is why the table above may have "
                               "cells that are other than zero or 100%</p>");
        } else {
            args.clear();
            q = gstate.tx->statement("select count(*)" + sqlFromClause() + sqlWhereClause(deps_, args));
            bindSqlVariables(q, args);
            if (int nTested = q->execute_int()) {
                wSummary_->setText("<p>Our automated testing system did not find any configurations of ROSE that "
                                   "pass even though it tested " +
                                   StringUtility::plural(nTested, "similar configurations") + ".</p>");
            } else {
                wSummary_->setText("<p>Our automated testing system has not tested any similar configurations for "
                                   "this version of ROSE, so we can't say whether your chosen configuration would work or "
                                   "not.</p>");
            }
        }

        tableModel_->updateModel(deps_);
    }

private:
    void selectNewVersion(int idx) {
        setRoseVersion(wVersions_->currentData().version);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global settings
class WSettings: public Wt::WContainerWidget {
    Session &session_;
    Wt::Signal<> settingsChanged_;
    Wt::WComboBox *passCriteria_, *userPassCriteria_;
    Wt::WPushButton *publishVersionButton_;

public:
    explicit WSettings(Session &session, WFindWorkingConfig *findWorkingConfig, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), session_(session) {


        // What test must be reached in order to qualify ROSE as being usable by end users?
        std::string publicPass = "end";
        SqlDatabase::StatementPtr q = gstate.tx->statement("select pass_criteria from interface_settings limit 1");
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
            publicPass = row.get<std::string>(0);

        // Build combo boxes that have the test names
        passCriteria_ = new Wt::WComboBox;
        userPassCriteria_ = new Wt::WComboBox;
        BOOST_FOREACH (const std::string &testName, gstate.testNames) {
            passCriteria_->addItem(testName);
            userPassCriteria_->addItem(testName);
            if (testName == "end")
                passCriteria_->setCurrentIndex(passCriteria_->count()-1);
            if (testName == publicPass)
                userPassCriteria_->setCurrentIndex(userPassCriteria_->count()-1);
        }

        //--------------------
        // Developer settings
        //--------------------
        addWidget(new Wt::WText("<h1>Developer settings</h1>"));

        addWidget(new Wt::WText("A configuration is defined to have passed if it makes it to the "));
        addWidget(passCriteria_);
        addWidget(new Wt::WText("step, otherwise it is considered to have failed. This rule generates "
                                "the 'pass' or 'fail' values for the \"pass/fail\" property used throughout "
                                "this application except in the public areas (see below).<br/>"));


        //-------------------------
        // Public session settings
        //-------------------------

        addWidget(new Wt::WText("<h1>Settings for public interface</h1>"));
        addWidget(new Wt::WText("<p>The changes you make here will be visible in your own session immediately, and "
                                "if you click the \"Publish\" button they will become the defaults for all new "
                                "public sessions for all users.</p>"));

        addWidget(new Wt::WText("Show ROSE version "));
        addWidget(findWorkingConfig->roseVersionChoices());
        addWidget(new Wt::WText(" in the publicly-visible parts of this application.<br/><br/>"));

        addWidget(new Wt::WText("Assume that ROSE is usable by users if we make it to the "));
        addWidget(userPassCriteria_);
        addWidget(new Wt::WText(" step. This is the pass/fail criteria used in the public interface.<br/><br/>"));

        publishVersionButton_ = new Wt::WPushButton("Publish");
        publishVersionButton_->setToolTip("Pressing this button will make these public settings the default for all "
                                          "subsequent sessions both public and private.");
        addWidget(publishVersionButton_);

        //----------
        // Wiring
        //----------
        passCriteria_->activated().connect(this, &WSettings::updatePassCriteria);
        userPassCriteria_->activated().connect(boost::bind(&WSettings::updateUserPassCriteria, this, findWorkingConfig));
        publishVersionButton_->clicked().connect(boost::bind(&WSettings::publishSettings, this, findWorkingConfig));

        //-----------------
        // Initialize data
        //-----------------
        authenticationEvent();
    }

    Wt::Signal<>& settingsChanged() {
        return settingsChanged_;
    }

    // Invoke this whenever a user logs in or out
    void authenticationEvent() {
        publishVersionButton_->setEnabled(session_.isPublisher(session_.currentUser()));
    }

private:
    void updatePassCriteria() {
        setPassDefinition(passCriteria_->currentText().narrow());
        settingsChanged_.emit();
    }

    void updateUserPassCriteria(WFindWorkingConfig *findWorkingConfig) {
        findWorkingConfig->setPassCriteria(userPassCriteria_->currentText().narrow());
        findWorkingConfig->updateCounts();
    }

    void publishSettings(WFindWorkingConfig *findWorkingConfig) {
        // We need a temporary transaction so we can commit the changes.
        SqlDatabase::TransactionPtr tx = gstate.tx->connection()->transaction();

        std::string publicVersion = findWorkingConfig->roseVersionChoices()->currentData().version;
        std::string passCriteria = userPassCriteria_->currentText().narrow();

        tx->statement("update interface_settings set rose_public_version = ?, pass_criteria = ?")
            ->bind(0, publicVersion)
            ->bind(1, passCriteria)
            ->execute();

        tx->commit();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// User-management classes
//
// On first run, the administrator account is initialized and the administrator is logged in.  The administrator can create
// additional accounts that are given random passwords. No email is sent because (1) sending passwords and password reset links
// by unencrypted email is unsafe, (2) the system running the web server might not have email capability.

// Used to validate that two user-entered passwords match
class ExactMatchValidator: public Wt::WValidator {
    Wt::WString toMatch_;
public:
    explicit ExactMatchValidator(Wt::WObject *parent = NULL)
        : Wt::WValidator(parent) {}

    virtual Result validate(const Wt::WString &input) const ROSE_OVERRIDE {
        return Wt::WValidator::Result(!input.empty() && input == toMatch_ ? Wt::WValidator::Valid : Wt::WValidator::Invalid);
    }

    void setTargetString(const Wt::WString &toMatch) {
        toMatch_ = toMatch;
    }
};

// Creates and registers a new user.  This is generally run only by an administrator.
class WUserEdit: public Wt::WContainerWidget {
public:
    enum UserItem { FullName, LoginName, Email, Password1, Password2, Publisher, Administrator };

private:
    Session &session_;
    Wt::WTable *table_;
    Wt::WLineEdit *wFullName_, *wLoginName_, *wEmail_, *wPassword1_, *wPassword2_;
    Wt::WText *wLoginNameError_, *wPasswordStrength_, *wPasswordsMatch_;
    Wt::WPushButton *wCreate_, *wUpdate_;
    Wt::Auth::PasswordStrengthValidator *pwStrengthValidator_;
    ExactMatchValidator *pwSameValidator_;
    Wt::WCheckBox *wIsPublisher_, *wIsAdministrator_;
    Wt::Signal<Wt::Auth::User> userCreated_, userEdited_;
    Wt::Auth::User modifyingUser_;                      // user begin modified; invalid if creating user

    std::string loginNameChecked_;                      // a login name that was checked for existence
    bool loginNameExists_;                              // whether that login name exists
    bool allowAuthorizationEdits_;

public:
    explicit WUserEdit(Session &session, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), session_(session), loginNameExists_(false), allowAuthorizationEdits_(false) {
        table_ = new Wt::WTable;
        addWidget(table_);

        std::string singleNameRe = "(([A-Z][a-zA-Z]*-)*[A-Z][a-zA-Z]*\\.?)"; // "Billy", "Billy-Bob", "Jr." and similar
        std::string parenNameRe = "(" + singleNameRe + "|\\(" + singleNameRe + "\\))"; // parenthesized nick name
        std::string multiNameRe = "(" + parenNameRe + "(,? " + parenNameRe + ")+)"; // "Smokey T. Bear"
        
        table_->elementAt(FullName, 0)->addWidget(new Wt::WLabel("Full name:"));
        wFullName_ = new Wt::WLineEdit;
        wFullName_->setTextSize(64);
        wFullName_->setMaxLength(64);
        Wt::WRegExpValidator *fullNameValidator = new Wt::WRegExpValidator(multiNameRe);
        fullNameValidator->setMandatory(true);
        wFullName_->setValidator(fullNameValidator);
        table_->elementAt(FullName, 1)->addWidget(wFullName_);

        table_->elementAt(LoginName, 0)->addWidget(new Wt::WLabel("Login name:"));
        wLoginName_ = new Wt::WLineEdit;
        wLoginName_->setTextSize(12);
        wLoginName_->setMaxLength(12);
        Wt::WRegExpValidator *loginNameValidator = new Wt::WRegExpValidator("[a-z][a-z0-9]{2,11}");
        loginNameValidator->setMandatory(true);
        wLoginName_->setValidator(loginNameValidator);
        table_->elementAt(LoginName, 1)->addWidget(wLoginName_);
        table_->elementAt(LoginName, 1)->addWidget(wLoginNameError_ = new Wt::WText);

        table_->elementAt(Email, 0)->addWidget(new Wt::WLabel("Email contact:"));
        wEmail_ = new Wt::WLineEdit;
        wEmail_->setTextSize(64);
        wEmail_->setMaxLength(64);
        Wt::WRegExpValidator *emailValidator =
            new Wt::WRegExpValidator("[a-zA-Z0-9._%+-]+@([a-zA-Z0-9.-]+\\.){1,2}[a-zA-Z]{2,4}");
        emailValidator->setMandatory(true);
        wEmail_->setValidator(emailValidator);
        table_->elementAt(Email, 1)->addWidget(wEmail_);

        table_->elementAt(Password1, 0)->addWidget(new Wt::WLabel("Password:"));
        wPassword1_ = new Wt::WLineEdit;
        wPassword1_->setTextSize(16);
        wPassword1_->setMaxLength(32);
        wPassword1_->setEchoMode(Wt::WLineEdit::Password);
        wPassword1_->setValidator(pwStrengthValidator_ = new Wt::Auth::PasswordStrengthValidator);
        table_->elementAt(Password1, 1)->addWidget(wPassword1_);
        table_->elementAt(Password1, 1)->addWidget(wPasswordStrength_ = new Wt::WText);
        
        table_->elementAt(Password2, 0)->addWidget(new Wt::WLabel("Password:"));
        wPassword2_ = new Wt::WLineEdit;
        wPassword2_->setTextSize(16);
        wPassword2_->setMaxLength(32);
        wPassword2_->setEchoMode(Wt::WLineEdit::Password);
        pwSameValidator_ = new ExactMatchValidator;
        pwSameValidator_->setMandatory(true);
        wPassword2_->setValidator(pwSameValidator_);
        table_->elementAt(Password2, 1)->addWidget(wPassword2_);
        table_->elementAt(Password2, 1)->addWidget(wPasswordsMatch_ = new Wt::WText);

        table_->elementAt(Publisher, 0)->addWidget(new Wt::WLabel("Publisher:"));
        table_->elementAt(Publisher, 1)->addWidget(wIsPublisher_ = new Wt::WCheckBox("Can user configure public interface?"));

        table_->elementAt(Administrator, 0)->addWidget(new Wt::WLabel("Administrator:"));
        wIsAdministrator_ = new Wt::WCheckBox("Does user have admin privileges?");
        table_->elementAt(Administrator, 1)->addWidget(wIsAdministrator_);

        wCreate_ = new Wt::WPushButton("Create account");
        wCreate_->setEnabled(false);
        addWidget(wCreate_);

        wUpdate_ = new Wt::WPushButton("Update account");
        wUpdate_->setEnabled(false);
        wUpdate_->setHidden(true);
        addWidget(wUpdate_);

        // Wiring
        wFullName_->textInput().connect(this, &WUserEdit::enableDisableSubmit);

        wLoginName_->textInput().connect(this, &WUserEdit::enableDisableSubmit);
        wLoginName_->textInput().connect(this, &WUserEdit::updateLoginExistsError);

        wEmail_->textInput().connect(this, &WUserEdit::enableDisableSubmit);

        wPassword1_->textInput().connect(this, &WUserEdit::enableDisableSubmit);
        wPassword2_->textInput().connect(this, &WUserEdit::enableDisableSubmit);
        wPassword1_->textInput().connect(this, &WUserEdit::updatePasswordStrength);
        wPassword1_->textInput().connect(this, &WUserEdit::updatePasswordsMatch);
        wPassword2_->textInput().connect(this, &WUserEdit::updatePasswordsMatch);

        wCreate_->clicked().connect(this, &WUserEdit::createAccount);
        wUpdate_->clicked().connect(this, &WUserEdit::updateAccount);

        // Initialize data
        setUser(Wt::Auth::User());
        allowAuthorizationEdits(false);
        enableDisableSubmit();
    }

    // Hide or show a certain user information
    void hideRow(UserItem row, bool doHide = true) {
        if (doHide) {
            table_->rowAt(row)->hide();
        } else {
            table_->rowAt(row)->show();
        }
    }

    // Make the user an administrator no matter what.
    void makeAdministrator(bool b = true) {
        hideRow(Administrator);
        wIsAdministrator_->setCheckState(b ? Wt::Checked : Wt::Unchecked);
    }

    // Make sure the user is a publisher no matter what.
    void makePublisher(bool b = true) {
        hideRow(Publisher);
        wIsPublisher_->setCheckState(b ? Wt::Checked : Wt::Unchecked);
    }

    // Set up widget to modify a user. If the usr is invalid then we're creating a user instead.
    void setUser(const Wt::Auth::User &authUser) {
        modifyingUser_ = authUser;
        if (authUser.isValid()) {
            wFullName_->setText(session_.fullName(authUser));
            wLoginName_->setText(session_.loginName(authUser));
            wLoginName_->setEnabled(false);             // login name cannot be changed this way
            wEmail_->setText(session_.email(authUser));
            wPassword1_->setText("");
            wPassword2_->setText("");
            wIsPublisher_->setCheckState(session_.isPublisher(authUser) ? Wt::Checked : Wt::Unchecked);
            wIsAdministrator_->setCheckState(session_.isAdministrator(authUser) ? Wt::Checked : Wt::Unchecked);
            wIsAdministrator_->setEnabled(allowAuthorizationEdits_ && authUser != session_.currentUser());
            wUpdate_->setHidden(false);                 // we are editing an existing account...
            wCreate_->setHidden(true);                  // ...not creating a new account
        } else {
            wFullName_->setText("");
            wLoginName_->setText("");
            wLoginName_->setEnabled(true);
            wEmail_->setText("");
            wPassword1_->setText("");
            wPassword2_->setText("");
            wIsPublisher_->setCheckState(Wt::Unchecked);
            wIsAdministrator_->setCheckState(Wt::Unchecked);
            wIsAdministrator_->setEnabled(allowAuthorizationEdits_);
            wCreate_->setHidden(false);                 // we are creating a new account,...
            wUpdate_->setHidden(true);                  // ...not editing an existing account.
        }
        enableDisableSubmit();
    }

    void allowAuthorizationEdits(bool b) {
        allowAuthorizationEdits_ = b;
        wIsPublisher_->setEnabled(b);
        wIsAdministrator_->setEnabled(b && (!modifyingUser_.isValid() || modifyingUser_ != session_.currentUser()));
    }

    Wt::Signal<Wt::Auth::User>& userCreated() {
        return userCreated_;
    }

    Wt::Signal<Wt::Auth::User>& userEdited() {
        return userEdited_;
    }

private:
    void updateLoginExistsError() {
        if (modifyingUser_.isValid()) {
            wLoginNameError_->setText("");              // we expect the name to exist when we're modifying a user!
        } else {
            if (loginNameExists(wLoginName_->text().narrow())) {
                wLoginNameError_->setText(" Name already exists");
            } else {
                wLoginNameError_->setText("");
            }
        }
    }

    // Check whether the login name exists. This is quite expensive, so cache the result.
    bool loginNameExists(const std::string &loginName) {
        if (loginName != loginNameChecked_) {
            loginNameChecked_ = loginName;
            Wt::Auth::User user = session_.findLogin(loginName);
            if (user.isValid()) {
                loginNameExists_ = true;
            } else {
                loginNameExists_ = false;
            }
        }
        return loginNameExists_;
    }


    // Enable or disable the button to actually create the user.
    void enableDisableSubmit() {
        bool isValid = wFullName_->validate() == Wt::WValidator::Valid &&
                       wLoginName_->validate() == Wt::WValidator::Valid &&
                       wEmail_->validate() == Wt::WValidator::Valid;

        if (modifyingUser_.isValid()) {
            // We're modifying an existing user. Empty password is okay and means don't change the password.
            isValid = isValid &&
                      ((wPassword1_->text().empty() && wPassword2_->text().empty()) ||
                       (wPassword1_->validate() == Wt::WValidator::Valid &&
                        wPassword2_->validate() == Wt::WValidator::Valid &&
                        wPassword1_->text() == wPassword2_->text()));
            wUpdate_->setEnabled(isValid);
        } else {
            // We're creating a new user.
            isValid = isValid &&
                      !loginNameExists(wLoginName_->text().narrow()) &&
                      wPassword1_->validate() == Wt::WValidator::Valid &&
                      wPassword2_->validate() == Wt::WValidator::Valid &&
                      wPassword1_->text() == wPassword2_->text();
            wCreate_->setEnabled(isValid);
        }
    }

    // Update the text about how good the password is.
    void updatePasswordStrength() {
        Wt::Auth::AbstractPasswordService::StrengthValidatorResult result =
            pwStrengthValidator_->evaluateStrength(wPassword1_->text(), wLoginName_->text(), wEmail_->text().narrow());
        wPasswordStrength_->setText(result.message());

        pwSameValidator_ = new ExactMatchValidator;
        pwSameValidator_->setMandatory(true);
        pwSameValidator_->setTargetString(wPassword1_->text());
        wPassword2_->setValidator(pwSameValidator_);
    }

    // Update text about whether passwords match.
    void updatePasswordsMatch() {
        Wt::WString s1 = wPassword1_->text();
        Wt::WString s2 = wPassword2_->text();

        if (s1.empty() || s2.empty()) {
            wPasswordsMatch_->setText("");
        } else if (s1 == s2) {
            wPasswordsMatch_->setText(" Match");
        } else {
            wPasswordsMatch_->setText(" Passwords do not match");
        }
    }

    void createAccount() {
        std::string fullName = wFullName_->text().narrow();
        std::string loginName = wLoginName_->text().narrow();
        std::string email = wEmail_->text().narrow();
        std::string passwd = wPassword1_->text().narrow();
        bool isAdministrator = wIsAdministrator_->checkState() == Wt::Checked;
        bool isPublisher = wIsPublisher_->checkState() == Wt::Checked;

        mlog[INFO] <<"creating user account \"" <<loginName <<"\" for " <<fullName <<" <" <<email <<">\n";
        Wt::Auth::User authUser = session_.createUser(fullName, loginName, email, passwd, isAdministrator, isPublisher);
        session_.setPwChangeRequired(authUser);
        setUser(authUser);
        userCreated_.emit(authUser);
    }

    void updateAccount() {
        ASSERT_require(modifyingUser_.isValid());
        std::string fullName = wFullName_->text().narrow();
        std::string loginName = wLoginName_->text().narrow();
        std::string email = wEmail_->text().narrow();
        std::string passwd = wPassword1_->text().narrow();
        bool isAdministrator = wIsAdministrator_->checkState() == Wt::Checked;
        bool isPublisher = wIsPublisher_->checkState() == Wt::Checked;

        mlog[INFO] <<"updating user account \"" <<loginName <<"\" for " <<fullName <<" <" <<email <<">\n";
        Wt::Auth::User authUser = session_.updateUser(modifyingUser_, fullName, email, passwd, isAdministrator, isPublisher);

        // Make sure we have the latest info for the user.
        setUser(authUser);
        userEdited_.emit(authUser);
    }
};

    

// First-run creation of the administrator account. Creates the administrator account and then logs in as the administrator.
class WCreateAdminAccount: public Wt::WContainerWidget {
    WUserEdit *createUser_;
    Session &session_;                                  // database session for user authentication

public:
    explicit WCreateAdminAccount(Session &session, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), session_(session) {
        addWidget(new Wt::WText("<h1>Create administrator account</h1>"));
        addWidget(new Wt::WText("<p>This page sets up the administrator account since the user database tables are empty.</p>"));
        addWidget(createUser_ = new WUserEdit(session));
        createUser_->makeAdministrator();
        createUser_->makePublisher();
        createUser_->allowAuthorizationEdits(true);

        createUser_->userCreated().connect(this, &WCreateAdminAccount::login);
    }

private:
    void login(const Wt::Auth::User &user) {
        session_.setPwChangeRequired(user, false);
        session_.login(user);
    }
};

// Widget to force a password change for the logged-in user.
class WChangePassword: public Wt::WContainerWidget {
    Session &session_;
    Wt::WText *wUserName_, *wPasswordStrength_, *wPasswordsMatch_;
    Wt::WTable *table_;
    Wt::WLineEdit *wPassword1_, *wPassword2_;
    Wt::Auth::PasswordStrengthValidator *pwStrengthValidator_;
    ExactMatchValidator *pwSameValidator_;
    Wt::WPushButton *wSubmit_;
    Wt::Auth::User modifyingUser_;
    Wt::Signal<Wt::Auth::User> passwordChanged_;
public:
    explicit WChangePassword(Session &session, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), session_(session) {
        addWidget(new Wt::WText("<h2>Change password</h2>"));
        addWidget(wUserName_ = new Wt::WText);

        table_ = new Wt::WTable(this);

        table_->elementAt(0, 0)->addWidget(new Wt::WLabel("Password:"));
        wPassword1_ = new Wt::WLineEdit;
        wPassword1_->setTextSize(16);
        wPassword1_->setMaxLength(32);
        wPassword1_->setEchoMode(Wt::WLineEdit::Password);
        wPassword1_->setValidator(pwStrengthValidator_ = new Wt::Auth::PasswordStrengthValidator);
        table_->elementAt(0, 1)->addWidget(wPassword1_);
        table_->elementAt(0, 1)->addWidget(wPasswordStrength_ = new Wt::WText);
        
        table_->elementAt(1, 0)->addWidget(new Wt::WLabel("Password:"));
        wPassword2_ = new Wt::WLineEdit;
        wPassword2_->setTextSize(16);
        wPassword2_->setMaxLength(32);
        wPassword2_->setEchoMode(Wt::WLineEdit::Password);
        pwSameValidator_ = new ExactMatchValidator;
        pwSameValidator_->setMandatory(true);
        wPassword2_->setValidator(pwSameValidator_);
        table_->elementAt(1, 1)->addWidget(wPassword2_);
        table_->elementAt(1, 1)->addWidget(wPasswordsMatch_ = new Wt::WText);

        addWidget(wSubmit_ = new Wt::WPushButton("Change"));

        wPassword1_->textInput().connect(this, &WChangePassword::enableDisableSubmit);
        wPassword2_->textInput().connect(this, &WChangePassword::enableDisableSubmit);
        wPassword1_->textInput().connect(this, &WChangePassword::updatePasswordStrength);
        wPassword1_->textInput().connect(this, &WChangePassword::updatePasswordsMatch);
        wPassword2_->textInput().connect(this, &WChangePassword::updatePasswordsMatch);
        wSubmit_->clicked().connect(this, &WChangePassword::changePassword);
    }

    void setUser(const Wt::Auth::User &user) {
        modifyingUser_ = user;
        if (user.isValid()) {
            wUserName_->setText("<p>" + session_.fullName(user) + " (" + session_.loginName(user) + ")"
                                ", your password has expired. Please change it now.</p>");
        } else {
            wUserName_->setText("No user");
        }
        wPassword1_->setText("");
        wPassword2_->setText("");
        enableDisableSubmit();
    }

    // Call this when a user logs in or out
    void authenticationEvent() {
        setUser(session_.currentUser());
    }

    Wt::Signal<Wt::Auth::User>& passwordChanged() {
        return passwordChanged_;
    }

private:
    void enableDisableSubmit() {
        std::string password = wPassword1_->text().narrow();
        wSubmit_->setEnabled(modifyingUser_.isValid() &&
                             !wPassword1_->text().empty() &&
                             wPassword1_->text() == wPassword2_->text() &&
                             wPassword1_->validate() == Wt::WValidator::Valid &&
                             wPassword2_->validate() == Wt::WValidator::Valid &&
                             session_.verifyPassword(modifyingUser_, password) != Wt::Auth::PasswordValid);
    }

    // Update the text about how good the password is.
    void updatePasswordStrength() {
        if (modifyingUser_.isValid()) {
            Wt::Auth::AbstractPasswordService::StrengthValidatorResult result =
                pwStrengthValidator_->evaluateStrength(wPassword1_->text(), session_.loginName(modifyingUser_),
                                                       session_.email(modifyingUser_));
            wPasswordStrength_->setText(result.message());

            pwSameValidator_ = new ExactMatchValidator;
            pwSameValidator_->setMandatory(true);
            pwSameValidator_->setTargetString(wPassword1_->text());
            wPassword2_->setValidator(pwSameValidator_);
        }
    }

    // Update text about whether passwords match.
    void updatePasswordsMatch() {
        std::string s1 = wPassword1_->text().narrow();
        std::string s2 = wPassword2_->text().narrow();

        if (s1.empty() || s2.empty()) {
            wPasswordsMatch_->setText("");
        } else if (s1 == s2) {
            if (session_.verifyPassword(modifyingUser_, s1) == Wt::Auth::PasswordValid) {
                wPasswordsMatch_->setText(" Old password");
            } else {
                wPasswordsMatch_->setText(" Match");
            }
        } else {
            wPasswordsMatch_->setText(" Passwords do not match");
        }
    }

    // Change a user's password
    void changePassword() {
        std::string password = wPassword1_->text().narrow();
        session_.setPassword(modifyingUser_, password);
        passwordChanged_.emit(modifyingUser_);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Developer's tab
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Data (the user) for a combo box that holds user names.
class UserComboBoxData {
    Wt::Auth::User user_;

public:
    UserComboBoxData() {}

    explicit UserComboBoxData(const Wt::Auth::User &user)
        : user_(user) {}

    std::string display() const {
        return "";
    }

    const Wt::Auth::User& user() const {
        return user_;
    }

    bool operator==(const UserComboBoxData &other) {
        return user_ == other.user_;                    // same identity and same database?
    }
};

typedef WComboBoxWithData<UserComboBoxData> UserComboBox;
    
// Top-level tab for doing various user things.
class WDevelopersTab: public Wt::WContainerWidget {
    Session &session_;
    Wt::Auth::AuthWidget *wAuthentication_;
    Wt::WStackedWidget *wStack_;
    Wt::WContainerWidget *wUsers_;
    UserComboBox *wUserChoices_;
    WUserEdit *wUserEdit_;
    WChangePassword *wChangePassword_;
    Wt::Signal<> developerActionFinished_;              // used for important developer actions, like password changes

public:
    explicit WDevelopersTab(Session &session, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), session_(session) {

        // User authentication: login/logout. In the logged-out state we show only the AuthWidget which is showing a login
        // form. In the logged in state we show the AuthWidget's logout button along with the stack widget.
        wAuthentication_ = new Wt::Auth::AuthWidget(session_.authenticationService(), session_.userDatabase(),
                                                    session_.loginService());
        wAuthentication_->model()->addPasswordAuth(&session_.passwordService());
        wAuthentication_->setRegistrationEnabled(false);
        wAuthentication_->processEnvironment();
        addWidget(wAuthentication_);

        // The stack of additional widgets.
        addWidget(wStack_ = new Wt::WStackedWidget);

        //---------------------------
        // Force password change
        //---------------------------
        wStack_->addWidget(wChangePassword_ = new WChangePassword(session_));
        wChangePassword_->passwordChanged().connect(this, &WDevelopersTab::finishedPwChange);

        //---------------------------
        // Editing user information
        //---------------------------

        {
            wStack_->addWidget(wUsers_ = new Wt::WContainerWidget);

            wUsers_->addWidget(new Wt::WText("<h2>User Info</h2>"));
            repopulateUserComboBox();
            wUsers_->addWidget(wUserChoices_);
            wUsers_->addWidget(wUserEdit_ = new WUserEdit(session_));

            // Wiring
            wUserChoices_->activated().connect(this, &WDevelopersTab::setUserEdit);
            wUserEdit_->userCreated().connect(this, &WDevelopersTab::userNameMaybeChanged);
            wUserEdit_->userEdited().connect(this, &WDevelopersTab::userNameMaybeChanged);

            // Initialize data
            authenticationEvent();
        }
    }

    // Signal emitted when an important developer action has been completed, such as a mandatory password change.
    Wt::Signal<>& developerActionFinished() {
        return developerActionFinished_;
    }

    // Invoke this whenever a user logs in or out
    void authenticationEvent() {
        if (session_.isAdministrator(session_.currentUser())) {
            wUserChoices_->setHidden(false);
            wUserEdit_->allowAuthorizationEdits(true);
            wStack_->setHidden(false);
            wChangePassword_->setUser(session_.currentUser());
        } else if (session_.currentUser().isValid()) {
            wUserChoices_->setHidden(true);
            wUserEdit_->allowAuthorizationEdits(false);
            wStack_->setHidden(false);
            wChangePassword_->setUser(session_.currentUser());
        } else {
            wStack_->setHidden(true);
        }

        // If a user is logged in, then make that user the one being edited and make sure the user combo box is right.
        Wt::Auth::User currentUser = session_.currentUser();
        if (currentUser.isValid()) {
            int idx = std::max(0, wUserChoices_->findData(UserComboBoxData(currentUser)));
            wUserChoices_->setCurrentIndex(idx);
            setUserEdit();
        }

        // If the user needs to change his password then show the password change dialog.
        if (session_.pwChangeRequired(session_.currentUser())) {
            wStack_->setCurrentWidget(wChangePassword_);
        } else {
            wStack_->setCurrentWidget(wUsers_);
        }
    }

private:
    // Change which user information is being edited.
    void setUserEdit() {
        wUserEdit_->setUser(wUserChoices_->currentData().user());
    }

    // Call this if user names might have changed due to editing or creating a user.
    void userNameMaybeChanged(const Wt::Auth::User &user) {
        Wt::Auth::User oldUser = wUserChoices_->currentData().user();
        repopulateUserComboBox();
        int idx = std::max(0, wUserChoices_->findData(UserComboBoxData(oldUser)));
        wUserChoices_->setCurrentIndex(idx);
        setUserEdit();
    }
    
    // (Re)build the user combo box.
    void repopulateUserComboBox() {
        if (wUserChoices_) {
            wUserChoices_->clear();
        } else {
            wUserChoices_ = new UserComboBox(wUsers_);
        }
        
        wUserChoices_->addItem("New user");
        SqlDatabase::StatementPtr q = gstate.tx->statement("select ident.identity, u.full_name"
                                                           " from auth_identities as ident"
                                                           " join auth_info as info on ident.auth_info_id = info.id"
                                                           " join auth_users as u on info.user_id = u.id"
                                                           " order by u.full_name");
        for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
            std::string loginName = row.get<std::string>(0);
            std::string realName = row.get<std::string>(1);
            Wt::Auth::User user = session_.findLogin(loginName);
            ASSERT_require(user.isValid());
            wUserChoices_->addItem(realName + " (" + loginName + ")", UserComboBoxData(user));
        }
    }

    // Called when a required password change is completed.
    void finishedPwChange(const Wt::Auth::User&) {
        wStack_->setCurrentWidget(wUsers_);
        developerActionFinished_.emit();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// One application object is created per user session.
class WApplication: public Wt::WApplication {
    WFindWorkingConfig *findWorkingConfig_;
    WResultsConstraintsTab *resultsConstraints_;
    WDetails *details_;
    WErrors *errors_;
    WSettings *settings_;
    WDevelopersTab *developers_;
    Wt::WTabWidget *tabs_;
    Session session_;
    WCreateAdminAccount *setup_;
    static const bool HIDE = true;
    static const bool SHOW = false;

public:
    explicit WApplication(const Wt::WEnvironment &env)
        : Wt::WApplication(env), session_(gstate.dbUrl) {
        setTitle("ROSE testing matrix");
        Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
        root()->setLayout(vbox);

        // Styles for constraints
        styleSheet().addRule(".constraint-name", "text-align:right;");
        styleSheet().addRule(".constraint-value", "text-align:left;");

        // Styles for command output
        styleSheet().addRule(".output-error",   "color:#680000; background-color:#ffc0c0;"); // reds
        styleSheet().addRule(".output-warning", "color:#8f4000; background-color:#ffe0c7;"); // oranges
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
        styleSheet().addRule(".error-comment-cell", "border:1px solid black;");
        styleSheet().addRule(".error-message-cell", "border:1px solid black; color:#680000; background-color:#ffc0c0;");
        styleSheet().addRule(".error-status-0", "border:1px solid black; background-color:#d0aae0;");// light purple
        styleSheet().addRule(".error-status-1", "border:1px solid black; background-color:#e1c2ba;");// light tomato
        styleSheet().addRule(".error-status-2", "border:1px solid black; background-color:#aed5df;");// light cyan
        styleSheet().addRule(".error-status-3", "border:1px solid black; background-color:#dfb9cd;");// light pink
        styleSheet().addRule(".error-status-4", "border:1px solid black; background-color:#dfd5b8;");// light ochre
        styleSheet().addRule(".error-status-5", "border:1px solid black; background-color:#bbc4df;");// light blue
        styleSheet().addRule(".error-status-6", "border:1px solid black; background-color:#edc7d5;");// light rose
        styleSheet().addRule(".error-status-7", "border:1px solid black; background-color:#bebadf;");// light purple

        // Main application
        tabs_ = new Wt::WTabWidget();
        vbox->addWidget(tabs_);
        mlog[INFO] <<"creating tab: Public view\n";
        tabs_->addTab(findWorkingConfig_ = new WFindWorkingConfig, "Public");
        mlog[INFO] <<"creating tab: Overview\n";
        tabs_->addTab(resultsConstraints_ = new WResultsConstraintsTab, "Overview");
        mlog[INFO] <<"creating tab: Details\n";
        tabs_->addTab(details_ = new WDetails, "Details");
        mlog[INFO] <<"creating tab: Errors\n";
        tabs_->addTab(errors_ = new WErrors, "Errors");
        mlog[INFO] <<"creating tab: Settings\n";
        tabs_->addTab(settings_ = new WSettings(session_, findWorkingConfig_), "Settings");
        mlog[INFO] <<"creating tab: Developers\n";
        tabs_->addTab(developers_ = new WDevelopersTab(session_), "Developers");
        mlog[INFO] <<"creating tab: Setup\n";
        tabs_->addTab(setup_ = new WCreateAdminAccount(session_), "Setup");

        // Wiring
        session_.loginService().changed().connect(this, &WApplication::authenticationEvent);
        resultsConstraints_->constraints()->constraintsChanged().connect(this, &WApplication::getMatchingTests);
        details_->testIdChanged().connect(this, &WApplication::updateDetails);
        errors_->testIdChanged().connect(this, &WApplication::showTestDetails);
        settings_->settingsChanged().connect(this, &WApplication::updateAll);
        developers_->developerActionFinished().connect(this, &WApplication::authenticationEvent);
        tabs_->currentChanged().connect(this, &WApplication::switchTabs);
        getMatchingTests();

        authenticationEvent();
    }

private:
    void authenticationEvent() {
        if (gstate.tx->statement("select count(*) from auth_info")->execute_int() == 0) {
            showSetupView();
        } else if (session_.pwChangeRequired(session_.currentUser())) {
            showDeveloperActionView();
        } else if (session_.currentUser().isValid()) {
            showLoggedInView();
        } else {
            showLoggedOutView();
        }
        settings_->authenticationEvent();
        developers_->authenticationEvent();
    }

    void showSetupView() {
        tabs_->setTabHidden(tabs_->indexOf(findWorkingConfig_),         HIDE);
        tabs_->setTabHidden(tabs_->indexOf(resultsConstraints_),        HIDE);
        tabs_->setTabHidden(tabs_->indexOf(details_),                   HIDE);
        tabs_->setTabHidden(tabs_->indexOf(errors_),                    HIDE);
        tabs_->setTabHidden(tabs_->indexOf(settings_),                  HIDE);
        tabs_->setTabHidden(tabs_->indexOf(developers_),                HIDE);
        tabs_->setTabHidden(tabs_->indexOf(setup_),                     SHOW);
        tabs_->setCurrentIndex(tabs_->indexOf(setup_));
    }

    void showLoggedInView() {
        tabs_->setTabHidden(tabs_->indexOf(findWorkingConfig_),         SHOW);
        tabs_->setTabHidden(tabs_->indexOf(resultsConstraints_),        SHOW);
        tabs_->setTabHidden(tabs_->indexOf(details_),                   SHOW);
        tabs_->setTabHidden(tabs_->indexOf(errors_),                    SHOW);
        tabs_->setTabHidden(tabs_->indexOf(settings_),                  SHOW);
        tabs_->setTabHidden(tabs_->indexOf(developers_),                SHOW);
        tabs_->setTabHidden(tabs_->indexOf(setup_),                     HIDE);
        tabs_->setCurrentIndex(tabs_->indexOf(resultsConstraints_));
    }

    void showLoggedOutView() {
        tabs_->setTabHidden(tabs_->indexOf(findWorkingConfig_),         SHOW);
        tabs_->setTabHidden(tabs_->indexOf(resultsConstraints_),        HIDE);
        tabs_->setTabHidden(tabs_->indexOf(details_),                   HIDE);
        tabs_->setTabHidden(tabs_->indexOf(errors_),                    HIDE);
        tabs_->setTabHidden(tabs_->indexOf(settings_),                  HIDE);
        tabs_->setTabHidden(tabs_->indexOf(developers_),                SHOW); // shows the login form
        tabs_->setTabHidden(tabs_->indexOf(setup_),                     HIDE);
        tabs_->setCurrentIndex(tabs_->indexOf(findWorkingConfig_));
    }

    void showDeveloperActionView() {
        tabs_->setTabHidden(tabs_->indexOf(findWorkingConfig_),         HIDE);
        tabs_->setTabHidden(tabs_->indexOf(resultsConstraints_),        HIDE);
        tabs_->setTabHidden(tabs_->indexOf(details_),                   HIDE);
        tabs_->setTabHidden(tabs_->indexOf(errors_),                    HIDE);
        tabs_->setTabHidden(tabs_->indexOf(settings_),                  HIDE);
        tabs_->setTabHidden(tabs_->indexOf(developers_),                SHOW);
        tabs_->setTabHidden(tabs_->indexOf(setup_),                     HIDE);
        tabs_->setCurrentIndex(tabs_->indexOf(developers_));
    }

    void switchTabs(int idx) {
        if (tabs_->widget(idx) == errors_)
            errors_->updateErrorList(resultsConstraints_->constraints()->dependencies());
        if (tabs_->widget(idx) == findWorkingConfig_)
            findWorkingConfig_->updateCounts();
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
        findWorkingConfig_->updateCounts();
    }

    void showTestDetails(int testId) {
        details_->setTestId(testId);
        tabs_->setCurrentWidget(details_);
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
        gstate.dependencyNames.insert(key, "test.rmc_"+key);
    }

    // Additional key/column relationships
    gstate.dependencyNames.insert("reporting_user", "auth_user.identity");
    gstate.dependencyNames.insert("reporting_time", "test.reporting_time");
    gstate.dependencyNames.insert("tester", "test.tester");
    gstate.dependencyNames.insert("os", "test.os");
    gstate.dependencyNames.insert("rose", "test.rose");
    gstate.dependencyNames.insert("rose_date", "test.rose_date");
    gstate.dependencyNames.insert("status", "test.status");
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
