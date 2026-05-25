#include "command.h"
#include "CommandRegistry.h"
#include <algorithm>
#include <cctype>
#include <iomanip>

// Always show exactly 2 decimal places — used for AVG.
static std::string fmtAvg(double d) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << d;
  return oss.str();
}

// Whole numbers as integers, fractions with 2 d.p. — used for MEDIAN.
static std::string fmtFixed2(double d) {
  if (d == std::floor(d) && std::abs(d) < 1e15)
    return std::to_string((long long)d);
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << d;
  return oss.str();
}

Result MinCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }
  r.ok = true;
  r.values.push_back(fmt(*std::min_element(vals.begin(), vals.end())));
  return r;
}

Result MaxCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }
  r.ok = true;
  r.values.push_back(fmt(*std::max_element(vals.begin(), vals.end())));
  return r;
}

Result SumCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }
  double sum = 0;
  for (double v : vals) sum += v;
  r.ok = true;
  r.values.push_back(fmt(sum));
  return r;
}

Result SortCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }
  std::sort(vals.begin(), vals.end());
  r.ok = true;
  for (double v : vals) r.values.push_back(fmt(v));
  return r;
}

// HELP queries the live registry so its list is always accurate.
Result HelpCommand::execute(const std::vector<std::string>& args, Session& session) {
  Result r;
  r.ok = true;
  if (args.empty()) {
    r.values = session.registry().commandNames();
    return r;
  }
  std::string target = args[0];
  for (auto& c : target) c = std::toupper((unsigned char)c);
  Command* cmd = session.registry().lookup(target);
  if (!cmd) { r.ok = false; r.reason = "INVALID_COMMAND"; return r; }
  r.values.push_back(cmd->description());
  return r;
}

Result QuitCommand::execute(const std::vector<std::string>& /*args*/, Session& /*session*/) {
  Result r;
  r.ok = true;
  r.should_close = true;
  return r;
}

// AVG
Result AvgCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }
  double sum = 0;
  for (double v : vals) sum += v;
  r.ok = true;
  r.values.push_back(fmtAvg(sum / (double)vals.size()));
  return r;
}

// MEDIAN
Result MedianCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }
  std::sort(vals.begin(), vals.end());
  size_t n = vals.size();
  double median = (n % 2 == 1) ? vals[n / 2]
                               : (vals[n/2 - 1] + vals[n/2]) / 2.0;
  r.ok = true;
  r.values.push_back(fmtFixed2(median));
  return r;
}

// COUNT
Result CountCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }
  r.ok = true;
  r.values.push_back(std::to_string(vals.size()));
  return r;
}

// STATS — computes all five statistics in one pass over the resolved values.
Result StatsCommand::execute(const std::vector<std::string>& args, Session& session) {
  auto vals = resolveArgs(args, session);
  Result r;
  if (vals.empty()) { r.reason = "EMPTY_ARGS"; return r; }

  auto sorted = vals;
  std::sort(sorted.begin(), sorted.end());

  double sum = 0;
  for (double v : vals) sum += v;

  size_t n = sorted.size();
  double median = (n % 2 == 1) ? sorted[n / 2]
                               : (sorted[n/2 - 1] + sorted[n/2]) / 2.0;

  r.ok = true;
  r.values.push_back("MIN="    + fmt(sorted.front()));
  r.values.push_back("MAX="    + fmt(sorted.back()));
  r.values.push_back("SUM="    + fmt(sum));
  r.values.push_back("AVG="    + fmtAvg(sum / (double)n));
  r.values.push_back("MEDIAN=" + fmtFixed2(median));
  return r;
}
