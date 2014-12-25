#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>


class Logger
{
    std::ofstream file_;
 public:
    Logger(const std::string &filename);
    ~Logger();
    void PrintHeader();
    void Log(const std::string &s);
};



Logger::Logger(const std::string &filename)
{
    file_.open(filename.c_str(), std::ofstream::out | std::ofstream::app);
}

Logger::~Logger()
{
    file_.close();
}

void Logger::Log(const std::string &s)
{
    file_ << s;
}


#endif // LOGGER_H_
