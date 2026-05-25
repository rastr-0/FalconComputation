#ifndef H_PARSER
#define H_PARSER

#include <cctype>
#include <string>
#include <vector>

struct ParsedCommand {
  std::string name;
  std::vector<std::string> args;
};

class Parser {
private:
  static std::string normalize(const std::string& name) {
    std::string uppercase_name;
    uppercase_name.reserve(name.size());

    for (unsigned char c : name) {
      uppercase_name.push_back(std::toupper(c));
    }
    return uppercase_name;
  };
public:
  static ParsedCommand parse(const std::string& line);
};

#endif // !H_PARSER

