#include "message.h"

#include <iostream>
#include <regex>

namespace
{

const std::regex NickRegex("^:([^ !]+).*\r\n");
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

std::string Message::GetStringForLogging()
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

    const std::string nick = GetFirstSubmatch(message_, NickRegex);

    return nick + " joined the room";
}

std::string Message::GetPrettyPrivateMessage() const
{
    const std::string nick = GetFirstSubmatch(message_, NickRegex);

    // messge_ is something like:
    //: nick!hostname PRIVMSG #channelName :message body goes here
    const std::regex msgBodyRegex("^:[^ ]+ PRIVMSG #[^ ]+ :(.*)\r\n");
    const std::string msgBody = GetFirstSubmatch(message_, msgBodyRegex);

    return nick + ": " + msgBody;
}

std::string Message::GetPrettyQuitMessage()
{
    const std::string nick = GetFirstSubmatch(message_, NickRegex);

    return GetColoredNick(nick) + " left the room";
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
    std::cerr << "Can't match command!" << std::endl;
    return CommandType::Unknown;
}

Message::CommandType Message::GetCommandFromMessageString(const std::string& message) const
{
    const std::regex commandRegex("^:[^ ]+ ([A-Z]+) .*\r\n");
    return StringToCommand(GetFirstSubmatch(message, commandRegex));
}

std::string Message::GetColoredNick(const std::string &raw_nickname)
{
    auto it = colored_nicknames.find(raw_nickname);

    if (it == colored_nicknames.end()) {
        std::size_t col_code = std::hash<std::string>{}(raw_nickname);
        auto r = (col_code % 256 + 255) / 2;
        auto g = ((col_code >> 8) % 256 + 255) / 2;
        auto b = ((col_code >> 16) % 256 + 255) / 2;
        std::ostringstream stringStream;
        stringStream << "<font color=\"rgb(" << r << "," << g << "," << b << ")\">" << raw_nickname << "</font>";
        colored_nicknames[raw_nickname] = stringStream.str();
        return stringStream.str();
    }
    else {
        return it->second;
    }
}
