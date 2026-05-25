#ifndef H_SESSION
#define H_SESSION

#include "VariableStore.h"
#include <string>
#include <vector>

class CommandRegistry;  // forward declaration to avoid circular include

class Session {
  VariableStore store_;
  CommandRegistry& registry_;
public:
  explicit Session(CommandRegistry& registry); // ToDo: what is `explicit` keywork

  void set(const std::string& name, std::vector<double> values);
  std::vector<double> get(const std::string& name);   // throws UNDEFINED_VAR
  void del(const std::string& name); // throws UNDEFINED_VAR
  void reset();
  std::vector<std::string> listNames() const;
  bool exists(const std::string& name) const;
  CommandRegistry& registry();
};

#endif // H_SESSION
