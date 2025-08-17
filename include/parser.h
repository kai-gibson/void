#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include "types.h"

std::vector<Token> tokenise_file(
  const std::string& file_name, 
  const std::string& file_data
);

#endif // PARSER_H
