#include "CommandRegistry.h"
#include <algorithm>

void CommandRegistry::registerCommand(std::unique_ptr<Command> cmd) {
  std::string key = cmd->name();
  commands_[key] = std::move(cmd);
}

Command* CommandRegistry::lookup(const std::string& name) const {
  auto it = commands_.find(name);
  return it != commands_.end() ? it->second.get() : nullptr;
}

std::vector<std::string> CommandRegistry::commandNames() const {
  std::vector<std::string> names;
  names.reserve(commands_.size());
  for (const auto& [name, _] : commands_)
    names.push_back(name);
  std::sort(names.begin(), names.end());
  return names;
}
