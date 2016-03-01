#include <string>

class Message
{
    enum CommandType
    {
        JOIN, PRIVMSG, QUIT, Unknown
    };

public:
    explicit Message(const std::string& message);
    std::string GetStringForLogging() const;

private:
    std::string GetPrettyJoinMessage() const;
    std::string GetPrettyPrivateMessage() const;
    std::string GetPrettyQuitMessage() const;

    CommandType StringToCommand(const std::string& command_string) const;
    CommandType GetCommandFromMessageString(const std::string& message);
    
private:
    std::string message_;
    CommandType command_;
};
