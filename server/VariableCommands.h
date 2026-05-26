#ifndef H_VARIABLE_COMMANDS
#define H_VARIABLE_COMMANDS

#include "command.h"

// all derived classes for working with variables

class SetCommand : public Command {
public:
  std::string name()        const override { return "SET"; }
  std::string description() const override {
    return "SET <name> <values...> | SET <name> <- <CMD> [args] -- Stores values under a name";
  }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class GetCommand : public Command {
public:
  std::string name()        const override { return "GET"; }
  std::string description() const override { return "GET $<name> -- Retrieves stored values"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class DelCommand : public Command {
public:
  std::string name()        const override { return "DEL"; }
  std::string description() const override { return "DEL $<name>... -- Deletes one or more variables"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class ListCommand : public Command {
public:
  std::string name()        const override { return "LIST"; }
  std::string description() const override { return "LIST -- Lists all variables and their values"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class ResetCommand : public Command {
public:
  std::string name()        const override { return "RESET"; }
  std::string description() const override { return "RESET -- Clears all session variables"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class AppendCommand : public Command {
public:
  std::string name()        const override { return "APPEND"; }
  std::string description() const override { return "APPEND $<name> <values...> -- Appends values to a variable"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class CopyCommand : public Command {
public:
  std::string name()        const override { return "COPY"; }
  std::string description() const override { return "COPY $<src> $<dst> -- Copies a variable to a new name"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

#endif // H_VARIABLE_COMMANDS
