
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
    case CommandType::JOIN:
        return GetPrettyJoinMessage();
    case CommandType::PRIVMSG:
        return GetPrettyPrivateMessage();
    case CommandType::QUIT:
        return GetPrettyQuitMessage();
    default:
        return std::string();
    }
}

std::string Message::GetPrettyJoinMessage() const
{
    const std::regex nickRegex("^:([^ !]+).*");
    std::smatch nickMatch;
    if (!std::regex_match(message_, nickMatch, nickRegex) || nickMatch.size() != 2)
    {
        std::cerr << "GetPrettyPrivateMessage: can't match nick in message \"" <<
            message_ << "\"" << ", match size: " << nickMatch.size() << std::endl;
        return std::string();
    }
    const std::string nick = nickMatch[1].str();

    const std::string prettyMessage = nick + " just joined the room";
    return prettyMessage;
}

std::string Message::GetPrettyPrivateMessage() const
{
    const std::regex nickRegex("^:([^ !]+).*");
    std::smatch nickMatch;
    if (!std::regex_match(message_, nickMatch, nickRegex) || nickMatch.size() != 2)
    {
        std::cerr << "GetPrettyPrivateMessage: can't match nick in message \"" <<
            message_ << "\"" << ", match size: " << nickMatch.size() << std::endl;
        return std::string();
    }
    const std::string nick = nickMatch[1].str();

    // ": nick!hostname PRIVMSG #channelName :message body goes here
    const std::regex msgBodyRegex("^:[^ ]+ PRIVMSG #[^ ]+ :(.*)");
    std::smatch msgBodyMatch;
    if (!std::regex_match(message_, msgBodyMatch, msgBodyRegex) || msgBodyMatch.size() != 2)
    {
        std::cerr << "GetPrettyPrivateMessage: can't match message body in message \"" <<
            message_ << "\"" << ", nick is " << nick << std::endl;
        return std::string();
    }
    const std::string msgBody = msgBodyMatch[1].str();

    const std::string prettyMessage = nick + ": " + msgBody;
    return prettyMessage;
}

std::string Message::GetPrettyQuitMessage() const
{
    return std::string();
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
            const std::string command = commandMatch[1].str();
            std::cerr << message << std::endl << " has command: " << command << '\n';
            return StringToCommand(command);
        }
    }

    std::cerr << message << std::endl << " NO COMMAND FOUND" << '\n';
    return CommandType::Unknown;
}


