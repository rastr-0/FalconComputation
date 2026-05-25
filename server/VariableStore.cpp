#include "VariableStore.h"
#include <stdexcept>

void VariableStore::set(const std::string& name, std::vector<double> values) {
  vars_[name] = std::move(values); // ToDo: what is std::move ... copying values and not just address ...?
}

std::vector<double> VariableStore::get(const std::string& name) const {
  auto it = vars_.find(name);
  if (it == vars_.end())
    throw std::runtime_error("UNDEFINED_VAR");
  return it->second;
}

void VariableStore::del(const std::string& name) {
  if (!vars_.erase(name))
    throw std::runtime_error("UNDEFINED_VAR");
}

void VariableStore::clear() {
  vars_.clear();
}

std::vector<std::string> VariableStore::names() const {
  std::vector<std::string> result;
  result.reserve(vars_.size());
  for (const auto& [name, _] : vars_) // ToDo: again this for syntax I don't really understand, to be checked
    result.push_back(name);
  return result;
}

bool VariableStore::contains(const std::string& name) const {
  return vars_.count(name) > 0;
}
