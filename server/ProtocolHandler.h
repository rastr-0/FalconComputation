#ifndef H_PROTOCOL_HANDLER
#define H_PROTOCOL_HANDLER

#include "CommandRegistry.h"
#include "Session.h"
#include <string>

// One ProtocolHandler is created per client connection.
// It owns the Session for that connection and processes each incoming line.
class ProtocolHandler {
  // Using registry field here only because it "decouples" it a bit from session
  // though, registry that holds session is the same object
  CommandRegistry& registry_;
  Session session_;
  bool closed_ = false;
public:
  explicit ProtocolHandler(CommandRegistry& registry);
  std::string handle(const std::string& rawMessage);
  bool closed() const { return closed_; }
};

#endif // H_PROTOCOL_HANDLER
