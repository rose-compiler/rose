#include "sage3basic.h"                                 // every librose .C file must start with this

#include "FunctionIdDatabaseInterface.h"

using namespace std;
using namespace sqlite3x;
using namespace LibraryIdentification;
using namespace Sawyer::Message::Common;
using namespace Rose::Diagnostics;

FunctionIdDatabaseInterface::FunctionIdDatabaseInterface(std::string dbName) 
{
    database_name = dbName;
    //open the database
    con.open(database_name.c_str());
    con.setbusytimeout(1800 * 1000); // 30 minutes
    createTables();
    
}


/**
 * @brief Create the function ID database tables "functions" and
 * "libraries" if they don't already exist
 **/
void 
FunctionIdDatabaseInterface::createTables()
{
    try { //Functions: identifying hash, name, link to the source library
        con.executenonquery("CREATE TABLE IF NOT EXISTS functions(functionID TEXT PRIMARY KEY, function_name TEXT, libraryID TEXT)");
        //libraries: id hash, name, version, ISA, and time as an int
        //May also want calling convention and compile flags
        con.executenonquery("CREATE TABLE IF NOT EXISTS libraries(libraryID TEXT PRIMARY KEY, library_name TEXT, library_version TEXT, architecture TEXT, time UNSIGNED BIG INTEGER)");
    }
    catch(exception &ex) {
        mlog[ERROR] << "Exception Occurred: " << ex.what() << endl;
    }
    
    /*    try {
        con.executenonquery("create index if not exists vectors_by_md5 on vectors(md5_sum)");
    }
    catch(exception &ex) {
        mlog[ERROR] << "Exception Occurred: " << ex.what() << endl;
        }*/
    
};

// @brief Add an entry for a function to the database
void FunctionIdDatabaseInterface::addFunctionToDB(const FunctionInfo& fInfo, bool replace) 
{
    FunctionInfo checkFunc(fInfo.funcHash);
    if(matchFunction(checkFunc)) 
        {
            LibraryInfo checkLib(checkFunc.libHash);
            if(!matchLibrary(checkLib)) 
                {
                    mlog[ERROR] << "INSERT of " << fInfo.funcName << " failed as duplicate hash was found in the database." << endl;
                    mlog[ERROR] << "         The duplicate was caused by " << checkFunc.funcName << " but no library was found for hash " << checkLib.libHash << endl;
                    ASSERT_require(false);
                    
                }
            else 
                {
                    mlog[WARN] << "INSERT of " << fInfo.funcName << " failed as duplicate hash was found in the database." << endl;
                    mlog[WARN] << "         The duplicate was caused by " << checkFunc.funcName << " of " << checkLib.libName << endl;
                }
            if(!replace)
                return;  //Otherwise entry will be replaced
            
        }
    

    string db_select_n = "REPLACE INTO functions( functionId, function_name, libraryId ) VALUES(?,?,?);";
    
    sqlite3_command cmd(con, db_select_n.c_str());
    cmd.bind(1, fInfo.funcHash );
    cmd.bind(2, fInfo.funcName);
    cmd.bind(3, fInfo.libHash);
    cmd.executenonquery();
}


/** @brief Lookup a function in the database.  True returned if found
 *  @param[inout] fInfo The FunctionInfo only needs to
 *  contain the hash, the rest will be filled in.
 **/
bool FunctionIdDatabaseInterface::matchFunction(FunctionInfo& fInfo) 
{
    std::string db_select_n = "select function_name, libraryId FROM functions where functionId=?;";
    sqlite3_command cmd(con, db_select_n );
    
    cmd.bind(1, fInfo.funcHash);
    
    sqlite3_reader r = cmd.executereader();
    
    
    if(r.read()) { //entry found in database
        fInfo.funcName = (std::string)(r.getstring(0));
        fInfo.libHash = (std::string)(r.getstring(1));

        //Only one entry should exist
        if(r.read()) 
            {  
                mlog[WARN] << "Duplicate entries for function: " << fInfo.funcName << " at hash: " << fInfo.funcHash<< " in the database. Exiting."  << endl;
            }
        return true;
    }
    
    return false;
}



// @brief Add an entry for a library to the database
void FunctionIdDatabaseInterface::addLibraryToDB(const LibraryInfo& lInfo, bool replace) 
{
    LibraryInfo checkLib(lInfo.libHash);
    if(matchLibrary(checkLib)) 
        {
            mlog[ERROR] << "ERROR: INSERT of library: " << lInfo.libName << " failed as duplicate hash was found in the database." << endl;
            mlog[ERROR] << "         The duplicate was caused by " << checkLib.libName << " version: " << checkLib.libVersion << endl;
            if(!replace) 
                {
                    return;   
                }
        }
    string db_select_n = "INSERT INTO libraries ( libraryId, library_name, library_version, architecture, time) VALUES(?,?,?,?,?);";
     
    sqlite3_command cmd(con, db_select_n.c_str());
    cmd.bind(1, lInfo.libHash );
    cmd.bind(2, lInfo.libName);
    cmd.bind(3, lInfo.libVersion);
    cmd.bind(4, lInfo.architecture);
    cmd.bind(5, boost::lexical_cast<unsigned long int>(lInfo.analysisTime));
    cmd.executenonquery();
    
}


/** @brief Lookup a library in the database.  True returned if found
 *  @param[inout] lInfo The LibraryInfo only needs to
 *  contain the hash, the rest will be filled in.
          **/
bool FunctionIdDatabaseInterface::matchLibrary(LibraryInfo& lInfo) 
{
    std::string db_select_n = "select library_name, library_version, architecture, time from libraries where libraryId=?;";
    sqlite3_command cmd(con, db_select_n );
    
    cmd.bind(1, lInfo.libHash);
    
    sqlite3_reader r = cmd.executereader();
    
    if(r.read()) { //entry found in database
        lInfo.libName = (std::string)(r.getstring(0));
        lInfo.libVersion = (std::string)(r.getstring(1));
        lInfo.architecture = (std::string)(r.getstring(2));
        lInfo.analysisTime = (time_t)r.getint64(3);

        //Only one entry should exist
        if(r.read()) 
            {  
                mlog[WARN] << "Duplicate entries for library: " << lInfo.libName << " at hash: " << lInfo.libHash<< " in the database. Exiting."  << endl;
            }
        return true;
    }
    
    return false;
}


