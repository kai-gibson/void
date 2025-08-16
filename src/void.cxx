#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::expected<std::vector<std::string>, std::string> read_file_lines(
    const std::string_view filename) {
  std::ifstream ifs(filename);

  if (!ifs.is_open()) {
    return std::unexpected(
        std::format("Couldn't find specified file: {}", filename));
  }

  std::vector<std::string> lines;
  std::string line;

  while (std::getline(ifs, line)) lines.push_back(line);

  return lines;
}

std::string read_entire_file(const std::string_view filename) {
  std::ifstream ifs(filename);
  if (!ifs.is_open()) throw std::runtime_error("unable to read file");

  namespace fs = std::filesystem;

  auto file_size = fs::file_size(filename);
  std::string file_data(file_size, '\0');

  ifs.read(file_data.data(), file_size);
  return file_data;
}

const constexpr auto KEYWORDS = {
    "fn", 
    "struct", 
    "enum",  
    "has",  
    "loop",       
    "in",
    "if",   
    "break",  
    "continue", 
    "else", 
    "interface",
    "switch",
    "const",
    "var",
};

const constexpr auto OPERATORS = {
    "<", ">", "?", "!", "!=", "==", "and", "or", "xor",
};

const constexpr auto SYMBOLS = {
    ";",
    ":",
    "=",
};

void parse_file(const std::vector<std::string>& lines) {
  for (auto& line : lines) {
    for (auto& chr : line) {
      switch (chr) {}
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage:\n\tvoid filename.void\n";
  }

  auto lines = read_file_lines(argv[1]); 
  if (!lines) {
    std::cerr << lines.error() << "\n";
    return EXIT_FAILURE;
  } else {
    lines = lines.value();
  }

  for (auto& line : *lines) {
    std::cout << "line: " << line << "\n";
  }
}
