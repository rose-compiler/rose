static const char *gPurpose = "manage attachments for a test result";
static const char *gDescription =
    "This tool queries test result attachments, or modifies the attachments. Each test has a set of zero or more data files "
    "that are associated with it. Each of these files is has a unique identifying number, a title, and some binary content.";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Rose/StringUtility/Escape.h>

#include <Sawyer/Database.h>
#include <boost/filesystem.hpp>
#include <fcntl.h>
#include <unistd.h>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    bool doAttach = false;                              // attach new files
    bool doDetach = false;                              // detach existing files
    std::string title;                                  // title for attachment (usually a file base name)
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    Format outputFormat = Format::PLAIN;                // type of output to produce
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis",
               "@prop{programName} [@v{switches}] @v{test_id}\n\n"
               "@prop{programName} [@v{switches}] @v{test_id} @v{attachment_id}\n\n"
               "@prop{programName} [@v{switches}] @s{attach} @v{test_id} @v{file}\n\n"
               "@prop{programName} [@v{switches}] @s{detach} @v{test_id} [@v{attachment_id}]");

    SwitchGroup sg("Tool-specific switches");

    insertDatabaseSwitch(sg, settings.databaseUri);

    insertOutputFormatSwitch(sg, settings.outputFormat,
                             FormatFlags()
                                 .set(Format::PLAIN)
                                 .set(Format::YAML)
                                 .set(Format::HTML)
                                 .set(Format::CSV)
                                 .set(Format::SHELL));

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

    std::vector<std::string> args = parser
                                    .with(Rose::CommandLine::genericSwitches())
                                    .with(sg)
                                    .parse(argc, argv)
                                    .apply()
                                    .unreachedArgs();
    if (args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    if (settings.doAttach && settings.doDetach) {
        mlog[FATAL] <<"mutually exlusive operations: --attach and --detach\n";
        exit(1);
    }
    return args;
}

static bool
isValidTestId(DB::Connection db, int testId) {
    return 1 == db.stmt("select count(*) from test_results where id = ?id").bind("id", testId).get<int>().get();
}

static std::vector<uint8_t>
readFileOrDie(const boost::filesystem::path &fileName) {
    const int fd = open(fileName.c_str(), O_RDONLY);
    if (-1 == fd) {
        mlog[FATAL] <<"cannot open " <<fileName <<" for reading\n";
        exit(1);
    }

    // Initially reserve enough space for the whole file
    const uintmax_t nBytes = boost::filesystem::file_size(fileName);
    if (static_cast<uintmax_t>(-1) == nBytes) {
        mlog[FATAL] <<"cannot open " <<fileName <<" for reading\n";
        exit(1);
    }
    std::vector<uint8_t> buf(nBytes);

    // Read the file into the vector. Be careful because the size of the file could change while we're reading. We'll
    // read only up to the size we already obtained, but the file could also shrink.
    size_t amountRead = 0;
    while (size_t canRead = buf.size() - amountRead) {
        ssize_t n = read(fd, buf.data() + amountRead, canRead);
        if (-1 == n) {
            mlog[FATAL] <<"read failed from " <<fileName <<"\n";
            exit(1);
        } else if (0 == n) {
            break;
        }
        amountRead += n;
    }
    buf.resize(amountRead);
    close(fd);
    return buf;
}

static std::string
titleForFile(const boost::filesystem::path &fileName, const Settings &settings) {
    if (!settings.title.empty())
        return settings.title;
    return fileName.filename().string();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

    int testId = boost::lexical_cast<int>(args[0]);
    if (!isValidTestId(db, testId)) {
        mlog[FATAL] <<"test #" <<testId <<" does not exist\n";
        exit(1);
    }
    args.erase(args.begin());

    if (settings.doAttach) {
        // Add an attachment
        if (1 != args.size()) {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        std::vector<uint8_t> content = readFileOrDie(args[0]);
        db.stmt("insert into attachments (test_id, name, content) values (?test_id, ?name, ?content)")
            .bind("test_id", testId)
            .bind("name", titleForFile(args[0], settings))
            .bind("content", content)
            .run();

    } else if (settings.doDetach) {
        // Remove an attachment
        if (1 != args.size()) {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        int attachmentId = boost::lexical_cast<int>(args[0]);
        db.stmt("delete from attachments where test_id = ?test_id and id = ?id")
            .bind("test_id", testId)
            .bind("id", attachmentId)
            .run();

    } else if (args.empty()) {
        // List attachments for a test
        if (!args.empty()) {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        auto stmt = db.stmt("select id, name from attachments where test_id = ?test_id order by id")
                    .bind("test_id", testId);

        FormattedTable table;
        table.format(tableFormat(settings.outputFormat));
        table.columnHeader(0, 0, "Attachment");
        table.columnHeader(0, 1, "Name");

        for (auto row: stmt) {
            const int id = *row.get<int>(0);
            const std::string name = row.get<std::string>(1).orElse("");
            if (Format::YAML == settings.outputFormat) {
                std::cout <<"- id: " <<id <<"\n"
                          <<"  name: " <<StringUtility::yamlEscape(name) <<"\n";
            } else {
                const size_t i = table.nRows();
                table.insert(i, 0, id);
                table.insert(i, 1, name);
            }
        }

        if (Format::YAML != settings.outputFormat)
            std::cout <<table;

    } else {
        // Show attachment content
        if (1 != args.size()) {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        int attachmentId = boost::lexical_cast<int>(args[0]);
        auto stmt = db.stmt("select content from attachments where test_id = ?test_id and id = ?id")
                    .bind("test_id", testId)
                    .bind("id", attachmentId);
        for (auto row: stmt) {
            auto content = row.get<std::vector<uint8_t>>(0).orDefault();
            for (uint8_t ch: content)
                std::cout <<(char)ch;
        }
    }
}
