#include "parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(tokens), position(0) {}

std::vector<std::shared_ptr<Stmt>> Parser::parseProgram() {
  auto stmts = std::vector<std::shared_ptr<Stmt>>();
  while (!match(TokenType::T_EOF)) {
    stmts.push_back(statement());
  }
  return stmts;
}
std::shared_ptr<Stmt> Parser::statement() {
  if (match(TokenType::T_VAR)) {
    if (!match(TokenType::T_IDENTIFIER))
      throw "Malformed var decl";
    auto ident = prev().lexeme;
    std::shared_ptr<Expr> init = nullptr;
    if (match(TokenType::T_EQUAL)) {
      init = expression();
    }
    if (!match(TokenType::T_SEMICOLON))
      throw "Missing semicolon";
    return std::make_shared<VarDecl>(ident, init);
  }
  if (match(TokenType::T_IF)) {
    expect(TokenType::T_LEFT_PAREN);
    auto cond = expression();
    expect(TokenType::T_RIGHT_PAREN);
    auto ifTrue = statement();
    std::shared_ptr<Stmt> ifFalse;
    if (match(TokenType::T_ELSE)) {
      ifFalse = statement();
    }
    return std::make_shared<If>(cond, ifTrue, ifFalse);
  }
  if (match(TokenType::T_WHILE)) {
    expect(TokenType::T_LEFT_PAREN);
    auto cond = expression();
    expect(TokenType::T_RIGHT_PAREN);
    auto body = statement();
    return std::make_shared<While>(cond, body);
  }
  if (match(TokenType::T_PRINT)) {
    return std::make_shared<Print>(expression());
  }
  if (match(TokenType::T_LEFT_BRACE)) {
    std::vector<std::shared_ptr<Stmt>> body;
    while (!match(TokenType::T_RIGHT_BRACE)) {
      body.push_back(statement());
    }
    return std::make_shared<Block>(body);
  }
  auto lit = expression();
  return std::make_shared<ExpressionStmt>(lit);
}

std::shared_ptr<Expr> Parser::expression() { return assignment(); }

std::shared_ptr<Expr> Parser::assignment() {
  auto lhs = equality();
  while (match(TokenType::T_EQUAL)) {
    Token t = prev();
    auto rhs = equality();
    lhs = std::make_shared<Binop>(BinopType::ASSIGN, lhs, rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::equality() {
  auto lhs = comparison();
  while (match(TokenType::T_EQUAL_EQUAL) || match(TokenType::T_BANG_EQUAL)) {
    Token t = prev();
    auto rhs = multiplication();
    lhs = std::make_shared<Binop>(
        t.type == TokenType::T_EQUAL_EQUAL ? BinopType::EQ : BinopType::NE, lhs,
        rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::comparison() {
  auto lhs = addition();
  while (match(TokenType::T_LESS) || match(TokenType::T_LESS_EQUAL) ||
         match(TokenType::T_GREATER) || match(TokenType::T_GREATER_EQUAL)) {
    Token t = prev();
    auto rhs = multiplication();
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
  auto lhs = multiplication();
  while (match(TokenType::T_PLUS) || match(TokenType::T_MINUS)) {
    Token t = prev();
    auto rhs = multiplication();
    lhs = std::make_shared<Binop>(t.type == TokenType::T_PLUS ? BinopType::ADD
                                                              : BinopType::SUB,
                                  lhs, rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::multiplication() {
  auto lhs = primary();
  while (match(TokenType::T_STAR) || match(TokenType::T_SLASH)) {
    Token t = prev();
    auto rhs = primary();
    lhs = std::make_shared<Binop>(t.type == TokenType::T_STAR ? BinopType::MUL
                                                              : BinopType::DIV,
                                  lhs, rhs);
  }
  return lhs;
}

std::shared_ptr<Expr> Parser::primary() {
  if (match(TokenType::T_NUMBER)) {
    double val = std::stod(prev().lexeme);
    return std::make_shared<Literal>(val);
  }
  if (match(TokenType::T_IDENTIFIER)) {
    return std::make_shared<Variable>(prev().lexeme);
  }
  if (match(TokenType::T_LEFT_PAREN)) {
    auto expr = expression();
    if (!match(TokenType::T_RIGHT_PAREN)) {
      throw "T_RIGHT_PAREN expected";
    }
    return expr;
  }
  throw "This is not an expression";
}

Token Parser::peek() const { return tokens[position]; }
Token Parser::prev() const { return tokens[position - 1]; }
bool Parser::check(TokenType type) const { return peek().type == type; }
bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}
void Parser::expect(TokenType type) {
  if (!match(type))
    throw "Unexpected token";
}
Token Parser::advance() { return tokens[position++]; }
bool Parser::isAtEnd() const { return position >= tokens.size(); }
