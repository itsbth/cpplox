#pragma once
#include "ast.hpp"
#include <map>
#include <string>
#include <typeinfo>

typedef double lvalue;

template <typename T> class Scope {
  std::map<std::string, T> vars;
  std::shared_ptr<Scope<T>> parent;

public:
  Scope() : parent(nullptr) {}
  Scope(std::shared_ptr<Scope<T>> parent) : parent(parent) {}

  T &operator[](const std::string &key) {
    if (vars.count(key))
      return vars[key];
    if (parent != nullptr)
      return (*parent)[key];
    throw "No such var";
  }
  void insert(std::string &key, T value) { vars[key] = value; }
};

inline bool isTruthy(stdx::any val) {
  if (val.type() == typeid(double)) {
    return stdx::any_cast<double>(val) != 0.0;
  }
  if (val.type() == typeid(bool)) {
    return stdx::any_cast<bool>(val);
  }
  return true;
}

inline std::string toString(stdx::any val) {
  if (val.type() == typeid(double)) {
    return std::to_string(stdx::any_cast<double>(val));
  }
  if (val.type() == typeid(bool)) {
    return std::to_string(stdx::any_cast<bool>(val));
  }
  return "n/a";
}

class Evaluator : StmtVisitor, ExprVisitor {
  std::shared_ptr<Scope<lvalue>> vars;

public:
  Evaluator();
  stdx::any run(std::vector<std::shared_ptr<Stmt>> &stmt);
  stdx::any run(Stmt &stmt);

private:
  virtual stdx::any visitBinop(Binop &);
  virtual stdx::any visitVariable(Variable &);
  virtual stdx::any visitCall(Call &);
  virtual stdx::any visitLiteral(Literal &);
  virtual stdx::any visitUnop(Unop &);
  virtual stdx::any visitExpressionStmt(ExpressionStmt &);
  virtual stdx::any visitWhile(While &);
  virtual stdx::any visitIf(If &);
  virtual stdx::any visitFun(Fun &);
  virtual stdx::any visitPrint(Print &);
  virtual stdx::any visitBlock(Block &);
  virtual stdx::any visitVarDecl(VarDecl &);
};