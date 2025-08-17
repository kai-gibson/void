#include <fstream>
#include <filesystem>
#include <format>

#include "utils.h"
#include "error.h"

Result<std::vector<std::string>> read_file_lines(const std::string& filename) {
  std::ifstream ifs(filename);

  if (!ifs.is_open()) {
    return StrError::unexpected(
        std::format("Couldn't find specified file: {}", filename));
  }

  std::vector<std::string> lines;
  std::string line;

  while (std::getline(ifs, line)) lines.push_back(line);

  return lines;
}

Result<std::string> read_entire_file(const std::string_view filename) {
  std::ifstream file(filename.data());
  if (!file) {
    return StrError::unexpected(
        std::format("Couldn't find specified file: {}", filename));
  }

  file >> std::noskipws;

  namespace fs = std::filesystem;

  auto file_size = fs::file_size(filename);
  std::string file_data(file_size, '\0');

  file.read(file_data.data(), file_size);
  return file_data;
}
