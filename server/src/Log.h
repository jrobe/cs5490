#ifndef __Log_h__
#define __Log_h__

#include <iostream>
#include "Globals.h"

class Log 
{
    public:
    enum Level 
    {
        DEBUG,
        INFO,
        ERROR
    };

        Log(Level lev);
        ~Log();
        

    static void log(Level lev, const std::string& message);
    static std::string levelToString(Level level);

    //TODO :Fix this so you can chain <<
    void operator<<(const std::string& msg);

    private:
        Level _level;

};

extern Log __debugLog;
extern Log __errorLog;
extern Log __infoLog;


#define logDebug __debugLog
#define logError __errorLog
#define logInfo  __infoLog

#endif
