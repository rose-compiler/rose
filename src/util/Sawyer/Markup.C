// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#include <Sawyer/Markup.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <Sawyer/Sawyer.h>
#include <stdexcept>
#include <sstream>

namespace Sawyer {
namespace Markup {

std::ostream& operator<<(std::ostream &o, const Content &c) { c.print(o); return o; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Input
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Consume N characters and return them.
SAWYER_EXPORT std::string
Input::consume(size_t nchars) {
    size_t begin = at_;
    at_ = std::min(at_+nchars, content_.size());
    return content_.substr(begin, at_-begin);
}

// Skip over white space.  Returns the characters skipped.  Backslashed escaped white space doesn't count.
SAWYER_EXPORT std::string
Input::consumeSpace() {
    size_t begin = at_;
    while (isspace(cur())) inc();
    return content_.substr(begin, at_-begin);
}

// Consume input until one of the specified characters is reached, returning the consumed characters.  The matched character is
// not consumed.  Only matches characters that are not escaped.
SAWYER_EXPORT std::string
Input::consumeUntil(const char *stopSet) {
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

// Check that argument declarations are consistent; throw an exception if not.
SAWYER_EXPORT void
Tag::checkArgDecls() const {
    if (type() == VERBATIM)
        throw std::runtime_error("tag '"+name()+"' cannot be of type 'VERBATIM'; only arguments can be verbatim");
    for (size_t i=0; i<argDecls_.size(); ++i) {
        if (type() == SPANNING && argType(i) == DIVIDING) {
            throw std::runtime_error("spanning tag '"+name()+"' cannot have a dividing argument (argument " +
                                     boost::lexical_cast<std::string>(i+1) + ")");
        }
    }
}

// Default tag evaluator evaluates each argument and returns the same tag but with evaluated arguments.
SAWYER_EXPORT Content::Ptr
Tag::eval(const TagArgs &args) {
    TagArgs evaluatedArgs;
    BOOST_FOREACH (const Content::Ptr &arg, args)
        evaluatedArgs.push_back(arg->eval());
    TagInstance::Ptr ti = TagInstance::instance(self(), evaluatedArgs);
    Content::Ptr retval = Content::instance();
    retval->append(ti);
    return retval;
}

// NullTag has no arguments, so just return self
SAWYER_EXPORT Content::Ptr
NullTag::eval(const TagArgs &args) {
    TagInstance::Ptr ti = TagInstance::instance(self());
    Content::Ptr retval = Content::instance();
    retval->append(ti);
    return retval;
}

// Comments are non-evaluating
// FIXME[Robb Matzke 2014-02-23]: actually, they should evaluate to the textual representation of their argument so that
// something like "@comment{@b{abc}}" is the string "@b{abc}" (not the bold tag with argument "abc").
SAWYER_EXPORT Content::Ptr
CommentTag::eval(const TagArgs &args) {
    TagInstance::Ptr t1 = TagInstance::instance(self(), args);
    Content::Ptr retval = Content::instance();
    retval->append(t1);
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Tag instances - tag + arguments
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


SAWYER_EXPORT Content::Ptr
TagInstance::eval() const {
    return tag_->eval(args_);
}

SAWYER_EXPORT void
TagInstance::emit(std::ostream &stream, const Formatter::Ptr &formatter) const {
    if (NullTag::Ptr nullTag = tag_.dynamicCast<NullTag>()) {
        formatter->text(stream, nullTag->text());
    } else {
        if (formatter->beginTag(stream, tag_, args_)) {
            BOOST_FOREACH (const Content::Ptr &arg, args_)
                arg->emit(stream, formatter);
        }
        formatter->endTag(stream, tag_, args_);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Content sequence
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
Content::append(const std::string &s) {
    TagInstance::Ptr tag = TagInstance::instance(NullTag::instance(s));
    append(tag);
}

// Traverses this content and rewrites it so adjacent list items are grouped into a ListTag.
SAWYER_EXPORT Content::Ptr
Content::fixupLists() const {
    Content::Ptr retval = Content::instance();
    TagInstance::Ptr list;
    Tag::Ptr prevListItemTag;

    for (size_t i=0; i<elmts_.size(); ++i) {
        // Recursively fixup children
        TagInstance::Ptr elmtFixed = TagInstance::instance(elmts_[i]->tag());
        BOOST_FOREACH (const Content::Ptr &arg, elmts_[i]->args()) {
            Content::Ptr argFixed = arg->fixupLists();
            elmtFixed->append(argFixed);
        }

        // If data is only white space and appears between two list items, then consume it.
        if (NullTag::Ptr data = elmtFixed->tag().dynamicCast<NullTag>()) {
            if (list && i+1<elmts_.size() && elmts_[i+1]->tag().dynamicCast<ListItemTag>()) {
                bool allSpace = true;
                for (size_t i=0; allSpace && i<data->text().size(); ++i)
                    allSpace = isspace(data->text()[i]) ? true : false; // avoid Microsoft C++ warning
                if (allSpace) {
                    list->args().back()->append(elmtFixed);
                    continue;
                }
            }
        }

        // Accumulate list items into lists
        if (elmtFixed->tag().dynamicCast<ListItemTag>()) {
            if (!list || (prevListItemTag && prevListItemTag->name() != elmtFixed->tag()->name())) {
                if (list)
                    retval->append(list);
                list = TagInstance::instance(ListTag::instance());
            }
            Content::Ptr listItem = Content::instance();
            listItem->append(elmtFixed);
            list->append(listItem);
            prevListItemTag = elmtFixed->tag();
        } else if (list) {
            retval->append(list);
            list = TagInstance::Ptr();
            retval->append(elmtFixed);
            prevListItemTag = elmtFixed->tag();
        } else {
            retval->append(elmtFixed);
            prevListItemTag = Tag::Ptr();
        }
    }
    if (list)
        retval->append(list);
    return retval;
}

// Evaluate each of the tag instances and concatenate all the results together.
SAWYER_EXPORT Content::Ptr
Content::eval() const {
    Content::Ptr retval = Content::instance();
    BOOST_FOREACH (const TagInstance::Ptr &elmt, elmts_)
        retval->append(elmt->eval());
    return retval;
}

SAWYER_EXPORT void
Content::emit(std::ostream &stream, const Formatter::Ptr &formatter) const {
    BOOST_FOREACH (const TagInstance::Ptr &elmt, elmts_)
        elmt->emit(stream, formatter);
}

// Text representation of content
SAWYER_EXPORT std::string
Content::asText() const {
    std::ostringstream ss;
    Formatter::Ptr fmt = TextFormatter::instance();
    emit(ss, fmt);
    return ss.str();
}

SAWYER_EXPORT void
Content::print(std::ostream &o) const {
    BOOST_FOREACH (const TagInstance::Ptr &elmt, elmts_) {
        if (NullTag::Ptr data = elmt->tag().dynamicCast<NullTag>()) {
            o <<"DATA{" <<data->text() <<"}";
        } else {
            o <<"@" <<elmt->tag()->name();
            BOOST_FOREACH (const Content::Ptr &arg, elmt->args()) {
                o <<"{";
                arg->print(o);
                o <<"}";
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Parser result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
ParserResult::emit(std::ostream &stream, const Formatter::Ptr &formatter) const {
    formatter->beginDocument(stream);
    content_->emit(stream, formatter);
    formatter->endDocument(stream);
}

SAWYER_EXPORT void
ParserResult::print(std::ostream &stream) const {
    content_->print(stream);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Parser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



SAWYER_EXPORT void
Parser::init() {
    // Dividing tags
    registerTag(ProseTag::instance(),           "prose");
    registerTag(NonProseTag::instance(),        "nonprose");
    registerTag(SectionTag::instance(),         "section");
    registerTag(BulletTag::instance(),          "bullet");
    registerTag(NumberedTag::instance(),        "numbered");
    registerTag(NamedTag::instance(),           "named");

    // Spanning tags
    registerTag(CommentTag::instance(),         "comment");
    registerTag(EmphasisTag::instance(),        "em");
    registerTag(VariableTag::instance(),        "var");
    registerTag(VariableTag::instance(),        "v");
    registerTag(LinkTag::instance(),            "link");
    registerTag(LinkTag::instance(),            "a");
    registerTag(VerbatimTag::instance(),        "c");
}

SAWYER_EXPORT Parser&
Parser::registerTag(const Tag::Ptr &tag, const std::string &name) {
    symtab_.insert(name.empty() ? tag->name() : name, tag);
    return *this;
}

    

// FIXME[Robb Matzke 2014-02-22]: make this much more informative
SAWYER_EXPORT std::string
Parser::location(const Input &input) {
    if (input.atEnd())
        return "end of input";
    std::ostringstream ss;
    ss <<"byte " <<input.at();
    return ss.str();
}

SAWYER_EXPORT void
Parser::warn(const std::string &mesg) {
    throw std::runtime_error(mesg);
}

SAWYER_EXPORT char
Parser::closingFor(char opening) {
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
SAWYER_EXPORT std::string
Parser::parseSymbol(Input &input) {
    std::string name;
    if (input.atEnd() || !isalpha(*input))
        return name;
    name = input.next();
    while (isalnum(*input))
        name += input.next();
    return name;
}

// Parse a word, such as appears for a single-word argument.
SAWYER_EXPORT std::string
Parser::parseWord(Input &input) {
    std::string word;
    while (!input.atEnd() && (isalnum(*input) || '-'==*input))
        word += input.next();
    return word;
}

// Parse input as a string, counting left and right delimiters and stopping just before we see a right delimiter that
// is not balanced with an earlier parsed left delimiter.  For instance, if the left delimiter is "(" and the right delimiter
// is ")" then given the input "one (two) three) four" the parser will stop prior to consuming the second right paren.
SAWYER_EXPORT std::string
Parser::parseNested(Input &input, char left, char right) {
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
SAWYER_EXPORT Content::Ptr
Parser::parseTagArgument(Input &input, DivSpan allowed) {
    ExcursionGuard guard(input);
    if (input.atEnd())
        return ContentPtr();

    Content::Ptr retval;
    char opening = input.next();
    if (isspace(opening)) {
        // Argument must be a single word, like "don't @em ever do that!" The space before the word is discarded.
        input.consumeSpace();
        std::string word = parseWord(input);
        if (word.empty())
            return ContentPtr();
        retval = Content::instance();
        retval->append(word);
    } else if (char closing = closingFor(opening)) {
        // Argument is enclosed in {}, (), [], or <>
        ParenTerminator stopAt(opening, closing);
        retval = parseContent(input, stopAt, allowed);
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
SAWYER_EXPORT TagInstance::Ptr
Parser::parseTag(Input &input, DivSpan allowed) {
    ExcursionGuard guard(input);
    if ('@'!=input.next())
        return TagInstance::Ptr();
    std::string tagName = parseSymbol(input);
    Tag::Ptr tag = symtab_.getOrDefault(tagName);
    if (!tag)
        return TagInstance::Ptr();
    if (allowed==SPANNING && tag->type()==DIVIDING) {
        throw std::runtime_error("'"+tag->name()+"' is not allowed inside a paragraph at " + location(input));
    }
    TagInstance::Ptr retval = TagInstance::instance(tag);
    for (size_t argno=0; argno<tag->nArgsDeclared(); ++argno) {
        if (Content::Ptr arg = parseTagArgument(input, tag->argType(argno))) {
            retval->append(arg);
        } else {
            warn("expected " + tag->name() + " argument " + boost::lexical_cast<std::string>(argno+1) + " at " +
                 location(input));
            return TagInstance::Ptr();
        }
    }
    guard.cancel();
    return retval;
}

// Parse text, including top-level tag instances.  Text that is not enclosed in a tag is returned as if it were inside a null
// tag.   For instance, if the input is "1 @c{2} 3" will parse as if it were "@{1 }@c{2}@{ 3}".  If @p allowed is DIVIDING then
// any type of tags are allowed; if it is SPANNING then only spanning tags are allowed.
SAWYER_EXPORT Content::Ptr
Parser::parseContent(Input &input, ParseTerminator &stopAt, DivSpan allowed) {
    Content::Ptr retval = Content::instance();
    std::string str;
    TagInstance::Ptr tag;
    while (!input.atEnd()) {
        if ('\\'==*input) {
            ++input;
            str = input.next();
        } else if (allowed != VERBATIM && (tag = parseTag(input, allowed))) {
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

SAWYER_EXPORT ParserResult
Parser::parse(const std::string &str) {
    Input input(str);
    ParseTerminator stopAtEof;
    Content::Ptr content = parseContent(input, stopAtEof, DIVIDING);
    content = content->eval();
    content = content->fixupLists();
#if 0 // DEBUGGING [Robb Matzke 2014-06-14]
    content->print(std::cerr);
#endif
    return ParserResult(content);
}

} // namespace
} // namespace
