#include <sawyer/Assert.h>

#include <cstdlib>

namespace Sawyer {
namespace Assert {

Message::SProxy assertionStream;                        // can't be initialized yet

void
fail(const char *mesg, const char *expr, const std::string &note, const char *filename, unsigned linenum, const char *funcname)
{
    if (!assertionStream)
        assertionStream = Message::mlog[Message::FATAL];

    *assertionStream <<mesg <<":\n";
    if (filename && *filename)
        *assertionStream <<"  " <<filename <<":" <<linenum <<"\n";
    if (funcname && *funcname)
        *assertionStream <<"  " <<funcname <<"\n";
    if (expr && *expr)
        *assertionStream <<"  " <<expr <<"\n";
    if (!note.empty())
        *assertionStream <<"  " <<note <<"\n";
    abort();
}

} // namespace
} // namespace
