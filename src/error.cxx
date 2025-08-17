#include "error.h"

const char* StrError::error() {
  return message.c_str();
}

const std::source_location& StrError::where() { 
  return location; 
}

Error StrError::init(std::string message, const std::source_location& location) {
  return std::make_unique<StrError>(std::move(message), location);
}

std::unexpected<Error> StrError::unexpected(
    std::string message,
    const std::source_location& location) {
  return std::unexpected<Error>(
      std::make_unique<StrError>(std::move(message), location));
}

std::ostream& operator<<(std::ostream& os, Error& err) {
  const char* last_slash = strrchr(err->where().file_name(), '/');
  return os << "Error: " << err->error() << "\n"
            << "  At: " << err->where().function_name() << " "
            << "[" << (last_slash ? last_slash + 1 : err->where().file_name())
            << ":" << err->where().line() << ":" << err->where().column()
            << "]";
}
