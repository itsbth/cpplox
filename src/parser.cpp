#include "parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(tokens), position(0) {}

std::shared_ptr<Stmt> Parser::parseProgram() {
  auto lit = this->expression();
  return std::make_shared<ExpressionStmt>(lit);
}

std::shared_ptr<Expr> Parser::expression() { return this->assignment(); }

std::shared_ptr<Expr> Parser::assignment() {
  auto lhs = this->equality();
  while (this->match(TokenType::T_EQUAL)) {
    Token t = this->prev();
    auto rhs = this->equality();
    lhs = std::make_shared<Binop>(BinopType::ASSIGN, lhs, rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::equality() {
  auto lhs = this->comparison();
  while (this->match(TokenType::T_EQUAL_EQUAL) ||
         this->match(TokenType::T_BANG_EQUAL)) {
    Token t = this->prev();
    auto rhs = this->multiplication();
    lhs = std::make_shared<Binop>(
        t.type == TokenType::T_EQUAL_EQUAL ? BinopType::EQ : BinopType::NE, lhs,
        rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::comparison() {
  auto lhs = this->addition();
  while (this->match(TokenType::T_LESS) ||
         this->match(TokenType::T_LESS_EQUAL) ||
         this->match(TokenType::T_GREATER) ||
         this->match(TokenType::T_GREATER_EQUAL)) {
    Token t = this->prev();
    auto rhs = this->multiplication();
    lhs = std::make_shared<Binop>(t.type == TokenType::T_LESS
                                      ? BinopType::LT
                                      : t.type == TokenType::T_LESS_EQUAL
                                            ? BinopType::LE
                                            : t.type == TokenType::T_GREATER
                                                  ? BinopType::GT
                                                  : BinopType::GE,
                                  lhs, rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::addition() {
  auto lhs = this->multiplication();
  while (this->match(TokenType::T_PLUS) || this->match(TokenType::T_MINUS)) {
    Token t = this->prev();
    auto rhs = this->multiplication();
    lhs = std::make_shared<Binop>(t.type == TokenType::T_PLUS ? BinopType::ADD
                                                              : BinopType::SUB,
                                  lhs, rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::multiplication() {
  auto lhs = this->primary();
  while (this->match(TokenType::T_STAR) || this->match(TokenType::T_SLASH)) {
    Token t = this->prev();
    auto rhs = this->primary();
    lhs = std::make_shared<Binop>(t.type == TokenType::T_STAR ? BinopType::MUL
                                                              : BinopType::DIV,
                                  lhs, rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::primary() {
  if (this->match(TokenType::T_NUMBER)) {
    double val = std::stod(this->prev().lexeme);
    return std::make_shared<Literal>(val);
  }
  if (this->match(TokenType::T_IDENTIFIER)) {
    return std::make_shared<Variable>(this->prev().lexeme);
  }
  if (this->match(TokenType::T_LEFT_PAREN)) {
    auto expr = this->expression();
    if (!this->match(TokenType::T_RIGHT_PAREN)) {
      throw "T_RIGHT_PAREN expected";
    }
    return expr;
  }
  throw "This is not an expression";
}

Token Parser::peek() const { return this->tokens[this->position]; }
Token Parser::prev() const { return this->tokens[this->position - 1]; }
bool Parser::check(TokenType type) const { return this->peek().type == type; }
bool Parser::match(TokenType type) {
  if (this->check(type)) {
    this->advance();
    return true;
  }
  return false;
}
Token Parser::advance() { return this->tokens[this->position++]; }
