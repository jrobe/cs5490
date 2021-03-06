#include "AuthDB.h"

#include <functional>
#include <future>
#include <cassert>
#include <cstring>
#include <sstream>
#include <vector>

#include "Utils.h"

//Horrible; forces this to be a singleton
//Single threaded at that.
std::function<void (int,char**,char**)> AuthDB::_currentHandler = [] (int,char**,char**) {};
#define resetHandler _currentHandler = [] (int argc,char**,char**) {};

AuthDB::AuthDB(const std::string& dbfile)
{
    int rc = sqlite3_open(dbfile.c_str(),&_db);
    if(rc)
    {
        logError << (std::string("Failed to open SQLITE3 database: ") + dbfile);
        _db = nullptr;
    }
    else
    {
        logInfo << (std::string("Database opened successfully (") +  dbfile + std::string(")"));
    }


    if(_db)
        _init();
}


static int _callback(void*, int argc, char** argv, char** colName)
{
    //Hacky; but works for now logDebug << "Calling Handler";
    //This could be remedied nicely if you were able to pass a lambda as a function pointer easier than the hacks I've seen.
    //For now, and because I don't care, just go ahead and assume you're a single-threaded singleton
    AuthDB::_currentHandler(argc,argv,colName);
    return 0;
}

//Create tables if not exist

void AuthDB::_init()
{
   std::string stmt = "SELECT COUNT(type) from sqlite_master WHERE type='table' and name = 'users';";
   std::promise<bool> tableCheck;

    char* zErrMsg = nullptr;

    _currentHandler = 
    [&tableCheck] (int argc, char** argv,char**azColName)
    {
        assert(argc == 1);
        tableCheck.set_value(!strcmp(argv[0],"0"));
        logDebug << std::string("Output: ") + argv[0];
        return 1;
    };

    if(_execute(stmt))
    {
        auto fut = tableCheck.get_future();    
        if(fut.get() == true)
        {
            logInfo << "Need to create table";
            std::string createTable = "CREATE TABLE users (username text not null primary key, data text not null)";
            resetHandler;
            _execute(createTable);

        }
        else
        {
            logDebug << "table already exists...";
        }
    }

}

AuthDB::~AuthDB()
{
    if(_db)
        sqlite3_close(_db);
}

bool AuthDB::_execute(const std::string& stmt)
{
    logDebug << std::string("Executing ") + stmt + "...";
    char* zErrMsg = nullptr;
    int rc = sqlite3_exec(_db,stmt.c_str(),_callback,0,&zErrMsg);
    if(rc != SQLITE_OK)
    {
        logError << std::string("Failed to execute statement: ") + stmt + std::string(" error: ") + zErrMsg;
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;

}

bool AuthDB::_execute(sqlite3_stmt* stmt)
{
    //logDebug << std::string("Executing ") + stmt + "...";
    logDebug << "Executing prepared";
    char* zErrMsg = nullptr;
    int rc;
    while(rc = sqlite3_step(stmt) == SQLITE_ROW)
    {
        logDebug << "Parsing result";
        char** row = new char*[sqlite3_data_count(stmt)];
        //Copy the way it's done in sqlite3_exec
        // Apparently no row count...
        for(int  i = 0 ; i < sqlite3_data_count(stmt); i++)
        {
            const unsigned char* tmp = sqlite3_column_text(stmt,0);
            int sLen = strlen((char*)tmp);
            row[i] = new char[sLen + 1];
            memcpy(row[i],tmp,sLen);
            row[i][sLen] = '\0'; 
        }
        // Misunderstood this function; callback gets called multiple times.
        // Sqlite was a bad choice; the interface is horrible
        logDebug << "Calling prepared handler";
        _currentHandler(sqlite3_data_count(stmt),row,nullptr);
        for(int i =0 ; i < sqlite3_data_count(stmt); i++)
        {
            delete[] row[i];
        }
        delete[] row;


    }
    if(rc != SQLITE_OK)
    {
        //logError << (std::string("Failed to execute statement: ") + stmt + std::string(" error: ") + zErrMsg);
        logError << zErrMsg;
        sqlite3_free(zErrMsg);
    }

    logDebug << "Finalizing statement";
    sqlite3_finalize(stmt);
    return rc == SQLITE_OK;

}

bool AuthDB::checkIfUserExists(const std::string& user)
{
    std::promise<bool> existPromise;
    auto fut = existPromise.get_future();
    _currentHandler = [&existPromise] (int argc, char** argv, char**)
    {
        logDebug << "Handler called";
        assert(argc == 1);
        logDebug << std::string("resulting existing users: ") + argv[0];
        existPromise.set_value(!strcmp(argv[0],"1"));
    };

    std::stringstream ss;
    ss << "SELECT COUNT(username) FROM users where username = '" << user << "';";
    //WHAT: SQLITE doesn't call the callback if the return is null; stupid
    std::string checkStmt = ss.str();
    if(_execute(checkStmt))
    {
        return fut.get();
    }
    else
    {
        logError << "Internal error when checking for existing user";
        return false;
    }

}

std::string AuthDB::insertUser(const std::string& user, const std::string& encryptedData)
{
    logDebug << "Inserting user into the database...";
    //Check if user exists first
    if(checkIfUserExists(user))
    {
        logError << "User already exists!";
        return "User already exists!";
    }
    else
    {
        logDebug << "User didn't exist, inserting";
        resetHandler;
        sqlite3_stmt* insertStmt = NULL;
        std::string insert = "INSERT INTO users VALUES (?1,?2);";
        sqlite3_prepare_v2(_db,insert.c_str(),-1,&insertStmt,NULL);
        assert(insertStmt);
        sqlite3_bind_text(insertStmt,1,user.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(insertStmt,2,encryptedData.c_str(),-1,SQLITE_STATIC);
        if(!_execute(insertStmt))
        {
            logError << "Failed to insert user!"; 
            return "Failed to insert user";
        }
        else
        {
            logDebug << "User inserted";
            return "";
        }
    }

}
char* AuthDB::getEncryptedUserData(const std::string& user,int& len)
{
    resetHandler;
    std::promise<std::string> existPromise;
    auto fut = existPromise.get_future();
    if(!checkIfUserExists(user))
    {
        logError << "User didn't exist when attempting to get user data";
        return nullptr;
    }
    _currentHandler = [&existPromise] (int argc, char** argv, char**)
    {
        logDebug << "Handler called for getting encrypted user data";
        assert(argc == 1);
        logDebug << std::string("resulting data: ") + argv[0];
        existPromise.set_value(std::string(argv[0]));
    };

    logDebug << "Grabbing encrypted user Data";
    sqlite3_stmt* stmt = nullptr;
    std::string stmtStr = "SELECT data from users WHERE userName = ?1";
    sqlite3_prepare_v2(_db,stmtStr.c_str(),-1,&stmt,NULL);

    assert(stmt);

    sqlite3_bind_text(stmt,1,user.c_str(),-1,SQLITE_STATIC);
    if(!_execute(stmt))
    {
        logError << "Failed to get user data!";
        return nullptr;
    }
    else
    {
        //Got data
        logDebug << "Data retrieved from database";
        std::string outData = fut.get();
        char* out = new char[outData.size() + 1];
        memcpy(out,outData.c_str(),outData.size());
        out[outData.size()] = '\0';
        len = outData.size();
        logDebug << "Returning from getEncrypted";
        return out;

    }

}
