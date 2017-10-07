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
  while (!isAtEnd()) {
    start = current;
    auto token = advance();
    switch (token) {
    case '(':
      addToken(TokenType::T_LEFT_PAREN);
      break;
    case ')':
      addToken(TokenType::T_RIGHT_PAREN);
      break;
    case '{':
      addToken(TokenType::T_LEFT_BRACE);
      break;
    case '}':
      addToken(TokenType::T_RIGHT_BRACE);
      break;
    case ',':
      addToken(TokenType::T_COMMA);
      break;
    case '.':
      addToken(TokenType::T_DOT);
      break;
    case '+':
      addToken(TokenType::T_PLUS);
      break;
    case '-':
      addToken(TokenType::T_MINUS);
      break;
    case '*':
      addToken(TokenType::T_STAR);
      break;
    case '!':
      addToken(match('=') ? TokenType::T_BANG_EQUAL : TokenType::T_BANG);
      break;
    case '=':
      addToken(match('=') ? TokenType::T_EQUAL_EQUAL : TokenType::T_EQUAL);
      break;
    case '<':
      addToken(match('=') ? TokenType::T_LESS_EQUAL : TokenType::T_LESS);
      break;
    case '>':
      addToken(match('=') ? TokenType::T_GREATER_EQUAL : TokenType::T_GREATER);
      break;
    case ';':
      addToken(TokenType::T_SEMICOLON);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n')
          advance();
        break;
      }
      addToken(TokenType::T_SLASH);
    case '\n':
      line += 1;
    case ' ':
    case '\t':
      break;
    case '"':
      addString();
      break;
    default:
      if (is_numeric(token)) {
        addNumber();
      } else if (is_alpha(token)) {
        addIdentifier();
      } else {
        throw "Unknown token found";
      }
    }
  }
  addToken(TokenType::T_EOF);
  return tokens;
}

void Scanner::addNumber() {
  while (is_numeric(peek()))
    advance();
  if (peek() == '.' && is_numeric(peekNext())) {
    advance();
    while (is_numeric(peek()))
      advance();
  }
  addToken(TokenType::T_NUMBER);
}

void Scanner::addIdentifier() {
  while (is_alphanumeric(peek()))
    advance();
  auto ident = source.substr(start, current - start);
  if (keywords.count(ident) != 0)
    addToken(keywords[ident]);
  else
    addToken(TokenType::T_IDENTIFIER);
}

void Scanner::addString() {
  while (!isAtEnd() && !match('"'))
    advance();
  addToken(TokenType::T_STRING);
}

void Scanner::addToken(TokenType type) {
  tokens.push_back(Token{type, source.substr(start, current - start), start});
}

char Scanner::peek() const { return source[current]; }
char Scanner::peekNext() const { return source[current + 1]; }

char Scanner::advance() {
  auto token = peek();
  current += 1;
  return token;
}

bool Scanner::match(char ch) {
  if (peek() == ch) {
    advance();
    return true;
  }
  return false;
}

bool Scanner::isAtEnd() const { return current >= source.length(); }
