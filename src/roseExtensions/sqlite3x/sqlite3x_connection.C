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

sqlite3_connection::sqlite3_connection() : db(NULL) {}

sqlite3_connection::sqlite3_connection(const char *db) : db(NULL) { this->open(db); }

sqlite3_connection::sqlite3_connection(const wchar_t *db) : db(NULL) { this->open(db); }

sqlite3_connection::~sqlite3_connection() { if(this->db) sqlite3_close(this->db); }

void sqlite3_connection::open(const char *db) {
        if(sqlite3_open(db, &this->db)!=SQLITE_OK)
                throw database_error("unable to open database");
}

void sqlite3_connection::open(const wchar_t *db) {
        if(sqlite3_open16(db, &this->db)!=SQLITE_OK)
                throw database_error("unable to open database");
}

void sqlite3_connection::close() {
        if(this->db) {
                if(sqlite3_close(this->db)!=SQLITE_OK)
                        throw database_error(*this);
                this->db=NULL;
        }
}

long long sqlite3_connection::insertid() {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_last_insert_rowid(this->db);
}

void sqlite3_connection::setbusytimeout(int ms) {
        if(!this->db) throw database_error("database is not open");

        if(sqlite3_busy_timeout(this->db, ms)!=SQLITE_OK)
                throw database_error(*this);
}

void sqlite3_connection::executenonquery(const char *sql) {
        if(!this->db) throw database_error("database is not open");
        sqlite3_command(*this, sql).executenonquery();
}

void sqlite3_connection::executenonquery(const wchar_t *sql) {
        if(!this->db) throw database_error("database is not open");
        sqlite3_command(*this, sql).executenonquery();
}

void sqlite3_connection::executenonquery(const std::string &sql) {
        if(!this->db) throw database_error("database is not open");
        sqlite3_command(*this, sql).executenonquery();
}

void sqlite3_connection::executenonquery(const std::wstring &sql) {
        if(!this->db) throw database_error("database is not open");
        sqlite3_command(*this, sql).executenonquery();
}

int sqlite3_connection::executeint(const char *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint();
}

int sqlite3_connection::executeint(const wchar_t *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint();
}

int sqlite3_connection::executeint(const std::string &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint();
}

int sqlite3_connection::executeint(const std::wstring &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint();
}

long long sqlite3_connection::executeint64(const char *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint64();
}

long long sqlite3_connection::executeint64(const wchar_t *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint64();
}

long long sqlite3_connection::executeint64(const std::string &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint64();
}

long long sqlite3_connection::executeint64(const std::wstring &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeint64();
}

double sqlite3_connection::executedouble(const char *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executedouble();
}

double sqlite3_connection::executedouble(const wchar_t *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executedouble();
}

double sqlite3_connection::executedouble(const std::string &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executedouble();
}

double sqlite3_connection::executedouble(const std::wstring &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executedouble();
}

std::string sqlite3_connection::executestring(const char *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring();
}

std::string sqlite3_connection::executestring(const wchar_t *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring();
}

std::string sqlite3_connection::executestring(const std::string &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring();
}

std::string sqlite3_connection::executestring(const std::wstring &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring();
}

std::wstring sqlite3_connection::executestring16(const char *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring16();
}

std::wstring sqlite3_connection::executestring16(const wchar_t *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring16();
}

std::wstring sqlite3_connection::executestring16(const std::string &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring16();
}

std::wstring sqlite3_connection::executestring16(const std::wstring &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executestring16();
}

std::string sqlite3_connection::executeblob(const char *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeblob();
}

std::string sqlite3_connection::executeblob(const wchar_t *sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeblob();
}

std::string sqlite3_connection::executeblob(const std::string &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeblob();
}

std::string sqlite3_connection::executeblob(const std::wstring &sql) {
        if(!this->db) throw database_error("database is not open");
        return sqlite3_command(*this, sql).executeblob();
}

}
