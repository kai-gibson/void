#include "types.h"

void Token::reset() {
    this->type = Type::None;
    this->value.clear();
    this->location = SourceLocation{"", 0, 0};
}

std::ostream& operator<<(std::ostream& os, Token::Type& token_type) {
  switch (token_type) {
    case Token::Type::None:
      os << "None";
      break;
    case Token::Type::Keyword:
      os << "Keyword";
      break;
    case Token::Type::Operator:
      os << "Operator";
      break;
    case Token::Type::Symbol:
      os << "Symbol";
      break;
    case Token::Type::Identifier:
      os << "Identifier";
      break;
    case Token::Type::Literal:
      os << "Literal";
      break;
    case Token::Type::Comment:
      os << "Comment";
      break;
    case Token::Type::Whitespace:
      os << "Whitespace";
      break;
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, Token& token) {
  return os << "{"
            << ".type=" << token.type 
            << ", .value=\"" << token.value << "\""
            << ", .location={"
              << "\"file_name\":\"" << token.location.file_name << "\", "
              << "\"line\":" << token.location.line << ", "
              << "\"column\":" << token.location.column
              << "}}";
}
