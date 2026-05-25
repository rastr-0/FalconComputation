#include "ProtocolHandler.h"
#include "parser.h"
#include "command.h"

ProtocolHandler::ProtocolHandler(CommandRegistry& registry)
  : registry_(registry), session_(registry)
{}

std::string ProtocolHandler::handle(const std::string& rawMessage) {
  try {
    ParsedCommand parsed = Parser::parse(rawMessage);
    Command* cmd = registry_.lookup(parsed.name);
    if (!cmd)
      return "ERROR INVALID_COMMAND\n";

    Result res = cmd->execute(parsed.args, session_);

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
