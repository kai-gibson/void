#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <source_location>
#include <string>
#include <vector>

template <class Container, class Value>
bool in_list(Container& list, const Value& value) {
  if (std::find(list.begin(), list.end(), value) != list.end()) {
    return true;
  }

  return false;
}

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

  const char* error() override { return message.c_str(); }

  const std::source_location& where() override { return location; }

  StrError(std::string message, const std::source_location& location)
      : location(location), message(message) {}

  static Error init(std::string message, const std::source_location& location =
                                             std::source_location::current()) {
    return std::make_unique<StrError>(std::move(message), location);
  }

  static std::unexpected<Error> unexpected(
      std::string message,
      const std::source_location& location = std::source_location::current()) {
    return std::unexpected<Error>(
        std::make_unique<StrError>(std::move(message), location));
  }
};

std::ostream& operator<<(std::ostream& os, Error& err) {
  const char* last_slash = strrchr(err->where().file_name(), '/');
  return os << "Error: " << err->error() << "\n"
            << "  At: " << err->where().function_name() << " "
            << "[" << (last_slash ? last_slash + 1 : err->where().file_name())
            << ":" << err->where().line() << ":" << err->where().column()
            << "]";
}

template <class T>
using Result = std::expected<T, Error>;

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

const constexpr auto KEYWORDS = {
    "fn",    "struct", "enum",     "has",    "loop",      "in",
    "if",    "break",  "continue", "else",   "interface", "switch",
    "const", "var",    "import",   "return", "module",
};

const constexpr auto OPERATORS = {
    "<", ">", "?",  "!",  "!=", "==", "and", "or", "xor", "+",  "-", "*",
    "/", "%", "++", "--", "=",  "+=", "-=",  "*=", "/=",  "%=", "?",
};

const constexpr auto SYMBOLS = {";", ":", "=", ",", ".", "{",
                                "}", "[", "]", "(", ")", "->"};

const constexpr auto WHITESPACE = {
    " ",
    "\t",
    "\n",
    "\r",
};

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

  void reset() {
    this->type = Type::None;
    this->value.clear();
    this->location = SourceLocation{"", 0, 0};
  }
};

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

std::vector<Token> parse_file(
  const std::string& file_name, 
  const std::string& file_data
) {
  std::vector<Token> tokens;
  Token current_token(Token::Type::None, "", SourceLocation{});

  enum class CommentType : uint8_t { MultiLine, SingleLine } comment_type;

  int64_t line = 1;
  int64_t column = 0;
  // break up into words, find comments
  for (int64_t i = 0; i < file_data.size(); i++) {
    char chr = file_data[i];

    auto is_whitespace = in_list(WHITESPACE, std::string(1, chr));
    auto is_symbol = in_list(SYMBOLS, std::string(1, chr));

    if (chr == '\n') {
      line++;
      column = 1;
    } else {
      column++;
    }

    if (is_whitespace or is_symbol) {
      if (current_token.type == Token::Type::Comment) {
        if (chr == '\n' and comment_type == CommentType::SingleLine) {
          tokens.push_back(std::move(current_token));

          current_token.reset();
          continue;
        } else {
          continue;
        }
      }

      if (current_token.value.size() > 0) {
        tokens.push_back(std::move(current_token));
      }

      // add whitespace or symbol character as a token
      if (is_whitespace) {
        current_token.type = Token::Type::Whitespace;
      } else if (is_symbol) {
        current_token.type = Token::Type::Symbol;
      } 

      current_token.value = chr;
      current_token.location = SourceLocation{
        .file_name = file_name, 
        .line = line,
        .column = column, 
      };  
      tokens.push_back(std::move(current_token));

      current_token.reset();
    } else {
      current_token.value.push_back(chr);
      // todo add location info
      current_token.location = SourceLocation{
        .file_name = file_name, 
        .line = line,
        .column = column, 
      };  
    }

    if (chr == '/' and current_token.value.back() == '/') {
      current_token.type = Token::Type::Comment;
      comment_type = CommentType::SingleLine;
    } else if (chr == '*' and current_token.value.back() == '/') {
      current_token.type = Token::Type::Comment;
      comment_type = CommentType::MultiLine;
    }

    if (current_token.type == Token::Type::Comment and
        comment_type == CommentType::MultiLine) {
      if (chr == '/' and current_token.value.back() == '*') {
      }
    }
  }

  // identify remaining tokens
  for (auto& token : tokens) {
    // we already know whitespace characters and comments
    if (token.type == Token::Type::Whitespace) continue;
    if (token.type == Token::Type::Comment) continue;

    if (in_list(KEYWORDS, token.value)) {
      token.type = Token::Type::Keyword;
    } else if (in_list(OPERATORS, token.value)) {
      token.type = Token::Type::Operator;
    } else if (in_list(SYMBOLS, token.value)) {
      token.type = Token::Type::Symbol;
    } else {
      // token.type = Token::Type::Identifier;
      //  literal
      //  identifier
    }
  }

  return tokens;
}

void create_ast() {}

int main(int argc, char** argv) {
  std::cout << "start\n";
  if (argc != 2) {
    std::cerr << "usage:\n\tvoid filename.void\n";
    return EXIT_FAILURE;
  }

  auto file_data = read_entire_file(argv[1]);
  if (!file_data) {
    std::cerr << file_data.error() << "\n";
    return EXIT_FAILURE;
  }

  const char* last_slash = strrchr(argv[1], '/');

  auto tokens = parse_file((last_slash ? last_slash + 1 : argv[1]), *file_data);
  

  for (auto& token : tokens) {
    std::cout << token << "\n";
  }
}
