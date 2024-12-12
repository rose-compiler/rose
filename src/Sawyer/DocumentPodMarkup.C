// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#include <Sawyer/DocumentPodMarkup.h>
#include <Sawyer/FileSystem.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

namespace Sawyer {
namespace Document {

namespace {

// Escape characters that are special to pod
static std::string
podEscape(const std::string &s) {
    std::string retval;
    BOOST_FOREACH (char ch, s) {
        if ('<' == ch) {
            retval += "E<lt>";
        } else if ('>' == ch) {
            retval += "E<gt>";
        } else if ('|' == ch) {
            retval += "E<verbar>";
        } else if ('/' == ch) {
            retval += "E<sol>";
        } else {
            retval += ch;
        }
    }
    return retval;
}

// To override BaseMarkup's @section{title}{body}
class Section: public Markup::Function {
protected:
    Section(const std::string &name): Markup::Function(name) {}

public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Section(name))->arg("title")->arg("body");
    }

    std::string eval(const Markup::Grammar&, const std::vector<std::string> &args) {
        ASSERT_require(args.size() == 2);

        // Do not produce a section if the body would be empty.
        if (!BaseMarkup::hasNonSpace(BaseMarkup::unescape(args[1])))
            return "";

        // The =section and =endsection are processed by Sawyer before perpod sees anything
        return "\n\n=section " + BaseMarkup::makeOneLine(args[0]) + "\n\n" + args[1] + "\n\n=endsection\n\n";
    }
};

// To override BaseMarkup's @named{item}{body}
class NamedItem: public Markup::Function {
protected:
    NamedItem(const std::string &name): Markup::Function(name) {}

public:
    static Ptr instance(const std::string &name) {
        return Ptr(new NamedItem(name))->arg("item")->arg("body");
    }

    std::string eval(const Markup::Grammar&, const std::vector<std::string> &args) {
        ASSERT_require(args.size() == 2);
        return ("\n\n"
                "=over\n\n"
                "=item Z<>" + BaseMarkup::makeOneLine(args[0]) + "\n\n" +
                args[1] + "\n\n"
                "=back\n\n");
    }
};

// To override BaseMarkup's @bullet{body} and @numbered{body}
class NumberedItem: public Markup::Function {
    std::string format_;                                // "*" or "1"
protected:
    NumberedItem(const std::string &name, const std::string &format)
        : Markup::Function(name), format_(format) {}

public:
    static Ptr instance(const std::string &name, const std::string &format) {
        return Ptr(new NumberedItem(name, format))->arg("body");
    }

    std::string eval(const Markup::Grammar&, const std::vector<std::string> &args) {
        ASSERT_require(args.size() == 1);
        return ("\n\n"
                "=over\n\n"
                "=item " + format_ + "\n\n" +
                args[0] + "\n\n"
                "=back\n\n");
    }
};

// To override BaseMarkup's @v{variable}
class InlineFormat: public Markup::Function {
    std::string format_;                                // I, B, C, E, F, S, X, Z (L is handled by Link)
protected:
    InlineFormat(const std::string &name, const std::string &format)
        : Markup::Function(name), format_(format) {}
public:
    static Ptr instance(const std::string &name, const std::string &format) {
        return Ptr(new InlineFormat(name, format))->arg("body"); 
    }
    std::string eval(const Markup::Grammar&, const std::vector<std::string> &args) {
        ASSERT_require(args.size() == 1);
        return format_ + "<" + podEscape(args[0]) + ">";
    }
};

// To handle links like @link{url}{title} where {title} is optional
class Link: public Markup::Function {
protected:
    Link(const std::string &name)
        : Markup::Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Link(name))->arg("url")->arg("title", "");
    }
    std::string eval(const Markup::Grammar&, const std::vector<std::string> &args) {
        ASSERT_require(args.size() == 2);
        if (args[1].empty())
            return "[L<" + podEscape(args[0]) + ">]";
        return "[" + args[1] + "](L<" + podEscape(args[0]) + ">)";
    }
};

// Verbatim paragraph
class Verbatim: public Markup::Function {
protected:
    Verbatim(const std::string &name)
        : Markup::Function(name) {}

public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Verbatim(name))->arg("content");
    }
    std::string eval(const Markup::Grammar&, const std::vector<std::string> &args) {
        ASSERT_require(args.size() == 1);
        std::string retval = "\n";
        std::vector<std::string> lines;
        boost::split(lines, args[0], boost::is_any_of("\n"));
        for (const std::string &line: lines)
            retval += "    " + line + "\n";             // no podEscape necessary
        return retval;
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
PodMarkup::init() {
    with(InlineFormat::instance("b", "B"));             // bold
    with(NumberedItem::instance("bullet", "*"));
    with(InlineFormat::instance("c", "C"));             // code
    with(Link::instance("link"));
    with(NamedItem::instance("named"));
    with(NumberedItem::instance("numbered", "1"));
    with(Section::instance("section"));
    with(InlineFormat::instance("v", "I"));;            // variable
    with(Verbatim::instance("quote"));
}

SAWYER_EXPORT bool
PodMarkup::emit(const std::string &doc) {
    // Generate POD documentation into a temporary file. Since perldc doesn't support the "name" property, but rather uses the
    // file name, we create a temporary directory and place a POD file inside with the name we want.
    FileSystem::TemporaryDirectory tmpDir;
    FileSystem::TemporaryFile tmpFile(tmpDir.name() / (pageName() + ".pod"));
    tmpFile.stream() <<(*this)(doc);
    tmpFile.stream().close();

    std::string lessOptions = []() {
        if (const char *less = ::getenv("LESS")) {
            return less;
        } else {
            return "";
        }
    }();

    std::string cmd = "env LESSCHARSET=utf-8"
                      " LESS=" + escapeSingleQuoted(lessOptions + "R") +
                      " perldoc"
                      " -o man"
                      " -w 'center:" + escapeSingleQuoted(chapterTitleOrDefault()) + "'"
                      " -w 'date:" + escapeSingleQuoted(versionDateOrDefault()) + "'"
                      " -w 'release:" + escapeSingleQuoted(versionStringOrDefault()) + "'"
                      " -w 'section:" + escapeSingleQuoted(chapterNumberOrDefault()) + "'"
                      " '" + escapeSingleQuoted(tmpFile.name().string()) + "'";

    return system(cmd.c_str()) == 0;
};

SAWYER_EXPORT std::string
PodMarkup::finalizeDocument(const std::string &s_) {
    std::string s = "=pod\n\n" + s_ + "\n\n=cut\n";

    // Remove pairs of "=back =over" and spaces at the ends of lines
    {
        boost::regex backOverRe("(^=back\\s*=over)|[ \\t\\r\\f]+$");
        std::ostringstream out(std::ios::out | std::ios::binary);
        std::ostream_iterator<char, char> oi(out);
        boost::regex_replace(oi, s.begin(), s.end(), backOverRe, "");
        s = out.str();
    }

    // Change "=section" to "=headN" where N is 1, 2, 3, or 4, and remove =endsection
    {
        std::string result;
        int sectionDepth = 0;
        std::vector<std::string> lines;
        boost::split(lines, s, boost::is_any_of("\n"));
        BOOST_FOREACH (const std::string &line, lines) {
            if (boost::starts_with(line, "=section")) {
                size_t headLevel = std::max(1, std::min(++sectionDepth, 4));
                result += "=head" + boost::lexical_cast<std::string>(headLevel) + line.substr(8) + "\n";
            } else if (boost::starts_with(line, "=endsection")) {
                --sectionDepth;
            } else {
                result += line + "\n";
            }
        }
        std::swap(s, result);
    }
    
    // Replace lots of blank lines with just one blank line
    {
        boost::regex blankLines("\\n{3,}");
        std::ostringstream out(std::ios::out | std::ios::binary);
        std::ostream_iterator<char, char> oi(out);
        boost::regex_replace(oi, s.begin(), s.end(), blankLines, "\n\n");
        s = out.str();
    }

    return s;
}

} // namespace
} // namespace
