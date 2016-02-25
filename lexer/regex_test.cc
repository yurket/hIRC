#include <iostream>
#include <regex>
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

void MatchStrings(const StringVector& v)
{
    const std::regex nickRegex(".* ([A-Z]+) .*");
    for (const auto& s : v)
    {

    }
}

} // namespace

int main()
{
    StringVector exampleStrings = GetExamples("test_messages.txt");

    MatchStrings(exampleStrings);
    return 0;
}

