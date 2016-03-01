#include "message.h"

#include <iostream>
#include <regex>


namespace
{

const std::string ParseError = "<parse ERROR>";

std::string GetFirstSubmatch(const std::string& s, const std::regex& regex)
{
    std::smatch match;
    if (!std::regex_match(s, match, regex) || match.size() != 2)
    {
        // std::cerr << "GetFirstSubmatch: can't find match in message \"" <<
        //     s << "\"" << ", match size: " << match.size() << std::endl;
        return ParseError;
    }
    return match[1].str();
}

} // namespace


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
        return message_;
    }
}

std::string Message::GetPrettyJoinMessage() const
{
    const std::regex nickRegex("^:([^ !]+).*");
    const std::string nick = GetFirstSubmatch(message_, nickRegex);

    return nick + " joined the room";
}

std::string Message::GetPrettyPrivateMessage() const
{
    const std::regex nickRegex("^:([^ !]+).*");
    const std::string nick = GetFirstSubmatch(message_, nickRegex);

    // messge_ is something like:
    //: nick!hostname PRIVMSG #channelName :message body goes here
    const std::regex msgBodyRegex("^:[^ ]+ PRIVMSG #[^ ]+ :(.*)");
    const std::string msgBody = GetFirstSubmatch(message_, msgBodyRegex);

    return nick + ": " + msgBody;
}

std::string Message::GetPrettyQuitMessage() const
{
    const std::regex nickRegex("^:([^ !]+).*");
    const std::string nick = GetFirstSubmatch(message_, nickRegex);

    return nick + " left the room";
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
    return StringToCommand(GetFirstSubmatch(message, commandRegex));
}


