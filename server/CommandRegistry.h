#ifndef H_COMMAND_REGISTRY
#define H_COMMAND_REGISTRY

#include "command.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


// holds commands' names and their unique_ptr
//  unique_ptr is used for automatic deleting when class is destroyed)
class CommandRegistry {
  std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
public:
  void registerCommand(std::unique_ptr<Command> cmd); // adds Command to `commands_`
  Command* lookup(const std::string& name) const;  // nullptr if not found
  std::vector<std::string> commandNames() const;
};

#endif // H_COMMAND_REGISTRY
