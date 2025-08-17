#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "error.h"

template <class Container, class Value>
bool in_list(Container& list, const Value& value) {
  if (std::find(list.begin(), list.end(), value) != list.end()) {
    return true;
  }

  return false;
}

Result<std::vector<std::string>> read_file_lines(const std::string& filename);
Result<std::string> read_entire_file(const std::string_view filename);

#endif // UTILS_H
