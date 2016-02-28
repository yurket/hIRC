#include "logger.h"

#include <iostream>
#include <stdexcept>

#include <ctime>

Logger::Logger(const std::string &filename, const std::fstream::openmode mode) :
    logging_enabled_(true)
{
    file_.open(filename.c_str(), mode);
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
    if (!logging_enabled_)
    {
        return;
    }

    std::time_t const now = std::time(NULL);
    char pretty_time[100];
    strftime(pretty_time, sizeof(pretty_time), "%T", std::localtime(&now));
    file_ << pretty_time << ": "  << s << std::endl;
    file_.flush();
}

void Logger::PrintHeader()
{
    std::string header;
    header += "\n\n================================================================================\n";
    std::time_t now = std::time(NULL);
    char pretty_time[100];
    strftime(pretty_time, sizeof(pretty_time), "%c %Z", std::localtime(&now));
    header += pretty_time;
    header += "\n================================================================================\n";
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
