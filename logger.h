#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <string>


class Logger
{
public:
    static bool Register(const std::string& logger_name, const std::string& log_filename);
    static Logger& Get(const std::string& logger_name);

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
    std::mutex mutex_;
    std::ofstream file_stream_;
    bool logging_enabled_;
};

using RegisteredLoggers = std::map<std::string, std::unique_ptr<Logger>>;

#endif // LOGGER_H_
