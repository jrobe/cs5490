#include "AuthDB.h"

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
}

AuthDB::~AuthDB()
{
    if(_db)
        sqlite3_close(_db);
}
