#define BOOST_FILESYSTEM_VERSION 3

#include <sawyer/MarkupPod.h>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <sawyer/Assert.h>
#include <sstream>

namespace Sawyer {
namespace Markup {

PodFormatter::Ptr
PodFormatter::title(const std::string &pageName, const std::string &chapterNumber, const std::string &chapterName) {
    this->pageName(pageName);
    this->chapterNumber(chapterNumber);
    this->chapterName(chapterName);
    return self();
}

PodFormatter::Ptr
PodFormatter::version(const std::string &versionString, const std::string &dateString) {
    this->versionString(versionString);
    this->dateString(dateString);                       // FIXME[Robb Matzke 2014-06-14]: use current date if empty
    return self();
}

static boost::filesystem::path
tempFileName(const std::string &ext="") {
    boost::filesystem::path path = boost::filesystem::temp_directory_path().string();
    path /= "%%%%-%%%%-%%%%-%%%%" + ext;
    return boost::filesystem::unique_path(path);
}
    
struct TempFile {
    boost::filesystem::path name;
    std::ofstream stream;
    TempFile(const boost::filesystem::path &name): name(name), stream(name.string().c_str()) {}
    ~TempFile() {
        stream.close();
        boost::filesystem::remove(name);
    }
};

struct TempDir {
    boost::filesystem::path name;
    TempDir(const boost::filesystem::path &name): name(name) {
        boost::filesystem::create_directory(name);
    }
    ~TempDir() {
        boost::filesystem::remove_all(name);
    }
};

static std::string
escapeSingleQuoted(const std::string &s) {
    std::string result;
    BOOST_FOREACH (char ch, s) {
        if ('\''==ch) {
            result += "\\'";
        } else {
            ASSERT_always_require(isprint(ch));
            result += ch;
        }
    }
    return result;
}

std::string
PodFormatter::toNroff(const ParserResult &parsed) {
    // Generate POD documentation into a temporary file
    TempFile tmpfile(tempFileName(".pod"));
    parsed.emit(tmpfile.stream, sharedFromThis());
    tmpfile.stream.close();

    std::string cmd = "pod2man"
                      " --center='" + escapeSingleQuoted(chapterName_) + "'"
                      " --date='" + escapeSingleQuoted(dateString_) + "'"
                      " --name='" + escapeSingleQuoted(pageName_) + "'"
                      " --release='" + escapeSingleQuoted(versionString_) + "'"
                      " --section='" + escapeSingleQuoted(chapterNumber_) + "'"
                      " " + tmpfile.name.string();

    FILE *f = popen(cmd.c_str(), "r");
    if (!f) {
#include <sawyer/WarningsOff.h>                         // suppress strerror unsafe warning from Microsoft C++
        throw std::runtime_error(std::string("cannot run command: ") + strerror(errno) + "\ncommand: " + cmd);
#include <sawyer/WarningsRestore.h>
    }
    
    std::string result;
    while (1) {
        std::string line = readOneLine(f);
        if (line.empty())
            break;
        result += line;
    }

    if (-1 == Sawyer::pclose(f))
        throw std::runtime_error("command failed: " + cmd);

    return result;
}

void
PodFormatter::emit(const ParserResult &parsed) {
    // Generate POD documentation into a temporary file.  Since perldoc doesn't support the "name" property, but rather
    // uses the file name, we create a temporary directory and place a POD file inside with the name we want.
    TempDir tmpdir(tempFileName());
    boost::filesystem::path fileName = tmpdir.name;
    fileName /= pageName_ + ".pod";
    {
        std::ofstream stream(fileName.string().c_str());
        parsed.emit(stream, sharedFromThis());
    }

    std::string cmd = "perldoc "
                      " -w 'center:" + escapeSingleQuoted(chapterName_) + "'"
                      " -w 'date:" + escapeSingleQuoted(dateString_) + "'"
                      // " -w 'name:" + escapeSingleQuoted(pageName_) + "'"
                      " -w 'release:" + escapeSingleQuoted(versionString_) + "'"
                      " -w 'section:" + escapeSingleQuoted(chapterNumber_) + "'"
                      " " + fileName.string();
    system(cmd.c_str());
}
    
// Number of times a tag with the same name as the top-of-stack tag appears in the stack, not counting the top-of-stack.
size_t
PodFormatter::nested() const {
    ASSERT_forbid(tagStack_.empty());
    size_t count = 0;
    BOOST_FOREACH (const Tag::Ptr &tag, tagStack_) {
        if (tag->name() == tagStack_.back()->name())
            ++count;
    }
    ASSERT_require(count>0);
    return count-1;                                     // don't count top-of-stack
}

// Check that the expected number of arguments are present.
void
PodFormatter::checkArgs(const Tag::Ptr &tag, size_t nArgs, const TagArgs &args) const {
    if (args.size()!=nArgs) {
        std::ostringstream ss;
        ss <<"expected " <<nArgs <<" argument" <<(1==nArgs?"":"s") <<" for @" <<tag->name() <<" but got " <<args.size();
        throw std::runtime_error(ss.str());
    }
}

// Escape S so that it will appear verbatim in pod output
std::string
PodFormatter::escape(const std::string &s) const {
    return s;
}

void
PodFormatter::beginDocument(std::ostream &out) {
    out <<"=pod\n\n";
    atBeginningOfLine_ = true;
}

void
PodFormatter::endDocument(std::ostream &out) {
    out <<(atBeginningOfLine_?"":"\n") <<"\n=cut\n";
}

bool
PodFormatter::beginTag(std::ostream &out, const Tag::Ptr &tag, const TagArgs &args) {
    tagStack_.push_back(tag);
    if (tag->type() == DIVIDING) {
        out <<(atBeginningOfLine_?"":"\n") <<"\n";
        atBeginningOfLine_ = true;
    }

    if (tag->name() == "prose") {
        checkArgs(tag, 1, args);
        out <<"=over\n\n";
        textModePush(PROSE);

    } else if (tag->name() == "nonprose") {
        checkArgs(tag, 1, args);
        out <<"=over\n\n";
        textModePush(NONPROSE);

    } else if (tag->name() == "section") {
        checkArgs(tag, 2, args);
        size_t level = nested() + 1;
        if (level > 4)
            throw std::runtime_error("POD formatter can handle at most four levels of section nesting");
        out <<"=head" <<level <<" ";
        atBeginningOfLine_ = false;
        args[0]->emit(out, self());
        out <<"\n\n";
        atBeginningOfLine_ = true;
        args[1]->emit(out, self());
        return false;                                   // no recursion

    } else if (tag->name() == "list") {
        out <<"=over\n\n";
        atBeginningOfLine_ = true;

    } else if (tag->name() == "bullet") {
        checkArgs(tag, 1, args);
        out <<"=item * ";
        atBeginningOfLine_ = false;

    } else if (tag->name() == "numbered") {
        checkArgs(tag, 1, args);
        out <<"=item 1 ";
        atBeginningOfLine_ = false;

    } else if (tag->name() == "named") {
        checkArgs(tag, 2, args);
        out <<"=item ";
        args[0]->emit(out, self());
        out <<"\n\n";
        atBeginningOfLine_ = true;
        args[1]->emit(out, self());
        return false;

    } else if (tag->name() == "comment") {
        return false;

    } else if (tag->name() == "emphasis") {
        checkArgs(tag, 1, args);
        out <<(nested() % 2 ? 'B' : 'I') <<'<';
        atBeginningOfLine_ = false;
        args[0]->emit(out, self());
        out <<">";
        atBeginningOfLine_ = false;
        return false;
            
    } else if (tag->name() == "variable") {
        checkArgs(tag, 1, args);
        out <<"I<";
        atBeginningOfLine_ = false;
        args[0]->emit(out, self());
        out <<">";
        atBeginningOfLine_ = false;
        return false;

    } else if (tag->name() == "link") {                 // second arg (text) is ignored; only link is shown
        checkArgs(tag, 2, args);
        out <<"L<";
        atBeginningOfLine_ = false;
        args[0]->emit(out, self());
        out <<">";
        atBeginningOfLine_ = false;
        return false;

    } else if (tag->name() == "verbatim") {

    } else {
        throw std::runtime_error("unrecognized markup tag \"" + tag->name() + "\" for Perl POD formatter");
    }
    return true;
}

void
PodFormatter::endTag(std::ostream &out, const Tag::Ptr &tag, const TagArgs &args) {
    ASSERT_forbid(tagStack_.empty());
    ASSERT_require(tagStack_.back()==tag);
    tagStack_.pop_back();
    if (tag->type() == DIVIDING)
        out <<(atBeginningOfLine_?"":"\n") <<"\n";

    if (tag->name() == "list") {
        out <<"=back\n\n";
    } else if (tag->name() == "prose" || tag->name() == "nonprose") {
        out <<"=back\n\n";
        textModePop();
    }
}

void
PodFormatter::text(std::ostream &out, const std::string &s) {
    BOOST_FOREACH (char ch, escape(s)) {
        if ('\n'==ch) {
            out <<ch;
            atBeginningOfLine_ = true;
        } else if (atBeginningOfLine_) {
            if (textMode()==PROSE && (' '==ch || '\t'==ch)) {
                // toss white space at beginning of prose lines
            } else if (textMode()==NONPROSE) {
                // make sure non-prose lines start with white space (we use tab so that tabs embedded in the rest of
                // the line still line up nicely in case a human needs to look at the POD output).
                out <<"\t" <<ch;
                atBeginningOfLine_ = false;
            } else {
                out <<ch;
                atBeginningOfLine_ = false;
            }
        } else {
            out <<ch;
        }
    }
}

} // namespace
} // namespace
