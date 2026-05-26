#include "ProtocolHandler.h"
#include "parser.h"
#include "command.h"

ProtocolHandler::ProtocolHandler(CommandRegistry& registry)
  : registry_(registry), session_(registry)
{}

std::string ProtocolHandler::handle(const std::string& rawMessage) {
  try {
    ParsedCommand parsed = Parser::parse(rawMessage); // ParsedCommand is essentially the name and arguments of the command

    Command* cmd = registry_.lookup(parsed.name); // find the specific registered Command that was registered earlier in main_server
    if (!cmd)
      return "ERROR INVALID_COMMAND\n";

    Result res = cmd->execute(parsed.args, session_); // Result is essentially status + reason (if failed) + result of the operation

    if (res.should_close) {
      closed_ = true;
      return "BYE\n";
    }
    if (!res.ok)
      return "ERROR " + res.reason + "\n";

    std::string out = "OK";
    for (const auto& val : res.values)
      out += " " + val;
    return out + "\n";

  } catch (const std::exception& e) {
    return "ERROR " + std::string(e.what()) + "\n";
  }
}
