#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>

#include <ctime>
#include <stdexcept>
#include <fstream>


class Logger
{
 public:
    Logger();
    explicit Logger(
        const std::string &filename,
        const std::fstream::openmode mode = std::fstream::app
        );
    ~Logger();
    // TODO:  note: ‘Logger::Logger(const Logger&)’ is implicitly deleted
    // because the default definition would be ill-formed
    Logger(const Logger&);

    void Log(const std::string &s);
    void PrintHeader();

private:
    std::ofstream file_;
    bool initialized_;
};


Logger::Logger()
    : initialized_(false)
{
}

Logger::Logger(const std::string &filename, const std::fstream::openmode mode)
{
    file_.open(filename.c_str(), mode);
    initialized_ = true;
    PrintHeader();
}

Logger::~Logger()
{
    if (file_.is_open())
    {
        file_.close();
    }
}

void Logger::Log(const std::string &s)
{
    if (!initialized_)
    {
        throw std::logic_error("Logger: not initialized!");
    }
    std::time_t const now = std::time(NULL);
    char now_string[100];
    strftime(now_string, sizeof(now_string), "%T", std::localtime(&now));
    file_ << now_string << ": "  << s << std::endl;
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
