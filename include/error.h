#ifndef ERROR_H
#define ERROR_H
#include <source_location>
#include <string>
#include <memory>
#include <expected>
#include <iostream>

class ErrorInterface {
 public:
  virtual const char* error() = 0;
  virtual const std::source_location& where() = 0;
  virtual ~ErrorInterface() = default;
};

typedef std::unique_ptr<ErrorInterface> Error;

class StrError : public ErrorInterface {
 public:
  std::string message;
  std::source_location location;

  const char* error() override;

  const std::source_location& where() override;

  StrError(std::string message, const std::source_location& location)
      : location(location), message(message) {}

  static Error init(std::string message, const std::source_location& location =
                                             std::source_location::current());

  static std::unexpected<Error> unexpected(
      std::string message,
      const std::source_location& location = std::source_location::current());
};

std::ostream& operator<<(std::ostream& os, Error& err);

template <class T>
using Result = std::expected<T, Error>;

#endif // ERROR_H
