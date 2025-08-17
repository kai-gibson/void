#include "parser.h"
#include "constants.h"
#include "utils.h"

std::vector<Token> tokenise_file(
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
