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

    Logger(Logger& other) = delete;
    Logger& operator= (Logger& rhs) = delete;

    Logger(Logger&& other);
    Logger& operator= (Logger&& rhs);


    void Log(const std::string& s);
    void PrintHeader();
    void DisableLogging();
    void EnableLogging();

private:
    std::ofstream file_stream_;
    bool logging_enabled_;
};


#endif // LOGGER_H_
