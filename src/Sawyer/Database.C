// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#include <Sawyer/Database.h>
#if __cplusplus >= 201103L

// When compiling this file, use C-preprocessor macros SAWYER_NO_SQLITE and/or SAWYER_NO_POSTGRESQL if you need to
// disable one or both of the drivers.

#ifndef SAWYER_NO_POSTGRESQL
#include <Sawyer/DatabasePostgresql.h>
#endif

#ifndef SAWYER_NO_SQLITE
#include <Sawyer/DatabaseSqlite.h>
#endif

#include <boost/regex.hpp>

namespace Sawyer {
namespace Database {

std::string
Connection::uriDocString() {
    return "The following URLs are recognized (if you don't see any listed here, it means that no drivers are configured)."

#ifndef SAWYER_NO_SQLITE
        "@named{SQLite}{The uniform resource locator for SQLite databases has the format "
        "\"sqlite://@v{filename}\" or just \"@v{filename}\"" "where @v{filename} is the name of a file in the local filesystem.}"
#endif

#ifndef SAWYER_NO_POSTGRESQL
        "@named{PostgreSQL}{The uniform resource locator for PostgreSQL databases has the format "
        "\"postgresql://[@v{user}[:@v{password}]@][@v{hostname}[:@v{port}]][/@v{database}]\" where @v{user} is the database user "
        "name; @v{password} is the user's password; @v{hostname} is the host name or IP address of the database "
        "server, defaulting to the localhost; @v{port} is the TCP port number at which the server listens; "
        "and @v{database} is the name of the database.}"
#endif
        ;
}

Connection
Connection::fromUri(const std::string &uri) {
    boost::regex sqlite1("sqlite://(.+)");
    boost::regex sqlite2("([^:@]+)");

    //                                   user       passwd    host       port        db
    boost::regex postgresql1("postgresql://([^:@/]+)(:([^@]*))?@([^:@/]+)(:([^:@/]+))?/(.+)");

    //                                   host      port        db
    boost::regex postgresql2("postgresql://([^:@/])(:([^:@/]+))?/(.+)");

    boost::smatch found;

#ifndef SAWYER_NO_SQLITE
    if (boost::regex_match(uri, found, sqlite1))
        return Sqlite(found.str(1));
#endif

#ifndef SAWYER_NO_POSTGRESQL
    if (boost::regex_match(uri, found, postgresql1)) {
        Postgresql::Locator loc;
        loc.user = found.str(1);
        loc.password = found.str(3);
        loc.hostname = found.str(4);
        loc.port = found.str(6);
        loc.database = found.str(7);
        return Postgresql(loc);
    }
    if (boost::regex_match(uri, found, postgresql2)) {
        Postgresql::Locator loc;
        loc.hostname = found.str(1);
        loc.port = found.str(3);
        loc.database = found.str(4);
        return Postgresql(loc);
    }
#endif

#ifndef SAWYER_NO_SQLITE
    if (boost::regex_match(uri, found, sqlite2))
        return Sqlite(found.str(1));
#endif

    throw Exception("invalid database URI: " + uri);
}

} // namespace
} // namespace

#endif
