#pragma once
#include <map>
#include <string>
#include <vector>

#include "token.hpp"

class Scanner {
  std::string source;
  std::vector<Token> tokens;
  size_t current, start;
  int line;

public:
  Scanner(std::string);
  std::vector<Token> &scanTokens();

private:
  void addToken(TokenType);
  void addNumber();
  void addIdentifier();
  void addString();

  char peek() const;
  char peekNext() const;
  char advance();
  bool match(char);
  void expect(char);
  bool isAtEnd() const;
};
