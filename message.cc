#include "message.h"

#include <iostream>
#include <regex>
#include <iomanip>

namespace
{

const std::regex NickRegex("^:([^ !]+).*\r\n");
const std::string ParseError = "<parse ERROR>";

std::string GetFirstSubmatch(const std::string& s, const std::regex& regex)
{
    std::smatch match;
    if (!std::regex_match(s, match, regex) || match.size() != 2)
    {
        return ParseError;
    }
    return match[1].str();
}

const std::string SystemGreyColor = "9EA2A3";
std::string GetColoredString(const std::string& s, const std::string& hexColor)
{
    return "<font color=\"#" + hexColor + "\">" + s + "</font>";
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

std::string Message::GetPrettyJoinMessage()
{

    const std::string nick = GetFirstSubmatch(message_, NickRegex);

    return GetColoredNick(nick) + GetColoredString(" joined the room", SystemGreyColor);
}

std::string Message::GetPrettyPrivateMessage()
{
    const std::string nick = GetFirstSubmatch(message_, NickRegex);

    // messge_ is something like:
    //: nick!hostname PRIVMSG #channelName :message body goes here
    const std::regex msgBodyRegex("^:[^ ]+ PRIVMSG #[^ ]+ :(.*)\r\n");
    const std::string msgBody = GetFirstSubmatch(message_, msgBodyRegex);

    return GetColoredNick(nick) + ": " + msgBody;
}

std::string Message::GetPrettyQuitMessage()
{
    const std::string nick = GetFirstSubmatch(message_, NickRegex);

    return GetColoredNick(nick) + GetColoredString(" left the room", SystemGreyColor);
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

std::string Message::GetColoredNick(const std::string& raw_nickname)
{
    auto it = colored_nicknames_.find(raw_nickname);

    if (it == colored_nicknames_.end()) {
        std::size_t hex_col_code = std::hash<std::string>{}(raw_nickname) & 0xFFFFFF;
        std::ostringstream stringStream;
        stringStream << "<font color=\"#"
                        << std::setfill ('0') << std::setw(6)
                        << std::hex << hex_col_code << "\"><b>" << raw_nickname << "</b></font>";
        colored_nicknames_[raw_nickname] = stringStream.str();
        return stringStream.str();
    }
    else {
        return it->second;
    }
}
