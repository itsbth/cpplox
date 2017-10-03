#pragma once
#include <string>

enum class TokenType {
  // Single-character tokens.
  T_LEFT_PAREN,
  T_RIGHT_PAREN,
  T_LEFT_BRACE,
  T_RIGHT_BRACE,
  T_COMMA,
  T_DOT,
  T_MINUS,
  T_PLUS,
  T_SEMICOLON,
  T_SLASH,
  T_STAR,

  // One or two character tokens.
  T_BANG,
  T_BANG_EQUAL,
  T_EQUAL,
  T_EQUAL_EQUAL,
  T_GREATER,
  T_GREATER_EQUAL,
  T_LESS,
  T_LESS_EQUAL,

  // Literals.
  T_IDENTIFIER,
  T_STRING,
  T_NUMBER,

  // Keywords.
  T_AND,
  T_CLASS,
  T_ELSE,
  T_FALSE,
  T_FUN,
  T_FOR,
  T_IF,
  T_NIL,
  T_OR,
  T_PRINT,
  T_RETURN,
  T_SUPER,
  T_THIS,
  T_TRUE,
  T_VAR,
  T_WHILE,

  T_EOF
};

constexpr const char *token_name(TokenType token) {
  switch (token) {
  // Single-character tokens.
  case TokenType::T_LEFT_PAREN:
    return "T_LEFT_PAREN";
  case TokenType::T_RIGHT_PAREN:
    return "T_RIGHT_PAREN";
  case TokenType::T_LEFT_BRACE:
    return "T_LEFT_BRACE";
  case TokenType::T_RIGHT_BRACE:
    return "T_RIGHT_BRACE";
  case TokenType::T_COMMA:
    return "T_COMMA";
  case TokenType::T_DOT:
    return "T_DOT";
  case TokenType::T_MINUS:
    return "T_MINUS";
  case TokenType::T_PLUS:
    return "T_PLUS";
  case TokenType::T_SEMICOLON:
    return "T_SEMICOLON";
  case TokenType::T_SLASH:
    return "T_SLASH";
  case TokenType::T_STAR:
    return "T_STAR";

  // One or two character tokens.
  case TokenType::T_BANG:
    return "T_BANG";
  case TokenType::T_BANG_EQUAL:
    return "T_BANG_EQUAL";
  case TokenType::T_EQUAL:
    return "T_EQUAL";
  case TokenType::T_EQUAL_EQUAL:
    return "T_EQUAL_EQUAL";
  case TokenType::T_GREATER:
    return "T_GREATER";
  case TokenType::T_GREATER_EQUAL:
    return "T_GREATER_EQUAL";
  case TokenType::T_LESS:
    return "T_LESS";
  case TokenType::T_LESS_EQUAL:
    return "T_LESS_EQUAL";

  // Literals.
  case TokenType::T_IDENTIFIER:
    return "T_IDENTIFIER";
  case TokenType::T_STRING:
    return "T_STRING";
  case TokenType::T_NUMBER:
    return "T_NUMBER";

  // Keywords.
  case TokenType::T_AND:
    return "T_AND";
  case TokenType::T_CLASS:
    return "T_CLASS";
  case TokenType::T_ELSE:
    return "T_ELSE";
  case TokenType::T_FALSE:
    return "T_FALSE";
  case TokenType::T_FUN:
    return "T_FUN";
  case TokenType::T_FOR:
    return "T_FOR";
  case TokenType::T_IF:
    return "T_IF";
  case TokenType::T_NIL:
    return "T_NIL";
  case TokenType::T_OR:
    return "T_OR";
  case TokenType::T_PRINT:
    return "T_PRINT";
  case TokenType::T_RETURN:
    return "T_RETURN";
  case TokenType::T_SUPER:
    return "T_SUPER";
  case TokenType::T_THIS:
    return "T_THIS";
  case TokenType::T_TRUE:
    return "T_TRUE";
  case TokenType::T_VAR:
    return "T_VAR";
  case TokenType::T_WHILE:
    return "T_WHILE";

  case TokenType::T_EOF:
    return "T_EOF";
  }
  // clang is apparently able to figure out that the case is exhaustive, but gcc
  // isn't...
  return nullptr;
}

struct Token {
  TokenType type;
  std::string lexeme;
  size_t pos;
};
