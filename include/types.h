#ifndef TYPES_H
#define TYPES_H
#include <cstdint>
#include <string>
#include <iostream>

struct SourceLocation {
  std::string file_name;
  int64_t line;
  int64_t column;
};

struct Token {
  enum class Type : uint8_t {
    None,
    Keyword,
    Operator,
    Symbol,
    Identifier,
    Literal,
    Comment,
    Whitespace,
  };

  Type type;
  std::string value;
  SourceLocation location;

  Token(Type type, std::string value, SourceLocation location)
      : type(type), value(std::move(value)), location(location) {}
  Token() : type(Type::None) {}

  void reset();
};

std::ostream& operator<<(std::ostream& os, Token::Type& token_type);
std::ostream& operator<<(std::ostream& os, Token& token);

#endif // TYPES_H
