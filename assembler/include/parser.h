#pragma once

#include <vector>
#include <string>
#include <regex>
#include <algorithm>

namespace parser {
  struct token {
    std::string token;
    int line, position;
  };

  std::vector<std::string> split(std::string& input, char delimiter) {
    std::vector<std::string> output;
    
    size_t pos = 0;
    std::string token;
    while ((pos = input.find(delimiter)) != std::string::npos) {
        token = input.substr(0, pos);
        output.push_back(token);
        input.erase(0, pos + 1);
    }

    return output;
  }

  std::vector<token> tokenize(std::string& input) {
    std::regex comment_reg(";(.)*");
    input = std::regex_replace(input, comment_reg, " ");
    input = std::regex_replace(input, std::regex("\n"), " \n");

    std::vector<std::string> lines = split(input, '\n');
    std::vector<token> tokens;
    
    for (int i = 0; i < lines.size(); i++) {
      std::vector<std::string> split_tokens = split(lines[i], ' ');
      for (int j = 0; j < split_tokens.size(); j++)
        tokens.push_back({split_tokens[j], i, j});
    }

    return tokens;
  }

  const std::vector<std::string> valid_registers    = {"reg0" , "reg1"    , "reg2" , "reg3", "reg4", "reg5", "reg6", "reg7"};
  const std::vector<std::string> valid_instructions = {"set"  , "add"     , "sub"  , "mlt" , "div" , "mod" , "inc" , "dec" , "bigg", "bigg_eq",
                                                       "small", "small_eq", "equal", "diff", "jmp" , "jmpz", "jmpo", "call", "ret" , 
                                                       "alloc", "allocm"  , "outu" , "outs", "outc"};
  const std::vector<std::string> valid_keywords     = {"func", "{", "}"};

  std::string assemble_address_regex() {
    std::string output = "^#((\\d)+|(";

    for (auto& i : valid_registers)
      output += i + "|";

    output = output.substr(0, output.length()-1) + "))-((\\d)+|(";

    for (auto& i : valid_registers)
      output += i + "|";

    output = output.substr(0, output.length()-1) + "))$";
    
    return output;
  }

  bool check_tokens(std::vector<token>& tokens) {
    std::regex address_reg(assemble_address_regex());

    int index = -1;
    for (auto& i : tokens) {
      index++;
      if (std::find(valid_registers.begin()   , valid_registers.end()   , i.token) != valid_registers.end())    continue; //* Valid register token
      if (std::find(valid_instructions.begin(), valid_instructions.end(), i.token) != valid_instructions.end()) continue; //* Valid instruction token
      if (std::find(valid_keywords.begin()    , valid_keywords.end()    , i.token) != valid_keywords.end())     continue; //* Valid keyword
      if (std::regex_match(i.token, address_reg)) continue;                                                               //* Valid address pattern
      if (i.token.find_first_not_of( "0123456789" ) == std::string::npos) continue;                                       //* Valid number
      if (index > 0 && (tokens[index-1].token == "func" || tokens[index-1].token == "call")) continue;                    //* Not checking function names

      std::cerr << "Token: \"" << i.token << "\" on line: " << (i.line+1) << ", position: " << (i.position+1) << " doesn't match any valid token. Aborting.\n";
      return false;
    }

    return true;
  }
}