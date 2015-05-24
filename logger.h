#ifndef LOGGER_H_
#define LOGGER_H_

#include <ctime>
#include <fstream>


class Logger
{
    std::ofstream file_;
 public:
    Logger(
        const std::string &filename,
        const std::fstream::openmode mode = std::fstream::out
        );
    ~Logger();
    void Log(const std::string &s);
    void PrintHeader();
};


Logger::Logger(const std::string &filename="test.log", const std::fstream::openmode mode)
{
    file_.open(filename.c_str(), mode);
    PrintHeader();
}

Logger::~Logger()
{
    file_.close();
}

void Logger::Log(const std::string &s)
{
    file_ << s;
    file_.flush();
}

void Logger::PrintHeader()
{
    Log("\n\n================================================================================\n");
    std::time_t now = std::time(NULL);
    Log(std::ctime(&now));
    Log("================================================================================\n");
}


#endif // LOGGER_H_
