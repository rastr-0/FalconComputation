#ifndef H_COMMAND
#define H_COMMAND

#include "parser.h"
#include "Session.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cmath>

struct Result {
  bool ok = false;
  bool should_close = false;
  std::string reason;
  std::vector<std::string> values;
};

// Format a double cleanly: whole numbers without decimals, fractions as-is.
// Inline so every translation unit that includes this header can use it.
inline std::string fmt(double d) {
  if (d == std::floor(d) && std::abs(d) < 1e15)
    return std::to_string((long long)d);
  std::ostringstream oss;
  oss << d;
  return oss.str();
}

class Command {
public:
  virtual ~Command() = default;
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual Result execute(const std::vector<std::string>& args, Session& session) = 0;
};

// Abstract template base for commands that operate on numeric values.
// resolveArgs expands $variable tokens by reading from the session, and
// parses literals as T. Subclasses call it at the top of execute().
template <typename T>
class ComputeCommand : public Command {
protected:
  std::vector<T> resolveArgs(const std::vector<std::string>& args, Session& session) {
    std::vector<T> result;
    for (const auto& token : args) {
      if (!token.empty() && token[0] == '$') {
        // Variable reference: expand every stored value into the list
        for (double v : session.get(token.substr(1)))
          result.push_back(static_cast<T>(v));
      } else {
        // Literal: parse as double then cast to T
        size_t pos;
        double v;
        try { v = std::stod(token, &pos); }
        catch (...) { throw std::runtime_error("TYPE_MISMATCH"); }
        if (pos != token.size()) throw std::runtime_error("TYPE_MISMATCH");
        result.push_back(static_cast<T>(v));
      }
    }
    return result;
  }
};

// all the commands as derived classes from ComputeCommand

class MinCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "MIN"; }
  std::string description() const override { return "MIN <values...> -- Returns the minimum value"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class MaxCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "MAX"; }
  std::string description() const override { return "MAX <values...> -- Returns the maximum value"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class SumCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "SUM"; }
  std::string description() const override { return "SUM <values...> -- Returns the sum of all values"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class SortCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "SORT"; }
  std::string description() const override { return "SORT <values...> -- Sorts values in ascending order"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class AvgCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "AVG"; }
  std::string description() const override { return "AVG <values...> -- Returns the arithmetic mean (2 d.p.)"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class MedianCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "MEDIAN"; }
  std::string description() const override { return "MEDIAN <values...> -- Returns the median value"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class CountCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "COUNT"; }
  std::string description() const override { return "COUNT <values...> -- Returns the number of values"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class StatsCommand : public ComputeCommand<double> {
public:
  std::string name() const override { return "STATS"; }
  std::string description() const override { return "STATS <values...> -- Returns MIN MAX SUM AVG MEDIAN"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class HelpCommand : public Command {
public:
  std::string name() const override { return "HELP"; }
  std::string description() const override { return "HELP [command] -- Lists commands or describes one"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

class QuitCommand : public Command {
public:
  std::string name() const override { return "QUIT"; }
  std::string description() const override { return "QUIT -- Ends the session"; }
  Result execute(const std::vector<std::string>& args, Session& session) override;
};

#endif // H_COMMAND
