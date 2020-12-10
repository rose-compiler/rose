#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"                                 // every librose .C file must start with this

#include "FunctionIdDatabaseInterface.h"

using namespace std;
using namespace sqlite3x;
using namespace LibraryIdentification;
using namespace Sawyer::Message::Common;
using namespace Rose::Diagnostics;

enum DUPLICATE_OPTION LibraryIdentification::duplicateOptionFromString(std::string option)
{
    char opChar = ::toupper(option[0]);
    if(opChar == 'C') {
        return COMBINE;
    } 
    if(opChar == 'R') {
        return REPLACE;
    } 
    if(opChar == 'N') {
        return NO_ADD;
    } 
    return UNKNOWN;    
}



FunctionIdDatabaseInterface::FunctionIdDatabaseInterface(const std::string& dbName) 
{
    database_name = dbName;
    //open the database
    sqConnection.open(database_name.c_str());
    sqConnection.setbusytimeout(1800 * 1000); // 30 minutes
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
        sqConnection.executenonquery("CREATE TABLE IF NOT EXISTS functions(functionID TEXT KEY, function_name TEXT, libraryID TEXT)");
        //libraries: id hash, name, version, ISA, and time as an int
        //May also want calling convention and compile flags
        sqConnection.executenonquery("CREATE TABLE IF NOT EXISTS libraries(libraryID TEXT PRIMARY KEY, library_name TEXT, library_version TEXT, architecture TEXT, time UNSIGNED BIG INTEGER)");
    }
    catch(exception &ex) {
        mlog[ERROR] << "Exception Occurred: " << ex.what() << endl;
    }
    
    /*    try {
        sqConnection.executenonquery("create index if not exists vectors_by_md5 on vectors(md5_sum)");
    }
    catch(exception &ex) {
        mlog[ERROR] << "Exception Occurred: " << ex.what() << endl;
        }*/
    
};

// @brief Add an entry for a function to the database
void FunctionIdDatabaseInterface::addFunctionToDB(const FunctionInfo& fInfo, enum DUPLICATE_OPTION dupOption) 
{
    vector<FunctionInfo> foundFunctions = matchFunction(fInfo);
    
    if(foundFunctions.size() > 0) 
        {
            if(dupOption == NO_ADD) 
                {
                    LibraryInfo checkLib(foundFunctions[0].libHash);
                    mlog[ERROR] << "INSERT of " << fInfo.funcName << " failed as duplicate hash was found in the database, and NO_ADD was set" << endl;
                    mlog[ERROR] << "         The duplicate was caused by " << foundFunctions[0].funcName << " of library: " << checkLib.toString() << endl;
                    return;
                }
            if(dupOption == REPLACE) 
                {//In order to replace, we have to delete all functions with that hash
                    removeFunctions(fInfo.funcHash);
                }
            else //Finally, even if we're adding, we need to make sure
                 //there isn't an exact match for this function 
                { //If there is an exact match, bail
                    if(exactMatchFunction(fInfo))
                        {
                            LibraryInfo checkLib(fInfo.libHash);
                            mlog[ERROR] << "INSERT of " << fInfo.funcName << " failed as duplicate function was found in the database" << endl;
                            mlog[ERROR] << "         The duplicate was caused by " << foundFunctions[0].funcName << " of library: " << checkLib.toString() << endl;
                            return;
                        }
                }
        }
    

    string db_select_n = "INSERT INTO functions( functionId, function_name, libraryId ) VALUES(?,?,?);";
    
    sqlite3_command cmd(sqConnection, db_select_n.c_str());
    cmd.bind(1, fInfo.funcHash );
    cmd.bind(2, fInfo.funcName);
    cmd.bind(3, fInfo.libHash);
    cmd.executenonquery();
}

/** @brief Removes any functions that match the hash
 *  @param[inout] The hash to remove from the database
 **/
void FunctionIdDatabaseInterface::removeFunctions(const std::string& funcHash) 
{
    std::string db_select_n = "delete from functions where functionID = ?;";
    sqlite3_command cmd(sqConnection, db_select_n );
    
    cmd.bind(1, funcHash);
    
    sqlite3_reader sqReader = cmd.executereader();
 
    while(sqReader.read());
}


/** @brief See if any function with this hash exists in the database.
 *  If so, return the first one
 *  @param[inout] fInfo The FunctionInfo only needs to
 *  contain the hash, the rest will be filled in.
 *  @return True if a function was found.
 **/
bool FunctionIdDatabaseInterface::matchOneFunction(FunctionInfo& fInfo) 
{
    std::string db_select_n = "select function_name, libraryId FROM functions where functionId=?;";
    sqlite3_command cmd(sqConnection, db_select_n );
    
    cmd.bind(1, fInfo.funcHash);
    
    sqlite3_reader sqReader = cmd.executereader();
    
    
    if(sqReader.read()) { //entry found in database
        fInfo.funcName = (std::string)(sqReader.getstring(0));
        fInfo.libHash = (std::string)(sqReader.getstring(1));

        return true;
    }
    
    return false;
}


/** @brief Lookup a function by hash in the database. All match
 * functions will be returned in vector
 *  
 *  @param[in] funcHash The hash to search for
 *  @return  All functions matching this hash will be returned
 **/
vector<FunctionInfo> FunctionIdDatabaseInterface::matchFunction(const FunctionInfo& inFInfo) 
{
    std::string db_select_n = "select function_name, libraryId FROM functions where functionId=?;";
    sqlite3_command cmd(sqConnection, db_select_n );
    
    cmd.bind(1, inFInfo.funcHash);
    
    sqlite3_reader sqReader = cmd.executereader();
    
    vector<FunctionInfo> funcVector;

    while(sqReader.read()) { //entry found in database
        FunctionInfo fInfo(inFInfo);
        fInfo.funcName = (std::string)(sqReader.getstring(0));
        fInfo.libHash = (std::string)(sqReader.getstring(1));
        funcVector.push_back(fInfo);
    }
    
    return funcVector;
}

/** @brief Exactly lookup a function in the database.  There should
 *  only be one that matches Id, name, and library hash
 *  @param[inout] fInfo This FunctionInfo needs to
 *  contain the hash, name, and library hash. There should only be one
 *  matching function in the database.
 *  @return true if found
 **/
bool FunctionIdDatabaseInterface::exactMatchFunction(const FunctionInfo& fInfo)
{
    std::string db_select_n = "select * FROM functions where functionId = ? and function_name = ? and libraryId = ?;";
    sqlite3_command cmd(sqConnection, db_select_n );
    
    cmd.bind(1, fInfo.funcHash);
    cmd.bind(2, fInfo.funcName);
    cmd.bind(3, fInfo.libHash);
    
    sqlite3_reader sqReader = cmd.executereader();
    
    vector<FunctionInfo> funcVector;

    while(sqReader.read()) { //entry found in database
        FunctionInfo throwaway((std::string)sqReader.getstring(0));
        throwaway.funcName = (std::string)(sqReader.getstring(1));
        throwaway.libHash = (std::string)(sqReader.getstring(2));
        
        ROSE_ASSERT(throwaway == fInfo);

        //Only one exact match can exist in the database
        if(sqReader.read()) 
            {  
                mlog[ERROR] << "Duplicate entries for function: " << fInfo.funcName << " at hash: " << fInfo.funcHash<< " in the database. Exiting."  << endl;
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
     
    sqlite3_command cmd(sqConnection, db_select_n.c_str());
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
    sqlite3_command cmd(sqConnection, db_select_n );
    
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

#endif
