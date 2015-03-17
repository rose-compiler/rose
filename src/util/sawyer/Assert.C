// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#include <sawyer/Assert.h>
#include <sawyer/Message.h>
#include <sawyer/Sawyer.h>

#include <cstdlib>

namespace Sawyer {
namespace Assert {

SAWYER_EXPORT void
fail(const char *mesg, const char *expr, const std::string &note, const char *filename, unsigned linenum, const char *funcname)
{
    if (!Message::assertionStream)
        Message::assertionStream = Message::mlog[Message::FATAL];

    *Message::assertionStream <<mesg <<":\n";
    if (filename && *filename)
        *Message::assertionStream <<"  " <<filename <<":" <<linenum <<"\n";
    if (funcname && *funcname)
        *Message::assertionStream <<"  " <<funcname <<"\n";
    if (expr && *expr)
        *Message::assertionStream <<"  " <<expr <<"\n";
    if (!note.empty())
        *Message::assertionStream <<"  " <<note <<"\n";
    abort();
}

} // namespace
} // namespace
