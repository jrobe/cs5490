#ifndef __AuthDB_h__
#define __AuthDB_h__

#include <iostream>
#include <sqlite3.h>

#include "Log.h"

class AuthDB
{
    public:
        AuthDB(const std::string& dbfile);
        ~AuthDB();
        bool good() const { return _db; }

    private:
        sqlite3* _db;

};


#endif
