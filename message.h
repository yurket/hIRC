#include <string>
#include <unordered_map>

class Message
{
    enum CommandType
    {
        JOIN, PRIVMSG, QUIT, NICK, Unknown
    };

public:
    explicit Message(const std::string& message);
    std::string GetStringForLogging();

private:
    std::string GetPrettyJoinMessage();
    std::string GetPrettyPrivateMessage();
    std::string GetPrettyQuitMessage();
    std::string GetColoredNick(const std::string& raw_nickname);

    CommandType StringToCommand(const std::string& command_string) const;
    CommandType GetCommandFromMessageString(const std::string& message) const;

private:
    std::string message_;
    CommandType command_;
    std::unordered_map<std::string, std::string> colored_nicknames_;
};
