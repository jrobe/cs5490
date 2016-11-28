#ifndef __AuthDB_h__
#define __AuthDB_h__

#include <iostream>
#include <sqlite3.h>
#include <map>
#include <functional>

#include "Log.h"

class AuthDB
{
    public:
        AuthDB(const std::string& dbfile);
        ~AuthDB();
        bool good() const { return _db; }

        //Hard coded functions
        std::string insertUser(const std::string& user, const std::string& encryptedData);
        

        static std::function< void (int,char**,char**)> _currentHandler;

    private:
        bool _execute(const std::string& stmt);
        bool _execute(sqlite3_stmt*);
        void _init();
        sqlite3* _db;

};


#endif
