#include "scanner.hpp"

constexpr bool is_numeric(char ch) { return ch >= '0' && ch <= '9'; }

constexpr bool is_alpha(char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

constexpr bool is_alphanumeric(char ch) {
  return is_alpha(ch) || is_numeric(ch);
}

std::map<std::string, TokenType> keywords = {
    {"and", TokenType::T_AND},       {"class", TokenType::T_CLASS},
    {"else", TokenType::T_ELSE},     {"false", TokenType::T_FALSE},
    {"for", TokenType::T_FOR},       {"fun", TokenType::T_FUN},
    {"if", TokenType::T_IF},         {"nil", TokenType::T_NIL},
    {"or", TokenType::T_OR},         {"print", TokenType::T_PRINT},
    {"return", TokenType::T_RETURN}, {"super", TokenType::T_SUPER},
    {"this", TokenType::T_THIS},     {"true", TokenType::T_TRUE},
    {"var", TokenType::T_VAR},       {"while", TokenType::T_WHILE},
};

Scanner::Scanner(std::string source) : source(source), current(0), start(0) {}

std::vector<Token> &Scanner::scanTokens() {
  while (!this->isAtEnd()) {
    this->start = this->current;
    auto token = this->advance();
    switch (token) {
    case '(':
      this->addToken(TokenType::T_LEFT_PAREN);
      break;
    case ')':
      this->addToken(TokenType::T_RIGHT_PAREN);
      break;
    case '{':
      this->addToken(TokenType::T_LEFT_BRACE);
      break;
    case '}':
      this->addToken(TokenType::T_RIGHT_BRACE);
      break;
    case ',':
      this->addToken(TokenType::T_COMMA);
      break;
    case '.':
      this->addToken(TokenType::T_DOT);
      break;
    case '+':
      this->addToken(TokenType::T_PLUS);
      break;
    case '-':
      this->addToken(TokenType::T_MINUS);
      break;
    case '*':
      this->addToken(TokenType::T_STAR);
      break;
    case '!':
      this->addToken(this->match('=') ? TokenType::T_BANG_EQUAL
                                      : TokenType::T_BANG);
      break;
    case '=':
      this->addToken(this->match('=') ? TokenType::T_EQUAL_EQUAL
                                      : TokenType::T_EQUAL);
      break;
    case '<':
      this->addToken(this->match('=') ? TokenType::T_LESS_EQUAL
                                      : TokenType::T_LESS);
      break;
    case '>':
      this->addToken(this->match('=') ? TokenType::T_GREATER_EQUAL
                                      : TokenType::T_GREATER);
      break;
    case ';':
      this->addToken(TokenType::T_SEMICOLON);
      break;
    case '/':
      if (match('/')) {
        while (this->peek() != '\n')
          this->advance();
        break;
      }
      this->addToken(TokenType::T_SLASH);
    case '\n':
      this->line += 1;
    case ' ':
    case '\t':
      break;
    case '"':
      this->addString();
      break;
    default:
      if (is_numeric(token)) {
        this->addNumber();
      } else if (is_alpha(token)) {
        this->addIdentifier();
      } else {
        throw "Unknown token found";
      }
    }
  }
  this->addToken(TokenType::T_EOF);
  return this->tokens;
}

void Scanner::addNumber() {
  while (is_numeric(this->peek()))
    this->advance();
  if (this->peek() == '.' && is_numeric(this->peekNext())) {
    this->advance();
    while (is_numeric(this->peek()))
      this->advance();
  }
  this->addToken(TokenType::T_NUMBER);
}

void Scanner::addIdentifier() {
  while (is_alphanumeric(this->peek()))
    this->advance();
  auto ident = this->source.substr(this->start, this->current - this->start);
  if (keywords.count(ident) != 0)
    this->addToken(keywords[ident]);
  else
    this->addToken(TokenType::T_IDENTIFIER);
}

void Scanner::addString() {
  while (!this->isAtEnd() && !this->match('"'))
    this->advance();
  this->addToken(TokenType::T_STRING);
}

void Scanner::addToken(TokenType type) {
  this->tokens.push_back(
      Token{type, this->source.substr(this->start, this->current - this->start),
            this->start});
}

char Scanner::peek() const { return this->source[this->current]; }
char Scanner::peekNext() const { return this->source[this->current + 1]; }

char Scanner::advance() {
  auto current = this->peek();
  this->current += 1;
  return current;
}

bool Scanner::match(char ch) {
  if (this->peek() == ch) {
    this->advance();
    return true;
  }
  return false;
}

bool Scanner::isAtEnd() const { return this->current >= this->source.length(); }
