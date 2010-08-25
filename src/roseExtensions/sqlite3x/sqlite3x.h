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

#ifndef __SQLITE3X_HPP__
#define __SQLITE3X_HPP__

#include <string>
#include <boost/utility.hpp>
#include <stdexcept>

namespace sqlite3x {
    class sqlite3_reader;
        class sqlite3_connection : boost::noncopyable {
        private:
                friend class sqlite3_command;
                friend class database_error;

                struct sqlite3 *db;

        public:
                sqlite3_connection();
                sqlite3_connection(const char *db);
                sqlite3_connection(const wchar_t *db);
                ~sqlite3_connection();

                void open(const char *db);
                void open(const wchar_t *db);
                void close();

                long long insertid();
                void setbusytimeout(int ms);

                void executenonquery(const char *sql);
                void executenonquery(const wchar_t *sql);
                void executenonquery(const std::string &sql);
                void executenonquery(const std::wstring &sql);

                int executeint(const char *sql);
                int executeint(const wchar_t *sql);
                int executeint(const std::string &sql);
                int executeint(const std::wstring &sql);

                long long executeint64(const char *sql);
                long long executeint64(const wchar_t *sql);
                long long executeint64(const std::string &sql);
                long long executeint64(const std::wstring &sql);

                double executedouble(const char *sql);
                double executedouble(const wchar_t *sql);
                double executedouble(const std::string &sql);
                double executedouble(const std::wstring &sql);

                std::string executestring(const char *sql);
                std::string executestring(const wchar_t *sql);
                std::string executestring(const std::string &sql);
                std::string executestring(const std::wstring &sql);

                std::wstring executestring16(const char *sql);
                std::wstring executestring16(const wchar_t *sql);
                std::wstring executestring16(const std::string &sql);
                std::wstring executestring16(const std::wstring &sql);

                std::string executeblob(const char *sql);
                std::string executeblob(const wchar_t *sql);
                std::string executeblob(const std::string &sql);
                std::string executeblob(const std::wstring &sql);
        };

        class sqlite3_transaction : boost::noncopyable {
        private:
                sqlite3_connection &con;
                bool intrans;

        public:
                sqlite3_transaction(sqlite3_connection &con, bool start=true);
                ~sqlite3_transaction();

                void begin();
                void commit();
                void rollback();
        };

        class sqlite3_command : boost::noncopyable {
        private:
                friend class sqlite3_reader;

                sqlite3_connection &con;
                struct sqlite3_stmt *stmt;
                unsigned int refs;
                int argc;

        public:
                sqlite3_command(sqlite3_connection &con, const char *sql);
                sqlite3_command(sqlite3_connection &con, const wchar_t *sql);
                sqlite3_command(sqlite3_connection &con, const std::string &sql);
                sqlite3_command(sqlite3_connection &con, const std::wstring &sql);
                ~sqlite3_command();

                void bind(int index);
                void bind(int index, int data);
                void bind(int index, long long data);
                void bind(int index, double data);
                void bind(int index, const char *data, int datalen);
                void bind(int index, const wchar_t *data, int datalen);
                void bind(int index, const void *data, int datalen);
                void bind(int index, const std::string &data);
                void bind(int index, const std::wstring &data);

                sqlite3_reader executereader();
                void executenonquery();
                int executeint();
                long long executeint64();
                double executedouble();
                std::string executestring();
                std::wstring executestring16();
                std::string executeblob();
        };

        class sqlite3_reader {
        private:
                friend class sqlite3_command;

                sqlite3_command *cmd;

                sqlite3_reader(sqlite3_command *cmd);

        public:
                sqlite3_reader();
                sqlite3_reader(const sqlite3_reader &copy);
                ~sqlite3_reader();

                sqlite3_reader& operator=(const sqlite3_reader &copy);

                bool read();
                void reset();
                void close();

                int getint(int index);
                long long getint64(int index);
                double getdouble(int index);
                std::string getstring(int index);
                std::wstring getstring16(int index);
                std::string getblob(int index);

                std::string getcolname(int index);
                std::wstring getcolname16(int index);
        };

        class database_error : 
          public 
          std::runtime_error {
        public:
                database_error(const char *msg);
                database_error(sqlite3_connection &con);
        };
}

#endif
