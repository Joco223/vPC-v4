#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>

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

  std::unordered_map<int, std::string> token_types_map = {{tt_instruction   , "instruction"        },
                                                          {tt_value         , "value"              },
                                                          {tt_register      , "register"           },
                                                          {tt_address       , "address"            },
                                                          {tt_identifier    , "identifier"         },
                                                          {tt_func_def      , "function definition"},
                                                          {tt_open_bracket  , "open bracket"       },
                                                          {tt_closed_bracket, "closed bracket"     }};

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

  struct function {
    std::string name;
    std::vector<std::vector<int>> intructions;
  };

  std::unordered_map<std::string, int> register_map = {{"reg0", 0}, {"reg1", 1}, {"reg2", 2}, {"reg3", 3}, 
                                                       {"reg4", 4}, {"reg5", 5}, {"reg6", 6}, {"reg7", 7}};

  std::vector<instruction_def> instructions_def = {
    {"add"     , 0x0B, {tt_register, tt_register             }},
    {"sub"     , 0x0C, {tt_register, tt_register             }},
    {"mlt"     , 0x0D, {tt_register, tt_register             }},
    {"div"     , 0x0E, {tt_register, tt_register             }},
    {"mod"     , 0x0F, {tt_register, tt_register             }},
    {"inc"     , 0x10, {tt_register                          }},
    {"dec"     , 0x11, {tt_register                          }},
    {"bigg"    , 0x12, {tt_register, tt_register, tt_register}},
    {"bigg_eq" , 0x13, {tt_register, tt_register, tt_register}},
    {"small"   , 0x14, {tt_register, tt_register, tt_register}},
    {"small_eq", 0x15, {tt_register, tt_register, tt_register}},
    {"equal"   , 0x16, {tt_register, tt_register, tt_register}},
    {"diff"    , 0x17, {tt_register, tt_register, tt_register}},
    {"jmp"     , 0x18, {tt_value                             }},
    {"jmpz"    , 0x19, {tt_value   , tt_register             }},
    {"jmpo"    , 0x1A, {tt_value   , tt_register             }},
    {"outu"    , 0x23, {tt_register                          }},
    {"outs"    , 0x24, {tt_register                          }},
    {"outc"    , 0x25, {tt_register                          }},
    {"size"    , 0x26, {tt_register, tt_value                }},
    {"in"      , 0x27, {tt_value                             }},
    {"conv"    , 0x28, {tt_register, tt_value                }}
  };
  //! Instructions set, call, ret, alloc and allocm are special ones and can't be fit here

  int instruction_exists(std::string token) {
    for (int i = 0; i < instructions_def.size(); i++)
      if (token == instructions_def[i].name)
        return i;
    
    return -1;
  }

  std::vector<token> classify_tokens(std::vector<parser::token>& input_tokens) {
    std::vector<token> output;
    for (auto& i : input_tokens) {
      //* Registers
      if (register_map.find(i.token) != register_map.end()) {
        output.push_back({i.token, tt_register, i.line, i.position, {{true, register_map[i.token]}}}); continue;
      }

      //* Instructions
      if ((instruction_exists(i.token)+1) || i.token == "set" || i.token == "ret" || i.token == "alloc" || i.token == "allocm" || i.token == "call") {
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
        std::vector<std::pair<bool, int>> address;

        if (first_pos.find_first_not_of("0123456789") == std::string::npos) {
          address.push_back({false, std::stoi(first_pos)});
        }else{
          address.push_back({true, register_map[first_pos]});
        }

        if (second_pos.find_first_not_of("0123456789") == std::string::npos) {
          address.push_back({false, std::stoi(second_pos)});
        }else{
          address.push_back({true, register_map[second_pos]});
        }

        output.push_back({i.token, tt_address, i.line, i.position, address});

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

  std::pair<int, std::vector<function>> generate_functions(std::vector<token>& tokens) {
    std::vector<std::string> function_identifiers;
    std::vector<function> functions;

    //* Collecting all function identifiers
    for (int i = 0; i < tokens.size(); i++) {
      if (tokens[i].type == tt_func_def) {
        if (tokens[i+1].type != tt_identifier) {
          std::cerr << "Error, invalid function identifier: " << tokens[i+1].name << " on line: " << tokens[i+1].line << " at position: " << tokens[i+1].position << ".\n";
          functions.clear();
          return {-1, functions};
        }else{
          function_identifiers.push_back(tokens[i+1].name);
          i++;
        }
      }
    }

    if (std::find(function_identifiers.begin(), function_identifiers.end(), "main") == function_identifiers.end()) {
      std::cerr << "Error, no \"main\" function found.\n";
      functions.clear();
      return {-1, functions};
    }

    //* Parsing instructions
    function current_function;
    current_function.name = "";

    for (int i = 0; i < tokens.size(); i++) {
      if (tokens[i].type == tt_func_def) {
        current_function.name = tokens[i+1].name;
        if (tokens[i+2].type != tt_open_bracket) {
          std::cerr << "Error, invalid function opening: " << tokens[i+2].name << " on line: " << (tokens[i+2].line+1) << " at position: " << (tokens[i+2].position+1) << ".\n";
          std::cerr << "It should be an open bracket: [\n";
          functions.clear();
          return {-1, functions};
        }else{
          i += 2;
        }
      }else if(tokens[i].type == tt_closed_bracket) {
        if (current_function.name == "") {
          std::cerr << "Error, no function to close on: " << tokens[i].name << " on line: " << (tokens[i].line+1) << " at position: " << (tokens[i].position+1) << ".\n";
          functions.clear();
          return {-1, functions};
        }else{
          functions.push_back(current_function);
          current_function.name = "";
          current_function.intructions.clear();
        }
      }else if(tokens[i].type == tt_instruction) {
        std::vector<int> new_instruction;
        int instruction_index = instruction_exists(tokens[i].name);
        if (instruction_index != -1) {
          new_instruction.push_back(instructions_def[instruction_index].op_code);
          for (int j = 0; j < instructions_def[instruction_index].arguments.size(); j++) {
            if (tokens[i+j+1].type == instructions_def[instruction_index].arguments[j]) {
              new_instruction.push_back(tokens[i+j+1].values[0].second);
            }else{
              std::cerr << "Invalid argument: " << tokens[i+j+1].name << " on line: " << (tokens[i+j+1].line+1) << " at position: " << (tokens[i+j+1].position+1) << ".\n";
              std::cerr << "Argument should be: " << token_types_map[instructions_def[instruction_index].arguments[j]] << ".\n";
              functions.clear();
              return {-1, functions};
            }
          }
          i += instructions_def[instruction_index].arguments.size();
        }else if (tokens[i].name == "set") {
          if (tokens[i+1].type == tt_register && tokens[i+2].type == tt_value) {
            new_instruction.push_back(0x01);
            new_instruction.push_back(tokens[i+1].values[0].second);
            new_instruction.push_back(tokens[i+2].values[0].second);
          }else if (tokens[i+1].type == tt_register && tokens[i+2].type == tt_register) {
            new_instruction.push_back(0x02);
            new_instruction.push_back(tokens[i+1].values[0].second);
            new_instruction.push_back(tokens[i+2].values[0].second);
          }else if (tokens[i+1].type == tt_register && tokens[i+2].type == tt_address) {
            if (!tokens[i+2].values[0].first && !tokens[i+2].values[1].first) {
              new_instruction.push_back(0x03);
            }else if (tokens[i+2].values[0].first && !tokens[i+2].values[1].first) {
              new_instruction.push_back(0x04);
            }else if (!tokens[i+2].values[0].first && tokens[i+2].values[1].first) {
              new_instruction.push_back(0x05);
            }else if (tokens[i+2].values[0].first && tokens[i+2].values[1].first) {
              new_instruction.push_back(0x06);
            }
            new_instruction.push_back(tokens[i+1].values[0].second);
            new_instruction.push_back(tokens[i+2].values[0].second);
            new_instruction.push_back(tokens[i+2].values[1].second);
          }else if (tokens[i+1].type == tt_address && tokens[i+2].type == tt_register) {
            if (!tokens[i+1].values[0].first && !tokens[i+1].values[1].first) {
              new_instruction.push_back(0x07);
            }else if (tokens[i+1].values[0].first && !tokens[i+1].values[1].first) {
              new_instruction.push_back(0x08);
            }else if (!tokens[i+1].values[0].first && tokens[i+1].values[1].first) {
              new_instruction.push_back(0x09);
            }else if (tokens[i+1].values[0].first && tokens[i+1].values[1].first) {
              new_instruction.push_back(0x0A);
            }
            new_instruction.push_back(tokens[i+1].values[0].second);
            new_instruction.push_back(tokens[i+1].values[1].second);
            new_instruction.push_back(tokens[i+2].values[0].second);
          }else{
            std::cerr << "Invalid arguments: " << tokens[i+1].name << " on line: " << (tokens[i+1].line+1) << " at position: " << (tokens[i+1].position+1) << ".\n";
            std::cerr << "Invalid arguments: " << tokens[i+2].name << " on line: " << (tokens[i+2].line+1) << " at position: " << (tokens[i+2].position+1) << ".\n";
            std::cerr << "Argument #1 should be a register or an address.\n";
            std::cerr << "Argument #2 should be a register or an address.\n";
            functions.clear();
            return {-1, functions};
          }
          i += 2;
        }else if (tokens[i].name == "ret") {
          new_instruction.push_back(0x1C);
          int index = 1;
          while (tokens[i+index].type == tt_value) {
            new_instruction.push_back(tokens[i+index].values[0].second);
            i++;
          }
        }else if (tokens[i].name == "alloc") {
          if (tokens[i+1].type == tt_value) {
            new_instruction.push_back(0x1D);
            new_instruction.push_back(tokens[i+1].values[0].second);
          }else if (tokens[i+1].type == tt_register) {
            new_instruction.push_back(0x1E);
            new_instruction.push_back(tokens[i+1].values[0].second);
          }else{
            std::cerr << "Invalid argument: " << tokens[i+1].name << " on line: " << (tokens[i+1].line+1) << " at position: " << (tokens[i+1].position+1) << ".\n";
            std::cerr << "Argument should be a value or a register.\n";
            functions.clear();
            return {-1, functions};
          }
          i++;
        }else if (tokens[i].name == "allocm") {
          if (tokens[i+1].type == tt_value && tokens[i+2].type == tt_value) {
            new_instruction.push_back(0x1F);
          }else if (tokens[i+1].type == tt_register && tokens[i+2].type == tt_value) {
            new_instruction.push_back(0x20);
          }else if (tokens[i+1].type == tt_value && tokens[i+2].type == tt_register) {
            new_instruction.push_back(0x21);
          }else if (tokens[i+1].type == tt_register && tokens[i+2].type == tt_register) {
            new_instruction.push_back(0x22);
          }else{
            std::cerr << "Invalid arguments: " << tokens[i+1].name << " on line: " << (tokens[i+1].line+1) << " at position: " << (tokens[i+1].position+1) << ".\n";
            std::cerr << "Invalid arguments: " << tokens[i+2].name << " on line: " << (tokens[i+2].line+1) << " at position: " << (tokens[i+2].position+1) << ".\n";
            std::cerr << "Argument #1 should be a register or a value.\n";
            std::cerr << "Argument #2 should be a register or a value.\n";
            functions.clear();
            return {-1, functions};
          }
          new_instruction.push_back(tokens[i+1].values[0].second);
          new_instruction.push_back(tokens[i+2].values[0].second);
          i += 2;
        }else if (tokens[i].name == "call") {
          new_instruction.push_back(0x1B);
          new_instruction.push_back(std::distance(function_identifiers.begin(), std::find(function_identifiers.begin(), function_identifiers.end(), tokens[i+1].name)));
          int index = 2;
          while (tokens[i+index].type == tt_value) {
            new_instruction.push_back(tokens[i+index].values[0].second);
            i++;
          }
          i++;
        }
        current_function.intructions.push_back(new_instruction);
      }else{
        std::cerr << "Error, invalid token: " << tokens[i].name << " on line: " << (tokens[i].line+1) << " at position: " << (tokens[i].position+1) << ".\n";
        functions.clear();
        return {-1, functions};
      }
    }

    return {std::distance(function_identifiers.begin(), std::find(function_identifiers.begin(), function_identifiers.end(), "main")), functions};
  }

  void compile(std::vector<parser::token>& input_tokens, std::string output_path) {
    std::vector<compiler::token> tokens = compiler::classify_tokens(input_tokens);
    std::pair<int, std::vector<function>> functions = generate_functions(tokens);

    if (functions.first != -1) {
      std::fstream output_file(output_path, std::ios::out);
      output_file << std::to_string(functions.first) << '\n';

      for (auto& i : functions.second) {
        for (auto& j : i.intructions) {
          for (auto& k : j) {
            output_file << std::to_string(k) << ' ';
          }
          output_file << '\n';
        }
        output_file << "###\n";
      }

      output_file.close();
    }
  }
}