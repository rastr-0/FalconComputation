#include "Session.h"
#include "CommandRegistry.h"

Session::Session(CommandRegistry& registry) : registry_(registry) {}

void Session::set(const std::string& name, std::vector<double> values) {
  store_.set(name, std::move(values));
}

std::vector<double> Session::get(const std::string& name) {
  return store_.get(name);
}

void Session::del(const std::string& name) {
  store_.del(name);
}

void Session::reset() {
  store_.clear();
}

std::vector<std::string> Session::listNames() const {
  return store_.names();
}

bool Session::exists(const std::string& name) const {
  return store_.contains(name);
}

CommandRegistry& Session::registry() {
  return registry_;
}
