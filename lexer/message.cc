#include "message.h"

#include <iostream>
#include <regex>


Message::Message(const std::string& message) :
    message_(message),
    command_(GetCommandFromMessageString(message))
{
}

std::string Message::GetStringForLogging() const
{
    switch(static_cast<int>(command_))
    {
    case CommandType::PRIVMSG:
        GetPrettyPrivateMessage();
        break;
    default:
        break;
    }
    return std::string();
}

std::string Message::GetPrettyPrivateMessage() const
{
    const std::regex nickRegex("^:(\d+)[! ] .*\r\n");
    std::smatch nickMatch;
    if (!std::regex_match(message_, nickMatch, nickRegex))
    {
        std::cerr << "can't match nick in message \"" << message_ << "\"" << std::endl;
    }
}

Message::CommandType Message::StringToCommand(const std::string& command) const
{
    if (command == "JOIN")
    {
        return CommandType::JOIN;
    }
    else if (command == "PRIVMSG")
    {
        return CommandType::PRIVMSG;
    }
    else if (command == "QUIT")
    {
        return CommandType::QUIT;
    }
    return CommandType::Unknown;
}

Message::CommandType Message::GetCommandFromMessageString(const std::string& message)
{
    const std::regex commandRegex("^:[^ ]+ ([A-Z]+) .*");
    std::smatch commandMatch;
    if (std::regex_match(message, commandMatch, commandRegex))
    {
        if (commandMatch.size() == 2)
        {
            std::string const command = commandMatch[1].str();
            std::cerr << message << std::endl << " has command: " << command << '\n';
            return StringToCommand(command);
        }
    }

    std::cerr << message << std::endl << " NO COMMAND FOUND" << '\n';
    return CommandType::Unknown;
}


