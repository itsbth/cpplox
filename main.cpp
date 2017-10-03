#include <experimental/any>
#include <iostream>

#include "src/ast.hpp"
#include "src/parser.hpp"
#include "src/scanner.hpp"

namespace stdx = std::experimental;

typedef double lvalue;

class EvalVisitor : public StmtVisitor, public ExprVisitor {
  std::map<std::string, lvalue> vars;
  stdx::any visitExpressionStmt(ExpressionStmt &stmt) {
    return stmt.expr->accept(*this);
  }
  stdx::any visitBlock(Block &) { return nullptr; }
  stdx::any visitFun(Fun &) { return nullptr; }
  stdx::any visitIf(If &) { return nullptr; }
  stdx::any visitPrint(Print &) { return nullptr; }
  stdx::any visitWhile(While &) { return nullptr; }
  stdx::any visitBinop(Binop &op) {
    if (op.op == BinopType::ASSIGN) {
      auto target = std::dynamic_pointer_cast<Variable>(op.lhs);
      if (!target)
        throw "Can't assign to that, stupid";
      double rhs = stdx::any_cast<double>(op.rhs->accept(*this));
      this->vars[target->ident] = rhs;
      return rhs;
    }
    double lhs = stdx::any_cast<double>(op.lhs->accept(*this));
    double rhs = stdx::any_cast<double>(op.rhs->accept(*this));
    switch (op.op) {
    case BinopType::ADD:
      return lhs + rhs;
    case BinopType::SUB:
      return lhs - rhs;
    case BinopType::MUL:
      return lhs * rhs;
    case BinopType::DIV:
      return lhs / rhs;
    default:
      return 0;
    }
  }
  stdx::any visitUnop(Unop &) { return nullptr; }
  stdx::any visitLiteral(Literal &op) { return op.value; }
  stdx::any visitVariable(Variable &v) { return this->vars[v.ident]; }
  stdx::any visitCall(Call &) { return nullptr; }
};

void runPrompt() {
  std::string line;
  EvalVisitor visit;
  for (;;) {
    std::cout << "> ";
    if (!std::getline(std::cin, line))
      break;
    auto scanner = Scanner(line);
    try {
      auto tokens = scanner.scanTokens();
      for (auto &token : tokens) {
        std::cout << token_name(token.type) << " " << token.lexeme << " @ "
                  << token.pos << std::endl;
      }
      auto parser = Parser(tokens);
      auto stmt = parser.parseProgram();
      std::cout << *stmt << std::endl;
      double ans = stdx::any_cast<double>(stmt->accept(visit));
      std::cout << "< " << ans << std::endl;
    } catch (const char *e) {
      std::cerr << e << std::endl;
    } catch (stdx::bad_any_cast &e) {
      // no return value
    }
  }
}

void runFile(std::string fname) {
  std::cout << "file " << fname << " goes here" << std::endl;
}

int main(int argc, char **argv) {
  std::cout << "Hello, World!" << std::endl;
  if (argc < 2) {
    runPrompt();
  } else {
    runFile(argv[1]);
  }
  return 0;
}
