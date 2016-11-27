#include "Log.h"


#include <ctime>

Log __debugLog(Log::DEBUG);
Log __errorLog(Log::ERROR);
Log __infoLog(Log::INFO);


Log::Log(Level lev)
: _level(lev)
{

}
Log::~Log()
{

}


std::string Log::levelToString(Level level)
{
    switch(level)
    {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Log::log(Level lev, const std::string& message)
{
    //For now
    time_t now;
    time(&now);
    char buf[sizeof"1991-03-18T07:08:09Z"];
    strftime(buf,sizeof(buf),"%FT%TZ",gmtime(&now));
    std::cout << buf << " " << levelToString(lev) << ": " << message << std::endl;
}
void Log::operator<<(const std::string& msg)
{
    log(_level,msg);
}

