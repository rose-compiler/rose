// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/DocumentPodMarkup.h>
#include <Sawyer/DocumentTextMarkup.h>
#include <Sawyer/Message.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>

using namespace Sawyer::Message::Common;
namespace doc = Sawyer::Document;
namespace mu = Sawyer::Document::Markup;

static std::string
escape(const std::string &s) {
    std::string retval;
    BOOST_FOREACH (char ch, s) {
        if (' '==ch || (isgraph(ch) && ch != '\\')) {
            retval += ch;
        } else {
            switch (ch) {
                case '\\': retval += "\\\\"; break;
                case '\a': retval += "\\a";  break;
                case '\b': retval += "\\b";  break;
                case '\f': retval += "\\f";  break;
                case '\n': retval += "\\n";  break;
                case '\r': retval += "\\r";  break;
                case '\t': retval += "\\t";  break;
                case '"':  retval += "\\\""; break;
                default: {
                    char buf[32];
                    sprintf(buf, "\\%03o", (unsigned)ch);
                    retval += buf;
                    break;
                }
            }
        }
    }
    return retval;
}

static std::string
mustParse(const mu::Grammar &grammar, const std::string &input, const std::string &answer) {
    std::string retval;
    try {
        retval = grammar(input);
    } catch (const mu::SyntaxError &e) {
        Sawyer::Message::mlog[ERROR] <<e.what() <<"\n";
        ASSERT_not_reachable("input = \"" + escape(input) + "\"");
    }

    ASSERT_always_require2(retval == answer,
                           "input  \"" + escape(input) + "\""
                           "\n  got      \"" + escape(retval) + "\""
                           "\n  expected \"" + escape(answer) + "\"");
    return retval;
}

static std::string
mustNotParse(const mu::Grammar &grammar, const std::string &input, const std::string &needle) {
    try {
        std::string retval = grammar(input);
        ASSERT_not_reachable("should have failed but returned \"" + escape(retval) + "\"");
    } catch (const mu::SyntaxError &e) {
        std::string mesg = e.what();
        ASSERT_always_require2(boost::contains(mesg, needle),
                               "parse failed as expected, but message doesn't contain \"" + escape(needle) + "\""
                               "\n  input = \"" + escape(input) + "\"\n" + mesg);
        return e.what();
    }
}

// Inserts its first arugment surrouned by "[]"
class DynamicContent: public mu::Function {
protected:
    DynamicContent(const std::string &name): mu::Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new DynamicContent(name))->arg("what");
    }
    std::string eval(const mu::Grammar&, const std::vector<std::string> &args) {
        ASSERT_always_require(args.size() == 1);
        return "[" + args[0] + "]";
    }
};

// Multiplies arguments
class Multiply: public mu::Function {
protected:
    Multiply(const std::string &name): mu::Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Multiply(name))->arg("string")->arg("multiplier");
    }
    std::string eval(const mu::Grammar&, const std::vector<std::string> &args) {
        ASSERT_always_require(args.size() == 2);
        std::string retval;
        for (size_t i=0; i<args[1].size(); ++i)
            retval += args[0];
        return retval;
    }
};

// Two args, one of which has a default value
class Manpage: public mu::Function {
protected:
    Manpage(const std::string &name): mu::Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Manpage(name))->arg("page")->arg("chapter", "none");
    }
    std::string eval(const mu::Grammar&, const std::vector<std::string> &args) {
        ASSERT_always_require(args.size() == 2);
        return args[0] + "(" + args[1] + ")";
    }
};

// Test the most basic aspects of markup
static void
testMarkup() {
    mu::Grammar grammar;
    grammar
        .with(mu::StaticContent::instance("f0", "[F0]"))
        .with(DynamicContent::instance("f1"))
        .with(mu::StaticContent::instance("email", "me@@example.com"))
        .with(Multiply::instance("mult"))
        .with(Manpage::instance("man"))
        .with(mu::Error::instance("err", "the-error"))
        .with(mu::Quote::instance("quote"))
        .with(mu::Eval::instance("eval"))
        .with(mu::IfEq::instance("if"));

    // Empty strings
    mustParse(grammar, "",                                      "");
    mustParse(grammar, " ",                                     " ");
    mustParse(grammar, "  ",                                    "  ");

    // Paragraph separators
    mustParse(grammar, "\n",                                    "\n");
    mustParse(grammar, "\n\n",                                  "\n\n");
    mustParse(grammar, "\n \n",                                 "\n \n");

    // Text
    mustParse(grammar, "a b c",                                 "a b c");
    mustParse(grammar, "ab c ",                                 "ab c ");

    // Text with balanced braces
    mustParse(grammar, "{}",                                    "{}");
    mustParse(grammar, "{}{}",                                  "{}{}");
    mustParse(grammar, "{{}}",                                  "{{}}");
    mustParse(grammar, "a {} c",                                "a {} c");
    mustParse(grammar, "a { b } c",                             "a { b } c");
    mustParse(grammar, "a{b{c}}d",                              "a{b{c}}d");
    mustNotParse(grammar, "}",                                  "unexpected end-of-argument");
    mustNotParse(grammar, "{x",                                 "expected end-of-argument");
    mustNotParse(grammar, "x{x",                                "expected end-of-argument");

    // Escapes
    mustParse(grammar, "@",                                     "@");
    mustParse(grammar, "@@",                                    "@");
    mustParse(grammar, "@@@",                                   "@@");
    mustParse(grammar, "@@@@",                                  "@@");
    mustParse(grammar, "@{ @@ @}",                              "{ @ }");
    mustParse(grammar, "@} @@ @{",                              "} @ {");
    mustParse(grammar, "@ f0",                                  "@ f0");
    mustParse(grammar, "@~",                                    "");
    mustParse(grammar, "a@~b",                                  "ab");
    mustParse(grammar, "~@~~",                                  "~~");
    mustParse(grammar, "@@@~@@",                                "@@");
    mustParse(grammar, "@@~",                                   "@~");

    // Missing function
    mustNotParse(grammar, "@no_such_function",                  "is not declared");

    // Function with no args
    mustParse(grammar, "@f0",                                   "[F0]");
    mustParse(grammar, "a@f0 b",                                "a[F0] b");
    mustNotParse(grammar, "@f0{}",                              "too many arguments");
    mustParse(grammar, "@f0@{@}",                               "[F0]{}");
    mustParse(grammar, "@f0@~{}",                               "[F0]{}");

    // Function with one arg
    mustNotParse(grammar, "@f1",                                "not enough arguments");
    mustParse(grammar, "-@f1{x}-",                              "-[x]-");
    mustNotParse(grammar, "@f1{x}{y}",                          "too many arguments");
    mustParse(grammar, "-@f1{x}@~{y}-",                         "-[x]{y}-");

    // Function inside another's argument
    mustParse(grammar, "@f1{@f0}",                              "[[F0]]");
    mustParse(grammar, "@f1{-@f0@~-}",                          "[-[F0]-]");

    // Function returning escaped chars
    mustParse(grammar, "i am @email, shhh",                     "i am me@example.com, shhh");
    mustParse(grammar, "-@f1{me@@example.com @email}-",         "-[me@example.com me@example.com]-");

    // Function with two args
    mustNotParse(grammar, "@mult@f1",                           "not enough arguments");
    mustNotParse(grammar, "@mult{*}",                           "not enough arguments");
    mustParse(grammar, "@mult{*}{xx}",                          "**");
    mustParse(grammar, "-@mult{}{xx}-",                         "--");
    mustNotParse(grammar, "@mult{}{}{}",                        "too many arguments");

    // Function with zero or one arg
    mustNotParse(grammar, "see @man.",                          "got 0 but need at least 1");
    mustParse(grammar, "see @man{cat}.",                        "see cat(none).");
    mustParse(grammar, "see @man{cat}{1}.",                     "see cat(1).");
    mustNotParse(grammar, "see @man{cat}{1}{foo}.",             "got 3 but only 2 allowed");

    // Error inside a function argument
    mustNotParse(grammar, "see @f1{@man}",                      "got 0 but need at least 1");
    mustNotParse(grammar, "see @f1{@f1{@f1{@man}}}",            "got 0 but need at least 1");
    mustNotParse(grammar, "to @err is human",                   "the-error");

    // Macro that simply quotes its argument
    mustParse(grammar, "@quote{x}",                             "x");
    mustParse(grammar, "@quote{@f1}",                           "@f1");
    mustParse(grammar, "@quote{@@}",                            "@"); // escapes still work
    mustParse(grammar, "@quote{{}}",                            "{}");
    mustParse(grammar, "@quote{@quote{x}}",                     "@quote{x}");
    mustParse(grammar, "@quote{@quote@}}",                      "@quote}");

    // Recursive evaluation
    mustParse(grammar, "@eval{@@}{f1{test}}",                   "[test]");
    mustParse(grammar, "@eval{-@@f0@@~-}",                      "-[F0]-");

    // "if" macro
    mustParse(grammar, "@if{x}{x}{a}{b}",                       "a");
    mustParse(grammar, "@if{x}{y}{a}{b}",                       "b");
    mustParse(grammar, "@if{x}{y}{a}",                          "");
    mustParse(grammar, "@if{x}{x}{a}{@err{not reached}}",       "a");
    mustParse(grammar, "@if{x}{y}{@err{not reached}}{b}",       "b");
    mustNotParse(grammar, "@if{x}{x}{@err}{b}",                 "the-error");
    mustParse(grammar, "@if{x}{x}{-@f0@~-}",                   "-[F0]-");
}

// Test POD documentation markup
static void
testPodMarkup() {
#if 0 // [Robb Matzke 2016-09-16]
    doc::PodMarkup render;
#else
    doc::TextMarkup render;
#endif

    std::cout <<render("@section{Description}{This is the description. Atque a et quis officia illum sit aut natus. "
                       "Illum quasi praesentium sed ipsa ea minus eius quis. Voluptas expedita ex tenetur consectetur "
                       "ratione. The variable @v{foo} holds the number and @b{only} the number."

                       "@named{item1}{This is item #1. Iure tenetur excepturi rerum qui magnam veniam. Excepturi excepturi "
                       "tenetur iusto temporibus quibusdam dolores. Voluptatem porro aut ducimus beatae quia. Vel dolorem "
                       "veritatis adipisci nulla vero molestiae. Blanditiis aperiam voluptates asperiores at. Sed quasi earum "
                       "porro voluptas facilis. Aut impedit fugit dolores rem voluptatem assumenda sed. Aut et rerum eos autem "
                       "nemo nobis. Adipisci omnis et mollitia aut. Nobis dolorem consequatur voluptas architecto incidunt. "
                       "Voluptatem et eius et voluptatem. Dolorem eaque earum eum ad qui nulla optio. Atque a et quis officia "
                       "illum sit aut natus. Illum quasi praesentium sed ipsa ea minus eius.}"

                       "@named{item2}{This is item #2. Consequatur exercitationem qui et quisquam reiciendis enim est. "
                       "Nesciunt perspiciatis unde pariatur esse quo molestiae aut impedit. Sed ut aut et nulla atque expedita "
                       "ut. Placeat aut reprehenderit et nemo voluptas. Sint maxime neque vitae et nesciunt ea quo doloribus. "
                       "Voluptate qui et corrupti accusamus quasi.}"

                       "And now followed by @v{n} bullets:"

                       "@bullet{Earum voluptatem tempora qui quae voluptas. Molestias recusandae voluptatem sit et eaque. "
                       "Dolor sit molestias fugiat at. Eius et repellendus porro. Repellendus alias praesentium at aliquam fugit "
                       "sed fugiat dolore. Alias culpa soluta dolorum similique quis nam. Eius facilis sed tempore eum. Itaque "
                       "ut doloribus quas atque tenetur voluptatibus nostrum. Autem non inventore dicta. Exercitationem quo "
                       "autem quia consequuntur.}"

                       "@bullet{Officia consectetur quibusdam rerum iure rerum sunt praesentium vero. Modi ad natus nihil sed "
                       "ab eius tempora. Iusto accusamus deleniti consequatur eius rem. Quam harum necessitatibus nam. "
                       "Voluptatem quibusdam velit laborum illo error sunt.}"

                       "Followed by a numbered list of things:"

                       "@numbered{Quidem et quos maxime. Voluptate vel rerum est aut laborum molestiae nemo. Consequatur "
                       "amet totam aut illo exercitationem.}"

                       "@numbered{Sed consequatur in et eos ut minima ea repellendus. Laboriosam perferendis cum reiciendis "
                       "et voluptatum rerum modi necessitatibus. Nisi quia et rem sapiente iure voluptatum. Eum pariatur "
                       "quisquam non libero. Velit dolorem omnis rerum voluptatum autem voluptates earum. Iure tempore "
                       "architecto ipsam. Quae in voluptatum odio doloremque quaerat. Autem est eum neque ratione sunt."
                       "@bullet{Lists can be nested.}"
                       "@bullet{Don't abuse this since some formats have very strict, arbitrary limitations}"
                       "}"

                       "And the final text.}"

                       "@section{See Also}{Another section.}");
}

int main(int argc, char *argv[]) {
    Sawyer::initializeLibrary();
    testMarkup();
    testPodMarkup();
}
