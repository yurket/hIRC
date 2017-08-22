#ifndef LOGGER_H_
#define LOGGER_H_

#include <cassert>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>

namespace
{

template <typename T>
struct is_string
    : std::integral_constant<
        bool,
        std::is_same<      char *, typename std::decay<T>::type >::value ||
        std::is_same<const char *, typename std::decay<T>::type >::value ||
        std::is_same< std::string, typename std::decay<T>::type >::value
    > {};
}


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
    void Log(const char* format_string);
    template <typename T, typename... Args>
    void Log(const char* format_string, T value, Args... args);
    void PrintHeader();
    void DisableLogging();
    void EnableLogging();

private:
    std::mutex mutex_;
    std::ofstream file_stream_;
    bool logging_enabled_;
};

using RegisteredLoggers = std::map<std::string, std::unique_ptr<Logger>>;


template <typename T, typename... Args>
void Logger::Log(const char* format_string, T value, Args... args)
{
    assert(format_string != nullptr);

    const char* s = format_string;
    while (*s)
    {
        if (*s == '%' && *++s != '%')
        {
            switch(*s)
            {
            case 'd':
                if (!std::is_integral<T>())
                {
                    throw std::runtime_error("Integral type expected");
                }
                break;
            case 'f':
                if (!std::is_floating_point<T>())
                {
                    throw std::runtime_error("Floating-point type expected");
                }
                break;
            case 's':
                if (!is_string<T>())
                {
                    throw std::runtime_error("String type expected");
                }
                break;
            default:
                throw std::runtime_error("Unsupported argument type ");
            }
            file_stream_ << value;
            return Log(++s, args...);
        }
        file_stream_ << *s++;
    }
    throw std::runtime_error("Not used argument passed");
}

#endif // LOGGER_H_
