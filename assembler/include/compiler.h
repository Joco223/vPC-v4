#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

#include "parser.h"

namespace compiler {
  enum token_types {
    tt_instruction,
    tt_value,
    tt_register,
    tt_address,
    tt_identifier,
    tt_func_def,
    tt_open_bracket,
    tt_closed_bracket
  };

  struct token {
    std::string name;
    int type, line, position;
    std::vector<std::pair<bool, int>> values; //* True for register, false for value
  };

  struct instruction_def {
    std::string name;
    int op_code;
    std::vector<int> arguments; //* Available types for each argument
  };

  std::unordered_map<std::string, int> register_map = {{"reg0", 0}, {"reg1", 1}, {"reg2", 2}, {"reg3", 3}, 
                                                             {"reg4", 4}, {"reg5", 5}, {"reg6", 6}, {"reg7", 7}};

  std::vector<instruction_def> instructions_def = {
    {"add"     , 0x0B, {tt_register, tt_register}},
    {"sub"     , 0x0C, {tt_register, tt_register}},
    {"mlt"     , 0x0D, {tt_register, tt_register}},
    {"div"     , 0x0E, {tt_register, tt_register}},
    {"mod"     , 0x0F, {tt_register, tt_register}},
    {"inc"     , 0x10, {tt_register, tt_register}},
    {"dec"     , 0x11, {tt_register, tt_register}},
    {"bigg"    , 0x12, {tt_register, tt_register}},
    {"bigg_eq" , 0x13, {tt_register, tt_register}},
    {"small"   , 0x14, {tt_register, tt_register}},
    {"small_eq", 0x15, {tt_register, tt_register}},
    {"equal"   , 0x16, {tt_register, tt_register}},
    {"diff"    , 0x17, {tt_register, tt_register}},
    {"jmp"     , 0x18, {tt_value                }},
    {"jmpz"    , 0x19, {tt_value                }},
    {"jmpo"    , 0x1A, {tt_value                }},
    {"call"    , 0x1B, {tt_identifier           }},
    {"outu"    , 0x23, {tt_register             }},
    {"outs"    , 0x24, {tt_register             }},
    {"out"     , 0x25, {tt_register             }}
  };
  //! Instructions set, ret, alloc and allocm are special ones and can't be fit here

  std::vector<std::string> function_identifiers;

  bool instruction_exists(parser::token& token) {
    for (auto& i : instructions_def)
      if (token.token == i.name)
        return true;
    
    return false;
  }

  std::vector<token> classify_tokens(std::vector<parser::token>& input_tokens) {
    std::vector<token> output;
    for (auto& i : input_tokens) {
      //* Registers
      if (register_map.find(i.token) != register_map.end()) {
        output.push_back({i.token, tt_register, i.line, i.position, {{true, register_map[i.token]}}}); continue;
      }

      //* Instructions
      if (instruction_exists(i) || i.token == "set" || i.token == "ret" || i.token == "alloc" || i.token == "allocm") {
        output.push_back({i.token, tt_instruction, i.line, i.position, {{false, 0}}}); continue;
      }

      //* Keywords
      if (i.token == "func") {
        output.push_back({i.token, tt_func_def, i.line, i.position, {{false, 0}}}); continue;
      }else if (i.token == "[") {
        output.push_back({i.token, tt_open_bracket, i.line, i.position, {{false, 0}}}); continue;
      }else if (i.token == "]") {
        output.push_back({i.token, tt_closed_bracket, i.line, i.position, {{false, 0}}}); continue;
      }

      //* Addresses
      if (i.token[0] == '#') {
        std::string first_pos = i.token.substr(1, std::distance(i.token.begin(), std::find(i.token.begin(), i.token.end(), '-')-1));
        std::string second_pos = i.token.substr(std::distance(i.token.begin(), std::find(i.token.begin(), i.token.end(), '-'))+1);

        if (first_pos.find_first_not_of("0123456789") == std::string::npos) {
          output.push_back({first_pos, tt_value, i.line, i.position, {{false, std::stoi(first_pos)}}});
        }else{
          output.push_back({first_pos, tt_register, i.line, i.position, {{true, register_map[first_pos]}}});
        }

        if (second_pos.find_first_not_of("0123456789") == std::string::npos) {
          output.push_back({second_pos, tt_value, i.line, i.position, {{false, std::stoi(second_pos)}}});
        }else{
          output.push_back({second_pos, tt_register, i.line, i.position, {{true, register_map[second_pos]}}});
        }

        continue;
      }

      //* Values
      if (i.token.find_first_not_of("0123456789") == std::string::npos) {
        output.push_back({i.token, tt_value, i.line, i.position, {{false, std::stoi(i.token)}}}); continue;
      }

      output.push_back({i.token, tt_identifier, i.line, i.position, {{false, 0}}});
    }

    return output;
  }
}