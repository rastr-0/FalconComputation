#include "server.h"
#include "command.h"
#include "VariableCommands.h"

#include <iostream>
#include <memory>

int main() {
  try {
    CommandRegistry registry; // registry holds all commands as their name + unique_ptr in unordered_map

    // Compute commands
    registry.registerCommand(std::make_unique<MinCommand>());
    registry.registerCommand(std::make_unique<MaxCommand>());
    registry.registerCommand(std::make_unique<SumCommand>());
    registry.registerCommand(std::make_unique<SortCommand>());
    registry.registerCommand(std::make_unique<AvgCommand>());
    registry.registerCommand(std::make_unique<MedianCommand>());
    registry.registerCommand(std::make_unique<CountCommand>());
    registry.registerCommand(std::make_unique<StatsCommand>());

    // Variable commands
    registry.registerCommand(std::make_unique<SetCommand>());
    registry.registerCommand(std::make_unique<GetCommand>());
    registry.registerCommand(std::make_unique<DelCommand>());
    registry.registerCommand(std::make_unique<ListCommand>());
    registry.registerCommand(std::make_unique<ResetCommand>());
    registry.registerCommand(std::make_unique<AppendCommand>());
    registry.registerCommand(std::make_unique<CopyCommand>());

    // Control commands
    registry.registerCommand(std::make_unique<HelpCommand>());
    registry.registerCommand(std::make_unique<QuitCommand>());

    Server server;
    server.make_connection("127.0.0.1", 5678); // handles all the network stuff: creating, binding and listening on socket to listen on
    server.run(registry); // main logic happens here
  } catch (const std::exception& e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }
}
