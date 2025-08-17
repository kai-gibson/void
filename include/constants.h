#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <initializer_list>

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


#endif // CONSTANTS_H
