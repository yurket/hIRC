#ifndef LOGGER_H_
#define LOGGER_H_

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
    void PrintHeader();
    void Log(const std::string &s);
};


Logger::Logger(const std::string &filename="test.log", const std::fstream::openmode mode)
{
    file_.open(filename.c_str(), mode);
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


#endif // LOGGER_H_
