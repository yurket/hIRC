#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>


class Logger
{
    std::ofstream file_;
 public:
    Logger(const std::string filename, const char mode);
    void PrintHeader();
    void Log(const std::string &s);
    void Log(const char *s);
};



Logger::Logger(const std::string filename, const char mode)
{
    file_.open(filename, std::ofstream::out | std::ofstream::app);
}
#endif // LOGGER_H_
