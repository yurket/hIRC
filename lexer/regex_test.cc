#include "message.h"

#include <iostream>
#include <string>
#include <vector>

#include <fstream>

namespace
{

typedef std::vector<std::string> StringVector;


StringVector GetExamples(const std::string& filename)
{
    std::ifstream ifs(filename);
    StringVector ret;
    std::string s;
    while(std::getline(ifs, s))
    {
        ret.emplace_back(s);
    }
    return ret;
}

void LogPrettyMessages(const StringVector& v)
{
    for (const auto& s : v)
    {
        Message message(s);
        // std::cerr << s << std::endl;
        std::cerr << message.GetStringForLogging() << std::endl;
    }
}

} // namespace

int main()
{
    StringVector exampleStrings = GetExamples("test_messages.txt");

    LogPrettyMessages(exampleStrings);
    return 0;
}

