#pragma once
#include "ast.hpp"
#include "token.hpp"
#include <vector>

class Parser {
  std::vector<Token> tokens;
  size_t position;

public:
  Parser(std::vector<Token>);
  std::shared_ptr<Stmt> parseProgram();

private:
  std::shared_ptr<Expr> expression();
  std::shared_ptr<Expr> assignment();
  std::shared_ptr<Expr> equality();
  std::shared_ptr<Expr> comparison();
  std::shared_ptr<Expr> addition();
  std::shared_ptr<Expr> multiplication();
  std::shared_ptr<Expr> primary();

  Token peek() const;
  Token prev() const;
  bool check(TokenType) const;
  bool match(TokenType);
  Token advance();
};
