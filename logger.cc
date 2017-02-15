#include "logger.h"

#include <iostream>
#include <stdexcept>

#include <ctime>

namespace
{

const std::string Delimiter = "\n================================================================================\n";
const std::string LogBasename = "IrcHistory";
const std::size_t StrftimeBufferLen = 100;

std::string GetStringFromStrftimeFormat(const std::string& format_string)
{
    std::time_t now = std::time(NULL);
    char formatted_time[StrftimeBufferLen];
    if(!strftime(formatted_time, sizeof(formatted_time), format_string.c_str(), std::localtime(&now)))
    {
        return std::string();
    }
    return std::string(formatted_time);
}

} // namespace

Logger::Logger(const std::string& filename, const std::fstream::openmode mode) :
    logging_enabled_(true)
{
    file_stream_.open(filename.c_str(), mode);
    PrintHeader();
}

Logger::~Logger()
{
    if (file_stream_.is_open())
    {
        file_stream_.close();
    }
}

Logger::Logger(Logger&& other) :
    logging_enabled_(other.logging_enabled_)
{
    std::swap(file_stream_, other.file_stream_);
}

Logger& Logger::operator= (Logger&& rhs)
{
    std::swap(file_stream_, rhs.file_stream_);
    logging_enabled_ = rhs.logging_enabled_;

    return *this;
}

void Logger::Log(const std::string& s)
{
    if (!logging_enabled_)
    {
        return;
    }

    const std::string hour_minutes_seconds = GetStringFromStrftimeFormat("%T");
    file_stream_ << hour_minutes_seconds << ": "  << s << std::endl;
    file_stream_.flush();
}

void Logger::PrintHeader()
{
    std::string header;
    header += "\n";
    header += Delimiter;
    const std::string date_time_timezone = GetStringFromStrftimeFormat("%c %Z");
    header += date_time_timezone;
    header += Delimiter;
    Log(header);
}

void Logger::DisableLogging()
{
    logging_enabled_ = false;
}

void Logger::EnableLogging()
{
    logging_enabled_ = true;
}
