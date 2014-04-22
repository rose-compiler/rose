#include <sawyer/Markup.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <sstream>

namespace Sawyer {
namespace Markup {

std::ostream& operator<<(std::ostream &o, const Content &c) { c.print(o); return o; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Input
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Consume N characters and return them.
std::string Input::consume(size_t nchars) {
    size_t begin = at_;
    at_ = std::min(at_+nchars, content_.size());
    return content_.substr(begin, at_-begin);
}

// Skip over white space.  Returns the characters skipped.  Backslashed escaped white space doesn't count.
std::string Input::consumeSpace() {
    size_t begin = at_;
    while (isspace(cur())) inc();
    return content_.substr(begin, at_-begin);
}

// Consume input until one of the specified characters is reached, returning the consumed characters.  The matched character is
// not consumed.  Only matches characters that are not escaped.
std::string Input::consumeUntil(const char *stopSet) {
    size_t begin = at_;
    size_t end = content_.find_first_of(stopSet, begin);
    if (end==std::string::npos) {
        at_ = content_.size();
        return content_.substr(begin);
    } else {
        at_ = end;
        return content_.substr(begin, end-begin);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Tags
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Default tag evaluator evaluates each argument and returns the same tag but with evaluated arguments.
ContentPtr Tag::eval(const TagArgs &args) {
    TagArgs evaluatedArgs;
    BOOST_FOREACH (const ContentPtr &arg, args)
        evaluatedArgs.push_back(arg->eval());
    TagInstancePtr ti = TagInstance::instance(self(), evaluatedArgs);
    ContentPtr retval = Content::instance();
    retval->append(ti);
    return retval;
}

// NullTag has no arguments, so just return self
ContentPtr NullTag::eval(const TagArgs &args) {
    TagInstancePtr ti = TagInstance::instance(self());
    ContentPtr retval = Content::instance();
    retval->append(ti);
    return retval;
}

// Comments are non-evaluating
// FIXME[Robb Matzke 2014-02-23]: actually, they should evaluate to the textual representation of their argument so that
// something like "@comment{@b{abc}}" is the string "@b{abc}" (not the bold tag with argument "abc").
ContentPtr CommentTag::eval(const TagArgs &args) {
    TagInstancePtr t1 = TagInstance::instance(self(), args);
    ContentPtr retval = Content::instance();
    retval->append(t1);
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Tag instances - tag + arguments
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ContentPtr TagInstance::eval() const {
    return tag_->eval(args_);
}

void TagInstance::emit(std::ostream &stream, const FormatterPtr &formatter) const {
    if (NullTagPtr nullTag = boost::dynamic_pointer_cast<NullTag>(tag_)) {
        formatter->text(stream, nullTag->text());
    } else {
        if (formatter->beginTag(stream, tag_, args_)) {
            BOOST_FOREACH (const ContentPtr &arg, args_)
                arg->emit(stream, formatter);
        }
        formatter->endTag(stream, tag_, args_);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Content sequence
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Content::append(const std::string &s) {
    TagInstancePtr tag = TagInstance::instance(NullTag::instance(s));
    append(tag);
}

// Evaluate each of the tag instances and concatenate all the results together.
ContentPtr Content::eval() const {
    ContentPtr retval = Content::instance();
    BOOST_FOREACH (const TagInstancePtr &elmt, elmts_)
        retval->append(elmt->eval());
    return retval;
}

void Content::emit(std::ostream &stream, const FormatterPtr &formatter) const {
    BOOST_FOREACH (const TagInstancePtr &elmt, elmts_)
        elmt->emit(stream, formatter);
}

// Text representation of content
std::string Content::asText() const {
    std::ostringstream ss;
    FormatterPtr fmt = TextFormatter::instance();
    emit(ss, fmt);
    return ss.str();
}

void Content::print(std::ostream &o) const {
    o <<"[";
    BOOST_FOREACH (const TagInstancePtr &tag, elmts_)
        o <<" @" <<tag->tag()->name();
    o <<" ]";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Parser result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParserResult::emit(std::ostream &stream, const FormatterPtr &formatter) {
    formatter->beginDocument(stream);
    content_->emit(stream, formatter);
    formatter->endDocument(stream);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Parser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void Parser::init() {
    registerTag(CommentTag::instance(),         "comment");
    registerTag(SectionTag::instance(),         "section");
    registerTag(SubSectionTag::instance(),      "subsection");
    registerTag(SubSectionTag::instance(),      "ssection");
    registerTag(NameBulletTag::instance(),      "defn");
    registerTag(BoldTag::instance(),            "bold");
    registerTag(BoldTag::instance(),            "b");
    registerTag(ItalicTag::instance(),          "italic");
    registerTag(ItalicTag::instance(),          "i");
    registerTag(VariableTag::instance(),        "variable");
    registerTag(VariableTag::instance(),        "var");
    registerTag(VariableTag::instance(),        "v");
    
    registerTag(ProseTag::instance(),   "prose");
    registerTag(CodeTag::instance(),    "code");
    registerTag(CodeTag::instance(),    "c");
}

Parser& Parser::registerTag(const TagPtr &tag, const std::string &name) {
    symtab_[name.empty() ? tag->name() : name] = tag;
    return *this;
}

    

// FIXME[Robb Matzke 2014-02-22]: make this much more informative
std::string Parser::location(const Input &input) {
    if (input.atEnd())
        return "end of input";
    std::ostringstream ss;
    ss <<"byte " <<input.at();
    return ss.str();
}

void Parser::warn(const std::string &mesg) {
    throw std::runtime_error(mesg);
}

char Parser::closingFor(char opening) {
    switch (opening) {
        case '{': return '}';
        case '(': return ')';
        case '[': return ']';
        case '<': return '>';
        case '"': return '"';
        case '`': return '\'';
        case '\'': return '\'';
        default:
            if (isspace(opening))
                return ' ';
    }
    return '\0';
}

// Parse a symbol: a letter followed by one or more letters, digits, and underscores.
std::string Parser::parseSymbol(Input &input) {
    std::string name;
    if (input.atEnd() || !isalpha(*input))
        return name;
    name = input.next();
    while (isalnum(*input))
        name += input.next();
    return name;
}

// Parse input as a string, counting left and right delimiters and stopping just before we see a right delimiter that
// is not balanced with an earlier parsed left delimiter.  For instance, if the left delimiter is "(" and the right delimiter
// is ")" then given the input "one (two) three) four" the parser will stop prior to consuming the second right paren.
std::string Parser::parseNested(Input &input, char left, char right) {
    std::string retval;
    int depth = 0;
    for (/*void*/; !input.atEnd(); ++input) {
        char c = *input;
        if ('\\'==c) {
            retval += input.next();
            c = input.atEnd() ? '\\' : *input;
        } else if (c==left) {
            depth++;
        } else if (c==right) {
            if (depth==0)
                return retval;
            --depth;
        }
        retval += c;
    }
    return retval;
}

// Parse a tag argument of the form "{TEXT}".  The delimiters can be other than curly braces, but must be balanced or escaped
// within TEXT.  The delimiters are not included in the returned delimiters.  Returns null if a tag could not be parsed.
ContentPtr Parser::parseTagArgument(Input &input) {
    ExcursionGuard guard(input);
    if (input.atEnd())
        return ContentPtr();

    ContentPtr retval;
    char opening = input.next();
    if (isspace(opening)) {
        input.consumeSpace();
        CharsetTerminator stopAt(" \t\n\r");
        retval = parseContent(input, stopAt);
    } else if (char closing = closingFor(opening)) {
        ParenTerminator stopAt(opening, closing);
        retval = parseContent(input, stopAt);
        if (closing!=input.next())
            return ContentPtr();
    } else {
        return ContentPtr();
    }
    guard.cancel();
    return retval;
}

// Parse a tag instance of the form @TAGNAME{ARG1}{ARG2}... with as many arguments as declared for the tag.  The argument
// delimiters need not be curly braces.  Returns null without adjusting the input cursor if TAGNAME is not valid or if its
// arguments cannot be parsed.
TagInstancePtr Parser::parseTag(Input &input) {
    ExcursionGuard guard(input);
    if ('@'!=input.next())
        return TagInstancePtr();
    std::string tagName = parseSymbol(input);
    TagPtr tag = symtab_[tagName];
    if (!tag)
        return TagInstancePtr();
    TagInstancePtr retval = TagInstance::instance(tag);
    for (size_t argno=0; argno<tag->nArgsDeclared(); ++argno) {
        if (ContentPtr arg = parseTagArgument(input)) {
            retval->append(arg);
        } else {
            warn("expected " + tag->name() + " argument " + boost::lexical_cast<std::string>(arg) + " at " +
                 location(input));
            return TagInstancePtr();
        }
    }
    guard.cancel();
    return retval;
}

// Parse text, including top-level tag instances.  Text that is not enclosed in a tag is returned as if it were inside a null
// tag.   For instance, if the input is "1 @c{2} 3" will parse as if it were "@{1 }@c{2}@{ 3}".
ContentPtr Parser::parseContent(Input &input, ParseTerminator &stopAt) {
    ContentPtr retval = Content::instance();
    std::string str;
    while (!input.atEnd()) {
        if ('\\'==*input) {
            ++input;
            str = input.next();
        } else if (TagInstancePtr tag = parseTag(input)) {
            if (!str.empty()) {
                retval->append(str);
                str = "";
            }
            retval->append(tag);
        } else if (!stopAt.isDone(*input)) {
            str += input.next();
        } else {
            break;
        }
    }
    if (!str.empty())
        retval->append(str);
    return retval;
}
            

ParserResult Parser::parse(const std::string &str) {
    Input input(str);
    ParseTerminator stopAtEof;
    ContentPtr content = parseContent(input, stopAtEof);
    content = content->eval();
    return ParserResult(content);
}

} // namespace
} // namespace
