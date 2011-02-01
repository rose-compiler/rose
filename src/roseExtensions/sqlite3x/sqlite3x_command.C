/*
        Copyright (C) 2004-2005 Cory Nelson

        This software is provided 'as-is', without any express or implied
        warranty.  In no event will the authors be held liable for any damages
        arising from the use of this software.

        Permission is granted to anyone to use this software for any purpose,
        including commercial applications, and to alter it and redistribute it
        freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
                claim that you wrote the original software. If you use this software
                in a product, an acknowledgment in the product documentation would be
                appreciated but is not required.
        2. Altered source versions must be plainly marked as such, and must not be
                misrepresented as being the original software.
        3. This notice may not be removed or altered from any source distribution.
        
        CVS Info :
                $Author: phrostbyte $
                $Date: 2005/06/16 20:46:40 $
                $Revision: 1.1 $
*/

#include <sqlite3.h>
#include "sqlite3x.h"

namespace sqlite3x {

sqlite3_command::sqlite3_command(sqlite3_connection &con, const char *sql) : con(con),refs(0) {
        const char *tail=NULL;
        if(sqlite3_prepare(con.db, sql, -1, &this->stmt, &tail)!=SQLITE_OK)
                throw database_error(con);

        this->argc=sqlite3_column_count(this->stmt);
}

sqlite3_command::sqlite3_command(sqlite3_connection &con, const wchar_t *sql) : con(con),refs(0) {
        const wchar_t *tail=NULL;
        if(sqlite3_prepare16(con.db, sql, -1, &this->stmt, (const void**)&tail)!=SQLITE_OK)
                throw database_error(con);

        this->argc=sqlite3_column_count(this->stmt);
}

sqlite3_command::sqlite3_command(sqlite3_connection &con, const std::string &sql) : con(con),refs(0) {
        const char *tail=NULL;
        if(sqlite3_prepare(con.db, sql.data(), (int)sql.length(), &this->stmt, &tail)!=SQLITE_OK)
                throw database_error(con);

        this->argc=sqlite3_column_count(this->stmt);
}

sqlite3_command::sqlite3_command(sqlite3_connection &con, const std::wstring &sql) : con(con),refs(0) {
        const wchar_t *tail=NULL;
        if(sqlite3_prepare16(con.db, sql.data(), (int)sql.length()*2, &this->stmt, (const void**)&tail)!=SQLITE_OK)
                throw database_error(con);

        this->argc=sqlite3_column_count(this->stmt);
}

sqlite3_command::~sqlite3_command() {
        sqlite3_finalize(this->stmt);
}

void sqlite3_command::bind(int index) {
        if(sqlite3_bind_null(this->stmt, index)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, int data) {
        if(sqlite3_bind_int(this->stmt, index, data)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, long long data) {
        if(sqlite3_bind_int64(this->stmt, index, data)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, double data) {
        if(sqlite3_bind_double(this->stmt, index, data)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, const char *data, int datalen) {
        if(sqlite3_bind_text(this->stmt, index, data, datalen, SQLITE_TRANSIENT)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, const wchar_t *data, int datalen) {
        if(sqlite3_bind_text16(this->stmt, index, data, datalen, SQLITE_TRANSIENT)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, const void *data, int datalen) {
        if(sqlite3_bind_blob(this->stmt, index, data, datalen, SQLITE_TRANSIENT)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, const std::string &data) {
        if(sqlite3_bind_text(this->stmt, index, data.data(), (int)data.length(), SQLITE_TRANSIENT)!=SQLITE_OK)
                throw database_error(this->con);
}

void sqlite3_command::bind(int index, const std::wstring &data) {
        if(sqlite3_bind_text16(this->stmt, index, data.data(), (int)data.length()*2, SQLITE_TRANSIENT)!=SQLITE_OK)
                throw database_error(this->con);
}

sqlite3_reader sqlite3_command::executereader() {
        return sqlite3_reader(this);
}

void sqlite3_command::executenonquery() {
        this->executereader().read();
}

int sqlite3_command::executeint() {
        sqlite3_reader reader=this->executereader();
        if(!reader.read()) throw database_error("nothing to read");
        return reader.getint(0);
}

long long sqlite3_command::executeint64() {
        sqlite3_reader reader=this->executereader();
        if(!reader.read()) throw database_error("nothing to read");
        return reader.getint64(0);
}

double sqlite3_command::executedouble() {
        sqlite3_reader reader=this->executereader();
        if(!reader.read()) throw database_error("nothing to read");
        return reader.getdouble(0);
}

std::string sqlite3_command::executestring() {
        sqlite3_reader reader=this->executereader();
        if(!reader.read()) throw database_error("nothing to read");
        return reader.getstring(0);
}

std::wstring sqlite3_command::executestring16() {
        sqlite3_reader reader=this->executereader();
        if(!reader.read()) throw database_error("nothing to read");
        return reader.getstring16(0);
}

std::string sqlite3_command::executeblob() {
        sqlite3_reader reader=this->executereader();
        if(!reader.read()) throw database_error("nothing to read");
        return reader.getblob(0);
}

}
