// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/MarkupPod.h>

#include <boost/foreach.hpp>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <fstream>
#include <Sawyer/Assert.h>
#include <Sawyer/Sawyer.h>
#include <sstream>

#ifdef BOOST_WINDOWS
# include <windows.h>                                   // GetTempPath
# include <shlwapi.h>                                   // PathFileExists
#else
# include <unistd.h>                                    // access
# include <sys/stat.h>                                  // IS_DIR
# include <paths.h>                                     // _PATH_TMP
#endif

namespace Sawyer {
namespace Markup {

SAWYER_EXPORT PodFormatter::Ptr
PodFormatter::title(const std::string &pageName, const std::string &chapterNumber, const std::string &chapterName) {
    this->pageName(pageName);
    this->chapterNumber(chapterNumber);
    this->chapterName(chapterName);
    return self();
}

SAWYER_EXPORT PodFormatter::Ptr
PodFormatter::version(const std::string &versionString, const std::string &dateString) {
    this->versionString(versionString);
    this->dateString(dateString);                       // FIXME[Robb Matzke 2014-06-14]: use current date if empty
    return self();
}

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

static std::string
tempFileName(const std::string &ext="") {
#if 0 // [Robb Matzke 2014-06-18]: temp_directory_path and unique_path are not always available
    boost::filesystem::path path = boost::filesystem::temp_directory_path().string();
    path /= "%%%%-%%%%-%%%%-%%%%" + ext;
    return boost::filesystem::unique_path(path).string();
#else
# ifdef BOOST_WINDOWS
    char dummy;
    size_t size = GetTempPath(0, &dummy);               // size includes NUL terminator
    if (0==size)
        throw std::runtime_error("could not get system temporary directory name");
    std::vector<char> buffer(size);
    size = GetTempPath(size, &buffer[0]);
    ASSERT_require(size==buffer.size());
    std::string tempPath(buffer.begin(), buffer.begin()+size); // includes final back-slash according to documentation
# else
    std::string tempPath;
    if (0!=geteuid()) {
        if (char *e = getenv("TMPDIR")) {
            struct stat sb;
            if (0==stat(e, &sb) && S_ISDIR(sb.st_mode))
                tempPath = e;
        }
    }
    if (tempPath.empty()) {
        struct stat sb;
        if (0==stat(P_tmpdir, &sb) && S_ISDIR(sb.st_mode))
            tempPath = P_tmpdir;
    }
    if (tempPath.empty())
        tempPath = _PATH_TMP;
    if (tempPath.empty())
        tempPath = "/tmp";
    if ('/'!=tempPath[tempPath.size()-1])
        tempPath += "/";
# endif
    while (1) {
        std::string basename;
        for (int i=0; i<4; ++i) {
            if (i!=0)
                basename += '-';
            for (int j=0; j<4; ++j) {
                int k = rand() % 16;
                char ch = k < 10 ? '0'+k : 'a'+k-10;
                basename += ch;
            }
        }
        std::string path = tempPath + basename + ext;
#ifdef BOOST_WINDOWS
        bool exists = PathFileExists(path.c_str());
#else
        bool exists = 0 == access(path.c_str(), F_OK);
#endif
        if (!exists)
            return path;
    }
#endif
}
    
struct TempFile {
    std::string name;
    std::ofstream stream;
    TempFile(const std::string &name): name(name), stream(name.c_str()) {}
    ~TempFile() {
        stream.close();
        std::remove(name.c_str());
    }
};

struct TempDir {
    std::string name;
    TempDir(const std::string &name): name(name) {
#ifdef BOOST_WINDOWS
        ASSERT_not_implemented("[Robb Matzke 2014-06-18]");
        this->name += '\\';
#else
        mkdir(name.c_str(), 0777);
        this->name += '/';
#endif
    }
    ~TempDir() {
#ifdef BOOST_WINDOWS
        ASSERT_not_implemented("[Robb Matzke 2014-06-18]");
#else
        system(("/bin/rm -rf '" + escapeSingleQuoted(name) + "'").c_str());
#endif
    }
};

SAWYER_EXPORT std::string
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
                      " " + tmpfile.name;

    FILE *f = popen(cmd.c_str(), "r");
    if (!f) {
#include <Sawyer/WarningsOff.h>                         // suppress strerror unsafe warning from Microsoft C++
        throw std::runtime_error(std::string("cannot run command: ") + strerror(errno) + "\ncommand: " + cmd);
#include <Sawyer/WarningsRestore.h>
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

SAWYER_EXPORT void
PodFormatter::emit(const ParserResult &parsed) {
    // Generate POD documentation into a temporary file.  Since perldoc doesn't support the "name" property, but rather
    // uses the file name, we create a temporary directory and place a POD file inside with the name we want.
    TempDir tmpdir(tempFileName());
    std::string fileName = tmpdir.name + pageName_ + ".pod";
    {
        std::ofstream stream(fileName.c_str());
        parsed.emit(stream, sharedFromThis());
    }

    std::string cmd = "perldoc "
                      " -o man"
                      " -w 'center:" + escapeSingleQuoted(chapterName_) + "'"
                      " -w 'date:" + escapeSingleQuoted(dateString_) + "'"
                      // " -w 'name:" + escapeSingleQuoted(pageName_) + "'"
                      " -w 'release:" + escapeSingleQuoted(versionString_) + "'"
                      " -w 'section:" + escapeSingleQuoted(chapterNumber_) + "'"
                      " " + fileName;
    system(cmd.c_str());
}
    
// Number of times a tag with the same name as the top-of-stack tag appears in the stack, not counting the top-of-stack.
SAWYER_EXPORT size_t
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
SAWYER_EXPORT void
PodFormatter::checkArgs(const Tag::Ptr &tag, size_t nArgs, const TagArgs &args) const {
    if (args.size()!=nArgs) {
        std::ostringstream ss;
        ss <<"expected " <<nArgs <<" argument" <<(1==nArgs?"":"s") <<" for @" <<tag->name() <<" but got " <<args.size();
        throw std::runtime_error(ss.str());
    }
}

// Escape S so that it will appear verbatim in pod output
SAWYER_EXPORT std::string
PodFormatter::escape(const std::string &s) const {
    return s;
}

SAWYER_EXPORT void
PodFormatter::beginDocument(std::ostream &out) {
    out <<"=pod\n\n";
    atBeginningOfLine_ = true;
}

SAWYER_EXPORT void
PodFormatter::endDocument(std::ostream &out) {
    out <<(atBeginningOfLine_?"":"\n") <<"\n=cut\n";
}

static bool
hasNonSpace(const std::string &s) {
    BOOST_FOREACH (char ch, s) {
        if (isgraph(ch))
            return true;
    }
    return false;
}

SAWYER_EXPORT bool
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
        // Nested sections, but only if the body is not empty.
        checkArgs(tag, 2, args);
        size_t level = nested() + 1;
        if (level > 4)
            throw std::runtime_error("POD formatter can handle at most four levels of section nesting");
        std::ostringstream arg0, arg1;
        arg0 <<"=head" <<level <<" ";
        atBeginningOfLine_ = false;
        args[0]->emit(arg0, self());
        arg0 <<"\n\n";
        atBeginningOfLine_ = true;
        args[1]->emit(arg1, self());
        if (hasNonSpace(arg1.str()))
            out <<arg0.str() <<arg1.str();
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

SAWYER_EXPORT void
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

SAWYER_EXPORT void
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
