#ifndef H_VARIABLE_STORE
#define H_VARIABLE_STORE

#include <string>
#include <vector>
#include <unordered_map>

class VariableStore {
  std::unordered_map<std::string, std::vector<double>> vars_;
public:
  void set(const std::string& name, std::vector<double> values);
  std::vector<double> get(const std::string& name) const;  // throws UNDEFINED_VAR
  void del(const std::string& name);                       // throws UNDEFINED_VAR
  void clear();
  std::vector<std::string> names() const;
  bool contains(const std::string& name) const;
};

#endif // H_VARIABLE_STORE
