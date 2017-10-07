#pragma once
#include <experimental/any>
#include <iostream>
#include <memory>
#include <vector>

namespace stdx = std::experimental;

template <typename T> void write_maybe_null(std::ostream &os, T &o) {
  if (o != nullptr)
    os << *o;
  else
    os << "<null>";
}

template <typename T> void write_vec(std::ostream &os, std::vector<T> vec) {
  bool first = true;
  for (auto &it : vec) {
    if (!first)
      os << ", ";
    os << it;
    first = false;
  }
}

template <typename T> void write_vec_ptr(std::ostream &os, std::vector<T> vec) {
  bool first = true;
  for (auto &it : vec) {
    if (!first)
      os << ", ";
    write_maybe_null(os, it);
    first = false;
  }
}

class Node {};

enum class BinopType { ADD, SUB, MUL, DIV, LT, LE, GT, GE, EQ, NE, ASSIGN };
inline std::ostream &operator<<(std::ostream &os, const BinopType &node) {
  switch (node) {
  case BinopType::ADD:
    os << "ADD";
    break;
  case BinopType::SUB:
    os << "SUB";
    break;
  case BinopType::MUL:
    os << "MUL";
    break;
  case BinopType::DIV:
    os << "DIV";
    break;
  case BinopType::LT:
    os << "LT";
    break;
  case BinopType::LE:
    os << "LE";
    break;
  case BinopType::GT:
    os << "GT";
    break;
  case BinopType::GE:
    os << "GE";
    break;
  case BinopType::EQ:
    os << "EQ";
    break;
  case BinopType::NE:
    os << "NE";
    break;
  case BinopType::ASSIGN:
    os << "ASSIGN";
    break;
  }
  return os;
}

enum class UnopType { NEGATE, NOT };
inline std::ostream &operator<<(std::ostream &os, const UnopType &node) {
  switch (node) {
  case UnopType::NEGATE:
    os << "NEGATE";
    break;
  case UnopType::NOT:
    os << "NOT";
    break;
  }
  return os;
}

struct Binop;
struct Variable;
struct Call;
struct Literal;
struct Unop;
class ExprVisitor {
protected:
  virtual stdx::any visitBinop(Binop &) = 0;
  virtual stdx::any visitVariable(Variable &) = 0;
  virtual stdx::any visitCall(Call &) = 0;
  virtual stdx::any visitLiteral(Literal &) = 0;
  virtual stdx::any visitUnop(Unop &) = 0;
  friend Binop;
  friend Variable;
  friend Call;
  friend Literal;
  friend Unop;
};
struct Expr : Node {
  virtual void write_to(std::ostream &) const = 0;
  virtual stdx::any accept(ExprVisitor &) = 0;
};
inline std::ostream &operator<<(std::ostream &os, const Expr &node) {
  node.write_to(os);
  return os;
}

struct Binop : Expr {
  BinopType op;
  std::shared_ptr<Expr> lhs;
  std::shared_ptr<Expr> rhs;
  Binop(BinopType op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs)
      : op(op), lhs(lhs), rhs(rhs) {}
  Binop(const Binop &other) = default;
  void write_to(std::ostream &os) const {
    os << "Binop("
       << "op = " << this->op << ", "
       << "lhs = ";
    write_maybe_null(os, this->lhs);
    os << ", "
       << "rhs = ";
    write_maybe_null(os, this->rhs);
    os << ")";
  }

  stdx::any accept(ExprVisitor &visitor) { return visitor.visitBinop(*this); }
};

struct Variable : Expr {
  std::string ident;
  Variable(std::string ident) : ident(ident) {}
  Variable(const Variable &other) = default;
  void write_to(std::ostream &os) const {
    os << "Variable("
       << "ident = " << this->ident << ")";
  }

  stdx::any accept(ExprVisitor &visitor) {
    return visitor.visitVariable(*this);
  }
};

struct Call : Expr {
  std::shared_ptr<Expr> callee;
  std::vector<std::shared_ptr<Expr>> args;
  Call(std::shared_ptr<Expr> callee, std::vector<std::shared_ptr<Expr>> args)
      : callee(callee), args(args) {}
  Call(const Call &other) = default;
  void write_to(std::ostream &os) const {
    os << "Call("
       << "callee = ";
    write_maybe_null(os, this->callee);
    os << ", "
       << "args = [";
    write_vec_ptr(os, this->args);
    os << "]"
       << ")";
  }

  stdx::any accept(ExprVisitor &visitor) { return visitor.visitCall(*this); }
};

struct Literal : Expr {
  double value;
  Literal(double value) : value(value) {}
  Literal(const Literal &other) = default;
  void write_to(std::ostream &os) const {
    os << "Literal("
       << "value = " << this->value << ")";
  }

  stdx::any accept(ExprVisitor &visitor) { return visitor.visitLiteral(*this); }
};

struct Unop : Expr {
  UnopType op;
  std::shared_ptr<Expr> rhs;
  Unop(UnopType op, std::shared_ptr<Expr> rhs) : op(op), rhs(rhs) {}
  Unop(const Unop &other) = default;
  void write_to(std::ostream &os) const {
    os << "Unop("
       << "op = " << this->op << ", "
       << "rhs = ";
    write_maybe_null(os, this->rhs);
    os << ")";
  }

  stdx::any accept(ExprVisitor &visitor) { return visitor.visitUnop(*this); }
};

struct ExpressionStmt;
struct While;
struct If;
struct Fun;
struct Print;
struct Block;
struct VarDecl;
class StmtVisitor {
protected:
  virtual stdx::any visitExpressionStmt(ExpressionStmt &) = 0;
  virtual stdx::any visitWhile(While &) = 0;
  virtual stdx::any visitIf(If &) = 0;
  virtual stdx::any visitFun(Fun &) = 0;
  virtual stdx::any visitPrint(Print &) = 0;
  virtual stdx::any visitBlock(Block &) = 0;
  virtual stdx::any visitVarDecl(VarDecl &) = 0;
  friend ExpressionStmt;
  friend While;
  friend If;
  friend Fun;
  friend Print;
  friend Block;
  friend VarDecl;
};
struct Stmt : Node {
  virtual void write_to(std::ostream &) const = 0;
  virtual stdx::any accept(StmtVisitor &) = 0;
};
inline std::ostream &operator<<(std::ostream &os, const Stmt &node) {
  node.write_to(os);
  return os;
}

struct ExpressionStmt : Stmt {
  std::shared_ptr<Expr> expr;
  ExpressionStmt(std::shared_ptr<Expr> expr) : expr(expr) {}
  ExpressionStmt(const ExpressionStmt &other) = default;
  void write_to(std::ostream &os) const {
    os << "ExpressionStmt("
       << "expr = ";
    write_maybe_null(os, this->expr);
    os << ")";
  }

  stdx::any accept(StmtVisitor &visitor) {
    return visitor.visitExpressionStmt(*this);
  }
};

struct While : Stmt {
  std::shared_ptr<Expr> cond;
  std::shared_ptr<Stmt> body;
  While(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> body)
      : cond(cond), body(body) {}
  While(const While &other) = default;
  void write_to(std::ostream &os) const {
    os << "While("
       << "cond = ";
    write_maybe_null(os, this->cond);
    os << ", "
       << "body = ";
    write_maybe_null(os, this->body);
    os << ")";
  }

  stdx::any accept(StmtVisitor &visitor) { return visitor.visitWhile(*this); }
};

struct If : Stmt {
  std::shared_ptr<Expr> cond;
  std::shared_ptr<Stmt> ifTrue;
  std::shared_ptr<Stmt> ifFalse;
  If(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> ifTrue,
     std::shared_ptr<Stmt> ifFalse)
      : cond(cond), ifTrue(ifTrue), ifFalse(ifFalse) {}
  If(const If &other) = default;
  void write_to(std::ostream &os) const {
    os << "If("
       << "cond = ";
    write_maybe_null(os, this->cond);
    os << ", "
       << "ifTrue = ";
    write_maybe_null(os, this->ifTrue);
    os << ", "
       << "ifFalse = ";
    write_maybe_null(os, this->ifFalse);
    os << ")";
  }

  stdx::any accept(StmtVisitor &visitor) { return visitor.visitIf(*this); }
};

struct Fun : Stmt {
  std::string name;
  std::vector<std::string> bindings;
  std::vector<std::shared_ptr<Stmt>> body;
  Fun(std::string name, std::vector<std::string> bindings,
      std::vector<std::shared_ptr<Stmt>> body)
      : name(name), bindings(bindings), body(body) {}
  Fun(const Fun &other) = default;
  void write_to(std::ostream &os) const {
    os << "Fun("
       << "name = " << this->name << ", "
       << "bindings = [";
    write_vec(os, this->bindings);
    os << "]"
       << ", "
       << "body = [";
    write_vec_ptr(os, this->body);
    os << "]"
       << ")";
  }

  stdx::any accept(StmtVisitor &visitor) { return visitor.visitFun(*this); }
};

struct Print : Stmt {
  std::shared_ptr<Expr> expr;
  Print(std::shared_ptr<Expr> expr) : expr(expr) {}
  Print(const Print &other) = default;
  void write_to(std::ostream &os) const {
    os << "Print("
       << "expr = ";
    write_maybe_null(os, this->expr);
    os << ")";
  }

  stdx::any accept(StmtVisitor &visitor) { return visitor.visitPrint(*this); }
};

struct Block : Stmt {
  std::vector<std::shared_ptr<Stmt>> stmts;
  Block(std::vector<std::shared_ptr<Stmt>> stmts) : stmts(stmts) {}
  Block(const Block &other) = default;
  void write_to(std::ostream &os) const {
    os << "Block("
       << "stmts = [";
    write_vec_ptr(os, this->stmts);
    os << "]"
       << ")";
  }

  stdx::any accept(StmtVisitor &visitor) { return visitor.visitBlock(*this); }
};

struct VarDecl : Stmt {
  std::string ident;
  std::shared_ptr<Expr> init;
  VarDecl(std::string ident, std::shared_ptr<Expr> init)
      : ident(ident), init(init) {}
  VarDecl(const VarDecl &other) = default;
  void write_to(std::ostream &os) const {
    os << "VarDecl("
       << "ident = " << this->ident << ", "
       << "init = ";
    write_maybe_null(os, this->init);
    os << ")";
  }

  stdx::any accept(StmtVisitor &visitor) { return visitor.visitVarDecl(*this); }
};
