#include "VariableCommands.h"
#include "CommandRegistry.h"
#include <algorithm>
#include <stdexcept>
#include <cctype>

static bool isValidVarName(const std::string& name) {
  if (name.empty() || std::isdigit((unsigned char)name[0]))
    return false;
  for (unsigned char c : name)
    if (!std::isalnum(c) && c != '_')
      return false;
  return true;
}

// Strip the leading '$' from a token. Returns empty string if token has no '$'.
static std::string extractVarName(const std::string& token) {
  if (token.size() < 2 || token[0] != '$')
    return "";
  return token.substr(1);
}

// SET
Result SetCommand::execute(const std::vector<std::string>& args, Session& session) {
  Result r;
  if (args.empty()) { r.reason = "INVALID_ARGS"; return r; }

  const std::string& varName = args[0];
  if (!isValidVarName(varName)) { r.reason = "INVALID_ARGS"; return r; }

  if (args.size() < 2) { r.reason = "EMPTY_ARGS"; return r; }

  // Capture form: SET name <- COMMAND [args...]
  if (args[1] == "<-") {
    if (args.size() < 3) { r.reason = "INVALID_ARGS"; return r; }
    std::string cmdName = args[2];
    for (auto& c : cmdName) c = std::toupper((unsigned char)c);
    Command* cmd = session.registry().lookup(cmdName);
    if (!cmd) { r.reason = "INVALID_COMMAND"; return r; }
    std::vector<std::string> innerArgs(args.begin() + 3, args.end());
    Result inner = cmd->execute(innerArgs, session);
    if (!inner.ok) return inner;
    std::vector<double> vals;
    for (const auto& sv : inner.values)
      vals.push_back(std::stod(sv));
    session.set(varName, std::move(vals));
    r.ok = true;
    return r;
  }

  // Basic form: SET name v1 v2 ...
  std::vector<double> vals;
  for (size_t i = 1; i < args.size(); i++) {
    size_t pos;
    double v;
    try { v = std::stod(args[i], &pos); }
    catch (...) { r.reason = "TYPE_MISMATCH"; return r; }
    if (pos != args[i].size()) { r.reason = "TYPE_MISMATCH"; return r; }
    vals.push_back(v);
  }
  session.set(varName, std::move(vals));
  r.ok = true;
  return r;
}

// GET
Result GetCommand::execute(const std::vector<std::string>& args, Session& session) {
  Result r;
  if (args.size() != 1) { r.reason = "INVALID_ARGS"; return r; }
  std::string varName = extractVarName(args[0]);
  if (varName.empty()) { r.reason = "INVALID_ARGS"; return r; }
  try {
    for (double v : session.get(varName))
      r.values.push_back(fmt(v));
    r.ok = true;
  } catch (const std::runtime_error& e) {
    r.reason = e.what();
  }
  return r;
}

// DEL
Result DelCommand::execute(const std::vector<std::string>& args, Session& session) {
  Result r;
  if (args.empty()) { r.reason = "INVALID_ARGS"; return r; }
  for (const auto& token : args) {
    std::string varName = extractVarName(token);
    if (varName.empty()) { r.reason = "INVALID_ARGS"; return r; }
    try {
      session.del(varName);
    } catch (const std::runtime_error& e) {
      r.reason = e.what();
      return r;
    }
  }
  r.ok = true;
  return r;
}

// LIST — outputs each variable as  name=[v1 v2 v3]
Result ListCommand::execute(const std::vector<std::string>& /*args*/, Session& session) {
  Result r;
  r.ok = true;
  auto names = session.listNames();
  std::sort(names.begin(), names.end());
  for (const auto& name : names) {
    auto vals = session.get(name);
    std::string entry = name + "=[";
    for (size_t i = 0; i < vals.size(); i++) {
      if (i > 0) entry += " ";
      entry += fmt(vals[i]);
    }
    entry += "]";
    r.values.push_back(entry);
  }
  return r;
}

// RESET
Result ResetCommand::execute(const std::vector<std::string>& /*args*/, Session& session) {
  session.reset();
  Result r;
  r.ok = true;
  return r;
}

// APPEND
Result AppendCommand::execute(const std::vector<std::string>& args, Session& session) {
  Result r;
  if (args.size() < 2) { r.reason = "INVALID_ARGS"; return r; }
  std::string varName = extractVarName(args[0]);
  if (varName.empty()) { r.reason = "INVALID_ARGS"; return r; }
  std::vector<double> vals;
  try { vals = session.get(varName); }
  catch (const std::runtime_error& e) { r.reason = e.what(); return r; }
  for (size_t i = 1; i < args.size(); i++) {
    size_t pos;
    double v;
    try { v = std::stod(args[i], &pos); }
    catch (...) { r.reason = "TYPE_MISMATCH"; return r; }
    if (pos != args[i].size()) { r.reason = "TYPE_MISMATCH"; return r; }
    vals.push_back(v);
  }
  session.set(varName, std::move(vals));
  r.ok = true;
  return r;
}

// COPY
Result CopyCommand::execute(const std::vector<std::string>& args, Session& session) {
  Result r;
  if (args.size() != 2) { r.reason = "INVALID_ARGS"; return r; }
  std::string srcName = extractVarName(args[0]);
  std::string dstName = extractVarName(args[1]);
  if (srcName.empty() || dstName.empty()) { r.reason = "INVALID_ARGS"; return r; }
  try { session.set(dstName, session.get(srcName)); }
  catch (const std::runtime_error& e) { r.reason = e.what(); return r; }
  r.ok = true;
  return r;
}
