#ifndef __Log_h__
#define __Log_h__

class Log 
{
    public:
        Log(Level lev);
        ~Log();

    enum Level 
    {
        DEBUG,
        INFO,
        ERROR
    };

        

    static void log(Level lev, const std::string& message);
    static std::string levelToString(Level level);

    std::istream& operator<<(std::istream&, const std::string& msg);

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
