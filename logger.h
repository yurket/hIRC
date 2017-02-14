#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <string>

class Logger
{
 public:
    Logger(
        const std::string& filename,
        const std::fstream::openmode mode = std::fstream::app
        );
    ~Logger();
    // TODO:  note: ‘Logger::Logger(const Logger&)’ is implicitly deleted
    // because the default definition would be ill-formed
    Logger(const Logger&);

    void Log(const std::string& s);
    void PrintHeader();
    void DisableLogging();
    void EnableLogging();

private:
    std::ofstream file_;
    bool logging_enabled_;
};


#endif // LOGGER_H_
