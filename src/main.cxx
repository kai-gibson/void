#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib> 

#include "utils.h"
#include "parser.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage:\n\tvoid filename.void\n";
    return EXIT_FAILURE;
  }

  auto file_data = read_entire_file(argv[1]);
  if (!file_data) {
    std::cerr << file_data.error() << "\n";
    return EXIT_FAILURE;
  }

  auto file_size = file_data->size();
  if (file_size == 0) {
    std::cerr << "file is empty\n";
    return EXIT_FAILURE;
  }

  const char* last_slash = strrchr(argv[1], '/');
  auto file_name = last_slash ? last_slash + 1 : argv[1];

  auto tokens = tokenise_file(file_name, *file_data);

  for (auto& token : tokens) {
    std::cout << token << "\n";
  }
}
