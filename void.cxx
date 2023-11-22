#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <array>
#include <functional>
#include <regex>

// utility funcs
template<class _container, class _Ty>
inline bool in_list(_container _C, const _Ty& _Val) {
    return std::find(_C.begin(), _C.end(), _Val) != _C.end();
}

template<class _container, class _Ty>
inline int find(_container _C, const _Ty& _Val) {
    auto it = std::find(_C.begin(), _C.end(), _Val);
    return (it == _C.end() ? -1 : it-_C.begin());
}

namespace chtype {
    enum Char_Type {
        TYPE,
        NAME
    };
}

bool is_non_special_char(char ch, chtype::Char_Type type=chtype::NAME) {
    bool ret_val = false;
    std::vector<char> char_list;

    switch (type) {
        case chtype::TYPE:
            char_list = {'[', ']', '(', ')'};
            if (in_list(char_list, ch)) {
                ret_val = true;
            }
            [[fallthrough]];
        case chtype::NAME:
            if (((ch >= 'a') && (ch <= 'z' )) 
                    || ((ch >= 'A') && (ch <= 'Z')) 
                    || ((ch >= '0') && (ch <= '9'))
                    || ((ch == '-')  || (ch == '_'))
                    || ((ch == '\"') || (ch == '\''))
                    || ((ch == '.')  || (ch == '\\'))) {
                ret_val = true;
            }
            break;
    }
    return ret_val;
}

bool is_ws(char ch) {
    std::array<char, 4> ws = {' ', 0x09, 0x15, 0x12};
    return in_list(ws, ch);
}

bool is_punc(char ch) {
    std::array<char, 6> punc = {';', '(', ')', '{', '}'};
    return in_list(punc, ch);
}

bool is_operator(char ch) {
    std::array<char, 11> ops = {'+', '-', '=', '>', '<', '*', '/', 
                                '!', '|', '&', '^'};
    return in_list(ops, ch);
}

bool is_keyword(std::string str) {
    std::array<std::string, 2> keywords = {
        "ret", "if"
    };
    return in_list(keywords, str);
}
// dt, id, and lits are context dependant, so i'll need to pass to parser
// to figure out what they are

/*
   I'd like to support weird literals like regex literals, html, 1e5, 
   hex, etc -- for now I'll just support string, float and int though
*/
            
enum Token_Type {
    UNKNOWN, /**/
    DATATYPE,
    IDENTIFIER,
    PUNCTUATION, /**/
    OPERATOR, /**/
    KEYWORD,  /**/
    LITERAL,  /**/
    COMMENT, /**/ 
    WHITESPACE /**/
};

const std::array<std::string, 9> str_token_type {
    "UNKNOWN", /**/
    "DATATYPE",
    "IDENTIFIER",
    "PUNCTUATION", /**/
    "OPERATOR", /**/
    "KEYWORD",  /**/
    "LITERAL",  
    "COMMENT", /**/ 
    "WHITESPACE", /**/
};

struct Token {
    Token_Type t = UNKNOWN;
    std::string val;
};

/**
  i8 x = 10;
  becomes 

  {DATA_TYPE, "i8"}, {IDENTIFIER, "x"},{OPERATOR, "="},
  {LITERAL, "10"},{PUNCTUATION, ";"}
**/

void lexer(std::string filename) {
    int vec_size = 1024;
    std::ifstream f(filename);
    std::vector<Token> tokens;
    tokens.reserve(vec_size);
    Token token;

    char ch;

    //int indx = 0;
    bool is_comment = false;

    std::function<void()> move_next = [&](){ 
        if ((token.val != "") && (token.t == UNKNOWN)) {
            std::regex literal ("\\d+\\.\\d+|\\d+|\\\"\\S*\\\"");

            // check for keywords
            if (is_keyword(token.val)) {
                token.t = KEYWORD;
            } 

            // check if literal
            else if (std::regex_match(token.val, literal)) {
                token.t = LITERAL;
            }
        }
        else if ((token.t == OPERATOR) && (token.val == "//")) {
            is_comment = true;
        }

        if (!(token.t == UNKNOWN && token.val == "")) tokens.push_back(token);
        token = {UNKNOWN, ""};
    };

    while (f >> std::noskipws >> ch) {
        if (!is_comment) {
            if (is_operator(ch)) {
                if (token.t != OPERATOR) move_next();
                token.t = OPERATOR;
                token.val += ch;
            }
            else if (is_non_special_char(ch, chtype::TYPE)) {
                if (token.t != UNKNOWN) move_next();
                token.t = UNKNOWN;
                token.val += ch;
            } 
            else if (is_ws(ch)) {
                move_next();
                token.t = WHITESPACE;
                token.val = ch;
            }
            else if (is_punc(ch)) {
                move_next();
                token.t = PUNCTUATION;
                token.val = ch;
            } 
        } 
        else if (ch == 0xA || ch == 0xF) {
            is_comment = false;
        }
    }
    move_next(); // catch last token

    // filter out comment operators and whitespace
    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), 
                 [](const Token& t) {
                    return (t.t == WHITESPACE) || (t.val == "//");
                }), tokens.end());

    // figure out if unknowns are types or idsR

    for (int i=0; i<(int)tokens.size(); i++) {
        if (tokens.at(i).t == UNKNOWN) {
            if (i == 0) {
                tokens.at(i).t = DATATYPE;
            } 
            else if (tokens.at(i-1).t == DATATYPE) {
                // if previous is a type, this is an id
                tokens.at(i).t = IDENTIFIER;
            }
            else if (tokens.at(i-1).t == IDENTIFIER) {
                // if previous is id, change it to type, and next is id
                tokens.at(i-1).t = DATATYPE;
                tokens.at(i).t = IDENTIFIER;
            } else {
                // lone unknown not at start must be an id
                tokens.at(i).t = IDENTIFIER;
            }
        }
    }

    for (auto t : tokens) {
        std::cout << "{ " << str_token_type.at(t.t) << ", \"" 
                  << t.val << "\" },\n";
    }
}

int main(int argc, char** argv) {
    std::string filename;

    if (argc >= 2) {
        filename = argv[1];
    } else {
        std::cerr << "Error: no files supplied\n";
        return 1;
    }

    int dot_pos = find(filename, '.');

    if (dot_pos != 1) {
        if (in_list(filename, ' ')
                || filename.substr(dot_pos, 5) != ".void") {
            std::cerr << "Error: incorrect file format\n" 
                      << "void only accepts .void files\n";
            return 1;
        }
    } else {
        std::cerr << "Error: incorrect file format\n" 
                  << "void only accepts .void files\n";
        return 1;
    }

    lexer(filename);
}
