#include "Log.h"


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
    std::cout << levelToString(lev) << ": " << message << std::endl;
}
std::istream& Log::operator<<(std::istream&, const std::string& msg)
{
    log(_level,msg);
}
