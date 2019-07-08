#include <rose.h>

#include <boost/foreach.hpp>
#include <CommandLine.h>                                // from ROSE
#include <FileSystem.h>                                 // from ROSE
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <SqlDatabase.h>

#include <fcntl.h>
#include <unistd.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

struct Settings {
    bool dryRun;
    bool doAttach;                                      // attach new files
    bool doDetach;                                      // detach existing files
    std::string title;                                  // title for attachment (usually a file base name)
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings(): dryRun(false), doAttach(false), doDetach(false)
#ifdef DEFAULT_DATABASE
          , databaseUri(DEFAULT_DATABASE)
#endif
        {}
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    parser.purpose("manages test result attached text files");
    parser.errorStream(mlog[FATAL]);
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis",
               "@prop{programName} [@v{switches}] @v{test_id}\n\n"
               "@prop{programName} [@v{switches}] @v{test_id} @v{attachment_id}\n\n"
               "@prop{programName} [@v{switches}] @s{attach} @v{test_id} @v{file}\n\n"
               "@prop{programName} [@v{switches}] @s{detach} @v{test_id} [@v{attachment_id}]");
    parser.doc("Description",
               "Manages text files attached to a test result.  Each test result can store zero or more text files "
               "as attachments, and this command is used to access them.  If called with only a @v{test_id}, it lists "
               "the attachment ID numbers and titles (if any) for the specified test.  If called with a test ID and "
               "an attachment ID, it outputs the attachment. If called with @s{attach} it inserts the contents of the "
               "specified files into the database attached to the specified test. The @s{detach} switch causes the "
               "specified attachments (or all attachments) to be removed from the database.");

    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("URI specifying which database to use. This switch overrides the ROSE_MATRIX_DATABASE environment variable. " +
                   SqlDatabase::uriDocumentation()));

    sg.insert(Switch("title", 't')
              .argument("name", anyParser(settings.title))
              .doc("When inserting attachments, use the specified argument as the title for the files.  If no title is "
                   "specified then this tool use the non-path part of the file name(s) as the titles."));

    sg.insert(Switch("attach")
              .intrinsicValue(true, settings.doAttach)
              .doc("Attach the specified files to the test result."));
    sg.insert(Switch("detach")
              .intrinsicValue(true, settings.doDetach)
              .doc("Detach specified files (or all files) from the test result."));

    sg.insert(Switch("dry-run")
              .intrinsicValue(true, settings.dryRun)
              .doc("Do everything but update the database.  When this switch is present, the database is accessed "
                   "like normal, but the final COMMIT is skipped, causing the database to roll back to its initial "
                   "state."));

    return parser.with(Rose::CommandLine::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs();
}

static bool
isAuthorized(const SqlDatabase::TransactionPtr &tx) {
    const char *userName = getenv("LOGNAME");
    if (!userName || !*userName)
        return false;                                   // LOGNAME not set
    SqlDatabase::StatementPtr q = tx->statement("select id from auth_identities where identity = ?")->bind(0, userName);
    SqlDatabase::Statement::iterator row = q->begin();
    if (row == q->end())
        return false;                                   // No such user name
    ++row;
    if (row != q->end()) {
        mlog[FATAL] <<"user \"" <<StringUtility::cEscape(userName) <<"\" is ambiguous\n";
        exit(1);
    }
    return true;
}

static bool
isValidTestId(int testId, const SqlDatabase::TransactionPtr &tx) {
    return 1 == tx->statement("select count(*) from test_results where id = ?")->bind(0, testId)->execute_int();
}

static std::string
readFileOrDie(const std::string &fileName) {
    std::string content;
    int fd = open(fileName.c_str(), O_RDONLY);
    if (-1 == fd) {
        mlog[FATAL] <<"cannot open \"" <<StringUtility::cEscape(fileName) <<"\"\n";
        exit(1);
    }
    char buf[4096];
    while (1) {
        ssize_t n = read(fd, buf, sizeof buf);
        if (-1 == n) {
            mlog[FATAL] <<"read failed from \"" <<StringUtility::cEscape(fileName) <<"\n";
            exit(1);
        } else if (0 == n) {
            break;
        } else {
            content += std::string(buf, n);
        }
    }
    close(fd);
    return content;
}

static std::string
titleForFile(const std::string &fileName, const Settings &settings) {
    using namespace Rose::FileSystem;
    if (!settings.title.empty())
        return settings.title;
    return Path(fileName).filename().native();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    if (const char *dbUri = getenv("ROSE_MATRIX_DATABASE"))
        settings.databaseUri = dbUri;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    if (args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    if (settings.doAttach && settings.doDetach) {
        mlog[FATAL] <<"mutually exlusive operations: --attach and --detach\n";
        exit(1);
    }
    SqlDatabase::TransactionPtr tx;
    try {
        tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    } catch (const SqlDatabase::Exception &e) {
        mlog[FATAL] <<"cannot open database: " <<e.what();
        exit(1);
    }

    int testId = boost::lexical_cast<int>(args[0]);
    if (!isValidTestId(testId, tx)) {
        mlog[FATAL] <<"test #" <<testId <<" does not exist\n";
        exit(1);
    }
    args.erase(args.begin());

    if (settings.doAttach) {
        // Attach some files to a test
        if (!isAuthorized(tx)) {
            mlog[FATAL] <<"insufficient permission to modify the database\n";
            exit(1);
        }
        BOOST_FOREACH (const std::string &fileName, args) {
            std::string content = readFileOrDie(fileName);
            std::string title = titleForFile(fileName, settings);
            tx->statement("insert into attachments (test_id, name, content) values (?, ?, ?)")
                ->bind(0, testId)
                ->bind(1, title)
                ->bind(2, content)
                ->execute();
        }
    } else if (settings.doDetach) {
        // Detach some files from a test
        if (!isAuthorized(tx)) {
            mlog[FATAL] <<"insufficient permission to modify the database\n";
            exit(1);
        }
        BOOST_FOREACH (const std::string &arg, args) {
            int attachmentId = boost::lexical_cast<int>(arg);
            tx->statement("delete from attachments where test_id=? and id=?")
                ->bind(0, testId)
                ->bind(1, attachmentId)
                ->execute();
        }
    } else if (args.empty()) {
        // List attachments for a test
        SqlDatabase::StatementPtr q = tx->statement("select id, name from attachments where test_id=? order by id")
                                      ->bind(0, testId);
        for (SqlDatabase::Statement::iterator row=q->begin(); row!=q->end(); ++row)
            std::cerr <<"[" <<row.get<std::string>(0) <<"]\t\"" <<StringUtility::cEscape(row.get<std::string>(1)) <<"\"\n";
            
    } else {
        // Show attachment content
        BOOST_FOREACH (const std::string &arg, args) {
            int attachmentId = boost::lexical_cast<int>(arg);
            SqlDatabase::StatementPtr q = tx->statement("select content from attachments where test_id=? and id=?")
                                          ->bind(0, testId)
                                          ->bind(1, attachmentId);
            for (SqlDatabase::Statement::iterator row=q->begin(); row!=q->end(); ++row)
                std::cout <<row.get<std::string>(0);
        }
    }

    tx->commit();
}
