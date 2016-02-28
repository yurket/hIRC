#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <fstream>

namespace
{

typedef std::vector<std::string> StringVector;


enum class Command
{
    JOIN, PRIVMSG, QUIT, UNKNOWN
};

Command ToCommand(const std::string& command)
{
    if (command == "JOIN")
    {
        return Command::JOIN;
    }
    else if (command == "PRIVMSG")
    {
        return Command::PRIVMSG;
    }
    else if (command == "QUIT")
    {
        return Command::QUIT;
    }
    return Command::UNKNOWN;
}

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
    const std::regex commandRegex("^:[^ ]+ ([A-Z]+) .*");
    for (const auto& s : v)
    {
        std::smatch commandMatch;
        if (std::regex_match(s, commandMatch, commandRegex))
        {
            if (commandMatch.size() == 2)
            {
                std::string const command = commandMatch[1].str();
                std::cout << s << std::endl << " has command: " << command << '\n';
                const Command cmd = ToCommand(command);
                switch(cmd)
                {
                case Command::PRIVMSG:
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            std::cout << s << std::endl << " NO COMMAND FOUND" << '\n';
        }
    }
}

} // namespace

int main()
{
    StringVector exampleStrings = GetExamples("test_messages.txt");

    MatchStrings(exampleStrings);
    return 0;
}

