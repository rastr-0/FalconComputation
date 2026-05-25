#include "parser.h"
#include <stdexcept>

ParsedCommand Parser::parse(const std::string& line) {
  ParsedCommand cmd;
  // checking whether line is empty
  if (line.empty())
    throw std::runtime_error("Empty command");
  
  // spliting line into separate words
  std::vector<std::string> words;
  std::string current_word;

  for (auto c : line) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
      if (!current_word.empty()) {
        words.push_back(current_word);
        current_word.clear();
      } 
    } else {
      current_word += c;
    }
  }
  
  // the last word case
  if (!current_word.empty())
    words.push_back(current_word);
 
  if (words.empty())
    throw std::runtime_error("Empty command");

  // the first word is always a command name
  cmd.name = Parser::normalize(words[0]);
  
  for (size_t i = 1; i < words.size(); i++)
    cmd.args.push_back(words[i]);

  return cmd;
}
