#ifndef H_SESSION
#define H_SESSION

#include "VariableStore.h"
#include <string>
#include <vector>

class CommandRegistry;  // forward declaration to avoid circular include


// extra-layer for holding VariableStore and CommandRegistry in one class 
//  for better isolation (e.g. passing one session object for Command execute() method, instead of both) 

class Session {
  VariableStore store_; // handler and storage for variables
  CommandRegistry& registry_; // handler for commands 
public:
  explicit Session(CommandRegistry& registry);

  void set(const std::string& name, std::vector<double> values);
  std::vector<double> get(const std::string& name);   // throws UNDEFINED_VAR
  void del(const std::string& name); // throws UNDEFINED_VAR
  void reset();
  std::vector<std::string> listNames() const;
  bool exists(const std::string& name) const;
  CommandRegistry& registry();
};

#endif // H_SESSION
